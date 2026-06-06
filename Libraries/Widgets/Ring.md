<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Standard Ring Widgets</h1>

<p align="center">
  <strong>Normative baseline for standardized ring control and ring indicator widget classes</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="./Readme.md">Widgets index</a></li>
  <li><a href="../../Expression/Widget.md">Expression widget instances</a></li>
  <li><a href="../../Expression/Widget%20interaction.md">Expression widget interaction</a></li>
  <li><a href="../../Expression/Widget%20realization.md">Expression widget realization</a></li>
  <li><a href="../../Expression/Widget%20package%20(.wfrog).md">Widget package publication</a></li>
  <li><a href="../Realizations/Default/Ring.md">Default realization - Ring</a></li>
  <li><a href="../Realizations/Default/ring.default.wfrog">Default Ring realization manifest</a></li>
</ul>

<hr/>

<h2>Classes Defined Here</h2>

<ul>
  <li><code>frog.widgets.ring_control</code></li>
  <li><code>frog.widgets.ring_indicator</code></li>
</ul>

<hr/>

<h2>Overview</h2>

<p>
The Ring family defines compact finite-choice widgets for FROG front panels.
A Ring exposes one selected item from a source-owned item list while displaying
the selected item through a compact selector surface.
</p>

<p>
A Ring is intentionally distinct from an Enum.
An Enum owns a typed symbolic domain and an explicit integer representation.
A Ring owns a selectable item list whose displayed strings map to ordinary
source-declared values. The LabVIEW-like default posture is numeric: the Ring
shows text but publishes the selected item's numeric value. A Ring realization
may look similar to an enum ring or dropdown, but it does not define a typed enum
domain.
</p>

<hr/>

<h2>Common Family Posture</h2>

<ul>
  <li>family: compact finite-choice selection widget family</li>
  <li>primary value: present</li>
  <li>value type: source-declared scalar item value; default baseline <code>u16</code></li>
  <li>public value-facing surface: yes</li>
  <li>object-style access surface: yes</li>
  <li>primary value mirror property: <code>value.selected_value</code></li>
  <li>item inventory surface: <code>items[]</code></li>
  <li>common label property: <code>label.text</code></li>
  <li>common caption property: <code>caption.text</code></li>
</ul>

<p>
The family separates:
</p>

<ul>
  <li><code>value.selected_id</code> - selected item identity for UI lookup and editing,</li>
  <li><code>value.selected_index</code> - selected item index when index access is required,</li>
  <li><code>value.selected_value</code> - scalar value carried by the selected item and published to the diagram,</li>
  <li><code>data_type.representation</code> - numeric representation such as <code>u8</code>, <code>u16</code>, <code>u32</code>, <code>i16</code>, <code>i32</code>, <code>i64</code>, <code>sgl</code>, <code>dbl</code>, or equivalent implementation-supported numeric carrier,</li>
  <li><code>items[]</code> - source-owned item inventory,</li>
  <li><code>display.*</code> - selector display posture,</li>
  <li><code>interaction.*</code> - enabled, read-only, opened, and focus posture,</li>
  <li>realization-private popup handles, host-native combo handles, row caches, and IDE-private item editors.</li>
</ul>

<hr/>

<h2><code>frog.widgets.ring_control</code></h2>

<h3>Class identity</h3>

<ul>
  <li><strong>class_id:</strong> <code>frog.widgets.ring_control</code></li>
  <li><strong>family:</strong> <code>ring_widget</code></li>
  <li><strong>compatible role:</strong> <code>control</code></li>
</ul>

<h3>Primary value posture</h3>

<ul>
  <li>primary value: present</li>
  <li>natural value participation: yes</li>
  <li>user-mutable: yes when enabled</li>
  <li>diagram-mutable: yes</li>
  <li>mirrored property: <code>value.selected_value</code></li>
</ul>

<p>
A ring control may open a selector, choose one enabled item, and publish the
selected item value to the diagram while keeping item id and label available for
front-panel editing and display.
</p>

<hr/>

<h2><code>frog.widgets.ring_indicator</code></h2>

<h3>Class identity</h3>

<ul>
  <li><strong>class_id:</strong> <code>frog.widgets.ring_indicator</code></li>
  <li><strong>family:</strong> <code>ring_widget</code></li>
  <li><strong>compatible role:</strong> <code>indicator</code></li>
</ul>

<h3>Primary value posture</h3>

<ul>
  <li>primary value: present</li>
  <li>natural value participation: yes</li>
  <li>user-mutable: no in the standard portable indicator posture</li>
  <li>diagram-mutable: yes</li>
  <li>mirrored property: <code>value.selected_value</code></li>
</ul>

<p>
A ring indicator displays the selected item posture. It may expose viewport or
selector inspection surfaces, but portable user-originated item mutation belongs
to <code>frog.widgets.ring_control</code>.
</p>

<hr/>

<h2>Ring Value Model</h2>

<p>
The portable Ring value is the selected item value. The preferred compact
surface is:
</p>

<ul>
  <li><code>value.selected_id</code></li>
  <li><code>value.selected_index</code></li>
  <li><code>value.selected_value</code></li>
  <li><code>value.selected_label</code></li>
  <li><code>data_type.representation</code></li>
</ul>

<p>
Each item may expose:
</p>

<ul>
  <li><code>items[].id</code> - stable item identifier,</li>
  <li><code>items[].label</code> - visible item label,</li>
  <li><code>items[].value</code> - scalar value, numeric in the LabVIEW-like default posture,</li>
  <li><code>items[].enabled</code>,</li>
  <li><code>items[].visible</code>,</li>
  <li><code>items[].tooltip</code>,</li>
  <li><code>items[].metadata</code>.</li>
</ul>

<p>
This mirrors the practical LabVIEW Ring distinction: visible strings are edited
as item text, while the diagram receives the associated numeric value. FROG also
keeps stable item ids so IDEs and runtimes can patch UI state without treating
labels as identity.
</p>

<hr/>

<h2>Public Part Model</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>value_face</code></li>
  <li><code>value_display</code></li>
  <li><code>selector_face</code></li>
  <li><code>focus_ring</code></li>
  <li><code>spinner</code> when a realization exposes increment/decrement commands</li>
  <li><code>increment_up</code> when a realization exposes increment/decrement commands</li>
  <li><code>increment_down</code> when a realization exposes increment/decrement commands</li>
  <li><code>list_panel</code></li>
  <li><code>option_row</code></li>
  <li><code>option_label</code></li>
  <li><code>option_selection_face</code></li>
</ul>

<p>
The selector arrow is a realization detail owned by <code>selector_face</code>,
not a public semantic part. The <code>focus_ring</code> is public so the SVG
skin owns focus geometry. Native handles, popup caches, and other host-private
interaction affordances remain runtime or realization-private unless a concrete
Ring behavior explicitly promotes them later.
</p>

<p>
When a realization promotes the LabVIEW-like increment/decrement command, the
command surfaces are semantic Ring parts. <code>spinner</code> owns visibility,
while <code>increment_up</code> and <code>increment_down</code> invoke the next
and previous visible enabled item command.
</p>

<hr/>

<h2>Standard Properties</h2>

<h3>Value and items</h3>

<ul>
  <li><code>value.selected_id : string</code></li>
  <li><code>value.selected_index : u32</code></li>
  <li><code>value.selected_value : scalar</code></li>
  <li><code>value.selected_label : string</code></li>
  <li><code>data_type.representation : numeric_representation</code></li>
  <li><code>data_type.named_numeric_size : string</code></li>
  <li><code>data_type.allow_undefined_values : bool</code></li>
  <li><code>data_type.last_value_hidden : bool</code></li>
  <li><code>items[]</code></li>
  <li><code>items[].id : string</code></li>
  <li><code>items[].label : string</code></li>
  <li><code>items[].value : scalar</code></li>
  <li><code>items[].enabled : bool</code></li>
  <li><code>items[].visible : bool</code></li>
  <li><code>items.sequential_values : bool</code></li>
</ul>

<h3>LabVIEW-like property-node mapping</h3>

<p>
The following FROG surfaces intentionally mirror the useful Ring property-node
families shown by LabVIEW without copying LabVIEW internals:
</p>

<ul>
  <li><strong>Bounds / Position:</strong> widget <code>layout</code> in <code>.frog</code>.</li>
  <li><strong>Label / Caption / Tip Strip / Description:</strong> <code>label.*</code>, <code>caption.*</code>, and <code>documentation.*</code>.</li>
  <li><strong>Visible / Disabled / Key Focus / Skip When Tabbing:</strong> <code>interaction.*</code>.</li>
  <li><strong>Representation / Named Numeric Size:</strong> <code>data_type.representation</code> and <code>data_type.named_numeric_size</code>.</li>
  <li><strong>Strings[]:</strong> <code>items[].label</code>.</li>
  <li><strong>Strings And Values[]:</strong> <code>items[].label</code> plus <code>items[].value</code>.</li>
  <li><strong>Disabled Items[]:</strong> <code>items[].enabled = false</code>.</li>
  <li><strong>Number Of Items:</strong> derived from <code>items[]</code>.</li>
  <li><strong>Allow Undefined Values / Last Value Hidden:</strong> <code>data_type.allow_undefined_values</code> and <code>data_type.last_value_hidden</code>.</li>
  <li><strong>Data Entry Limits:</strong> <code>data_entry.*</code>.</li>
  <li><strong>Display Format:</strong> <code>display.format.*</code>.</li>
  <li><strong>Digital Display / Increment-Decrement Visible:</strong> <code>display.digital_display_visible</code> and <code>display.increment_decrement_visible</code>.</li>
  <li><strong>Unit Label:</strong> <code>display.unit_label.*</code>.</li>
  <li><strong>Data Binding / Control Index:</strong> <code>binding.*</code>.</li>
</ul>

<h3>Display and interaction</h3>

<ul>
  <li><code>display.selector_visible : bool</code></li>
  <li><code>display.digital_display_visible : bool</code></li>
  <li><code>display.increment_decrement_visible : bool</code></li>
  <li><code>display.popup_max_visible_items : u32</code></li>
  <li><code>display.text_overflow_visible : bool</code></li>
  <li><code>display.format.mode : enum</code></li>
  <li><code>display.format.string : string</code></li>
  <li><code>display.unit_label.visible : bool</code></li>
  <li><code>display.unit_label.text : string</code></li>
  <li><code>interaction.visible : bool</code></li>
  <li><code>interaction.enabled : bool</code></li>
  <li><code>interaction.read_only : bool</code></li>
  <li><code>interaction.focusable : bool</code></li>
  <li><code>interaction.opened : bool</code></li>
  <li><code>interaction.mouse_wheel_enabled : bool</code></li>
  <li><code>interaction.skip_when_tabbing : bool</code></li>
  <li><code>interaction.key_focus : bool</code></li>
</ul>

<h3>Data entry</h3>

<ul>
  <li><code>data_entry.use_default_limits : bool</code></li>
  <li><code>data_entry.minimum : scalar</code></li>
  <li><code>data_entry.maximum : scalar</code></li>
  <li><code>data_entry.increment : scalar</code></li>
  <li><code>data_entry.increment_wrap : bool</code></li>
  <li><code>data_entry.page_size : scalar</code></li>
  <li><code>data_entry.response_below_minimum : enum</code></li>
  <li><code>data_entry.response_above_maximum : enum</code></li>
  <li><code>data_entry.response_outside_items : enum</code></li>
  <li><code>key_binding.increment : string</code></li>
  <li><code>key_binding.decrement : string</code></li>
  <li><code>key_binding.focus : string</code></li>
</ul>

<h3>Documentation and binding</h3>

<ul>
  <li><code>documentation.description : string</code></li>
  <li><code>documentation.tip_strip : string</code></li>
  <li><code>binding.data_binding_ref : string</code></li>
  <li><code>binding.control_index : u32</code></li>
</ul>

<h3>Style</h3>

<ul>
  <li><code>style.value_face.*</code></li>
  <li><code>style.value_display.*</code></li>
  <li><code>style.value_display.vertical_offset : signed px length</code></li>
  <li><code>style.selector_face.*</code></li>
  <li><code>style.focus_ring.*</code></li>
  <li><code>style.increment_button.*</code> when a realization exposes increment/decrement command parts</li>
  <li><code>style.popup.*</code></li>
  <li><code>style.popup.option.*</code></li>
</ul>

<hr/>

<h2>Standard Events</h2>

<ul>
  <li><code>value_changed</code></li>
  <li><code>selection_changed</code></li>
  <li><code>selector_opened</code></li>
  <li><code>selector_closed</code></li>
  <li><code>focus_changed</code></li>
</ul>

<hr/>

<h2>Validation Expectations</h2>

<ul>
  <li>Ring controls must publish an item inventory in source-owned instance data.</li>
  <li>In the LabVIEW-like default posture, Ring controls publish the selected item value to the diagram and use the selected item id for UI identity.</li>
  <li><code>items[].value</code>, <code>data_type.representation</code>, data-entry limits, display-format posture, undefined-value policy, disabled-item posture, and optional digital/increment surfaces must be source-owned properties when used.</li>
  <li>The visible selector must consume a realization asset and published parts.</li>
  <li>Runtime overlays must align to <code>value_face</code>, <code>value_display</code>, <code>selector_face</code>, optional increment/decrement parts, and <code>list_panel</code>.</li>
  <li>Visual values such as colors, borders, thickness, text, hover, and selected states must come from source-owned properties or the Default realization.</li>
  <li>Ring must not be treated as Enum unless the source explicitly uses an Enum widget class.</li>
</ul>
