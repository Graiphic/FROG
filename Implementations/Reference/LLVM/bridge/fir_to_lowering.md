<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FIR to Lowering Bridge</h1>

<p align="center">
  <strong>Example-scoped bridge notes for the first LLVM closure</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Overview</h2>
<p>
This document explains the narrow bridge used for the first native LLVM closure of <code>05_bounded_ui_accumulator</code>.
</p>

<h2>Accepted FIR Subset</h2>
<ul>
  <li>one scalar numeric input of type <code>u16</code>,</li>
  <li>one explicit local state carrier initialized to <code>0</code>,</li>
  <li>one bounded deterministic loop with exactly <code>5</code> iterations,</li>
  <li>one additive recurrence of the form <code>state = state + input</code>,</li>
  <li>one final publication of the resulting state.</li>
</ul>

<h2>Bridge Rule</h2>
<p>
For this first corridor, FIR is reduced to a single example-specific lowering artifact whose meaning is:
</p>
<pre><code>entry(input_value: i16) -&gt; i16
state := 0
repeat 5 times:
    state := state + input_value
return state
</code></pre>

<h2>Deliberate Limits</h2>
<ul>
  <li>No generalized control-flow selection is claimed.</li>
  <li>No generalized memory model is claimed.</li>
  <li>No generalized widget-host lowering is claimed.</li>
  <li>No generalized event bridge is claimed.</li>
</ul>

<h2>Summary</h2>
<p>
This bridge is intentionally example-scoped. Its role is to make the first native LLVM corridor explicit, not to declare full backend completeness.
</p>
