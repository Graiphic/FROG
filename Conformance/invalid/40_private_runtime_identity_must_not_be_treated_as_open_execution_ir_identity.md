<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 40</h1>

<p align="center">
  <strong>Private runtime identity must not be treated as open execution IR identity</strong><br/>
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
  <code>40_private_runtime_identity_must_not_be_treated_as_open_execution_ir_identity</code>
</p>

<hr/>

<h2 id="expected">2. Expected</h2>

<p><strong>Expected:</strong> invalid</p>

<p><strong>Expected reason:</strong> rejected as an architectural collapse.</p>

<p>
  <strong>Expected rejection basis:</strong>
  the case attempts to treat runtime-private identity as if it were the public identity
  surface of the open execution-facing IR, thereby collapsing the distinction between
  execution representation and private realization.
</p>

<hr/>

<h2 id="why">3. Why</h2>

<p>
  This case exists to reject the forbidden substitution:
</p>

<pre><code>private runtime identity
            ->
open execution IR identity
</code></pre>

<p>
  A runtime may create many kinds of private identity:
</p>

<ul>
  <li>heap object handles,</li>
  <li>scheduler tokens,</li>
  <li>worker-local references,</li>
  <li>buffer ownership identifiers,</li>
  <li>backend-private instance IDs,</li>
  <li>target-specific execution descriptors.</li>
</ul>

<p>
  Those identities may be necessary for implementation.
  They do not become the public identity surface of the open execution-facing IR merely
  because they exist or because one runtime depends on them.
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
  <li>open execution-facing IR identity that belongs to the public derived layer, and</li>
  <li>private runtime identity that belongs only to a downstream realization.</li>
</ul>

<p>
  The invalid substitution is:
</p>

<pre><code>public execution-facing identity
            !=
runtime-private realization identity
</code></pre>

<p>
  The case attempts to let implementation-owned identity replace the identity semantics
  of the public execution-facing layer.
</p>

<hr/>

<h2 id="scenario">5. Scenario</h2>

<p>
  A valid source program is validated and represented in an open execution-facing form.
  A runtime then realizes that form and introduces runtime-private identities needed for
  scheduling, allocation, synchronization, ownership tracking, or backend-specific execution.
</p>

<p>
  The conformance failure occurs when those runtime-private identities are then treated as:
</p>

<ul>
  <li>the canonical identity of the open execution-facing IR,</li>
  <li>the required public inspection surface,</li>
  <li>the semantic owner of execution meaning,</li>
  <li>the only identity model that must be preserved across implementations.</li>
</ul>

<p>
  In that scenario, the runtime layer is silently replacing the public execution-facing layer.
</p>

<hr/>

<h2 id="what-makes-the-case-invalid">6. What Makes the Case Invalid</h2>

<p>
  This case is invalid because it promotes downstream private realization into public
  execution truth.
</p>

<p>
  More precisely, it is invalid because it does at least one of the following:
</p>

<ul>
  <li>treats runtime-private handles as if they were the canonical public IR identity,</li>
  <li>makes public inspection depend on private runtime object identity,</li>
  <li>collapses the open IR layer into backend- or runtime-specific realization machinery,</li>
  <li>assumes one implementation’s identity model is the required execution truth surface,</li>
  <li>turns implementation convenience into architectural ownership.</li>
</ul>

<p>
  A conforming implementation may use private runtime identity.
  It must not promote that identity into the open execution IR layer.
</p>

<hr/>

<h2 id="why-runtime-identity-is-not-enough">7. Why Runtime Identity Is Not Enough</h2>

<p>
  Runtime-private identity is not enough because it is downstream, implementation-owned,
  and potentially target-specific.
</p>

<p>
  Different runtimes may realize the same open execution IR using different identity models.
  That freedom is acceptable only if the public execution-facing layer remains independent
  from those private choices.
</p>

<p>
  If runtime identity were enough, then:
</p>

<ul>
  <li>the open IR would stop being a stable public layer,</li>
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
  but they must not preserve it as the public truth surface of open execution IR identity.
</p>

<p>
  In particular, a conforming pipeline must not preserve as normative truth:
</p>

<ul>
  <li>“the runtime handle is the IR identity,”</li>
  <li>“the object allocated by this runtime is the canonical public execution entity,”</li>
  <li>“the scheduler token is the required cross-implementation identity model,”</li>
  <li>“because the runtime can execute it, this private identity is now the public IR identity.”</li>
</ul>

<p>
  The rejected promotion is:
</p>

<pre><code>private runtime identity
        -/-> open execution IR identity
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
  <li>but it must not be treated as the public identity surface of the open execution-facing IR.</li>
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
private runtime realization
</code></pre>

<p>
  If runtime-private identity can replace open execution IR identity, that stack collapses into:
</p>

<pre><code>validated meaning
        ->
runtime-private machinery
        ->
hidden public truth
</code></pre>

<p>
  That is precisely what the repository is trying to avoid.
</p>

<p>
  Independent implementations need freedom to realize the same open execution layer in
  different ways.
  They do not need freedom to redefine the open execution identity surface as private runtime state.
</p>

<p>
  The preserved rule remains:
</p>

<pre><code>private runtime identity
            must remain distinct from
open execution IR identity
</code></pre>

<hr/>

<h2 id="minimum-conformance-reading">11. Minimum Conformance Reading</h2>

<p>
  A minimal conforming reading of this case is:
</p>

<ul>
  <li>the case relies on runtime-private identity,</li>
  <li>that identity is downstream and implementation-owned,</li>
  <li>it is being promoted to the public IR identity surface,</li>
  <li>that promotion breaks the open IR / private runtime boundary,</li>
  <li>the case must therefore be rejected as invalid.</li>
</ul>

<p>
  The public truth asserted by this rejection is:
</p>

<pre><code>private runtime identity
            must not be treated as
open execution IR identity
</code></pre>
