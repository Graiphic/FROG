<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 05 Execution Pipeline</h1>

<p align="center">
  <strong>Concrete source, FIR, lowering, runtime, widget, and LLVM-oriented proof corridor for Example 05</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This page explains how <code>Examples/05_bounded_ui_accumulator</code> runs through the current public reference corridor.
It is a concrete companion to <a href="./Readme.md">Example 05 — Bounded UI Accumulator</a>.
</p>

<p>
The page is descriptive and non-normative. It does not make LLVM part of FROG semantics, does not define a production runtime, and does not turn one hosted UI into the FROG language runtime identity.
</p>

<hr/>

<h2>Artifact Flow</h2>

<pre><code>Examples/05_bounded_ui_accumulator/main.frog
  -&gt; loadability
  -&gt; structural validation
  -&gt; semantic validation
  -&gt; Examples/05_bounded_ui_accumulator/main.fir.json
  -&gt; Examples/05_bounded_ui_accumulator/main.lowering.json
  -&gt; Implementations/Reference/ContractEmitter/examples/05_bounded_ui_accumulator.reference_host_runtime_ui_binding.contract.json
  -&gt; Implementations/Reference/Runtime/acceptance/example05_runtime_family.acceptance.json
  -&gt; Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/module.ll
  -&gt; Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/kernel.ll
  -&gt; Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/native_kernel_manifest.json
  -&gt; runtime bridge and hosted front-panel snapshot</code></pre>

<hr/>

<h2>Two Branches Joined by the Runtime</h2>

<h3>Diagram and computation branch</h3>

<pre><code>main.frog diagram
  -&gt; validated program meaning
  -&gt; FIR / Execution IR
  -&gt; lowering
  -&gt; backend contract
  -&gt; LLVM-oriented module / native proof where applicable
  -&gt; runtime call boundary</code></pre>

<p>
The diagram branch owns the executable dataflow path. In this example, the bounded accumulator computes the result from the public input and explicit stateful iteration shape.
</p>

<h3>Front-panel and widget branch</h3>

<pre><code>main.frog front_panel
  -&gt; ui/accumulator_panel.wfrog
  -&gt; Libraries/Realizations/Default/assets/numeric/templates/numeric_rectangular.svg
  -&gt; hosted numeric control and indicator realization
  -&gt; runtime widget bindings</code></pre>

<p>
The front-panel branch owns the visible widget layout and realization-facing path. The <code>.wfrog</code> package and SVG asset support realization; they do not replace <code>.frog</code> as the program source and do not define hidden language semantics.
</p>

<hr/>

<h2>Runtime Role</h2>

<p>
The runtime is the orchestration consumer at the point where the two branches meet.
For this example, the runtime-family corridor may:
</p>

<ul>
  <li>consume the host-runtime UI-binding contract,</li>
  <li>consume the <code>.frog</code> front-panel instances,</li>
  <li>resolve the <code>.wfrog</code> realization package and numeric SVG realization asset,</li>
  <li>consume the native-kernel manifest where the LLVM-oriented proof path is used,</li>
  <li>bind <code>ctrl_input.value</code> to the execution input,</li>
  <li>execute the bounded accumulator kernel through the declared call boundary where applicable,</li>
  <li>publish the result to the public output and to <code>ind_result.value</code>,</li>
  <li>emit the runtime snapshot and diagnostic evidence.</li>
</ul>

<p>
In a hosted Run/Execute loop, the host samples the input-side widget value, the runtime executes the bounded dataflow unit, and the result is projected back into the output-side widget and published snapshot.
</p>

<hr/>

<h2>Boundary</h2>

<ul>
  <li><code>.frog</code> remains the canonical program source.</li>
  <li><code>.wfrog</code> remains the widget/front-panel package and realization publication path.</li>
  <li>SVG remains a visual realization resource.</li>
  <li>FIR remains the open execution-facing representation derived from validated meaning.</li>
  <li>Lowering and backend contracts remain the explicit downstream handoff.</li>
  <li>LLVM remains one compiler-family native proof path.</li>
  <li>The runtime remains a consumer and orchestrator of explicit contracts and artifacts.</li>
  <li>The host UI remains replaceable and does not define FROG semantics.</li>
</ul>
