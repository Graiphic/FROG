<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FIR in Hybrid UI and Compiler Execution</h1>

<p align="center">
  <strong>Clarifying the role of FIR when front-panel widgets, runtime orchestration, and compiler-family artifacts meet</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This companion note clarifies how FIR participates in a practical FROG execution corridor that includes a front panel, widget bindings, runtime orchestration, and optional compiler-family artifacts such as LLVM-oriented native proof material.
</p>

<p>
It complements <a href="./Readme.md">IR/Readme.md</a>, <a href="./Lowering.md">IR/Lowering.md</a>, and <a href="./Backend%20contract.md">IR/Backend contract.md</a> without replacing their ownership.
</p>

<hr/>

<h2>Core Rule</h2>

<p>
FIR is the open execution-facing representation derived from validated program meaning. It is not authored directly, and it is not a runtime-private graph, compiler-private graph, LLVM IR file, widget package, visual asset, or host UI object table.
</p>

<pre><code>canonical .frog source
  -&gt; loadability
  -&gt; structural validation
  -&gt; semantic validation
  -&gt; validated program meaning
  -&gt; FIR / Execution IR
  -&gt; lowering
  -&gt; backend contract
  -&gt; runtime-family and/or compiler-family consumption</code></pre>

<hr/>

<h2>What FIR May Preserve</h2>

<p>
In hybrid UI/compiler execution, FIR may preserve execution-relevant consequences of validated widget and interface meaning, including:
</p>

<ul>
  <li>public input and output participation,</li>
  <li>validated diagram units and dataflow dependencies,</li>
  <li>validated types and execution-facing units,</li>
  <li>stable source attribution and recoverable source mapping,</li>
  <li>widget identities where execution-facing interaction needs them,</li>
  <li><code>widget_value</code> participation where a widget's primary value enters or receives execution data,</li>
  <li><code>widget_reference</code> participation where object-style interaction targets properties, methods, events, or parts,</li>
  <li>execution-relevant consequences of <code>frog.ui.property_read</code>, <code>frog.ui.property_write</code>, or <code>frog.ui.method_invoke</code>.</li>
</ul>

<p>
This preservation exists so later lowering, backend contracts, runtime binding, diagnostics, probes, watches, and source-aligned observability can remain traceable.
</p>

<hr/>

<h2>What FIR Must Not Absorb</h2>

<p>
FIR must not absorb or redefine ownership of:
</p>

<ul>
  <li>canonical <code>.frog</code> source structure,</li>
  <li><code>.wfrog</code> package definition,</li>
  <li>SVG rendering strategy,</li>
  <li>host-specific widget storage,</li>
  <li>runtime-private scheduler state,</li>
  <li>runtime-private UI object tables,</li>
  <li>LLVM IR or compiler-private optimization state,</li>
  <li>one host's rendering pipeline as if it were canonical open execution representation.</li>
</ul>

<hr/>

<h2>Hybrid Execution Reading</h2>

<p>
A practical hosted execution may contain two branches:
</p>

<pre><code>diagram/computation branch:
.frog diagram
  -&gt; semantic validation
  -&gt; FIR
  -&gt; lowering
  -&gt; backend contract
  -&gt; LLVM-oriented artifact or other backend-family artifact where applicable

front-panel/widget branch:
.frog front_panel
  -&gt; widget instances and bindings
  -&gt; .wfrog packages or realization packages
  -&gt; SVG/assets or other realization resources
  -&gt; host UI</code></pre>

<p>
FIR sits on the execution-facing branch after validated meaning. It may carry enough widget-related identity and mapping to allow the runtime to bind values and object-style accesses, but it does not own the front-panel realization itself.
</p>

<hr/>

<h2>Runtime and Compiler Boundary</h2>

<p>
A runtime may consume FIR-derived contracts, manifests, and artifacts. A compiler-family path such as LLVM may consume lowered material to produce a native proof or loadable artifact. Neither the runtime nor LLVM becomes the language owner.
</p>

<table>
  <thead>
    <tr>
      <th>Layer</th>
      <th>Correct role</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>FIR</td>
      <td>Open execution-facing representation derived from validated meaning.</td>
    </tr>
    <tr>
      <td>Lowering</td>
      <td>Specialization toward target, backend, runtime, provider, or deployment concerns.</td>
    </tr>
    <tr>
      <td>Backend contract</td>
      <td>Explicit handoff to downstream consumers.</td>
    </tr>
    <tr>
      <td>LLVM</td>
      <td>One possible compiler-family consumer and native proof path.</td>
    </tr>
    <tr>
      <td>Runtime</td>
      <td>Execution and orchestration consumer of explicit contracts and artifacts.</td>
    </tr>
    <tr>
      <td>Host UI</td>
      <td>Rendering and interaction surface, replaceable by host family.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Boundary Summary</h2>

<ul>
  <li>FIR is not <code>.frog</code>.</li>
  <li>FIR is not <code>.wfrog</code>.</li>
  <li>FIR is not SVG.</li>
  <li>FIR is not LLVM IR.</li>
  <li>FIR is not a runtime-private graph.</li>
  <li>FIR may preserve execution-relevant widget consequences.</li>
  <li>FIR must preserve source attribution where downstream execution, diagnostics, or observability need it.</li>
</ul>
