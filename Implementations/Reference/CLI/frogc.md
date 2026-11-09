<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center"><code>frogc</code> Command Surface</h1>

<p align="center">
  <strong>Stage-aware command-line surface for the non-normative FROG reference pipeline</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#status-and-boundary">2. Status and Boundary</a></li>
  <li><a href="#current-primary-example">3. Current Primary Example</a></li>
  <li><a href="#command-summary">4. Command Summary</a></li>
  <li><a href="#validate">5. <code>validate</code></a></li>
  <li><a href="#derive-ir">6. <code>derive-ir</code></a></li>
  <li><a href="#lower">7. <code>lower</code></a></li>
  <li><a href="#emit-contract">8. <code>emit-contract</code></a></li>
  <li><a href="#run">9. <code>run</code></a></li>
  <li><a href="#backend-family-selection">10. Backend-Family Selection</a></li>
  <li><a href="#artifact-visibility">11. Artifact Visibility</a></li>
  <li><a href="#design-rules">12. Design Rules</a></li>
  <li><a href="#summary">13. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
<code>frogc</code> is the command-line surface of the non-normative FROG reference pipeline.
Its purpose is to expose the published stage boundaries explicitly rather than hide them behind one opaque execution command.
</p>

<p>
The reference command shape is:
</p>

<pre><code>frogc validate &lt;file.frog&gt;
frogc derive-ir &lt;file.frog&gt;
frogc lower &lt;file.frog&gt; [--backend-family &lt;family_id&gt;]
frogc emit-contract &lt;file.frog&gt; [--backend-family &lt;family_id&gt;]
frogc run &lt;file.frog&gt; [--backend-family &lt;family_id&gt;]
</code></pre>

<hr/>

<h2 id="status-and-boundary">2. Status and Boundary</h2>

<p>
<code>frogc</code> is not a specification layer.
It is a convenience surface over the reference implementation modules under:
</p>

<pre><code>Implementations/Reference/
</code></pre>

<p>
Accordingly, <code>frogc</code> must not:
</p>

<ul>
  <li>become the owner of source validity,</li>
  <li>become the owner of semantic truth,</li>
  <li>replace the open FROG Execution IR,</li>
  <li>replace backend contract emission with hidden runtime behavior.</li>
</ul>

<hr/>

<h2 id="current-primary-example">3. Current Primary Example</h2>

<p>
The current primary reference example is:
</p>

<pre><code>Examples/05_bounded_ui_accumulator/main.frog</code></pre>

<p>
This example is the current preferred anchor because it exercises, in one coherent corridor:
</p>

<ul>
  <li>front-panel widgets,</li>
  <li><code>widget_value</code>,</li>
  <li><code>widget_reference</code>,</li>
  <li><code>frog.ui.property_write</code>,</li>
  <li>canonical <code>for_loop</code> structure,</li>
  <li>explicit local memory through <code>frog.core.delay</code>,</li>
  <li>public output exposure,</li>
  <li>and first end-to-end runtime execution posture.</li>
</ul>

<hr/>

<h2 id="command-summary">4. Command Summary</h2>

<pre><code># validate canonical source against the supported reference subset
frogc validate Examples/05_bounded_ui_accumulator/main.frog

# derive open execution-facing IR
frogc derive-ir Examples/05_bounded_ui_accumulator/main.frog

# lower for the selected backend family
frogc lower Examples/05_bounded_ui_accumulator/main.frog --backend-family llvm_cpu_v1

# emit the backend-facing contract
frogc emit-contract Examples/05_bounded_ui_accumulator/main.frog --backend-family llvm_cpu_v1

# execute through the reference runtime path
frogc run Examples/05_bounded_ui_accumulator/main.frog --backend-family llvm_cpu_v1
</code></pre>

<hr/>

<h2 id="validate">5. <code>validate</code></h2>

<p>
The <code>validate</code> command checks whether the source belongs to the supported validated reference subset.
</p>

<p>
Typical responsibilities include:
</p>

<ul>
  <li>top-level section validation,</li>
  <li>interface validation,</li>
  <li>front-panel widget validation,</li>
  <li>presentation metadata validation,</li>
  <li>diagram-node validation,</li>
  <li>canonical <code>for_loop</code> structure validation,</li>
  <li>explicit local-memory validation,</li>
  <li><code>widget_reference</code> and property-write validation.</li>
</ul>

<p>
Example:
</p>

<pre><code>frogc validate Examples/05_bounded_ui_accumulator/main.frog
</code></pre>

<p>
Expected output posture:
</p>

<ul>
  <li>a validation artifact,</li>
  <li>validated subset flags,</li>
  <li>and explicit diagnostics if validation fails.</li>
</ul>

<hr/>

<h2 id="derive-ir">6. <code>derive-ir</code></h2>

<p>
The <code>derive-ir</code> command derives an open execution-facing representation from the validated source.
</p>

<p>
For the current slice, the derived IR should preserve at least:
</p>

<ul>
  <li><code>widget_value</code> participation,</li>
  <li><code>widget_reference</code> participation,</li>
  <li><code>frog.ui.property_write</code> object intent,</li>
  <li>canonical <code>for_loop</code> identity,</li>
  <li>body-region structure,</li>
  <li>explicit local memory,</li>
  <li>public output attribution,</li>
  <li>front-panel declarations as non-semantic UI metadata.</li>
</ul>

<p>
Example:
</p>

<pre><code>frogc derive-ir Examples/05_bounded_ui_accumulator/main.frog
</code></pre>

<hr/>

<h2 id="lower">7. <code>lower</code></h2>

<p>
The <code>lower</code> command specializes the open Execution IR for a selected backend family.
</p>

<p>
This step may:
</p>

<ul>
  <li>specialize the loop representation,</li>
  <li>specialize explicit state handling,</li>
  <li>materialize UI-binding operations,</li>
  <li>prepare backend-oriented assumptions,</li>
  <li>but it must not collapse FROG itself into the backend family.</li>
</ul>

<p>
Example:
</p>

<pre><code>frogc lower Examples/05_bounded_ui_accumulator/main.frog --backend-family llvm_cpu_v1
</code></pre>

<p>
For the first executable slice, the lowered form should still preserve:
</p>

<ul>
  <li>bounded loop identity,</li>
  <li>explicit local memory,</li>
  <li>natural UI value paths,</li>
  <li>object-style UI write operations,</li>
  <li>non-semantic front-face metadata preservation.</li>
</ul>

<hr/>

<h2 id="emit-contract">8. <code>emit-contract</code></h2>

<p>
The <code>emit-contract</code> command emits the backend-facing handoff artifact consumed by a backend or runtime family.
</p>

<p>
This is the explicit downstream boundary after lowering.
The emitted contract should remain attributable to:
</p>

<ul>
  <li>the original source,</li>
  <li>the selected backend family,</li>
  <li>the preserved loop and state model,</li>
  <li>the preserved UI participation model.</li>
</ul>

<p>
Example:
</p>

<pre><code>frogc emit-contract Examples/05_bounded_ui_accumulator/main.frog --backend-family llvm_cpu_v1
</code></pre>

<p>
For the current slice, the emitted contract should make explicit:
</p>

<ul>
  <li>one bounded loop of five iterations,</li>
  <li>one explicit local state cell initialized to zero,</li>
  <li>one control input value,</li>
  <li>one indicator output value,</li>
  <li>one public output value,</li>
  <li>minimal object-style UI writes for <code>face_color</code>,</li>
  <li>and preserved non-semantic UI metadata.</li>
</ul>

<hr/>

<h2 id="run">9. <code>run</code></h2>

<p>
The <code>run</code> command executes through the runtime-side consumption of the emitted or implied backend contract.
</p>

<p>
For the first executable slice, the conservative reference reading is:
</p>

<ol>
  <li>load source,</li>
  <li>validate,</li>
  <li>derive IR,</li>
  <li>lower,</li>
  <li>emit backend contract,</li>
  <li>prepare runtime-side execution,</li>
  <li>sample the control value,</li>
  <li>apply explicit UI property writes,</li>
  <li>run the bounded accumulation loop,</li>
  <li>publish the final indicator value,</li>
  <li>publish the same public output value.</li>
</ol>

<p>
Example:
</p>

<pre><code>frogc run Examples/05_bounded_ui_accumulator/main.frog --backend-family llvm_cpu_v1
</code></pre>

<hr/>

<h2 id="backend-family-selection">10. Backend-Family Selection</h2>

<p>
Backend-family selection is explicit whenever lowering, contract emission, or runtime execution depends on it.
</p>

<p>
The recommended first LLVM-oriented family id is:
</p>

<pre><code>llvm_cpu_v1</code></pre>

<p>
This identifier means:
</p>

<ul>
  <li>an LLVM-oriented downstream backend-family target,</li>
  <li>not the definition of FROG,</li>
  <li>not a replacement for open FROG IR,</li>
  <li>not a collapse of backend contract into one private runtime implementation.</li>
</ul>

<hr/>

<h2 id="artifact-visibility">11. Artifact Visibility</h2>

<p>
A key role of <code>frogc</code> is artifact visibility.
A user should be able to inspect the result of each stage rather than rely on hidden transitions.
</p>

<p>
At minimum, the current reference path should make visible:
</p>

<ul>
  <li>validation result,</li>
  <li>derived Execution IR,</li>
  <li>lowered form,</li>
  <li>backend contract,</li>
  <li>runtime result.</li>
</ul>

<hr/>

<h2 id="design-rules">12. Design Rules</h2>

<ul>
  <li>Keep stage boundaries explicit.</li>
  <li>Prefer inspectability over hidden convenience.</li>
  <li>Fail explicitly when a stage is unsupported.</li>
  <li>Do not silently bypass validation, derivation, lowering, or contract emission.</li>
  <li>Keep the command surface small enough to audit.</li>
  <li>Keep backend-family choice explicit when it matters.</li>
  <li>Do not turn front-face metadata into hidden semantic law.</li>
</ul>

<hr/>

<h2 id="summary">13. Summary</h2>

<p>
<code>frogc</code> is the visible command surface of the non-normative reference pipeline.
Its job is to expose a small, stage-aware, inspectable corridor across:
</p>

<ul>
  <li>validation,</li>
  <li>Execution IR derivation,</li>
  <li>lowering,</li>
  <li>backend contract emission,</li>
  <li>runtime execution.</li>
</ul>

<p>
For the current campaign, it should be centered primarily on:
</p>

<pre><code>Examples/05_bounded_ui_accumulator/main.frog</code></pre>

<p>
That keeps the command surface aligned with the repository’s most important near-term proof target: one fully explicit executable vertical slice.
</p>
