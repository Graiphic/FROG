<p align="center">
  <img src="../../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default String Realization Assets</h1>

<p align="center">
  <strong>SVG template assets for the default string realization variant</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="../../String.md">Default string realization posture</a></li>
  <li><a href="../../string.default.wfrog">Machine-readable string realization manifest</a></li>
  <li><a href="../../Package.md">Default realization package posture</a></li>
  <li><a href="../../../../Widgets/String.md">String widget class law</a></li>
</ul>

<hr/>

<h2>1. Purpose</h2>

<p>
This directory contains realization assets for the default string realization family.
</p>

<p>
The assets here do not define the string widget classes.
The string class law belongs to <code>Libraries/Widgets/String.md</code>.
These files are SVG template resources consumed by the default realization manifest.
</p>

<hr/>

<h2>2. Published Assets</h2>

<ul>
  <li><code>templates/string_rectangular.svg</code> — rectangular default realization variant.</li>
</ul>

<hr/>

<h2>3. Required Template Markers</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>frame</code></li>
  <li><code>text_region</code></li>
  <li><code>text_value</code></li>
  <li><code>placeholder</code></li>
</ul>

<p>
The String rectangular template intentionally has no <code>focus_ring</code> marker.
Interactive hover and text-region styling are carried through the manifest bindings and instance <code>.wfrog</code> properties.
</p>
