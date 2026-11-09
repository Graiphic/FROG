<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Executable Conformance Case 02 — Execution Contract Valid but Required Host Service Missing</h1>

<p align="center">
  <strong>Negative execution-ready host-service case for a bounded Native CPU LLVM corridor</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#purpose">2. Purpose</a></li>
  <li><a href="#status">3. Status</a></li>
  <li><a href="#corridor-covered">4. Corridor Covered</a></li>
  <li><a href="#case-intent">5. Case Intent</a></li>
  <li><a href="#required-upstream-assumptions">6. Required Upstream Assumptions</a></li>
  <li><a href="#program-shape">7. Program Shape</a></li>
  <li><a href="#failure-shape">8. Failure Shape</a></li>
  <li><a href="#host-service-focus">9. Host-Service Focus</a></li>
  <li><a href="#expected-behavior">10. Expected Behavior</a></li>
  <li><a href="#expected-conformance-outcome">11. Expected Conformance Outcome</a></li>
  <li><a href="#what-this-case-does-not-test">12. What This Case Does Not Test</a></li>
  <li><a href="#future-companion-material">13. Future Companion Material</a></li>
  <li><a href="#summary">14. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This case defines the second negative executable slice for a bounded Native CPU LLVM execution corridor.
</p>

<p>
It extends the first negative anchor:
</p>

<pre>backend consumable
   but
execution-contract invalid</pre>

<p>
by moving one stage further:
</p>

<ul>
  <li>the case is already far enough advanced to be backend-family consumable,</li>
  <li>the case also satisfies the bounded execution-contract assumptions in principle,</li>
  <li>but execution still cannot be treated as valid executable truth because one required declared host service is missing.</li>
</ul>

<p>
Its role is to make one public statement testable:
</p>

<pre>A bounded accepted FROG program
may be execution-contract-valid in abstract corridor terms
and still fail as executable public truth
because one explicitly required host service is unavailable.</pre>

<hr/>

<h2 id="purpose">2. Purpose</h2>

<p>
The purpose of this case is to establish the first negative executable anchor that fails for an operational reason more specific than execution-contract invalidity itself.
</p>

<p>
It closes the next observable negative step after:
</p>

<pre>execution-contract assumptions fail before start</pre>

<p>
by showing:
</p>

<pre>execution-contract assumptions are satisfied
   +
declared host-service dependency exists
   +
required host service is missing
   +
valid executable truth is rejected</pre>

<p>
This case therefore exists to prove that executable conformance can distinguish:
</p>

<ul>
  <li>corridor invalidity,</li>
  <li>from valid corridor assumptions whose execution still cannot proceed under published conditions.</li>
</ul>

<hr/>

<h2 id="status">3. Status</h2>

<p>
This case is intended as part of a future executable conformance family.
</p>

<p>
It should only be treated as fully published conformance once the repository also publishes:
</p>

<ul>
  <li>a profile-side execution contract for the bounded Native CPU LLVM execution-ready corridor,</li>
  <li>the containing <code>Conformance/invalid/executable/</code> family,</li>
  <li>the companion case assets required by the published conformance format.</li>
</ul>

<p>
Until then, it should be read as a coherent draft-to-add aligned with the current compiler-corridor architecture.
</p>

<hr/>

<h2 id="corridor-covered">4. Corridor Covered</h2>

<p>
This case covers the following corridor:
</p>

<pre>.frog source
   -&gt;
structural acceptance
   -&gt;
semantic acceptance
   -&gt;
canonical Execution IR
   -&gt;
lowering
   -&gt;
backend contract
   -&gt;
declared backend-family consumability
   -&gt;
execution-contract satisfaction
   -&gt;
execution-start precondition check
   -&gt;
required host service missing
   -&gt;
executable rejection or startup failure under published bounded conditions</pre>

<p>
It is therefore the smallest negative executable corridor slice in which the execution contract itself is not the failing stage, but the declared start environment still blocks valid execution truth.
</p>

<hr/>

<h2 id="case-intent">5. Case Intent</h2>

<p>
This case asserts that a bounded accepted program:
</p>

<ul>
  <li>belongs far enough to the compiler corridor to remain backend-family consumable,</li>
  <li>satisfies the bounded execution-contract assumptions,</li>
  <li>still depends on one explicitly declared host service,</li>
  <li>must be rejected as a valid executable case when that service is unavailable,</li>
  <li>must not be silently upgraded into execution-start success,</li>
  <li>must not be silently “repaired” by inventing an undeclared substitute service.</li>
</ul>

<hr/>

<h2 id="required-upstream-assumptions">6. Required Upstream Assumptions</h2>

<p>
A conforming implementation may only claim this case if all of the following are true:
</p>

<ul>
  <li>the source is canonical where required,</li>
  <li>the program is semantically accepted,</li>
  <li>canonical Execution IR has been derived faithfully,</li>
  <li>the lowered form remains valid for the relevant compiler corridor,</li>
  <li>a backend contract has been emitted successfully,</li>
  <li>declared backend-family consumability is still true,</li>
  <li>the relevant bounded execution contract is satisfied,</li>
  <li>the case depends on one explicitly declared host service,</li>
  <li>the required host service is intentionally absent, unsupported, or unavailable under the case conditions.</li>
</ul>

<p>
This case is therefore not:
</p>

<ul>
  <li>a compiler rejection case,</li>
  <li>an execution-contract-invalid case,</li>
  <li>a private runtime crash with no published meaning.</li>
</ul>

<hr/>

<h2 id="program-shape">7. Program Shape</h2>

<p>
The intended program shape remains conservative and should avoid dragging in unnecessary ambiguity.
</p>

<p>
Natural candidates include:
</p>

<ul>
  <li>a bounded executable core whose start requires one declared runtime bridge,</li>
  <li>a case whose executable corridor explicitly depends on one named host service,</li>
  <li>a case whose source and backend corridor remain otherwise valid and stable.</li>
</ul>

<p>
The important property is:
</p>

<pre>compiler corridor succeeds
execution-contract assumptions succeed
host environment still fails one declared requirement</pre>

<hr/>

<h2 id="failure-shape">8. Failure Shape</h2>

<p>
The failure shape of this case is:
</p>

<ul>
  <li>not language rejection,</li>
  <li>not IR rejection,</li>
  <li>not lowering rejection,</li>
  <li>not backend-contract rejection,</li>
  <li>not backend-family consumer rejection,</li>
  <li>not execution-contract invalidity,</li>
  <li>but missing required declared host service at execution-start time.</li>
</ul>

<p>
Typical bounded reasons may include:
</p>

<ul>
  <li>a declared clock or time service is unavailable,</li>
  <li>a declared runtime bridge required by lowering is unavailable,</li>
  <li>a declared callable host entry required for start cannot be provided,</li>
  <li>another explicitly declared host-side prerequisite is missing.</li>
</ul>

<p>
Compactly:
</p>

<pre>execution-contract valid
   but
required host service missing</pre>

<hr/>

<h2 id="host-service-focus">9. Host-Service Focus</h2>

<p>
The public truth exposed by this case is not just that “startup fails”.
</p>

<p>
It is that the failure occurs for one precise published reason:
</p>

<pre>the corridor explicitly required a host service
and that service was not available</pre>

<p>
That distinction matters because a conforming implementation MUST NOT:
</p>

<ul>
  <li>silently pretend the dependency did not exist,</li>
  <li>silently substitute a private undeclared fallback,</li>
  <li>claim valid executable success despite the missing requirement.</li>
</ul>

<p>
This case therefore protects the boundary between:
</p>

<pre>declared execution dependency
and
implementation convenience workaround</pre>

<hr/>

<h2 id="expected-behavior">10. Expected Behavior</h2>

<p>
A conforming implementation is expected to produce the following staged behavior:
</p>

<ol>
  <li>accept the source and semantic shape,</li>
  <li>derive canonical Execution IR correctly,</li>
  <li>lower the case correctly,</li>
  <li>emit a valid backend contract,</li>
  <li>allow the case to remain backend-family consumable,</li>
  <li>accept the case at the execution-contract stage,</li>
  <li>detect that the required declared host service is missing,</li>
  <li>reject valid executable truth or report bounded startup failure accordingly,</li>
  <li>not report successful valid execution start.</li>
</ol>

<p>
The expected public truth is therefore:
</p>

<pre>accepted through compiler corridor
   +
execution-contract valid
   +
required host service missing
   +
not valid as executable truth</pre>

<hr/>

<h2 id="expected-conformance-outcome">11. Expected Conformance Outcome</h2>

<p>
The expected conformance outcome for this case is:
</p>

<ul>
  <li><strong>not rejected</strong> at source level,</li>
  <li><strong>not rejected</strong> at semantic level,</li>
  <li><strong>not rejected</strong> at IR derivation level,</li>
  <li><strong>not rejected</strong> at lowering level,</li>
  <li><strong>not rejected</strong> at backend-contract level,</li>
  <li><strong>not rejected</strong> at backend-family consumability level,</li>
  <li><strong>not rejected</strong> at execution-contract level,</li>
  <li><strong>rejected</strong> or <strong>startup-failed</strong> at the declared host-service availability stage,</li>
  <li><strong>not claimable</strong> as valid executable success.</li>
</ul>

<hr/>

<h2 id="what-this-case-does-not-test">12. What This Case Does Not Test</h2>

<p>
This case does not test:
</p>

<ul>
  <li>UI runtime rejection,</li>
  <li>widget-object runtime semantics,</li>
  <li>event-loop ownership,</li>
  <li>real-time guarantees,</li>
  <li>distributed execution,</li>
  <li>vendor-specific packaging behavior,</li>
  <li>private runtime crash signatures with no published architectural meaning.</li>
</ul>

<p>
That restriction is deliberate.
This case is meant to anchor the next negative executable slice with a bounded operational failure that still remains specification-visible.
</p>

<hr/>

<h2 id="future-companion-material">13. Future Companion Material</h2>

<p>
A fully published version of this case should later be accompanied by repository-visible assets such as:
</p>

<ul>
  <li>the canonical source file,</li>
  <li>the expected IR representation where applicable,</li>
  <li>the expected lowered form where applicable,</li>
  <li>the expected backend contract where applicable,</li>
  <li>the execution-contract assumptions for the case,</li>
  <li>the declared required host service,</li>
  <li>the expected rejection or startup-failure reason,</li>
  <li>any machine-checkable metadata required by the published conformance format.</li>
</ul>

<hr/>

<h2 id="summary">14. Summary</h2>

<p>
This case defines the second negative executable conformance slice for a bounded Native CPU LLVM corridor.
</p>

<p>
It proves one thing cleanly:
</p>

<pre>a FROG case
may already be backend consumable
and execution-contract valid
yet still fail as executable public truth
because one required declared host service is missing</pre>

<p>
That is the intended role of
<code>02_execution_contract_valid_but_required_host_service_missing</code>
in a future negative executable conformance family.
</p>
