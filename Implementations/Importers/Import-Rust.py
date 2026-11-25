# SPDX-License-Identifier: Apache-2.0
"""Import compiler-checked Rust functions as editable FROG documents."""

import argparse
from contextlib import contextmanager
from dataclasses import dataclass, field
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile
import tomllib
from uuid import uuid4


class Refusal(ValueError):
    def __init__(self, code, message):
        super().__init__(message)
        self.code = code


@contextmanager
def scratch_directory():
    # Python 3.13's Windows mode 0700 applies an ACL that can exclude the
    # compiler process in Studio's job. Inherit the user's normal temp ACL.
    scratch = Path(tempfile.gettempdir()).resolve() / ("frog-rust-" + uuid4().hex)
    scratch.mkdir(mode=0o777 if os.name == "nt" else 0o700)
    try:
        yield scratch
    finally:
        shutil.rmtree(scratch)


def write_json(path, data, mode="w"):
    with path.open(mode, encoding="utf-8") as stream:
        json.dump(data, stream, ensure_ascii=False, allow_nan=False, indent=2)
        stream.write("\n")


def read_bounded(path, limit):
    with path.open("rb") as stream:
        data = stream.read(limit + 1)
    if len(data) > limit:
        raise Refusal("input_limit", f"{path.name} exceeds the import limit of {limit} bytes")
    return data


@dataclass
class Crate:
    root: Path
    target: Path
    name: str
    kind: str
    edition: str
    features: list = field(default_factory=list)
    profile: dict = field(default_factory=dict)
    manifests: dict = field(default_factory=dict)


def package_config(source, args, compiler, scratch):
    """Ask Cargo for the real target, including inherited editions and nonstandard paths."""
    if source.name != "Cargo.toml":
        if source.suffix != ".rs":
            raise Refusal("source", "Select a Rust .rs file or Cargo.toml")
        crate = re.sub(r"\W", "_", source.stem) or "imported"
        return Crate(source.parent, source, crate, "lib", args.edition or "2024")
    for parent in [source.parent, *source.parent.parents]:
        if any((parent / ".cargo" / name).is_file() for name in ("config", "config.toml")):
            raise Refusal("cargo_config", f"Review the Cargo configuration in {parent / '.cargo'} before importing")
    cargo = compiler.with_name("cargo.exe" if os.name == "nt" else "cargo")
    manifest = read_bounded(source, 1024 * 1024)
    command = [cargo, "metadata", "--frozen", "--no-deps", "--format-version", "1", "--manifest-path", source]
    metadata = json.loads(invoke(command, scratch, phase="cargo"))
    root = Path(metadata["workspace_root"]).resolve(strict=True)
    selected = [p for p in metadata["packages"] if Path(p["manifest_path"]).resolve() == source]
    if not selected:
        selected = [p for p in metadata["packages"] if p["id"] in metadata["workspace_default_members"]]
    if len(selected) != 1:
        raise Refusal("cargo_package", "Choose the Cargo.toml of one workspace member")
    package = selected[0]
    package_file = Path(package["manifest_path"]).resolve(strict=True)
    root_manifest = root / "Cargo.toml"
    manifests = {p.relative_to(root): read_bounded(p, 1024 * 1024)
                 for p in {source, package_file, root_manifest}}
    if manifests[source.relative_to(root)] != manifest:
        raise Refusal("source_changed", "The selected manifest changed during analysis")
    if package.get("links") or any("custom-build" in t["kind"] for t in package["targets"]):
        raise Refusal("cargo_build", "This package requires a build script or native library")
    if any(d.get("kind") != "dev" and not d.get("optional") for d in package["dependencies"]):
        raise Refusal("cargo_context", "This package needs build or runtime dependencies outside the scalar import profile")
    # Cargo's metadata command is read-only here. Recheck its target decision after
    # taking the manifest snapshots, including other workspace members' manifests.
    if json.loads(invoke(command, scratch, phase="cargo")) != metadata:
        raise Refusal("source_changed", "Cargo metadata changed while capturing the selected target")
    libraries = [t for t in package["targets"] if any(k in t["kind"] for k in
                 ("lib", "rlib", "cdylib", "dylib", "staticlib", "proc-macro"))]
    targets = libraries or [t for t in package["targets"] if t["kind"] == ["bin"]]
    if len(targets) != 1:
        raise Refusal("cargo_target", "Select a package with one binary target or a library")
    target = targets[0]
    if target["crate_types"] not in (["lib"], ["rlib"], ["bin"]):
        raise Refusal("cargo_target", "Unsupported crate type: " + ", ".join(target["crate_types"]))
    edition = target["edition"]
    if edition not in ("2015", "2018", "2021", "2024") or (args.edition and args.edition != edition):
        raise Refusal("cargo_edition", "Use the edition declared by the Cargo target")
    target_path = Path(target["src_path"]).resolve(strict=True)
    if not target_path.is_relative_to(root):
        raise Refusal("cargo_target", "The target is outside the selected workspace")
    config = tomllib.loads(manifests[root_manifest.relative_to(root)].decode("utf-8"))
    features = package["features"]
    enabled = set()
    pending = ["default"] if "default" in features else []
    while pending:
        feature = pending.pop()
        if feature in enabled:
            continue
        if feature not in features or not re.fullmatch(r"[\w.+-]+", feature):
            raise Refusal("cargo_feature", f"Unsupported Cargo feature dependency {feature!r}")
        enabled.add(feature)
        pending.extend(features[feature])
    profile = config.get("profile", {}).get(args.profile, {})
    if not set(target.get("required-features", ())).issubset(enabled):
        raise Refusal("cargo_feature", "The selected target needs features outside the default set")
    unknown = profile.keys() - {"debug-assertions", "overflow-checks", "opt-level", "debug", "incremental",
                                "strip", "codegen-units", "lto"}
    if unknown:
        raise Refusal("cargo_profile", "Unsupported profile configuration: " + ", ".join(sorted(unknown)))
    return Crate(root, target_path, target["name"].replace("-", "_"), target["crate_types"][0], edition,
                 sorted(enabled), profile, manifests)


def invoke(command, cwd, phase="rustc", tool_directory=None):
    # env! and option_env! belong to the build context. Do not expose unrelated
    # process credentials to compile-time macros. Environment inputs are refused below.
    allowed = {"systemroot", "windir", "temp", "tmp", "tmpdir", "path", "pathext", "userprofile",
               "home", "rustup_home", "cargo_home", "rustup_toolchain", "ld_library_path"}
    env = {k: v for k, v in os.environ.items() if k.lower() in allowed}
    if tool_directory is not None:
        path_key = next((key for key in env if key.lower() == "path"), "PATH")
        env[path_key] = os.pathsep.join((str(tool_directory), env.get(path_key, "")))
    run = subprocess.run(command, cwd=cwd, env=env, stdin=subprocess.DEVNULL, capture_output=True,
                         timeout=45, encoding="utf-8", errors="replace")
    if run.returncode:
        diagnostics = []
        for line in run.stderr.splitlines():
            try:
                diagnostic = json.loads(line)
            except ValueError:
                diagnostics.append(line)
            else:
                diagnostics.append(diagnostic.get("rendered") or diagnostic.get("message", line))
        raise Refusal(phase, ("\n".join(diagnostics) or run.stdout or f"{phase} exited {run.returncode}")[:12000])
    return run.stdout.strip()


def compiler_path(args, scratch):
    path = args.rustc or shutil.which("rustc") or Path.home() / ".cargo/bin" / ("rustc.exe" if os.name == "nt" else "rustc")
    launcher = Path(path).absolute()  # rustup proxies rely on the invoked basename
    if not launcher.is_file():
        raise Refusal("rustc_missing", "Install the rustc 1.96.0 toolchain or supply --rustc PATH")
    version = invoke([launcher, "-vV"], scratch)
    if not re.search(r"(?m)^release: 1\.96\.0$", version):
        raise Refusal("rustc_version", f"This MIR reader supports rustc 1.96.0. Found {version.splitlines()[0]}")
    # Resolve rustup outside the project to prevent a local override from changing
    # the compiler between inspection of the version and emission of MIR.
    sysroot = Path(invoke([launcher, "--print", "sysroot"], scratch))
    compiler = (sysroot / "bin" / ("rustc.exe" if os.name == "nt" else "rustc")).resolve(strict=True)
    if invoke([compiler, "-vV"], scratch) != version:
        raise Refusal("rustc_version", "The Rust launcher and sysroot identify different compilers")
    return compiler, version


def dependency_paths(depfile, root):
    # rustc also emits a separate empty Make rule for each source, one per line.
    paths = set()
    for line in read_bounded(depfile, 2 * 1024 * 1024).decode("utf-8").splitlines():
        if line.startswith("# env-dep:"):
            raise Refusal("rust_environment", "An environment-dependent macro requires a declared build context")
        if line.endswith(":") and not line.startswith("#"):
            relative = Path(line[:-1].replace("\\ ", " "))
            if relative.is_absolute() or not (root / relative).resolve(strict=True).is_relative_to(root):
                raise Refusal("source_scope", f"Compiler input outside the selected root: {relative}")
            if ".." in relative.parts:
                relative = (root / relative).resolve().relative_to(root)
            paths.add(relative)
    if not paths or len(paths) > 512:
        raise Refusal("inputs", "Expected between 1 and 512 compiler input files")
    return paths


def prepare(args, scratch, report):
    source = args.source.resolve(strict=True)
    compiler, version = compiler_path(args, scratch)
    crate = package_config(source, args, compiler, scratch)
    root = crate.root
    relative = crate.target.relative_to(root)
    debug = crate.profile.get("debug-assertions", args.profile == "dev")
    overflow = crate.profile.get("overflow-checks", args.profile == "dev")
    if not isinstance(debug, bool) or not isinstance(overflow, bool):
        raise Refusal("cargo_profile", "Expected Boolean values for debug-assertions and overflow-checks")
    flags = [compiler, "--crate-type", crate.kind, "--edition", crate.edition, "--crate-name", crate.name,
             "-C", "opt-level=0", "-C", f"debug-assertions={'yes' if debug else 'no'}",
             "-C", f"overflow-checks={'yes' if overflow else 'no'}", "--error-format=json"]
    for feature in crate.features:
        flags += ["--cfg", f'feature="{feature}"']
    report.update(compiler=version.splitlines()[0], host=version.split("host: ")[1].splitlines()[0],
                  crate=crate.name, target_path=relative.as_posix(), edition=crate.edition,
                  cargo_profile=args.profile, overflow_checks=overflow, debug_assertions=debug, features=crate.features)
    depfile = scratch / "original.d"
    invoke([*flags, f"--emit=dep-info={depfile}", relative], root)
    paths = dependency_paths(depfile, root)
    if relative not in paths:
        raise Refusal("inputs", "The Rust compiler did not report the crate root")
    paths.update(crate.manifests)
    snapshot = scratch / "source"
    snapshot.mkdir()
    hashes = {}
    size = 0
    for path in sorted(paths):
        data = read_bounded(root / path, 8 * 1024 * 1024)
        size += len(data)
        if size > 32 * 1024 * 1024:
            raise Refusal("input_limit", "The compilation inputs exceed 32 MiB")
        if path in crate.manifests and data != crate.manifests[path]:
            raise Refusal("source_changed", f"{path} changed during analysis")
        dest = snapshot / path
        dest.parent.mkdir(parents=True, exist_ok=True)
        dest.write_bytes(data)
        hashes[path.as_posix()] = hashlib.sha256(data).hexdigest()
    digest = next(iter(hashes.values())) if len(hashes) == 1 else hashlib.sha256(
        json.dumps(hashes, ensure_ascii=False, sort_keys=True, separators=(",", ":")).encode("utf-8")).hexdigest()
    report.update(inputs=hashes, source_sha256=digest)
    if args.expected_sha256 and digest != args.expected_sha256.lower():
        raise Refusal("source_changed", "The compilation inputs changed since analysis")
    mir = scratch / "crate.mir"
    captured = scratch / "snapshot.d"
    invoke([*flags, f"--emit=mir={mir},dep-info={captured}", relative], snapshot)
    inputs = dependency_paths(captured, snapshot)
    if paths != inputs | crate.manifests.keys():
        raise Refusal("source_changed", "The compiler used a different set of inputs in the snapshot")
    return read_bounded(mir, 32 * 1024 * 1024).decode("utf-8"), crate, flags[1:], inputs


def derive(frontend, stage, main, graphs, source):
    members = []
    dependencies = []
    for graph in graphs.values():
        name = "main.frog" if graph is main else graph.filename
        document = graph.draft(graph is main)
        document["metadata"]["description"] = f"Source (Rust): {source}\n{graph.origin}"
        write_json(stage / name, document)
        revision = str(document["draft_revision"])
        members.append({"function": graph.key, "document": name, "origin": graph.origin})
        if graph is main:
            main_revision = revision
        else:
            dependencies.append({"kind": 1, "reference": name, "source_uri": "import-rust:/" + name,
                                 "identity": graph.key, "version": revision, "path": name})
    write_json(stage / "frontend-dependencies.json",
               {"format": "frog.frontend.dependencies", "version": "1", "dependencies": dependencies})
    if frontend:
        result = subprocess.run([frontend.resolve(strict=True), "derive", "--source", stage / "main.frog",
            "--source-uri", "import-rust:/main.frog", "--document-id", main.key, "--fir-document-id",
            main.key + ":fir", "--source-format", "frog.document.draft", "--source-version", main_revision,
            "--fir-version", "0.1", "--dependencies", stage / "frontend-dependencies.json", "--output",
            stage / "main.fir.json", "--source-maps-output", stage / "source-maps.json"],
            capture_output=True, encoding="utf-8", errors="replace", timeout=45)
        if result.returncode:
            raise Refusal("frontend", (result.stderr or result.stdout or "Frontend refused the document")[:6000])
    return members


def convert(args, report):
    from import_rust_mir import compile_graphs

    destination = args.output.absolute() if args.output else None
    if destination and (destination.exists() or destination.is_symlink()):
        raise Refusal("destination_exists", "Choose a new import destination")
    with scratch_directory() as scratch:
        try:
            mir, crate, flags, inputs = prepare(args, scratch, report)
        except (AttributeError, TypeError, IndexError) as error:
            raise Refusal("configuration", f"Invalid manifest or compiler metadata: {error}") from error
        llbc_path = None
        use_charon = args.backend == "charon" or args.charon is not None
        if not use_charon:
            try:
                main, graphs = compile_graphs(mir, args.entry, report["target_path"])
                report["backend"] = "rustc-mir"
            except ValueError as error:
                if args.backend == "mir" or args.source.name != "Cargo.toml":
                    raise
                from import_rust_charon_driver import charon_path
                if charon_path(None) is None:
                    raise Refusal("rust_profile", f"{error}\nFor extended Cargo imports, install Charon 0.1.323.") from error
                use_charon = True
        if use_charon:
            from import_rust_charon_driver import extract
            raw, llbc_path = extract(args, scratch, crate, flags, inputs, report,
                                     invoke, dependency_paths, read_bounded, Refusal)
            from import_rust_charon import compile_llbc
            main, graphs = compile_llbc(raw, args.entry, report["target_path"])
        stage_parent = destination.parent if destination else scratch
        stage = stage_parent / (".import-rust-" + uuid4().hex)
        stage.mkdir()
        try:
            members = derive(args.frontend, stage, main, graphs, args.source.resolve())
            for name, expected in report["inputs"].items():
                if hashlib.sha256(read_bounded(crate.root / name, 8 * 1024 * 1024)).hexdigest() != expected:
                    raise Refusal("source_changed", f"{name} changed during import")
            report.update(compatible=True, entry=main.key, functions=members, frontend_validated=bool(args.frontend))
            report["report"] = (f"{report['backend']} | {report['compiler']} | {report['host']} | {report['cargo_profile']}\n"
                                f"Entry: {main.key}\nEditable functions: " + ", ".join(graphs))
            if destination:
                if llbc_path:
                    shutil.copyfile(llbc_path, stage / "source.llbc")
                    write_json(stage / "rust-source-map.json", {
                        "llbc_sha256": report["llbc_sha256"],
                        "documents": {member["document"]: graphs[member["function"]].source_nodes
                                      for member in members},
                    })
                write_json(stage / "import-analysis.json", report)
                if destination.exists() or destination.is_symlink():
                    raise Refusal("destination_exists", "The destination appeared during import")
                stage.rename(destination)
        finally:
            if stage.exists():
                shutil.rmtree(stage)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", required=True, type=Path)
    parser.add_argument("--entry", help="Rust function path; inferred for a single root")
    parser.add_argument("--edition", choices=["2015", "2018", "2021", "2024"])
    parser.add_argument("--profile", choices=["dev", "release"], default="release")
    parser.add_argument("--backend", choices=["auto", "mir", "charon"], default="auto")
    parser.add_argument("--charon", type=Path, help="Explicit path to the pinned Charon executable")
    parser.add_argument("--analyze", action="store_true")
    for option in ("frontend", "output", "report", "rustc"):
        parser.add_argument("--" + option, type=Path)
    parser.add_argument("--expected-sha256")
    args = parser.parse_args()
    if args.charon is not None and args.backend == "mir":
        parser.error("--charon cannot be used with --backend mir")
    if (args.analyze and args.output) or (not args.analyze and (not args.output or not args.frontend or args.report)):
        parser.error("Use --analyze [--report FILE] or --output NEW_DIRECTORY --frontend EXE")
    sys.path.insert(0, str(Path(__file__).resolve().parent))
    report = {"format": "frog.rust-import.analysis.v1", "source": str(args.source.absolute()),
              "compatible": False, "diagnostics": [], "source_sha256": ""}
    try:
        convert(args, report)
    except (OSError, ValueError, subprocess.TimeoutExpired, ImportError, RecursionError) as exc:
        report.update(compatible=False, report=str(exc))
        report["diagnostics"].append({"code": getattr(exc, "code", "rust_import"), "message": str(exc)})
    if args.report:
        try:
            write_json(args.report, report, "x")
        except OSError as exc:
            print(str(exc), file=sys.stderr)
            return 1
    print(json.dumps(report, ensure_ascii=True))
    return 0 if args.analyze or report["compatible"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
