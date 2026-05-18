<p align="center">
  <img src="../../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Label Realization Assets</h1>

<p align="center">
  <strong>SVG template asset for the default label realization variant</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="../../Label.md">Default label realization posture</a></li>
  <li><a href="../../label.default.wfrog">Machine-readable label realization manifest</a></li>
  <li><a href="../../Package.md">Default realization package posture</a></li>
  <li><a href="../../../../Widgets/Label.md">Label widget class law</a></li>
</ul>

<hr/>

<h2>1. Published Asset</h2>

<ul>
  <li><code>templates/label_rectangular.svg</code> — rectangular default label realization template.</li>
</ul>

<hr/>

<p>
The published rectangular template uses square default corners and scales to
the source-owned front-panel layout rectangle with
<code>preserveAspectRatio="none"</code>. This keeps background and frame parts
aligned to the Label instance bounds while the semantic text remains owned by
<code>.frog</code> <code>text.value</code>. The published
<code>background</code>, <code>frame</code>, and <code>focus_ring</code> parts
span the full template viewBox so the frame is the external Label border.
Stroke-carrying parts use non-scaling strokes so border thickness remains
consistent across horizontal and vertical scaling.
</p>

<p>
The external Label frame has a strict uniform-border rule. A resized Label
must still show the same visible border thickness on every edge. If a runtime
uses a host-rendered frame for pixel fidelity, it must align that frame to the
published <code>data-frog-part="frame"</code> geometry and source it from the
same <code>style.frame.*</code> properties.
</p>

<hr/>

<h2>2. Required Template Markers</h2>

<ul>
  <li><code>root</code></li>
  <li><code>background</code></li>
  <li><code>frame</code></li>
  <li><code>text_surface</code></li>
  <li><code>focus_ring</code></li>
</ul>
