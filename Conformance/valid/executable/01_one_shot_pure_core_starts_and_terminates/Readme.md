<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Executable Conformance Case 01 — One-Shot Pure Core Starts and Terminates</h1>

<p align="center">
  <strong>Minimal execution-ready positive case for a bounded Native CPU LLVM corridor</strong><br/>
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
  <li><a href="#execution-mode">8. Execution Mode</a></li>
  <li><a href="#expected-behavior">9. Expected Behavior</a></li>
  <li><a href="#expected-conformance-outcome">10. Expected Conformance Outcome</a></li>
  <li><a href="#what-this-case-does-not-test">11. What This Case Does Not Test</a></li>
  <li><a href="#future-companion-material">12. Future Companion Material</a></li>
  <li><a href="#summary">13. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This case defines the smallest execution-ready positive slice for a bounded Native CPU LLVM executable corridor.
</p>

<p>
It is intentionally minimal:
</p>

<ul>
  <li>pure core only,</li>
  <li>no retained instance,</li>
  <li>no UI,</li>
  <li>no host-mediated event loop,</li>
  <li>no external interop dependency,</li>
  <li>one-shot execution only.</li>
</ul>

<p>
Its role is to make one public statement testable:
</p>

<pre>At least one bounded accepted FROG program
may be not only backend-family consumable,
but also execution-start ready
and normally terminating
under an explicit one-shot corridor.</pre>

<hr/>

<h2 id="purpose">2. Purpose</h2>

<p>
The purpose of this case is to establish the first executable conformance anchor beyond compiler-corridor consumability.
</p>

<p>
It closes the smallest observable step after:
</p>

<pre>backend-contract-valid
   -&gt;
execution-contract-satisfied
   -&gt;
execution-start succeeds
   -&gt;
normal termination occurs
   -&gt;
observable outputs match expectation</pre>

<p>
This case therefore exists to prove that executable conformance growth begins from a bounded, disciplined, low-ambiguity slice.
</p>

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
  <li>the containing <code>Conformance/valid/executable/</code> family,</li>
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
execution-contract satisfaction
   -&gt;
one-shot execution start
   -&gt;
normal termination
   -&gt;
observable output match</pre>

<p>
It is therefore the smallest positive executable corridor slice.
</p>

<hr/>

<h2 id="case-intent">5. Case Intent</h2>

<p>
This case asserts that a bounded accepted pure-core program:
</p>

<ul>
  <li>belongs to the accepted Native CPU LLVM compilation subset,</li>
  <li>satisfies the companion execution contract,</li>
  <li>can be instantiated without hidden runtime services,</li>
  <li>can be started in one-shot mode,</li>
  <li>can terminate normally,</li>
  <li>can expose the expected outputs at completion.</li>
</ul>

<p>
This case is intentionally not broader than that.
</p>

<hr/>

<h2 id="required-upstream-assumptions">6. Required Upstream Assumptions</h2>

<p>
A conforming implementation may only claim this case if all of the following are true:
</p>

<ul>
  <li>the source is canonical where required,</li>
  <li>the program is semantically accepted,</li>
  <li>canonical Execution IR has been derived faithfully,</li>
  <li>the lowered form remains valid for the Native CPU LLVM profile,</li>
  <li>a backend contract has been emitted successfully,</li>
  <li>the execution unit is explicit,</li>
  <li>the execution mode is explicit and one-shot,</li>
  <li>no undeclared host-service dependency exists,</li>
  <li>no UI runtime dependency exists,</li>
  <li>no retained-instance semantics are required.</li>
</ul>

<hr/>

<h2 id="program-shape">7. Program Shape</h2>

<p>
The intended program shape is:
</p>

<ul>
  <li>pure arithmetic or equivalent pure dataflow transformation,</li>
  <li>no UI nodes,</li>
  <li>no widget references,</li>
  <li>no external interop call,</li>
  <li>no asynchronous event delivery,</li>
  <li>no retained local state across activations,</li>
  <li>single explicit input/output boundary.</li>
</ul>

<p>
Compactly:
</p>

<pre>explicit pure core
   -&gt;
deterministic computation
   -&gt;
observable output
   -&gt;
normal termination</pre>

<hr/>

<h2 id="execution-mode">8. Execution Mode</h2>

<p>
This case is strictly one-shot.
</p>

<p>
That means:
</p>

<ul>
  <li>the execution unit is instantiated,</li>
  <li>any required initialization is performed,</li>
  <li>execution starts once,</li>
  <li>execution runs to completion,</li>
  <li>outputs become externally visible at completion,</li>
  <li>the execution unit terminates normally.</li>
</ul>

<p>
This case does not admit:
</p>

<ul>
  <li>step mode,</li>
  <li>retained-instance mode,</li>
  <li>quiescent wait semantics,</li>
  <li>interactive continuation.</li>
</ul>

<hr/>

<h2 id="expected-behavior">9. Expected Behavior</h2>

<p>
A conforming implementation is expected to produce the following behavior:
</p>

<ol>
  <li>accept the source and semantic shape,</li>
  <li>derive canonical Execution IR correctly,</li>
  <li>lower the case correctly,</li>
  <li>emit a valid backend contract,</li>
  <li>satisfy the execution-contract preconditions,</li>
  <li>start execution successfully,</li>
  <li>terminate normally,</li>
  <li>produce the expected observable outputs.</li>
</ol>

<p>
The expected public truth is therefore:
</p>

<pre>accepted
   +
execution-start succeeds
   +
normal termination
   +
observable outputs match expectation</pre>

<hr/>

<h2 id="expected-conformance-outcome">10. Expected Conformance Outcome</h2>

<p>
The expected conformance outcome for this case is:
</p>

<ul>
  <li><strong>not rejected</strong> at source level,</li>
  <li><strong>not rejected</strong> at semantic level,</li>
  <li><strong>not rejected</strong> at IR derivation level,</li>
  <li><strong>not rejected</strong> at lowering level,</li>
  <li><strong>not rejected</strong> at backend-contract level,</li>
  <li><strong>not rejected</strong> at execution-contract level,</li>
  <li><strong>not faulting</strong> during startup,</li>
  <li><strong>normally terminating</strong> under one-shot execution,</li>
  <li><strong>output-correct</strong> at the observable boundary.</li>
</ul>

<hr/>

<h2 id="what-this-case-does-not-test">11. What This Case Does Not Test</h2>

<p>
This case does not test:
</p>

<ul>
  <li>UI execution readiness,</li>
  <li>widget-object semantics,</li>
  <li>event-loop ownership,</li>
  <li>external interop execution,</li>
  <li>retained-instance semantics,</li>
  <li>step mode,</li>
  <li>real-time guarantees,</li>
  <li>multi-thread scheduling guarantees,</li>
  <li>runtime-mediated host services beyond the minimal declared execution surface.</li>
</ul>

<p>
That restriction is deliberate. This case is meant to anchor the executable family with the smallest coherent positive slice.
</p>

<hr/>

<h2 id="future-companion-material">12. Future Companion Material</h2>

<p>
A fully published version of this case should later be accompanied by repository-visible assets such as:
</p>

<ul>
  <li>the canonical source file,</li>
  <li>the expected IR representation where applicable,</li>
  <li>the expected lowered form where applicable,</li>
  <li>the expected backend contract where applicable,</li>
  <li>the expected observable execution result,</li>
  <li>any machine-checkable metadata required by the published conformance format.</li>
</ul>

<hr/>

<h2 id="summary">13. Summary</h2>

<p>
This case defines the smallest execution-ready positive conformance slice for a bounded Native CPU LLVM corridor.
</p>

<p>
It proves only one thing, but it proves it cleanly:
</p>

<pre>a pure accepted FROG core
may pass
from accepted source
to accepted execution start
to normal termination
with correct observable output</pre>

<p>
That is the intended role of
<code>01_one_shot_pure_core_starts_and_terminates</code>
in a future executable conformance family.
</p>
