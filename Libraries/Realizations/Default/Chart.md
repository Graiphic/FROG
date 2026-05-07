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

<h2>Overview</h2>

<p>
This document describes the default realization posture for <code>frog.widgets.waveform_chart</code>.
The default realization exposes a rectangular SVG-backed composite chart template with plot area, history region, plot layer, grid, scales, scale labels, legends, graph palette, scrollbars, cursor layer, cursor readout, plot-image layers, annotation layer, threshold band, caption, label, and focus ring.
</p>

<p>
The SVG-backed realization is a template and fallback.
A runtime may replace plot rendering with a host-native plot, canvas, GPU surface, or optimized polyline layer while preserving the public part model.
</p>

<hr/>

<h2>Public Parts Realized</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>frame</code></li>
  <li><code>plot_area</code></li>
  <li><code>history_region</code></li>
  <li><code>plot_layer</code></li>
  <li><code>plot_polyline</code></li>
  <li><code>plot_marker</code></li>
  <li><code>grid</code></li>
  <li><code>axis_tick</code></li>
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
  <li><code>cursor_readout</code></li>
  <li><code>plot_image_back</code></li>
  <li><code>plot_image_middle</code></li>
  <li><code>plot_image_front</code></li>
  <li><code>annotation_layer</code></li>
  <li><code>threshold_band</code></li>
  <li><code>focus_ring</code></li>
</ul>

<hr/>

<h2>Published Assets</h2>

<ul>
  <li><code>assets/chart/templates/waveform_chart_shell.svg</code></li>
  <li><code>assets/chart/subobjects/plot_area.svg</code></li>
  <li><code>assets/chart/subobjects/grid.svg</code></li>
  <li><code>assets/chart/subobjects/x_scale.svg</code></li>
  <li><code>assets/chart/subobjects/y_scale.svg</code></li>
  <li><code>assets/chart/subobjects/plot_legend.svg</code></li>
  <li><code>assets/chart/subobjects/scale_legend.svg</code></li>
  <li><code>assets/chart/subobjects/cursor_legend.svg</code></li>
  <li><code>assets/chart/subobjects/graph_palette.svg</code></li>
  <li><code>assets/chart/subobjects/x_scrollbar.svg</code></li>
  <li><code>assets/chart/subobjects/y_scrollbar.svg</code></li>
  <li><code>assets/chart/subobjects/cursor_layer.svg</code></li>
  <li><code>assets/chart/subobjects/plot_image_layer.svg</code></li>
  <li><code>assets/chart/subobjects/axis_tick.svg</code></li>
  <li><code>assets/chart/subobjects/plot_marker.svg</code></li>
  <li><code>assets/chart/subobjects/cursor_readout.svg</code></li>
  <li><code>assets/chart/subobjects/annotation_layer.svg</code></li>
  <li><code>assets/chart/subobjects/threshold_band.svg</code></li>
</ul>

<hr/>

<h2>Rendering Posture</h2>

<ul>
  <li><code>plot_area</code> anchors the visible chart body.</li>
  <li><code>history_region</code> defines the retained visible history window.</li>
  <li><code>plot_layer</code> and <code>plot_polyline</code> render chart samples.</li>
  <li><code>plot_marker</code> renders optional point markers.</li>
  <li><code>grid</code> and <code>axis_tick</code> render scale structure.</li>
  <li><code>cursor_layer</code> and <code>cursor_readout</code> render cursors and cursor values.</li>
  <li><code>annotation_layer</code> and <code>threshold_band</code> render display overlays.</li>
</ul>

<hr/>

<h2>Validation Expectations</h2>

<ul>
  <li>All declared public parts must exist in chart class law.</li>
  <li>All declared SVG resources must resolve relative to this package.</li>
  <li>The declared SVG resources should expose all declared <code>data-frog-part</code> markers.</li>
  <li>Resources must not become semantic owners of chart value, samples, or history.</li>
  <li>Host-native plotting handles, canvas handles, GPU surfaces, and renderer caches must remain realization-private.</li>
</ul>
