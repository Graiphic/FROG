<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 05 FIR Derivation Rules</h1>

<p align="center">
  <strong>Reference derivation rules from the frozen Example 05 source shape to FIR</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This document records the first explicit reference derivation rules used by <code>fir_deriver.py</code>.
It is non-normative implementation-workspace material.
It does not replace <code>IR/Derivation rules.md</code>.
</p>

<hr/>

<h2>Source Pattern</h2>

<p>
The current supported source pattern is:
</p>

<ul>
  <li>one public input <code>input_value : u16</code>,</li>
  <li>one public output <code>result : u16</code>,</li>
  <li>one <code>widget_value</code> input binding for <code>ctrl_input</code>,</li>
  <li>one <code>widget_value</code> output binding for <code>ind_result</code>,</li>
  <li>two <code>frog.ui.property_write</code> operations for <code>foreground_color</code>,</li>
  <li>one <code>frog.core.delay</code> explicit state carrier with initial value <code>0</code>,</li>
  <li>one bounded <code>for_loop</code> named <code>loop_accumulate</code>,</li>
  <li>one accumulation expression <code>state_next = state_current + input_value</code>,</li>
  <li>publication to public output and indicator widget value.</li>
</ul>

<hr/>

<h2>Derivation Mapping</h2>

<table>
  <thead>
    <tr>
      <th>Source surface</th>
      <th>FIR surface</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>metadata.name</code></td>
      <td><code>source_ref.example_id</code></td>
    </tr>
    <tr>
      <td><code>interface.inputs[]</code></td>
      <td><code>units[0].public_interface.inputs[]</code></td>
    </tr>
    <tr>
      <td><code>interface.outputs[]</code></td>
      <td><code>units[0].public_interface.outputs[]</code></td>
    </tr>
    <tr>
      <td><code>front_panel.package_refs[0]</code></td>
      <td><code>front_panel_ref.package_path</code></td>
    </tr>
    <tr>
      <td><code>widget_value(widget=ctrl_input)</code></td>
      <td><code>ui_bindings.control_bindings[]</code></td>
    </tr>
    <tr>
      <td><code>widget_value(widget=ind_result)</code></td>
      <td><code>ui_bindings.indicator_bindings[]</code></td>
    </tr>
    <tr>
      <td><code>widget_reference + frog.ui.property_write</code></td>
      <td><code>ui_bindings.reference_writes[]</code></td>
    </tr>
    <tr>
      <td><code>frog.core.delay</code> initial constant</td>
      <td><code>state_model.carrier</code></td>
    </tr>
    <tr>
      <td><code>for_loop count</code></td>
      <td><code>execution_model.iteration_count</code></td>
    </tr>
    <tr>
      <td>accumulator loop body</td>
      <td><code>execution_model.body_rule.expression</code></td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Boundary</h2>

<p>
These rules are deliberately narrow.
They capture the frozen Example 05 source pattern and preserve its source-to-FIR mapping.
They are not a general FROG derivation engine yet.
</p>
