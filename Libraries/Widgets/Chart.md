<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Standard Waveform Chart Widget</h1>

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

<h2>Classes Defined Here</h2>

<ul>
  <li><code>frog.widgets.waveform_chart</code></li>
</ul>

<hr/>

<h2>Overview</h2>

<p>
The Waveform Chart family defines the standardized indicator-oriented chart widget used for time-ordered numeric samples, retained history, multiple plots, axes, scale labels, legends, cursors, scrollbars, palette tools, and lightweight plot-image overlays.
</p>

<p>
A waveform chart is a composite front-panel object.
It is not merely a rectangle containing a curve, and it is not a full graphing framework.
It is the portable FROG baseline for streaming or appended numeric history visualization.
</p>

<p>
The intrinsic baseline is intentionally narrower than a full graph family.
XY graphs, intensity graphs, mixed-signal graphs, polar plots, histograms, annotations frameworks, plugin renderers, and domain-specific plotting systems belong to later classes, profiles, or host integrations.
</p>

<hr/>

<h2>Class Identity</h2>

<ul>
  <li><strong>class_id:</strong> <code>frog.widgets.waveform_chart</code></li>
  <li><strong>family:</strong> <code>waveform_chart_widget</code></li>
  <li><strong>compatible role:</strong> <code>indicator</code></li>
</ul>

<hr/>

<h2>Primary Value Posture</h2>

<ul>
  <li>primary value: present</li>
  <li>value type: <code>frog.waveform_chart_value</code></li>
  <li>natural value participation: yes</li>
  <li>user-mutable: no for plotted data in the standard portable posture</li>
  <li>diagram-mutable: yes</li>
  <li>mirrored property: <code>value</code></li>
</ul>

<p>
A waveform chart may allow user-originated viewport, cursor, palette, and scrollbar changes.
Those interactions do not mutate the retained sample history unless an explicit method or data operation does so.
</p>

<hr/>

<h2>Chart Class versus Realization</h2>

<p>
The chart class owns the public value model, history model, plot model, scale model, legend/cursor/palette/scrollbar surfaces, methods, events, and public parts.
</p>

<p>
The realization owns the drawing strategy: SVG template, host-native plot surface, canvas, GPU surface, polyline rasterization, tick rendering, legend layout, palette layout, cursor drawing, plot-image layer composition, and host-specific optimization.
</p>

<hr/>

<h2>Composite Object Model</h2>

<pre><code>frog.widgets.waveform_chart
├── value
├── history.*
├── samples.*
├── timebase.*
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
├── annotations.*
├── thresholds.*
├── interaction.*
├── style.*
└── realization.*
</code></pre>

<p>
The sub-object model gives the chart a property-node and method-node posture similar in spirit to mature graphical systems, while keeping the portable core bounded.
</p>

<hr/>

<h2>Value, Samples, and Timebase Model</h2>

<h3>Value payload kinds</h3>

<p>
The portable chart value may carry one of these payload postures:
</p>

<ul>
  <li><code>single_sample</code> — one numeric value appended to the active plot,</li>
  <li><code>sample_array</code> — multiple samples appended to the active plot,</li>
  <li><code>multi_plot_sample</code> — one sample per plot,</li>
  <li><code>multi_plot_arrays</code> — multiple samples per plot,</li>
  <li><code>waveform</code> — numeric samples plus timing metadata,</li>
  <li><code>history_replace</code> — explicit history replacement payload.</li>
</ul>

<h3>Sample model</h3>

<ul>
  <li><code>samples.value_type</code> — <code>i32</code>, <code>u32</code>, <code>f32</code>, <code>f64</code>, or profile-supported numeric type</li>
  <li><code>samples.nan_policy</code> — <code>preserve</code>, <code>gap</code>, or <code>reject</code></li>
  <li><code>samples.timestamp_policy</code> — <code>implicit_index</code>, <code>dt_t0</code>, or <code>explicit_x</code></li>
  <li><code>samples.order</code> — <code>append_order</code> for the intrinsic baseline</li>
</ul>

<h3>Timebase model</h3>

<ul>
  <li><code>timebase.mode</code> — <code>sample_index</code>, <code>relative_time</code>, or <code>absolute_time</code></li>
  <li><code>timebase.t0</code></li>
  <li><code>timebase.dt</code></li>
  <li><code>timebase.unit</code></li>
  <li><code>timebase.sample_rate</code></li>
</ul>

<hr/>

<h2>History Model</h2>

<ul>
  <li><code>history.data</code></li>
  <li><code>history.capacity</code></li>
  <li><code>history.length</code></li>
  <li><code>history.update_mode</code> — <code>append</code>, <code>replace_history</code>, or <code>rolling_window</code></li>
  <li><code>history.retention_policy</code> — <code>drop_oldest</code>, <code>reject_when_full</code>, or <code>grow_until_capacity</code></li>
  <li><code>history.clear_on_start</code></li>
  <li><code>history.visible_window</code></li>
</ul>

<p>
Retained history is chart-owned display state.
It is not an unbounded data logger.
Data logging belongs to another library or runtime integration.
</p>

<hr/>

<h2>Plot Model</h2>

<ul>
  <li><code>plots.count</code></li>
  <li><code>plots.active_index</code></li>
  <li><code>plots[].id</code></li>
  <li><code>plots[].name</code></li>
  <li><code>plots[].visible</code></li>
  <li><code>plots[].color</code></li>
  <li><code>plots[].line_style</code></li>
  <li><code>plots[].line_width</code></li>
  <li><code>plots[].point_style</code></li>
  <li><code>plots[].point_size</code></li>
  <li><code>plots[].interpolation</code> — <code>none</code>, <code>linear</code>, <code>step_left</code>, or <code>step_right</code></li>
  <li><code>plots[].fill_to</code></li>
  <li><code>plots[].x_scale_index</code></li>
  <li><code>plots[].y_scale_index</code></li>
</ul>

<hr/>

<h2>Scale and Grid Model</h2>

<h3>X scales</h3>

<ul>
  <li><code>scales.x.active_index</code></li>
  <li><code>scales.x[].name</code></li>
  <li><code>scales.x[].visible</code></li>
  <li><code>scales.x[].label.text</code></li>
  <li><code>scales.x[].label.visible</code></li>
  <li><code>scales.x[].autoscale</code></li>
  <li><code>scales.x[].minimum</code></li>
  <li><code>scales.x[].maximum</code></li>
  <li><code>scales.x[].log</code></li>
  <li><code>scales.x[].inverted</code></li>
  <li><code>scales.x[].offset</code></li>
  <li><code>scales.x[].multiplier</code></li>
  <li><code>scales.x[].tick.major.visible</code></li>
  <li><code>scales.x[].tick.minor.visible</code></li>
  <li><code>scales.x[].grid.major.visible</code></li>
  <li><code>scales.x[].grid.minor.visible</code></li>
</ul>

<h3>Y scales</h3>

<ul>
  <li><code>scales.y.active_index</code></li>
  <li><code>scales.y[].name</code></li>
  <li><code>scales.y[].visible</code></li>
  <li><code>scales.y[].label.text</code></li>
  <li><code>scales.y[].label.visible</code></li>
  <li><code>scales.y[].autoscale</code></li>
  <li><code>scales.y[].minimum</code></li>
  <li><code>scales.y[].maximum</code></li>
  <li><code>scales.y[].log</code></li>
  <li><code>scales.y[].inverted</code></li>
  <li><code>scales.y[].offset</code></li>
  <li><code>scales.y[].multiplier</code></li>
  <li><code>scales.y[].tick.major.visible</code></li>
  <li><code>scales.y[].tick.minor.visible</code></li>
  <li><code>scales.y[].grid.major.visible</code></li>
  <li><code>scales.y[].grid.minor.visible</code></li>
</ul>

<hr/>

<h2>Cursor, Legend, Palette, and Scrollbar Model</h2>

<h3>Cursors</h3>

<ul>
  <li><code>cursors.count</code></li>
  <li><code>cursors.active_index</code></li>
  <li><code>cursors[].id</code></li>
  <li><code>cursors[].name</code></li>
  <li><code>cursors[].visible</code></li>
  <li><code>cursors[].color</code></li>
  <li><code>cursors[].plot_index</code></li>
  <li><code>cursors[].position.x</code></li>
  <li><code>cursors[].position.y</code></li>
  <li><code>cursors[].snap_mode</code> — <code>free</code>, <code>nearest_sample</code>, or profile-supported mode</li>
  <li><code>cursors[].dragging_mode</code></li>
</ul>

<h3>Legends</h3>

<ul>
  <li><code>legends.plot.visible</code></li>
  <li><code>legends.plot.position</code></li>
  <li><code>legends.scale.visible</code></li>
  <li><code>legends.scale.position</code></li>
  <li><code>legends.cursor.visible</code></li>
  <li><code>legends.cursor.position</code></li>
</ul>

<h3>Palette</h3>

<ul>
  <li><code>palette.visible</code></li>
  <li><code>palette.zoom_enabled</code></li>
  <li><code>palette.pan_enabled</code></li>
  <li><code>palette.selection_enabled</code></li>
  <li><code>palette.autoscale_enabled</code></li>
</ul>

<h3>Scrollbars</h3>

<ul>
  <li><code>scrollbars.x.visible</code></li>
  <li><code>scrollbars.y.visible</code></li>
  <li><code>scrollbars.x.position</code></li>
  <li><code>scrollbars.y.position</code></li>
  <li><code>scrollbars.x.value</code></li>
  <li><code>scrollbars.y.value</code></li>
</ul>

<hr/>

<h2>Annotations, Thresholds, and Plot Image Layers</h2>

<ul>
  <li><code>annotations.visible</code></li>
  <li><code>annotations.items</code></li>
  <li><code>thresholds.visible</code></li>
  <li><code>thresholds.items</code></li>
  <li><code>plot_images.back</code></li>
  <li><code>plot_images.middle</code></li>
  <li><code>plot_images.front</code></li>
</ul>

<p>
Annotations and thresholds are display overlays in the intrinsic baseline.
They do not mutate sample history or plot semantics.
</p>

<hr/>

<h2>Public Part Model</h2>

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

<h2>Standard Methods</h2>

<ul>
  <li><code>append_sample(sample)</code></li>
  <li><code>append_samples(samples)</code></li>
  <li><code>append_waveform(waveform)</code></li>
  <li><code>replace_history(samples)</code></li>
  <li><code>clear_history()</code></li>
  <li><code>set_history_capacity(capacity)</code></li>
  <li><code>autoscale_x()</code></li>
  <li><code>autoscale_y()</code></li>
  <li><code>autoscale_all()</code></li>
  <li><code>set_x_range(minimum, maximum)</code></li>
  <li><code>set_y_range(minimum, maximum)</code></li>
  <li><code>set_active_plot(index)</code></li>
  <li><code>set_active_x_scale(index)</code></li>
  <li><code>set_active_y_scale(index)</code></li>
  <li><code>add_cursor(cursor)</code></li>
  <li><code>remove_cursor(index)</code></li>
  <li><code>move_cursor(index, position)</code></li>
  <li><code>clear_cursors()</code></li>
  <li><code>add_threshold(threshold)</code></li>
  <li><code>clear_thresholds()</code></li>
  <li><code>reset_to_default_style()</code></li>
  <li><code>export_view()</code> when supported</li>
  <li><code>focus()</code></li>
</ul>

<hr/>

<h2>Standard Events</h2>

<ul>
  <li><code>value_rendered</code></li>
  <li><code>sample_appended</code></li>
  <li><code>waveform_appended</code></li>
  <li><code>history_replaced</code></li>
  <li><code>history_cleared</code></li>
  <li><code>history_capacity_changed</code></li>
  <li><code>axis_range_changed</code></li>
  <li><code>plot_visibility_changed</code></li>
  <li><code>cursor_added</code></li>
  <li><code>cursor_removed</code></li>
  <li><code>cursor_moved</code></li>
  <li><code>threshold_changed</code></li>
  <li><code>palette_action</code></li>
  <li><code>viewport_changed</code></li>
  <li><code>focus_gained</code></li>
  <li><code>focus_lost</code></li>
</ul>

<hr/>

<h2>Diagram Interaction Posture</h2>

<p>
The waveform chart supports natural value participation through <code>widget_value</code>, property access through <code>frog.ui.property_read</code> and <code>frog.ui.property_write</code>, method invocation through <code>frog.ui.method_invoke</code>, and event observation where legal.
</p>

<p>
Ordinary streaming updates should prefer <code>widget_value</code> or explicit append methods.
Object-style access should be used for plots, scales, legends, palette, cursor, threshold, history, and viewport configuration.
</p>

<hr/>

<h2>Validation Expectations</h2>

<p>
Validators SHOULD diagnose at least:
</p>

<ul>
  <li>non-numeric sample payloads,</li>
  <li>invalid history capacity,</li>
  <li>history length exceeding capacity,</li>
  <li>invalid plot index,</li>
  <li>invalid active scale index,</li>
  <li>invalid cursor index,</li>
  <li>invalid axis range,</li>
  <li>invalid logarithmic scale ranges,</li>
  <li>incompatible multi-plot sample shapes,</li>
  <li>unsupported cursor snap modes,</li>
  <li>unsupported threshold specifications,</li>
  <li>attempts to treat realization-only drawing internals, canvas handles, GPU surfaces, or plot renderer handles as public class members.</li>
</ul>

<hr/>

<h2>Summary</h2>

<p>
<code>frog.widgets.waveform_chart</code> is the standard FROG baseline for time-ordered numeric history visualization.
It covers streaming sample updates, retained history, multiple plots, axes, scale labels, legends, scrollbars, palette posture, optional cursors, annotations, thresholds, and plot-image layers while preserving the boundary between class law, realization, runtime, and host-native rendering.
</p>
