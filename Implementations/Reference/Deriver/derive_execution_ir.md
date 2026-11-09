<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 Reference Deriver — derive_execution_ir()</h1>

<p align="center">
  MVP Execution IR derivation pseudo-code for the non-normative FROG reference implementation<br/>
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
  <li><a href="#derivation-strategy">6. Derivation Strategy</a></li>
  <li><a href="#derivation-phases">7. Derivation Phases</a></li>
  <li><a href="#helper-conventions">8. Helper Conventions</a></li>
  <li><a href="#pseudo-code-main-function">9. Pseudo-Code — Main Function</a></li>
  <li><a href="#pseudo-code-phase-functions">10. Pseudo-Code — Phase Functions</a></li>
  <li><a href="#mvp-preservation-rules">11. MVP Preservation Rules</a></li>
  <li><a href="#example-expectations">12. Example Expectations</a></li>
  <li><a href="#summary">13. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the first detailed pseudo-code sketch for <code>derive_execution_ir()</code> in the FROG reference implementation.
Its purpose is to turn the validated program basis produced by the validator into an <strong>open execution-facing IR artifact</strong> suitable for later lowering.
</p>

<p>
The deriver is the first stage that materializes execution-facing objects explicitly.
It must preserve validated meaning, preserve attribution, preserve recoverable distinctions, and remain upstream from lowering and runtime-private realization.
</p>

<hr/>

<h2 id="status-and-boundary">2. Status and Boundary</h2>

<p>
This document is <strong>non-normative</strong>.
It does not redefine:
</p>

<ul>
  <li>what a valid FROG source is,</li>
  <li>what validated program meaning is,</li>
  <li>the normative Execution IR architecture,</li>
  <li>the normative derivation rules.</li>
</ul>

<p>
It is an implementation sketch that consumes the published derivation boundary and turns it into a directly codable reference-implementation procedure.
</p>

<hr/>

<h2 id="mvp-scope">3. MVP Scope</h2>

<p>
The first derivation slice covers:
</p>

<ul>
  <li>one validated FROG to one execution unit,</li>
  <li><code>interface_input</code>,</li>
  <li><code>interface_output</code>,</li>
  <li><code>primitive</code>,</li>
  <li><code>widget_value</code>,</li>
  <li><code>widget_reference</code>,</li>
  <li>standardized UI primitives:
    <ul>
      <li><code>frog.ui.property_read</code></li>
      <li><code>frog.ui.property_write</code></li>
      <li><code>frog.ui.method_invoke</code></li>
    </ul>
  </li>
  <li>explicit local memory through <code>frog.core.delay</code>,</li>
  <li>directed connections.</li>
</ul>

<p>
The first slice does not yet attempt to cover the full future language surface.
It covers enough to process the initial examples and conformance cases coherently.
</p>

<hr/>

<h2 id="input-contract">4. Input Contract</h2>

<p>
The deriver consumes a successful validation artifact conceptually equivalent to:
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
The deriver may also need read access to the loaded source document itself,
because the validation result is a compact validated basis rather than a full source replacement.
</p>

<p>
The deriver assumes:
</p>

<ul>
  <li>validation already succeeded,</li>
  <li>resolved node facts and type facts are trustworthy for the selected MVP subset,</li>
  <li>cycle legality has already been checked.</li>
</ul>

<hr/>

<h2 id="output-contract">5. Output Contract</h2>

<p>
On success, the deriver returns an artifact conceptually equivalent to:
</p>

<pre><code>{
  "artifact_kind": "frog_derived_execution_ir",
  "artifact_version": "0.1-dev",
  "source_ref": {
    "path": "...",
    "content_hash": "..."
  },
  "validation_ref": {
    "program_id": "...",
    "status": "ok"
  },
  "execution_unit": {
    "id": "unit:main",
    "family": "execution_unit",
    "objects": [],
    "connections": [],
    "support_objects": []
  },
  "diagnostics": []
}</code></pre>

<p>
On failure, the deriver returns:
</p>

<pre><code>{
  "artifact_kind": "frog_derived_execution_ir",
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
A derivation error means that the implementation failed to build a conforming open execution-facing artifact from a previously validated basis.
That should be rare in the first slice.
</p>

<hr/>

<h2 id="derivation-strategy">6. Derivation Strategy</h2>

<p>
The first deriver should use a strict staged strategy:
</p>

<ol>
  <li>confirm validation success,</li>
  <li>create one execution unit for the whole validated FROG,</li>
  <li>derive primary execution-facing objects from validated source families,</li>
  <li>derive support objects only where useful in the MVP slice,</li>
  <li>derive directed connections,</li>
  <li>attach attribution to all execution-visible objects and connections,</li>
  <li>run final preservation checks,</li>
  <li>emit the derived artifact.</li>
</ol>

<p>
This structure keeps the code easy to debug and matches the published derivation boundary closely.
</p>

<hr/>

<h2 id="derivation-phases">7. Derivation Phases</h2>

<h3>7.1 Precondition check</h3>

<p>
Derivation begins only if validation returned <code>status = ok</code>.
</p>

<h3>7.2 Unit creation</h3>

<p>
Create one execution unit for the validated FROG program.
The first slice does not split one validated FROG into multiple independent execution units.
</p>

<h3>7.3 Object derivation</h3>

<p>
Walk validated diagram nodes and derive one primary execution-facing object for each execution-relevant source node,
plus support objects where the first slice finds them useful.
</p>

<h3>7.4 Connection derivation</h3>

<p>
Walk validated edges and derive explicit directed connections between attributable objects and ports.
</p>

<h3>7.5 Attribution attachment</h3>

<p>
Ensure every execution-visible object and connection has enough source attribution for later mapping and diagnostics.
</p>

<h3>7.6 Final preservation check</h3>

<p>
Confirm that the derived artifact still preserves the distinctions required by the published derivation rules for the selected MVP subset.
</p>

<hr/>

<h2 id="helper-conventions">8. Helper Conventions</h2>

<p>
The pseudo-code below uses the following conventions:
</p>

<ul>
  <li><code>ctx</code> is a mutable derivation context.</li>
  <li><code>ctx.errors</code> stores hard derivation failures.</li>
  <li><code>ctx.warnings</code> stores optional non-fatal notes.</li>
  <li><code>fail(...)</code> appends an error and marks derivation as failed.</li>
  <li><code>emit_object(...)</code> appends one derived execution-facing object.</li>
  <li><code>emit_connection(...)</code> appends one derived directed connection.</li>
  <li><code>sources</code> stores source anchors for attribution.</li>
</ul>

<hr/>

<h2 id="pseudo-code-main-function">9. Pseudo-Code — Main Function</h2>

<pre><code class="language-text">function derive_execution_ir(loaded_source, validation_result):
    ctx = new DerivationContext()

    ctx.source_ref = validation_result.source_ref
    ctx.validation_ref = {
        "program_id": validation_result.validated_program.program_id,
        "status": validation_result.status
    }

    if validation_result.status != "ok":
        return build_derivation_error(
            ctx,
            "derivation_requires_successful_validation",
            "derive_execution_ir() requires validation_result.status = ok."
        )

    document = loaded_source.document

    create_execution_unit(document, validation_result, ctx)
    if ctx.has_errors():
        return build_derivation_error_from_ctx(ctx, "execution_unit_creation_failed")

    derive_execution_objects(document, validation_result, ctx)
    if ctx.has_errors():
        return build_derivation_error_from_ctx(ctx, "object_derivation_failed")

    derive_connections(document, validation_result, ctx)
    if ctx.has_errors():
        return build_derivation_error_from_ctx(ctx, "connection_derivation_failed")

    attach_unit_level_support_objects(document, validation_result, ctx)
    if ctx.has_errors():
        return build_derivation_error_from_ctx(ctx, "support_object_derivation_failed")

    run_final_preservation_checks(document, validation_result, ctx)
    if ctx.has_errors():
        return build_derivation_error_from_ctx(ctx, "final_preservation_check_failed")

    return {
        "artifact_kind": "frog_derived_execution_ir",
        "artifact_version": "0.1-dev",
        "source_ref": ctx.source_ref,
        "validation_ref": ctx.validation_ref,
        "execution_unit": ctx.execution_unit,
        "diagnostics": ctx.warnings
    }</code></pre>

<hr/>

<h2 id="pseudo-code-phase-functions">10. Pseudo-Code — Phase Functions</h2>

<h3>10.1 Create execution unit</h3>

<pre><code class="language-text">function create_execution_unit(document, validation_result, ctx):
    ctx.execution_unit = {
        "id": "unit:main",
        "family": "execution_unit",
        "program_id": validation_result.validated_program.program_id,
        "objects": [],
        "connections": [],
        "support_objects": [],
        "sources": ["program:" + validation_result.validated_program.program_id]
    }

    if validation_result.validated_program.entry_kind != "single_frog_program":
        fail(ctx,
             code = "unsupported_entry_kind_for_mvp_derivation",
             message = "The first derivation slice supports only single_frog_program entry kind.",
             anchor = {"program_id": validation_result.validated_program.program_id})</code></pre>

<h3>10.2 Derive execution objects</h3>

<pre><code class="language-text">function derive_execution_objects(document, validation_result, ctx):
    for node in document["diagram"].get("nodes", []):
        derive_single_node_object(node, validation_result, ctx)

function derive_single_node_object(node, validation_result, ctx):
    kind = node.get("kind")

    if kind == "interface_input":
        derive_interface_input_object(node, ctx)
        return

    if kind == "interface_output":
        derive_interface_output_object(node, ctx)
        return

    if kind == "primitive":
        derive_primitive_object(node, ctx)
        return

    if kind == "widget_value":
        derive_widget_value_object(node, ctx)
        return

    if kind == "widget_reference":
        derive_widget_reference_object(node, ctx)
        return

    fail(ctx,
         code = "unsupported_node_kind_for_derivation",
         message = "Unsupported node kind for MVP derivation: " + to_string(kind),
         anchor = {"node_id": node.get("id")})</code></pre>

<h3>10.3 Interface boundary objects</h3>

<pre><code class="language-text">function derive_interface_input_object(node, ctx):
    object_id = "obj:" + node["id"]

    emit_object(ctx, {
        "id": object_id,
        "kind": "public_input_boundary",
        "interface_port": node["interface_port"],
        "ports": [
            {
                "id": "value",
                "direction": "out"
            }
        ],
        "sources": ["diagram.node:" + node["id"]]
    })

function derive_interface_output_object(node, ctx):
    object_id = "obj:" + node["id"]

    emit_object(ctx, {
        "id": object_id,
        "kind": "public_output_boundary",
        "interface_port": node["interface_port"],
        "ports": [
            {
                "id": "value",
                "direction": "in"
            }
        ],
        "sources": ["diagram.node:" + node["id"]]
    })</code></pre>

<h3>10.4 Primitive objects</h3>

<pre><code class="language-text">function derive_primitive_object(node, ctx):
    primitive_ref = node["type"]
    object_id = "obj:" + node["id"]

    if primitive_ref == "frog.core.add":
        emit_object(ctx, {
            "id": object_id,
            "kind": "primitive",
            "primitive_ref": primitive_ref,
            "ports": [
                { "id": "a", "direction": "in" },
                { "id": "b", "direction": "in" },
                { "id": "result", "direction": "out" }
            ],
            "sources": ["diagram.node:" + node["id"]]
        })
        return

    if primitive_ref == "frog.core.delay":
        emit_object(ctx, {
            "id": object_id,
            "kind": "explicit_local_memory",
            "primitive_ref": primitive_ref,
            "initial": node["initial"],
            "ports": [
                { "id": "in", "direction": "in" },
                { "id": "out", "direction": "out" }
            ],
            "sources": ["diagram.node:" + node["id"]]
        })
        return

    if primitive_ref == "frog.ui.property_read":
        emit_object(ctx, {
            "id": object_id,
            "kind": "ui_object_primitive",
            "primitive_ref": primitive_ref,
            "member": node["widget_member"],
            "ports": [
                { "id": "ref", "direction": "in" },
                { "id": "value", "direction": "out" }
            ],
            "sources": ["diagram.node:" + node["id"]]
        })
        return

    if primitive_ref == "frog.ui.property_write":
        emit_object(ctx, {
            "id": object_id,
            "kind": "ui_object_primitive",
            "primitive_ref": primitive_ref,
            "member": node["widget_member"],
            "ports": [
                { "id": "ref", "direction": "in" },
                { "id": "value", "direction": "in" }
            ],
            "sources": ["diagram.node:" + node["id"]]
        })
        return

    if primitive_ref == "frog.ui.method_invoke":
        emit_object(ctx, {
            "id": object_id,
            "kind": "ui_object_primitive",
            "primitive_ref": primitive_ref,
            "method": node["widget_method"],
            "ports": [
                { "id": "ref", "direction": "in" }
            ],
            "sources": ["diagram.node:" + node["id"]]
        })
        return

    fail(ctx,
         code = "unsupported_primitive_for_derivation",
         message = "Unsupported MVP primitive in derivation: " + primitive_ref,
         anchor = {"node_id": node["id"]})</code></pre>

<h3>10.5 Widget participation objects</h3>

<pre><code class="language-text">function derive_widget_value_object(node, ctx):
    object_id = "obj:" + node["id"]

    emit_object(ctx, {
        "id": object_id,
        "kind": "widget_value_participation",
        "widget_id": node["widget"],
        "ports": [
            { "id": "value", "direction": "unknown_until_role_resolved" }
        ],
        "sources": ["diagram.node:" + node["id"]]
    })

function derive_widget_reference_object(node, ctx):
    object_id = "obj:" + node["id"]

    emit_object(ctx, {
        "id": object_id,
        "kind": "widget_reference_participation",
        "widget_id": node["widget"],
        "ports": [
            { "id": "ref", "direction": "out" }
        ],
        "sources": ["diagram.node:" + node["id"]]
    })</code></pre>

<h3>10.6 Derive connections</h3>

<pre><code class="language-text">function derive_connections(document, validation_result, ctx):
    for edge in document["diagram"].get("edges", []):
        derive_single_connection(edge, ctx)

function derive_single_connection(edge, ctx):
    from_object_id = "obj:" + edge["from"]["node"]
    to_object_id = "obj:" + edge["to"]["node"]

    source_object = find_object(ctx, from_object_id)
    target_object = find_object(ctx, to_object_id)

    if source_object is null:
        fail(ctx,
             code = "missing_source_object_for_connection",
             message = "Cannot derive connection: source object not found.",
             anchor = {"edge_id": edge["id"]})
        return

    if target_object is null:
        fail(ctx,
             code = "missing_target_object_for_connection",
             message = "Cannot derive connection: target object not found.",
             anchor = {"edge_id": edge["id"]})
        return

    emit_connection(ctx, {
        "id": "conn:" + edge["id"],
        "from": {
            "object": from_object_id,
            "port": edge["from"]["port"]
        },
        "to": {
            "object": to_object_id,
            "port": edge["to"]["port"]
        },
        "sources": ["diagram.edge:" + edge["id"]]
    })</code></pre>

<h3>10.7 Attach support objects</h3>

<pre><code class="language-text">function attach_unit_level_support_objects(document, validation_result, ctx):
    ctx.execution_unit.support_objects.append({
        "id": "support:unit_source_map",
        "kind": "unit_source_map",
        "program_id": validation_result.validated_program.program_id,
        "sources": ["program:" + validation_result.validated_program.program_id]
    })</code></pre>

<p>
For the first slice, support objects should remain minimal.
They exist only to keep attribution and later debugging easier.
</p>

<h3>10.8 Final preservation checks</h3>

<pre><code class="language-text">function run_final_preservation_checks(document, validation_result, ctx):
    ensure_one_execution_unit(ctx)
    ensure_all_objects_are_attributable(ctx)
    ensure_all_connections_are_attributable(ctx)
    ensure_widget_value_and_widget_reference_remain_distinct(ctx)
    ensure_ui_object_primitives_remain_distinct_from_widget_reference(ctx)
    ensure_explicit_local_memory_remains_explicit(ctx)
    ensure_no_runtime_private_fields_leaked(ctx)

function ensure_one_execution_unit(ctx):
    if ctx.execution_unit is null:
        fail(ctx,
             code = "missing_execution_unit",
             message = "Derivation did not produce an execution unit.",
             anchor = {})

function ensure_all_objects_are_attributable(ctx):
    for obj in ctx.execution_unit["objects"]:
        if "sources" not in obj or len(obj["sources"]) == 0:
            fail(ctx,
                 code = "unattributable_execution_object",
                 message = "Derived execution object has no source attribution.",
                 anchor = {"object_id": obj["id"]})

function ensure_all_connections_are_attributable(ctx):
    for conn in ctx.execution_unit["connections"]:
        if "sources" not in conn or len(conn["sources"]) == 0:
            fail(ctx,
                 code = "unattributable_connection",
                 message = "Derived connection has no source attribution.",
                 anchor = {"connection_id": conn["id"]})

function ensure_widget_value_and_widget_reference_remain_distinct(ctx):
    for obj in ctx.execution_unit["objects"]:
        if obj["kind"] == "widget_value_participation" and "ref" in [p["id"] for p in obj["ports"]]:
            fail(ctx,
                 code = "widget_value_reference_confusion_in_ir",
                 message = "widget_value participation was confused with widget reference participation.",
                 anchor = {"object_id": obj["id"]})

function ensure_ui_object_primitives_remain_distinct_from_widget_reference(ctx):
    widget_ref_ids = set(obj["id"] for obj in ctx.execution_unit["objects"] if obj["kind"] == "widget_reference_participation")
    ui_primitive_ids = set(obj["id"] for obj in ctx.execution_unit["objects"] if obj["kind"] == "ui_object_primitive")

    for obj_id in widget_ref_ids:
        if obj_id in ui_primitive_ids:
            fail(ctx,
                 code = "widget_reference_ui_primitive_collapse",
                 message = "widget_reference participation was collapsed with a UI-object primitive.",
                 anchor = {"object_id": obj_id})

function ensure_explicit_local_memory_remains_explicit(ctx):
    for obj in ctx.execution_unit["objects"]:
        if obj.get("primitive_ref") == "frog.core.delay":
            if obj["kind"] != "explicit_local_memory":
                fail(ctx,
                     code = "explicit_memory_erased_during_derivation",
                     message = "frog.core.delay must derive as explicit_local_memory.",
                     anchor = {"object_id": obj["id"]})

function ensure_no_runtime_private_fields_leaked(ctx):
    forbidden_keys = ["scheduler_private", "runtime_private", "abi_layout", "thread_binding"]
    for obj in ctx.execution_unit["objects"]:
        for key in forbidden_keys:
            if key in obj:
                fail(ctx,
                     code = "runtime_private_leak_in_open_ir",
                     message = "Runtime-private detail leaked into derived open IR.",
                     anchor = {"object_id": obj["id"]})</code></pre>

<h3>10.9 Utility helpers</h3>

<pre><code class="language-text">function emit_object(ctx, obj):
    ctx.execution_unit["objects"].append(obj)

function emit_connection(ctx, conn):
    ctx.execution_unit["connections"].append(conn)

function find_object(ctx, object_id):
    for obj in ctx.execution_unit["objects"]:
        if obj["id"] == object_id:
            return obj
    return null

function build_derivation_error(ctx, code, message):
    return {
        "artifact_kind": "frog_derived_execution_ir",
        "artifact_version": "0.1-dev",
        "source_ref": ctx.source_ref,
        "status": "error",
        "error_code": code,
        "diagnostics": [
            {
                "severity": "error",
                "message": message,
                "source_anchor": {}
            }
        ]
    }

function build_derivation_error_from_ctx(ctx, code):
    return {
        "artifact_kind": "frog_derived_execution_ir",
        "artifact_version": "0.1-dev",
        "source_ref": ctx.source_ref,
        "status": "error",
        "error_code": code,
        "diagnostics": ctx.errors + ctx.warnings
    }</code></pre>

<hr/>

<h2 id="mvp-preservation-rules">11. MVP Preservation Rules</h2>

<p>
The first deriver must enforce at least the following preservation rules:
</p>

<ul>
  <li>one validated FROG becomes one execution unit,</li>
  <li><code>interface_input</code> derives as public input participation,</li>
  <li><code>interface_output</code> derives as public output participation,</li>
  <li><code>widget_value</code> derives as widget-value participation,</li>
  <li><code>widget_reference</code> derives as widget-reference participation,</li>
  <li>standardized UI primitives remain primitive execution objects,</li>
  <li><code>frog.core.delay</code> remains explicit local memory,</li>
  <li>every connection remains explicit and attributable,</li>
  <li>no runtime-private scheduling, ABI, or storage policy is introduced.</li>
</ul>

<hr/>

<h2 id="example-expectations">12. Example Expectations</h2>

<p>
For the initial example corpus, the first deriver should produce at minimum:
</p>

<ul>
  <li><strong>01_pure_addition</strong> — two public input boundaries, one <code>frog.core.add</code> primitive, one public output boundary, three connections,</li>
  <li><strong>02_ui_value_roundtrip</strong> — two widget-value participation objects, one <code>frog.core.add</code> primitive, one widget-value participation output, three connections,</li>
  <li><strong>03_ui_property_write</strong> — one public input boundary, one widget-reference participation object, one <code>frog.ui.property_write</code> primitive, two connections,</li>
  <li><strong>04_stateful_feedback_delay</strong> — one public input boundary, one <code>frog.core.add</code> primitive, one explicit local-memory object for <code>frog.core.delay</code>, one public output boundary, four connections.</li>
</ul>

<hr/>

<h2 id="summary">13. Summary</h2>

<p>
This document provides a directly codable MVP pseudo-code sketch for <code>derive_execution_ir()</code> in the FROG reference implementation.
It keeps the derivation stage explicit,
attributable,
and strictly aligned with the published architecture:
validated meaning in,
open execution-facing IR out,
no lowering shortcuts,
no runtime-private leakage.
</p>
