# SPDX-License-Identifier: Apache-2.0
"""Check an explicitly prepared C Project fixture against the common Frontend."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile


def require(condition, message):
    if not condition:
        raise AssertionError(message)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--database", type=Path, default=Path(os.getenv("FROG_C_PROJECT_TEST_DATABASE", "")))
    for arg in ("clang", "frontend", "scratch"):
        parser.add_argument("--" + arg, type=Path, required=True)
    args = parser.parse_args()
    if not args.database.is_file():
        print("Prepare the CMake fixture and set FROG_C_PROJECT_TEST_DATABASE.")
        return 77
    importer = Path(__file__).resolve().parents[2] / "Import-CProject.py"
    before = args.database.read_bytes()
    args.scratch.mkdir(parents=True, exist_ok=True)
    scratch_root = Path(os.environ.get("FROG_C_PROJECT_TEST_SCRATCH", args.scratch))
    scratch_root.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="cp-", dir=scratch_root) as scratch:
        scratch = Path(scratch)

        def run(target, config, entry, label, diagnostic=None, database=args.database):
            output = scratch / label
            result = subprocess.run([sys.executable, "-B", "-X", "utf8", str(importer),
                                     "--source", str(database), "--clang", str(args.clang),
                                     "--frontend", str(args.frontend), "--output", str(output),
                                     "--target=" + target, "--configuration=" + config, "--entry=" + entry],
                                    capture_output=True, text=True, encoding="utf-8", timeout=45)
            if diagnostic:
                require(result.returncode == 2 and not output.exists() and
                        all(term in result.stderr for term in diagnostic), result.stderr or "Missing refusal")
                print(f"{label}: {result.stderr.strip()}")
            else:
                require(result.returncode == 0 and (output / "main.fir.json").is_file(), result.stderr)
            return output

        catalogues = []
        for config, mode in (("Debug", 7), ("Release", 19)):
            output = run("calculation", config, "compute", config)
            index = json.loads((output / "import-map.json").read_text(encoding="utf-8"))
            require(index["configuration"] == config, "Mixed configurations")
            require(index["compile_commands_sha256"] == hashlib.sha256(before).hexdigest(),
                    "The result did not identify the compilation database it consumed")
            symbols = {fn["function"]: fn for fn in index["functions"]}
            require(set(symbols) == {"compute", "middle", "leaf", "src/entry.c::adjust", "src/middle.c::adjust"},
                    "Expected five distinct C functions")
            catalogues.append(symbols)
            for symbol, record in symbols.items():
                require(record["origin"].startswith("src/") and record["origin"].split(":")[-1].isdigit(),
                        "Missing C source location")
                graph = json.loads((output / record["document"]).read_text(encoding="utf-8"))
                nodes = {node["id"]: node for node in graph["diagram"]["nodes"]}
                edges = graph["diagram"]["edges"]
                require(edges and all(e["from"]["node"] in nodes and e["to"]["node"] in nodes for e in edges),
                        f"Dangling wire in {symbol}")
                if symbol == "leaf":
                    require(any(n.get("text") == str(mode) for n in nodes.values()), "Wrong C macro")
                if symbol in {"compute", "middle"}:
                    expected = (("src/entry.c::adjust", "middle") if symbol == "compute" else
                                ("src/middle.c::adjust", "leaf"))
                    calls = [n["call"] for n in nodes.values() if n["kind"] == "frog_call"]
                    require({call["document_path"] for call in calls} == {symbols[k]["document"] for k in expected},
                            "Cross-file or static call resolved to the wrong dependency")
                    require(all(len(call["ports"]) == 2 and all(p["value_type"] == "u32" for p in call["ports"])
                                for call in calls), "Call lost its typed ports")
            print(f"{config}: 5 editable documents; MODE_VALUE={mode}; common FIR derived")
        require(catalogues[0] == catalogues[1], "Stable identities changed between configurations")
        linked = run("reference", "Debug", "compute", "target-closure")
        require(json.loads((linked / "import-map.json").read_text(encoding="utf-8"))["functions"] ==
                list(catalogues[0].values()), "CMake's true link dependency did not contribute its definitions")
        run("calculation", "Bogus", "compute", "bad-config", ("c_project.configuration", "Debug"))
        run("not-a-target", "Debug", "compute", "bad-target", ("c_project.target", "calculation"))
        run("import_refusals", "Debug", "missing_entry", "missing-definition",
            ("c_project.missing_definition", "src/refusals.c:3", "missing_helper"))
        if os.name == "nt":
            run("import_refusals", "Debug", "external_entry", "external-dependency",
                ("c_project.external_call", "vendor_helper", "NativeCall"))
        # Hide only the generated include directory in copied metadata.
        # The real configured header and build directory remain unmodified.
        rows = json.loads(before)
        empty_include = scratch / "without-generated-header"
        empty_include.mkdir()
        generated = args.database.resolve().parent / "generated"
        require((generated / "generated.h").is_file(), "Prepare the fixture's generated.h first")
        replacements = 0
        for row in rows:
            if not row["file"].replace("\\", "/").endswith("/src/middle.c"):
                continue
            for old, new in ((str(generated), str(empty_include)), (generated.as_posix(), empty_include.as_posix())):
                previous = row["command"]
                row["command"] = previous.replace(old, new)
                replacements += previous != row["command"]
        require(replacements >= 2, "Cannot identify the Debug and Release generated include")
        with tempfile.NamedTemporaryFile(prefix="negative-c-project-", suffix=".json",
                                         dir=args.database.parent, delete=False) as handle:
            alternate = Path(handle.name)
            handle.write(json.dumps(rows).encode())
        try:
            run("calculation", "Debug", "compute", "missing-include",
                ("c_project.header_missing", "middle.c", "generated.h", "header-generation"), alternate)
        finally:
            alternate.unlink()
        require(args.database.read_bytes() == before, "Import rewrote compile_commands.json")
    return 0


if __name__ == "__main__":
    sys.exit(main())
