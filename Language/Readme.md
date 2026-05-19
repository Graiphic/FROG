<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Language</h1>

<p align="center">
  <strong>Normative semantic layer for validated FROG programs</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#architectural-position">2. Architectural Position</a></li>
  <li><a href="#ownership-boundary">3. Ownership Boundary</a></li>
  <li><a href="#document-map">4. Document Map</a></li>
  <li><a href="#current-documents">5. Current Documents</a></li>
  <li><a href="#expression-to-meaning-boundary">6. Expression to Meaning Boundary</a></li>
  <li><a href="#relation-with-expression">7. Relation with Expression</a></li>
  <li><a href="#relation-with-ir">8. Relation with IR</a></li>
  <li><a href="#relation-with-libraries-and-profiles">9. Relation with Libraries and Profiles</a></li>
  <li><a href="#relation-with-implementations">10. Relation with Implementations</a></li>
  <li><a href="#current-repository-posture">11. Current Repository Posture</a></li>
  <li><a href="#summary">12. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
<code>Language/</code> defines what a validated FROG program means.
</p>

<p>
This directory is the normative semantic layer of the FROG repository.
</p>

<p>
It is not the source-format layer.<br/>
It is not the structural-validation layer.<br/>
It is not the execution-IR layer.<br/>
It is not the lowering layer.<br/>
It is not the backend-contract layer.<br/>
It is not the runtime layer.<br/>
It is not the IDE layer.
</p>

<p>
The ownership rule is:
</p>

<pre><code>Expression/  -&gt; what is written and structurally valid as canonical source
Language/    -&gt; what the validated program means
IR/          -&gt; what canonical execution-facing representation is derived from that meaning
</code></pre>

<p>
The role of <code>Language/</code> is to define the semantic truth that exists after structural validation and semantic admission, and before any canonical execution-facing representation is derived.
</p>

<hr/>

<h2 id="architectural-position">2. Architectural Position</h2>

<p>
The FROG repository separates architectural concerns on purpose.
</p>

<p>
The broad chain is:
</p>

<pre><code>canonical .frog source
        |
        v
loadability
        |
        v
structural validation
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
backend contract / backend-oriented handoff
        |
        v
known compiler/runtime backend
        |
        v
deployable artifact
</code></pre>

<p>
<code>Language/</code> owns the semantic stage in that chain:
</p>

<pre><code>validated program meaning
</code></pre>

<p>
This means:
</p>
<ul>
  <li>it is upstream of IR derivation,</li>
  <li>it is downstream of canonical source and structural validation,</li>
  <li>it is the first execution-relevant normative truth layer.</li>
</ul>

<p>
This also means that <code>Language/</code> must not blur the boundary between:
</p>

<ul>
  <li>source-owned structural validity,</li>
  <li>semantic acceptance,</li>
  <li>canonical execution-facing derivation,</li>
  <li>implementation-side realization.</li>
</ul>

<hr/>

<h2 id="ownership-boundary">3. Ownership Boundary</h2>

<p>
<code>Language/</code> owns semantic truth for validated programs.
</p>

<p>
It defines:
</p>
<ul>
  <li>what a validated program means,</li>
  <li>what source-visible control structures mean,</li>
  <li>what explicit state and valid cycles mean,</li>
  <li>what the execution model guarantees at language level,</li>
  <li>what execution control and observation boundaries are safe and meaningful.</li>
</ul>

<p>
<code>Language/</code> does <strong>not</strong> own:
</p>
<ul>
  <li>canonical source serialization,</li>
  <li>source-schema posture or machine-checkable source shape,</li>
  <li>graphical source layout,</li>
  <li>canonical Execution IR structure,</li>
  <li>IR canonical JSON schema posture,</li>
  <li>backend lowering policy,</li>
  <li>backend contract shape,</li>
  <li>runtime-private realization,</li>
  <li>IDE authoring workflow,</li>
  <li>primitive-local intrinsic behavior catalogs,</li>
  <li>optional profile capability catalogs.</li>
</ul>

<p>
That separation matters because the language must remain stable and inspectable even when implementations, runtimes, validators, IR producers, and IDEs evolve independently.
</p>

<hr/>

<h2 id="document-map">4. Document Map</h2>

<p>
The surrounding repository architecture is:
</p>

<pre><code>Expression/  -&gt; source and structural validity
Language/    -&gt; meaning
IR/          -&gt; canonical execution-facing representation derived from meaning
Libraries/   -&gt; intrinsic primitives
Profiles/    -&gt; optional capabilities
IDE/         -&gt; tooling
</code></pre>

<p>
The internal structure of <code>Language/</code> is currently:
</p>

<pre><code>Language/
├── Readme.md
├── Expression to validated meaning.md
├── Control structures.md
├── State and cycles.md
├── Execution model.md
└── Execution control and observation boundaries.md
</code></pre>

<p>
The semantic flow inside this layer is:
</p>

<pre><code>structurally valid canonical source
          |
          v
Expression to validated meaning
          |
          v
Control structures + State and cycles
                  |
                  v
            Execution model
                  |
                  v
Execution control and observation boundaries
                  |
                  v
        canonical IR derivation
</code></pre>

<p>
This is semantic layering, not a compiler pipeline.
The documents in this directory define meaning and semantic boundaries that later layers depend on.
They do not define one mandatory implementation pipeline.
</p>

<p>
<code>Semantic validation before FIR.md</code> makes the pre-FIR validation
gate explicit. It should be read when the question is whether a structurally
valid source file has enough accepted meaning for FIR derivation.
</p>

<hr/>

<h2 id="current-documents">5. Current Documents</h2>

<p>
The documents currently published in this directory are:
</p>

<ul>
  <li><code>Readme.md</code> — architectural entry point for the semantic layer and its ownership boundary.</li>
  <li><code>Expression to validated meaning.md</code> — normative boundary from structurally valid canonical <code>.frog</code> source to validated program meaning.</li>
  <li><code>Control structures.md</code> — normative meaning of canonical control structures such as <code>case</code>, <code>for_loop</code>, and <code>while_loop</code>.</li>
  <li><code>State and cycles.md</code> — normative meaning of explicit local memory and valid feedback cycles.</li>
  <li><code>Execution model.md</code> — language-level execution-model core, including validated program meaning, activation, execution context, semantic milestones, and committed source-level state.</li>
  <li><code>Execution control and observation boundaries.md</code> — safe semantic observation points, pause-consistent snapshots, safe stop boundaries, and minimal completion / fault / abort boundaries.</li>
</ul>

<p>
Together, these documents define the current cross-cutting semantic baseline of the FROG language.
They do not replace primitive-local behavior owned by <code>Libraries/</code> or optional capability behavior owned by <code>Profiles/</code>.
</p>

<hr/>

<h2 id="expression-to-meaning-boundary">6. Expression to Meaning Boundary</h2>

<p>
<code>Expression/</code> and <code>Language/</code> meet at one critical normative boundary:
</p>

<pre><code>structurally valid canonical source
        |
        v
validated program meaning
</code></pre>

<p>
That boundary is owned by <code>Language/Expression to validated meaning.md</code>.
</p>

<p>
Its role is to define:
</p>
<ul>
  <li>what validation must establish before a program has language-level meaning,</li>
  <li>which source-visible families directly contribute to validated execution meaning,</li>
  <li>which source-visible families remain non-semantic or non-authoritative,</li>
  <li>which distinctions must remain recoverable before IR derivation.</li>
</ul>

<p>
This document is downstream of source-owned structural validity and upstream of <code>IR/Derivation rules.md</code>.
</p>

<p>
It therefore prevents several architectural failures:
</p>
<ul>
  <li>source representation being mistaken for semantic truth,</li>
  <li>structural validation being mistaken for semantic acceptance,</li>
  <li>validator behavior becoming hidden language law,</li>
  <li>front-panel composition being mistaken for public interface semantics,</li>
  <li>Execution IR becoming the first semantic authority.</li>
</ul>

<hr/>

<h2 id="relation-with-expression">7. Relation with Expression</h2>

<p>
<code>Expression/</code> defines how a FROG program is serialized and represented in canonical source.
It also defines what counts as structurally valid canonical source.
</p>

<p>
That includes source-visible families such as:
</p>
<ul>
  <li>metadata,</li>
  <li>interface,</li>
  <li>diagram,</li>
  <li>front panel,</li>
  <li>connector,</li>
  <li>icon,</li>
  <li>IDE-facing recoverability and convenience material.</li>
</ul>

<p>
But source representation and structural validity are not yet validated language meaning.
</p>

<p>
The semantic reading rule is therefore:
</p>

<pre><code>Expression/ defines the canonical source shape
Expression/ defines structural validity
Language/   defines what that structurally valid source means
</code></pre>

<p>
The dedicated boundary document <code>Expression to validated meaning.md</code> makes that bridge explicit.
It defines what successful semantic validation establishes as language-level meaning before any canonical execution-facing IR may be derived.
</p>

<hr/>

<h2 id="relation-with-ir">8. Relation with IR</h2>

<p>
<code>IR/</code> is downstream of <code>Language/</code>.
</p>

<p>
The relation is:
</p>

<pre><code>validated program meaning
        |
        v
canonical Execution IR Document
</code></pre>

<p>
This means:
</p>
<ul>
  <li><code>Language/</code> is not an IR document set,</li>
  <li><code>IR/</code> must not redefine semantic truth,</li>
  <li>IR derivation must preserve the distinctions established by validated program meaning.</li>
</ul>

<p>
More precisely:
</p>
<ul>
  <li><code>Language/</code> defines the semantic truth of validated FROG programs,</li>
  <li><code>IR/</code> defines the canonical execution-facing representation derived from that truth,</li>
  <li><code>IR/</code> may make execution-facing consequences more explicit,</li>
  <li><code>IR/</code> may not become the first owner of meaning,</li>
  <li><code>IR/</code> may not collapse into one downstream compiler-family form.</li>
</ul>

<p>
The repository intentionally prevents this collapse:
</p>

<pre><code>source               != semantics
structural validity  != semantic acceptance
semantics            != IR
IR                   != lowering
lowering             != backend contract
backend contract     != runtime-private representation
</code></pre>

<p>
That separation is required for inspectability, portability, conformance, and independent implementation.
</p>

<hr/>

<h2 id="relation-with-libraries-and-profiles">9. Relation with Libraries and Profiles</h2>

<p>
<code>Language/</code> does not replace primitive-local intrinsic behavior defined in <code>Libraries/</code>.
</p>

<p>
It also does not replace optional standardized capability behavior defined in <code>Profiles/</code>.
</p>

<p>
The ownership rule is:
</p>

<pre><code>Libraries/  -&gt; intrinsic primitive behavior
Profiles/   -&gt; optional standardized capability behavior
Language/   -&gt; cross-cutting semantic meaning of validated programs
</code></pre>

<p>
So:
</p>
<ul>
  <li>primitive catalogs stay where they belong,</li>
  <li>profile capability rules stay where they belong,</li>
  <li><code>Language/</code> defines the semantic layer that composes with them.</li>
</ul>

<p>
A program does not gain validated meaning from source shape alone.
It gains validated meaning from semantic interpretation over structurally valid source together with the relevant language, library, and profile rules.
</p>

<hr/>

<h2 id="relation-with-implementations">10. Relation with Implementations</h2>

<p>
Implementations may load, structurally validate, semantically validate, derive, lower, compile, execute, observe, and debug FROG programs.
</p>

<p>
But implementations do not own language truth.
</p>

<p>
The authority flow remains:
</p>

<pre><code>repository law
      |
      v
conforming implementation behavior
</code></pre>

<p>
Not the reverse.
</p>

<p>
Therefore:
</p>
<ul>
  <li>a structural validator does not become hidden semantic law,</li>
  <li>a semantic validator does not become hidden repository ownership,</li>
  <li>a runtime does not define the meaning of the language,</li>
  <li>a reference implementation remains non-normative even when executable,</li>
  <li>implementation convenience must not silently redefine semantics,</li>
  <li>a downstream compiler-family path such as LLVM must not become the definition of FROG meaning.</li>
</ul>

<hr/>

<h2 id="current-repository-posture">11. Current Repository Posture</h2>

<p>
The current repository posture is intentionally staged.
</p>

<p>
At this stage:
</p>
<ul>
  <li>canonical source structure and source-schema posture are already separated into <code>Expression/</code>,</li>
  <li>structural validity is already treated explicitly as a source-owned stage,</li>
  <li>semantic meaning is separated into <code>Language/</code>,</li>
  <li>canonical execution-facing derived representation is separated into <code>IR/</code>,</li>
  <li>the repository already supports public examples, conformance material, and a non-normative reference implementation path.</li>
</ul>

<p>
This means <code>Language/</code> must stay disciplined:
</p>
<ul>
  <li>it must not absorb source serialization concerns,</li>
  <li>it must not absorb source-schema posture,</li>
  <li>it must not absorb IR wire-shape concerns,</li>
  <li>it must not absorb implementation-private policy,</li>
  <li>it must make semantic truth explicit enough that later layers stay honest.</li>
</ul>

<p>
More specifically, the semantic layer must remain strong enough that:
</p>
<ul>
  <li>the canonical Execution IR Document can be derived without becoming the first owner of meaning,</li>
  <li>lowering can specialize without laundering semantics,</li>
  <li>backend contracts can declare assumptions without redefining language truth,</li>
  <li>downstream compiler families can consume FROG without replacing it.</li>
</ul>

<p>
The semantic layer should therefore remain compact, explicit, and durable.
</p>

<hr/>

<h2 id="summary">12. Summary</h2>

<p>
<code>Language/</code> defines what a validated FROG program means.
</p>

<p>
It sits:
</p>
<ul>
  <li>after canonical source and structural validation,</li>
  <li>before canonical execution-facing IR,</li>
  <li>before lowering, backend contract, runtime realization, and IDE workflow concerns.</li>
</ul>

<p>
The essential reading rule remains:
</p>

<pre><code>Expression/  -&gt; source and structural validity
Language/    -&gt; meaning
IR/          -&gt; canonical execution-facing representation
</code></pre>

<p>
That separation is required so that FROG can remain:
</p>
<ul>
  <li>inspectable,</li>
  <li>portable,</li>
  <li>conformant,</li>
  <li>independently implementable,</li>
  <li>architecturally durable as an open graphical language.</li>
</ul>
