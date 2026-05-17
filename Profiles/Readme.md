<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Profiles</h1>

<p align="center">
  <strong>Optional standardized capability families for FROG</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#purpose-of-profiles">2. Purpose of Profiles</a></li>
  <li><a href="#architectural-role">3. Architectural Role</a></li>
  <li><a href="#scope-of-this-directory">4. Scope of this Directory</a></li>
  <li><a href="#capability-taxonomy">5. Capability Taxonomy</a></li>
  <li><a href="#profile-vs-runtime-boundary">6. Profile vs Runtime Boundary</a></li>
  <li><a href="#what-profiles-may-define">7. What Profiles MAY Define</a></li>
  <li><a href="#what-profiles-must-not-define">8. What Profiles MUST NOT Define</a></li>
  <li><a href="#relation-with-other-specification-layers">9. Relation with Other Specification Layers</a></li>
  <li><a href="#profile-identification-and-claims">10. Profile Identification and Claims</a></li>
  <li><a href="#current-profile-specifications">11. Current Profile Specifications</a></li>
  <li><a href="#core-conformance-profile-support-and-certification">12. Core Conformance, Profile Support, and Certification</a></li>
  <li><a href="#profile-evolution">13. Profile Evolution</a></li>
  <li><a href="#status">14. Status</a></li>
  <li><a href="#summary">15. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This directory defines optional standardized capability families for FROG.
</p>

<p>
A profile is a standardized specification layer that extends the usable capability surface of FROG without redefining the canonical source structure of the language core, without replacing the cross-cutting execution semantics owned by the core language architecture, and without turning one implementation strategy into mandatory language law.
</p>

<p>
Profiles exist to standardize capability areas that matter for real ecosystems but that should not be treated as intrinsic, always-present, mandatory parts of the minimal FROG core.
</p>

<p>
Within the current repository architecture:
</p>

<pre><code>Expression/
   - canonical source representation

Language/
   - normative validated-program meaning
   - cross-cutting execution semantics

Libraries/
   - intrinsic standardized primitive vocabularies

IR/
   - canonical execution-facing representation
   - derivation
   - construction
   - schema posture
   - lowering
   - backend contract

Profiles/
   - optional standardized capability families

IDE/
   - authoring
   - observability
   - debugging
   - inspection
   - related tooling responsibilities

Implementations/
   - non-normative executable realization workspaces
</code></pre>

<p>
A profile therefore extends the standardized capability surface of FROG, but it does so from outside the intrinsic core.
</p>

<hr/>

<h2 id="purpose-of-profiles">2. Purpose of Profiles</h2>

<p>
The purpose of this directory is to provide a clean architectural home for specification material that is:
</p>

<ul>
  <li>standardized,</li>
  <li>useful across multiple implementations,</li>
  <li>optional rather than mandatory for the minimal language core,</li>
  <li>sometimes dependent on broader execution assumptions, deployment assumptions, host environments, interoperability contracts, capability classes, or target-facing restrictions.</li>
</ul>

<p>
This separation keeps the FROG core small, stable, portable, and durable while still allowing the standard to define broader capability surfaces where justified.
</p>

<p>
A profile is therefore neither:
</p>

<ul>
  <li>a replacement for the language core,</li>
  <li>a second hidden core-language layer,</li>
  <li>a dumping ground for implementation-specific features,</li>
  <li>a place where runtime-private realization becomes normative language truth,</li>
  <li>nor an excuse to move core language meaning into optional documents.</li>
</ul>

<hr/>

<h2 id="architectural-role">3. Architectural Role</h2>

<p>
Profiles occupy a distinct architectural role between the minimal core standard and purely implementation-specific extensions.
</p>

<p>
They are intended to standardize capability families that are broader than one vendor-specific extension but narrower than the mandatory language core.
</p>

<pre><code>FROG specification architecture
Expression/        - canonical source form
Language/          - normative validated-program meaning
Libraries/         - intrinsic standardized primitive vocabularies
IR/                - canonical derived execution representation and backend-facing boundaries
Profiles/          - optional standardized capability families
IDE/               - authoring and observability tooling
Implementations/   - non-normative executable realizations
</code></pre>

<p>
This model preserves a stable architectural center while allowing the ecosystem to grow through bounded optional capability families.
</p>

<p>
Profiles are therefore part of the published FROG specification architecture, but they are not part of the minimal intrinsic core.
</p>

<hr/>

<h2 id="scope-of-this-directory">4. Scope of this Directory</h2>

<p>
This directory is the normative home of optional profile specifications.
</p>

<p>
A profile specification MAY define:
</p>

<ul>
  <li>an optional primitive namespace or namespace family,</li>
  <li>additional primitive catalogs and port models,</li>
  <li>profile-local capability assumptions,</li>
  <li>profile-local validation rules,</li>
  <li>profile-local execution constraints that remain subordinate to the core language meaning,</li>
  <li>target-facing capability classes,</li>
  <li>deployment-facing capability classes,</li>
  <li>runtime-facing capability contracts,</li>
  <li>profile-specific terminology and behavior that remain outside the minimal intrinsic language surface,</li>
  <li>optional capability claims that implementations MAY support explicitly,</li>
  <li>profile-level execution contracts for bounded execution corridors, where justified,</li>
  <li>portable capability catalogs needed by multiple tools without moving their ownership into the minimal core.</li>
</ul>

<p>
A profile specification MUST remain subordinate to the core repository architecture.
</p>

<p>
This means that a profile may add an optional capability layer, but it MUST do so without collapsing the ownership boundaries of <code>Expression/</code>, <code>Language/</code>, <code>Libraries/</code>, <code>IR/</code>, or <code>IDE/</code>.
</p>

<hr/>

<h2 id="capability-taxonomy">5. Capability Taxonomy</h2>

<p>
To keep the architecture explicit, FROG distinguishes several different concepts that are related but must not be conflated.
</p>

<h3>5.1 Optional capability profiles</h3>

<p>
An optional capability profile is the top-level object standardized in this directory.
It defines one bounded optional capability family that implementations may claim explicitly.
</p>

<p>
A profile is not automatically:
</p>

<ul>
  <li>a target profile,</li>
  <li>a deployment mode,</li>
  <li>a backend family,</li>
  <li>a runtime module.</li>
</ul>

<p>
A profile MAY define one or more of those related capability classes or constraints when that is part of its bounded scope.
</p>

<h3>5.2 Target-facing capability classes</h3>

<p>
A profile may define target-facing capability classes when a bounded optional corridor depends on explicit execution assumptions.
</p>

<p>
A target-facing capability class is not a private runtime module and not a backend implementation by itself.
It is a standardized capability class that helps delimit where a profile applies.
</p>

<p>
Examples of target-facing posture that may be relevant inside published profiles include:
</p>

<ul>
  <li>host-oriented native CPU execution,</li>
  <li>UI-capable host environments,</li>
  <li>restricted interoperability-ready environments.</li>
</ul>

<p>
These target-facing classes are profile-side constraints.
They do not replace the core semantic model and they do not, by themselves, define runtime-private realization.
</p>

<h3>5.3 Deployment-facing capability classes</h3>

<p>
A profile may define deployment-facing capability classes when a bounded corridor depends on packaging, observability, or execution-environment assumptions.
</p>

<p>
A deployment-facing class may constrain how a prepared program is delivered or started.
It is not the same thing as:
</p>

<ul>
  <li>language semantics,</li>
  <li>core IR structure,</li>
  <li>backend family identity.</li>
</ul>

<p>
Examples of deployment-facing classes may include full-runtime-hosted execution, generated-launcher execution, self-contained executable packaging, statically linked native packaging, dynamically linked native packaging, or embedded-host packaging.
These classes describe delivery and start posture.
They do not, by themselves, decide what the source means or require every deployed program to carry a complete general-purpose runtime.
</p>

<h3>5.4 Backend families</h3>

<p>
A backend family is a class of downstream execution consumers or compiler consumers.
Backend families remain downstream from the canonical FROG Execution IR.
</p>

<p>
Examples of backend-family posture that are already relevant in the current published repository include:
</p>

<ul>
  <li>LLVM-oriented native CPU backend families,</li>
  <li>other future native, VM, interpreter, or accelerator-oriented families that may later be bounded explicitly.</li>
</ul>

<p>
A backend family is not a profile by itself, although a profile MAY define bounded compatibility with one or more backend families.
</p>

<h3>5.5 Runtime-facing capability contracts</h3>

<p>
A runtime-facing capability contract is a standardized statement about assumptions, services, or guarantees expected at a runtime boundary.
</p>

<p>
It is not identical to:
</p>

<ul>
  <li>canonical open IR,</li>
  <li>backend contract ownership in <code>IR/</code>,</li>
  <li>private runtime realization.</li>
</ul>

<p>
A profile MAY define runtime-facing capability expectations where justified, but those expectations MUST remain subordinate to the repository-wide separation between open IR, lowering, backend handoff, and private realization.
</p>

<h3>5.6 Profile-level execution contracts</h3>

<p>
A profile-level execution contract is a bounded profile companion document that closes the additional question:
</p>

<pre><code>Given a lowered and backend-contract-emitted program
inside one accepted profile subset,
under what explicit assumptions
may execution actually begin?
</code></pre>

<p>
A profile-level execution contract is therefore not:
</p>

<ul>
  <li>the generic backend contract owned by <code>IR/</code>,</li>
  <li>a universal runtime specification for all FROG programs,</li>
  <li>a license to move private runtime realization into normative language truth.</li>
</ul>

<p>
Instead, it is a profile-side closure surface for one bounded execution corridor.
</p>

<p>
Compactly:
</p>

<pre><code>backend contract
   =
standardized downstream handoff

profile-level execution contract
   =
additional bounded execution-start closure
for one published optional capability corridor
</code></pre>

<hr/>

<h2 id="profile-vs-runtime-boundary">6. Profile vs Runtime Boundary</h2>

<p>
A profile may constrain or classify runtime-facing expectations, but it does not own one runtime implementation and does not replace the architectural runtime boundary.
</p>

<p>
This distinction matters especially for advanced capability surfaces.
</p>

<pre><code>profile
   may define
capability assumptions

profile
   does not define
one private runtime realization
</code></pre>

<p>
Likewise:
</p>

<pre><code>profile
   may define
target-facing or deployment-facing classes

profile
   does not redefine
canonical open IR
or
backend contract ownership
</code></pre>

<p>
Profiles therefore remain optional specification layers, not hidden runtime specifications.
</p>

<p>
Where a profile publishes an execution contract, that execution contract remains:
</p>

<ul>
  <li>bounded,</li>
  <li>subordinate to the rest of the repository architecture,</li>
  <li>distinct from private runtime realization.</li>
</ul>

<hr/>

<h2 id="what-profiles-may-define">7. What Profiles MAY Define</h2>

<p>
A profile MAY define any bounded optional capability family that remains architecturally subordinate to the rest of the repository.
</p>

<p>
Examples already visible or directly supported by the current repository posture include:
</p>

<ul>
  <li>interoperability surfaces,</li>
  <li>restricted native compilation corridors,</li>
  <li>bounded execution-side companion contracts,</li>
  <li>portable widget-class catalogs and class-contract families,</li>
  <li>tool-consumable capability metadata needed by multiple conforming implementations.</li>
</ul>

<p>
A profile MAY therefore state:
</p>

<ul>
  <li>what optional capability claim exists,</li>
  <li>what assumptions that claim requires,</li>
  <li>what subset or bounded capability class it applies to,</li>
  <li>what explicit rejection conditions apply when those assumptions are not met,</li>
  <li>what additional execution-start assumptions must be satisfied when compilability alone is not enough.</li>
</ul>

<hr/>

<h2 id="what-profiles-must-not-define">8. What Profiles MUST NOT Define</h2>

<p>
A profile MUST NOT:
</p>

<ul>
  <li>redefine canonical source structure,</li>
  <li>redefine core validated-program meaning,</li>
  <li>move intrinsic language truth out of the core architecture,</li>
  <li>replace the ownership of <code>Libraries/</code> for intrinsic primitives,</li>
  <li>replace the ownership of <code>IR/</code> for canonical execution representation, derivation, lowering, or backend contract,</li>
  <li>replace the ownership of <code>Expression/</code> for canonical widget-instance declaration or other source-owned forms,</li>
  <li>replace the ownership of <code>IDE/</code> for authoring ergonomics, Program Model realization, or editor-private workflow behavior,</li>
  <li>turn one implementation strategy into mandatory language law,</li>
  <li>present one private runtime design as though it were the only conforming meaning of the capability family,</li>
  <li>collapse backend family, target-facing class, deployment-facing class, execution contract, runtime realization, and tool-private metadata into one ambiguous concept.</li>
</ul>

<p>
Profiles are optional architectural extensions.
They are not ownership shortcuts.
</p>

<hr/>

<h2 id="relation-with-other-specification-layers">9. Relation with Other Specification Layers</h2>

<p>
Profiles must remain readable relative to the rest of the repository.
</p>

<p>
The intended reading model is:
</p>

<pre><code>Expression/
   defines canonical source structure

Language/
   defines validated meaning

Libraries/
   define intrinsic primitives

IR/
   defines canonical execution-facing representation,
   derivation,
   construction,
   schema posture,
   lowering,
   backend contract

Profiles/
   define optional standardized capability families
   that remain subordinate to all of the above

IDE/
   defines authoring and observability concerns

Implementations/
   provide non-normative executable workspaces
</code></pre>

<p>
A profile may depend on these layers.
It must not absorb them.
</p>

<p>
Where a profile publishes an execution contract, the intended reading remains:
</p>

<pre><code>IR/
   keeps ownership of
backend handoff

Profiles/
   may add
bounded execution-side closure
for one optional capability corridor
</code></pre>

<p>
The same boundary rule applies to richer UI capability families:
</p>

<pre><code>Expression/
   keeps ownership of
widget-instance source representation

Libraries/
   keep ownership of
intrinsic frog.ui primitive identities

Profiles/
   may define
portable widget-class catalogs,
class contracts,
and optional rich UI capability families

IDE/
   may consume those contracts
for authoring and guided node generation
without becoming their normative owner
</code></pre>

<hr/>

<h2 id="profile-identification-and-claims">10. Profile Identification and Claims</h2>

<p>
Profiles are intended to be claimable.
</p>

<p>
An implementation MAY state that it supports:
</p>

<ul>
  <li>the core language only,</li>
  <li>the core language plus one or more published profiles,</li>
  <li>different subsets or maturity levels of profile support, provided those claims are explicit and not misleading.</li>
</ul>

<p>
A program, toolchain, or deployment path MAY also declare that it targets a given profile when that is meaningful and explicitly defined by the relevant profile specification.
</p>

<p>
A profile claim MUST remain precise.
For example:
</p>

<ul>
  <li><code>interop</code> means support for the published interoperability profile, not generic foreign-function support of unknown scope,</li>
  <li><code>native_cpu_llvm</code> means support for the published optional native CPU LLVM-oriented compilation profile, not a claim that LLVM defines FROG or that every valid FROG program is automatically compatible with that corridor,</li>
  <li><code>ui_widget_classes</code> means support for the published optional widget-class contract profile, not a vague claim that one IDE happens to expose proprietary property panels or object reflection.</li>
</ul>

<p>
If a profile also publishes an execution contract, support claims SHOULD remain equally precise.
For example:
</p>

<ul>
  <li>support for the compilation profile alone,</li>
  <li>support for the compilation profile plus its published execution contract,</li>
  <li>support for only a bounded execution-ready subset of that pair.</li>
</ul>

<p>
Vague claims such as “supports all advanced features” are not profile claims.
Explicit profile naming is preferred.
</p>

<hr/>

<h2 id="current-profile-specifications">11. Current Profile Specifications</h2>

<p>
The currently published profile specifications in this directory are:
</p>

<ul>
  <li><code>Interop.md</code> — optional interoperability capability profile,</li>
  <li><code>Native CPU LLVM.md</code> — optional native CPU compilation profile for LLVM-oriented backend families,</li>
  <li><code>Native CPU LLVM Execution contract.md</code> — optional Native CPU LLVM companion execution contract for bounded execution-start closure,</li>
  <li><code>UI Widget Classes.md</code> — optional rich UI widget-class catalog and class-contract profile for portable property, method, part, and event surfaces.</li>
</ul>

<p>
Profiles MAY also be accompanied by narrower companion documents when their corridor requires additional bounded closure, including profile-level execution contracts.
</p>

<h3>11.1 Interop</h3>

<p>
<code>Interop.md</code> is the normative home for the optional interoperability capability surface, including the standardized home for capability families that should not remain in the intrinsic core library layer.
</p>

<h3>11.2 Native CPU LLVM</h3>

<p>
<code>Native CPU LLVM.md</code> defines a first optional compilation-oriented profile for a conservative bounded subset capable of traveling through:
</p>

<pre><code>validated meaning
   -&gt;
canonical FROG Execution IR
   -&gt;
lowering
   -&gt;
backend contract
   -&gt;
LLVM-oriented native CPU backend family
</code></pre>

<p>
This profile exists to close one serious industrial compilation corridor without collapsing the language into LLVM and without redefining IR ownership inside <code>Profiles/</code>.
</p>

<p>
It should be read as:
</p>

<ul>
  <li>a bounded optional capability claim,</li>
  <li>a target-facing native compilation profile,</li>
  <li>a backend-family-oriented downstream compatibility claim,</li>
  <li>not a replacement for the core language or core IR stack.</li>
</ul>

<h3>11.3 Native CPU LLVM Execution Contract</h3>

<p>
<code>Native CPU LLVM Execution contract.md</code> is the companion execution-side closure document for the same bounded Native CPU LLVM profile family.
</p>

<p>
Its role is distinct:
</p>

<ul>
  <li><code>Native CPU LLVM.md</code> closes the compilation corridor,</li>
  <li><code>Native CPU LLVM Execution contract.md</code> closes the additional execution-start assumptions for a bounded accepted subset,</li>
  <li><code>IR/Backend contract.md</code> remains the generic consumer-facing backend handoff owned by <code>IR/</code>.</li>
</ul>

<p>
The intended distinction is:
</p>

<pre><code>compilation profile
   =
bounded route to backend-family consumability

backend contract
   =
generic standardized downstream handoff

profile-level execution contract
   =
additional bounded execution-start closure
for one accepted optional corridor
</code></pre>

<h3>11.4 UI Widget Classes</h3>

<p>
<code>UI Widget Classes.md</code> is the optional rich UI widget-class profile.
</p>

<p>
Its role is to standardize a portable widget-class catalog and class-contract surface for richer front-panel object ecosystems without moving ownership away from:
</p>

<ul>
  <li><code>Expression/</code> for canonical widget-instance source declaration,</li>
  <li><code>Libraries/UI.md</code> for intrinsic <code>frog.ui.*</code> executable primitives,</li>
  <li><code>IDE/</code> for Program Model realization and editor behavior,</li>
  <li><code>IR/</code> for execution-facing preservation, lowering, and backend handoff.</li>
</ul>

<p>
This profile should be read as:
</p>

<ul>
  <li>a bounded optional capability claim for portable widget-class catalogs,</li>
  <li>a class-contract surface for property, method, part, and event declaration,</li>
  <li>a validation aid for object-style UI interaction,</li>
  <li>not a universal rendering toolkit specification,</li>
  <li>not a hidden runtime-private widget ABI,</li>
  <li>not a replacement for core widget-instance source ownership.</li>
</ul>

<hr/>

<h2 id="core-conformance-profile-support-and-certification">12. Core Conformance, Profile Support, and Certification</h2>

<p>
Core language conformance and profile support are related but distinct.
</p>

<p>
An implementation may:
</p>

<ul>
  <li>conform to the FROG core without supporting any optional profile,</li>
  <li>conform to the FROG core and also support one or more optional profiles,</li>
  <li>support only bounded subsets of a profile, provided those claims are explicit.</li>
</ul>

<p>
Accordingly:
</p>

<pre><code>core conformance
   !=
profile support claim

profile support claim
   !=
support for every optional profile

support for one profile
   !=
support for every bounded subset,
every companion contract,
or every host environment
</code></pre>

<p>
Where conformance material exists for a profile corridor, acceptance or rejection under that corridor SHOULD remain explicit and separately claimable.
</p>

<p>
The same discipline applies to rich UI profiles:
</p>

<ul>
  <li>support for widget-instance source is not automatically support for a rich widget-class catalog,</li>
  <li>support for property-read nodes is not automatically support for every class family,</li>
  <li>support for one class family is not automatically support for every declared event surface or runtime host capability.</li>
</ul>

<hr/>

<h2 id="profile-evolution">13. Profile Evolution</h2>

<p>
Profiles allow the published specification architecture to grow without destabilizing the minimal core.
</p>

<p>
Over time, new profiles may standardize additional bounded capability families.
That growth should remain disciplined and should follow already justified repository-visible closure fronts rather than speculative category expansion.
</p>

<p>
New profiles SHOULD:
</p>

<ul>
  <li>name one bounded capability family clearly,</li>
  <li>state their assumptions explicitly,</li>
  <li>preserve ownership boundaries across the repository,</li>
  <li>avoid turning one implementation strategy into universal truth,</li>
  <li>remain claimable, inspectable, and compatible with conformance growth where relevant.</li>
</ul>

<p>
The preferred growth posture is therefore:
</p>

<pre><code>stabilize one high-value published corridor
   -&gt;
align profile claim, conformance, and reference consumption
   -&gt;
only then open the next optional family
</code></pre>

<hr/>

<h2 id="status">14. Status</h2>

<p>
This directory is normative.
</p>

<p>
It defines the architectural role of optional standardized capability families within the published FROG specification.
</p>

<p>
At the current repository posture, this surface should be read as:
</p>

<ul>
  <li>published and meaningful,</li>
  <li>already containing high-value bounded corridors,</li>
  <li>still under active closure for some of its most important families.</li>
</ul>

<p>
That evolution must remain architecture-preserving:
</p>

<ul>
  <li>the core language remains the core language,</li>
  <li>core IR ownership remains in <code>IR/</code>,</li>
  <li>intrinsic primitive ownership remains in <code>Libraries/</code>,</li>
  <li>source ownership remains in <code>Expression/</code>,</li>
  <li>authoring and observability ownership remain in <code>IDE/</code>,</li>
  <li>profiles remain optional standardized capability families.</li>
</ul>

<hr/>

<h2 id="summary">15. Summary</h2>

<p>
FROG profiles are optional standardized capability families that extend the usable capability surface of the language without redefining the minimal core.
</p>

<p>
They exist so that the repository can standardize broader industrially meaningful capability corridors while preserving architectural ownership boundaries.
</p>

<p>
In short:
</p>

<pre><code>Expression/
   owns canonical source

Language/
   owns validated meaning

Libraries/
   own intrinsic primitive vocabularies

IR/
   owns canonical execution-facing representation,
   lowering,
   and backend handoff

Profiles/
   own bounded optional capability families

IDE/
   owns authoring and observability tooling

Implementations/
   remain non-normative executable workspaces
</code></pre>

<p>
This makes it possible for FROG to grow through explicit optional profiles such as interoperability, bounded native compilation corridors, bounded execution-side closures, and portable rich UI widget-class capability families, without collapsing the architecture into one implementation or one runtime.
</p>
