<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Standard Boolean Widgets</h1>

<p align="center">
  <strong>Normative baseline for standardized boolean control and boolean indicator widget classes</strong><br/>
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
  <li><a href="../../Profiles/UI%20Widget%20Classes.md">Profile posture for widget-class adoption</a></li>
</ul>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#classes-defined-here">2. Classes Defined Here</a></li>
  <li><a href="#boolean-class-versus-realization">3. Boolean Class versus Realization</a></li>
  <li><a href="#label-caption-and-state-text-posture">4. Label, Caption, and State Text Posture</a></li>
  <li><a href="#common-family-posture">5. Common Family Posture</a></li>
  <li><a href="#public-visual-part-model">6. Public Visual Part Model</a></li>
  <li><a href="#standard-property-surface">7. Standard Property Surface</a></li>
  <li><a href="#frogwidgetsboolean_control">8. <code>frog.widgets.boolean_control</code></a></li>
  <li><a href="#frogwidgetsboolean_indicator">9. <code>frog.widgets.boolean_indicator</code></a></li>
  <li><a href="#default-svg-realization-posture">10. Default SVG Realization Posture</a></li>
  <li><a href="#rectangular-and-circular-realization-variants">11. Rectangular and Circular Realization Variants</a></li>
  <li><a href="#diagram-interaction-posture">12. Diagram Interaction Posture</a></li>
  <li><a href="#behavior-expectations">13. Behavior Expectations</a></li>
  <li><a href="#validation-expectations">14. Validation Expectations</a></li>
  <li><a href="#compatibility-with-bounded-flat-slices">15. Compatibility with Bounded Flat Slices</a></li>
  <li><a href="#summary">16. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the intrinsic standardized baseline for boolean widgets in FROG.
</p>

<p>
The boolean family provides the standard widget surfaces used for true/false interaction and true/false display.
It is intentionally small, portable, inspectable, and strong enough to serve as part of the reusable front-panel baseline.
</p>

<p>
The intrinsic boolean baseline is strictly true/false.
It does not standardize a tri-state boolean model in the intrinsic core.
</p>

<p>
This file defines the boolean class law.
It does not make a particular SVG asset, host toolkit, runtime implementation, or IDE editor operation the semantic owner of the widget.
</p>

<hr/>

<h2 id="classes-defined-here">2. Classes Defined Here</h2>

<ul>
  <li><code>frog.widgets.boolean_control</code></li>
  <li><code>frog.widgets.boolean_indicator</code></li>
</ul>

<hr/>

<h2 id="boolean-class-versus-realization">3. Boolean Class versus Realization</h2>

<p>
A boolean class is not the same thing as its visible embodiment.
</p>

<p>
The class owns the boolean value meaning, the control-versus-indicator distinction, the public property inventory, the public method inventory, the public event inventory, the public part model, and the legality of object-style access through <code>frog.ui.*</code>.
</p>

<p>
The realization owns rectangular, circular, checkbox-like, switch-like, toggle-like, LED-like, or other visual embodiment choices, SVG assets or host-native drawing resources, part-to-visual bindings, state-to-visual mappings, text placement and anchoring, skin resources, and compatible visual variants.
</p>

<p>
A different shape does not automatically create a different boolean class.
A rectangular boolean and a circular boolean remain <code>frog.widgets.boolean_control</code> or <code>frog.widgets.boolean_indicator</code> as long as their public contract remains the same.
</p>

<p>
The rectangular Boolean realization is a Boolean value-control / value-indicator skin.
It MUST NOT be treated as the standardized Button command widget; Button command behavior is defined separately by <code>frog.widgets.button</code>.
</p>

<hr/>

<h2 id="label-caption-and-state-text-posture">4. Label, Caption, and State Text Posture</h2>

<p>
The boolean family follows the shared FROG label/caption convention and adds a boolean-specific internal state text surface.
</p>

<pre><code>label.*
    - structural / logical widget name surface

caption.*
    - front-panel presentation caption shown near the widget

state_text.*
    - optional text displayed inside the boolean face for true/false values
</code></pre>

<p>
This distinction prevents ambiguity between the text naming the widget, the user-facing caption, and the text displayed as part of the true/false embodiment.
</p>

<hr/>

<h2 id="common-family-posture">5. Common Family Posture</h2>

<ul>
  <li>family: scalar boolean widget family</li>
  <li>primary value: present</li>
  <li>value type: <code>bool</code></li>
  <li>primary value mirror property: <code>value</code></li>
  <li>natural value participation: yes</li>
  <li>object-style access surface: yes</li>
  <li>structural label surface: <code>label.*</code></li>
  <li>front-panel caption surface: <code>caption.*</code></li>
  <li>internal state text surface: <code>state_text.*</code></li>
  <li>interaction surface: <code>interaction.*</code></li>
  <li>portable style surface: <code>style.*</code></li>
  <li>portable realization-selection surface: <code>realization.*</code></li>
</ul>

<p>
The boolean family follows these architectural rules:
</p>

<ul>
  <li><code>value</code> is class-owned semantic boolean data.</li>
  <li><code>label.text</code> is class-owned logical widget-name text.</li>
  <li><code>caption.text</code> is class-owned front-panel display caption text.</li>
  <li><code>state_text.true_text</code> and <code>state_text.false_text</code> are optional state text surfaces, not the semantic source of the boolean value.</li>
  <li><code>state_face</code> is the Boolean SVG mapping to the shared <code>main_body</code> vocabulary.</li>
  <li><code>placement_bounds</code> is the non-rendered placement aura used by IDEs and container widgets.</li>
  <li><code>aura_band</code> is the uniform spacing between <code>placement_bounds</code> and <code>main_body</code>.</li>
  <li>The control click edits the Boolean control value. It does not execute a diagram by itself; execution remains an explicit host action unless an event structure is declared later.</li>
</ul>

<hr/>

<h2 id="public-visual-part-model">6. Public Visual Part Model</h2>

<ul>
  <li><code>root</code> - root widget surface and hit-test container,</li>
  <li><code>label</code> - structural label surface when displayed by tooling or host,</li>
  <li><code>caption</code> - front-panel user-facing caption surface,</li>
  <li><code>placement_bounds</code> - non-rendered placement aura for IDE selection, sizing, and container fit,</li>
  <li><code>state_face</code> - Boolean mapping to <code>main_body</code>, the visible true/false value face,</li>
  <li><code>state_text</code> - optional internal true/false text surface,</li>
  <li><code>focus_ring</code> - optional focus visualization surface following <code>state_face</code>.</li>
</ul>

<p>
For the Default Boolean realization, <code>focus_ring</code> follows the visible
<code>state_face</code> boundary. It is a styleable outline, not the larger
<code>placement_bounds</code> aura and not an IDE selection envelope.
</p>

<p>
The Default Boolean realization uses the shared placement vocabulary:
<code>placement_bounds</code> is a 72 x 72 source-unit aura,
<code>state_face</code> is the Boolean mapping to a 64 x 64
<code>main_body</code>, and <code>aura_band</code> is 4 source units on each
side. That proportion belongs to this realization. It must not be generalized
to Numeric, String, Enum, Ring, Array, or future widgets without their own
review.
</p>

<hr/>

<h2 id="standard-property-surface">7. Standard Property Surface</h2>

<h3>7.1 Semantic value properties</h3>

<ul>
  <li><code>value : bool</code> — primary boolean value.</li>
</ul>

<h3>7.2 Structural label properties</h3>

<ul>
  <li><code>label.visible : bool</code></li>
  <li><code>label.text : string</code></li>
  <li><code>label.style.text_color : frog.color.rgba8</code></li>
  <li><code>label.style.font_family : string</code></li>
  <li><code>label.style.font_size : length</code></li>
  <li><code>label.style.font_weight : enum</code></li>
  <li><code>label.style.font_style : enum</code></li>
  <li><code>label.style.text_decoration : enum</code></li>
</ul>

<h3>7.3 Caption properties</h3>

<ul>
  <li><code>caption.visible : bool</code></li>
  <li><code>caption.text : string</code></li>
  <li><code>caption.placement : enum</code> — preferred caption placement such as <code>top</code>, <code>bottom</code>, <code>left</code>, or <code>right</code>.</li>
  <li><code>caption.padding : length</code></li>
  <li><code>caption.style.text_color : frog.color.rgba8</code></li>
  <li><code>caption.style.font_family : string</code></li>
  <li><code>caption.style.font_size : length</code></li>
  <li><code>caption.style.font_weight : enum</code></li>
  <li><code>caption.style.font_style : enum</code></li>
  <li><code>caption.style.text_decoration : enum</code></li>
  <li><code>caption.style.text_alignment : enum</code></li>
</ul>

<h3>7.4 Internal state-text properties</h3>

<ul>
  <li><code>state_text.visible : bool</code></li>
  <li><code>state_text.true_text : string</code></li>
  <li><code>state_text.false_text : string</code></li>
  <li><code>state_text.placement : enum</code></li>
  <li><code>state_text.style.text_color.true : frog.color.rgba8</code></li>
  <li><code>state_text.style.text_color.false : frog.color.rgba8</code></li>
  <li><code>state_text.style.font_family : string</code></li>
  <li><code>state_text.style.font_size : length</code></li>
  <li><code>state_text.style.font_weight : enum</code></li>
  <li><code>state_text.style.font_style : enum</code></li>
  <li><code>state_text.style.text_decoration : enum</code></li>
  <li><code>state_text.style.text_alignment : enum</code></li>
</ul>

<h3>7.5 Interaction properties</h3>

<ul>
  <li><code>interaction.visible : bool</code></li>
  <li><code>interaction.enabled : bool</code></li>
  <li><code>interaction.focusable : bool</code></li>
  <li><code>interaction.focused : bool</code></li>
  <li><code>interaction.pressed : bool</code> — readable transient pressed posture for controls when exposed.</li>
</ul>

<h3>7.6 State-face style properties</h3>

<ul>
  <li><code>style.state_face.fill_color.true : frog.color.rgba8</code></li>
  <li><code>style.state_face.fill_color.false : frog.color.rgba8</code></li>
  <li><code>style.state_face.fill_color.hover_true : frog.color.rgba8</code></li>
  <li><code>style.state_face.fill_color.hover_false : frog.color.rgba8</code></li>
  <li><code>style.state_face.fill_color.pressed_true : frog.color.rgba8</code></li>
  <li><code>style.state_face.fill_color.pressed_false : frog.color.rgba8</code></li>
  <li><code>style.state_face.border_color.true : frog.color.rgba8</code></li>
  <li><code>style.state_face.border_color.false : frog.color.rgba8</code></li>
  <li><code>style.state_face.border_width : length</code></li>
</ul>

<p>
The Default Boolean realization deliberately does not publish <code>outer_face</code>,
<code>inner_face</code>, or <code>frame</code>. Alternate skins may change the
shape, proportions, and colors of <code>state_face</code> while preserving the
same Boolean class semantics.
</p>

<h3>7.7 Focus and disabled style properties</h3>

<ul>
  <li><code>style.focus_ring.visible : bool</code></li>
  <li><code>style.focus_ring.color : frog.color.rgba8</code></li>
  <li><code>style.focus_ring.width : length</code></li>
  <li><code>style.disabled.opacity : number</code></li>
</ul>

<h3>7.8 Realization-selection properties</h3>

<ul>
  <li><code>realization.family : string</code></li>
  <li><code>realization.variant : string</code> — such as <code>rectangular</code> or <code>circular</code>.</li>
  <li><code>realization.skin_id : string</code></li>
</ul>

<hr/>

<h2 id="frogwidgetsboolean_control">8. <code>frog.widgets.boolean_control</code></h2>

<h3>8.1 Class identity</h3>

<ul>
  <li><strong>class_id:</strong> <code>frog.widgets.boolean_control</code></li>
  <li><strong>family:</strong> <code>boolean_widget</code></li>
  <li><strong>compatible role:</strong> <code>control</code></li>
</ul>

<h3>8.2 Primary value posture</h3>

<ul>
  <li>primary value: present</li>
  <li>value type: <code>bool</code></li>
  <li>natural value participation: yes</li>
  <li>user-mutable: yes</li>
  <li>diagram-mutable: yes</li>
  <li>runtime-mutable: yes</li>
  <li>mirrored property: <code>value</code></li>
</ul>

<h3>8.3 Standard methods</h3>

<ul>
  <li><code>focus()</code></li>
  <li><code>toggle()</code></li>
  <li><code>set_true()</code></li>
  <li><code>set_false()</code></li>
  <li><code>reset_to_default()</code></li>
</ul>

<h3>8.4 Standard events</h3>

<ul>
  <li><code>value_changed</code></li>
  <li><code>pressed</code></li>
  <li><code>released</code></li>
  <li><code>focus_gained</code></li>
  <li><code>focus_lost</code></li>
</ul>

<hr/>

<h2 id="frogwidgetsboolean_indicator">9. <code>frog.widgets.boolean_indicator</code></h2>

<h3>9.1 Class identity</h3>

<ul>
  <li><strong>class_id:</strong> <code>frog.widgets.boolean_indicator</code></li>
  <li><strong>family:</strong> <code>boolean_widget</code></li>
  <li><strong>compatible role:</strong> <code>indicator</code></li>
</ul>

<h3>9.2 Primary value posture</h3>

<ul>
  <li>primary value: present</li>
  <li>value type: <code>bool</code></li>
  <li>natural value participation: yes</li>
  <li>user-mutable: no in the standard portable posture</li>
  <li>diagram-mutable: yes</li>
  <li>runtime-mutable: yes</li>
  <li>mirrored property: <code>value</code></li>
</ul>

<h3>9.3 Standard methods</h3>

<ul>
  <li><code>focus()</code> when supported by the host and active publication corridor.</li>
  <li><code>reset_to_default()</code> when default value or style defaults exist and the active publication corridor exposes them.</li>
</ul>

<h3>9.4 Standard events</h3>

<ul>
  <li><code>value_rendered</code></li>
  <li><code>focus_gained</code></li>
  <li><code>focus_lost</code></li>
</ul>

<hr/>

<h2 id="default-svg-realization-posture">10. Default SVG Realization Posture</h2>

<p>
The boolean family should have a default SVG-oriented realization posture.
This does not mean that the SVG file owns boolean semantics.
It means that a standard realization package may publish SVG-backed assets and part mappings that consume the public boolean property surface.
</p>

<p>
A default SVG realization SHOULD support resizing, structural label and caption placement, internal state-text rendering, state-dependent color application, configurable border widths, configurable fills, and focus ring display.
</p>

<pre><code>widget property
    -> realization parameter
        -> SVG part/style application
</code></pre>

<hr/>

<h2 id="rectangular-and-circular-realization-variants">11. Rectangular and Circular Realization Variants</h2>

<p>
The standard boolean class may be realized through rectangular and circular variants.
These are realization variants, not separate widget classes.
</p>

<ul>
  <li><code>realization.variant = rectangular</code></li>
  <li><code>realization.variant = circular</code></li>
</ul>

<p>
Changing <code>realization.variant</code> changes embodiment, not class meaning.
</p>

<hr/>

<h2 id="diagram-interaction-posture">12. Diagram Interaction Posture</h2>

<p>
The boolean family supports natural value participation through <code>widget_value</code>, property access through <code>frog.ui.property_read</code> and <code>frog.ui.property_write</code>, method invocation through <code>frog.ui.method_invoke</code>, and event observation through <code>frog.ui.event_observe</code> where legal.
</p>

<p>
Typical legal object-style surfaces include <code>value</code>, <code>label.*</code>, <code>caption.*</code>, <code>state_text.*</code>, <code>interaction.*</code>, <code>style.*</code>, and <code>realization.*</code>.
</p>

<hr/>

<h2 id="behavior-expectations">13. Behavior Expectations</h2>

<ul>
  <li>the primary value remains boolean,</li>
  <li>boolean controls accept user-originated toggling only when enabled,</li>
  <li>boolean value transitions may emit <code>value_changed</code>,</li>
  <li>indicator realizations may emit <code>value_rendered</code> when their visible state is refreshed,</li>
  <li>state text follows the current boolean value but does not define it.</li>
</ul>

<hr/>

<h2 id="validation-expectations">14. Validation Expectations</h2>

<p>
Validators SHOULD diagnose non-boolean values, role/class mismatches, unsupported public members, illegal writes to indicator-only surfaces, attempts to treat realization-only internals as public class members, and attempts to use styling or realization selection to imply a distinct boolean class without explicit class publication.
</p>

<hr/>

<h2 id="compatibility-with-bounded-flat-slices">15. Compatibility with Bounded Flat Slices</h2>

<p>
Some bounded executable slices may preserve older flat property names for stability.
Those slice-local compatibility surfaces do not replace the canonical boolean widget law defined here.
</p>

<p>
Tools may support explicit compatibility mappings for frozen examples, but new boolean-facing widget work should use the hierarchical surface defined in this document.
</p>

<hr/>

<h2 id="summary">16. Summary</h2>

<p>
The boolean widget family defines the intrinsic standardized true/false widget baseline of FROG:
</p>

<ul>
  <li><code>frog.widgets.boolean_control</code></li>
  <li><code>frog.widgets.boolean_indicator</code></li>
</ul>

<p>
The boolean family now follows the shared <code>label</code> / <code>caption</code> convention while preserving boolean-specific <code>state_text</code> for true/false text inside the state face.
</p>
