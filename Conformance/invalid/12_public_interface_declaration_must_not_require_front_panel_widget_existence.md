<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1>FROG Conformance</h1>

<p><strong>Public accept / reject / preserve truth surface for the published FROG specification</strong><br>
FROG — Free Open Graphical Language</p>

<hr>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#why-this-directory-exists">2. Why this Directory Exists</a></li>
  <li><a href="#what-conformance-means-here">3. What Conformance Means Here</a></li>
  <li><a href="#what-this-directory-does-not-do">4. What this Directory Does Not Do</a></li>
  <li><a href="#relation-with-specification-ownership">5. Relation with Specification Ownership</a></li>
  <li><a href="#relation-with-the-expression-to-meaning-boundary">6. Relation with the Expression-to-Meaning Boundary</a></li>
  <li><a href="#directory-structure">7. Directory Structure</a></li>
  <li><a href="#current-published-cases">8. Current Published Cases</a></li>
  <li><a href="#expected-outcomes">9. Expected Outcomes</a></li>
  <li><a href="#active-v01-conformance-focus">10. Active v0.1 Conformance Focus</a></li>
  <li><a href="#relation-with-examples-and-reference-implementation">11. Relation with Examples and Reference Implementation</a></li>
  <li><a href="#future-growth">12. Future Growth</a></li>
  <li><a href="#summary">13. Summary</a></li>
</ul>

<hr>

<h2 id="overview">1. Overview</h2>

<p>This directory contains public conformance material for the published FROG specification.</p>

<p>Its role is to make expected outcomes explicit for named cases such as:</p>
<ul>
  <li>what a conforming toolchain should accept,</li>
  <li>what a conforming toolchain should reject,</li>
  <li>what distinctions a conforming derivation must preserve,</li>
  <li>what assumptions a later lowering or backend handoff may specialize,</li>
  <li>what must remain explicit rather than being silently reinterpreted.</li>
</ul>

<p>Conformance exists because a specification repository should not rely only on prose. It should also publish explicit public cases that validators, semantic analyzers, derivation pipelines, lowerers, executors, and future independent implementations can check against.</p>

<p>The repository-level model is:</p>

<pre><code>specification text
        +
named conformance cases
        |
        v
public truth surface
</code></pre>

<p>A conforming implementation may vary in internal architecture.<br>
It may not vary in what published conformance cases mean.</p>

<hr>

<h2 id="why-this-directory-exists">2. Why this Directory Exists</h2>

<p>FROG is intentionally organized as a specification-first repository rather than as a single product implementation.</p>

<p>That separation is valuable, but it creates a practical need:</p>

<pre><code>different future implementations
            need
one public way to check whether they stay aligned
</code></pre>

<p>This directory exists to provide that shared starting point.</p>

<p>At minimum, conformance material should help answer questions such as:</p>
<ul>
  <li>Should this program validate or fail?</li>
  <li>If it validates, which distinctions must remain explicit after semantic validation and derivation?</li>
  <li>If it is lowered and emitted as a backend contract, which assumptions may be fixed and which may not be silently invented?</li>
  <li>If a feature is unsupported by one backend family, should it be rejected rather than reinterpreted?</li>
</ul>

<hr>

<h2 id="what-conformance-means-here">3. What Conformance Means Here</h2>

<p>In this directory, conformance means alignment with the published FROG specification layers and their ownership boundaries.</p>

<p>It includes at least:</p>
<ul>
  <li>source validation conformance,</li>
  <li>semantic validation conformance,</li>
  <li>semantic preservation conformance,</li>
  <li>derivation-boundary conformance,</li>
  <li>backend-contract conformance,</li>
  <li>rejection conformance for invalid or unsupported cases.</li>
</ul>

<p>This is intentionally broader than just “does the tool run the program?”</p>

<p>A tool can run something incorrectly.<br>
A tool can validate something incorrectly.<br>
A tool can derive something executable while still violating published distinctions.</p>

<p>Conformance must therefore include both:</p>
<ul>
  <li>acceptance / rejection correctness, and</li>
  <li>preservation correctness.</li>
</ul>

<hr>

<h2 id="what-this-directory-does-not-do">4. What this Directory Does Not Do</h2>

<p>This directory is not:</p>
<ul>
  <li>a complete certification program,</li>
  <li>a frozen cross-vendor compatibility matrix,</li>
  <li>a universal deployment test suite,</li>
  <li>a replacement for the normative specification documents,</li>
  <li>a substitute for implementation debugging logs,</li>
  <li>a place where new language law is invented informally.</li>
</ul>

<p>In base v0.1, this directory is the beginning of a conformance story. Its job is to make the first expectations explicit, reviewable, and durable.</p>

<hr>

<h2 id="relation-with-specification-ownership">5. Relation with Specification Ownership</h2>

<p>This directory does not own language truth.</p>

<p>Ownership remains in the published specification layers:</p>
<ul>
  <li><code>Expression/</code> owns canonical source structure,</li>
  <li><code>Language/</code> owns validated semantic truth,</li>
  <li><code>Libraries/</code> owns intrinsic primitive definitions,</li>
  <li><code>Profiles/</code> owns optional standardized capability families,</li>
  <li><code>IR/</code> owns execution-facing representation, derivation, construction, identity, lowering, and backend-hand-off boundaries,</li>
  <li><code>IDE/</code> owns authoring-facing and observability-facing tooling concerns.</li>
</ul>

<p>Conformance material should therefore always point back to those owners instead of silently inventing new rules.</p>

<p>The authority rule is:</p>

<pre><code>specification owners
        |
        v
conformance cases
        |
        v
implementation checks
</code></pre>

<p>Not the reverse.</p>

<hr>

<h2 id="relation-with-the-expression-to-meaning-boundary">6. Relation with the Expression-to-Meaning Boundary</h2>

<p>A critical conformance concern is the normative boundary:</p>

<pre><code>canonical .frog source
        |
        v
validated program meaning
</code></pre>

<p>That boundary is owned by <code>Language/Expression to validated meaning.md</code>.</p>

<p>Conformance material must make that boundary testable in public terms.</p>

<p>In practice, this means conformance cases should increasingly express whether:</p>
<ul>
  <li>a source program validly establishes language-level meaning,</li>
  <li>a source program fails before semantic meaning exists,</li>
  <li>source-visible distinctions remain recoverable after validation,</li>
  <li>execution-facing derivation preserves the distinctions that validated meaning established.</li>
</ul>

<p>This boundary is especially important for cases involving:</p>
<ul>
  <li><code>interface_input</code> / <code>interface_output</code>,</li>
  <li><code>widget_value</code> / <code>widget_reference</code>,</li>
  <li>front-panel participation versus public interface contract,</li>
  <li>explicit state versus illegal feedback,</li>
  <li>source-visible structure legality versus execution convenience.</li>
</ul>

<p>Conformance should therefore help prevent these forbidden collapses:</p>

<pre><code>source            -/-> semantic authority drift
validator         -/-> hidden language law
front panel       -/-> public interface
widget_value      -/-> widget_reference
illegal cycle     -/-> implicit memory
runtime shortcut  -/-> semantic truth
</code></pre>

<hr>

<h2 id="directory-structure">7. Directory Structure</h2>

<p>The current minimal structure is:</p>

<pre><code>Conformance/
├── Readme.md
├── valid/
└── invalid/
</code></pre>

<p>Each conformance case should normally state:</p>
<ul>
  <li>the case name,</li>
  <li>whether the case is expected to validate or fail,</li>
  <li>which specification boundaries are being exercised,</li>
  <li>which preservation obligations matter if the case is valid,</li>
  <li>which rejection reason matters if the case is invalid.</li>
</ul>

<p>A compact case structure is usually enough:</p>

<pre><code>Case name
Expected: valid | invalid
Why:
Boundaries exercised:
Expected preservation:
Expected rejection:
Notes:
</code></pre>

<hr>

<h2 id="current-published-cases">8. Current Published Cases</h2>

<h3>8.1 Valid cases</h3>

<ul>
  <li><code>valid/01_pure_addition.md</code></li>
  <li><code>valid/02_ui_value_roundtrip.md</code></li>
  <li><code>valid/03_ui_property_write.md</code></li>
  <li><code>valid/04_stateful_feedback_delay.md</code></li>
  <li><code>valid/05_public_interface_and_widget_participation_distinct.md</code></li>
  <li><code>valid/07_widget_reference_remains_distinct_from_widget_value.md</code></li>
  <li><code>valid/09_front_panel_presence_does_not_by_itself_define_execution_meaning.md</code></li>
  <li><code>valid/11_public_interface_declaration_does_not_require_front_panel_widget_existence.md</code></li>
</ul>

<h3>8.2 Invalid cases</h3>

<ul>
  <li><code>invalid/illegal_feedback_without_explicit_memory.md</code></li>
  <li><code>invalid/interface_widget_role_confusion.md</code></li>
  <li><code>invalid/ui_reference_without_ui_primitive.md</code></li>
  <li><code>invalid/06_widget_must_not_be_promoted_to_public_interface.md</code></li>
  <li><code>invalid/08_widget_reference_must_not_be_treated_as_widget_value.md</code></li>
  <li><code>invalid/10_front_panel_presence_must_not_be_treated_as_execution_meaning.md</code></li>
  <li><code>invalid/12_public_interface_declaration_must_not_require_front_panel_widget_existence.md</code></li>
</ul>

<p>The published set already shows the intended balance:</p>

<pre><code>valid cases   -&gt; acceptance truth
invalid cases -&gt; rejection truth
both together -&gt; public boundary truth
</code></pre>

<p>The pair formed by:</p>
<ul>
  <li><code>valid/05_public_interface_and_widget_participation_distinct.md</code>, and</li>
  <li><code>invalid/06_widget_must_not_be_promoted_to_public_interface.md</code></li>
</ul>

<p>makes one boundary especially explicit:</p>

<pre><code>public interface participation
            !=
widget-owned value participation
</code></pre>

<p>The pair formed by:</p>
<ul>
  <li><code>valid/07_widget_reference_remains_distinct_from_widget_value.md</code>, and</li>
  <li><code>invalid/08_widget_reference_must_not_be_treated_as_widget_value.md</code></li>
</ul>

<p>makes another boundary especially explicit:</p>

<pre><code>widget_reference
        !=
widget_value
</code></pre>

<p>The pair formed by:</p>
<ul>
  <li><code>valid/09_front_panel_presence_does_not_by_itself_define_execution_meaning.md</code>, and</li>
  <li><code>invalid/10_front_panel_presence_must_not_be_treated_as_execution_meaning.md</code></li>
</ul>

<p>makes another boundary especially explicit:</p>

<pre><code>front-panel presence
        !=
execution meaning
</code></pre>

<p>The pair formed by:</p>
<ul>
  <li><code>valid/11_public_interface_declaration_does_not_require_front_panel_widget_existence.md</code>, and</li>
  <li><code>invalid/12_public_interface_declaration_must_not_require_front_panel_widget_existence.md</code></li>
</ul>

<p>makes another boundary especially explicit:</p>

<pre><code>public interface declaration
        !=
required front-panel widget existence
</code></pre>

<p>This balance matters because explicit rejection is better than silent semantic laundering.</p>

<hr>

<h2 id="expected-outcomes">9. Expected Outcomes</h2>

<p>Conformance cases in this directory should express outcomes in a structured way.</p>

<p>At minimum, a case should make clear whether:</p>
<ul>
  <li>validation is expected to pass or fail,</li>
  <li>semantic meaning is established or not established,</li>
  <li>derivation is allowed or forbidden,</li>
  <li>specific distinctions must remain recoverable,</li>
  <li>specific backend-family assumptions may be declared,</li>
  <li>specific unsupported or invalid situations must cause explicit rejection.</li>
</ul>

<p>Examples of outcome language:</p>
<ul>
  <li><strong>Expected:</strong> valid</li>
  <li><strong>Expected:</strong> invalid</li>
  <li><strong>Expected meaning:</strong> language-level meaning established</li>
  <li><strong>Expected meaning:</strong> language-level meaning not established</li>
  <li><strong>Expected preservation:</strong> explicit local memory remains explicit</li>
  <li><strong>Expected preservation:</strong> <code>widget_value</code> and <code>widget_reference</code> remain distinct</li>
  <li><strong>Expected preservation:</strong> interface contract remains distinct from widget participation</li>
  <li><strong>Expected rejection:</strong> illegal feedback without explicit local memory</li>
  <li><strong>Expected rejection:</strong> UI reference usage without a valid UI primitive context</li>
  <li><strong>Expected rejection:</strong> widget-owned value participation must not be promoted to public interface participation</li>
  <li><strong>Expected rejection:</strong> widget-reference participation must not be treated as widget-value participation</li>
  <li><strong>Expected rejection:</strong> front-panel presence must not be treated as execution meaning</li>
  <li><strong>Expected rejection:</strong> public interface declaration must not require front-panel widget existence</li>
</ul>

<hr>

<h2 id="active-v01-conformance-focus">10. Active v0.1 Conformance Focus</h2>

<p>The active base-v0.1 conformance focus should remain small and architectural.</p>

<p>At this stage, the priority is not broad feature coverage. The priority is boundary correctness.</p>

<p>That means concentrating on cases that clarify:</p>
<ul>
  <li>what validates and what fails,</li>
  <li>what establishes semantic meaning and what does not,</li>
  <li>what distinctions later layers must still preserve,</li>
  <li>what must be rejected rather than silently repaired or reinterpreted.</li>
</ul>

<p>The practical v0.1 focus can be summarized as:</p>

<pre><code>1. source validity
2. semantic validity
3. preservation of key distinctions
4. explicit rejection of architectural violations
</code></pre>

<p>In particular, the following distinctions should remain visible in public conformance material:</p>
<ul>
  <li><code>interface_input</code> versus <code>widget_value</code>,</li>
  <li><code>widget_value</code> versus <code>widget_reference</code>,</li>
  <li>legal explicit state versus illegal implicit state,</li>
  <li>front-panel composition versus executable meaning,</li>
  <li>validated meaning versus derived IR convenience.</li>
</ul>

<p>The current mirrored pairs sharpen four especially important rules:</p>

<pre><code>widget declaration
    does not create
public interface declaration

widget_reference
    does not become
widget_value

front-panel presence
    does not create
execution meaning

public interface declaration
    does not require
front-panel widget existence
</code></pre>

<hr>

<h2 id="relation-with-examples-and-reference-implementation">11. Relation with Examples and Reference Implementation</h2>

<p><code>Examples/</code>, <code>Conformance/</code>, and <code>Implementations/Reference/</code> have different roles.</p>

<p>The distinction is:</p>

<pre><code>Examples/                   -&gt; illustrative executable or inspectable slices
Conformance/                -&gt; public accept / reject / preserve expectations
Implementations/Reference/  -&gt; non-normative executable workspace
</code></pre>

<p>An example may demonstrate a pattern.<br>
A conformance case states what must be accepted, rejected, or preserved.<br>
A reference implementation may execute a case, but it does not become language law by doing so.</p>

<p>This distinction must remain explicit.</p>

<hr>

<h2 id="future-growth">12. Future Growth</h2>

<p>This directory should grow gradually, not by uncontrolled accumulation.</p>

<p>Future growth should prefer small, named, reviewable cases that each exercise one clear boundary or one tight cluster of related obligations.</p>

<p>Useful future directions include:</p>
<ul>
  <li>more source-to-meaning boundary cases,</li>
  <li>more preservation cases across derivation,</li>
  <li>more structure legality cases,</li>
  <li>more type / value / state legality cases,</li>
  <li>profile-gated acceptance and rejection cases,</li>
  <li>backend-family rejection cases where silent reinterpretation would be wrong,</li>
  <li>mirrored valid/invalid pairs for each critical boundary.</li>
</ul>

<p>Growth should remain architecture-led:</p>

<pre><code>small case
   -&gt; clear expectation
   -&gt; clear owning specification layer
   -&gt; clear public reviewability
</code></pre>

<hr>

<h2 id="summary">13. Summary</h2>

<p>This directory is the public conformance surface of the FROG repository.</p>

<p>Its job is to make visible and durable:</p>
<ul>
  <li>what a conforming implementation should accept,</li>
  <li>what a conforming implementation should reject,</li>
  <li>what distinctions later layers must preserve,</li>
  <li>what must remain explicit rather than being silently laundered by tooling.</li>
</ul>

<p>The essential rule remains:</p>

<pre><code>specification owns truth
conformance publishes expectations
implementations must align
</code></pre>

<p>As the repository matures, this directory should progressively become the public truth surface for:</p>
<ul>
  <li>acceptance,</li>
  <li>rejection,</li>
  <li>preservation,</li>
  <li>boundary discipline.</li>
</ul>
