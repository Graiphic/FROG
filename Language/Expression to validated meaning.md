<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Expression to Validated Meaning</h1>

<p align="center">
  <strong>Normative boundary from structurally valid canonical <code>.frog</code> source to validated program meaning</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#boundary-contract">2. Boundary Contract</a></li>
  <li><a href="#position-in-the-architecture">3. Position in the Architecture</a></li>
  <li><a href="#scope">4. Scope</a></li>
  <li><a href="#relation-with-other-specifications">5. Relation with Other Specifications</a></li>
  <li><a href="#validation-inputs">6. Validation Inputs</a></li>
  <li><a href="#validation-output">7. Validation Output</a></li>
  <li><a href="#semantic-closure-guarantees">8. Semantic Closure Guarantees</a></li>
  <li><a href="#source-families-and-semantic-ownership">9. Source Families and Semantic Ownership</a></li>
  <li><a href="#execution-relevant-content">10. Execution-Relevant Content</a></li>
  <li><a href="#non-semantic-and-support-content">11. Non-Semantic and Support Content</a></li>
  <li><a href="#critical-distinctions-preserved">12. Critical Distinctions Preserved</a></li>
  <li><a href="#structures-state-and-cycles">13. Structures, State, and Cycles</a></li>
  <li><a href="#type-value-and-interface-commitments">14. Type, Value, and Interface Commitments</a></li>
  <li><a href="#ui-participation-at-the-language-boundary">15. UI Participation at the Language Boundary</a></li>
  <li><a href="#attribution-and-recoverability">16. Attribution and Recoverability</a></li>
  <li><a href="#forbidden-boundary-collapses">17. Forbidden Boundary Collapses</a></li>
  <li><a href="#contract-with-ir-derivation">18. Contract with IR Derivation</a></li>
  <li><a href="#out-of-scope">19. Out of Scope</a></li>
  <li><a href="#summary">20. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the normative transition from <strong>structurally valid canonical <code>.frog</code> source</strong> to
<strong>validated program meaning</strong>.
</p>

<p>
It establishes what MUST be true before any canonical FROG Execution IR may be derived.
</p>

<pre><code>Expression/  -&gt;  Language/  -&gt;  IR/
   source          meaning       execution
</code></pre>

<p>
This document governs the first semantic transition:
</p>

<pre><code>structurally valid canonical source
    -&gt;
validated program meaning
</code></pre>

<p>
Its role is not to restate the full source model and not to define IR structure.
Its role is to define the <strong>semantic closure boundary</strong> between those two layers.
</p>

<p>
That distinction matters:
a source file may be loadable,
it may be structurally valid as canonical source,
and it may still fail to produce validated meaning.
This document governs only the final step in that sequence.
</p>

<hr/>

<h2 id="boundary-contract">2. Boundary Contract</h2>

<p>
This boundary produces exactly one of two outcomes:
</p>

<ul>
  <li>a valid, semantically closed program meaning, or</li>
  <li>a rejection, meaning no validated meaning exists for that accepted source.</li>
</ul>

<p>
No conforming system may derive a canonical Execution IR Document without validated meaning.
</p>

<p>
The output of this boundary MUST be:
</p>

<ul>
  <li>semantically complete for the accepted source,</li>
  <li>execution-relevant,</li>
  <li>non-ambiguous,</li>
  <li>attributable to accepted source content,</li>
  <li>independent of runtime-private execution policy.</li>
</ul>

<p>
The output of this boundary is not:
</p>

<ul>
  <li>an IR,</li>
  <li>a runtime representation,</li>
  <li>a lowered form,</li>
  <li>a deployment form,</li>
  <li>a backend-family-specific handoff.</li>
</ul>

<p>
This document therefore defines the semantic admission contract that must be satisfied before any canonical execution-facing derivation can begin.
</p>

<hr/>

<h2 id="position-in-the-architecture">3. Position in the Architecture</h2>

<p>
The intended architectural order is:
</p>

<pre><code>canonical .frog source
        |
        v
loadability
        |
        v
structural validity
        |
        v
validated program meaning
        |
        v
canonical Execution IR Document
        |
        v
lowering
        |
        v
backend contract
        |
        v
private realization
</code></pre>

<p>
This document owns the semantic line only:
</p>

<pre><code>structurally valid canonical source
        |
        v
validated program meaning
</code></pre>

<p>
Accordingly:
</p>

<ul>
  <li><code>Expression/</code> owns source shape and structural validity,</li>
  <li><code>Language/</code> owns semantic truth,</li>
  <li><code>IR/</code> owns execution-oriented derived representation.</li>
</ul>

<p>
A conforming implementation MUST therefore not:
</p>

<ul>
  <li>bypass structural validity when attempting semantic admission,</li>
  <li>bypass validated meaning by jumping directly from source to IR,</li>
  <li>bypass validated meaning by jumping directly from source to private execution.</li>
</ul>

<hr/>

<h2 id="scope">4. Scope</h2>

<p>
This document defines:
</p>

<ul>
  <li>what semantic validation MUST establish before semantic acceptance,</li>
  <li>what becomes validated program meaning,</li>
  <li>which distinctions MUST remain explicit at the semantic boundary,</li>
  <li>what MUST remain attributable and recoverable,</li>
  <li>what source content MUST NOT influence semantic meaning.</li>
</ul>

<p>
This document does not define:
</p>

<ul>
  <li>source serialization syntax in general,</li>
  <li>source-schema posture or machine-checkable structural source shape,</li>
  <li>primitive implementation internals,</li>
  <li>Execution IR structure,</li>
  <li>IR canonical JSON schema shape,</li>
  <li>lowering strategy,</li>
  <li>backend contract shape,</li>
  <li>runtime-private behavior.</li>
</ul>

<hr/>

<h2 id="relation-with-other-specifications">5. Relation with Other Specifications</h2>

<pre><code>Expression/   -&gt; canonical source shape and structural validity
Language/     -&gt; semantic truth at the validated-program boundary
IR/           -&gt; canonical execution-oriented derived representation
</code></pre>

<p>
This document is downstream from the source model in <code>Expression/</code> and upstream from
<code>IR/Derivation rules.md</code>.
</p>

<p>
In practical terms:
</p>

<ul>
  <li><code>Expression/</code> answers: <em>what may be written canonically, and what counts as structurally valid source?</em></li>
  <li>this document answers: <em>what must be true for that structurally valid source to count as validated meaning?</em></li>
  <li><code>IR/Derivation rules.md</code> answers: <em>how does validated meaning become the canonical Execution IR Document?</em></li>
</ul>

<p>
This document also works together with:
</p>

<ul>
  <li><code>Language/Execution model.md</code>,</li>
  <li><code>Language/State and cycles.md</code>,</li>
  <li><code>Language/Control structures.md</code>,</li>
  <li><code>Expression/Diagram.md</code>,</li>
  <li><code>Expression/Interface.md</code>,</li>
  <li><code>Expression/Front panel.md</code>,</li>
  <li><code>Expression/Widget.md</code>,</li>
  <li><code>Expression/Widget interaction.md</code>,</li>
  <li><code>Expression/Type.md</code>.</li>
</ul>

<hr/>

<h2 id="validation-inputs">6. Validation Inputs</h2>

<p>
Validation at this boundary MUST consider all structurally valid source content that can affect semantic acceptance.
That includes, where present and relevant:
</p>

<ul>
  <li>public interface declarations,</li>
  <li>diagram nodes and edges,</li>
  <li>type expressions and type compatibility,</li>
  <li>primitive identity and primitive-local requirements,</li>
  <li>structure declarations and region usage,</li>
  <li>state and cycle legality,</li>
  <li>widget participation where execution-relevant,</li>
  <li>profile-owned capability requirements when explicitly used.</li>
</ul>

<p>
Structural validation is upstream of this document.
Semantic admission at this boundary MUST therefore assume structurally valid canonical source as input, not raw malformed or structurally rejected source.
</p>

<p>
Validation is therefore cross-document and cross-layer in input, but its resulting semantic acceptance is owned here.
</p>

<hr/>

<h2 id="validation-output">7. Validation Output</h2>

<p>
Validated meaning MUST define, for every accepted program:
</p>

<ul>
  <li>the public interface boundary,</li>
  <li>the executable participation of accepted diagram content,</li>
  <li>the legality and meaning of structures,</li>
  <li>the legality and meaning of explicit state,</li>
  <li>typed value-flow commitments,</li>
  <li>UI participation where that participation is execution-relevant,</li>
  <li>required profile-owned capability assumptions where applicable.</li>
</ul>

<p>
After this stage, semantic ambiguity MUST NOT remain for IR derivation.
IR derivation may still expand, normalize, or make execution-facing structure more explicit later,
but it MUST NOT still be solving basic semantic uncertainty that should have been resolved here.
</p>

<hr/>

<h2 id="semantic-closure-guarantees">8. Semantic Closure Guarantees</h2>

<p>
Validation at this boundary MUST guarantee:
</p>

<ul>
  <li>no missing execution-relevant semantic information for the accepted subset,</li>
  <li>no implicit execution dependency created only by layout,</li>
  <li>no implicit memory used to legalize invalid recurrence,</li>
  <li>no semantic reliance on editor-only arrangement,</li>
  <li>no semantic reliance on runtime-private scheduling policy,</li>
  <li>no hidden promotion of implementation convenience into language meaning.</li>
</ul>

<pre><code>validated meaning must be:
✔ explicit
✔ semantically complete
✔ deterministic at the dependency level
✔ attributable
✔ independent of runtime-private policy
</code></pre>

<p>
This does not mean that every later execution detail is already fixed here.
It means that everything needed to derive correct execution meaning is already settled here.
</p>

<hr/>

<h2 id="source-families-and-semantic-ownership">9. Source Families and Semantic Ownership</h2>

<h3>9.1 Source families that may contribute to semantic meaning</h3>

<ul>
  <li>public interface declarations,</li>
  <li>diagram-side executable participation,</li>
  <li>structure boundaries and structure terminals,</li>
  <li>explicit state ownership and state initialization,</li>
  <li>primitive usage and primitive references,</li>
  <li>execution-relevant widget participation.</li>
</ul>

<h3>9.2 Source families that are support-only unless explicitly made semantically relevant elsewhere</h3>

<ul>
  <li>connector presentation,</li>
  <li>front-panel layout that does not participate in execution meaning,</li>
  <li>icon data,</li>
  <li>IDE-facing metadata,</li>
  <li>cache content.</li>
</ul>

<p>
A support family MAY still matter for tooling, authoring, discoverability, or transport.
That does not by itself make it part of validated semantic truth.
</p>

<hr/>

<h2 id="execution-relevant-content">10. Execution-Relevant Content</h2>

<p>
Execution-relevant content is the source content whose accepted meaning contributes to later correct execution.
That includes, where present:
</p>

<ul>
  <li>executable nodes,</li>
  <li>typed edges and directed data dependencies,</li>
  <li>structure boundaries and structure-local terminals,</li>
  <li>explicit state-bearing constructs,</li>
  <li>public interface participation,</li>
  <li><code>widget_value</code> participation,</li>
  <li><code>widget_reference</code> participation,</li>
  <li>standardized UI-object interaction primitives when validly used,</li>
  <li>profile-owned executable capability usage when validly declared.</li>
</ul>

<p>
Execution relevance does not mean that the source must already look like IR.
It means that accepted semantic meaning must already know what counts as real executable participation.
</p>

<hr/>

<h2 id="non-semantic-and-support-content">11. Non-Semantic and Support Content</h2>

<p>
The following MUST NOT influence validated meaning by themselves:
</p>

<ul>
  <li>visual layout,</li>
  <li>geometry,</li>
  <li>styling,</li>
  <li>editor metadata,</li>
  <li>cache payloads,</li>
  <li>purely presentational front-panel arrangement,</li>
  <li>connector arrangement when it adds no semantic distinction beyond already-declared interface truth.</li>
</ul>

<p>
A conforming implementation MUST NOT legalize, reject, or reinterpret program meaning merely because one of those support-only families looks suggestive.
</p>

<hr/>

<h2 id="critical-distinctions-preserved">12. Critical Distinctions Preserved</h2>

<p>
Validation at this boundary MUST preserve at least the following distinctions whenever they are relevant:
</p>

<ul>
  <li><code>interface_input</code> versus <code>interface_output</code>,</li>
  <li>public interface versus front-panel presence,</li>
  <li><code>widget_value</code> versus <code>widget_reference</code>,</li>
  <li>ordinary primitive participation versus standardized UI primitive participation,</li>
  <li>explicit structure boundary versus visual grouping,</li>
  <li>explicit structure terminals versus inferred region crossing,</li>
  <li>explicit state versus inferred persistence,</li>
  <li>explicit state initialization versus inferred default value,</li>
  <li>explicit dependency versus inferred evaluation order.</li>
</ul>

<p>
These distinctions are not optional editorial preferences.
They are part of the semantic discipline that prevents source meaning from being laundered into runtime guesswork.
</p>

<hr/>

<h2 id="structures-state-and-cycles">13. Structures, State, and Cycles</h2>

<p>
At this boundary:
</p>

<ul>
  <li>structures MUST be explicit and valid,</li>
  <li>structure-local terminals MUST be explicit where required,</li>
  <li>state MUST be explicit where recurrence depends on state,</li>
  <li>cycles MUST be validated against explicit state legality rather than repaired later by runtime guesswork.</li>
</ul>

<pre><code>loop shape
    !=
state meaning

explicit semantic memory
    -&gt;
valid stateful recurrence
</code></pre>

<p>
A cycle that would only become executable through hidden scheduler repair, hidden buffering, or hidden retained memory MUST NOT be accepted as validated meaning.
</p>

<hr/>

<h2 id="type-value-and-interface-commitments">14. Type, Value, and Interface Commitments</h2>

<p>
Validation at this boundary MUST guarantee, for the accepted program meaning:
</p>

<ul>
  <li>type correctness under the published type rules,</li>
  <li>value compatibility under the published coercion and compatibility rules,</li>
  <li>state legality under the published state rules,</li>
  <li>public interface legality under the published interface rules.</li>
</ul>

<p>
This means that validated meaning already knows:
</p>

<ul>
  <li>which values may flow,</li>
  <li>through which boundaries,</li>
  <li>under which type commitments,</li>
  <li>with which explicit state obligations where relevant.</li>
</ul>

<p>
Later derivation and lowering may normalize or specialize these commitments,
but they MUST NOT invent them after the fact.
</p>

<hr/>

<h2 id="ui-participation-at-the-language-boundary">15. UI Participation at the Language Boundary</h2>

<p>
UI participation is not automatically part of semantic meaning merely because a front panel exists.
It becomes part of validated meaning only where the accepted source actually makes UI participation execution-relevant.
</p>

<p>
Accordingly:
</p>

<ul>
  <li>front-panel presence alone does not define execution meaning,</li>
  <li>a declared public interface does not require front-panel widget existence,</li>
  <li><code>widget_value</code> and <code>widget_reference</code> remain semantically distinct,</li>
  <li>standardized UI-object operations performed through widget references remain semantically distinct from ordinary typed value flow.</li>
</ul>

<p>
This document does not define a fully general event model.
It defines only the validated semantic boundary for UI-related participation that is already expressible and accepted in the current source model.
</p>

<hr/>

<h2 id="attribution-and-recoverability">16. Attribution and Recoverability</h2>

<p>
All execution-relevant accepted meaning MUST remain attributable to accepted source contributors.
That includes, where relevant:
</p>

<ul>
  <li>nodes,</li>
  <li>edges,</li>
  <li>structures and regions,</li>
  <li>state-bearing elements,</li>
  <li>public interface contributors,</li>
  <li>UI participation contributors,</li>
  <li>profile-owned capability contributors.</li>
</ul>

<p>
The purpose of this requirement is not editorial neatness.
It is to preserve later diagnosability, conformance reasoning, source alignment, and IR derivation recoverability.
</p>

<p>
A conforming implementation MUST NOT accept semantic meaning in a way that destroys the ability to relate accepted meaning back to the relevant source contributors.
</p>

<hr/>

<h2 id="forbidden-boundary-collapses">17. Forbidden Boundary Collapses</h2>

<pre><code>loadable source            -/-> validated meaning
structurally valid source  -/-> canonical Execution IR
source                     -/-> private runtime truth
validator                  -/-> language law
front panel                -/-> public interface
connector layout           -/-> public interface meaning
layout                     -/-> execution dependency
runtime policy             -/-> semantics
IDE metadata               -/-> semantics
cache                      -/-> semantics
implementation habit       -/-> accepted program meaning
compiler-family form       -/-> language meaning
</code></pre>

<p>
In addition:
</p>

<ul>
  <li>source acceptance MUST NOT depend on undeclared runtime behavior,</li>
  <li>semantic legality MUST NOT depend on one implementation’s private executor,</li>
  <li>front-panel arrangement MUST NOT silently become interface truth,</li>
  <li>support-only source families MUST NOT silently acquire semantic force by implementation convenience,</li>
  <li>downstream compiler-family expectations MUST NOT redefine the validated meaning boundary.</li>
</ul>

<hr/>

<h2 id="contract-with-ir-derivation">18. Contract with IR Derivation</h2>

<p>
This document guarantees that IR derivation:
</p>

<ul>
  <li>does not need to infer semantic truth from ambiguous source hints,</li>
  <li>does not need to repair missing execution meaning,</li>
  <li>does not need to invent explicit state where none was validated,</li>
  <li>does not need to reinterpret support-only layout content as execution structure,</li>
  <li>only needs to transform validated meaning into the canonical Execution IR Document.</li>
</ul>

<pre><code>this document answers:
"What must be true before the canonical Execution IR Document exists?"

IR/Derivation rules.md answers:
"How does validated meaning become the canonical Execution IR Document?"
</code></pre>

<p>
If a future IR derivation rule appears to require hidden semantic invention,
the defect is upstream and must be fixed in the semantic boundary or in the source model,
not hidden inside derivation.
</p>

<hr/>

<h2 id="out-of-scope">19. Out of Scope</h2>

<ul>
  <li>validator implementation architecture,</li>
  <li>private internal validator data structures,</li>
  <li>Execution IR format,</li>
  <li>IR canonical JSON schema shape,</li>
  <li>lowering strategy,</li>
  <li>backend contract structure,</li>
  <li>runtime-private realization,</li>
  <li>deployment packaging,</li>
  <li>IDE-specific projection logic.</li>
</ul>

<hr/>

<h2 id="summary">20. Summary</h2>

<p>
Validated program meaning is the first semantic truth of a FROG program.
</p>

<p>
It is:
</p>

<ul>
  <li>the result of successful semantic validation over structurally valid source,</li>
  <li>fully explicit for execution-relevant meaning,</li>
  <li>non-ambiguous,</li>
  <li>attributable,</li>
  <li>ready for canonical Execution IR derivation.</li>
</ul>

<pre><code>Expression  -&gt;  Language  -&gt;  IR

source      -&gt;  meaning   -&gt;  execution
</code></pre>

<p>
A conforming system therefore follows this rule:
</p>

<pre><code>no validated meaning
        -&gt;
no conforming canonical Execution IR
</code></pre>
