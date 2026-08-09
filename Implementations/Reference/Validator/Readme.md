<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 Reference Validator</h1>

<p align="center">
  Validation stage for the non-normative FROG reference implementation<br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#why-this-directory-exists">2. Why This Directory Exists</a></li>
  <li><a href="#non-normative-status">3. Non-Normative Status</a></li>
  <li><a href="#what-this-directory-consumes">4. What This Directory Consumes</a></li>
  <li><a href="#validation-staging">5. Validation Staging</a></li>
  <li><a href="#scope-of-the-current-reference-slice">6. Scope of the Current Reference Slice</a></li>
  <li><a href="#what-this-directory-owns">7. What This Directory Owns</a></li>
  <li><a href="#what-this-directory-does-not-own">8. What This Directory Does Not Own</a></li>
  <li><a href="#diagnostic-posture">9. Diagnostic Posture</a></li>
  <li><a href="#design-rules">10. Design Rules</a></li>
  <li><a href="#relation-with-conformance">11. Relation with Conformance</a></li>
  <li><a href="#relation-with-derivation-and-runtime">12. Relation with Derivation and Runtime</a></li>
  <li><a href="#summary">13. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This directory contains validator logic for the non-normative FROG reference implementation.
Its role is to determine, for the supported reference slice, whether loaded canonical source may proceed from source acceptance toward validated program meaning and then toward downstream derivation.
</p>

<p>
This validator is an implementation-side consumer of published repository law.
It is not the owner of that law.
</p>

<p>
Accordingly, this directory exists to apply published validation boundaries, not to redefine them.
</p>

<hr/>

<h2 id="why-this-directory-exists">2. Why This Directory Exists</h2>

<p>
The reference implementation needs an explicit validation stage between loading canonical source and producing any downstream derived form.
</p>

<p>
Without that stage, an implementation could too easily collapse:
</p>

<pre><code>loadable source
    !=
structurally valid canonical source

structurally valid canonical source
    !=
validated meaning

validated meaning
    !=
derived execution-facing form
</code></pre>

<p>
The reference validator exists to keep those stages explicit in the implementation workspace.
</p>

<p>
It also helps ensure that the reference path remains inspectable, reproducible, and aligned with the published repository structure.
</p>

<hr/>

<h2 id="non-normative-status">3. Non-Normative Status</h2>

<p>
This validator is part of <code>Implementations/Reference/</code>.
It is therefore non-normative.
</p>

<p>
That means:
</p>

<ul>
  <li>it does not define the language,</li>
  <li>it does not define source-schema ownership,</li>
  <li>it does not define semantic truth,</li>
  <li>it does not define Execution IR,</li>
  <li>it does not define backend contracts,</li>
  <li>it does not define runtime-private realization.</li>
</ul>

<p>
If validator behavior and the published specification disagree, the specification wins.
If the disagreement reveals ambiguity, the fix belongs in the relevant specification owner, not in private implementation folklore.
</p>

<hr/>

<h2 id="what-this-directory-consumes">4. What This Directory Consumes</h2>

<p>
This directory consumes the published repository boundaries, including:
</p>

<ul>
  <li><code>Expression/</code> for canonical source structure, structural validity, and source-schema posture,</li>
  <li><code>Language/</code> for validated program meaning,</li>
  <li><code>Libraries/</code> for intrinsic primitive identity and primitive-local requirements,</li>
  <li><code>Profiles/</code> where optional standardized capability families affect legality,</li>
  <li><code>Conformance/</code> for public accept / reject / preserve expectations relevant to the supported slice.</li>
</ul>

<p>
A machine-checkable schema artifact MAY assist the structural part of validation.
However, such an artifact remains downstream from the published specification and upstream from semantic validation.
</p>

<p>
The validator therefore consumes published law in staged form.
It does not invent a replacement source of truth.
</p>

<hr/>

<h2 id="validation-staging">5. Validation Staging</h2>

<p>
The reference validator should preserve the staged corridor explicitly:
</p>

<pre><code>.frog source
    -&gt; loadability
    -&gt; source-shape / structural validation
    -&gt; semantic validation
    -&gt; candidate for downstream derivation
</code></pre>

<p>
These stages must not be collapsed.
</p>

<h3>5.1 Loadability</h3>

<p>
This stage answers whether the source can be loaded as canonical JSON-based input at all.
A failure here is not yet a structural-validity decision.
</p>

<h3>5.2 Source-shape / structural validation</h3>

<p>
This stage answers whether the loaded source satisfies the published structural rules of canonical FROG source for the relevant slice.
</p>

<p>
This includes top-level source shape, required versus optional section presence where relevant, and structurally required source objects for the supported slice.
</p>

<p>
A machine-checkable schema artifact may cover part of this stage.
The validator may also contain additional structural checks where the published source contract requires them and a minimal declarative schema artifact is not sufficient on its own.
</p>

<h3>5.3 Semantic validation</h3>

<p>
This stage answers whether meaning is established for the supported slice after structural acceptance has already succeeded.
</p>

<p>
Typical semantic decisions include primitive legality, role legality, cycle legality, state legality, and other meaning-bearing rules that do not belong purely to source shape.
</p>

<h3>5.4 Candidate for downstream derivation</h3>

<p>
Only after structural and semantic validation have succeeded may the reference pipeline treat the program as a legitimate candidate for downstream Execution IR derivation or equivalent downstream reference processing.
</p>

<hr/>

<h2 id="scope-of-the-current-reference-slice">6. Scope of the Current Reference Slice</h2>

<p>
The current reference validator should concentrate on the MVP slice exercised by the initial published examples and relevant conformance cases.
</p>

<p>
That slice includes, as applicable:
</p>

<ul>
  <li>public interface participation,</li>
  <li>ordinary core primitives,</li>
  <li><code>widget_value</code>,</li>
  <li><code>widget_reference</code>,</li>
  <li>standardized UI primitives,</li>
  <li>explicit local memory through <code>frog.core.delay</code>,</li>
  <li>basic cycle legality,</li>
  <li>clear rejection of role confusion and structurally or semantically invalid participation.</li>
</ul>

<p>
The validator should remain narrow enough that acceptance and rejection can be explained clearly for the supported slice.
</p>

<hr/>

<h2 id="what-this-directory-owns">7. What This Directory Owns</h2>

<p>
This directory owns implementation-side validation behavior for the supported reference slice, including:
</p>

<ul>
  <li>implementation-side checking of published validation rules,</li>
  <li>staged validation flow inside the reference implementation,</li>
  <li>explicit acceptance or rejection of source cases for the supported slice,</li>
  <li>source-aligned diagnostics,</li>
  <li>enough source attribution to support downstream traceability if validation succeeds.</li>
</ul>

<p>
It may also own internal decomposition such as:
</p>

<ul>
  <li>loadability checks,</li>
  <li>structural checks,</li>
  <li>semantic checks,</li>
  <li>diagnostic classification.</li>
</ul>

<hr/>

<h2 id="what-this-directory-does-not-own">8. What This Directory Does Not Own</h2>

<p>
This directory does not own:
</p>

<ul>
  <li>the normative validation rules themselves,</li>
  <li>the canonical definition of source schema,</li>
  <li>language-semantics ownership,</li>
  <li>primitive-catalog ownership,</li>
  <li>Execution IR design ownership,</li>
  <li>lowering ownership,</li>
  <li>backend-contract ownership,</li>
  <li>runtime-private scheduling truth.</li>
</ul>

<p>
It must not silently shift ownership from the published repository into implementation code.
</p>

<p>
In particular, this validator must not:
</p>

<ul>
  <li>turn one machine-checkable schema artifact into the full definition of the language,</li>
  <li>treat private implementation repair as public structural acceptance,</li>
  <li>treat an implementation subset limitation as if the published specification had rejected the source,</li>
  <li>treat successful execution as proof that the source was valid.</li>
</ul>

<hr/>

<h2 id="diagnostic-posture">9. Diagnostic Posture</h2>

<p>
Diagnostics should preserve the stage at which failure occurred.
</p>

<p>
At minimum, the reference validator SHOULD distinguish:
</p>

<ul>
  <li>loadability failure,</li>
  <li>structural validation failure,</li>
  <li>semantic validation failure,</li>
  <li>supported-by-specification but not yet supported-by-reference-slice situations where applicable.</li>
</ul>

<p>
This distinction matters because:
</p>

<pre><code>"not loadable"
    !=
"structurally invalid"

"structurally invalid"
    !=
"semantically invalid"

"not yet supported by this reference slice"
    !=
"rejected by the language"
</code></pre>

<p>
Diagnostics should remain source-aligned and should preserve enough attribution to explain which source-side construct caused acceptance or rejection.
</p>

<hr/>

<h2 id="design-rules">10. Design Rules</h2>

<ul>
  <li>Reject invalid source explicitly.</li>
  <li>Do not silently reinterpret invalid source.</li>
  <li>Do not silently repair invalid source into a private acceptable form.</li>
  <li>Keep structural validation distinct from semantic validation.</li>
  <li>Preserve enough source identity for later attribution.</li>
  <li>Keep the supported slice narrow and coherent.</li>
  <li>Keep downstream derivation unavailable until validation has succeeded.</li>
  <li>Reject non-empty <code>diagram.wire_fragments</code> as incomplete authoring state before downstream derivation.</li>
  <li>Keep implementation convenience downstream from published repository law.</li>
</ul>

<hr/>

<h2 id="relation-with-conformance">11. Relation with Conformance</h2>

<p>
This validator is not the conformance surface.
It is one implementation attempting to align with that surface.
</p>

<pre><code>Specification defines
Conformance exposes
Reference validator consumes and applies
</code></pre>

<p>
Accordingly:
</p>

<ul>
  <li>published conformance cases define public expectations,</li>
  <li>the reference validator should classify those cases correctly for the supported slice,</li>
  <li>passing a case demonstrates alignment, not ownership.</li>
</ul>

<p>
If a conformance case cannot be classified clearly, that usually indicates one of three problems:
</p>

<ul>
  <li>the validator is incomplete,</li>
  <li>the supported reference slice is too vague,</li>
  <li>the owning specification document needs clarification.</li>
</ul>

<hr/>

<h2 id="relation-with-derivation-and-runtime">12. Relation with Derivation and Runtime</h2>

<p>
The validator is upstream from derivation and downstream from loading.
</p>

<pre><code>Loader
   -&gt; Validator
   -&gt; Deriver
   -&gt; Lowerer
   -&gt; ContractEmitter
   -&gt; Runtime-side consumption
</code></pre>

<p>
The validator does not own the shape of the derived execution-facing representation.
It only decides whether the source is an acceptable candidate to reach that stage for the supported slice.
</p>

<p>
Likewise, the validator does not own runtime realization.
It must not let runtime-private behavior redefine validation truth.
</p>

<hr/>

<h2 id="summary">13. Summary</h2>

<p>
The reference validator is the stage that classifies loaded canonical source for the non-normative reference implementation.
</p>

<p>
Its job is to preserve the staged corridor:
</p>

<pre><code>loadability
   -&gt; structural validation
   -&gt; semantic validation
   -&gt; candidate for downstream derivation
</code></pre>

<p>
Without successful validation, the reference implementation must not claim that a case is a legitimate candidate for derived execution-facing form.
</p>

<p>
Core rule:
</p>

<pre><code>published law first
reference validation second
downstream derivation only after acceptance
</code></pre>
