<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization Package</h1>

<p align="center">
  <strong>Machine-readable publication posture for the official <code>Default</code> widget realization family</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>1. Overview</h2>

<p>
This document defines the package-level posture for the official <code>Default</code> widget realization family.
</p>

<p>
The Default realization family publishes embodiment information for standardized widget classes.
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
    <tr><td><code>boolean.default.wfrog</code></td><td><code>frog.widgets.boolean_control</code>, <code>frog.widgets.boolean_indicator</code></td><td>Boolean templates</td></tr>
    <tr><td><code>string.default.wfrog</code></td><td><code>frog.widgets.string_control</code>, <code>frog.widgets.string_indicator</code></td><td>String template</td></tr>
    <tr><td><code>button.default.wfrog</code></td><td><code>frog.widgets.button</code></td><td>Button template and state assets</td></tr>
    <tr><td><code>numeric.default.wfrog</code></td><td><code>frog.widgets.numeric_control</code>, <code>frog.widgets.numeric_indicator</code></td><td>Numeric template</td></tr>
    <tr><td><code>chart.default.wfrog</code></td><td><code>frog.widgets.waveform_chart</code></td><td>Chart shell and subobject assets</td></tr>
    <tr><td><code>label.default.wfrog</code></td><td><code>frog.widgets.label</code></td><td>Label template</td></tr>
    <tr><td><code>frame.default.wfrog</code></td><td><code>frog.widgets.frame</code></td><td>Frame template</td></tr>
  </tbody>
</table>

<hr/>

<h2>3. Frame Realization Note</h2>

<p>
For <code>frog.widgets.frame</code>, <code>label.text</code> remains the logical object name, while <code>caption.text</code> is the visible frame title.
The <code>content_region</code> part is a visual grouping region and does not automatically own child-widget semantics.
</p>

<hr/>

<h2>4. Summary</h2>

<p>
The Default realization package posture keeps class law, realization publication, assets, and runtime implementation separated.
</p>
