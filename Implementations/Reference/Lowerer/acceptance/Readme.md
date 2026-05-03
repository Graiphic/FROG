<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Lowerer Acceptance</h1>

<p align="center">
  <strong>Acceptance posture for the first Example 05 FIR to lowered-artifact reference lowering</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Current Acceptance Target</h2>

<pre><code>Examples/05_bounded_ui_accumulator/main.fir.json
  -&gt;
Examples/05_bounded_ui_accumulator/main.lowering.json
</code></pre>

<hr/>

<h2>Required Observable Result</h2>

<p>
The generated lowering must match the published Example 05 lowering as JSON data.
Formatting differences are not meaningful for acceptance.
</p>

<hr/>

<h2>Command</h2>

<pre><code>python Implementations/Reference/Lowerer/lower_example05_fir.py --check
</code></pre>
