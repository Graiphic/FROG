<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Executable Conformance Case 01 — Backend Consumable but Execution Contract Invalid</h1>

<p align="center">
  <strong>Minimal negative execution-ready case for a bounded Native CPU LLVM corridor</strong><br/>
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
  <li><a href="#expected-behavior">9. Expected Behavior</a></li>
  <li><a href="#expected-conformance-outcome">10. Expected Conformance Outcome</a></li>
  <li><a href="#what-this-case-does-not-test">11. What This Case Does Not Test</a></li>
  <li><a href="#future-companion-material">12. Future Companion Material</a></li>
  <li><a href="#summary">13. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This case defines the smallest negative executable slice for a bounded Native CPU LLVM execution corridor.
</p>

<p>
It is intentionally minimal:
</p>

<ul>
  <li>the case is already far enough advanced to be backend-family consumable,</li>
  <li>but it still fails before valid executable truth may be claimed,</li>
  <li>the failure is execution-contract-side,</li>
  <li>the rejection occurs before valid execution start is recognized.</li>
</ul>

<p>
Its role is to make one public statement testable:
</p>

<pre>At least one bounded accepted FROG program
may survive the compiler corridor
and still be invalid as an executable case
because execution-side closure is missing.</pre>

<hr/>

<h2 id="purpose">2. Purpose</h2>

<p>
The purpose of this case is to establish the first negative executable conformance anchor beyond compiler-corridor consumability.
</p>

<p>
It closes the smallest observable negative step after:
</p>

<pre>backend-contract-valid
   -&gt;
execution-contract requirements are checked
   -&gt;
execution-contract requirements fail
   -&gt;
valid executable truth is rejected</pre>

<p>
This case therefore exists to prove that executable conformance growth begins with a sharp distinction between:
</p>

<pre>backend-family consumable
and
execution-ready</pre>

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
execution-contract check
   -&gt;
execution-contract rejection</pre>

<p>
It is therefore the smallest negative executable corridor slice.
</p>

<hr/>

<h2 id="case-intent">5. Case Intent</h2>

<p>
This case asserts that a bounded accepted program:
</p>

<ul>
  <li>belongs far enough to the compiler corridor to remain backend-family consumable,</li>
  <li>does not yet satisfy the execution-side requirements needed for valid executable truth,</li>
  <li>must therefore be rejected as an executable case,</li>
  <li>must not be silently upgraded into execution-start success.</li>
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
  <li>the lowered form remains valid for the relevant compiler corridor,</li>
  <li>a backend contract has been emitted successfully,</li>
  <li>declared backend-family consumability is still true,</li>
  <li>the case fails only at the execution-contract stage, not earlier.</li>
</ul>

<p>
This case is therefore not a compiler rejection case disguised as an executable rejection case.
</p>

<hr/>

<h2 id="program-shape">7. Program Shape</h2>

<p>
The intended program shape is conservative and should avoid dragging in unnecessary ambiguity.
</p>

<p>
Natural candidates include:
</p>

<ul>
  <li>a simple pure or mostly pure core whose compiler corridor is otherwise valid,</li>
  <li>a case whose entry assumptions are intentionally incomplete,</li>
  <li>a case whose execution mode is intentionally incompatible with the bounded execution corridor,</li>
  <li>a case whose required execution-unit assumptions are intentionally missing or contradictory.</li>
</ul>

<p>
The important property is not program richness.
The important property is:
</p>

<pre>compiler corridor succeeds
while
execution-side closure still fails</pre>

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
  <li>but execution-contract rejection.</li>
</ul>

<p>
Typical bounded reasons may include:
</p>

<ul>
  <li>missing explicit execution mode,</li>
  <li>missing explicit execution unit,</li>
  <li>missing explicit state-instantiation posture where required,</li>
  <li>missing required declared host-service assumptions,</li>
  <li>another bounded execution-start prerequisite not satisfied.</li>
</ul>

<p>
Compactly:
</p>

<pre>backend consumable
   but
not execution-contract valid</pre>

<hr/>

<h2 id="expected-behavior">9. Expected Behavior</h2>

<p>
A conforming implementation is expected to produce the following staged behavior:
</p>

<ol>
  <li>accept the source and semantic shape,</li>
  <li>derive canonical Execution IR correctly,</li>
  <li>lower the case correctly,</li>
  <li>emit a valid backend contract,</li>
  <li>allow the case to remain backend-family consumable,</li>
  <li>reject the case at the execution-contract stage,</li>
  <li>not report valid execution-start success.</li>
</ol>

<p>
The expected public truth is therefore:
</p>

<pre>accepted through compiler corridor
   +
backend consumable
   +
not execution-contract valid
   +
not valid as executable truth</pre>

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
  <li><strong>not rejected</strong> at backend-family consumability level,</li>
  <li><strong>rejected</strong> at execution-contract level,</li>
  <li><strong>not claimable</strong> as valid executable success.</li>
</ul>

<hr/>

<h2 id="what-this-case-does-not-test">11. What This Case Does Not Test</h2>

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
  <li>private runtime crash signatures.</li>
</ul>

<p>
That restriction is deliberate.
This case is meant to anchor the negative executable family with the smallest coherent mirror of the first positive executable anchor.
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
  <li>the expected execution-contract rejection reason,</li>
  <li>any machine-checkable metadata required by the published conformance format.</li>
</ul>

<hr/>

<h2 id="summary">13. Summary</h2>

<p>
This case defines the smallest negative executable conformance slice for a bounded Native CPU LLVM corridor.
</p>

<p>
It proves one thing cleanly:
</p>

<pre>a FROG case
may already be backend consumable
and still fail as executable public truth
because execution-side closure is missing</pre>

<p>
That is the intended role of
<code>01_backend_consumable_but_execution_contract_invalid</code>
in a future negative executable conformance family.
</p>
