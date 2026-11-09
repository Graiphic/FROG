<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 45</h1>

<p align="center">
  <strong>Backend contract structure must remain distinct from private runtime schedule</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#case-name">1. Case Name</a></li>
  <li><a href="#expected">2. Expected</a></li>
  <li><a href="#why">3. Why</a></li>
  <li><a href="#boundary-being-exercised">4. Boundary Being Exercised</a></li>
  <li><a href="#scenario">5. Scenario</a></li>
  <li><a href="#what-makes-the-case-valid">6. What Makes the Case Valid</a></li>
  <li><a href="#expected-meaning">7. Expected Meaning</a></li>
  <li><a href="#expected-preservation">8. Expected Preservation</a></li>
  <li><a href="#what-must-not-happen">9. What Must Not Happen</a></li>
  <li><a href="#rationale">10. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">11. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>

<p>
  Case:
  <code>45_backend_contract_structure_must_remain_distinct_from_private_runtime_schedule</code>
</p>

<hr/>

<h2 id="expected">2. Expected</h2>

<p><strong>Expected:</strong> valid</p>

<p><strong>Expected meaning:</strong> language-level meaning is established.</p>

<p>
  <strong>Expected preservation:</strong>
  backend-contract structure remains distinct from any private runtime schedule,
  dispatch ordering, queue planning, task partitioning, synchronization plan,
  batching strategy, or executor-specific timing behavior introduced during realization.
</p>

<hr/>

<h2 id="why">3. Why</h2>

<p>
  This case exists to make one architectural rule explicit:
</p>

<pre><code>backend contract structure
            !=
private runtime schedule
</code></pre>

<p>
  A backend contract is a published downstream handoff surface.
  It may carry structural organization relevant to a backend family, such as execution regions,
  dependency groupings, state-related boundaries, resource-oriented partitions, or other
  execution-relevant organization that remains above private runtime realization.
</p>

<p>
  A runtime may then choose its own private schedule for realization purposes:
</p>

<ul>
  <li>dispatch order,</li>
  <li>queue topology,</li>
  <li>worker assignment,</li>
  <li>barrier placement,</li>
  <li>batching strategy,</li>
  <li>target-specific sequencing,</li>
  <li>executor-private concurrency control.</li>
</ul>

<p>
  Those scheduling choices may exist.
  They do not become the same thing as backend-contract structure.
</p>

<p>
  The public backend-contract structure must remain distinguishable from runtime-private scheduling.
</p>

<hr/>

<h2 id="boundary-being-exercised">4. Boundary Being Exercised</h2>

<p>
  This case exercises the preservation boundary across:
</p>

<pre><code>validated program meaning
        |
        v
open execution-facing IR
        |
        v
backend contract structure
        |
        v
private runtime scheduling and realization
</code></pre>

<p>
  The distinction under test is between:
</p>

<ul>
  <li>structure that belongs to the public backend-contract handoff layer, and</li>
  <li>ordering or scheduling introduced only by one private runtime realization.</li>
</ul>

<hr/>

<h2 id="scenario">5. Scenario</h2>

<p>
  A valid FROG program is validated, represented in an open execution-facing form, and lowered
  into a backend contract suitable for a backend family or execution target.
</p>

<p>
  That backend contract contains structural organization that is meaningful at the public handoff layer,
  such as explicit dependency groupings, execution partitions, state-related boundaries,
  backend-facing regions, or other organization derived from validated semantics and lowering rules.
</p>

<p>
  A downstream runtime then realizes that backend-contract structure and introduces its own private
  schedule for execution efficiency, target adaptation, concurrency control, synchronization,
  batching, or resource management.
</p>

<p>
  The case is valid if:
</p>

<ul>
  <li>the backend-contract structural organization remains meaningful as part of the public handoff surface,</li>
  <li>the runtime may still choose its own private scheduling strategy,</li>
  <li>the implementation does not collapse public backend-contract structure into runtime-private ordering.</li>
</ul>

<hr/>

<h2 id="what-makes-the-case-valid">6. What Makes the Case Valid</h2>

<p>
  This case is valid because it preserves the published stage boundary between backend-contract structure
  and runtime-private scheduling.
</p>

<p>
  More precisely, it is valid because:
</p>

<ul>
  <li>validated meaning and derivation remain upstream from runtime realization,</li>
  <li>a backend contract is allowed to carry public structural organization at the handoff layer,</li>
  <li>runtime realization may introduce private scheduling choices,</li>
  <li>the two are not silently treated as the same ownership layer.</li>
</ul>

<p>
  The case does not require that all runtimes execute the same work in the same order,
  with the same queues, barriers, workers, or dispatch plan.
</p>

<p>
  It does require that runtime-private schedule does not overwrite, erase, or become confused with
  the structure of the backend-contract handoff surface.
</p>

<hr/>

<h2 id="expected-meaning">7. Expected Meaning</h2>

<p>
  If this case validates, the established meaning includes at least the following:
</p>

<ul>
  <li>backend contract may have its own public structural organization,</li>
  <li>private runtime scheduling may exist downstream for realization purposes,</li>
  <li>private runtime schedule does not become the owner of backend-contract structure,</li>
  <li>the distinction between backend-contract structure and runtime-private schedule remains preserved.</li>
</ul>

<p>
  The semantic reading is therefore:
</p>

<pre><code>validated meaning
        ->
open execution-facing IR
        ->
backend-contract structure
        ->
private runtime scheduling
</code></pre>

<p>Not:</p>

<pre><code>runtime-private scheduling plan
        replaces
backend-contract structure
</code></pre>

<hr/>

<h2 id="expected-preservation">8. Expected Preservation</h2>

<p>
  A conforming implementation must preserve the distinction between backend-contract structure
  and private runtime schedule.
</p>

<p>
  At minimum, later layers must not silently collapse:
</p>

<ul>
  <li>public backend-contract structure, into</li>
  <li>opaque runtime-private scheduling behavior.</li>
</ul>

<p>
  This does not require that an implementation publish its internal dispatch logic as part of the backend contract.
</p>

<p>
  It does require that:
</p>

<ul>
  <li>backend-contract structural organization remains attributable as part of the public handoff layer,</li>
  <li>runtime-private scheduling remains clearly downstream and implementation-owned,</li>
  <li>the public handoff structure / private schedule boundary is not erased for convenience,</li>
  <li>independent implementations remain free to realize the same backend-contract structure with different private schedules.</li>
</ul>

<hr/>

<h2 id="what-must-not-happen">9. What Must Not Happen</h2>

<p>
  A conforming implementation must not do any of the following:
</p>

<ul>
  <li>treat private runtime ordering as if it were the canonical public structure of the backend contract,</li>
  <li>erase public backend-contract structure once runtime dispatch planning begins,</li>
  <li>make conformance or inspection depend on access to private runtime scheduling details,</li>
  <li>claim that scheduler behavior is sufficient explanation of backend-contract structure,</li>
  <li>collapse the backend-contract / private runtime boundary into one hidden execution layer.</li>
</ul>

<p>
  The forbidden collapse is:
</p>

<pre><code>backend contract structure
        -/-> private runtime schedule
</code></pre>

<hr/>

<h2 id="rationale">10. Rationale</h2>

<p>
  This distinction matters because FROG explicitly aims to keep the architectural descent visible:
</p>

<pre><code>validated meaning
        ->
open execution-facing IR
        ->
backend contract
        ->
private runtime realization
</code></pre>

<p>
  If backend-contract structure were allowed to collapse into runtime-private schedule, then:
</p>

<ul>
  <li>the backend contract would stop being a meaningful public structural handoff layer,</li>
  <li>independent implementations would become harder to compare,</li>
  <li>conformance would become implementation-dependent,</li>
  <li>scheduler-private behavior would drift upward into public truth,</li>
  <li>backend portability and inspectability would weaken.</li>
</ul>

<p>
  The repository-level architectural rule must remain:
</p>

<pre><code>backend-contract structural organization
            remains distinct from
private runtime scheduling strategy
</code></pre>

<p>
  Structure and schedule are one of the easiest places where a hidden runtime can silently
  replace a public handoff representation.
  This case ensures that it does not.
</p>

<hr/>

<h2 id="minimum-conformance-reading">11. Minimum Conformance Reading</h2>

<p>
  A minimal conforming reading of this case is:
</p>

<ul>
  <li>the source is valid,</li>
  <li>language-level meaning is established,</li>
  <li>a backend contract may carry public structural organization,</li>
  <li>a private runtime may introduce downstream scheduling choices,</li>
  <li>the two layers must remain distinct.</li>
</ul>

<p>
  The public truth asserted by this case is:
</p>

<pre><code>backend contract structure
            must remain distinct from
private runtime schedule
</code></pre>
