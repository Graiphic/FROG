<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 46</h1>

<p align="center">
  <strong>Private runtime schedule must not be treated as backend contract structure</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#case-name">1. Case Name</a></li>
  <li><a href="#expected">2. Expected</a></li>
  <li><a href="#why">3. Why</a></li>
  <li><a href="#boundary-being-violated">4. Boundary Being Violated</a></li>
  <li><a href="#scenario">5. Scenario</a></li>
  <li><a href="#what-makes-the-case-invalid">6. What Makes the Case Invalid</a></li>
  <li><a href="#why-runtime-schedule-is-not-enough">7. Why Runtime Schedule Is Not Enough</a></li>
  <li><a href="#what-must-not-be-preserved-as-truth">8. What Must Not Be Preserved as Truth</a></li>
  <li><a href="#expected-conformance-result">9. Expected Conformance Result</a></li>
  <li><a href="#rationale">10. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">11. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>

<p>
  Case:
  <code>46_private_runtime_schedule_must_not_be_treated_as_backend_contract_structure</code>
</p>

<hr/>

<h2 id="expected">2. Expected</h2>

<p><strong>Expected:</strong> invalid</p>

<p><strong>Expected reason:</strong> rejected as an architectural collapse.</p>

<p>
  <strong>Expected rejection basis:</strong>
  the case attempts to treat runtime-private scheduling, dispatch ordering,
  queue planning, task partitioning, synchronization strategy, batching policy,
  or executor-specific timing behavior as if it were the public structural
  organization of the backend contract.
</p>

<hr/>

<h2 id="why">3. Why</h2>

<p>
  This case exists to reject the forbidden substitution:
</p>

<pre><code>private runtime schedule
            ->
backend contract structure
</code></pre>

<p>
  A runtime may construct many kinds of private scheduling behavior:
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
  Those scheduling choices may be necessary for implementation efficiency.
  They do not become the public structural organization of the backend contract
  merely because one runtime depends on them.
</p>

<p>
  If a case attempts that promotion, it must be rejected.
</p>

<hr/>

<h2 id="boundary-being-violated">4. Boundary Being Violated</h2>

<p>
  This case violates the normative separation between:
</p>

<ul>
  <li>backend-contract structure that belongs to the public downstream handoff layer, and</li>
  <li>private runtime schedule that belongs only to an implementation-owned realization.</li>
</ul>

<p>
  The invalid substitution is:
</p>

<pre><code>public backend-contract structure
            !=
runtime-private scheduling strategy
</code></pre>

<p>
  The case attempts to let implementation-owned schedule replace the structural
  semantics of the public backend-contract handoff layer.
</p>

<hr/>

<h2 id="scenario">5. Scenario</h2>

<p>
  A valid source program is validated, represented in an execution-facing form,
  and lowered into a backend contract suitable for a backend family or execution target.
</p>

<p>
  A runtime then realizes that backend contract and introduces private scheduling
  choices needed for execution efficiency, concurrency control, synchronization,
  batching, target adaptation, or resource management.
</p>

<p>
  The conformance failure occurs when those runtime-private scheduling choices are then treated as:
</p>

<ul>
  <li>the canonical structure of the backend contract,</li>
  <li>the required public inspection surface for backend-oriented execution organization,</li>
  <li>the semantic owner of backend-contract structural meaning,</li>
  <li>the only organizational model that must be preserved across implementations.</li>
</ul>

<p>
  In that scenario, runtime realization is silently replacing the public backend-contract layer.
</p>

<hr/>

<h2 id="what-makes-the-case-invalid">6. What Makes the Case Invalid</h2>

<p>
  This case is invalid because it promotes downstream private realization into public
  backend-contract structural truth.
</p>

<p>
  More precisely, it is invalid because it does at least one of the following:
</p>

<ul>
  <li>treats runtime-private ordering as if it were the canonical public backend-contract structure,</li>
  <li>makes public inspection depend on private scheduling behavior,</li>
  <li>collapses the backend-contract layer into executor-specific dispatch machinery,</li>
  <li>assumes one implementation’s scheduling strategy is the required structural truth surface for the handoff layer,</li>
  <li>turns scheduling convenience into architectural ownership.</li>
</ul>

<p>
  A conforming implementation may use private runtime scheduling.
  It must not promote that scheduling into backend-contract structure.
</p>

<hr/>

<h2 id="why-runtime-schedule-is-not-enough">7. Why Runtime Schedule Is Not Enough</h2>

<p>
  Runtime-private schedule is not enough because it is downstream, implementation-owned,
  and potentially target-specific.
</p>

<p>
  Different runtimes may realize the same backend contract structure using different
  scheduling strategies.
  That freedom is acceptable only if the public structural handoff layer remains
  independent from those private choices.
</p>

<p>
  If runtime scheduling were enough, then:
</p>

<ul>
  <li>the backend contract would stop being a stable public structural layer,</li>
  <li>independent implementations would become harder to compare,</li>
  <li>conformance would drift toward runtime-specific behavior,</li>
  <li>public inspectability would depend on private scheduling machinery,</li>
  <li>the architecture would lose a critical stage boundary.</li>
</ul>

<p>
  FROG must reject that substitution.
</p>

<hr/>

<h2 id="what-must-not-be-preserved-as-truth">8. What Must Not Be Preserved as Truth</h2>

<p>
  Later layers may preserve runtime-private schedule as implementation machinery,
  but they must not preserve it as the public truth surface of backend-contract structure.
</p>

<p>
  In particular, a conforming pipeline must not preserve as normative truth:
</p>

<ul>
  <li>“the scheduler order is the backend-contract structure,”</li>
  <li>“the runtime dispatch plan is the canonical public backend organization,”</li>
  <li>“this executor’s queue topology is the required cross-implementation structural model for the handoff layer,”</li>
  <li>“because the runtime can execute it, this private schedule is now the public backend-contract structure.”</li>
</ul>

<p>
  The rejected promotion is:
</p>

<pre><code>private runtime schedule
        -/-> backend contract structure
</code></pre>

<hr/>

<h2 id="expected-conformance-result">9. Expected Conformance Result</h2>

<p>
  This case must be rejected as invalid.
</p>

<p>
  The rejection result is not:
</p>

<ul>
  <li>“runtime-private scheduling is forbidden,”</li>
  <li>“implementations may not choose dispatch strategies,”</li>
  <li>“executors may not optimize scheduling.”</li>
</ul>

<p>
  The actual rejection result is:
</p>

<ul>
  <li>runtime-private scheduling may exist,</li>
  <li>but it must not be treated as the public structural organization of the backend contract.</li>
</ul>

<hr/>

<h2 id="rationale">10. Rationale</h2>

<p>
  This rejection matters because FROG explicitly separates:
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
  If runtime-private schedule can replace backend-contract structure, that stack collapses into:
</p>

<pre><code>validated meaning
        ->
backend-oriented handoff
        ->
runtime-private scheduling
        ->
hidden public structure
</code></pre>

<p>
  That is precisely what the repository is trying to avoid.
</p>

<p>
  Independent implementations need freedom to realize the same backend-contract structure
  in different ways.
  They do not need freedom to redefine the public structural handoff layer as private
  runtime scheduling behavior.
</p>

<p>
  The preserved rule remains:
</p>

<pre><code>private runtime schedule
            must remain distinct from
backend contract structure
</code></pre>

<hr/>

<h2 id="minimum-conformance-reading">11. Minimum Conformance Reading</h2>

<p>
  A minimal conforming reading of this case is:
</p>

<ul>
  <li>the case relies on runtime-private scheduling,</li>
  <li>that scheduling is downstream and implementation-owned,</li>
  <li>it is being promoted to the public backend-contract structural layer,</li>
  <li>that promotion breaks the backend-contract / private runtime boundary,</li>
  <li>the case must therefore be rejected as invalid.</li>
</ul>

<p>
  The public truth asserted by this rejection is:
</p>

<pre><code>private runtime schedule
            must not be treated as
backend contract structure
</code></pre>
