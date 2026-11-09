<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Reference Internal Artifacts</h1>

<p align="center">
  <strong>Internal artifact taxonomy used by the FROG reference implementation across load, validation, derivation, lowering, contract emission, and runtime execution</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#status-and-boundary">2. Status and Boundary</a></li>
  <li><a href="#design-goal">3. Design Goal</a></li>
  <li><a href="#artifact-family-map">4. Artifact Family Map</a></li>
  <li><a href="#artifact-kinds">5. Artifact Kinds</a></li>
  <li><a href="#loader-artifacts">6. Loader Artifacts</a></li>
  <li><a href="#validation-artifacts">7. Validation Artifacts</a></li>
  <li><a href="#execution-ir-derivation-artifacts">8. Execution IR Derivation Artifacts</a></li>
  <li><a href="#lowering-artifacts">9. Lowering Artifacts</a></li>
  <li><a href="#backend-contract-artifacts">10. Backend Contract Artifacts</a></li>
  <li><a href="#runtime-artifacts">11. Runtime Artifacts</a></li>
  <li><a href="#ui-host-artifacts">12. UI Host Artifacts</a></li>
  <li><a href="#subset-honesty-and-unsupported-status">13. Subset Honesty and Unsupported Status</a></li>
  <li><a href="#example-05-minimum-artifact-chain">14. Example 05 Minimum Artifact Chain</a></li>
  <li><a href="#stability-rule">15. Stability Rule</a></li>
  <li><a href="#summary">16. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the internal artifact taxonomy used by the reference implementation workspace.
</p>

<p>
Its purpose is to keep implementation stages explicit and to ensure that the following documents all talk about the same internal objects:
</p>

<ul>
  <li><code>Readme.md</code>,</li>
  <li><code>pipeline.md</code>,</li>
  <li><code>frogc.md</code>,</li>
  <li><code>example-artifact-requirements.md</code>.</li>
</ul>

<p>
This document does not standardize repository-wide normative syntax for every possible implementation artifact.
It defines a coherent internal naming and boundary model for the reference implementation workspace.
</p>

<hr/>

<h2 id="status-and-boundary">2. Status and Boundary</h2>

<p>
All artifact kinds defined here are non-normative implementation artifacts.
They do not replace:
</p>

<ul>
  <li>canonical source,</li>
  <li>structural validity,</li>
  <li>validated semantic meaning,</li>
  <li>canonical Execution IR,</li>
  <li>lowering law,</li>
  <li>backend contract law.</li>
</ul>

<p>
Instead, they are the reference implementation's working surfaces for consuming the published repository architecture in a recoverable way.
</p>

<p>
The key rule is:
</p>

<pre><code>internal artifact
    !=
normative language layer
</code></pre>

<hr/>

<h2 id="design-goal">3. Design Goal</h2>

<p>
The internal artifact model should satisfy the following goals:
</p>

<ul>
  <li><strong>stage clarity</strong> — each artifact should correspond to a clear pipeline stage,</li>
  <li><strong>traceability</strong> — source attribution should remain recoverable,</li>
  <li><strong>subset honesty</strong> — unsupported-but-valid situations should remain explicit,</li>
  <li><strong>cross-document consistency</strong> — the same artifact names should appear coherently in CLI, pipeline, and example requirements,</li>
  <li><strong>bounded usefulness</strong> — the first priority slice should be fully representable through these artifacts.</li>
</ul>

<hr/>

<h2 id="artifact-family-map">4. Artifact Family Map</h2>

<p>
The intended internal artifact family map is:
</p>

<pre><code>source file
    |
    v
frog_loaded_source
    |
    v
frog_validation_result
    |
    v
frog_derived_execution_ir
    |
    v
frog_lowered_form
    |
    v
frog_backend_contract
    |
    v
frog_runtime_result
</code></pre>

<p>
Optional implementation-side helper artifacts MAY appear between those main stages, but they SHOULD map clearly to one of those families rather than inventing a competing pipeline vocabulary.
</p>

<hr/>

<h2 id="artifact-kinds">5. Artifact Kinds</h2>

<p>
The primary artifact kinds used by the reference implementation are:
</p>

<ul>
  <li><code>frog_loaded_source</code>,</li>
  <li><code>frog_validation_result</code>,</li>
  <li><code>frog_derived_execution_ir</code>,</li>
  <li><code>frog_lowered_form</code>,</li>
  <li><code>frog_backend_contract</code>,</li>
  <li><code>frog_runtime_result</code>.</li>
</ul>

<p>
Each artifact SHOULD carry:
</p>

<ul>
  <li><code>artifact_kind</code>,</li>
  <li><code>artifact_version</code>,</li>
  <li><code>source_ref</code> when meaningful,</li>
  <li><code>program_id</code> when meaningful,</li>
  <li><code>stage_ref</code> or equivalent stage identity when useful.</li>
</ul>

<p>
A useful common envelope pattern is:
</p>

<pre><code>{
  "artifact_kind": "&lt;kind&gt;",
  "artifact_version": "0.1",
  "source_ref": {
    "path": "&lt;path&gt;"
  },
  "program_id": "&lt;program_id&gt;",
  "stage_ref": "&lt;stage_name&gt;"
}</code></pre>

<hr/>

<h2 id="loader-artifacts">6. Loader Artifacts</h2>

<p>
The loader stage SHOULD produce a <code>frog_loaded_source</code> artifact.
</p>

<p>
Its job is to record:
</p>

<ul>
  <li>the source path,</li>
  <li>the decoded top-level object,</li>
  <li>the load status,</li>
  <li>and any immediate decoding diagnostics.</li>
</ul>

<p>
Conceptual shape:
</p>

<pre><code>{
  "artifact_kind": "frog_loaded_source",
  "artifact_version": "0.1",
  "source_ref": {
    "path": "Examples/05_bounded_ui_accumulator/main.frog"
  },
  "status": "ok",
  "decoded_source": {
    "&lt;top_level_sections&gt;": {}
  },
  "diagnostics": []
}</code></pre>

<p>
A failed load SHOULD still return a recoverable artifact surface when possible, but with:
</p>

<ul>
  <li><code>status</code> set to a load failure state,</li>
  <li>and no claim that validation succeeded.</li>
</ul>

<hr/>

<h2 id="validation-artifacts">7. Validation Artifacts</h2>

<p>
The validation stage SHOULD produce a <code>frog_validation_result</code> artifact.
</p>

<p>
Its job is to distinguish at least:
</p>

<ul>
  <li>load failure,</li>
  <li>structural invalidity,</li>
  <li>semantic rejection,</li>
  <li>unsupported-but-valid status,</li>
  <li>successful validation.</li>
</ul>

<p>
Conceptual shape:
</p>

<pre><code>{
  "artifact_kind": "frog_validation_result",
  "artifact_version": "0.1",
  "source_ref": {
    "path": "Examples/05_bounded_ui_accumulator/main.frog"
  },
  "status": "ok",
  "validated_subset": {
    "widget_value": true,
    "widget_reference": true,
    "ui_object_primitives": true,
    "explicit_local_memory": true,
    "bounded_loop": true
  },
  "validated_program": {
    "program_id": "prog:05_bounded_ui_accumulator",
    "entry_kind": "single_frog_program"
  },
  "diagnostics": []
}</code></pre>

<p>
This artifact is the first one that must explicitly record subset posture.
It is the correct place to say:
</p>

<ul>
  <li>supported and accepted,</li>
  <li>rejected,</li>
  <li>or valid in the broader published model but unsupported by the current reference subset.</li>
</ul>

<hr/>

<h2 id="execution-ir-derivation-artifacts">8. Execution IR Derivation Artifacts</h2>

<p>
The derivation stage SHOULD produce a <code>frog_derived_execution_ir</code> artifact.
</p>

<p>
Its job is to expose an attributable execution-facing representation derived from validated meaning.
</p>

<p>
Conceptual shape:
</p>

<pre><code>{
  "artifact_kind": "frog_derived_execution_ir",
  "artifact_version": "0.1",
  "source_ref": {
    "path": "Examples/05_bounded_ui_accumulator/main.frog"
  },
  "program_id": "prog:05_bounded_ui_accumulator",
  "execution_unit": {
    "id": "unit:main",
    "objects": [],
    "connections": []
  },
  "diagnostics": []
}</code></pre>

<p>
This artifact SHOULD preserve at least:
</p>

<ul>
  <li>execution-unit identity,</li>
  <li>boundary objects,</li>
  <li>primitive identity,</li>
  <li>explicit state identity when present,</li>
  <li>distinction between semantic and presentation-only surfaces.</li>
</ul>

<hr/>

<h2 id="lowering-artifacts">9. Lowering Artifacts</h2>

<p>
The lowering stage SHOULD produce a <code>frog_lowered_form</code> artifact.
</p>

<p>
Its job is to specialize the open execution-facing representation for one selected backend family.
</p>

<p>
Conceptual shape:
</p>

<pre><code>{
  "artifact_kind": "frog_lowered_form",
  "artifact_version": "0.1",
  "program_id": "prog:05_bounded_ui_accumulator",
  "backend_family": "reference_host_runtime_ui_binding",
  "assumptions": {},
  "units": [],
  "diagnostics": []
}</code></pre>

<p>
This artifact SHOULD make explicit:
</p>

<ul>
  <li>selected backend family,</li>
  <li>lowered units,</li>
  <li>specialization assumptions,</li>
  <li>any narrowed runtime expectations.</li>
</ul>

<p>
The lowered form remains downstream from canonical Execution IR.
It is not a replacement for it.
</p>

<hr/>

<h2 id="backend-contract-artifacts">10. Backend Contract Artifacts</h2>

<p>
The contract emission stage SHOULD produce a <code>frog_backend_contract</code> artifact.
</p>

<p>
Its job is to declare what the runtime or downstream backend-side consumer must accept and execute.
</p>

<p>
Conceptual shape:
</p>

<pre><code>{
  "artifact_kind": "frog_backend_contract",
  "artifact_version": "0.1",
  "program_id": "prog:05_bounded_ui_accumulator",
  "backend_family": "reference_host_runtime_ui_binding",
  "assumptions": {},
  "units": [],
  "unsupported": [],
  "diagnostics": []
}</code></pre>

<p>
This artifact SHOULD make explicit:
</p>

<ul>
  <li>unit identities,</li>
  <li>state-cell obligations when present,</li>
  <li>UI bindings when present,</li>
  <li>public boundary obligations when present,</li>
  <li>unsupported surfaces preserved only as non-semantic metadata.</li>
</ul>

<hr/>

<h2 id="runtime-artifacts">11. Runtime Artifacts</h2>

<p>
The runtime stage SHOULD produce a <code>frog_runtime_result</code> artifact.
</p>

<p>
Its job is to record the outcome of runtime-side consumption of the backend contract.
</p>

<p>
Conceptual shape:
</p>

<pre><code>{
  "artifact_kind": "frog_runtime_result",
  "artifact_version": "0.1",
  "program_id": "prog:05_bounded_ui_accumulator",
  "status": "ok",
  "contract_ref": {
    "unit_ids": ["main"]
  },
  "execution_summary": {},
  "outputs": {},
  "diagnostics": []
}</code></pre>

<p>
This artifact SHOULD distinguish:
</p>

<ul>
  <li>runtime success,</li>
  <li>explicit runtime rejection,</li>
  <li>runtime failure after contract acceptance.</li>
</ul>

<p>
It SHOULD also preserve observable result surfaces separately, such as:
</p>

<ul>
  <li>public outputs,</li>
  <li>UI outputs,</li>
  <li>UI effects,</li>
  <li>explicit state surfaces.</li>
</ul>

<hr/>

<h2 id="ui-host-artifacts">12. UI Host Artifacts</h2>

<p>
For UI-bearing slices, the implementation MAY use additional internal UI-host-facing helper artifacts.
</p>

<p>
Examples include:
</p>

<ul>
  <li><code>frog_ui_binding_plan</code>,</li>
  <li><code>frog_ui_host_state</code>,</li>
  <li><code>frog_ui_effect_log</code>.</li>
</ul>

<p>
Such helper artifacts MAY be useful internally, especially for:
</p>

<ul>
  <li>widget value reads and writes,</li>
  <li>widget reference resolution,</li>
  <li>property-write application,</li>
  <li>UI effect reporting.</li>
</ul>

<p>
However:
</p>

<ul>
  <li>they SHOULD map clearly to the main artifact family chain,</li>
  <li>they MUST NOT become hidden substitutes for the main stage artifacts,</li>
  <li>they MUST NOT be mistaken for normative specification layers.</li>
</ul>

<hr/>

<h2 id="subset-honesty-and-unsupported-status">13. Subset Honesty and Unsupported Status</h2>

<p>
Every stage that can encounter a broader published feature than the current reference subset supports SHOULD remain explicit about unsupported status.
</p>

<p>
A useful diagnostic distinction is:
</p>

<ul>
  <li><strong>rejected</strong> — invalid even in the published model being claimed against,</li>
  <li><strong>unsupported_but_valid</strong> — valid in the broader published model, but outside the current reference subset,</li>
  <li><strong>implementation_private_extension</strong> — available only as an explicit implementation-side extension.</li>
</ul>

<p>
This distinction is especially important when the repository architecture already permits richer future widget-class families than the subset actually exercised by the reference pipeline.
</p>

<hr/>

<h2 id="example-05-minimum-artifact-chain">14. Example 05 Minimum Artifact Chain</h2>

<p>
For the first priority slice:
</p>

<pre><code>Examples/05_bounded_ui_accumulator/main.frog</code></pre>

<p>
the minimum expected internal artifact chain is:
</p>

<pre><code>frog_loaded_source
    -&gt; frog_validation_result
    -&gt; frog_derived_execution_ir
    -&gt; frog_lowered_form
    -&gt; frog_backend_contract
    -&gt; frog_runtime_result
</code></pre>

<p>
The minimum observable facts that this chain must preserve include:
</p>

<ul>
  <li>the existence of the u16 control and indicator widgets,</li>
  <li>the distinction between <code>widget_value</code> and <code>widget_reference</code>,</li>
  <li>the bounded loop count of five,</li>
  <li>the explicit state path through <code>frog.core.delay</code>,</li>
  <li>the final public output,</li>
  <li>the final indicator value,</li>
  <li>the separate reporting of presentation-property writes such as <code>face_color</code>,</li>
  <li>the non-semantic status of source-persisted <code>face_template</code>.</li>
</ul>

<hr/>

<h2 id="stability-rule">15. Stability Rule</h2>

<p>
The exact field spelling of these internal artifacts MAY evolve as the reference implementation grows.
</p>

<p>
However, the following SHOULD remain stable unless a documented reference-side migration is introduced:
</p>

<ul>
  <li>the primary artifact-kind names,</li>
  <li>their stage ordering,</li>
  <li>their semantic intent,</li>
  <li>their role in CLI output and artifact requirements.</li>
</ul>

<p>
This rule exists so that:
</p>

<ul>
  <li><code>frogc.md</code>,</li>
  <li><code>pipeline.md</code>,</li>
  <li><code>example-artifact-requirements.md</code>,</li>
  <li>and implementation code</li>
</ul>

<p>
do not drift into competing internal vocabularies.
</p>

<hr/>

<h2 id="summary">16. Summary</h2>

<p>
This document defines the reference implementation's internal artifact taxonomy.
</p>

<p>
Its purpose is to keep the workspace aligned on one stage-separated internal chain:
</p>

<ul>
  <li><code>frog_loaded_source</code>,</li>
  <li><code>frog_validation_result</code>,</li>
  <li><code>frog_derived_execution_ir</code>,</li>
  <li><code>frog_lowered_form</code>,</li>
  <li><code>frog_backend_contract</code>,</li>
  <li><code>frog_runtime_result</code>.</li>
</ul>

<p>
That alignment makes it possible for the reference implementation to remain explicit, traceable, and honest about what it really executes today, especially for the first priority bounded UI accumulator slice.
</p>
