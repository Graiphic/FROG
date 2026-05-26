<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Standard Array Widget</h1>

<p align="center">
  <strong>Composite-object baseline for the standardized array widget class</strong><br/>
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
  <li><a href="../../Libraries/Realizations/Default/Array.md">Default array realization</a></li>
  <li><a href="../../Libraries/Realizations/Default/array.default.wfrog">Default array realization manifest</a></li>
</ul>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#class-defined-here">2. Class Defined Here</a></li>
  <li><a href="#array-class-versus-realization">3. Array Class versus Realization</a></li>
  <li><a href="#generic-element-model">4. Generic Element Model</a></li>
  <li><a href="#value-shape-and-dimension-model">5. Value, Shape, and Dimension Model</a></li>
  <li><a href="#label-caption-and-view-surfaces">6. Label, Caption, and View Surfaces</a></li>
  <li><a href="#public-part-model">7. Public Part Model</a></li>
  <li><a href="#standard-property-surface">8. Standard Property Surface</a></li>
  <li><a href="#standard-methods">9. Standard Methods</a></li>
  <li><a href="#standard-events">10. Standard Events</a></li>
  <li><a href="#default-realization-posture">11. Default Realization Posture</a></li>
  <li><a href="#diagram-interaction-posture">12. Diagram Interaction Posture</a></li>
  <li><a href="#validation-expectations">13. Validation Expectations</a></li>
  <li><a href="#summary">14. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines <code>frog.widgets.array</code>, the standardized array widget class of FROG.
</p>

<p>
An array widget is a composite collection widget.
It displays and, when legal, edits an ordered homogeneous collection of elements.
The element may be numeric, boolean, string, enum, path, cluster, or another compatible widget-backed element posture.
</p>

<p>
The array is therefore not a numeric-array widget.
A numeric array is only one specialization created by binding the array element model to a numeric element class.
</p>

<hr/>

<h2 id="class-defined-here">2. Class Defined Here</h2>

<ul>
  <li><code>frog.widgets.array</code></li>
  <li><code>frog.widgets.array_control</code></li>
  <li><code>frog.widgets.array_indicator</code></li>
</ul>

<p>
<code>frog.widgets.array</code> is the abstract structured collection class.
<code>frog.widgets.array_control</code> is the interactive front-panel form
that may edit element values, selection, index displays, and viewport state
when enabled. <code>frog.widgets.array_indicator</code> is the read-only
front-panel form used to display collection values published by diagram flow.
</p>

<hr/>

<h2 id="array-class-versus-realization">3. Array Class versus Realization</h2>

<p>
The array class owns collection value semantics, element type binding, rank and shape posture, visible index and viewport posture, array-level properties, methods, events, and public composite parts.
</p>

<p>
The realization owns cell layout, viewport drawing, scrollbar embodiment, index-display placement, element-slot placement, spacing, element-gap visualization, SVG composition, and host-native repeated-element rendering.
</p>

<p>
The element widget class owns the behavior of the individual element.
The array owns the repeated collection structure around that element.
</p>

<hr/>

<h2 id="generic-element-model">4. Generic Element Model</h2>

<ul>
  <li><code>element.class_id</code> — widget class used as the array element template when the array is UI-realized.</li>
  <li><code>element.value_type</code> — semantic value type of each element.</li>
  <li><code>element.role</code> — <code>control</code>, <code>indicator</code>, or <code>mixed</code>.</li>
  <li><code>element.template_ref</code> — optional reference to the published element template or element widget instance model.</li>
  <li><code>element.default_value</code> — value used when new elements are inserted or appended.</li>
</ul>

<p>
A conforming array must not make the visible repeated cells the only source of collection truth.
The collection value remains the class-owned semantic value.
</p>

<hr/>

<h2 id="value-shape-and-dimension-model">5. Value, Shape, and Dimension Model</h2>

<ul>
  <li><code>value</code> — collection value.</li>
  <li><code>length</code> — number of elements in a one-dimensional posture.</li>
  <li><code>dimensions.rank</code> — number of dimensions.</li>
  <li><code>dimensions.shape[]</code> — per-dimension size.</li>
  <li><code>dimensions.allow_resize</code> — whether runtime/user resizing is legal.</li>
  <li><code>dimensions.index_base</code> — normally <code>0</code>.</li>
</ul>

<p>
The baseline supports the array structure required to represent multiple dimensions.
The Default realization remains intentionally simple: a one-dimensional array is
displayed as a single visible axis, a two-dimensional array is displayed as a
matrix viewport, and a three-dimensional array is displayed as one two-dimensional
layer at a time through a layer index display plus row/column index displays.
Higher-dimensional arrays require host-specific navigation or later specialized
realization profiles.
</p>

<p>
For numeric arrays, editing a value outside the current shape materializes the
regular rectangular shape required to include that value. Newly created numeric
cells are initialized to <code>0</code>. This keeps one-dimensional vectors,
two-dimensional matrices, and three-dimensional <code>[layer,row,column]</code>
volumes regular instead of creating sparse or ragged collection shapes.
</p>

<hr/>

<h2 id="label-caption-and-view-surfaces">6. Label, Caption, and View Surfaces</h2>

<pre><code>label.*
    - structural / logical widget name surface

caption.*
    - user-facing front-panel presentation surface

index_display
    - visible current index or first visible index surface

element_region
    - visible repeated element viewport
</code></pre>

<hr/>

<h2 id="public-part-model">7. Public Part Model</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>frame</code></li>
  <li><code>index_display</code></li>
  <li><code>dimension_controls</code></li>
  <li><code>element_region</code></li>
  <li><code>element_slot</code></li>
  <li><code>element_gap</code></li>
  <li><code>horizontal_scrollbar</code></li>
  <li><code>vertical_scrollbar</code></li>
  <li><code>focus_ring</code></li>
</ul>

<hr/>

<h2 id="standard-property-surface">8. Standard Property Surface</h2>

<h3>8.1 Value, element, and dimensions</h3>

<ul>
  <li><code>value</code></li>
  <li><code>length</code></li>
  <li><code>element.class_id</code></li>
  <li><code>element.value_type</code></li>
  <li><code>element.role</code></li>
  <li><code>element.template_ref</code></li>
  <li><code>element.default_value</code></li>
  <li><code>dimensions.rank</code></li>
  <li><code>dimensions.shape[]</code></li>
  <li><code>dimensions.index_base</code></li>
  <li><code>dimensions.allow_resize</code></li>
</ul>

<h3>8.2 Viewport and scrolling</h3>

<ul>
  <li><code>viewport.first_indices[]</code></li>
  <li><code>viewport.visible_counts[]</code></li>
  <li><code>viewport.orientation</code> — <code>vertical</code>, <code>horizontal</code>, or <code>grid</code>.</li>
  <li><code>viewport.selection_start[]</code></li>
  <li><code>viewport.selection_size[]</code></li>
  <li><code>viewport.scrollbar_gap</code> — source-owned distance between the element viewport and any visible scrollbar surfaces.</li>
  <li><code>scrollbars.vertical.visible</code></li>
  <li><code>scrollbars.horizontal.visible</code></li>
  <li><code>scrollbars.vertical.position</code></li>
  <li><code>scrollbars.horizontal.position</code></li>
</ul>

<h3>8.3 Appearance and layout</h3>

<ul>
  <li><code>label.visible</code></li>
  <li><code>label.text</code></li>
  <li><code>caption.visible</code></li>
  <li><code>caption.text</code></li>
  <li><code>index_display.visible</code></li>
  <li><code>index_display.values[]</code></li>
  <li><code>element_gap.size</code></li>
  <li><code>layout.cell_width</code></li>
  <li><code>layout.cell_height</code></li>
  <li><code>layout.row_gap</code></li>
  <li><code>layout.column_gap</code></li>
</ul>

<h3>8.4 Interaction and style</h3>

<ul>
  <li><code>interaction.visible</code></li>
  <li><code>interaction.enabled</code></li>
  <li><code>interaction.read_only</code></li>
  <li><code>interaction.focused</code></li>
  <li><code>style.frame.*</code></li>
  <li><code>style.element_region.*</code></li>
  <li><code>style.index_display.*</code></li>
  <li><code>style.scrollbar.*</code></li>
  <li><code>style.focus_ring.*</code></li>
  <li><code>realization.family</code></li>
  <li><code>realization.variant</code></li>
  <li><code>realization.skin_id</code></li>
</ul>

<hr/>

<h2 id="standard-methods">9. Standard Methods</h2>

<ul>
  <li><code>append(value)</code></li>
  <li><code>insert(index, value)</code></li>
  <li><code>remove(index)</code></li>
  <li><code>clear()</code></li>
  <li><code>resize(shape)</code></li>
  <li><code>add_dimension(size)</code></li>
  <li><code>remove_dimension(index)</code></li>
  <li><code>add_element_gap()</code></li>
  <li><code>set_first_visible_index(indices)</code></li>
  <li><code>scroll_to(indices)</code></li>
  <li><code>focus_first_element()</code></li>
  <li><code>reset_to_default()</code></li>
</ul>

<hr/>

<h2 id="standard-events">10. Standard Events</h2>

<ul>
  <li><code>value_changed</code></li>
  <li><code>element_appended</code></li>
  <li><code>element_inserted</code></li>
  <li><code>element_removed</code></li>
  <li><code>shape_changed</code></li>
  <li><code>dimension_added</code></li>
  <li><code>dimension_removed</code></li>
  <li><code>viewport_changed</code></li>
  <li><code>selection_changed</code></li>
  <li><code>focus_gained</code></li>
  <li><code>focus_lost</code></li>
</ul>

<hr/>

<h2 id="default-realization-posture">11. Default Realization Posture</h2>

<p>
The Default realization should expose an array shell with separate subobject assets for index display, element region, element slot, element gap, and scrollbars.
</p>

<p>
The shell owns composition slots.
The element region owns repeated placement.
The element widget class owns the element's individual behavior.
</p>

<hr/>

<h2 id="diagram-interaction-posture">12. Diagram Interaction Posture</h2>

<p>
The array supports natural value participation through <code>widget_value</code>, property access through <code>frog.ui.property_read</code> and <code>frog.ui.property_write</code>, method invocation through <code>frog.ui.method_invoke</code>, event observation where legal, and widget reference targeting through <code>widget_reference</code>.
</p>

<hr/>

<h2 id="validation-expectations">13. Validation Expectations</h2>

<p>
Validators SHOULD diagnose element-type mismatch, invalid rank, invalid shape, invalid index, invalid visible-count posture, illegal resize when resizing is disabled, unsupported element class, and attempts to treat realization-only repeated-cell structures as semantic array storage.
</p>

<hr/>

<h2 id="summary">14. Summary</h2>

<p>
<code>frog.widgets.array</code> is the standardized homogeneous collection widget of FROG.
It can represent arrays of numeric, boolean, string, enum, path, cluster, or other compatible elements.
Its public law defines the collection, element, dimension, viewport, selection, and layout surfaces while leaving repeated rendering and virtualization to realization and runtime.
</p>
