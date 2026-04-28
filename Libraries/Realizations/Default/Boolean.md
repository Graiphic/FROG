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
  <li><a href="../../Widgets/Boolean.md">Boolean widget class law</a></li>
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

<h2>2. Label, Caption, and State Text</h2>

<p>
The default boolean realization follows the shared widget convention:
</p>

<ul>
  <li><code>label</code> realizes the structural/logical label surface when a host chooses to expose it.</li>
  <li><code>caption</code> realizes the front-panel display caption.</li>
  <li><code>state_text</code> realizes optional internal true/false text.</li>
</ul>

<hr/>

<h2>3. Published Default Variants</h2>

<ul>
  <li><code>rectangular</code></li>
  <li><code>circular</code></li>
</ul>

<pre><code>frog.widgets.boolean_control
frog.widgets.boolean_indicator
    |
    +-- frog.realizations.default.boolean.rectangular
    |
    +-- frog.realizations.default.boolean.circular
</code></pre>

<hr/>

<h2>4. Machine-Readable Manifest</h2>

<pre><code>Libraries/Realizations/Default/boolean.default.wfrog</code></pre>

<p>
That manifest declares target classes, realization records, variants, SVG resources, part bindings, state maps, property bindings, anchors, and host hints.
</p>

<hr/>

<h2>5. Public Parts Realized</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>outer_face</code></li>
  <li><code>inner_face</code></li>
  <li><code>state_text</code></li>
  <li><code>focus_ring</code></li>
</ul>

<hr/>

<h2>6. SVG Template Assets</h2>

<ul>
  <li><code>assets/boolean/templates/boolean_rectangular.svg</code></li>
  <li><code>assets/boolean/templates/boolean_circular.svg</code></li>
</ul>

<hr/>

<h2>7. Property Binding Posture</h2>

<pre><code>boolean public property
    -> realization property binding
        -> SVG template part or host-rendered surface
</code></pre>

<p>
Representative bindings include <code>label.text</code> to <code>label</code>, <code>caption.text</code> to <code>caption</code>, <code>state_text.true_text</code> and <code>state_text.false_text</code> to <code>state_text</code>, <code>style.outer.*</code> to <code>outer_face</code>, <code>style.inner.*</code> to <code>inner_face</code>, and <code>style.focus_ring.*</code> to <code>focus_ring</code>.
</p>

<hr/>

<h2>8. Summary</h2>

<p>
The default boolean realization keeps class law in <code>Libraries/Widgets/Boolean.md</code>, publishes SVG template resources under <code>assets/boolean/</code>, and binds them through <code>boolean.default.wfrog</code>.
</p>
