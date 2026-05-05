<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Lowering to LLVM Dispatch</h1>

<p align="center">
  <strong>Current lowered-unit-kind dispatch posture for the reference LLVM proof path</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Current Dispatch Key</h2>

<pre><code>lowered_units[0].kind</code></pre>

<hr/>

<h2>Current Mapping</h2>

<table>
  <thead>
    <tr>
      <th>Lowered unit kind</th>
      <th>Emitter behavior</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>pure_addition_kernel</code></td>
      <td>Emits a native <code>f64</code> addition proof.</td>
    </tr>
    <tr>
      <td><code>ui_value_roundtrip_kernel</code></td>
      <td>Emits a native proof of widget-value arithmetic payload.</td>
    </tr>
    <tr>
      <td><code>ui_property_write_effect_unit</code></td>
      <td>Emits a native proof of the UI property-write payload.</td>
    </tr>
    <tr>
      <td><code>stateful_feedback_delay_kernel</code></td>
      <td>Emits a native proof of one explicit-delay state step.</td>
    </tr>
    <tr>
      <td><code>bounded_accumulator_kernel_with_ui_bindings</code></td>
      <td>Emits the checked <code>u16</code> accumulator loop proof with overflow rejection.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Boundary</h2>

<p>
This dispatch is still a non-normative proof path.
It consolidates the current example-specific emitters, but it is not a complete production LLVM backend.
</p>
