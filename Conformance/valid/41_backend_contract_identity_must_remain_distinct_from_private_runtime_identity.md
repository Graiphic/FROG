<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 41</h1>

<p align="center">
  <strong>Backend contract identity must remain distinct from private runtime identity</strong><br/>
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
  <code>41_backend_contract_identity_must_remain_distinct_from_private_runtime_identity</code>
</p>

<hr/>

<h2 id="expected">2. Expected</h2>

<p><strong>Expected:</strong> valid</p>

<p><strong>Expected meaning:</strong> language-level meaning is established.</p>

<p>
  <strong>Expected preservation:</strong>
  backend-contract identity remains distinct from any private runtime identity,
  even when a downstream runtime introduces additional handles, object references,
  allocation owners, scheduling tokens, or target-private execution descriptors.
</p>

<hr/>

<h2 id="why">3. Why</h2>

<p>
  This case exists to make one architectural rule explicit:
</p>

<pre><code>backend contract identity
            !=
private runtime identity
</code></pre>

<p>
  A backend contract is a published downstream handoff surface.
  It may specialize assumptions relative to upstream execution-facing forms,
  but it still belongs to the public architectural stack rather than to one
  implementation’s hidden runtime realization.
</p>

<p>
  A runtime may then introduce additional private identities for execution convenience:
</p>

<ul>
  <li>heap object handles,</li>
  <li>scheduler-owned tokens,</li>
  <li>buffer ownership references,</li>
  <li>worker-local object identities,</li>
  <li>target-private allocation descriptors,</li>
  <li>backend-specific runtime instance IDs.</li>
</ul>

<p>
  Those private identities may exist.
  They do not become the same thing as backend-contract identity.
</p>

<p>
  The backend-contract layer must remain distinguishable from private runtime realization.
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
backend contract
        |
        v
private runtime realization
</code></pre>

<p>
  The distinction under test is between:
</p>

<ul>
  <li>identity that belongs to the backend-contract handoff surface, and</li>
  <li>identity introduced only by a private runtime realization.</li>
</ul>

<hr/>

<h2 id="scenario">5. Scenario</h2>

<p>
  A valid FROG program is validated, derived into an execution-facing representation,
  and then lowered into a backend contract suitable for a backend family or execution target.
</p>

<p>
  That backend contract contains execution-relevant identities, references, or named entities
  that are part of the public handoff surface rather than mere hidden runtime machinery.
</p>

<p>
  A downstream runtime then realizes that backend contract and introduces additional private
  identities for allocation, scheduling, synchronization, memory ownership, buffering,
  or target-specific execution convenience.
</p>

<p>
  The case is valid if:
</p>

<ul>
  <li>the backend-contract identities remain meaningful as part of the public handoff surface,</li>
  <li>the runtime may still use private identities internally,</li>
  <li>the implementation does not collapse backend-contract identity into runtime-private ownership.</li>
</ul>

<hr/>

<h2 id="what-makes-the-case-valid">6. What Makes the Case Valid</h2>

<p>
  This case is valid because it preserves the published stage boundary between backend contract
  and private runtime realization.
</p>

<p>
  More precisely, it is valid because:
</p>

<ul>
  <li>validated meaning and derivation remain upstream from runtime realization,</li>
  <li>a backend contract is allowed to carry its own public identity surface,</li>
  <li>runtime-private realization may introduce additional downstream identity,</li>
  <li>the two are not silently treated as the same ownership layer.</li>
</ul>

<p>
  The case does not require that all runtimes expose the same internal identity model.
</p>

<p>
  It does require that implementation-private identity does not overwrite, erase, or become
  confused with the identity semantics of the backend-contract handoff surface.
</p>

<hr/>

<h2 id="expected-meaning">7. Expected Meaning</h2>

<p>
  If this case validates, the established meaning includes at least the following:
</p>

<ul>
  <li>backend contract may have its own public identity surface,</li>
  <li>private runtime identity may exist downstream for realization purposes,</li>
  <li>private runtime identity does not become the owner of backend-contract meaning,</li>
  <li>the distinction between backend-contract identity and runtime-private identity remains preserved.</li>
</ul>

<p>
  The semantic reading is therefore:
</p>

<pre><code>validated meaning
        ->
open execution-facing IR
        ->
backend-contract identity
        ->
private runtime realization
</code></pre>

<p>Not:</p>

<pre><code>runtime-private identity
        replaces
backend-contract identity
</code></pre>

<hr/>

<h2 id="expected-preservation">8. Expected Preservation</h2>

<p>
  A conforming implementation must preserve the distinction between backend-contract identity
  and private runtime identity.
</p>

<p>
  At minimum, later layers must not silently collapse:
</p>

<ul>
  <li>public backend-contract identity, into</li>
  <li>opaque runtime-private identity.</li>
</ul>

<p>
  This does not require that a backend contract serializes all runtime internals.
</p>

<p>
  It does require that:
</p>

<ul>
  <li>backend-contract identities remain attributable as part of the public handoff layer,</li>
  <li>runtime-private handles remain clearly downstream and implementation-owned,</li>
  <li>the handoff/private boundary is not erased for convenience,</li>
  <li>independent implementations remain free to realize the same backend contract without pretending that one runtime identity model is the architectural truth.</li>
</ul>

<hr/>

<h2 id="what-must-not-happen">9. What Must Not Happen</h2>

<p>
  A conforming implementation must not do any of the following:
</p>

<ul>
  <li>treat private runtime handles as if they were the canonical backend-contract identity surface,</li>
  <li>erase backend-contract identity once runtime allocation begins,</li>
  <li>make conformance or inspection depend on access to private runtime object identity,</li>
  <li>claim that runtime ownership is sufficient explanation of backend-contract identity,</li>
  <li>collapse the backend-contract / private runtime boundary into one hidden implementation layer.</li>
</ul>

<p>
  The forbidden collapse is:
</p>

<pre><code>backend contract identity
        -/-> private runtime identity
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
  If backend-contract identity were allowed to collapse into runtime-private identity, then:
</p>

<ul>
  <li>the backend contract would stop being a meaningful public handoff layer,</li>
  <li>independent implementations would become harder to compare,</li>
  <li>conformance would become implementation-dependent,</li>
  <li>runtime-private machinery would drift upward into public truth,</li>
  <li>backend portability and inspectability would weaken.</li>
</ul>

<p>
  The repository-level architectural rule must remain:
</p>

<pre><code>backend-contract handoff surface
            remains distinct from
private runtime realization
</code></pre>

<p>
  Identity is one of the places where this distinction can silently disappear.
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
  <li>a backend contract may carry public handoff identity,</li>
  <li>a private runtime may introduce additional internal identity,</li>
  <li>the two identity layers must remain distinct.</li>
</ul>

<p>
  The public truth asserted by this case is:
</p>

<pre><code>backend contract identity
            must remain distinct from
private runtime identity
</code></pre>
