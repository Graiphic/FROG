# SPDX-License-Identifier: Apache-2.0
"""Resolve decoded LabVIEW heap objects into an attributed source graph."""

from dataclasses import dataclass
import math
import struct

from pylabview import LVdatafill, LVdatatype, LVheap, LVmisc, LVparts
from import_labview_interfaces import ExactRead, InterfaceTypes
from import_labview_profile import (
    TYPES, ProfileRefusal, children, constant_value, decode_constant_value, field, heap_root, identity, initial_value, items,
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
    waveform: dict | None = None


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
    region_owners: dict
    resource_nodes: dict


STRUCTURES = {"flatSequence", "sequenceFrame", "forLoop", "select", "whileLoop"}
BOUNDARIES = {"seqTun", "flatSeqTun", "lMax", "lCnt", "lSR", "rSR", "lpTun", "lTst", "caseSel", "selTun"}
TERM_BINDINGS = BOUNDARIES | {"fPDCO", "bDConstDCO", "parm", "overridableParm", "aBuildDCO",
                              "mxDCO", "dmxDCO", "aIDCO", "aRepDCO", "subsetDCO", "concatDCO", "Class018A"}


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


class HeapTypes(InterfaceTypes):
    def set_element(self, dtype, location):
        stream = ExactRead(dtype.raw_data)
        try:
            code, flags, size = LVdatatype.TDObject.parseRSRCDataHeader(stream)
            require(code == 0x73 and size == len(dtype.raw_data), "set_type",
                    "Invalid LabVIEW Set descriptor.", location)
            index = LVmisc.readVariableSizeFieldU2p2(stream)
            if flags & LVdatatype.TYPEDESC_FLAGS.HasLabel.value:
                label = stream.read(stream.read(1)[0])
                require(label == dtype.label, "set_type", "Inconsistent Set label.", location)
            require(stream.read() in (b"", b"\x00"), "set_type", "Unknown Set descriptor suffix.", location)
            return self.consolidated.getFlatType(index)
        except (EOFError, IndexError) as exc:
            raise ProfileRefusal("set_type", "Invalid Set element type or truncated descriptor.", location) from exc

    def descriptor(self, dtype, location, ancestry=()):
        require(dtype is not None and id(dtype) not in ancestry and len(ancestry)<16, "type_reference",
                "Missing, cyclic or excessively nested value type.", location)
        family = getattr(dtype.fullType(), "name", "unknown")
        if dtype.fullType() == 0x73:
            element = self.descriptor(self.set_element(dtype, location), location, (*ancestry, id(dtype)))
            if element.startswith("array:"):
                element = f"array<{element[6:]}>"
            return f"set<{element}>"
        if family in ("Path", "LVVariant", "Refnum"):
            native = self.describe(dtype, location).native_type(self.encoding)
            require(native, "unsupported_type", f"Unsupported {family} descriptor.", location)
            return native
        if family == "Cluster":
            require(0 < len(dtype.clients) <= 32, "cluster_type", "The positional Cluster has no supported arity.", location)
            named = self.describe(dtype, location).native_type(self.encoding)
            if named == 'cluster<"status":bool,"code":i32,"source":string>':
                return named
            fields = []
            for index, client in enumerate(dtype.clients, 1):
                child = self.consolidated.getFlatType(client.index)
                element = self.descriptor(child, location, (*ancestry,id(dtype)))
                if element.startswith("array:"):
                    element = f"array<{element[6:]}>"
                fields.append(f'"element{index}":{element}')
            return "cluster<" + ",".join(fields) + ">"
        if family in ("Array", "SubArray"):
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


def empty_numeric_array_element(types, descriptor, display, location):
    if display is None or kind(display) != "indArr":
        return None
    raw = field(descriptor, "ConstValue", location, optional=True)
    if raw is None or raw.content not in (bytes(4), bytes(5)):
        return None
    try:
        dtype = types.of(descriptor, location)
        if dtype in ("array:i32", "array:f64") and types.of(display, location) == dtype:
            return dtype.removeprefix("array:")
    except ProfileRefusal:
        pass
    return None


def waveform_display(body, dtype, role, location, encoding):
    graph_type = optional_number(body, "graphType", location)
    require(graph_type == 1 and dtype == "array:f64" and role == "indicator", "panel_widget",
            f"Unsupported stdGraph variant {graph_type} with {dtype} ({role}).", location)
    settings = {"profile": "WFG-1-linear"}
    axes = set()
    for scale in members(body, "partsList", location):
        if kind(scale) != "scale":
            continue
        axis = {18: "x", 19: "y"}.get(number(scale, "partID", location))
        require(axis and axis not in axes, "graph_scale", "The Waveform Graph has unsupported extra axes.", location)
        axes.add(axis)
        for key, name in (("min", "scaleDMin"), ("max", "scaleDMax")):
            value = field(scale, name, location, optional=True)
            require(value is None or isinstance(value.content, bytes) and len(value.content) == 8, "graph_scale",
                    "The Waveform Graph scale bound is not a saved double.", location)
            settings[f"{axis}_{key}"] = 0.0 if value is None else struct.unpack(">d", value.content)[0]
        lo, hi = settings[f"{axis}_min"], settings[f"{axis}_max"]
        require(math.isfinite(lo) and math.isfinite(hi) and lo < hi, "graph_scale",
                "The Waveform Graph scale interval is unsupported.", location)
        for part in members(scale, "partsList", location):
            if kind(part) == "label" and number(part, "partID", location) == 94:
                record = field(part, "textRec", location)
                labels = children(record, "text")
                if labels:
                    settings[f"{axis}_label"] = labels[0].content.decode(encoding)
    require(axes == {"x", "y"}, "graph_scale", "The Waveform Graph requires both saved axes.", location)
    scale_data = members(body, "GraphScaleData", location)
    palette = members(body, "GraphScalePalette", location)
    require(len(scale_data) == len(palette) == 2, "graph_scale",
            "Cannot identify the autoscale mode of both Waveform Graph axes.", location)
    for record, entry in zip(scale_data, palette):
        require(kind(record) == "ScaleData" and kind(entry) == "ScaleLegendData", "graph_scale",
                "The Waveform Graph has unsupported scale metadata.", location)
        axis = {18: "x", 19: "y"}.get(number(record, "partID", location))
        name = field(entry, "name", location)
        automatic = field(entry, "autoScale", location)
        require(axis in axes and isinstance(name.content, bytes) and name.content.decode(encoding) == settings.get(f"{axis}_label")
                and type(automatic.value) is bool, "graph_scale",
                "Cannot associate the saved autoscale flag with its Waveform Graph axis.", location)
        settings[f"autoscale_{axis}"] = automatic.value
        axes.remove(axis)
    return settings


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
    type_map = vi.blocks.get(b"TM80")
    require(type_map is not None, "default_table", "Stored defaults have no LabVIEW 8+ type map.", "DFDS")
    primary = type_map.getParsedSection(None)
    require(not primary.parse_failed, "default_table", "The primary type map could not be decoded.", "DFDS")
    index = primary.indexShift
    candidates = [item for item in records if item.tdType.name == "RepeatedBlock" and
                  len(item.value) == len(LVparts.DSINIT) and
                  all(child.tdType.name == "NumInt32" for child in item.value)]
    require(len(candidates) == 1, "default_table", "The data-space initialization table is not unique.", "DFDS")
    ds_init = candidates[0]
    init = ds_init.value

    def top_index(value):
        ordinal = value & 0xFFFFFF
        if (value >> 24) & 0xFF == 0x40:
            require(vi.getFileVersion()["major"] == 20 and ordinal != 0xFFFFFF,
                    "default_table", "Unsupported relative data-space type reference.", "DFDS")
            resolved = ds_init.index - 1 + ordinal
            require(index <= resolved < index + len(primary.content), "default_table",
                    "The relative data-space type reference is outside the primary type map.", "DFDS")
            return resolved
        return index + ordinal

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


def inline_panel_default(vi, obj, descriptor, location):
    raw = field(obj, "DefaultData", location, optional=True)
    if raw is None:
        return None
    family = descriptor.fullType().name
    require(family in TYPES and isinstance(raw.content, bytes), "panel_default",
            f"No verified inline panel default reader for {family}.", location)
    stream = ExactRead(raw.content)
    record = LVdatafill.newDataFillObjectWithTD(vi, (b"FPHb", 0), -1, 0, descriptor, vi.po)
    record.initWithRSRC(stream)
    require(stream.tell() == len(raw.content), "panel_default",
            "The inline panel default contains unread bytes.", location)
    return record


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
        waveform = None
        role = "indicator" if flags(item, location) & 1 else "control"
        if kind(body) == "stdGraph":
            waveform = waveform_display(body, dtype, role, location, vi.textEncoding)
        else:
            require(kind(body) == family, "panel_widget", f"Unsupported display class '{kind(body)}'.", location)
        require(types.of(body, location) == dtype, "panel_type",
                "The panel object and its display type disagree.", location)
        parts = members(body, "partsList", location)
        labels = [part for part in parts if kind(part) == "label" and number(part, "partID", location) == 16]
        require(len(labels) == 1, "panel_label", "The widget has no unique readable label.", location)
        label_bytes = field(field(labels[0], "textRec", location), "text", location).content
        require(isinstance(label_bytes, bytes) and len(label_bytes) <= 512, "panel_label",
                "The saved panel label is missing or exceeds 512 bytes.", location)
        name = label_bytes.decode(vi.textEncoding)
        value = implicit_value(dtype)
        if uid in defaults:
            descriptor, data, default_role = defaults[uid]
            require(types.descriptor(descriptor, location) == dtype and default_role == role,
                    "default_type", "The data-space role or type disagrees with the panel heap.", location)
            if data is not None:
                value = data_value(data, dtype, types, location)
        else:
            descriptor = types.heap(item, "typeDesc", location)
            stored = inline_panel_default(vi, item, descriptor, location)
            if stored is not None:
                value = data_value(stored, dtype, types, location)
        widgets[uid] = Widget(uid, name, dtype, role, rect(body, "bounds", location), value, waveform)
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

    def read_terminal(self, item, diagram, owner, owner_kind, location):
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
            if binding_kind == "lpTun" and flags(descriptor, location) & 0x01000000 and owner_kind == "sRN":
                require(dtype.startswith("array:"), "loop_tunnel_type",
                        "An indexing tunnel must connect a rank-one Array to its element.", location)
                dtype = dtype[6:]
            value = None
            position = rect(descriptor, "termBounds", location, optional=True)
            if binding_kind == "bDConstDCO":
                display = field(descriptor, "ddo", location)
                if dtype.startswith("array:"):
                    require(empty_numeric_array_element(self.types, descriptor, display, location),
                            "constant_type", "Only a decoded empty rank-one numeric array literal is supported.", location)
                    value = []
                elif dtype.startswith("cluster<"):
                    td = self.types.heap(descriptor, "typeDesc", location)
                    raw = field(descriptor, "ConstValue", location).content
                    require(isinstance(raw,bytes), "cluster_literal", "Missing Cluster literal payload.", location)
                    stream = ExactRead(raw)
                    value = []
                    for index, client in enumerate(td.clients,1):
                        member_type = self.types._child(client)
                        member_dtype = self.types.descriptor(member_type, location)
                        require(member_dtype in TYPES.values() and member_dtype not in ("string","bool"), "cluster_literal",
                                "The positional literal requires fixed-width numeric fields.", location)
                        label = getattr(member_type,"label",None)
                        value.append((label.decode(self.types.encoding) if label else f"Element {index}", member_dtype,
                                      decode_constant_value(stream.read(int(member_dtype[1:])//8),member_dtype,location)))
                    require(stream.tell()==len(raw), "cluster_literal", "Unread bytes in Cluster literal.", location)
                else:
                    require(dtype in TYPES.values(), "constant_type", f"Unsupported literal type '{dtype}'.", location)
                    value = constant_value(descriptor, dtype, location)
                position = rect(display, "bounds", location)
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
        require(uid not in self.nodes and category in STRUCTURES | {"sRN", "prim", "aBuild", "aIndx", "aReplace", "mux", "demux", "subset", "concat", "Class0189"},
                "diagram_node", f"Unsupported or repeated diagram node '{category}'.", location)
        self.nodes[uid] = None
        terms = tuple(self.read_terminal(self.resolve(term, location), diagram, uid, category, location)
                      for term in members(item, "termList", location, optional=category == "flatSequence"))
        frame_nodes = [self.resolve(frame, location)
                       for frame in members(item, "sequenceList", location, optional=category != "flatSequence")]
        require(all(kind(frame) == "sequenceFrame" for frame in frame_nodes) and
                bool(frame_nodes) == (category == "flatSequence"), "diagram_regions",
                "The sequence contains an unsupported or missing frame.", location)
        frames = tuple(self.read_node(frame, diagram, depth) for frame in frame_nodes)
        regions = tuple(self.read_diagram(self.resolve(region, location), depth + 1)
                        for region in members(item, "diagramList", location, optional=category not in {"sequenceFrame", "forLoop", "select", "whileLoop"}))
        require(len(regions) == (2 if category == "select" else int(category in {"sequenceFrame", "forLoop", "whileLoop"})),
                "diagram_regions", "The diagram regions do not match their structure.", location)
        primitive = None
        if category == "prim":
            primitive = (number(item, "primResID", location), number(item, "primIndex", location))
        operands = ()
        result = None
        if category in {"aBuild", "aIndx", "aReplace", "mux", "demux", "subset", "concat", "Class0189"}:
            operands = tuple(identity(operand, location) for operand in members(item, "dcoList", location))
            result = identity(field(item, "dcoAgg", location), location)
        if category == "aBuild":
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
        state = number(signal, "state", location)
        self.signal_uids.add(uid)
        terminals = [self.terms.get(identity(term, location)) for term in members(signal, "termList", location)]
        require(len(terminals) >= 2 and all(term is not None and term.diagram == diagram for term in terminals),
                "signal_scope", "A signal crosses an unresolved diagram boundary.", location)
        sources = [term for term in terminals if term.source]
        targets = [term for term in terminals if not term.source]
        # LV marks an integral wire coerced at the input to floating Divide.
        # Admit the flag only on the decoded operation, direction and representation.
        coerced_divide = any(term.flags & 0x40000 and term.dtype == "i32" and term.binding_kind == "parm"
                             and self.nodes[term.owner].primitive == (1053, 106) for term in targets)
        require(state in (1, 33) or state in (17, 49) and coerced_divide, "signal_state",
                "The saved wire state is outside the verified profile.", location)
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
                       if self.terms[term].binding_kind in {"fPDCO", "bDConstDCO", "lCnt", "lTst"})
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
            if node and node.kind in {"select","whileLoop"}:
                # The shared region reader verifies all per-frame faces (a Case
                # tunnel has one interior terminal per arm, not one per node).
                continue
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
    owners = {}
    resources = {}
    from import_labview_editable import BindingTypes, _RESOURCE_PRIMITIVES, resource_primitive
    if any(n.kind in {"select", "whileLoop"} or n.primitive in _RESOURCE_PRIMITIVES for n in reader.nodes.values()):
        from import_labview_source_diagram import decode_source_diagram
        from import_labview_regions import project_region_owner, loop_tunnel_modes
        decoded = decode_source_diagram(vi)
        binding_types = BindingTypes(vi)
        modes = loop_tunnel_modes(decoded, binding_types)
        for n in decoded.nodes.values():
            if reader.nodes[n.uid].primitive in _RESOURCE_PRIMITIVES:
                wired = {tid for sid in decoded.diagrams[n.diagram].signals for tid in decoded.signals[sid].endpoints}
                translated = resource_primitive(n, decoded, binding_types, wired)
                require(translated, "resource_signature", "Unverified resource operation or element type.", f"BDHb/uid:{n.uid}")
                resources[n.uid] = translated
            if n.kind in {"select","whileLoop"}:
                target, endpoints, notes = project_region_owner(decoded, n, dict.fromkeys(n.regions), binding_types, loop_modes=modes)
                owners[n.uid] = (target, endpoints, notes)
    return Graph(root, reader.diagrams, reader.nodes, reader.terms, boundaries, widgets, owners, resources)
