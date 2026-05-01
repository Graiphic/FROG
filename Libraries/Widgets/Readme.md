<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Standard Widgets</h1>

<p align="center">
  <strong>Repository-visible standard widget family index</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#ownership-boundary">2. Ownership Boundary</a></li>
  <li><a href="#shared-conventions">3. Shared Conventions</a></li>
  <li><a href="#core-widget-baseline">4. Core Widget Baseline</a></li>
  <li><a href="#default-realization-status">5. Default Realization Status</a></li>
  <li><a href="#next-growth-direction">6. Next Growth Direction</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This directory indexes the standardized widget families published in the repository-visible widget layer.
</p>

<p>
Each document in this directory defines widget class law.
Realization posture remains downstream in <code>../Realizations/</code>.
Runtime implementations consume widget definitions and realization artifacts, but they do not define the public widget classes.
</p>

<hr/>

<h2 id="ownership-boundary">2. Ownership Boundary</h2>

<pre><code>Libraries/Widgets/
    - public widget class law

Libraries/Realizations/
    - official realization families and machine-readable realization manifests

Expression/
    - widget instances and source representation

Libraries/UI.md
    - executable UI interaction primitives

Implementations/
    - runtime-family consumption
</code></pre>

<hr/>

<h2 id="shared-conventions">3. Shared Conventions</h2>

<p>
The current widget baseline follows the shared <code>label</code> / <code>caption</code> convention.
</p>

<ul>
  <li><code>label.*</code> is the structural or logical widget-name surface.</li>
  <li><code>caption.*</code> is the user-facing front-panel presentation surface.</li>
</ul>

<p>
Widgets may also expose class-specific text surfaces such as <code>state_text.*</code>, <code>unit_label.*</code>, or scale-label surfaces when those are part of their class law.
</p>

<hr/>

<h2 id="core-widget-baseline">4. Core Widget Baseline</h2>

<table>
  <thead>
    <tr>
      <th>Widget document</th>
      <th>Standard classes</th>
      <th>Current role</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><a href="./Boolean.md"><code>Boolean.md</code></a></td>
      <td><code>frog.widgets.boolean_control</code>, <code>frog.widgets.boolean_indicator</code></td>
      <td>True/false control and display baseline.</td>
    </tr>
    <tr>
      <td><a href="./String.md"><code>String.md</code></a></td>
      <td><code>frog.widgets.string_control</code>, <code>frog.widgets.string_indicator</code></td>
      <td>Text control and display baseline.</td>
    </tr>
    <tr>
      <td><a href="./Button.md"><code>Button.md</code></a></td>
      <td><code>frog.widgets.button</code></td>
      <td>Command-oriented button baseline with mechanical-action behavior.</td>
    </tr>
    <tr>
      <td><a href="./Numeric.md"><code>Numeric.md</code></a></td>
      <td><code>frog.widgets.numeric_control</code>, <code>frog.widgets.numeric_indicator</code></td>
      <td>Numeric value control and display baseline.</td>
    </tr>
    <tr>
      <td><a href="./Chart.md"><code>Chart.md</code></a></td>
      <td><code>frog.widgets.waveform_chart</code></td>
      <td>Composite waveform chart baseline with sub-object model.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2 id="default-realization-status">5. Default Realization Status</h2>

<p>
The current Default realization family publishes machine-readable manifests for the core widget baseline:
</p>

<ul>
  <li><code>../Realizations/Default/boolean.default.wfrog</code></li>
  <li><code>../Realizations/Default/string.default.wfrog</code></li>
  <li><code>../Realizations/Default/button.default.wfrog</code></li>
  <li><code>../Realizations/Default/numeric.default.wfrog</code></li>
  <li><code>../Realizations/Default/chart.default.wfrog</code></li>
</ul>

<p>
Chart uses a subobject composition posture because a waveform chart is a composite widget.
Its Default realization uses a shell plus separate subobject SVG assets.
</p>

<hr/>

<h2 id="next-growth-direction">6. Next Growth Direction</h2>

<p>
With Boolean, String, Button, Numeric, and Waveform Chart now represented in the repository-visible baseline, the next widget work should prioritize additional core-support widgets and near-core widgets rather than reopening the frozen Example 05 corridor.
</p>

<p>
Likely next candidates include:
</p>

<ul>
  <li><code>Label.md</code></li>
  <li><code>Frame.md</code></li>
  <li><code>Enum.md</code></li>
  <li><code>Path.md</code></li>
  <li><code>Array.md</code></li>
  <li><code>Cluster.md</code></li>
</ul>
