"""Mechanical source-capture regressions; no substitute Frontend or compiler.

The tiny files below are packaging metadata/byte fixtures, never executable
language examples. Closure checking uses the existing real audit implementation.
"""
import importlib.util
from pathlib import Path
import re
import tempfile
import unittest
from unittest import mock

TOOL = Path(__file__).resolve().parents[1] / "tools/export_frontend_candidate.py"
spec = importlib.util.spec_from_file_location("frontend_candidate_export", TOOL)
exporter = importlib.util.module_from_spec(spec)
spec.loader.exec_module(exporter)


class CandidateExportTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory(prefix="frog-candidate-export-")
        self.addCleanup(self.temporary.cleanup)
        self.base = Path(self.temporary.name)
        self.frog, self.kit = self.base / "FROG", self.base / "kit"
        self.front = self.frog / exporter.FRONT
        fixtures = {
            "LICENSE": "Apache-2.0 fixture text; not a production licence inventory\n",
            exporter.FRONT + "/LICENSE": "Apache-2.0 fixture text\n",
            exporter.FRONT + "/NOTICE": "Mechanical packaging fixture\n",
            exporter.FRONT + "/README.md": "Exact fixture bytes, no language evaluation.\n",
            exporter.FRONT + "/CMakeLists.txt": 'set(FROG_FRONTEND_SOFTWARE_VERSION "0.1.0-local")\nset(FROG_FRONTEND_BASE_REVISION "' + "1"*40 + '")\n',
            exporter.FRONT + "/production-sources.cmake": "set(FROG_FRONTEND_PRODUCTION_SOURCES src/packaging_fixture.cpp)\n",
            exporter.FRONT + "/src/packaging_fixture.cpp": "// Mechanical closure fixture, never compiled.\n",
            exporter.FRONT + "/src/FrontendApi.cpp": "// API filename required by the capture identity contract.\n",
            exporter.FRONT + "/src/FrontendOperation.cpp": "// Operation filename required by the capture identity contract.\n",
            exporter.FRONT + "/src/FrontendCli.cpp": "// CLI filename required by the existing closure audit.\n",
            exporter.FRONT + "/snapshot/api/frog/frontend.h": "#define FROG_FRONTEND_API_VERSION UINT32_C(3)\n",
            "IR/schema/fixture.schema.json": "{}\n",
            "IR/Readme.md": "IR documentation byte fixture\n",
            "sdk/ecosystem/docs/fixture.md": "Public SDK documentation byte fixture\n",
            "sdk/ecosystem/schemas/fixture.schema.json": "{}\n",
            "sdk/ecosystem/fixtures/fixture.json": "{}\n",
            "sdk/ecosystem/tests/validate_frontend_results.py": "# Mechanical capture support fixture.\n",
            "sdk/ecosystem/tests/test_frontend_documents.py": "# Mechanical capture support fixture.\n",
        }
        for name, text in fixtures.items():
            path = self.frog / name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(text, encoding="utf-8", newline="\n")
        origin = {"sources": [{"path": "snapshot/api/frog/frontend.h", "ownership": "snapshot",
                  "sha256": exporter.digest((self.front / "snapshot/api/frog/frontend.h").read_bytes())}]}
        (self.front / "source-origin.json").write_bytes(exporter.encoded(origin))
        self.refresh_inventory()
        identity = exporter.implementation(self.front)
        self.pin = self.base / "pin.json"
        self.pin.write_bytes(exporter.encoded({"format": "frog.studio.frontend-pin", "version": 1, **identity,
                                             "local_overlay_required": True}))

    def refresh_inventory(self):
        path = self.front / "docs/license-inventory.json"
        path.parent.mkdir(exist_ok=True)
        path.write_bytes(b"{}")
        rows = []
        for source in sorted(self.front.rglob("*")):
            if source.is_file():
                name = source.relative_to(self.front).as_posix()
                self_entry = source == path
                rows.append({"path": name, "root": "frontend", "current_hash": None if self_entry else exporter.digest(source.read_bytes()),
                             "current_bytes": None if self_entry else source.stat().st_size,
                             **({"hash_exclusion": "inventory self-hash/size excluded"} if self_entry else {})})
        path.write_bytes(exporter.encoded({"schemaVersion": 3, "redistributionRightsGrantedForSelectedGraiphicOriginals": True, "files": rows}))

    def capture(self):
        return exporter.export(self.frog, self.kit, self.pin)

    def manifest(self):
        return exporter.document((self.kit / "source-kit.json").read_bytes())

    def save_manifest(self, value):
        (self.kit / "source-kit.json").write_bytes(exporter.encoded(value))

    def test_compressor_intrinsic_is_scoped_and_unresolved_includes_refuse(self):
        # These are source-closure fixtures, never a replacement compressor.
        vendor = self.front / "third_party/zstd/lib/compress/zstd_compress.c"
        vendor.parent.mkdir(parents=True)
        vendor.write_text("#include <immintrin.h>\n", encoding="utf-8")
        cmake = self.front / "CMakeLists.txt"
        cmake.write_text(cmake.read_text(encoding="utf-8") +
                         "\nadd_library(frog-frontend-zstd STATIC third_party/zstd/lib/compress/zstd_compress.c)\n"
                         "target_compile_definitions(frog-frontend-zstd PRIVATE ZSTD_DISABLE_ASM=1 ZSTD_LEGACY_SUPPORT=0)\n",
                         encoding="utf-8")
        source = self.front / "src/packaging_fixture.cpp"
        source.write_text('#include "../third_party/zstd/lib/compress/zstd_compress.c"\n', encoding="utf-8")
        report = exporter.audit_closure(self.front)
        self.assertTrue(any(item["source"] == "third_party/zstd/lib/compress/zstd_compress.c" and
                            item["include"] == "immintrin.h" for item in report["compilerHeaderRequirements"]))
        vendor.write_text("#include <not_a_compiler_intrinsic.h>\n", encoding="utf-8")
        with self.assertRaisesRegex(exporter.ExportError, "unresolved-include"):
            exporter.audit_closure(self.front)
        vendor.write_text("#include <immintrin.h>\n", encoding="utf-8")
        source.write_text("#include <immintrin.h>\n", encoding="utf-8")
        with self.assertRaisesRegex(exporter.ExportError, "unresolved-include"):
            exporter.audit_closure(self.front)

    def test_capture_exact_identity_notice_pin_and_self_exclusion(self):
        before = {p: p.read_bytes() for p in self.frog.rglob("*") if p.is_file()}
        result = self.capture()
        self.assertEqual(result["status"], "verified")
        manifest = self.manifest()
        self.assertEqual(manifest["format"], "frog.frontend.local-source-kit")
        self.assertNotIn("source-kit.json", [e["path"] for e in manifest["files"]])
        self.assertEqual((self.kit / "NOTICE").read_bytes(), (self.front / "NOTICE").read_bytes())
        self.assertEqual(manifest["expected_studio_pin"], exporter.document(self.pin.read_bytes()))
        self.assertFalse(manifest["standalone_reconstruction_passed"])
        self.assertEqual(exporter.verify(self.kit, self.pin)["manifest_sha256"], result["manifest_sha256"])
        self.assertEqual(before, {p: p.read_bytes() for p in self.frog.rglob("*") if p.is_file()})
        self.assertNotIn(str(self.frog), (self.kit / "source-kit.json").read_text(encoding="utf-8"))

    def test_tamper_missing_and_unlisted_file_refuse(self):
        self.capture()
        path = self.kit / "LICENSE"
        original = path.read_bytes()
        path.write_bytes(original+b"changed")
        with self.assertRaisesRegex(exporter.ExportError, "differs from manifest"):
            exporter.verify(self.kit)
        path.unlink()
        with self.assertRaises(OSError):
            exporter.verify(self.kit)
        path.write_bytes(original)
        (self.kit / "unlisted.exe").write_bytes(b"not an executable")
        with self.assertRaisesRegex(exporter.ExportError, "file closure differs"):
            exporter.verify(self.kit)

    def test_manifest_escape_private_and_case_collision_refuse(self):
        self.capture()
        original = self.manifest()
        for name in ("../outside.txt", "/absolute.txt", "C:/absolute.txt", "Implementations/Frontend/../outside.txt", "FROG-Runtime/cpp/private.cpp"):
            with self.subTest(name=name):
                changed = dict(original)
                changed["files"] = original["files"] + [{"path": name, "sha256": "0"*64, "bytes": 0}]
                self.save_manifest(changed)
                with self.assertRaises(exporter.ExportError):
                    exporter.verify(self.kit)
        changed = dict(original)
        changed["files"] = original["files"] + [{"path": "license", "sha256": "0"*64, "bytes": 0}]
        self.save_manifest(changed)
        with self.assertRaises(exporter.ExportError):
            exporter.verify(self.kit)

    def test_snapshot_stale_inventory_and_pin_refuse_before_output(self):
        snapshot = self.front / "snapshot/api/frog/frontend.h"
        raw = snapshot.read_bytes()
        snapshot.write_bytes(raw+b"// drift\n")
        with self.assertRaisesRegex(exporter.ExportError, "snapshot drift"):
            self.capture()
        self.assertFalse(self.kit.exists())
        snapshot.write_bytes(raw)
        readme = self.front / "README.md"
        readme.write_bytes(readme.read_bytes()+b"unrecorded")
        with self.assertRaisesRegex(exporter.ExportError, "inventory is stale"):
            self.capture()
        self.refresh_inventory()
        pin = exporter.document(self.pin.read_bytes())
        pin["base_revision"] = "2"*40
        self.pin.write_bytes(exporter.encoded(pin))
        with self.assertRaisesRegex(exporter.ExportError, "pin differs: base_revision"):
            self.capture()
        self.assertFalse(self.kit.exists())

    def test_source_changes_during_capture_are_not_sealed(self):
        real_audit = exporter.audit_closure
        def changing_audit(front):
            report = real_audit(front)
            path = front / "README.md"
            path.write_bytes(path.read_bytes()+b"concurrent edit")
            return report
        with mock.patch.object(exporter, "audit_closure", side_effect=changing_audit):
            with self.assertRaisesRegex(exporter.ExportError, "changed during capture"):
                self.capture()
        self.assertFalse(self.kit.exists())

    def test_cli_change_invalidates_identity_and_old_pin(self):
        original = exporter.implementation(self.front)
        cli = self.front / "src/FrontendCli.cpp"
        cli.write_bytes(cli.read_bytes() + b"// changed CLI behavior\n")
        current = exporter.implementation(self.front)
        self.assertNotEqual(original["content_sha256"], current["content_sha256"])
        self.assertEqual(original["software_version"], current["software_version"])
        self.assertEqual(original["base_revision"], current["base_revision"])
        self.refresh_inventory()
        with self.assertRaisesRegex(exporter.ExportError, "pin differs: content_sha256"):
            self.capture()
        self.assertFalse(self.kit.exists())
        pin = exporter.document(self.pin.read_bytes())
        pin.update(current)
        self.pin.write_bytes(exporter.encoded(pin))
        result = self.capture()
        self.assertEqual(result["frontend_content_sha256"], current["content_sha256"])
        self.assertEqual(exporter.verify(self.kit, self.pin)["status"], "verified")

    def test_cmake_identity_includes_cli_in_same_order(self):
        # Read the actual build owner; no compiler or substitute build runs.
        cmake = (TOOL.parents[1] / "cmake/FrontendInputs.cmake").read_text(encoding="utf-8")
        match = re.search(r"foreach\(path IN ITEMS\s+([^)]*)\)", cmake)
        self.assertIsNotNone(match)
        paths = match.group(1).split()
        self.assertEqual(paths, ["src/FrontendApi.cpp", "src/FrontendOperation.cpp",
                                 "src/FrontendCli.cpp", "CMakeLists.txt", "production-sources.cmake"])
        origins = exporter.document((self.front / "source-origin.json").read_bytes())
        records = "".join(entry["path"] + ":" + exporter.digest((self.front / entry["path"]).read_bytes()) + "\n"
                          for entry in origins["sources"])
        records += "".join(name + ":" + exporter.digest((self.front / name).read_bytes()) + "\n" for name in paths)
        self.assertEqual(exporter.frontend_identity(self.front), exporter.digest(records.encode("utf-8")))

    def test_build_named_sources_are_kept_but_build_directories_are_excluded(self):
        retained = {
            "include/frog_native_engine/BuildArrayBinding.hpp": "#pragma once\n",
            "src/BuildArrayBinding.cpp": '#include "frog_native_engine/BuildArrayBinding.hpp"\n',
            "tools/Build-Frontend.py": "# Authored build tool, not a build output directory.\n",
        }
        origins = exporter.document((self.front / "source-origin.json").read_bytes())
        for name, text in retained.items():
            path = self.front / name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(text, encoding="utf-8")
            origins["sources"].append({"path": name, "ownership": "created", "sha256": exporter.digest(path.read_bytes())})
        (self.front / "source-origin.json").write_bytes(exporter.encoded(origins))
        (self.front / "production-sources.cmake").write_text(
            "set(FROG_FRONTEND_PRODUCTION_SOURCES src/packaging_fixture.cpp src/BuildArrayBinding.cpp)\n", encoding="utf-8")
        self.refresh_inventory()
        pin = exporter.document(self.pin.read_bytes())
        pin.update(exporter.implementation(self.front))
        self.pin.write_bytes(exporter.encoded(pin))
        # Outputs are not part of the inventory. They must stay excluded even
        # when their file extensions are otherwise admitted source extensions.
        excluded = ("build-ninja/generated.hpp", "src/.build-cache/generated.hpp", "tests/build-generated/result.json")
        for name in excluded:
            path = self.front / name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text("generated output\n", encoding="utf-8")
        selected = exporter.source_selection(self.frog)
        for name in retained:
            self.assertIn(exporter.FRONT + "/" + name, selected)
        for name in excluded:
            self.assertNotIn(exporter.FRONT + "/" + name, selected)
        self.assertEqual(self.capture()["status"], "verified")
        for name in retained:
            self.assertEqual((self.kit / exporter.FRONT / name).read_bytes(), (self.front / name).read_bytes())


if __name__ == "__main__":
    unittest.main()
