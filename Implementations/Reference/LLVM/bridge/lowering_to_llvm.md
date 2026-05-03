<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Lowering to LLVM Bridge</h1>

<p align="center">
  <strong>Bridge posture from the Example 05 lowered kernel to the LLVM-oriented native proof</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Current Mapping</h2>

<table>
  <thead>
    <tr>
      <th>Lowered kernel surface</th>
      <th>LLVM module surface</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>initial_state = 0</code></td>
      <td>Initial incoming value of <code>%state_current</code> in the loop PHI.</td>
    </tr>
    <tr>
      <td><code>state_type = u16</code></td>
      <td><code>i16</code> state and return type for <code>@frog_example05_accumulate</code>.</td>
    </tr>
    <tr>
      <td><code>iteration_count = 5</code></td>
      <td><code>%done = icmp uge i32 %i, 5</code>.</td>
    </tr>
    <tr>
      <td><code>state_next = state_current + input_value</code></td>
      <td><code>%state_next = add i16 %state_current, %input_value</code>.</td>
    </tr>
    <tr>
      <td><code>state_current &lt;- state_next after each iteration</code></td>
      <td>Loop PHI update from <code>%state_next</code> back to <code>%state_current</code>.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Boundary</h2>

<p>
This bridge is intentionally narrow.
It maps the frozen Example 05 lowered kernel into one LLVM-native proof module.
It does not define a general LLVM backend.
</p>

<p>
The rendered front panel remains outside the current native proof.
UI and widget effects remain runtime-mediated.
</p>
