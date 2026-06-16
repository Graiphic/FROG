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

<p>
An array may realize each cell as an instance of another widget class. The array
owns the repeated collection, viewport, index display, scrolling, insertion,
materialization, and shape rules; the contained element widget owns per-cell
editing and display behavior.
</p>

<p>
For widget-backed arrays, each visible cell is a composition boundary whose
default size is the contained widget <code>placement_bounds</code> unless the
source declares an explicit cell size. The Array owns the outer collection
frame, index displays, viewport, scrolling, and materialization; the contained
widget owns its visible <code>main_body</code>, its declared
<code>aura_band</code>, and its own published skin parts. When source-owned
padding and gaps are zero, neighboring contained widget instances may touch or
visually superpose their adjacent <code>placement_bounds</code>. The runtime
must not insert array-local separator lines, fake per-cell shells, or extra
borders between contained widgets.
</p>

<p>
The array must preserve the contained widget property surface. It may repeat,
clip, scroll, materialize, and position contained widget instances, but it must
not replace the contained widget with a reduced hardcoded surrogate. If the
contained widget supports visible items, styles, interaction posture, data-entry
limits, default value, or type representation, those properties remain
source-owned element properties and must be passed through the array container
contract.
</p>

<ul>
  <li><code>element.class_id</code> — widget class used as the array element template when the array is UI-realized.</li>
  <li><code>element.value_type</code> — semantic value type of each element.</li>
  <li><code>element.role</code> — <code>control</code>, <code>indicator</code>, or <code>mixed</code>.</li>
  <li><code>element.template_ref</code> — optional reference to the published element template or element widget instance model.</li>
  <li><code>element.props.*</code> — source-owned property surface passed through to each contained element widget realization.</li>
  <li><code>element.default_value</code> — value used when new elements are inserted or appended.</li>
</ul>

<p>
A conforming array must not make the visible repeated cells the only source of collection truth.
The collection value remains the class-owned semantic value.
</p>

<p>
A numeric array is an array whose element template is a numeric widget. It is
not a separate hardcoded array widget class. For source-owned materialization,
<code>element.default_value</code> is the value used to create new semantic
elements. If the element class also defines a natural default, the
<code>.frog</code> array instance may mirror that value explicitly. The
<code>.wfrog</code> realization resolves visual assets and host capabilities; it
does not own the semantic default value.
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
The baseline supports the array structure required to represent N dimensions.
The Default realization remains intentionally simple and LabVIEW-like: a
one-dimensional array is displayed as a single visible axis, a two-dimensional
array is displayed as a matrix viewport, and arrays of rank three or higher are
displayed by projecting the last two dimensions into the visible matrix while
all leading dimensions are addressed through stacked index displays.
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

<p>
The public <code>index_display</code> part represents the Array index-display
subcontainer as a whole. Realizations may name per-dimension rows, arrow
glyphs, separators, frames, and displayed values as internal SVG details, but
those details must not expand the Array public part contract unless the class
law explicitly adds them.
</p>

<hr/>

<h2 id="standard-property-surface">8. Standard Property Surface</h2>

<h3>8.1 Value, element, and dimensions</h3>

<p>
Array element sizing may be derived from the contained widget template or
overridden by the source instance through <code>element.cell_width</code> and
<code>element.cell_height</code>. Runtime hosts may compose overlays for the
repeated element widgets, but those overlays must align to the published Array
and element realization parts.
</p>

<p>
When a contained widget realization publishes <code>placement_bounds</code>, the
array cell uses those placement bounds as the repeated element surface. This is
the reviewed Default posture for widget-backed cells. The visible widget body
remains inside that cell according to the contained widget
<code>aura_band</code> law; Array does not invent a second inset or redraw the
element as an array-local surrogate.
</p>

<p>
The array container may use explicit contained-widget fitting postures only
when the source intentionally opts out of the placement-bounds policy. For the
reviewed Default Numeric compact realization, both controls and indicators use
<code>placement_bounds</code>: the cell is <code>96x32</code>, the visible body
is <code>88x24</code>, and the contained Numeric body stays centered by its own
uniform aura band.
</p>

<p>
The Array widget also owns its own <code>placement_bounds</code> as a widget.
Those Array <code>placement_bounds</code> are the external placement envelope of
the complete array container: index display, element viewport, scrollbars,
frame, and container background. They are not the internal element grid, and
they are not any contained element widget <code>placement_bounds</code>. In an
IDE view, selecting the Array widget highlights that complete Array placement
surface. Selecting or hovering a cell highlights the cell placement surface
inside the Array. These two overlays must remain distinct.
</p>

<p>
The Array caption/label surface follows the same authoring rule as Numeric:
the label may have its own IDE label bounds above the Array body
<code>placement_bounds</code>, and the two editor overlays may touch at the
shared edge without redefining the Array body bounds. The Array body
<code>placement_bounds</code> remain the grid-snapped top-left placement
reference for the widget as a whole when source/container policy declares snap.
</p>

<p>
Numeric-backed Array controls and indicators keep a visible role distinction by
source-owned style: editable control cells use the white Default Numeric value
face and visible increment/decrement commands, while read-only indicator cells
use the Default Numeric indicator gray <code>#E8EBED</code> and hide those
commands. Both postures keep the compact Numeric value face border visible with
the reviewed gray <code>#A8ABAE</code> at <code>1px</code>; this border belongs
to <code>element.props.style.value_face.*</code>, not to an Array-local fake
frame. The reviewed Array container body fill for Numeric-backed examples is
the pale gray-blue <code>#EEF4F6</code>; the index value surface remains white
inside that container. The Array frame/region border uses
<code>#A8ABAE</code> at the declared width, and scrollbar/button helpers use
the same reviewed neutral palette (<code>#DDE1E4</code> faces,
<code>#A8ABAE</code> borders/thumbs) unless the source explicitly declares a
different skin. The runtime must read these properties from the
source/realization contract rather than infer colors from a hardcoded Array
mode.
</p>

<p>
The neutral Array shell palette is not an example-only override. It is the
Default Array skin published through
<code>Libraries/Realizations/Default/array.default.wfrog</code>
<code>default_widget_properties</code> for Array controls and indicators:
container/body fill <code>#EEF4F6</code>, white index value surfaces,
<code>#A8ABAE</code> frame/index/scrollbar borders, <code>#DDE1E4</code>
helper button faces, and <code>#00ADEF</code> selection/focus accents. Reused
Array examples should inherit that default posture and only override it when
the source intentionally declares a different skin.
</p>

<p>
When the array cell intentionally wraps a contained widget with extra breathing
room, <code>element.layout.padding</code> defines the source-owned
space between the repeated cell bounds and the contained widget instance. This
padding must be consumed as layout geometry; it must not be replaced by a
runtime-local fake border or duplicated element skin. Border-sharing repeated
widgets keep this value at <code>0</code> when
<code>element.layout.boundary_policy=contained_widget_placement_bounds</code>;
the apparent breathing room is already the contained widget's
<code>aura_band</code> inside its <code>placement_bounds</code>.
</p>

<p>
Widget-backed cells follow the shared placement-grid law documented in
<a href="./PlacementGrid.md"><code>PlacementGrid.md</code></a>. The contained
widget remains autonomous and may resize according to its own law, but the
Array element cell is the contained widget placement surface inside the Array.
Cell hover, preselection, and selection belong to the Array cell. A contained
widget focus ring is shown only when that contained widget itself receives
focus for editing.
</p>

<ul>
  <li><code>value</code></li>
  <li><code>length</code></li>
  <li><code>element.class_id</code></li>
  <li><code>element.value_type</code></li>
  <li><code>element.role</code></li>
  <li><code>element.template_ref</code></li>
  <li><code>element.default_value</code></li>
  <li><code>element.layout.padding</code> - optional source-owned inset between the array cell bounds and the contained widget instance; must be <code>0</code> for reviewed <code>contained_widget_placement_bounds</code> cells because the cell already is the contained widget <code>placement_bounds</code>.</li>
  <li><code>element.layout.grid_pitch</code> - optional source-owned placement-grid pitch for widget-backed cells; the reviewed Default posture uses <code>16px</code>.</li>
  <li><code>element.layout.snap_policy</code> - optional placement sizing policy; widget-backed cells should snap the placement surface to the common grid.</li>
  <li><code>element.layout.fit_part</code> — optional contained-widget public part used as the repeated-cell bounds; reviewed widget-backed cells use <code>placement_bounds</code>.</li>
  <li><code>element.layout.boundary_policy</code> - optional source-owned posture for contained-widget boundary handling; the Default realization supports <code>contained_widget_placement_bounds</code> for grid-snapped widget-backed cells.</li>
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

<p>
<code>viewport.visible_counts[]</code> is the source-owned visible-shape
property surface of the Array viewport. It describes how many element slots are
visible along each displayed Array axis; it does not change the semantic Array
rank, shape, or stored value by itself.
</p>

<p>
The visible count may exceed the currently materialized semantic shape. In that
case, the Array viewport displays additional default-valued element positions
without forcing the source value to grow until ordinary Array edit/materialization
rules require it. A data-entry limit on a visible-count control is therefore a
source-owned UI policy, not an implicit clamp to the Array's allocated shape.
</p>

<p>
For a two-dimensional or higher-rank Array, the Default convention is:
</p>

<ul>
  <li><code>viewport.visible_counts[0]</code> controls the visible row count of the penultimate semantic dimension.</li>
  <li><code>viewport.visible_counts[1]</code> controls the visible column count of the final semantic dimension.</li>
</ul>

<p>
For one-dimensional Array examples, the <code>.frog</code> instance declares
which visible display axis is controlled. A vertical 1D viewport may drive the
visible row count; a horizontal 1D viewport may drive the visible column count.
This mapping is source-owned and must survive FIR and lowering instead of being
invented by a runtime host.
</p>

<p>
When <code>viewport.visible_counts[]</code> is changed by another front-panel
control, that change is represented as explicit diagram flow through
<code>frog.ui.property_write</code> / property-write nodes. In the executable
example posture, those writes are consumed on <code>Execute</code>. A host may
later support IDE/design-time live preview behavior, but that behavior is a
separate host capability and must not be confused with diagram execution.
</p>

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

<p>
The array container frame is distinct from the contained element widget frame.
For a widget-backed array, <code>style.frame.*</code> controls the outer
array viewport shell around element viewports and scrollbar surfaces.
Visible index displays are neighboring Array subcontainers controlled through
<code>index_display.layout.*</code> and <code>style.index_display.*</code>;
they are not visually merged into the element viewport frame. The index display
may have its own source-owned container fill, border, border width, and padding
around the visible arrow/value indexer.
When <code>index_display.rank</code> changes, the realization must wrap the
active number of visible index boxes using the source-owned box height, gap,
container border, and container padding; it must not leave stale empty
subcontainer space from a previous rank.
<code>element.props.style.*</code> controls the
repeated contained widget. This preserves the visual distinction between
the collection container and the element realization.
</p>

<h3>8.4 Interaction and style</h3>

<ul>
  <li><code>interaction.visible</code></li>
  <li><code>interaction.enabled</code></li>
  <li><code>interaction.read_only</code></li>
  <li><code>interaction.focused</code></li>
  <li><code>style.frame.*</code></li>
  <li><code>style.frame.padding</code> — optional source-owned padding between the outer container frame and the element viewport / scrollbar surfaces.</li>
  <li><code>style.element_region.*</code></li>
  <li><code>style.index_display.*</code></li>
  <li><code>style.index_display.container_fill_color</code></li>
  <li><code>style.index_display.container_border_color</code></li>
  <li><code>style.index_display.container_border_width</code></li>
  <li><code>style.index_display.container_padding</code></li>
  <li><code>style.index_display.step_gap</code> — optional source-owned spacing between the increment and decrement arrow surfaces.</li>
  <li><code>style.index_display.value_gap</code> — optional source-owned spacing between the arrow surfaces and the visible index value surface.</li>
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

<p>
Array viewport properties are first-class diagram targets. A LabVIEW-like
property-node flow must be visible in <code>.frog</code>, preserved in FIR, and
declared through lowering before the runtime consumes the native manifest and
front-panel realization package. Example 34 demonstrates this for a 2D Array:
two Numeric U8 controls feed property-write nodes targeting
<code>viewport.visible_counts[0]</code> and
<code>viewport.visible_counts[1]</code> on both the Array control and the
Array indicator, while the selected element value still passes through the
native manifest-backed value corridor.
</p>

<p>
The same rule applies to dimension-count changes. A command that changes
<code>dimensions.rank</code> must be represented as explicit diagram flow and,
when the visible index display count is meant to follow the semantic rank, it
must also write the matching <code>index_display.rank</code> surface. For an
N-dimensional Array, the source may provide explicit rank profiles and a
default shape policy. The realization displays rank <code>1</code> as one
visible axis, rank <code>2</code> as a matrix, and rank <code>3</code> or
higher by projecting the final two semantic dimensions into the matrix while
leading dimensions are controlled through index displays.
</p>

<hr/>

<h2 id="validation-expectations">13. Validation Expectations</h2>

<p>
Validators SHOULD diagnose element-type mismatch, invalid rank, invalid shape, invalid index, invalid visible-count posture, illegal resize when resizing is disabled, unsupported element class, and attempts to treat realization-only repeated-cell structures as semantic array storage.
</p>

<p>
A visible-count property example is not valid unless the source contains
explicit property-write nodes, FIR preserves those nodes and edges, lowering
declares the property-write flow and native manifest boundary, and the runtime
updates only the declared Array viewport properties on diagram execution. The
runtime must not create an implicit live-layout shortcut that bypasses
<code>.frog</code> flow.
</p>

<hr/>

<h2 id="summary">14. Summary</h2>

<p>
<code>frog.widgets.array</code> is the standardized homogeneous collection widget of FROG.
It can represent arrays of numeric, boolean, string, enum, path, cluster, or other compatible elements.
Its public law defines the collection, element, dimension, viewport, selection, and layout surfaces while leaving repeated rendering and virtualization to realization and runtime.
</p>
