<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 Reference Runtime — accept_contract_and_execute()</h1>

<p align="center">
  MVP runtime acceptance and execution pseudo-code for the non-normative FROG reference implementation<br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#status-and-boundary">2. Status and Boundary</a></li>
  <li><a href="#mvp-scope">3. MVP Scope</a></li>
  <li><a href="#selected-runtime-family">4. Selected Runtime Family</a></li>
  <li><a href="#input-contract">5. Input Contract</a></li>
  <li><a href="#output-contract">6. Output Contract</a></li>
  <li><a href="#runtime-strategy">7. Runtime Strategy</a></li>
  <li><a href="#runtime-phases">8. Runtime Phases</a></li>
  <li><a href="#helper-conventions">9. Helper Conventions</a></li>
  <li><a href="#pseudo-code-main-function">10. Pseudo-Code — Main Function</a></li>
  <li><a href="#pseudo-code-phase-functions">11. Pseudo-Code — Phase Functions</a></li>
  <li><a href="#mvp-acceptance-rules">12. MVP Acceptance Rules</a></li>
  <li><a href="#example-expectations">13. Example Expectations</a></li>
  <li><a href="#summary">14. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the first detailed pseudo-code sketch for <code>accept_contract_and_execute()</code> in the FROG reference runtime.
Its purpose is to consume a backend contract artifact,
accept or reject it explicitly,
and, if accepted,
execute it through the first reference runtime family.
</p>

<p>
The runtime is the first private realization stage of the reference implementation.
It is downstream from:
</p>

<ul>
  <li>canonical source,</li>
  <li>validation,</li>
  <li>Execution IR derivation,</li>
  <li>lowering,</li>
  <li>backend contract emission.</li>
</ul>

<p>
The runtime therefore does <strong>not</strong> define language truth.
It consumes a declared contract and realizes it privately.
</p>

<hr/>

<h2 id="status-and-boundary">2. Status and Boundary</h2>

<p>
This document is <strong>non-normative</strong>.
It does not redefine:
</p>

<ul>
  <li>the language,</li>
  <li>the open Execution IR,</li>
  <li>the lowering boundary,</li>
  <li>the backend contract boundary.</li>
</ul>

<p>
It is an implementation sketch for one reference runtime family.
If a runtime shortcut conflicts with the published contract boundary,
the contract boundary wins.
</p>

<hr/>

<h2 id="mvp-scope">3. MVP Scope</h2>

<p>
The first runtime slice covers:
</p>

<ul>
  <li>one backend contract to one execution session,</li>
  <li>public input and public output boundaries,</li>
  <li>deterministic step execution,</li>
  <li>ordinary core arithmetic computation for the MVP examples,</li>
  <li>UI value binding for <code>widget_value</code>-derived cases,</li>
  <li>UI reference resolution for <code>widget_reference</code>-derived cases,</li>
  <li>standardized UI operations:
    <ul>
      <li><code>property_read</code></li>
      <li><code>property_write</code></li>
      <li><code>method_invoke</code></li>
    </ul>
  </li>
  <li>explicit local memory initialization, read, and commit for <code>frog.core.delay</code>-derived cases.</li>
</ul>

<p>
The first slice does not attempt to define:
</p>

<ul>
  <li>a universal runtime scheduler,</li>
  <li>a multi-process runtime,</li>
  <li>a distributed runtime,</li>
  <li>a standardized UI event loop model,</li>
  <li>a universal debugger protocol.</li>
</ul>

<hr/>

<h2 id="selected-runtime-family">4. Selected Runtime Family</h2>

<p>
The first runtime family is:
</p>

<pre><code>reference_host_runtime_ui_binding</code></pre>

<p>
This family assumes:
</p>

<ul>
  <li>single-process host execution,</li>
  <li>deterministic step execution,</li>
  <li>optional UI value binding,</li>
  <li>optional UI reference binding,</li>
  <li>explicit local memory preservation when present,</li>
  <li>no first-class standardized UI event execution model.</li>
</ul>

<hr/>

<h2 id="input-contract">5. Input Contract</h2>

<p>
The runtime consumes a successful backend contract artifact conceptually equivalent to:
</p>

<pre><code>{
  "artifact_kind": "frog_backend_contract",
  "artifact_version": "0.1",
  "backend_family": "reference_host_runtime_ui_binding",
  "producer": "reference_contract_emitter",
  "compatibility": "family_specific",
  "source_ref": { ... },
  "assumptions": { ... },
  "units": [
    {
      "id": "main",
      "role": "entry_unit",
      "boundaries": [],
      "state": [],
      "operations": [],
      "attribution": { ... }
    }
  ],
  "unsupported": [],
  "diagnostics": []
}</code></pre>

<p>
The runtime assumes:
</p>

<ul>
  <li>the contract was emitted from a semantically valid lowered basis,</li>
  <li>the contract states all required assumptions explicitly,</li>
  <li>the contract is family-compatible if it is to be accepted.</li>
</ul>

<hr/>

<h2 id="output-contract">6. Output Contract</h2>

<p>
On success, the runtime returns an artifact conceptually equivalent to:
</p>

<pre><code>{
  "artifact_kind": "frog_runtime_result",
  "artifact_version": "0.1-dev",
  "backend_family": "reference_host_runtime_ui_binding",
  "contract_ref": {
    "unit_ids": ["main"]
  },
  "status": "ok",
  "execution_summary": {
    "mode": "deterministic_step_execution",
    "state_initialized": true,
    "ui_bound": false
  },
  "outputs": {
    "public": {},
    "ui": {}
  },
  "diagnostics": []
}</code></pre>

<p>
On rejection or failure, the runtime returns:
</p>

<pre><code>{
  "artifact_kind": "frog_runtime_result",
  "artifact_version": "0.1-dev",
  "backend_family": "reference_host_runtime_ui_binding",
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
The runtime must distinguish:
</p>

<ul>
  <li><strong>contract rejection</strong> — the contract is not acceptable for this runtime family,</li>
  <li><strong>execution failure</strong> — the contract was accepted but execution failed.</li>
</ul>

<hr/>

<h2 id="runtime-strategy">7. Runtime Strategy</h2>

<p>
The first runtime should use a strict staged strategy:
</p>

<ol>
  <li>confirm the contract is successful and family-compatible,</li>
  <li>check required assumptions,</li>
  <li>check unsupported declarations,</li>
  <li>create one execution session,</li>
  <li>bind public boundaries and UI boundaries,</li>
  <li>initialize explicit state,</li>
  <li>build an internal execution plan,</li>
  <li>execute one deterministic step or one simple run cycle depending on the example,</li>
  <li>collect outputs and diagnostics,</li>
  <li>return success or failure.</li>
</ol>

<p>
The first slice should prefer explicit runtime phases over aggressive optimization.
</p>

<hr/>

<h2 id="runtime-phases">8. Runtime Phases</h2>

<h3>8.1 Contract gate</h3>

<p>
Reject the contract if:
</p>

<ul>
  <li>the backend family does not match,</li>
  <li>required assumptions are missing,</li>
  <li>an unsupported feature is required for execution,</li>
  <li>explicit state semantics are required but not representable,</li>
  <li>a standardized UI event model is required.</li>
</ul>

<h3>8.2 Session creation</h3>

<p>
Create one runtime session for the accepted contract.
This session owns:
</p>

<ul>
  <li>boundary bindings,</li>
  <li>UI bindings,</li>
  <li>state cells,</li>
  <li>operation table,</li>
  <li>temporary values for the current deterministic step.</li>
</ul>

<h3>8.3 Boundary binding</h3>

<p>
Bind:
</p>

<ul>
  <li>public inputs,</li>
  <li>public outputs,</li>
  <li>UI value inputs,</li>
  <li>UI value outputs,</li>
  <li>UI references.</li>
</ul>

<h3>8.4 State initialization</h3>

<p>
Create state cells for every explicit local-memory declaration and initialize them deterministically from the contract.
</p>

<h3>8.5 Plan construction</h3>

<p>
Build a simple execution plan for the first slice.
For the MVP examples, a direct ordered operation list is sufficient.
</p>

<h3>8.6 Execution</h3>

<p>
Execute operations in deterministic order.
For stateful examples,
reads must observe the pre-step state and commits must update the next-step state only at the commit phase.
</p>

<h3>8.7 Output collection</h3>

<p>
Collect public outputs and UI-side effects into the runtime result artifact.
</p>

<hr/>

<h2 id="helper-conventions">9. Helper Conventions</h2>

<p>
The pseudo-code below uses the following conventions:
</p>

<ul>
  <li><code>ctx</code> is a mutable runtime context.</li>
  <li><code>ctx.errors</code> stores hard runtime failures.</li>
  <li><code>ctx.warnings</code> stores optional non-fatal notes.</li>
  <li><code>reject(...)</code> produces an explicit contract rejection.</li>
  <li><code>fail(...)</code> produces an execution failure after acceptance.</li>
  <li><code>session</code> is the runtime execution session.</li>
  <li><code>step_values</code> stores transient values for the current deterministic step.</li>
</ul>

<hr/>

<h2 id="pseudo-code-main-function">10. Pseudo-Code — Main Function</h2>

<pre><code class="language-text">function accept_contract_and_execute(contract, runtime_inputs = {}):
    ctx = new RuntimeContext()

    ctx.backend_family = "reference_host_runtime_ui_binding"

    if contract.get("status") == "error":
        return build_runtime_error(
            ctx,
            "runtime_requires_successful_contract",
            "accept_contract_and_execute() requires a successful backend contract artifact."
        )

    if contract["backend_family"] != ctx.backend_family:
        return build_runtime_rejection(
            ctx,
            "unsupported_backend_family",
            "The contract backend family is not supported by this runtime."
        )

    acceptance = check_contract_acceptance(contract, ctx)
    if acceptance == "reject":
        return build_runtime_rejection_from_ctx(ctx, "contract_rejected")

    session = create_runtime_session(contract, runtime_inputs, ctx)
    if ctx.has_errors():
        return build_runtime_error_from_ctx(ctx, "session_creation_failed")

    initialize_state(session, contract, ctx)
    if ctx.has_errors():
        return build_runtime_error_from_ctx(ctx, "state_initialization_failed")

    bind_boundaries(session, contract, runtime_inputs, ctx)
    if ctx.has_errors():
        return build_runtime_error_from_ctx(ctx, "boundary_binding_failed")

    plan = build_execution_plan(session, contract, ctx)
    if ctx.has_errors():
        return build_runtime_error_from_ctx(ctx, "execution_plan_failed")

    execute_plan(session, plan, ctx)
    if ctx.has_errors():
        return build_runtime_error_from_ctx(ctx, "execution_failed")

    result = collect_runtime_result(session, contract, ctx)
    return result</code></pre>

<hr/>

<h2 id="pseudo-code-phase-functions">11. Pseudo-Code — Phase Functions</h2>

<h3>11.1 Check contract acceptance</h3>

<pre><code class="language-text">function check_contract_acceptance(contract, ctx):
    if contract["artifact_kind"] != "frog_backend_contract":
        reject(ctx,
               code = "invalid_contract_kind",
               message = "Runtime expected artifact_kind = frog_backend_contract.")
        return "reject"

    required_assumptions = ["profiles_required", "scheduling", "ui_binding", "state_model"]
    for key in required_assumptions:
        if key not in contract["assumptions"]:
            reject(ctx,
                   code = "missing_contract_assumption",
                   message = "Missing required contract assumption: " + key)
            return "reject"

    if contract["assumptions"]["scheduling"].get("family_rule") != "deterministic_step_execution":
        reject(ctx,
               code = "unsupported_scheduling_rule",
               message = "The reference runtime supports only deterministic_step_execution.")
        return "reject"

    if contract["assumptions"]["ui_binding"].get("event_model") != "not_standardized":
        reject(ctx,
               code = "unsupported_ui_event_model",
               message = "The reference runtime does not support a standardized first-class UI event model.")
        return "reject"

    if len(contract.get("unsupported", [])) &gt; 0:
        for entry in contract["unsupported"]:
            if is_required_for_execution(entry):
                reject(ctx,
                       code = "unsupported_required_feature",
                       message = "Contract requires an unsupported feature: " + summarize_unsupported(entry))
                return "reject"

    if len(contract.get("units", [])) != 1:
        reject(ctx,
               code = "unsupported_unit_count",
               message = "The reference runtime MVP supports exactly one contract unit.")
        return "reject"

    return "accept"

function is_required_for_execution(entry):
    return true

function summarize_unsupported(entry):
    return to_string(entry.get("kind", "unknown"))</code></pre>

<h3>11.2 Create runtime session</h3>

<pre><code class="language-text">function create_runtime_session(contract, runtime_inputs, ctx):
    unit = contract["units"][0]

    session = {
        "contract": contract,
        "unit": unit,
        "public_inputs": {},
        "public_outputs": {},
        "ui_value_inputs": {},
        "ui_value_outputs": {},
        "ui_references": {},
        "state_cells": {},
        "step_values": {},
        "ui_effects": [],
        "operation_results": {}
    }

    return session</code></pre>

<h3>11.3 Initialize state</h3>

<pre><code class="language-text">function initialize_state(session, contract, ctx):
    for state_entry in session["unit"].get("state", []):
        if state_entry["kind"] != "explicit_local_memory":
            fail(ctx,
                 code = "unsupported_state_kind",
                 message = "Unsupported state kind in MVP runtime: " + to_string(state_entry["kind"]),
                 anchor = {"state_id": state_entry.get("id")})
            continue

        session["state_cells"][state_entry["id"]] = {
            "current": state_entry["initial"],
            "next": state_entry["initial"]
        }</code></pre>

<h3>11.4 Bind boundaries</h3>

<pre><code class="language-text">function bind_boundaries(session, contract, runtime_inputs, ctx):
    for boundary in session["unit"].get("boundaries", []):
        kind = boundary["kind"]

        if kind == "public_input":
            bind_public_input(session, boundary, runtime_inputs, ctx)
            continue

        if kind == "public_output":
            bind_public_output(session, boundary, ctx)
            continue

        if kind == "ui_value_input":
            bind_ui_value_input(session, boundary, runtime_inputs, ctx)
            continue

        if kind == "ui_value_output":
            bind_ui_value_output(session, boundary, ctx)
            continue

        if kind == "ui_reference":
            bind_ui_reference(session, boundary, runtime_inputs, ctx)
            continue

        fail(ctx,
             code = "unsupported_boundary_kind",
             message = "Unsupported boundary kind in MVP runtime: " + to_string(kind),
             anchor = {"boundary_id": boundary.get("id")})

function bind_public_input(session, boundary, runtime_inputs, ctx):
    name = boundary["name"]
    if name in runtime_inputs.get("public", {}):
        session["public_inputs"][name] = runtime_inputs["public"][name]
    else:
        session["public_inputs"][name] = null

function bind_public_output(session, boundary, ctx):
    session["public_outputs"][boundary["name"]] = null

function bind_ui_value_input(session, boundary, runtime_inputs, ctx):
    widget_id = boundary["widget_id"]
    if widget_id in runtime_inputs.get("ui", {}):
        session["ui_value_inputs"][widget_id] = runtime_inputs["ui"][widget_id]
    else:
        session["ui_value_inputs"][widget_id] = null

function bind_ui_value_output(session, boundary, ctx):
    session["ui_value_outputs"][boundary["widget_id"]] = null

function bind_ui_reference(session, boundary, runtime_inputs, ctx):
    widget_id = boundary["widget_id"]
    session["ui_references"][widget_id] = make_widget_handle(widget_id)

function make_widget_handle(widget_id):
    return {
        "widget_id": widget_id,
        "handle_kind": "reference_runtime_widget_handle"
    }</code></pre>

<h3>11.5 Build execution plan</h3>

<pre><code class="language-text">function build_execution_plan(session, contract, ctx):
    plan = {
        "mode": "deterministic_step_execution",
        "operations": session["unit"].get("operations", [])
    }

    return plan</code></pre>

<p>
For the first slice, the contract-visible operation order is assumed to be executable in order.
That is a family-specific simplification,
not a universal runtime rule.
</p>

<h3>11.6 Execute plan</h3>

<pre><code class="language-text">function execute_plan(session, plan, ctx):
    for op in plan["operations"]:
        execute_single_operation(session, op, ctx)

    commit_all_state(session, ctx)

function execute_single_operation(session, op, ctx):
    kind = op["kind"]

    if kind == "core_compute":
        execute_core_compute(session, op, ctx)
        return

    if kind == "ui_operation":
        execute_ui_operation(session, op, ctx)
        return

    if kind == "state_access":
        execute_state_access(session, op, ctx)
        return

    fail(ctx,
         code = "unsupported_contract_operation",
         message = "Unsupported contract operation in MVP runtime: " + to_string(kind),
         anchor = {"operation_id": op.get("id")})</code></pre>

<h3>11.7 Execute core compute</h3>

<pre><code class="language-text">function execute_core_compute(session, op, ctx):
    if op["operation"] != "frog.core.add":
        fail(ctx,
             code = "unsupported_core_compute_operation",
             message = "Unsupported core compute operation: " + to_string(op["operation"]),
             anchor = {"operation_id": op.get("id")})
        return

    inputs = resolve_add_inputs(session, ctx)
    if inputs is null:
        fail(ctx,
             code = "missing_add_inputs",
             message = "Could not resolve inputs for frog.core.add.",
             anchor = {"operation_id": op.get("id")})
        return

    result = inputs["a"] + inputs["b"]
    session["operation_results"][op["id"]] = {
        "result": result
    }

function resolve_add_inputs(session, ctx):
    # MVP example-oriented resolution:
    # - Example 01: public inputs a, b
    # - Example 02: UI value inputs ctrl_a, ctrl_b
    # - Example 04: public input x + state_read result
    #
    # The first runtime may use a simple example-compatible resolver.
    #
    # In a later iteration this should be replaced by a wire-aware local dataflow evaluator.

    if "a" in session["public_inputs"] and "b" in session["public_inputs"]:
        return {
            "a": default_zero_if_null(session["public_inputs"]["a"]),
            "b": default_zero_if_null(session["public_inputs"]["b"])
        }

    if "ctrl_a" in session["ui_value_inputs"] and "ctrl_b" in session["ui_value_inputs"]:
        return {
            "a": default_zero_if_null(session["ui_value_inputs"]["ctrl_a"]),
            "b": default_zero_if_null(session["ui_value_inputs"]["ctrl_b"])
        }

    if "x" in session["public_inputs"] and "state:last_read" in session["step_values"]:
        return {
            "a": default_zero_if_null(session["public_inputs"]["x"]),
            "b": session["step_values"]["state:last_read"]
        }

    return null

function default_zero_if_null(v):
    if v is null:
        return 0
    return v</code></pre>

<h3>11.8 Execute UI operations</h3>

<pre><code class="language-text">function execute_ui_operation(session, op, ctx):
    operation = op["operation"]

    if operation == "property_write":
        execute_ui_property_write(session, op, ctx)
        return

    if operation == "property_read":
        execute_ui_property_read(session, op, ctx)
        return

    if operation == "method_invoke":
        execute_ui_method_invoke(session, op, ctx)
        return

    fail(ctx,
         code = "unsupported_ui_operation",
         message = "Unsupported UI operation: " + to_string(operation),
         anchor = {"operation_id": op.get("id")})

function execute_ui_property_write(session, op, ctx):
    ref = resolve_first_ui_reference(session)
    value = resolve_ui_property_write_value(session)

    if ref is null:
        fail(ctx,
             code = "missing_ui_reference_for_property_write",
             message = "No UI reference is available for property_write.",
             anchor = {"operation_id": op.get("id")})
        return

    session["ui_effects"].append({
        "kind": "property_write",
        "widget_id": ref["widget_id"],
        "member": op["member"],
        "value": value
    })

function execute_ui_property_read(session, op, ctx):
    ref = resolve_first_ui_reference(session)
    if ref is null:
        fail(ctx,
             code = "missing_ui_reference_for_property_read",
             message = "No UI reference is available for property_read.",
             anchor = {"operation_id": op.get("id")})
        return

    session["operation_results"][op["id"]] = {
        "value": null
    }

function execute_ui_method_invoke(session, op, ctx):
    ref = resolve_first_ui_reference(session)
    if ref is null:
        fail(ctx,
             code = "missing_ui_reference_for_method_invoke",
             message = "No UI reference is available for method_invoke.",
             anchor = {"operation_id": op.get("id")})
        return

    session["ui_effects"].append({
        "kind": "method_invoke",
        "widget_id": ref["widget_id"],
        "method": op["method"]
    })

function resolve_first_ui_reference(session):
    for widget_id in session["ui_references"]:
        return session["ui_references"][widget_id]
    return null

function resolve_ui_property_write_value(session):
    if "status" in session["public_inputs"]:
        return session["public_inputs"]["status"]
    return null</code></pre>

<h3>11.9 Execute state access</h3>

<pre><code class="language-text">function execute_state_access(session, op, ctx):
    state_id = op["state_id"]

    if state_id not in session["state_cells"]:
        fail(ctx,
             code = "unknown_state_id",
             message = "Unknown state cell in runtime execution: " + state_id,
             anchor = {"operation_id": op.get("id")})
        return

    if op["operation"] == "read":
        session["step_values"]["state:last_read"] = session["state_cells"][state_id]["current"]
        return

    if op["operation"] == "commit":
        next_value = resolve_state_commit_value(session)
        session["state_cells"][state_id]["next"] = next_value
        return

    fail(ctx,
         code = "unsupported_state_access_operation",
         message = "Unsupported state access operation: " + to_string(op["operation"]),
         anchor = {"operation_id": op.get("id")})

function resolve_state_commit_value(session):
    for op_id in session["operation_results"]:
        if "result" in session["operation_results"][op_id]:
            return session["operation_results"][op_id]["result"]
    return 0</code></pre>

<h3>11.10 Commit state and publish outputs</h3>

<pre><code class="language-text">function commit_all_state(session, ctx):
    for state_id in session["state_cells"]:
        session["state_cells"][state_id]["current"] = session["state_cells"][state_id]["next"]

function publish_outputs(session, ctx):
    for op_id in session["operation_results"]:
        result_record = session["operation_results"][op_id]

        if "result" in result_record:
            if "result" in session["public_outputs"]:
                session["public_outputs"]["result"] = result_record["result"]
            if "y" in session["public_outputs"]:
                session["public_outputs"]["y"] = result_record["result"]
            if "ind_result" in session["ui_value_outputs"]:
                session["ui_value_outputs"]["ind_result"] = result_record["result"]</code></pre>

<h3>11.11 Collect runtime result</h3>

<pre><code class="language-text">function collect_runtime_result(session, contract, ctx):
    publish_outputs(session, ctx)

    ui_bound = (
        len(session["ui_value_inputs"]) &gt; 0 or
        len(session["ui_value_outputs"]) &gt; 0 or
        len(session["ui_references"]) &gt; 0
    )

    state_initialized = len(session["state_cells"]) &gt; 0
    state_updated = len(session["state_cells"]) &gt; 0

    ui_outputs = {}
    for widget_id in session["ui_value_outputs"]:
        ui_outputs[widget_id] = session["ui_value_outputs"][widget_id]

    for effect in session["ui_effects"]:
        if effect["kind"] == "property_write":
            ui_outputs[effect["widget_id"] + "." + effect["member"]["part"] + "." + effect["member"]["member"]] = effect["value"]
        if effect["kind"] == "method_invoke":
            ui_outputs[effect["widget_id"] + ".method:" + effect["method"]] = "invoked"

    return {
        "artifact_kind": "frog_runtime_result",
        "artifact_version": "0.1-dev",
        "backend_family": "reference_host_runtime_ui_binding",
        "contract_ref": {
            "unit_ids": [unit["id"] for unit in contract["units"]]
        },
        "status": "ok",
        "execution_summary": {
            "mode": "deterministic_step_execution",
            "state_initialized": state_initialized,
            "state_updated": state_updated,
            "ui_bound": ui_bound
        },
        "outputs": {
            "public": session["public_outputs"],
            "ui": ui_outputs
        },
        "diagnostics": ctx.warnings
    }</code></pre>

<h3>11.12 Utility helpers</h3>

<pre><code class="language-text">function reject(ctx, code, message):
    ctx.errors.append({
        "severity": "error",
        "kind": "rejection",
        "code": code,
        "message": message
    })

function fail(ctx, code, message, anchor = {}):
    ctx.errors.append({
        "severity": "error",
        "kind": "execution_failure",
        "code": code,
        "message": message,
        "source_anchor": anchor
    })

function build_runtime_rejection(ctx, code, message):
    return {
        "artifact_kind": "frog_runtime_result",
        "artifact_version": "0.1-dev",
        "backend_family": "reference_host_runtime_ui_binding",
        "status": "error",
        "error_code": code,
        "diagnostics": [
            {
                "severity": "error",
                "kind": "rejection",
                "message": message
            }
        ]
    }

function build_runtime_rejection_from_ctx(ctx, code):
    return {
        "artifact_kind": "frog_runtime_result",
        "artifact_version": "0.1-dev",
        "backend_family": "reference_host_runtime_ui_binding",
        "status": "error",
        "error_code": code,
        "diagnostics": ctx.errors + ctx.warnings
    }

function build_runtime_error(ctx, code, message):
    return {
        "artifact_kind": "frog_runtime_result",
        "artifact_version": "0.1-dev",
        "backend_family": "reference_host_runtime_ui_binding",
        "status": "error",
        "error_code": code,
        "diagnostics": [
            {
                "severity": "error",
                "kind": "execution_failure",
                "message": message
            }
        ]
    }

function build_runtime_error_from_ctx(ctx, code):
    return {
        "artifact_kind": "frog_runtime_result",
        "artifact_version": "0.1-dev",
        "backend_family": "reference_host_runtime_ui_binding",
        "status": "error",
        "error_code": code,
        "diagnostics": ctx.errors + ctx.warnings
    }</code></pre>

<hr/>

<h2 id="mvp-acceptance-rules">12. MVP Acceptance Rules</h2>

<p>
The first runtime must enforce at least the following acceptance rules:
</p>

<ul>
  <li>the contract must belong to <code>reference_host_runtime_ui_binding</code>,</li>
  <li>required assumptions must be declared explicitly,</li>
  <li>deterministic step execution must be the scheduling rule,</li>
  <li>a first-class standardized UI event model must not be required,</li>
  <li>explicit local-memory state must be representable when declared,</li>
  <li>UI reference and UI operation distinctions must not have been collapsed,</li>
  <li>public boundaries and UI boundaries must remain explicit enough for binding.</li>
</ul>

<hr/>

<h2 id="example-expectations">13. Example Expectations</h2>

<p>
For the initial example corpus, the first runtime should handle at minimum:
</p>

<ul>
  <li><strong>01_pure_addition</strong> — bind public inputs <code>a</code> and <code>b</code>, compute <code>result</code>, publish public output,</li>
  <li><strong>02_ui_value_roundtrip</strong> — bind UI value inputs <code>ctrl_a</code> and <code>ctrl_b</code>, compute result, publish UI value output <code>ind_result</code>,</li>
  <li><strong>03_ui_property_write</strong> — bind public input <code>status</code>, resolve UI reference <code>ctrl_gain</code>, emit property-write UI side effect for <code>label.text</code>,</li>
  <li><strong>04_stateful_feedback_delay</strong> — initialize explicit state, read state, compute result, commit next state, publish public output <code>y</code>.</li>
</ul>

<p>
These example behaviors are sufficient to prove the end-to-end executable slice of the first reference family.
</p>

<hr/>

<h2 id="summary">14. Summary</h2>

<p>
This document provides a directly codable MVP pseudo-code sketch for <code>accept_contract_and_execute()</code> in the FROG reference runtime.
It closes the first executable vertical slice:
backend contract in,
explicit acceptance or rejection,
private deterministic execution,
explicit UI binding where declared,
explicit state handling where required,
and a runtime result artifact out.
</p>
