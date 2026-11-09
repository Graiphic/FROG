<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 Reference Validator — validate_source()</h1>

<p align="center">
  MVP validation pseudo-code for the non-normative FROG reference implementation<br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#status-and-boundary">2. Status and Boundary</a></li>
  <li><a href="#mvp-scope">3. MVP Scope</a></li>
  <li><a href="#input-contract">4. Input Contract</a></li>
  <li><a href="#output-contract">5. Output Contract</a></li>
  <li><a href="#validation-strategy">6. Validation Strategy</a></li>
  <li><a href="#phase-breakdown">7. Phase Breakdown</a></li>
  <li><a href="#helper-conventions">8. Helper Conventions</a></li>
  <li><a href="#pseudo-code-main-function">9. Pseudo-Code — Main Function</a></li>
  <li><a href="#pseudo-code-phase-functions">10. Pseudo-Code — Phase Functions</a></li>
  <li><a href="#mvp-rules-enforced">11. MVP Rules Enforced</a></li>
  <li><a href="#expected-invalid-rejections">12. Expected Invalid Rejections</a></li>
  <li><a href="#summary">13. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the first detailed pseudo-code sketch for <code>validate_source()</code> in the FROG reference implementation.
Its purpose is to make the first validation stage directly implementable for the MVP slice already selected by the repository work.
</p>

<p>
The validator must decide whether loaded canonical source is acceptable as the basis for later derivation.
If validation fails, the pipeline stops.
If validation succeeds, the validator returns a compact validated basis for derivation.
</p>

<hr/>

<h2 id="status-and-boundary">2. Status and Boundary</h2>

<p>
This document is <strong>non-normative</strong>.
It does not own the language rules.
It is an implementation sketch that consumes the published repository rules and turns them into a staged validation procedure.
</p>

<p>
If this pseudo-code reveals ambiguity in the language rules, the specification should be clarified upstream instead of silently inventing private validator behavior.
</p>

<hr/>

<h2 id="mvp-scope">3. MVP Scope</h2>

<p>
The first validator slice covers:
</p>

<ul>
  <li>required top-level sections,</li>
  <li>basic metadata presence,</li>
  <li>basic interface declarations,</li>
  <li>diagram nodes and edges,</li>
  <li>core primitive use for the first examples,</li>
  <li><code>interface_input</code> and <code>interface_output</code>,</li>
  <li><code>widget_value</code>,</li>
  <li><code>widget_reference</code>,</li>
  <li><code>frog.ui.property_read</code>,</li>
  <li><code>frog.ui.property_write</code>,</li>
  <li><code>frog.ui.method_invoke</code>,</li>
  <li><code>frog.core.delay</code> with required <code>initial</code>,</li>
  <li>basic cycle legality.</li>
</ul>

<p>
The first slice does <strong>not</strong> attempt to validate the entire future language surface.
It validates enough to process the first example and conformance corpus coherently.
</p>

<hr/>

<h2 id="input-contract">4. Input Contract</h2>

<p>
The validator consumes a loaded-source artifact conceptually equivalent to:
</p>

<pre><code>{
  "artifact_kind": "frog_loaded_source",
  "artifact_version": "0.1-dev",
  "source": {
    "path": "...",
    "content_hash": "...",
    "spec_version": "0.1"
  },
  "document": { ... canonical source object ... },
  "diagnostics": []
}</code></pre>

<p>
The validator assumes the source has already been decoded as structured data.
It does not assume that the source is valid.
</p>

<hr/>

<h2 id="output-contract">5. Output Contract</h2>

<p>
On success, the validator returns an artifact conceptually equivalent to:
</p>

<pre><code>{
  "artifact_kind": "frog_validation_result",
  "artifact_version": "0.1-dev",
  "source_ref": {
    "path": "...",
    "content_hash": "..."
  },
  "status": "ok",
  "validated_subset": { ... },
  "validated_program": {
    "program_id": "...",
    "entry_kind": "single_frog_program",
    "type_facts": [],
    "resolved_entities": { ... }
  },
  "diagnostics": []
}</code></pre>

<p>
On failure, the validator returns:
</p>

<pre><code>{
  "artifact_kind": "frog_validation_result",
  "artifact_version": "0.1-dev",
  "source_ref": {
    "path": "...",
    "content_hash": "..."
  },
  "status": "error",
  "error_code": "...",
  "diagnostics": [
    {
      "severity": "error",
      "message": "...",
      "source_anchor": { ... }
    }
  ]
}</code></pre>

<p>
No later stage may claim success if validation returned <code>status = error</code>.
</p>

<hr/>

<h2 id="validation-strategy">6. Validation Strategy</h2>

<p>
The first validator should use a strict phased strategy:
</p>

<ol>
  <li>check top-level document shape,</li>
  <li>build local indexes,</li>
  <li>validate interface declarations,</li>
  <li>validate front-panel declarations if present,</li>
  <li>validate diagram node declarations,</li>
  <li>validate diagram edges,</li>
  <li>resolve types and participation roles,</li>
  <li>validate UI primitive usage,</li>
  <li>validate cycle legality,</li>
  <li>emit success or failure.</li>
</ol>

<p>
This phased shape keeps the code easy to debug and makes rejection reasons easier to explain.
</p>

<hr/>

<h2 id="phase-breakdown">7. Phase Breakdown</h2>

<h3>7.1 Top-level shape</h3>

<p>
Check that the source contains the required sections for canonical FROG source in the first slice.
</p>

<h3>7.2 Index build</h3>

<p>
Build fast lookup maps for:
</p>

<ul>
  <li>interface inputs,</li>
  <li>interface outputs,</li>
  <li>widgets,</li>
  <li>diagram nodes,</li>
  <li>diagram edges.</li>
</ul>

<h3>7.3 Interface validation</h3>

<p>
Validate uniqueness, identifiers, and supported MVP types for interface declarations.
</p>

<h3>7.4 Front-panel validation</h3>

<p>
If a <code>front_panel</code> exists, validate widgets, widget IDs, widget classes, roles, and value-carrying constraints.
</p>

<h3>7.5 Diagram-node validation</h3>

<p>
Validate every node by kind and collect resolved facts.
</p>

<h3>7.6 Edge validation</h3>

<p>
Validate edge endpoints, port existence, directionality, and provisional type compatibility.
</p>

<h3>7.7 UI interaction validation</h3>

<p>
Check that:
</p>

<ul>
  <li><code>widget_value</code> is used only for value participation,</li>
  <li><code>widget_reference</code> is used only as object-style widget participation,</li>
  <li>standardized UI primitives receive the right kinds of inputs,</li>
  <li>no confusion exists between natural valueflow and object-style access.</li>
</ul>

<h3>7.8 Cycle validation</h3>

<p>
Detect directed cycles and confirm that each cycle contains at least one explicit local-memory primitive in the MVP slice.
</p>

<hr/>

<h2 id="helper-conventions">8. Helper Conventions</h2>

<p>
The pseudo-code below uses the following conventions:
</p>

<ul>
  <li><code>ctx</code> is a mutable validation context.</li>
  <li><code>ctx.errors</code> stores accumulated hard failures.</li>
  <li><code>ctx.warnings</code> stores optional non-fatal notes.</li>
  <li><code>fail(...)</code> appends an error and marks the validation as failed.</li>
  <li><code>stop_if_errors(ctx)</code> returns early if hard failures already exist.</li>
  <li><code>resolved_*</code> helpers compute implementation-side facts for later derivation.</li>
</ul>

<hr/>

<h2 id="pseudo-code-main-function">9. Pseudo-Code — Main Function</h2>

<pre><code class="language-text">function validate_source(loaded_source):
    ctx = new ValidationContext()

    ctx.source_ref = {
        "path": loaded_source.source.path,
        "content_hash": loaded_source.source.content_hash
    }

    document = loaded_source.document

    validate_top_level_shape(document, ctx)
    if ctx.has_errors():
        return build_validation_error(ctx, "invalid_top_level_shape")

    build_indexes(document, ctx)
    if ctx.has_errors():
        return build_validation_error(ctx, "index_build_failed")

    validate_metadata(document, ctx)
    validate_interface(document, ctx)
    validate_front_panel(document, ctx)
    if ctx.has_errors():
        return build_validation_error(ctx, "invalid_declarations")

    validate_diagram_nodes(document, ctx)
    if ctx.has_errors():
        return build_validation_error(ctx, "invalid_diagram_nodes")

    validate_diagram_edges(document, ctx)
    if ctx.has_errors():
        return build_validation_error(ctx, "invalid_diagram_edges")

    validate_ui_interaction_rules(document, ctx)
    if ctx.has_errors():
        return build_validation_error(ctx, "invalid_ui_interaction")

    validate_cycle_legality(document, ctx)
    if ctx.has_errors():
        return build_validation_error(ctx, "invalid_cycles")

    validated_program = build_validated_program(document, ctx)

    return {
        "artifact_kind": "frog_validation_result",
        "artifact_version": "0.1-dev",
        "source_ref": ctx.source_ref,
        "status": "ok",
        "validated_subset": build_validated_subset(ctx),
        "validated_program": validated_program,
        "diagnostics": ctx.warnings
    }</code></pre>

<hr/>

<h2 id="pseudo-code-phase-functions">10. Pseudo-Code — Phase Functions</h2>

<h3>10.1 Top-level shape</h3>

<pre><code class="language-text">function validate_top_level_shape(document, ctx):
    required_sections = ["spec_version", "metadata", "interface", "diagram"]

    for key in required_sections:
        if key not in document:
            fail(ctx,
                 code = "missing_required_section",
                 message = "Missing required top-level section: " + key,
                 anchor = {"section": key})

    if "spec_version" in document and document["spec_version"] != "0.1":
        fail(ctx,
             code = "unsupported_spec_version",
             message = "Only spec_version 0.1 is supported in the first reference slice.",
             anchor = {"section": "spec_version"})

    if "interface" in document:
        if type(document["interface"]) is not object:
            fail(ctx,
                 code = "invalid_interface_section",
                 message = "interface must be an object.",
                 anchor = {"section": "interface"})

    if "diagram" in document:
        if type(document["diagram"]) is not object:
            fail(ctx,
                 code = "invalid_diagram_section",
                 message = "diagram must be an object.",
                 anchor = {"section": "diagram"})</code></pre>

<h3>10.2 Index build</h3>

<pre><code class="language-text">function build_indexes(document, ctx):
    ctx.interface_inputs = {}
    ctx.interface_outputs = {}
    ctx.widgets = {}
    ctx.nodes = {}
    ctx.edges = {}

    if "interface" in document:
        for item in document["interface"].get("inputs", []):
            register_unique(ctx.interface_inputs, item, "id", ctx, "duplicate_interface_input_id")

        for item in document["interface"].get("outputs", []):
            register_unique(ctx.interface_outputs, item, "id", ctx, "duplicate_interface_output_id")

    if "front_panel" in document:
        for widget in document["front_panel"].get("widgets", []):
            register_unique(ctx.widgets, widget, "id", ctx, "duplicate_widget_id")

    if "diagram" in document:
        for node in document["diagram"].get("nodes", []):
            register_unique(ctx.nodes, node, "id", ctx, "duplicate_node_id")

        for edge in document["diagram"].get("edges", []):
            register_unique(ctx.edges, edge, "id", ctx, "duplicate_edge_id")</code></pre>

<h3>10.3 Metadata validation</h3>

<pre><code class="language-text">function validate_metadata(document, ctx):
    metadata = document.get("metadata", {})

    if "name" not in metadata:
        fail(ctx,
             code = "missing_metadata_name",
             message = "metadata.name is required in the first reference slice.",
             anchor = {"section": "metadata"})

    if "program_version" not in metadata:
        ctx.warnings.append({
            "severity": "warning",
            "message": "metadata.program_version is recommended.",
            "source_anchor": {"section": "metadata"}
        })</code></pre>

<h3>10.4 Interface validation</h3>

<pre><code class="language-text">function validate_interface(document, ctx):
    interface = document.get("interface", {})
    inputs = interface.get("inputs", [])
    outputs = interface.get("outputs", [])

    for port in inputs:
        validate_port_decl(port, "input", ctx)

    for port in outputs:
        validate_port_decl(port, "output", ctx)

function validate_port_decl(port, role, ctx):
    if "id" not in port:
        fail(ctx,
             code = "missing_interface_port_id",
             message = "Interface " + role + " is missing id.",
             anchor = {"role": role})
        return

    if "type" not in port:
        fail(ctx,
             code = "missing_interface_port_type",
             message = "Interface " + role + " '" + port["id"] + "' is missing type.",
             anchor = {"interface_port": port["id"]})
        return

    if not is_supported_mvp_type(port["type"]):
        fail(ctx,
             code = "unsupported_interface_type",
             message = "Unsupported MVP interface type: " + to_string(port["type"]),
             anchor = {"interface_port": port["id"]})</code></pre>

<h3>10.5 Front-panel validation</h3>

<pre><code class="language-text">function validate_front_panel(document, ctx):
    if "front_panel" not in document:
        return

    fp = document["front_panel"]

    if "widgets" not in fp:
        fail(ctx,
             code = "missing_front_panel_widgets",
             message = "front_panel.widgets is required when front_panel is present.",
             anchor = {"section": "front_panel"})
        return

    for widget in fp["widgets"]:
        validate_widget_decl(widget, ctx)

function validate_widget_decl(widget, ctx):
    widget_id = widget.get("id")

    if "widget" not in widget:
        fail(ctx,
             code = "missing_widget_class",
             message = "Widget is missing widget class.",
             anchor = {"widget_id": widget_id})
        return

    if not is_supported_mvp_widget_class(widget["widget"]):
        fail(ctx,
             code = "unsupported_widget_class",
             message = "Unsupported MVP widget class: " + widget["widget"],
             anchor = {"widget_id": widget_id})

    role = widget.get("role")
    if role not in ["control", "indicator"]:
        fail(ctx,
             code = "invalid_widget_role",
             message = "Widget role must be 'control' or 'indicator' in the MVP slice.",
             anchor = {"widget_id": widget_id})

    if widget_requires_value_type(widget["widget"]):
        if "value_type" not in widget:
            fail(ctx,
                 code = "missing_widget_value_type",
                 message = "Value-carrying widget is missing value_type.",
                 anchor = {"widget_id": widget_id})
        elif not is_supported_mvp_type(widget["value_type"]):
            fail(ctx,
                 code = "unsupported_widget_value_type",
                 message = "Unsupported MVP widget value_type.",
                 anchor = {"widget_id": widget_id})</code></pre>

<h3>10.6 Diagram node validation</h3>

<pre><code class="language-text">function validate_diagram_nodes(document, ctx):
    for node in document["diagram"].get("nodes", []):
        kind = node.get("kind")

        if kind == "interface_input":
            validate_interface_input_node(node, ctx)

        elif kind == "interface_output":
            validate_interface_output_node(node, ctx)

        elif kind == "primitive":
            validate_primitive_node(node, ctx)

        elif kind == "widget_value":
            validate_widget_value_node(node, ctx)

        elif kind == "widget_reference":
            validate_widget_reference_node(node, ctx)

        else:
            fail(ctx,
                 code = "unsupported_node_kind",
                 message = "Unsupported MVP node kind: " + to_string(kind),
                 anchor = {"node_id": node.get("id")})</code></pre>

<h3>10.7 Interface nodes</h3>

<pre><code class="language-text">function validate_interface_input_node(node, ctx):
    port_id = node.get("interface_port")
    if port_id not in ctx.interface_inputs:
        fail(ctx,
             code = "unknown_interface_input",
             message = "interface_input references unknown input port.",
             anchor = {"node_id": node.get("id")})
        return

    ctx.resolved_node_facts[node["id"]] = {
        "kind": "interface_input",
        "value_type": ctx.interface_inputs[port_id]["type"],
        "interface_port": port_id
    }

function validate_interface_output_node(node, ctx):
    port_id = node.get("interface_port")
    if port_id not in ctx.interface_outputs:
        fail(ctx,
             code = "unknown_interface_output",
             message = "interface_output references unknown output port.",
             anchor = {"node_id": node.get("id")})
        return

    ctx.resolved_node_facts[node["id"]] = {
        "kind": "interface_output",
        "value_type": ctx.interface_outputs[port_id]["type"],
        "interface_port": port_id
    }</code></pre>

<h3>10.8 Primitive nodes</h3>

<pre><code class="language-text">function validate_primitive_node(node, ctx):
    primitive_ref = node.get("type")

    if primitive_ref not in supported_mvp_primitives():
        fail(ctx,
             code = "unsupported_primitive_ref",
             message = "Unsupported MVP primitive: " + to_string(primitive_ref),
             anchor = {"node_id": node.get("id")})
        return

    if primitive_ref == "frog.core.delay":
        if "initial" not in node:
            fail(ctx,
                 code = "missing_delay_initial",
                 message = "frog.core.delay requires explicit initial in the MVP slice.",
                 anchor = {"node_id": node.get("id")})
            return

        if not is_numeric_literal(node["initial"]):
            fail(ctx,
                 code = "invalid_delay_initial",
                 message = "frog.core.delay initial must be a numeric literal in the MVP slice.",
                 anchor = {"node_id": node.get("id")})

    if primitive_ref in ["frog.ui.property_read", "frog.ui.property_write", "frog.ui.method_invoke"]:
        validate_ui_primitive_metadata(node, ctx)

    ctx.resolved_node_facts[node["id"]] = {
        "kind": "primitive",
        "primitive_ref": primitive_ref
    }

function validate_ui_primitive_metadata(node, ctx):
    primitive_ref = node["type"]

    if primitive_ref in ["frog.ui.property_read", "frog.ui.property_write"]:
        if "widget_member" not in node:
            fail(ctx,
                 code = "missing_widget_member",
                 message = primitive_ref + " requires widget_member.",
                 anchor = {"node_id": node.get("id")})
            return

        member = node["widget_member"]
        if "member" not in member:
            fail(ctx,
                 code = "missing_widget_member_name",
                 message = "widget_member.member is required.",
                 anchor = {"node_id": node.get("id")})

    if primitive_ref == "frog.ui.method_invoke":
        if "widget_method" not in node:
            fail(ctx,
                 code = "missing_widget_method",
                 message = "frog.ui.method_invoke requires widget_method.",
                 anchor = {"node_id": node.get("id")})</code></pre>

<h3>10.9 Widget participation nodes</h3>

<pre><code class="language-text">function validate_widget_value_node(node, ctx):
    widget_id = node.get("widget")

    if widget_id not in ctx.widgets:
        fail(ctx,
             code = "unknown_widget_for_widget_value",
             message = "widget_value references unknown widget.",
             anchor = {"node_id": node.get("id")})
        return

    widget = ctx.widgets[widget_id]

    if not widget_has_primary_value(widget):
        fail(ctx,
             code = "widget_value_on_non_value_widget",
             message = "widget_value requires a value-carrying widget.",
             anchor = {"node_id": node.get("id")})
        return

    ctx.resolved_node_facts[node["id"]] = {
        "kind": "widget_value",
        "widget_id": widget_id,
        "widget_role": widget.get("role"),
        "value_type": widget.get("value_type")
    }

function validate_widget_reference_node(node, ctx):
    widget_id = node.get("widget")

    if widget_id not in ctx.widgets:
        fail(ctx,
             code = "unknown_widget_for_widget_reference",
             message = "widget_reference references unknown widget.",
             anchor = {"node_id": node.get("id")})
        return

    ctx.resolved_node_facts[node["id"]] = {
        "kind": "widget_reference",
        "widget_id": widget_id
    }</code></pre>

<h3>10.10 Edge validation</h3>

<pre><code class="language-text">function validate_diagram_edges(document, ctx):
    for edge in document["diagram"].get("edges", []):
        validate_single_edge(edge, ctx)

function validate_single_edge(edge, ctx):
    from_node_id = edge.get("from", {}).get("node")
    from_port = edge.get("from", {}).get("port")
    to_node_id = edge.get("to", {}).get("node")
    to_port = edge.get("to", {}).get("port")

    if from_node_id not in ctx.nodes:
        fail(ctx,
             code = "unknown_edge_source_node",
             message = "Edge source node does not exist.",
             anchor = {"edge_id": edge.get("id")})
        return

    if to_node_id not in ctx.nodes:
        fail(ctx,
             code = "unknown_edge_target_node",
             message = "Edge target node does not exist.",
             anchor = {"edge_id": edge.get("id")})
        return

    source_fact = ctx.resolved_node_facts.get(from_node_id)
    target_fact = ctx.resolved_node_facts.get(to_node_id)

    if source_fact is null or target_fact is null:
        fail(ctx,
             code = "edge_before_node_resolution",
             message = "Cannot validate edge because node facts were not resolved.",
             anchor = {"edge_id": edge.get("id")})
        return

    if not is_valid_output_port(source_fact, from_port):
        fail(ctx,
             code = "invalid_source_port",
             message = "Invalid edge source port.",
             anchor = {"edge_id": edge.get("id")})
        return

    if not is_valid_input_port(target_fact, to_port):
        fail(ctx,
             code = "invalid_target_port",
             message = "Invalid edge target port.",
             anchor = {"edge_id": edge.get("id")})
        return

    source_type = resolve_output_port_type(source_fact, from_port)
    target_type = resolve_input_port_type(target_fact, to_port)

    if not types_compatible(source_type, target_type):
        fail(ctx,
             code = "edge_type_mismatch",
             message = "Edge type mismatch: " + to_string(source_type) + " -> " + to_string(target_type),
             anchor = {"edge_id": edge.get("id")})</code></pre>

<h3>10.11 UI interaction rules</h3>

<pre><code class="language-text">function validate_ui_interaction_rules(document, ctx):
    for node in document["diagram"].get("nodes", []):
        if node.get("kind") != "primitive":
            continue

        primitive_ref = node.get("type")

        if primitive_ref in ["frog.ui.property_read", "frog.ui.property_write", "frog.ui.method_invoke"]:
            validate_ui_primitive_wiring(node, document, ctx)

    validate_no_widget_reference_confusion(document, ctx)

function validate_ui_primitive_wiring(node, document, ctx):
    node_id = node["id"]
    incoming = find_incoming_edges(document, node_id)

    has_ref = any(edge.to.port == "ref" for edge in incoming)
    if not has_ref:
        fail(ctx,
             code = "missing_ui_primitive_ref_input",
             message = node["type"] + " requires a ref input.",
             anchor = {"node_id": node_id})
        return

    ref_edge = first(edge for edge in incoming if edge.to.port == "ref")
    ref_source_fact = ctx.resolved_node_facts[ref_edge["from"]["node"]]

    if ref_source_fact["kind"] != "widget_reference":
        fail(ctx,
             code = "ui_primitive_ref_must_come_from_widget_reference",
             message = "UI primitive ref input must be driven by widget_reference.",
             anchor = {"node_id": node_id})

function validate_no_widget_reference_confusion(document, ctx):
    for edge in document["diagram"].get("edges", []):
        from_node_id = edge["from"]["node"]
        source_fact = ctx.resolved_node_facts[from_node_id]

        if source_fact["kind"] != "widget_reference":
            continue

        target_node_id = edge["to"]["node"]
        target_node = ctx.nodes[target_node_id]

        if target_node.get("kind") != "primitive":
            fail(ctx,
                 code = "widget_reference_without_ui_primitive",
                 message = "widget_reference must be used through a valid standardized UI primitive in the MVP slice.",
                 anchor = {"edge_id": edge.get("id")})
            continue

        if target_node.get("type") not in ["frog.ui.property_read", "frog.ui.property_write", "frog.ui.method_invoke"]:
            fail(ctx,
                 code = "widget_reference_without_ui_primitive",
                 message = "widget_reference cannot feed ordinary primitives in the MVP slice.",
                 anchor = {"edge_id": edge.get("id")})</code></pre>

<h3>10.12 Cycle legality</h3>

<pre><code class="language-text">function validate_cycle_legality(document, ctx):
    graph = build_directed_graph_from_edges(document["diagram"].get("edges", []))
    cycles = find_directed_cycles(graph)

    for cycle in cycles:
        if not cycle_contains_explicit_local_memory(cycle, ctx):
            fail(ctx,
                 code = "illegal_feedback_without_explicit_memory",
                 message = "Directed cycle without explicit local-memory primitive.",
                 anchor = {
                     "node_ids": cycle.node_ids,
                     "edge_ids": cycle.edge_ids
                 })

function cycle_contains_explicit_local_memory(cycle, ctx):
    for node_id in cycle.node_ids:
        fact = ctx.resolved_node_facts[node_id]
        if fact["kind"] == "primitive" and fact["primitive_ref"] == "frog.core.delay":
            return true
    return false</code></pre>

<h3>10.13 Build validated program</h3>

<pre><code class="language-text">function build_validated_program(document, ctx):
    return {
        "program_id": make_program_id(ctx.source_ref["path"], document),
        "entry_kind": "single_frog_program",
        "type_facts": collect_type_facts(ctx),
        "resolved_entities": {
            "interface_inputs": list(keys(ctx.interface_inputs)),
            "interface_outputs": list(keys(ctx.interface_outputs)),
            "widgets": list(keys(ctx.widgets)),
            "primitive_refs": collect_primitive_refs(ctx)
        }
    }

function build_validated_subset(ctx):
    return {
        "core_primitives": ctx.used_core_primitives,
        "public_interface": ctx.used_public_interface,
        "widget_value": ctx.used_widget_value,
        "widget_reference": ctx.used_widget_reference,
        "ui_object_primitives": ctx.used_ui_object_primitives,
        "explicit_local_memory": ctx.used_explicit_local_memory
    }</code></pre>

<hr/>

<h2 id="mvp-rules-enforced">11. MVP Rules Enforced</h2>

<p>
The first validator should enforce at least the following MVP rules:
</p>

<ul>
  <li>required top-level sections must exist,</li>
  <li>interface ports must be declared explicitly and typed,</li>
  <li>front-panel widgets must be declared explicitly if referenced,</li>
  <li><code>widget_value</code> requires a value-carrying widget,</li>
  <li><code>widget_reference</code> requires an existing widget,</li>
  <li>UI primitives must carry the required metadata,</li>
  <li>UI primitive <code>ref</code> must come from <code>widget_reference</code>,</li>
  <li><code>widget_reference</code> must not be treated as ordinary valueflow,</li>
  <li><code>frog.core.delay</code> requires explicit <code>initial</code>,</li>
  <li>a cycle is valid only if it contains explicit local memory.</li>
</ul>

<hr/>

<h2 id="expected-invalid-rejections">12. Expected Invalid Rejections</h2>

<p>
The first validator should explicitly reject the three initial invalid conformance cases:
</p>

<ul>
  <li><strong>widget reference without UI primitive</strong></li>
  <li><strong>illegal feedback without explicit memory</strong></li>
  <li><strong>interface / widget role confusion</strong></li>
</ul>

<p>
The validator must reject those cases rather than:
</p>

<ul>
  <li>inventing a missing primitive,</li>
  <li>inserting hidden memory,</li>
  <li>auto-converting widget participation into public interface participation,</li>
  <li>normalizing <code>widget_value</code> into property access or the reverse.</li>
</ul>

<hr/>

<h2 id="summary">13. Summary</h2>

<p>
This document provides a directly codable MVP pseudo-code sketch for <code>validate_source()</code> in the FROG reference implementation.
It keeps the validator narrow,
explicit,
and aligned with the published repository boundaries:
reject invalid source,
establish a validated basis,
and stop the pipeline unless derivation is semantically justified.
</p>
