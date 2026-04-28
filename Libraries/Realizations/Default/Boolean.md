<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization — Boolean Widgets</h1>

<p align="center">
  <strong>Default realization posture for standardized boolean widgets</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="./Package.md">Default realization package posture</a></li>
  <li><a href="./boolean.default.wfrog">Machine-readable boolean realization manifest</a></li>
  <li><a href="./assets/boolean/Readme.md">Boolean default realization assets</a></li>
  <li><a href="../Widgets/Boolean.md">Boolean widget class law</a></li>
  <li><a href="../../UI.md">Executable UI primitives</a></li>
</ul>

<hr/>

<h2>1. Overview</h2>

<p>
This document describes the default realization posture for the standardized boolean widget classes:
</p>

<ul>
  <li><code>frog.widgets.boolean_control</code></li>
  <li><code>frog.widgets.boolean_indicator</code></li>
</ul>

<p>
It does not define boolean widget semantics.
The class law belongs to <code>Libraries/Widgets/Boolean.md</code>.
This document describes how the default realization family can embody that law through SVG template resources, part bindings, state maps, and realization variants.
</p>

<hr/>

<h2>2. Published Default Variants</h2>

<p>
The default boolean realization publishes two compatible variants:
</p>

<ul>
  <li><code>rectangular</code></li>
  <li><code>circular</code></li>
</ul>

<p>
Both variants target the same boolean classes.
They are not separate widget classes.
</p>

<pre><code>frog.widgets.boolean_control
frog.widgets.boolean_indicator
    |
    +-- frog.realizations.default.boolean.rectangular
    |
    +-- frog.realizations.default.boolean.circular
</code></pre>

<hr/>

<h2>3. Machine-Readable Manifest</h2>

<p>
The machine-readable publication for this realization family is:
</p>

<pre><code>Libraries/Realizations/Default/boolean.default.wfrog</code></pre>

<p>
That manifest declares:
</p>

<ul>
  <li>target classes,</li>
  <li>realization records,</li>
  <li>realization variants,</li>
  <li>SVG resources,</li>
  <li>part bindings,</li>
  <li>state maps,</li>
  <li>property bindings,</li>
  <li>anchors,</li>
  <li>host hints.</li>
</ul>

<hr/>

<h2>4. Public Parts Realized</h2>

<p>
The default boolean realization maps the following boolean public parts:
</p>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>outer_face</code></li>
  <li><code>inner_face</code></li>
  <li><code>state_text</code></li>
  <li><code>focus_ring</code></li>
</ul>

<p>
The default realization may approximate these parts through SVG, host-native drawing, or a mixed host strategy, but it must preserve their public class meaning.
</p>

<hr/>

<h2>5. SVG Template Assets</h2>

<p>
The default SVG template assets are:
</p>

<ul>
  <li><code>assets/boolean/templates/boolean_rectangular.svg</code></li>
  <li><code>assets/boolean/templates/boolean_circular.svg</code></li>
</ul>

<p>
These assets contain stable <code>data-frog-*</code> markers so that IDEs and runtimes can bind widget properties to realization surfaces without guessing from geometry.
</p>

<hr/>

<h2>6. Property Binding Posture</h2>

<p>
The default realization uses the following conceptual mapping:
</p>

<pre><code>boolean public property
    -> realization property binding
        -> SVG template part or host-rendered surface
</code></pre>

<p>
Representative bindings include:
</p>

<ul>
  <li><code>label.text</code> to <code>label</code></li>
  <li><code>state_text.true_text</code> and <code>state_text.false_text</code> to <code>state_text</code></li>
  <li><code>style.outer.*</code> to <code>outer_face</code></li>
  <li><code>style.inner.*</code> to <code>inner_face</code></li>
  <li><code>style.focus_ring.*</code> to <code>focus_ring</code></li>
</ul>

<hr/>

<h2>7. State Posture</h2>

<p>
The semantic boolean value remains <code>value</code>.
The default realization uses state maps only to select visual embodiment for the true and false postures and their interaction-derived visual states.
</p>

<p>
Representative visual states include:
</p>

<ul>
  <li><code>false</code></li>
  <li><code>true</code></li>
  <li><code>disabled_false</code></li>
  <li><code>disabled_true</code></li>
  <li><code>focused_false</code></li>
  <li><code>focused_true</code></li>
  <li><code>pressed_false</code></li>
  <li><code>pressed_true</code></li>
</ul>

<p>
Those visual states do not create additional boolean values.
</p>

<hr/>

<h2>8. Summary</h2>

<p>
The default boolean realization completes the first reusable default-asset posture for the boolean widget family.
It keeps the class law in <code>Libraries/Widgets/Boolean.md</code>, publishes SVG template resources under <code>assets/boolean/</code>, and binds them through <code>boolean.default.wfrog</code>.
</p>
