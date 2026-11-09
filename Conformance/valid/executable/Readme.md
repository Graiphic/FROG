<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Executable Conformance — Valid Cases</h1>

<p align="center">
  <strong>Positive execution-ready cases for bounded published FROG execution corridors</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#why-this-family-exists">2. Why This Family Exists</a></li>
  <li><a href="#architectural-position">3. Architectural Position</a></li>
  <li><a href="#scope">4. Scope</a></li>
  <li><a href="#non-goals">5. Non-Goals</a></li>
  <li><a href="#reading-model">6. Reading Model</a></li>
  <li><a href="#family-entry-condition">7. Family Entry Condition</a></li>
  <li><a href="#expected-positive-truth">8. Expected Positive Truth</a></li>
  <li><a href="#required-case-material">9. Required Case Material</a></li>
  <li><a href="#minimal-case-shapes">10. Minimal Case Shapes</a></li>
  <li><a href="#relation-with-compiler-conformance">11. Relation with Compiler Conformance</a></li>
  <li><a href="#relation-with-profiles-and-execution-contracts">12. Relation with Profiles and Execution Contracts</a></li>
  <li><a href="#case-naming-and-ordering">13. Case Naming and Ordering</a></li>
  <li><a href="#first-family-anchor">14. First Family Anchor</a></li>
  <li><a href="#future-growth">15. Future Growth</a></li>
  <li><a href="#summary">16. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This directory defines positive executable conformance cases for bounded published FROG execution corridors.
</p>

<p>
A case in this family does not merely state that a program is:
</p>

<ul>
  <li>loadable,</li>
  <li>structurally valid,</li>
  <li>semantically accepted,</li>
  <li>IR-derivable,</li>
  <li>lowerable,</li>
  <li>backend-contract-emittable,</li>
  <li>backend-family consumable.</li>
</ul>

<p>
It goes one stage further and states that, under a bounded published execution corridor:
</p>

<pre>execution-contract assumptions are satisfied
   -&gt;
execution start succeeds
   -&gt;
the execution unit behaves as expected
   -&gt;
observable outcomes match expectation</pre>

<p>
This family therefore exists to make positive execution-ready claims public, explicit, and testable.
</p>

<hr/>

<h2 id="why-this-family-exists">2. Why This Family Exists</h2>

<p>
The compiler corridor and the executable corridor are related, but they are not identical.
</p>

<p>
A case may already be:
</p>

<pre>language-valid
   +
profile-valid
   +
IR-valid
   +
lowerable
   +
backend-contract-valid
   +
backend-family consumable</pre>

<p>
without yet being publicly closed as:
</p>

<pre>execution-start-ready
   +
observably executable
   +
correct at the output boundary</pre>

<p>
This family exists to expose that additional truth surface.
</p>

<p>
It prevents a silent leap from:
</p>

<pre>compilable in corridor terms</pre>

<p>
to:
</p>

<pre>publicly execution-correct</pre>

<p>
without an explicit published basis for doing so.
</p>

<hr/>

<h2 id="architectural-position">3. Architectural Position</h2>

<p>
This family must be read under the repository-wide layering:
</p>

<pre>Expression/
   - canonical source

Language/
   - validated meaning

IR/
   - canonical execution-facing representation
   - lowering
   - backend contract

Profiles/
   - optional bounded compilation corridors
   - optional bounded execution contracts

Conformance/
   - public accept / reject / preserve truth surface</pre>

<p>
Accordingly:
</p>

<ul>
  <li>this family does not redefine source law,</li>
  <li>this family does not redefine semantic law,</li>
  <li>this family does not redefine canonical IR,</li>
  <li>this family does not redefine backend contract ownership,</li>
  <li>this family only exposes positive public truth for execution-ready cases where the relevant published corridor already exists.</li>
</ul>

<hr/>

<h2 id="scope">4. Scope</h2>

<p>
This family defines positive executable conformance cases for bounded corridors where the repository publishes enough upstream material to justify them.
</p>

<p>
A valid executable case in this family is intended to show that:
</p>

<ul>
  <li>the upstream compiler corridor is already satisfied,</li>
  <li>the relevant execution-contract assumptions are also satisfied where such a contract exists,</li>
  <li>execution start succeeds,</li>
  <li>termination, quiescence, or repeated activation behavior matches the published bounded corridor,</li>
  <li>observable outputs or externally visible behavior match the case expectation.</li>
</ul>

<p>
This family is therefore narrower than “everything that runs somewhere somehow”.
</p>

<hr/>

<h2 id="non-goals">5. Non-Goals</h2>

<p>
This family is not:
</p>

<ul>
  <li>a benchmark suite,</li>
  <li>a performance suite,</li>
  <li>a private runtime smoke-test folder,</li>
  <li>a place where implementation success alone becomes normative truth,</li>
  <li>a promise that every compiler-valid case belongs automatically to the executable family,</li>
  <li>a promise that UI-heavy, runtime-heavy, or interactive event-loop cases are already part of the first published executable corridor.</li>
</ul>

<hr/>

<h2 id="reading-model">6. Reading Model</h2>

<p>
The intended reading model for a positive executable case is:
</p>

<pre>source accepted
   -&gt;
meaning accepted
   -&gt;
IR preserved
   -&gt;
lowering accepted
   -&gt;
backend contract accepted
   -&gt;
execution-contract assumptions satisfied where published
   -&gt;
execution starts
   -&gt;
observable result matches expectation</pre>

<p>
This family therefore depends on compiler-corridor truth, but it adds an additional execution-side public truth surface.
</p>

<hr/>

<h2 id="family-entry-condition">7. Family Entry Condition</h2>

<p>
A case belongs in this family only if all of the following are true:
</p>

<ul>
  <li>the relevant source and semantic rules are already published,</li>
  <li>the relevant IR, lowering, and backend-contract rules are already published,</li>
  <li>the relevant profile corridor is already published where the case depends on one,</li>
  <li>the relevant execution contract is already published where execution-start closure depends on one,</li>
  <li>the case can state its observable executable truth without relying on hidden private assumptions.</li>
</ul>

<p>
If these conditions are not met, the case should remain draft-only or stay in a compiler-corridor family instead.
</p>

<hr/>

<h2 id="expected-positive-truth">8. Expected Positive Truth</h2>

<p>
A positive executable case in this family is expected to make the following public claim:
</p>

<pre>this bounded program shape
not only survives the compiler corridor,
but also begins execution correctly
and exhibits the expected observable behavior</pre>

<p>
The expected truth may therefore include:
</p>

<ul>
  <li>successful execution start,</li>
  <li>normal termination,</li>
  <li>quiescent stable wait state,</li>
  <li>correct repeated-step behavior,</li>
  <li>correct output visibility,</li>
  <li>correct externally visible state evolution where explicitly part of the corridor.</li>
</ul>

<p>
Each case MUST make clear which of those outcomes are actually in scope.
</p>

<hr/>

<h2 id="required-case-material">9. Required Case Material</h2>

<p>
A fully published executable case in this family SHOULD include enough companion material to make its public truth mechanically or procedurally checkable.
</p>

<p>
Depending on the case, that may include:
</p>

<ul>
  <li>the canonical source file,</li>
  <li>the expected canonical IR form where relevant,</li>
  <li>the expected lowered form where relevant,</li>
  <li>the expected backend contract where relevant,</li>
  <li>the execution mode,</li>
  <li>the execution-unit assumptions,</li>
  <li>the expected observable result,</li>
  <li>machine-checkable metadata or manifests where the published conformance format later standardizes them.</li>
</ul>

<p>
The more execution-side closure a case claims, the more explicit its companion material should be.
</p>

<hr/>

<h2 id="minimal-case-shapes">10. Minimal Case Shapes</h2>

<p>
The first cases in this family should remain conservative.
</p>

<p>
The most natural first positive executable shapes are:
</p>

<ul>
  <li>one-shot pure cores,</li>
  <li>one-shot structured-control cores without external service dependency,</li>
  <li>bounded explicit-state cases where state initialization and execution mode are explicit.</li>
</ul>

<p>
The least suitable first anchors are:
</p>

<ul>
  <li>UI-heavy interactive cases,</li>
  <li>event-loop-driven runtime cases,</li>
  <li>interop-heavy host-service cases,</li>
  <li>distributed or target-fragmented cases,</li>
  <li>cases whose correctness depends on large private runtime surfaces.</li>
</ul>

<hr/>

<h2 id="relation-with-compiler-conformance">11. Relation with Compiler Conformance</h2>

<p>
This family does not replace <code>valid/compiler/</code>.
</p>

<p>
Instead, it extends it.
</p>

<p>
The intended staged relation is:
</p>

<pre>valid/compiler/
   proves
the corridor remains consumable

valid/executable/
   proves
the corridor also reaches correct execution
under a published bounded execution-ready surface</pre>

<p>
Accordingly:
</p>

<pre>backend-family consumable
   is necessary
but not always sufficient
for valid/executable/ membership</pre>

<hr/>

<h2 id="relation-with-profiles-and-execution-contracts">12. Relation with Profiles and Execution Contracts</h2>

<p>
Where an executable case depends on a bounded optional profile corridor, it must be read relative to that profile.
</p>

<p>
Where execution-start closure further depends on a published profile-level execution contract, the case must also be read relative to that document.
</p>

<p>
The intended distinction is:
</p>

<pre>profile
   defines
bounded capability corridor

backend contract
   defines
generic standardized downstream handoff

execution contract
   defines
bounded execution-start closure

valid/executable/
   tests
positive public truth
after all of the above are satisfied</pre>

<hr/>

<h2 id="case-naming-and-ordering">13. Case Naming and Ordering</h2>

<p>
Cases in this family SHOULD be numbered conservatively and named by the public truth they expose.
</p>

<p>
Good names are names such as:
</p>

<ul>
  <li><code>01_one_shot_pure_core_starts_and_terminates</code>,</li>
  <li><code>02_structured_control_one_shot_executes_correctly</code>,</li>
  <li><code>03_explicit_state_step_mode_initializes_and_advances</code>.</li>
</ul>

<p>
Poor names are names tied only to implementation details, vendor internals, or private runtime machinery.
</p>

<hr/>

<h2 id="first-family-anchor">14. First Family Anchor</h2>

<p>
The first coherent anchor for this family is a one-shot pure core case.
</p>

<p>
That is the right first anchor because it minimizes ambiguity across:
</p>

<ul>
  <li>state retention,</li>
  <li>interactive continuation,</li>
  <li>host-service dependency,</li>
  <li>UI runtime dependency,</li>
  <li>event-loop ownership.</li>
</ul>

<p>
It therefore gives the repository the smallest serious execution-ready truth surface before widening to richer cases.
</p>

<hr/>

<h2 id="future-growth">15. Future Growth</h2>

<p>
Future growth in this family should remain corridor-first and conservative.
</p>

<p>
A good growth order is:
</p>

<ol>
  <li>one-shot pure core,</li>
  <li>one-shot structured control,</li>
  <li>bounded explicit-state execution,</li>
  <li>step-mode execution where explicitly closed,</li>
  <li>more complex runtime or target-facing cases only after the necessary published corridor exists.</li>
</ol>

<p>
This family should therefore grow by closing one public executable corridor at a time rather than opening many loosely specified execution claims.
</p>

<hr/>

<h2 id="summary">16. Summary</h2>

<p>
This directory defines positive executable conformance cases for bounded published FROG execution corridors.
</p>

<p>
A case in this family proves more than compilability.
It proves that, under a published bounded corridor:
</p>

<pre>execution may begin
and the observable result is correct</pre>

<p>
That is the intended role of <code>Conformance/valid/executable/</code> in the future growth of the FROG public truth surface.
</p>
