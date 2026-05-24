<h1>Frame Rectangle Board</h1>

<p>
Example <code>25</code> demonstrates the standalone <code>frog.widgets.frame</code>
as a simple support rectangle.
</p>

<p>
The example renders three differently sized Frames with different background colors,
border colors, and border widths. It proves that the Default Frame SVG remains
resizable and that each border keeps one uniform thickness on all sides.
</p>

<pre><code>.frog Frame instances
  -&gt; FIR
  -&gt; lowering
  -&gt; LLVM/native bool proof manifest
  -&gt; C++ Graiphic private runtime
  -&gt; .wfrog Default Frame realization asset
  -&gt; SVG background + border parts
</code></pre>

<h2>Files</h2>

<ul>
  <li><a href="./main.frog"><code>main.frog</code></a></li>
  <li><a href="./main.fir.json"><code>main.fir.json</code></a></li>
  <li><a href="./main.lowering.json"><code>main.lowering.json</code></a></li>
  <li><a href="./ui/frame_panel.wfrog"><code>ui/frame_panel.wfrog</code></a></li>
  <li><a href="../../Implementations/Reference/LLVM/examples/25_frame_grouping_board/native_kernel_manifest.json"><code>native_kernel_manifest.json</code></a></li>
  <li><a href="../../Implementations/Reference/LLVM/examples/25_frame_grouping_board/kernel.ll"><code>kernel.ll</code></a></li>
</ul>

<h2>Validation Intent</h2>

<ul>
  <li>Frame is visual support only: no value, no child <code>.frog</code>, no Subpanel behavior.</li>
  <li>No caption is built into the Frame. Use a separate Label widget when text is needed.</li>
  <li>The <code>.frog</code> source owns every Frame position, size, background color, border color, and border width.</li>
  <li>The <code>.wfrog</code> package resolves the Default Frame asset; no example-local SVG skin is duplicated.</li>
  <li>The Default SVG publishes only <code>root</code>, <code>background</code>, and <code>border</code> parts.</li>
  <li>The border uses a non-scaling stroke so its thickness stays uniform under resize.</li>
  <li>The native manifest remains consumed through the runtime; LLVM is still a backend artifact producer.</li>
</ul>
