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

<h2>Contract-to-runtime direction</h2>

<p>
The current Examples <code>01</code> through <code>04</code> contracts are intentionally shaped so that runtime acceptance can be executed by a generic contract executor rather than by hard-coded example identifiers.
</p>

<p>
Example <code>04_stateful_feedback_delay</code> now carries <code>execution_kernel.state_id</code> in its lowering and contract so that runtime snapshots can identify the delay state without relying on an implicit name.
</p>

<hr/>

<p>
Contract emission is downstream from FIR and lowering.
It does not define FROG semantics.
</p>
