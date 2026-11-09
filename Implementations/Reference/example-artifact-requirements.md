<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Reference Example Artifact Requirements</h1>

<p align="center">
  <strong>Per-example artifact requirements for the bounded executable slices exercised by the FROG reference pipeline</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#status-and-boundary">2. Status and Boundary</a></li>
  <li><a href="#global-stage-rules">3. Global Stage Rules</a></li>
  <li><a href="#subset-honesty-rule">4. Subset Honesty Rule</a></li>
  <li><a href="#example-01-pure-addition">5. Example 01 — Pure Addition</a></li>
  <li><a href="#example-02-ui-value-roundtrip">6. Example 02 — UI Value Roundtrip</a></li>
  <li><a href="#example-03-ui-property-write">7. Example 03 — UI Property Write</a></li>
  <li><a href="#example-04-stateful-feedback-with-explicit-delay">8. Example 04 — Stateful Feedback with Explicit Delay</a></li>
  <li><a href="#example-05-bounded-ui-accumulator">9. Example 05 — Bounded UI Accumulator</a></li>
  <li><a href="#cross-example-rejection-rules">10. Cross-Example Rejection Rules</a></li>
  <li><a href="#implementation-note">11. Implementation Note</a></li>
  <li><a href="#summary">12. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the minimum required artifacts that the reference implementation should produce for each bounded published example at each pipeline stage:
</p>

<ul>
  <li><code>validate</code>,</li>
  <li><code>derive-ir</code>,</li>
  <li><code>lower</code>,</li>
  <li><code>emit-contract</code>,</li>
  <li><code>run</code>.</li>
</ul>

<p>
The goal is not to freeze one universal transport syntax.
The goal is to make the first executable corridors directly codable, testable, inspectable, and comparable against the published repository surfaces.
</p>

<p>
This document therefore serves one practical function:
</p>

<ul>
  <li>to state what the reference implementation must minimally materialize,</li>
  <li>for which published slices,</li>
  <li>without pretending that the full open-ended FROG model is already implemented.</li>
</ul>

<hr/>

<h2 id="status-and-boundary">2. Status and Boundary</h2>

<p>
All requirements in this document are non-normative reference-implementation requirements.
They do not replace:
</p>

<ul>
  <li>canonical source ownership,</li>
  <li>structural-validation ownership,</li>
  <li>validated semantic ownership,</li>
  <li>open Execution IR ownership,</li>
  <li>lowering ownership,</li>
  <li>backend contract ownership.</li>
</ul>

<p>
They only define what the reference implementation should minimally materialize in order to prove that the published architecture is executable for the supported subset.
</p>

<hr/>

<h2 id="global-stage-rules">3. Global Stage Rules</h2>

<p>
For every valid supported example, the following stage rules apply:
</p>

<ul>
  <li><code>validate</code> MUST produce <code>status</code>, <code>source_ref</code>, <code>validated_subset</code>, and at least one stable <code>program_id</code>.</li>
  <li><code>derive-ir</code> MUST produce at least one attributable execution unit or equivalent attributable execution-facing root object.</li>
  <li><code>lower</code> MUST produce one <code>backend_family</code>, one lowered unit set, and explicit <code>assumptions</code>.</li>
  <li><code>emit-contract</code> MUST produce one backend contract with explicit <code>units</code>, <code>assumptions</code>, and <code>unsupported</code>.</li>
  <li><code>run</code> MUST produce <code>status</code>, <code>contract_ref</code>, and either an execution summary or an explicit rejection reason.</li>
</ul>

<p>
For every invalid conformance case:
</p>

<ul>
  <li><code>validate</code> MUST fail explicitly,</li>
  <li><code>derive-ir</code>, <code>lower</code>, <code>emit-contract</code>, and <code>run</code> MUST NOT claim success.</li>
</ul>

<p>
For every unsupported-but-valid case:
</p>

<ul>
  <li><code>validate</code> SHOULD distinguish that the source is valid in the broader published model,</li>
  <li>the implementation MAY stop before later stages,</li>
  <li>and it MUST NOT silently reinterpret the unsupported construct as if it belonged to the supported subset.</li>
</ul>

<hr/>

<h2 id="subset-honesty-rule">4. Subset Honesty Rule</h2>

<p>
The reference implementation MUST remain explicit about the distinction between:
</p>

<ul>
  <li>the general extensible FROG model published by the repository,</li>
  <li>the specific published subset currently exercised by the reference pipeline,</li>
  <li>implementation-private convenience shapes used internally.</li>
</ul>

<p>
Accordingly:
</p>

<ul>
  <li>artifact production MUST identify the supported corridor being exercised,</li>
  <li>unsupported-but-valid situations SHOULD be reported explicitly,</li>
  <li>the first complete success target remains one bounded published slice, not maximal feature breadth.</li>
</ul>

<hr/>

<h2 id="example-01-pure-addition">5. Example 01 — Pure Addition</h2>

<p>
Target source:
</p>

<pre><code>Examples/01_pure_addition/main.frog</code></pre>

<h3>5.1 validate — minimum required fields</h3>

<pre><code>{
  "artifact_kind": "frog_validation_result",
  "status": "ok",
  "source_ref": {
    "path": "Examples/01_pure_addition/main.frog"
  },
  "validated_subset": {
    "core_primitives": true,
    "public_interface": true,
    "widget_value": false,
    "widget_reference": false,
    "ui_object_primitives": false,
    "explicit_local_memory": false,
    "bounded_loop": false
  },
  "validated_program": {
    "program_id": "prog:01_pure_addition",
    "entry_kind": "single_frog_program"
  }
}</code></pre>

<p>
Minimum semantic facts that must be known after validation:
</p>

<ul>
  <li>two public inputs exist,</li>
  <li>one public output exists,</li>
  <li>one valid <code>frog.core.add</code> primitive exists,</li>
  <li>the graph is acyclic.</li>
</ul>

<h3>5.2 derive-ir — minimum required fields</h3>

<pre><code>{
  "artifact_kind": "frog_derived_execution_ir",
  "execution_unit": {
    "id": "unit:main",
    "objects": [
      { "kind": "public_input_boundary" },
      { "kind": "public_input_boundary" },
      { "kind": "primitive", "primitive_ref": "frog.core.add" },
      { "kind": "public_output_boundary" }
    ],
    "connections": [
      {},
      {},
      {}
    ]
  }
}</code></pre>

<p>
Minimum distinctions that must remain explicit:
</p>

<ul>
  <li>input boundary versus output boundary,</li>
  <li>primitive identity for <code>frog.core.add</code>,</li>
  <li>connection attribution.</li>
</ul>

<h3>5.3 lower — minimum required fields</h3>

<pre><code>{
  "artifact_kind": "frog_lowered_form",
  "backend_family": "reference_host_runtime_ui_binding",
  "assumptions": {
    "deterministic_step_execution": true,
    "ui_binding_enabled": false
  },
  "units": [
    {
      "id": "lowered:main",
      "role": "entry_unit",
      "operations": [
        { "kind": "public_input" },
        { "kind": "public_input" },
        { "kind": "core_primitive_add" },
        { "kind": "public_output" }
      ]
    }
  ]
}</code></pre>

<h3>5.4 emit-contract — minimum required fields</h3>

<pre><code>{
  "artifact_kind": "frog_backend_contract",
  "backend_family": "reference_host_runtime_ui_binding",
  "assumptions": {
    "state_model": "none",
    "ui_binding": { "enabled": false }
  },
  "units": [
    {
      "id": "main",
      "role": "entry_unit",
      "boundaries": [
        { "kind": "public_input", "value_type": "f64" },
        { "kind": "public_input", "value_type": "f64" },
        { "kind": "public_output", "value_type": "f64" }
      ]
    }
  ],
  "unsupported": []
}</code></pre>

<h3>5.5 run — minimum required fields</h3>

<pre><code>{
  "artifact_kind": "frog_runtime_result",
  "status": "ok",
  "contract_ref": {
    "unit_ids": ["main"]
  },
  "execution_summary": {
    "mode": "deterministic_step_execution"
  },
  "outputs": {
    "public": {
      "result": "&lt;value&gt;"
    }
  }
}</code></pre>

<hr/>

<h2 id="example-02-ui-value-roundtrip">6. Example 02 — UI Value Roundtrip</h2>

<p>
Target source:
</p>

<pre><code>Examples/02_ui_value_roundtrip/main.frog</code></pre>

<h3>6.1 validate — minimum required fields</h3>

<pre><code>{
  "artifact_kind": "frog_validation_result",
  "status": "ok",
  "source_ref": {
    "path": "Examples/02_ui_value_roundtrip/main.frog"
  },
  "validated_subset": {
    "core_primitives": false,
    "public_interface": false,
    "widget_value": true,
    "widget_reference": false,
    "ui_object_primitives": false,
    "explicit_local_memory": false,
    "bounded_loop": false
  },
  "validated_program": {
    "program_id": "prog:02_ui_value_roundtrip",
    "entry_kind": "single_frog_program"
  }
}</code></pre>

<p>
Minimum semantic facts that must be known after validation:
</p>

<ul>
  <li>one value-carrying control widget exists,</li>
  <li>one value-carrying indicator widget exists,</li>
  <li>one valid <code>widget_value</code> read path exists,</li>
  <li>one valid <code>widget_value</code> write path exists.</li>
</ul>

<h3>6.2 derive-ir — minimum required fields</h3>

<pre><code>{
  "artifact_kind": "frog_derived_execution_ir",
  "execution_unit": {
    "id": "unit:main",
    "objects": [
      { "kind": "widget_value_boundary", "widget_id": "ctrl_input", "direction": "read" },
      { "kind": "widget_value_boundary", "widget_id": "ind_output", "direction": "write" }
    ],
    "connections": [
      {}
    ]
  }
}</code></pre>

<h3>6.3 lower — minimum required fields</h3>

<pre><code>{
  "artifact_kind": "frog_lowered_form",
  "backend_family": "reference_host_runtime_ui_binding",
  "assumptions": {
    "ui_binding_enabled": true,
    "ui_object_surface_enabled": false
  },
  "units": [
    {
      "id": "lowered:main",
      "operations": [
        { "kind": "widget_value_read", "widget_id": "ctrl_input" },
        { "kind": "widget_value_write", "widget_id": "ind_output" }
      ]
    }
  ]
}</code></pre>

<h3>6.4 emit-contract — minimum required fields</h3>

<pre><code>{
  "artifact_kind": "frog_backend_contract",
  "backend_family": "reference_host_runtime_ui_binding",
  "assumptions": {
    "ui_binding": {
      "enabled": true,
      "widget_value_path": true,
      "widget_reference_path": false
    }
  },
  "units": [
    {
      "id": "main",
      "ui_bindings": [
        { "kind": "widget_value_read", "widget_id": "ctrl_input" },
        { "kind": "widget_value_write", "widget_id": "ind_output" }
      ]
    }
  ],
  "unsupported": []
}</code></pre>

<h3>6.5 run — minimum required fields</h3>

<pre><code>{
  "artifact_kind": "frog_runtime_result",
  "status": "ok",
  "contract_ref": {
    "unit_ids": ["main"]
  },
  "execution_summary": {
    "mode": "ui_value_roundtrip"
  },
  "outputs": {
    "ui": {
      "ind_output.value": "&lt;value&gt;"
    }
  }
}</code></pre>

<hr/>

<h2 id="example-03-ui-property-write">7. Example 03 — UI Property Write</h2>

<p>
Target source:
</p>

<pre><code>Examples/03_ui_property_write/main.frog</code></pre>

<h3>7.1 validate — minimum required fields</h3>

<pre><code>{
  "artifact_kind": "frog_validation_result",
  "status": "ok",
  "source_ref": {
    "path": "Examples/03_ui_property_write/main.frog"
  },
  "validated_subset": {
    "core_primitives": false,
    "public_interface": false,
    "widget_value": false,
    "widget_reference": true,
    "ui_object_primitives": true,
    "explicit_local_memory": false,
    "bounded_loop": false
  },
  "validated_program": {
    "program_id": "prog:03_ui_property_write",
    "entry_kind": "single_frog_program"
  }
}</code></pre>

<p>
Minimum semantic facts that must be known after validation:
</p>

<ul>
  <li>a widget reference anchor exists,</li>
  <li>a writable property target exists on the targeted widget or part,</li>
  <li>a legal <code>frog.ui.property_write</code> path exists,</li>
  <li>the value supplied to the write is type-compatible with the addressed property.</li>
</ul>

<h3>7.2 derive-ir — minimum required fields</h3>

<pre><code>{
  "artifact_kind": "frog_derived_execution_ir",
  "execution_unit": {
    "id": "unit:main",
    "objects": [
      { "kind": "widget_reference_boundary", "widget_id": "ctrl_input" },
      {
        "kind": "ui_property_write",
        "primitive_ref": "frog.ui.property_write",
        "member_ref": {
          "member": "face_color"
        }
      }
    ],
    "connections": [
      {}
    ]
  }
}</code></pre>

<h3>7.3 lower — minimum required fields</h3>

<pre><code>{
  "artifact_kind": "frog_lowered_form",
  "backend_family": "reference_host_runtime_ui_binding",
  "assumptions": {
    "ui_binding_enabled": true,
    "ui_object_surface_enabled": true
  },
  "units": [
    {
      "id": "lowered:main",
      "operations": [
        { "kind": "widget_reference_acquire", "widget_id": "ctrl_input" },
        {
          "kind": "ui_property_write",
          "widget_id": "ctrl_input",
          "member": "face_color"
        }
      ]
    }
  ]
}</code></pre>

<h3>7.4 emit-contract — minimum required fields</h3>

<pre><code>{
  "artifact_kind": "frog_backend_contract",
  "backend_family": "reference_host_runtime_ui_binding",
  "assumptions": {
    "ui_binding": {
      "enabled": true,
      "widget_reference_path": true
    }
  },
  "units": [
    {
      "id": "main",
      "ui_bindings": [
        {
          "kind": "property_write",
          "widget_id": "ctrl_input",
          "member": "face_color"
        }
      ]
    }
  ],
  "unsupported": []
}</code></pre>

<h3>7.5 run — minimum required fields</h3>

<pre><code>{
  "artifact_kind": "frog_runtime_result",
  "status": "ok",
  "contract_ref": {
    "unit_ids": ["main"]
  },
  "execution_summary": {
    "mode": "ui_property_write"
  },
  "outputs": {
    "ui_effects": [
      {
        "widget_id": "ctrl_input",
        "member": "face_color",
        "effect_kind": "property_write_applied"
      }
    ]
  }
}</code></pre>

<hr/>

<h2 id="example-04-stateful-feedback-with-explicit-delay">8. Example 04 — Stateful Feedback with Explicit Delay</h2>

<p>
Target source:
</p>

<pre><code>Examples/04_stateful_feedback_with_explicit_delay/main.frog</code></pre>

<h3>8.1 validate — minimum required fields</h3>

<pre><code>{
  "artifact_kind": "frog_validation_result",
  "status": "ok",
  "source_ref": {
    "path": "Examples/04_stateful_feedback_with_explicit_delay/main.frog"
  },
  "validated_subset": {
    "core_primitives": true,
    "public_interface": true,
    "widget_value": false,
    "widget_reference": false,
    "ui_object_primitives": false,
    "explicit_local_memory": true,
    "bounded_loop": false
  },
  "validated_program": {
    "program_id": "prog:04_stateful_feedback_with_explicit_delay",
    "entry_kind": "single_frog_program"
  }
}</code></pre>

<p>
Minimum semantic facts that must be known after validation:
</p>

<ul>
  <li>a legal explicit memory path exists,</li>
  <li><code>frog.core.delay</code> has an explicit initial input,</li>
  <li>the cycle is valid only because explicit local memory exists,</li>
  <li>the state path is attributable.</li>
</ul>

<h3>8.2 derive-ir — minimum required fields</h3>

<pre><code>{
  "artifact_kind": "frog_derived_execution_ir",
  "execution_unit": {
    "id": "unit:main",
    "objects": [
      { "kind": "primitive", "primitive_ref": "frog.core.delay" },
      { "kind": "explicit_state_cell" }
    ],
    "connections": [
      {}
    ]
  }
}</code></pre>

<h3>8.3 lower — minimum required fields</h3>

<pre><code>{
  "artifact_kind": "frog_lowered_form",
  "backend_family": "reference_host_runtime_ui_binding",
  "assumptions": {
    "state_model": "explicit_local_memory",
    "ui_binding_enabled": false
  },
  "units": [
    {
      "id": "lowered:main",
      "operations": [
        { "kind": "state_init" },
        { "kind": "state_read" },
        { "kind": "core_primitive_delay" },
        { "kind": "state_write" }
      ]
    }
  ]
}</code></pre>

<h3>8.4 emit-contract — minimum required fields</h3>

<pre><code>{
  "artifact_kind": "frog_backend_contract",
  "backend_family": "reference_host_runtime_ui_binding",
  "assumptions": {
    "state_model": "explicit_local_memory",
    "ui_binding": { "enabled": false }
  },
  "units": [
    {
      "id": "main",
      "state_cells": [
        {
          "id": "state:main:0",
          "kind": "explicit_local_memory"
        }
      ]
    }
  ],
  "unsupported": []
}</code></pre>

<h3>8.5 run — minimum required fields</h3>

<pre><code>{
  "artifact_kind": "frog_runtime_result",
  "status": "ok",
  "contract_ref": {
    "unit_ids": ["main"]
  },
  "execution_summary": {
    "mode": "explicit_state_feedback"
  },
  "outputs": {
    "public": {
      "result": "&lt;value&gt;"
    },
    "state": {
      "state:main:0": "&lt;value&gt;"
    }
  }
}</code></pre>

<hr/>

<h2 id="example-05-bounded-ui-accumulator">9. Example 05 — Bounded UI Accumulator</h2>

<p>
Target source:
</p>

<pre><code>Examples/05_bounded_ui_accumulator/main.frog</code></pre>

<p>
This is the first priority vertical slice for the current repository campaign.
It is intentionally the first example that must prove a complete bounded corridor touching:
</p>

<ul>
  <li>front-panel control and indicator,</li>
  <li>natural widget value participation,</li>
  <li>minimal object-style presentation-property interaction,</li>
  <li>bounded loop execution,</li>
  <li>explicit state through <code>frog.core.delay</code>,</li>
  <li>public output observation,</li>
  <li>runtime-visible UI result surface.</li>
</ul>

<h3>9.1 validate — minimum required fields</h3>

<pre><code>{
  "artifact_kind": "frog_validation_result",
  "status": "ok",
  "source_ref": {
    "path": "Examples/05_bounded_ui_accumulator/main.frog"
  },
  "validated_subset": {
    "core_primitives": true,
    "public_interface": true,
    "widget_value": true,
    "widget_reference": true,
    "ui_object_primitives": true,
    "explicit_local_memory": true,
    "bounded_loop": true,
    "minimal_u16_widget_family": true,
    "presentation_metadata_persisted_in_source": true
  },
  "validated_program": {
    "program_id": "prog:05_bounded_ui_accumulator",
    "entry_kind": "single_frog_program"
  }
}</code></pre>

<p>
Minimum semantic facts that must be known after validation:
</p>

<ul>
  <li>one numeric control widget exists and is value-carrying,</li>
  <li>one numeric indicator widget exists and is value-carrying,</li>
  <li>the control and indicator belong to the supported minimal widget family,</li>
  <li>the natural <code>widget_value</code> path is used for ordinary accumulation value flow,</li>
  <li>the object-style <code>widget_reference</code> path is used for minimal property writes such as <code>face_color</code>,</li>
  <li>the bounded loop executes exactly five iterations in the supported source form,</li>
  <li><code>frog.core.delay</code> carries explicit state with an explicit initial input,</li>
  <li>the final accumulated value is routed both to a public output and to the numeric indicator,</li>
  <li>source-persisted presentation metadata such as <code>face_template</code> is recognized as non-semantic.</li>
</ul>

<h3>9.2 derive-ir — minimum required fields</h3>

<pre><code>{
  "artifact_kind": "frog_derived_execution_ir",
  "execution_unit": {
    "id": "unit:main",
    "objects": [
      {
        "kind": "widget_value_boundary",
        "widget_id": "ctrl_input",
        "direction": "read",
        "value_type": "u16"
      },
      {
        "kind": "widget_reference_boundary",
        "widget_id": "ctrl_input"
      },
      {
        "kind": "widget_reference_boundary",
        "widget_id": "ind_result"
      },
      {
        "kind": "ui_property_write",
        "primitive_ref": "frog.ui.property_write",
        "member_ref": { "member": "face_color" }
      },
      {
        "kind": "bounded_loop_region",
        "iteration_count": 5
      },
      {
        "kind": "primitive",
        "primitive_ref": "frog.core.delay"
      },
      {
        "kind": "primitive",
        "primitive_ref": "frog.core.add"
      },
      {
        "kind": "widget_value_boundary",
        "widget_id": "ind_result",
        "direction": "write",
        "value_type": "u16"
      },
      {
        "kind": "public_output_boundary",
        "interface_port": "result",
        "value_type": "u16"
      }
    ],
    "connections": [
      {}
    ]
  }
}</code></pre>

<p>
Minimum distinctions that must remain explicit:
</p>

<ul>
  <li>natural widget value boundaries versus widget reference boundaries,</li>
  <li>semantic accumulation state versus presentation-only property writes,</li>
  <li>bounded loop structure versus straight-line operations,</li>
  <li>public output boundary versus UI output boundary.</li>
</ul>

<h3>9.3 lower — minimum required fields</h3>

<pre><code>{
  "artifact_kind": "frog_lowered_form",
  "backend_family": "reference_host_runtime_ui_binding",
  "assumptions": {
    "state_model": "explicit_local_memory",
    "ui_binding_enabled": true,
    "ui_object_surface_enabled": true,
    "bounded_loop_lowering_mode": "counted_loop",
    "presentation_template_execution_semantics": "none"
  },
  "units": [
    {
      "id": "lowered:main",
      "role": "entry_unit",
      "operations": [
        { "kind": "widget_value_read", "widget_id": "ctrl_input", "value_type": "u16" },
        { "kind": "widget_reference_acquire", "widget_id": "ctrl_input" },
        { "kind": "ui_property_write", "widget_id": "ctrl_input", "member": "face_color" },
        { "kind": "widget_reference_acquire", "widget_id": "ind_result" },
        { "kind": "ui_property_write", "widget_id": "ind_result", "member": "face_color" },
        { "kind": "state_init", "value_type": "u16" },
        { "kind": "counted_loop_begin", "iteration_count": 5 },
        { "kind": "state_read", "value_type": "u16" },
        { "kind": "core_primitive_add", "value_type": "u16" },
        { "kind": "state_write", "value_type": "u16" },
        { "kind": "counted_loop_end" },
        { "kind": "widget_value_write", "widget_id": "ind_result", "value_type": "u16" },
        { "kind": "public_output", "interface_port": "result", "value_type": "u16" }
      ]
    }
  ]
}</code></pre>

<h3>9.4 emit-contract — minimum required fields</h3>

<pre><code>{
  "artifact_kind": "frog_backend_contract",
  "backend_family": "reference_host_runtime_ui_binding",
  "assumptions": {
    "state_model": "explicit_local_memory",
    "loop_model": "counted_loop",
    "ui_binding": {
      "enabled": true,
      "widget_value_path": true,
      "widget_reference_path": true,
      "presentation_template_runtime_support": "optional"
    }
  },
  "units": [
    {
      "id": "main",
      "role": "entry_unit",
      "ui_bindings": [
        {
          "kind": "widget_value_read",
          "widget_id": "ctrl_input",
          "value_type": "u16"
        },
        {
          "kind": "property_write",
          "widget_id": "ctrl_input",
          "member": "face_color"
        },
        {
          "kind": "property_write",
          "widget_id": "ind_result",
          "member": "face_color"
        },
        {
          "kind": "widget_value_write",
          "widget_id": "ind_result",
          "value_type": "u16"
        }
      ],
      "state_cells": [
        {
          "id": "state:main:0",
          "kind": "explicit_local_memory",
          "value_type": "u16"
        }
      ],
      "boundaries": [
        {
          "kind": "public_output",
          "interface_port": "result",
          "value_type": "u16"
        }
      ]
    }
  ],
  "unsupported": [
    {
      "surface": "face_template",
      "reason": "presentation-only metadata may be preserved without being executed as semantic behavior"
    }
  ]
}</code></pre>

<h3>9.5 run — minimum required fields</h3>

<pre><code>{
  "artifact_kind": "frog_runtime_result",
  "status": "ok",
  "contract_ref": {
    "unit_ids": ["main"]
  },
  "execution_summary": {
    "mode": "bounded_ui_accumulator",
    "iteration_count": 5,
    "state_model": "explicit_local_memory"
  },
  "outputs": {
    "public": {
      "result": "&lt;value&gt;"
    },
    "ui": {
      "ind_result.value": "&lt;value&gt;"
    },
    "ui_effects": [
      {
        "widget_id": "ctrl_input",
        "member": "face_color",
        "effect_kind": "property_write_applied"
      },
      {
        "widget_id": "ind_result",
        "member": "face_color",
        "effect_kind": "property_write_applied"
      }
    ],
    "state": {
      "state:main:0": "&lt;value&gt;"
    }
  }
}</code></pre>

<h3>9.6 Required Observable Facts for Example 05</h3>

<p>
A successful run of the supported Example 05 corridor MUST make it possible to observe at least:
</p>

<ul>
  <li>that the final public output exists,</li>
  <li>that the indicator receives the same final accumulated value,</li>
  <li>that the loop iteration count is five,</li>
  <li>that explicit state exists and is attributable,</li>
  <li>that presentation-property writes are reported separately from semantic accumulation state.</li>
</ul>

<h3>9.7 What Example 05 Is Not Required to Prove</h3>

<p>
Example 05 is not required to prove:
</p>

<ul>
  <li>the full future open-ended widget ecosystem,</li>
  <li>a complete event model,</li>
  <li>runtime execution semantics for <code>face_template</code>,</li>
  <li>pixel-perfect rendering equivalence across hosts,</li>
  <li>all future backend families.</li>
</ul>

<p>
Its purpose is narrower:
to prove one real complete corridor on a published bounded subset.
</p>

<hr/>

<h2 id="cross-example-rejection-rules">10. Cross-Example Rejection Rules</h2>

<p>
Across all reference examples, the following rejection rules apply:
</p>

<ul>
  <li>a non-loadable source MUST stop at load,</li>
  <li>a structurally invalid source MUST stop at validate,</li>
  <li>a semantically rejected source MUST stop at validate,</li>
  <li>an unsupported-but-valid source MAY stop at validate with an explicit unsupported status,</li>
  <li>derive-ir MUST NOT claim success for a failed or rejected prior stage,</li>
  <li>lower MUST NOT claim success if no valid derived execution-facing artifact exists,</li>
  <li>emit-contract MUST NOT claim success if no valid lowered form exists,</li>
  <li>run MUST NOT claim success if no valid backend contract exists.</li>
</ul>

<p>
Across all UI-bearing examples, the implementation MUST also reject any silent collapse of:
</p>

<ul>
  <li>natural <code>widget_value</code> access into implicit object-style access,</li>
  <li>presentation-only metadata into executable semantic state,</li>
  <li>widget reference transport into ordinary typed value transport.</li>
</ul>

<hr/>

<h2 id="implementation-note">11. Implementation Note</h2>

<p>
The concrete JSON layout used by the reference implementation MAY evolve while keeping the same minimum observable facts.
</p>

<p>
What must remain stable is not every exact field spelling forever, but the recoverable materialization of:
</p>

<ul>
  <li>source attribution,</li>
  <li>validated subset status,</li>
  <li>execution-facing derivation,</li>
  <li>backend-family lowering posture,</li>
  <li>contract emission,</li>
  <li>runtime-visible result reporting.</li>
</ul>

<p>
Until the syntax is further stabilized, this document should be read as:
</p>

<ul>
  <li>a minimum artifact-obligation document,</li>
  <li>not as the final canonical syntax law for every internal artifact.</li>
</ul>

<hr/>

<h2 id="summary">12. Summary</h2>

<p>
This document defines the minimum artifacts that the reference pipeline should materialize for each bounded published example.
</p>

<p>
Its current priority is no longer only to cover the early MVP slices independently.
Its main success target is now the first complete UI-bearing bounded vertical slice:
</p>

<ul>
  <li><code>Examples/05_bounded_ui_accumulator/main.frog</code>,</li>
  <li>through validation,</li>
  <li>through open Execution IR derivation,</li>
  <li>through lowering,</li>
  <li>through backend contract emission,</li>
  <li>through runtime-visible outputs and UI effects.</li>
</ul>

<p>
This artifact-requirements document therefore exists to keep the reference implementation explicit, stage-separated, and honest about what it really proves today.
</p>
