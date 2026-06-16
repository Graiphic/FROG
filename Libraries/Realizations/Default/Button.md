<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization - Button Widget</h1>

<p align="center">
  <strong>Default realization posture for the standardized button widget</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="./Package.md">Default realization package posture</a></li>
  <li><a href="./button.default.wfrog">Machine-readable button realization manifest</a></li>
  <li><a href="./assets/button/Readme.md">Button default realization assets</a></li>
  <li><a href="../../Widgets/Button.md">Button widget class law</a></li>
  <li><a href="../../UI.md">Executable UI primitives</a></li>
</ul>

<hr/>

<h2>1. Overview</h2>

<p>
This document describes the default realization posture for <code>frog.widgets.button</code>.
</p>

<p>
The class law belongs to <code>Libraries/Widgets/Button.md</code>.
This document describes how the default realization family can embody that law through one semantic SVG template resource, part bindings, state maps, and realization variants.
</p>

<hr/>

<h2>2. Published Default Variant</h2>

<ul>
  <li><code>rectangular</code></li>
</ul>

<hr/>

<h2>3. Machine-Readable Manifest</h2>

<pre><code>Libraries/Realizations/Default/button.default.wfrog</code></pre>

<hr/>

<h2>4. Public Parts Realized</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>placement_bounds</code></li>
  <li><code>button_face</code></li>
  <li><code>state_text</code></li>
</ul>

<hr/>

<h2>5. SVG Template and State Overrides</h2>

<ul>
  <li><code>assets/button/templates/button_rectangular.svg</code></li>
</ul>

<p>
The default button realization uses one parameterized SVG template.
True/false, hover, pressed, disabled, and transition visuals are expressed through properties and style bindings, not separate default SVG skins.
</p>

<p>
The rectangular Button template is accepted for the bounded
<code>Examples/10_button_press_to_boolean</code> corridor as of 2026-05-15 and
is reused by the bounded switch corridors. That acceptance covers the
SVG-published public parts, configurable <code>button_face</code> and state-text styling, hover and
pressed state visuals, placement aura semantics through <code>placement_bounds</code>,
and host overlay alignment to the published <code>button_face</code> part.
It remains a Default realization asset acceptance, not
a runtime-defined HTML/CSS skin.
</p>

<hr/>

<h2>6. Mechanical Action Posture</h2>

<p>
The default realization visualizes the state produced by the class-owned mechanical-action model.
It does not define the mechanical-action semantics itself.
</p>

<p>
The manifest publishes the canonical action vocabulary
<code>switch_when_pressed</code>, <code>switch_when_released</code>,
<code>switch_until_released</code>, <code>latch_when_pressed</code>,
<code>latch_when_released</code>, and <code>latch_until_released</code>,
including the realization posture needed by host overlays.
Runtime families must still validate each action before accepting it as
executable behavior. The current accepted corridors validate
<code>switch_until_released</code> in Example 10 and
<code>switch_when_pressed</code> in Example 11. Example 12 introduces a C++
first <code>switch_when_released</code> corridor; Python and Rust parity must
not be claimed until those runtimes are explicitly aligned and validated.
</p>
