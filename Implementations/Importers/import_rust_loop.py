# SPDX-License-Identifier: Apache-2.0
"""Lower a bounded shape of Rust MIR while-loops to a native FROG For loop."""

import re

from import_rust_mir import Value, _ASSIGN, _COPY, _GOTO, _SWITCH, _TYPES, bounds


def assignments(rows):
    result = []
    for line, text in rows:
        match = _ASSIGN.fullmatch(text)
        if not match:
            return None
        result.append((line, *match.groups()))
    return result


def recognize(blocks, types, parameters):
    # A single-entry, single-exit loop whose I32/U32 induction variable
    # advances from zero to an invariant bound in unit steps.
    if len(blocks) != 4 or not blocks.get("bb0"):
        return None
    entry = blocks["bb0"]
    jump = _GOTO.fullmatch(entry[-1][1])
    if not jump or jump[1] == "bb0":
        return None
    header_id = jump[1]
    header = blocks.get(header_id, ())
    if len(header) < 2:
        return None
    switch = _SWITCH.fullmatch(header[-1][1])
    if not switch or len({"bb0", header_id, switch[2], switch[3]}) != 4:
        return None
    exit_id, body_id = switch[2], switch[3]
    body = blocks.get(body_id, ())
    back = _GOTO.fullmatch(body[-1][1]) if body else None
    if not back or back[1] != header_id or not blocks.get(exit_id):
        return None
    pre, condition, operations = assignments(entry[:-1]), assignments(header[:-1]), assignments(body[:-1])
    if not pre or not condition or not operations:
        return None

    aliases = {}
    for _, local, expression in condition[:-1]:
        origin = _COPY.fullmatch(expression)
        if not origin or local in aliases or types.get(local) != types.get(origin[1]):
            return None
        aliases[local] = aliases.get(origin[1], origin[1])
    _, predicate, expression = condition[-1]
    switch_local = _COPY.fullmatch(switch[1])
    compare = re.fullmatch(r"Lt\((?:copy|move) (_[0-9]+), (?:copy|move) (_[0-9]+)\)", expression)
    if not compare or not switch_local or switch_local[1] != predicate or types.get(predicate) != "bool":
        return None
    counter = aliases.get(compare[1], compare[1])
    limit = aliases.get(compare[2], compare[2])
    dtype = types.get(counter)
    if counter == limit or dtype not in {"i32", "u32"} or types.get(limit) != dtype:
        return None
    header_writes = set(aliases) | {predicate}
    pre_values = {local: expression for _, local, expression in pre}
    increments = [expression for _, local, expression in operations if local == counter]
    increment = rf"Add\((?:copy|move) {counter}, const 1_{dtype}\)"
    if (pre_values.get(counter) != f"const 0_{dtype}" or len(increments) != 1
            or not re.fullmatch(increment, increments[0])
            or any(local in header_writes or local == limit for _, local, _ in operations)):
        return None
    if header_writes & (pre_values.keys() | parameters.keys()):
        return None
    return header_id, body_id, exit_id, counter, limit, dtype, pre, operations


def lower_assignments(lowerer, block, rows, values):
    lowerer.block = block
    for line, local, expression in rows:
        lowerer.line = line
        if len(lowerer.graph.nodes) > 512:
            lowerer.refuse("The editable graph exceeds the node limit")
        expected = lowerer.types.get(local)
        if expected not in _TYPES:
            lowerer.refuse(f"Unsupported assignment to {local} of type {expected or 'unknown'}")
        value = lowerer.expression(expression, values)
        lowerer.require_type(value, expected, local)
        values[local] = value


def add_boundary(structure, tunnel, dtype, output, position, pair=None):
    declaration = {"id": tunnel, "type": dtype}
    knob = {"id": tunnel, "side": "right" if output else "left", "normalized_position": position,
            "value_type": dtype, "output_from_structure": output}
    if pair:
        declaration["shift_register_pair_id"] = knob["shift_register_pair_id"] = pair
    structure["boundary"]["outputs" if output else "inputs"].append(declaration)
    structure["structure_tunnels"].append(knob)


def lower_counted_loop(lowerer, parameter_values):
    plan = recognize(lowerer.function.blocks, lowerer.types, parameter_values)
    if plan is None:
        return None
    header, body, end, counter, limit, dtype, pre, operations = plan
    incoming = parameter_values.copy()
    lower_assignments(lowerer, "bb0", pre, incoming)
    if limit not in incoming:
        lowerer.refuse(f"Loop bound {limit} is not initialized before the loop")
    graph = lowerer.graph
    loop = "rust-counted-loop"
    limit_value = incoming[limit]
    if dtype == "i32":
        zero = incoming[counter]
        positive = graph.comparison("Lt", [zero, limit_value])
        limit_value = graph.select(positive, limit_value, zero)
    widen = graph.node("rust-count64", "primitive", graph.operation_layout(),
                       type="frog.studio.numeric.conversion.to_i64", palette_catalog_id="numeric.conversion.to_i64")
    graph.wire(limit_value.endpoint, widen, "value")

    # A value read before any body assignment arrives on the left border.
    # Variables assigned again retain their previous value with paired
    # registers. Their original init value also flows through on zero trips.
    assigned, used = set(), set()
    for _, local, expression in operations:
        used.update(set(re.findall(r"(?:copy|move) (_[0-9]+)", expression)) - assigned)
        assigned.add(local)
    carried = assigned & incoming.keys()
    invariant = (used & incoming.keys()) - carried
    states = sorted(carried | invariant, key=lambda local: int(local[1:]))
    if len(states) > 8:
        lowerer.refuse("The loop requires more than eight scalar inputs or registers")

    boundary = {"inputs": [], "outputs": []}
    tunnels = []
    structure = {"boundary": boundary, "structure_tunnels": tunnels}
    body_values = {}
    for index, local in enumerate(states):
        value = incoming[local]
        left, right = f"left{local}", f"right{local}"
        pair = f"state{local}" if local in carried else None
        position = (index + 1) / (len(states) + 1)
        add_boundary(structure, left, value.dtype, False, position, pair)
        graph.wire(value.endpoint, loop, f"tunnel:{left}:input")
        body_values[local] = Value(value.dtype, (loop, f"tunnel:{left}:output"))
        if pair:
            add_boundary(structure, right, value.dtype, True, position, pair)
            incoming[local] = Value(value.dtype, (loop, f"tunnel:{right}:output"))

    # Use the existing scalar graph writer inside the one real body region.
    root_nodes, root_edges, root_column = graph.nodes, graph.edges, graph.column
    graph.nodes, graph.edges, graph.column = [], [], 0
    try:
        lower_assignments(lowerer, body, operations, body_values)
        for local in states:
            if local in carried:
                graph.wire(body_values[local].endpoint, loop, f"tunnel:right{local}:input")
        body_nodes, body_edges = graph.nodes, graph.edges
    finally:
        graph.nodes, graph.edges, graph.column = root_nodes, root_edges, root_column

    left = max(node["layout"]["x"] + node["layout"]["width"] for node in root_nodes) + 90
    top = 40
    width = max(node["layout"]["x"] + node["layout"]["width"] for node in body_nodes) + 80
    height = max(220, max(node["layout"]["y"] + node["layout"]["height"] for node in body_nodes) + 48)
    for node in body_nodes:
        node["id"] = f"{loop}-{node['id']}"
        node["layout"]["x"] += left
        node["layout"]["y"] += top
    for edge in body_edges:
        edge["id"] = f"{loop}-{edge['id']}"
        for endpoint in (edge["from"], edge["to"]):
            if endpoint["node"] != loop:
                endpoint["node"] = f"{loop}-{endpoint['node']}"
    graph.node(loop, "structure", bounds(left, top, width, height),
               type="frog.studio.structures.for_loop", palette_catalog_id="structures.for_loop",
               structure_type="for_loop", label_visible=False, **structure,
               structure_terminals={"count": {"type": "i64", "outer_visible": True, "exposed_in_body": True,
                                               "read_only": True, "role": "count", "outer_port": "count", "body_port": "count_value"},
                                    "index": {"type": "i64", "outer_visible": False, "exposed_in_body": True,
                                              "read_only": True, "role": "index", "body_port": "iteration"}},
               regions=[{"id": "body", "diagram": {"nodes": body_nodes, "edges": body_edges, "zoom_percent": 100}}])
    graph.wire((widen, "result"), loop, "count")
    graph.column = max(graph.column, (left + width) // 180 + 1)
    return lowerer.walk(end, incoming, frozenset({"bb0", header, body}))
