# SPDX-License-Identifier: Apache-2.0
"""Derive the editable QMH Win32 Front Panel from the admitted Clang AST.

This adapter covers the authored layout and widget declarations. It reports the
native host behavior separately and does not bind controls to a substitute graph.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import time


def children(node):
    return node.get("inner", [])


def walk(node):
    if node.get("kind"):
        yield node
        for child in children(node):
            yield from walk(child)


def syntax(node):
    kind = node.get("kind")
    parts = children(node)
    if kind in {"ImplicitCastExpr", "ParenExpr", "CStyleCastExpr"} and len(parts) == 1:
        return syntax(parts[0])
    if kind == "IntegerLiteral":
        return int(node["value"])
    if kind == "StringLiteral":
        token = node["value"]
        return json.loads(token.removeprefix("L"))
    if kind == "DeclRefExpr":
        return node["referencedDecl"]["name"]
    tag = node.get("opcode", node.get("name", kind))
    if kind == "UnaryOperator" and node.get("isPostfix"):
        tag = "post" + tag
    return (tag, *(syntax(child) for child in parts))


def one(values):
    value, = values
    return value


def calls(node, name):
    return [n for n in walk(node) if n["kind"] == "CallExpr" and syntax(children(n)[0]) == name]


def local(node, name):
    return one(n for n in walk(node) if n["kind"] == "VarDecl" and n.get("name") == name)


def position(node, data, source_name="calculator.c"):
    begin = node.get("range", {}).get("begin", node.get("loc", {}))
    begin = begin.get("expansionLoc", begin)
    offset = begin["offset"]
    return {"file": source_name, "line": data.count(b"\n", 0, offset) + 1,
            "column": offset - data.rfind(b"\n", 0, offset), "byte_offset": offset}


def require(condition, code):
    if not condition:
        raise ValueError(code)


def bits(expr):
    if isinstance(expr, int):
        return expr
    op, left, right = expr
    require(op == "|", "unsupported_window_style")
    return bits(left) | bits(right)


def rect(values):
    require(len(values) == 4 and all(type(value) is int for value in values)
            and min(values) >= 0 and min(values[2:]) > 0, "unsupported_rectangle")
    return dict(zip(("x", "y", "width", "height"), values))


class BindingRefusal(ValueError):
    def __init__(self, reason, function, node, data, source_name):
        super().__init__(reason)
        self.diagnostic = {"code": "c_ui_import." + reason, "member": function,
                           **position(node, data, source_name),
                           "message": "The observed C binding differs from the bounded panel profile. "
                                      "No executable binding or partial output was emitted."}


def host_bindings(body, data, source_name):
    """Attribute the observed C wiring, without admitting an executable graph.

    These are call-site/source observations, not a callback-name-to-FROG-event
    registry. Keep the layout-only boundary explicit until the complete imported
    consumer and evaluator are admitted by the common Frontend.
    """
    queue = ("&", ("queue", "app"))

    def call(function, name, arguments):
        node = one(calls(body[function], name))
        if syntax(node)[2:] != arguments:
            raise BindingRefusal("unsupported_" + function + "_binding", function, node, data, source_name)
        return node

    key = call("producer_key", "enqueue_element", (queue, "label", ("++", ("sent", "app")),
               "error", ("&", "outgoing")))
    close = call("producer_close", "enqueue_element", (queue, "close", ("++", ("sent", "app")),
                 ("CallExpr", "producer_error"), ("&", "outgoing")))
    release = call("producer_close", "release_queue", (queue, "outgoing", ("&", "outgoing")))
    abort = call("host_abort", "SetEvent", (("abort_requested", "app"),))
    abort_release = call("host_abort", "queue_release", (queue,))
    dequeue = call("consumer_thread", "dequeue_element", (queue, ("&", "message"),
                   ("&", "sequence"), ("error", "state"), ("&", ("error", "state"))))
    obtain = call("initialize", "obtain_queue", (queue, "startup_error", ("&", ("producer_error", "app"))))
    if position(close, data)["byte_offset"] >= position(release, data)["byte_offset"]:
        raise BindingRefusal("unsupported_close_release_order", "producer_close", release, data, source_name)
    if position(abort, data)["byte_offset"] >= position(abort_release, data)["byte_offset"]:
        raise BindingRefusal("unsupported_abort_release_order", "host_abort", abort_release, data, source_name)

    # Follow actual calls from the producer/consumer entry points. In particular,
    # EDIT/Control/field names alone do not establish a computational input.
    pending, reachable = ["producer_key", "producer_close", "consumer_thread"], set()
    while pending:
        name = pending.pop()
        if name in reachable:
            continue
        reachable.add(name)
        for node in walk(body[name]):
            if (node.get("kind") == "MemberExpr" and node.get("name") == "error_fields"
                    and syntax(node) == ("error_fields", "app")):
                raise BindingRefusal("unsupported_error_field_access", name, node, data, source_name)
            if node.get("kind") == "CallExpr":
                callee = syntax(children(node)[0])
                if isinstance(callee, str) and callee in body and callee not in reachable:
                    pending.append(callee)

    def site(function, node):
        return {"function": function, "source": position(node, data, source_name),
                "call": syntax(node)}

    return {
        "scope": "observed C call sites; not executable FROG bindings",
        "execution_bound": False,
        "shared_queue": [site(name, node) for name, node in (
            ("initialize", obtain), ("producer_key", key), ("producer_close", close),
            ("producer_close", release), ("host_abort", abort_release), ("consumer_thread", dequeue))],
        "close_order": [site("producer_close", close), site("producer_close", release)],
        "abort_order": [site("host_abort", abort), site("host_abort", abort_release)],
        "error_input_read_check": {"source_scope": source_name, "reachable_members": sorted(reachable),
                                   "app_error_fields_references": 0,
                                   "meaning": "No error EDIT handle reaches the observed producer/consumer calls. Startup error is a separate input."},
    }


def derive(ast, data, source_name="calculator.c"):
    body = {n["name"]: n for n in children(ast) if n.get("kind") == "FunctionDecl"
            and children(n) and children(n)[-1].get("kind") == "CompoundStmt"}
    window = body["window_proc"]
    layout = body["layout"]
    bg = body["draw_background"]
    init = body["initialize"]
    bindings = host_bindings(body, data, source_name)
    key_table = local(ast, "keys")
    key_rows = children(one(children(key_table)))
    display, key, stop, edit = [syntax(n)[2:] for n in calls(window, "CreateWindowExW")]
    require((display[1], key[1], stop[1], edit[1]) == ("STATIC", "BUTTON", "BUTTON", "EDIT")
            and all(call[8] == "window" for call in (display, key, stop, edit)),
            "unsupported_child_creation")
    require(bits(display[3]) == 0x5000000D and
            bits(key[3]) == bits(stop[3]) == 0x5001000B and
            edit[3] == ("|", ("|", 0x40000000, 0x10000000),
                         ("ConditionalOperator", (">=", "i", 3), 0x800, 0x10000)),
            "unsupported_child_style")
    require(key[2] == ("caption", ("ArraySubscriptExpr", "keys", "i")),
            "unsupported_caption_binding")
    key_base, i = key[9][1:]
    require(key[9][0] == "+" and i == "i" and isinstance(key_base, int),
            "unsupported_key_identity")
    require(len(key_rows) == 17, "unsupported_key_count")

    move_calls = calls(layout, "move_control")
    move_display, move_keys, move_stop, move_errors = [syntax(n)[2:] for n in move_calls]
    require(move_display[0] == ("display", "app") and move_stop[0] == ("stop", "app") and
            move_keys == (("ArraySubscriptExpr", ("buttons", "app"), "i"),
                          *((field, ("ArraySubscriptExpr", "keys", "i"))
                            for field in ("x", "y", "width", "height"))),
            "unsupported_layout_binding")
    require(move_errors[0] == ("ArraySubscriptExpr", ("error_fields", "app"),
                               ("+", ("*", "group", 3), "field")) and
            move_errors[1][0:2] == ("ConditionalOperator", "group") and
            move_errors[2][0] == "+" and move_errors[2][2][0:2] == ("*", "field"),
            "unsupported_error_layout")
    left, right = move_errors[1][3], move_errors[1][2]
    y, stride = move_errors[2][1], move_errors[2][2][2]
    width, height = move_errors[3:]
    require(edit[2][0:2] == ("ConditionalOperator", ("==", ("%", "i", 3), 0)) and
            edit[2][3][0:2] == ("ConditionalOperator", ("==", ("%", "i", 3), 1)) and
            edit[9][0] == "+" and edit[9][2] == "i", "unsupported_error_initialization")
    defaults = (edit[2][2], *edit[2][3][2:])

    scale_call = syntax(one(calls(body["scale"], "MulDiv")))[2:]
    require(scale_call == ("value", ("dpi", "app"), 96), "unsupported_dpi_context")
    font = syntax(one(calls(layout, "CreateFontW")))
    require(font[2][:2] == ("-", ("CallExpr", "scale", font[2][1][2]))
            and isinstance(font[-1], str), "unsupported_font")
    font_px = font[2][1][2]
    font_family = font[-1]
    initial_size = [syntax(n)[2:] for n in calls(init, "MulDiv")]
    require(all(item[1:] == ("dpi", 96) for item in initial_size), "unsupported_initial_size")
    scroll = [syntax(local(layout, axis))[1][4] for axis in ("horizontal", "vertical")]
    require(all(item[:2] == ("-", ("CallExpr", "scale", item[1][2])) and item[2] == 1
                for item in scroll), "unsupported_scroll_range")
    canvas_w, canvas_h = [item[1][2] for item in scroll]
    pad = syntax(local(bg, "pad"))[1][1:]
    pad_rect = [item[1][2] for item in pad]
    require(all(item[0] == "-" and item[1][:2] == ("CallExpr", "scale")
                for item in pad), "unsupported_keypad_frame")
    headings = syntax(local(bg, "labels"))[1][1:]
    heading_area = syntax(local(bg, "area"))[1][1:]
    require(len(headings) == 2 and all(item[0] == "-" and item[1][:2] == ("CallExpr", "scale")
                                      for item in heading_area), "unsupported_error_headings")
    title = syntax(one(calls(init, "CreateWindowExW")))[4]

    prop = syntax(one(calls(window, "SetPropW")))[2:]
    require(prop == (("ArraySubscriptExpr", ("buttons", "app"), "i"), "QMH.Label.Text",
                     ("label", ("ArraySubscriptExpr", "keys", "i"))), "unsupported_label_binding")
    read = one(calls(body["producer_key"], "GetPropW"))
    enqueue = one(calls(body["producer_key"], "enqueue_element"))
    require(syntax(read)[2:] == ("source", prop[1]) and
            syntax(enqueue)[2:5] == (("&", ("queue", "app")), "label", ("++", ("sent", "app"))),
            "unsupported_producer_binding")

    widgets, mapping = [], []
    def add(ident, klass, role, value_type, label, bounds, props, origin, control_id=None):
        widget = {"id": ident, "classId": "frog.widgets." + klass, "role": role,
                  "valueType": value_type, "label": label, "bounds": rect(bounds), "props": props}
        if value_type == "bool":
            widget["state"] = {"value": False}
        if klass.startswith("string_"):
            props.update({"layout.size_to_text": False, "style.text.font_family": font_family,
                          "style.text.font_size": f"{font_px}px"})
            widget["typography"] = {"inheritanceVersion": 1, "resolvedPointSize": font_px * 72 / 96}
        widgets.append(widget)
        mapping.append({"widget": ident, "source": position(origin, data, source_name), "control_id": control_id,
                        "class": widget["classId"], "role": role, "value_type": value_type,
                        "layout_coverage": "exact_logical_at_96_dpi",
                        "logical_bounds": widget["bounds"], "label": label})

    def button(ident, label, caption, bounds, origin, control_id, mechanical_action):
        add(ident, "button", "control", "bool", label, bounds,
            {"label.visible": False, "caption.independent": True,
             "caption.text": caption, "caption.visible": False, "state_text.visible": True,
             "state_text.false_text": caption, "state_text.true_text": caption,
             "behavior.mechanical_action": mechanical_action}, origin, control_id)
        mapping[-1].update(caption=caption, studio_mechanical_action=mechanical_action,
                           interaction_coverage="unbound")

    add("qmh-display", "string_indicator", "indicator", "string", "display", move_display[1:],
        {"label.visible": False, "text.value": display[2], "interaction.text.single_line": True,
         "text.horizontal_alignment": "right"}, move_calls[0], display[9])
    for index, row in enumerate(key_rows):
        _, label, caption, *bounds = syntax(row)
        require(isinstance(label, str) and isinstance(caption, str), "unsupported_key_literal")
        cid = key_base + index
        button(f"qmh-key-{cid}", label, caption, bounds, row, cid, "latch_when_pressed")
        mapping[-1]["c_queue_label"] = label
    require(len({item.get("c_queue_label") for item in mapping[1:]}) == len(key_rows),
            "duplicate_key_label")
    button(f"qmh-stop-{stop[9]}", stop[2], stop[2], move_stop[1:], move_calls[2],
           stop[9], "switch_until_released")
    for group, x in enumerate((left, right)):
        role = ("control", "indicator")[group]
        group_id = ("input", "output")[group]
        ht, hb = heading_area[1][1][2], heading_area[3][1][2]
        heading_right = heading_area[2][1][2][2 if group else 3]
        add(f"qmh-error-{group_id}-heading", "label", "support", "support_text", headings[group],
            (x, ht, heading_right - x, hb - ht), {"text.value": headings[group]}, local(bg, "labels"))
        for field, (name, initial) in enumerate(zip(("status", "code", "source"), defaults)):
            cid = edit[9][1] + group * 3 + field
            # Win32 EDIT stores all three fields as text, including status and code.
            add(f"qmh-error-{group_id}-{name}", "string_" + role, role, "string",
                f"{headings[group]} {name}", (x, y + field * stride, width, height),
                {"label.visible": False, "text.value": initial, "interaction.text.single_line": True},
                move_calls[3], cid)
            mapping[-1]["c_initial_text"] = initial
            mapping[-1]["c_read_by_consumer"] = False

    document = {"format": "frog.document.draft", "draft_revision": 2, "language": "FROG",
                "title": title + " - Front Panel import",
                "metadata": {"name": title + " - Front Panel import",
                             "description": "Editable Win32 layout import. QMH program bindings are pending."},
                "text_defaults": {"font_family": font_family, "point_size": font_px * 72 / 96},
                "interface": {"inputs": [], "outputs": []},
                "frontPanel": {"canvas": {"coordinate_space": "panel_pixels", "width": canvas_w,
                                           "height": canvas_h}, "grid": {"pitch": 12}, "widgets": widgets},
                "diagram": {"nodes": [], "edges": []}}
    effects = {
        "key_message": (read, "partial", "Label and visible caption are imported. Runtime source reference, latch read and queue delivery remain unbound."),
        "close": (body["producer_close"], "absent", "Enqueue close then release before consumer join remains native C."),
        "abort": (body["host_abort"], "absent", "Signal abort and release the same queue remains native C."),
        "snapshot": (body["receive_snapshot"], "partial", "Display and error widget roles/defaults are imported. Publication, termination and display writes remain native C."),
        "dpi": (body["move_control"], "partial", "Import uses logical bounds at 96 DPI. Win32 resize, scroll and font mapping use the existing Studio presentation."),
        "focus": (body["button_proc"], "partial", "Key order is retained. Win32 mouse-down and keyboard/focus dispatch remain native C."),
        "fifo": (body["consumer_thread"], "absent", "The Lot 09 parser-check fragment cannot replace the QMH consumer and FIFO."),
    }
    receipt = {"source_sha256": hashlib.sha256(data).hexdigest(), "logical_dpi": scale_call[-1],
               "initial_client_size": [args[0] for args in initial_size], "keypad_frame": pad_rect,
               "host_binding_evidence": bindings,
               "widgets": mapping, "effects": {
                   name: {"source": position(node, data, source_name), "coverage": coverage, "detail": detail}
                   for name, (node, coverage, detail) in effects.items()}}
    return document, receipt


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--clang", type=Path, required=True)
    parser.add_argument("--include", type=Path, action="append", default=[])
    parser.add_argument("--source", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--frontend", type=Path, required=True,
                        help="Validate with the current common Frontend before publication.")
    args = parser.parse_args()
    source = args.source.resolve(strict=True)
    output = args.output.resolve()
    if output.exists():
        parser.exit(2, "c_ui_import.destination_exists\n")
    data = source.read_bytes()
    clang = args.clang.resolve(strict=True)
    argv = [str(clang), "/nologo", "/TC", "/std:c17", "/utf-8", "/W4", "/WX", "/O2",
            "/MT", "/DUNICODE", "/D_UNICODE", "/D_WIN32_WINNT=0x0A00", "/clang:-fsyntax-only",
            "/clang:-Xclang", "/clang:-ast-dump=json", *(f"/I{p.resolve(strict=True)}" for p in args.include),
            str(source)]
    env = {key: value for key, value in os.environ.items() if key.upper() not in {"CL", "_CL_", "INCLUDE"}}
    begin = time.perf_counter()
    try:
        result = subprocess.run(argv, env=env, capture_output=True, timeout=30)
    except subprocess.TimeoutExpired:
        parser.exit(2, "c_ui_import.clang_timeout\n")
    if result.returncode:
        parser.exit(2, "c_ui_import.clang_refused\n" + result.stderr.decode("utf-8", errors="replace"))
    capture = time.perf_counter()
    if source.read_bytes() != data:
        parser.exit(2, "c_ui_import.source_changed\n")
    ast = {}
    try:
        ast = json.loads(result.stdout)
        document, receipt = derive(ast, data, source.name)
    except (KeyError, ValueError, IndexError, TypeError) as error:
        if isinstance(error, BindingRefusal):
            parser.exit(2, json.dumps(error.diagnostic, ensure_ascii=False) + "\n")
        code = "c_ui_import.required_member_missing" if isinstance(error, KeyError) else "c_ui_import.unsupported_ast"
        detail = {"code": code, "source": str(source), "member": str(error),
                  "message": "The parsed C does not match this specialized Win32 layout profile. Required members include window_proc, keys and layout; no callback is inferred as a FROG event. The complete original AST refusal is retained.",
                  "original": str(error)}
        member = str(error).strip("'") if isinstance(error, KeyError) else (
            "layout" if "layout" in str(error) else "keys" if "key" in str(error) else "window_proc")
        context = next((n for n in children(ast) if n.get("name") == member), None)
        if context is not None:
            detail["context"] = {"member": member, **position(context, data, source.name)}
        parser.exit(2, json.dumps(detail, ensure_ascii=False) + "\n")
    receipt.update(clang_sha256=hashlib.sha256(clang.read_bytes()).hexdigest(), command=argv)
    output.parent.mkdir(parents=True, exist_ok=True)
    stage = Path(tempfile.mkdtemp(prefix=".qmh-ui-import-", dir=output.parent))
    try:
        for filename, value in (("qmh-front-panel.frog", document), ("mapping.json", receipt)):
            (stage / filename).write_text(json.dumps(value, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
        try:
            validation = subprocess.run([str(args.frontend.resolve(strict=True)), "validate", "--source",
                                         str(stage / "qmh-front-panel.frog"), "--source-uri", "import:/c/win32-panel.frog",
                                         "--source-format", "frog.document.draft", "--source-version", "2",
                                         "--fir-version", "0.1", "--document-id", "c:draft:win32-panel",
                                         "--fir-document-id", "c:fir:win32-panel", "--diagnostics-output",
                                         str(stage / "frontend-diagnostics.json")], capture_output=True, timeout=30)
        except (OSError, subprocess.TimeoutExpired) as error:
            parser.exit(2, f"c_ui_import.frontend_unavailable: {error}\n")
        if validation.returncode:
            detail = validation.stdout or validation.stderr
            parser.exit(2, "c_ui_import.frontend_refused\n" + detail.decode("utf-8", errors="replace"))
        os.rename(stage, output)
    finally:
        if stage.exists():
            shutil.rmtree(stage)
    print(f"QMH_UI_LAYOUT_IMPORTED widgets={len(receipt['widgets'])} "
          f"ast_ms={(capture-begin)*1000:.3f} publish_ms={(time.perf_counter()-capture)*1000:.3f}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

