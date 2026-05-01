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
The realization owns visual embodiment, popup layout, selector affordances, increment/decrement buttons, optional digital display, optional text-overflow markers, and SVG or host-native resources.
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

digital_display
    - optional numeric representation display
</code></pre>

<p>
This distinction prevents item text, logical widget identity, caption text, and numeric representation display from collapsing into one ambiguous field.
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
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>frame</code></li>
  <li><code>value_face</code></li>
  <li><code>value_display</code></li>
  <li><code>selector_face</code></li>
  <li><code>increment_up</code> when present</li>
  <li><code>increment_down</code> when present</li>
  <li><code>digital_display</code> when present</li>
  <li><code>text_overflow_marker</code> when present</li>
  <li><code>popup_layer</code> when the active realization exposes it</li>
  <li><code>focus_ring</code> when present</li>
</ul>

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
  <li><code>display.digital_display_visible : bool</code></li>
  <li><code>display.increment_buttons_visible : bool</code></li>
  <li><code>display.text_overflow_visible : bool</code></li>
  <li><code>display.text_width_chars : u32</code></li>
  <li><code>display.format_kind : enum</code> — <code>text</code>, <code>numeric</code>, or <code>text_and_numeric</code></li>
  <li><code>display.format_string : string</code> when custom numeric display is exposed</li>
</ul>

<h3>8.6 Interaction</h3>

<ul>
  <li><code>interaction.visible : bool</code></li>
  <li><code>interaction.enabled : bool</code></li>
  <li><code>interaction.read_only : bool</code></li>
  <li><code>interaction.focusable : bool</code></li>
  <li><code>interaction.focused : bool</code></li>
  <li><code>interaction.opened : bool</code> when an opened selector posture exists</li>
</ul>

<h3>8.7 Authoring item-editing posture</h3>

<ul>
  <li><code>item_editing.allow_insert : bool</code></li>
  <li><code>item_editing.allow_delete : bool</code></li>
  <li><code>item_editing.allow_reorder : bool</code></li>
  <li><code>item_editing.allow_disable_items : bool</code></li>
  <li><code>item_editing.allow_undefined_runtime_values : bool</code></li>
</ul>

<h3>8.8 Style and realization</h3>

<ul>
  <li><code>style.frame.*</code></li>
  <li><code>style.value_face.*</code></li>
  <li><code>style.value_display.*</code></li>
  <li><code>style.selector_face.*</code></li>
  <li><code>style.digital_display.*</code></li>
  <li><code>style.focus_ring.*</code></li>
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
The Default realization should expose a rectangular enum ring/selector template with a value face, visible value text, selector affordance, optional increment/decrement buttons, optional digital display, optional overflow marker, caption and label surfaces, and a focus ring.
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
