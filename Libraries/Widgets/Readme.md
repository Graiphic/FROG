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
  <li><a href="#support-widget-baseline">5. Support Widget Baseline</a></li>
  <li><a href="#default-realization-status">6. Default Realization Status</a></li>
  <li><a href="#next-growth-direction">7. Next Growth Direction</a></li>
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

<ul>
  <li><code>label.*</code> is the structural or logical widget-name surface.</li>
  <li><code>caption.*</code> is the user-facing front-panel presentation surface.</li>
</ul>

<p>
For <code>frog.widgets.label</code>, the displayed support text is <code>text.value</code>.
For <code>frog.widgets.frame</code>, the visible frame title is <code>caption.text</code>.
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

<h2 id="support-widget-baseline">5. Support Widget Baseline</h2>

<table>
  <thead>
    <tr>
      <th>Widget document</th>
      <th>Standard class</th>
      <th>Current role</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><a href="./Label.md"><code>Label.md</code></a></td>
      <td><code>frog.widgets.label</code></td>
      <td>Standalone front-panel support text.</td>
    </tr>
    <tr>
      <td><a href="./Frame.md"><code>Frame.md</code></a></td>
      <td><code>frog.widgets.frame</code></td>
      <td>Standalone visual grouping and emphasis region.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2 id="default-realization-status">6. Default Realization Status</h2>

<ul>
  <li><code>../Realizations/Default/boolean.default.wfrog</code></li>
  <li><code>../Realizations/Default/string.default.wfrog</code></li>
  <li><code>../Realizations/Default/button.default.wfrog</code></li>
  <li><code>../Realizations/Default/numeric.default.wfrog</code></li>
  <li><code>../Realizations/Default/chart.default.wfrog</code></li>
  <li><code>../Realizations/Default/label.default.wfrog</code></li>
  <li><code>../Realizations/Default/frame.default.wfrog</code></li>
</ul>

<hr/>

<h2 id="next-growth-direction">7. Next Growth Direction</h2>

<p>
With the first core widget baseline and the two support widgets represented in the repository, the next widget work should prioritize near-core typed and composite data widgets.
</p>

<ul>
  <li><code>Enum.md</code></li>
  <li><code>Path.md</code></li>
  <li><code>Array.md</code></li>
  <li><code>Cluster.md</code></li>
</ul>
