<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization — Frame Widget</h1>

<p align="center">
  <strong>Default realization posture for the standalone frame support widget</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="./Package.md">Default realization package posture</a></li>
  <li><a href="./frame.default.wfrog">Machine-readable frame realization manifest</a></li>
  <li><a href="./assets/frame/Readme.md">Frame default realization assets</a></li>
  <li><a href="../../Widgets/Frame.md">Frame widget class law</a></li>
  <li><a href="../../UI.md">Executable UI primitives</a></li>
</ul>

<hr/>

<h2>1. Overview</h2>

<p>
This document describes the default realization posture for <code>frog.widgets.frame</code>.
</p>

<p>
The class law belongs to <code>Libraries/Widgets/Frame.md</code>.
This realization defines how the Default family can embody a frame through a rectangular SVG-backed or host-native grouping surface while preserving the distinction between the standalone frame class and a widget-local frame part.
</p>

<hr/>

<h2>2. Published Default Variant</h2>

<ul>
  <li><code>rectangular</code></li>
</ul>

<hr/>

<h2>3. Machine-Readable Manifest</h2>

<pre><code>Libraries/Realizations/Default/frame.default.wfrog</code></pre>

<hr/>

<h2>4. Public Parts Realized</h2>

<ul>
  <li><code>root</code></li>
  <li><code>background</code></li>
  <li><code>border</code></li>
  <li><code>caption_surface</code></li>
  <li><code>content_region</code></li>
  <li><code>focus_ring</code></li>
</ul>

<hr/>

<h2>5. SVG Template Asset</h2>

<ul>
  <li><code>assets/frame/templates/frame_rectangular.svg</code></li>
</ul>

<p>
The SVG template provides geometry, caption placement, and content-region guidance.
It does not own grouping semantics or the semantic caption text.
</p>
