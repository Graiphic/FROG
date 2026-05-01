<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Standard Chart Widget</h1>

<p align="center">
  <strong>Composite-object baseline for the standardized waveform chart widget class</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="./Readme.md">Widgets index</a></li>
  <li><a href="../../Expression/Widget.md">Expression widget instances</a></li>
  <li><a href="../../Expression/Widget%20interaction.md">Expression widget interaction</a></li>
  <li><a href="../../Expression/Widget%20realization.md">Expression widget realization</a></li>
  <li><a href="../../Expression/Widget%20package%20(.wfrog).md">Widget package publication</a></li>
  <li><a href="../../Libraries/UI.md">Executable UI primitives</a></li>
  <li><a href="../../Libraries/Realizations/Default/Chart.md">Default chart realization</a></li>
  <li><a href="../../Libraries/Realizations/Default/chart.default.wfrog">Default chart realization manifest</a></li>
</ul>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#class-defined-here">2. Class Defined Here</a></li>
  <li><a href="#design-position">3. Design Position</a></li>
  <li><a href="#chart-class-versus-realization">4. Chart Class versus Realization</a></li>
  <li><a href="#composite-object-model">5. Composite Object Model</a></li>
  <li><a href="#label-caption-and-text-surfaces">6. Label, Caption, and Text Surfaces</a></li>
  <li><a href="#value-and-history-model">7. Value and History Model</a></li>
  <li><a href="#plot-model">8. Plot Model</a></li>
  <li><a href="#scale-model">9. Scale Model</a></li>
  <li><a href="#legend-palette-scrollbar-and-cursor-surfaces">10. Legend, Palette, Scrollbar, and Cursor Surfaces</a></li>
  <li><a href="#plot-image-layers">11. Plot Image Layers</a></li>
  <li><a href="#public-part-model">12. Public Part Model</a></li>
  <li><a href="#standard-property-surface">13. Standard Property Surface</a></li>
  <li><a href="#standard-methods">14. Standard Methods</a></li>
  <li><a href="#standard-events">15. Standard Events</a></li>
  <li><a href="#default-realization-posture">16. Default Realization Posture</a></li>
  <li><a href="#diagram-interaction-posture">17. Diagram Interaction Posture</a></li>
  <li><a href="#core-boundary-and-non-goals">18. Core Boundary and Non-Goals</a></li>
  <li><a href="#validation-expectations">19. Validation Expectations</a></li>
  <li><a href="#summary">20. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the standardized baseline for <code>frog.widgets.waveform_chart</code>.
</p>

<p>
A waveform chart is a composite front-panel object.
It is not just a rectangle containing a curve.
It contains public sub-objects such as plot area, plots, scales, legends, palette, scrollbars, optional cursors, and plot-image layers.
</p>

<p>
The purpose of this document is to define a serious but bounded FROG chart baseline inspired by mature graphical environments while preserving the FROG architecture:
class law is not realization, realization is not SVG, SVG is not runtime, and runtime is not the language.
</p>

<hr/>

<h2 id="class-defined-here">2. Class Defined Here</h2>

<ul>
  <li><code>frog.widgets.waveform_chart</code></li>
</ul>

<p>
This class is indicator-oriented.
It displays numeric samples and retained history.
It is not a user-editable graphing canvas in the intrinsic baseline.
</p>

<hr/>

<h2 id="design-position">3. Design Position</h2>

<p>
The core FROG chart is a waveform chart, not a full graphing ecosystem.
</p>

<p>
The chart baseline includes enough public sub-objects to be useful:
</p>

<ul>
  <li>history and value update posture,</li>
  <li>plot area,</li>
  <li>plots,</li>
  <li>X and Y scales,</li>
  <li>plot legend, scale legend, and cursor legend visibility posture,</li>
  <li>graph palette posture,</li>
  <li>scrollbar posture,</li>
  <li>a small optional cursor surface,</li>
  <li>plot-image layers for realization-side overlays.</li>
</ul>

<p>
The chart baseline does not standardize the full family of waveform graphs, XY graphs, intensity graphs, mixed-signal graphs, annotations, advanced cursor snapping, full plugin systems, or advanced data export semantics.
</p>

<hr/>

<h2 id="chart-class-versus-realization">4. Chart Class versus Realization</h2>

<p>
The chart class owns:
</p>

<ul>
  <li>the value and history posture,</li>
  <li>the public sub-object model,</li>
  <li>the public property inventory,</li>
  <li>the public method inventory,</li>
  <li>the public event inventory,</li>
  <li>the public part model.</li>
</ul>

<p>
The realization owns:
</p>

<ul>
  <li>plot drawing implementation,</li>
  <li>axis layout and tick rendering,</li>
  <li>legend layout,</li>
  <li>palette layout,</li>
  <li>scrollbar embodiment,</li>
  <li>cursor drawing,</li>
  <li>plot-image layer drawing,</li>
  <li>SVG, canvas, host-native, or mixed rendering.</li>
</ul>

<hr/>

<h2 id="composite-object-model">5. Composite Object Model</h2>

<p>
The waveform chart is a composite standard widget.
Its public object model is organized as follows:
</p>

<pre><code>frog.widgets.waveform_chart
├── value
├── history.*
├── plot_area.*
├── plots.*
├── scales.x.*
├── scales.y.*
├── legends.plot.*
├── legends.scale.*
├── legends.cursor.*
├── palette.*
├── scrollbars.x.*
├── scrollbars.y.*
├── cursors.*
├── plot_images.*
├── interaction.*
├── style.*
└── realization.*
</code></pre>

<p>
This structure gives the chart a property-node and method-node posture similar in spirit to mature graphical systems while preserving a bounded FROG core.
</p>

<hr/>

<h2 id="label-caption-and-text-surfaces">6. Label, Caption, and Text Surfaces</h2>

<p>
The waveform chart follows the shared FROG label/caption convention and adds scale labels.
</p>

<pre><code>label.*
    - structural / logical widget name surface

caption.*
    - visible chart title or front-panel caption

scales.x[].label.*
scales.y[].label.*
    - visible scale-label surfaces
</code></pre>

<p>
The chart's logical identity, visible title, and scale labels are separate surfaces.
A realization may place these surfaces visually, but it does not own their semantics.
</p>

<hr/>

<h2 id="value-and-history-model">7. Value and History Model</h2>

<ul>
  <li><code>value</code> — current chart-compatible value payload.</li>
  <li><code>history.data</code> — retained visible history when exposed.</li>
  <li><code>history.capacity</code> — maximum retained visible samples.</li>
  <li><code>history.length</code> — current retained sample count.</li>
  <li><code>history.update_mode</code> — <code>append</code>, <code>replace_history</code>, or <code>rolling_window</code>.</li>
</ul>

<p>
A FROG chart SHOULD provide <code>clear_history()</code> as an explicit method rather than relying on indirect assignment patterns.
</p>

<hr/>

<h2 id="plot-model">8. Plot Model</h2>

<ul>
  <li><code>plots.count</code></li>
  <li><code>plots.active_index</code></li>
  <li><code>plots[].name</code></li>
  <li><code>plots[].visible</code></li>
  <li><code>plots[].color</code></li>
  <li><code>plots[].line_style</code></li>
  <li><code>plots[].line_width</code></li>
  <li><code>plots[].point_style</code></li>
  <li><code>plots[].interpolation</code></li>
  <li><code>plots[].fill_to</code></li>
  <li><code>plots[].x_scale_index</code></li>
  <li><code>plots[].y_scale_index</code></li>
</ul>

<p>
The core does not standardize full per-point styling, hit testing, marker collections, or arbitrary plot renderers.
</p>

<hr/>

<h2 id="scale-model">9. Scale Model</h2>

<p>
The chart exposes X and Y scale collections rather than a single flat axis field.
</p>

<ul>
  <li><code>scales.x.active_index</code></li>
  <li><code>scales.y.active_index</code></li>
  <li><code>scales.x[].name</code></li>
  <li><code>scales.y[].name</code></li>
  <li><code>scales.x[].visible</code></li>
  <li><code>scales.y[].visible</code></li>
  <li><code>scales.x[].label.visible</code></li>
  <li><code>scales.y[].label.visible</code></li>
  <li><code>scales.x[].label.text</code></li>
  <li><code>scales.y[].label.text</code></li>
  <li><code>scales.x[].autoscale</code></li>
  <li><code>scales.y[].autoscale</code></li>
  <li><code>scales.x[].minimum</code></li>
  <li><code>scales.x[].maximum</code></li>
  <li><code>scales.y[].minimum</code></li>
  <li><code>scales.y[].maximum</code></li>
  <li><code>scales.x[].log</code></li>
  <li><code>scales.y[].log</code></li>
  <li><code>scales.x[].inverted</code></li>
  <li><code>scales.y[].inverted</code></li>
  <li><code>scales.x[].offset</code></li>
  <li><code>scales.y[].offset</code></li>
  <li><code>scales.x[].multiplier</code></li>
  <li><code>scales.y[].multiplier</code></li>
  <li><code>scales.x[].grid.major.visible</code></li>
  <li><code>scales.y[].grid.major.visible</code></li>
  <li><code>scales.x[].grid.minor.visible</code></li>
  <li><code>scales.y[].grid.minor.visible</code></li>
  <li><code>scales.x[].grid.major.color</code></li>
  <li><code>scales.y[].grid.major.color</code></li>
  <li><code>scales.x[].grid.minor.color</code></li>
  <li><code>scales.y[].grid.minor.color</code></li>
</ul>

<hr/>

<h2 id="legend-palette-scrollbar-and-cursor-surfaces">10. Legend, Palette, Scrollbar, and Cursor Surfaces</h2>

<h3>10.1 Legends</h3>

<ul>
  <li><code>legends.plot.visible</code></li>
  <li><code>legends.plot.autosize</code></li>
  <li><code>legends.plot.position</code></li>
  <li><code>legends.plot.rows</code></li>
  <li><code>legends.scale.visible</code></li>
  <li><code>legends.scale.position</code></li>
  <li><code>legends.cursor.visible</code></li>
  <li><code>legends.cursor.position</code></li>
</ul>

<h3>10.2 Graph palette</h3>

<ul>
  <li><code>palette.visible</code></li>
  <li><code>palette.zoom_enabled</code></li>
  <li><code>palette.pan_enabled</code></li>
  <li><code>palette.selection_enabled</code></li>
</ul>

<h3>10.3 Scrollbars</h3>

<ul>
  <li><code>scrollbars.x.visible</code></li>
  <li><code>scrollbars.y.visible</code></li>
  <li><code>scrollbars.x.position</code></li>
  <li><code>scrollbars.y.position</code></li>
</ul>

<h3>10.4 Cursors</h3>

<p>
Cursors are included as an optional core-light surface because they are structurally important in graphical measurement environments.
They remain smaller than a full advanced cursor framework.
</p>

<ul>
  <li><code>cursors.count</code></li>
  <li><code>cursors.active_index</code></li>
  <li><code>cursors[].name</code></li>
  <li><code>cursors[].visible</code></li>
  <li><code>cursors[].color</code></li>
  <li><code>cursors[].plot_index</code></li>
  <li><code>cursors[].position.x</code></li>
  <li><code>cursors[].position.y</code></li>
  <li><code>cursors[].line_style</code></li>
  <li><code>cursors[].line_width</code></li>
  <li><code>cursors[].point_style</code></li>
  <li><code>cursors[].dragging_mode</code></li>
</ul>

<hr/>

<h2 id="plot-image-layers">11. Plot Image Layers</h2>

<ul>
  <li><code>plot_images.back</code></li>
  <li><code>plot_images.middle</code></li>
  <li><code>plot_images.front</code></li>
</ul>

<p>
These layers are drawing overlays.
They are not chart data and do not redefine plot semantics.
</p>

<hr/>

<h2 id="public-part-model">12. Public Part Model</h2>

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

<h2 id="standard-property-surface">13. Standard Property Surface</h2>

<p>
The standard property surface is the union of the sub-object members defined above.
Object-style access through <code>frog.ui.property_read</code> and <code>frog.ui.property_write</code> should address these members by their full hierarchical names.
</p>

<hr/>

<h2 id="standard-methods">14. Standard Methods</h2>

<ul>
  <li><code>append_sample(sample)</code></li>
  <li><code>append_samples(samples)</code></li>
  <li><code>replace_history(samples)</code></li>
  <li><code>clear_history()</code></li>
  <li><code>set_history_capacity(capacity)</code></li>
  <li><code>autoscale_x()</code></li>
  <li><code>autoscale_y()</code></li>
  <li><code>autoscale_all()</code></li>
  <li><code>set_active_plot(index)</code></li>
  <li><code>set_active_x_scale(index)</code></li>
  <li><code>set_active_y_scale(index)</code></li>
  <li><code>add_cursor(cursor)</code></li>
  <li><code>remove_cursor(index)</code></li>
  <li><code>clear_cursors()</code></li>
  <li><code>reset_to_default_style()</code></li>
  <li><code>focus()</code></li>
</ul>

<hr/>

<h2 id="standard-events">15. Standard Events</h2>

<ul>
  <li><code>value_rendered</code></li>
  <li><code>sample_appended</code></li>
  <li><code>history_cleared</code></li>
  <li><code>history_capacity_changed</code></li>
  <li><code>axis_range_changed</code></li>
  <li><code>plot_visibility_changed</code></li>
  <li><code>cursor_added</code></li>
  <li><code>cursor_removed</code></li>
  <li><code>cursor_moved</code> when cursor posture is supported</li>
  <li><code>focus_gained</code></li>
  <li><code>focus_lost</code></li>
</ul>

<hr/>

<h2 id="default-realization-posture">16. Default Realization Posture</h2>

<p>
The Default realization uses a subobject composition posture.
A top-level shell defines slots, and separate SVG resources define subobjects such as plot area, grid, scales, legends, graph palette, scrollbars, cursor layer, and plot-image layers.
</p>

<p>
The SVG assets do not own chart data.
A runtime or IDE injects plotted samples into <code>plot_layer</code> and cursor positions into <code>cursor_layer</code>.
</p>

<hr/>

<h2 id="diagram-interaction-posture">17. Diagram Interaction Posture</h2>

<p>
The waveform chart supports natural value participation through <code>widget_value</code>, property access through <code>frog.ui.property_read</code> and <code>frog.ui.property_write</code>, method invocation through <code>frog.ui.method_invoke</code>, and event observation where legal.
</p>

<hr/>

<h2 id="core-boundary-and-non-goals">18. Core Boundary and Non-Goals</h2>

<ul>
  <li>annotation collections,</li>
  <li>advanced cursor snapping,</li>
  <li>advanced point hit-testing,</li>
  <li>full graph plugin systems,</li>
  <li>XY graph semantics,</li>
  <li>intensity graph semantics,</li>
  <li>mixed-signal graph semantics,</li>
  <li>one mandatory rendering engine.</li>
</ul>

<hr/>

<h2 id="validation-expectations">19. Validation Expectations</h2>

<p>
Validators SHOULD diagnose unsupported chart members, invalid history capacity, invalid active plot index, invalid active scale index, invalid cursor index, incompatible value payloads, invalid scale ranges, and attempts to treat realization-only drawing internals as public class members.
</p>

<hr/>

<h2 id="summary">20. Summary</h2>

<p>
<code>frog.widgets.waveform_chart</code> is a composite standard widget.
It is richer than a simple SVG chart but deliberately narrower than a full graphing ecosystem.
</p>

<p>
It standardizes a serious core for plotted history, plots, scales, legends, palette, scrollbars, optional cursors, and plot-image layers while preserving the boundary between class law, realization, SVG assets, runtime, and IDE tooling.
</p>
