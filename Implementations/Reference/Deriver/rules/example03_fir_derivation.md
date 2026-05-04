<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 03 FIR Derivation Rules</h1>

<p align="center">
  <strong>Reference derivation rules from object-style UI property write source shape to FIR</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Source Pattern</h2>

<ul>
  <li>one public input <code>status : string</code></li>
  <li>one widget reference <code>ctrl_gain_ref</code></li>
  <li>one <code>frog.ui.property_write</code> to <code>label.text</code></li>
</ul>

<hr/>

<h2>Derivation Mapping</h2>

<table>
  <thead>
    <tr><th>Source surface</th><th>FIR surface</th></tr>
  </thead>
  <tbody>
    <tr><td><code>interface_input(status)</code></td><td><code>public_interface.inputs[]</code></td></tr>
    <tr><td><code>widget_reference(ctrl_gain)</code></td><td><code>ui_bindings.widget_reference_support[]</code></td></tr>
    <tr><td><code>frog.ui.property_write(label.text)</code></td><td><code>ui_bindings.reference_writes[] / execution_model.effects[]</code></td></tr>
  </tbody>
</table>

<hr/>

<h2>Boundary</h2>

<p>This example is a UI effect slice. It does not define widget class law or runtime-private behavior.</p>
