<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Standard Widgets</h1>

<p align="center">
  <strong>Repository-visible standard widget family index</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Current Widget Baseline</h2>

<table>
  <thead>
    <tr>
      <th>Widget document</th>
      <th>Standard classes</th>
      <th>Current role</th>
    </tr>
  </thead>
  <tbody>
    <tr><td><a href="./Boolean.md"><code>Boolean.md</code></a></td><td><code>frog.widgets.boolean_control</code>, <code>frog.widgets.boolean_indicator</code></td><td>True/false control and display baseline.</td></tr>
    <tr><td><a href="./String.md"><code>String.md</code></a></td><td><code>frog.widgets.string_control</code>, <code>frog.widgets.string_indicator</code></td><td>Text control and display baseline.</td></tr>
    <tr><td><a href="./Button.md"><code>Button.md</code></a></td><td><code>frog.widgets.button</code></td><td>Command-oriented button baseline.</td></tr>
    <tr><td><a href="./Numeric.md"><code>Numeric.md</code></a></td><td><code>frog.widgets.numeric_control</code>, <code>frog.widgets.numeric_indicator</code></td><td>Numeric value control and display baseline.</td></tr>
    <tr><td><a href="./Chart.md"><code>Chart.md</code></a></td><td><code>frog.widgets.waveform_chart</code></td><td>Composite waveform chart baseline.</td></tr>
    <tr><td><a href="./Label.md"><code>Label.md</code></a></td><td><code>frog.widgets.label</code></td><td>Standalone support text.</td></tr>
    <tr><td><a href="./Frame.md"><code>Frame.md</code></a></td><td><code>frog.widgets.frame</code></td><td>Standalone grouping frame.</td></tr>
    <tr><td><a href="./Enum.md"><code>Enum.md</code></a></td><td><code>frog.widgets.enum_control</code>, <code>frog.widgets.enum_indicator</code></td><td>Finite discrete named-value selection and display.</td></tr>
    <tr><td><a href="./Path.md"><code>Path.md</code></a></td><td><code>frog.widgets.path_control</code>, <code>frog.widgets.path_indicator</code></td><td>Path-typed selection and display.</td></tr>
  </tbody>
</table>

<hr/>

<h2>Shared Convention</h2>

<ul>
  <li><code>label.*</code> is the structural or logical widget-name surface.</li>
  <li><code>caption.*</code> is the user-facing front-panel presentation surface.</li>
</ul>

<p>
Widget-specific semantic text surfaces remain class-owned when published, such as <code>text.value</code> for labels, <code>state_text.*</code> for buttons and booleans, <code>selected.text</code> for enums, and <code>path.display_value</code> for paths.
</p>

<hr/>

<h2>Next Growth Direction</h2>

<p>
The next coherent widget candidates are <code>Array</code> and <code>Cluster</code>.
</p>
