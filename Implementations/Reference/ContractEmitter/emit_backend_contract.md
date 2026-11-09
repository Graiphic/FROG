<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 Reference Contract Emitter — emit_backend_contract()</h1>

<p align="center">
  MVP backend contract emission pseudo-code for the non-normative FROG reference implementation<br/>
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
  <li><a href="#emission-strategy">7. Emission Strategy</a></li>
  <li><a href="#emission-phases">8. Emission Phases</a></li>
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
This document defines the first detailed pseudo-code sketch for <code>emit_backend_contract()</code> in the FROG reference implementation.
Its purpose is to transform a backend-family-oriented lowered form into a <strong>consumer-facing backend contract artifact</strong> that can be accepted or rejected explicitly by the reference runtime.
</p>

<p>
The contract emitter is the stage that turns family-oriented specialization into declared consumption truth:
</p>

<ul>
  <li>what executable units are offered,</li>
  <li>which assumptions are fixed,</li>
  <li>which boundaries remain explicit,</li>
  <li>which state semantics remain explicit,</li>
  <li>which attribution and diagnostics support remain available,</li>
  <li>which unsupported situations must be rejected.</li>
</ul>

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
  <li>the normative backend contract architecture,</li>
  <li>runtime-private realization.</li>
</ul>

<p>
It is an implementation sketch that consumes the published backend contract boundary and turns it into a directly codable reference-implementation procedure.
</p>

<hr/>

<h2 id="mvp-scope">3. MVP Scope</h2>

<p>
The first backend-contract emission slice covers:
</p>

<ul>
  <li>one lowered unit to one consumable entry unit,</li>
  <li>public interface boundaries,</li>
  <li>UI value participation boundaries,</li>
  <li>UI reference participation and explicit UI operations,</li>
  <li>explicit local memory state declarations,</li>
  <li>deterministic step execution assumptions,</li>
  <li>diagnostic support and attribution support,</li>
  <li>explicit unsupported-feature declaration.</li>
</ul>

<p>
The first slice does not attempt to define:
</p>

<ul>
  <li>a universal binary transport,</li>
  <li>a universal ABI,</li>
  <li>a universal debugger protocol,</li>
  <li>a universal deployment package,</li>
  <li>a first-class standardized UI event model.</li>
</ul>

<hr/>

<h2 id="selected-backend-family">4. Selected Backend Family</h2>

<p>
The first target family is:
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
  <li>no first-class standardized event model.</li>
</ul>

<hr/>

<h2 id="input-contract">5. Input Contract</h2>

<p>
The contract emitter consumes a successful lowered form artifact conceptually equivalent to:
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
The emitter assumes:
</p>

<ul>
  <li>the lowered form is semantically valid for the selected backend family,</li>
  <li>all required distinctions still exist in lowered explicit form,</li>
  <li>family assumptions are already attached explicitly,</li>
  <li>the lowered form remains attributable.</li>
</ul>

<hr/>

<h2 id="output-contract">6. Output Contract</h2>

<p>
On success, the emitter returns a backend contract artifact conceptually equivalent to:
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
On failure, the emitter returns:
</p>

<pre><code>{
  "artifact_kind": "frog_backend_contract",
  "artifact_version": "0.1",
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

<p>
The emitted artifact is intended to be directly consumable by the reference runtime family.
</p>

<hr/>

<h2 id="emission-strategy">7. Emission Strategy</h2>

<p>
The first emitter should use a strict staged strategy:
</p>

<ol>
  <li>confirm the lowered form is successful and family-compatible,</li>
  <li>create one backend contract skeleton,</li>
  <li>map lowered units to contract units,</li>
  <li>derive contract-visible boundaries from lowered operations,</li>
  <li>derive explicit state declarations from lowered support objects and operations,</li>
  <li>derive contract-visible operations where needed,</li>
  <li>attach assumptions, attribution, and unsupported declarations,</li>
  <li>run final preservation checks,</li>
  <li>emit the backend contract artifact.</li>
</ol>

<p>
The first slice should prefer explicit boundaries and explicit declarations over compressed transport.
</p>

<hr/>

<h2 id="emission-phases">8. Emission Phases</h2>

<h3>8.1 Family gate</h3>

<p>
Reject any lowered artifact whose <code>backend_family</code> is not supported by the reference emitter.
</p>

<h3>8.2 Contract skeleton creation</h3>

<p>
Create the backend contract header and root assumption object.
</p>

<h3>8.3 Unit emission</h3>

<p>
Create one contract unit from the one lowered entry unit of the MVP slice.
</p>

<h3>8.4 Boundary emission</h3>

<p>
Convert lowered public input/output operations and lowered UI participation operations into contract-visible boundaries.
</p>

<h3>8.5 State emission</h3>

<p>
Convert lowered explicit state support into contract-visible explicit local-memory declarations.
</p>

<h3>8.6 Operation emission</h3>

<p>
Expose only the operation categories that still matter at the contract boundary for the selected family.
</p>

<h3>8.7 Attribution emission</h3>

<p>
Attach source-mapping and diagnostic support declarations.
</p>

<h3>8.8 Final checks</h3>

<p>
Ensure that all contract-required distinctions remain explicit enough for consumption by the runtime family.
</p>

<hr/>

<h2 id="helper-conventions">9. Helper Conventions</h2>

<p>
The pseudo-code below uses the following conventions:
</p>

<ul>
  <li><code>ctx</code> is a mutable contract-emission context.</li>
  <li><code>ctx.errors</code> stores hard emission failures.</li>
  <li><code>ctx.warnings</code> stores optional non-fatal notes.</li>
  <li><code>fail(...)</code> appends an error and marks emission as failed.</li>
  <li><code>emit_boundary(...)</code> appends one contract boundary declaration.</li>
  <li><code>emit_state(...)</code> appends one explicit state declaration.</li>
  <li><code>emit_contract_operation(...)</code> appends one contract-visible operation descriptor.</li>
</ul>

<hr/>

<h2 id="pseudo-code-main-function">10. Pseudo-Code — Main Function</h2>

<pre><code class="language-text">function emit_backend_contract(lowered_form):
    ctx = new ContractEmitterContext()

    ctx.source_ref = lowered_form.source_ref
    ctx.backend_family = lowered_form.backend_family

    if lowered_form.get("status") == "error":
        return build_contract_error(
            ctx,
            "contract_emission_requires_successful_lowered_form",
            "emit_backend_contract() requires a successful lowered form artifact."
        )

    if lowered_form.backend_family != "reference_host_runtime_ui_binding":
        return build_contract_error(
            ctx,
            "unsupported_backend_family",
            "The first emitter slice supports only reference_host_runtime_ui_binding."
        )

    create_contract_skeleton(lowered_form, ctx)
    if ctx.has_errors():
        return build_contract_error_from_ctx(ctx, "contract_skeleton_creation_failed")

    emit_contract_units(lowered_form, ctx)
    if ctx.has_errors():
        return build_contract_error_from_ctx(ctx, "contract_unit_emission_failed")

    attach_contract_assumptions(lowered_form, ctx)
    attach_unsupported_declarations(lowered_form, ctx)

    run_final_contract_checks(lowered_form, ctx)
    if ctx.has_errors():
        return build_contract_error_from_ctx(ctx, "final_contract_check_failed")

    return {
        "artifact_kind": "frog_backend_contract",
        "artifact_version": "0.1",
        "backend_family": ctx.backend_family,
        "producer": "reference_contract_emitter",
        "compatibility": "family_specific",
        "source_ref": ctx.source_ref,
        "assumptions": ctx.assumptions,
        "units": ctx.units,
        "unsupported": ctx.unsupported,
        "diagnostics": ctx.warnings
    }</code></pre>

<hr/>

<h2 id="pseudo-code-phase-functions">11. Pseudo-Code — Phase Functions</h2>

<h3>11.1 Create contract skeleton</h3>

<pre><code class="language-text">function create_contract_skeleton(lowered_form, ctx):
    ctx.assumptions = {}
    ctx.units = []
    ctx.unsupported = []

    if len(lowered_form["units"]) != 1:
        fail(ctx,
             code = "unsupported_unit_count_for_mvp_contract",
             message = "The first contract slice expects exactly one lowered unit.",
             anchor = {"unit_count": len(lowered_form["units"])})</code></pre>

<h3>11.2 Emit contract units</h3>

<pre><code class="language-text">function emit_contract_units(lowered_form, ctx):
    for lowered_unit in lowered_form["units"]:
        emit_single_contract_unit(lowered_unit, lowered_form, ctx)

function emit_single_contract_unit(lowered_unit, lowered_form, ctx):
    contract_unit = {
        "id": "main",
        "role": lowered_unit["role"],
        "origin": lowered_unit["origin"],
        "boundaries": [],
        "state": [],
        "operations": [],
        "attribution": {
            "source_mapping_available": true,
            "fault_anchor_support": true,
            "debug_anchor_support": true
        }
    }

    emit_boundaries_from_lowered_unit(lowered_unit, contract_unit, ctx)
    emit_state_from_lowered_unit(lowered_unit, contract_unit, ctx)
    emit_operations_from_lowered_unit(lowered_unit, contract_unit, ctx)

    ctx.units.append(contract_unit)</code></pre>

<h3>11.3 Emit boundaries</h3>

<pre><code class="language-text">function emit_boundaries_from_lowered_unit(lowered_unit, contract_unit, ctx):
    for op in lowered_unit["operations"]:
        kind = op["kind"]

        if kind == "public_input":
            emit_boundary(contract_unit, {
                "id": "b:" + op["id"],
                "kind": "public_input",
                "name": op["name"],
                "origin": op["origin"]
            })
            continue

        if kind == "public_output":
            emit_boundary(contract_unit, {
                "id": "b:" + op["id"],
                "kind": "public_output",
                "name": op["name"],
                "origin": op["origin"]
            })
            continue

        if kind == "ui_value_in":
            emit_boundary(contract_unit, {
                "id": "b:" + op["id"],
                "kind": "ui_value_input",
                "widget_id": op["widget_id"],
                "origin": op["origin"]
            })
            continue

        if kind == "ui_value_out":
            emit_boundary(contract_unit, {
                "id": "b:" + op["id"],
                "kind": "ui_value_output",
                "widget_id": op["widget_id"],
                "origin": op["origin"]
            })
            continue

        if kind == "ui_reference_handle":
            emit_boundary(contract_unit, {
                "id": "b:" + op["id"],
                "kind": "ui_reference",
                "widget_id": op["widget_id"],
                "origin": op["origin"]
            })
            continue</code></pre>

<h3>11.4 Emit state</h3>

<pre><code class="language-text">function emit_state_from_lowered_unit(lowered_unit, contract_unit, ctx):
    for support_obj in lowered_unit.get("support_objects", []):
        if support_obj["kind"] == "explicit_local_memory_state":
            emit_state(contract_unit, {
                "id": support_obj["id"],
                "kind": "explicit_local_memory",
                "initial": support_obj["initial"],
                "origin": support_obj["origin"]
            })</code></pre>

<h3>11.5 Emit contract-visible operations</h3>

<pre><code class="language-text">function emit_operations_from_lowered_unit(lowered_unit, contract_unit, ctx):
    for op in lowered_unit["operations"]:
        kind = op["kind"]

        if kind == "core_primitive_add":
            emit_contract_operation(contract_unit, {
                "id": "c:" + op["id"],
                "kind": "core_compute",
                "operation": "frog.core.add",
                "origin": op["origin"]
            })
            continue

        if kind == "ui_property_read":
            emit_contract_operation(contract_unit, {
                "id": "c:" + op["id"],
                "kind": "ui_operation",
                "operation": "property_read",
                "member": op["member"],
                "origin": op["origin"]
            })
            continue

        if kind == "ui_property_write":
            emit_contract_operation(contract_unit, {
                "id": "c:" + op["id"],
                "kind": "ui_operation",
                "operation": "property_write",
                "member": op["member"],
                "origin": op["origin"]
            })
            continue

        if kind == "ui_method_invoke":
            emit_contract_operation(contract_unit, {
                "id": "c:" + op["id"],
                "kind": "ui_operation",
                "operation": "method_invoke",
                "method": op["method"],
                "origin": op["origin"]
            })
            continue

        if kind == "state_read":
            emit_contract_operation(contract_unit, {
                "id": "c:" + op["id"],
                "kind": "state_access",
                "operation": "read",
                "state_id": op["state_id"],
                "origin": op["origin"]
            })
            continue

        if kind == "state_commit":
            emit_contract_operation(contract_unit, {
                "id": "c:" + op["id"],
                "kind": "state_access",
                "operation": "commit",
                "state_id": op["state_id"],
                "origin": op["origin"]
            })
            continue</code></pre>

<p>
For the first slice, public boundaries and UI participations are emitted as boundaries.
Core arithmetic, UI-object operations, and explicit state access are emitted as contract-visible operations.
</p>

<h3>11.6 Attach assumptions</h3>

<pre><code class="language-text">function attach_contract_assumptions(lowered_form, ctx):
    lowered_assumptions = lowered_form.get("assumptions", {})

    ctx.assumptions["profiles_required"] = []
    ctx.assumptions["scheduling"] = {
        "fixed": lowered_assumptions.get("deterministic_step_execution", false),
        "family_rule": "deterministic_step_execution"
    }

    ctx.assumptions["ui_binding"] = {
        "enabled": lowered_assumptions.get("ui_binding_enabled", false),
        "event_model": lowered_assumptions.get("ui_event_model", "not_standardized"),
        "value_binding": infer_value_binding_support(lowered_form),
        "reference_binding": infer_reference_binding_support(lowered_form)
    }

    ctx.assumptions["state_model"] = infer_state_model(lowered_form)

function infer_value_binding_support(lowered_form):
    if any(op["kind"] in ["ui_value_in", "ui_value_out"] for unit in lowered_form["units"] for op in unit["operations"]):
        return "supported"
    return "not_required"

function infer_reference_binding_support(lowered_form):
    if any(op["kind"] == "ui_reference_handle" for unit in lowered_form["units"] for op in unit["operations"]):
        return "supported"
    return "not_required"

function infer_state_model(lowered_form):
    if any(obj["kind"] == "explicit_local_memory_state" for unit in lowered_form["units"] for obj in unit.get("support_objects", [])):
        return "explicit_local_memory_preserved"
    return "none"</code></pre>

<h3>11.7 Attach unsupported declarations</h3>

<pre><code class="language-text">function attach_unsupported_declarations(lowered_form, ctx):
    ctx.unsupported = []

    if ctx.assumptions["ui_binding"]["event_model"] != "not_standardized":
        ctx.unsupported.append({
            "kind": "standardized_ui_event_model",
            "reason": "The first reference backend family does not standardize a first-class UI event model."
        })</code></pre>

<h3>11.8 Final checks</h3>

<pre><code class="language-text">function run_final_contract_checks(lowered_form, ctx):
    ensure_contract_units_exist(ctx)
    ensure_public_boundaries_remain_explicit(ctx)
    ensure_ui_value_and_ui_reference_remain_distinct(ctx)
    ensure_ui_reference_and_ui_operation_remain_distinct(ctx)
    ensure_explicit_state_remains_explicit(ctx)
    ensure_assumptions_are_declared(ctx)
    ensure_no_runtime_private_fields_leaked(ctx)

function ensure_contract_units_exist(ctx):
    if len(ctx.units) == 0:
        fail(ctx,
             code = "missing_contract_units",
             message = "Contract emission produced no contract units.",
             anchor = {})

function ensure_public_boundaries_remain_explicit(ctx):
    for unit in ctx.units:
        public_boundary_kinds = [b["kind"] for b in unit["boundaries"] if b["kind"] in ["public_input", "public_output"]]
        if len(public_boundary_kinds) == 0 and not any(b["kind"].startswith("ui_") for b in unit["boundaries"]):
            ctx.warnings.append({
                "severity": "warning",
                "message": "Contract unit has no public or UI-visible boundaries.",
                "source_anchor": {"unit_id": unit["id"]}
            })

function ensure_ui_value_and_ui_reference_remain_distinct(ctx):
    for unit in ctx.units:
        has_ui_value = any(b["kind"] in ["ui_value_input", "ui_value_output"] for b in unit["boundaries"])
        has_ui_ref = any(b["kind"] == "ui_reference" for b in unit["boundaries"])

        if has_ui_value and has_ui_ref:
            return

function ensure_ui_reference_and_ui_operation_remain_distinct(ctx):
    for unit in ctx.units:
        ui_ref_ids = set(b["id"] for b in unit["boundaries"] if b["kind"] == "ui_reference")
        ui_op_ids = set(op["id"] for op in unit["operations"] if op["kind"] == "ui_operation")

        for rid in ui_ref_ids:
            if rid in ui_op_ids:
                fail(ctx,
                     code = "ui_reference_ui_operation_collapse_in_contract",
                     message = "UI reference participation was collapsed with a UI operation in the contract.",
                     anchor = {"id": rid})

function ensure_explicit_state_remains_explicit(ctx):
    for unit in ctx.units:
        state_entries = [s for s in unit["state"] if s["kind"] == "explicit_local_memory"]
        state_ops = [op for op in unit["operations"] if op["kind"] == "state_access"]

        if len(state_ops) &gt; 0 and len(state_entries) == 0:
            fail(ctx,
                 code = "state_access_without_explicit_state_declaration",
                 message = "State access exists in contract operations but no explicit local-memory state entry was emitted.",
                 anchor = {"unit_id": unit["id"]})

function ensure_assumptions_are_declared(ctx):
    required_keys = ["profiles_required", "scheduling", "ui_binding", "state_model"]
    for key in required_keys:
        if key not in ctx.assumptions:
            fail(ctx,
                 code = "missing_contract_assumption",
                 message = "Missing required contract assumption: " + key,
                 anchor = {"assumption": key})

function ensure_no_runtime_private_fields_leaked(ctx):
    forbidden_keys = ["scheduler_private", "thread_binding", "toolkit_handle_ptr", "abi_layout"]
    for unit in ctx.units:
        for boundary in unit["boundaries"]:
            for key in forbidden_keys:
                if key in boundary:
                    fail(ctx,
                         code = "runtime_private_leak_in_contract_boundary",
                         message = "Runtime-private field leaked into backend contract boundary.",
                         anchor = {"boundary_id": boundary["id"]})

        for op in unit["operations"]:
            for key in forbidden_keys:
                if key in op:
                    fail(ctx,
                         code = "runtime_private_leak_in_contract_operation",
                         message = "Runtime-private field leaked into backend contract operation.",
                         anchor = {"operation_id": op["id"]})</code></pre>

<h3>11.9 Utility helpers</h3>

<pre><code class="language-text">function emit_boundary(contract_unit, boundary):
    contract_unit["boundaries"].append(boundary)

function emit_state(contract_unit, state_entry):
    contract_unit["state"].append(state_entry)

function emit_contract_operation(contract_unit, op):
    contract_unit["operations"].append(op)

function build_contract_error(ctx, code, message):
    return {
        "artifact_kind": "frog_backend_contract",
        "artifact_version": "0.1",
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

function build_contract_error_from_ctx(ctx, code):
    return {
        "artifact_kind": "frog_backend_contract",
        "artifact_version": "0.1",
        "backend_family": ctx.backend_family,
        "source_ref": ctx.source_ref,
        "status": "error",
        "error_code": code,
        "diagnostics": ctx.errors + ctx.warnings
    }</code></pre>

<hr/>

<h2 id="mvp-preservation-rules">12. MVP Preservation Rules</h2>

<p>
The first contract emitter must enforce at least the following rules:
</p>

<ul>
  <li>one lowered entry unit becomes one contract entry unit,</li>
  <li>public boundaries remain explicit as public boundaries,</li>
  <li>UI value participation remains explicit as UI value participation,</li>
  <li>UI reference participation remains explicit as UI reference participation,</li>
  <li>UI-object primitives remain explicit as UI operations,</li>
  <li>explicit local memory remains explicit in the contract state section,</li>
  <li>family assumptions remain declared rather than hidden,</li>
  <li>the first slice must not silently introduce a standardized UI event model,</li>
  <li>the contract must remain directly consumable by the reference runtime family.</li>
</ul>

<hr/>

<h2 id="example-expectations">13. Example Expectations</h2>

<p>
For the initial example corpus, the first contract emitter should produce at minimum:
</p>

<ul>
  <li><strong>01_pure_addition</strong> — one contract unit with public input/output boundaries, one core compute operation, no state, UI disabled,</li>
  <li><strong>02_ui_value_roundtrip</strong> — one contract unit with UI value input/output boundaries, one core compute operation, no state, UI binding enabled,</li>
  <li><strong>03_ui_property_write</strong> — one contract unit with one public input boundary, one UI reference boundary, one UI operation of type property_write, no state, UI binding enabled,</li>
  <li><strong>04_stateful_feedback_delay</strong> — one contract unit with public boundaries, one explicit local-memory state entry, state access operations, one core compute operation, UI disabled.</li>
</ul>

<hr/>

<h2 id="summary">14. Summary</h2>

<p>
This document provides a directly codable MVP pseudo-code sketch for <code>emit_backend_contract()</code> in the FROG reference implementation.
It keeps backend contract emission explicit,
family-oriented,
and faithful to the published architecture:
lowered form in,
consumer-facing contract out,
with declared assumptions,
explicit boundaries,
explicit state,
explicit UI distinctions,
and no runtime-private semantic laundering.
</p>
