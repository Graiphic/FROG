<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 Reference Lowerer — lower_for_backend_family()</h1>

<p align="center">
  MVP lowering pseudo-code for the non-normative FROG reference implementation<br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#status-and-boundary">2. Status and Boundary</a></li>
  <li><a href="#mvp-scope">3. MVP Scope</a></li>
  <li><a href="#selected-backend-family">4. Selected Backend Family</a></li>
  <li><a href="#input-contract">5. Input Contract</a></li>
  <li><a href="#output-contract">6. Output Contract</a></li>
  <li><a href="#lowering-strategy">7. Lowering Strategy</a></li>
  <li><a href="#lowering-phases">8. Lowering Phases</a></li>
  <li><a href="#helper-conventions">9. Helper Conventions</a></li>
  <li><a href="#pseudo-code-main-function">10. Pseudo-Code — Main Function</a></li>
  <li><a href="#pseudo-code-phase-functions">11. Pseudo-Code — Phase Functions</a></li>
  <li><a href="#mvp-preservation-rules">12. MVP Preservation Rules</a></li>
  <li><a href="#example-expectations">13. Example Expectations</a></li>
  <li><a href="#summary">14. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the first detailed pseudo-code sketch for <code>lower_for_backend_family()</code> in the FROG reference implementation.
Its purpose is to transform the derived open Execution IR into a <strong>backend-family-oriented lowered form</strong> that is still attributable, still semantically faithful, and ready for backend contract emission.
</p>

<p>
The lowerer is the first stage where backend-family specialization becomes explicit.
It must not redefine language meaning,
must not erase explicit state semantics,
and must not pretend that one private runtime shape is the language itself.
</p>

<hr/>

<h2 id="status-and-boundary">2. Status and Boundary</h2>

<p>
This document is <strong>non-normative</strong>.
It does not redefine:
</p>

<ul>
  <li>validated program meaning,</li>
  <li>the open Execution IR model,</li>
  <li>the derivation boundary,</li>
  <li>the backend contract boundary,</li>
  <li>runtime-private realization.</li>
</ul>

<p>
It is an implementation sketch that consumes the published lowering boundary and turns it into a directly codable reference-implementation procedure.
</p>

<hr/>

<h2 id="mvp-scope">3. MVP Scope</h2>

<p>
The first lowering slice covers:
</p>

<ul>
  <li>one derived execution unit to one lowered entry unit,</li>
  <li>public interface boundaries,</li>
  <li>ordinary primitive execution for the MVP examples,</li>
  <li><code>widget_value</code> lowering to value-binding endpoints,</li>
  <li><code>widget_reference</code> lowering to UI-reference handles,</li>
  <li>standardized UI primitives lowering to explicit UI operations,</li>
  <li><code>frog.core.delay</code> lowering to explicit state read / state commit style operations,</li>
  <li>deterministic step-oriented assumptions for the first reference family.</li>
</ul>

<p>
The first slice does not attempt to define:
</p>

<ul>
  <li>a universal scheduler,</li>
  <li>a universal ABI,</li>
  <li>a universal binary format,</li>
  <li>a first-class standardized event execution model.</li>
</ul>

<hr/>

<h2 id="selected-backend-family">4. Selected Backend Family</h2>

<p>
The first target family is:
</p>

<pre><code>reference_host_runtime_ui_binding</code></pre>

<p>
This family is intentionally narrow.
It assumes:
</p>

<ul>
  <li>single-process host execution,</li>
  <li>deterministic step execution,</li>
  <li>explicit state initialization and commit points,</li>
  <li>optional UI value binding,</li>
  <li>optional UI reference binding,</li>
  <li>no first-class standardized event model.</li>
</ul>

<hr/>

<h2 id="input-contract">5. Input Contract</h2>

<p>
The lowerer consumes a successful derived Execution IR artifact conceptually equivalent to:
</p>

<pre><code>{
  "artifact_kind": "frog_derived_execution_ir",
  "artifact_version": "0.1-dev",
  "source_ref": { ... },
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
The lowerer assumes:
</p>

<ul>
  <li>validation already succeeded,</li>
  <li>the derived IR is attributable,</li>
  <li>the derived IR preserves all MVP distinctions required by the published derivation boundary.</li>
</ul>

<hr/>

<h2 id="output-contract">6. Output Contract</h2>

<p>
On success, the lowerer returns an artifact conceptually equivalent to:
</p>

<pre><code>{
  "artifact_kind": "frog_lowered_form",
  "artifact_version": "0.1-dev",
  "backend_family": "reference_host_runtime_ui_binding",
  "source_ref": { ... },
  "ir_ref": {
    "execution_unit_id": "unit:main"
  },
  "assumptions": { ... },
  "units": [
    {
      "id": "lowered:main",
      "role": "entry_unit",
      "operations": [],
      "wires": [],
      "support_objects": []
    }
  ],
  "diagnostics": []
}</code></pre>

<p>
On failure, the lowerer returns:
</p>

<pre><code>{
  "artifact_kind": "frog_lowered_form",
  "artifact_version": "0.1-dev",
  "backend_family": "reference_host_runtime_ui_binding",
  "source_ref": { ... },
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

<hr/>

<h2 id="lowering-strategy">7. Lowering Strategy</h2>

<p>
The first lowerer should use a strict staged strategy:
</p>

<ol>
  <li>confirm the selected backend family is supported,</li>
  <li>confirm the derived IR is acceptable for the first slice,</li>
  <li>create one lowered unit from one execution unit,</li>
  <li>lower each execution-facing object to one or more backend-family operations,</li>
  <li>lower connections to family wires,</li>
  <li>introduce minimal support objects where useful,</li>
  <li>attach explicit family assumptions,</li>
  <li>run final preservation checks,</li>
  <li>emit the lowered artifact.</li>
</ol>

<p>
The first slice should prefer explicitness over compression.
A readable lowered form is better than an aggressively compact one.
</p>

<hr/>

<h2 id="lowering-phases">8. Lowering Phases</h2>

<h3>8.1 Family gate</h3>

<p>
Reject any backend family other than <code>reference_host_runtime_ui_binding</code> in the first slice.
</p>

<h3>8.2 Lowered unit creation</h3>

<p>
Create one lowered entry unit corresponding to the single execution unit of the MVP slice.
</p>

<h3>8.3 Object lowering</h3>

<p>
Transform each derived execution object into one or more family-oriented lowered operations.
</p>

<h3>8.4 Wire lowering</h3>

<p>
Transform each explicit connection into one explicit lowered wire between lowered operations and lowered inputs.
</p>

<h3>8.5 Support attachment</h3>

<p>
Attach minimal family-oriented support objects such as source maps, widget-handle metadata, or state metadata.
</p>

<h3>8.6 Final preservation check</h3>

<p>
Ensure that family specialization did not erase distinctions still required for backend contract emission.
</p>

<hr/>

<h2 id="helper-conventions">9. Helper Conventions</h2>

<p>
The pseudo-code below uses the following conventions:
</p>

<ul>
  <li><code>ctx</code> is a mutable lowering context.</li>
  <li><code>ctx.errors</code> stores hard lowering failures.</li>
  <li><code>ctx.warnings</code> stores optional non-fatal notes.</li>
  <li><code>fail(...)</code> appends an error and marks lowering as failed.</li>
  <li><code>emit_operation(...)</code> appends one lowered operation.</li>
  <li><code>emit_wire(...)</code> appends one lowered wire.</li>
  <li><code>origin</code> stores explicit attribution to the derived IR object and source anchors.</li>
</ul>

<hr/>

<h2 id="pseudo-code-main-function">10. Pseudo-Code — Main Function</h2>

<pre><code class="language-text">function lower_for_backend_family(derived_ir, backend_family):
    ctx = new LoweringContext()

    ctx.source_ref = derived_ir.source_ref
    ctx.ir_ref = {
        "execution_unit_id": derived_ir.execution_unit.id
    }
    ctx.backend_family = backend_family

    if backend_family != "reference_host_runtime_ui_binding":
        return build_lowering_error(
            ctx,
            "unsupported_backend_family",
            "The first lowering slice supports only reference_host_runtime_ui_binding."
        )

    if derived_ir.get("status") == "error":
        return build_lowering_error(
            ctx,
            "lowering_requires_successful_derived_ir",
            "lower_for_backend_family() requires a successful derived IR artifact."
        )

    create_lowered_unit(derived_ir, ctx)
    if ctx.has_errors():
        return build_lowering_error_from_ctx(ctx, "lowered_unit_creation_failed")

    attach_family_assumptions(ctx)

    lower_execution_objects(derived_ir, ctx)
    if ctx.has_errors():
        return build_lowering_error_from_ctx(ctx, "object_lowering_failed")

    lower_connections(derived_ir, ctx)
    if ctx.has_errors():
        return build_lowering_error_from_ctx(ctx, "wire_lowering_failed")

    attach_support_objects(derived_ir, ctx)
    if ctx.has_errors():
        return build_lowering_error_from_ctx(ctx, "support_attachment_failed")

    run_final_lowering_checks(derived_ir, ctx)
    if ctx.has_errors():
        return build_lowering_error_from_ctx(ctx, "final_lowering_check_failed")

    return {
        "artifact_kind": "frog_lowered_form",
        "artifact_version": "0.1-dev",
        "backend_family": ctx.backend_family,
        "source_ref": ctx.source_ref,
        "ir_ref": ctx.ir_ref,
        "assumptions": ctx.assumptions,
        "units": [ctx.lowered_unit],
        "diagnostics": ctx.warnings
    }</code></pre>

<hr/>

<h2 id="pseudo-code-phase-functions">11. Pseudo-Code — Phase Functions</h2>

<h3>11.1 Create lowered unit</h3>

<pre><code class="language-text">function create_lowered_unit(derived_ir, ctx):
    ctx.lowered_unit = {
        "id": "lowered:main",
        "role": "entry_unit",
        "origin": {
            "execution_unit_id": derived_ir.execution_unit.id,
            "sources": derived_ir.execution_unit.get("sources", [])
        },
        "operations": [],
        "wires": [],
        "support_objects": []
    }</code></pre>

<h3>11.2 Attach family assumptions</h3>

<pre><code class="language-text">function attach_family_assumptions(ctx):
    ctx.assumptions = {
        "deterministic_step_execution": true,
        "single_process_host_runtime": true,
        "ui_binding_enabled": false,
        "ui_event_model": "not_standardized",
        "state_model": "explicit_local_memory_preserved_if_present"
    }</code></pre>

<h3>11.3 Lower execution objects</h3>

<pre><code class="language-text">function lower_execution_objects(derived_ir, ctx):
    for obj in derived_ir.execution_unit["objects"]:
        lower_single_object(obj, ctx)

function lower_single_object(obj, ctx):
    kind = obj["kind"]

    if kind == "public_input_boundary":
        lower_public_input_boundary(obj, ctx)
        return

    if kind == "public_output_boundary":
        lower_public_output_boundary(obj, ctx)
        return

    if kind == "primitive":
        lower_core_primitive(obj, ctx)
        return

    if kind == "explicit_local_memory":
        lower_explicit_local_memory(obj, ctx)
        return

    if kind == "widget_value_participation":
        lower_widget_value_participation(obj, ctx)
        return

    if kind == "widget_reference_participation":
        lower_widget_reference_participation(obj, ctx)
        return

    if kind == "ui_object_primitive":
        lower_ui_object_primitive(obj, ctx)
        return

    fail(ctx,
         code = "unsupported_object_kind_for_lowering",
         message = "Unsupported derived object kind for MVP lowering: " + to_string(kind),
         anchor = {"object_id": obj["id"]})</code></pre>

<h3>11.4 Public boundaries</h3>

<pre><code class="language-text">function lower_public_input_boundary(obj, ctx):
    emit_operation(ctx, {
        "id": "op:" + obj["id"],
        "kind": "public_input",
        "name": obj["interface_port"],
        "origin": make_origin(obj),
        "outputs": ["value"]
    })

function lower_public_output_boundary(obj, ctx):
    emit_operation(ctx, {
        "id": "op:" + obj["id"],
        "kind": "public_output",
        "name": obj["interface_port"],
        "origin": make_origin(obj),
        "inputs": ["value"]
    })</code></pre>

<h3>11.5 Core primitives</h3>

<pre><code class="language-text">function lower_core_primitive(obj, ctx):
    primitive_ref = obj["primitive_ref"]

    if primitive_ref == "frog.core.add":
        emit_operation(ctx, {
            "id": "op:" + obj["id"],
            "kind": "core_primitive_add",
            "origin": make_origin(obj),
            "inputs": ["a", "b"],
            "outputs": ["result"]
        })
        return

    fail(ctx,
         code = "unsupported_core_primitive_for_lowering",
         message = "Unsupported core primitive for MVP lowering: " + primitive_ref,
         anchor = {"object_id": obj["id"]})</code></pre>

<h3>11.6 Explicit local memory</h3>

<pre><code class="language-text">function lower_explicit_local_memory(obj, ctx):
    state_id = "state:" + obj["id"]

    ctx.assumptions["state_model"] = "explicit_local_memory_preserved_if_present"

    emit_operation(ctx, {
        "id": "op:" + obj["id"] + ":read",
        "kind": "state_read",
        "state_id": state_id,
        "origin": make_origin(obj),
        "outputs": ["value"]
    })

    emit_operation(ctx, {
        "id": "op:" + obj["id"] + ":commit",
        "kind": "state_commit",
        "state_id": state_id,
        "initial": obj["initial"],
        "origin": make_origin(obj),
        "inputs": ["value"]
    })

    ctx.lowered_unit["support_objects"].append({
        "id": state_id,
        "kind": "explicit_local_memory_state",
        "initial": obj["initial"],
        "origin": make_origin(obj)
    })</code></pre>

<h3>11.7 Widget-value participation</h3>

<pre><code class="language-text">function lower_widget_value_participation(obj, ctx):
    ctx.assumptions["ui_binding_enabled"] = true

    direction = resolve_widget_value_direction(obj)

    if direction == "out":
        emit_operation(ctx, {
            "id": "op:" + obj["id"],
            "kind": "ui_value_in",
            "widget_id": obj["widget_id"],
            "origin": make_origin(obj),
            "outputs": ["value"]
        })
        return

    if direction == "in":
        emit_operation(ctx, {
            "id": "op:" + obj["id"],
            "kind": "ui_value_out",
            "widget_id": obj["widget_id"],
            "origin": make_origin(obj),
            "inputs": ["value"]
        })
        return

    fail(ctx,
         code = "unresolved_widget_value_direction",
         message = "Could not resolve widget_value direction during lowering.",
         anchor = {"object_id": obj["id"]})

function resolve_widget_value_direction(obj):
    ports = obj.get("ports", [])
    if len(ports) == 0:
        return "unknown"

    # MVP convention:
    # the deriver may leave the direction unresolved textually,
    # but the lowerer must resolve it from the validated role when available.
    #
    # In the first reference slice we assume:
    # - control-side widget_value => out
    # - indicator-side widget_value => in
    #
    # This can be refined later when richer validated facts are available.

    inferred_role = obj.get("widget_role_hint")
    if inferred_role == "control":
        return "out"
    if inferred_role == "indicator":
        return "in"

    # Fallback: preserve explicit uncertainty as failure in the MVP slice
    return "unknown"</code></pre>

<h3>11.8 Widget-reference participation</h3>

<pre><code class="language-text">function lower_widget_reference_participation(obj, ctx):
    ctx.assumptions["ui_binding_enabled"] = true

    emit_operation(ctx, {
        "id": "op:" + obj["id"],
        "kind": "ui_reference_handle",
        "widget_id": obj["widget_id"],
        "origin": make_origin(obj),
        "outputs": ["ref"]
    })</code></pre>

<h3>11.9 UI-object primitives</h3>

<pre><code class="language-text">function lower_ui_object_primitive(obj, ctx):
    ctx.assumptions["ui_binding_enabled"] = true

    primitive_ref = obj["primitive_ref"]

    if primitive_ref == "frog.ui.property_read":
        emit_operation(ctx, {
            "id": "op:" + obj["id"],
            "kind": "ui_property_read",
            "member": obj["member"],
            "origin": make_origin(obj),
            "inputs": ["ref"],
            "outputs": ["value"]
        })
        return

    if primitive_ref == "frog.ui.property_write":
        emit_operation(ctx, {
            "id": "op:" + obj["id"],
            "kind": "ui_property_write",
            "member": obj["member"],
            "origin": make_origin(obj),
            "inputs": ["ref", "value"]
        })
        return

    if primitive_ref == "frog.ui.method_invoke":
        emit_operation(ctx, {
            "id": "op:" + obj["id"],
            "kind": "ui_method_invoke",
            "method": obj["method"],
            "origin": make_origin(obj),
            "inputs": ["ref"]
        })
        return

    fail(ctx,
         code = "unsupported_ui_primitive_for_lowering",
         message = "Unsupported UI primitive for MVP lowering: " + primitive_ref,
         anchor = {"object_id": obj["id"]})</code></pre>

<h3>11.10 Lower connections</h3>

<pre><code class="language-text">function lower_connections(derived_ir, ctx):
    for conn in derived_ir.execution_unit["connections"]:
        lower_single_connection(conn, ctx)

function lower_single_connection(conn, ctx):
    from_operation = resolve_lowered_endpoint_for_source(conn["from"]["object"], conn["from"]["port"], ctx)
    to_operation = resolve_lowered_endpoint_for_target(conn["to"]["object"], conn["to"]["port"], ctx)

    if from_operation is null:
        fail(ctx,
             code = "unresolved_lowered_source_endpoint",
             message = "Could not resolve lowered source endpoint.",
             anchor = {"connection_id": conn["id"]})
        return

    if to_operation is null:
        fail(ctx,
             code = "unresolved_lowered_target_endpoint",
             message = "Could not resolve lowered target endpoint.",
             anchor = {"connection_id": conn["id"]})
        return

    emit_wire(ctx, {
        "id": "wire:" + conn["id"],
        "from": from_operation,
        "to": to_operation,
        "origin": {
            "connection_id": conn["id"],
            "sources": conn.get("sources", [])
        }
    })</code></pre>

<h3>11.11 Endpoint resolution</h3>

<pre><code class="language-text">function resolve_lowered_endpoint_for_source(object_id, port_id, ctx):
    op_base = "op:" + object_id

    # explicit local memory split into read/commit
    if operation_exists(ctx, op_base + ":read"):
        if port_id == "out":
            return { "operation": op_base + ":read", "port": "value" }

    if operation_exists(ctx, op_base):
        return { "operation": op_base, "port": port_id }

    return null

function resolve_lowered_endpoint_for_target(object_id, port_id, ctx):
    op_base = "op:" + object_id

    # explicit local memory split into read/commit
    if operation_exists(ctx, op_base + ":commit"):
        if port_id == "in":
            return { "operation": op_base + ":commit", "port": "value" }

    if operation_exists(ctx, op_base):
        return { "operation": op_base, "port": port_id }

    return null</code></pre>

<h3>11.12 Support objects</h3>

<pre><code class="language-text">function attach_support_objects(derived_ir, ctx):
    ctx.lowered_unit["support_objects"].append({
        "id": "support:lowering_source_map",
        "kind": "lowering_source_map",
        "origin": {
            "execution_unit_id": derived_ir.execution_unit.id,
            "sources": derived_ir.execution_unit.get("sources", [])
        }
    })</code></pre>

<h3>11.13 Final checks</h3>

<pre><code class="language-text">function run_final_lowering_checks(derived_ir, ctx):
    ensure_lowered_unit_exists(ctx)
    ensure_all_operations_are_attributable(ctx)
    ensure_all_wires_are_attributable(ctx)
    ensure_explicit_state_still_exists_if_required(derived_ir, ctx)
    ensure_ui_value_and_ui_reference_still_differ(ctx)
    ensure_ui_reference_and_ui_operation_still_differ(ctx)
    ensure_no_event_model_was_silently_introduced(ctx)

function ensure_lowered_unit_exists(ctx):
    if ctx.lowered_unit is null:
        fail(ctx,
             code = "missing_lowered_unit",
             message = "Lowering did not produce a lowered unit.",
             anchor = {})

function ensure_all_operations_are_attributable(ctx):
    for op in ctx.lowered_unit["operations"]:
        if "origin" not in op:
            fail(ctx,
                 code = "unattributable_lowered_operation",
                 message = "Lowered operation has no origin.",
                 anchor = {"operation_id": op["id"]})

function ensure_all_wires_are_attributable(ctx):
    for wire in ctx.lowered_unit["wires"]:
        if "origin" not in wire:
            fail(ctx,
                 code = "unattributable_lowered_wire",
                 message = "Lowered wire has no origin.",
                 anchor = {"wire_id": wire["id"]})

function ensure_explicit_state_still_exists_if_required(derived_ir, ctx):
    had_explicit_state = any(obj["kind"] == "explicit_local_memory" for obj in derived_ir.execution_unit["objects"])
    has_lowered_state = any(obj["kind"] == "explicit_local_memory_state" for obj in ctx.lowered_unit["support_objects"])

    if had_explicit_state and not has_lowered_state:
        fail(ctx,
             code = "explicit_state_erased_during_lowering",
             message = "Explicit local memory was lost during lowering.",
             anchor = {})

function ensure_ui_value_and_ui_reference_still_differ(ctx):
    kinds = [op["kind"] for op in ctx.lowered_unit["operations"]]
    if "ui_value_in" in kinds and "ui_reference_handle" in kinds:
        return
    # Only check collapse when both source categories were present
    # For the MVP slice, simple presence checks are sufficient.

function ensure_ui_reference_and_ui_operation_still_differ(ctx):
    reference_ops = [op for op in ctx.lowered_unit["operations"] if op["kind"] == "ui_reference_handle"]
    ui_ops = [op for op in ctx.lowered_unit["operations"] if op["kind"] in ["ui_property_read", "ui_property_write", "ui_method_invoke"]]

    for ref in reference_ops:
        for ui_op in ui_ops:
            if ref["id"] == ui_op["id"]:
                fail(ctx,
                     code = "ui_reference_ui_operation_collapse_in_lowering",
                     message = "UI reference handle was collapsed with a UI operation.",
                     anchor = {"operation_id": ref["id"]})

function ensure_no_event_model_was_silently_introduced(ctx):
    if ctx.assumptions.get("ui_event_model") not in ["not_standardized"]:
        fail(ctx,
             code = "unexpected_ui_event_model_in_first_slice",
             message = "The first lowering slice must not introduce a standardized UI event model.",
             anchor = {})</code></pre>

<h3>11.14 Utility helpers</h3>

<pre><code class="language-text">function emit_operation(ctx, op):
    ctx.lowered_unit["operations"].append(op)

function emit_wire(ctx, wire):
    ctx.lowered_unit["wires"].append(wire)

function operation_exists(ctx, operation_id):
    for op in ctx.lowered_unit["operations"]:
        if op["id"] == operation_id:
            return true
    return false

function make_origin(obj):
    return {
        "derived_object_id": obj["id"],
        "sources": obj.get("sources", [])
    }

function build_lowering_error(ctx, code, message):
    return {
        "artifact_kind": "frog_lowered_form",
        "artifact_version": "0.1-dev",
        "backend_family": ctx.backend_family,
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

function build_lowering_error_from_ctx(ctx, code):
    return {
        "artifact_kind": "frog_lowered_form",
        "artifact_version": "0.1-dev",
        "backend_family": ctx.backend_family,
        "source_ref": ctx.source_ref,
        "status": "error",
        "error_code": code,
        "diagnostics": ctx.errors + ctx.warnings
    }</code></pre>

<hr/>

<h2 id="mvp-preservation-rules">12. MVP Preservation Rules</h2>

<p>
The first lowerer must enforce at least the following rules:
</p>

<ul>
  <li>one derived execution unit becomes one lowered entry unit,</li>
  <li>public boundaries remain recognizable as public boundaries,</li>
  <li><code>widget_value</code> lowers to UI value endpoints rather than UI reference handles,</li>
  <li><code>widget_reference</code> lowers to UI reference handles rather than generic valueflow,</li>
  <li>standardized UI primitives lower to explicit UI operations,</li>
  <li><code>frog.core.delay</code> lowers to explicit state read / state commit style machinery plus explicit state support,</li>
  <li>the first slice must not silently introduce a standardized UI event model,</li>
  <li>the lowered form must remain attributable and ready for backend contract emission.</li>
</ul>

<hr/>

<h2 id="example-expectations">13. Example Expectations</h2>

<p>
For the initial example corpus, the first lowerer should produce at minimum:
</p>

<ul>
  <li><strong>01_pure_addition</strong> — public input operations, one <code>core_primitive_add</code>, one public output operation, explicit wires,</li>
  <li><strong>02_ui_value_roundtrip</strong> — two <code>ui_value_in</code> operations, one <code>core_primitive_add</code>, one <code>ui_value_out</code> operation, explicit wires,</li>
  <li><strong>03_ui_property_write</strong> — one public input operation, one <code>ui_reference_handle</code>, one <code>ui_property_write</code> operation, explicit wires,</li>
  <li><strong>04_stateful_feedback_delay</strong> — one public input operation, one <code>core_primitive_add</code>, one <code>state_read</code>, one <code>state_commit</code>, one public output operation, explicit state support, explicit wires.</li>
</ul>

<hr/>

<h2 id="summary">14. Summary</h2>

<p>
This document provides a directly codable MVP pseudo-code sketch for <code>lower_for_backend_family()</code> in the FROG reference implementation.
It keeps lowering explicit,
family-oriented,
and semantically disciplined:
open execution-facing IR in,
backend-family lowered form out,
with attribution,
explicit state preservation,
UI distinction preservation,
and no runtime-private semantic laundering.
</p>
