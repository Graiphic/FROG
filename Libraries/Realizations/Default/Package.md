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
  <li><a href="../../Widgets/Chart.md">Chart widget class law</a></li>
  <li><a href="./Boolean.md">Default boolean realization</a></li>
  <li><a href="./String.md">Default string realization</a></li>
  <li><a href="./Button.md">Default button realization</a></li>
  <li><a href="./Numeric.md">Default numeric realization</a></li>
  <li><a href="./Chart.md">Default chart realization</a></li>
</ul>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#ownership-boundary">2. Ownership Boundary</a></li>
  <li><a href="#active-default-manifests">3. Active Default Manifests</a></li>
  <li><a href="#label-and-caption-doctrine">4. Label and Caption Doctrine</a></li>
  <li><a href="#composite-widget-posture">5. Composite Widget Posture</a></li>
  <li><a href="#manifest-requirements">6. Manifest Requirements</a></li>
  <li><a href="#summary">7. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the package-level posture for the official <code>Default</code> widget realization family.
</p>

<p>
The Default realization family publishes embodiment information for standardized widget classes.
It does not own widget class law, language semantics, executable UI primitives, or runtime-private implementation details.
</p>

<hr/>

<h2 id="ownership-boundary">2. Ownership Boundary</h2>

<pre><code>Libraries/Widgets/
    - standardized widget class law

Libraries/Realizations/Default/
    - official default realization posture

*.default.wfrog
    - machine-readable realization manifests

assets/
    - SVG templates, subobject assets, and optional state resources

runtime implementation
    - host execution of the package
</code></pre>

<p>
A Default realization manifest may publish resources, variants, state maps, part bindings, property bindings, anchors, text regions, host hints, and validation expectations.
It must not redefine public widget semantics.
</p>

<hr/>

<h2 id="active-default-manifests">3. Active Default Manifests</h2>

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
      <td><a href="./boolean.default.wfrog"><code>boolean.default.wfrog</code></a></td>
      <td><code>frog.widgets.boolean_control</code>, <code>frog.widgets.boolean_indicator</code></td>
      <td><code>assets/boolean/templates/boolean_rectangular.svg</code>, <code>assets/boolean/templates/boolean_circular.svg</code></td>
    </tr>
    <tr>
      <td><a href="./string.default.wfrog"><code>string.default.wfrog</code></a></td>
      <td><code>frog.widgets.string_control</code>, <code>frog.widgets.string_indicator</code></td>
      <td><code>assets/string/templates/string_rectangular.svg</code></td>
    </tr>
    <tr>
      <td><a href="./button.default.wfrog"><code>button.default.wfrog</code></a></td>
      <td><code>frog.widgets.button</code></td>
      <td><code>assets/button/templates/button_rectangular.svg</code> plus optional state resources</td>
    </tr>
    <tr>
      <td><a href="./numeric.default.wfrog"><code>numeric.default.wfrog</code></a></td>
      <td><code>frog.widgets.numeric_control</code>, <code>frog.widgets.numeric_indicator</code></td>
      <td><code>assets/numeric/templates/numeric_rectangular.svg</code></td>
    </tr>
    <tr>
      <td><a href="./chart.default.wfrog"><code>chart.default.wfrog</code></a></td>
      <td><code>frog.widgets.waveform_chart</code></td>
      <td><code>assets/chart/templates/waveform_chart_shell.svg</code> plus subobject assets under <code>assets/chart/subobjects/</code></td>
    </tr>
  </tbody>
</table>

<hr/>

<h2 id="label-and-caption-doctrine">4. Label and Caption Doctrine</h2>

<ul>
  <li><code>label.*</code> is the structural or logical widget-name surface.</li>
  <li><code>caption.*</code> is the user-facing front-panel presentation surface.</li>
</ul>

<p>
Default realization assets may place and style captions.
They must not make SVG-baked text the semantic owner of <code>caption.text</code>.
</p>

<hr/>

<h2 id="composite-widget-posture">5. Composite Widget Posture</h2>

<p>
Some widgets are simple enough to use one primary SVG template.
Other widgets are composite.
</p>

<p>
The waveform chart is currently the main composite widget in the Default realization family.
Its manifest uses:
</p>

<ul>
  <li>a shell SVG with composition slots,</li>
  <li>separate subobject SVG assets,</li>
  <li>slot-to-resource declarations,</li>
  <li>part bindings that connect public chart sub-objects to realization resources.</li>
</ul>

<p>
This is the correct posture for widgets whose sub-objects have independent public properties and runtime behavior.
</p>

<hr/>

<h2 id="manifest-requirements">6. Manifest Requirements</h2>

<p>
A Default manifest SHOULD make the following surfaces inspectable:
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
  <li><code>state_maps</code> where relevant</li>
  <li><code>property_bindings</code> where relevant</li>
  <li><code>anchors</code> where relevant</li>
  <li><code>host_hints</code></li>
  <li><code>validation_expectations</code></li>
</ul>

<hr/>

<h2 id="summary">7. Summary</h2>

<p>
The Default realization package posture keeps class law, realization publication, assets, and runtime implementation separated.
The current active manifests cover Boolean, String, Button, Numeric, and Waveform Chart widget families.
</p>
