<h1>Reference Contract Emitter</h1>

<p>Backend-contract emission stage for the non-normative FROG reference implementation.</p>

<hr/>

<h2>Current Contract Surface</h2>

<ul>
  <li>Examples <code>01</code> through <code>04</code> use <code>emit_examples01_04_contracts.py</code>.</li>
  <li>Example <code>05</code> uses <code>reference_contract_emitter.py</code>.</li>
</ul>

<hr/>

<h2>Checks</h2>

<pre><code>python Implementations/Reference/ContractEmitter/emit_examples01_04_contracts.py --check
python -m Implementations.Reference.ContractEmitter.reference_contract_emitter --check
</code></pre>

<hr/>

<h2>Example 05 Normalized Contract Surface</h2>

<p>
The Example 05 contract now exposes the normalized surface only:
</p>

<ul>
  <li><code>example_id</code></li>
  <li><code>artifact_refs</code></li>
  <li><code>units[].public_io</code></li>
  <li><code>units[].ui_bindings</code></li>
  <li><code>units[].execution_kernel</code></li>
  <li><code>units[].effects</code></li>
  <li><code>units[].publications</code></li>
</ul>

<p>
The legacy compatibility fields have been removed:
</p>

<ul>
  <li><code>public_interface</code></li>
  <li><code>ui_binding</code></li>
  <li><code>state_model</code></li>
  <li><code>execution_model</code></li>
  <li><code>property_writes</code></li>
  <li><code>public_output_publication</code></li>
</ul>

<hr/>

<p>
Contract emission is downstream from FIR and lowering.
It does not define FROG semantics.
</p>
