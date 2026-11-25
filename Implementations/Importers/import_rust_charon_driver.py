# SPDX-License-Identifier: Apache-2.0
"""Extract structured LLBC from the same Cargo target and source snapshot reviewed by Studio."""

import hashlib
import json
import os
from pathlib import Path
import re
import shutil


VERSION = "0.1.323"
REVISION = "ab5b6d60f53691bc4edee3b11cc52656c090071d"
TOOLCHAIN = "nightly-2026-09-17"


def charon_path(explicit):
    if explicit is not None:
        path = Path(explicit).absolute()
        return path if path.is_file() else None
    exe = "charon.exe" if os.name == "nt" else "charon"
    path = Path.home() / ".cargo" / "bin" / exe
    if path.is_file():
        return path
    found = shutil.which(exe)
    return Path(found).absolute() if found else None


def extract(args, scratch, crate, flags, inputs, report, invoke, dependency_paths, read_bounded, refusal):
    tool = charon_path(args.charon)
    if tool is None:
        raise refusal("charon_missing", f"Charon {VERSION} was not found. Install the pinned charon and charon-driver executables.")
    driver = tool.with_name("charon-driver" + tool.suffix)
    if not driver.is_file():
        raise refusal("charon_missing", f"The companion driver is missing: {driver}")
    version = invoke([tool, "version"], scratch, phase="charon_version")
    if version != f"{VERSION} ({REVISION})" or invoke([tool, "toolchain-version"], scratch, phase="charon_version") != TOOLCHAIN:
        raise refusal("charon_version", f"Expected Charon {VERSION} at {REVISION} with {TOOLCHAIN}. Found {version}.")
    # rustup is invoked outside the Cargo project. Scope the driver to its already
    # installed toolchain and compare its compiler input list with the stable one.
    rustup = shutil.which("rustup") or Path.home() / ".cargo" / "bin" / ("rustup.exe" if os.name == "nt" else "rustup")
    nightly = Path(invoke([rustup, "which", "--toolchain", TOOLCHAIN, "rustc"], scratch, phase="charon_toolchain"))
    compiler = invoke([nightly, "-vV"], scratch, phase="charon_toolchain")
    if f"host: {report['host']}" not in compiler.splitlines():
        raise refusal("charon_target", "The Charon toolchain and the reviewed crate have different host targets.")
    snapshot = scratch / "source"
    relative = crate.target.relative_to(crate.root)
    depfile = scratch / "charon-inputs.d"
    invoke([nightly, *flags, f"--emit=dep-info={depfile}", relative], snapshot, phase="charon_inputs")
    if dependency_paths(depfile, snapshot) != inputs:
        raise refusal("charon_inputs", "Charon's compiler selected different source inputs from the reviewed Cargo target.")

    llbc = scratch / "source.llbc"
    command = [tool, "rustc", "--sysroot", "default", "--dest-file", llbc,
               "--no-dedup-serialized-ast", "--reconstruct-fallible-operations", "--monomorphize"]
    if args.entry:
        if not re.fullmatch(r"[a-zA-Z_][a-zA-Z_0-9]*(?:::[a-zA-Z_][a-zA-Z_0-9]*)*", args.entry):
            raise refusal("charon_entry", "Select a named Rust function, such as module::function")
        entry = args.entry.removeprefix("crate::").removeprefix(crate.name + "::")
        command.extend(["--start-from", crate.name + "::" + entry])
    invoke([*command, "--", *flags, "--remap-path-prefix", f"{snapshot}=.", relative], snapshot,
           phase="charon", tool_directory=Path(rustup).parent)
    content = read_bounded(llbc, 32 * 1024 * 1024)
    raw = json.loads(content)
    if not isinstance(raw, dict) or raw.get("has_errors") is not False or raw.get("charon_version") != VERSION:
        raise refusal("charon_llbc", "Charon emitted a partial crate or an incompatible LLBC version.")
    if not isinstance(raw.get("translated"), dict) or raw["translated"].get("crate_name") != crate.name:
        raise refusal("charon_llbc", "The LLBC crate does not identify the reviewed Cargo target.")
    report.update(backend="charon-llbc", charon_version=VERSION, charon_revision=REVISION,
                  charon_toolchain=TOOLCHAIN, review_compiler=report["compiler"],
                  compiler=compiler.splitlines()[0], llbc_sha256=hashlib.sha256(content).hexdigest())
    return raw, llbc
