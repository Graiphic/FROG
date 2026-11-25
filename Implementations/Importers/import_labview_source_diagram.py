# SPDX-License-Identifier: Apache-2.0
"""Read the editable LabVIEW diagram topology before attempting a FROG translation."""

from collections import defaultdict
from dataclasses import dataclass, replace

from pylabview import LVheap

from import_labview_heap_graph import HeapTypes, empty_numeric_array_element

from import_labview_profile import (
    ATTR_UID, field, heap_root, identity, items, kind, one_section, require, tag,
)


DiagramBounds = tuple[int, int, int, int]
TERMINAL_CLASSES = {"term", "fPTerm"}
CALL_BINDINGS = {"iUseDCO", "polyIUseDCO"}
NODE_FIELDS = (
    "activeDiag", "caseSelDCO", "clumpNum", "commentSelInfoArray", "CommentSelLabFlags",
    "connectorTM", "dcoAgg", "dcoList", "srcDCO", "dIdx", "defaultDiag", "EventNodeEvents", "invokeFlags", "label",
    "loopIndexDCO", "loopLimitDCO", "loopTestDCO", "timeOutDCO", "srDCOList", "ConditionDCOList",
    "methCode", "methName", "nodeName", "oMId", "permDCOList", "privDataList",
    "primIndex", "primResID", "propList", "SelectDefaultCase", "SelectNRightType",
    "SelectRangeArray32", "SelectSelLabFlags", "SelectStringArray", "selString", "shortCount",
)
BINDING_FIELDS = (
    "ConstValue", "DefaultData", "englishName", "i", "id", "label", "lsrDCOList", "mate",
    "methParamIdx", "objFlags", "override", "paramIdx", "parmIndex", "propCode", "propList", "propName", "rsrDCO", "termBMPs",
)


@dataclass(frozen=True)
class SourceField:
    tag: str
    kind: str | None
    uid: int | None
    value: int | bool | None
    payload: bytes | None
    children: tuple["SourceField", ...]


@dataclass(frozen=True)
class Issue:
    code: str
    uid: int
    related: tuple[int, ...] = ()


@dataclass(frozen=True)
class Diagram:
    uid: int
    owner: int | None
    index: int | None
    nodes: tuple[int, ...]
    z_order: tuple[int, ...]
    signals: tuple[int, ...]


@dataclass(frozen=True)
class Node:
    uid: int
    kind: str
    diagram: int
    container: int | None
    bounds: DiagramBounds | None
    content_bounds: DiagramBounds | None
    flags: int | None
    terminals: tuple[int, ...]
    regions: tuple[int, ...]
    frames: tuple[int, ...]
    boundary_bindings: tuple[int, ...]
    fields: dict[str, SourceField]
    field_names: tuple[str, ...]


@dataclass(frozen=True)
class Terminal:
    uid: int
    kind: str
    diagram: int
    owner: int
    flags: int | None
    direction: str | None
    direction_evidence: str | None
    binding: int | None
    bounds: DiagramBounds | None


@dataclass(frozen=True)
class Binding:
    uid: int
    kind: str
    resource: str
    owner: int | None
    flags: int | None
    type_id: int | None
    type_kind: str | None
    type_code: int | None
    type_label: str | None
    enum_labels: tuple[str, ...]
    record_fields: tuple[tuple[str | None, str | None], ...]
    bounds: DiagramBounds | None
    terminals: tuple[int, ...]
    fields: dict[str, SourceField]
    zero_ring_item_table: bytes | None = None
    padded_scalar_zero: bool = False
    empty_array_element: str | None = None


@dataclass(frozen=True)
class Signal:
    uid: int
    diagram: int
    endpoints: tuple[int, ...]
    source: int | None
    targets: tuple[int, ...]
    state: int | None
    last_kind: int | None
    route_data: bytes | None


@dataclass(frozen=True)
class CaseRange:
    region: int
    start: int
    end: int
    start_kind: int
    end_kind: int


@dataclass(frozen=True)
class RegionSelection:
    structure: int
    selector: int | None
    range_rules: tuple[CaseRange, ...]
    default_index: int | None
    default_region: int | None
    active_region: int | None
    displayed_region: int | None
    displayed_label: str | None
    exact_string_table: tuple[str, ...] = ()


@dataclass(frozen=True)
class SourceDiagram:
    root: int
    diagrams: dict[int, Diagram]
    nodes: dict[int, Node]
    terminals: dict[int, Terminal]
    bindings: dict[int, Binding]
    signals: dict[int, Signal]
    selections: dict[int, RegionSelection]
    issues: tuple[Issue, ...]


def _definitions(vi, resource):
    definitions = {}
    section = one_section(vi, resource)
    location = resource.decode("ascii")
    require(len(section.objects) <= 100_000, "source_diagram_records", "The heap has too many records.", location)
    for item in section.objects:
        if item.getScopeInfo() == LVheap.NODE_SCOPE.TagClose or kind(item) is None or ATTR_UID not in item.attribs:
            continue
        uid = identity(item, location)
        require(uid not in definitions, "heap_uid", "The heap defines the same UID twice.", f"{location}/uid:{uid}")
        definitions[uid] = item
    return definitions


def _number(item, name, location):
    child = field(item, name, location, optional=True)
    value = getattr(child, "value", None)
    return value if type(value) is int else None


def _bounds(item, name, location):
    child = field(item, name, location, optional=True)
    # The upstream rectangle member names follow the serialized row/column order.
    if isinstance(child, LVheap.HeapNodeRect) and child.format == "inline":
        return child.top, child.left, child.bottom, child.right
    return None


def _members(item, name, location):
    child = field(item, name, location, optional=True)
    return items(child, location) if child is not None else ()


def _blob(item):
    return item.content if item is not None and isinstance(item.content, bytes) else None


def _zero_ring_item_table(display, location):
    # The selected ordinal in the multiLabel is omitted for the first item.
    if display is None:
        return None
    if kind(display) == "typeDef":
        rings = [part for part in _members(display, "partsList", location) if kind(part) == "stdRing"]
        if len(rings) != 1:
            return None
        display = rings[0]
    if kind(display) != "stdRing":
        return None
    labels = [part for part in _members(display, "partsList", location) if kind(part) == "multiLabel"]
    if len(labels) != 1:
        return None
    index = field(labels[0], "index", location, optional=True)
    if index is not None and _number(labels[0], "index", location) != 0:
        return None
    return _blob(field(labels[0], "buf", location, optional=True))


def _field_snapshot(item, depth):
    require(depth < 48, "source_field_depth", "Saved diagram metadata has too many nested fields.", "BDHb")
    value = getattr(item, "value", None)
    return SourceField(tag(item), kind(item), item.attribs.get(ATTR_UID),
                       value if type(value) in (int, bool) else None, _blob(item),
                       tuple(_field_snapshot(child, depth + 1) for child in item.childs
                             if child.getScopeInfo() != LVheap.NODE_SCOPE.TagClose))


def _fields(item, names, location):
    result = {}
    for name in names:
        child = field(item, name, location, optional=True)
        if child is not None:
            result[name] = _field_snapshot(child, 0)
    return result


def _saved_integer(field):
    if field is not None:
        if type(field.value) is int:
            return field.value
        if field.payload is not None and len(field.payload) in (1, 2, 4):
            return int.from_bytes(field.payload, "big", signed=True)
    return None


def _saved_text(value, encoding):
    if not isinstance(value, bytes) or len(value) > 1024:
        return None
    try:
        return value.decode(encoding)
    except UnicodeError:
        return None


def _member(record, name):
    if record is not None:
        return next((field for field in record.children if field.tag == name), None)
    return None


def _direction(flags, binding):
    # SubVI bindings explicitly save connector direction. The flag on a wire's
    # terminal normally agrees, except for alternate call connector terminals.
    if binding is not None and binding.flags is not None:
        if binding.kind in CALL_BINDINGS and binding.flags & 0x200 and (flags is None or flags & 0x4000):
            return ("source" if binding.flags & 1 else "sink"), "connector_binding"
        if binding.kind == "xTunnel" and binding.flags & 0x201 == 0x201 and flags is None:
            return "source", "xnode_output_binding"
        if binding.kind in {"parm", "overridableParm"} and binding.flags & 1 and flags is None:
            return "source", "primitive_output_binding"
    if flags is not None:
        return ("sink" if flags & 0x8000 else "source"), "terminal_flags"
    return None, None


def is_quit_application(node, binding):
    return (node.kind == "prim" and len(node.terminals) == 1 and binding is not None
            and tuple(node.fields[k].value if k in node.fields else None for k in ("primResID", "primIndex")) == (1352, 138)
            and binding.kind == "parm" and binding.type_kind == "Boolean" and not (binding.flags or 0) & 1
            and "parmIndex" not in binding.fields)


class _Reader:
    def __init__(self, vi):
        self.encoding = vi.textEncoding
        self.major_version = vi.getFileVersion()["major"]
        self.definitions = _definitions(vi, b"BDHb")
        self.panel = _definitions(vi, b"FPHb") if b"FPHb" in vi.blocks else {}
        self.types = HeapTypes(vi) if b"DTHP" in vi.blocks and b"VCTP" in vi.blocks else None
        self.flat_type_count = len(one_section(vi, b"VCTP").content) if self.types is not None else 0
        self.diagrams = {}
        self.nodes = {}
        self.terminals = {}
        self.bindings = {}
        self.signals = {}
        self.issues = []

    def _defined(self, item, location):
        uid = identity(item, location)
        result = self.definitions.get(uid)
        require(result is not None and (kind(item) is None or result is item), "heap_reference",
                f"A source object reference does not resolve to UID {uid}.", location)
        return result

    def _uids(self, item, name, location):
        uids = tuple(identity(child, location) for child in _members(item, name, location))
        require(len(uids) == len(set(uids)), "heap_reference", f"'{name}' lists the same UID twice.", location)
        return uids

    def diagram(self, item, owner=None, index=None, depth=0):
        uid = identity(item, "BDHb")
        location = f"BDHb/uid:{uid}"
        require(len(self.diagrams) < 4096 and depth < 64 and uid not in self.diagrams and kind(item) == "diag",
                "source_diagram_region", "A diagram region is invalid, repeated or exceeds the traversal limit.", location)
        ordered = tuple(self._defined(child, location) for child in _members(item, "nodeList", location))
        drawn = tuple(self._defined(child, location) for child in _members(item, "zPlaneList", location))
        saved = [identity(child, location) for child in ordered]
        z_order = tuple(identity(child, location) for child in drawn)
        require(len(set(saved)) == len(saved) and len(set(z_order)) == len(z_order), "diagram_order",
                "A region lists a node or drawing item twice.", location)
        # Flat Sequence containers appear in drawing order, while their frames
        # are referenced in the logical node list as well as sequenceList.
        extra = [child for child in drawn if field(child, "sequenceList", location, optional=True) is not None
                 and identity(child, location) not in saved]
        saved.extend(identity(child, location) for child in extra)
        all_nodes = (*ordered, *extra)
        frames = {}
        for node in all_nodes:
            for frame in self._uids(node, "sequenceList", location):
                require(frame in saved and frame not in frames, "frame_reference",
                        "A sequence frame has no unique identity in its region.", location)
                frames[frame] = identity(node, location)
        signals = self._uids(item, "signalList", location)
        self.diagrams[uid] = Diagram(uid, owner, index, tuple(saved), z_order, signals)
        for child in all_nodes:
            self.node(child, uid, frames.get(identity(child, location)), depth)
        for child in _members(item, "signalList", location):
            self.signal(self._defined(child, location), uid)
        return uid

    def node(self, item, diagram, container, depth):
        uid = identity(item, "BDHb")
        location = f"BDHb/uid:{uid}"
        category = kind(item)
        require(uid not in self.nodes and category not in TERMINAL_CLASSES | {"diag", "signal"}, "source_node",
                "A logical diagram node is repeated or has the wrong class.", location)
        terminal_ids = self._uids(item, "termList", location)
        region_ids = self._uids(item, "diagramList", location)
        frame_ids = self._uids(item, "sequenceList", location)
        boundary_ids = dict.fromkeys(uid for name in ("tunnelList", "srDCOList", "ConditionDCOList")
                                    for uid in self._uids(item, name, location))
        for name in ("caseSelDCO", "loopIndexDCO", "loopLimitDCO", "loopTestDCO", "timeOutDCO"):
            descriptor = field(item, name, location, optional=True)
            if descriptor is not None and ATTR_UID in descriptor.attribs:
                boundary_ids[identity(descriptor, location)] = None
        saved_fields = (*NODE_FIELDS, "ddo") if category == "ctlRefConst" else NODE_FIELDS
        self.nodes[uid] = Node(uid, category, diagram, container, _bounds(item, "bounds", location),
                               _bounds(item, "contRect", location), _number(item, "objFlags", location),
                               terminal_ids, region_ids, frame_ids, tuple(boundary_ids), _fields(item, saved_fields, location),
                               tuple(tag(child) for child in item.childs if child.getScopeInfo() != LVheap.NODE_SCOPE.TagClose))
        for terminal in _members(item, "termList", location):
            self.terminal(self._defined(terminal, location), diagram, uid)
        for index, region in enumerate(_members(item, "diagramList", location)):
            self.diagram(self._defined(region, location), uid, index, depth + 1)

    def terminal(self, item, diagram, owner):
        uid = identity(item, "BDHb")
        location = f"BDHb/uid:{uid}"
        category = kind(item)
        require(uid not in self.terminals and category in TERMINAL_CLASSES, "source_terminal",
                "A terminal is repeated or has an unknown class.", location)
        reference = field(item, "dco", location, optional=True)
        binding = None
        binding_id = None
        if reference is not None:
            binding_id = identity(reference, location)
            if binding_id in self.bindings:
                binding = self.bindings[binding_id]
            else:
                binding = self.binding(binding_id, category == "fPTerm")
        if binding is None:
            self.issues.append(Issue("terminal_binding_unresolved", uid, (binding_id,) if binding_id is not None else ()))
        flags = _number(item, "objFlags", location)
        direction, evidence = _direction(flags, binding)
        if flags == 0x1000 and is_quit_application(self.nodes[owner], binding):
            # An unwired optional input has no consuming-wire flag. Its direction
            # is part of the saved primitive signature, including when unwired.
            direction, evidence = "sink", "quit_application_input"
        self.terminals[uid] = Terminal(uid, category, diagram, owner, flags, direction, evidence,
                                       binding_id, _bounds(item, "bounds", location))

    def binding(self, uid, panel):
        item = (self.panel if panel else self.definitions).get(uid)
        if item is None:
            return None
        resource = "FPHb" if panel else "BDHb"
        location = f"{resource}/uid:{uid}"
        type_id = _number(item, "typeDesc", location)
        type_name = None
        type_code = None
        type_label = None
        enum_labels = ()
        record_fields = ()
        if self.types is not None and type_id is not None and 1 <= type_id <= self.types.slice.tdCount:
            descriptor = self.types.consolidated.getTopType(self.types.slice.indexShift + type_id - 1)
            if descriptor is not None:
                source_type = descriptor.fullType()
                type_name = getattr(source_type, "name", None)
                type_code = int(source_type)
                type_label = _saved_text(descriptor.label, self.encoding)
                if type_name in {"UnitUInt8", "UnitUInt16", "UnitUInt32", "UnitUInt64"}:
                    labels = tuple(_saved_text(item.label, self.encoding) for item in descriptor.values)
                    if None not in labels:
                        enum_labels = labels
                elif type_name == "Cluster":
                    children = [self.types.consolidated.getFlatType(client.index)
                                if 0 <= client.index < self.flat_type_count else None
                                for client in descriptor.clients]
                    record_fields = tuple((_saved_text(child.label, self.encoding), getattr(child.fullType(), "name", None))
                                          if child is not None else (None, None) for child in children)
        box = _bounds(item, "termBounds", location)
        zero_items = None
        padded_scalar_zero = False
        array_element = None
        if kind(item) == "bDConstDCO":
            display = field(item, "ddo", location, optional=True)
            if display is not None:
                box = _bounds(display, "bounds", location) or box
                value = _blob(field(item, "ConstValue", location, optional=True))
                if type_name == "Array":
                    array_element = empty_numeric_array_element(self.types, item, display, location)
                if value in (bytes(3), bytes(5)):
                    zero_items = _zero_ring_item_table(display, location)
                if self.major_version == 20 and (type_name, kind(display), value) in (
                        ("NumUInt32", "stdNum", bytes(5)), ("Boolean", "stdBool", bytes(2))):
                    display_id = _number(display, "typeDesc", location)
                    if display_id and 1 <= display_id <= self.types.slice.tdCount:
                        display_td = self.types.consolidated.getTopType(self.types.slice.indexShift + display_id - 1)
                        padded_scalar_zero = display_td is not None and display_td.fullType() == descriptor.fullType()
        result = Binding(uid, kind(item), resource, None, _number(item, "objFlags", location), type_id, type_name,
                         type_code, type_label, enum_labels, record_fields, box,
                         self._uids(item, "termList", location),
                         _fields(item, BINDING_FIELDS, location), zero_items, padded_scalar_zero, array_element)
        self.bindings[uid] = result
        return result

    def signal(self, item, diagram):
        uid = identity(item, "BDHb")
        location = f"BDHb/uid:{uid}"
        require(uid not in self.signals and kind(item) == "signal", "source_signal",
                "A signal is repeated or has the wrong class.", location)
        endpoints = tuple(identity(child, location) for child in _members(item, "termList", location))
        # Some producers save partially connected or non-projectable wires. Keep
        # their ordered UID list, and do not manufacture any oriented endpoint.
        sources = []
        targets = []
        for endpoint in endpoints:
            terminal = self.terminals.get(endpoint)
            if terminal is None or terminal.diagram != diagram:
                self.issues.append(Issue("signal_endpoint_scope", uid, (endpoint,)))
            elif terminal.direction == "source":
                sources.append(endpoint)
            elif terminal.direction == "sink":
                targets.append(endpoint)
        state = _number(item, "state", location)
        usable = (len(sources) == 1 and bool(targets) and
                  len(set(endpoints)) == len(endpoints) == len(sources) + len(targets))
        def coerced_divide_input(term_uid):
            terminal = self.terminals[term_uid]
            node = self.nodes[terminal.owner]
            binding = self.bindings.get(terminal.binding)
            return ((terminal.flags or 0) & 0x40000 and binding and binding.kind == "parm" and
                    binding.type_kind == "NumInt32" and node.kind == "prim" and
                    tuple(node.fields[k].value if k in node.fields else None for k in ("primResID", "primIndex"))
                    == (1053, 106))
        if state not in (1, 33) and not (state in (17, 49) and any(coerced_divide_input(t) for t in targets)):
            self.issues.append(Issue("signal_state_unqualified", uid, endpoints))
        if not usable:
            self.issues.append(Issue("signal_orientation_unresolved", uid, endpoints))
        self.signals[uid] = Signal(uid, diagram, endpoints, sources[0] if usable else None,
                                   tuple(targets) if usable else (), state, _number(item, "lastSignalKind", location),
                                   _blob(field(item, "compressedWireTable", location, optional=True)))

    def select_regions(self, node):
        fields = node.fields
        selector_field = fields.get("caseSelDCO")
        selector = selector_field.uid if selector_field is not None else None
        displayed_text = _member(_member(fields.get("selString"), "textRec"), "text")
        displayed_label = _saved_text(displayed_text.payload, self.encoding) if displayed_text is not None else None

        def region(index):
            if index is None or index < 0:
                return None
            if index < len(node.regions):
                return node.regions[index]
            self.issues.append(Issue("region_index_unresolved", node.uid))
            return None

        rules = []
        ranges = fields.get("SelectRangeArray32")
        for row in ranges.children if ranges is not None else ():
            target = region(_saved_integer(_member(row, "diagramIdx")))
            parameters = tuple(_saved_integer(_member(row, name))
                               for name in ("start", "end", "startRangeType", "endRangeType"))
            if target is None or None in parameters:
                self.issues.append(Issue("case_range_unresolved", node.uid))
            else:
                rules.append(CaseRange(target, *parameters))
        # Keep label bytes in NI's index order, independent of diagram order
        # and of whichever case happens to be visible in the editor.
        string_table = ()
        strings = fields.get("SelectStringArray")
        selector_flags = fields.get("SelectSelLabFlags")
        if (strings and strings.children and selector_flags and selector_flags.payload == b"\x10\xd4"
                and _saved_integer(fields.get("SelectNRightType")) == len(rules)
                and all(row.tag == "SL__arrayElement" and not row.children for row in strings.children)):
            labels = tuple(_saved_text(row.payload, self.encoding) for row in strings.children)
            if all(label is not None for label in labels) and len(set(labels)) == len(labels):
                string_table = labels
        default = _saved_integer(fields.get("defaultDiag" if node.kind == "commentNode" else "SelectDefaultCase"))
        return RegionSelection(node.uid, selector, tuple(rules), default, region(default),
                               region(_saved_integer(fields.get("activeDiag"))),
                               region(_saved_integer(fields.get("dIdx"))), displayed_label.strip() if displayed_label else None,
                               string_table)

    def finish(self, root):
        # Ownership derives from explicit boundary lists, or from an interface
        # terminal on a structure and another terminal in one of its regions.
        owners = {}
        for node in self.nodes.values():
            for binding in node.boundary_bindings:
                if binding not in self.bindings or binding in owners and owners[binding] != node.uid:
                    self.issues.append(Issue("boundary_owner_unresolved", node.uid, (binding,)))
                else:
                    owners[binding] = node.uid
        inferred = {}
        for terminal in self.terminals.values():
            binding = self.bindings.get(terminal.binding)
            if binding is None or binding.uid in owners or not binding.terminals:
                continue
            node = self.nodes[terminal.owner]
            if any(self.terminals.get(end) is not None and self.terminals[end].diagram in node.regions
                   for end in binding.terminals):
                inferred.setdefault(binding.uid, set()).add(node.uid)
        for binding, candidates in inferred.items():
            if len(candidates) == 1:
                owners[binding] = next(iter(candidates))
            else:
                self.issues.append(Issue("boundary_owner_unresolved", binding, tuple(sorted(candidates))))
        owner_members = defaultdict(list)
        for uid, binding in self.bindings.items():
            if uid in owners:
                self.bindings[uid] = replace(binding, owner=owners[uid])
                owner_members[owners[uid]].append(uid)
            missing = tuple(end for end in binding.terminals if end not in self.terminals or self.terminals[end].binding != uid)
            if missing:
                self.issues.append(Issue("binding_terminals_unresolved", uid, missing))
        for uid, members in owner_members.items():
            node = self.nodes[uid]
            self.nodes[uid] = replace(node, boundary_bindings=tuple(dict.fromkeys((*node.boundary_bindings, *members))))
        sink_signals = defaultdict(list)
        for signal in self.signals.values():
            for target in signal.targets:
                sink_signals[target].append(signal.uid)
        for target, signals in sink_signals.items():
            if len(signals) > 1:
                self.issues.append(Issue("signal_multiple_drivers", target, tuple(signals)))
                for uid in signals:
                    self.signals[uid] = replace(self.signals[uid], source=None, targets=())
        for uid, item in self.definitions.items():
            category = kind(item)
            directory = self.diagrams if category == "diag" else self.terminals if category in TERMINAL_CLASSES else None
            if directory is not None and uid not in directory:
                self.issues.append(Issue("source_object_unattached", uid))
        selections = {node.uid: self.select_regions(node) for node in self.nodes.values()
                      if node.regions and node.kind in {"select", "commentNode"}}
        return SourceDiagram(root, self.diagrams, self.nodes, self.terminals, self.bindings, self.signals,
                             selections, tuple(self.issues))


def decode_source_diagram(vi):
    """Read a bounded, decoded BDHb heap. Return topology and explicit unresolved evidence."""
    reader = _Reader(vi)
    root = reader.diagram(field(heap_root(vi, b"BDHb"), "root", "BDHb"))
    return reader.finish(root)


def merge_error_ports(graph, node):
    """Map verified first-error scalar inputs to the native Retain First Error ports."""
    default_fields = {"termList", "bounds", "shortCount", "clumpNum", "dcoList", "dcoAgg"}
    if node.kind != "mergeErrors" or set(node.field_names) - default_fields:
        return None
    parameters = node.fields.get("dcoList")
    result = node.fields.get("dcoAgg")
    if parameters is None or result is None or not 2 <= len(parameters.children) <= 16:
        return None
    terms = [graph.terminals[uid] for uid in node.terminals]
    by_binding = {term.binding: term for term in terms}
    order = tuple(field.uid for field in parameters.children)
    if (len(by_binding) != len(terms) or len(set(order)) != len(order) or result.uid in order or
            set(by_binding) != {*order, result.uid}):
        return None
    schema = (("status", "Boolean"), ("code", "NumInt32"), ("source", "String"))
    for term in terms:
        binding = graph.bindings.get(term.binding)
        if binding is None or binding.kind != "mergeErrorsDCO" or binding.type_kind != "Cluster" or binding.record_fields != schema:
            return None
    incoming = [by_binding[uid] for uid in order]
    output = by_binding[result.uid]
    if output.direction != "source" or any(term.direction != "sink" for term in incoming):
        return None
    anchors = [graph.bindings[uid].bounds for uid in order]
    if None in anchors or any(a[1] >= b[1] for a, b in zip(anchors, anchors[1:])):
        return None
    ports = {term.uid: name for term, name in zip(incoming, ("a", "b", *(f"input_{i}" for i in range(3, 17))))}
    ports[output.uid] = "error_out"
    return ports
