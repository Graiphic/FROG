"""First bounded, semantically admitted producer of canonical open Execution IR.

This is distinct from frog_fir_unit and from the private runtime artifact. It
accepts natural f64 widget values and two-input frog.core.add in an acyclic,
single root scope with no public interface. It neither lowers nor executes.
Unsupported families are diagnosed before construction, never flattened.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import math
import re
import sys
from collections import deque
from dataclasses import dataclass
from pathlib import Path
from typing import Any

REPOSITORY_ROOT = Path(__file__).resolve().parents[3]
if str(REPOSITORY_ROOT) not in sys.path:
    sys.path.insert(0, str(REPOSITORY_ROOT))

# Reuse the strict reference reader's duplicate-member and number policies;
# read bytes once here so the identity includes original CRLF and UTF-8 bytes.
from Implementations.Reference.common import (
    FrogPipelineError, _unique_json_members, _reject_non_json_number,
    _finite_json_float, _exact_json_integer,
)
from Implementations.Reference.Validator.source_envelope import validate_envelope

SUBSET = "frog.canonical.widget-f64-add.v1"
PRODUCER = "frog.reference.canonical-execution-ir"
ID_PATTERN = re.compile(r"^[A-Za-z0-9._:-]+$")
NUMERIC_CLASSES = {
    "frog.ui.standard.numeric_control": "control",
    "frog.ui.standard.numeric_indicator": "indicator",
    "frog.widgets.numeric_control": "control",
    "frog.widgets.numeric_indicator": "indicator",
}


@dataclass(frozen=True)
class CanonicalIRLimits:
    """Implementation admission budgets, not limits of the FROG language."""
    max_source_bytes: int = 8 * 1024 * 1024
    max_nodes: int = 1024
    max_edges: int = 4096
    max_widgets: int = 1024
    max_id_bytes: int = 1024


def _limits(limits: CanonicalIRLimits) -> None:
    ceilings = vars(CanonicalIRLimits())
    if not isinstance(limits, CanonicalIRLimits) or any(
        type(value) is not int or value <= 0 or value > ceilings[name]
        for name, value in vars(limits).items()
    ):
        _reject("invalid_admission_limits", "Admission budgets must be positive integers within the declared producer ceilings.", "/", unsupported=True)


def _reject(code: str, message: str, location: str, *, unsupported: bool = False) -> None:
    raise FrogPipelineError(
        stage="semantic_validation", error_code=code, message=message,
        diagnostics=[{"code": code, "severity": "error", "message": message,
                      "location": location, "owner": "source-to-FIR boundary",
                      "status": "unsupported" if unsupported else "rejected"}],
    )


def _object(value: Any, location: str) -> dict[str, Any]:
    if not isinstance(value, dict):
        _reject("invalid_object", "Expected a source object.", location)
    return value


def _array(value: Any, location: str) -> list[Any]:
    if not isinstance(value, list):
        _reject("invalid_array", "Expected a source array.", location)
    return value


def _identifier(value: Any, location: str, limits: CanonicalIRLimits) -> str:
    if isinstance(value, str) and len(value) > limits.max_id_bytes:
        _reject("source_resource_limit", "A source identity exceeds the admitted UTF-8 byte budget.", location, unsupported=True)
    if not isinstance(value, str) or not value or any(0xD800 <= ord(c) <= 0xDFFF for c in value):
        _reject("invalid_source_id", "An identity must be a nonempty Unicode scalar string.", location)
    if len(value.encode("utf-8")) > limits.max_id_bytes:
        _reject("source_resource_limit", "A source identity exceeds the admitted UTF-8 byte budget.", location, unsupported=True)
    return value


def _collection(value: Any, location: str, maximum: int) -> list[Any]:
    values = _array(value, location)
    if len(values) > maximum:
        _reject("source_resource_limit", "The source collection exceeds this producer's admitted work budget.", location, unsupported=True)
    return values


def _fields(value: dict[str, Any], allowed: set[str], location: str) -> None:
    unknown = sorted(set(value) - allowed)
    if unknown:
        _reject("unsupported_source_field", f"Fields outside the admitted subset: {', '.join(unknown)}.",
                location, unsupported=True)


def _ir_id(category: str, source_id: str) -> str:
    # Source IDs have broader syntax than canonical IR IDs. This injective
    # encoding retains arbitrary source identity without array-position IDs.
    return f"{category}:{source_id.encode('utf-8').hex()}"


@dataclass(frozen=True)
class _Port:
    id: str
    direction: str
    role: str
    type_ref: str = "f64"


@dataclass(frozen=True)
class _Node:
    id: str
    pointer: str
    family: str
    kind: str
    ports: tuple[_Port, ...]
    widget_id: str | None = None
    role: str | None = None


@dataclass(frozen=True)
class _Edge:
    id: str
    pointer: str
    from_node: str
    from_port: str
    to_node: str
    to_port: str


@dataclass(frozen=True)
class _Meaning:
    source_uri: str
    source_hash: str
    document: dict[str, Any]
    nodes: tuple[_Node, ...]
    edges: tuple[_Edge, ...]
    widgets: tuple[tuple[str, str, dict[str, Any]], ...]


def _read_source(path: Path, limits: CanonicalIRLimits) -> tuple[dict[str, Any], str]:
    try:
        # No unbounded read_bytes allocation. The extra byte distinguishes an
        # exact-boundary source from a source exceeding the admission budget.
        with path.open("rb") as stream:
            data = stream.read(limits.max_source_bytes + 1)
        if len(data) > limits.max_source_bytes:
            _reject("source_resource_limit", "Source bytes exceed this producer's admitted byte budget.", "/", unsupported=True)
        text = data.decode("utf-8", errors="strict")
        document = json.loads(text, object_pairs_hook=_unique_json_members,
                              parse_constant=_reject_non_json_number,
                              parse_float=_finite_json_float, parse_int=_exact_json_integer)
    except RecursionError:
        _reject("source_resource_limit", "Source nesting exceeds this reader's admitted capacity.", "/", unsupported=True)
    except (OSError, UnicodeError, json.JSONDecodeError) as exc:
        raise FrogPipelineError("load", "invalid_source", f"Cannot read strict UTF-8 JSON: {exc}") from exc
    _object(document, "/")
    return document, "sha256:" + hashlib.sha256(data).hexdigest()


def _validate(path: Path, source_uri: str, limits: CanonicalIRLimits) -> _Meaning:
    document, source_hash = _read_source(path, limits)
    status, diagnostics = validate_envelope(document)
    if status != "ok":
        raise FrogPipelineError("semantic_validation", diagnostics[0]["code"],
                                diagnostics[0]["message"], diagnostics)

    interface = document["interface"]
    _fields(interface, {"inputs", "outputs"}, "/interface")
    for direction in ("inputs", "outputs"):
        if _array(interface.get(direction, []), f"/interface/{direction}"):
            _reject("unsupported_public_interface", "Public ports require a real boundary adapter; they are not widget values.",
                    f"/interface/{direction}", unsupported=True)
    # These policies have downstream consequences not carried by this first
    # open-IR corridor. Their presence is not silently mistaken for support UI.
    for section in ("host", "execution_policy"):
        if document.get(section):
            _reject("unsupported_source_policy", f"{section} is not admitted by this subset.",
                    f"/{section}", unsupported=True)

    panel = _object(document.get("front_panel", {}), "/front_panel")
    _fields(panel, {"canvas", "widgets"}, "/front_panel")
    widgets: dict[str, tuple[str, dict[str, Any]]] = {}
    for index, value in enumerate(_collection(panel.get("widgets", []), "/front_panel/widgets", limits.max_widgets)):
        pointer = f"/front_panel/widgets/{index}"
        widget = _object(value, pointer)
        _fields(widget, {"id", "role", "widget", "class_ref", "value_type", "layout", "props", "parts", "doc", "tags"}, pointer)
        widget_id = _identifier(widget.get("id"), pointer + "/id", limits)
        if widget_id in widgets:
            _reject("duplicate_widget_id", f"Duplicate widget identity {widget_id!r}.", pointer)
        class_id = widget.get("widget", widget.get("class_ref"))
        if not isinstance(class_id, str) or class_id not in NUMERIC_CLASSES:
            _reject("unsupported_widget_class", "Only the published numeric control/indicator classes are admitted.", pointer, unsupported=True)
        if "widget" in widget and "class_ref" in widget and widget["widget"] != widget["class_ref"]:
            _reject("conflicting_widget_class", "widget and class_ref must identify the same class.", pointer)
        if widget.get("role") != NUMERIC_CLASSES[class_id]:
            _reject("incompatible_widget_role", "The widget role is incompatible with its class.", pointer)
        if widget.get("value_type") != "f64":
            _reject("unsupported_value_type", "This first corridor admits f64 widget values only.", pointer + "/value_type", unsupported=True)
        props = _object(widget.get("props", {}), pointer + "/props")
        _fields(props, {"default_value", "data_type.representation"}, pointer + "/props")
        if props.get("data_type.representation", "f64") != "f64":
            _reject("conflicting_value_type", "The representation conflicts with the declared f64 value type.", pointer + "/props")
        if widget["role"] == "control" and "default_value" not in props:
            _reject("unsupported_implicit_widget_default", "An admitted control must declare its default value; no default is invented.",
                    pointer + "/props", unsupported=True)
        if "default_value" in props:
            number = props["default_value"]
            try:
                representable = type(number) in (int, float) and math.isfinite(float(number))
                if type(number) is int:
                    representable = representable and int(float(number)) == number
            except (OverflowError, ValueError):
                representable = False
            if not representable:
                _reject("invalid_f64_default", "The default must be a finite, representable f64 number, not a Boolean or string.", pointer + "/props/default_value")
        parts = _object(widget.get("parts", {}), pointer + "/parts")
        _fields(parts, {"label", "caption"}, pointer + "/parts")
        for name, part in parts.items():
            member_pointer = pointer + "/parts/" + name
            part = _object(part, member_pointer)
            _fields(part, {"class", "class_ref", "props", "layout"}, member_pointer)
            part_class = part.get("class", part.get("class_ref"))
            if not isinstance(part_class, str) or part_class not in {"frog.ui.standard.label_part", "frog.widgets.label"}:
                _reject("unsupported_widget_part", "Only the published label/caption parts are admitted.", member_pointer, unsupported=True)
            if "class" in part and "class_ref" in part and part["class"] != part["class_ref"]:
                _reject("conflicting_part_class", "class and class_ref must identify the same part class.", member_pointer)
            part_props = _object(part.get("props", {}), member_pointer + "/props")
            _fields(part_props, {"text", "visible", "style"}, member_pointer + "/props")
            if "text" in part_props and not isinstance(part_props["text"], str):
                _reject("invalid_part_text", "Label/caption text must be a string.", member_pointer)
            if "visible" in part_props and type(part_props["visible"]) is not bool:
                _reject("invalid_part_visibility", "Label/caption visibility must be Boolean.", member_pointer)
        widgets[widget_id] = (pointer, widget)

    diagram = document["diagram"]
    _fields(diagram, {"nodes", "edges", "annotations", "wire_fragments"}, "/diagram")
    if _array(diagram.get("wire_fragments", []), "/diagram/wire_fragments"):
        _reject("incomplete_wire_fragments", "Incomplete wires have no accepted executable meaning.", "/diagram/wire_fragments")
    nodes: dict[str, _Node] = {}
    participants: set[str] = set()
    # Read the actual published signature rather than a per-example port table.
    manifest = json.loads((REPOSITORY_ROOT / "Libraries/Manifests/frog.core.add.v1.json").read_text(encoding="utf-8"))
    signature = manifest.get("ports", {})
    if (manifest.get("definition_id") != "frog.core.add" or signature.get("required_inputs") != ["a", "b"]
            or signature.get("output") != "result" or signature.get("optional_input_first_index") != 3):
        _reject("unsupported_primitive_signature", "The published add signature differs from this corridor's proven contract.", "/diagram", unsupported=True)
    raw_nodes = _collection(diagram.get("nodes"), "/diagram/nodes", limits.max_nodes)
    raw_edges = _collection(diagram.get("edges"), "/diagram/edges", limits.max_edges)
    for index, value in enumerate(raw_nodes):
        pointer = f"/diagram/nodes/{index}"
        node = _object(value, pointer)
        node_id = _identifier(node.get("id"), pointer + "/id", limits)
        if node_id in nodes:
            _reject("duplicate_node_id", f"Duplicate node identity {node_id!r}.", pointer)
        kind = node.get("kind")
        if not isinstance(kind, str):
            _reject("invalid_node_kind", "Every node must declare a string family identity.", pointer + "/kind")
        if kind not in {"widget_value", "primitive"}:
            _reject("unsupported_node_family", f"Node family {kind!r} is not implemented by this corridor.", pointer, unsupported=True)
        _fields(node, {"id", "kind", "layout", "doc", "tags", "type" if kind == "primitive" else "widget"}, pointer)
        if kind == "primitive":
            if not isinstance(node.get("type"), str) or not node["type"]:
                _reject("invalid_primitive_identity", "A primitive must declare its canonical identity.", pointer + "/type")
            if node.get("type") != manifest["definition_id"]:
                _reject("unsupported_primitive", "Only frog.core.add is admitted; other primitives retain explicit unsupported diagnostics.", pointer, unsupported=True)
            ports = tuple(_Port(port, "in", "data") for port in signature["required_inputs"]) + (_Port(signature["output"], "out", "data"),)
            nodes[node_id] = _Node(node_id, pointer, "primitive", node["type"], ports)
        else:
            widget_id = _identifier(node.get("widget"), pointer + "/widget", limits)
            if widget_id not in widgets:
                _reject("unknown_widget", "A widget_value must reference an actual declared widget.", pointer + "/widget")
            if widget_id in participants:
                _reject("unsupported_repeated_widget_participation", "Multiple natural participation objects for one widget are not admitted yet.", pointer, unsupported=True)
            participants.add(widget_id)
            role = widgets[widget_id][1]["role"]
            nodes[node_id] = _Node(node_id, pointer, "widget_value", "widget_value",
                                  (_Port("value", "out" if role == "control" else "in", "data"),), widget_id, role)
    if not nodes:
        _reject("unsupported_empty_program", "This corridor requires actual executable participation, not an empty schema fixture.", "/diagram/nodes", unsupported=True)

    ports = {(node.id, port.id): port for node in nodes.values() for port in node.ports}
    edges: dict[str, _Edge] = {}
    drivers: set[tuple[str, str]] = set()
    adjacency: dict[str, list[str]] = {node_id: [] for node_id in nodes}
    indegree = {node_id: 0 for node_id in nodes}
    for index, value in enumerate(raw_edges):
        pointer = f"/diagram/edges/{index}"
        edge = _object(value, pointer)
        _fields(edge, {"id", "from", "to", "bends"}, pointer)
        edge_id = _identifier(edge.get("id"), pointer + "/id", limits)
        if edge_id in edges:
            _reject("duplicate_edge_id", f"Duplicate edge identity {edge_id!r}.", pointer)
        endpoints = []
        for side, direction in (("from", "out"), ("to", "in")):
            endpoint = _object(edge.get(side), pointer + "/" + side)
            _fields(endpoint, {"node", "port"}, pointer + "/" + side)
            node_id = _identifier(endpoint.get("node"), pointer + "/" + side + "/node", limits)
            port_id = _identifier(endpoint.get("port"), pointer + "/" + side + "/port", limits)
            if node_id not in nodes:
                _reject("unknown_edge_node", "The endpoint's node does not exist in this scope.", pointer + "/" + side)
            if (node_id, port_id) not in ports:
                optional = re.fullmatch(r"input_([1-9][0-9]*)", port_id)
                if nodes[node_id].kind == "frog.core.add" and optional and (len(optional[1]) > 1 or optional[1] in "3456789"):
                    _reject("unsupported_variadic_add", "Connected optional add operands need a separate admitted variadic corridor.", pointer + "/" + side, unsupported=True)
                _reject("unknown_edge_port", "The endpoint's port is not in the resolved signature.", pointer + "/" + side)
            if ports[node_id, port_id].direction != direction:
                _reject("invalid_edge_direction", "Edges must flow from an output to an input.", pointer + "/" + side)
            endpoints.append((node_id, port_id))
        if ports[endpoints[0]].type_ref != ports[endpoints[1]].type_ref:
            _reject("edge_type_mismatch", "The resolved endpoint types are incompatible.", pointer)
        if endpoints[1] in drivers:
            _reject("multiple_input_drivers", "Each input accepts at most one ordinary driver.", pointer)
        drivers.add(endpoints[1])
        adjacency[endpoints[0][0]].append(endpoints[1][0])
        indegree[endpoints[1][0]] += 1
        edges[edge_id] = _Edge(edge_id, pointer, *endpoints[0], *endpoints[1])
    for endpoint, port in ports.items():
        if port.direction == "in" and endpoint not in drivers:
            _reject("missing_input_driver", "An admitted primitive/indicator input must have one real driver.", nodes[endpoint[0]].pointer)
    pending = deque(node_id for node_id, degree in indegree.items() if degree == 0)
    visited = 0
    while pending:
        node_id = pending.popleft()
        visited += 1
        for destination in adjacency[node_id]:
            indegree[destination] -= 1
            if indegree[destination] == 0:
                pending.append(destination)
    if visited != len(nodes):
        _reject("combinational_cycle", "A cycle cannot be repaired through hidden memory or scheduling.", "/diagram/edges")
    return _Meaning(source_uri, source_hash, document, tuple(nodes.values()), tuple(edges.values()),
                    tuple((wid, pointer, widget) for wid, (pointer, widget) in widgets.items()))


def _construct(meaning: _Meaning, document_id: str) -> dict[str, Any]:
    source_ref = lambda pointer: meaning.source_uri + "#" + pointer
    objects, connections, source_map, correspondence = [], [], [], []
    widget_objects: dict[str, list[str]] = {wid: [] for wid, _, _ in meaning.widgets}

    def mapping(record_id, source_kind, pointer, target_kind, targets, relation="direct_primary"):
        source_map.append({"id": record_id, "source_kind": source_kind, "source_ref": source_ref(pointer),
                           "target_kind": target_kind, "target_refs": targets, "relation": relation})

    def non_primary(record_id, pointer, targets=None, relation="non_primary_outcome", metadata=None):
        record = {"id": record_id, "source_ref": source_ref(pointer), "relation_kind": relation,
                  "target_refs": targets or []}
        if metadata:
            record["metadata"] = metadata
        correspondence.append(record)

    widgets = {wid: widget for wid, _, widget in meaning.widgets}
    for node in meaning.nodes:
        object_id = _ir_id("node", node.id)
        attrs = {"source_node_id": node.id}
        refs = [source_ref(node.pointer)]
        corr_refs = []
        if node.widget_id:
            widget = widgets[node.widget_id]
            attrs.update(widget_id=node.widget_id, widget_class=widget.get("widget", widget.get("class_ref")), value_type="f64")
            if "default_value" in widget.get("props", {}):
                attrs["default_value"] = widget["props"]["default_value"]
            widget_pointer = next(pointer for wid, pointer, _ in meaning.widgets if wid == node.widget_id)
            refs.append(source_ref(widget_pointer))
            corr_refs.append(_ir_id("declaration", node.widget_id))
            widget_objects[node.widget_id].append(object_id)
        record = {"id": object_id, "family": node.family, "kind": node.kind,
                  "classification": {"primary_or_support": "primary"},
                  "ports": [{"id": p.id, "direction": p.direction, "role": p.role, "type_ref": p.type_ref,
                             "boundary_kind": "none", "metadata": {"source_node_id": node.id, "source_port_id": p.id}}
                            for p in node.ports],
                  "attributes": attrs, "attribution": {"source_refs": refs, "mode": "direct" if len(refs) == 1 else "multi_contributor"},
                  "correspondence_refs": corr_refs}
        if node.role:
            record["role"] = node.role
        if node.family == "primitive":
            record["classification"]["library_id"] = "frog.core"
        objects.append(record)
        mapping(_ir_id("map-node", node.id), "diagram_node", node.pointer, "object", [object_id])
        raw_node = next(n for n in meaning.document["diagram"]["nodes"] if n["id"] == node.id)
        for field in ("layout", "doc", "tags"):
            if field in raw_node:
                non_primary(_ir_id("node-" + field, node.id), node.pointer + "/" + field)
    for edge in meaning.edges:
        connection_id = _ir_id("edge", edge.id)
        connections.append({"id": connection_id, "kind": "dataflow",
                            "from": {"object_id": _ir_id("node", edge.from_node), "port_id": edge.from_port},
                            "to": {"object_id": _ir_id("node", edge.to_node), "port_id": edge.to_port},
                            "attribution": {"source_refs": [source_ref(edge.pointer)], "mode": "direct"},
                            "metadata": {"source_edge_id": edge.id, "type_ref": "f64"}})
        mapping(_ir_id("map-edge", edge.id), "diagram_edge", edge.pointer, "connection", [connection_id])
        raw_edge = next(e for e in meaning.document["diagram"]["edges"] if e["id"] == edge.id)
        if "bends" in raw_edge:
            non_primary(_ir_id("edge-bends", edge.id), edge.pointer + "/bends")
    for wid, pointer, widget in meaning.widgets:
        targets = widget_objects[wid]
        mapping(_ir_id("map-widget", wid), "widget_instance", pointer, "object" if targets else "none", targets,
                "non_primary")
        non_primary(_ir_id("declaration", wid), pointer, targets, "declaration_reference", {"widget_id": wid})
        class_field = "widget" if "widget" in widget else "class_ref"
        non_primary(_ir_id("widget-class", wid), pointer + "/" + class_field, targets,
                    "declaration_reference", {"class_id": widget[class_field]})
        if "default_value" in widget.get("props", {}):
            mapping(_ir_id("map-default", wid), "widget_member", pointer + "/props/default_value",
                    "object" if targets else "none", targets, "direct_primary" if targets else "non_primary")
        for field in ("layout", "parts", "doc", "tags"):
            if field in widget:
                non_primary(_ir_id("widget-" + field, wid), pointer + "/" + field)
    for section in ("connector", "icon", "ide", "cache"):
        if section in meaning.document:
            non_primary("section:" + section, "/" + section)
    if "canvas" in meaning.document.get("front_panel", {}):
        non_primary("panel:canvas", "/front_panel/canvas")
    if "annotations" in meaning.document["diagram"]:
        non_primary("diagram:annotations", "/diagram/annotations")
    result = {"schema_family": "frog.execution_ir", "schema_version": "0.1", "document_id": document_id,
              "metadata": {"producer": PRODUCER, "producer_version": "1", "notes": [
                  "Accepted subset: " + SUBSET, "Exact source bytes: " + meaning.source_hash,
                  "Source URI: " + meaning.source_uri, "Open IR only; lowering and normal Studio execution are downstream."]},
              "unit": {"id": "unit:main", "objects": objects, "connections": connections, "regions": [],
                       "source_map": source_map, "correspondence": correspondence}}
    check_constructed_ir(result)
    return result


def check_constructed_ir(artifact: dict[str, Any]) -> None:
    """Post-construction identity/endpoint checks; not a JSON Schema substitute."""
    def fail(message):
        raise FrogPipelineError("canonical_ir_construction", "invalid_constructed_ir", message)
    if artifact.get("schema_family") != "frog.execution_ir" or artifact.get("schema_version") != "0.1":
        fail("The artifact is not the canonical open IR family/version.")
    unit = artifact.get("unit", {})
    ids = {artifact.get("document_id"), unit.get("id")}
    if len(ids) != 2:
        fail("Document and execution-unit identities must be distinct.")
    objects = {}
    ports = {}
    for record in unit.get("objects", []):
        if record["id"] in ids:
            fail("An execution identity is duplicated.")
        ids.add(record["id"])
        objects[record["id"]] = record
        if record["family"] not in {"widget_value", "primitive"} or not record.get("attribution", {}).get("source_refs"):
            fail("An execution object lost its admitted family or attribution.")
        expected = {"value": "out" if record.get("role") == "control" else "in"} if record["family"] == "widget_value" else {"a": "in", "b": "in", "result": "out"}
        if record["kind"] != ("widget_value" if record["family"] == "widget_value" else "frog.core.add"):
            fail("An execution object lost its primitive/participation identity.")
        if record["family"] == "widget_value" and record.get("role") not in {"control", "indicator"}:
            fail("Widget-value direction must come from the declared role.")
        if len(record["ports"]) != len(expected):
            fail("An execution object has the wrong admitted signature.")
        for port in record["ports"]:
            key = (record["id"], port["id"])
            if key in ports or expected.get(port["id"]) != port["direction"] or port.get("type_ref") != "f64":
                fail("An execution port lost its identity, direction or resolved type.")
            ports[key] = port
    connections = {}
    drivers = set()
    adjacency = {object_id: [] for object_id in objects}
    indegree = {object_id: 0 for object_id in objects}
    for edge in unit.get("connections", []):
        if edge["id"] in ids or edge["kind"] != "dataflow" or not edge.get("attribution", {}).get("source_refs"):
            fail("A connection lost its identity, category or attribution.")
        ids.add(edge["id"])
        connections[edge["id"]] = edge
        ends = []
        for side, direction in (("from", "out"), ("to", "in")):
            key = (edge[side]["object_id"], edge[side]["port_id"])
            if key not in ports or ports[key]["direction"] != direction:
                fail("A connection endpoint does not resolve to the correct real port.")
            ends.append(key)
        if ends[1] in drivers:
            fail("A constructed input has multiple drivers.")
        drivers.add(ends[1])
        adjacency[ends[0][0]].append(ends[1][0])
        indegree[ends[1][0]] += 1
    if any(port["direction"] == "in" and key not in drivers for key, port in ports.items()):
        fail("An admitted input lost its connection during construction.")
    pending = deque(object_id for object_id, degree in indegree.items() if degree == 0)
    visited = 0
    while pending:
        object_id = pending.popleft()
        visited += 1
        for destination in adjacency[object_id]:
            indegree[destination] -= 1
            if indegree[destination] == 0:
                pending.append(destination)
    if visited != len(objects):
        fail("A combinational cycle appeared during construction.")
    mapping_targets = {"object": objects, "connection": connections, "none": {}}
    primary_maps = {"object": set(), "connection": set()}
    for mapping in unit.get("source_map", []):
        if mapping["id"] in ids:
            fail("A source-map identity is duplicated.")
        ids.add(mapping["id"])
        target_kind = mapping["target_kind"]
        if target_kind not in mapping_targets or not mapping.get("source_ref"):
            fail("A source map lost its category or source attribution.")
        if any(target not in mapping_targets[target_kind] for target in mapping["target_refs"]):
            fail("A source map points outside the constructed unit.")
        if mapping["source_kind"] in {"diagram_node", "diagram_edge"}:
            expected_kind = "object" if mapping["source_kind"] == "diagram_node" else "connection"
            if target_kind != expected_kind or len(mapping["target_refs"]) != 1 or mapping.get("relation") != "direct_primary":
                fail("A primary contributor lost its direct one-to-one mapping.")
            primary_maps[target_kind].update(mapping["target_refs"])
    if primary_maps["object"] != set(objects) or primary_maps["connection"] != set(connections):
        fail("Primary objects or connections lost their direct source mapping.")
    corr_ids = set()
    for relation in unit.get("correspondence", []):
        if relation["id"] in ids or not relation.get("source_ref") or any(target not in objects for target in relation.get("target_refs", [])):
            fail("A correspondence lost identity, source or target recoverability.")
        ids.add(relation["id"])
        corr_ids.add(relation["id"])
    for record in objects.values():
        if any(ref not in corr_ids for ref in record.get("correspondence_refs", [])):
            fail("A widget participation lost its declaration correspondence.")


def derive_canonical_ir(source_path: str | Path, *, document_id: str, source_uri: str | None = None,
                        limits: CanonicalIRLimits = CanonicalIRLimits()) -> dict[str, Any]:
    """Admit actual source bytes, then construct one canonical JSON document.

    document_id is a stable caller-owned IR artifact identity, not metadata.name,
    a UI handle, cache entry, or runtime activation identity.
    """
    _limits(limits)
    if isinstance(document_id, str) and len(document_id) > limits.max_id_bytes:
        _reject("source_resource_limit", "The caller's IR identity exceeds the admitted identity budget.", "/document_id", unsupported=True)
    if not isinstance(document_id, str) or not ID_PATTERN.fullmatch(document_id) or document_id == "unit:main":
        raise FrogPipelineError("canonical_ir_construction", "invalid_document_id", "An IR document ID must be a valid, distinct canonical identifier.")
    path = Path(source_path)
    uri = source_uri if source_uri is not None else path.resolve().as_uri()
    if not isinstance(uri, str) or not uri or "#" in uri:
        raise FrogPipelineError("load", "invalid_source_uri", "The source URI must be nonempty and have no fragment.")
    return _construct(_validate(path, uri, limits), document_id)


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", type=Path)
    parser.add_argument("--document-id", required=True)
    parser.add_argument("--output", type=Path)
    parser.add_argument("--max-source-bytes", type=int, default=CanonicalIRLimits().max_source_bytes,
                        help="Implementation byte admission budget; the language itself has no such limit.")
    args = parser.parse_args(argv)
    try:
        artifact = derive_canonical_ir(args.source, document_id=args.document_id,
                                       limits=CanonicalIRLimits(max_source_bytes=args.max_source_bytes))
    except FrogPipelineError as exc:
        print(json.dumps(exc.as_dict(), ensure_ascii=False), file=sys.stderr)
        return 2
    text = json.dumps(artifact, ensure_ascii=False, indent=2, allow_nan=False) + "\n"
    if args.output:
        args.output.write_text(text, encoding="utf-8", newline="\n")
    else:
        print(text, end="")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
