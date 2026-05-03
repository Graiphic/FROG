<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Deriver Acceptance</h1>

<p align="center">
  <strong>Acceptance posture for the first Example 05 .frog to FIR reference derivation</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Current Acceptance Target</h2>

<pre><code>Examples/05_bounded_ui_accumulator/main.frog
  -&gt;
Examples/05_bounded_ui_accumulator/main.fir.json
</code></pre>

<hr/>

<h2>Required Observable Result</h2>

<p>
The generated FIR must match the published Example 05 FIR as JSON data.
Formatting differences are not meaningful for acceptance.
</p>

<hr/>

<h2>Command</h2>

<pre><code>python Implementations/Reference/Deriver/derive_example05_fir.py --check
</code></pre>
