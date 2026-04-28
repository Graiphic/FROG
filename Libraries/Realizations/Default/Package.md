<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization Package</h1>

<p align="center">
  <strong>Machine-readable publication posture for the official <code>Default</code> widget realization family</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="../Readme.md">Realizations index</a></li>
  <li><a href="../../Widgets/Readme.md">Standard widgets</a></li>
  <li><a href="../../Widgets/Boolean.md">Boolean widget class law</a></li>
  <li><a href="../../Widgets/String.md">String widget class law</a></li>
  <li><a href="../../Widgets/Button.md">Button widget class law</a></li>
  <li><a href="../../Widgets/Numeric.md">Numeric widget class law</a></li>
  <li><a href="./Boolean.md">Default boolean realization</a></li>
  <li><a href="./String.md">Default string realization</a></li>
  <li><a href="./Button.md">Default button realization</a></li>
  <li><a href="./Numeric.md">Default numeric realization</a></li>
  <li><a href="./boolean.default.wfrog">Boolean realization manifest</a></li>
  <li><a href="./string.default.wfrog">String realization manifest</a></li>
  <li><a href="./button.default.wfrog">Button realization manifest</a></li>
  <li><a href="./numeric.default.wfrog">Numeric realization manifest</a></li>
</ul>

<hr/>

<h2>1. Overview</h2>

<p>
This document defines the publication posture for the official <code>Default</code> realization family.
</p>

<p>
The default realization family publishes embodiment information for standardized widget classes.
It does not own widget class law, language semantics, executable UI primitives, or runtime-private implementation details.
</p>

<hr/>

<h2>2. Active Default Manifests</h2>

<table>
  <thead>
    <tr>
      <th>Manifest</th>
      <th>Targets</th>
      <th>Primary realization assets</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>boolean.default.wfrog</code></td>
      <td><code>frog.widgets.boolean_control</code>, <code>frog.widgets.boolean_indicator</code></td>
      <td><code>assets/boolean/templates/boolean_rectangular.svg</code>, <code>assets/boolean/templates/boolean_circular.svg</code></td>
    </tr>
    <tr>
      <td><code>string.default.wfrog</code></td>
      <td><code>frog.widgets.string_control</code>, <code>frog.widgets.string_indicator</code></td>
      <td><code>assets/string/templates/string_rectangular.svg</code></td>
    </tr>
    <tr>
      <td><code>button.default.wfrog</code></td>
      <td><code>frog.widgets.button</code></td>
      <td><code>assets/button/templates/button_rectangular.svg</code> plus optional state resources</td>
    </tr>
    <tr>
      <td><code>numeric.default.wfrog</code></td>
      <td><code>frog.widgets.numeric_control</code>, <code>frog.widgets.numeric_indicator</code></td>
      <td><code>assets/numeric/templates/numeric_rectangular.svg</code></td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>3. Ownership Boundary</h2>

<pre><code>Libraries/Widgets/
    - standardized widget class law

Libraries/Realizations/Default/
    - official default realization posture

*.default.wfrog
    - machine-readable realization manifests

assets/
    - SVG templates and optional state resources

runtime implementation
    - host execution of the package
</code></pre>

<p>
A default realization manifest may publish resources, variants, state maps, part bindings, property bindings, anchors, text regions, host hints, and validation expectations.
It must not redefine public widget semantics.
</p>

<hr/>

<h2>4. Label and Caption Doctrine</h2>

<p>
The default realization family follows the shared standard-widget distinction between <code>label</code> and <code>caption</code>.
</p>

<ul>
  <li><code>label.*</code> is the structural or logical widget-name surface.</li>
  <li><code>caption.*</code> is the user-facing front-panel presentation surface.</li>
</ul>

<p>
Default realization assets may place and style captions.
They must not make SVG-baked text the semantic owner of <code>caption.text</code>.
</p>

<hr/>

<h2>5. Required Manifest Surfaces</h2>

<p>
A focused default manifest SHOULD make the following surfaces inspectable:
</p>

<ul>
  <li><code>format</code></li>
  <li><code>kind</code></li>
  <li><code>publication_role</code></li>
  <li><code>version_governance_ref</code></li>
  <li><code>package</code></li>
  <li><code>ownership_boundary</code></li>
  <li><code>targets</code></li>
  <li><code>exports</code></li>
  <li><code>realizations</code></li>
  <li><code>variants</code></li>
  <li><code>resources</code></li>
  <li><code>part_bindings</code></li>
  <li><code>state_maps</code></li>
  <li><code>property_bindings</code></li>
  <li><code>anchors</code> or equivalent text-region posture when relevant</li>
  <li><code>host_hints</code></li>
  <li><code>validation_expectations</code></li>
</ul>

<hr/>

<h2>6. Summary</h2>

<p>
The default realization package posture keeps class law, realization publication, assets, and runtime implementation separated.
The current active manifests cover boolean, string, button, and numeric widget families.
</p>
