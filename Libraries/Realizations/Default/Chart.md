<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization — Waveform Chart Widget</h1>

<p align="center">
  <strong>Default realization posture for the composite waveform chart widget</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="./Package.md">Default realization package posture</a></li>
  <li><a href="./chart.default.wfrog">Machine-readable chart realization manifest</a></li>
  <li><a href="./assets/chart/Readme.md">Chart default realization assets</a></li>
  <li><a href="../../Widgets/Chart.md">Chart widget class law</a></li>
  <li><a href="../../UI.md">Executable UI primitives</a></li>
</ul>

<hr/>

<h2>1. Overview</h2>

<p>
This document describes the default realization posture for <code>frog.widgets.waveform_chart</code>.
</p>

<p>
The default realization exposes a rectangular SVG-backed composite chart template with plot area, plot layer, scales, legends, graph palette, scrollbars, cursor layer, plot-image layers, caption, label, and focus ring.
</p>

<hr/>

<h2>2. Machine-Readable Manifest</h2>

<pre><code>Libraries/Realizations/Default/chart.default.wfrog</code></pre>

<hr/>

<h2>3. Published Default Variant</h2>

<ul>
  <li><code>rectangular</code></li>
</ul>

<hr/>

<h2>4. Public Parts Realized</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>frame</code></li>
  <li><code>plot_area</code></li>
  <li><code>plot_layer</code></li>
  <li><code>grid</code></li>
  <li><code>x_scale</code></li>
  <li><code>y_scale</code></li>
  <li><code>x_scale_label</code></li>
  <li><code>y_scale_label</code></li>
  <li><code>plot_legend</code></li>
  <li><code>scale_legend</code></li>
  <li><code>cursor_legend</code></li>
  <li><code>graph_palette</code></li>
  <li><code>x_scrollbar</code></li>
  <li><code>y_scrollbar</code></li>
  <li><code>cursor_layer</code></li>
  <li><code>plot_image_back</code></li>
  <li><code>plot_image_middle</code></li>
  <li><code>plot_image_front</code></li>
  <li><code>focus_ring</code></li>
</ul>

<hr/>

<h2>5. SVG Template Asset</h2>

<ul>
  <li><code>assets/chart/templates/waveform_chart_rectangular.svg</code></li>
</ul>

<p>
The template is intended to expose stable parts for a runtime or IDE.
It is not the semantic owner of plotted data.
</p>
