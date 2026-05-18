<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 18 - Decoration Board</h1>

<p align="center">
  <strong>One flat box and two line decorations rendered from source-owned layout/style and Default SVG assets</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This example introduces standalone Decoration support widgets after the current
public reference runtime closure. It keeps the public source and realization
surface visible while Graiphic runtime implementation continues privately.
</p>

<p>
The front panel intentionally uses only three visible widgets:
one <code>frog.widgets.flat_box</code>, one
<code>frog.widgets.horizontal_line</code>, and one
<code>frog.widgets.vertical_line</code>. These widgets do not own value
semantics and do not participate in the diagram contract. The diagram only
publishes a manifest-backed <code>scene_ready</code> proof output.
</p>

<hr/>

<h2>Source / Realization Boundary</h2>

<ul>
  <li><code>.frog</code> owns every decoration instance, position, size, fill, border color, border width, line color, and line width.</li>
  <li><code>.wfrog</code> references the Default Decorations realization package and SVG assets.</li>
  <li>The Default Decorations SVG assets publish <code>root</code>, <code>shape</code>, and <code>line</code> parts.</li>
  <li>The runtime consumes the manifest-backed native proof artifact and binds visual properties to the published SVG parts.</li>
</ul>

<hr/>

<h2>Native Proof</h2>

<p>
The LLVM/native artifact is intentionally a small boolean identity proof:
</p>

<pre><code>scene_ready = scene_ready_request</code></pre>

<p>
Decoration widgets are support widgets. The proof keeps the example on the
native manifest corridor without assigning scalar value semantics to
decorations.
</p>

<hr/>

<h2>Runtime Status</h2>

<p>
This example is repository-visible public specification material beyond the
current public reference runtime closure. It is validated by Graiphic private
C++, Python, and Rust runtime families in <code>Graiphic/FROG-Runtime</code>.
Those runtimes consume this public <code>.frog</code>, <code>.wfrog</code>,
Default SVG asset, and native manifest surface without publishing Graiphic's
production runtime implementation in this repository.
</p>
