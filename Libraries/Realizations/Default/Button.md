<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization — Button Widget</h1>

<p align="center">
  <strong>Default realization posture for the standardized button widget</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
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
This document describes how the default realization family can embody that law through SVG template resources, state-specific overrides, part bindings, state maps, and realization variants.
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
  <li><code>frame</code></li>
  <li><code>face</code></li>
  <li><code>state_face</code></li>
  <li><code>state_text</code></li>
  <li><code>focus_ring</code></li>
</ul>

<hr/>

<h2>5. SVG Template and State Overrides</h2>

<ul>
  <li><code>assets/button/templates/button_rectangular.svg</code></li>
  <li><code>assets/button/states/button_false.svg</code></li>
  <li><code>assets/button/states/button_true.svg</code></li>
  <li><code>assets/button/states/button_hover_false.svg</code></li>
  <li><code>assets/button/states/button_hover_true.svg</code></li>
  <li><code>assets/button/states/button_transition_false_to_true.svg</code></li>
  <li><code>assets/button/states/button_transition_true_to_false.svg</code></li>
</ul>

<p>
The state-specific files are optional realization overrides.
They do not redefine button semantics.
</p>

<hr/>

<h2>6. Mechanical Action Posture</h2>

<p>
The default realization visualizes the state produced by the class-owned mechanical-action model.
It does not define the mechanical-action semantics itself.
</p>
