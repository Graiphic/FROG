<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 43</h1>

<p align="center">
  <strong>Open execution IR structure must remain distinct from private runtime schedule</strong><br/>
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
  <code>43_open_execution_ir_structure_must_remain_distinct_from_private_runtime_schedule</code>
</p>

<hr/>

<h2 id="expected">2. Expected</h2>

<p><strong>Expected:</strong> valid</p>

<p><strong>Expected meaning:</strong> language-level meaning is established.</p>

<p>
  <strong>Expected preservation:</strong>
  open execution-facing IR structure remains distinct from any private runtime schedule,
  dispatch plan, work ordering, queue strategy, task partitioning, or executor-specific
  timing plan introduced during realization.
</p>

<hr/>

<h2 id="why">3. Why</h2>

<p>
  This case exists to make one architectural rule explicit:
</p>

<pre><code>open execution IR structure
            !=
private runtime schedule
</code></pre>

<p>
  An open execution-facing IR may carry public structural information about executable regions,
  dependencies, state boundaries, structure participation, explicit terminals, or other execution-relevant
  organization derived from validated meaning.
</p>

<p>
  A runtime may then choose its own private schedule for realization purposes:
</p>

<ul>
  <li>task ordering,</li>
  <li>queue topology,</li>
  <li>worker assignment,</li>
  <li>batching strategy,</li>
  <li>barrier placement,</li>
  <li>concurrency partitioning,</li>
  <li>target-specific dispatch sequencing.</li>
</ul>

<p>
  Those schedule choices may exist.
  They do not become the same thing as the public structural organization of the open execution-facing IR.
</p>

<p>
  The public execution-facing structure must remain distinguishable from runtime-private scheduling.
</p>

<hr/>

<h2 id="boundary-being-exercised">4. Boundary Being Exercised</h2>

<p>
  This case exercises the preservation boundary across:
</p>

<pre><code>validated program meaning
        |
        v
open execution-facing IR structure
        |
        v
private runtime scheduling and realization
</code></pre>

<p>
  The distinction under test is between:
</p>

<ul>
  <li>structure that belongs to the public execution-facing representation, and</li>
  <li>ordering or scheduling introduced only by one runtime realization.</li>
</ul>

<hr/>

<h2 id="scenario">5. Scenario</h2>

<p>
  A valid FROG program is validated and represented in an open execution-facing form.
  That representation includes structural organization that is meaningful at the public derived layer,
  such as explicit dependency shape, region boundaries, state-related boundaries, or execution-relevant grouping
  derived from validated semantics.
</p>

<p>
  A downstream runtime then realizes that execution-facing structure and introduces its own private schedule
  for execution efficiency, target adaptation, concurrency control, synchronization, or resource management.
</p>

<p>
  The case is valid if:
</p>

<ul>
  <li>the open execution-facing structural organization remains meaningful as part of the public derived representation,</li>
  <li>the runtime may still choose its own private scheduling strategy,</li>
  <li>the implementation does not collapse public structure into runtime-private ordering.</li>
</ul>

<hr/>

<h2 id="what-makes-the-case-valid">6. What Makes the Case Valid</h2>

<p>
  This case is valid because it preserves the published stage boundary between open execution-facing structure
  and runtime-private scheduling.
</p>

<p>
  More precisely, it is valid because:
</p>

<ul>
  <li>validated meaning is upstream from runtime realization,</li>
  <li>an open execution-facing representation is allowed to carry public structural organization,</li>
  <li>runtime realization may introduce private scheduling choices,</li>
  <li>the two are not silently treated as the same ownership layer.</li>
</ul>

<p>
  The case does not require that all runtimes execute the same work in the same order,
  with the same queues, barriers, workers, or dispatch plan.
</p>

<p>
  It does require that runtime-private schedule does not overwrite, erase, or become confused with
  the structure of the open execution-facing representation.
</p>

<hr/>

<h2 id="expected-meaning">7. Expected Meaning</h2>

<p>
  If this case validates, the established meaning includes at least the following:
</p>

<ul>
  <li>open execution-facing IR may have its own public structural organization,</li>
  <li>private runtime scheduling may exist downstream for realization purposes,</li>
  <li>private runtime schedule does not become the owner of open execution structure,</li>
  <li>the distinction between open IR structure and runtime-private schedule remains preserved.</li>
</ul>

<p>
  The semantic reading is therefore:
</p>

<pre><code>validated meaning
        ->
open execution-facing structure
        ->
private runtime scheduling
</code></pre>

<p>Not:</p>

<pre><code>runtime-private scheduling plan
        replaces
open execution-facing structure
</code></pre>

<hr/>

<h2 id="expected-preservation">8. Expected Preservation</h2>

<p>
  A conforming implementation must preserve the distinction between open execution IR structure
  and private runtime schedule.
</p>

<p>
  At minimum, later layers must not silently collapse:
</p>

<ul>
  <li>public execution-facing structure, into</li>
  <li>opaque runtime-private scheduling behavior.</li>
</ul>

<p>
  This does not require that an implementation publish its internal dispatch logic as part of the IR.
</p>

<p>
  It does require that:
</p>

<ul>
  <li>open execution-facing structural organization remains attributable as part of the public derived layer,</li>
  <li>runtime-private scheduling remains clearly downstream and implementation-owned,</li>
  <li>the public structure / private schedule boundary is not erased for convenience,</li>
  <li>independent implementations remain free to realize the same open execution-facing structure with different private schedules.</li>
</ul>

<hr/>

<h2 id="what-must-not-happen">9. What Must Not Happen</h2>

<p>
  A conforming implementation must not do any of the following:
</p>

<ul>
  <li>treat private runtime ordering as if it were the canonical public structure of the open execution-facing IR,</li>
  <li>erase public execution-facing structure once runtime dispatch planning begins,</li>
  <li>make conformance or inspection depend on access to private runtime scheduling details,</li>
  <li>claim that scheduler behavior is sufficient explanation of open execution-facing structure,</li>
  <li>collapse the public IR / private runtime boundary into one hidden execution layer.</li>
</ul>

<p>
  The forbidden collapse is:
</p>

<pre><code>open execution IR structure
        -/-> private runtime schedule
</code></pre>

<hr/>

<h2 id="rationale">10. Rationale</h2>

<p>
  This distinction matters because FROG explicitly aims to keep open execution representation separate
  from private realization strategy.
</p>

<p>
  If open execution-facing structure were allowed to collapse into runtime-private schedule, then:
</p>

<ul>
  <li>the IR would stop being a meaningful public structural layer,</li>
  <li>independent implementations would become harder to compare,</li>
  <li>conformance would become implementation-dependent,</li>
  <li>scheduler-private behavior would drift upward into public truth,</li>
  <li>backend portability and inspectability would weaken.</li>
</ul>

<p>
  The repository-level architectural rule must remain:
</p>

<pre><code>open execution-facing structural organization
            remains distinct from
private runtime scheduling strategy
</code></pre>

<p>
  Structure and schedule are one of the easiest places where a hidden runtime can silently
  replace a public representation.
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
  <li>an open execution-facing representation may carry public structure,</li>
  <li>a private runtime may introduce downstream scheduling choices,</li>
  <li>the two layers must remain distinct.</li>
</ul>

<p>
  The public truth asserted by this case is:
</p>

<pre><code>open execution IR structure
            must remain distinct from
private runtime schedule
</code></pre>
