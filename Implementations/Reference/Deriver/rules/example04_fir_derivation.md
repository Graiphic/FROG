<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 04 FIR Derivation Rules</h1>

<p align="center">
  <strong>Reference derivation rules from explicit feedback delay source shape to FIR</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Source Pattern</h2>

<ul>
  <li>one public input <code>x : f64</code></li>
  <li>one public output <code>y : f64</code></li>
  <li>one <code>frog.core.delay</code> with initial value <code>0.0</code></li>
  <li>one feedback add operation</li>
</ul>

<hr/>

<h2>Derivation Mapping</h2>

<table>
  <thead>
    <tr><th>Source surface</th><th>FIR surface</th></tr>
  </thead>
  <tbody>
    <tr><td><code>frog.core.delay(initial=0.0)</code></td><td><code>state_model.carrier</code></td></tr>
    <tr><td><code>input_x</code></td><td><code>execution_model.edges[]</code></td></tr>
    <tr><td><code>delay_1.out</code></td><td><code>execution_model.edges[]</code></td></tr>
    <tr><td><code>add_1.result -> delay_1.in</code></td><td><code>state commit surface</code></td></tr>
    <tr><td><code>add_1.result -> output_y</code></td><td><code>publications[]</code></td></tr>
  </tbody>
</table>

<hr/>

<h2>Boundary</h2>

<p>This example preserves a valid feedback cycle only through explicit delay state.</p>
