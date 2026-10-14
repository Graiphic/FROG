"""Exercise the canonical ownership checker on small real source trees.

This driver invokes the checker, not a second implementation of its scan. A
real directory alias and back-edge demonstrate cycle handling. No unsupported
filesystem feature becomes a skip; junction creation failures fail the owner.
"""
import argparse
import json
import os
from pathlib import Path
import subprocess
import sys
import shutil
import tempfile


CONTRACT_TYPES = (
    "ArtifactId", "ArtifactRevision", "Diagnostic", "DocumentId", "EdgeId",
    "NodeId", "PortId", "ProgramArtifact", "ProtocolVersion", "RuntimeCapabilities",
    "RuntimeEdge", "RuntimeNode", "RuntimePort", "RuntimeValue", "SessionCommand",
    "SessionEvent", "SessionId", "SourceIdentity", "UiBinding", "WidgetId",
)
EXTENSIONS = ("h", "hpp", "c", "cc", "cpp", "cxx")


def require(condition, message):
    if not condition:
        raise RuntimeError(message)


def native_io_path(path):
    native = os.path.abspath(path)
    if os.name != "nt" or native.startswith("\\\\?\\"):
        return native
    return "\\\\?\\UNC\\" + native[2:] if native.startswith("\\\\") else "\\\\?\\" + native


def actual_file_id(path):
    information = os.stat(native_io_path(path), follow_symlinks=True)
    require(information.st_dev != 0 and information.st_ino != 0,
            "The actual filesystem supplied no physical volume/file identifier")
    return str(information.st_dev) + ":" + str(information.st_ino)


def put(tree, relative, text):
    path = tree / relative
    os.makedirs(native_io_path(path.parent), exist_ok=True)
    with open(native_io_path(path), "w", encoding="utf-8") as stream:
        stream.write(text)
    return path


def alias_directory(sandbox, target, link):
    require(target.resolve().is_relative_to(sandbox) and link.parent.resolve().is_relative_to(sandbox) and
            link.absolute().is_relative_to(sandbox), "A fixture alias target or link escapes the verified temporary root")
    require(target.is_dir() and not link.exists(), "The bounded directory alias fixture is malformed")
    if os.name == "nt":
        # Windows junctions need no symlink privilege and preserve the same
        # directory alias semantics; paths belong to the one TemporaryDirectory.
        result = subprocess.run(["cmd.exe", "/d", "/c", "mklink", "/J", str(link), str(target)],
                                capture_output=True, timeout=10)
        require(result.returncode == 0, "A real fixture junction could not be created: " +
                result.stderr.decode(errors="replace"))
    else:
        link.symlink_to(target, target_is_directory=True)
    require(link.is_dir() and link.resolve() == target.resolve(), "The fixture alias is not a real resolved directory")


def excluded(path):
    # Test only the historical lexical path boundaries, not an alternative
    # scanner. The checker itself decides discovery/read/ownership.
    components = path.as_posix().split("/")
    return any(component.startswith("build") or component in ("third_party", ".git")
               for component in components[:-1])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cmake", type=Path, required=True)
    parser.add_argument("--checker", type=Path, required=True)
    parser.add_argument("--canonical-header", type=Path, required=True)
    parser.add_argument("--output-directory", type=Path, required=True)
    args = parser.parse_args()
    for path in (args.cmake, args.checker, args.canonical_header):
        require(path.is_file(), "A required canonical checker input is missing: " + str(path))
    with args.canonical_header.open("rb") as stream:
        canonical_bytes = stream.read(1024 * 1024 + 1)
    require(0 < len(canonical_bytes) <= 1024 * 1024, "Actual canonical contract exceeds the fixture copy budget")
    args.output_directory.mkdir(parents=True, exist_ok=True)
    records = []

    with tempfile.TemporaryDirectory(prefix="frog-runtime-ownership-") as temporary:
        sandbox = Path(temporary).resolve()
        require(sandbox.parent == Path(tempfile.gettempdir()).resolve() and
                sandbox.name.startswith("frog-runtime-ownership-"),
                "The temporary directory does not resolve to its explicitly named parent and prefix")
        require(not excluded(sandbox / "sentinel.hpp"), "Fixture temp parent unexpectedly belongs to a historically excluded source path")

        def run(name, configure, errors=(), check_sources=(), has_cycle=False, ordered_errors=False):
            tree = sandbox / name
            canonical = tree / "canonical/runtime_contract.hpp"
            canonical.parent.mkdir(parents=True, exist_ok=True)
            canonical.write_bytes(canonical_bytes)
            configure(tree, canonical)
            trace = sandbox / (name + ".trace.jsonl")
            command = [str(args.cmake), "--trace-expand", "--trace-format=json-v1", "--trace-redirect=" + str(trace),
                       "-DSCAN_ROOT=" + str(tree), "-DCANONICAL_CONTRACT_HEADER=" + str(canonical),
                       "-DFROG_OWNERSHIP_PYTHON_EXECUTABLE=" + sys.executable, "-P", str(args.checker)]
            result = subprocess.run(command, capture_output=True, timeout=15)
            output = (result.stdout + result.stderr).decode("utf-8", errors="replace")
            require((result.returncode != 0) == bool(errors), name + ": ownership accept/refuse result differs\n" + output)
            normalized_output = " ".join(output.split())
            for error in errors:
                require(error in normalized_output, name + ": missing exact ownership assertion " + error + "\n" + output)
            if ordered_errors:
                positions = [normalized_output.index(error) for error in errors]
                require(positions == sorted(positions) and len(set(positions)) == len(positions),
                        name + ": the unchanged contract-type assertion order differs")
            if not errors:
                require("Runtime contract ownership verified for " in output, name + ": checker did not report successful ownership")
            # Observe the genuine CMake file commands: excluded folders are
            # pruned before any listing, source content still read everywhere
            # else. This detects a broadened exclusion or recursive regression.
            calls = []
            with trace.open("rb") as stream:
                raw_trace = stream.read(4 * 1024 * 1024 + 1)
            require(len(raw_trace) <= 4 * 1024 * 1024, name + ": trace exceeds the bounded fixture budget")
            for line in raw_trace.decode("utf-8").splitlines():
                event = json.loads(line)
                if event.get("cmd") == "file":
                    calls.append(event["args"])
            read_paths = [Path(call[1]) for call in calls if call[0] == "READ"]
            require(not any(excluded(path) for path in read_paths), name + ": checker read an excluded source")
            for relative in check_sources:
                target = actual_file_id(tree / relative)
                require(any(actual_file_id(path) == target for path in read_paths), name + ": checker failed to inspect actual source " + relative)
            require(not any(call[0] == "GLOB_RECURSE" for call in calls), name + ": source discovery recursed before exclusions")
            discovery = args.checker.parent / "runtime_contract_source_inventory.py"
            require(discovery.is_file(), "The private physical discovery helper is missing")
            inventory_command = [sys.executable, "-B", "-X", "utf8", str(discovery),
                                 "--scan-root", str(tree), "--canonical-header", str(canonical)]
            inventory_result = subprocess.run(inventory_command, capture_output=True, timeout=15)
            require(inventory_result.returncode == 0, name + ": real physical discovery failed: " +
                    inventory_result.stderr.decode("utf-8", errors="replace"))
            require(0 < len(inventory_result.stdout) <= 4 * 1024 * 1024, name + ": real inventory is empty or over fixture budget")
            inventory = [json.loads(line) for line in inventory_result.stdout.decode("utf-8").splitlines()]
            require(inventory[0]["kind"] == "canonical" and
                    inventory[0]["physical"] == actual_file_id(canonical),
                    name + ": actual canonical physical identity differs")
            directory_records = [record for record in inventory if record["kind"] == "directory"]
            require(directory_records, name + ": physical discovery did not visit any actual directory")
            directories = []
            for record in directory_records:
                directory = Path(record["lexical"])
                require(not excluded(directory / "sentinel.hpp"), name + ": checker descended into an excluded directory")
                require(record["physical"] == actual_file_id(directory),
                        name + ": discovery returned an incomplete physical directory identity")
                directories.append(actual_file_id(directory))
            require(len(directories) == len(set(directories)), name + ": the same physical directory was traversed more than once")
            sources = [record for record in inventory if record["kind"] == "source"]
            require(sources and all(not excluded(Path(record["lexical"])) for record in sources),
                    name + ": source inventory omitted all sources or admitted an excluded path")
            require(all(record["physical"] == actual_file_id(record["lexical"]) == actual_file_id(record["read_path"]) for record in sources),
                    name + ": discovered lexical/IO source paths do not identify their actual physical file")
            required_reads = [record for record in sources if
                              record["physical"] != inventory[0]["physical"] and Path(record["lexical"]).name != "runtime_contract.hpp"]
            require(all(any(actual_file_id(read_path) == actual_file_id(record["lexical"]) for read_path in read_paths) for record in required_reads),
                    name + ": CMake did not READ every genuine noncanonical/noncopied discovered source")
            require(all(any(actual_file_id(record["lexical"]) == actual_file_id(path) and
                            os.path.normcase(str(path)) == os.path.normcase(str(Path(record["read_path"])))
                            for record in sources) for path in read_paths),
                    name + ": CMake source content inspection used a different or fabricated inventory")
            if has_cycle:
                require(any(directory == actual_file_id(tree) for directory in directories), name + ": cycle fixture did not inspect its true root")
            records.append({"case": name, "accepted": result.returncode == 0, "expected_assertions": list(errors),
                            "actual_directories": len(directories), "actual_source_reads": len(read_paths), "real_cycle": has_cycle})

        positive_sources = ["src/a.h", "include/a.hpp", "tests/a.c", "arbitrary/nested/a.cc", ".validation-logs/copy/a.cpp", "outputs/generated/a.cxx"]

        def nonduplicated(tree, canonical):
            for relative in positive_sources:
                put(tree, relative, "#include <frog/sdk/runtime_contract.hpp>\n// struct NodeId is only a mention.\nint source_value;\n")
            for directory in ("build", "build-nmake-verify", "builder", "third_party", ".git"):
                put(tree, directory + "/nested/copied/runtime_contract.hpp", "struct ProgramArtifact { int forbidden; };\n")
            alias_directory(sandbox, canonical.parent, tree / "canonical_alias")
            alias_directory(sandbox, tree, tree / "src/back_to_root")

        # Comments deliberately do not resemble a definition regex; a matching
        # comment would remain rejected by the original unchanged assertion.
        run("nonduplicated-all-directories", nonduplicated, check_sources=positive_sources, has_cycle=True)

        for index, extension in enumerate(EXTENSIONS):
            type_name = CONTRACT_TYPES[index]
            relative = "outside_src/arbitrary/deeper/duplicate." + extension
            def duplicate_extension(tree, canonical, relative=relative, type_name=type_name):
                put(tree, relative, "struct\n" + type_name + " \t { int copied; };\n")
            run("duplicate-extension-" + extension, duplicate_extension,
                errors=("private definition of " + type_name,), check_sources=(relative,))

        def every_type(tree, canonical):
            for index, type_name in enumerate(CONTRACT_TYPES):
                put(tree, ".validation-logs/types/" + type_name + "." + EXTENSIONS[index % len(EXTENSIONS)],
                    ("class " if index % 2 else "struct ") + type_name + "\n{ int copied; };\n")
        run("all-contract-type-assertions", every_type,
            errors=tuple("private definition of " + name for name in CONTRACT_TYPES))

        # One union match must still execute all twenty original assertions in
        # their table order, independently of definition order in the real file.
        def all_types_one_source(tree, canonical):
            definitions = ["class\n" + name + " \t { int copied; };\n" for name in reversed(CONTRACT_TYPES)]
            put(tree, "arbitrary/all-types.cpp", "".join(definitions))
        run("all-contract-types-one-source", all_types_one_source,
            errors=tuple("private definition of " + name for name in CONTRACT_TYPES),
            check_sources=("arbitrary/all-types.cpp",), ordered_errors=True)

        # The prefilter has exactly the same trailing whitespace/brace rule;
        # mere mentions and longer identifiers are actual accepted source.
        def type_mentions_and_suffixes(tree, canonical):
            text = "".join("// struct " + name + " is a mention.\nstruct " + name + "_Shadow { int value; };\n"
                           for name in CONTRACT_TYPES)
            put(tree, "arbitrary/mentions-and-suffixes.hpp", text)
        run("type-mentions-and-suffixes", type_mentions_and_suffixes,
            check_sources=("arbitrary/mentions-and-suffixes.hpp",))

        run("copied-header", lambda tree, canonical: put(tree, "include/nested/runtime_contract.hpp", canonical_bytes.decode("utf-8")),
            errors=("copied public contract header is forbidden",))

        # An admitted alias into a lexical build directory must still reveal
        # its real copied header; excluded lexical traversal may not premark it.
        def alias_into_excluded(tree, canonical):
            put(tree, "build-private/copied/runtime_contract.hpp", "struct NodeId { int copied; };\n")
            alias_directory(sandbox, tree / "build-private", tree / "real_source_alias")
            alias_directory(sandbox, tree, tree / "real_source_alias/copied/cycle")
        run("alias-into-excluded-with-cycle", alias_into_excluded,
            errors=("copied public contract header is forbidden",), has_cycle=True)

        # Prefixes/suffixes that are not the existing directory exclusions must
        # keep their original exact ownership assertions.
        run("unexcluded-directory-boundaries", lambda tree, canonical: (
                put(tree, "nested_build/x.hpp", "struct NodeId { int copied; };\n"),
                put(tree, "third_party_extra/buildfile.cpp", "class WidgetId { int copied; };\n")),
            errors=("private definition of NodeId", "private definition of WidgetId"),
            check_sources=("nested_build/x.hpp", "third_party_extra/buildfile.cpp"))

        if os.name == "nt" or sys.platform == "darwin":
            run("host-uppercase-extension", lambda tree, canonical: put(tree, "include/uppercase.CPP", "struct PortId { int copied; };\n"),
                errors=("private definition of PortId",), check_sources=("include/uppercase.CPP",))
        else:
            run("host-uppercase-extension", lambda tree, canonical: put(tree, "include/uppercase.CPP", "struct PortId { int copied; };\n"))

        long_relative = "/".join("depth-" + str(index) + "-" + "x" * 32 for index in range(6)) + "/long-source.cpp"
        def actual_long_path(tree, canonical):
            actual = put(tree, long_relative, "struct NodeId { int copied; };\n")
            require(260 < len(str(actual)) < 768, "The real long-path fixture does not exercise the bounded Windows path limit")
            require(actual_file_id(actual) != actual_file_id(canonical), "The long source did not create its own real file")
        try:
            run("actual-bounded-long-path", actual_long_path,
                errors=("private definition of NodeId",), check_sources=(long_relative,))
        finally:
            long_tree = sandbox / "actual-bounded-long-path"
            if os.path.exists(native_io_path(long_tree)):
                require(long_tree.parent == sandbox and long_tree.resolve(strict=True).parent == sandbox and
                        long_tree.name == "actual-bounded-long-path",
                        "Long-path cleanup would leave the verified temporary root")
                # Preserve the extended namespace through recursive cleanup;
                # TemporaryDirectory then has only the original short fixtures.
                shutil.rmtree(native_io_path(long_tree))

        # The validation interpreter must be explicit, never discovered from
        # PATH or replaced by an incomplete CMake physical resolver.
        missing_python = subprocess.run([str(args.cmake), "-DSCAN_ROOT=" + str(sandbox),
            "-DCANONICAL_CONTRACT_HEADER=" + str(args.canonical_header), "-P", str(args.checker)],
            capture_output=True, timeout=15)
        require(missing_python.returncode != 0 and "FROG_OWNERSHIP_PYTHON_EXECUTABLE" in
                (missing_python.stdout + missing_python.stderr).decode("utf-8", errors="replace"),
                "The checker silently selected an interpreter or incomplete physical resolver")

    proof = {"status": "PASS", "cases": records, "case_count": len(records), "no_skips": True,
             "canonical_checker": str(args.checker.resolve()), "all_type_assertions": list(CONTRACT_TYPES)}
    (args.output_directory / "ownership-fixtures-proof.json").write_text(json.dumps(proof, indent=2) + "\n", encoding="utf-8")
    print("RUNTIME_CONTRACT_OWNERSHIP_FIXTURES_PASS " + str(len(records)) + " actual CMake scans, canonical/copies/all extensions/types/aliases/cycles/exclusions")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
