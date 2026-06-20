<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Default Image Realization</h1>

<p align="center">
  <strong>Default SVG-backed skin contract for static pasted Image widgets</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<p>
Reviewed 2026-06-20: the Default Image realization defines a static rectangular
media surface for <code>frog.widgets.image_static</code>.
</p>

<h2>Public Parts</h2>

<ul>
  <li><code>root</code></li>
  <li><code>placement_bounds</code></li>
  <li><code>media_region</code></li>
  <li><code>media_content</code></li>
</ul>

<p>
The visible body is <code>media_region</code>. The IDE placement/selection aura
is <code>placement_bounds</code>. The default distance between both is the shared
<code>4px</code> aura band used by the recent Numeric, Boolean, Enum, Ring,
Button, and Listbox placement contracts.
</p>

<h2>Runtime Binding</h2>

<p>
The host reads <code>media_ref</code> and <code>media_type</code> from the
<code>.frog</code> widget instance. The SVG template provides semantic geometry;
it does not embed a permanent image payload.
</p>

<pre><code>.frog media_ref
  -> project asset path or accepted data URI
  -> host media endpoint
  -> media_content inside media_region</code></pre>

<h2>Non-Goals</h2>

<ul>
  <li>No dataflow image processing; that remains the Picture widget family.</li>
  <li>No runtime-hardcoded media file.</li>
  <li>No public focus ring, caret, marker, or selection handle part.</li>
</ul>

