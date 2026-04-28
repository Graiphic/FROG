<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Standard Button Widget</h1>

<p align="center">
  <strong>Normative baseline for the standardized button control widget class</strong><br/>
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
</ul>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#class-defined-here">2. Class Defined Here</a></li>
  <li><a href="#button-class-versus-realization">3. Button Class versus Realization</a></li>
  <li><a href="#label-caption-and-state-text-posture">4. Label, Caption, and State Text Posture</a></li>
  <li><a href="#mechanical-action-model">5. Mechanical Action Model</a></li>
  <li><a href="#public-visual-part-model">6. Public Visual Part Model</a></li>
  <li><a href="#standard-property-surface">7. Standard Property Surface</a></li>
  <li><a href="#frogwidgetsbutton">8. <code>frog.widgets.button</code></a></li>
  <li><a href="#default-svg-realization-posture">9. Default SVG Realization Posture</a></li>
  <li><a href="#state-resource-override-posture">10. State Resource Override Posture</a></li>
  <li><a href="#diagram-interaction-posture">11. Diagram Interaction Posture</a></li>
  <li><a href="#validation-expectations">12. Validation Expectations</a></li>
  <li><a href="#summary">13. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the intrinsic standardized baseline for the button widget in FROG.
The button is a command-oriented control with a boolean state, explicit mechanical-action behavior, event observation, and realization-driven visual state support.
</p>

<p>
The intrinsic button baseline is not merely a decorative clickable rectangle and is not equivalent to a generic boolean control.
It owns button-specific timing and interaction semantics while remaining compatible with the shared widget model.
</p>

<hr/>

<h2 id="class-defined-here">2. Class Defined Here</h2>

<ul>
  <li><code>frog.widgets.button</code></li>
</ul>

<hr/>

<h2 id="button-class-versus-realization">3. Button Class versus Realization</h2>

<p>
The button class owns state meaning, mechanical action, properties, methods, events, parts, and legal access through <code>frog.ui.*</code>.
The realization owns shape, skin, SVG resources, state-specific visual resources, part bindings, anchors, transitions, and host-specific rendering.
</p>

<p>
A state-specific SVG override does not create a new button class.
A new class is justified only when the public contract changes.
</p>

<hr/>

<h2 id="label-caption-and-state-text-posture">4. Label, Caption, and State Text Posture</h2>

<pre><code>label.*
    - structural / logical widget name surface

caption.*
    - front-panel presentation caption shown near the button

state_text.*
    - optional text displayed inside the button face
      and optionally varied by button state
</code></pre>

<ul>
  <li><code>label.text</code> names the object for tooling, references, diagram readability, accessibility, and debugging.</li>
  <li><code>caption.text</code> is the external front-panel caption shown near the button.</li>
  <li><code>state_text.false_text</code> and <code>state_text.true_text</code> are the internal button-face strings.</li>
</ul>

<hr/>

<h2 id="mechanical-action-model">5. Mechanical Action Model</h2>

<p>
The button exposes the following canonical property:
</p>

<pre><code>behavior.mechanical_action</code></pre>

<p>
The standard values are:
</p>

<ul>
  <li><code>switch_when_pressed</code></li>
  <li><code>switch_when_released</code></li>
  <li><code>switch_until_released</code></li>
  <li><code>latch_when_pressed</code></li>
  <li><code>latch_when_released</code></li>
  <li><code>latch_until_released</code></li>
</ul>

<p>
For latch modes, the reset condition MUST be explicit.
The portable baseline defines the reset trigger as consumption through the natural button value path when that path is present in the executable diagram.
A mere object-style property read of <code>value</code> SHOULD NOT by itself reset the latch unless a later profile or runtime-family contract explicitly publishes that behavior.
</p>

<hr/>

<h2 id="public-visual-part-model">6. Public Visual Part Model</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>frame</code></li>
  <li><code>face</code></li>
  <li><code>state_face</code></li>
  <li><code>state_text</code></li>
  <li><code>focus_ring</code></li>
</ul>

<hr/>

<h2 id="standard-property-surface">7. Standard Property Surface</h2>

<h3>7.1 State</h3>

<ul>
  <li><code>value : bool</code></li>
</ul>

<h3>7.2 Label and caption</h3>

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

<h3>7.3 Internal state text</h3>

<ul>
  <li><code>state_text.visible : bool</code></li>
  <li><code>state_text.lock_in_center : bool</code></li>
  <li><code>state_text.multiple_strings : bool</code></li>
  <li><code>state_text.false_text : string</code></li>
  <li><code>state_text.true_text : string</code></li>
  <li><code>state_text.hover_false_text : string</code> when exposed</li>
  <li><code>state_text.hover_true_text : string</code> when exposed</li>
  <li><code>state_text.style.*</code></li>
</ul>

<h3>7.4 Interaction and behavior</h3>

<ul>
  <li><code>interaction.visible : bool</code></li>
  <li><code>interaction.enabled : bool</code></li>
  <li><code>interaction.focusable : bool</code></li>
  <li><code>interaction.focused : bool</code></li>
  <li><code>interaction.hovered : bool</code></li>
  <li><code>interaction.pressed : bool</code></li>
  <li><code>behavior.mechanical_action : enum</code></li>
  <li><code>behavior.latch_reset_policy : enum</code></li>
</ul>

<h3>7.5 Style and realization</h3>

<ul>
  <li><code>style.frame.*</code></li>
  <li><code>style.face.*</code></li>
  <li><code>style.state_face.*</code></li>
  <li><code>style.focus_ring.*</code></li>
  <li><code>style.disabled.opacity : number</code></li>
  <li><code>realization.family : string</code></li>
  <li><code>realization.variant : string</code></li>
  <li><code>realization.skin_id : string</code></li>
</ul>

<hr/>

<h2 id="frogwidgetsbutton">8. <code>frog.widgets.button</code></h2>

<h3>8.1 Class identity</h3>

<ul>
  <li><strong>class_id:</strong> <code>frog.widgets.button</code></li>
  <li><strong>family:</strong> <code>button_widget</code></li>
  <li><strong>compatible role:</strong> <code>control</code></li>
</ul>

<h3>8.2 Value posture</h3>

<ul>
  <li>primary value: present</li>
  <li>value type: <code>bool</code></li>
  <li>natural value participation: optional and mechanical-action-dependent</li>
  <li>mirrored property: <code>value</code></li>
</ul>

<h3>8.3 Standard methods</h3>

<ul>
  <li><code>focus()</code></li>
  <li><code>press()</code></li>
  <li><code>release()</code></li>
  <li><code>activate()</code></li>
  <li><code>click()</code></li>
  <li><code>set_true()</code></li>
  <li><code>set_false()</code></li>
  <li><code>toggle()</code></li>
  <li><code>reset_to_default()</code></li>
</ul>

<h3>8.4 Standard events</h3>

<ul>
  <li><code>pressed</code></li>
  <li><code>released</code></li>
  <li><code>clicked</code></li>
  <li><code>value_changed</code></li>
  <li><code>hover_entered</code></li>
  <li><code>hover_left</code></li>
  <li><code>focus_gained</code></li>
  <li><code>focus_lost</code></li>
</ul>

<hr/>

<h2 id="default-svg-realization-posture">9. Default SVG Realization Posture</h2>

<p>
The default realization SHOULD support a parameterized rectangular SVG template, state-dependent color application, caption and internal state-text rendering, configurable frame / face / state-face border widths, and focus ring display.
</p>

<pre><code>widget property
    -> realization parameter
        -> SVG part/style application
</code></pre>

<hr/>

<h2 id="state-resource-override-posture">10. State Resource Override Posture</h2>

<p>
The default button realization may be driven by one parameterized template SVG and may also publish optional state-specific SVG overrides.
</p>

<p>
The initial state-specific override names are:
</p>

<ul>
  <li><code>false</code></li>
  <li><code>true</code></li>
  <li><code>hover_false</code></li>
  <li><code>hover_true</code></li>
  <li><code>transition_false_to_true</code></li>
  <li><code>transition_true_to_false</code></li>
</ul>

<p>
Additional visual states such as <code>pressed_false</code>, <code>pressed_true</code>, <code>disabled_false</code>, <code>disabled_true</code>, <code>focused_false</code>, and <code>focused_true</code> may be represented by parameter binding or later explicit override assets.
</p>

<hr/>

<h2 id="diagram-interaction-posture">11. Diagram Interaction Posture</h2>

<p>
The button supports object-style property access through <code>frog.ui.property_read</code> and <code>frog.ui.property_write</code>, method invocation through <code>frog.ui.method_invoke</code>, event observation through <code>frog.ui.event_observe</code>, and widget reference targeting through <code>widget_reference</code>.
</p>

<p>
When the button participates through a natural value path, that value path is mechanical-action-aware and may be relevant to latch reset.
Object-style reads of <code>value</code> remain distinct from natural value consumption.
</p>

<hr/>

<h2 id="validation-expectations">12. Validation Expectations</h2>

<p>
Validators SHOULD diagnose unknown mechanical actions, illegal value access for the selected button posture, unsupported state-resource overrides, attempts to use realization-only SVG layers as public class members, and attempts to make a skin or state asset redefine class semantics.
</p>

<hr/>

<h2 id="summary">13. Summary</h2>

<p>
The standardized button defines the intrinsic command-oriented action widget of the FROG baseline:
</p>

<ul>
  <li><code>frog.widgets.button</code></li>
</ul>

<p>
The button is a stateful command control with explicit mechanical-action behavior, event posture, internal state text, shared <code>label</code> / <code>caption</code> convention, and default SVG realization support with optional state-specific overrides.
</p>
