<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Standard Enum Widgets</h1>

<p align="center">
  <strong>Normative baseline for standardized enum control and enum indicator widget classes</strong><br/>
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
  <li><a href="../../Libraries/Realizations/Default/Enum.md">Default enum realization</a></li>
  <li><a href="../../Libraries/Realizations/Default/enum.default.wfrog">Default enum realization manifest</a></li>
</ul>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#classes-defined-here">2. Classes Defined Here</a></li>
  <li><a href="#enum-class-versus-realization">3. Enum Class versus Realization</a></li>
  <li><a href="#value-item-and-representation-model">4. Value, Item, and Representation Model</a></li>
  <li><a href="#label-caption-and-display-surfaces">5. Label, Caption, and Display Surfaces</a></li>
  <li><a href="#common-family-posture">6. Common Family Posture</a></li>
  <li><a href="#public-part-model">7. Public Part Model</a></li>
  <li><a href="#standard-property-surface">8. Standard Property Surface</a></li>
  <li><a href="#frogwidgetsenum_control">9. <code>frog.widgets.enum_control</code></a></li>
  <li><a href="#frogwidgetsenum_indicator">10. <code>frog.widgets.enum_indicator</code></a></li>
  <li><a href="#authoring-and-item-editing-posture">11. Authoring and Item Editing Posture</a></li>
  <li><a href="#default-realization-posture">12. Default Realization Posture</a></li>
  <li><a href="#diagram-interaction-posture">13. Diagram Interaction Posture</a></li>
  <li><a href="#validation-expectations">14. Validation Expectations</a></li>
  <li><a href="#summary">15. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the standardized baseline for enum widgets in FROG.
</p>

<p>
Latest Enum widget review:
<time datetime="2026-06-12">2026-06-12</time>. The reviewed Default posture
keeps the compact value SVG semantic, publishes optional increment/decrement
command parts for LabVIEW-like item stepping, and publishes a separate dropdown
list SVG skin for the host-rendered item popup.
</p>

<p>
The enum family provides single-selection discrete named-value widgets.
It covers the common graphical-programming case where a front-panel object selects one symbolic value from a finite item set, while preserving a clear underlying integer representation for serialization, comparison, and backend-friendly execution.
</p>

<p>
The design is analogous to LabVIEW-style enum controls, but normalized for FROG:
</p>

<ul>
  <li>the selected semantic value is distinct from visible item text,</li>
  <li>the item set is explicit and inspectable,</li>
  <li>the underlying representation is explicit,</li>
  <li>the realization may be ring-like, dropdown-like, selector-like, or host-native,</li>
  <li>item editing is primarily an authoring concern, not a runtime-private trick.</li>
</ul>

<hr/>

<h2 id="classes-defined-here">2. Classes Defined Here</h2>

<ul>
  <li><code>frog.widgets.enum_control</code></li>
  <li><code>frog.widgets.enum_indicator</code></li>
</ul>

<hr/>

<h2 id="enum-class-versus-realization">3. Enum Class versus Realization</h2>

<p>
An enum class is not the same thing as a dropdown widget, ring widget, popup menu, host-native combo box, or SVG selector.
</p>

<p>
The class owns selected enum value semantics, the legal item set, the representation posture, the control-versus-indicator distinction, properties, methods, events, and public parts.
The realization owns visual embodiment, popup layout, optional increment/decrement affordances, dropdown/list surfaces, accepted reusable default style values, and SVG or host-native resources. The reviewed Default Enum posture does not publish digital display, selector face/arrow, or text-overflow marker parts.
</p>

<p>
Enum and Ring may share the same compact finite-choice visual posture and runtime rendering helpers. They remain separate widget classes because their terminal values are different: Enum publishes an item identity from a typed enum domain, while Ring publishes a source-declared scalar item value.
</p>

<hr/>

<h2 id="value-item-and-representation-model">4. Value, Item, and Representation Model</h2>

<p>
The canonical semantic selected value is <code>value</code>.
In the intrinsic baseline, <code>value</code> denotes the selected item identity, not merely visible text.
The declared item set maps item identities to display text and numeric representation values.
</p>

<p>
Each enum item SHOULD preserve:
</p>

<ul>
  <li><code>items[].id</code> — stable item identity.</li>
  <li><code>items[].text</code> — user-visible item text.</li>
  <li><code>items[].numeric_value</code> — underlying integer representation value.</li>
  <li><code>items[].enabled</code> — whether the item is selectable in the active posture.</li>
</ul>

<p>
The underlying representation is selected through <code>representation.kind</code>.
The standard representation kinds are <code>uint8</code>, <code>uint16</code>, <code>uint32</code>, <code>uint64</code>, <code>int8</code>, <code>int16</code>, <code>int32</code>, and <code>int64</code>.
The preferred default representation is <code>uint16</code>.
</p>

<hr/>

<h2 id="label-caption-and-display-surfaces">5. Label, Caption, and Display Surfaces</h2>

<pre><code>label.*
    - structural / logical widget name surface

caption.*
    - front-panel presentation caption shown near the enum widget

value_display
    - visible selected-item text surface
</code></pre>

<p>
This distinction prevents item text, logical widget identity, and caption text
from collapsing into one ambiguous field.
</p>

<hr/>

<h2 id="common-family-posture">6. Common Family Posture</h2>

<ul>
  <li>family: discrete named-value widget family</li>
  <li>primary value: present</li>
  <li>primary value kind: one item identity from a finite declared item set</li>
  <li>natural value participation: yes</li>
  <li>object-style access: yes</li>
  <li>primary value mirror property: <code>value</code></li>
  <li>item inventory surface: <code>items[]</code></li>
  <li>selected item surface: <code>selected.*</code></li>
  <li>underlying representation surface: <code>representation.*</code></li>
  <li>display-format surface: <code>display.*</code></li>
  <li>item-editing posture: <code>item_editing.*</code></li>
</ul>

<hr/>

<h2 id="public-part-model">7. Public Part Model</h2>

<ul>
  <li><code>root</code></li>
  <li><code>placement_bounds</code> when a realization publishes a placement aura</li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>value_face</code></li>
  <li><code>value_display</code></li>
  <li><code>focus_ring</code></li>
  <li><code>spinner</code> when a realization exposes increment/decrement commands</li>
  <li><code>increment_up</code> when a realization exposes increment/decrement commands</li>
  <li><code>increment_down</code> when a realization exposes increment/decrement commands</li>
</ul>

<p>
<code>placement_bounds</code> is a placement aura part, not a visible widget
face, focus ring, or selection overlay. IDE hosts, grid-aware layout, and Array
containment MAY consume it as the widget footprint when the realization
publishes it. Internal visible parts such as <code>value_face</code>,
<code>spinner</code>, <code>increment_up</code>, and
<code>increment_down</code> must not be substituted for the placement footprint.
</p>

<p>
The <code>focus_ring</code> is public so focus geometry is owned by the SVG skin
and not invented by runtime code. In the Default
rectangular-ring realization, <code>focus_ring</code> follows
<code>value_face</code> only; it does not enclose <code>spinner</code>,
<code>increment_up</code>, or
<code>increment_down</code>. The compact value SVG does not expose popup rows
as public parts. When a realization publishes a host dropdown surface, that surface
MAY carry its own skin parts such as <code>list_panel</code>,
<code>option_row</code>, and <code>option_text</code>; hidden value state
controls, overflow handling, and any future numeric-representation display
remain host or variant details unless explicitly published by a later
realization contract.
</p>

<hr/>

<h2 id="standard-property-surface">8. Standard Property Surface</h2>

<h3>8.1 Value and selection</h3>

<ul>
  <li><code>value : enum_item_id</code></li>
  <li><code>selected.index : u32</code></li>
  <li><code>selected.id : string</code></li>
  <li><code>selected.text : string</code></li>
  <li><code>selected.numeric_value : integer</code></li>
</ul>

<h3>8.2 Item inventory</h3>

<ul>
  <li><code>items.count : u32</code></li>
  <li><code>items[]</code></li>
  <li><code>items[].id : string</code></li>
  <li><code>items[].text : string</code></li>
  <li><code>items[].numeric_value : integer</code></li>
  <li><code>items[].enabled : bool</code></li>
</ul>

<h3>8.3 Representation</h3>

<ul>
  <li><code>representation.kind : enum</code></li>
  <li><code>representation.allow_undefined_runtime_values : bool</code></li>
</ul>

<h3>8.4 Label and caption</h3>

<ul>
  <li><code>label.visible : bool</code></li>
  <li><code>label.text : string</code></li>
  <li><code>label.style.*</code></li>
  <li><code>caption.visible : bool</code></li>
  <li><code>caption.text : string</code></li>
  <li><code>caption.placement : enum</code></li>
  <li><code>caption.style.*</code></li>
</ul>

<h3>8.5 Display</h3>

<ul>
  <li><code>display.value_text_visible : bool</code></li>
  <li><code>display.increment_buttons_visible : bool</code></li>
  <li><code>display.text_width_chars : u32</code></li>
</ul>

<h3>8.6 Data entry</h3>

<ul>
  <li><code>data_entry.increment_wrap : bool</code></li>
</ul>

<h3>8.7 Interaction</h3>

<ul>
  <li><code>interaction.visible : bool</code></li>
  <li><code>interaction.enabled : bool</code></li>
  <li><code>interaction.read_only : bool</code></li>
  <li><code>interaction.focusable : bool</code></li>
  <li><code>interaction.focused : bool</code></li>
  <li><code>interaction.opened : bool</code> when an opened selector posture exists</li>
</ul>

<h3>8.8 Authoring item-editing posture</h3>

<ul>
  <li><code>item_editing.allow_insert : bool</code></li>
  <li><code>item_editing.allow_delete : bool</code></li>
  <li><code>item_editing.allow_reorder : bool</code></li>
  <li><code>item_editing.allow_disable_items : bool</code></li>
  <li><code>item_editing.allow_undefined_runtime_values : bool</code></li>
</ul>

<h3>8.9 Style and realization</h3>

<ul>
  <li><code>style.value_face.*</code></li>
  <li><code>style.value_display.*</code></li>
  <li><code>style.focus_ring.*</code></li>
  <li><code>style.increment_button.*</code> when a realization exposes increment/decrement command parts</li>
  <li><code>style.dropdown.*</code> when a host exposes a popup/dropdown selector surface or dropdown SVG skin</li>
  <li><code>realization.family : string</code></li>
  <li><code>realization.variant : string</code></li>
  <li><code>realization.skin_id : string</code></li>
</ul>

<hr/>

<h2 id="frogwidgetsenum_control">9. <code>frog.widgets.enum_control</code></h2>

<ul>
  <li><strong>class_id:</strong> <code>frog.widgets.enum_control</code></li>
  <li><strong>family:</strong> <code>enum_widget</code></li>
  <li><strong>compatible role:</strong> <code>control</code></li>
  <li>user-mutable: yes</li>
  <li>diagram-mutable: yes</li>
</ul>

<h3>9.1 Standard methods</h3>

<ul>
  <li><code>focus()</code></li>
  <li><code>open_selector()</code></li>
  <li><code>close_selector()</code></li>
  <li><code>increment()</code> as the LabVIEW-like visual command alias for <code>select_next()</code></li>
  <li><code>decrement()</code> as the LabVIEW-like visual command alias for <code>select_previous()</code></li>
  <li><code>select_next()</code></li>
  <li><code>select_previous()</code></li>
  <li><code>select_item(id)</code></li>
  <li><code>set_selected_index(index)</code></li>
  <li><code>reset_to_default()</code></li>
</ul>

<h3>9.2 Standard events</h3>

<ul>
  <li><code>value_changed</code></li>
  <li><code>selection_opened</code></li>
  <li><code>selection_committed</code></li>
  <li><code>item_set_changed</code></li>
  <li><code>item_disabled_changed</code></li>
  <li><code>focus_gained</code></li>
  <li><code>focus_lost</code></li>
</ul>

<hr/>

<h2 id="frogwidgetsenum_indicator">10. <code>frog.widgets.enum_indicator</code></h2>

<ul>
  <li><strong>class_id:</strong> <code>frog.widgets.enum_indicator</code></li>
  <li><strong>family:</strong> <code>enum_widget</code></li>
  <li><strong>compatible role:</strong> <code>indicator</code></li>
  <li>user-mutable: no in the standard portable posture</li>
  <li>diagram-mutable: yes</li>
</ul>

<h3>10.1 Standard methods</h3>

<ul>
  <li><code>focus()</code> when supported by the host</li>
  <li><code>reset_to_default_style()</code></li>
</ul>

<h3>10.2 Standard events</h3>

<ul>
  <li><code>value_rendered</code></li>
  <li><code>focus_gained</code></li>
  <li><code>focus_lost</code></li>
</ul>

<hr/>

<h2 id="authoring-and-item-editing-posture">11. Authoring and Item Editing Posture</h2>

<p>
Item editing is primarily an authoring concern.
An IDE may expose operations such as select item, insert item, add item before, add item after, delete item, move item, edit items, disable item, and allow undefined runtime values.
</p>

<p>
Those authoring operations must modify the declared item inventory and validation posture explicitly.
They must not create hidden runtime-only enum values.
</p>

<hr/>

<h2 id="default-realization-posture">12. Default Realization Posture</h2>

<p>
The Default realization should expose a rectangular enum value template with
a value face, visible selected-item text, caption, and label surfaces. A control
realization MAY expose LabVIEW-like increment and decrement command parts as
<code>spinner</code>, <code>increment_up</code>, and
<code>increment_down</code>, controlled by
<code>display.increment_buttons_visible</code>. Popup/dropdown option
interaction is host-owned for the current baseline, but the visible dropdown
panel, row, and option text skin are published through a separate SVG template.
</p>

<p>
Popup/dropdown layout must be declared by the realization manifest, not inferred
from runtime-specific widget knowledge. The Default realization declares that
its dropdown host surface anchors to <code>value_face</code>, measures
<code>outer_width</code> with a declared source-width outset, and centers the
popup under the body.
</p>

<p>
When the Default posture is embedded in Array, the popup/list surface remains a
host overlay and may be reparented to the Array overlay layer so it renders
above repeated cells instead of being clipped by a cell. This does not make the
popup a public part of the closed widget. The Default realization sets
<code>data_entry.increment_wrap=true</code>, so visual increment/decrement
commands wrap circularly unless source-owned properties request clamping.
</p>

<hr/>

<h2 id="diagram-interaction-posture">13. Diagram Interaction Posture</h2>

<p>
The enum family supports natural value participation through <code>widget_value</code>, property access through <code>frog.ui.property_read</code> and <code>frog.ui.property_write</code>, method invocation through <code>frog.ui.method_invoke</code>, event observation where legal, and widget reference targeting through <code>widget_reference</code>.
</p>

<hr/>

<h2 id="validation-expectations">14. Validation Expectations</h2>

<p>
Validators SHOULD diagnose unknown item ids, duplicate item ids, duplicate numeric representation values where disallowed, selected values not in the item set, disabled selected items where disallowed, invalid representation kinds, invalid selected indexes, and attempts to treat realization-private popup internals as public class members.
</p>

<hr/>

<h2 id="summary">15. Summary</h2>

<p>
The enum widget family defines standardized finite single-selection discrete-value widgets:
</p>

<ul>
  <li><code>frog.widgets.enum_control</code></li>
  <li><code>frog.widgets.enum_indicator</code></li>
</ul>

<p>
The family is value-oriented, item-set-oriented, representation-aware, and realization-neutral.
</p>
