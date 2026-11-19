# SPDX-License-Identifier: Apache-2.0
"""Import a schema-checked, bounded ONNX Boolean graph into an editable Studio draft."""
import argparse
import copy
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
    "Xor": ("frog.core.xor", ("a", "b")),
    "Not": ("frog.core.not", ("in",)),
}
MIN_IR, MAX_IR = 7, 13
MIN_OPSET, MAX_OPSET = 7, 25
UI = {
    "en": ("Inputs", "Outputs", "Suggested control — ONNX input: ", "Suggested indicator — ONNX output: "),
    "fr": ("Entrées", "Sorties", "Contrôle suggéré — entrée ONNX : ", "Indicateur suggéré — sortie ONNX : "),
    "it": ("Ingressi", "Uscite", "Controllo suggerito — ingresso ONNX: ", "Indicatore suggerito — uscita ONNX: "),
    "zh": ("输入", "输出", "建议控件 — ONNX 输入：", "建议指示器 — ONNX 输出："),
    "ja": ("入力", "出力", "推奨コントロール — ONNX 入力: ", "推奨インジケーター — ONNX 出力: "),
    "de": ("Eingänge", "Ausgänge", "Vorgeschlagenes Bedienelement — ONNX-Eingang: ", "Vorgeschlagener Indikator — ONNX-Ausgang: "),
    "es": ("Entradas", "Salidas", "Control sugerido — entrada ONNX: ", "Indicador sugerido — salida ONNX: "),
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
    require(tensor.HasField("shape"), "shape_unavailable", location, "An explicit shape is required; unknown rank is not a scalar.")
    dimensions = tensor.shape.dim
    require(len(dimensions) <= 1 and all(d.HasField("dim_value") and 1 <= d.dim_value <= 64 for d in dimensions),
            "shape_unavailable", location, "Supported BOOL shapes are [] and fixed rank-1 [1..64]; symbolic, empty and higher-rank tensors remain outside this profile.")
    return tuple(d.dim_value for d in dimensions)


def describe(model):
    def declaration(v):
        t = v.type.tensor_type
        return {"name": v.name, "type": TensorProto.DataType.Name(t.elem_type) if t.elem_type else "UNDEFINED",
                "shape": [d.dim_value if d.HasField("dim_value") else d.dim_param or "?" for d in t.shape.dim]
                         if t.HasField("shape") else None}
    return {"ir_version": model.ir_version,
            "opsets": [{"domain": x.domain or "ai.onnx", "version": x.version} for x in model.opset_import],
            "inputs": [declaration(v) for v in model.graph.input],
            "outputs": [declaration(v) for v in model.graph.output],
            "operations": [{"name": n.name, "domain": n.domain or "ai.onnx", "operator": n.op_type} for n in model.graph.node],
            "initializers": [{"name": t.name, "type": TensorProto.DataType.Name(t.data_type), "shape": list(t.dims),
                              "encoding": "external" if t.data_location == TensorProto.EXTERNAL else "raw_data" if t.HasField("raw_data") else "typed_fields"}
                             for t in model.graph.initializer],
            "available_profile": {"ir": [MIN_IR, MAX_IR], "ai.onnx_opset": [MIN_OPSET, MAX_OPSET],
                                  "operators": list(OPERATIONS), "types": ["BOOL"], "shapes": ["[]", "[1..64]"]}}


def model_shapes(model):
    result = {v.name: shape(v, "/graph/value/" + v.name)
              for group in (model.graph.input, model.graph.output, model.graph.value_info) for v in group}
    result.update({t.name: tuple(t.dims) for t in model.graph.initializer})
    return result


def name(value, location):
    require(bool(value) and len(value) <= MAX_NAME_LENGTH and all(ch.isprintable() for ch in value),
            "name_unavailable", location, "Expected a nonempty printable name within the preview size budget.")


def load_model(data, resource_base=None):
    try:
        model = onnx.load_model_from_string(data)
    except (ValueError, DecodeError) as error:
        raise ImportRefusal("decode", "/", str(error)) from error
    versions = [(item.domain, item.version) for item in model.opset_import]
    require(MIN_IR <= model.ir_version <= min(MAX_IR, onnx.IR_VERSION) and
            len(versions) == 1 and versions[0][0] == "" and MIN_OPSET <= versions[0][1] <= MAX_OPSET,
            "profile_unavailable", "/opset_import",
            f"Found IR {model.ir_version}, opsets {versions}. This host covers IR {MIN_IR}..{MAX_IR} and ai.onnx opset {MIN_OPSET}..{MAX_OPSET} only for the schema-checked BOOL operations {', '.join(OPERATIONS)}. No version fields are rewritten.")
    require(model.HasField("graph") and not model.functions and not model.training_info,
            "regions_unavailable", "/graph", "Functions and training subgraphs are outside the admitted profile.")
    graph = model.graph
    require(not model.configuration, "profile_unavailable", "/configuration",
            "Device configurations are outside the admitted profile.")
    require(not graph.sparse_initializer, "initializer_unavailable", "/graph/sparse_initializer",
            "Sparse initializers require a different resource profile.")
    require(not graph.quantization_annotation, "profile_unavailable", "/graph/quantization_annotation",
            "Quantization annotations are outside this Boolean profile.")
    require(0 < len(graph.node) <= MAX_NODES and 0 < len(graph.input) <= MAX_PORTS and
            0 < len(graph.output) <= MAX_PORTS and
            sum(len(x) for x in (graph.input, graph.output, graph.value_info, graph.initializer)) <= MAX_DECLARATIONS,
            "graph_budget", "/graph", "Expected a bounded graph with inputs, outputs and operations.")
    for i, tensor in enumerate(graph.initializer):
        path = f"/graph/initializer/{i}"
        if tensor.external_data or tensor.data_location == TensorProto.EXTERNAL:
            location = next((x.value for x in tensor.external_data if x.key == "location"), "")
            if resource_base is not None and location:
                root = Path(resource_base).resolve()
                candidate = (root / location).resolve()
                require(not Path(location).is_absolute() and candidate.is_relative_to(root),
                        "external_resource_path", path, f"External initializer {tensor.name!r} references a resource outside the selected model's bounded directory: {location!r}.")
                require(candidate.is_file(), "external_resource_missing", path,
                        f"External initializer {tensor.name!r} requires missing file {location!r} beside the model. No resource was downloaded or replaced.")
        require(not tensor.external_data and tensor.data_location != TensorProto.EXTERNAL,
                "external_initializer_unavailable", path,
                f"Initializer {tensor.name!r} declares external resources {[(x.key, x.value) for x in tensor.external_data]}. This host does not activate external tensor resources; supply an explicitly embedded model. Nothing is downloaded.")
        require(tensor.data_type == TensorProto.BOOL and len(tensor.dims) <= 1 and all(1 <= d <= 64 for d in tensor.dims),
                "initializer_unavailable", path,
                f"Found {TensorProto.DataType.Name(tensor.data_type)} shape {list(tensor.dims)} for {tensor.name!r}; this host covers embedded BOOL shapes [] and [1..64], preserving their rank.")
        try:
            array = numpy_helper.to_array(tensor)
            require(array.dtype.kind == "b" and array.shape == tuple(tensor.dims), "initializer_value", path,
                    "The ONNX decoder did not preserve the declared Boolean type and shape.")
        except (ValueError, TypeError, OverflowError) as error:
            raise ImportRefusal("initializer_value", path, f"{tensor.name}: {error}") from error
    for i, node in enumerate(graph.node):
        path = f"/graph/node/{i}"
        require(not node.domain and node.op_type in OPERATIONS, "operation_unavailable", path,
                f"Unsupported operator {node.domain or 'ai.onnx'}::{node.op_type} ({node.name}).")
        require(not node.attribute, "attribute_unavailable", path + "/attribute",
                "The admitted Boolean operators have no attributes or subgraphs.")
        # Match the actual operator schema at the model's opset, not a renamed
        # model or a permissive numeric maximum without an operator contract.
        schema = onnx.defs.get_schema(node.op_type, versions[0][1], "")
        require(schema.since_version == (1 if node.op_type == "Not" else 7) and
                len(node.input) == len(OPERATIONS[node.op_type][1]) and len(node.output) == 1,
                "operator_schema", path, "The selected version/arity has no qualified Boolean realization.")
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
    shapes = model_shapes(inferred)
    for i, node in enumerate(inferred.graph.node):
        input_shapes = [shapes[v] for v in node.input]
        arrays = {s for s in input_shapes if s}
        require(len(arrays) <= 1, "broadcast_unavailable", f"/graph/node/{i}",
                f"Shapes {input_shapes} need tensor-to-tensor broadcasting; the covered native profile supports equal rank-1 shapes or scalar-to-array broadcasting only.")
        expected = next(iter(arrays), ())
        require(shapes[node.output[0]] == expected, "shape_unavailable", f"/graph/node/{i}",
                "The inferred result shape differs from the exact native Boolean operation.")
    return inferred


def convert(model, digest, language="en"):
    layout_started = time.perf_counter()
    graph = model.graph
    title = graph.name or "ONNX Boolean graph"
    name(title, "/graph/name")
    inputs = {v.name for v in graph.input}
    initial = {}
    shapes = (graph.input, graph.output, graph.value_info)
    tensor_names = [item.name for group in shapes for item in group]
    tensor_names.extend(name for operation in graph.node for name in operation.output)
    note_width = 210
    lane_height = 110
    first_row = 96
    operations_x = 270
    shapes_by_name = model_shapes(model)
    depth = {v.name: 0 for v in graph.input}
    depth.update({v.name: 0 for v in graph.initializer})
    layers = {}
    for op in graph.node:
        level = 1 + max(depth[v] for v in op.input)
        depth[op.output[0]] = level
        layers.setdefault(level, []).append(op)
    output_x = operations_x + 132 * max(layers, default=0) + 80
    last_row = first_row + lane_height * (min(len(graph.node), len(inputs)) - 1)
    nodes, edges, widgets, mapping = [], [], [], []
    producers = {}
    prototypes = json.loads(Path(__file__).with_name("Import-WidgetPrototypes.json").read_text(encoding="utf-8"))
    positions = {v.name: first_row + lane_height * i for i, v in enumerate(graph.input)}
    for i, v in enumerate(graph.initializer):
        positions.setdefault(v.name, first_row + lane_height * (len(inputs) + i))
    for level, group in layers.items():
        group.sort(key=lambda op: (sum(positions[v] for v in op.input) / len(op.input), list(graph.node).index(op)))
        previous = 0
        for op in group:
            target = sum(positions[v] for v in op.input) / len(op.input)
            positions[op.output[0]] = max(first_row, target, previous + 90)
            previous = positions[op.output[0]]
    layout_ms = (time.perf_counter() - layout_started) * 1000

    def node(ident, kind, location, layout, **properties):
        nodes.append({"id": ident, "kind": kind, "layout": layout, **properties})
        mapping.append({"frog_id": ident, "source": location})

    def link(value, target, port, location):
        source, out = producers[value]
        ident = stable_id("wire", target + "/" + port)
        edge = {"id": ident, "from": {"node": source, "port": out},
                "to": {"node": target, "port": port}}
        # Long source identities are retained in the mapping, not repeated as
        # overlapping painted labels on every internal wire.
        edges.append(edge)
        mapping.append({"frog_id": ident, "source": location, "onnx_value": value, "type": "BOOL", "shape": list(shapes_by_name[value])})

    def widget(value, role, index, default=None):
        source = f"/graph/{'input' if role == 'control' else 'output'}/{index}"
        ident = stable_id(role, value)
        terminal = ident + "-value"
        note = UI[language][2 if role == "control" else 3] + value
        # Logical diagram lanes leave space for the native comments.
        x, y = ((120, first_row + lane_height * index) if role == "control"
                else (output_x, positions[value]))
        px, py = (40 if role == "control" else 340), 90 + 100 * index
        tensor_shape = shapes_by_name[value]
        declaration = copy.deepcopy(prototypes["boolean_array" if tensor_shape else "boolean_" + role])
        declaration.update(id=ident, role=role, label=value)
        declaration["props"].pop("data.default", None)
        declaration["props"].update({"label.visible": True, "caption.independent": True,
                                     "caption.text": note, "caption.visible": False})
        if tensor_shape:
            declaration["classId"] = "frog.widgets.array_" + role
            declaration["bounds"] = bounds(px, py, 182, 80)
            props = declaration["props"]
            props.update({"dimensions.allow_resize": False, "dimensions.shape": list(tensor_shape),
                          "element.role": role, "element.class_id": "frog.widgets.boolean_" + role,
                          "elements": [{"indices": [i], "value": bool(v)} for i, v in enumerate(default or [False] * tensor_shape[0])]})
            props["element.template"].update(id=ident+".element", role=role, classId="frog.widgets.boolean_" + role)
        else:
            declaration["bounds"] = bounds(px, py, declaration["bounds"]["width"], declaration["bounds"]["height"])
            declaration.setdefault("state", {})["value"] = bool(default)
            declaration["props"]["value"] = bool(default)
        declaration["props"]["data.default"] = json.dumps(copy.deepcopy(declaration), ensure_ascii=False)
        widgets.append(declaration)
        node(terminal, "widget_value", source, bounds(x, y, 66.3, 29.75), type="array:bool" if tensor_shape else "bool", widget=ident)
        node(terminal + "-note", "comment", source, bounds(x, y + 35, note_width, 32), hidden=True,
             text=note, comment_auto_size=False, vertical_arrangement="none",
             font={"family": "", "style": "plain", "point_size": 15,
                   "use_default_size": True, "geometry_initialized": False})
        mapping[-2].update(onnx_value=value, type="BOOL", shape=list(tensor_shape), widget=ident, suggestion=note)
        if role == "control":
            producers[value] = (terminal, "value")
            mapping[-2]["default_origin"] = "initializer" if value in initial else "editor_placeholder"
        else:
            link(value, terminal, "value", source)

    for i, tensor in enumerate(graph.initializer):
        path = f"/graph/initializer/{i}"
        name(tensor.name, path + "/name")
        initial[tensor.name] = numpy_helper.to_array(tensor).tolist()
        if tensor.name not in inputs:
            ident = stable_id("initializer", tensor.name)
            if tensor.dims:
                node(ident, "primitive", path, bounds(100, positions[tensor.name], 130, 60),
                     type="frog.studio.numeric.numeric_array_constant", value_type="bool", label=tensor.name, label_visible=True,
                     array_constant={"rank": 1, "shape": list(tensor.dims), "values": [str(v).lower() for v in initial[tensor.name]],
                                     "first_indices": [0], "visible_counts": [1], "one_dimensional_vertical": False,
                                     "cell_size": {"width": 46.2, "height": 46.2, "element_gap": 0}})
            else:
                token = str(initial[tensor.name]).lower()
                node(ident, "primitive", path, bounds(120, positions[tensor.name], 34, 24),
                     type=f"frog.studio.boolean.{token}_constant", value_type="bool",
                     text=token, label=tensor.name, label_visible=True)
            producers[tensor.name] = (ident, "value")
            mapping[-1].update(onnx_value=tensor.name, type="BOOL", shape=list(tensor.dims), value=initial[tensor.name])
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
        label = operation.name if operation.name and len(operation.name) <= 14 else operation.op_type + " " + str(i + 1)
        node(ident, "primitive", path, bounds(operations_x + 132 * (depth[output] - 1), positions[output], 46, 40),
             type=operation_type, label=label, label_visible=True)
        mapping[-1].update(onnx_name=operation.name, op_type=operation.op_type,
                           domain=operation.domain, opset=model.opset_import[0].version, attributes=[],
                           inputs=list(operation.input), outputs=list(operation.output), shape=list(shapes_by_name[output]))
        for input_index, (value, port) in enumerate(zip(operation.input, ports, strict=True)):
            link(value, ident, port, f"{path}/input/{input_index}")
        producers[output] = (ident, "result")
    for i, value in enumerate(graph.output):
        widget(value.name, "indicator", i)
    for role, text_value, x in (("inputs", UI[language][0], 40), ("outputs", UI[language][1], 340)):
        widgets.append({"id": stable_id("group", role), "classId": "frog.widgets.label", "role": "support",
                        "valueType": "support_text", "label": text_value, "bounds": bounds(x, 24, 220, 32),
                        "props": {"text.value": text_value, "label.visible": False}})

    document = {"format": "frog.document.draft", "draft_revision": 2, "language": "FROG",
                "title": title, "metadata": {"name": title, "description": f"ONNX IR {model.ir_version}, ai.onnx opset {model.opset_import[0].version}. SHA-256 {digest}."},
                "interface": {"inputs": [], "outputs": []},
                "frontPanel": {"canvas": {"coordinate_space": "panel_pixels", "width": 2 * note_width + 120,
                                          "height": 150 + 112 * max(len(graph.input), len(graph.output))},
                               "widgets": widgets},
                # The native producer omits a neutral viewport origin. Keeping
                # an explicit zero that normalization omits would later make
                # the lossless save guard treat camera movement as a conflict.
                "diagram": {"zoom_percent": 100,
                            "nodes": nodes, "edges": edges}}
    initializer_records = [
        {"name": item.name, "type": "BOOL", "shape": list(item.dims), "value": initial[item.name],
         "overridable": item.name in inputs, "source": f"/graph/initializer/{i}"}
        for i, item in enumerate(graph.initializer)
    ]
    receipt = {"onnx_sha256": digest, "onnx_library": onnx.__version__, "ir_version": model.ir_version,
               "opset_import": {x.domain: x.version for x in model.opset_import}, "graph": graph.name, "objects": mapping,
               "initializers": initializer_records, "compatibility": describe(model), "transformations": [],
               "shape_inference": "ONNX strict; original bytes retained", "language": language,
               "layout": "dependency ranks; native Studio cleanup measures final footprints and routes",
               "timings": {"initial_layout_ms": layout_ms}}
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


def analyze(data, resource_base=None):
    report = {"format": "frog.import.analysis.v1", "kind": "onnx", "sha256": hashlib.sha256(data).hexdigest(),
              "status": "out_of_profile", "diagnostics": [], "transformations": []}
    try:
        raw = onnx.load_model_from_string(data)
        report["content"] = describe(raw)
        model = load_model(data, resource_base)
        report.update(status="compatible", content=describe(model))
    except ImportRefusal as error:
        report["diagnostics"].append(error.diagnostic)
        report["status"] = "invalid" if error.diagnostic["code"] in ("onnx_import.decode", "onnx_import.checker") else "out_of_profile"
        if "content" in report and report["status"] != "invalid":
            # Independent compatibility facts can be reported together. Do not
            # infer secondary type errors from an invalid/missing intermediate.
            for i, n in enumerate(raw.graph.node):
                if n.domain or n.op_type not in OPERATIONS:
                    d = {"code": "onnx_import.operation_unavailable", "source": f"/graph/node/{i}",
                         "message": f"Unsupported operator {n.domain or 'ai.onnx'}::{n.op_type} ({n.name})."}
                    if not any(x["source"] == d["source"] for x in report["diagnostics"]): report["diagnostics"].append(d)
            for field in ("input", "output"):
                for i, v in enumerate(getattr(raw.graph, field)):
                    try: shape(v, f"/graph/{field}/{i}")
                    except ImportRefusal as problem:
                        if not any(x["source"] == problem.diagnostic["source"] for x in report["diagnostics"]): report["diagnostics"].append(problem.diagnostic)
    except (ValueError, DecodeError) as error:
        report.update(status="invalid", diagnostics=[{"code": "onnx_import.decode", "source": "/", "message": str(error)}])
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("onnx_file", type=Path)
    parser.add_argument("--frontend", type=Path)
    parser.add_argument("--output", type=Path)
    parser.add_argument("--analyze", action="store_true")
    parser.add_argument("--report", type=Path)
    parser.add_argument("--expected-sha256", default="")
    parser.add_argument("--language", choices=tuple(UI), default="en")
    args = parser.parse_args()
    if not args.analyze and (args.frontend is None or args.output is None):
        parser.error("--frontend and --output are required for conversion")
    output = args.output.resolve() if args.output else None
    try:
        require(output is None or not output.exists(), "destination_exists", "/", "The destination must be new.")
        start = time.perf_counter()
        with args.onnx_file.open("rb") as stream:
            data = stream.read(MAX_MODEL_BYTES + 1)
        require(len(data) <= MAX_MODEL_BYTES, "model_budget", "/", "Maximum ONNX size is 1 MiB.")
        if args.analyze:
            report = analyze(data, args.onnx_file.parent)
            if args.report: args.report.write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
            print(json.dumps(report, ensure_ascii=False))
            return 0
        digest = hashlib.sha256(data).hexdigest()
        require(not args.expected_sha256 or digest == args.expected_sha256,
                "source_changed", "/", "The selected source differs from its analyzed bytes. Analyze the file again.")
        model = load_model(data, args.onnx_file.parent)
        checked = time.perf_counter()
        draft, receipt = convert(model, digest, args.language)
        converted = time.perf_counter()
        output.parent.mkdir(parents=True, exist_ok=True)
        stage = Path(tempfile.mkdtemp(prefix=".onnx-import-", dir=output.parent))
        try:
            for filename, value in (("model.frog", draft), ("onnx-mapping.json", receipt)):
                (stage / filename).write_text(json.dumps(value, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
            (stage / "original.onnx").write_bytes(data)
            validate(args.frontend.resolve(strict=True), stage)
            validated = time.perf_counter()
            receipt["timings"].update(analysis_ms=(checked-start)*1000, conversion_ms=(converted-checked)*1000,
                                      frontend_ms=(validated-converted)*1000)
            (stage / "onnx-mapping.json").write_text(json.dumps(receipt, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
            report = analyze(data, args.onnx_file.parent)
            report.update(frontend_admitted=True, objects=len(draft["diagram"]["nodes"]), wires=len(draft["diagram"]["edges"]))
            (stage / "import-analysis.json").write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
            require(hashlib.sha256(args.onnx_file.read_bytes()).hexdigest() == digest,
                    "source_changed", "/", "The selected model changed during preparation. Analyze the current file again; no result was published.")
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
