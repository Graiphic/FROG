<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG IR Identity and Mapping</h1>

<p align="center">
  <strong>Normative identity, attribution, correspondence, and recoverability rules for the canonical Execution IR Document</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#boundary-contract">2. Boundary Contract</a></li>
  <li><a href="#scope-of-this-document">3. Scope of This Document</a></li>
  <li><a href="#position-in-the-pipeline">4. Position in the Pipeline</a></li>
  <li><a href="#identity-layers">5. Identity Layers</a></li>
  <li><a href="#general-mapping-model">6. General Mapping Model</a></li>
  <li><a href="#preconditions">7. Preconditions</a></li>
  <li><a href="#required-recoverability">8. Required Recoverability</a></li>
  <li><a href="#mapping-rules">9. Mapping Rules</a></li>
  <li><a href="#document-unit-and-object-identity">10. Document, Unit, and Object Identity</a></li>
  <li><a href="#ports-terminals-connections-and-regions">11. Ports, Terminals, Connections, and Regions</a></li>
  <li><a href="#attribution-and-correspondence-records">12. Attribution and Correspondence Records</a></li>
  <li><a href="#canonical-json-shape-posture">13. Canonical JSON Shape Posture</a></li>
  <li><a href="#allowed-normalization">14. Allowed Normalization</a></li>
  <li><a href="#forbidden-transformations">15. Forbidden Transformations</a></li>
  <li><a href="#relation-with-schema-construction-lowering-and-backend-contract">16. Relation with Schema, Construction, Lowering, and Backend Contract</a></li>
  <li><a href="#relation-with-observation-and-debugging">17. Relation with Observation and Debugging</a></li>
  <li><a href="#widget-object-and-ui-recoverability">18. Widget Object and UI Recoverability</a></li>
  <li><a href="#examples">19. Examples</a></li>
  <li><a href="#out-of-scope-for-v01">20. Out of Scope for v0.1</a></li>
  <li><a href="#summary">21. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the identity and mapping rules that connect:
</p>

<ul>
  <li>validated source-visible contributors,</li>
  <li>validated program meaning,</li>
  <li>the canonical Execution IR Document,</li>
  <li>execution-facing IR identities inside that document.</li>
</ul>

<p>
Its purpose is to ensure that the canonical open Execution IR remains:
</p>

<ul>
  <li>source-attributable,</li>
  <li>semantically grounded,</li>
  <li>recoverable for inspection and tooling,</li>
  <li>usable for lowering without collapsing into private runtime form.</li>
</ul>

<p>
Identity in the canonical Execution IR is not decoration. It is part of correctness.
</p>

<p>
A conforming Execution IR representation must therefore answer questions such as:
</p>

<ul>
  <li>Which validated contributor or contributors gave rise to this IR-side object?</li>
  <li>Is this IR-side object primary, support-only, or recoverable only through correspondence?</li>
  <li>Does this record represent public-interface participation, ordinary connectivity, explicit state, UI participation, structure-boundary participation, or something else?</li>
  <li>Which distinctions must still remain recoverable before lowering and backend-facing handoff?</li>
</ul>

<p>
This document exists so that canonical IR identity does not drift into:
</p>

<ul>
  <li>undocumented implementation convenience,</li>
  <li>opaque runtime-private handles,</li>
  <li>position-based coincidence,</li>
  <li>silent category collapse,</li>
  <li>loss of source attribution or non-primary correspondence.</li>
</ul>

<p>
The same rule applies explicitly to widget-related execution structure. Canonical open IR identity must remain able to distinguish execution-facing widget roles from widget-package ownership, host realization, and runtime-private UI storage.
</p>

<hr/>

<h2 id="boundary-contract">2. Boundary Contract</h2>

<p>
Every execution-facing IR object MUST be attributable to validated program meaning.
</p>

<p>
Every conforming mapping MUST allow recovery of:
</p>

<ul>
  <li>its validated source-visible contributor or contributors,</li>
  <li>its semantic role,</li>
  <li>its derivation relation,</li>
  <li>its object-family classification where relevant,</li>
  <li>its place inside the canonical Execution IR Document.</li>
</ul>

<p>
Identity is therefore not optional metadata. It is part of the correctness of the canonical open Execution IR.
</p>

<pre><code>valid canonical IR
   =
correct execution-facing correspondence
+
recoverable identity
+
recoverable attribution
+
recoverable non-primary correspondence where required</code></pre>

<p>
A conforming implementation MUST NOT treat identity as disposable convenience data that may be dropped once derivation succeeds.
</p>

<p>
A conforming implementation MUST NOT rely on later runtime-private structure to restore distinctions that were already required at the canonical open-IR boundary.
</p>

<p>
This includes widget-related distinctions such as:
</p>

<ul>
  <li><code>widget_value</code> participation,</li>
  <li><code>widget_reference</code> participation,</li>
  <li>standardized UI-object primitive operation,</li>
  <li>member-addressed access,</li>
  <li>intentional non-primary widget-package correspondence.</li>
</ul>

<hr/>

<h2 id="scope-of-this-document">3. Scope of This Document</h2>

<p>
This document defines:
</p>

<ul>
  <li>identity classes relevant to the canonical Execution IR Document,</li>
  <li>mapping obligations from validated contributors to IR-side records,</li>
  <li>recoverability requirements for attribution, correspondence, and role distinctions,</li>
  <li>allowed and forbidden normalization behavior at the canonical open-IR boundary,</li>
  <li>the identity-facing relation with schema, construction, lowering, and backend-facing handoff.</li>
</ul>

<p>
This document does not define:
</p>

<ul>
  <li>source-shape law,</li>
  <li>semantic validity by itself,</li>
  <li>the full construction procedure for materially building IR payloads,</li>
  <li>the machine-checkable JSON schema itself,</li>
  <li>backend-family-specific private identity models,</li>
  <li>runtime activation identities, scheduler tokens, or internal debugging handles,</li>
  <li>the full widget-package architecture,</li>
  <li>host realization identity models.</li>
</ul>

<p>
Accordingly:
</p>

<pre><code>Expression/
   owns source structure

Language/
   owns validated meaning

IR/Derivation rules.md
   owns semantic-to-IR derivation law

This document
   owns identity, attribution, correspondence, and recoverability law
   at the canonical open-IR boundary

IR/Construction rules.md
   owns material IR construction requirements

IR/Schema.md
   owns canonical machine-checkable payload posture

IR/Lowering.md and IR/Backend contract.md
   own later downstream-facing specialization boundaries</code></pre>

<hr/>

<h2 id="position-in-the-pipeline">4. Position in the Pipeline</h2>

<p>
Identity and mapping obligations apply after validated meaning is established and before canonical IR is allowed to collapse into later downstream forms.
</p>

<pre><code>canonical source
   -&gt;
structural validity
   -&gt;
validated program meaning
   -&gt;
identity-preserving canonical Execution IR
   -&gt;
later lowering
   -&gt;
backend-facing handoff
   -&gt;
private realization</code></pre>

<p>
This document therefore sits at the boundary where open execution-facing representation becomes inspectable, attributable, and portable across implementations.
</p>

<p>
It is the point where the canonical Execution IR must remain:
</p>

<ul>
  <li>grounded in validated meaning,</li>
  <li>inspectable as an open artifact,</li>
  <li>recoverable enough for conformance, diagnostics, and observability,</li>
  <li>suitable for later lowering without erasing upstream truth.</li>
</ul>

<p>
This is especially important for a serious compiler corridor. A downstream backend consumer may be LLVM-oriented or otherwise target-oriented. That downstream consumer still remains downstream from the canonical open-IR identity boundary.
</p>

<hr/>

<h2 id="identity-layers">5. Identity Layers</h2>

<p>
Identity in the canonical Execution IR is layered. A conforming implementation MUST preserve enough structure to distinguish the following layers whenever they are relevant.
</p>

<h3>5.1 Source-visible contributor identity</h3>

<p>
This is the identity of the validated source-visible contributor as recognized by the source and language layers. It is not yet execution-facing IR identity.
</p>

<p>
Examples include:
</p>

<ul>
  <li>a validated diagram node,</li>
  <li>a validated interface declaration participant,</li>
  <li>a validated widget declaration participant when such participation is execution-relevant,</li>
  <li>a validated structure or structure-terminal contributor,</li>
  <li>a validated explicit local-memory contributor.</li>
</ul>

<h3>5.2 Validated semantic contributor identity</h3>

<p>
This is the contributor identity as admitted by validated program meaning. Semantic validation may reject, constrain, or reinterpret source-visible material before canonical IR is allowed to exist.
</p>

<p>
No canonical IR identity can compensate for missing semantic legality upstream.
</p>

<h3>5.3 Canonical Execution IR document identity</h3>

<p>
This is the identity of the top-level canonical Execution IR artifact for one validated FROG program.
</p>

<p>
In base v0.1:
</p>

<ul>
  <li>one validated FROG program yields one canonical Execution IR Document,</li>
  <li>that document carries its own identity,</li>
  <li>that document contains one execution unit.</li>
</ul>

<p>
Document identity is distinct from source identity and distinct from execution-object identity. It identifies the open IR artifact itself.
</p>

<h3>5.4 Open Execution IR unit and object identity</h3>

<p>
This is the identity of the execution unit and of the objects inside it:
</p>

<ul>
  <li>unique within the relevant document or unit scope,</li>
  <li>stable for the lifetime of that IR instance,</li>
  <li>mapped to validated meaning through explicit correspondence.</li>
</ul>

<p>
IR identity is execution-facing representation identity. It is not yet runtime activation identity.
</p>

<h3>5.5 Explicit attribution and correspondence record identity</h3>

<p>
At the canonical open-IR boundary, attribution and correspondence are not merely implied concepts. Where they are materialized as explicit records, those records carry their own identity-bearing role inside the document.
</p>

<p>
In base v0.1, the preferred canonical JSON posture is:
</p>

<ul>
  <li><code>unit.source_map</code> as an explicit record array,</li>
  <li><code>unit.correspondence</code> as an explicit record array.</li>
</ul>

<h3>5.6 Downstream-added private identity</h3>

<p>
Later lowering, backend handoff, runtime realization, or debugging systems MAY introduce additional identities. Those identities are not canonical open-IR identity.
</p>

<p>
They MUST remain downstream additions. They MUST NOT be retroactively treated as though they were the canonical identity law of FROG.
</p>

<h3>5.7 Widget-related execution identity</h3>

<p>
When widget-related execution structure is present, identity layering MUST also distinguish:
</p>

<ul>
  <li>widget instance contributor identity,</li>
  <li>widget-related execution-facing carrier identity,</li>
  <li>member-address identity where applicable,</li>
  <li>UI-operation identity where applicable,</li>
  <li>non-primary widget-package correspondence identity where preserved.</li>
</ul>

<p>
These layers are not identical to host widget identities, platform control identities, or runtime UI object handles.
</p>

<hr/>

<h2 id="general-mapping-model">6. General Mapping Model</h2>

<p>
The canonical mapping model is not limited to a one-source-object to one-IR-object rule.
</p>

<p>
A validated contributor MAY map to:
</p>

<ul>
  <li>one primary IR object,</li>
  <li>multiple attributable IR objects,</li>
  <li>one primary object plus support objects,</li>
  <li>no primary object but an intentional non-primary correspondence record.</li>
</ul>

<p>
Likewise, one IR object MAY arise from:
</p>

<ul>
  <li>one validated contributor,</li>
  <li>multiple contributors where the semantic law permits multi-contributor attribution,</li>
  <li>a support-only construction introduced by canonicalization, provided attribution remains recoverable.</li>
</ul>

<p>
The general model is therefore:
</p>

<pre><code>validated contributor
   -&gt; primary IR object
or -&gt; support IR object(s)
or -&gt; intentional non-primary correspondence

multiple validated contributors
   -&gt; attributed IR-side consolidation where semantically permitted

never
   -&gt; unattributable opaque execution-facing object</code></pre>

<p>
This means:
</p>

<ul>
  <li>one-to-one mapping is allowed,</li>
  <li>one-to-many mapping is allowed when recoverable,</li>
  <li>many-to-one mapping is allowed when recoverable and semantically lawful,</li>
  <li>absence of a primary execution object is allowed only when intentional non-primary correspondence remains explicit where relevant.</li>
</ul>

<p>
A conforming implementation MUST NOT use accidental omission as a substitute for explicit non-primary correspondence.
</p>

<p>
For widget-related execution structure, the same general model applies:
</p>

<ul>
  <li>a validated widget interaction may yield one primary operation object,</li>
  <li>it may also yield support-side addressing records,</li>
  <li>widget-package detail may remain non-primary but corresponded,</li>
  <li>no widget-related execution-facing object may remain unattributable.</li>
</ul>

<hr/>

<h2 id="preconditions">7. Preconditions</h2>

<p>
Identity-preserving canonical Execution IR requires:
</p>

<ul>
  <li>complete validation for the accepted subset,</li>
  <li>type correctness under the accepted semantic rules,</li>
  <li>structure correctness,</li>
  <li>cycle legality,</li>
  <li>boundary consistency,</li>
  <li>resolved execution-relevant distinctions at the semantic boundary.</li>
</ul>

<pre><code>no validation
   -&gt;
no conforming identity-preserving canonical Execution IR</code></pre>

<p>
A conforming implementation MUST NOT use open-IR identity to compensate for missing semantic validation upstream.
</p>

<p>
In particular, canonical IR identity MUST NOT be used to smuggle in:
</p>

<ul>
  <li>inferred memory where explicit state was required,</li>
  <li>structure participation where no valid structure-boundary relation existed,</li>
  <li>UI sequencing where only ordinary connectivity existed,</li>
  <li>public-interface participation where the source only described front-panel content,</li>
  <li>backend-driven interpretation of an otherwise semantically unresolved construct,</li>
  <li>widget realization meaning where only widget execution participation was validated.</li>
</ul>

<hr/>

<h2 id="required-recoverability">8. Required Recoverability</h2>

<p>
The following distinctions MUST remain recoverable whenever present and relevant:
</p>

<ul>
  <li>document identity versus execution-unit identity,</li>
  <li><code>interface_input</code> versus <code>interface_output</code>,</li>
  <li><code>widget_value</code> versus <code>widget_reference</code>,</li>
  <li>standardized UI-object primitive versus widget-reference participation,</li>
  <li>public interface participation versus UI participation,</li>
  <li><code>widget_value</code> participation versus property-based access to member <code>value</code>,</li>
  <li>structure family identity,</li>
  <li>regions and structure-owned boundaries,</li>
  <li>structure-terminal roles,</li>
  <li>explicit local memory identity,</li>
  <li>explicit initialization versus inferred defaulting,</li>
  <li>ordinary connectivity versus structure-boundary participation,</li>
  <li>ordinary connectivity versus public-interface-boundary participation,</li>
  <li>ordinary connectivity versus explicit state participation,</li>
  <li>ordinary connectivity versus explicit UI sequencing,</li>
  <li>structure-boundary participation versus public-interface-boundary participation,</li>
  <li>structure-boundary participation versus explicit UI sequencing,</li>
  <li>explicit state participation versus public-interface-boundary participation,</li>
  <li>explicit state participation versus explicit UI sequencing,</li>
  <li>sub-FROG invocation identity and callable boundary,</li>
  <li>primary versus support versus non-primary roles,</li>
  <li>direct attribution versus multi-contributor attribution where that distinction matters,</li>
  <li>intentional non-primary correspondence versus accidental identity loss,</li>
  <li>declaration-reference correspondence versus primary execution-object identity.</li>
</ul>

<pre><code>recoverability set
   =
minimal invariant surface
for canonical Execution IR correspondence</code></pre>

<p>
These recoverability requirements are the minimum architectural surface that later lowering, observability, conformance reasoning, and fault attribution must not silently destroy.
</p>

<p>
Recoverability does not require that every contributor become a top-level primary object. It does require that the relevant relation remain inspectable and unambiguous.
</p>

<p>
For widget-related execution structure, recoverability also includes, where relevant:
</p>

<ul>
  <li>widget identity versus member-address identity,</li>
  <li>widget reference versus UI primitive operation,</li>
  <li>member-addressed access versus root-level widget access,</li>
  <li>execution-facing widget interaction versus non-primary widget realization correspondence.</li>
</ul>

<hr/>

<h2 id="mapping-rules">9. Mapping Rules</h2>

<h3>9.1 Objects</h3>

<ul>
  <li>Primary execution-facing objects MUST be directly attributable to validated contributors or validated contributor groups allowed by this specification.</li>
  <li>Support objects MUST be attributable to their contributing validated objects.</li>
  <li>Non-primary source-visible contributors MAY remain outside the set of primary execution objects, but their correspondence obligations MUST remain recoverable where relevant.</li>
</ul>

<h3>9.2 Boundaries</h3>

<ul>
  <li>Public-interface-boundary participation MUST remain recoverable as public-interface-boundary participation.</li>
  <li>Structure-boundary participation MUST remain recoverable as structure-boundary participation.</li>
  <li>Explicit state-boundary participation MUST remain recoverable as explicit state participation.</li>
  <li>Explicit UI sequencing participation MUST remain recoverable as explicit UI sequencing participation.</li>
</ul>

<p>
A conforming implementation MUST NOT merge these categories into one undifferentiated boundary class.
</p>

<h3>9.3 Structure ownership</h3>

<ul>
  <li>Structure-owned regions MUST remain attributable to their owning structure.</li>
  <li>Structure terminals MUST remain attributable to their owning structure and role.</li>
  <li>Flattening or normalization MAY change representation detail, but MUST NOT destroy family, region, or terminal recoverability.</li>
</ul>

<h3>9.4 State</h3>

<ul>
  <li>Explicit local memory contributors MUST remain recoverable as explicit local memory.</li>
  <li>Initialization carriers MUST remain distinguishable from ordinary data contributors.</li>
  <li>Read-side and write-side state participation MUST remain recoverable when that distinction is semantically relevant.</li>
</ul>

<p>
A conforming implementation MUST NOT present inferred persistence as though it had direct validated source origin.
</p>

<h3>9.5 Connectivity</h3>

<ul>
  <li>Ordinary dataflow connectivity MUST remain distinguishable from boundary participation.</li>
  <li>Adjacency or layout coincidence MUST NOT become a substitute for explicit attributable connectivity.</li>
  <li>Connection-side support records MAY be introduced, but MUST remain attributable and role-safe.</li>
</ul>

<h3>9.6 UI participation</h3>

<ul>
  <li>UI-related participation objects MUST preserve their semantic role distinctions.</li>
  <li>Widget declarations referenced by participation objects MUST remain recoverably linked to those participation objects.</li>
  <li>Standardized UI-object primitive operations MUST remain distinct from both widget declaration identity and widget-reference participation identity.</li>
</ul>

<h3>9.7 Attribution and non-primary correspondence</h3>

<ul>
  <li>Attribution carriers MUST preserve which validated contributor or contributors gave rise to an IR-side record.</li>
  <li>Correspondence carriers MUST preserve whether a source-visible contributor became a primary object, a support object, a declaration reference, or an intentional non-primary outcome.</li>
  <li>A conforming implementation MUST NOT rely on absence alone where the distinction between intentional non-primary outcome and accidental loss would otherwise become ambiguous.</li>
</ul>

<h3>9.8 Widget-related recoverability rules</h3>

<ul>
  <li>Execution-facing widget interaction MUST remain linked to the relevant widget contributor identity.</li>
  <li>When a member or part is addressed, that addressed target MUST remain recoverable at execution-facing level.</li>
  <li>Widget package, realization package, SVG, and host-private realization information MAY remain non-primary, but their relation to validated execution-facing use MUST remain correspondable where needed.</li>
</ul>

<hr/>

<h2 id="document-unit-and-object-identity">10. Document, Unit, and Object Identity</h2>

<p>
The canonical Execution IR uses layered identity so that different questions can be answered without ambiguity.
</p>

<pre><code>canonical Execution IR Document
└── document identity
    └── execution unit identity
        ├── object identity
        ├── connection identity
        ├── region identity
        ├── source_map record anchoring where applicable
        └── correspondence record identity where applicable</code></pre>

<p>
This layered identity model exists so that tools can answer different questions without ambiguity:
</p>

<ul>
  <li>Which canonical IR artifact is this?</li>
  <li>Which execution unit does this record belong to?</li>
  <li>Which execution-facing object is this?</li>
  <li>Is this object primary, support, or only referenced through correspondence?</li>
</ul>

<p>
A conforming implementation MUST NOT collapse:
</p>

<ul>
  <li>document identity,</li>
  <li>unit identity,</li>
  <li>object identity</li>
</ul>

<p>
into one ambiguous identifier role.
</p>

<p>
Identifier syntax MAY vary across conforming implementations. Identity role ambiguity MUST NOT.
</p>

<p>
For widget-related execution structure, this same rule implies:
</p>

<ul>
  <li>widget identity anchors are not document identity,</li>
  <li>member-address descriptors are not object identity by default,</li>
  <li>UI-operation identity is not interchangeable with widget-reference carrier identity.</li>
</ul>

<hr/>

<h2 id="ports-terminals-connections-and-regions">11. Ports, Terminals, Connections, and Regions</h2>

<p>
Ports, terminals, connections, and regions are execution-facing carriers of structure. Their identity posture must remain sufficient for recoverability.
</p>

<ul>
  <li>Ports or terminal-like records MUST remain attributable to the object or structure role they belong to.</li>
  <li>Connections MUST remain attributable as execution-facing relations, not as mere positional adjacency.</li>
  <li>Regions MUST remain attributable to their owning structure where regions are semantically relevant.</li>
  <li>Structure terminals MUST remain role-distinguishable from ordinary ports.</li>
</ul>

<p>
If a conforming implementation chooses not to materialize a given helper category as a first-class record, equivalent recoverability MUST still remain possible through explicit references or explicit structured membership.
</p>

<pre><code>object
  ├── ports / terminals
  ├── region membership where applicable
  └── attributable connectivity

structure
  ├── owned regions
  ├── boundary roles
  └── structure-terminal roles</code></pre>

<p>
This rule exists so that canonical open IR stays implementation-portable without forcing one single private internal layout.
</p>

<p>
For widget-related execution structure, the same logic applies to:
</p>

<ul>
  <li>member-address carriers,</li>
  <li>UI-operation attachment points,</li>
  <li>widget-related boundary distinctions where applicable.</li>
</ul>

<hr/>

<h2 id="attribution-and-correspondence-records">12. Attribution and Correspondence Records</h2>

<p>
Attribution and correspondence are not merely diagnostic conveniences. They are part of the canonical open IR boundary.
</p>

<p>
At minimum, a conforming canonical Execution IR representation MUST support:
</p>

<ul>
  <li>attribution — which validated contributor or contributors gave rise to this IR-side record,</li>
  <li>correspondence — how a validated source-visible contributor relates to primary objects, support objects, or intentional non-primary outcomes.</li>
</ul>

<p>
Accordingly:
</p>

<ul>
  <li>primary objects SHOULD expose direct attribution,</li>
  <li>support objects MUST expose contributor attribution,</li>
  <li>non-primary source-visible contributors that still matter for recoverability MUST remain visible through correspondence,</li>
  <li>in the preferred canonical JSON posture of base v0.1, those carriers appear explicitly through <code>unit.source_map</code> and <code>unit.correspondence</code> record arrays,</li>
  <li>inline attribution on objects or connections MAY coexist with those arrays where the published schema and construction rules permit it.</li>
</ul>

<pre><code>primary object
   -&gt; direct attribution

support object
   -&gt; contributor attribution

non-primary source-visible contributor
   -&gt; correspondence without forced primary execution identity

preferred canonical JSON carriers
   -&gt; source_map[]
   -&gt; correspondence[]</code></pre>

<p>
A conforming implementation MUST NOT rely on undocumented ordering conventions or positional coincidence as the only mapping mechanism.
</p>

<p>
Source provenance is not an IR identity system and does not become IR semantics. However, when a source-visible contributor is covered by <code>ide.provenance</code>, identity and mapping carriers SHOULD remain sufficient for tools to navigate from execution-facing records back to the provenance-covered source object where such a source object contributed to validated meaning.
</p>

<p>
For widget-related execution structure, attribution and correspondence records SHOULD be able to represent, where relevant:
</p>

<ul>
  <li>widget instance contributor,</li>
  <li>widget member-address contributor,</li>
  <li>UI primitive operation contributor,</li>
  <li>intentional non-primary widget-package correspondence,</li>
  <li>intentional non-primary realization-package correspondence.</li>
</ul>

<hr/>

<h2 id="canonical-json-shape-posture">13. Canonical JSON Shape Posture</h2>

<p>
The canonical JSON Execution IR form SHOULD expose or imply information equivalent to:
</p>

<ul>
  <li>document identity,</li>
  <li>unit identity,</li>
  <li>IR-side object identity,</li>
  <li>object classification,</li>
  <li>mapping relation,</li>
  <li>semantic origin identity,</li>
  <li>source attribution,</li>
  <li>correspondence where needed.</li>
</ul>

<pre><code>{
  "document_id": "...",
  "unit": {
    "id": "...",
    "objects": [
      {
        "id": "...",
        "family": "...",
        "role": "...",
        "attribution": { ... },
        "correspondence_refs": [ ... ]
      }
    ],
    "source_map": [ ... ],
    "correspondence": [ ... ]
  }
}</code></pre>

<p>
In base v0.1, the preferred canonical JSON posture is explicit record arrays for <code>source_map</code> and <code>correspondence</code>. That posture keeps the open IR inspection-friendly, schema-checkable, and portable across implementations.
</p>

<p>
The exact field names above are illustrative only. What matters here is the recoverable presence of equivalent information. The machine-checkable schema layer owns the exact published payload validation surface.
</p>

<p>
A schema-valid payload is not automatically architecturally sufficient unless the required recoverability surface is actually preserved.
</p>

<p>
For widget-related execution structure, canonical JSON posture SHOULD remain able to encode, where required:
</p>

<ul>
  <li>widget-related object identity,</li>
  <li>member-address descriptors,</li>
  <li>UI-operation identity,</li>
  <li>non-primary widget-package correspondence,</li>
  <li>without introducing host-realization payload families as canonical requirements.</li>
</ul>

<hr/>

<h2 id="allowed-normalization">14. Allowed Normalization</h2>

<p>
The following are allowed when semantic equivalence and recoverability are preserved:
</p>

<ul>
  <li>making ports, types, and directions explicit,</li>
  <li>making document and unit identity explicit,</li>
  <li>making regions and terminals explicit,</li>
  <li>adding support objects,</li>
  <li>introducing canonical IR identifiers,</li>
  <li>expanding one validated contributor into multiple attributable IR-side support objects,</li>
  <li>normalizing equivalent validated forms into one canonical execution-facing representation,</li>
  <li>materializing explicit attribution and correspondence record arrays compatible with the published canonical JSON posture.</li>
</ul>

<p>
These normalizations are allowed only if all of the following remain true:
</p>

<ul>
  <li>semantic equivalence is preserved,</li>
  <li>attribution is preserved,</li>
  <li>required distinctions are preserved,</li>
  <li>explicit memory remains explicit,</li>
  <li>structured control remains recoverable,</li>
  <li>category-safe downstream handoff remains possible.</li>
</ul>

<p>
Canonicalization of identifier syntax is permitted. Loss of identity relation is not.
</p>

<p>
Lowering-readiness is allowed to motivate additional explicit carriers. It is not allowed to justify early loss of required open-IR recoverability.
</p>

<p>
For widget-related execution structure, allowed normalization includes:
</p>

<ul>
  <li>introducing explicit widget identity anchors,</li>
  <li>introducing explicit member-address carriers,</li>
  <li>introducing explicit UI-operation role tags,</li>
  <li>normalizing equivalent validated widget accesses into one canonical execution-facing representation.</li>
</ul>

<p>
It does not permit:
</p>

<ul>
  <li>rewriting canonical widget interaction into one runtime's native API vocabulary,</li>
  <li>replacing widget-member addressing with host-widget-tree paths,</li>
  <li>using SVG node identity as the canonical open-IR identity law.</li>
</ul>

<hr/>

<h2 id="forbidden-transformations">15. Forbidden Transformations</h2>

<p>
The following are forbidden:
</p>

<ul>
  <li>loss of attribution for execution-facing objects,</li>
  <li>loss of explicit correspondence where non-primary outcomes remain relevant,</li>
  <li>opaque object collapse that destroys contributor recoverability,</li>
  <li>hidden memory insertion presented as though it had validated source origin,</li>
  <li>collapse of interface participation and UI participation into one undifferentiated identity class,</li>
  <li>collapse of <code>widget_value</code> participation and property-based access to member <code>value</code> into one undifferentiated identity class,</li>
  <li>collapse of document identity, unit identity, and object identity into one ambiguous identifier role,</li>
  <li>structure flattening that destroys family, region, or terminal recoverability,</li>
  <li>promotion of editor-only state into execution-facing identity,</li>
  <li>forced executionization of non-execution source content merely because it exists in source,</li>
  <li>rewriting support objects as though they were independently authored semantic truth,</li>
  <li>treating one runtime-private identity model as though it were the canonical open IR identity model,</li>
  <li>using backend-family convenience to erase a distinction that the canonical open IR still requires,</li>
  <li>using lowering pressure to erase intentional non-primary correspondence before the permitted downstream boundary,</li>
  <li>collapsing widget identity, widget-reference participation, and UI-operation identity into one undifferentiated UI identity class,</li>
  <li>promoting widget realization packages, SVG assets, host-native controls, or runtime-private UI handles into canonical execution identity without explicit language-owned standardization.</li>
</ul>

<pre><code>forbidden
   =
anything that breaks semantic traceability
or destroys required recoverability</code></pre>

<hr/>

<h2 id="relation-with-schema-construction-lowering-and-backend-contract">16. Relation with Schema, Construction, Lowering, and Backend Contract</h2>

<p>
This document defines recoverability obligations at the canonical open-IR boundary. Schema, construction, lowering, and backend contract remain distinct concerns.
</p>

<p>
Accordingly:
</p>

<ul>
  <li>construction MUST materialize the required identity and mapping carriers,</li>
  <li>schema MAY validate the structural presence of those carriers where published,</li>
  <li>lowering MAY refine or expand identity,</li>
  <li>backend contract MAY carry consumer-facing identity anchors,</li>
  <li>runtime or backend-private realization MAY introduce private handles or activation identities,</li>
  <li>but none of those stages may erase required recoverability while still claiming faithful downstream correspondence.</li>
</ul>

<p>
In particular:
</p>

<ul>
  <li><code>IR/Construction rules.md</code> owns how conforming canonical IR payloads are materially built,</li>
  <li><code>IR/Schema.md</code> owns how canonical JSON payload structure is machine-checked,</li>
  <li><code>IR/Lowering.md</code> owns how canonical open IR moves toward target-oriented specialization,</li>
  <li><code>IR/Backend contract.md</code> owns how downstream consumers receive backend-facing handoff information.</li>
</ul>

<p>
This document therefore enforces the identity-side rule:
</p>

<pre><code>canonical open-IR recoverability
must survive
until the specification explicitly permits a later downstream boundary</code></pre>

<p>
This matters directly to a compiler corridor. A backend-oriented consumer may later compress, specialize, or reframe execution structures. That consumer must still start from an IR whose identity, attribution, and category distinctions were valid at the canonical boundary.
</p>

<p>
The same applies to UI-aware downstream consumers:
</p>

<ul>
  <li>Python, Rust, C/C++, or mixed runtime families may later realize widget interaction differently,</li>
  <li>but they all remain downstream from the same canonical open-IR recoverability boundary.</li>
</ul>

<hr/>

<h2 id="relation-with-observation-and-debugging">17. Relation with Observation and Debugging</h2>

<p>
Open IR identity and mapping exist partly so that observation, debugging, diagnostics, and conformance analysis can speak about the same execution-facing artifact without ambiguity.
</p>

<p>
This does not mean that:
</p>

<ul>
  <li>the canonical Execution IR must contain every debugger-private concept,</li>
  <li>every observation point must become a canonical IR object,</li>
  <li>runtime-private stepping handles become normative.</li>
</ul>

<p>
It does mean that canonical IR identity must remain sufficient so that:
</p>

<ul>
  <li>errors can be attributed,</li>
  <li>recoverable source-facing explanations remain possible,</li>
  <li>structure ownership and boundary roles remain understandable,</li>
  <li>state participation remains diagnosable,</li>
  <li>future observation surfaces do not need to invent upstream identity after the fact.</li>
</ul>

<p>
For widget-related execution structure, this also means:
</p>

<ul>
  <li>diagnostics can distinguish widget-value flow from widget-reference flow,</li>
  <li>diagnostics can distinguish reference carrying from property or method operation,</li>
  <li>diagnostics do not need host-native widget handles to recover canonical execution intent.</li>
</ul>

<hr/>

<h2 id="widget-object-and-ui-recoverability">18. Widget Object and UI Recoverability</h2>

<p>
The widget object system introduces a particularly important recoverability corridor because it crosses:
</p>

<ul>
  <li>canonical source,</li>
  <li>class-law resolution,</li>
  <li>diagram-side executable interaction,</li>
  <li>open execution IR,</li>
  <li>later lowering,</li>
  <li>runtime and host realization.</li>
</ul>

<p>
At the canonical open-IR boundary, the following widget-related distinctions MUST remain recoverable whenever relevant:
</p>

<ul>
  <li>widget declaration identity versus widget execution participation identity,</li>
  <li><code>widget_value</code> participation versus <code>widget_reference</code> participation,</li>
  <li><code>widget_reference</code> participation versus standardized UI-object primitive operation,</li>
  <li>root widget access versus member-addressed access,</li>
  <li>member-addressed access versus host-visual-node addressing,</li>
  <li>execution-facing widget interaction versus non-primary realization-package correspondence.</li>
</ul>

<p>
The open-IR identity layer MAY preserve:
</p>

<ul>
  <li>widget identity anchors,</li>
  <li>member-address descriptors,</li>
  <li>UI-operation family identity,</li>
  <li>non-primary widget-package correspondence where needed.</li>
</ul>

<p>
It MUST NOT require:
</p>

<ul>
  <li>host control identities,</li>
  <li>platform widget pointers,</li>
  <li>SVG node identifiers as semantic truth,</li>
  <li>runtime-private UI object tables.</li>
</ul>

<pre><code>canonical widget interaction recoverability
   =
widget execution-facing identity
 + addressed-access recoverability
 + operation-family recoverability
 - host-private realization dependency</code></pre>

<hr/>

<h2 id="examples">19. Examples</h2>

<h3>19.1 One validated contributor becomes one primary IR object</h3>

<pre><code>validated contributor: pure arithmetic primitive node
   -&gt;
primary IR object: canonical arithmetic object
   -&gt;
direct attribution preserved</code></pre>

<h3>19.2 One validated contributor becomes one primary object plus support objects</h3>

<pre><code>validated contributor: explicit local-memory construct
   -&gt;
primary IR object: memory participation object
   +
support IR object: initialization carrier
   +
support IR object: state-boundary carrier

all remain attributable to validated meaning</code></pre>

<h3>19.3 Source-visible contributor remains non-primary but recoverable</h3>

<pre><code>validated source-visible declaration
   -&gt;
no primary execution object
   -&gt;
correspondence record retained
   -&gt;
intentional non-primary outcome remains explicit</code></pre>

<h3>19.4 Forbidden collapse</h3>

<pre><code>validated widget reference
   +
standardized UI property write primitive
   -&gt;
one undifferentiated "UI object"

Result: non-conforming
Reason: widget-reference participation and standardized UI-object primitive role were collapsed</code></pre>

<h3>19.5 Forbidden backend-driven early erasure</h3>

<pre><code>canonical IR preserves explicit state participation
   -&gt;
implementation immediately rewrites it as opaque backend-private storage
   -&gt;
source attribution and state-boundary recoverability are lost before the permitted downstream boundary

Result: non-conforming at the canonical open-IR identity layer</code></pre>

<h3>19.6 Valid widget-related non-primary correspondence</h3>

<pre><code>validated widget interaction uses class-law-resolved addressing
   -&gt;
primary IR object: UI property write operation
   -&gt;
support record: member-address descriptor
   -&gt;
correspondence record: realization package remains non-primary

Result: conforming
Reason: execution-facing access is preserved while realization ownership remains downstream</code></pre>

<hr/>

<h2 id="out-of-scope-for-v01">20. Out of Scope for v0.1</h2>

<p>
The following are out of scope for this document in base v0.1:
</p>

<ul>
  <li>full runtime activation identity systems,</li>
  <li>scheduler-internal token models,</li>
  <li>thread, fiber, or task-private identity semantics,</li>
  <li>backend-family-specific object identity conventions,</li>
  <li>full source-to-machine-code debug information standards,</li>
  <li>persistent cross-build identity guarantees,</li>
  <li>multi-unit canonical IR documents,</li>
  <li>one mandatory runtime UI object ABI,</li>
  <li>one mandatory host-side event identity taxonomy.</li>
</ul>

<p>
Those may appear later. They do not weaken the current obligations at the canonical open-IR boundary.
</p>

<hr/>

<h2 id="summary">21. Summary</h2>

<p>
This document defines the identity and mapping law of the canonical Execution IR Document.
</p>

<p>
Its core rules are:
</p>

<ul>
  <li>every execution-facing IR object must be attributable to validated meaning,</li>
  <li>required distinctions must remain recoverable,</li>
  <li>attribution and correspondence are part of the canonical open-IR boundary,</li>
  <li>normalization is allowed only when semantic traceability survives,</li>
  <li>runtime-private or backend-private identity models remain downstream and non-canonical.</li>
</ul>

<p>
The canonical open IR is therefore not only execution-facing. It is also:
</p>

<ul>
  <li>identity-bearing,</li>
  <li>recoverable,</li>
  <li>portable across conforming implementations,</li>
  <li>suitable for conformance, tooling, observability, and later lowering.</li>
</ul>

<p>
A serious downstream compilation path depends on this discipline. FROG may later lower toward backend-facing consumers, including LLVM-oriented routes. That downstream path must still begin from a canonical Execution IR whose attribution, correspondence, and recoverability were preserved correctly.
</p>

<p>
The same discipline now applies explicitly to widget-related execution structure:
</p>

<ul>
  <li>execution-facing widget identities and accesses remain recoverable in the canonical open IR,</li>
  <li>but widget-package ownership, realization-package detail, SVG visual structure, and runtime-private UI handles remain downstream and non-canonical.</li>
</ul>
