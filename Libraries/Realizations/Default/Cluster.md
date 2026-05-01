<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization — Cluster Widget</h1>

<p align="center">
  <strong>Default realization posture for the composite cluster widget</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="./Package.md">Default realization package posture</a></li>
  <li><a href="./cluster.default.wfrog">Machine-readable cluster realization manifest</a></li>
  <li><a href="./assets/cluster/Readme.md">Cluster default realization assets</a></li>
  <li><a href="../../Widgets/Cluster.md">Cluster widget class law</a></li>
  <li><a href="../../UI.md">Executable UI primitives</a></li>
</ul>

<hr/>

<h2>1. Overview</h2>

<p>
This document describes the default realization posture for <code>frog.widgets.cluster</code>.
</p>

<p>
The default cluster realization exposes a shell with slots for caption, content region, field slots, field labels, and frame.
It does not define the semantics of the child field widget classes.
</p>

<hr/>

<h2>2. Published Default Variant</h2>

<ul>
  <li><code>rectangular_group</code></li>
</ul>

<hr/>

<h2>3. Machine-Readable Manifest</h2>

<pre><code>Libraries/Realizations/Default/cluster.default.wfrog</code></pre>

<hr/>

<h2>4. Public Parts Realized</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>frame</code></li>
  <li><code>content_region</code></li>
  <li><code>field_slot</code></li>
  <li><code>field_label</code></li>
  <li><code>field_gap</code></li>
  <li><code>focus_ring</code></li>
</ul>

<hr/>

<h2>5. SVG Assets</h2>

<ul>
  <li><code>assets/cluster/templates/cluster_shell.svg</code></li>
  <li><code>assets/cluster/subobjects/content_region.svg</code></li>
  <li><code>assets/cluster/subobjects/field_slot.svg</code></li>
  <li><code>assets/cluster/subobjects/field_label.svg</code></li>
</ul>
