<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 01 FIR Derivation Rules</h1>

<p align="center">
  <strong>Reference derivation rules from the pure addition source shape to FIR</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Source Pattern</h2>

<ul>
  <li>two public inputs <code>a : f64</code>, <code>b : f64</code></li>
  <li>one public output <code>result : f64</code></li>
  <li>one <code>frog.core.add</code> primitive</li>
</ul>

<hr/>

<h2>Derivation Mapping</h2>

<table>
  <thead>
    <tr><th>Source surface</th><th>FIR surface</th></tr>
  </thead>
  <tbody>
    <tr><td><code>interface.inputs[]</code></td><td><code>public_interface.inputs[]</code></td></tr>
    <tr><td><code>interface.outputs[]</code></td><td><code>public_interface.outputs[]</code></td></tr>
    <tr><td><code>frog.core.add</code></td><td><code>execution_model.primitive_operations[]</code></td></tr>
    <tr><td><code>source edges</code></td><td><code>execution_model.edges[]</code></td></tr>
    <tr><td><code>interface_output(result)</code></td><td><code>publications[]</code></td></tr>
  </tbody>
</table>

<hr/>

<h2>Boundary</h2>

<p>This example has no front panel, widget participation, structured control, or explicit state.</p>
