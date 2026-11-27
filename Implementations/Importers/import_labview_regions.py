# SPDX-License-Identifier: Apache-2.0
"""Project saved LabVIEW structure ownership and proven ports into an editable Draft."""

from import_labview_structured import boundary_port, structure


STRUCTURE_KINDS = {
    "commentNode": "conditional_disable_structure",
    "select": "case_structure",
    "forLoop": "for_loop",
    "whileLoop": "while_loop",
    "eventStruct": "event_structure",
}
INTEGER_TYPES = {f"{prefix}{width}" for prefix in ("i", "u") for width in (8, 16, 32, 64)}
NATIVE_BOUNDARIES = {
    "caseSel": ("selector", "selected_case", None, None),
    "lCnt": (None, "iteration", None, None),
    "lMax": ("count", "count_value", None, None),
    "lTst": (None, None, None, "condition"),
    "eventTimeOut": ("timeout", "timeout_value", None, None),
    "eventDynDCO": ("event_registration", "event_registration_value", "event_registration_out",
                    "event_registration_update"),
}
NATIVE_TERMINALS = {
    "iteration": ("index", "i64", "index", None, "iteration", True),
    "count": ("count", "i64", "count", "count", "count_value", True),
    "timeout": ("timeout", "i64", "timeout_milliseconds", "timeout", "timeout_value", True),
    "condition": ("condition", "bool", "stop_if_true", None, "condition", False),
}


def _note(node, code, message, **identity):
    return {"code": code, "node_uid": node.uid, "message": message, **identity}


def _region_id(uid):
    return f"lv-region-{uid}"


def _terminal_record(dtype, role, outer, inner, read_only):
    result = {"type": dtype, "role": role, "outer_visible": outer is not None,
              "exposed_in_body": inner is not None, "read_only": read_only}
    if outer is not None:
        result["outer_port"] = outer
    if inner is not None:
        result["body_port"] = inner
    return result


def _case_regions(graph, node, regions, target, binding_type, notes):
    # The native palette and node type use case_structure, but the Draft
    # reader recurses into regions only for the canonical structure type case.
    target["structure_type"] = "case"
    target["case_profile"] = "frog.case-editing@1"
    selection = graph.selections[node.uid]
    binding = graph.bindings.get(selection.selector)
    dtype = (binding_type(binding) or "unknown") if binding else "unknown"
    terminal = _terminal_record(dtype, "selector", "selector", "selected_case", True)
    target["structure_terminals"]["selector"] = terminal
    target["binding_profile"] = "frog.typed-binding@1"
    target["case_structure_manual_order"] = True
    if selection.displayed_region is not None:
        target["case_structure_visible_region_id"] = _region_id(selection.displayed_region)
    enum_items = ()
    enum_labels = binding.enum_labels if binding else ()
    if dtype in getattr(binding_type, "enums", {}):
        enum_labels = binding_type.enums[dtype][2]
    if enum_labels and dtype.startswith("enum<"):
        enum_items = tuple({"item_id": f"lv-ordinal-{index}", "display_name": name, "ordinal": index}
                           for index, name in enumerate(enum_labels))
        terminal["enum_items"] = list(enum_items)
    matched = set()
    status_values = set()
    enum_values = set()
    error_selector = dtype == 'cluster<"status":bool,"code":i32,"source":string>'
    error_regions = {}
    if error_selector and len(regions) == 2 and selection.default_region is None:
        # NI also stores the two Error Case arms as ranges over status/code.
        # The no-error sentinel includes warnings. Restrict this conversion
        # to the complete, two-arm table verified against a running VI.
        low, high = -(1 << 31), (1 << 31) - 1
        selector_kind = node.fields.get("SelectNRightType")
        ranges = {(r.start, r.end, r.start_kind, r.end_kind): r.region for r in selection.range_rules}
        clear, failed = ranges.get((low, low, 3, 1)), ranges.get((low, high, 3, 3))
        if (selector_kind and selector_kind.value == 2 and len(selection.range_rules) == 2 and
                clear != failed and {clear, failed} == set(regions)):
            error_regions = {clear: False, failed: True}
    for rule in selection.range_rules:
        match = None
        if rule.region in error_regions:
            failed = error_regions[rule.region]
            match = {"kind": "error_status", "value": "true" if failed else "false"}
            regions[rule.region]["title"] = "Error" if failed else "No Error"
            status_values.add(int(failed))
        elif rule.start_kind == rule.end_kind == 0 and rule.start == rule.end:
            if dtype == "bool" and rule.start in (0, 1):
                match = {"kind": "boolean_exact", "value": bool(rule.start)}
                status_values.add(rule.start)
            elif error_selector and rule.start in (0, 1):
                match = {"kind": "error_status", "value": "true" if rule.start else "false"}
                regions[rule.region]["title"] = "Error" if rule.start else "No Error"
                status_values.add(rule.start)
            elif enum_items and 0 <= rule.start < len(enum_items):
                match = {"kind": "enum_item", "item": enum_items[rule.start]["item_id"]}
                regions[rule.region]["title"] = enum_items[rule.start]["display_name"]
                enum_values.add(rule.start)
            elif dtype == "string" and 0 <= rule.start < len(selection.exact_string_table):
                message = selection.exact_string_table[rule.start]
                match = {"kind": "string_exact", "value": message}
            elif not (binding and binding.enum_labels) and dtype in INTEGER_TYPES:
                match = {"kind": "integer_exact", "value": str(rule.start)}
        if match is not None:
            regions[rule.region].setdefault("matches", []).append(match)
            matched.add(rule.region)
        elif rule.region in regions:
            notes.append(_note(node, "case_rule_unqualified", "A stored Case rule has no verified native match.",
                               region_uid=rule.region))
    default_region = selection.default_region
    # NI can omit SelectDefaultCase while saving the selected Enum item and
    # its Default designation in the Case selector. Cross-check the label
    # against the typed range table before assigning a fallback region.
    if (default_region is None and "SelectDefaultCase" not in node.fields
            and enum_items and selection.displayed_label):
        candidates = {rule.region for rule in selection.range_rules
                      if rule.start_kind == rule.end_kind == 0 and rule.start == rule.end
                      and 0 <= rule.start < len(enum_items)
                      and selection.displayed_label == f'"{enum_labels[rule.start]}", Default'}
        if len(candidates) == 1:
            candidate, = candidates
            if selection.displayed_region in (None, candidate):
                default_region = candidate
    for uid, region in regions.items():
        if uid == default_region:
            region["default"] = True
            region.setdefault("matches", [])
        if selection.displayed_region == uid and selection.displayed_label:
            region.setdefault("title", selection.displayed_label)
        if uid not in matched and uid != default_region:
            text = region.get("title") or f"LabVIEW region {uid}"
            region["draft_text"] = f"{text} (selector requires review)"
            notes.append(_note(node, "case_match_unqualified", "The saved selector rule is retained as source evidence.", region_uid=uid))
    exhaustive = len(selection.range_rules) == len(matched) == len(regions) == 2 and status_values == {0, 1}
    if enum_items:
        exhaustive = (len(enum_values) == len(enum_items) == len(selection.range_rules) and len(matched) == len(regions))
    if default_region is None and not exhaustive:
        notes.append(_note(node, "case_default_unqualified", "There is no verified explicit default region in the source data."))


def _disable_expression(row):
    if row.kind != "SelectorInfoElement" or len(row.children) != 2:
        return None
    marker, expression = row.children
    if (marker.tag != "Tag0000" or marker.payload or marker.children or
            expression.kind != "ExpressionInfo" or len(expression.children) != 2):
        return None
    strings, tree = expression.children
    if strings.tag != "Tag0000" or tree.tag != "activeDiag":
        return None
    return strings.children, tree.children


def _disable_comparisons(node, selection):
    table = node.fields.get("commentSelInfoArray")
    if not table or len(table.children) != len(node.regions) or selection.default_region not in node.regions:
        return {}
    result = {}
    for uid, row in zip(node.regions, table.children):
        expression = _disable_expression(row)
        if expression is None:
            return {}
        strings, tree = expression
        if uid == selection.default_region:
            if strings or tree:
                return {}
            continue
        # NI's serialized binary equality has a root (parent -1, children 1/2,
        # opcode -3) and two leaves that index the original string table.
        if len(strings) != 2 or len(tree) != 3 or any(
                s.tag != "SL__arrayElement" or s.children or not isinstance(s.payload, bytes) for s in strings):
            continue
        rows = []
        for item in tree:
            if item.kind != "TreeElt" or [c.tag for c in item.children] != ["Tag0000", "activeDiag", "activeMarker", "activePlot"]:
                break
            rows.append(tuple(c.payload for c in item.children if not c.children))
        if rows != [(b"\xff", b"\x01", b"\x02", b"\xfd"), (b"\x00", b"\xff", b"\xff", b"\x00"),
                    (b"\x00", b"\xff", b"\xff", b"\x01")]:
            continue
        symbol, value = (s.payload for s in strings)
        if symbol == b"OS" and value in (b"Win", b"Mac", b"Linux"):
            result[uid] = {"any": [{"all": [{"symbol": "OS", "operator": "==", "value": value.decode("ascii")}]}]}
    return result


def _disable_regions(graph, node, regions, target, notes):
    selection = graph.selections[node.uid]
    comparisons = _disable_comparisons(node, selection)
    for uid, region in regions.items():
        if uid == selection.default_region:
            region["default"] = True
        elif uid in comparisons:
            region["condition"] = comparisons[uid]
        else:
            region["condition_draft"] = True
            notes.append(_note(node, "disable_condition_unqualified", "The LabVIEW compile-time predicate requires review.", region_uid=uid))
    displayed = selection.displayed_region or selection.active_region
    if displayed is not None:
        target["conditional_disable_visible_case_index"] = node.regions.index(displayed)
        if selection.displayed_label:
            regions[displayed]["subdiagram_label_text"] = selection.displayed_label


def _event_specs(node):
    table = node.fields.get("EventNodeEvents")
    if node.kind != "eventStruct" or not table:
        return {}
    by_index = {}
    for row in table.children:
        fields = {child.tag: child for child in row.children}
        index = fields.get("diagramIdx")
        if row.kind != "EventSpec" or len(fields) != len(row.children) or index is None:
            return {}
        by_index.setdefault(index.value, []).append(fields)
    return {index: entries[0] for index, entries in by_index.items() if len(entries) == 1}


def _matches_static_event(spec, code, source, flags):
    if not spec:
        return False
    expected = {"source": source, "regFlags": 1, "eFlags": flags, "eSource": 0, "dynIndex": 0}
    return (set(spec) == set(expected) | {"diagramIdx", "type", "ddoUID", "menuTag"}
            and spec["type"].payload == code and not spec["menuTag"].payload and not spec["menuTag"].children
            and all(spec[name].value == value for name, value in expected.items()))


def event_control_displays(node):
    return {index: row["ddoUID"].value for index, row in _event_specs(node).items()
            if _matches_static_event(row, bytes.fromhex("40000002"), 3, 4) and row["ddoUID"].value}


def panel_close_responses(graph, node, binding_type, wired):
    """Identify the VI's filter and its saved, wired Discard? response."""
    by_index = _event_specs(node)
    matches = {}
    for uid in node.regions:
        diagram = graph.diagrams[uid]
        spec = by_index.get(diagram.index)
        if not _matches_static_event(spec, bytes.fromhex("80000003"), 5, 2) or spec["ddoUID"].value != 0:
            continue
        candidates = []
        for child_uid in diagram.nodes:
            child = graph.nodes[child_uid]
            if child.kind != "eventDataNode":
                continue
            aggregate = child.fields.get("dcoAgg")
            fields = child.fields.get("dcoList")
            if not fields or not aggregate or len(fields.children) != 1:
                continue
            sinks = [graph.terminals[t] for t in child.terminals if t in wired]
            if len(sinks) != 1 or sinks[0].direction != "sink" or sinks[0].binding != fields.children[0].uid:
                continue
            field = graph.bindings.get(sinks[0].binding)
            if (not field or field.kind != "nmxDCO" or not field.fields.get("i") or
                    field.fields["i"].value != 4 or binding_type(field) != "bool"):
                continue
            if not all(graph.terminals[t].binding == aggregate.uid for t in child.terminals if t != sinks[0].uid):
                continue
            candidates.append((child_uid, sinks[0].uid))
        if len(candidates) == 1:
            matches[uid] = candidates[0]
    return matches


def unwired_panel_close_data(graph, region, types, wired):
    """Read the saved display rows without turning an unwired shelf into a function."""
    candidates = []
    field_names = {b"Source": "source", b"Type": "type", b"Time": "time", b"VIRef": "program_ref"}
    for uid in graph.diagrams[region].nodes:
        node = graph.nodes[uid]
        if node.kind != "eventDataNode" or not node.terminals or wired.intersection(node.terminals):
            continue
        terminals = [graph.terminals[t] for t in node.terminals]
        if any(t.direction != "source" for t in terminals):
            continue
        aggregate = node.fields.get("dcoAgg")
        rows = node.fields.get("dcoList")
        binding = graph.bindings.get(aggregate.uid) if aggregate else None
        if not binding or not rows or binding.type_label != "Event Data" or not rows.children:
            continue
        schema = types.schema(binding)
        if not schema or schema.family != "Cluster" or len(rows.children) > 16:
            continue
        if {t.binding for t in terminals} != {binding.uid, *(row.uid for row in rows.children)}:
            continue
        fields = []
        for row in rows.children:
            selected = graph.bindings.get(row.uid)
            if not selected or selected.kind != "nmxDCO":
                break
            index = selected.fields["i"].value if "i" in selected.fields else 0
            if not isinstance(index, int) or not 0 <= index < len(schema.children):
                break
            field = field_names.get(schema.children[index].field_label())
            if not field:
                break
            fields.append(field)
        if len(fields) == len(rows.children):
            candidates.append((uid, fields))
    return candidates[0] if len(candidates) == 1 else None


def _single_dynamic_events(graph, node, types):
    # The LV20 index 2 selects the sole User Event nested in this one-field
    # source cluster. Other layouts require resolving their own index table.
    registrations = []
    for uid in node.boundary_bindings:
        binding = graph.bindings.get(uid)
        if not binding or binding.kind != "eventDynDCO" or binding.owner != node.uid:
            continue
        if any(graph.signals[s].source is not None and t in graph.signals[s].targets
               for s in graph.diagrams[node.diagram].signals for t in binding.terminals):
            registrations.append(types.schema(binding))
    if len(registrations) != 1 or not registrations[0]:
        return {}
    source = registrations[0].single_user_event_field()
    if not source:
        return {}
    expected = {"source": 1, "regFlags": 1, "eSource": 25, "eFlags": 0, "ddoUID": 0, "dynIndex": 2}
    return {i: source.field_label().decode(types.encoding) for i, row in _event_specs(node).items()
            if row.keys() == expected.keys() | {"diagramIdx", "type", "menuTag"}
            and row["type"].value == 1000 and not row["menuTag"].payload and not row["menuTag"].children
            and all(row[k].value == v for k, v in expected.items())}


def _event_regions(node, regions, target, notes, responses, controls, dynamic):
    target["structure_subdiagram_label_visible"] = True
    for uid, row in regions.items():
        if uid in dynamic:
            row["event"] = {"kind": "user", "studio_bindings": [
                {"kind": "user_event", "source": "dynamic:source_1", "source_scope": "dynamic"}]}
            row["event_label"] = dynamic[uid]
            row["studio_event_data_node_visible"] = False
            continue
        if uid in controls:
            widget = controls[uid]
            row["event"] = {"kind": "value_change", "source": widget["id"]}
            row["event_label"] = widget["label"] + ": Value Change"
            row["studio_event_data_node_visible"] = False
            continue
        if uid in responses:
            row["event"] = {"kind": "user", "studio_bindings": [
                {"kind": "panel_close_filter", "source": "this_vi", "source_scope": "vi"}]}
            row["studio_event_data_node_visible"] = False
            row["studio_event_response_fields"] = ["discard"]
            row["event_label"] = "Panel Close?"
            continue
        # An absent descriptor is interpreted as a User Event by older codecs.
        # Use an explicit unsupported kind so no live registration is inferred.
        row["event"] = {"kind": "labview_unqualified"}
        row["event_label"] = row["subdiagram_label_text"] = f"LabVIEW event {uid} (requires review)"
        row["studio_event_data_node_visible"] = False
        notes.append(_note(node, "event_binding_unqualified", "The LabVIEW event subscription has no translated binding.", region_uid=uid))


def _native_endpoints(node, target, binding, exterior, interior, dtype, wired, notes, indexed_count):
    input_outer, output_inner, output_outer, input_inner = NATIVE_BOUNDARIES[binding.kind]
    unused_counter = (binding.kind == "lCnt" or (binding.kind == "lMax" and indexed_count)) and not any(
        term.uid in wired for term in (*exterior, *interior))
    ports = {}
    for term in (*exterior, *interior):
        is_outer = term in exterior
        if term.direction is None:
            name = None
        elif is_outer:
            name = output_outer if term.direction == "source" else input_outer
        else:
            name = output_inner if term.direction == "source" else input_inner
        if name is not None:
            ports[term.uid] = (target["id"], name)
        elif not unused_counter:
            notes.append(_note(node, "native_boundary_face_unqualified", "No equivalent native port is verified for this face.",
                               binding_uid=binding.uid, terminal_uid=term.uid))
    port = input_outer or output_inner or input_inner
    if port in NATIVE_TERMINALS:
        key, native_type, role, outer, inner, read_only = NATIVE_TERMINALS[port]
        target["structure_terminals"][key] = _terminal_record(native_type, role, outer, inner, read_only)
        if dtype != native_type and not unused_counter:
            notes.append(_note(node, "native_boundary_type_differs", "The native port's type differs from the saved terminal type.",
                               binding_uid=binding.uid, source_type=dtype, native_type=native_type))
    if binding.kind == "eventDynDCO":
        target["studio_event_dynamic_terminals_visible"] = True
    if binding.kind == "lTst":
        if node.kind == "forLoop":
            target["for_loop_conditional_terminal"] = True
        notes.append(_note(node, "loop_condition_polarity_unqualified", "The LabVIEW loop stop predicate polarity requires review.", binding_uid=binding.uid))
    return ports


def _placement(node, binding, output):
    if node.bounds and binding.bounds:
        left, top, right, bottom = node.bounds
        tx, ty, ex, ey = binding.bounds
        if right > left and bottom > top:
            # Tunnel bounds are local to the structure, including at negative
            # absolute diagram coordinates. Keep the saved relative position.
            side = "left" if tx + ex < right - left else "right"
            return side, min(0.98, max(0.02, (ty + ey) / (2 * (bottom - top))))
    return ("right" if output else "left"), 0.5


def _register_pair(graph, binding):
    if binding.kind == "lSR":
        mate = binding.fields.get("rsrDCO")
        other = graph.bindings.get(mate.uid) if mate is not None else None
        rows = other.fields.get("lsrDCOList") if other is not None and other.kind == "rSR" else None
        if rows is not None and len(rows.children) == 1 and rows.children[0].uid == binding.uid:
            return f"lv-register-{binding.uid}-{other.uid}" if binding.owner == other.owner else None
    elif binding.kind == "rSR":
        rows = binding.fields.get("lsrDCOList")
        if rows is not None and len(rows.children) == 1:
            other = graph.bindings.get(rows.children[0].uid)
            if other is not None and other.kind == "lSR":
                return _register_pair(graph, other)
    return None


def loop_tunnel_modes(graph, types):
    """Verify both faces against their producers and consumers in the enclosing and inner diagrams."""
    def array_type(binding):
        schema = types.schema(binding)
        if (not schema or schema.family not in ("Array", "SubArray") or
                schema.detail != ((255, 0xFFFFFF),) or len(schema.children) != 1):
            return None
        return schema.children[0].native_type(types.encoding, types.enums)

    signals = {uid: signal for signal in graph.signals.values() if signal.source and signal.targets
               for uid in signal.endpoints}
    found = {}
    for b in graph.bindings.values():
        if b.kind != "lpTun" or b.flags not in (0x800, 0x801, 0x01000800, 0x01000801) or len(b.terminals) != 2:
            continue
        owner = graph.nodes.get(b.owner)
        indexed = bool(b.flags & 0x01000000)
        if not owner or owner.kind not in ("forLoop", "whileLoop") or len(owner.regions) != 1:
            continue
        element = array_type(b) if indexed else types(b)
        if not element or element == "unknown" or (indexed and owner.kind != "forLoop"):
            continue
        output = bool(b.flags & 1)
        if not indexed and output and owner.kind == "forLoop":
            # Last-value semantics for zero iterations also need a verified source default.
            continue
        expected_faces = {owner.diagram: ("source" if output else "sink", True),
                          owner.regions[0]: ("sink" if output else "source", False)}
        matched = set()
        for uid in b.terminals:
            terminal, signal = graph.terminals[uid], signals.get(uid)
            face = expected_faces.get(terminal.diagram)
            if not face or not signal or face[0] != terminal.direction:
                break
            peers = signal.targets if uid == signal.source else (signal.source,)
            peer_bindings = [graph.bindings.get(graph.terminals[p].binding) for p in peers]
            if any((array_type(peer) if indexed and face[1] else types(peer)) != element for peer in peer_bindings):
                break
            matched.add(terminal.diagram)
        if matched == expected_faces.keys():
            # FROG stores the producer-side type; an output collects one element per iteration.
            found[b.uid] = (indexed, f"array<{element}>" if indexed and not output else element)
    return found


def _tunnel(graph, node, target, binding, exterior, interior, dtype, wired, notes, mode):
    if len(exterior) != 1 or exterior[0].direction is None:
        notes.append(_note(node, "tunnel_faces_unqualified", "The stored inner and outer directions do not establish a native tunnel.",
                           binding_uid=binding.uid))
        return {}
    outer = exterior[0]
    output = outer.direction == "source"
    if outer.uid not in wired and binding.kind in {"lSR", "rSR"}:
        output = binding.kind == "rSR"
    outer_direction, inner_direction = ("source", "sink") if output else ("sink", "source")
    if ((outer.uid in wired and outer.direction != outer_direction) or
            any(term.uid in wired and term.direction != inner_direction for term in interior)):
        notes.append(_note(node, "tunnel_faces_unqualified", "Connected faces do not agree with a native tunnel.",
                           binding_uid=binding.uid))
        return {}
    tunnel = f"lv-tunnel-{binding.uid}"
    side, position = _placement(node, binding, output)
    boundary = {"id": tunnel, "type": dtype}
    placement = {"id": tunnel, "value_type": dtype, "side": side, "normalized_position": position,
                 "output_from_structure": output}
    if mode and mode[0]:
        boundary["tunnel_mode"] = placement["tunnel_mode"] = "indexing"
    # LV20 saved scalar output mode measured on both error Case branches.
    if (binding.kind == "selTun" and binding.flags == 0x300801 and output
            and dtype in {"bool", "u32"} and outer.uid in wired and len(interior) == 2):
        missing = [term for term in interior if term.uid not in wired]
        error_case = [{"kind": "error_status", "value": "true"}]
        clean_case = [{"kind": "error_status", "value": "false"}]
        regions = {r["id"]: r.get("matches") for r in target["regions"]}
        if (len(missing) == 1 and len(regions) == 2 and clean_case in regions.values()
                and regions.get(_region_id(missing[0].diagram)) == error_case):
            boundary["use_default_if_unwired"] = placement["use_default_if_unwired"] = True
    if binding.kind in {"rSR", "lSR"}:
        pair = _register_pair(graph, binding)
        if pair is not None:
            boundary["shift_register_pair_id"] = placement["shift_register_pair_id"] = pair
        notes.append(_note(node, "register_lifetime_unqualified", "The saved register transfer and initialization require review.",
                           binding_uid=binding.uid))
    elif binding.kind == "lpTun" and mode is None:
        notes.append(_note(node, "loop_tunnel_mode_unqualified", "The saved loop indexing and iteration behavior require review.",
                           binding_uid=binding.uid))
    if any(term.direction != inner_direction for term in interior) or outer.direction != outer_direction:
        notes.append(_note(node, "unwired_boundary_role", "The unused source face is placed by its verified boundary role.",
                           binding_uid=binding.uid))
    target["boundary"]["outputs" if output else "inputs"].append(boundary)
    target["structure_tunnels"].append(placement)
    ports = {term.uid: (target["id"], boundary_port(tunnel, "input" if output else "output")) for term in interior}
    ports[outer.uid] = (target["id"], boundary_port(tunnel, "output" if output else "input"))
    return ports


def project_region_owner(graph, node, diag_by_uid, binding_type, *, close_responses=None, event_controls=None,
                         loop_modes=None):
    """Return a native region container, source terminal endpoints and unqualified semantics."""
    if node.kind not in STRUCTURE_KINDS or not node.regions:
        raise ValueError(f"LabVIEW node {node.uid} has no supported Draft region container.")
    target = structure(f"lv-node-{node.uid}", STRUCTURE_KINDS[node.kind])
    target["structure_terminals"] = {}
    if node.bounds is not None:
        left, top, right, bottom = node.bounds
        target["layout"] = {"x": left, "y": top, "width": max(160, right - left), "height": max(140, bottom - top)}
    loop = node.kind in {"forLoop", "whileLoop"}
    region_rows = {uid: {"id": "body" if loop and len(node.regions) == 1 else _region_id(uid), "diagram": diag_by_uid[uid]}
                   for uid in node.regions}
    target["regions"] = list(region_rows.values())
    notes = []
    if node.kind == "commentNode":
        _disable_regions(graph, node, region_rows, target, notes)
    elif node.kind == "select":
        _case_regions(graph, node, region_rows, target, binding_type, notes)
    elif node.kind == "eventStruct":
        dynamic = _single_dynamic_events(graph, node, binding_type)
        _event_regions(node, region_rows, target, notes, close_responses or {}, event_controls or {},
                       {uid: dynamic[graph.diagrams[uid].index] for uid in region_rows
                        if graph.diagrams[uid].index in dynamic})
    wired = {terminal for diagram in (node.diagram, *node.regions) for signal in graph.diagrams[diagram].signals
             for terminal in graph.signals[signal].endpoints}
    endpoints = {}
    modes = loop_modes or {}
    indexed_count = any(uid in modes and modes[uid][0] and graph.bindings[uid].flags == 0x01000800
                        for uid in node.boundary_bindings)
    for uid in node.boundary_bindings:
        binding = graph.bindings.get(uid)
        if binding is None or binding.owner != node.uid:
            notes.append(_note(node, "boundary_binding_unqualified", "A saved boundary binding has no verified owner.", binding_uid=uid))
            continue
        terms = [graph.terminals[uid] for uid in binding.terminals if uid in graph.terminals]
        exterior = [term for term in terms if term.diagram == node.diagram and term.owner == node.uid]
        interior = [term for term in terms if term.diagram in region_rows]
        dtype = binding_type(binding) or "unknown"
        if len(terms) != len(binding.terminals) or len(terms) != len(exterior) + len(interior):
            notes.append(_note(node, "boundary_scope_unqualified", "A boundary face falls outside the saved parent and subdiagrams.", binding_uid=uid))
            continue
        if binding.kind in NATIVE_BOUNDARIES:
            endpoints.update(_native_endpoints(node, target, binding, exterior, interior, dtype, wired, notes, indexed_count))
        else:
            mode = modes.get(uid)
            endpoints.update(_tunnel(graph, node, target, binding, exterior, interior,
                                     mode[1] if mode else dtype, wired, notes, mode))
    return target, endpoints, notes
