<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 02 FIR Derivation Rules</h1>

<p align="center">
  <strong>Reference derivation rules from widget value roundtrip source shape to FIR</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Source Pattern</h2>

<ul>
  <li>two numeric controls read through <code>widget_value</code></li>
  <li>one <code>frog.core.add</code> primitive</li>
  <li>one numeric indicator written through <code>widget_value</code></li>
</ul>

<hr/>

<h2>Derivation Mapping</h2>

<table>
  <thead>
    <tr><th>Source surface</th><th>FIR surface</th></tr>
  </thead>
  <tbody>
    <tr><td><code>widget_value(ctrl_a)</code></td><td><code>ui_bindings.control_bindings[]</code></td></tr>
    <tr><td><code>widget_value(ctrl_b)</code></td><td><code>ui_bindings.control_bindings[]</code></td></tr>
    <tr><td><code>frog.core.add</code></td><td><code>execution_model.primitive_operations[]</code></td></tr>
    <tr><td><code>widget_value(ind_result)</code></td><td><code>ui_bindings.indicator_bindings[] / publications[]</code></td></tr>
  </tbody>
</table>

<hr/>

<h2>Boundary</h2>

<p>This example has no public interface IO, widget_reference, structured control, or explicit state.</p>
