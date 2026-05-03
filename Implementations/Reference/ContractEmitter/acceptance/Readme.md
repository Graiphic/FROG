<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">ContractEmitter Acceptance</h1>

<p align="center">
  <strong>Acceptance posture for the first Example 05 lowering to backend-contract reference emission</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Current Acceptance Target</h2>

<pre><code>Examples/05_bounded_ui_accumulator/main.lowering.json
  -&gt;
Implementations/Reference/ContractEmitter/examples/
  05_bounded_ui_accumulator.reference_host_runtime_ui_binding.contract.json
</code></pre>

<hr/>

<h2>Required Observable Result</h2>

<p>
The emitted contract must match the published Example 05 contract as JSON data.
Formatting differences are not meaningful for acceptance.
</p>

<hr/>

<h2>Command</h2>

<pre><code>python -m Implementations.Reference.ContractEmitter.reference_contract_emitter --check
</code></pre>
