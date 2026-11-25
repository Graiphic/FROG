# SPDX-License-Identifier: Apache-2.0
"""Project decoded LabVIEW structures into editable FROG Draft regions."""

from dataclasses import dataclass
import json

from import_labview_profile import bounds, convert_constant, require


# Slot numbers are NI parameter identities. Optional, unwired NI parameters
# can look like output terminals in termList. The DCO carries their actual direction.
SCALAR_PORTS = {
    (1903, 210): ("numeric.add_array_elements", ("result",), ("value",)),
    (1156, 61): ("numeric.conversion.to_u64", ("result",), ("value",)),
    (1147, 63): ("numeric.conversion.to_dbl", ("result",), ("value",)),
    (1200, 72): ("math.trigonometric.sin", ("result",), ("x",)),
    (1503, 119): ("string.subset", ("result",), ("length", "offset", "value")),
    (1502, 26): ("string.length", ("result",), ("value",)),
    (1906, 214): ("array.rotate_1d", ("result",), ("array", "offset")),
    (1901, 208): ("array.search_1d", ("index",), ("start_index", "value", "array")),
    (1145, 60): ("numeric.conversion.to_u32", ("result",), ("value",)),
    (1143, 58): ("numeric.conversion.to_u8", ("result",), ("value",)),
    (1081, 101): ("numeric.data_manipulation.logical_shift", ("result",), ("x", "y")),
    (1063, 98): ("boolean.xor", ("result",), ("a", "b")),
    (1061, 94): ("boolean.and", ("result",), ("a", "b")),
    (1064, 29): ("boolean.not", ("result",), ("in",)),
    (1208, 80): ("math.trigonometric.atan", ("result",), ("x",)),
    (1188, 24): ("string.to_upper", ("result",), ("value",)),
    (1102, 109): ("comparison.equal", ("result",), ("y", "x")),
    (1111, 114): ("comparison.less", ("result",), ("y", "x")),
    (1167, 69): ("numeric.conversion.boolean_to_integer", ("result",), ("value",)),
    (1180, 14): ("string.conversion.number_to_decimal_string", ("result",), ("width", "value")),
    (1184, 20): ("string.conversion.decimal_string_to_number", ("result", "offset_past_number"),
                 ("default_value", "offset", "value")),
    (3913, 453): ("string.replace_substring", ("replaced_substring", "result"), ("length", "offset", "substring", "value")),
    (1516, 136): ("structures.select", ("result",), ("false", "select", "true")),
    (1108, 117): ("numeric.max_min", ("min", "max"), ("y", "x")),
    (1078, 32): ("path.from_string", ("result",), ("value",)),
    (1419, 193): ("path.compose", ("result",), ("name_or_relative_path", "base_path")),
    (1809, 139): ("array.size", ("size",), ("array",)),
    (1814, 146): ("array.conversion.number_to_boolean_array", ("array",), ("number",)),
    (1815, 147): ("array.conversion.boolean_array_to_number", ("number",), ("array",)),
    (1127, 118): ("numeric.in_range", ("in_range", "coerced"), ("lower_limit", "x", "upper_limit")),
    (1900, 207): ("array.reverse_1d", ("result",), ("array",)),
    (1169, 162): ("array.conversion.cluster_to_array", ("array",), ("cluster",)),
    (8201, 371): ("variant.to_variant", ("variant",), ("data",)),
    (1103, 110): ("comparison.greater_or_equal", ("result",), ("y", "x")),
}

# For these primitives NI mixes input and output indices in the serialized DCO list.
MIXED_PORTS = {
    (1907, 203): ("array.max_min", ("array", "max", "max_index", "min", "min_index"), {1, 2, 3, 4}),
    (1535, 128): ("string.match_pattern", ("value", "regular_expression", "offset", "before_substring",
                   "match_substring", "after_substring", "offset_past_match"), {3, 4, 5, 6}),
    (1420, 194): ("path.decompose", ("path", "stripped_path", "name"), {1, 2}),
    (1423, 197): ("path.to_string_array", ("path", "relative", "array"), {1, 2}),
}


OPERATIONS = {
    # Input type, result type and NI parameter indices from the saved function.
    # Binary primitive parameters 1 and 2 designate y and x respectively.
    (1300, 131): ("frog.studio.structures.tick_count", "structures.tick_count", "u32", "u32", ()),
    (1050, 103): ("frog.core.add", "numeric.add", "u32", "u32", ((2, "a"), (1, "b"))),
    (1051, 104): ("frog.core.sub", "numeric.subtract", "u32", "u32", ((2, "a"), (1, "b"))),
    (1052, 105): ("frog.core.mul", "numeric.multiply", "f64", "f64", ((1, "a"), (2, "b"))),
    (1053, 106): ("frog.core.div", "numeric.divide", "i32", "f64", ((2, "a"), (1, "b"))),
    (1057, 34): ("frog.studio.numeric.increment", "numeric.increment", "i32", "i32", ((1, "in"),)),
    (1201, 73): ("frog.studio.math.trigonometric.cos", "math.trigonometric.cos", "f64", "f64", ((1, "x"),)),
    (1110, 113): ("frog.studio.comparison.greater", "comparison.greater", "u32", "bool", ((2, "x"), (1, "y"))),
}


def matches_operation_types(primitive, inputs, output):
    _, _, dtype, result_type, parameters = OPERATIONS[primitive]
    if primitive in ((1050, 103), (1051, 104), (1052, 105)):
        if output not in ("i32", "u32", "u64", "f64"):
            return False
        dtype = result_type = output
    elif primitive == (1053, 106) and all(t == "f64" for t in inputs):
        dtype = "f64"
    return output == result_type and len(inputs) == len(parameters) and all(t == dtype for t in inputs)


def result_binding(primitive):
    # Arithmetic configuration is attached to the result, including when Divide
    # changes two integral inputs to a double. The trigonometric result is fixed.
    return ("overridableParm", 3) if primitive in ((1050, 103), (1051, 104), (1052, 105), (1053, 106), (1057, 34)) else ("parm", None)


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
    if source.waveform is not None:
        require(dtype == "array:f64" and role == "indicator", "panel_widget",
                "The Waveform Graph requires a rank-one double indicator.", f"FPHb/uid:{uid}")
        x, y, right, bottom = source.bounds
        return {"id": id_, "classId": "frog.widgets.waveform_graph", "role": role, "label": source.label,
                "valueType": "array<f64>", "bounds": bounds(x, y, right - x, bottom - y),
                "props": {"label.visible": True, "waveform_graph": {**source.waveform, "datatype": "array<f64>",
                          "typed_initial_data": source.value}}}
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


def empty_array_constant(id_, element, shape=(0,)):
    rank = len(shape)
    string = element == "string"
    return {"id": id_, "kind": "primitive", "type": "frog.studio.string.string_array_constant" if string
            else "frog.studio.numeric.numeric_array_constant", "value_type": element,
            "label_visible": False, "text": "" if string else "0",
            "array_constant": {"rank": rank, "shape": list(shape), "values": [], "first_indices": [0] * rank,
                               "visible_counts": [1] * rank, "cell_size": {"width": 36, "height": 27}}}


def append_ports(flags, operands, result, terminals):
    """Map an ordered LV rank-one array + scalar to the two native Build Array inputs."""
    if flags != 0x10000 or len(operands) != 2 or len(set((*operands, result))) != 3 or len(terminals) != 3:
        return None
    by_binding = {binding: (uid, dtype, config, output) for uid, binding, kind, dtype, config, output in terminals
                  if kind == "aBuildDCO"}
    if by_binding.keys() != {*operands, result}:
        return None
    first, second, aggregate = (by_binding[b] for b in (*operands, result))
    if (second[1] not in ("i32", "f64") or first[1:] != (f"array<{second[1]}>", 0x50000, False)
            or second[2:] != (0, False) or aggregate[1:] != (first[1], 1, True)):
        return None
    return {first[0]: "array_a", second[0]: "array_b", aggregate[0]: "result"}


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
                 for kind in ("lCnt", "lMax", "lSR", "rSR", "lpTun")}
        require(sum(map(len, owned.values())) == len(members) and len(owned["lCnt"]) == len(owned["lMax"]) == 1
                and len(owned["lSR"]) == len(owned["rSR"]) <= 8, "for_boundary",
                "The qualified For loop uses an index, a count and at most eight simple paired shift registers.", location)
        count = owned["lMax"][0]
        index = owned["lCnt"][0]
        require(count.dtype == index.dtype == "i32" and count.outer is not None and index.outer is None,
                "for_indices", "This profile requires the original signed 32-bit count and index terminals.", location)
        drivers = [self.graph.terms[signal.source] for signal in self.graph.diagrams[source.diagram].signals
                   if count.outer in signal.targets]
        require(len(drivers) == 1 and drivers[0].dtype == "i32", "for_count_semantics",
                "The For loop requires one signed I32 count source.", location)
        literal_count = drivers[0].value if drivers[0].binding_kind == "bDConstDCO" else None
        self.boundaries.update((count.uid, index.uid))
        for tunnel in owned["lpTun"]:
            indexed = tunnel.flags in (0x01000800, 0x01000801)
            output = bool(tunnel.flags & 1)
            require(tunnel.outer is not None and (indexed or tunnel.flags == 0x800 or
                    tunnel.flags == 0x801 and literal_count is not None and literal_count > 0),
                    "loop_tunnel_mode", "The For tunnel mode or its zero-iteration value is unverified.",
                    f"BDHb/uid:{tunnel.uid}")
            name = f"lv-tunnel-{tunnel.uid}"
            inner, outer = (self.graph.terms[uid] for uid in (tunnel.inner, tunnel.outer))
            require(outer.dtype == ("array:" + inner.dtype if indexed else inner.dtype),
                    "loop_tunnel_type", "The two For tunnel faces have incompatible types.", location)
            dtype = tunnel_type(inner.dtype if output else outer.dtype)
            mode = {"tunnel_mode": "indexing"} if indexed else {}
            target["boundary"]["outputs" if output else "inputs"].append({"id": name, "type": dtype, **mode})
            target["structure_tunnels"].append({"id": name, "side": "right" if output else "left",
                "normalized_position": 0.24, "value_type": dtype, "output_from_structure": output, **mode})
            self.port(inner.uid, target["id"], boundary_port(name, "input" if output else "output"), inner.dtype, not output)
            self.port(outer.uid, target["id"], boundary_port(name, "output" if output else "input"), outer.dtype, output)
            self.boundaries.add(tunnel.uid)
        target["structure_terminals"] = {
            "count": {"type": "i64", "outer_visible": True, "exposed_in_body": True, "read_only": True,
                      "role": "count", "outer_port": "count", "body_port": "count_value"},
            "index": {"type": "i64", "outer_visible": False, "exposed_in_body": True, "read_only": True,
                      "role": "index", "body_port": "iteration"},
        }
        count_cast = f"lv-count64-{count.uid}"
        self.node(source.diagram, count.uid, operation(count_cast, "frog.studio.numeric.conversion.to_i64",
                                                      "numeric.conversion.to_i64"), adaptation="signed LabVIEW I32 count to native I64 count")
        if literal_count is not None and literal_count >= 0:
            self.port(count.outer, count_cast, "value", "i32", False)
        else:
            clamp = f"lv-nonnegative-count-{count.uid}"
            zero = f"lv-count-zero-{count.uid}"
            self.node(source.diagram, count.uid, operation(clamp, "frog.studio.numeric.max_min", "numeric.max_min"),
                      adaptation="LabVIEW For treats a negative N as zero iterations")
            self.node(source.diagram, count.uid, dict(convert_constant(count.uid, "i32", 0, 0, 0), id=zero),
                      adaptation="zero lower bound on loop count")
            self.port(count.outer, clamp, "x", "i32", False)
            self.add_adaptation_edge(source.diagram, zero + "-edge", Connector(zero, "value", "i32", True),
                                     Connector(clamp, "y", "i32", False))
            self.add_adaptation_edge(source.diagram, clamp + "-edge", Connector(clamp, "max", "i32", True),
                                     Connector(count_cast, "value", "i32", False))
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
            if transfer.kind == "seqTun":
                require(transfer.mate is None and transfer.outer is not None and transfer.flags in (0x800, 0x801),
                        "sequence_tunnel", "Unexpected external sequence tunnel configuration.", location)
                output = bool(transfer.flags & 1)
                name = f"lv-tunnel-{transfer.uid}"
                dtype = tunnel_type(transfer.dtype)
                target["boundary"]["outputs" if output else "inputs"].append({"id": name, "type": dtype})
                target["structure_tunnels"].append({"id": name, "value_type": dtype, "output_from_structure": output,
                    "side": "right" if output else "left", "normalized_position": 0.4,
                    "studio_sequence_frame": frame_by_uid[transfer.owner]})
                self.port(transfer.outer, target["id"], boundary_port(name, "output" if output else "input"), transfer.dtype, output)
                self.port(transfer.inner, target["id"], boundary_port(name, "input" if output else "output"), transfer.dtype, not output)
                self.boundaries.add(transfer.uid)
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
        if member.primitive == (8100, 352):
            id_ = f"lv-operation-{member.uid}"
            op = "timestamp.format_date_time"
            self.node(member.diagram, member.uid, operation(id_, "frog.studio." + op, op), member.bounds)
            terms = {self.graph.terms[i].parameter or 0: self.graph.terms[i] for i in member.terms}
            require(len(member.terms) == 4 and terms.keys() == {0, 1, 2, 3}
                    and [terms[k].dtype for k in range(4)] == ["string", "bool", "f64", "string"]
                    and all(t.binding_kind == "parm" and bool(t.binding_flags & 1) == (k == 0)
                            for k, t in terms.items()), "timestamp_signature",
                    "Unrecognized Format Date/Time String signature.", location)
            for k, port in ((0, "date_time_string"), (1, "utc_format"), (3, "time_format_string")):
                t = terms[k]
                if self.is_wired(t.uid):
                    self.port(t.uid, id_, port, t.dtype, k == 0)
            seconds = terms[2]
            if self.is_wired(seconds.uid):
                converter = f"lv-timestamp-{seconds.uid}"
                convert = "timestamp.to_timestamp"
                self.node(member.diagram, member.uid, operation(converter, "frog.studio." + convert, convert),
                          adaptation="convert numeric LabVIEW epoch seconds to a Timestamp")
                self.port(seconds.uid, converter, "number", seconds.dtype, False)
                self.add_adaptation_edge(member.diagram, f"lv-time-{seconds.uid}",
                    Connector(converter, "time_stamp", "timestamp", True), Connector(id_, "time_stamp", "timestamp", False))
            return
        if member.primitive in ((2452, 578), (2454, 580)):
            insert = member.primitive == (2452, 578)
            names = {10: "element", 11: "set", 3: "set_out" if insert else "included"}
            if insert:
                names[2] = "already_included"
            terms = {self.graph.terms[i].parameter: self.graph.terms[i] for i in member.terms}
            require(len(member.terms) == len(names) and terms.keys() == names.keys()
                    and terms[11].dtype == f"set<{terms[10].dtype}>"
                    and terms[3].dtype == (terms[11].dtype if insert else "bool")
                    and (not insert or terms[2].dtype == "bool")
                    and all(t.binding_kind == "parm" and bool(t.binding_flags & 1) == (k < 10)
                            for k, t in terms.items()), "set_signature", "Inconsistent Set operation signature.", location)
            op = "collection.set." + ("insert" if insert else "contains")
            id_ = f"lv-operation-{member.uid}"
            self.node(member.diagram, member.uid, operation(id_, "frog.studio." + op, op), member.bounds)
            for k, t in terms.items():
                if self.is_wired(t.uid):
                    self.port(t.uid, id_, names[k], t.dtype, k < 10)
            return
        if member.uid in self.graph.resource_nodes:
            target, endpoints = self.graph.resource_nodes[member.uid]
            self.node(member.diagram, member.uid, target, member.bounds)
            for tid, (node, port) in endpoints.items():
                if self.is_wired(tid):
                    t = self.graph.terms[tid]
                    self.port(tid, node, port, t.dtype, t.source)
            return
        if member.primitive in ((8205,375), (8204,374)):
            get = member.primitive == (8205,375)
            slots = {0:"error_out", 2:"found" if get else None, 3:None if get else "result", 4:"error_in",
                     5:"default_value" if get else "value", 6:"name", 7:"variant"}
            if get:
                slots[1] = "value"
            terms = {self.graph.terms[i].parameter or 0:self.graph.terms[i] for i in member.terms}
            require(len(terms)==len(member.terms) and terms.keys()==slots.keys() and
                    terms[7].dtype=="variant" and terms[6].dtype=="string", "variant_signature",
                    "The saved Variant attribute signature is inconsistent.", location)
            op = "variant.get_attribute" if get else "variant.set_attribute"
            id_ = f"lv-operation-{member.uid}"
            self.node(member.diagram, member.uid, operation(id_, "frog.studio."+op,op), member.bounds)
            for slot, t in terms.items():
                if self.is_wired(t.uid):
                    require(slots[slot] is not None, "variant_output", "No native counterpart for this wired variant output.", location)
                    self.port(t.uid,id_,slots[slot],t.dtype,slot<=3)
            return
        if member.primitive in SCALAR_PORTS or member.primitive in MIXED_PORTS:
            if member.primitive in MIXED_PORTS:
                op, names, source_slots = MIXED_PORTS[member.primitive]
            else:
                op, outputs, inputs = SCALAR_PORTS[member.primitive]
                names = outputs + inputs
                source_slots = range(len(outputs))
            id_ = f"lv-operation-{member.uid}"
            terms = [self.graph.terms[uid] for uid in member.terms]
            slots = {t.parameter or 0:t for t in terms}
            require(len(slots) == len(terms) == len(names) and set(slots) == set(range(len(names))),
                    "primitive_signature", "The saved parameter list differs from the primitive signature.", location)
            for slot, name in enumerate(names):
                term = slots[slot]
                output = slot in source_slots
                result_override = member.primitive == (1815,147) and slot == 0
                require(term.binding_kind == ("overridableParm" if result_override else "parm")
                        and (not result_override or (term.override is None and term.dtype == "u32"))
                        and bool(term.binding_flags & 1) == output,
                        "primitive_signature", "The saved NI parameter direction is inconsistent.", location)
                if self.is_wired(term.uid):
                    self.port(term.uid, id_, name, term.dtype, output)
            type_ = ("frog.core." + op.split(".")[1] if op in ("boolean.and", "boolean.xor", "boolean.not")
                     else "frog.studio." + op)
            self.node(member.diagram, member.uid, operation(id_, type_, op), member.bounds)
            return
        if member.primitive == (1056, 108):
            # The persisted GS port named "quotient" is the R socket; "remainder" is IQ.
            ports = {(True, 0): "remainder", (True, 1): "quotient", (False, 2): "y", (False, 3): "x"}
            terms = [self.graph.terms[uid] for uid in member.terms]
            keys = [(t.source, t.parameter or 0) for t in terms]
            require(len(keys) == len(ports) and set(keys) == ports.keys() and
                    len({t.dtype for t in terms}) == 1 and
                    all(t.binding_kind == "parm" and t.dtype in ("i32", "u32") for t in terms), "primitive_signature",
                    "Unexpected Quotient & Remainder parameters or representation.", location)
            id_ = f"lv-operation-{member.uid}"
            self.node(member.diagram, member.uid, operation(id_, "frog.studio.numeric.quotient_remainder",
                      "numeric.quotient_remainder"), member.bounds)
            for term, key in zip(terms, keys):
                self.port(term.uid, id_, ports[key], term.dtype, term.source)
            return
        signature = OPERATIONS.get(member.primitive)
        require(signature is not None, "primitive", f"Unqualified LabVIEW primitive {member.primitive}.", location)
        target_kind, palette, _, _, parameters = signature
        terms = [self.graph.terms[uid] for uid in member.terms]
        outputs = [term for term in terms if term.source]
        inputs = [term for term in terms if not term.source]
        require(len(outputs) == 1 and matches_operation_types(member.primitive, [t.dtype for t in inputs], outputs[0].dtype),
                "primitive_signature", "Saved terminals do not match the qualified operation's exact type and directions.", location)
        output = outputs[0]
        expected_result = result_binding(member.primitive)
        require((output.binding_kind, output.override) == expected_result,
                "primitive_result", "The saved primitive result configuration is outside this profile.", location)
        id_ = f"lv-operation-{member.uid}"
        self.node(member.diagram, member.uid, operation(id_, target_kind, palette), member.bounds)
        self.source_nodes[-1].update(source_operation=palette, primitive_identity=list(member.primitive))
        self.port(output.uid, id_, "count" if palette == "structures.tick_count" else "result", output.dtype, True)
        if inputs:
            by_parameter = {term.parameter: term for term in inputs if term.binding_kind == "parm"}
            require(by_parameter.keys() == {parameter for parameter, _ in parameters}, "primitive_input_order",
                    "The saved parameter identities do not match the qualified operation.", location)
            for parameter, name in parameters:
                term = by_parameter[parameter]
                self.port(term.uid, id_, name, term.dtype, False)

    def append(self, member):
        location = f"BDHb/uid:{member.uid}"
        terms = [self.graph.terms[uid] for uid in member.terms]
        ports = append_ports(member.flags, member.operand_bindings, member.result_binding,
                             [(t.uid, t.binding_uid, t.binding_kind, tunnel_type(t.dtype), t.binding_flags, t.source)
                              for t in terms])
        require(ports, "build_array_signature",
                "Build Array must append a matching scalar to the ordered rank-one numeric array.", location)
        id_ = f"lv-build-array-{member.uid}"
        self.node(member.diagram, member.uid, operation(id_, "frog.studio.array.build", "array.build",
                  build_array_inputs=2, build_array_concatenate=False), member.bounds)
        self.source_nodes[-1].update(source_operation="array.build", operand_bindings=list(member.operand_bindings))
        for term in terms:
            self.port(term.uid, id_, ports[term.uid], term.dtype, term.source)

    def array_access(self, member):
        replace = member.kind == "aReplace"
        location = f"BDHb/uid:{member.uid}"
        terms = [self.graph.terms[uid] for uid in member.terms]
        roles = {t.binding_flags: t for t in terms}
        if member.kind == "subset":
            names = {0x10000:"array", 0:"index", 0x20000:"length", 1:"result"}
            require(len(terms)==4 and names.keys()==roles.keys() and roles[1].binding_uid==member.result_binding
                    and roles[1].dtype==roles[0x10000].dtype and roles[1].dtype.startswith("array:")
                    and roles[0].dtype==roles[0x20000].dtype=="i32",
                    "array_subset", "Only a rank-one Array Subset has this terminal arrangement.", location)
            id_=f"lv-subset-{member.uid}"
            self.node(member.diagram, member.uid, operation(id_, "frog.studio.array.subset", "array.subset"), member.bounds)
            for role, term in roles.items():
                if self.is_wired(term.uid):
                    self.port(term.uid, id_, names[role], term.dtype, role==1)
            return
        operands = {0x20000: "array", 0x600000: "index", 1: "result" if replace else "element"}
        if replace:
            operands[0x40000] = "subset"
        require(len(terms) == len(operands) and roles.keys() == operands.keys() and
                roles[1].binding_uid == member.result_binding and
                tuple(roles[key].binding_uid for key in ((0x40000, 0x600000) if replace else (0x600000,)))
                == member.operand_bindings,
                "array_access", "The source is not a single indexed element access.", location)
        array_type = roles[0x20000].dtype
        require(array_type.startswith("array:") and roles[0x600000].dtype == "i32" and
                roles[1].dtype == (array_type if replace else array_type[6:]) and
                (not replace or roles[0x40000].dtype == array_type[6:]), "array_access_type",
                "The source Array, index and element types do not agree.", location)
        op = "array.replace_subset" if replace else "array.index"
        id_ = f"lv-array-{member.uid}"
        self.node(member.diagram, member.uid, operation(id_, "frog.studio." + op, op), member.bounds)
        for role, term in roles.items():
            self.port(term.uid, id_, operands[role], term.dtype, role == 1)

    def cluster(self, member):
        bundle = member.kind == "mux"
        terms = [self.graph.terms[uid] for uid in member.terms]
        aggregate = [t for t in terms if t.binding_uid == member.result_binding]
        fields = {t.binding_uid: t for t in terms if t.binding_uid != member.result_binding}
        ordered = member.operand_bindings
        location = f"BDHb/uid:{member.uid}"
        require(aggregate and len(ordered) == len(set(ordered)) and set(ordered) == fields.keys(),
                "cluster_bindings", "The Cluster members do not agree with the ordered binding list.", location)
        expected = "cluster<" + ",".join(f'"element{i}":{tunnel_type(fields[key].dtype)}'
                                         for i, key in enumerate(ordered, 1)) + ">"
        op = "cluster.bundle" if bundle else "cluster.unbundle"
        id_ = f"lv-cluster-{member.uid}"
        self.node(member.diagram, member.uid, operation(id_, "frog.studio." + op, op,
                  cluster_function_terminals=len(ordered)), member.bounds)
        for term in aggregate:
            if self.is_wired(term.uid):
                require(term.dtype == expected, "cluster_type", "Cluster port and field types differ.", location)
                self.port(term.uid, id_, "result" if bundle else "cluster", term.dtype, bundle)
        for i, key in enumerate(ordered, 1):
            term = fields[key]
            self.port(term.uid, id_, f"element{i}", term.dtype, not bundle)

    def root_terminal(self, term):
        location = f"BDHb/uid:{term.uid}"
        if term.binding_kind == "fPDCO":
            widget = self.graph.widgets.get(term.binding_uid)
            require(widget and term.binding_uid not in self.mapped_widgets and term.dtype == widget.dtype
                    and term.source == (widget.role == "control"), "panel_terminal",
                    "The diagram terminal must identify one panel object with the same role and type.", location)
            self.mapped_widgets.add(term.binding_uid)
            id_ = f"lv-terminal-{term.uid}"
            self.node(term.diagram, term.uid, {"id": id_, "kind": "widget_value", "type": tunnel_type(term.dtype),
                      "widget": f"lv-widget-{term.binding_uid}", "label_visible": True}, term.bounds)
            self.port(term.uid, id_, "value", term.dtype, term.source)
        elif term.binding_kind == "bDConstDCO":
            require(term.source and term.value is not None, "diagram_constant",
                    "The constant must have a decoded value and a source port.", location)
            if term.dtype.startswith("cluster<"):
                payload = operation(f"lv-constant-{term.uid}", "frog.studio.cluster.constant","cluster.constant",
                                    cluster_constant_collapsed=True)
                for i,(label,dtype,value) in enumerate(term.value,1):
                    child=convert_constant(f"{term.uid}-field-{i}",dtype,value,8,24+(i-1)*32)
                    child.update(constant_owner_node_id=payload["id"],constant_field_id=f"element{i}",
                                 constant_field_order=i-1,label=label,label_visible=False)
                    self.node(term.diagram,term.uid,child)
            else:
                payload = (empty_array_constant(f"lv-constant-{term.uid}", term.dtype[6:]) if term.dtype.startswith("array:")
                           else convert_constant(term.uid, term.dtype, term.value, 0, 0))
            self.node(term.diagram, term.uid, payload, term.bounds)
            self.port(term.uid, payload["id"], "value", term.dtype, True)
        else:
            require(term.binding_uid in self.boundaries or term.uid in self.ports, "diagram_terminal",
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
            elif member.kind in ("select", "whileLoop"):
                target, endpoints, notes = self.graph.region_owners[member.uid]
                location = f"BDHb/uid:{member.uid}"
                for note in notes:
                    binding = note.get("binding_uid")
                    faces = [self.graph.terms[i] for i in endpoints if self.graph.terms[i].binding_uid==binding]
                    adapted_index = (note["code"]=="native_boundary_type_differs" and len(faces)==1
                                     and faces[0].binding_kind=="lCnt" and faces[0].dtype=="i32")
                    stop = (note["code"]=="loop_condition_polarity_unqualified" and len(faces)==1
                            and faces[0].binding_kind=="lTst" and faces[0].binding_flags==0x10001)
                    initialized_pair = False
                    if note["code"]=="register_lifetime_unqualified":
                        placements = target["structure_tunnels"]
                        tunnel = next((t for t in placements if t["id"]==f"lv-tunnel-{binding}"), {})
                        pair = tunnel.get("shift_register_pair_id")
                        both = [t for t in placements if pair and t.get("shift_register_pair_id")==pair]
                        initialized_pair = (len(both)==2 and {t["output_from_structure"] for t in both}=={False,True}
                            and all(self.is_wired(tid) for tid in endpoints if self.graph.terms[tid].binding_uid in
                                    {int(t["id"].removeprefix("lv-tunnel-")) for t in both}))
                    require(adapted_index or stop or initialized_pair, note["code"], note["message"], location)
                self.node(uid,member.uid,target,member.bounds)
                for tid,(node_id,name) in endpoints.items():
                    t=self.graph.terms[tid]
                    if name=="iteration" and self.is_wired(tid):
                        cast=f"lv-body32-{t.binding_uid}"
                        self.node(t.diagram,t.binding_uid,operation(cast,"frog.studio.numeric.conversion.to_i32","numeric.conversion.to_i32"),
                                  adaptation="While iteration remains a signed I32")
                        self.add_adaptation_edge(t.diagram,cast+"-edge",Connector(node_id,name,"i64",True),Connector(cast,"value","i64",False))
                        self.port(tid,cast,"result","i32",True)
                    else:
                        self.port(tid,node_id,name,t.dtype,t.source)
                for region, row in zip(member.regions,target["regions"]):
                    row["diagram"]=self.diagrams[region]
                    self.project_diagram(region)
            elif member.kind == "Class0189":
                terms = {self.graph.terms[i].binding_uid: self.graph.terms[i] for i in member.terms}
                inputs = member.operand_bindings
                result = terms.get(member.result_binding)
                require(result and result.source and result.dtype.startswith("set<") and inputs
                        and len(terms) == len(inputs) + 1 == len(member.terms) and len(set(inputs)) == len(inputs)
                        and all(uid in terms and not terms[uid].source and f"set<{terms[uid].dtype}>" == result.dtype
                                for uid in inputs), "set_signature", "Inconsistent Build Set bindings.", f"BDHb/uid:{member.uid}")
                rows = [f"row_{index}" for index in range(len(inputs))]
                op = "collection.set.build"
                id_ = f"lv-set-{member.uid}"
                self.node(member.diagram, member.uid, operation(id_, "frog.studio." + op, op,
                    studio_collection_build={"profile":"frog.studio.collection@1", "rows":rows,
                                             "next_row_serial":str(len(rows))}), member.bounds)
                self.port(result.uid, id_, "set", result.dtype, True)
                for row, binding in zip(rows, inputs):
                    term = terms[binding]
                    self.port(term.uid, id_, f"row:{row}:element", term.dtype, False)
            elif member.kind == "aBuild":
                self.append(member)
            elif member.kind == "concat":
                parts = {self.graph.terms[i].binding_uid:self.graph.terms[i] for i in member.terms}
                ordered = (*member.operand_bindings, member.result_binding)
                require(len(parts) == len(ordered) == 3 and set(ordered) == parts.keys()
                        and all(t.dtype=="string" for t in parts.values()), "concatenate_signature",
                        "Expected two ordered string operands and one result.", f"BDHb/uid:{member.uid}")
                id_ = f"lv-concat-{member.uid}"
                self.node(uid, member.uid, operation(id_, "frog.studio.string.concatenate", "string.concatenate"), member.bounds)
                for key, name in zip(ordered, ("a", "b", "result")):
                    self.port(parts[key].uid, id_, name, "string", name=="result")
            elif member.kind in ("aIndx", "aReplace", "subset"):
                self.array_access(member)
            elif member.kind in ("mux", "demux"):
                self.cluster(member)
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
    nodes = {item["id"]: item for item in diagram["nodes"] if not item.get("constant_owner_node_id")}
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
        widget_mapping.append({"id": widget["id"], "uid": uid, "class_id": widget["classId"], "source_bounds": source_widget.bounds,
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
