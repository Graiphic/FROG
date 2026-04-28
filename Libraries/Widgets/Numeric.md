<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Standard Numeric Widgets</h1>

<p align="center">
  <strong>Normative baseline for standardized numeric control and numeric indicator widget classes</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#classes-defined-here">2. Classes Defined Here</a></li>
  <li><a href="#common-family-posture">3. Common Family Posture</a></li>
  <li><a href="#label-and-caption-posture">4. Label and Caption Posture</a></li>
  <li><a href="#numeric-value-and-representation-posture">5. Numeric Value and Representation Posture</a></li>
  <li><a href="#frogwidgetsnumeric_control">6. <code>frog.widgets.numeric_control</code></a></li>
  <li><a href="#frogwidgetsnumeric_indicator">7. <code>frog.widgets.numeric_indicator</code></a></li>
  <li><a href="#common-parts">8. Common Parts</a></li>
  <li><a href="#common-behavior-expectations">9. Common Behavior Expectations</a></li>
  <li><a href="#common-realization-expectations">10. Common Realization Expectations</a></li>
  <li><a href="#diagram-interaction-posture">11. Diagram Interaction Posture</a></li>
  <li><a href="#validation-expectations">12. Validation Expectations</a></li>
  <li><a href="#summary">13. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the intrinsic standardized baseline for numeric widgets in FROG.
</p>

<p>
The numeric family provides the standard widget surfaces used for numeric entry and numeric display.
The family is intended to cover the common scalar-number posture of mature graphical systems while remaining inspectable, portable, and implementation-neutral.
</p>

<p>
The standard numeric family is therefore defined here as a real object surface with:
</p>

<ul>
  <li>a primary numeric value posture,</li>
  <li>a published representation posture,</li>
  <li>a minimal but real property surface,</li>
  <li>a minimal but real method surface,</li>
  <li>a minimal but real event surface,</li>
  <li>a stable public part model for realization targeting.</li>
</ul>

<p>
This keeps the intrinsic baseline close in spirit to LabVIEW-like numeric widgets while modernizing the representation model and the public contract.
</p>

<hr/>

<h2 id="classes-defined-here">2. Classes Defined Here</h2>

<p>
This document defines the following standardized widget classes:
</p>

<ul>
  <li><code>frog.widgets.numeric_control</code></li>
  <li><code>frog.widgets.numeric_indicator</code></li>
</ul>

<hr/>

<h2 id="common-family-posture">3. Common Family Posture</h2>

<p>
The numeric family has the following common posture:
</p>

<ul>
  <li>family: scalar numeric widget family</li>
  <li>primary value: present</li>
  <li>value kind: numeric scalar</li>
  <li>public value-facing surface: yes</li>
  <li>object-style access surface: yes</li>
  <li>primary value mirror property: <code>value</code></li>
  <li>common label property: <code>label.text</code></li>
  <li>common caption property: <code>caption.text</code></li>
  <li>common visibility property: <code>interaction.visible</code></li>
</ul>

<p>
The numeric family also follows an important architectural rule:
</p>

<ul>
  <li><code>value</code> is class-owned semantic numeric data,</li>
  <li><code>representation.kind</code> selects the published numeric representation posture,</li>
  <li><code>text_value</code> is a stable public dynamic part,</li>
  <li><code>spinner</code> is a stable public optional interaction part,</li>
  <li>the visual embodiment of face, buttons, arrows, badges, or text layout belongs downstream to realization.</li>
</ul>

<hr/>

<h2 id="label-and-caption-posture">4. Label and Caption Posture</h2>

<p>
Numeric widgets follow the shared distinction between <code>label</code> and <code>caption</code>.
</p>

<ul>
  <li><code>label.*</code> is the structural or logical widget-identification surface.</li>
  <li><code>caption.*</code> is the front-panel user-facing text surface.</li>
</ul>

<p>
A realization MAY display both, one, or neither according to their visibility properties.
However, the semantic owner of these surfaces remains the class-owned public properties.
</p>

<hr/>

<h2 id="numeric-value-and-representation-posture">5. Numeric Value and Representation Posture</h2>

<p>
The intrinsic numeric family uses one semantic value property named <code>value</code> and one published representation posture named <code>representation.kind</code>.
</p>

<p>
The standard representation kinds are:
</p>

<ul>
  <li><code>int8</code></li>
  <li><code>int16</code></li>
  <li><code>int32</code></li>
  <li><code>int64</code></li>
  <li><code>uint8</code></li>
  <li><code>uint16</code></li>
  <li><code>uint32</code></li>
  <li><code>uint64</code></li>
  <li><code>float32</code></li>
  <li><code>float64</code></li>
  <li><code>decimal64</code></li>
  <li><code>decimal128</code></li>
  <li><code>fixed_point_signed</code></li>
  <li><code>fixed_point_unsigned</code></li>
  <li><code>complex64</code></li>
  <li><code>complex128</code></li>
</ul>

<p>
Profiles, IDEs, and runtimes MAY expose ergonomic aliases such as <code>I32</code>, <code>DBL</code>, <code>SGL</code>, or <code>FXP</code>.
However, the public standard representation contract should remain the explicit names above.
</p>

<p>
When the active representation is fixed-point, the following additional public members become relevant:
</p>

<ul>
  <li><code>representation.encoding</code> with values <code>signed</code> or <code>unsigned</code></li>
  <li><code>representation.word_length_bits</code></li>
  <li><code>representation.integer_word_length_bits</code></li>
  <li><code>representation.include_overflow_status</code></li>
</ul>

<hr/>

<h2 id="frogwidgetsnumeric_control">6. <code>frog.widgets.numeric_control</code></h2>

<h3>6.1 Class identity</h3>

<ul>
  <li><strong>class_id:</strong> <code>frog.widgets.numeric_control</code></li>
  <li><strong>family:</strong> <code>numeric_widget</code></li>
  <li><strong>compatible role:</strong> <code>control</code></li>
</ul>

<h3>6.2 Primary value posture</h3>

<ul>
  <li>primary value: present</li>
  <li>natural value participation: yes</li>
  <li>user-mutable: yes</li>
  <li>diagram-mutable: yes</li>
  <li>mirrored property: <code>value</code></li>
</ul>

<h3>6.3 Standard properties</h3>

<ul>
  <li><code>value</code> — readable and writable</li>
  <li><code>label.text</code> — readable and writable</li>
  <li><code>label.visible</code> — readable and writable</li>
  <li><code>caption.text</code> — readable and writable</li>
  <li><code>caption.visible</code> — readable and writable</li>
  <li><code>interaction.enabled</code> — readable and writable</li>
  <li><code>interaction.visible</code> — readable and writable</li>
  <li><code>interaction.read_only</code> — readable and writable</li>
  <li><code>representation.kind</code> — readable and writable</li>
  <li><code>representation.encoding</code> — readable and writable when applicable</li>
  <li><code>representation.word_length_bits</code> — readable and writable when applicable</li>
  <li><code>representation.integer_word_length_bits</code> — readable and writable when applicable</li>
  <li><code>representation.include_overflow_status</code> — readable and writable when applicable</li>
  <li><code>data_entry.minimum</code> — readable and writable</li>
  <li><code>data_entry.maximum</code> — readable and writable</li>
  <li><code>data_entry.increment_step</code> — readable and writable</li>
  <li><code>data_entry.coerce_on_commit</code> — readable and writable</li>
  <li><code>data_entry.response_to_out_of_range</code> — readable and writable</li>
  <li><code>display.format_kind</code> — readable and writable</li>
  <li><code>display.format_string</code> — readable and writable</li>
  <li><code>display.precision_digits</code> — readable and writable</li>
  <li><code>display.radix_visible</code> — readable and writable</li>
  <li><code>display.increment_buttons_visible</code> — readable and writable</li>
  <li><code>unit_label.text</code> — readable and writable</li>
  <li><code>unit_label.visible</code> — readable and writable</li>
  <li><code>portable style.*</code> surfaces when exposed by the class or active profile</li>
  <li><code>realization.family</code> when realization selection is publicly exposed</li>
  <li><code>realization.variant</code> when realization selection is publicly exposed</li>
  <li><code>realization.skin_id</code> when realization selection is publicly exposed</li>
</ul>

<h3>6.4 Standard methods</h3>

<ul>
  <li><code>focus()</code></li>
  <li><code>select_text()</code></li>
  <li><code>increment()</code></li>
  <li><code>decrement()</code></li>
  <li><code>set_value(number)</code></li>
  <li><code>clamp_to_limits()</code></li>
  <li><code>reset_to_default()</code> when a default value exists</li>
  <li><code>parse_and_commit(text)</code></li>
</ul>

<h3>6.5 Standard events</h3>

<ul>
  <li><code>value_changed</code></li>
  <li><code>value_committed</code></li>
  <li><code>increment_pressed</code></li>
  <li><code>decrement_pressed</code></li>
  <li><code>limits_changed</code></li>
  <li><code>focus_gained</code></li>
  <li><code>focus_lost</code></li>
</ul>

<h3>6.6 Standard parts</h3>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>frame</code></li>
  <li><code>value_face</code></li>
  <li><code>text_value</code></li>
  <li><code>spinner</code> when present</li>
  <li><code>increment_up</code> when present</li>
  <li><code>increment_down</code> when present</li>
  <li><code>unit_label</code> when present</li>
  <li><code>focus_ring</code> when present</li>
</ul>

<hr/>

<h2 id="frogwidgetsnumeric_indicator">7. <code>frog.widgets.numeric_indicator</code></h2>

<h3>7.1 Class identity</h3>

<ul>
  <li><strong>class_id:</strong> <code>frog.widgets.numeric_indicator</code></li>
  <li><strong>family:</strong> <code>numeric_widget</code></li>
  <li><strong>compatible role:</strong> <code>indicator</code></li>
</ul>

<h3>7.2 Primary value posture</h3>

<ul>
  <li>primary value: present</li>
  <li>natural value participation: yes</li>
  <li>user-mutable: no in the standard portable posture</li>
  <li>diagram-mutable: yes</li>
  <li>mirrored property: <code>value</code></li>
</ul>

<h3>7.3 Standard properties</h3>

<ul>
  <li><code>value</code> — readable and writable for diagram/runtime update surfaces where legal</li>
  <li><code>label.text</code> — readable and writable</li>
  <li><code>label.visible</code> — readable and writable</li>
  <li><code>caption.text</code> — readable and writable</li>
  <li><code>caption.visible</code> — readable and writable</li>
  <li><code>interaction.visible</code> — readable and writable</li>
  <li><code>representation.kind</code> — readable and writable</li>
  <li><code>display.format_kind</code> — readable and writable</li>
  <li><code>display.format_string</code> — readable and writable</li>
  <li><code>display.precision_digits</code> — readable and writable</li>
  <li><code>display.radix_visible</code> — readable and writable</li>
  <li><code>unit_label.text</code> — readable and writable</li>
  <li><code>unit_label.visible</code> — readable and writable</li>
  <li><code>portable style.*</code> surfaces when exposed by the class or active profile</li>
  <li><code>realization.family</code> when realization selection is publicly exposed</li>
  <li><code>realization.variant</code> when realization selection is publicly exposed</li>
  <li><code>realization.skin_id</code> when realization selection is publicly exposed</li>
</ul>

<h3>7.4 Standard methods</h3>

<ul>
  <li><code>focus()</code> when supported by the host</li>
  <li><code>format_value()</code></li>
  <li><code>reset_to_default()</code> when a default numeric value exists and the active class posture exposes it</li>
</ul>

<h3>7.5 Standard events</h3>

<ul>
  <li><code>value_rendered</code></li>
  <li><code>focus_gained</code></li>
  <li><code>focus_lost</code></li>
</ul>

<h3>7.6 Standard parts</h3>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>frame</code></li>
  <li><code>value_face</code></li>
  <li><code>text_value</code></li>
  <li><code>unit_label</code> when present</li>
  <li><code>focus_ring</code> when present</li>
</ul>

<hr/>

<h2 id="common-parts">8. Common Parts</h2>

<p>
The numeric family uses the following common stable parts:
</p>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>frame</code></li>
  <li><code>value_face</code></li>
  <li><code>text_value</code></li>
  <li><code>spinner</code> when present</li>
  <li><code>increment_up</code> when present</li>
  <li><code>increment_down</code> when present</li>
  <li><code>unit_label</code> when present</li>
  <li><code>focus_ring</code> when present</li>
</ul>

<hr/>

<h2 id="common-behavior-expectations">9. Common Behavior Expectations</h2>

<p>
The intrinsic behavior baseline of the numeric family includes at least:
</p>

<ul>
  <li>the primary value remains numeric,</li>
  <li>representation changes preserve numeric meaning while changing interpretation and formatting posture,</li>
  <li>numeric controls accept user-originated editing only when enabled and not read-only,</li>
  <li>increment and decrement actions use <code>data_entry.increment_step</code>,</li>
  <li>commits respect the configured out-of-range posture,</li>
  <li>indicator realizations may emit <code>value_rendered</code> when their visible state is refreshed.</li>
</ul>

<hr/>

<h2 id="common-realization-expectations">10. Common Realization Expectations</h2>

<p>
A conforming realization of the numeric family SHOULD provide:
</p>

<ul>
  <li>a visible numeric value surface,</li>
  <li>optional visible label support,</li>
  <li>optional visible caption support,</li>
  <li>optional visible unit-label support,</li>
  <li>optional visible radix support,</li>
  <li>optional increment/decrement buttons for controls,</li>
  <li>part-to-visual mapping for the published parts.</li>
</ul>

<p>
The realization MAY be SVG-backed, host-native, toolkit-driven, template-driven, or mixed.
It MUST NOT change the published class meaning.
</p>

<hr/>

<h2 id="diagram-interaction-posture">11. Diagram Interaction Posture</h2>

<p>
The numeric family supports:
</p>

<ul>
  <li>natural value participation through <code>widget_value</code>,</li>
  <li>property access through <code>frog.ui.property_read</code> and <code>frog.ui.property_write</code>,</li>
  <li>method invocation where legal,</li>
  <li>event observation where legal.</li>
</ul>

<p>
Typical legal object-style surfaces include:
</p>

<ul>
  <li><code>value</code></li>
  <li><code>label.*</code></li>
  <li><code>caption.*</code></li>
  <li><code>representation.*</code></li>
  <li><code>data_entry.*</code></li>
  <li><code>display.*</code></li>
  <li><code>unit_label.*</code></li>
  <li><code>interaction.*</code></li>
  <li>portable <code>style.*</code> properties when publicly exposed</li>
  <li>realization-selection members when publicly exposed</li>
</ul>

<hr/>

<h2 id="validation-expectations">12. Validation Expectations</h2>

<p>
Validators SHOULD diagnose at least:
</p>

<ul>
  <li>non-numeric <code>value_type</code> on numeric widgets,</li>
  <li>role/class mismatches,</li>
  <li>unknown representation kinds,</li>
  <li>fixed-point members used without a fixed-point representation kind,</li>
  <li>minimum greater than maximum,</li>
  <li>non-positive increment steps where the control posture requires a positive step,</li>
  <li>attempts to write user-edit surfaces on indicator-only classes where forbidden,</li>
  <li>unknown numeric family members or parts.</li>
</ul>

<hr/>

<h2 id="summary">13. Summary</h2>

<p>
The numeric widget family defines the intrinsic standardized numeric widget baseline of FROG:
</p>

<ul>
  <li><code>frog.widgets.numeric_control</code></li>
  <li><code>frog.widgets.numeric_indicator</code></li>
</ul>

<p>
These classes provide the standard portable numeric interaction and display surfaces of the reusable widget core.
They expose a real minimal object surface with properties, methods, events, and parts while keeping realization ownership and runtime-private embodiment clearly separated from class meaning.
</p>
