<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 39</h1>

<p align="center">
  <strong>Open execution IR identity must remain distinct from private runtime identity</strong><br/>
  FROG — Free Open Graphical Language
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#1-case-name">1. Case Name</a></li>
  <li><a href="#2-expected">2. Expected</a></li>
  <li><a href="#3-why">3. Why</a></li>
  <li><a href="#4-boundary-being-exercised">4. Boundary Being Exercised</a></li>
  <li><a href="#5-scenario">5. Scenario</a></li>
  <li><a href="#6-what-makes-the-case-valid">6. What Makes the Case Valid</a></li>
  <li><a href="#7-expected-meaning">7. Expected Meaning</a></li>
  <li><a href="#8-expected-preservation">8. Expected Preservation</a></li>
  <li><a href="#9-what-must-not-happen">9. What Must Not Happen</a></li>
  <li><a href="#10-rationale">10. Rationale</a></li>
  <li><a href="#11-minimum-conformance-reading">11. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="1-case-name">1. Case Name</h2>

<p>
  Case:
  <code>39_open_execution_ir_identity_must_remain_distinct_from_private_runtime_identity</code>
</p>

<hr/>

<h2 id="2-expected">2. Expected</h2>

<p><strong>Expected:</strong> valid</p>

<p><strong>Expected meaning:</strong> language-level meaning is established.</p>

<p>
  <strong>Expected preservation:</strong>
  an open execution-facing IR identity remains distinct from any private runtime identity,
  even when a runtime introduces additional handles, object identities, scheduler tokens,
  memory owners, or backend-private instance identifiers.
</p>

<hr/>

<h2 id="3-why">3. Why</h2>

<p>
  This case exists to make one architectural rule explicit:
</p>

<pre><code>open execution IR identity
            !=
private runtime identity
</code></pre>

<p>
  FROG may define execution-facing open representations that are public, inspectable,
  derivable from validated meaning, and suitable for independent implementations.
</p>

<p>
  A runtime may then introduce additional private identities for execution convenience:
</p>

<ul>
  <li>heap object identities,</li>
  <li>scheduler-owned tokens,</li>
  <li>worker-local handles,</li>
  <li>buffer owners,</li>
  <li>backend-private instance references,</li>
  <li>target-specific runtime descriptors.</li>
</ul>

<p>
  Those private identities may exist.
  They do not become the same thing as open execution IR identity.
</p>

<p>
  The public execution-facing layer must remain distinguishable from private runtime realization.
</p>

<hr/>

<h2 id="4-boundary-being-exercised">4. Boundary Being Exercised</h2>

<p>
  This case exercises the preservation boundary across:
</p>

<pre><code>validated program meaning
        |
        v
open execution-facing IR
        |
        v
private runtime realization
</code></pre>

<p>
  The distinction under test is between:
</p>

<ul>
  <li>identity that belongs to the open execution-facing representation, and</li>
  <li>identity introduced only by a private runtime realization.</li>
</ul>

<hr/>

<h2 id="5-scenario">5. Scenario</h2>

<p>
  A valid FROG program is validated and then represented in an open execution-facing form.
  That open form contains execution-relevant identities, references, or named entities that are
  part of the derived public representation rather than mere hidden runtime machinery.
</p>

<p>
  A downstream runtime then realizes the same execution through its own internal machinery and
  introduces additional private identities for implementation convenience.
</p>

<p>
  The case is valid if:
</p>

<ul>
  <li>the open execution-facing identities remain meaningful as part of the public derived representation,</li>
  <li>the runtime may still use private identities internally,</li>
  <li>the implementation does not collapse the open layer into runtime-private ownership.</li>
</ul>

<hr/>

<h2 id="6-what-makes-the-case-valid">6. What Makes the Case Valid</h2>

<p>
  This case is valid because it preserves the published stage boundary between open IR and private runtime.
</p>

<p>
  More precisely, it is valid because:
</p>

<ul>
  <li>validated meaning is upstream from runtime realization,</li>
  <li>an open execution-facing representation is allowed to carry its own public identity surface,</li>
  <li>runtime-private realization may introduce additional identity,</li>
  <li>the two are not silently treated as the same ownership layer.</li>
</ul>

<p>
  The case does not require that all runtimes expose the same internal identity model.
</p>

<p>
  It does require that implementation-private identity does not overwrite, erase, or become confused with
  the identity semantics of the open execution-facing representation.
</p>

<hr/>

<h2 id="7-expected-meaning">7. Expected Meaning</h2>

<p>
  If this case validates, the established meaning includes at least the following:
</p>

<ul>
  <li>open execution-facing IR may have its own public identity surface,</li>
  <li>private runtime identity may exist downstream for realization purposes,</li>
  <li>private runtime identity does not become the owner of open execution meaning,</li>
  <li>the distinction between open IR identity and runtime-private identity remains preserved.</li>
</ul>

<p>
  The semantic reading is therefore:
</p>

<pre><code>validated meaning
        ->
open execution-facing identity
        ->
private runtime realization
</code></pre>

<p>Not:</p>

<pre><code>runtime-private identity
        replaces
open execution-facing identity
</code></pre>

<hr/>

<h2 id="8-expected-preservation">8. Expected Preservation</h2>

<p>
  A conforming implementation must preserve the distinction between open execution IR identity and private runtime identity.
</p>

<p>
  At minimum, later layers must not silently collapse:
</p>

<ul>
  <li>public execution-facing identity, into</li>
  <li>opaque runtime-private identity.</li>
</ul>

<p>
  This does not require that an implementation serializes its runtime internals into the public IR.
</p>

<p>
  It does require that:
</p>

<ul>
  <li>open execution-facing identities remain attributable as part of the public derived layer,</li>
  <li>runtime-private handles remain clearly downstream and implementation-owned,</li>
  <li>the public/ private boundary is not erased for convenience,</li>
  <li>independent implementations remain free to realize the same open IR without pretending that one runtime identity model is the language truth.</li>
</ul>

<hr/>

<h2 id="9-what-must-not-happen">9. What Must Not Happen</h2>

<p>
  A conforming implementation must not do any of the following:
</p>

<ul>
  <li>treat private runtime handles as if they were the canonical public execution identity surface,</li>
  <li>erase open execution-facing identity once runtime allocation begins,</li>
  <li>make conformance or inspection depend on access to private runtime object identity,</li>
  <li>claim that runtime ownership is sufficient explanation of IR identity,</li>
  <li>collapse the public IR / private runtime boundary into one hidden implementation layer.</li>
</ul>

<p>
  The forbidden collapse is:
</p>

<pre><code>open execution IR identity
        -/-> private runtime identity
</code></pre>

<hr/>

<h2 id="10-rationale">10. Rationale</h2>

<p>
  This distinction matters because FROG explicitly aims to keep open execution representation separate
  from private realization.
</p>

<p>
  If open execution-facing identity were allowed to collapse into runtime-private identity, then:
</p>

<ul>
  <li>the IR would stop being a public inspectable layer,</li>
  <li>independent implementations would become harder to compare,</li>
  <li>conformance would become implementation-dependent,</li>
  <li>runtime-private machinery would drift upward into public truth,</li>
  <li>backend portability would weaken.</li>
</ul>

<p>
  The repository-level architectural rule must remain:
</p>

<pre><code>open execution-facing representation
            remains distinct from
private runtime realization
</code></pre>

<p>
  Identity is one of the places where this distinction can silently disappear.
  This case ensures that it does not.
</p>

<hr/>

<h2 id="11-minimum-conformance-reading">11. Minimum Conformance Reading</h2>

<p>
  A minimal conforming reading of this case is:
</p>

<ul>
  <li>the source is valid,</li>
  <li>language-level meaning is established,</li>
  <li>an open execution-facing representation may carry public identity,</li>
  <li>a private runtime may introduce additional internal identity,</li>
  <li>the two identity layers must remain distinct.</li>
</ul>

<p>
  The public truth asserted by this case is:
</p>

<pre><code>open execution IR identity
            must remain distinct from
private runtime identity
</code></pre>
