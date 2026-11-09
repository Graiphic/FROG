<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 42</h1>

<p align="center">
  <strong>Private runtime identity must not be treated as backend contract identity</strong><br/>
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
  <li><a href="#why-runtime-identity-is-not-enough">7. Why Runtime Identity Is Not Enough</a></li>
  <li><a href="#what-must-not-be-preserved-as-truth">8. What Must Not Be Preserved as Truth</a></li>
  <li><a href="#expected-conformance-result">9. Expected Conformance Result</a></li>
  <li><a href="#rationale">10. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">11. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>

<p>
  Case:
  <code>42_private_runtime_identity_must_not_be_treated_as_backend_contract_identity</code>
</p>

<hr/>

<h2 id="expected">2. Expected</h2>

<p><strong>Expected:</strong> invalid</p>

<p><strong>Expected reason:</strong> rejected as an architectural collapse.</p>

<p>
  <strong>Expected rejection basis:</strong>
  the case attempts to treat runtime-private identity as if it were the public identity
  surface of the backend contract, thereby collapsing the distinction between published
  backend handoff and downstream private realization.
</p>

<hr/>

<h2 id="why">3. Why</h2>

<p>
  This case exists to reject the forbidden substitution:
</p>

<pre><code>private runtime identity
            ->
backend contract identity
</code></pre>

<p>
  A runtime may create many kinds of private identity:
</p>

<ul>
  <li>heap object handles,</li>
  <li>buffer ownership references,</li>
  <li>scheduler tokens,</li>
  <li>worker-local execution handles,</li>
  <li>allocation descriptors,</li>
  <li>target-private instance identifiers.</li>
</ul>

<p>
  Those identities may be necessary for implementation.
  They do not become the public identity surface of the backend contract merely because
  one runtime depends on them.
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
  <li>backend-contract identity that belongs to the public downstream handoff layer, and</li>
  <li>private runtime identity that belongs only to an implementation-owned realization.</li>
</ul>

<p>
  The invalid substitution is:
</p>

<pre><code>public backend-contract identity
            !=
runtime-private realization identity
</code></pre>

<p>
  The case attempts to let implementation-owned identity replace the identity semantics
  of the public backend-contract handoff surface.
</p>

<hr/>

<h2 id="scenario">5. Scenario</h2>

<p>
  A valid source program is validated, represented in an execution-facing form, and lowered
  into a backend contract suitable for a backend family or execution target.
</p>

<p>
  A runtime then realizes that backend contract and introduces runtime-private identities
  needed for scheduling, memory ownership, buffering, synchronization, allocation,
  or target-specific execution.
</p>

<p>
  The conformance failure occurs when those runtime-private identities are then treated as:
</p>

<ul>
  <li>the canonical identity of the backend contract,</li>
  <li>the required public inspection surface for the handoff layer,</li>
  <li>the semantic owner of backend-oriented execution meaning,</li>
  <li>the only identity model that must be preserved across implementations.</li>
</ul>

<p>
  In that scenario, the runtime layer is silently replacing the backend-contract layer.
</p>

<hr/>

<h2 id="what-makes-the-case-invalid">6. What Makes the Case Invalid</h2>

<p>
  This case is invalid because it promotes downstream private realization into public
  backend-contract truth.
</p>

<p>
  More precisely, it is invalid because it does at least one of the following:
</p>

<ul>
  <li>treats runtime-private handles as if they were the canonical backend-contract identity surface,</li>
  <li>makes public inspection depend on private runtime object identity,</li>
  <li>collapses the backend-contract layer into backend-specific or runtime-specific realization machinery,</li>
  <li>assumes one implementation’s identity model is the required public handoff truth surface,</li>
  <li>turns implementation convenience into architectural ownership.</li>
</ul>

<p>
  A conforming implementation may use private runtime identity.
  It must not promote that identity into backend-contract identity.
</p>

<hr/>

<h2 id="why-runtime-identity-is-not-enough">7. Why Runtime Identity Is Not Enough</h2>

<p>
  Runtime-private identity is not enough because it is downstream, implementation-owned,
  and potentially target-specific.
</p>

<p>
  Different runtimes may realize the same backend contract using different identity models.
  That freedom is acceptable only if the public handoff layer remains independent from
  those private choices.
</p>

<p>
  If runtime identity were enough, then:
</p>

<ul>
  <li>the backend contract would stop being a stable public handoff layer,</li>
  <li>independent implementations would become harder to compare,</li>
  <li>conformance would drift toward runtime-specific behavior,</li>
  <li>public inspectability would depend on private machinery,</li>
  <li>the architecture would lose a critical stage boundary.</li>
</ul>

<p>
  FROG must reject that substitution.
</p>

<hr/>

<h2 id="what-must-not-be-preserved-as-truth">8. What Must Not Be Preserved as Truth</h2>

<p>
  Later layers may preserve runtime-private identity as implementation machinery,
  but they must not preserve it as the public truth surface of backend-contract identity.
</p>

<p>
  In particular, a conforming pipeline must not preserve as normative truth:
</p>

<ul>
  <li>“the runtime handle is the backend-contract identity,”</li>
  <li>“the object allocated by this runtime is the canonical public backend entity,”</li>
  <li>“the scheduler token is the required cross-implementation handoff identity model,”</li>
  <li>“because the runtime can execute it, this private identity is now the public backend-contract identity.”</li>
</ul>

<p>
  The rejected promotion is:
</p>

<pre><code>private runtime identity
        -/-> backend contract identity
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
  <li>“runtime-private identity is forbidden,”</li>
  <li>“implementations may not create internal handles,”</li>
  <li>“backends may not use private instance models.”</li>
</ul>

<p>
  The actual rejection result is:
</p>

<ul>
  <li>runtime-private identity may exist,</li>
  <li>but it must not be treated as the public identity surface of the backend contract.</li>
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
  If runtime-private identity can replace backend-contract identity, that stack collapses into:
</p>

<pre><code>validated meaning
        ->
backend-oriented handoff
        ->
runtime-private machinery
        ->
hidden public truth
</code></pre>

<p>
  That is precisely what the repository is trying to avoid.
</p>

<p>
  Independent implementations need freedom to realize the same backend contract in
  different ways.
  They do not need freedom to redefine the public handoff identity surface as private runtime state.
</p>

<p>
  The preserved rule remains:
</p>

<pre><code>private runtime identity
            must remain distinct from
backend contract identity
</code></pre>

<hr/>

<h2 id="minimum-conformance-reading">11. Minimum Conformance Reading</h2>

<p>
  A minimal conforming reading of this case is:
</p>

<ul>
  <li>the case relies on runtime-private identity,</li>
  <li>that identity is downstream and implementation-owned,</li>
  <li>it is being promoted to the public backend-contract identity surface,</li>
  <li>that promotion breaks the backend-contract / private runtime boundary,</li>
  <li>the case must therefore be rejected as invalid.</li>
</ul>

<p>
  The public truth asserted by this rejection is:
</p>

<pre><code>private runtime identity
            must not be treated as
backend contract identity
</code></pre>
