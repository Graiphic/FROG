# SPDX-License-Identifier: Apache-2.0
"""Resolve decoded LabVIEW heap objects into an attributed source graph."""

from dataclasses import dataclass
import math

from pylabview import LVheap, LVparts

from import_labview_profile import (
    TYPES, Types, constant_value, field, heap_root, identity, initial_value, items,
    kind, number, one_section, require, text,
)


@dataclass(frozen=True)
class Widget:
    uid: int
    label: str
    dtype: str
    role: str
    bounds: tuple[int, int, int, int]
    value: object


@dataclass(frozen=True)
class Terminal:
    uid: int
    kind: str
    diagram: int
    owner: int
    flags: int
    dtype: str
    binding_kind: str
    binding_uid: int
    bounds: tuple[int, int, int, int] | None
    value: object
    binding_flags: int
    parameter: int | None
    override: int | None

    @property
    def source(self):
        return not bool(self.flags & 0x8000)


@dataclass(frozen=True)
class Node:
    uid: int
    kind: str
    diagram: int
    bounds: tuple[int, int, int, int] | None
    terms: tuple[int, ...]
    regions: tuple[int, ...]
    frames: tuple[int, ...]
    primitive: tuple[int, int] | None
    flags: int
    operand_bindings: tuple[int, ...]
    result_binding: int | None


@dataclass(frozen=True)
class Boundary:
    uid: int
    kind: str
    owner: int
    inner: int
    outer: int | None
    dtype: str
    mate: int | None
    bounds: tuple[int, int, int, int] | None
    flags: int


@dataclass(frozen=True)
class Signal:
    uid: int
    source: int
    targets: tuple[int, ...]


@dataclass(frozen=True)
class Diagram:
    uid: int
    nodes: tuple[int, ...]
    signals: tuple[Signal, ...]


@dataclass(frozen=True)
class Graph:
    root: int
    diagrams: dict[int, Diagram]
    nodes: dict[int, Node]
    terms: dict[int, Terminal]
    boundaries: dict[int, Boundary]
    widgets: dict[int, Widget]


STRUCTURES = {"flatSequence", "sequenceFrame", "forLoop"}
BOUNDARIES = {"flatSeqTun", "lMax", "lCnt", "lSR", "rSR", "lpTun"}
TERM_BINDINGS = BOUNDARIES | {"fPDCO", "bDConstDCO", "parm", "overridableParm", "aBuildDCO"}


def members(node, name, location, optional=False):
    found = field(node, name, location, optional=optional)
    return items(found, location) if found is not None else []


def flags(node, location):
    return optional_number(node, "objFlags", location) or 0


def optional_number(node, name, location):
    return number(node, name, location) if field(node, name, location, optional=True) is not None else None


def rect(node, name, location, optional=False):
    found = field(node, name, location, optional=optional)
    if found is None:
        return None
    require(isinstance(found, LVheap.HeapNodeRect) and found.format == "inline",
            "heap_bounds", f"'{name}' is not a decoded rectangle.", location)
    # RSRC heap rectangles store row before column. The upstream attribute names
    # follow field order; translate them to horizontal and vertical coordinates.
    return found.top, found.left, found.bottom, found.right


class HeapTypes(Types):
    def descriptor(self, dtype, location):
        family = dtype.fullType().name if dtype is not None else "unknown"
        if family == "Array":
            require(len(dtype.dimensions) == 1 and len(dtype.clients) == 1 and
                    dtype.dimensions[0].flags == 255 and dtype.dimensions[0].fixedSize == 0xFFFFFF,
                    "array_type", "The source array is not a dynamic rank-one array.", location)
            item = self.consolidated.getFlatType(dtype.clients[0].index)
            family = item.fullType().name if item is not None else "unknown"
            require(family in TYPES, "array_type", f"Unsupported array element type '{family}'.", location)
            return "array:" + TYPES[family]
        require(family in TYPES, "unsupported_type", f"Unsupported source type '{family}'.", location)
        return TYPES[family]

    def of(self, descriptor, resource):
        heap_id = number(descriptor, "typeDesc", resource)
        require(1 <= heap_id <= self.slice.tdCount, "type_reference", "Heap TypeID is out of range.", resource)
        return self.descriptor(self.consolidated.getTopType(self.slice.indexShift + heap_id - 1), resource)


def implicit_value(dtype):
    return [] if dtype.startswith("array:") else initial_value(dtype)


def data_value(data, dtype, types, location):
    require(types.descriptor(data.td, location) == dtype, "default_type",
            "The stored default type disagrees with the panel type.", location)
    if dtype.startswith("array:"):
        require(len(data.dimensions) == 1 and data.dimensions[0] == len(data.value), "default_payload",
                "The stored array default is not available as scalar cells.", location)
        return [data_value(item, dtype[6:], types, location) for item in data.value]
    if dtype == "string":
        return text(data.value, location)
    if dtype == "bool":
        require(data.value in (0, 1), "default_payload", "Invalid Boolean default.", location)
        return bool(data.value)
    require(type(data.value) in (int, float) and math.isfinite(data.value), "default_payload",
            "The stored numeric default is not finite.", location)
    return data.value


def panel_defaults(vi, types, order):
    if b"DFDS" not in vi.blocks:
        return {}
    records = one_section(vi, b"DFDS").content
    if not records:
        return {}
    by_index = {record.index: record for record in records}
    require(len(by_index) == len(records), "default_index", "Duplicate DFDS entry.", "DFDS")
    index = one_section(vi, b"TM80").indexShift
    candidates = [item for item in records if item.tdType.name == "RepeatedBlock" and
                  len(item.value) == len(LVparts.DSINIT) and
                  all(child.tdType.name == "NumInt32" for child in item.value)]
    require(len(candidates) == 1, "default_table", "The data-space initialization table is not unique.", "DFDS")
    init = candidates[0].value

    def top_index(value):
        return index + (value & 0xFFFFFF)

    table = by_index.get(top_index(init[LVparts.DSINIT.fpdcoTableTMI].value))
    require(table is not None and table.tdType.name == "RepeatedBlock" and
            init[LVparts.DSINIT.nDCOs].value == len(table.value) == len(order),
            "default_table", "The data-space panel table does not match the panel object list.", "DFDS")
    columns = {name: i for i, (name, _) in enumerate(LVparts.DCO._fields_)}
    result = {}
    for row in table.value:
        require(row.tdType.name == "Cluster" and len(row.value) == len(columns), "default_table",
                "The data-space panel entry has an unexpected layout.", "DFDS")
        position = row.value[columns["dcoIndex"]].value
        require(type(position) is int and 0 <= position < len(order) and order[position] not in result,
                "default_index", "The data-space panel entry has an invalid DCO index.", "DFDS")
        default_index = top_index(row.value[columns["defaultDataTMI"]].value)
        role = "indicator" if row.value[columns["isIndicator"]].value else "control"
        result[order[position]] = (types.consolidated.getTopType(default_index), by_index.get(default_index), role)
    return result


def read_widgets(vi, types):
    root = field(heap_root(vi, b"FPHb"), "root", "FPHb")
    pane = field(root, "paneHierarchy", "FPHb")
    require(kind(pane) == "pane", "panel_pane", "The profile requires one panel pane.", "FPHb")
    order = [identity(item, "FPHb/ddoList") for item in members(root, "ddoList", "FPHb")]
    defaults = panel_defaults(vi, types, order)
    widgets = {}
    for item in members(pane, "zPlaneList", "FPHb"):
        uid = identity(item, "FPHb")
        location = f"FPHb/uid:{uid}"
        require(kind(item) == "fPDCO" and uid not in widgets, "panel_object",
                "The panel contains an unsupported object or duplicate UID.", location)
        body = field(item, "ddo", location)
        dtype = types.of(item, location)
        family = {"string": "stdString", "bool": "stdBool"}.get(dtype, "stdNum")
        if dtype.startswith("array:"):
            family = "indArr"
        require(kind(body) == family, "panel_widget", f"Unsupported display class '{kind(body)}'.", location)
        require(types.of(body, location) == dtype, "panel_type",
                "The panel object and its display type disagree.", location)
        parts = members(body, "partsList", location)
        labels = [part for part in parts if kind(part) == "label" and number(part, "partID", location) == 16]
        require(len(labels) == 1, "panel_label", "The widget has no unique readable label.", location)
        name = text(field(field(labels[0], "textRec", location), "text", location).content, location)
        role = "indicator" if flags(item, location) & 1 else "control"
        value = implicit_value(dtype)
        if uid in defaults:
            descriptor, data, default_role = defaults[uid]
            require(types.descriptor(descriptor, location) == dtype and default_role == role,
                    "default_type", "The data-space role or type disagrees with the panel heap.", location)
            if data is not None:
                value = data_value(data, dtype, types, location)
        widgets[uid] = Widget(uid, name, dtype, role, rect(body, "bounds", location), value)
    require(len(order) == len(widgets) and set(order) == widgets.keys(), "panel_coverage",
            "Panel references and objects do not match.", "FPHb/ddoList")
    return widgets


class DiagramReader:
    def __init__(self, vi, types, widgets):
        self.types = types
        self.widgets = widgets
        self.definitions = {}
        for item in one_section(vi, b"BDHb").objects:
            if item.getScopeInfo() == LVheap.NODE_SCOPE.TagClose or kind(item) is None:
                continue
            uid = item.attribs.get(LVheap.attributeNameToId("uid"))
            if uid is None:
                continue
            require(uid not in self.definitions, "heap_uid", "Duplicate source object UID.", f"BDHb/uid:{uid}")
            self.definitions[uid] = item
        self.diagrams = {}
        self.nodes = {}
        self.terms = {}
        self.signal_uids = set()
        self.boundary_descriptors = {}

    def resolve(self, item, location):
        uid = identity(item, location)
        result = self.definitions.get(uid)
        require(result is not None and (kind(item) is None or item is result), "heap_reference",
                f"Unable to resolve source UID {uid}.", location)
        return result

    def read_terminal(self, item, diagram, owner, location):
        uid = identity(item, location)
        location += f"/term:{uid}"
        require(uid not in self.terms and kind(item) in {"term", "fPTerm"}, "terminal",
                "The source terminal has an unsupported class or duplicate UID.", location)
        binding = field(item, "dco", location)
        binding_uid = identity(binding, location)
        terminal_flags = flags(item, location)
        if kind(item) == "fPTerm":
            widget = self.widgets.get(binding_uid)
            require(widget is not None and kind(binding) is None, "panel_reference",
                    "The diagram terminal does not resolve to a panel widget.", location)
            require(bool(terminal_flags & 0x8000) == (widget.role == "indicator"), "panel_direction",
                    "The panel and diagram directions differ.", location)
            term = Terminal(uid, kind(item), diagram, owner, terminal_flags, widget.dtype, "fPDCO",
                            binding_uid, rect(item, "bounds", location), None, 0, None, None)
        else:
            descriptor = self.resolve(binding, location)
            binding_kind = kind(descriptor)
            require(binding_kind in TERM_BINDINGS, "terminal_descriptor",
                    f"Unsupported terminal descriptor '{binding_kind}'.", location)
            dtype = self.types.of(descriptor, location)
            value = None
            position = rect(descriptor, "termBounds", location, optional=True)
            if binding_kind == "bDConstDCO":
                require(not dtype.startswith("array:"), "constant_type",
                        "Array-literal payloads are outside this profile.", location)
                value = constant_value(descriptor, dtype, location)
                position = rect(field(descriptor, "ddo", location), "bounds", location)
            if binding_kind in BOUNDARIES:
                self.boundary_descriptors[binding_uid] = descriptor
            term = Terminal(uid, kind(item), diagram, owner, terminal_flags, dtype, binding_kind,
                            binding_uid, position, value, flags(descriptor, location),
                            optional_number(descriptor, "parmIndex", location),
                            optional_number(descriptor, "override", location))
        self.terms[uid] = term
        return uid

    def read_node(self, item, diagram, depth):
        uid = identity(item, "BDHb")
        location = f"BDHb/uid:{uid}"
        category = kind(item)
        require(uid not in self.nodes and category in STRUCTURES | {"sRN", "prim", "aBuild"},
                "diagram_node", f"Unsupported or repeated diagram node '{category}'.", location)
        self.nodes[uid] = None
        terms = tuple(self.read_terminal(self.resolve(term, location), diagram, uid, location)
                      for term in members(item, "termList", location, optional=category == "flatSequence"))
        frame_nodes = [self.resolve(frame, location)
                       for frame in members(item, "sequenceList", location, optional=category != "flatSequence")]
        require(all(kind(frame) == "sequenceFrame" for frame in frame_nodes) and
                bool(frame_nodes) == (category == "flatSequence"), "diagram_regions",
                "The sequence contains an unsupported or missing frame.", location)
        frames = tuple(self.read_node(frame, diagram, depth) for frame in frame_nodes)
        regions = tuple(self.read_diagram(self.resolve(region, location), depth + 1)
                        for region in members(item, "diagramList", location, optional=category not in {"sequenceFrame", "forLoop"}))
        require(len(regions) == int(category in {"sequenceFrame", "forLoop"}),
                "diagram_regions", "The diagram regions do not match their structure.", location)
        primitive = None
        if category == "prim":
            primitive = (number(item, "primResID", location), number(item, "primIndex", location))
        operands = ()
        result = None
        if category == "aBuild":
            operands = tuple(identity(operand, location) for operand in members(item, "dcoList", location))
            result = identity(field(item, "dcoAgg", location), location)
            incoming = [self.terms[term].binding_uid for term in terms if not self.terms[term].source]
            outgoing = [self.terms[term].binding_uid for term in terms if self.terms[term].source]
            require(len(operands) == len(set(operands)) == len(incoming) and set(operands) == set(incoming)
                    and outgoing == [result], "array_bindings",
                    "The array operand order or aggregate does not match the declared terminals.", location)
        self.nodes[uid] = Node(uid, category, diagram, rect(item, "bounds", location), terms,
                               regions, frames, primitive, flags(item, location), operands, result)
        return uid

    def read_signal(self, signal, diagram, used_inputs):
        uid = identity(signal, "BDHb")
        location = f"BDHb/uid:{uid}"
        require(kind(signal) == "signal" and uid not in self.signal_uids,
                "signal", "The entry is not a unique source signal.", location)
        require(number(signal, "state", location) in (1, 33), "signal_state",
                "The saved wire state is outside the verified profile.", location)
        self.signal_uids.add(uid)
        terminals = [self.terms.get(identity(term, location)) for term in members(signal, "termList", location)]
        require(len(terminals) >= 2 and all(term is not None and term.diagram == diagram for term in terminals),
                "signal_scope", "A signal crosses an unresolved diagram boundary.", location)
        sources = [term for term in terminals if term.source]
        targets = [term for term in terminals if not term.source]
        target_ids = tuple(term.uid for term in targets)
        require(len(sources) == 1 and targets and all(term.dtype == sources[0].dtype for term in targets)
                and len(set(target_ids)) == len(target_ids) and not used_inputs.intersection(target_ids),
                "signal_topology", "The signal is ambiguous or requires an unsupported conversion.", location)
        used_inputs.update(target_ids)
        return Signal(uid, sources[0].uid, target_ids)

    def read_diagram(self, item, depth):
        uid = identity(item, "BDHb")
        location = f"BDHb/uid:{uid}"
        require(depth <= 16 and uid not in self.diagrams and kind(item) == "diag", "diagram_region",
                "The diagram is ambiguous or exceeds the supported depth.", location)
        self.diagrams[uid] = None
        listed = [self.resolve(child, location) for child in members(item, "nodeList", location)]
        drawing = [self.resolve(child, location) for child in members(item, "zPlaneList", location)]
        sequences = [node for node in drawing if kind(node) == "flatSequence"]
        composed = [self.read_node(node, uid, depth) for node in sequences]
        frames = {frame for node in composed for frame in self.nodes[node].frames}
        require(len({identity(node, location) for node in listed}) == len(listed) and
                frames == {identity(node, location) for node in listed if kind(node) == "sequenceFrame"},
                "frame_reference", "The sequence contents and diagram node list disagree.", location)
        normal = [self.read_node(node, uid, depth) for node in listed if kind(node) != "sequenceFrame"]
        drawn = [identity(node, location) for node in drawing]
        visible = set(composed)
        visible.update(node for node in normal if self.nodes[node].kind != "sRN")
        visible.update(term for node in normal if self.nodes[node].kind == "sRN" for term in self.nodes[node].terms
                       if self.terms[term].binding_kind in {"fPDCO", "bDConstDCO", "lCnt"})
        require(len(drawn) == len(set(drawn)) and set(drawn) == visible, "diagram_coverage",
                "Diagram nodes, terminals and visible source objects disagree.", location)
        used_inputs = set()
        signals = tuple(self.read_signal(self.resolve(signal, location), uid, used_inputs)
                        for signal in members(item, "signalList", location, optional=True))
        self.diagrams[uid] = Diagram(uid, tuple(normal + composed), signals)
        return uid

    def read_boundaries(self):
        boundaries = {}
        for uid, descriptor in self.boundary_descriptors.items():
            location = f"BDHb/uid:{uid}"
            category = kind(descriptor)
            owner = descriptor.parent
            while owner is not None and kind(owner) not in STRUCTURES:
                owner = owner.parent
            require(owner is not None, "boundary_owner", "A boundary descriptor has no structure owner.", location)
            node = self.nodes.get(identity(owner, location))
            require(node is not None and len(node.regions) == 1, "boundary_owner",
                    "The boundary's owning structure has no unique diagram.", location)
            ends = [self.terms.get(identity(item, location)) for item in members(descriptor, "termList", location)]
            require(len(ends) == (1 if category == "lCnt" else 2) and
                    all(term is not None and term.binding_uid == uid for term in ends),
                    "boundary_term", "The boundary ends do not resolve to the same descriptor.", location)
            interior = [term for term in ends if term.diagram == node.regions[0]]
            exterior = [term for term in ends if term.diagram == node.diagram and term.owner == node.uid]
            require(len(interior) == 1 and len(exterior) == len(ends) - 1,
                    "boundary_scope", "The boundary ends do not match its inner and outer diagrams.", location)
            mate = field(descriptor, "mate", location, optional=True)
            if category == "lSR":
                mate = field(descriptor, "rsrDCO", location)
            if category == "rSR":
                aliases = members(descriptor, "lsrDCOList", location)
                require(len(aliases) == 1, "register_history", "The shift register has multiple history cells.", location)
                mate = aliases[0]
            boundaries[uid] = Boundary(uid, category, node.uid, interior[0].uid,
                                       exterior[0].uid if exterior else None, interior[0].dtype,
                                       identity(mate, location) if mate is not None else None,
                                       rect(descriptor, "termBounds", location, optional=True), flags(descriptor, location))
        for boundary in boundaries.values():
            location = f"BDHb/uid:{boundary.uid}"
            if boundary.mate is not None:
                other = boundaries.get(boundary.mate)
                require(other is not None and other.mate == boundary.uid and other.dtype == boundary.dtype,
                        "boundary_mate", "The boundary mate is missing or inconsistent.", location)
        return boundaries


def decode_graph(vi):
    """Resolve the qualified source region profile through heap UIDs and VCTP types."""
    types = HeapTypes(vi)
    widgets = read_widgets(vi, types)
    reader = DiagramReader(vi, types, widgets)
    root = reader.read_diagram(field(heap_root(vi, b"BDHb"), "root", "BDHb"), 0)
    boundaries = reader.read_boundaries()
    return Graph(root, reader.diagrams, reader.nodes, reader.terms, boundaries, widgets)
