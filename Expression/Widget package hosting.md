<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Widget Package Hosting Role</h1>

<p align="center">
  <strong>How <code>.wfrog</code> packages participate in hosted front-panel realization</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This companion note clarifies how widget packages participate when a FROG front panel is hosted by a runtime or UI host.
It complements <a href="./Widget%20package%20%28.wfrog%29.md">FROG Widget Package (.wfrog)</a>.
</p>

<hr/>

<h2>Boundary</h2>

<ul>
  <li><code>.frog</code> is the canonical program source.</li>
  <li><code>.wfrog</code> publishes widget package and realization-facing metadata.</li>
  <li>SVG and related assets are visual realization resources.</li>
  <li>A runtime or host consumes compatible package information to realize and bind widgets.</li>
</ul>

<p>
A <code>.frog</code> source may identify widget classes, widget package identities, or realization choices. A host may then resolve compatible <code>.wfrog</code> packages and related assets when a front panel needs to be rendered or interacted with.
</p>

<hr/>

<h2>Hosting Flow</h2>

<pre><code>.frog front_panel
  -&gt; widget instances, layout, initial values, and diagram/widget bindings
  -&gt; referenced widget classes or package identities
  -&gt; compatible .wfrog package
  -&gt; SVG/assets or other realization resources
  -&gt; host UI
  -&gt; runtime widget bindings</code></pre>

<p>
The host/runtime consumption role may include package resolution, asset loading, part mapping, value binding, property binding, event routing, capability checks, and diagnostic reporting.
</p>

<hr/>

<h2>Non-Ownership Rules</h2>

<ul>
  <li>A <code>.wfrog</code> package does not replace the canonical <code>.frog</code> program source.</li>
  <li>SVG assets do not define widget semantics.</li>
  <li>A host-specific rendering path does not become portable widget law.</li>
  <li>Runtime support claims remain separate from package validity.</li>
  <li>Public widget law remains published through the widget/package specification surfaces.</li>
</ul>

<hr/>

<h2>Practical Reading</h2>

<p>
In a hosted example, the front-panel branch and execution branch meet through runtime bindings.
The <code>.wfrog</code> package helps the host realize visible widgets, while FIR, lowering, backend contracts, manifests, and runtime-family artifacts carry the execution-facing path.
</p>
