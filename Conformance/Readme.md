<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance</h1>

<p align="center">
  <strong>Public accept / reject / preserve truth surface for the published FROG specification</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#core-purpose">2. Core Purpose</a></li>
  <li><a href="#why-this-directory-exists">3. Why This Directory Exists</a></li>
  <li><a href="#definition-of-conformance">4. Definition of Conformance</a></li>
  <li><a href="#non-goals">5. Non-Goals</a></li>
  <li><a href="#relation-with-specification-ownership">6. Relation with Specification Ownership</a></li>
  <li><a href="#critical-boundaries">7. Critical Boundaries</a></li>
  <li><a href="#directory-structure">8. Directory Structure</a></li>
  <li><a href="#published-cases">9. Published Cases</a></li>
  <li><a href="#expected-outcomes">10. Expected Outcomes</a></li>
  <li><a href="#active-v01-conformance-focus">11. Active v0.1 Conformance Focus</a></li>
  <li><a href="#relation-with-examples-ir-profiles-and-reference-implementation">12. Relation with Examples, IR, Profiles, and Reference Implementation</a></li>
  <li><a href="#future-growth">13. Future Growth</a></li>
  <li><a href="#summary">14. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This directory defines the public conformance surface of the published FROG repository.
</p>

<p>
It publishes explicit cases that make the specification testable, reviewable, and comparable across independent implementations.
</p>

<p>
It answers three fundamental questions:
</p>

<pre><code>What must be accepted?
What must be rejected?
What must be preserved?
</code></pre>

<p>
This directory does not define the language by itself.
It makes already-published language law operationally checkable.
Conformance therefore sits at the boundary between repository truth and observable implementation behavior.
It turns architectural distinctions into public expectations.
</p>

<p>
In v0.1, that public truth surface is not limited to source acceptance or semantic rejection alone.
It also includes preservation obligations across the published execution corridor:
</p>

<pre><code>.frog source
   -&gt;
loadability
   -&gt;
structural validity
   -&gt;
validated meaning
   -&gt;
canonical Execution IR Document
   -&gt;
canonical JSON IR validation where applicable
   -&gt;
later lowering / backend-facing handoff where applicable
   -&gt;
declared backend-family consumption where applicable
</code></pre>

<p>
Conformance therefore already matters to the compiler corridor, even when the downstream compilation route is still being consolidated.
A future industrial compilation chain must remain compatible with the same public accept / reject / preserve truth surface.
</p>

<hr/>

<h2 id="core-purpose">2. Core Purpose</h2>

<p>
The purpose of conformance is to transform specification rules into checkable public expectations.
</p>

<pre><code>Specification   = defines the rules
Conformance     = exposes testable cases
Implementation  = must behave accordingly
</code></pre>

<p>
A conforming implementation MAY vary internally.
It MUST NOT vary in published accept / reject / preserve outcomes for the same published case while still claiming conformance to the same published repository state.
</p>

<p>
Conformance therefore prevents the following drift:
</p>

<pre><code>parseable file
        -/-> structurally valid canonical source

structurally valid canonical source
        -/-> validated meaning

validated meaning
        -/-> arbitrary execution-facing form

implementation convenience
        -/-> language truth

"it runs"
        -/-> "it is correct"
</code></pre>

<p>
Conformance is therefore not a commentary layer.
It is the public truth surface that makes published repository law observable, comparable, and reviewable.
</p>

<hr/>

<h2 id="why-this-directory-exists">3. Why This Directory Exists</h2>

<p>
FROG is specification-first and implementation-independent.
</p>

<p>
That creates a requirement:
</p>

<pre><code>multiple independent implementations
            require
one shared public validation surface
</code></pre>

<p>
This directory provides that surface.
</p>

<p>
It ensures that:
</p>

<ul>
  <li>implementations do not silently diverge,</li>
  <li>semantic meaning is not reinterpreted implicitly,</li>
  <li>invalid constructs are rejected instead of silently repaired,</li>
  <li>critical distinctions remain visible across loadability, structural validation, semantic validation, IR derivation, IR construction, canonical JSON validation, later lowering / backend-facing handoff, and declared backend-family consumption where applicable.</li>
</ul>

<p>
Conformance is therefore not commentary.
It is the public truth surface that turns specification architecture into comparable observable behavior.
</p>

<hr/>

<h2 id="definition-of-conformance">4. Definition of Conformance</h2>

<p>
Conformance is alignment with the published specification across the relevant architectural stages.
</p>

<p>
It includes:
</p>

<ul>
  <li>correct rejection of malformed or non-loadable source where applicable,</li>
  <li>correct source-structure validation,</li>
  <li>correct semantic validation,</li>
  <li>correct rejection where validated meaning does not exist,</li>
  <li>correct preservation of distinctions across derivation into the canonical Execution IR Document,</li>
  <li>correct canonical-IR construction and canonical JSON compatibility where the case reaches that stage,</li>
  <li>correct preservation of required boundaries across later lowering and backend-facing handoff where applicable,</li>
  <li>correct staged acceptance or rejection across declared compiler-corridor stages where applicable.</li>
</ul>

<p>
Where published cases include <code>ide.provenance</code>, conformance expectations MAY also test preservation and reporting behavior: provenance-aware tools should preserve valid or unknown attestations when safe, report stale or malformed provenance explicitly, and never use provenance to redefine what the program computes.
</p>

<p>
Conformance therefore requires both:
</p>

<ul>
  <li>accept / reject correctness, and</li>
  <li>preservation correctness.</li>
</ul>

<pre><code>Conformance =
  correct interpretation
  +
  correct preservation
  +
  correct rejection
</code></pre>

<p>
A public conformance surface SHOULD make explicit whether a case is:
</p>

<ul>
  <li>not loadable as source,</li>
  <li>loadable but structurally invalid as canonical source,</li>
  <li>structurally valid but semantically rejected,</li>
  <li>semantically accepted but preservation-invalid at the IR boundary,</li>
  <li>semantically accepted but canonical-IR-schema-invalid,</li>
  <li>semantically accepted and IR-valid but rejected at a later lowering, backend-contract, or declared backend-family consumer boundary,</li>
  <li>accepted with preservation requirements.</li>
</ul>

<p>
Where useful, a case MAY also make explicit whether it is:
</p>

<ul>
  <li>schema-checkable at the source-shape level,</li>
  <li>structurally invalid for reasons that remain source-owned even when they exceed a minimal declarative schema artifact,</li>
  <li>semantically invalid even though canonical source shape has already been accepted,</li>
  <li>IR-schema-invalid even though semantic meaning was previously established.</li>
</ul>

<p>
This distinction matters because machine-checkable schema is part of the validation corridor.
It is not a replacement for semantic validation.
Likewise, canonical JSON IR validity is not a replacement for semantic validity or IR architectural validity.
</p>

<hr/>

<h2 id="non-goals">5. Non-Goals</h2>

<p>
This directory does not define:
</p>

<ul>
  <li>a certification program,</li>
  <li>a universal implementation compatibility matrix,</li>
  <li>a full execution-performance suite,</li>
  <li>a debugger validation framework in full,</li>
  <li>a deployment validation system in full.</li>
</ul>

<p>
It also does not introduce new language rules.
</p>

<pre><code>Specification defines
Conformance exposes
Implementation follows
</code></pre>

<p>
Conformance must never become the place where missing language law is invented retroactively.
When a case reveals ambiguity, the owning specification document must be clarified.
</p>

<hr/>

<h2 id="relation-with-specification-ownership">6. Relation with Specification Ownership</h2>

<p>
Conformance does not own language truth.
</p>

<p>
Ownership remains:
</p>

<ul>
  <li><code>Expression/</code> — source structure, canonical source shape, structural validity, and source-schema posture,</li>
  <li><code>Language/</code> — semantic truth and validated meaning,</li>
  <li><code>Libraries/</code> — intrinsic primitive vocabularies,</li>
  <li><code>Profiles/</code> — optional standardized capability families,</li>
  <li><code>IR/</code> — canonical execution-facing representation, derivation, identity, construction, schema posture, lowering, and backend-facing boundaries,</li>
  <li><code>IDE/</code> — tooling behavior and authoring-facing concerns.</li>
</ul>

<p>
Conformance cases must always map back to these owners.
</p>

<pre><code>specification
      -&gt;
conformance
      -&gt;
implementation
</code></pre>

<p>
Never the reverse.
</p>

<p>
In particular:
</p>

<ul>
  <li>source-shape and schema-owned rejection cases map back to <code>Expression/</code>,</li>
  <li>semantic rejection cases map back to <code>Language/</code>, <code>Libraries/</code>, or <code>Profiles/</code> as appropriate,</li>
  <li>preservation cases across derivation, identity, construction, and canonical JSON IR validation map back to <code>IR/</code>.</li>
</ul>

<p>
A case file is therefore not a second specification.
It is a public executable reading of already-published ownership.
</p>

<hr/>

<h2 id="critical-boundaries">7. Critical Boundaries</h2>

<p>
The first critical conformance boundary is the staged progression:
</p>

<pre><code>.frog source
      |
      v
loadable source
      |
      v
structurally valid canonical source
      |
      v
validated program meaning
</code></pre>

<p>
Conformance must make these boundaries observable.
</p>

<p>
This includes verifying:
</p>

<ul>
  <li>what fails before the source is even loadable,</li>
  <li>what fails at source-shape or schema-owned structural validation,</li>
  <li>what fails after canonical source shape is accepted but before semantic meaning is established,</li>
  <li>what establishes semantic meaning,</li>
  <li>what distinctions must survive validation,</li>
  <li>what distinctions must survive derivation into the canonical Execution IR Document,</li>
  <li>what identity, attribution, and correspondence carriers must remain explicit at the canonical JSON IR boundary,</li>
  <li>what distinctions must remain explicit before later lowering and backend-facing handoff.</li>
</ul>

<p>
The second critical boundary is:
</p>

<pre><code>validated program meaning
      |
      v
canonical Execution IR Document
</code></pre>

<p>
The third critical downstream boundary is:
</p>

<pre><code>canonical Execution IR Document
      |
      v
lowering / backend-facing handoff
</code></pre>

<p>
Critical invariants therefore include:
</p>

<pre><code>front panel                    != public interface
widget_value                   != widget_reference
layout                         != execution
visual order                   != execution order
feedback shape                 != state
structural validity            != semantic acceptance
schema acceptance              != semantic acceptance
validated meaning              != canonical Execution IR
canonical Execution IR         != private runtime realization
backend contract               != private runtime structure
backend family                 != target profile
deployment mode                != runtime-private realization
compiler-family route          != FROG semantic truth
source provenance            != executable truth
missing provenance           != automatic AI origin
</code></pre>

<p>
These invariants are enforced through explicit valid / invalid and preserve / reject expectations.
</p>

<hr/>

<h2 id="directory-structure">8. Directory Structure</h2>

<p>
The current published directory structure is layered rather than uniform.
</p>

<p>
At top level:
</p>

<p>
The top-level conformance directory includes both staged matrix documents:
<code>Source to FIR conformance matrix.md</code> and
<code>Source FIR Lowering conformance matrix.md</code>.
</p>

<pre><code>Conformance/
├── Readme.md
├── Source to FIR conformance matrix.md
├── Source FIR Lowering conformance matrix.md
├── valid/
└── invalid/
</code></pre>

<p>
<code>Source to FIR conformance matrix.md</code> defines the staged
accept / reject / preserve expectations for moving from canonical
<code>.frog</code> source through semantic validation into canonical
Execution IR / FIR. It is a matrix for organizing future cases, not a
replacement for the case corpus.
</p>

<p>
Inside <code>valid/</code>, the published corpus already combines:
</p>

<ul>
  <li>a top-level historical block of valid source-shape, architectural, IR-sensitive, and executable-anchor cases,</li>
  <li>a dedicated <code>compiler/</code> family,</li>
  <li>a dedicated <code>executable/</code> family,</li>
  <li>a dedicated <code>structural/</code> family with its own local index.</li>
</ul>

<p>
This means the published positive corpus is already hybrid:
</p>

<pre><code>top-level historical valid cases
   +
valid/compiler/
   +
valid/executable/
   +
valid/structural/
</code></pre>

<p>
The positive structural subtree is organized as:
</p>

<pre><code>valid/structural/
├── Readme.md
├── 01_front_panel_canvas_widgets_and_ui_libraries/
└── 02_front_panel_recursive_children_shape_is_valid/
</code></pre>

<p>
Inside <code>invalid/</code>, the published corpus combines:
</p>

<ul>
  <li>a top-level historical block of invalid source-shape and architectural-rejection cases,</li>
  <li>a dedicated <code>compiler/</code> family,</li>
  <li>a dedicated <code>structural/</code> family with its own local index.</li>
</ul>

<p>
This means the published negative corpus is also hybrid:
</p>

<pre><code>top-level historical invalid cases
   +
invalid/compiler/
   +
invalid/structural/
</code></pre>

<p>
The negative structural subtree is organized as:
</p>

<pre><code>invalid/structural/
├── Readme.md
├── 01_front_panel_widgets_must_be_array/
└── 02_front_panel_canvas_must_be_object/
</code></pre>

<p>
Structured subtrees do not erase earlier top-level anchors.
They provide clearer homes for focused family growth after the original historical block.
</p>

<p>
Each case should define, as applicable:
</p>

<ul>
  <li>expected loadability,</li>
  <li>expected structural validity,</li>
  <li>expected semantic acceptance,</li>
  <li>expected IR eligibility,</li>
  <li>expected canonical JSON IR validity where relevant,</li>
  <li>tested boundaries,</li>
  <li>preservation requirements,</li>
  <li>rejection reason if invalid.</li>
</ul>

<p>
In practice, the directory serves three public roles:
</p>

<ul>
  <li>accept — what must validate and remain correct,</li>
  <li>reject — what must fail explicitly,</li>
  <li>preserve — what distinctions must remain explicit across later stages.</li>
</ul>

<p>
For v0.1, published cases SHOULD remain small, sharply owned, and explicit about the stage at which acceptance or rejection occurs.
</p>

<hr/>

<h2 id="published-cases">9. Published Cases</h2>

<p>
The currently published conformance corpus is staged rather than monolithic.
</p>

<p>
The source-to-FIR matrix is the public planning and classification surface for
future cases that test canonical source acceptance, semantic validation, FIR
eligibility, and preservation. Case files remain the concrete truth surface;
the matrix keeps those cases aligned with the repository architecture.
</p>

<p>
The source/FIR/lowering matrix is the corresponding public planning and
classification surface for later corridor cases. It does not replace concrete
case files. It states which stage owns acceptance, rejection, preservation,
manifest/provider resolution, and runtime-consumption expectations.
</p>

<h3>9.1 Published top-level valid anchors</h3>

<p>
The published positive corpus already contains a top-level historical block under <code>Conformance/valid/</code> that includes:
</p>

<ul>
  <li>minimal canonical source-shape cases,</li>
  <li>optional-section validity cases,</li>
  <li>front-panel and connector structure cases,</li>
  <li>named executable anchors,</li>
  <li>architectural distinction cases tied to canonical Execution IR correspondence discipline.</li>
</ul>

<p>
This top-level block remains part of the published truth surface and must not be retroactively described as though it had been replaced already.
</p>

<h3>9.2 Early executable anchor cases</h3>

<p>
The published top-level positive corpus includes the following early valid executable anchor cases:
</p>

<pre><code>01_pure_addition.md
02_ui_value_roundtrip.md
03_ui_property_write.md
04_stateful_feedback_delay.md
</code></pre>

<p>
These anchors are important because they already expose bounded executable truth across four distinct axes:
</p>

<ul>
  <li>pure dataflow computation,</li>
  <li>natural <code>widget_value</code> participation,</li>
  <li><code>widget_reference</code> object-operation participation,</li>
  <li>explicit state through feedback and delay.</li>
</ul>

<p>
They therefore already constrain future compiler and runtime growth, even before the entire executable corpus becomes fully normalized under one subtree.
</p>

<h3>9.3 Additional top-level architectural distinction cases</h3>

<p>
The same published top-level positive corpus also includes additional cases such as:
</p>

<pre><code>05_public_interface_and_widget_participation_distinct.md
07_widget_reference_remains_distinct_from_widget_value.md
</code></pre>

<p>
and later correspondence-sensitive cases with higher numbering that protect canonical Execution IR boundary distinctions.
</p>

<p>
This means the top-level positive corpus is not only “early examples”.
It is already part of the published architectural truth surface.
</p>

<h3>9.4 Compiler-corridor families</h3>

<p>
The conformance tree also opens a dedicated compiler-corridor family:
</p>

<pre><code>valid/compiler/
invalid/compiler/
</code></pre>

<p>
These families extend the public truth surface beyond ordinary language validity and IR correctness by making later corridor stages readable and testable:
</p>

<pre><code>semantic acceptance
   -&gt;
IR derivation
   -&gt;
canonical JSON IR validity where applicable
   -&gt;
lowering eligibility
   -&gt;
backend-contract eligibility
   -&gt;
declared backend-family consumability
</code></pre>

<p>
They should also be read together with the illustrative compiler mirror published under <code>Examples/compiler/</code>.
That mirror is illustrative.
The conformance families remain the public expectation surface.
</p>

<p>
The compiler-corridor families now include concrete source/FIR/lowering case
records for provider-backed standard-library calls, provider requirement
omission, ABI/profile mismatch, manifest artifact or checksum failure, LLVM
identity collapse, widget value/reference preservation, and selected-profile
<code>unsupported_subset</code> rejection. These records instantiate the
source/FIR/lowering matrix while remaining implementation-independent.
</p>

<h3>9.5 Executable subfamily posture</h3>

<p>
The published positive tree also already contains a dedicated <code>valid/executable/</code> subtree.
</p>

<p>
That means the repository has already opened a structural place for executable-family growth.
</p>

<p>
However, the top-level executable anchors still remain published at the root of <code>valid/</code>.
The correct reading is therefore:
</p>

<pre><code>top-level executable anchors
   remain published historical anchors

valid/executable/
   provides structured executable-family growth

the two are not mutually exclusive
</code></pre>

<h3>9.6 Structural subfamily posture</h3>

<p>
The same growth rule applies to source-shape closure.
</p>

<p>
Dedicated subtrees such as:
</p>

<pre><code>valid/structural/
invalid/structural/
</code></pre>

<p>
are appropriate when:
</p>

<ul>
  <li>the owning law is already published under <code>Expression/</code>,</li>
  <li>the goal is focused source-shape growth rather than semantic growth,</li>
  <li>the new subtree clarifies, rather than rewrites, the earlier top-level historical block.</li>
</ul>

<p>
This is especially appropriate for front-panel structural closure, where the repository already publishes:
</p>

<ul>
  <li>front-panel source ownership,</li>
  <li>widget instance source ownership,</li>
  <li>a class-level widget contract model,</li>
  <li>a source-schema posture that remains intentionally conservative.</li>
</ul>

<hr/>

<h2 id="expected-outcomes">10. Expected Outcomes</h2>

<p>
Each case expresses structured expectations such as:
</p>

<ul>
  <li>Expected loadability: <code>loadable</code> | <code>not loadable</code></li>
  <li>Expected structural validity: <code>valid</code> | <code>invalid</code></li>
  <li>Expected meaning: <code>established</code> | <code>not established</code></li>
  <li>Expected IR result: <code>derivable</code> | <code>not derivable</code></li>
  <li>Expected IR schema result: <code>schema-valid</code> | <code>not schema-valid</code></li>
  <li>Expected lowering result: <code>lowerable</code> | <code>rejected</code> | <code>not applicable</code></li>
  <li>Expected backend-contract result: <code>emittable</code> | <code>not emittable</code> | <code>not applicable</code></li>
  <li>Expected backend-family result: <code>consumable</code> | <code>rejected</code> | <code>not applicable</code></li>
  <li>Expected executable result where already published: bounded positive executable truth for the named anchor or executable-family case</li>
</ul>

<p>
The expected question is therefore not only:
</p>

<pre><code>Did the tool run?
</code></pre>

<p>
but also:
</p>

<pre><code>Did it accept or reject at the correct stage?
Did it preserve the correct distinctions?
Did it avoid inventing meaning that the specification does not grant?
Did it match the published executable truth where executable anchors already exist?
</code></pre>

<hr/>

<h2 id="active-v01-conformance-focus">11. Active v0.1 Conformance Focus</h2>

<p>
The active v0.1 conformance focus is now multi-layered.
</p>

<p>
The published corpus already contains:
</p>

<ul>
  <li>top-level source-shape truth,</li>
  <li>top-level architectural distinction truth,</li>
  <li>top-level early executable anchors,</li>
  <li>a dedicated compiler-corridor family,</li>
  <li>a dedicated executable subtree for structured future executable growth,</li>
  <li>dedicated positive and negative structural subtrees for focused source-shape growth.</li>
  <li>focused source/FIR/lowering compiler-corridor cases for provider, manifest, backend-contract, runtime-consumption, widget-preservation, and unsupported-subset boundaries.</li>
</ul>

<p>
This is already a substantial closure step.
</p>

<p>
It means the repository no longer treats downstream execution as future-only strategy.
It already exposes:
</p>

<pre><code>source truth
   +
meaning truth
   +
IR-sensitive truth
   +
early executable truth
   +
compiler-corridor truth
   +
structured source-shape truth
   +
room for structured executable-family growth
</code></pre>

<p>
The next structural closure step should therefore remain conservative:
</p>

<ul>
  <li>keep the historical top-level block readable,</li>
  <li>grow focused structural subfamilies where the source law is already explicit,</li>
  <li>preserve early executable anchors as published truth,</li>
  <li>align compiler-corridor conformance with the bounded illustrative mirror under <code>Examples/compiler/</code>,</li>
  <li>prefer small high-signal cases over broad unfocused expansion.</li>
</ul>

<p>
Front-panel structural closure is a good example of such growth because:
</p>

<ul>
  <li>its ownership is clearly in <code>Expression/</code>,</li>
  <li>its schema posture is intentionally conservative but now more explicit,</li>
  <li>its failure modes are source-owned and easy to stage cleanly.</li>
</ul>

<hr/>

<h2 id="relation-with-examples-ir-profiles-and-reference-implementation">12. Relation with Examples, IR, Profiles, and Reference Implementation</h2>

<p>
This directory must be read together with nearby repository areas, but without ownership collapse.
</p>

<ul>
  <li><code>Examples/</code> provides illustrative named slices. Examples do not become hidden language law.</li>
  <li><code>IR/</code> owns canonical execution-facing representation, derivation, identity, construction, schema posture, lowering, and backend-facing boundaries.</li>
  <li><code>Profiles/</code> owns optional capability corridors such as <code>native_cpu_llvm</code>, and may later own bounded execution-contract companion documents where published.</li>
  <li><code>Implementations/Reference/</code> consumes the published layers without owning them.</li>
</ul>

<p>
The practical relation is:
</p>

<pre><code>Examples
   -&gt; illustrate

Conformance
   -&gt; expose public expectations

IR
   -&gt; define canonical execution-facing law

Profiles
   -&gt; bound optional downstream corridors

Reference implementation
   -&gt; consume the published layers without owning them
</code></pre>

<p>
Within that reading discipline, the relation with <code>Examples/compiler/</code> must remain explicit:
</p>

<pre><code>Examples/compiler/
   -&gt; bounded illustrative mirror

Conformance/valid/compiler/ and Conformance/invalid/compiler/
   -&gt; public acceptance / rejection / preservation surface

the mirror does not replace the public truth surface
</code></pre>

<p>
Likewise, the early top-level executable anchors in <code>Conformance/valid/</code> remain part of the published truth surface even though <code>Examples/</code> now also exposes a bounded compiler-oriented mirror.
The two surfaces serve different roles and should remain distinguishable.
</p>

<hr/>

<h2 id="future-growth">13. Future Growth</h2>

<p>
Future growth should remain conservative and corridor-oriented.
</p>

<p>
A good growth rule is:
</p>

<pre><code>close one public corridor fully
before opening many half-closed fronts
</code></pre>

<p>
The preferred near-term growth order is therefore:
</p>

<pre><code>top-level source-shape and architectural closure
   -&gt;
structured source-shape subfamilies where useful
   -&gt;
top-level executable anchor preservation
   -&gt;
named compiler-corridor family closure
   -&gt;
structured executable-family growth
   -&gt;
later corridor expansion
</code></pre>

<p>
That means structural growth should:
</p>

<ul>
  <li>remain compatible with already-published top-level source-shape anchors,</li>
  <li>avoid pretending those anchors never existed,</li>
  <li>treat <code>valid/structural/</code> and <code>invalid/structural/</code> as structured growth rather than retroactive erasure,</li>
  <li>stay tightly mapped to published <code>Expression/</code> ownership.</li>
</ul>

<p>
Compiler-corridor growth should:
</p>

<ul>
  <li>remain explicitly bounded by published profile posture,</li>
  <li>stay compatible with the open IR boundary rather than collapsing into backend-private truth,</li>
  <li>treat <code>Examples/compiler/</code> as an illustrative neighbor rather than as a substitute for conformance,</li>
  <li>preserve the distinction between backend-family consumption and FROG semantic truth.</li>
</ul>

<p>
Conformance growth should therefore follow publication maturity, not wishful scope expansion.
</p>

<p>
For source-to-FIR growth, each feature family should gain source scope,
semantic validation, positive cases, negative cases, preservation
expectations, and bounded implementation posture before being described as
systematically covered.
</p>

<hr/>

<h2 id="summary">14. Summary</h2>

<p>
This directory defines the public conformance surface of the published FROG specification.
</p>

<p>
It answers:
</p>

<pre><code>What must be accepted?
What must be rejected?
What must be preserved?
</code></pre>

<p>
In v0.1, the active published conformance surface already includes:
</p>

<ul>
  <li>top-level source-shape and architectural truth,</li>
  <li>top-level early executable anchor cases,</li>
  <li>a named valid / invalid compiler-corridor family,</li>
  <li>a structured executable subtree for further executable-family growth,</li>
  <li>structured positive and negative source-shape subtrees for focused structural closure.</li>
</ul>

<p>
It should therefore be read as a public truth surface for the full published corridor:
</p>

<pre><code>source
   -&gt;
meaning
   -&gt;
IR
   -&gt;
lowering
   -&gt;
backend handoff
   -&gt;
backend-family consumability
   -&gt;
bounded executable truth where already published
</code></pre>

<p>
Within that corridor:
</p>

<ul>
  <li><code>Examples/</code> illustrates,</li>
  <li><code>Conformance/</code> exposes public expectations,</li>
  <li><code>Implementations/Reference/</code> consumes the published layers without owning them.</li>
</ul>

<p>
This keeps the repository disciplined:
</p>

<pre><code>specification defines

conformance checks

implementation must align
</code></pre>

<p>
That is the intended role of <code>Conformance/</code> in FROG v0.1.
</p>
