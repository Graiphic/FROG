#!/usr/bin/env python3
"""Audit the real frontend's literal production-source/include closure, without building.

This is a source dependency check, not a preprocessor, linker or licence grant.
All conditional includes are checked. Comments/literals do not count as symbols.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import re
import sys
from pathlib import Path


STANDARD_HEADERS = frozenset("""
algorithm any array atomic barrier bit bitset assert.h cassert ccomplex cctype
cerrno cfenv cfloat charconv chrono cinttypes ciso646 climits clocale cmath codecvt
compare complex complex.h concepts condition_variable coroutine csetjmp csignal
cstdalign cstdarg cstdbool cstddef cstdint cstdio cstdlib cstring ctgmath ctime
cuchar cwchar cwctype deque errno.h exception execution fenv.h filesystem float.h
forward_list fstream functional future initializer_list inttypes.h iomanip ios
iosfwd iostream istream iterator latch limits limits.h list locale locale.h map
math.h memory memory_resource mutex new numbers numeric optional ostream queue
random ranges ratio regex scoped_allocator semaphore set setjmp.h shared_mutex
signal.h source_location span sstream stack stdalign.h stdarg.h stdbool.h stddef.h
stdint.h stdio.h stdlib.h stdexcept stop_token streambuf string string.h string_view
strstream syncstream system_error tgmath.h thread time.h tuple type_traits
typeindex typeinfo uchar.h unordered_map unordered_set utility valarray variant
vector version wchar.h wctype.h
""".split())

FORBIDDEN_INCLUDES = (
    ("private-runtime-include", re.compile(r"(^|/)(frog/runtime|FROG-Runtime)(/|$)", re.I)),
    ("private-studio-include", re.compile(r"(^|/)FROG-STUDIO(/|$)", re.I)),
    ("editor-model-include", re.compile(r"(^|/)[^/]*(?:Canvas|Controller|LoadedFrogDocument|WidgetInstance|FrogDocument|DiagramNode)[^/]*$")),
    ("win32-include", re.compile(r"(^|/)(?:windows|windowsx|commctrl|commdlg|shellapi|shlobj|shlwapi|objbase|ole2|wingdi|winuser|winnt|d2d1|dwrite)\b", re.I)),
    ("llvm-include", re.compile(r"(^|/)(?:llvm|llvm-c|clang)(/|$)", re.I)),
)
FORBIDDEN_SYMBOLS = (
    ("editor-model-symbol", re.compile(r"\b(?:[A-Za-z_]\w*Canvas\w*|Canvas\w*|WidgetInstance\w*|FrogDocument\w*|LoadedFrogDocument|ShellState|[A-Z]\w*Controller\w*)\b")),
    ("diagram-node-value-symbol", re.compile(r"\bDiagramNode\s+(?:[A-Za-z_]\w*\s*[;={]|\{)|\bDiagramNode\s*\{")),
    ("win32-symbol", re.compile(r"\b(?:HWND|HINSTANCE|HDC|HBITMAP|HMENU|WNDPROC|WNDCLASSW?|MSG|WPARAM|LPARAM|LRESULT|HANDLE|MEMORYSTATUSEX|GlobalMemoryStatusEx|CreateWindow(?:Ex)?[AW]?|DestroyWindow|DefWindowProc[AW]?|MessageBox[AW]?|LoadLibrary(?:Ex)?[AW]?|GetProcAddress|FreeLibrary|CreateFile[AW]|GetModuleHandle[AW]?|GetModuleFileName[AW]?)\b")),
    ("llvm-symbol", re.compile(r"\b(?:llvm|clang)\s*::|\bLLVM[A-Z]\w*|\b(?:WindowsLlvm|WindowsLLVM)\w*\b")),
    ("runtime-execution-symbol", re.compile(r"\b(?:WorkBudget|ProgramSession|NativeExecutionSession|NativeExecutionDispatcher|CanonicalRuntimeSession|CanonicalPublicRunSession|CanonicalExecutionWorker)\b")),
)
PEER_WINDOWS_HEADERS = frozenset({"windows.h", "winver.h", "bcrypt.h"})

# These are the actual extracted data/codec owners, not an exception for all
# names beginning with WidgetInstance/FrogDocument. An include is admitted only
# when it resolves to this exact path inside Frontend; its contents are still
# scanned for UI/runtime/platform dependencies.
PUBLIC_MODEL_HEADERS = frozenset({
    "include/frog_native_engine/WidgetInstance.hpp",
    "include/frog_native_engine/FrogDocumentFormat.hpp",
    "include/frog_native_engine/FrogDocumentWidgetValues.hpp",
    *("src/FrogDocument" + name + ".hpp" for name in (
        "ApplicationControlSerialization", "ArrayStorage", "CaseStructureSerialization",
        "ConditionalDisable", "ConditionalDisableSerialization", "DisabledStructureSerialization",
        "Encoding", "EventStructureSerialization", "ForLoopSerialization", "IdeSerialization",
        "Identity", "IntegerLexeme", "JsonValue", "JsonWriter", "PublicImport",
        "StaticVisualSerialization", "StructureSerializationHelpers", "TimedLoopSerialization",
        "UnavailableNumeric", "WaveformGraph", "WhileLoopSerialization", "XYGraph", "ZstdCodec",
    )),
})
PUBLIC_MODEL_TYPES = {
    "WidgetInstance": "include/frog_native_engine/WidgetInstance.hpp",
    "DiagramNode": "include/frog_native_engine/FrogDraftData.hpp",
    "FrogDocumentState": "include/frog_native_engine/FrogDraftData.hpp",
    "FrogDocumentStateData": "include/frog_native_engine/FrogDraftData.hpp",
    "FrogDocumentReadDiagnostic": "include/frog_native_engine/FrogDraftData.hpp",
    "FrogDocumentReadErrorKind": "include/frog_native_engine/FrogDraftData.hpp",
}
PUBLIC_MODEL_SYMBOLS = frozenset((*PUBLIC_MODEL_TYPES, "FrogDocumentCompressFn"))
PUBLIC_DATA_ACCESSORS = {
    "frontPanelCanvas": frozenset({"src/FrogDraftReader.cpp"}),
    "frontPanelCanvasSize": frozenset({"src/FrogDraftWriter.cpp", "include/frog_native_engine/FrogWidgetData.hpp"}),
    "setFrontPanelCanvasSize": frozenset({"src/FrogDraftReader.cpp", "include/frog_native_engine/FrogWidgetData.hpp"}),
}
ZSTD_COMPILER_HEADERS = {
    "third_party/zstd/lib/common/cpu.h": frozenset({"intrin.h"}),
    "third_party/zstd/lib/common/mem.h": frozenset({"intrin.h", "intrinsics.h"}),
    "third_party/zstd/lib/common/compiler.h": frozenset({"mmintrin.h", "intrin.h", "immintrin.h", "emmintrin.h", "arm_neon.h"}),
    "third_party/zstd/lib/common/bitstream.h": frozenset({"immintrin.h", "intrinsics.h"}),
    "third_party/zstd/lib/common/portability_macros.h": frozenset({"cet.h"}),
    "third_party/zstd/lib/common/xxhash.h": frozenset({"arm_sve.h", "arm_neon.h", "immintrin.h", "emmintrin.h", "intrin.h", "s390intrin.h", "altivec.h", "mmintrin.h"}),
}


class AuditFailure(Exception):
    pass


RAW_STRING_START = re.compile(r'(?:u8|u|U|L)?R"([^\s\\()]*)\(')


def masked_cpp(text: str, keep_literals: bool = False) -> str:
    """Blank comments and optionally C++ literals, preserving line coordinates."""
    out = list(text)
    pos = 0
    while pos < len(text):
        end = None
        literal = False
        if text.startswith("//", pos):
            end = text.find("\n", pos + 2)
            if end < 0:
                end = len(text)
        elif text.startswith("/*", pos):
            closing = text.find("*/", pos + 2)
            end = len(text) if closing < 0 else closing + 2
        else:
            raw = RAW_STRING_START.match(text, pos)
            if raw:
                closing = text.find(")" + raw.group(1) + '"', raw.end())
                end = len(text) if closing < 0 else closing + len(raw.group(1)) + 2
                literal = True
            elif text[pos] in "\"'":
                # Apostrophes between digits/hex digits can be C++ separators.
                if text[pos] == "'" and pos and text[pos-1].isalnum() and pos+1 < len(text) and text[pos+1].isalnum():
                    pos += 1
                    continue
                quote, end = text[pos], pos + 1
                while end < len(text):
                    if text[end] == "\\":
                        end += 2
                    elif text[end] == quote:
                        end += 1
                        break
                    else:
                        end += 1
                literal = True
        if end is not None:
            if not literal or not keep_literals:
                for index in range(pos, min(end, len(text))):
                    if text[index] not in "\r\n":
                        out[index] = " "
            pos = end
        else:
            pos += 1
    return "".join(out)


def production_sources(path: Path) -> list[str]:
    """Read a literal manifest; never evaluate CMake or execute project code."""
    text = path.read_text(encoding="utf-8-sig")
    if path.suffix.lower() == ".json":
        value = json.loads(text)
        value = value.get("sources") if isinstance(value, dict) else value
        if not isinstance(value, list) or not all(isinstance(item, str) for item in value):
            raise AuditFailure("A JSON source manifest must be an array or contain a sources array.")
        return value
    if path.suffix.lower() != ".cmake":
        return [line.strip() for line in text.splitlines() if line.strip() and not line.lstrip().startswith("#")]
    text = re.sub(r"#[^\n]*", "", text)
    blocks = re.findall(r"\bset\s*\(\s*FROG_FRONTEND_PRODUCTION_SOURCES\s+([^)]*)\)", text, re.I)
    if len(blocks) != 1:
        raise AuditFailure("Expected exactly one literal set(FROG_FRONTEND_PRODUCTION_SOURCES ...).")
    tokens = re.findall(r'"([^"\n]*)"|([^\s;]+)', blocks[0])
    values = [quoted or bare for quoted, bare in tokens]
    if any(re.search(r"[$()\\]", item) for item in values):
        raise AuditFailure("Dynamic CMake source entries cannot be independently audited.")
    return values


def frontend_root(path: Path) -> Path:
    path = path.resolve(strict=True)
    for candidate in (path, path / "Implementations/Frontend", path / "FROG/Implementations/Frontend"):
        if (candidate / "src").is_dir():
            return candidate.resolve()
    raise AuditFailure("--root must identify Frontend, FROG, or the three-repository workspace/snapshot.")


def public_root(path: Path) -> Path:
    path = path.resolve(strict=True)
    for candidate in (path, path / "FROG"):
        if all((candidate / owner).is_dir() for owner in
               ("Implementations/Frontend", "sdk/ecosystem", "Implementations/Bridges/reference-c")):
            return candidate.resolve()
    raise AuditFailure("--public-consumer requires a FROG root containing the three exact public owners.")


def literal_target_sources(path: Path, command: str, target: str) -> list[str]:
    text = re.sub(r"#[^\n]*", "", path.read_text(encoding="utf-8-sig"))
    blocks = re.findall(r"\b" + command + r"\s*\(\s*" + re.escape(target) + r"\s+([^)]*)\)", text, re.I)
    if len(blocks) != 1 or re.search(r"\btarget_sources\s*\(\s*" + re.escape(target) + r"\b", text, re.I):
        raise AuditFailure("Expected one literal source owner for target " + target)
    sources = [a or b for a, b in re.findall(r'"([^"\n]*)"|([^\s;]+)', blocks[0])]
    sources = [value for value in sources if value not in ("STATIC", "SHARED", "EXCLUDE_FROM_ALL", "WIN32", "MACOSX_BUNDLE")]
    if not sources or any(re.search(r"[$()\\]", value) or not value.endswith((".cpp", ".cc", ".cxx", ".c")) for value in sources):
        raise AuditFailure("Dynamic/non-source target entries cannot be independently audited: " + target)
    return sources


def literal_target_definitions(path: Path, target: str) -> list[str]:
    text = re.sub(r"#[^\n]*", "", path.read_text(encoding="utf-8-sig"))
    blocks = re.findall(r"\btarget_compile_definitions\s*\(\s*" + re.escape(target) + r"\s+([^)]*)\)", text, re.I)
    if len(blocks) != 1:
        raise AuditFailure("Expected one explicit compile-definition owner for " + target)
    values = [a or b for a, b in re.findall(r'"([^"\n]*)"|([^\s;]+)', blocks[0])]
    values = [value for value in values if value not in ("PRIVATE", "PUBLIC", "INTERFACE")]
    if any(not re.fullmatch(r"[A-Za-z_]\w*(?:=[A-Za-z_0-9]+)?", value) for value in values):
        raise AuditFailure("Dynamic compile definitions cannot be independently audited: " + target)
    return values


def audit(root: Path, manifest: Path, extra_includes: list[str], extra_sources: list[str] | None = None,
          public_consumer: bool = False) -> dict:
    manifest_bytes = manifest.read_bytes()
    front = root / "Implementations/Frontend" if public_consumer else root
    sdk, peer = root / "sdk/ecosystem", root / "Implementations/Bridges/reference-c"
    owners = (front, sdk, peer) if public_consumer else (root,)
    manifests = {manifest: manifest_bytes}
    sources = production_sources(manifest)
    dependency_targets = []
    frontend_cmake = front / "CMakeLists.txt"
    if frontend_cmake.is_file() and re.search(r"\badd_library\s*\(\s*frog-frontend-zstd\b", frontend_cmake.read_text(encoding="utf-8-sig")):
        # This is the existing decoder target, not an independent approximation
        # of its transitive inputs. CMake itself remains outside this auditor.
        zstd_sources = literal_target_sources(frontend_cmake, "add_library", "frog-frontend-zstd")
        zstd_definitions = literal_target_definitions(frontend_cmake, "frog-frontend-zstd")
        dependency_targets.append(dict(target="frog-frontend-zstd", sources=zstd_sources, definitions=zstd_definitions))
        manifests[frontend_cmake] = frontend_cmake.read_bytes()
        sources += zstd_sources
    if public_consumer:
        if extra_includes or extra_sources:
            raise AuditFailure("Public-consumer uses the exact three source owners; additional arbitrary inputs are not admitted.")
        sources = ["Implementations/Frontend/" + value for value in sources]
        sources += ["Implementations/Frontend/src/" + name for name in ("FrontendApi.cpp", "FrontendOperation.cpp", "FrontendCli.cpp")]
        for directory, command, target in ((sdk, "add_library", "frog-bridge-protocol"),
                                           (peer, "add_executable", "frog-reference-c-bridge")):
            cmake = directory / "CMakeLists.txt"
            manifests[cmake] = cmake.read_bytes()
            prefix = directory.relative_to(root).as_posix() + "/"
            sources += [prefix + value for value in literal_target_sources(cmake, command, target)]
    else:
        sources += extra_sources or []
    if not sources or len(sources) != len(set(sources)):
        raise AuditFailure("The production source list is empty or contains duplicate paths.")
    include_dirs = [front / item for item in (
        "include", "include/frog_native_engine", "src", "snapshot/api", "snapshot/sdk/include", "snapshot/generated")]
    if dependency_targets:
        include_dirs.append(front / "third_party/zstd/lib")
    if public_consumer:
        include_dirs += [sdk / "include", peer / "src"]
    include_dirs += [root / item for item in extra_includes]
    issues, files, edges, compiler_headers, inactive_includes = [], [], [], [], []
    visited: set[Path] = set()

    def relative(path: Path) -> str:
        try:
            return path.relative_to(root).as_posix()
        except ValueError:
            return "<outside-owner>/" + path.name

    def issue(code: str, path: Path, line: int = 0, evidence: str = "") -> None:
        issues.append(dict(code=code, file=relative(path), line=line, evidence=evidence))

    def admitted(path: Path) -> bool:
        return any(path.is_relative_to(owner) for owner in owners)

    def model_include(path: Path | None) -> bool:
        return path is not None and path.is_relative_to(front) and path.relative_to(front).as_posix() in PUBLIC_MODEL_HEADERS

    def model_symbol(path: Path, code: str, token: str) -> bool:
        if not path.is_relative_to(front):
            return False
        if code == "diagram-node-value-symbol":
            return True  # DiagramNode is now the shared persistent DTO.
        if code != "editor-model-symbol":
            return False
        return token in PUBLIC_MODEL_SYMBOLS or path.relative_to(front).as_posix() in PUBLIC_DATA_ACCESSORS.get(token, ())

    def visit(path: Path) -> None:
        path = path.resolve()
        if not admitted(path):
            issue("outside-public-owners" if public_consumer else "outside-frontend-owner", path)
            return
        if path in visited:
            return
        visited.add(path)
        if not path.is_file():
            issue("missing-production-input", path)
            return
        raw = path.read_bytes()
        try:
            text = raw.decode("utf-8-sig")
        except UnicodeDecodeError:
            issue("non-utf8-production-input", path)
            return
        files.append(dict(path=relative(path), sha256=hashlib.sha256(raw).hexdigest(), size=len(raw)))
        includes = masked_cpp(text, keep_literals=True)
        for match in re.finditer(r"^[ \t]*#[ \t]*include[ \t]+([^\r\n]+)", includes, re.M):
            expression = match.group(1).strip()
            line = includes.count("\n", 0, match.start()) + 1
            literal = re.fullmatch(r'["<]([^">]+)[">]\s*', expression)
            if not literal:
                issue("dynamic-include", path, line, expression)
                continue
            name = literal.group(1).replace("\\", "/")
            peer_file = public_consumer and path.is_relative_to(peer)
            if peer_file and expression.startswith("<") and name in PEER_WINDOWS_HEADERS:
                continue
            if expression.startswith("<") and name in STANDARD_HEADERS:
                continue
            front_relative = path.relative_to(front).as_posix() if path.is_relative_to(front) else ""
            if dependency_targets and expression.startswith("<") and name in ZSTD_COMPILER_HEADERS.get(front_relative, ()):
                compiler_headers.append(dict(source=relative(path), line=line, include=name,
                                             provider="compiler intrinsic headers", scope="Zstandard conditional branches"))
                continue
            if (dependency_targets and front_relative in (
                    "third_party/zstd/lib/decompress/zstd_decompress.c",
                    "third_party/zstd/lib/decompress/zstd_ddict.c") and name == "../legacy/zstd_legacy.h"
                    and "ZSTD_LEGACY_SUPPORT=0" in dependency_targets[0]["definitions"]
                    and re.search(r"#\s*if\s+defined\(ZSTD_LEGACY_SUPPORT\)\s*&&\s*\(ZSTD_LEGACY_SUPPORT\s*>=\s*1\)\s*$",
                                  includes[:match.start()].rstrip())):
                inactive_includes.append(dict(source=relative(path), line=line, include=name,
                                              reason="Exact decoder profile explicitly defines ZSTD_LEGACY_SUPPORT=0"))
                continue
            candidates = ([path.parent / name] if expression.startswith('"') else [])
            candidates += [directory / name for directory in include_dirs]
            resolved = next((candidate.resolve() for candidate in candidates if candidate.is_file()), None)
            forbidden = [code for code, pattern in FORBIDDEN_INCLUDES if pattern.search(name)
                         and not (code == "editor-model-include" and model_include(resolved))]
            for code in forbidden:
                issue(code, path, line, name)
            if forbidden:
                continue
            if resolved is None and peer_file and name == "Profile.hpp" and path == peer / "src/main.cpp":
                # Only this exact configure_file owner, not a generated-file fallback.
                cmake_text = manifests[peer / "CMakeLists.txt"].decode("utf-8-sig")
                if re.search(r'configure_file\s*\(\s*src/Profile\.hpp\.in\s+"\$\{CMAKE_CURRENT_BINARY_DIR\}/generated/Profile\.hpp"\s+@ONLY\s*\)', cmake_text):
                    resolved = peer / "src/Profile.hpp.in"
            if resolved is None:
                issue("unresolved-include", path, line, name)
                continue
            if public_consumer and path.is_relative_to(sdk / "include") and not resolved.is_relative_to(sdk / "include"):
                issue("public-sdk-header-private-include", path, line, name)
            if peer_file and not (resolved.is_relative_to(peer) or resolved.is_relative_to(sdk / "include")):
                issue("public-peer-private-include", path, line, name)
            edges.append(dict(source=relative(path), line=line, include=name, destination=relative(resolved)))
            visit(resolved)
        symbols = re.sub(r"^[ \t]*#[ \t]*include[^\r\n]*", "", masked_cpp(text), flags=re.M)
        # Shared DTOs have one declaration owner. A lookalike definition in a
        # new file does not acquire the exemption merely by reusing its name.
        type_names = "|".join(re.escape(name) for name in PUBLIC_MODEL_TYPES)
        definitions = re.compile(r"\b(?:struct|class|enum(?:\s+class)?)\s+(" + type_names +
                                 r")\s*(?:final\s*)?(?::[^;{}]+)?\{")
        for match in definitions.finditer(symbols):
            if path != front / PUBLIC_MODEL_TYPES[match.group(1)]:
                issue("public-model-declaration-owner", path, symbols.count("\n", 0, match.start()) + 1, match.group(1))
        for code, pattern in FORBIDDEN_SYMBOLS:
            if public_consumer and path.is_relative_to(peer) and code == "win32-symbol":
                continue  # The finite Windows transport/compiler peer owns WinAPI.
            for match in pattern.finditer(symbols):
                if model_symbol(path, code, match.group()):
                    continue
                issue(code, path, symbols.count("\n", 0, match.start()) + 1, match.group())
        if raw != path.read_bytes():
            issue("input-changed-during-audit", path)

    for source in sources:
        candidate = root / source
        if candidate.suffix.lower() not in (".cpp", ".cc", ".cxx", ".c"):
            issue("non-source-production-entry", candidate, evidence=source)
        visit(candidate)
    for entry in files:
        current = root / entry["path"]
        if not current.is_file() or hashlib.sha256(current.read_bytes()).hexdigest() != entry["sha256"]:
            issue("input-changed-during-audit", current)
    for input_path, before in manifests.items():
        if input_path.read_bytes() != before:
            issue("manifest-changed-during-audit", input_path)
    return dict(
        schemaVersion=1, kind="frog.public-consumer.literal-closure-audit" if public_consumer else "frog.frontend.literal-production-closure-audit",
        owners=[relative(owner) for owner in owners],
        sourceManifests=[dict(path=relative(path), sha256=hashlib.sha256(data).hexdigest()) for path, data in manifests.items()],
        productionManifest=relative(manifest.resolve()),
        productionManifestSha256=hashlib.sha256(manifest_bytes).hexdigest(),
        dependencySourceTargets=dependency_targets,
        compilerHeaderRequirements=compiler_headers,
        inactiveConditionalIncludes=inactive_includes,
        sources=sources, files=sorted(files, key=lambda item: item["path"]),
        includeEdges=sorted(edges, key=lambda item: (item["source"], item["line"], item["include"])),
        issues=sorted(issues, key=lambda item: (item["file"], item["line"], item["code"])),
        outcome="pass" if not issues else "fail",
        compilation="NOT_RUN", linking="NOT_RUN", packaging="NOT_RUN",
        limitations=["Literal includes in every conditional branch are checked, except recorded exact dependency branches disabled by explicit target definitions; this is not a preprocessor or link-symbol audit.",
                     "Exact extracted DTO/codec owners are allowed; their contents remain subject to the same UI, runtime and platform checks.",
                     "Public SDK runtime value/schema types and historical frog::runtime semantic namespaces are allowed.",
                     "A passing closure audit grants no redistribution rights and does not prove source/SDK snapshot parity."],
    )


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", required=True, type=Path)
    parser.add_argument("--public-consumer", action="store_true",
                        help="Audit Frontend+public SDK+reference C peer owners; --root is FROG, WinAPI belongs only to the peer.")
    parser.add_argument("--sources", type=Path, help="Literal production list, default production-sources.cmake under Frontend.")
    parser.add_argument("--include-dir", action="append", default=[], help="Additional include directory, confined to Frontend.")
    parser.add_argument("--source", action="append", default=[], help="Additional production TU, e.g. the C ABI/CLI adapters.")
    parser.add_argument("--output", type=Path, help="Optional JSON evidence file; sources are never copied or changed.")
    args = parser.parse_args()
    try:
        root = public_root(args.root) if args.public_consumer else frontend_root(args.root)
        if args.public_consumer and args.sources:
            raise AuditFailure("Public-consumer requires the real Frontend production-sources.cmake owner.")
        manifest = args.sources or root / ("Implementations/Frontend/production-sources.cmake" if args.public_consumer else "production-sources.cmake")
        if not manifest.is_absolute():
            manifest = root / manifest
        for directory in args.include_dir:
            if not (root / directory).resolve().is_relative_to(root):
                raise AuditFailure("Additional include directories must remain inside the Frontend owner.")
        report = audit(root, manifest, args.include_dir, args.source, args.public_consumer)
        if args.output:
            args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
        print(json.dumps(dict(outcome=report["outcome"], sources=len(report["sources"]), files=len(report["files"]),
                              includeEdges=len(report["includeEdges"]), issues=report["issues"], compilation="NOT_RUN"), indent=2))
        return 0 if report["outcome"] == "pass" else 1
    except (AuditFailure, OSError, ValueError) as exc:
        print(json.dumps(dict(outcome="error", code="audit-configuration-error", message=str(exc))), file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
