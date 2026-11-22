# SPDX-License-Identifier: Apache-2.0
"""Project decoded LabVIEW structures into editable FROG Draft regions."""

from dataclasses import dataclass
import json

from import_labview_profile import bounds, convert_constant, require


OPERATIONS = {
    (1300, 131): ("frog.studio.structures.tick_count", "structures.tick_count", "u32", ()),
    (1051, 104): ("frog.core.sub", "numeric.subtract", "u32", ((2, "a"), (1, "b"))),
    (1052, 105): ("frog.core.mul", "numeric.multiply", "f64", ((1, "a"), (2, "b"))),
}


def boundary_port(tunnel, direction):
    return f"tunnel:{tunnel}:{direction}"


def wire(id_, start, end):
    return {"id": id_, "from": {"node": start.node, "port": start.name},
            "to": {"node": end.node, "port": end.name}}


def operation(id_, type_, palette, **configuration):
    return {"id": id_, "kind": "primitive", "type": type_, "palette_catalog_id": palette,
            "label_visible": False, **configuration}


def draft_diagram():
    return {"zoom_percent": 100, "nodes": [], "edges": []}


def structure(id_, kind):
    type_ = "structures." + kind
    return {"id": id_, "kind": "structure", "type": "frog.studio." + type_,
            "palette_catalog_id": type_, "structure_type": kind, "label_visible": False,
            "boundary": {"inputs": [], "outputs": []}, "structure_tunnels": [], "regions": []}


def declaration(uid, source):
    dtype = source.dtype
    role = source.role
    origin_x = 32 if role == "control" else 350
    id_ = f"lv-widget-{uid}"
    if dtype.startswith("array:"):
        element = dtype.removeprefix("array:")
        require(element == "i32" and source.value == [], "panel_array",
                "This profile requires the intrinsic empty default of a dynamic rank-one I32 Array.", f"FPHb/uid:{uid}")
        element_class = f"frog.widgets.numeric_{role}"
        template = {"id": f"{id_}.element", "classId": element_class, "role": role,
                    "valueType": element, "bounds": bounds(0, 0, 92, 30), "state": {"value": 0},
                    "props": {"label.visible": False, "data_type.representation": element}}
        widget = {"id": id_, "classId": f"frog.widgets.array_{role}", "role": role, "label": source.label,
                  "valueType": f"frog.array<{element},rank=1>", "bounds": bounds(origin_x, 0, 210, 55),
                  "props": {"dimensions.rank": 1, "dimensions.shape": [0], "dimensions.allow_resize": True,
                            "index_display.visible": True, "viewport.first_indices": [0],
                            "viewport.visible_counts": [1], "viewport.orientation": "vertical",
                            "label.visible": True, "element.class_id": element_class,
                            "element.value_type": element, "element.role": role, "element.template": template}}
    else:
        family = {"bool": "boolean", "string": "string"}.get(dtype, "numeric")
        widget = {"id": id_, "classId": f"frog.widgets.{family}_{role}", "role": role, "label": source.label,
                  "valueType": dtype, "bounds": bounds(origin_x, 0, 152, 32),
                  "props": {"label.visible": True, "text.value" if family == "string" else "value": source.value}}
        if family == "numeric":
            widget["props"]["data_type.representation"] = dtype
        if family != "string":
            widget["state"] = {"value": source.value}
    return widget


def empty_array_constant(id_, element):
    return {"id": id_, "kind": "primitive", "type": "frog.studio.numeric.numeric_array_constant",
            "value_type": element, "label_visible": False, "text": "0",
            "array_constant": {"rank": 1, "shape": [0], "values": [], "first_indices": [0],
                               "visible_counts": [1], "cell_size": {"width": 36, "height": 27}}}


def tunnel_type(dtype):
    return f"array<{dtype.removeprefix('array:')}>" if dtype.startswith("array:") else dtype


@dataclass(frozen=True)
class Connector:
    node: str
    name: str
    dtype: str
    source: bool


class Projection:
    def __init__(self, graph):
        self.graph = graph
        self.diagrams = {uid: draft_diagram() for uid in graph.diagrams}
        self.ports = {}
        self.mapped_widgets = set()
        self.source_nodes = []
        self.source_edges = []
        self.adaptations = []
        self.sequence_carries = set()
        self.boundaries = set()

    def node(self, diagram, uid, payload, source_bounds=None, adaptation=None):
        self.diagrams[diagram]["nodes"].append(payload)
        record = {"id": payload["id"], "uid": uid, "diagram_uid": diagram}
        if source_bounds is not None:
            record["source_bounds"] = source_bounds
        if adaptation:
            record["adaptation"] = adaptation
        self.source_nodes.append(record)

    def port(self, uid, node, name, dtype, source):
        terminal = self.graph.terms[uid]
        resource = f"BDHb/uid:{uid}"
        require(uid not in self.ports, "terminal_duplicate", "Terminal already has a projected port.", resource)
        require(terminal.source == source, "terminal_direction",
                "The saved terminal direction differs from the modelled boundary face.", resource)
        self.ports[uid] = Connector(node, name, dtype, source)
        return self.ports[uid]

    def add_adaptation_edge(self, diagram, id_, origin, target):
        self.diagrams[diagram]["edges"].append(wire(id_, origin, target))

    def shift_registers(self, source, target):
        location = f"BDHb/uid:{source.uid}"
        body, = source.regions
        members = {item.uid: item for item in self.graph.boundaries.values() if item.owner == source.uid}
        owned = {kind: [item for item in members.values() if item.kind == kind]
                 for kind in ("lCnt", "lMax", "lSR", "rSR")}
        require(sum(map(len, owned.values())) == len(members) and len(owned["lCnt"]) == len(owned["lMax"]) == 1
                and len(owned["lSR"]) == len(owned["rSR"]) <= 8, "for_boundary",
                "The qualified For loop uses an index, a count and at most eight simple paired shift registers.", location)
        count = owned["lMax"][0]
        index = owned["lCnt"][0]
        require(count.dtype == index.dtype == "i32" and count.outer is not None and index.outer is None,
                "for_indices", "This profile requires the original signed 32-bit count and index terminals.", location)
        drivers = [self.graph.terms[signal.source] for signal in self.graph.diagrams[source.diagram].signals
                   if count.outer in signal.targets]
        require(len(drivers) == 1 and drivers[0].binding_kind == "bDConstDCO" and drivers[0].dtype == "i32"
                and type(drivers[0].value) is int and drivers[0].value >= 0, "for_count_semantics",
                "This profile requires a nonnegative I32 constant for N to preserve LabVIEW's loop-entry rules.", location)
        self.boundaries.update((count.uid, index.uid))
        target["structure_terminals"] = {
            "count": {"type": "i64", "outer_visible": True, "exposed_in_body": True, "read_only": True,
                      "role": "count", "outer_port": "count", "body_port": "count_value"},
            "index": {"type": "i64", "outer_visible": False, "exposed_in_body": True, "read_only": True,
                      "role": "index", "body_port": "iteration"},
        }
        count_cast = f"lv-count64-{count.uid}"
        self.node(source.diagram, count.uid, operation(count_cast, "frog.studio.numeric.conversion.to_i64",
                                                      "numeric.conversion.to_i64"), adaptation="signed LabVIEW I32 count to native I64 count")
        self.port(count.outer, count_cast, "value", "i32", False)
        self.add_adaptation_edge(source.diagram, f"lv-count-edge-{count.uid}", Connector(count_cast, "result", "i64", True),
                                 Connector(target["id"], "count", "i64", False))
        # GS exposes both index and count value as I64. LV retains I32 in the body.
        for boundary, name in ((index, "iteration"), (count, "count_value")):
            if not self.is_wired(boundary.inner):
                continue
            cast = f"lv-body32-{boundary.uid}"
            self.node(body, boundary.uid, operation(cast, "frog.studio.numeric.conversion.to_i32",
                                                    "numeric.conversion.to_i32"), adaptation=f"{name} retains LabVIEW I32")
            self.port(boundary.inner, cast, "result", "i32", True)
            self.add_adaptation_edge(body, f"lv-index-edge-{boundary.uid}", Connector(target["id"], name, "i64", True),
                                     Connector(cast, "value", "i64", False))

        for left in owned["lSR"]:
            right = members.get(left.mate)
            require(right and right.kind == "rSR" and right.mate == left.uid and right.dtype == left.dtype
                    and left.outer is not None and right.outer is not None and right.uid not in self.boundaries,
                    "shift_pair", "The exact shift-register faces and counterpart identities do not match.", location)
            self.boundaries.update((left.uid, right.uid))
            pair = f"lv-shift-{left.uid}-{right.uid}"
            left_name, right_name = f"lv-tunnel-{left.uid}", f"lv-tunnel-{right.uid}"
            for item, port_id, side in ((left, left_name, "left"), (right, right_name, "right")):
                output = side == "right"
                target["boundary"]["outputs" if output else "inputs"].append(
                    {"id": port_id, "type": tunnel_type(item.dtype), "shift_register_pair_id": pair})
                target["structure_tunnels"].append({"id": port_id, "side": side,
                    "normalized_position": 0.45 + 0.06 * (len(target["boundary"]["inputs"]) - 1),
                    "value_type": tunnel_type(item.dtype), "output_from_structure": output, "shift_register_pair_id": pair})
            init_port = boundary_port(left_name, "input")
            result_port = boundary_port(right_name, "output")
            self.port(left.inner, target["id"], boundary_port(left_name, "output"), left.dtype, True)
            self.port(right.inner, target["id"], boundary_port(right_name, "input"), right.dtype, False)
            self.port(right.outer, target["id"], result_port, right.dtype, True)
            if self.is_wired(left.outer):
                self.port(left.outer, target["id"], init_port, left.dtype, False)
            else:
                self.loaded_register(source.diagram, left, right, target["id"], init_port, result_port)

    def loaded_register(self, diagram, left, right, owner, init_port, result_port):
        location = f"BDHb/uid:{left.uid}"
        require(left.dtype == "array:i32", "shift_lifetime",
                "Only the loaded-instance lifetime of a dynamic rank-one I32 Array register is qualified here.", location)
        state = f"lv-loaded-state-{left.uid}"
        default = f"lv-loaded-default-{left.uid}"
        self.node(diagram, left.uid, operation(state, "frog.studio.structures.feedback_node",
                                              "structures.feedback_node", feedback_delay=1,
                                              feedback_initialization="compile_or_load", feedback_initialization_explicit=True),
                  adaptation="loaded-instance lifetime of an unwired LabVIEW shift register")
        self.node(diagram, left.uid, empty_array_constant(default, "i32"), adaptation="intrinsic empty I32 Array on load")
        for suffix, start, end in (
            ("default", Connector(default, "value", left.dtype, True), Connector(state, "initial_value", left.dtype, False)),
            ("initialize", Connector(state, "result", left.dtype, True), Connector(owner, init_port, left.dtype, False)),
            ("retain", Connector(owner, result_port, left.dtype, True), Connector(state, "value", left.dtype, False)),
        ):
            self.add_adaptation_edge(diagram, f"lv-state-{suffix}-{left.uid}", start, end)
        self.adaptations.append({"kind": "loaded_shift_register", "source_uids": [left.uid, right.uid],
                                 "frog_state_node": state, "initial_value": [],
                                 "lifetime": "loaded_instance", "state_commit": "completed_loop"})

    def is_wired(self, terminal):
        return any(terminal == signal.source or terminal in signal.targets
                   for signal in self.graph.diagrams[self.graph.terms[terminal].diagram].signals)

    def sequence(self, source, target):
        location = f"BDHb/uid:{source.uid}"
        frames = tuple(self.graph.nodes[uid] for uid in source.frames)
        require(1 <= len(frames) <= 16 and all(frame.kind == "sequenceFrame" and len(frame.regions) == 1 for frame in frames),
                "sequence_order", "The source Flat Sequence needs one decoded diagram per ordered frame.", location)
        frame_by_uid = {frame.uid: f"frame-{frame.uid}" for frame in frames}
        position = {frame.uid: index for index, frame in enumerate(frames)}
        members = {item.uid: item for item in self.graph.boundaries.values() if item.owner in frame_by_uid}
        root_signals = self.graph.diagrams[source.diagram].signals
        for transfer in members.values():
            if transfer.uid in self.boundaries:
                continue
            mate = members.get(transfer.mate)
            require(mate and transfer.uid != mate.uid and mate.mate == transfer.uid
                    and transfer.kind == mate.kind == "flatSeqTun" and transfer.dtype == mate.dtype
                    and transfer.outer is not None and mate.outer is not None,
                    "sequence_tunnel_pair", "A sequence tunnel has no exact typed counterpart.", location)
            origin, destination = sorted((transfer, mate), key=lambda item: position[item.owner])
            carries = [signal for signal in root_signals if signal.source == origin.outer and signal.targets == (destination.outer,)]
            require(position[origin.owner] < position[destination.owner] and len(carries) == 1,
                    "sequence_tunnel_connection", "The source wire does not connect the exact forward frame faces.", location)
            tunnel = f"lv-seq-{origin.uid}-{destination.uid}"
            target["boundary"]["inputs"].append({"id": tunnel, "type": tunnel_type(origin.dtype)})
            target["structure_tunnels"].append({"id": tunnel, "side": "left", "normalized_position": 0.20,
                "value_type": tunnel_type(origin.dtype), "output_from_structure": False,
                "studio_sequence_outer_frame": frame_by_uid[origin.owner], "studio_sequence_frame": frame_by_uid[destination.owner]})
            self.port(origin.inner, target["id"], boundary_port(tunnel, "input"), origin.dtype, False)
            self.port(destination.inner, target["id"], boundary_port(tunnel, "output"), destination.dtype, True)
            self.boundaries.update((origin.uid, destination.uid))
            signal = carries[0]
            self.sequence_carries.add(signal.uid)
            self.source_edges.append({"signal_uid": signal.uid, "producer_uid": origin.outer,
                                      "consumer_uid": destination.outer, "sequence_tunnel": tunnel})
        target["regions"] = [{"id": frame_by_uid[frame.uid], "diagram": self.diagrams[frame.regions[0]],
                              "studio_title": "", "studio_subdiagram_label_text": ""} for frame in frames]
        return tuple(frame.regions[0] for frame in frames)

    def primitive(self, member):
        location = f"BDHb/uid:{member.uid}"
        signature = OPERATIONS.get(member.primitive)
        require(signature is not None, "primitive", f"Unqualified LabVIEW primitive {member.primitive}.", location)
        target_kind, palette, dtype, parameters = signature
        terms = [self.graph.terms[uid] for uid in member.terms]
        outputs = [term for term in terms if term.source]
        inputs = [term for term in terms if not term.source]
        require(len(outputs) == 1 and len(inputs) == len(parameters) and all(term.dtype == dtype for term in terms),
                "primitive_signature", "Saved terminals do not match the qualified operation's exact type and directions.", location)
        output = outputs[0]
        require((output.binding_kind, output.override) == (("parm", None) if not parameters else ("overridableParm", 3)),
                "primitive_result", "The saved primitive result configuration is outside this profile.", location)
        id_ = f"lv-operation-{member.uid}"
        self.node(member.diagram, member.uid, operation(id_, target_kind, palette), member.bounds)
        self.port(output.uid, id_, "count" if palette == "structures.tick_count" else "result", dtype, True)
        if inputs:
            by_parameter = {term.parameter: term for term in inputs if term.binding_kind == "parm"}
            require(by_parameter.keys() == {parameter for parameter, _ in parameters}, "primitive_input_order",
                    "The saved parameter identities do not match the qualified operation.", location)
            for parameter, name in parameters:
                self.port(by_parameter[parameter].uid, id_, name, dtype, False)

    def append(self, member):
        location = f"BDHb/uid:{member.uid}"
        terms = [self.graph.terms[uid] for uid in member.terms]
        outputs = [term for term in terms if term.source]
        inputs = [term for term in terms if not term.source]
        by_binding = {term.binding_uid: term for term in inputs if term.binding_kind == "aBuildDCO"}
        require(member.flags == 0x10000 and len(outputs) == 1 and outputs[0].dtype == "array:i32"
                and outputs[0].binding_uid == member.result_binding and outputs[0].binding_flags == 1
                and len(inputs) == len(by_binding) == len(member.operand_bindings) == 2
                and by_binding.keys() == set(member.operand_bindings),
                "build_array_signature", "Only the qualified dynamic rank-one I32 append profile is admitted.", location)
        first, second = (by_binding[uid] for uid in member.operand_bindings)
        require((first.dtype, first.binding_flags) == ("array:i32", 0x50000)
                and (second.dtype, second.binding_flags) == ("i32", 0), "build_array_rank",
                "The source Build Array must append the scalar to the incoming rank-one I32 data.", location)
        id_ = f"lv-build-array-{member.uid}"
        self.node(member.diagram, member.uid, operation(id_, "frog.studio.array.build", "array.build",
                  build_array_inputs=2, build_array_concatenate=False), member.bounds)
        for term, name in ((first, "array_a"), (second, "array_b"), (outputs[0], "result")):
            self.port(term.uid, id_, name, term.dtype, term.source)

    def root_terminal(self, term):
        location = f"BDHb/uid:{term.uid}"
        if term.binding_kind == "fPDCO":
            widget = self.graph.widgets.get(term.binding_uid)
            require(widget and term.binding_uid not in self.mapped_widgets and term.dtype == widget.dtype
                    and term.source == (widget.role == "control"), "panel_terminal",
                    "The diagram terminal must identify one panel object with the same role and type.", location)
            self.mapped_widgets.add(term.binding_uid)
            id_ = f"lv-terminal-{term.uid}"
            self.node(term.diagram, term.uid, {"id": id_, "kind": "widget_value", "type": term.dtype,
                      "widget": f"lv-widget-{term.binding_uid}", "label_visible": True}, term.bounds)
            self.port(term.uid, id_, "value", term.dtype, term.source)
        elif term.binding_kind == "bDConstDCO":
            require(term.source and term.value is not None and not term.dtype.startswith("array:"), "diagram_constant",
                    "The constant must have one decoded scalar value and a source port.", location)
            payload = convert_constant(term.uid, term.dtype, term.value, 0, 0)
            self.node(term.diagram, term.uid, payload, term.bounds)
            self.port(term.uid, payload["id"], "value", term.dtype, True)
        else:
            require(term.binding_uid in self.boundaries, "diagram_terminal",
                    f"Unsupported saved terminal class '{term.binding_kind}'.", location)

    def project_diagram(self, uid):
        for member_uid in self.graph.diagrams[uid].nodes:
            member = self.graph.nodes[member_uid]
            if member.kind == "flatSequence":
                projected = structure(f"lv-sequence-{member.uid}", "sequence_structure")
                self.node(uid, member.uid, projected, member.bounds)
                for region in self.sequence(member, projected):
                    self.project_diagram(region)
            elif member.kind == "forLoop":
                require(len(member.regions) == 1, "for_regions", "One source For body is required.", f"BDHb/uid:{member.uid}")
                projected = structure(f"lv-for-{member.uid}", "for_loop")
                self.node(uid, member.uid, projected, member.bounds)
                body, = member.regions
                projected["regions"] = [{"id": "body", "diagram": self.diagrams[body]}]
                self.shift_registers(member, projected)
                self.project_diagram(body)
            elif member.kind == "prim":
                self.primitive(member)
            elif member.kind == "aBuild":
                self.append(member)
            elif member.kind == "sRN":
                for term_uid in member.terms:
                    self.root_terminal(self.graph.terms[term_uid])
            else:
                require(False, "diagram_node", f"Unsupported diagram class '{member.kind}'.", f"BDHb/uid:{member.uid}")

        for signal in self.graph.diagrams[uid].signals:
            if signal.uid in self.sequence_carries:
                continue
            location = f"BDHb/uid:{signal.uid}"
            origin = self.ports.get(signal.source)
            require(origin and origin.source, "signal_source", "The source wire has no projected producer.", location)
            for consumer in signal.targets:
                target = self.ports.get(consumer)
                require(target and not target.source and origin.dtype == target.dtype, "signal_target",
                        "The wire has no exact typed projected consumer.", location)
                id_ = f"lv-signal-{signal.uid}-{consumer}"
                self.diagrams[uid]["edges"].append(wire(id_, origin, target))
                self.source_edges.append({"id": id_, "signal_uid": signal.uid,
                                          "producer_uid": signal.source, "consumer_uid": consumer})


def arrange(diagram, left, top):
    """Arrange editable nodes by signal dependencies and retain roomy region bounds."""
    nodes = {item["id"]: item for item in diagram["nodes"]}
    parents = {id_: set() for id_ in nodes}
    for edge in diagram["edges"]:
        origin, target = edge["from"]["node"], edge["to"]["node"]
        feedback_write = (target in nodes and nodes[target]["type"] == "frog.studio.structures.feedback_node"
                          and edge["to"]["port"] == "value")
        if origin in nodes and target in nodes and origin != target and not feedback_write:
            parents[target].add(origin)
    done = set()
    x = left + 28
    ceiling = top + 180
    while len(done) < len(nodes):
        group = [id_ for id_, inputs in parents.items() if id_ not in done and inputs <= done]
        require(group, "diagram_cycle", "The local chart contains an unresolved non-state cycle.", "BDHb")
        y, width = top + 70, 0
        for id_ in group:
            node = nodes[id_]
            if node["kind"] == "structure":
                region_left = x
                region_bottom = y
                for region in node["regions"]:
                    changed_right, changed_bottom = arrange(region["diagram"], region_left, y)
                    region_width = changed_right - region_left
                    if node["structure_type"] == "sequence_structure":
                        region["studio_width"] = region_width
                    region_left += region_width
                    region_bottom = max(region_bottom, changed_bottom)
                size = bounds(x, y, region_left - x, max(180, region_bottom + 24 - y))
            else:
                dimension = (66, 30) if node["kind"] == "widget_value" else (88, 31) if "constant" in node["type"] else (48, 39)
                size = bounds(x, y, *dimension)
            node["layout"] = size
            width = max(width, size["width"])
            y += size["height"] + 35
            ceiling = max(ceiling, y + 10)
        x += width + 74
        done.update(group)
    return max(left + 180, x - 32), ceiling


def project(graph, source, source_sha256):
    projection = Projection(graph)
    projection.project_diagram(graph.root)
    require(projection.mapped_widgets == graph.widgets.keys() and projection.boundaries == graph.boundaries.keys(),
            "diagram_coverage", "Some decoded panel terminals or structure boundaries were not projected.", "BDHb")
    document_diagram = projection.diagrams[graph.root]
    arrange(document_diagram, 70, 50)
    widgets, widget_mapping = [], []
    positions = {"control": 40, "indicator": 40}
    for uid, source_widget in graph.widgets.items():
        widget = declaration(uid, source_widget)
        widget["bounds"]["y"] = positions[source_widget.role]
        positions[source_widget.role] += widget["bounds"]["height"] + 45
        widget["props"]["data.default"] = json.dumps(widget, ensure_ascii=False)
        widgets.append(widget)
        widget_mapping.append({"id": widget["id"], "uid": uid, "source_bounds": source_widget.bounds,
                               "role": source_widget.role, "value_type": source_widget.dtype, "initial_value": source_widget.value})
    document = {"format": "frog.document.draft", "draft_revision": 3, "language": "FROG", "title": source.stem,
                "metadata": {"name": source.stem, "description": f"LabVIEW structured dataflow. Source SHA-256 {source_sha256}."},
                "frontPanel": {"widgets": widgets}, "diagram": document_diagram,
                "storage": {"version": 1, "array_mode": "automatic"}}
    mapping = {"profile": "labview-structured-dataflow-v1", "source_sha256": source_sha256,
               "widgets": widget_mapping, "nodes": projection.source_nodes, "edges": projection.source_edges,
               "adaptations": projection.adaptations,
               "scope": "Editable local dataflow. Loaded shift register state is retained after a completed loop."}
    return document, mapping
