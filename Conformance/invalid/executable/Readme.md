<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">Executable Conformance — Invalid Cases</h1>

<p align="center">
  <strong>Negative execution-ready cases for bounded published FROG execution corridors</strong><br/>
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
  <li><a href="#expected-negative-truth">8. Expected Negative Truth</a></li>
  <li><a href="#required-case-material">9. Required Case Material</a></li>
  <li><a href="#minimal-negative-case-shapes">10. Minimal Negative Case Shapes</a></li>
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
This directory defines negative executable conformance cases for bounded published FROG execution corridors.
</p>

<p>
A case in this family does not merely state that a program is rejected somewhere upstream in the compiler corridor.
It states that a case that may already be:
</p>

<ul>
  <li>loadable,</li>
  <li>structurally valid,</li>
  <li>semantically accepted,</li>
  <li>IR-derivable,</li>
  <li>lowerable,</li>
  <li>backend-contract-emittable,</li>
  <li>backend-family consumable,</li>
</ul>

<p>
still fails to qualify as valid executable public truth under the bounded published execution corridor.
</p>

<p>
This family therefore exists to make execution-side rejection public, explicit, and testable.
</p>

<hr/>

<h2 id="why-this-family-exists">2. Why This Family Exists</h2>

<p>
The repository-wide architecture already distinguishes:
</p>

<pre>backend-family consumability
   !=
execution-start readiness</pre>

<p>
That means a case may have passed the compiler-oriented corridor and still fail later because:
</p>

<ul>
  <li>execution-contract assumptions are not satisfied,</li>
  <li>required host-service assumptions are not explicit or not available,</li>
  <li>the execution unit is ambiguous,</li>
  <li>the selected execution mode is unsupported or inconsistent,</li>
  <li>startup fails under conditions that the bounded corridor does not allow to be treated as success.</li>
</ul>

<p>
This family exists so that those failures become public truth rather than implementation folklore.
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
  <li>this family only exposes negative public truth for execution-side failure or rejection once the upstream corridor has gone far enough for that distinction to matter.</li>
</ul>

<hr/>

<h2 id="scope">4. Scope</h2>

<p>
This family defines negative executable conformance cases for bounded corridors where the repository publishes enough upstream material to justify execution-side rejection as a distinct public outcome.
</p>

<p>
An invalid executable case in this family is intended to show that:
</p>

<ul>
  <li>the upstream compiler corridor may already have succeeded far enough,</li>
  <li>but the relevant execution-contract assumptions are not satisfied where such a contract exists,</li>
  <li>or execution start fails under explicitly in-scope published conditions,</li>
  <li>or observable executable truth cannot be claimed without violating the bounded corridor.</li>
</ul>

<p>
This family is therefore narrower than general invalidity.
</p>

<hr/>

<h2 id="non-goals">5. Non-Goals</h2>

<p>
This family is not:
</p>

<ul>
  <li>a catch-all folder for all rejected programs,</li>
  <li>a replacement for <code>invalid/compiler/</code>,</li>
  <li>a private runtime failure log,</li>
  <li>a place where unpublished implementation instability becomes normative truth,</li>
  <li>a proof that the language is invalid merely because one bounded execution corridor rejects a case.</li>
</ul>

<p>
A case belongs here only when executable rejection is the correct architectural stage of rejection.
</p>

<hr/>

<h2 id="reading-model">6. Reading Model</h2>

<p>
The intended reading model for a negative executable case is:
</p>

<pre>source may be accepted
   -&gt;
meaning may be accepted
   -&gt;
IR may be preserved
   -&gt;
lowering may be accepted
   -&gt;
backend contract may be accepted
   -&gt;
execution-contract assumptions fail
      or
   execution-start fails under published bounded conditions
   -&gt;
public executable truth is rejection or failure, not success</pre>

<p>
This family therefore depends on the compiler corridor, but it adds a distinct execution-side rejection surface.
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
  <li>the relevant execution contract is already published where execution-side rejection depends on one,</li>
  <li>the rejection or failure can be stated as public truth without relying on hidden private assumptions.</li>
</ul>

<p>
If those conditions are not met, the case should remain draft-only or stay in a compiler-corridor family instead.
</p>

<hr/>

<h2 id="expected-negative-truth">8. Expected Negative Truth</h2>

<p>
A negative executable case in this family is expected to make one of the following public claims:
</p>

<ul>
  <li>the case is backend-consumable but not execution-contract-valid,</li>
  <li>the case is execution-contract-invalid before start,</li>
  <li>the case is permitted to try starting but startup fault occurs under a published in-scope condition,</li>
  <li>the case cannot claim valid executable truth because its execution-side obligations are not satisfied.</li>
</ul>

<p>
Each case MUST make clear whether it is testing:
</p>

<ul>
  <li>pre-start rejection,</li>
  <li>startup fault,</li>
  <li>unsupported execution mode,</li>
  <li>unsupported host-service dependency,</li>
  <li>another explicitly published execution-side negative condition.</li>
</ul>

<hr/>

<h2 id="required-case-material">9. Required Case Material</h2>

<p>
A fully published negative executable case in this family SHOULD include enough companion material to make its rejection stage clear and checkable.
</p>

<p>
Depending on the case, that may include:
</p>

<ul>
  <li>the canonical source file,</li>
  <li>the expected canonical IR form where relevant,</li>
  <li>the expected lowered form where relevant,</li>
  <li>the expected backend contract where relevant,</li>
  <li>the execution mode being claimed or denied,</li>
  <li>the execution-unit assumptions,</li>
  <li>the expected rejection stage or expected startup-fault condition,</li>
  <li>machine-checkable metadata or manifests where the published conformance format later standardizes them.</li>
</ul>

<hr/>

<h2 id="minimal-negative-case-shapes">10. Minimal Negative Case Shapes</h2>

<p>
The first cases in this family should remain conservative and sharply staged.
</p>

<p>
The most natural first negative executable shapes are:
</p>

<ul>
  <li>backend-contract-valid but execution-contract-invalid,</li>
  <li>execution-contract-valid in appearance but missing a required declared host service,</li>
  <li>execution mode mismatch against the bounded published corridor,</li>
  <li>startup fault under an explicitly published in-scope condition.</li>
</ul>

<p>
The least suitable first anchors are:
</p>

<ul>
  <li>large UI-runtime failure cases,</li>
  <li>vendor-specific packaging failures,</li>
  <li>private runtime bugs with no published normative meaning,</li>
  <li>target-fragmented failures that depend on unpublished backend internals.</li>
</ul>

<hr/>

<h2 id="relation-with-compiler-conformance">11. Relation with Compiler Conformance</h2>

<p>
This family does not replace <code>invalid/compiler/</code>.
</p>

<p>
Instead, it extends the staged corridor beyond compiler-side rejection.
</p>

<p>
The intended relation is:
</p>

<pre>invalid/compiler/
   proves
rejection before executable truth is even in scope

invalid/executable/
   proves
the case passed far enough that executable truth is now the correct rejection surface</pre>

<p>
Accordingly:
</p>

<pre>backend-family consumable
   does not imply
valid/executable/ membership

and may still imply
invalid/executable/ membership</pre>

<hr/>

<h2 id="relation-with-profiles-and-execution-contracts">12. Relation with Profiles and Execution Contracts</h2>

<p>
Where a negative executable case depends on a bounded optional profile corridor, it must be read relative to that profile.
</p>

<p>
Where execution-side rejection depends on a published profile-level execution contract, the case must also be read relative to that contract.
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

invalid/executable/
   tests
negative public truth
after the corridor has advanced far enough
for execution-side rejection to matter</pre>

<hr/>

<h2 id="case-naming-and-ordering">13. Case Naming and Ordering</h2>

<p>
Cases in this family SHOULD be numbered conservatively and named by the public negative truth they expose.
</p>

<p>
Good names are names such as:
</p>

<ul>
  <li><code>01_backend_consumable_but_execution_contract_invalid</code>,</li>
  <li><code>02_execution_contract_valid_but_required_host_service_missing</code>,</li>
  <li><code>03_one_shot_startup_faults_under_published_condition</code>.</li>
</ul>

<p>
Poor names are names tied only to vendor internals, incidental crash signatures, or private runtime implementation details.
</p>

<hr/>

<h2 id="first-family-anchor">14. First Family Anchor</h2>

<p>
The first coherent anchor for this family is a case that is:
</p>

<ul>
  <li>backend-contract-valid,</li>
  <li>clearly still outside valid execution-start truth,</li>
  <li>rejected for one bounded, published, execution-side reason.</li>
</ul>

<p>
That makes the smallest sharp first anchor:
</p>

<pre>backend consumable
   but
execution-contract invalid</pre>

<p>
This is the right first mirror of the positive executable family because it isolates the missing execution-side closure without introducing large runtime-private ambiguity.
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
  <li>backend-consumable but execution-contract-invalid,</li>
  <li>execution-mode mismatch,</li>
  <li>required declared host-service missing,</li>
  <li>published startup-fault conditions,</li>
  <li>richer runtime-side rejections only after the corresponding published execution corridor exists.</li>
</ol>

<p>
This family should therefore grow by closing one negative executable truth surface at a time rather than by collecting arbitrary runtime failures.
</p>

<hr/>

<h2 id="summary">16. Summary</h2>

<p>
This directory defines negative executable conformance cases for bounded published FROG execution corridors.
</p>

<p>
A case in this family proves more than compiler-side rejection.
It proves that, under a published bounded corridor:
</p>

<pre>execution truth is not valid
even though the case may already have advanced far into the compiler corridor</pre>

<p>
That is the intended role of <code>Conformance/invalid/executable/</code> in the future growth of the FROG public truth surface.
</p>
