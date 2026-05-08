<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference Runtime</h1>

<p align="center">
  <strong>Runtime-family consumers and contract-driven acceptance checks for the non-normative FROG reference implementation</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Overview</h2>

<p>
This directory coordinates runtime-family checks in the non-normative reference implementation.
The runtime consumes emitted backend contracts and verifies repository-visible runtime acceptance snapshots.
</p>

<p>
The current runtime executor dispatches by <code>contract.units[0].kind</code>.
The source example identifier remains preserved as traceability metadata, but it is not the runtime-executor authority.
</p>

<hr/>

<h2>Current Runtime Surface</h2>

<pre><code>Examples 01–04
  backend contract
    -&gt; contract.units[0].kind
    -&gt; generic contract executor
    -&gt; runtime acceptance snapshot

Example 05
  normalized backend contract + .wfrog package
    -&gt; contract.units[0].kind
    -&gt; generic contract executor
    -&gt; specialized UI/state/overflow acceptance snapshot
</code></pre>

<hr/>

<h2>Current Runtime Executor Kinds</h2>

<table>
  <thead>
    <tr>
      <th>Contract unit kind</th>
      <th>Executor role</th>
      <th>Published examples</th>
    </tr>
  </thead>
  <tbody>
    <tr><td><code>pure_addition_kernel</code></td><td>Pure public-input addition executor.</td><td><code>01_pure_addition</code></td></tr>
    <tr><td><code>ui_value_roundtrip_kernel</code></td><td>Widget-value arithmetic executor.</td><td><code>02_ui_value_roundtrip</code></td></tr>
    <tr><td><code>ui_property_write_effect_unit</code></td><td>Explicit UI property-write effect executor.</td><td><code>03_ui_property_write</code></td></tr>
    <tr><td><code>stateful_feedback_delay_kernel</code></td><td>Explicit delay-backed state-step executor.</td><td><code>04_stateful_feedback_delay</code></td></tr>
    <tr><td><code>bounded_executable_ui_unit</code></td><td>Normalized bounded UI accumulator executor with <code>.wfrog</code> support.</td><td><code>05_bounded_ui_accumulator</code></td></tr>
  </tbody>
</table>

<hr/>

<h2>Normalized Example 05 Runtime</h2>

<p>
The specialized Python runtime and the generic contract executor consume the normalized Example 05 contract surface:
</p>

<pre><code>public_io
ui_bindings
execution_kernel
effects
publications
</code></pre>

<p>
The legacy compatibility fields have been removed from the published Example 05 contract.
</p>

<hr/>

<h2>Commands</h2>

<pre><code>python Implementations/Reference/Runtime/check_examples01_05_runtime_acceptance.py
python Implementations/Reference/Runtime/check_examples01_05_runtime_acceptance.py --skip-example05
python Implementations/Reference/Runtime/python/execute_contract.py 3
python Implementations/Reference/Runtime/execute_reference_contract.py \
  --acceptance Implementations/Reference/Runtime/acceptance/example05_runtime_family.acceptance.json \
  --check
</code></pre>

<hr/>

<h2>Test Coverage</h2>

<p>
The runtime pytest surface includes checks that executor selection depends on <code>contract.units[0].kind</code>, not on <code>example_id</code>.
</p>

<pre><code>python -m pytest Implementations/Reference/Runtime/tests</code></pre>

<hr/>

<h2>Boundary</h2>

<p>
This directory does not define FROG semantics, backend contract law, widget law, or UI host law.
It consumes emitted backend contracts and validates repository-visible runtime behavior for the current bounded reference corridor.
</p>
