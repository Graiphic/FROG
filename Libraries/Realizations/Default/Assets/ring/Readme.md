<p align="center">
  <img src="../../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Ring Assets</h1>

<p align="center">
  <strong>SVG assets for the default Ring realization family</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Published Assets</h2>

<ul>
  <li><code>assets/ring/templates/ring_rectangular.svg</code></li>
  <li><code>assets/ring/templates/ring_dropdown_list.svg</code></li>
</ul>

<hr/>

<h2>Required Public Parts</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>value_face</code></li>
  <li><code>value_display</code></li>
  <li><code>selector_face</code></li>
  <li><code>list_panel</code></li>
  <li><code>option_row</code></li>
  <li><code>option_label</code></li>
  <li><code>option_selection_face</code></li>
</ul>

<p>
The closed selector skin publishes <code>root</code>, <code>label</code>,
<code>caption</code>, <code>value_face</code>, <code>value_display</code>, and
<code>selector_face</code>. The dropdown skin publishes <code>list_panel</code>,
<code>option_row</code>, <code>option_label</code>, and
<code>option_selection_face</code>. Selector arrows and focus affordances are
realization or host details, not public Ring parts.
</p>

<hr/>

<h2>Value Posture</h2>

<p>
The SVG asset is visual grammar only. It does not own item labels, item numeric
values, disabled item state, data-entry limits, display-format posture, or popup
behavior. Those properties are supplied by the <code>.frog</code> instance and
resolved through the Default Ring <code>.wfrog</code> package.
</p>
