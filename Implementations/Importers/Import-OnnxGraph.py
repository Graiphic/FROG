# SPDX-License-Identifier: Apache-2.0
"""Import a bounded ONNX scalar Boolean graph into an editable Studio draft."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import time

import onnx
from google.protobuf.message import DecodeError
from onnx import TensorProto, checker, numpy_helper, shape_inference


MAX_MODEL_BYTES = 1024 * 1024
MAX_NODES = 24
MAX_DECLARATIONS = 64
MAX_NAME_LENGTH = 48
MAX_PORTS = 8
OPERATIONS = {
    "And": ("frog.core.and", ("a", "b")),
    "Or": ("frog.core.or", ("a", "b")),
    "Not": ("frog.core.not", ("in",)),
}


class ImportRefusal(ValueError):
    def __init__(self, code, location, message):
        super().__init__(message)
        self.diagnostic = {"code": "onnx_import." + code, "source": location, "message": message}


def require(condition, code, location, message):
    if not condition:
        raise ImportRefusal(code, location, message)


def stable_id(kind, name):
    digest = hashlib.sha256(name.encode("utf-8")).hexdigest()
    return f"onnx-{kind}-{digest[:24]}"


def bounds(x, y, width, height):
    return {"x": x, "y": y, "width": width, "height": height}


def shape(value, location):
    typ = value.type
    require(typ.HasField("tensor_type"), "type_unavailable", location, "Expected a tensor declaration.")
    tensor = typ.tensor_type
    require(tensor.elem_type == TensorProto.BOOL, "type_unavailable", location, "Expected the exact ONNX BOOL type.")
    require(tensor.HasField("shape") and not tensor.shape.dim,
            "shape_unavailable", location, "Only an explicit rank-0 shape is supported. Array, symbolic and unknown shapes are refused.")


def name(value, location):
    require(bool(value) and len(value) <= MAX_NAME_LENGTH and all(ch.isprintable() for ch in value),
            "name_unavailable", location, "Expected a nonempty printable name within the preview size budget.")


def load_model(data):
    try:
        model = onnx.load_model_from_string(data)
    except (ValueError, DecodeError) as error:
        raise ImportRefusal("decode", "/", str(error)) from error
    require(model.ir_version == 9 and [(item.domain, item.version) for item in model.opset_import] == [("", 18)],
            "profile_unavailable", "/opset_import", "This importer supports IR 9 and the default domain opset 18.")
    require(model.HasField("graph") and not model.functions and not model.training_info,
            "regions_unavailable", "/graph", "Functions and training subgraphs are outside the admitted profile.")
    graph = model.graph
    require(not model.configuration, "profile_unavailable", "/configuration",
            "Device configurations are outside the admitted profile.")
    require(not graph.sparse_initializer, "initializer_unavailable", "/graph/sparse_initializer",
            "Sparse initializers require a different resource profile.")
    require(0 < len(graph.node) <= MAX_NODES and 0 < len(graph.input) <= MAX_PORTS and
            0 < len(graph.output) <= MAX_PORTS and
            sum(len(x) for x in (graph.input, graph.output, graph.value_info, graph.initializer)) <= MAX_DECLARATIONS,
            "graph_budget", "/graph", "Expected a bounded graph with inputs, outputs and operations.")
    for i, tensor in enumerate(graph.initializer):
        path = f"/graph/initializer/{i}"
        require(not tensor.external_data and tensor.data_location != TensorProto.EXTERNAL,
                "external_initializer_unavailable", path, "External initializer data is outside the admitted resource profile.")
        require(tensor.data_type == TensorProto.BOOL and not tensor.dims,
                "initializer_unavailable", path, "Expected an embedded rank-0 BOOL initializer.")
        if tensor.HasField("raw_data"):
            require(tensor.raw_data in (b"\x00", b"\x01") and not tensor.int32_data,
                    "initializer_value", path, "Expected one exact BOOL byte.")
        else:
            require(list(tensor.int32_data) in ([0], [1]),
                    "initializer_value", path, "Expected one exact BOOL value.")
    for i, node in enumerate(graph.node):
        path = f"/graph/node/{i}"
        require(not node.domain and node.op_type in OPERATIONS, "operation_unavailable", path,
                f"Unsupported operator {node.domain or 'ai.onnx'}::{node.op_type} ({node.name}).")
        require(not node.attribute, "attribute_unavailable", path + "/attribute",
                "The admitted Boolean operators have no attributes or subgraphs.")
    for field in ("input", "output", "value_info"):
        for i, value in enumerate(getattr(graph, field)):
            path = f"/graph/{field}/{i}"
            shape(value, path)
            name(value.name, path + "/name")
    try:
        checker.check_model(model, full_check=True)
        inferred = shape_inference.infer_shapes(model, check_type=True, strict_mode=True, data_prop=False)
    except (ValueError, checker.ValidationError, shape_inference.InferenceError) as error:
        raise ImportRefusal("checker", "/graph", str(error)) from error
    for i, value in enumerate(inferred.graph.value_info):
        shape(value, f"/graph/value_info/{i}")
    return model


def convert(model, digest):
    graph = model.graph
    title = graph.name or "ONNX Boolean graph"
    name(title, "/graph/name")
    inputs = {v.name for v in graph.input}
    initial = {}
    shapes = (graph.input, graph.output, graph.value_info)
    tensor_names = [item.name for group in shapes for item in group]
    tensor_names.extend(name for operation in graph.node for name in operation.output)
    note_width = max(380, (max(map(len, tensor_names), default=0) + 35) * 12)
    lane_height = 142
    first_row = 96
    operations_x = 75 + note_width
    output_x = operations_x + 132 * len(graph.node) + 42
    last_row = first_row + lane_height * (min(len(graph.node), len(inputs)) - 1)
    nodes, edges, widgets, mapping = [], [], [], []
    producers = {}

    def node(ident, kind, location, layout, **properties):
        nodes.append({"id": ident, "kind": kind, "layout": layout, **properties})
        mapping.append({"frog_id": ident, "source": location})

    def link(value, target, port, location):
        source, out = producers[value]
        ident = stable_id("wire", target + "/" + port)
        edge = {"id": ident, "from": {"node": source, "port": out},
                "to": {"node": target, "port": port}}
        if out == "result" and port != "value":
            edge["label"] = {"text": value, "normalized_position": 0.35, "offset_x": 34, "offset_y": 0}
        edges.append(edge)
        mapping.append({"frog_id": ident, "source": location, "onnx_value": value, "type": "BOOL", "shape": []})

    def widget(value, role, index, default=None):
        source = f"/graph/{'input' if role == 'control' else 'output'}/{index}"
        ident = stable_id(role, value)
        terminal = ident + "-value"
        note = ("Contrôle suggéré — entrée ONNX : " if role == "control"
                else "Indicateur suggéré — sortie ONNX : ") + value
        # Logical diagram lanes leave space for the native comments.
        x, y = ((120, first_row + lane_height * index) if role == "control"
                else (output_x, last_row + lane_height * index))
        px, py = (32 if role == "control" else note_width + 86), 110 + 112 * index
        declaration = {"id": ident, "classId": "frog.widgets.boolean_" + role, "role": role,
                       "valueType": "bool", "label": value, "bounds": bounds(px, py, 108, 32),
                       "props": {"label.visible": True, "value": bool(default), "caption.independent": True,
                                 "caption.text": note, "caption.visible": True},
                       "state": {"value": bool(default)}}
        widgets.append(declaration)
        node(terminal, "widget_value", source, bounds(x, y, 66, 28), type="bool", widget=ident)
        node(terminal + "-note", "comment", source, bounds(x, y + 35, note_width, 32),
             text=note, comment_auto_size=False, vertical_arrangement="none",
             font={"family": "", "style": "plain", "point_size": 15,
                   "use_default_size": True, "geometry_initialized": False})
        mapping[-2].update(onnx_value=value, type="BOOL", shape=[], widget=ident)
        if role == "control":
            producers[value] = (terminal, "value")
            mapping[-2]["default_origin"] = "initializer" if value in initial else "editor_placeholder"
        else:
            link(value, terminal, "value", source)

    for i, tensor in enumerate(graph.initializer):
        path = f"/graph/initializer/{i}"
        name(tensor.name, path + "/name")
        initial[tensor.name] = bool(numpy_helper.to_array(tensor).item())
        if tensor.name not in inputs:
            ident = stable_id("initializer", tensor.name)
            token = str(initial[tensor.name]).lower()
            node(ident, "primitive", path, bounds(120, 30 if i == 0 else first_row + lane_height * (len(inputs) + i - 1), 34, 24),
                 type=f"frog.studio.boolean.{token}_constant", value_type="bool",
                 text=token, label=tensor.name, label_visible=True)
            producers[tensor.name] = (ident, "value")
            mapping[-1].update(onnx_value=tensor.name, type="BOOL", shape=[], value=initial[tensor.name])
    for i, value in enumerate(graph.input):
        widget(value.name, "control", i, initial.get(value.name))
    for i, operation in enumerate(graph.node):
        path = f"/graph/node/{i}"
        if operation.name:
            name(operation.name, path + "/name")
        output = operation.output[0]
        name(output, path + "/output/0")
        ident = stable_id("op", output)
        operation_type, ports = OPERATIONS[operation.op_type]
        node(ident, "primitive", path, bounds(operations_x + 132 * i, first_row + lane_height * min(i, len(inputs) - 1), 46, 40),
             type=operation_type, label=operation.name or operation.op_type, label_visible=True)
        mapping[-1].update(onnx_name=operation.name, op_type=operation.op_type,
                           domain=operation.domain, opset=18, attributes=[],
                           inputs=list(operation.input), outputs=list(operation.output), shape=[])
        for input_index, (value, port) in enumerate(zip(operation.input, ports, strict=True)):
            link(value, ident, port, f"{path}/input/{input_index}")
        producers[output] = (ident, "result")
    for i, value in enumerate(graph.output):
        widget(value.name, "indicator", i)

    document = {"format": "frog.document.draft", "draft_revision": 2, "language": "FROG",
                "title": title, "metadata": {"name": title, "description": f"ONNX IR 9, ai.onnx opset 18. SHA-256 {digest}."},
                "interface": {"inputs": [], "outputs": []},
                "frontPanel": {"canvas": {"coordinate_space": "panel_pixels", "width": 2 * note_width + 120,
                                          "height": 150 + 112 * max(len(graph.input), len(graph.output))},
                               "widgets": widgets},
                "diagram": {"zoom_percent": 70, "viewport_origin": {"x": 0, "y": 0},
                            "nodes": nodes, "edges": edges}}
    initializer_records = [
        {"name": item.name, "type": "BOOL", "shape": [], "value": initial[item.name],
         "overridable": item.name in inputs, "source": f"/graph/initializer/{i}"}
        for i, item in enumerate(graph.initializer)
    ]
    receipt = {"onnx_sha256": digest, "onnx_library": onnx.__version__, "ir_version": 9,
               "opset_import": {"": 18}, "graph": graph.name, "objects": mapping,
               "initializers": initializer_records}
    return document, receipt


def validate(frontend, stage):
    result = subprocess.run([str(frontend), "validate", "--source", str(stage / "model.frog"),
                             "--source-uri", "import:/onnx/model.frog", "--document-id", "onnx:studio:draft",
                             "--fir-document-id", "onnx:studio:fir", "--source-format", "frog.document.draft",
                             "--source-version", "2", "--fir-version", "0.1", "--diagnostics-output",
                             str(stage / "frontend-diagnostics.json")],
                            check=False, capture_output=True, timeout=30)
    if result.returncode:
        diagnostics = stage / "frontend-diagnostics.json"
        detail = diagnostics.read_text(encoding="utf-8") if diagnostics.exists() else result.stderr.decode(errors="replace")
        raise ImportRefusal("frontend_refused", "/", detail)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("onnx_file", type=Path)
    parser.add_argument("--frontend", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    output = args.output.resolve()
    try:
        require(not output.exists(), "destination_exists", "/", "The destination must be new.")
        start = time.perf_counter()
        with args.onnx_file.open("rb") as stream:
            data = stream.read(MAX_MODEL_BYTES + 1)
        require(len(data) <= MAX_MODEL_BYTES, "model_budget", "/", "Maximum ONNX size is 1 MiB.")
        digest = hashlib.sha256(data).hexdigest()
        model = load_model(data)
        checked = time.perf_counter()
        draft, receipt = convert(model, digest)
        output.parent.mkdir(parents=True, exist_ok=True)
        stage = Path(tempfile.mkdtemp(prefix=".onnx-import-", dir=output.parent))
        try:
            for filename, value in (("model.frog", draft), ("onnx-mapping.json", receipt)):
                (stage / filename).write_text(json.dumps(value, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
            (stage / "original.onnx").write_bytes(data)
            validate(args.frontend.resolve(strict=True), stage)
            os.rename(stage, output)
        finally:
            if stage.exists():
                shutil.rmtree(stage)
        end = time.perf_counter()
        print(f"ONNX_IMPORT_PASS checker_ms={(checked-start)*1000:.3f} "
              f"prepare_publish_ms={(end-checked)*1000:.3f} "
              f"nodes={len(draft['diagram']['nodes'])} edges={len(draft['diagram']['edges'])}")
    except ImportRefusal as error:
        print(json.dumps(error.diagnostic, ensure_ascii=False))
        return 2
    except (OSError, subprocess.TimeoutExpired) as error:
        print(json.dumps({"code": "onnx_import.io_or_timeout", "source": "/", "message": str(error)}))
        return 2
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
