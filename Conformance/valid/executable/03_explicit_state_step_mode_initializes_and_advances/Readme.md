<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Executable Conformance Case 03 — Explicit State Step Mode Initializes and Advances</h1>

<p align="center">
  <strong>Positive explicit-state step-mode execution-ready case for a bounded Native CPU LLVM corridor</strong><br/>
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
  <li><a href="#explicit-state-focus">8. Explicit-State Focus</a></li>
  <li><a href="#execution-mode">9. Execution Mode</a></li>
  <li><a href="#initialization-posture">10. Initialization Posture</a></li>
  <li><a href="#expected-behavior">11. Expected Behavior</a></li>
  <li><a href="#expected-conformance-outcome">12. Expected Conformance Outcome</a></li>
  <li><a href="#what-this-case-does-not-test">13. What This Case Does Not Test</a></li>
  <li><a href="#future-companion-material">14. Future Companion Material</a></li>
  <li><a href="#summary">15. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This case defines the third positive executable slice for a bounded Native CPU LLVM execution corridor.
</p>

<p>
It extends the first one-shot pure-core anchor and the second structured-control one-shot anchor by adding <strong>explicit state</strong> together with <strong>step-mode execution</strong>.
</p>

<p>
It remains intentionally conservative:
</p>

<ul>
  <li>explicit state only,</li>
  <li>step mode only,</li>
  <li>no UI,</li>
  <li>no event-loop ownership,</li>
  <li>no external interop dependency,</li>
  <li>no runtime-mediated widget behavior,</li>
  <li>no hidden persistence.</li>
</ul>

<p>
Its role is to make one further public statement testable:
</p>

<pre>At least one bounded accepted FROG program
with explicit state
may be execution-start ready
under a step-mode corridor,
correctly initialize its state,
correctly advance its state across permitted activations,
and expose the expected observable results.</pre>

<hr/>

<h2 id="purpose">2. Purpose</h2>

<p>
The purpose of this case is to establish the first positive execution-ready anchor that proves more than one-shot behavior.
</p>

<p>
It closes the next observable step after:
</p>

<pre>one-shot pure computation executes correctly
one-shot structured control executes correctly</pre>

<p>
by showing:
</p>

<pre>explicit state
   +
step-mode execution
   +
correct initialization
   +
correct advancement across activations
   +
correct observable results</pre>

<p>
This case therefore exists to prove that executable conformance growth reaches retained execution structure in a disciplined, explicit-state form without yet opening broad interactive runtime territory.
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
Until then, it should be read as a coherent draft-to-add aligned with the currently published compiler-corridor architecture.
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
step-mode execution unit instantiation
   -&gt;
explicit-state initialization
   -&gt;
first permitted activation
   -&gt;
later permitted activation
   -&gt;
correct state advancement
   -&gt;
correct observable result</pre>

<p>
It is therefore the smallest positive executable corridor slice in which explicit state and repeated activation are part of the public truth.
</p>

<hr/>

<h2 id="case-intent">5. Case Intent</h2>

<p>
This case asserts that a bounded accepted explicit-state program:
</p>

<ul>
  <li>belongs to the accepted Native CPU LLVM compilation subset,</li>
  <li>satisfies the companion execution contract,</li>
  <li>can be instantiated without hidden runtime services,</li>
  <li>can begin in step mode,</li>
  <li>can initialize explicit state correctly,</li>
  <li>can preserve and advance that state correctly across permitted activations,</li>
  <li>can expose the expected observable outputs at each relevant observation boundary.</li>
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
  <li>explicit state remains explicit across derivation,</li>
  <li>canonical Execution IR has been derived faithfully,</li>
  <li>the lowered form remains valid for the Native CPU LLVM profile,</li>
  <li>a backend contract has been emitted successfully,</li>
  <li>the execution unit is explicit,</li>
  <li>the execution mode is explicit and step-based,</li>
  <li>the state-instantiation posture is explicit,</li>
  <li>no undeclared host-service dependency exists,</li>
  <li>no UI runtime dependency exists,</li>
  <li>state continuity across permitted activations is explicit rather than inferred accidentally.</li>
</ul>

<hr/>

<h2 id="program-shape">7. Program Shape</h2>

<p>
The intended program shape is:
</p>

<ul>
  <li>a bounded executable core with explicit state participation,</li>
  <li>a clear step-call or repeated activation boundary,</li>
  <li>one clear accepted input/output boundary,</li>
  <li>no UI nodes,</li>
  <li>no widget references,</li>
  <li>no external interop call,</li>
  <li>no asynchronous event-loop ownership.</li>
</ul>

<p>
Typical candidate forms include:
</p>

<ul>
  <li>a delay-based stateful accumulator,</li>
  <li>a bounded explicit-state loop advanced by repeated activations,</li>
  <li>a small stateful core whose successive outputs depend on prior explicit state rather than hidden persistence.</li>
</ul>

<hr/>

<h2 id="explicit-state-focus">8. Explicit-State Focus</h2>

<p>
The public truth exposed by this case is not merely that repeated execution happens.
</p>

<p>
It is that the repeated execution remains correct <strong>because state is explicit and correctly handled</strong>.
</p>

<p>
That means the conforming implementation must preserve, at the execution-visible level:
</p>

<ul>
  <li>the explicit initialization posture,</li>
  <li>the explicit first-state value or equivalent permitted initialization effect,</li>
  <li>the correct transition from one activation to the next,</li>
  <li>the correct observable result after each relevant permitted activation.</li>
</ul>

<p>
Compactly:
</p>

<pre>accepted explicit state
   -&gt;
correct execution-unit initialization
   -&gt;
correct state continuity across permitted steps
   -&gt;
correct observable results</pre>

<hr/>

<h2 id="execution-mode">9. Execution Mode</h2>

<p>
This case is strictly step-mode.
</p>

<p>
That means:
</p>

<ul>
  <li>the execution unit is instantiated once,</li>
  <li>required initialization is performed once according to the declared posture,</li>
  <li>execution advances through repeated explicit activations,</li>
  <li>state continuity across those activations is preserved only because the corridor explicitly allows it,</li>
  <li>observable results may be checked at one or more permitted step boundaries.</li>
</ul>

<p>
This case does not admit:
</p>

<ul>
  <li>one-shot-only interpretation,</li>
  <li>hidden fresh-instance reset between activations,</li>
  <li>interactive event-loop continuation,</li>
  <li>implicit state retention not declared by the corridor.</li>
</ul>

<hr/>

<h2 id="initialization-posture">10. Initialization Posture</h2>

<p>
Initialization is part of the public truth of this case.
</p>

<p>
A conforming implementation must make explicit and honor:
</p>

<ul>
  <li>whether the execution unit is freshly instantiated or retained,</li>
  <li>what initial explicit-state value is supplied or derived,</li>
  <li>what the first permitted activation must observe,</li>
  <li>how later activations differ from the first one because of explicit state advancement.</li>
</ul>

<p>
The critical distinction is:
</p>

<pre>first activation
   !=
later activation</pre>

<p>
when explicit state is part of the program meaning.
</p>

<hr/>

<h2 id="expected-behavior">11. Expected Behavior</h2>

<p>
A conforming implementation is expected to produce the following behavior:
</p>

<ol>
  <li>accept the source and semantic shape,</li>
  <li>derive canonical Execution IR correctly,</li>
  <li>preserve explicit-state commitments correctly through lowering,</li>
  <li>emit a valid backend contract,</li>
  <li>satisfy the execution-contract preconditions,</li>
  <li>instantiate the execution unit correctly,</li>
  <li>initialize explicit state correctly,</li>
  <li>execute the first permitted step correctly,</li>
  <li>execute at least one later permitted step correctly,</li>
  <li>expose the expected observable outputs.</li>
</ol>

<p>
The expected public truth is therefore:
</p>

<pre>accepted
   +
execution-start succeeds
   +
explicit state initializes correctly
   +
later steps advance correctly
   +
observable outputs match expectation</pre>

<hr/>

<h2 id="expected-conformance-outcome">12. Expected Conformance Outcome</h2>

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
  <li><strong>initialization-correct</strong> at first activation,</li>
  <li><strong>state-advance-correct</strong> at later activation,</li>
  <li><strong>output-correct</strong> at the observable boundary.</li>
</ul>

<hr/>

<h2 id="what-this-case-does-not-test">13. What This Case Does Not Test</h2>

<p>
This case does not test:
</p>

<ul>
  <li>UI execution readiness,</li>
  <li>widget-object semantics,</li>
  <li>event-loop ownership,</li>
  <li>external interop execution,</li>
  <li>real-time guarantees,</li>
  <li>multi-thread scheduling guarantees,</li>
  <li>runtime-mediated host services beyond the minimal declared execution surface,</li>
  <li>large long-lived application lifecycle management.</li>
</ul>

<p>
That restriction is deliberate. This case is meant to anchor explicit-state step-mode execution as the next bounded positive executable slice after the two one-shot anchors.
</p>

<hr/>

<h2 id="future-companion-material">14. Future Companion Material</h2>

<p>
A fully published version of this case should later be accompanied by repository-visible assets such as:
</p>

<ul>
  <li>the canonical source file,</li>
  <li>the expected IR representation where applicable,</li>
  <li>the expected lowered form where applicable,</li>
  <li>the expected backend contract where applicable,</li>
  <li>the expected first-step observable result,</li>
  <li>the expected later-step observable result,</li>
  <li>any machine-checkable metadata required by the published conformance format.</li>
</ul>

<p>
For this case in particular, the expected material should make the initialization boundary and later-step state advancement unambiguous.
</p>

<hr/>

<h2 id="summary">15. Summary</h2>

<p>
This case defines the third positive executable conformance slice for a bounded Native CPU LLVM corridor.
</p>

<p>
It proves one thing cleanly:
</p>

<pre>a stateful FROG core
may pass
from accepted source
to accepted execution start
to correct explicit-state initialization
to correct later step advancement
with correct observable outputs</pre>

<p>
That is the intended role of
<code>03_explicit_state_step_mode_initializes_and_advances</code>
in a future executable conformance family.
</p>
