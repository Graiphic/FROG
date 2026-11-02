#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
"""Exercise the supplied real kit CLI against an externally supplied fixture.

This is a CLI/ABI host contract check. It neither generates valid Source nor
substitutes a Python frontend; corpus and Studio result captures are supplied
by their owner.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess
import tempfile


def need(condition, message):
    if not condition:
        raise AssertionError(message)


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def diagnostic_codes(value):
    if isinstance(value, dict):
        for key, child in value.items():
            if key == "code" and isinstance(child, str) and child:
                yield child
            else:
                yield from diagnostic_codes(child)
    elif isinstance(value, list):
        for child in value:
            yield from diagnostic_codes(child)


def run_cli(cli, directory, operation, parameters=(), expected=0):
    result = subprocess.run([str(cli), operation, *map(str, parameters)], cwd=directory,
                            capture_output=True, text=True, encoding="utf-8", timeout=90)
    need(result.returncode == expected,
         f"CLI {operation}: expected {expected}, got {result.returncode}\n{result.stdout}\n{result.stderr}")
    return result


def require_frontend_pin(pin, frontend):
    need(pin.get("format") == "frog.studio.frontend-pin" and pin.get("version") == 1,
         "Unsupported explicit Frontend pin format")
    for pinned, actual in (("api_version", "api_version"), ("software_version", "software_version"),
                           ("base_revision", "commit"), ("content_sha256", "sha256")):
        expected = str(pin[pinned]) if pinned == "api_version" else pin[pinned]
        need(type(expected) is str and expected and frontend.get(actual) == expected,
             "The real Frontend differs from its explicit pin: " + pinned)


def check_frontend_pin(pin, frontend):
    require_frontend_pin(pin, frontend)
    for key in ("api_version", "software_version", "base_revision", "content_sha256"):
        changed = dict(pin)
        if key == "api_version":
            changed[key] = pin[key] + 1
        elif key in ("base_revision", "content_sha256"):
            changed[key] = ("b" if pin[key][0] == "a" else "a") + pin[key][1:]
        else:
            changed[key] = pin[key] + "-explicit-other-version"
        try:
            require_frontend_pin(changed, frontend)
        except AssertionError as error:
            need(key in str(error), "Pin mutation was refused for an unrelated field")
        else:
            raise AssertionError("A changed Frontend pin was accepted: " + key)


def studio_parity(cli, corpus):
    corpus = corpus.resolve(strict=True)
    need(corpus.is_dir(), "The Studio parity export must be an existing directory")
    watched = {}

    def selected(name):
        path = (corpus / name).resolve(strict=True)
        need(path.is_relative_to(corpus) and path.is_file(), "A Studio parity input escaped its explicit export directory")
        need(path.stat().st_size <= 64 * 1024 * 1024, "A Studio parity document exceeded the bounded read budget")
        watched[path] = digest(path)
        return path

    index = selected("index.json")
    need(index.stat().st_size <= 64 * 1024, "The Studio parity index exceeded its bounded budget")
    cases = json.loads(index.read_bytes())
    need(isinstance(cases, list) and 0 < len(cases) <= 64, "The Studio parity index must contain a bounded nonempty case array")
    seen = set()
    calls = 0
    outcome_status = {"success": 0, "rejected": 2, "unsupported": 3}
    outcome_counts = dict.fromkeys(outcome_status, 0)
    required_fields = {"name", "source_uri", "document_id", "fir_document_id",
                       "source_format", "source_version", "semantic_context"}
    with tempfile.TemporaryDirectory(prefix="frog-frontend-studio-parity-") as temporary:
        directory = Path(temporary)
        for case in cases:
            need(isinstance(case, dict) and required_fields <= set(case) <= required_fields | {"outcome"},
                 "A Studio parity index entry has an undeclared or missing field")
            need(all(isinstance(value, str) and value and "\0" not in value for value in case.values()),
                 "Studio parity identities and names must be nonempty explicit strings")
            outcome = case.get("outcome", "success")
            need(outcome in outcome_status, "A Studio parity outcome must be success, rejected or unsupported")
            outcome_counts[outcome] += 1
            name = case["name"]
            need(re.fullmatch(r"[A-Za-z0-9][A-Za-z0-9_.-]{0,95}", name) is not None and name.casefold() not in seen,
                 "Studio parity case names must be distinct confined filenames")
            seen.add(name.casefold())
            source = selected(name + ".source.frog")
            semantic_path = selected(case["semantic_context"])
            semantic = json.loads(semantic_path.read_bytes())
            need(isinstance(semantic, dict) and semantic.get("profile") == "frog.frontend.resolution@2" and
                 isinstance(semantic.get("dependencies"), list) and isinstance(semantic.get("conditional_disable"), dict),
                 "The Studio export omitted its explicit complete semantic context")
            need(case["source_format"] in ("frog.source", "frog.document.draft"),
                 "The parity source format is not an admitted original source family")
            expected = {kind: selected(name + suffix).read_bytes() for kind, suffix in (
                ("fir", ".fir.json"), ("diagnostics", ".diagnostics.json"),
                ("maps", ".maps.json"), ("provenance", ".provenance.json"))}
            expected_provenance = json.loads(expected["provenance"])
            if outcome == "success":
                need(json.loads(expected["fir"])["document_id"] == case["fir_document_id"],
                     "The successful Studio FIR disagrees with its requested document identity")
            else:
                need(expected["fir"] == b"", "A refused Studio export must have a zero-byte FIR document")
                need(list(diagnostic_codes(json.loads(expected["diagnostics"]))),
                     "A refused Studio export omitted its attributed diagnostic code")
            need(expected_provenance["source"]["uri"] == case["source_uri"] and
                 expected_provenance["source"]["document_id"] == case["document_id"] and
                 expected_provenance["source"]["sha256"] == watched[source] and
                 expected_provenance["source"]["family"] == case["source_format"] and
                 expected_provenance["source"]["version"] == case["source_version"] and
                 expected_provenance["semantic_context"] == semantic and
                 expected_provenance["fir"]["document_id"] == case["fir_document_id"],
                 "The Studio export disagrees with its explicit Source/FIR identities or exact captured bytes")
            common = ["--source", source, "--source-uri", case["source_uri"],
                      "--document-id", case["document_id"], "--fir-document-id", case["fir_document_id"],
                      "--source-format", case["source_format"], "--source-version", case["source_version"],
                      "--semantic-context", semantic_path, "--fir-version", "0.1"]
            for operation in ("validate", "derive"):
                outputs = {kind: directory / (name + "." + operation + "." + kind + ".json")
                           for kind in ("diagnostics", "maps", "provenance")}
                parameters = [*common, "--diagnostics-output", outputs["diagnostics"],
                              "--source-maps-output", outputs["maps"], "--provenance-output", outputs["provenance"]]
                if operation == "derive":
                    outputs["fir"] = directory / (name + ".derive.fir.json")
                    parameters += ["--output", outputs["fir"]]
                result = run_cli(cli, directory, operation, parameters, expected=outcome_status[outcome])
                calls += 1  # Exit 0/2/3 must match the exported real C ABI outcome.
                need(json.loads(result.stdout) == json.loads(expected["diagnostics"]),
                     f"Studio/CLI {name} {operation} diagnostics stdout differs")
                for kind, output in outputs.items():
                    if kind == "fir" and outcome != "success":
                        need(not output.exists(), f"Refused CLI derivation created FIR: case={name} outcome={outcome}")
                        continue
                    actual = output.read_bytes()
                    need(actual == expected[kind],
                         f"Studio/CLI byte parity mismatch: case={name} operation={operation} document={kind} "
                         f"expected_sha256={hashlib.sha256(expected[kind]).hexdigest()} "
                         f"actual_sha256={hashlib.sha256(actual).hexdigest()}")
        for path, before in watched.items():
            need(digest(path) == before, f"CLI changed a read-only Studio parity input: {path}")
    outcomes = ",".join(name.upper() for name, count in outcome_counts.items() if count)
    print(f"FRONTEND_STUDIO_CLI_PARITY_PASS cases={len(cases)} calls={calls} outcomes={outcomes} "
          f"success_cases={outcome_counts['success']} rejected_cases={outcome_counts['rejected']} "
          f"unsupported_cases={outcome_counts['unsupported']} "
          "byte_exact_fir_diagnostics_maps_provenance=1 immutable_exports=1")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cli", type=Path, required=True)
    parser.add_argument("--studio-parity-directory", type=Path,
                        help="Read-only Studio index/documents; exclusive with the single-fixture options")
    parser.add_argument("--source", type=Path)
    parser.add_argument("--source-format")
    parser.add_argument("--source-version")
    parser.add_argument("--fir-version")
    parser.add_argument("--source-uri")
    parser.add_argument("--document-id")
    parser.add_argument("--fir-document-id")
    parser.add_argument("--invalid-source", type=Path)
    parser.add_argument("--dependencies", type=Path)
    parser.add_argument("--check-dependency-failures", action="store_true",
                        help="The supplied valid Source requires the supplied nonempty lock; test missing/conflicting snapshots")
    parser.add_argument("--semantic-context", type=Path)
    parser.add_argument("--c-consumer", type=Path)
    parser.add_argument("--frontend-pin", type=Path,
                        help="Compare actual producer API/software/base revision/content with this immutable Studio pin")
    args = parser.parse_args()
    if args.studio_parity_directory is not None:
        if any(value is not None and value is not False for key, value in vars(args).items()
               if key not in ("cli", "studio_parity_directory")):
            parser.error("--studio-parity-directory accepts only --cli and the export directory")
        cli = args.cli.resolve(strict=True)
        need(cli.is_file(), "The real kit CLI is required")
        studio_parity(cli, args.studio_parity_directory)
        return
    for key in ("source", "source_format", "source_version", "fir_version"):
        if getattr(args, key) is None:
            parser.error("Required single-fixture option missing: --" + key.replace("_", "-"))
    for key, default in (("source_uri", "frog-test:/frontend-cli/source.frog"),
                         ("document_id", "source:frontend-cli-fixture"),
                         ("fir_document_id", "fir:frontend-cli-fixture")):
        if getattr(args, key) is None:
            setattr(args, key, default)
    need(args.document_id != args.fir_document_id,
         "This identity oracle requires distinct supplied Source and FIR document identities")
    need(not args.check_dependency_failures or args.dependencies,
         "Dependency-failure checks require the owner's valid dependency lock/fixture")
    cli, source = args.cli.resolve(), args.source.resolve()
    need(cli.is_file() and source.is_file(), "The real CLI and the owner's source fixture are required")
    watched = {source: digest(source)}
    frontend_pin = None
    if args.frontend_pin:
        path = args.frontend_pin.resolve(strict=True)
        watched[path] = digest(path)
        frontend_pin = json.loads(path.read_text(encoding="utf-8"))
    for candidate in (args.invalid_source, args.dependencies, args.semantic_context):
        if candidate:
            watched[candidate.resolve()] = digest(candidate.resolve())
    if args.dependencies:
        lock = args.dependencies.resolve()
        for item in json.loads(lock.read_text(encoding="utf-8"))["dependencies"]:
            entry = Path(item["path"])
            selected = (entry if entry.is_absolute() else lock.parent / entry).resolve()
            watched[selected] = digest(selected)
    with tempfile.TemporaryDirectory(prefix="frog-frontend-cli-") as temporary:
        directory = Path(temporary)
        common = ["--source", str(source), "--source-uri", args.source_uri,
                  "--document-id", args.document_id, "--fir-document-id", args.fir_document_id,
                  "--source-format", args.source_format,
                  "--source-version", args.source_version, "--fir-version", args.fir_version]
        if args.dependencies:
            common += ["--dependencies", str(args.dependencies.resolve())]
        if args.semantic_context:
            common += ["--semantic-context", str(args.semantic_context.resolve())]
        calls = 0

        def run(operation, parameters=(), expected=0):
            nonlocal calls
            calls += 1
            return run_cli(cli, directory, operation, parameters, expected)

        descriptor = directory / "descriptor.json"
        described = run("describe", ["--descriptor-output", descriptor])
        need(json.loads(described.stdout) == json.loads(descriptor.read_text(encoding="utf-8")),
             "Describe stdout differs from the actual descriptor output")
        need(isinstance(json.loads(described.stdout), dict), "Real describe did not return a descriptor object")
        need(json.loads(described.stdout)["frontend"]["api_version"] == "3", "The supplied CLI did not identify API 3")
        if frontend_pin is not None:
            check_frontend_pin(frontend_pin, json.loads(described.stdout)["frontend"])
        run("describe", ["--fir-document-id", args.fir_document_id], expected=64)
        missing_fir_id = list(common)
        at = missing_fir_id.index("--fir-document-id")
        del missing_fir_id[at:at + 2]
        run("validate", missing_fir_id, expected=64)
        run("derive", [*missing_fir_id, "--output", directory / "missing-id.fir.json"], expected=64)
        need(not (directory / "missing-id.fir.json").exists(), "Missing explicit FIR identity published an artifact")
        run("validate", [*common, "--fir-document-id", args.fir_document_id], expected=64)
        run("validate", common)
        fir = directory / "program.fir.json"
        diagnostics, maps, provenance = [directory / name for name in ("diagnostics.json", "maps.json", "provenance.json")]
        outputs = ["--output", fir, "--diagnostics-output", diagnostics,
                   "--source-maps-output", maps, "--provenance-output", provenance]
        run("derive", [*common, *outputs])
        if frontend_pin is not None:
            check_frontend_pin(frontend_pin, json.loads(provenance.read_text(encoding="utf-8"))["frontend"])
        original = fir.read_bytes()
        need(isinstance(json.loads(original), dict) and original, "Derive did not emit real complete FIR")
        need(json.loads(original)["document_id"] == args.fir_document_id,
             "The CLI replaced the requested FIR document identity with its Source identity")
        authored = json.loads(provenance.read_text(encoding="utf-8"))["source"]
        need(authored["document_id"] == args.document_id and authored["sha256"] == digest(source),
             "Explicit FIR identity relabeled the immutable authored Source")
        need(json.loads(provenance.read_text(encoding="utf-8"))["fir"]["document_id"] == args.fir_document_id,
             "Provenance lost the separately requested FIR document identity")
        first_documents = [p.read_bytes() for p in (diagnostics, maps, provenance)]
        for payload in first_documents:
            need(isinstance(json.loads(payload), (dict, list)), "A requested result is not a complete JSON document")
        run("derive", [*common, *outputs])
        need(fir.read_bytes() == original and [p.read_bytes() for p in (diagnostics, maps, provenance)] == first_documents,
             "Repeated identical CLI derivation changed FIR/maps/diagnostics/provenance")
        # These inputs are explicitly invalid CLI/JSON cases, not another
        # implementation or a fabricated valid program corpus.
        malformed = args.invalid_source.resolve() if args.invalid_source else directory / "malformed.frog"
        if not args.invalid_source:
            malformed.write_bytes(b"{")
        invalid = list(common)
        invalid[invalid.index("--source") + 1] = str(malformed)
        run("derive", [*invalid, *outputs], expected=2)
        need(fir.read_bytes() == original, "Rejected derivation overwrote the last valid FIR")
        need(list(diagnostic_codes(json.loads(diagnostics.read_text(encoding="utf-8")))), "Rejected derivation emitted no attributed diagnostic code")
        run("derive", [*common, "--output", fir, "--max-source-bytes", "1"], expected=5)
        need(fir.read_bytes() == original, "A limit failure overwrote the last valid FIR")
        unsupported = list(common)
        unsupported[unsupported.index("--source-version") + 1] = "unsupported-test-version"
        run("derive", [*unsupported, "--output", fir], expected=3)
        need(fir.read_bytes() == original, "Unsupported input overwrote the last valid FIR")
        run("derive", [*common, "--output", fir, "--unknown-option", "x"], expected=64)
        run("validate", [*common, "--output", fir], expected=64)
        run("derive", [*common, "--output", fir, "--diagnostics-output", fir], expected=64)
        run("derive", [*common, "--output", source], expected=64)
        before = fir.stat().st_mtime_ns
        run("derive", [*common, "--output", fir, "--diagnostics-output", directory / "absent" / "diagnostics.json"], expected=64)
        need(fir.read_bytes() == original and fir.stat().st_mtime_ns == before,
             "An auxiliary-output I/O failure republished the main FIR")
        bad_lock = directory / "invalid-lock.json"
        bad_lock.write_text(json.dumps({"format": "frog.frontend.dependencies", "version": "2", "dependencies": []}), encoding="utf-8")
        without_lock = list(common)
        if "--dependencies" in without_lock:
            at = without_lock.index("--dependencies")
            del without_lock[at:at + 2]
        run("validate", [*without_lock, "--dependencies", bad_lock], expected=64)
        if args.check_dependency_failures:
            # The owner supplies the real dependency-bearing program. Only
            # host lock metadata is changed; no alternative Source is made.
            run("derive", [*without_lock, *outputs], expected=2)
            need(fir.read_bytes() == original and list(diagnostic_codes(json.loads(diagnostics.read_text(encoding="utf-8")))),
                 "A missing explicit lock resolved implicitly, lost its diagnostic or replaced valid FIR")
            supplied_lock = args.dependencies.resolve()
            altered = json.loads(supplied_lock.read_text(encoding="utf-8"))
            need(altered["dependencies"], "The dependency-bearing fixture needs a nonempty real lock")
            for item in altered["dependencies"]:
                selected = Path(item["path"])
                item["path"] = str((selected if selected.is_absolute() else supplied_lock.parent / selected).resolve())
            altered["dependencies"][0]["identity"] += "-cli-conflicting-identity"
            conflicting_lock = directory / "conflicting-snapshot-lock.json"
            conflicting_lock.write_text(json.dumps(altered, ensure_ascii=False), encoding="utf-8")
            run("derive", [*without_lock, "--dependencies", conflicting_lock, *outputs], expected=2)
            need(fir.read_bytes() == original and list(diagnostic_codes(json.loads(diagnostics.read_text(encoding="utf-8")))),
                 "Conflicting dependency identity was trusted, lost its diagnostic or replaced valid FIR")
        need(not list(directory.glob("*.frog-front-*.tmp")), "Failed publication retained partial sibling files")
        for selected, before_hash in watched.items():
            need(digest(selected) == before_hash, f"CLI changed an immutable input: {selected}")
        if args.c_consumer:
            consumer = subprocess.run([str(args.c_consumer.resolve())], cwd=directory,
                                      capture_output=True, text=True, encoding="utf-8", timeout=30)
            need(consumer.returncode == 0 and "FRONTEND_C_CONSUMER_PASS" in consumer.stdout,
                 f"The actual C ABI consumer failed\n{consumer.stdout}\n{consumer.stderr}")
        print(f"FRONTEND_CLI_CHECKS_PASS calls={calls} supplied_fixture=1 explicit_cwd_isolation=1 exact_inputs_unchanged=1 last_valid_fir_preserved=1 result_outputs_real=1 dependency_failure_cases={2 if args.check_dependency_failures else 0}")


if __name__ == "__main__":
    main()
