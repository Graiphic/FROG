<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG IR Schema Posture</h1>

<p align="center">
  <strong>Normative schema posture for the canonical JSON Execution IR Document</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#why-this-document-exists">2. Why This Document Exists</a></li>
  <li><a href="#scope">3. Scope</a></li>
  <li><a href="#relation-with-other-ir-documents">4. Relation with Other IR Documents</a></li>
  <li><a href="#normative-posture">5. Normative Posture</a></li>
  <li><a href="#what-the-schema-validates">6. What the Schema Validates</a></li>
  <li><a href="#what-the-schema-does-not-validate">7. What the Schema Does Not Validate</a></li>
  <li><a href="#canonical-document-boundary">8. Canonical Document Boundary</a></li>
  <li><a href="#canonical-json-posture">9. Canonical JSON Posture</a></li>
  <li><a href="#schema-family">10. Schema Family</a></li>
  <li><a href="#top-level-document-categories">11. Top-Level Document Categories</a></li>
  <li><a href="#execution-unit-categories">12. Execution Unit Categories</a></li>
  <li><a href="#object-category-posture">13. Object Category Posture</a></li>
  <li><a href="#connection-category-posture">14. Connection Category Posture</a></li>
  <li><a href="#region-category-posture">15. Region Category Posture</a></li>
  <li><a href="#source-map-and-correspondence-posture">16. Source Map and Correspondence Posture</a></li>
  <li><a href="#identity-and-recoverability-posture">17. Identity and Recoverability Posture</a></li>
  <li><a href="#widget-object-and-ui-schema-posture">18. Widget Object and UI Schema Posture</a></li>
  <li><a href="#current-finite-producer-profiles">18.1 Current Finite Producer Profiles (Experimental 0.1)</a></li>
  <li><a href="#schema-validity-versus-architectural-validity">19. Schema Validity Versus Architectural Validity</a></li>
  <li><a href="#forbidden-misreadings">20. Forbidden Misreadings</a></li>
  <li><a href="#relation-with-lowering-and-backend-contract">21. Relation with Lowering and Backend Contract</a></li>
  <li><a href="#summary">22. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the schema posture for the canonical FROG Execution IR Document in base v0.1.
</p>

<p>
The Execution IR is not only an architectural layer. It is also a canonical open artifact that a conforming implementation MUST be able to emit as JSON. This document defines how that canonical JSON form is validated structurally.
</p>

<p>
Accordingly, the schema exists to provide:
</p>

<ul>
  <li>a machine-checkable structural boundary for canonical JSON IR emission,</li>
  <li>a stable validation surface for interchange, tooling, regression testing, and conformance reasoning,</li>
  <li>a companion to the architectural, derivation, identity, and construction law of the IR layer,</li>
  <li>a disciplined boundary between canonical open IR and later downstream specialization.</li>
</ul>

<p>
The schema is therefore part of the open IR corridor:
</p>

<pre><code>validated program meaning
   -&gt;
canonical Execution IR Document
   -&gt;
canonical JSON validation
   -&gt;
later lowering
   -&gt;
backend-facing handoff
   -&gt;
private realization</code></pre>

<p>
This document does not say that JSON schema replaces architectural law. It says that canonical JSON IR must be machine-checkable in a way that remains aligned with architectural law.
</p>

<p>
This alignment is especially important for widget-related execution structure, because canonical JSON must remain able to represent execution-facing widget distinctions without collapsing into host realization or runtime-private UI storage.
</p>

<hr/>

<h2 id="why-this-document-exists">2. Why This Document Exists</h2>

<p>
A canonical open IR is only durable if it is:
</p>

<ul>
  <li>architecturally defined,</li>
  <li>derivable from validated meaning,</li>
  <li>recoverable and attributable,</li>
  <li>materially constructible,</li>
  <li>machine-checkable as a canonical artifact.</li>
</ul>

<p>
Without a schema posture, the canonical JSON form would drift into:
</p>

<ul>
  <li>implementation-specific payload conventions,</li>
  <li>unstated omissions,</li>
  <li>shape ambiguity,</li>
  <li>non-portable inspection tooling,</li>
  <li>backend-driven accidental formats.</li>
</ul>

<p>
This document therefore exists to ensure that:
</p>

<ul>
  <li>the canonical JSON Execution IR form is a real public artifact,</li>
  <li>implementations can be compared at the structural JSON boundary,</li>
  <li>construction remains aligned with a machine-checkable target,</li>
  <li>schema validity is clearly separated from deeper architectural validity.</li>
</ul>

<p>
Without this separation, widget-related execution content would be especially vulnerable to drift into host-specific payloads, UI toolkit conventions, or platform-native control encodings disguised as canonical IR.
</p>

<hr/>

<h2 id="scope">3. Scope</h2>

<p>
This document defines:
</p>

<ul>
  <li>the normative posture of the canonical JSON IR schema artifacts,</li>
  <li>what structural categories the schema validates,</li>
  <li>how the canonical IR document family is framed in JSON,</li>
  <li>the required presence of the principal top-level and execution-unit categories in base v0.1,</li>
  <li>the posture of <code>source_map</code> and <code>correspondence</code> as explicit schema-validated record arrays,</li>
  <li>the distinction between schema-validity and deeper architectural validity.</li>
</ul>

<p>
This document does not define:
</p>

<ul>
  <li>validated language meaning,</li>
  <li>the full execution semantics of any object family,</li>
  <li>the full derivation law from meaning to IR,</li>
  <li>the full identity and recoverability law in isolation,</li>
  <li>the full material construction procedure for every implementation,</li>
  <li>backend-specific lowered forms,</li>
  <li>runtime-private execution artifacts,</li>
  <li>the full widget-package architecture,</li>
  <li>the full host-side realization model.</li>
</ul>

<hr/>

<h2 id="relation-with-other-ir-documents">4. Relation with Other IR Documents</h2>

<p>
This document MUST be read together with the rest of the IR layer.
</p>

<p>
Ownership remains:
</p>

<pre><code>Execution IR.md
   -&gt; architectural identity of the canonical open IR

Derivation rules.md
   -&gt; what must correspond from validated meaning to IR content

Identity and Mapping.md
   -&gt; attribution, correspondence, and recoverability law

Construction rules.md
   -&gt; how conforming canonical IR payloads are materially built

Schema.md
   -&gt; schema posture for machine-checkable structural validation

IR/schema/
   -&gt; machine-checkable schema artifacts

Lowering.md
   -&gt; later target-oriented specialization

Backend contract.md
   -&gt; later backend-facing handoff</code></pre>

<p>
This document therefore MUST be read as a schema companion to the architectural and construction material rather than as a replacement for them.
</p>

<pre><code>Architecture
   -&gt; Execution IR.md

Derivation
   -&gt; Derivation rules.md

Identity / recoverability
   -&gt; Identity and Mapping.md

Construction
   -&gt; Construction rules.md

Schema posture
   -&gt; Schema.md

Machine-checkable schema
   -&gt; IR/schema/</code></pre>

<p>
The same reading rule applies to widget-related execution structure:
</p>

<ul>
  <li>widget distinctions are architecturally owned by the broader IR and Expression layers,</li>
  <li>this document only defines the schema posture that keeps those distinctions structurally representable in canonical JSON.</li>
</ul>

<hr/>

<h2 id="normative-posture">5. Normative Posture</h2>

<p>
The schema artifacts published in <code>IR/schema/</code> are normative for structural validation of the canonical JSON Execution IR Document in the matching schema version.
</p>

<p>
Accordingly:
</p>

<ul>
  <li>a conforming implementation MUST be able to emit a canonical JSON Execution IR Document that satisfies the published schema for the claimed version,</li>
  <li>a payload that fails the published schema MUST NOT be claimed as a schema-valid canonical Execution IR Document for that schema version,</li>
  <li>schema validity alone does not imply full semantic validity, derivation correctness, or architectural sufficiency,</li>
  <li>schema compliance MUST be interpreted together with the rest of the IR and Language specifications.</li>
</ul>

<p>
The schema is therefore:
</p>

<ul>
  <li>normative for structural JSON validity,</li>
  <li>non-sufficient for full architectural correctness,</li>
  <li>downstream from derivation law,</li>
  <li>upstream from lowering and backend-facing specialization.</li>
</ul>

<p>
A schema-valid payload that erases required widget-related distinctions, or that silently imports host realization detail as canonical open IR, is still non-conforming at the architectural level even if it satisfies structural validation.
</p>

<hr/>

<h2 id="what-the-schema-validates">6. What the Schema Validates</h2>

<p>
The schema validates the structural JSON boundary of the canonical Execution IR Document.
</p>

<p>
In base v0.1, this includes at least:
</p>

<ul>
  <li>top-level document shape,</li>
  <li>required document categories,</li>
  <li>execution-unit shape,</li>
  <li>required execution-unit arrays and records,</li>
  <li>object, connection, and region record families,</li>
  <li>identifier-bearing fields where published,</li>
  <li>source attribution and correspondence support categories,</li>
  <li>enum-style structural category restrictions where published by the schema artifacts.</li>
</ul>

<p>
The schema therefore answers questions such as:
</p>

<ul>
  <li>Is this a structurally valid canonical JSON Execution IR payload?</li>
  <li>Does the execution unit include the required major categories?</li>
  <li>Are object, connection, region, source-map, and correspondence carriers present at the required structural level?</li>
  <li>Do records satisfy the machine-checkable field and category constraints of the claimed schema version?</li>
</ul>

<p>
Where widget-related execution structure is present, the schema may also validate the structural carriers needed for:
</p>

<ul>
  <li>widget-related object families or roles,</li>
  <li>member-address descriptors where published,</li>
  <li>UI-operation family tags where published,</li>
  <li>widget-related attribution and correspondence carriers.</li>
</ul>

<hr/>

<h2 id="what-the-schema-does-not-validate">7. What the Schema Does Not Validate</h2>

<p>
The schema does not validate everything that matters.
</p>

<p>
The schema does not by itself prove:
</p>

<ul>
  <li>that semantic validation upstream was correct,</li>
  <li>that derivation preserved validated meaning correctly,</li>
  <li>that identity and mapping obligations were satisfied in the strong architectural sense,</li>
  <li>that all category distinctions remain recoverable in the way required by the IR architecture,</li>
  <li>that a schema-valid payload is suitable for every later lowering route,</li>
  <li>that a backend-facing contract derived later will be correct.</li>
</ul>

<p>
In particular:
</p>

<pre><code>schema-valid
   does not imply
semantically valid

schema-valid
   does not imply
architecturally sufficient

schema-valid
   does not imply
identity-correct in every deeper sense

schema-valid
   does not imply
lowering-correct

schema-valid
   does not imply
backend-correct</code></pre>

<p>
The schema validates structural admissibility of the canonical JSON form. It does not replace the rest of the specification.
</p>

<p>
For widget-related execution structure, the schema does not by itself prove:
</p>

<ul>
  <li>that <code>widget_value</code> was not confused with property access to member <code>value</code>,</li>
  <li>that <code>widget_reference</code> was not confused with standardized UI-object primitive operation,</li>
  <li>that host-specific realization detail was correctly kept downstream.</li>
</ul>

<hr/>

<h2 id="canonical-document-boundary">8. Canonical Document Boundary</h2>

<p>
In base v0.1, the canonical JSON IR posture is intentionally conservative:
</p>

<ul>
  <li>one canonical Execution IR Document,</li>
  <li>one execution unit inside that document,</li>
  <li>one principal open-boundary document family,</li>
  <li>one primary schema artifact rooted at the document level.</li>
</ul>

<p>
That posture exists so the open IR boundary remains:
</p>

<ul>
  <li>portable,</li>
  <li>inspectable,</li>
  <li>diffable,</li>
  <li>stable across conforming implementations.</li>
</ul>

<p>
The canonical document boundary is therefore not:
</p>

<ul>
  <li>a bag of backend-private fragments,</li>
  <li>a runtime snapshot,</li>
  <li>a private scheduler graph dump,</li>
  <li>a target-family-specific lowered artifact,</li>
  <li>a host-private UI realization package.</li>
</ul>

<hr/>

<h2 id="canonical-json-posture">9. Canonical JSON Posture</h2>

<p>
The preferred base v0.1 canonical posture is explicit and regular.
</p>

<p>
At the JSON boundary, the canonical Execution IR Document SHOULD expose clearly separated categories rather than relying on positional conventions or implementation-private packing.
</p>

<p>
The preferred posture is:
</p>

<pre><code>{
  "document_id": "...",
  "unit": {
    "id": "...",
    "objects": [ ... ],
    "connections": [ ... ],
    "regions": [ ... ],
    "source_map": [ ... ],
    "correspondence": [ ... ],
    "metadata": { ... }
  }
}</code></pre>

<p>
The exact schema-owned fields and constraints belong to the published JSON Schema artifacts. The architectural posture here is that canonical JSON IR remains explicit, stable, and category-oriented.
</p>

<p>
This posture supports:
</p>

<ul>
  <li>ordinary JSON validation tools,</li>
  <li>repository diffability,</li>
  <li>portable inspection,</li>
  <li>clear separation between execution structure and attribution support structure,</li>
  <li>future lowering without forcing lowering details into the canonical open IR payload.</li>
</ul>

<p>
This same explicit posture is important for widget-related execution structure, because widget identity, UI operation families, and non-primary correspondence should remain representable without relying on runtime-private conventions.
</p>

<hr/>

<h2 id="schema-family">10. Schema Family</h2>

<p>
The base v0.1 IR schema family is rooted in:
</p>

<ul>
  <li><code>IR/schema/frog.execution-ir.schema.json</code></li>
</ul>

<p>
That file is the primary machine-checkable schema artifact for the canonical Execution IR Document in base v0.1.
</p>

<p>
Future versions MAY introduce:
</p>

<ul>
  <li>versioned schema families,</li>
  <li>split component schemas,</li>
  <li>referenced sub-schemas,</li>
  <li>later schema bundles for future IR evolution.</li>
</ul>

<p>
For base v0.1, the posture remains conservative:
</p>

<ul>
  <li>one primary schema artifact,</li>
  <li>one canonical document family,</li>
  <li>one execution-unit model at the open boundary.</li>
</ul>

<p>
Future schema decomposition MAY add subordinate schemas for:
</p>

<ul>
  <li>objects,</li>
  <li>connections,</li>
  <li>regions,</li>
  <li>source-map records,</li>
  <li>correspondence records,</li>
  <li>widget-related execution carriers.</li>
</ul>

<hr/>

<h2 id="top-level-document-categories">11. Top-Level Document Categories</h2>

<p>
At the top level, the canonical JSON IR document MUST expose a document-level identity boundary and an execution-unit carrier compatible with the published schema family.
</p>

<p>
The top-level posture exists so that:
</p>

<ul>
  <li>the IR artifact is identifiable as one canonical document,</li>
  <li>the execution-bearing content is grouped as one explicit unit in base v0.1,</li>
  <li>document identity remains distinct from unit identity,</li>
  <li>later tooling can reason about the document as an artifact rather than only about anonymous arrays.</li>
</ul>

<p>
A conforming implementation MUST NOT collapse document identity and execution-unit identity into one ambiguous structural role if the published schema version keeps them distinct.
</p>

<hr/>

<h2 id="execution-unit-categories">12. Execution Unit Categories</h2>

<p>
The execution unit is the principal structural container of canonical execution-facing content.
</p>

<p>
In base v0.1, the machine-checkable posture requires that the execution unit carry explicit major categories including:
</p>

<ul>
  <li><code>objects</code>,</li>
  <li><code>connections</code>,</li>
  <li><code>regions</code>,</li>
  <li><code>source_map</code>,</li>
  <li><code>correspondence</code>.</li>
</ul>

<p>
This posture matters because it prevents accidental collapse of:
</p>

<ul>
  <li>execution-facing objects,</li>
  <li>execution-facing relations,</li>
  <li>region-owned structure,</li>
  <li>source attribution support,</li>
  <li>recoverable correspondence support.</li>
</ul>

<p>
These categories are therefore not optional conveniences. At the schema boundary in base v0.1, they are part of the canonical structural surface.
</p>

<p>
Where widget-related execution structure is present, these categories MUST remain sufficient to carry:
</p>

<ul>
  <li>execution-facing widget objects or roles,</li>
  <li>widget-related operation records,</li>
  <li>attribution and correspondence for non-primary widget-side contributors.</li>
</ul>

<hr/>

<h2 id="object-category-posture">13. Object Category Posture</h2>

<p>
Objects are the primary execution-facing carriers inside the execution unit.
</p>

<p>
The schema posture for objects exists to ensure that canonical JSON IR can express:
</p>

<ul>
  <li>execution-facing identity,</li>
  <li>object family classification,</li>
  <li>typed interface or equivalent terminal structure,</li>
  <li>object-local metadata where permitted,</li>
  <li>compatibility with attribution and correspondence references where relevant.</li>
</ul>

<p>
The schema MAY validate object-level category fields, enum values, and required record structure.
</p>

<p>
However, the schema does not by itself prove that the chosen object family is semantically correct. That remains downstream from validated meaning and derivation law.
</p>

<p>
The posture rule is:
</p>

<pre><code>object structure
   is schema-validated

object meaning
   is not created by schema</code></pre>

<p>
For widget-related execution structure, object posture SHOULD remain able to carry:
</p>

<ul>
  <li><code>widget_value</code> participation families,</li>
  <li><code>widget_reference</code> participation families,</li>
  <li>standardized UI-operation families,</li>
  <li>support-side addressing records where the schema family chooses to expose them.</li>
</ul>

<hr/>

<h2 id="connection-category-posture">14. Connection Category Posture</h2>

<p>
Connections are execution-facing relation carriers.
</p>

<p>
The schema posture for connections exists so that canonical JSON IR can machine-check:
</p>

<ul>
  <li>that connection records exist explicitly,</li>
  <li>that endpoint-related structure is present in the expected schema-owned form,</li>
  <li>that connection identity and metadata posture are structurally consistent where published.</li>
</ul>

<p>
The schema does not by itself prove that every connection preserves the deeper architectural distinction between:
</p>

<ul>
  <li>ordinary connectivity,</li>
  <li>public-interface-boundary participation,</li>
  <li>structure-boundary participation,</li>
  <li>explicit state participation,</li>
  <li>explicit UI sequencing.</li>
</ul>

<p>
That distinction remains governed by derivation and identity law. The schema only validates the structural carrier boundary that makes such distinctions expressible and inspectable.
</p>

<p>
For widget-related execution structure, this same rule includes the distinction between:
</p>

<ul>
  <li>ordinary valueflow involving widget values,</li>
  <li>widget-reference carrying flows where represented,</li>
  <li>connections related to UI primitive operations or sequencing where represented.</li>
</ul>

<hr/>

<h2 id="region-category-posture">15. Region Category Posture</h2>

<p>
Regions are explicit structural carriers needed for structured execution forms.
</p>

<p>
The schema posture for regions exists so that canonical JSON IR can machine-check:
</p>

<ul>
  <li>the existence of explicit region records,</li>
  <li>their identity-bearing structure,</li>
  <li>their place inside the execution unit,</li>
  <li>their compatibility with structure-owned execution families.</li>
</ul>

<p>
The schema does not by itself prove that structure ownership, boundary roles, or terminal roles are semantically correct. Those remain governed by validated meaning, derivation rules, and recoverability law.
</p>

<p>
The posture rule is:
</p>

<pre><code>region record presence
   is schema-checkable

correct structured-control meaning
   is not created by schema alone</code></pre>

<hr/>

<h2 id="source-map-and-correspondence-posture">16. Source Map and Correspondence Posture</h2>

<p>
The schema MUST validate explicit categories for attribution and recoverable correspondence support.
</p>

<p>
In base v0.1, these appear as:
</p>

<ul>
  <li><code>source_map</code></li>
  <li><code>correspondence</code></li>
</ul>

<p>
The preferred base v0.1 posture is explicit record arrays for both categories. That keeps the canonical JSON form:
</p>

<ul>
  <li>simple,</li>
  <li>portable,</li>
  <li>diffable,</li>
  <li>straightforward to inspect with ordinary JSON tooling.</li>
</ul>

<p>
This posture also prevents a common failure mode:
</p>

<pre><code>execution objects exist
but
attribution and non-primary correspondence are only implicit
or
buried in implementation-private conventions</code></pre>

<p>
The schema therefore supports a canonical JSON IR where attribution and correspondence are visible as first-class structural categories rather than inferred after the fact.
</p>

<p>
For widget-related execution structure, this posture is especially important because it allows canonical JSON to preserve:
</p>

<ul>
  <li>widget-related attribution,</li>
  <li>member-address correspondence where needed,</li>
  <li>intentional non-primary correspondence for widget packages or realization packages,</li>
  <li>without forcing such materials into primary execution categories.</li>
</ul>

<hr/>

<h2 id="identity-and-recoverability-posture">17. Identity and Recoverability Posture</h2>

<p>
Schema posture must remain compatible with the stronger identity and recoverability law of the IR layer.
</p>

<p>
This means the schema-owned JSON boundary SHOULD support explicit or equivalent structural carriers for:
</p>

<ul>
  <li>document identity,</li>
  <li>execution-unit identity,</li>
  <li>object identity,</li>
  <li>connection identity where published,</li>
  <li>region identity where published,</li>
  <li>source attribution records,</li>
  <li>correspondence records.</li>
</ul>

<p>
The schema does not need to encode every semantic explanation. It does need to keep the canonical JSON form structurally capable of carrying the recoverability surface required by the open IR boundary.
</p>

<p>
Accordingly, a schema evolution MUST NOT simplify the JSON form by erasing the structural possibility of:
</p>

<ul>
  <li>attribution,</li>
  <li>non-primary correspondence,</li>
  <li>document-versus-unit identity,</li>
  <li>explicit object / connection / region categorization.</li>
</ul>

<p>
Where widget-related execution structure is present, the same posture SHOULD preserve the structural possibility of:
</p>

<ul>
  <li>widget identity anchors,</li>
  <li>UI-operation family tags,</li>
  <li>member-address descriptors where required,</li>
  <li>non-primary widget-side correspondence.</li>
</ul>

<hr/>

<h2 id="widget-object-and-ui-schema-posture">18. Widget Object and UI Schema Posture</h2>

<p>
The canonical IR schema must remain able to represent execution-facing widget interaction without taking ownership of widget realization.
</p>

<p>
Accordingly, the schema posture SHOULD support, where the published schema version chooses to expose them:
</p>

<ul>
  <li>distinct object-family or role carriers for <code>widget_value</code>,</li>
  <li>distinct object-family or role carriers for <code>widget_reference</code>,</li>
  <li>distinct object-family or role carriers for standardized UI-object primitive operations,</li>
  <li>member-address support structures where required,</li>
  <li>widget-related source-map and correspondence records.</li>
</ul>

<p>
At the same time, the schema posture MUST NOT require canonical JSON IR to include:
</p>

<ul>
  <li>host widget trees,</li>
  <li>SVG scene graphs,</li>
  <li>platform-native control classes,</li>
  <li>runtime UI pointers,</li>
  <li>toolkit-specific rendering metadata as open-IR truth.</li>
</ul>

<p>
The posture rule is therefore:
</p>

<pre><code>schema supports execution-facing widget distinction
without
requiring host-realization payloads</code></pre>

<p>
This keeps the same canonical JSON IR compatible with:
</p>

<ul>
  <li>Python runtimes,</li>
  <li>Rust runtimes,</li>
  <li>C/C++ runtimes,</li>
  <li>native executable front-panel hosts,</li>
  <li>mixed compute / UI target corridors.</li>
</ul>

<hr/>

<h3 id="current-finite-producer-profiles">18.1 Current Finite Producer Profiles (Experimental 0.1)</h3>

<p>
The experimental <code>frog.execution_ir</code> format remains version <code>0.1</code>.
The existing schemas add structural recognition of forms already emitted by the common
<a href="../sdk/ecosystem/docs/frontend-api.md">Frontend</a>. This is an additive schema
alignment, not a new Frontend API, a second IR grammar, or an assertion that every
consumer realizes these profiles. Historical role-bearing ports and existing metadata
remain valid. Unknown object families, document metadata members and port members remain
rejected by their closed schema owners.
</p>

<ul>
  <li><code>constant</code> is an object family. An explicit empty object <code>role</code>
      is permitted only for this family; other families still require a nonempty role
      when that optional field is present. Literal meaning belongs to the declared
      <code>attributes.constant_profile</code>, exact <code>literal_value</code> and type.
      Structural recognition does not turn the emitted scalar-constant extension into
      an unrestricted canonical constant language.</li>
  <li>Document metadata may contain <code>profile</code>, a nonempty string of at most
      256 characters, and <code>source_sha256</code>, exactly <code>sha256:</code> followed
      by 64 lowercase hexadecimal digits. The latter identifies the exact authoritative
      Source bytes; checking that identity remains the consumer's responsibility.</li>
  <li>A port retains its existing nonempty enumerated <code>role</code>, or carries the
      complete nonempty object <code>studio_type_schema</code>. Only the latter form may
      omit <code>role</code>. The surrounding port record remains closed. An empty or
      non-object type schema cannot serve as this alternative.</li>
</ul>

<p>
<code>studio_type_schema</code> is a profile-defined semantic object, not a host storage
descriptor. No separate public JSON Schema currently defines its complete type grammar;
the common Frontend's existing type codec validates that meaning. The IR schema therefore
checks its object boundary without inventing another recursive type system. Consumers
MUST preserve its complete content, including nested children, nominal identity, fields
and choice metadata. A physical <code>type_ref</code> such as <code>u32</code> MUST NOT
replace an Enum schema such as <code>enum&lt;"test.collection.nonzero",u32&gt;</code>.
Schema validity alone neither permits ignoring that schema nor grants nominal-type
support to a backend. A consumer that cannot preserve its meaning must refuse the graph
with attribution rather than flatten the type or choose another backend implicitly.
</p>

<table>
  <thead><tr><th>Emitted profile</th><th>Forms preserved at this boundary</th></tr></thead>
  <tbody>
    <tr><td><code>frog.canonical.widget-f64-add.v1</code></td><td>Legacy role-bearing
        f64 control/indicator ports and <code>frog.core.add</code>.</td></tr>
    <tr><td><code>frog.canonical.typed-structures.v1</code></td><td>Exact scalar types,
        typed constants and explicit structured regions, terminals and state transfers.</td></tr>
    <tr><td><code>frog.studio.scalar-constant@1</code></td><td>Declared scalar literal
        extension, retaining exact I64/U64 JSON integer tokens instead of conversion
        through binary64.</td></tr>
    <tr><td><code>frog.studio.array.real1d@1</code></td><td>The declared one-dimensional
        f64 Build/Index/Size operations, with their own checked indexing contract;
        this is not a claim to implement all public Collections.</td></tr>
    <tr><td><code>frog.studio.composed-program@1</code></td><td>Complete semantic
        schemas on objects and ports, including nominal types whose physical carrier
        alone is insufficient.</td></tr>
  </tbody>
</table>

<p>
The finite reference-C consumer uses the same emitted FIR as LLVM. Its declared
<code>graiphic.reference-c.finite@1</code> realization admits bool, f64, i32, u32, i64 and
u64 scalars, rank-one f64 Arrays of at most 32 elements, the listed addition and Array
operations, and pure statically bounded For loops of 0 through 32 iterations. These are
consumer admission limits, not restrictions imposed on every schema-valid IR document.
Valid Frontend subtraction and nominal Enum documents remain valid IR even when this
consumer rejects them. The complete graph still requires its actual backend admission.
</p>

<p>
For the emitted <code>for_loop</code>, count and iteration terminals are explicit, body
objects belong to an owned region, and <code>attributes.shift_registers</code> declares
each pair's initializer/read stages, write port and exterior result port. The finite
corpus initializes the shift register before the first iteration, reads the current
state in the body, writes the next state after that iteration, and publishes the last
state through <code>result_port</code>. Count zero executes no body and returns the
initializer. Iterations are zero-based and stop before count. Internal iteration,
count-value and history-read ports are not exterior results. The full type and operation
contracts still determine value semantics; the JSON Schema does not execute this loop
or infer transfers from port names or geometry.
</p>

<p>
The regression owner
<a href="../sdk/ecosystem/tests/test_reference_c_fir_contract.py">test_reference_c_fir_contract.py</a>
validates eight immutable real Frontend artifacts: scalar Add, exact typed scalars,
Array Build/Index/Size, three For bounds, and two valid-Frontend documents refused by
the finite backend. It resolves the existing schema family through a local-only
registry. Neither fixtures nor emitted FIR are rewritten to satisfy validation.
</p>

<hr/>

<h2 id="schema-validity-versus-architectural-validity">19. Schema Validity Versus Architectural Validity</h2>

<p>
This distinction is critical.
</p>

<p>
A payload may be:
</p>

<ul>
  <li>schema-valid but architecturally insufficient,</li>
  <li>schema-valid but derivation-wrong,</li>
  <li>schema-valid but attribution-poor in ways that only deeper specification checks can detect,</li>
  <li>schema-valid but not acceptable for a later backend corridor because upstream distinctions were already lost.</li>
</ul>

<p>
Conversely, a conceptually intended IR may be architecturally plausible but still fail the canonical JSON schema if required structural carriers are missing.
</p>

<p>
Therefore:
</p>

<pre><code>architectural law
   is broader than
schema law

schema law
   is stricter than
informal JSON convenience

schema-valid
   is necessary
but not sufficient
for full canonical IR correctness</code></pre>

<p>
This section must remain explicit because the repository now already uses conformance material to distinguish not only semantic rejection, but also IR architectural preservation and schema-valid versus architecturally valid outcomes.
</p>

<hr/>

<h2 id="forbidden-misreadings">20. Forbidden Misreadings</h2>

<p>
The following interpretations are forbidden:
</p>

<ul>
  <li>“If the JSON validates, the IR is automatically correct in every deeper sense.”</li>
  <li>“The schema replaces derivation law.”</li>
  <li>“The schema replaces identity and mapping law.”</li>
  <li>“The schema may erase attribution and correspondence as long as objects still exist.”</li>
  <li>“The schema may be specialized around one backend family and still define canonical open IR.”</li>
  <li>“The canonical JSON schema may collapse the open IR boundary into a lowered or runtime-private form.”</li>
  <li>“The schema may require one host UI toolkit model as the canonical representation of widget interaction.”</li>
</ul>

<p>
The correct reading is:
</p>

<pre><code>schema
   validates canonical JSON structure

it does not replace
validated meaning
or derivation
or identity / recoverability
or lowering boundaries</code></pre>

<hr/>

<h2 id="relation-with-lowering-and-backend-contract">21. Relation with Lowering and Backend Contract</h2>

<p>
The schema boundary is upstream from lowering and backend-facing handoff.
</p>

<p>
Accordingly:
</p>

<ul>
  <li>the canonical JSON IR must remain an open IR artifact,</li>
  <li>it must not be prematurely shaped around one backend family,</li>
  <li>it must not encode runtime-private realization as though that were canonical open IR truth,</li>
  <li>it must remain suitable as the machine-checkable output of the canonical IR stage before later specialization.</li>
</ul>

<p>
This matters directly to a future industrial compiler corridor. A later LLVM-oriented route remains valid only if it starts from a canonical IR boundary that:
</p>

<ul>
  <li>is schema-valid,</li>
  <li>is architecturally valid,</li>
  <li>preserves attribution and recoverability,</li>
  <li>has not already collapsed into backend-private assumptions.</li>
</ul>

<p>
The schema therefore supports the compiler corridor indirectly:
</p>

<pre><code>validated meaning
   -&gt;
canonical open IR
   -&gt;
canonical JSON schema-valid artifact
   -&gt;
later lowering
   -&gt;
backend-facing contract
   -&gt;
LLVM-oriented or other downstream consumer</code></pre>

<p>
The schema does not define LLVM. It protects the open IR JSON boundary that LLVM-oriented lowering must remain downstream from.
</p>

<p>
The same rule applies to UI-aware downstream consumers:
</p>

<ul>
  <li>native front-panel targets,</li>
  <li>Python / Rust / C/C++ runtime families,</li>
  <li>mixed compute and host-realization pipelines.</li>
</ul>

<p>
All of them remain downstream from the same canonical JSON schema boundary.
</p>

<hr/>

<h2 id="summary">22. Summary</h2>

<p>
This document defines the schema posture for the canonical JSON Execution IR Document in base v0.1.
</p>

<p>
Its core rules are:
</p>

<ul>
  <li>the published schema artifacts are normative for structural JSON validity,</li>
  <li>the canonical JSON IR boundary remains explicit and category-oriented,</li>
  <li>the execution unit carries explicit structural categories including objects, connections, regions, source_map, and correspondence,</li>
  <li>schema validity is necessary but not sufficient for full canonical IR correctness,</li>
  <li>schema posture must remain aligned with derivation, identity, recoverability, construction, and later lowering boundaries.</li>
</ul>

<p>
The canonical JSON Execution IR is therefore a real public artifact:
</p>

<ul>
  <li>machine-checkable,</li>
  <li>portable,</li>
  <li>inspectable,</li>
  <li>stable enough for conformance and tooling,</li>
  <li>still distinct from lowered form and backend-private realization.</li>
</ul>

<p>
The same posture now applies explicitly to widget-related execution structure:
</p>

<ul>
  <li>canonical JSON must remain able to represent execution-facing widget distinctions,</li>
  <li>but it must not collapse into widget-package ownership, host-realization payloads, SVG scene graphs, or runtime-private UI structures.</li>
</ul>

<p>
As the repository moves toward a more industrial compilation corridor, this document keeps the structural JSON boundary honest. A future LLVM-oriented route may be downstream. It must still begin from a canonical IR artifact whose JSON shape remains faithful to the open FROG IR boundary.
</p>
