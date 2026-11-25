# SPDX-License-Identifier: Apache-2.0
"""Import one direct C-to-DLL scalar forwarding function into a Studio draft."""

import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import tempfile


TARGET = "x86_64-pc-windows-msvc"
CTYPE = {"int": "i32", "unsigned int": "u32", "long long": "i64",
         "unsigned long long": "u64", "double": "f64"}
MAX_LIBRARY_BYTES = 128 * 1024 * 1024


class Refusal(ValueError):
    def __init__(self, code, message, node=None):
        super().__init__(message)
        self.code = "c_import.native_call." + code
        self.node = node or {}


def children(node):
    return node.get("inner", [])


def reference(node, cast):
    while node.get("kind") == "ParenExpr" or (
            node.get("kind") == "ImplicitCastExpr" and node.get("castKind") == cast):
        node, = children(node)
    return node.get("referencedDecl", {}) if node.get("kind") == "DeclRefExpr" else {}


def signature(function, suffix):
    params = [p for p in children(function) if p["kind"] == "ParmVarDecl"]
    types = [p["type"]["qualType"] for p in params]
    prototype = function["type"]["qualType"]
    returned = prototype.partition(" (")[0]
    if (not 1 <= len(params) <= 4 or returned not in CTYPE
            or any(t not in CTYPE for t in types)
            or prototype != f"{returned} ({', '.join(types)}){suffix}"):
        raise Refusal("abi_unsupported", "Expected a fixed, exact scalar Windows x64 prototype.", function)
    return CTYPE[returned], [CTYPE[t] for t in types], params


def read_call(ast, entry, expected_symbol):
    functions = [n for n in children(ast) if n.get("kind") == "FunctionDecl"]
    definitions = [n for n in functions if n.get("name") == entry
                   and children(n) and children(n)[-1]["kind"] == "CompoundStmt"]
    if len(definitions) != 1:
        raise Refusal("entry_unsupported", f"Expected one definition of {entry}.")
    wrapper = definitions[0]
    ret, types, params = signature(wrapper, "")
    body = children(wrapper)[-1]
    statements = children(body)
    if len(statements) != 1 or statements[0]["kind"] != "ReturnStmt":
        raise Refusal("body_unsupported", "Expected a single return of a direct DLL call.", body)
    expression, = children(statements[0])
    while expression["kind"] == "ParenExpr":
        expression, = children(expression)
    if expression["kind"] != "CallExpr":
        raise Refusal("body_unsupported", "The returned expression must be the direct DLL call.", expression)
    callee, *arguments = children(expression)
    decl_id = reference(callee, "FunctionToPointerDecay").get("id")
    declaration = next((n for n in functions if n["id"] == decl_id), None)
    if not declaration or not any(n["kind"] == "DLLImportAttr" for n in children(declaration)):
        raise Refusal("symbol_unsupported", "The call must target a declared dllimport function.", callee)
    dll_ret, dll_types, _ = signature(declaration, " __attribute__((cdecl))")
    symbol = declaration.get("mangledName")
    if symbol != expected_symbol or symbol != declaration.get("name"):
        raise Refusal("symbol_mismatch", f"Expected C export {expected_symbol!r}, found {symbol!r}.", callee)
    actual = [reference(arg, "LValueToRValue").get("id") for arg in arguments]
    if actual != [p["id"] for p in params] or (ret, types) != (dll_ret, dll_types):
        raise Refusal("arguments_unsupported", "Forward each argument once, in order, without conversion.", expression)
    return expression, declaration, list(zip(params, types)), ret


def invoke(command):
    env = {k: v for k, v in os.environ.items() if k.upper() not in {"CL", "_CL_", "INCLUDE"}}
    return subprocess.run(command, env=env, capture_output=True, timeout=30)


def inspect_library(inspector, library, expected_symbol):
    result = invoke([str(inspector), "--file-headers", "--coff-exports", "--coff-imports", str(library)])
    report = result.stdout.decode("utf-8", errors="replace")
    if result.returncode:
        raise Refusal("library_invalid", result.stderr.decode("utf-8", errors="replace").strip())
    if ("Machine: IMAGE_FILE_MACHINE_AMD64 (0x8664)" not in report
            or "Magic: 0x20B" not in report or "IMAGE_FILE_DLL (0x2000)" not in report):
        raise Refusal("target_incompatible", "Expected a Windows x64 PE32+ DLL.")
    exports = re.findall(r"^Export \{\s+Ordinal: \d+\s+Name: (\S+)\s+RVA: (0x[0-9A-Fa-f]+)\s*\}", report, re.M)
    table = re.search(r"ExportTableRVA: (0x[0-9A-Fa-f]+)\s+ExportTableSize: (0x[0-9A-Fa-f]+)", report)
    if not table:
        raise Refusal("library_invalid", "Cannot read the PE export directory.")
    start, size = (int(n, 16) for n in table.groups())
    if not any(name == expected_symbol and int(address, 16)
               and not start <= int(address, 16) < start + size for name, address in exports):
        raise Refusal("symbol_missing", f"Direct export {expected_symbol!r} is absent.")
    if not re.search(r"^\s*ImportTableRVA: 0x0\s*$", report, re.M) or not re.search(
            r"^\s*DelayImportDescriptorRVA: 0x0\s*$", report, re.M):
        raise Refusal("dependency_unsupported", "This import profile requires a DLL with no PE imports.")
    return report


def native_type(typ):
    return {"kind": "numeric", "numeric_type": typ, "element_type": typ, "rank": 1,
            "adapted_type": {"canonical": "unknown", "children": [], "choice_kind": 0,
                             "choices": [], "field_names": []},
            "ansi_code_page": 0, "array_format": "data_pointer",
            "string_encoding": "utf8", "string_format": "c_string",
            "struct_layout": {"alignment": 0, "byte_size": 0, "c_type_name": "", "fields": [], "packing": 0}}


def emit(entry, symbol, library, digest, params, returned):
    def bounds(x, y, width, height):
        return dict(x=x, y=y, width=width, height=height)

    nodes, edges, widgets = [], [], []

    def terminal(ident, typ, label, role, x, y, diagram_x):
        widgets.append({"id": ident, "role": role, "label": label, "valueType": typ,
                        "classId": f"frog.widgets.numeric_{role}", "bounds": bounds(x, y, 130, 38),
                        "props": {"label.visible": True, "data_type.representation": typ}, "state": {"value": 0}})
        nodes.append({"id": ident + "-terminal", "kind": "widget_value", "widget": ident,
                      "type": typ, "label_visible": True, "layout": bounds(diagram_x, y, 60, 24)})
        return nodes[-1]["id"]

    def edge(ident, source, out, dest, port):
        edges.append({"id": ident, "from": {"node": source, "port": out}, "to": {"node": dest, "port": port}})

    capacity = {"constant": 0, "parameter_id": "", "source": "unspecified", "unit": "elements"}
    arguments = []
    call = "dll-call"
    for index, (param, typ) in enumerate(params, 1):
        ident = f"p{index}"
        name = param["name"]
        arguments.append({"id": ident, "display_name": name, "type": native_type(typ), "pass": "value",
                          "access": "read_only", "input_requirement": "required", "initialization": "zero",
                          "nullable": False, "default_canonical_value": None, "minimum_capacity": capacity,
                          "dimensions": [], "valid_length_parameter_id": None})
        source = terminal(f"arg-{ident}", typ, name, "control", 45, 50 + 68 * (index - 1), 90)
        edge(f"wire-{ident}", source, "value", call, f"param.{ident}.in")
    result = terminal("result", returned, "result", "indicator", 305, 50, 500)
    edge("wire-result", call, "return_value", result, "value")
    error_code = terminal("error-code", "i32", "NativeCall error code", "indicator", 305, 130, 590)
    error_reader = "read-native-error-code"
    nodes.append({"id": error_reader, "kind": "primitive", "type": "frog.studio.error.read_code",
                  "palette_catalog_id": "error.read_code", "layout": bounds(450, 130, 44, 44)})
    edge("wire-error", call, "error_out", error_reader, "error_in")
    edge("wire-error-code", error_reader, "code", error_code, "value")
    catalog = "connectivity.libraries.call_library_function"
    nodes.append({"id": call, "kind": "primitive", "type": "frog.studio." + catalog,
                  "palette_catalog_id": catalog, "label": symbol, "label_visible": True,
                  "layout": bounds(275, 50, 105, 84),
                  "studio_native_call": {
                      "schema_version": 1, "symbol": symbol, "calling_convention": "cdecl",
                      "thread": "any", "error_checking": "default", "variadic": False,
                      "library": {"mode": "document_relative", "value": library,
                                  "expected_sha256": digest,
                                  "specify_path_on_diagram": False, "dynamic_relative_base": "document_relative"},
                      "return": {"display_name": "result", "type": native_type(returned), "ownership": "borrowed",
                                 "maximum_read": capacity, "lifetime": ""},
                      "arguments": arguments, "callbacks": {"profile_version": 1, "reserve": "", "unreserve": "", "abort": ""},
                      "next_identity": len(arguments) + 1,
                      "presentation": {"show_parameter_names": True, "header_title_visible": True,
                                       "compact": True, "requested_width": 0,
                                       "visual_parameter_order": [p["id"] for p in arguments]}}})
    return {"format": "frog.document.draft", "language": "FROG", "draft_revision": 7, "title": entry,
            "metadata": {"name": entry}, "interface": {"inputs": [], "outputs": []},
            "frontPanel": {"canvas": {"width": 560, "height": 390, "coordinate_space": "panel_pixels"},
                           "grid": {"pitch": 12}, "widgets": widgets},
            "diagram": {"nodes": nodes, "edges": edges}}


def write_json(path, value):
    path.write_text(json.dumps(value, indent=2, ensure_ascii=False, allow_nan=False) + "\n", encoding="utf-8")


def import_call(args):
    if not args.library.is_file():
        raise Refusal("library_missing", f"DLL not found: {args.library}")
    if args.library.suffix.lower() != ".dll" or args.library.stat().st_size > MAX_LIBRARY_BYTES:
        raise Refusal("library_invalid", "Expected a bounded .dll input.")
    data = args.source.read_bytes()
    if args.output.exists():
        raise Refusal("destination_exists", f"Destination exists: {args.output}")
    args.output.parent.mkdir(parents=True, exist_ok=True)
    stage = Path(tempfile.mkdtemp(prefix=".native-call-import-", dir=args.output.parent))
    try:
        library = stage / args.library.name
        with args.library.open("rb") as stream:
            binary = stream.read(MAX_LIBRARY_BYTES + 1)
        if len(binary) > MAX_LIBRARY_BYTES or hashlib.sha256(binary).hexdigest() != args.sha256:
            raise Refusal("dependency_identity", "DLL bytes differ from the declared SHA-256.")
        library.write_bytes(binary)
        report = inspect_library(args.readobj.resolve(strict=True), library, args.symbol)
        command = [str(args.clang.resolve(strict=True)), "/nologo", "/TC", "/std:c17", "/W4", "/WX",
                   "/clang:--target=" + TARGET, "/clang:-fsyntax-only", "/clang:-Xclang", "/clang:-ast-dump=json",
                   str(args.source.resolve(strict=True))]
        result = invoke(command)
        if result.returncode:
            raise Refusal("clang_refused", result.stderr.decode("utf-8", errors="replace").strip())
        if args.source.read_bytes() != data:
            raise Refusal("source_changed", "The C source changed during import.")
        call, declaration, params, returned = read_call(json.loads(result.stdout), args.entry, args.symbol)
        document = stage / f"{args.entry}.frog"
        draft = emit(args.entry, args.symbol, library.name, args.sha256, params, returned)
        draft["metadata"]["description"] = f"Source (C): {args.source.resolve()}\n{args.entry} → {library.name} / {args.symbol}"
        write_json(document, draft)
        validation = invoke([str(args.frontend.resolve(strict=True)), "validate", "--source", str(document),
                             "--source-uri", "import:/c/nativecall.frog", "--source-format", "frog.document.draft",
                             "--source-version", "7", "--fir-version", "0.1",
                             "--document-id", "c:draft:nativecall", "--fir-document-id", "c:fir:nativecall",
                             "--diagnostics-output", str(stage / "frontend-diagnostics.json")])
        if validation.returncode:
            detail = validation.stdout or validation.stderr
            raise Refusal("frontend_refused", detail.decode("utf-8", errors="replace"))
        write_json(stage / "c-import.json", {
            "source": args.source.name, "source_sha256": hashlib.sha256(data).hexdigest(),
            "target": TARGET, "command": command,
            "dependency": {"file": library.name, "sha256": args.sha256, "size": len(binary),
                           "symbol": args.symbol, "calling_convention": "cdecl",
                           "parameters": [typ for _, typ in params], "return": returned},
            "source_map": {"dll-call": call["range"]["begin"]["offset"],
                           "dll-declaration": declaration["loc"]["offset"],
                           **{f"arg-p{i}-terminal": p["loc"]["offset"] for i, (p, _) in enumerate(params, 1)}}})
        (stage / "dll-inspection.txt").write_text(report, encoding="utf-8")
        stage.rename(args.output)
    finally:
        if stage.exists():
            shutil.rmtree(stage)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for option in ("source", "library", "clang", "readobj", "frontend", "output"):
        parser.add_argument("--" + option, type=Path, required=True)
    for option in ("entry", "symbol"):
        parser.add_argument("--" + option, required=True)
    identity = parser.add_mutually_exclusive_group(required=True)
    identity.add_argument("--sha256")
    identity.add_argument("--capture-library-identity", action="store_true",
                          help="Pin the selected file's SHA-256 in the generated NativeCall recipe.")
    args = parser.parse_args()
    if not all(re.fullmatch(r"[A-Za-z_][A-Za-z_0-9]*", name) for name in (args.entry, args.symbol)):
        parser.error("entry and symbol must be C identifiers")
    if args.sha256 and not re.fullmatch(r"[a-fA-F0-9]{64}", args.sha256):
        parser.error("sha256 must be a declared 64-digit hexadecimal digest")
    try:
        if args.capture_library_identity:
            if not args.library.is_file():
                raise Refusal("library_missing", f"DLL not found: {args.library}")
            if args.library.stat().st_size > MAX_LIBRARY_BYTES:
                raise Refusal("library_invalid", "Expected a bounded .dll input.")
            with args.library.open("rb") as stream:
                args.sha256 = hashlib.sha256(stream.read(MAX_LIBRARY_BYTES + 1)).hexdigest()
        else:
            args.sha256 = args.sha256.lower()
        import_call(args)
    except Refusal as error:
        loc = error.node.get("range", {}).get("begin", error.node.get("loc", {}))
        print(json.dumps({"code": error.code, "message": str(error), "source": args.source.name,
                          "byte_offset": loc.get("offset")}, ensure_ascii=False))
        return 2
    except (OSError, subprocess.TimeoutExpired, ValueError, KeyError) as error:
        print(json.dumps({"code": "c_import.native_call.input_error", "message": str(error)}, ensure_ascii=False))
        return 2
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
