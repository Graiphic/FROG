# SPDX-License-Identifier: Apache-2.0
"""Import the QMH expression check from a pinned Clang JSON AST.

The admitted statements cover checked() and its complete fail() dependency.
Other entry points are diagnosed with the responsible source location.
"""
import argparse
import hashlib
import json
import os
import shutil
from pathlib import Path
import subprocess
import tempfile


def file_hash(path):
    with path.open("rb") as stream:
        return hashlib.file_digest(stream, "sha256").hexdigest()


def children(node):
    return node.get("inner", [])


def strip(node):
    while node["kind"] in {"ImplicitCastExpr", "ParenExpr", "CompoundLiteralExpr"}:
        node, = children(node)
    return node


def shape(node):
    node = strip(node)
    kind = node["kind"]
    ref = node.get("referencedDecl", {})
    if kind == "DeclRefExpr":
        return ref["name"]
    if kind == "IntegerLiteral":
        return int(node["value"])
    parts = children(node)
    if kind == "InitListExpr":
        return tuple(shape(c) for c in parts)
    tag = node.get("opcode", node.get("name", kind))
    if kind == "UnaryOperator" and node.get("isPostfix"):
        tag = "post" + tag
    return (tag, *(shape(c) for c in parts))


def definition(ast, name):
    return next(n for n in children(ast) if n.get("kind") == "FunctionDecl"
                and n.get("name") == name and children(n)
                and children(n)[-1]["kind"] == "CompoundStmt")


def location(node, data, source_name="expression.c"):
    begin = node.get("range", {}).get("begin", node.get("loc", {}))
    begin = begin.get("expansionLoc", begin)
    offset = begin.get("offset", 0)
    return {"file": source_name, "line": data.count(b"\n", 0, offset) + 1,
            "column": offset - data.rfind(b"\n", 0, offset), "byte_offset": offset}


def record_fields(ast, name):
    record = next(n for n in children(ast) if n.get("kind") == "RecordDecl"
                  and n.get("name") == name and n.get("completeDefinition"))
    return [(field["name"], field["type"]["qualType"]) for field in children(record)
            if field["kind"] == "FieldDecl"]


def unavailable_entry(ast, data, check, source_name):
    """Explain the actual reachable effects, without admitting an opaque call.

    This is diagnostic discovery only. read_check remains the owner of the
    admitted checked/fail profile; this function never emits a partial result.
    """
    definitions = {node["name"]: node for node in children(ast)
                   if node.get("kind") == "FunctionDecl" and children(node)
                   and children(node)[-1]["kind"] == "CompoundStmt"}
    pending, seen, members, effects = [check["name"]], set(), [], []

    def walk(node):
        pending_nodes = [node]
        while pending_nodes:
            current = pending_nodes.pop()
            yield current
            pending_nodes.extend(reversed(children(current)))

    runtime_services = {"_create_locale", "_free_locale", "_strtod_l", "strlen", "isspace", "isdigit"}
    while pending:
        name = pending.pop(0)
        if name in seen:
            continue
        seen.add(name)
        function = definitions[name]
        members.append({"name": name, **location(function, data, source_name)})
        for node in walk(children(function)[-1]):
            if node.get("kind") != "CallExpr" or not children(node):
                continue
            callee = strip(children(node)[0])
            reference = callee.get("referencedDecl", {})
            called = reference.get("name")
            if reference.get("kind") != "FunctionDecl":
                continue
            if called in definitions and called not in seen:
                pending.append(called)
            if called in runtime_services or called == name:
                effects.append({"function": name, "callee": called,
                    "reason": "recursive_call" if called == name else "c_runtime_effect",
                    **location(node, data, source_name)})
    return {"code": "c_import.entry_unavailable", **location(check, data, source_name),
            "entry": check["name"], "reachable_members": members, "unavailable_effects": effects,
            "message": "The checked/fail profile does not translate this complete entry. "
                       "C byte cursors, recursive control flow and CRT calls need admitted typed realizations; "
                       "ordinary numeric String conversion is not a replacement for _strtod_l. "
                       "No executable partial document was emitted."}


def read_check(ast, data, entry, source_name="expression.c"):
    try:
        check = definition(ast, entry)
    except StopIteration as error:
        raise ValueError({"code": "c_import.entry_missing", "file": source_name,
                          "entry": entry, "message": "The selected C function has no definition in this translation unit."}) from error
    if entry != "checked":
        raise ValueError(unavailable_entry(ast, data, check, source_name))
    fail = definition(ast, "fail")
    parser_fields = dict(record_fields(ast, "Parser"))
    if (parser_fields.get("nodes") != "unsigned int" or parser_fields.get("error") != "int"
            or record_fields(ast, "Value") != [("number", "double"), ("depth", "unsigned int")]):
        raise ValueError({"code": "c_import.record_unavailable", **location(check, data, source_name)})
    # AST casts and referenced declarations, rather than comments, determine the
    # admitted C profile. This intentionally rejects new shapes before emitting.
    signatures = {"checked": "Value (Parser *, double, unsigned int)",
                  "fail": "Value (Parser *, int)"}
    for func in (check, fail):
        if func["type"]["qualType"] != signatures[func["name"]]:
            raise ValueError({"code": "c_import.signature_unavailable", **location(func, data, source_name)})
    fail_body = shape(children(fail)[-1])
    if fail_body != ("CompoundStmt", ("IfStmt", ("!", ("error", "parser")),
                     ("=", ("error", "parser"), "code")), ("ReturnStmt", (0, 0))):
        raise ValueError({"code": "c_import.effect_unavailable", **location(fail, data, source_name)})
    statements = children(children(check)[-1])
    if len(statements) != 3:
        raise ValueError({"code": "c_import.statement_unavailable", **location(check, data, source_name)})
    limit, finite, success = map(shape, statements)
    try:
        l_if, (disjunction, (count_cmp, (increment, (count_field, parser)), count),
               (depth_cmp, depth_name, depth)), (ret1, (call1, name1, p1, limit_error)) = limit
        f_if, (negation, (classification, (builtin, classifier, *classification_values), zero)), (
            ret2, (call2, name2, p2, finite_error)) = finite
    except (TypeError, ValueError) as error:
        raise ValueError({"code": "c_import.statement_unavailable", **location(check, data, source_name)}) from error
    if ((l_if, disjunction, count_cmp, increment, count_field, parser, depth_cmp, depth_name,
         ret1, call1, name1, p1) != ("IfStmt", "||", ">", "++", "nodes", "parser",
                                      ">", "depth", "ReturnStmt", "CallExpr", "fail", "parser")
            or (f_if, negation, classification, builtin, classifier, classification_values, zero,
                ret2, call2, name2, p2) != (
                "IfStmt", "!", "<=", "CallExpr", "__builtin_fpclassify",
                [2, 1, ("-", 1), ("-", 2), 0, "number"], 0, "ReturnStmt", "CallExpr", "fail", "parser")
            or success != ("ReturnStmt", ("number", "depth"))
            or not all(isinstance(x, int) for x in (count, depth, limit_error, finite_error))
            or not all(0 <= x <= 0xFFFFFFFF for x in (count, depth))
            or not all(-(2**31) <= x < 2**31 for x in (limit_error, finite_error))):
        raise ValueError({"code": "c_import.statement_unavailable", **location(check, data, source_name)})
    return (count, depth, limit_error, finite_error), {
        key: location(node, data, source_name) for key, node in zip(
            ("checked", "fail", "limit", "finite", "success"), (check, fail, *statements))}


class Graph:
    def __init__(self, locations):
        self.nodes, self.edges, self.widgets, self.mapping = [], [], [], {}
        self.locations = locations

    def node(self, ident, kind, origin, **options):
        self.nodes.append({"id": ident, "kind": kind, **options})
        self.mapping[ident] = self.locations[origin]
        return ident

    def edge(self, source, dest, port, source_port="result"):
        self.edges.append({"id": f"{dest}.{port}", "from": {"node": source, "port": source_port},
                           "to": {"node": dest, "port": port}})

    def constant(self, ident, typ, value, origin):
        self.node(ident, "constant", origin, type=typ, value=value,
                  constant_profile="frog.studio.scalar-constant@1")
        return (ident, "value")

    def op(self, ident, typ, arguments, origin):
        self.node(ident, "primitive", origin, type=typ)
        for port, (node, output) in arguments.items():
            self.edge(node, ident, port, output)
        return (ident, "result")

    def binary(self, ident, typ, a, b, origin):
        return self.op(ident, typ, {"a": a, "b": b}, origin)

    def select(self, ident, condition, yes, no, origin):
        return self.op(ident, "frog.studio.scalar.select.exact.v1",
                       {"select": condition, "true": yes, "false": no}, origin)

    def widget(self, ident, typ, default=None, source=None):
        role = "control" if source is None else "indicator"
        y = 25 + 56 * sum(w["role"] == role for w in self.widgets)
        widget = {"id": ident, "role": role, "class_ref": "frog.widgets.numeric_" + role,
                  "value_type": typ, "layout": {"x": 24 if source is None else 300,
                                             "y": y, "width": 210, "height": 30},
                  "parts": {"label": {"class": "frog.ui.standard.label_part",
                                      "props": {"text": ident.replace("-", " ")}}}}
        if source is None:
            widget["props"] = {"default_value": default}
        self.widgets.append(widget)
        node = self.node(ident + "-terminal", "widget_value", "checked", widget=ident)
        if source is not None:
            self.edge(source[0], node, "value", source[1])
        return (node, "value")


def emit(constants, locations, source, entry):
    max_count, max_depth, limit_error, finite_error = constants
    g = Graph(locations)
    count = g.widget("nodes-before", "u32", 1999)
    error = g.widget("error-before", "i32", 0)
    number = g.widget("number", "f64", -0.0)
    depth = g.widget("depth", "u32", 24)
    uzero = g.constant("zero-u32", "u32", 0, "fail")
    izero = g.constant("zero-i32", "i32", 0, "fail")
    dzero = g.constant("zero-f64", "f64", 0.0, "fail")
    one = g.constant("one-u32", "u32", 1, "limit")
    count = g.binary("increment-nodes", "frog.studio.numeric.add.wrap32.v1", count, one, "limit")
    max_count = g.constant("limit-count", "u32", max_count, "limit")
    max_depth = g.constant("limit-depth", "u32", max_depth, "limit")
    compare = "frog.studio.comparison.greater.exact32.v1"
    count_over = g.binary("over-count", compare, count, max_count, "limit")
    depth_over = g.binary("over-depth", compare, depth, max_depth, "limit")
    # The right operand of || is only a read and an unsigned comparison.
    # Evaluating it in the graph has no observable effect on the left path.
    over = g.binary("over-limit", "frog.core.or", count_over, depth_over, "limit")
    max_double = float.fromhex("0x1.fffffffffffffp+1023")
    lower = g.constant("min-finite", "f64", -max_double, "finite")
    upper = g.constant("max-finite", "f64", max_double, "finite")
    above = g.binary("above-min", "frog.studio.comparison.greater_or_equal.ieee64.v1",
                     number, lower, "finite")
    below = g.binary("below-max", "frog.studio.comparison.less_or_equal.ieee64.v1",
                     number, upper, "finite")
    # Ordered comparisons reject NaN as well as both infinities.
    finite = g.binary("number-finite", "frog.core.and", above, below, "finite")
    failure = g.select("needs-fail", over, g.constant("true", "bool", True, "limit"),
                       g.op("not-finite", "frog.core.not", {"in": finite}, "finite"), "checked")
    limit = g.constant("error-limit", "i32", limit_error, "limit")
    domain = g.constant("error-nonfinite", "i32", finite_error, "finite")
    code = g.select("failure-code", over, limit, domain, "checked")
    no_error = g.binary("no-prior-error", "frog.studio.comparison.equal.exact32.v1",
                        error, izero, "fail")
    error = g.select("retain-error", failure, g.select("first-error", no_error, code, error, "fail"),
                     error, "checked")
    g.widget("nodes-after", "u32", source=count)
    g.widget("error-after", "i32", source=error)
    g.widget("checked-number", "f64", source=g.select("return-number", failure, dzero, number, "checked"))
    g.widget("checked-depth", "u32", source=g.select("return-depth", failure, uzero, depth, "checked"))
    document = {"spec_version": "0.1", "metadata": {"name": "QMH expression checked",
                "summary": f"Source (C): {source}\nClang import of {entry}() and fail()."},
                "interface": {"inputs": [], "outputs": []},
                "diagram": {"nodes": g.nodes, "edges": g.edges},
                "front_panel": {"canvas": {"width": 550, "height": 270}, "widgets": g.widgets}}
    return document, g.mapping


def editable_draft(source):
    operations = {
        "frog.studio.numeric.add.wrap32.v1": "numeric.add",
        "frog.studio.comparison.greater.exact32.v1": "comparison.greater",
        "frog.studio.comparison.equal.exact32.v1": "comparison.equal",
        "frog.studio.comparison.greater_or_equal.ieee64.v1": "comparison.greater_or_equal",
        "frog.studio.comparison.less_or_equal.ieee64.v1": "comparison.less_or_equal",
        "frog.studio.scalar.select.exact.v1": "structures.select",
    }
    widgets = []
    for widget in source["front_panel"]["widgets"]:
        value = widget.get("props", {}).get("default_value", 0)
        widgets.append({"id": widget["id"], "classId": widget["class_ref"],
                        "role": widget["role"], "valueType": widget["value_type"],
                        "label": widget["parts"]["label"]["props"]["text"],
                        "bounds": widget["layout"],
                        "props": {"label.visible": True,
                                  "data_type.representation": widget["value_type"]},
                        "state": {"value": value}})
    by_id = {widget["id"]: widget for widget in widgets}
    nodes = []
    comparisons = set()
    for index, node in enumerate(source["diagram"]["nodes"]):
        node = dict(node)
        if node["kind"] == "widget_value":
            widget = by_id[node["widget"]]
            node["type"] = widget["valueType"]
            x = 60 if widget["role"] == "control" else 1150
            y = widget["bounds"]["y"] * 2
        else:
            x, y = 205 + index // 5 * 155, 60 + index % 5 * 110
        if node["kind"] == "constant":
            typ, value = node.pop("type"), node.pop("value")
            node.pop("constant_profile")
            node.update(kind="primitive", value_type=typ,
                        type=(f"frog.studio.boolean.{str(value).lower()}_constant" if typ == "bool"
                              else "frog.studio.numeric.numeric_constant"),
                        text=str(value).lower(), label=node["id"], label_visible=True)
        elif node["type"] in operations:
            catalog = operations[node["type"]]
            node.update(type="frog.core.add" if catalog == "numeric.add" else "frog.studio." + catalog,
                        palette_catalog_id=catalog)
            if catalog.startswith("comparison."):
                comparisons.add(node["id"])
        node["layout"] = {"x": x, "y": y, "width": 46, "height": 32}
        nodes.append(node)
    edges = []
    for edge in source["diagram"]["edges"]:
        target = edge["to"]
        if target["node"] in comparisons:
            edge = {**edge, "to": {**target, "port": {"a": "x", "b": "y"}[target["port"]]}}
        edges.append(edge)
    return {"format": "frog.document.draft", "draft_revision": 2, "language": "FROG",
            "title": source["metadata"]["name"], "metadata": source["metadata"],
            "interface": source["interface"],
            "frontPanel": {"canvas": {**source["front_panel"]["canvas"],
                                      "coordinate_space": "panel_pixels"},
                           "widgets": widgets},
            "diagram": {"nodes": nodes, "edges": edges}}


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--clang", type=Path, required=True)
    p.add_argument("--include", action="append", type=Path, default=[])
    p.add_argument("--source", type=Path, required=True)
    p.add_argument("--entry", default="checked")
    p.add_argument("--frontend", type=Path, required=True)
    p.add_argument("--output", type=Path, required=True)
    args = p.parse_args()
    source_header = args.source.with_suffix(".h")
    try:
        data = args.source.read_bytes()
        header_data = source_header.read_bytes()
    except OSError as error:
        print(json.dumps({"code": "c_import.header_or_source_missing", "source": str(source_header),
                          "message": "This specialized QMH profile requires the source and its sibling header. Choose Pure C function for ordinary covered functions.", "detail": str(error)}))
        return 2
    command = [str(args.clang.resolve(strict=True)), "/nologo", "/TC", "/std:c17",
               "/utf-8", "/W4", "/WX", "/O2", "/MT", "/DUNICODE", "/D_UNICODE",
               "/D_WIN32_WINNT=0x0A00", "/clang:-fsyntax-only",
               "/clang:-Xclang", "/clang:-ast-dump=json",
               *(f"/I{directory.resolve(strict=True)}" for directory in args.include),
               str(args.source.resolve(strict=True))]
    env = {k: v for k, v in os.environ.items() if k.upper() not in {"CL", "_CL_", "INCLUDE"}}
    try:
        ast_result = subprocess.run(command, env=env, capture_output=True, timeout=30)
    except subprocess.TimeoutExpired:
        print(json.dumps({"code": "c_import.clang_timeout"}))
        return 2
    if ast_result.returncode:
        print(json.dumps({"code": "c_import.clang_refused", "diagnostics":
                          ast_result.stderr.decode("utf-8", errors="replace")}))
        return 2
    if args.source.read_bytes() != data or source_header.read_bytes() != header_data:
        print(json.dumps({"code": "c_import.source_changed"}))
        return 2
    try:
        ast = json.loads(ast_result.stdout)
        constants, locations = read_check(ast, data, args.entry, args.source.name)
    except (StopIteration, KeyError, ValueError) as error:
        details = error.args[0] if error.args and isinstance(error.args[0], dict) else {
            "code": "c_import.profile_structure_unavailable", "entry": args.entry, "source": str(args.source),
            "message": "Clang parsed the file, but its AST does not match the specialized checked/fail QMH contract. Use a detected pure function or another profile.",
            "detail": str(error)}
        print(json.dumps(details))
        return 2
    doc, mapping = emit(constants, locations, args.source.resolve(), args.entry)
    output = args.output.resolve()
    if output.exists():
        print(json.dumps({"code": "c_import.destination_exists", "path": str(output)}))
        return 2
    output.parent.mkdir(parents=True, exist_ok=True)
    stage = Path(tempfile.mkdtemp(prefix=".qmh-import-", dir=output.parent))
    try:
        source = stage / "checked.frog"
        source.write_text(json.dumps(doc, indent=2, allow_nan=False) + "\n", encoding="utf-8")
        artifacts = {name: stage / f"{name}.json" for name in ("fir", "diagnostics", "provenance", "maps")}
        result = subprocess.run([str(args.frontend.resolve()), "derive", "--source", str(source),
                                 "--source-uri", "import:/qmh/checked.frog",
                                 "--source-format", "frog.source", "--source-version", "0.1", "--fir-version", "0.1",
                                 "--document-id", "qmh:source:checked",
                                 "--fir-document-id", "qmh:fir:checked", "--output", str(artifacts["fir"]),
                                 "--diagnostics-output", str(artifacts["diagnostics"]),
                                 "--provenance-output", str(artifacts["provenance"]),
                                 "--source-maps-output", str(artifacts["maps"])], check=False)
        if result.returncode:
            if artifacts["diagnostics"].exists():
                print(artifacts["diagnostics"].read_text(encoding="utf-8"))
            return result.returncode
        editable = stage / "checked-editable.frog"
        editable.write_text(json.dumps(editable_draft(doc), indent=2, allow_nan=False) + "\n", encoding="utf-8")
        result = subprocess.run([str(args.frontend.resolve()), "validate", "--source", str(editable),
                                 "--source-uri", "import:/qmh/checked-editable.frog",
                                 "--source-format", "frog.document.draft", "--source-version", "2", "--fir-version", "0.1",
                                 "--document-id", "qmh:draft:checked", "--fir-document-id", "qmh:fir:checked",
                                 "--diagnostics-output", str(stage / "draft-diagnostics.json")],
                                check=False, timeout=30)
        if result.returncode:
            diagnostics = stage / "draft-diagnostics.json"
            if diagnostics.exists():
                print(diagnostics.read_text(encoding="utf-8"))
            return result.returncode
        mapping_path = stage / "c-source-map.json"
        mapping_path.write_text(json.dumps({"source_sha256": hashlib.sha256(data).hexdigest(),
                                 "header_sha256": hashlib.sha256(header_data).hexdigest(),
                                 "clang_sha256": file_hash(args.clang),
                                 "command": command,
                                 "nodes": mapping}, indent=2) + "\n", encoding="utf-8")
        # Publish the Source, FIR and receipts together on the same volume.
        os.rename(stage, output)
    finally:
        if stage.exists():
            shutil.rmtree(stage)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

