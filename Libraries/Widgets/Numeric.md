<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Standard Numeric Widgets</h1>

<p align="center">
  <strong>Normative baseline for standardized numeric control and numeric indicator widget classes</strong><br/>
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
  <li><a href="../../Libraries/Realizations/Default/Numeric.md">Default numeric realization</a></li>
  <li><a href="../../Examples/05_bounded_ui_accumulator/NumericCompatibility.md">Example 05 numeric compatibility note</a></li>
</ul>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#classes-defined-here">2. Classes Defined Here</a></li>
  <li><a href="#numeric-class-versus-realization">3. Numeric Class versus Realization</a></li>
  <li><a href="#label-caption-and-value-text-posture">4. Label, Caption, and Value Text Posture</a></li>
  <li><a href="#common-family-posture">5. Common Family Posture</a></li>
  <li><a href="#placement-grid-and-resize-posture">5.1 Placement Grid and Resize Posture</a></li>
  <li><a href="#numeric-representation-model">6. Numeric Representation Model</a></li>
  <li><a href="#public-visual-part-model">7. Public Visual Part Model</a></li>
  <li><a href="#standard-property-surface">8. Standard Property Surface</a></li>
  <li><a href="#frogwidgetsnumeric_control">9. <code>frog.widgets.numeric_control</code></a></li>
  <li><a href="#frogwidgetsnumeric_indicator">10. <code>frog.widgets.numeric_indicator</code></a></li>
  <li><a href="#default-svg-realization-posture">11. Default SVG Realization Posture</a></li>
  <li><a href="#diagram-interaction-posture">12. Diagram Interaction Posture</a></li>
  <li><a href="#behavior-expectations">13. Behavior Expectations</a></li>
  <li><a href="#compatibility-with-example-05-flat-surface">14. Compatibility with Example 05 Flat Surface</a></li>
  <li><a href="#validation-expectations">15. Validation Expectations</a></li>
  <li><a href="#summary">16. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the intrinsic standardized baseline for numeric widgets in FROG.
</p>

<p>
The numeric family provides the standard widget surfaces used for numeric entry and numeric display.
It is richer than the boolean and string families because a numeric widget naturally combines scalar value semantics, representation selection, data-entry limits, formatting, radix presentation, optional unit labels, and optional increment/decrement interaction.
</p>

<p>
The standard numeric family is defined as a real object surface with:
</p>

<ul>
  <li>a primary numeric value posture,</li>
  <li>a portable representation model,</li>
  <li>a portable data-entry model,</li>
  <li>a portable display-format model,</li>
  <li>a minimal but real method surface,</li>
  <li>a minimal but real event surface,</li>
  <li>a stable public part model for realization targeting.</li>
</ul>

<p>
The design is intentionally analogous to mature graphical environments such as LabVIEW, while modernizing the public contract and preserving strict separation between class law, realization assets, runtime interpretation, and IDE tooling.
</p>

<hr/>

<h2 id="classes-defined-here">2. Classes Defined Here</h2>

<ul>
  <li><code>frog.widgets.numeric_control</code></li>
  <li><code>frog.widgets.numeric_indicator</code></li>
</ul>

<p>
Both classes share one numeric family posture.
The control is user-editable in the portable baseline.
The indicator is display-oriented and receives its value from diagram/runtime publication.
</p>

<hr/>

<h2 id="numeric-class-versus-realization">3. Numeric Class versus Realization</h2>

<p>
A numeric class is not the same thing as a host numeric text box, a spinbox, a fixed-point editor, a runtime-private field, or a SVG picture containing a number.
</p>

<p>
The class owns:
</p>

<ul>
  <li>the numeric value surface,</li>
  <li>the control-versus-indicator distinction,</li>
  <li>the representation model,</li>
  <li>the data-entry model,</li>
  <li>the display-format model,</li>
  <li>the public property inventory,</li>
  <li>the public method inventory,</li>
  <li>the public event inventory,</li>
  <li>the public part model.</li>
</ul>

<p>
The realization owns:
</p>

<ul>
  <li>the visible text box or numeric face geometry,</li>
  <li>increment/decrement button embodiment,</li>
  <li>optional radix surfaces when a realization variant explicitly publishes them,</li>
  <li>inline unit text rendered through <code>text_value</code> formatting when units are enabled,</li>
  <li>SVG assets or host-native drawing resources,</li>
  <li>state maps,</li>
  <li>style application,</li>
  <li>skin and variant selection.</li>
</ul>

<p>
A spinbox-like realization, a compact numeric indicator, or a radix-badge style does not automatically create a new numeric class.
A distinct standardized class should be introduced only when the public contract itself changes.
</p>

<hr/>

<h2 id="label-caption-and-value-text-posture">4. Label, Caption, and Value Text Posture</h2>

<p>
The numeric family follows the shared FROG label/caption convention and adds a numeric-specific value-text surface.
</p>

<pre><code>label.*
    - structural / logical widget name surface

caption.*
    - front-panel presentation caption shown near the numeric widget

value / text_value
    - semantic numeric value and its rendered text surface
</code></pre>

<ul>
  <li><code>label.text</code> names the object for tooling, references, diagram readability, accessibility, and debugging.</li>
  <li><code>caption.text</code> is the external front-panel caption shown near the numeric widget.</li>
  <li><code>value</code> is the semantic numeric value.</li>
  <li><code>text_value</code> is the rendered value text part owned by realization but fed by the class-owned <code>value</code>.</li>
</ul>

<hr/>

<h2 id="common-family-posture">5. Common Family Posture</h2>

<ul>
  <li>family: scalar numeric widget family</li>
  <li>primary value: present</li>
  <li>value kind: numeric scalar</li>
  <li>primary value mirror property: <code>value</code></li>
  <li>natural value participation: yes</li>
  <li>object-style access surface: yes</li>
  <li>structural label surface: <code>label.*</code></li>
  <li>front-panel caption surface: <code>caption.*</code></li>
  <li>representation surface: <code>representation.*</code></li>
  <li>data-entry surface: <code>data_entry.*</code></li>
  <li>display-format surface: <code>display.*</code></li>
  <li>optional inline unit text rendered through the chosen value-text formatting</li>
  <li>interaction surface: <code>interaction.*</code></li>
  <li>portable style surface: <code>style.*</code></li>
  <li>portable realization-selection surface: <code>realization.*</code></li>
</ul>

<hr/>

<h2 id="placement-grid-and-resize-posture">5.1 Placement Grid and Resize Posture</h2>

<p>
Numeric widgets participate in the shared FROG placement grid defined in
<a href="./PlacementGrid.md"><code>PlacementGrid.md</code></a>. The widget
remains autonomous: its skin, value face, spinner, text anchors, and resize
constraints are Numeric realization concerns. The front panel, IDE, and
container widgets consume the Numeric placement aura instead of inventing
runtime-private bounds.
</p>

<p>
The following placement law is specific to the Numeric family and the reviewed
Default Numeric compact realization. Other widget families must define their own
body, <code>aura_band</code>, resize, and focus-ring law instead of inheriting Numeric
dimensions.
</p>

<ul>
  <li>Default front-panel grid pitch: <code>16px</code> at 100% design zoom.</li>
  <li>Width may be resized manually to fit expected value display range.</li>
  <li>Height may grow from font size and display comfort requirements.</li>
  <li>The default compact proportions are designed to align cleanly to the common grid, but grid snap is a source-owned placement policy rather than an intrinsic Numeric class requirement.</li>
  <li>When the front panel or containing widget declares snap, the top-left of the placement aura snaps to the common grid.</li>
  <li>When the front panel or containing widget declares top-right snap or width quantization, the top-right of the Numeric placement aura also snaps to the common grid and <code>layout.width</code> is quantized as an integer multiple of <code>canvas.grid.pitch</code>.</li>
  <li>When horizontal resize is grid-quantized, the aura width moves from grid point to grid point while keeping the declared uniform <code>aura_band</code> around <code>main_body</code>.</li>
  <li>The distance between the visible skin and the placement aura remains declared and uniform for a given placement posture.</li>
  <li>The value face, control body, indicator body, and focus ring are stretchable surfaces.</li>
  <li>The increment/decrement spinner follows the source-owned side declaration: <code>display.increment_buttons_side</code>. It may be placed on the right, placed on the left, or hidden through <code>display.increment_buttons_visible</code>.</li>
  <li>The rendered value text stays right-aligned and vertically centered unless a source-owned style overrides it. Inline unit text, when enabled, is part of this rendered value text.</li>
  <li>The default caption anchor is high enough above <code>placement_bounds</code> that the IDE label aura does not overlap the body placement aura.</li>
</ul>

<p>
The placement aura is not the runtime focus ring and it is not
<code>main_body</code>. When a Numeric is contained by Array, the Array cell
consumes the Numeric placement posture. Array hover and selection cover the
cell; the Numeric focus ring appears only when the Numeric itself has editing
focus. In an IDE view, the host renders the selection aura from
<code>placement_bounds</code>. <code>control_body</code> and
<code>indicator_body</code> are the Numeric SVG mappings to the shared
<code>main_body</code> vocabulary. A label aura may be derived from
<code>caption</code>. These auras are runtime/IDE overlays, not additional SVG
public parts.
</p>

<p>
The placement contract is source-owned and applies whether the grid points are
drawn or hidden. <code>canvas.grid.visible=false</code> only hides the review
surface. It does not by itself enable or disable snap; strict Numeric
top-left/top-right validation and grid-pitch width quantization are active only
when the source or containing widget declares that policy. Array containment
still consumes <code>placement_bounds</code> as the portable contained-widget
surface.
</p>

<hr/>

<h2 id="numeric-representation-model">6. Numeric Representation Model</h2>

<p>
The canonical source-owned representation members are:
</p>

<pre><code>data_type.representation
data_type.named_numeric_size</code></pre>

<p>
The <code>data_type.representation</code> member is the machine-readable semantic carrier used by
the diagram, FIR, lowering, native artifact contract, runtime, and host UI binding. The
<code>data_type.named_numeric_size</code> member is the canonical human-readable name an IDE may
show in a palette or property dialog.
</p>

<p>
The canonical vocabulary, exact source tokens, parameterized forms, migration
rules, array propagation law, and reference visual type-family colors are
defined in
<a href="../../Expression/Numeric%20representations.md"><code>Expression/Numeric representations.md</code></a>.
The baseline contains 16 main representations and 9 advanced representations.
The advanced grouping is a discoverability tier, not a different semantic
class.
</p>

<p>
The standard Numeric widget default is <code>Float64</code> / <code>f64</code>.
It accepts negative and fractional values. A representation change is
source-owned and must propagate to the corresponding diagram terminal, typed
array terminal, and compatible interface-map binding.
</p>

<p>
Older reference material may still contain <code>representation.kind</code>. That member is a
compatibility alias for the same source-owned concept and should be normalized to
<code>data_type.representation</code> in new examples and runtime-facing artifacts. If both
members are present, they MUST identify the same representation.
</p>

<p>
Declaring a representation in the specification does not automatically validate broad runtime
behavior. A runtime may accept only the representation corridor proven by an example/native
artifact pair until additional representation examples are accepted.
</p>

<p>
When the active representation is fixed-point, the following additional members are relevant:
</p>

<ul>
  <li><code>representation.encoding</code> — <code>signed</code> or <code>unsigned</code></li>
  <li><code>representation.word_length_bits</code></li>
  <li><code>representation.integer_word_length_bits</code></li>
  <li><code>representation.include_overflow_status</code></li>
</ul>

<hr/>

<h2 id="public-visual-part-model">7. Public Visual Part Model</h2>

<ul>
  <li><code>root</code></li>
  <li><code>placement_bounds</code> - invisible placement aura: portable placement and containment rectangle, height equal to <code>main_body</code> plus declared <code>aura_band</code>; not focus and not the visible body. Top-left/top-right snap and grid-controlled width apply when source or container policy declares them.</li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>control_body</code> when present - Numeric control mapping to <code>main_body</code>.</li>
  <li><code>indicator_body</code> when present - Numeric indicator mapping to <code>main_body</code>.</li>
  <li><code>value_face</code></li>
  <li><code>text_value</code></li>
  <li><code>spinner</code> when present</li>
  <li><code>increment_up</code> when present</li>
  <li><code>increment_down</code> when present</li>
  <li><code>focus_ring</code> when present - widget focus interaction state, not placement aura and not Array cell selection.</li>
</ul>

<p>
The default compact Numeric realization intentionally has no separate public
<code>frame</code> part: the visible one-pixel border belongs to
<code>value_face</code>. Radix surfaces remain possible for future variants,
but they are not public parts of the reviewed compact default skin. Unit text
for the compact default is rendered inline inside <code>text_value</code>, not
as a separate <code>unit_label</code> part.
</p>

<p>
When the Default Numeric realization is embedded as a contained element, hosts
should align the cell to <code>placement_bounds</code> and preserve the
published compact proportions: the uniform aura band, value face width,
value-to-spinner gap, spinner width, separated increment/decrement button
height, and the vertical button gap.
For the reviewed compact default this is <code>73 x 24</code> for
<code>value_face</code>, <code>2px</code> value-to-spinner gap,
<code>13 x 11</code> for each increment/decrement button, and a
<code>2px</code> vertical button gap.
Those proportions are realization-owned geometry, not runtime-local widget drawing rules.
The default compact increment/decrement buttons intentionally share the same separated-button proportion
as the Default Array index display, mirrored to the right of the numeric value face.
</p>

<hr/>

<h2 id="standard-property-surface">8. Standard Property Surface</h2>

<h3>8.1 Value and representation</h3>

<ul>
  <li><code>value : numeric</code></li>
  <li><code>default_value : numeric</code> — semantic value used when a numeric control is reset or when another source-owned container materializes a new numeric element without an explicit value.</li>
  <li><code>data_type.representation : enum</code></li>
  <li><code>data_type.named_numeric_size : string</code></li>
  <li><code>data_type.fixed_point.encoding : enum</code> when applicable</li>
  <li><code>data_type.fixed_point.word_length_bits : u32</code> when applicable</li>
  <li><code>data_type.fixed_point.integer_word_length_bits : u32</code> when applicable</li>
  <li><code>data_type.fixed_point.include_overflow_status : bool</code> when applicable</li>
</ul>

<p>
The numeric default value is a semantic widget/class value, not a Default realization visual property.
A <code>.frog</code> widget instance or a source-owned container such as <code>frog.widgets.array</code> may bind a concrete <code>default_value</code>.
The Default <code>.wfrog</code> realization may publish the parts needed to display and edit the value, but it must not become the owner of the semantic default.
</p>

<h3>8.2 Label, caption, and inline unit text</h3>

<ul>
  <li><code>label.visible : bool</code></li>
  <li><code>label.text : string</code></li>
  <li><code>label.style.*</code></li>
  <li><code>caption.visible : bool</code></li>
  <li><code>caption.text : string</code></li>
  <li><code>caption.placement : enum</code></li>
  <li><code>caption.padding : length</code></li>
  <li><code>caption.style.*</code></li>
</ul>

<h3>8.3 Interaction</h3>

<ul>
  <li><code>interaction.visible : bool</code></li>
  <li><code>interaction.enabled : bool</code></li>
  <li><code>interaction.read_only : bool</code></li>
  <li><code>interaction.focusable : bool</code></li>
  <li><code>interaction.focused : bool</code></li>
</ul>

<h3>8.4 Data entry</h3>

<ul>
  <li><code>data_entry.use_type_defaults : bool</code></li>
  <li><code>data_entry.minimum : numeric</code> when a custom lower limit is present</li>
  <li><code>data_entry.maximum : numeric</code> when a custom upper limit is present</li>
  <li><code>data_entry.minimum_response : enum</code> — <code>ignore</code> or <code>coerce</code></li>
  <li><code>data_entry.maximum_response : enum</code> — <code>ignore</code> or <code>coerce</code></li>
  <li><code>data_entry.increment_step : numeric</code></li>
  <li><code>data_entry.increment_response : enum</code> — <code>ignore</code>, <code>nearest</code>, <code>up</code>, or <code>down</code></li>
  <li><code>data_entry.page_step : numeric</code></li>
</ul>

<p>
Data-entry policy applies to interactive user commits on a Numeric control. It
does not silently rewrite programmatic Diagram/runtime writes, property writes,
or indicator publication. The numeric representation remains authoritative for
every path: unsigned representations cannot store negative values and integral
representations store whole values.
</p>

<p>
The lower and upper responses are independent. <code>ignore</code> accepts an
out-of-range user value when the selected representation can store it.
<code>coerce</code> clamps that value to the corresponding configured boundary.
Changing a Data Entry property does not retroactively mutate the current value;
the policy is applied by a later interactive commit or by an explicit validation
operation.
</p>

<p>
Increment response quantizes user input on a grid whose origin is the custom
minimum when present, otherwise zero. <code>ignore</code> accepts off-grid input;
<code>nearest</code>, <code>up</code>, and <code>down</code> select the corresponding
grid point. An <code>increment_step</code> of zero disables custom quantization
without disabling the control or its natural increment/decrement interaction.
<code>page_step</code> is the standard FROG Page Up/Page Down interaction amount.
An exact configured minimum or maximum remains accessible when it is not aligned
with the increment grid; coercion to that boundary preserves the boundary value.
</p>

<pre><code>"data_entry.use_type_defaults": false,
"data_entry.minimum": 0,
"data_entry.maximum": 100,
"data_entry.minimum_response": "coerce",
"data_entry.maximum_response": "ignore",
"data_entry.increment_step": 1,
"data_entry.increment_response": "nearest",
"data_entry.page_step": 10</code></pre>

<h4>8.4.1 Interactive commit pipeline</h4>

<p>
An implementation MUST apply interactive Numeric input in the following order:
</p>

<ol>
  <li>parse the edit text without applying Display Format decoration,</li>
  <li>reject values that the selected numeric representation cannot store,</li>
  <li>apply configured minimum and maximum coercion independently,</li>
  <li>apply increment quantization when its response is not <code>ignore</code>,</li>
  <li>convert integral representations to a whole value using the selected rounding posture,</li>
  <li>reapply coercing limits so quantization cannot escape a required boundary,</li>
  <li>commit the typed value and then regenerate its visible text through Display Format.</li>
</ol>

<p>
The representation domain is unconditional. For example, a <code>u16</code>
control rejects negative input even when a custom minimum response is
<code>ignore</code>. Integral representations do not retain a fractional value:
<code>nearest</code> and the default posture round to the nearest integer,
<code>up</code> uses the next integer toward positive infinity, and
<code>down</code> uses the next integer toward negative infinity. Interactive
<code>NaN</code> and infinity text is rejected by the standard Numeric editor;
applications that exchange such floating values through dataflow MUST handle
them explicitly.
</p>

<h4>8.4.2 Defaults, roles, and failure behavior</h4>

<p>
When <code>data_entry.use_type_defaults</code> is true, custom Data Entry
members are ignored and the representation supplies its natural domain.
Signed integers use their signed bit range, unsigned integers start at zero,
and floating representations use their finite storage domain. The natural
increment/decrement interaction step is one unless a realization publishes a
more specific compatible step.
</p>

<p>
Data Entry governs Numeric controls because they accept interactive commits.
Numeric indicators may retain the properties for roundtrip and role switching,
but published indicator values are not coerced by them. Invalid settings, such
as a minimum greater than the maximum, a negative increment, a non-positive
page step, or a custom bound outside the representation domain, MUST prevent the
settings transaction from being accepted and MUST leave the widget unchanged.
</p>

<h3>8.5 Display format</h3>

<ul>
  <li><code>display.format.kind : enum</code> - <code>automatic</code>, <code>fixed</code>, <code>scientific</code>, <code>engineering</code>, <code>si</code>, <code>decimal</code>, <code>hexadecimal</code>, <code>binary</code>, <code>octal</code>, or <code>custom</code></li>
  <li><code>display.format.digits : u32</code> - clamped to <code>1..18</code></li>
  <li><code>display.format.precision_type : enum</code> - <code>fractional</code> or <code>significant</code></li>
  <li><code>display.format.hide_trailing_zeros : bool</code></li>
  <li><code>display.format.exponent_multiple_of_3 : bool</code></li>
  <li><code>display.format.minimum_field_width : u32</code> - minimum only, never a truncation width</li>
  <li><code>display.format.padding : enum</code> - <code>none</code>, <code>spaces</code>, or <code>zeros</code></li>
  <li><code>display.format.alignment : enum</code> - <code>left</code> or <code>right</code></li>
  <li><code>display.format.sign : enum</code> - <code>negative_only</code>, <code>always</code>, or <code>space_positive</code></li>
  <li><code>display.format.locale : enum</code> - <code>invariant</code> or <code>system</code></li>
  <li><code>display.format.prefix : string</code> - decorative text before the formatted value</li>
  <li><code>display.format.suffix : string</code> - decorative text after the formatted value</li>
  <li><code>display.format.radix.show_prefix : bool</code></li>
  <li><code>display.format.radix.uppercase : bool</code></li>
  <li><code>display.format.radix.group_size : u32</code> - <code>0..8</code>, where zero disables grouping</li>
  <li><code>display.format.radix.interpretation : enum</code> - <code>numeric</code> or <code>raw_bits</code></li>
  <li><code>display.format.custom_string : string</code> - expert/import format with one safe conversion</li>
  <li><code>display.increment_buttons_visible : bool</code></li>
  <li><code>display.increment_buttons_side : enum</code> - <code>right</code> or <code>left</code> when increment/decrement buttons are visible.</li>
  <li><code>display.unit_visible : bool</code> - when supported by the realization, render the unit inside <code>text_value</code>.</li>
  <li><code>display.unit_suffix : string</code> - optional unit suffix included in the formatted value text.</li>
  <li><code>display.text_width_chars : u32</code></li>
</ul>

<p>
Display Format is a presentation-only contract. It must not change the numeric
representation, stored value, calculation precision, Data Entry policy, Diagram
terminal type, or binding color. A Numeric control keeps raw in-progress edit
text separate from its committed typed value; the formatted text is restored
after a successful commit. Numeric indicators format every published value
without rewriting it.
</p>

<p>
<code>fractional</code> precision counts digits after the decimal separator.
<code>significant</code> precision counts meaningful digits across the complete
value. <code>exponent_multiple_of_3</code> converts scientific output to
engineering posture. <code>si</code> uses standard SI exponent prefixes when the
exponent is representable and falls back to engineering notation otherwise.
</p>

<p>
Radix formatting is available for integral representations. In
<code>numeric</code> interpretation a signed value retains its sign. In
<code>raw_bits</code> interpretation the complete storage-width bit pattern is
shown; hexadecimal, octal, and binary digits are padded to the selected numeric
representation width before optional grouping. Source JSON and executable
artifacts remain locale-independent even when <code>locale=system</code> changes
the visible decimal separator.
</p>

<p>
The expert format accepts one conversion among <code>f</code>, <code>e</code>,
<code>g</code>, <code>p</code>, <code>d</code>, <code>u</code>, <code>x</code>,
<code>X</code>, <code>o</code>, and <code>b</code>. It supports the safe modifiers
<code>-</code>, <code>+</code>, <code>#</code>, <code>^</code>, <code>0</code>,
minimum width, <code>.N</code> fractional precision, <code>_N</code> significant
precision, and <code>%%</code> literals. Additional conversions are rejected.
Timestamp and Duration are distinct typed widget families and are not Numeric
Display Format variants.
</p>

<pre><code>"display.format.kind": "si",
"display.format.digits": 4,
"display.format.precision_type": "significant",
"display.format.hide_trailing_zeros": true,
"display.format.exponent_multiple_of_3": false,
"display.format.minimum_field_width": 0,
"display.format.padding": "none",
"display.format.alignment": "right",
"display.format.sign": "negative_only",
"display.format.locale": "system",
"display.format.prefix": "",
"display.format.suffix": " V",
"display.format.radix.show_prefix": false,
"display.format.radix.uppercase": true,
"display.format.radix.group_size": 0,
"display.format.radix.interpretation": "numeric",
"display.format.custom_string": "%.6g"</code></pre>

<p>
Typed Array cells inherit the Display Format contract from their contained
Numeric element template. The Array does not replace the Numeric formatter with
a generic cell formatter.
</p>

<h4>8.5.1 Formatting pipeline</h4>

<p>
The standard formatting pipeline is value preserving:
</p>

<ol>
  <li>read the committed typed value,</li>
  <li>select notation and precision rules compatible with its representation,</li>
  <li>apply sign, radix, case, and grouping rules,</li>
  <li>apply minimum width, alignment, and padding without truncation,</li>
  <li>apply decorative prefix and suffix text,</li>
  <li>publish the resulting display string.</li>
</ol>

<p>
The formatter MUST NOT feed its rounded or decorated text back into the stored
numeric value. A live preview in an editor MUST use the same formatter as the
Front Panel realization. Changing locale affects only visible separators;
serialized <code>.frog</code> numeric literals remain invariant. Timestamp and
Duration formatting are outside this Numeric contract.
</p>

<h3>8.6 Key bindings</h3>

<ul>
  <li><code>key_binding.focus : string</code> when exposed</li>
  <li><code>key_binding.increment : string</code> when exposed</li>
  <li><code>key_binding.decrement : string</code> when exposed</li>
</ul>

<h3>8.7 Style and realization</h3>

<ul>
  <li><code>style.value_face.*</code></li>
  <li><code>style.text_value.*</code></li>
  <li><code>style.spinner.*</code></li>
  <li><code>style.focus_ring.*</code></li>
  <li><code>style.disabled.opacity : number</code></li>
  <li><code>realization.family : string</code></li>
  <li><code>realization.variant : string</code></li>
  <li><code>realization.skin_id : string</code></li>
</ul>

<hr/>

<h2 id="frogwidgetsnumeric_control">9. <code>frog.widgets.numeric_control</code></h2>

<h3>9.1 Class identity</h3>

<ul>
  <li><strong>class_id:</strong> <code>frog.widgets.numeric_control</code></li>
  <li><strong>family:</strong> <code>numeric_widget</code></li>
  <li><strong>compatible role:</strong> <code>control</code></li>
</ul>

<h3>9.2 Standard methods</h3>

<ul>
  <li><code>focus()</code></li>
  <li><code>select_text()</code></li>
  <li><code>set_value(number)</code></li>
  <li><code>increment()</code></li>
  <li><code>decrement()</code></li>
  <li><code>clamp_to_limits()</code></li>
  <li><code>format_value()</code></li>
  <li><code>parse_and_commit(text)</code></li>
  <li><code>reset_to_default()</code></li>
</ul>

<h3>9.3 Standard events</h3>

<ul>
  <li><code>value_changed</code></li>
  <li><code>value_committed</code></li>
  <li><code>edit_started</code></li>
  <li><code>edit_committed</code></li>
  <li><code>out_of_range</code></li>
  <li><code>increment_pressed</code></li>
  <li><code>decrement_pressed</code></li>
  <li><code>focus_gained</code></li>
  <li><code>focus_lost</code></li>
</ul>

<hr/>

<h2 id="frogwidgetsnumeric_indicator">10. <code>frog.widgets.numeric_indicator</code></h2>

<h3>10.1 Class identity</h3>

<ul>
  <li><strong>class_id:</strong> <code>frog.widgets.numeric_indicator</code></li>
  <li><strong>family:</strong> <code>numeric_widget</code></li>
  <li><strong>compatible role:</strong> <code>indicator</code></li>
</ul>

<h3>10.2 Standard methods</h3>

<ul>
  <li><code>focus()</code> when supported by the host</li>
  <li><code>format_value()</code></li>
  <li><code>reset_to_default_style()</code></li>
</ul>

<h3>10.3 Standard events</h3>

<ul>
  <li><code>value_rendered</code></li>
  <li><code>focus_gained</code></li>
  <li><code>focus_lost</code></li>
</ul>

<hr/>

<h2 id="default-svg-realization-posture">11. Default SVG Realization Posture</h2>

<p>
The default numeric realization SHOULD support a rectangular SVG template with:
</p>

<ul>
  <li>a value face,</li>
  <li>dynamic value text,</li>
  <li>optional increment/decrement buttons,</li>
  <li>label and caption surfaces,</li>
  <li>focus-ring posture.</li>
</ul>

<hr/>

<h2 id="diagram-interaction-posture">12. Diagram Interaction Posture</h2>

<p>
The numeric family supports natural value participation through <code>widget_value</code>, property access through <code>frog.ui.property_read</code> and <code>frog.ui.property_write</code>, method invocation through <code>frog.ui.method_invoke</code>, and event observation where legal.
</p>

<hr/>

<h2 id="behavior-expectations">13. Behavior Expectations</h2>

<ul>
  <li>numeric controls accept user-originated editing only when enabled and not read-only,</li>
  <li>interactive commits respect the selected representation and <code>data_entry.*</code> policies,</li>
  <li>increment and decrement use a positive <code>data_entry.increment_step</code>, or their natural step when the configured step is zero,</li>
  <li>programmatic dataflow writes bypass Data Entry policy while remaining subject to representation validity,</li>
  <li>formatting follows <code>display.*</code>,</li>
  <li>representation changes must preserve or explicitly convert numeric meaning according to the active validation posture.</li>
</ul>

<hr/>

<h2 id="compatibility-with-example-05-flat-surface">14. Compatibility with Example 05 Flat Surface</h2>

<p>
Example 05 was intentionally frozen as an executable corridor and uses a bounded flat numeric surface for its acceptance path.
That frozen surface should not be silently rewritten by the general numeric baseline.
</p>

<p>
The compatibility mapping is:
</p>

<ul>
  <li><code>label</code> maps to <code>caption.text</code> or <code>label.text</code> depending on the host compatibility mode.</li>
  <li><code>visible</code> maps to <code>interaction.visible</code>.</li>
  <li><code>enabled</code> maps to <code>interaction.enabled</code>.</li>
  <li><code>foreground_color</code> maps to a compatible text or foreground style surface in the runtime-family corridor.</li>
</ul>

<p>
New numeric work should use the canonical hierarchical surface defined here.
The frozen Example 05 runtime-family acceptance path may continue to preserve its existing compatibility surface.
</p>

<hr/>

<h2 id="validation-expectations">15. Validation Expectations</h2>

<p>
Validators SHOULD diagnose at least:
</p>

<ul>
  <li>non-numeric values on numeric widgets,</li>
  <li>unknown <code>data_type.representation</code> values,</li>
  <li>fixed-point members used without <code>data_type.representation = fxp</code>,</li>
  <li>runtime-facing examples whose native artifact ABI type does not match the source-owned numeric representation,</li>
  <li>minimum greater than maximum,</li>
  <li>negative increment steps,</li>
  <li>custom limits outside the selected representation domain,</li>
  <li>unsupported public members,</li>
  <li>attempts to treat realization-only spinner internals as public class members.</li>
</ul>

<hr/>

<h2 id="summary">16. Summary</h2>

<p>
The numeric widget family defines the intrinsic standardized numeric baseline of FROG:
</p>

<ul>
  <li><code>frog.widgets.numeric_control</code></li>
  <li><code>frog.widgets.numeric_indicator</code></li>
</ul>

<p>
It standardizes numeric value, representation, data-entry limits, display format, optional increment/decrement interaction, and a realization-ready public part model while preserving the class-versus-realization boundary. Optional unit and radix surfaces are variant-level extensions unless the selected realization publishes them explicitly.
</p>
