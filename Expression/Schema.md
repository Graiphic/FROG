<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Source Schema</h1>

<p align="center">
  <strong>Schema posture and machine-checkable structural validation for canonical <code>.frog</code> source</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#why-this-document-exists">2. Why This Document Exists</a></li>
  <li><a href="#ownership-boundary">3. Ownership Boundary</a></li>
  <li><a href="#what-schema-means-in-frog">4. What “Schema” Means in FROG</a></li>
  <li><a href="#structural-validation-boundary">5. Structural Validation Boundary</a></li>
  <li><a href="#non-goals">6. Non-Goals</a></li>
  <li><a href="#schema-layers">7. Schema Layers</a></li>
  <li><a href="#top-level-canonical-source-shape">8. Top-Level Canonical Source Shape</a></li>
  <li><a href="#section-level-schema-ownership">9. Section-Level Schema Ownership</a></li>
  <li><a href="#cross-cutting-source-subsystems">10. Cross-Cutting Source Subsystems</a></li>
  <li><a href="#machine-checkable-schema-posture">11. Machine-Checkable Schema Posture</a></li>
  <li><a href="#schema-vs-semantic-validation">12. Schema vs Semantic Validation</a></li>
  <li><a href="#schema-vs-ir-vs-runtime">13. Schema vs IR vs Runtime</a></li>
  <li><a href="#validator-posture">14. Validator Posture</a></li>
  <li><a href="#relation-with-conformance">15. Relation with Conformance</a></li>
  <li><a href="#representation-and-evolution-rules">16. Representation and Evolution Rules</a></li>
  <li><a href="#minimum-guarantees">17. Minimum Guarantees</a></li>
  <li><a href="#reading-rule">18. Reading Rule</a></li>
  <li><a href="#status">19. Status</a></li>
  <li><a href="#license">20. License</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the schema posture of canonical FROG source. It specifies how machine-checkable structural validation relates to the published FROG Expression specification.
</p>

<p>
In FROG, a schema is not the whole language. A schema is the machine-checkable structural contract of canonical <code>.frog</code> source. It exists so that source-shape validation becomes explicit, reproducible, implementation-comparable, and clearly separated from semantic validation, IR derivation, lowering, backend-facing handoff, and private runtime realization.
</p>

<p>
This document therefore sits at the following boundary:
</p>

<pre><code>canonical source prose
        +
machine-checkable structural rules
        -
validated meaning
        -
Execution IR
        -
lowering and backend contract
        -
runtime-private realization
</code></pre>

<p>
This is a source-ownership document. It defines what schema means in FROG, what schema is allowed to own, what schema must not own, and how machine-checkable source validation must remain disciplined as the repository grows.
</p>

<p>
Cross-version policy is governed centrally in <code>Versioning/Readme.md</code>. In particular, schema interpretation of <code>spec_version</code> should remain compatible with the repository-wide cumulative version model, under which later source-format versions should normally extend earlier valid forms rather than silently replace them unless an explicit breaking boundary is declared.
</p>

<hr/>

<h2 id="why-this-document-exists">2. Why This Document Exists</h2>

<p>
The repository already defines canonical source structure in <code>Expression/</code>. However, once disciplined validators, conformance growth, and repeatable reference execution matter, structural validity must not remain prose-only or validator-folklore-only.
</p>

<p>
This document exists so that:
</p>

<ul>
  <li>machine-checkable structural validation has a clear ownership boundary,</li>
  <li>schema growth remains subordinate to published source ownership,</li>
  <li>semantic and execution-facing rules do not silently drift upstream into source-shape validation,</li>
  <li>implementations can align on structural acceptance and rejection without treating one validator as hidden law.</li>
</ul>

<p>
This document is therefore about discipline, not maximal declarative coverage.
</p>

<hr/>

<h2 id="ownership-boundary">3. Ownership Boundary</h2>

<p>
This document owns:
</p>

<ul>
  <li>the schema posture of canonical source,</li>
  <li>the structural validation boundary between loadability and semantic validation,</li>
  <li>the relation between published prose ownership and machine-checkable artifacts,</li>
  <li>the rules for conservative schema growth.</li>
</ul>

<p>
This document does not own:
</p>

<ul>
  <li>the complete source model itself,</li>
  <li>the complete semantic meaning of source constructs,</li>
  <li>IR construction,</li>
  <li>lowering,</li>
  <li>backend contract content,</li>
  <li>runtime-private realization,</li>
  <li>IDE-private Program Model details,</li>
  <li>the full repository-wide doctrine for version transition.</li>
</ul>

<p>
Normative ownership of specific source constructs remains in the relevant published <code>Expression/</code> documents. Schema artifacts are downstream machine-checkable support for that ownership.
</p>

<hr/>

<h2 id="what-schema-means-in-frog">4. What “Schema” Means in FROG</h2>

<p>
In FROG, “schema” means the machine-checkable structural contract of canonical source.
</p>

<p>
It does not mean:
</p>

<ul>
  <li>the whole language,</li>
  <li>all legality checks,</li>
  <li>the entire validator,</li>
  <li>the full semantic interpretation of a FROG,</li>
  <li>the execution-facing representation of a validated program.</li>
</ul>

<p>
A schema MAY include:
</p>

<ul>
  <li>required top-level sections,</li>
  <li>section kind constraints,</li>
  <li>structural discriminators,</li>
  <li>object-shape requirements,</li>
  <li>array-versus-object distinctions,</li>
  <li>selected source-owned structural rules whose ownership is explicit and stable enough to encode.</li>
</ul>

<p>
It does not automatically cover every rule that can be stated in prose. Some structural rules may remain validator logic rather than pure declarative schema when they clearly belong to source structure but are awkward to encode declaratively without reducing clarity or recoverability.
</p>

<p>
Therefore, “schema” in FROG is:
</p>

<ul>
  <li>broader than one serialization technology,</li>
  <li>broader than one validator implementation,</li>
  <li>narrower than the whole language,</li>
  <li>strictly upstream from semantic validation and IR work.</li>
</ul>

<hr/>

<h2 id="structural-validation-boundary">5. Structural Validation Boundary</h2>

<p>
Structural validation is the stage after loadability and before semantic validation.
</p>

<pre><code>bytes / text
    -&gt; loadable JSON source
    -&gt; structurally valid canonical source
    -&gt; validated program meaning
    -&gt; open execution-facing derivation
    -&gt; lowering / backend-facing handoff
    -&gt; private realization
</code></pre>

<p>
This document governs the second step only.
</p>

<ul>
  <li>not malformed bytes,</li>
  <li>not parser failure,</li>
  <li>not semantic legality,</li>
  <li>not execution correctness,</li>
  <li>not backend suitability,</li>
  <li>not deployment readiness.</li>
</ul>

<p>
A source file may therefore be:
</p>

<ul>
  <li>not loadable,</li>
  <li>loadable but structurally invalid,</li>
  <li>structurally valid but semantically invalid,</li>
  <li>semantically valid but unsupported by a given implementation subset.</li>
</ul>

<p>
These states MUST NOT be collapsed. A structural rejection is not a semantic rejection. A semantic rejection is not malformed source. An implementation subset limitation is not specification invalidity.
</p>

<hr/>

<h2 id="non-goals">6. Non-Goals</h2>

<p>
This document does not define:
</p>

<ul>
  <li>the complete semantic interpretation of nodes, edges, structures, widgets, widget class contracts, widget interactions, or state,</li>
  <li>the complete legality of type and value combinations,</li>
  <li>the full primitive catalog,</li>
  <li>profile-owned optional capability semantics,</li>
  <li>Execution IR construction,</li>
  <li>lowering strategy,</li>
  <li>backend contract content,</li>
  <li>runtime scheduling choices,</li>
  <li>private debug state,</li>
  <li>IDE Program Model details.</li>
</ul>

<p>
This document also does not require one mandatory schema technology. A JSON Schema artifact may be used. Other machine-checkable validator-support artifacts may also exist. The normative requirement is the published structural contract, not one implementation convenience format.
</p>

<hr/>

<h2 id="schema-layers">7. Schema Layers</h2>

<p>
FROG source-schema posture has three layers.
</p>

<h3>7.1 Normative structural prose</h3>

<p>
The primary owner remains the published specification text in <code>Expression/</code>. If a machine artifact and the published prose disagree, the prose wins until the repository is corrected and republished coherently.
</p>

<h3>7.2 Repository-visible machine-checkable schema artifacts</h3>

<p>
The repository MAY publish machine-checkable artifacts such as:
</p>

<ul>
  <li>JSON Schema documents,</li>
  <li>section-local schema fragments,</li>
  <li>validator-facing structural rule tables,</li>
  <li>generated schema bundles whose ownership remains traceable to published source documents.</li>
</ul>

<p>
These artifacts make structural validation more reproducible. They do not replace the normative specification layer.
</p>

<h3>7.3 Validator implementation logic</h3>

<p>
A validator MAY implement additional structural checks procedurally when those checks remain attributable to published source ownership and do not silently become semantic reinterpretation.
</p>

<p>
Accordingly:
</p>

<pre><code>published structural prose
    -&gt; repository-visible machine-checkable artifact when useful
    -&gt; validator implementation support

not the reverse
</code></pre>

<hr/>

<h2 id="top-level-canonical-source-shape">8. Top-Level Canonical Source Shape</h2>

<p>
At the broadest structural level, canonical <code>.frog</code> source is a JSON object with required and optional top-level sections.
</p>

<p>
The required top-level sections are:
</p>

<ul>
  <li><code>spec_version</code>,</li>
  <li><code>metadata</code>,</li>
  <li><code>interface</code>,</li>
  <li><code>diagram</code>.</li>
</ul>

<p>
The optional top-level sections are:
</p>

<ul>
  <li><code>connector</code>,</li>
  <li><code>front_panel</code>,</li>
  <li><code>icon</code>,</li>
  <li><code>ide</code>,</li>
  <li><code>cache</code>.</li>
</ul>

<p>
This top-level posture remains intentionally conservative. Cross-cutting source subsystems such as the type model, the widget model, the widget class contract model, the widget interaction model, control-structure source representation, and state/cycle source representation do not become dedicated required top-level sections merely because they are normatively specified in <code>Expression/</code>.
</p>

<p>
The top-level presence of <code>spec_version</code> is also the schema-level anchor for source-format compatibility targeting. Under the centralized cumulative version model, later source-format versions should normally be treated as later bounded extensions of earlier valid forms unless an explicit breaking boundary is declared in the repository-wide versioning surface.
</p>

<hr/>

<h2 id="section-level-schema-ownership">9. Section-Level Schema Ownership</h2>

<p>
Top-level section presence rules and section kind constraints are natural schema concerns. However, detailed section-local structure remains owned by the corresponding source document.
</p>

<p>
Examples:
</p>

<ul>
  <li><code>metadata</code> section-local ownership belongs to <code>Metadata.md</code>,</li>
  <li><code>interface</code> section-local ownership belongs to <code>Interface.md</code>,</li>
  <li><code>diagram</code> section-local ownership belongs to <code>Diagram.md</code>,</li>
  <li><code>front_panel</code> section-local ownership belongs to <code>Front panel.md</code>.</li>
</ul>

<p>
This means a machine-checkable artifact MAY initially say “this top-level field must be an object” without immediately encoding the full object shape.
</p>

<p>
That is still useful structural validation, provided the ownership boundary remains explicit.
</p>

<hr/>

<h2 id="cross-cutting-source-subsystems">10. Cross-Cutting Source Subsystems</h2>

<p>
FROG canonical source includes cross-cutting subsystems that are not independent top-level sections but still contribute to source structure.
</p>

<p>
At the current repository stage, those subsystems include:
</p>

<ul>
  <li>the type system,</li>
  <li>the widget instance model,</li>
  <li>the widget class contract model,</li>
  <li>the widget interaction model,</li>
  <li>the source-facing representation of control structures,</li>
  <li>the source-facing representation of explicit local memory and cycle formation.</li>
</ul>

<p>
The optional source provenance model is another cross-cutting source subsystem. A schema MAY check the structural shape of <code>ide.provenance</code> when that object is present, but signature verification, issuer trust, and review policy remain provenance-verification concerns rather than executable semantic validation.
</p>

<p>
These subsystems may contribute machine-checkable structural rules when they affect canonical source shape. However, their semantic consequences remain outside the schema layer when they belong to validated meaning.
</p>

<p>
Example distinction:
</p>

<ul>
  <li>“this object MUST contain a <code>kind</code> discriminator and an array named <code>ports</code>” may be schema-level,</li>
  <li>“this structure is semantically legal only when its selector type matches all branch expectations” is not automatically schema-level,</li>
  <li>“this widget interaction targets a readable property under the active widget class contract and active profile” is not automatically schema-level even though parts of its source shape may be.</li>
</ul>

<p>
Schema growth across cross-cutting subsystems should therefore remain selective and boundary-aware: encode structural shape where useful, but do not drag semantic legality upstream into source-shape validation.
</p>

<hr/>

<h2 id="machine-checkable-schema-posture">11. Machine-Checkable Schema Posture</h2>

<p>
A repository-visible machine-checkable schema artifact SHOULD aim to capture the parts of structural validity that are:
</p>

<ul>
  <li>source-owned,</li>
  <li>stable enough to encode,</li>
  <li>implementation-independent,</li>
  <li>useful for reproducible validator behavior.</li>
</ul>

<p>
It SHOULD NOT attempt to force every meaningful rule into one declarative format when that would:
</p>

<ul>
  <li>blur the structural versus semantic boundary,</li>
  <li>smuggle execution meaning into source schema,</li>
  <li>encode implementation-specific convenience as if it were repository law,</li>
  <li>make published ownership harder to understand,</li>
  <li>reduce recoverability of why a rule exists and where it is owned.</li>
</ul>

<p>
Accordingly, the machine-checkable schema posture in FROG is intentionally conservative:
</p>

<ul>
  <li>capture source shape first,</li>
  <li>capture top-level section presence rules,</li>
  <li>capture obvious structural object requirements,</li>
  <li>leave semantic interpretation to later validation stages,</li>
  <li>leave IR construction to the IR corridor,</li>
  <li>leave runtime choices to implementations.</li>
</ul>

<p>
At the current repository stage, the published machine-checkable posture may therefore be partial without being weak. Partial machine-checkability is acceptable when:
</p>

<ul>
  <li>its limits are explicit,</li>
  <li>ownership remains attributable,</li>
  <li>structural versus semantic boundaries remain clear,</li>
  <li>implementations are not forced to guess silently,</li>
  <li>schema growth remains compatible with the centralized cumulative version model.</li>
</ul>

<p>
The important rule is not “encode everything”. The important rule is:
</p>

<pre><code>encode what is structurally stable
publish what remains prose-owned
do not blur the boundary
</code></pre>

<hr/>

<h2 id="schema-vs-semantic-validation">12. Schema vs Semantic Validation</h2>

<p>
Schema validation and semantic validation are related but distinct.
</p>

<h3>12.1 Schema validation asks</h3>

<ul>
  <li>Is the source loadable as JSON?</li>
  <li>Is the root shape correct?</li>
  <li>Are required sections present?</li>
  <li>Are optional sections placed correctly?</li>
  <li>Do section values have the expected structural kind?</li>
  <li>Are structurally required identifiers or discriminators present where the source model requires them?</li>
</ul>

<h3>12.2 Semantic validation asks</h3>

<ul>
  <li>Do types match?</li>
  <li>Are graph connections legal?</li>
  <li>Is a control structure semantically well-formed?</li>
  <li>Is a local-memory cycle legal?</li>
  <li>Does a widget interaction target a member that is actually legal under the relevant widget class contract?</li>
  <li>Is a profile-gated construct allowed under the active profile?</li>
</ul>

<p>
Those questions MUST NOT be collapsed into one generic “validation” blur.
</p>

<hr/>

<h2 id="schema-vs-ir-vs-runtime">13. Schema vs IR vs Runtime</h2>

<p>
Schema is upstream from validated meaning, IR, lowering, backend handoff, and runtime realization.
</p>

<pre><code>schema
    -&gt; does not define validated meaning
    -&gt; does not define Execution IR
    -&gt; does not define lowering
    -&gt; does not define backend contract content
    -&gt; does not define private runtime realization
</code></pre>

<p>
This is especially important for cross-cutting source subsystems such as widgets and widget interaction.
</p>

<p>
For example:
</p>

<ul>
  <li>the source shape of a widget instance may be schema-level,</li>
  <li>the existence of a widget class contract as a source-owned concept may be schema-relevant where serialized shape is concerned,</li>
  <li>the legality of a property read against a given widget class is not schema ownership by default,</li>
  <li>runtime UI object behavior is not schema ownership.</li>
</ul>

<hr/>

<h2 id="validator-posture">14. Validator Posture</h2>

<p>
A FROG validator is a downstream consumer of the published structural contract. It may be split internally into stages such as:
</p>

<pre><code>loader
    -&gt; source-shape checker
    -&gt; structural validator
    -&gt; semantic validator
</code></pre>

<p>
That split is encouraged because it keeps rejection reasons explicit.
</p>

<p>
A validator SHOULD report at least:
</p>

<ul>
  <li>loadability failures,</li>
  <li>structural validation failures,</li>
  <li>semantic validation failures,</li>
  <li>unsupported-but-valid situations when an implementation subset is narrower than repository law.</li>
</ul>

<p>
A provenance-aware validator MAY additionally report malformed, stale, unsupported, invalid, or untrusted provenance. Such reporting MUST remain distinct from determining whether the program computes a valid result. Missing provenance is a valid base source condition unless a separate optional policy or profile requires provenance for a deployment workflow.
</p>

<p>
A validator MAY use repository-visible machine-checkable artifacts to support structural validation. However:
</p>

<ul>
  <li>a machine-checkable artifact does not override normative prose,</li>
  <li>implementation convenience does not override published ownership,</li>
  <li>a passed validator run does not redefine the language.</li>
</ul>

<p>
A validator MUST NOT:
</p>

<ul>
  <li>silently repair malformed canonical source,</li>
  <li>quietly reinterpret structurally invalid source into some private acceptable form,</li>
  <li>treat implementation convenience as if it were specification truth,</li>
  <li>collapse source validation, semantic validation, and runtime support into one blurred rejection category.</li>
</ul>

<p>
When <code>spec_version</code> affects structural acceptance, the validator posture SHOULD remain compatible with the centralized cumulative version model: later source-format versions should normally be treated as later bounded extensions of earlier valid forms, while unsupported newer constructs are handled through explicit limitation rather than silent reinterpretation.
</p>

<hr/>

<h2 id="relation-with-conformance">15. Relation with Conformance</h2>

<p>
This document defines schema posture. <code>Conformance/</code> defines public testable expectations against that posture.
</p>

<pre><code>Expression/                  -&gt; structural contract is written
Conformance/                 -&gt; structural contract is tested
Implementations/Reference/   -&gt; structural contract is consumed
</code></pre>

<p>
Accordingly:
</p>

<ul>
  <li>schema-owned rejection cases belong naturally in <code>Conformance/invalid/</code>,</li>
  <li>schema-owned acceptance cases belong naturally in <code>Conformance/valid/</code>,</li>
  <li>conformance expectations SHOULD distinguish loadability, structural validity, semantic acceptance, and preservation.</li>
</ul>

<p>
Conformance does not redefine schema ownership. It makes it inspectable. A case passing in one implementation does not turn implementation behavior into repository law.
</p>

<hr/>

<h2 id="representation-and-evolution-rules">16. Representation and Evolution Rules</h2>

<p>
The schema posture of FROG must remain evolvable without becoming unstable.
</p>

<p>
Therefore:
</p>

<ul>
  <li>schema artifacts SHOULD be derived from published source ownership rather than invented independently,</li>
  <li>schema artifacts SHOULD be version-aware where <code>spec_version</code> materially affects allowed source structure,</li>
  <li>new schema constraints SHOULD be introduced conservatively,</li>
  <li>schema growth SHOULD prefer explicit ownership and recoverability over compact cleverness,</li>
  <li>schema changes MUST NOT silently move semantic, IR, or runtime ownership into <code>Expression/</code>.</li>
</ul>

<p>
A repository-visible schema artifact MAY be incomplete relative to all structural prose if its limits are explicit. Partial machine-checkability is acceptable. Silent ambiguity is not.
</p>

<p>
Under the centralized cumulative version model, schema evolution SHOULD also follow this default interpretation:
</p>

<pre><code>later source-format version
    =
earlier valid structural forms
    +
explicit bounded structural additions
</code></pre>

<p>
That model does not remove the possibility of a breaking boundary, but it means such a boundary should be explicit and deliberate rather than implicit in schema drift.
</p>

<p>
Preferred growth pattern:
</p>

<pre><code>published ownership
    -&gt; explicit structural rule
    -&gt; repository-visible machine-checkable form
    -&gt; conformance case
    -&gt; implementation alignment
</code></pre>

<hr/>

<h2 id="minimum-guarantees">17. Minimum Guarantees</h2>

<p>
At minimum, the repository-visible schema posture SHOULD make the following explicit:
</p>

<ul>
  <li>canonical <code>.frog</code> source is a JSON object,</li>
  <li>required top-level sections are explicit,</li>
  <li>optional top-level sections are explicit,</li>
  <li>top-level unexpected fields are not silently accepted,</li>
  <li>machine-checkable coverage limits are explicit when the schema artifact is partial.</li>
</ul>

<p>
Anything beyond that MAY be added progressively, provided ownership remains attributable and the structural-versus-semantic boundary remains clear.
</p>

<hr/>

<h2 id="reading-rule">18. Reading Rule</h2>

<p>
This document MUST be read together with the rest of <code>Expression/</code>.
</p>

<p>
It does not replace those documents. It defines how machine-checkable structural artifacts relate to them.
</p>

<p>
If a reader wants to know:
</p>

<ul>
  <li>what sections exist, read <code>Readme.md</code>,</li>
  <li>what the front panel structurally represents, read <code>Front panel.md</code>,</li>
  <li>what a widget instance is, read <code>Widget.md</code>,</li>
  <li>what the widget class contract owns, read <code>Widget class contract.md</code>,</li>
  <li>how widget interaction is represented in source, read <code>Widget interaction.md</code>,</li>
  <li>what the schema layer may machine-check, read this document,</li>
  <li>what the repository-wide cumulative version model is, read <code>Versioning/Readme.md</code>.</li>
</ul>

<hr/>

<h2 id="status">19. Status</h2>

<p>
The published machine-checkable schema posture of FROG is intentionally conservative.
</p>

<p>
At the current stage, it is acceptable for repository-visible schema artifacts to cover top-level shape, section vocabulary, required-versus-optional sections, and selected obvious structural object requirements, while leaving broader section-local and cross-cutting legality primarily prose-owned or validator-owned.
</p>

<p>
This is not a weakness. It is an explicit boundary discipline.
</p>

<p>
It is also compatible with the centralized cumulative version model: later source-format versions may add bounded structural shape, while earlier valid structural forms should normally remain interpretable as part of later source evolution unless an explicit breaking boundary is declared.
</p>

<hr/>

<h2 id="license">20. License</h2>

<p>
See the repository-level license information for the licensing terms governing this specification.
</p>
