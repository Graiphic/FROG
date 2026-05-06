<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Version Matrix</h1>

<p align="center">
  <strong>Centralized current-status table for the published FROG specification corpus</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Executable Example Closure Matrix</h2>

<table>
  <thead>
    <tr>
      <th>Example</th>
      <th>Source</th>
      <th>FIR</th>
      <th>Lowering</th>
      <th>Contract</th>
      <th>Runtime acceptance</th>
      <th>LLVM proof</th>
      <th>Current note</th>
    </tr>
  </thead>
  <tbody>
    <tr><td>01 Pure Addition</td><td>Published</td><td>Published</td><td>Published</td><td>Common surface</td><td>Published</td><td>Published</td><td>Pure arithmetic corridor.</td></tr>
    <tr><td>02 UI Value Roundtrip</td><td>Published</td><td>Published</td><td>Published</td><td>Common surface</td><td>Published</td><td>Published</td><td>Natural <code>widget_value</code> corridor.</td></tr>
    <tr><td>03 UI Property Write</td><td>Published</td><td>Published</td><td>Published</td><td>Common surface</td><td>Published</td><td>Published</td><td>Object-style <code>widget_reference</code> / <code>property_write</code> corridor.</td></tr>
    <tr><td>04 Stateful Feedback Delay</td><td>Published</td><td>Published</td><td>Published</td><td>Common surface</td><td>Published</td><td>Published</td><td>Explicit delay-state corridor.</td></tr>
    <tr><td>05 Bounded UI Accumulator</td><td>Published</td><td>Published</td><td>Published</td><td>Common surface</td><td>Published</td><td>Published</td><td>Primary applicative UI/state/runtime/native reference corridor.</td></tr>
  </tbody>
</table>

<hr/>

<h2>Backend Contract Common Surface</h2>

<p>
Examples <code>01</code> through <code>05</code> now share the common backend-contract unit surface:
</p>

<pre><code>public_io
ui_bindings
execution_kernel
effects
publications
</code></pre>
