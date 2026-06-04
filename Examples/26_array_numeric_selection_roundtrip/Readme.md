<h1>Array 2D Numeric Selection Roundtrip</h1>

<p>
Example <code>26</code> demonstrates a bounded 2D numeric
<code>frog.widgets.array_control</code> with a read-only
<code>frog.widgets.array_indicator</code>.
</p>

<blockquote>
<strong>Deprecated historical milestone. Reviewed 2026-06-04.</strong>
This rigid/non-widget-composed Array example is retained for regression and
traceability only. It is superseded by the widget-composed Array container
examples starting at Example <code>29</code>, where Array repeats contained
Default widget realizations.
</blockquote>

<p>
The example proves the first Default Array baseline: rank, shape, element
records, selected element, index displays, viewport, scrollbars, labels, layout,
and visual overrides are owned by <code>.frog</code>; the <code>.wfrog</code>
package references Default Array assets; the C++ Graiphic private runtime
consumes the manifest-backed native selected-element proof corridor.
</p>

<pre><code>.frog Array instances
  -&gt; FIR
  -&gt; lowering
  -&gt; LLVM/native u16 proof manifest
  -&gt; C++ Graiphic private runtime
  -&gt; .wfrog Default Array realization assets
  -&gt; SVG array shell + index display + element region + scrollbars
</code></pre>

<h2>Files</h2>

<ul>
  <li><a href="./main.frog"><code>main.frog</code></a></li>
  <li><a href="./main.fir.json"><code>main.fir.json</code></a></li>
  <li><a href="./main.lowering.json"><code>main.lowering.json</code></a></li>
  <li><a href="./ui/array_panel.wfrog"><code>ui/array_panel.wfrog</code></a></li>
  <li><a href="../../Implementations/Reference/LLVM/examples/26_array_numeric_selection_roundtrip/native_kernel_manifest.json"><code>native_kernel_manifest.json</code></a></li>
  <li><a href="../../Implementations/Reference/LLVM/examples/26_array_numeric_selection_roundtrip/kernel.ll"><code>kernel.ll</code></a></li>
</ul>

<h2>Validation Intent</h2>

<ul>
  <li>Array is a homogeneous collection widget, not a hardcoded numeric table.</li>
  <li>This deprecated historical example intentionally validates a bounded 2D <code>u16</code> Array specialization for regression context.</li>
  <li>The <code>.frog</code> source owns rank, shape, element records, selected indices, viewport, labels, layout, and visual overrides.</li>
  <li>Scrollbar placement and distance from the element viewport are source-owned viewport/style properties, not runtime constants.</li>
  <li>The <code>.wfrog</code> package resolves Default Array assets and declares Array host capabilities.</li>
  <li>Cell edits and selection remain front-panel state until <code>Execute</code>.</li>
  <li><code>Execute</code> consumes the selected element through the manifest-backed native <code>u16</code> corridor.</li>
  <li>The full array record flow is declared in <code>.frog</code>, FIR, and lowering as a typed dataflow value, not as implicit runtime UI synchronization.</li>
  <li>No example-local SVG skin is duplicated.</li>
</ul>
