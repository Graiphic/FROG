<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Reference CLI</h1>

<p align="center">
  <strong>Command-line surface for the non-normative FROG reference implementation</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#status-and-boundary">2. Status and Boundary</a></li>
  <li><a href="#current-role">3. Current Role</a></li>
  <li><a href="#current-campaign-priority">4. Current Campaign Priority</a></li>
  <li><a href="#command-surface">5. Command Surface</a></li>
  <li><a href="#relation-with-the-reference-pipeline">6. Relation with the Reference Pipeline</a></li>
  <li><a href="#current-entry-points">7. Current Entry Points</a></li>
  <li><a href="#backend-family-posture">8. Backend-Family Posture</a></li>
  <li><a href="#design-rules">9. Design Rules</a></li>
  <li><a href="#relation-with-examples-and-stage-results">10. Relation with Examples and Stage Results</a></li>
  <li><a href="#what-this-directory-must-not-do">11. What this Directory Must Not Do</a></li>
  <li><a href="#summary">12. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This directory contains the command-line surface of the non-normative FROG reference implementation.
Its role is to expose a small, explicit, stage-aware toolchain for the first executable vertical slices of FROG.
</p>

<p>
The CLI is not a language specification layer.
It is a practical implementation-facing shell around the published:
</p>

<ul>
  <li>source loading,</li>
  <li>validation,</li>
  <li>Execution IR derivation,</li>
  <li>lowering,</li>
  <li>backend contract emission,</li>
  <li>and runtime execution path.</li>
</ul>

<p>
Its purpose is to make the first executable reference corridor observable and testable without collapsing the repository's architectural stages into one opaque command.
</p>

<hr/>

<h2 id="status-and-boundary">2. Status and Boundary</h2>

<p>
This directory is non-normative.
It does not define:
</p>

<ul>
  <li>the language,</li>
  <li>the canonical source format,</li>
  <li>the validated program meaning,</li>
  <li>the open Execution IR,</li>
  <li>the backend contract boundary,</li>
  <li>or the runtime as a normative semantic owner.</li>
</ul>

<p>
Those remain owned by the published specification layers and by the non-CLI reference modules under <code>Implementations/Reference/</code>.
</p>

<p>
The CLI exists to make those published boundaries executable and inspectable.
It must not become a hidden source of language truth.
</p>

<hr/>

<h2 id="current-role">3. Current Role</h2>

<p>
At the current stage of the repository, the CLI serves four practical purposes:
</p>

<ul>
  <li>provide explicit stage entry points for the first executable slice,</li>
  <li>make intermediate stage results observable during reference-implementation work,</li>
  <li>support repeatable command-line execution of the minimal published example path,</li>
  <li>and preserve attribution between the canonical <code>.frog</code> source and every intermediate artifact.</li>
</ul>

<p>
The CLI should therefore remain compact, explicit, and easy to audit before it expands to broader UI-oriented and state-oriented slices.
</p>

<hr/>

<h2 id="current-campaign-priority">4. Current Campaign Priority</h2>

<p>
The current reference priority is the first complete executable vertical slice built around:
</p>

<pre><code>Examples/05_bounded_ui_accumulator/main.frog</code></pre>

<p>
That slice is intentionally small but cross-layer complete.
It combines:
</p>

<ul>
  <li>front-panel declaration,</li>
  <li><code>u16</code> numeric widgets,</li>
  <li><code>widget_value</code> participation,</li>
  <li><code>widget_reference</code> participation,</li>
  <li><code>frog.ui.property_write</code> on <code>face_color</code>,</li>
  <li>one canonical <code>for_loop</code> structure,</li>
  <li>explicit local memory through <code>frog.core.delay</code>,</li>
  <li>one public output,</li>
  <li>and one first runtime corridor.</li>
</ul>

<p>
The CLI must therefore prioritize that slice over older narrower regressions when command behavior and documentation are updated.
</p>

<hr/>

<h2 id="command-surface">5. Command Surface</h2>

<p>
The reference command model is:
</p>

<pre><code>frogc validate &lt;file.frog&gt;
frogc derive-ir &lt;file.frog&gt;
frogc lower &lt;file.frog&gt; [--backend-family &lt;family_id&gt;]
frogc emit-contract &lt;file.frog&gt; [--backend-family &lt;family_id&gt;]
frogc run &lt;file.frog&gt; [--backend-family &lt;family_id&gt;]
</code></pre>

<p>
The exact user-facing surface may evolve, but the stage separation should remain explicit.
The CLI should mirror the published architecture rather than collapsing multiple stages into one opaque tool behavior.
</p>

<p>
For the first executable corridor, the recommended backend-family identifier is:
</p>

<pre><code>llvm_cpu_v1</code></pre>

<p>
This identifier is a backend-family contract target.
It does not redefine FROG itself.
LLVM remains downstream from FROG.
</p>

<hr/>

<h2 id="relation-with-the-reference-pipeline">6. Relation with the Reference Pipeline</h2>

<p>
The CLI is the operational shell of the reference pipeline described under:
</p>

<pre><code>Implementations/Reference/</code></pre>

<p>
Its commands should remain aligned with the stage boundaries used by the reference implementation:
</p>

<ul>
  <li><strong>validate</strong> — confirm that source belongs to the supported validated subset,</li>
  <li><strong>derive-ir</strong> — derive an open execution-facing representation with recoverable attribution,</li>
  <li><strong>lower</strong> — specialize the open IR for a selected backend family,</li>
  <li><strong>emit-contract</strong> — produce the handoff consumed by a backend or runtime,</li>
  <li><strong>run</strong> — execute through runtime-side contract consumption.</li>
</ul>

<p>
The CLI should make those stages visible.
It should not silently bypass them while still claiming to represent the full pipeline.
</p>

<hr/>

<h2 id="current-entry-points">7. Current Entry Points</h2>

<p>
A compact demonstration entry point may exist in this directory for early executable slices.
That is acceptable as long as it remains explicitly non-normative and does not blur ownership of the stages.
</p>

<p>
In the current reference direction, the practical CLI entry point may be:
</p>

<pre><code>frog_demo_pipeline.py</code></pre>

<p>
while the implementation logic progressively lives in specialized modules under:
</p>

<pre><code>Implementations/Reference/
├── Loader/
├── Validator/
├── Deriver/
├── Lowerer/
├── ContractEmitter/
└── Runtime/
</code></pre>

<p>
This is the preferred direction because it preserves a simple command-line surface without turning one convenience script into accidental architecture.
</p>

<hr/>

<h2 id="backend-family-posture">8. Backend-Family Posture</h2>

<p>
The CLI may expose backend-family selection through:
</p>

<pre><code>--backend-family &lt;family_id&gt;</code></pre>

<p>
For the first complete executable slice, two postures are useful:
</p>

<ul>
  <li><code>DEFAULT_BACKEND_FAMILY</code> — the conservative internal reference family used by the current Python demo path,</li>
  <li><code>llvm_cpu_v1</code> — the first explicit LLVM-oriented backend-family contract target.</li>
</ul>

<p>
The CLI must preserve the following distinction:
</p>

<pre><code>FROG source / validation / open IR
    !=
backend family
    !=
runtime-private realization
</code></pre>

<p>
A command such as:
</p>

<pre><code>frogc emit-contract Examples/05_bounded_ui_accumulator/main.frog --backend-family llvm_cpu_v1</code></pre>

<p>
means:
</p>

<ul>
  <li>derive a backend-facing contract suitable for an LLVM-oriented consumer family,</li>
  <li>not redefine the language in LLVM terms,</li>
  <li>and not erase the open FROG pipeline stages that come before backend handoff.</li>
</ul>

<hr/>

<h2 id="design-rules">9. Design Rules</h2>

<ul>
  <li>Commands should reflect published architectural boundaries.</li>
  <li>Commands should fail explicitly rather than silently skipping invalid stages.</li>
  <li>Intermediate artifacts should remain observable.</li>
  <li>The CLI should remain stage-aware rather than pretending one monolithic command is the architecture.</li>
  <li>The CLI should remain small enough to audit.</li>
  <li>Example-specific shortcuts, if any, must remain explicit and non-normative.</li>
  <li>Backend-family choice must remain explicit when it matters.</li>
  <li>Front-panel presentation metadata may be preserved in artifacts, but must remain distinguished from executable semantic surfaces.</li>
</ul>

<hr/>

<h2 id="relation-with-examples-and-stage-results">10. Relation with Examples and Stage Results</h2>

<p>
The CLI exists partly to make stage results inspectable against named examples.
That means a user should be able to run one published example and inspect at least:
</p>

<ul>
  <li>the validation result,</li>
  <li>the derived Execution IR,</li>
  <li>the lowered form,</li>
  <li>the emitted backend contract,</li>
  <li>and the runtime-visible result.</li>
</ul>

<p>
For the current campaign, the reference example is:
</p>

<pre><code>Examples/05_bounded_ui_accumulator/main.frog</code></pre>

<p>
The CLI should make that example easy to execute and easy to inspect, including the preservation of:
</p>

<ul>
  <li>the canonical <code>for_loop</code> structure,</li>
  <li>explicit local memory,</li>
  <li><code>widget_value</code> paths,</li>
  <li><code>widget_reference</code> paths,</li>
  <li>and object-style <code>face_color</code> writes.</li>
</ul>

<hr/>

<h2 id="what-this-directory-must-not-do">11. What this Directory Must Not Do</h2>

<p>
This directory must not:
</p>

<ul>
  <li>become the hidden owner of source validity rules,</li>
  <li>redefine the canonical source model,</li>
  <li>redefine the validated semantic layer,</li>
  <li>collapse FROG Execution IR into backend-family detail,</li>
  <li>collapse backend contract into runtime-private realization,</li>
  <li>treat SVG face-template resources as executable semantic truth,</li>
  <li>or bypass intermediate stages while still pretending to preserve them.</li>
</ul>

<hr/>

<h2 id="summary">12. Summary</h2>

<p>
The CLI is the command-line shell of the non-normative FROG reference implementation.
Its job is to expose a small, stage-aware, inspectable path across:
</p>

<ul>
  <li>validation,</li>
  <li>Execution IR derivation,</li>
  <li>lowering,</li>
  <li>backend contract emission,</li>
  <li>and runtime execution.</li>
</ul>

<p>
For the current campaign, it should be centered primarily on the first complete executable slice:
</p>

<pre><code>Examples/05_bounded_ui_accumulator/main.frog</code></pre>

<p>
That keeps the command surface aligned with the repository’s real short-term credibility target: one complete, explicit, end-to-end executable corridor.
</p>
