<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG IR Derivation Rules</h1>

<p align="center">
  <strong>Normative derivation rules from validated FROG program meaning to the canonical Execution IR Document</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#reading-legend">2. Reading Legend</a></li>
  <li><a href="#scope-of-this-document">3. Scope of This Document</a></li>
  <li><a href="#relation-with-other-specifications">4. Relation with Other Specifications</a></li>
  <li><a href="#derivation-boundary">5. Derivation Boundary</a></li>
  <li><a href="#inputs-and-preconditions">6. Inputs and Preconditions</a></li>
  <li><a href="#derivation-result">7. Derivation Result</a></li>
  <li><a href="#core-derivation-invariants">8. Core Derivation Invariants</a></li>
  <li><a href="#derivation-relation-shapes">9. Derivation Relation Shapes</a></li>
  <li><a href="#source-to-ir-family-mapping">10. Source-to-IR Family Mapping</a></li>
  <li><a href="#identity-and-attribution-rules">11. Identity and Attribution Rules</a></li>
  <li><a href="#connectivity-derivation">12. Connectivity Derivation</a></li>
  <li><a href="#structured-control-derivation">13. Structured Control Derivation</a></li>
  <li><a href="#interface-and-ui-derivation">14. Interface and UI Derivation</a></li>
  <li><a href="#widget-object-derivation-corridor">15. Widget Object Derivation Corridor</a></li>
  <li><a href="#state-and-cycle-preservation">16. State and Cycle Preservation</a></li>
  <li><a href="#allowed-normalization-during-derivation">17. Allowed Normalization During Derivation</a></li>
  <li><a href="#forbidden-derivation-outcomes">18. Forbidden Derivation Outcomes</a></li>
  <li><a href="#relation-with-construction-schema-lowering-and-backend-contract">19. Relation with Construction, Schema, Lowering, and Backend Contract</a></li>
  <li><a href="#out-of-scope-for-v01">20. Out of Scope for v0.1</a></li>
  <li><a href="#summary">21. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the normative derivation boundary between validated FROG program meaning and the canonical Execution IR Document.
</p>

<p>
It specifies:
</p>

<ul>
  <li>which validated execution-relevant families derive to execution-facing IR families,</li>
  <li>which validated distinctions MUST remain recoverable at the derivation boundary,</li>
  <li>which support objects MAY be introduced to make already-validated execution structure explicit,</li>
  <li>which source-visible families do not become primary execution objects in the canonical open IR,</li>
  <li>which attribution and correspondence carriers MUST remain preserved and compatible with the canonical JSON IR boundary,</li>
  <li>which transformations are forbidden because they blur ownership, erase recoverability, or introduce downstream-private meaning too early.</li>
</ul>

<p>
This document is intentionally about derivation law. It is not:
</p>

<ul>
  <li>the full source specification,</li>
  <li>the full language semantics specification,</li>
  <li>the complete Execution IR object model specification,</li>
  <li>the full construction procedure of every implementation,</li>
  <li>the canonical schema text itself,</li>
  <li>the lowering strategy of every backend family,</li>
  <li>a runtime-private realization guide.</li>
</ul>

<p>
Compact mental model:
</p>

<pre><code>canonical source
        |
        v
validated program meaning
        |
        v
derivation rules   &lt;-- this document
        |
        v
canonical Execution IR Document
        |
        v
lowering / specialization
        |
        v
backend-facing contract
        |
        v
runtime-private realization</code></pre>

<p>
In base v0.1, derivation is intentionally conservative. It preserves validated meaning, preserves attribution, preserves recoverable identity, preserves explicit structured control, preserves explicit local memory, preserves validated dependency structure, and preserves the distinction between:
</p>

<ul>
  <li>public interface participation,</li>
  <li><code>widget_value</code> participation,</li>
  <li><code>widget_reference</code> participation,</li>
  <li>standardized UI-object primitive operation.</li>
</ul>

<p>
It also preserves the architectural separation between:
</p>

<ul>
  <li>widget instance participation,</li>
  <li>widget class law,</li>
  <li>widget realization packages,</li>
  <li>runtime interpretation,</li>
  <li>host realization.</li>
</ul>

<p>
Execution IR may preserve validated execution-facing consequences of those layers. It does not absorb ownership of all of them.
</p>

<hr/>

<h2 id="reading-legend">2. Reading Legend</h2>

<ul>
  <li>Open specification-facing representation or layer</li>
  <li>Semantic truth, attribution, recoverability, or validation result</li>
  <li>Boundary, correspondence, mapping, or standardized handoff</li>
  <li>Lowering, specialization, or target adaptation zone</li>
  <li>Implementation-private or runtime-private realization zone</li>
</ul>

<hr/>

<h2 id="scope-of-this-document">3. Scope of This Document</h2>

<p>
This document defines:
</p>

<ul>
  <li>the normative entry condition for Execution IR derivation,</li>
  <li>the relation shapes permitted at the derivation boundary,</li>
  <li>the base v0.1 correspondence between validated execution-relevant families and Execution IR families,</li>
  <li>the minimum attribution and recoverability obligations that MUST survive derivation,</li>
  <li>the preferred explicit canonical JSON posture for attribution and correspondence carriers,</li>
  <li>the allowed and forbidden kinds of derivation-time explicitness and normalization,</li>
  <li>the rule that one validated FROG program derives to one canonical Execution IR Document containing one execution unit in base v0.1.</li>
</ul>

<p>
This document does not define:
</p>

<ul>
  <li>the canonical source model in full,</li>
  <li>the full normative semantics of validated FROG programs,</li>
  <li>the full Execution IR object model in complete detail,</li>
  <li>the exact material build sequence of every implementation,</li>
  <li>the JSON schema text of the canonical payload,</li>
  <li>the lowering strategy of every backend family,</li>
  <li>the backend contract in full,</li>
  <li>the private scheduler, storage, or ABI policy of any runtime,</li>
  <li>the full widget-package architecture,</li>
  <li>the full host realization model.</li>
</ul>

<pre><code>This document defines:
- what must correspond
- what must remain recoverable
- what may be made explicit during derivation
- what attribution / correspondence posture must remain compatible
- what derivation must not silently change
- that one validated program yields one canonical IR document

This document does not define:
- canonical source in full               -&gt; Expression/
- validated program meaning in full     -&gt; Language/
- full open IR model                    -&gt; Execution IR.md
- material payload construction         -&gt; Construction rules.md
- machine-checkable schema text         -&gt; Schema.md / IR/schema/
- runtime-private realization           -&gt; outside IR ownership
- widget-package ownership              -&gt; Expression/
- host-side realization law             -&gt; Expression/ + downstream runtime/host</code></pre>

<hr/>

<h2 id="relation-with-other-specifications">4. Relation with Other Specifications</h2>

<p>
Ownership remains:
</p>

<pre><code>Expression/                 -&gt; canonical source shape and structural validity
Language/                   -&gt; validated program meaning
IR/Execution IR.md          -&gt; canonical open Execution IR architecture
IR/Derivation rules.md      -&gt; correspondence from meaning to IR
IR/Construction rules.md    -&gt; material IR construction and canonical JSON emission
IR/Schema.md                -&gt; schema posture for canonical IR validation
IR/schema/                  -&gt; machine-checkable schema artifacts
IR/Identity and Mapping.md  -&gt; cross-layer identity, attribution, and recoverability law
IR/Lowering.md              -&gt; later target-oriented specialization
IR/Backend contract.md      -&gt; later backend-facing handoff</code></pre>

<p>
Accordingly:
</p>

<ul>
  <li>this document MUST NOT redefine what a valid <code>.frog</code> file looks like,</li>
  <li>this document MUST NOT redefine what validated meaning is,</li>
  <li>this document MUST NOT redefine the complete canonical IR schema,</li>
  <li>this document MUST NOT redefine construction as a universal procedural algorithm,</li>
  <li>this document MUST NOT redefine backend-private or runtime-private structures,</li>
  <li>this document MUST define how validated meaning becomes the canonical open Execution IR boundary artifact.</li>
</ul>

<p>
This document should be read together with:
</p>

<ul>
  <li><code>Language/Expression to validated meaning.md</code>,</li>
  <li><code>IR/Execution IR.md</code>,</li>
  <li><code>IR/Identity and Mapping.md</code>,</li>
  <li><code>IR/Construction rules.md</code>,</li>
  <li><code>IR/Schema.md</code>,</li>
  <li><code>Expression/Diagram.md</code>,</li>
  <li><code>Expression/Control structures.md</code>,</li>
  <li><code>Expression/State and cycles.md</code>,</li>
  <li><code>Expression/Interface.md</code>,</li>
  <li><code>Expression/Front panel.md</code>,</li>
  <li><code>Expression/Widget.md</code>,</li>
  <li><code>Expression/Widget interaction.md</code>,</li>
  <li><code>Expression/Widget package.md</code>,</li>
  <li><code>Expression/Widget realization.md</code>,</li>
  <li><code>Language/Control structures.md</code>,</li>
  <li><code>Language/State and cycles.md</code>.</li>
</ul>

<hr/>

<h2 id="derivation-boundary">5. Derivation Boundary</h2>

<p>
Derivation is the normative correspondence boundary from validated program meaning to the canonical Execution IR Document.
</p>

<p>
This boundary does not decide whether a program is valid. That decision has already been made upstream. This boundary does not invent missing semantics. It only derives execution-facing representation from already-validated meaning.
</p>

<pre><code>validation decides:
  whether meaning exists

derivation decides:
  how validated meaning becomes the canonical Execution IR Document</code></pre>

<p>
The derivation boundary MUST therefore preserve:
</p>

<ul>
  <li>semantic faithfulness,</li>
  <li>execution-facing explicitness where required,</li>
  <li>source attribution,</li>
  <li>cross-stage recoverability,</li>
  <li>freedom from runtime-private semantic invention.</li>
</ul>

<p>
Derivation is therefore neither:
</p>

<ul>
  <li>a second semantic validation stage,</li>
  <li>a source repair stage,</li>
  <li>a lowering stage,</li>
  <li>a backend-family adaptation stage,</li>
  <li>a runtime realization stage.</li>
</ul>

<p>
The same boundary rule applies to widget-related execution structure:
</p>

<ul>
  <li>derivation may preserve execution-facing widget identities and accesses,</li>
  <li>derivation may not reinterpret widget realization assets or host-private convenience as validated meaning.</li>
</ul>

<hr/>

<h2 id="inputs-and-preconditions">6. Inputs and Preconditions</h2>

<p>
Execution IR derivation begins only after semantic validation has succeeded.
</p>

<p>
The derivation input is not:
</p>

<ul>
  <li>raw serialized source as such,</li>
  <li>merely loadable source,</li>
  <li>merely structurally valid source,</li>
  <li>partially edited authoring state,</li>
  <li>editor convenience state,</li>
  <li>runtime speculation,</li>
  <li>implementation-private scheduler assumptions.</li>
</ul>

<p>
The derivation input is:
</p>

<ul>
  <li>one validated FROG program meaning.</li>
</ul>

<p>
An implementation MAY internally start from canonical source, a validated Program Model, or another validated internal form that is semantically equivalent. However, the canonical Execution IR it derives MUST be grounded in validated meaning rather than in editor-only convenience or runtime-private invention.
</p>

<pre><code>raw source ------------&gt; structural validity ------------&gt; semantic validation ------------&gt; derivation
                         outside this document             outside this document            this document</code></pre>

<p>
Derivation is therefore not the place where an implementation:
</p>

<ul>
  <li>adds hidden scheduler policy,</li>
  <li>adds hidden memory,</li>
  <li>adds opaque backend partitions,</li>
  <li>collapses structured control into private machinery,</li>
  <li>promotes presentation metadata into execution semantics,</li>
  <li>promotes widget realization-package detail into execution semantics.</li>
</ul>

<p>
Preconditions for conforming derivation include:
</p>

<ul>
  <li>semantic acceptance of the program subset being derived,</li>
  <li>resolved execution-relevant type legality,</li>
  <li>resolved structure legality,</li>
  <li>resolved boundary legality,</li>
  <li>resolved cycle legality,</li>
  <li>resolved distinction between execution-relevant and non-execution source content,</li>
  <li>resolved distinction between widget participation meaning and widget realization detail.</li>
</ul>

<hr/>

<h2 id="derivation-result">7. Derivation Result</h2>

<p>
The result of derivation in base v0.1 is one canonical Execution IR Document for one validated FROG program. That document contains one execution unit.
</p>

<p>
That execution unit MUST contain enough information to represent:
</p>

<ul>
  <li>execution-facing object identity,</li>
  <li>object-family classification,</li>
  <li>typed ports or equivalent explicit terminal interfaces,</li>
  <li>directed connectivity,</li>
  <li>structured regions where applicable,</li>
  <li>public boundary participation and UI participation where applicable,</li>
  <li>support objects where needed to make already-validated execution structure explicit,</li>
  <li>mandatory attribution to validated source-visible execution content,</li>
  <li>mandatory correspondence for declaration linkage and intentional non-primary outcomes where required.</li>
</ul>

<p>
The derivation result MUST be:
</p>

<ul>
  <li>semantically equivalent to validated meaning,</li>
  <li>mapping-compatible,</li>
  <li>inspectable,</li>
  <li>compatible with <code>IR/Execution IR.md</code>,</li>
  <li>constructible into the canonical JSON form defined by the IR layer.</li>
</ul>

<pre><code>one validated FROG
        |
        v
one Execution IR Document
        └── one execution unit
            ├── execution-facing objects
            ├── typed ports / explicit terminals
            ├── directed connections
            ├── explicit regions where applicable
            ├── explicit boundary participation
            ├── support objects where required
            ├── source attribution
            └── correspondence where needed</code></pre>

<p>
The derivation result is not yet:
</p>

<ul>
  <li>a lowered target-facing form,</li>
  <li>a backend-facing contract artifact,</li>
  <li>a runtime-private artifact,</li>
  <li>a host-private UI realization graph.</li>
</ul>

<hr/>

<h2 id="core-derivation-invariants">8. Core Derivation Invariants</h2>

<p>
All conforming derivations in base v0.1 MUST satisfy the following invariants:
</p>

<ul>
  <li>Derivation MUST preserve validated language meaning.</li>
  <li>Derivation MUST preserve attribution to validated execution-visible source content.</li>
  <li>Derivation MUST preserve enough structure and identity for later recoverable mapping.</li>
  <li>Derivation MUST preserve explicit memory as explicit memory.</li>
  <li>Derivation MUST preserve structured control as structured control.</li>
  <li>Derivation MUST preserve validated dependency structure.</li>
  <li>Derivation MUST preserve the distinction between public interface participation and UI participation.</li>
  <li>Derivation MUST preserve the distinction between <code>widget_value</code> participation and <code>widget_reference</code> participation.</li>
  <li>Derivation MUST preserve the distinction between <code>widget_reference</code> participation and standardized UI-object primitive operation.</li>
  <li>Derivation MUST preserve the distinction between <code>widget_value</code> participation and property-based access to a widget member named <code>value</code>.</li>
  <li>Derivation MUST preserve the distinction between ordinary connectivity and boundary participation.</li>
  <li>Derivation MUST preserve the distinction between public-interface-boundary participation, structure-boundary participation, explicit state participation, and explicit UI sequencing where those distinctions are semantically relevant.</li>
  <li>Derivation MUST preserve the distinction between direct primary correspondence, support derivation, declaration reference, intentional non-primary outcome, and true non-participation where those distinctions are required for recoverability.</li>
  <li>Derivation MUST produce content compatible with one canonical Execution IR Document for one validated program.</li>
  <li>Derivation MUST NOT reinterpret non-execution source content as execution semantics merely because it exists in canonical source.</li>
  <li>Derivation MUST NOT import editor-only geometry, styling, annotation placement, or similar presentation state as execution semantics.</li>
  <li>Derivation MUST NOT standardize one runtime-private scheduler policy as though it were the open IR.</li>
  <li>Derivation MUST NOT erase attribution or correspondence that the canonical open-IR boundary still requires.</li>
  <li>Derivation MUST NOT collapse widget package ownership, realization-package detail, or SVG visual detail into open execution categories.</li>
</ul>

<pre><code>Core invariants

preserve semantic truth
preserve source attribution
preserve recoverable identity
preserve explicit memory
preserve structured control
preserve validated dependencies
preserve interface / UI distinction
preserve widget_value / widget_reference distinction
preserve widget_reference / UI-primitive distinction
preserve ordinary connectivity / boundary distinction
preserve canonical-document compatibility
preserve non-primary correspondence distinctions where required
do not promote non-execution source content
do not import editor-only state
do not standardize private scheduler policy
do not erase required correspondence early
do not import realization-package detail as execution truth</code></pre>

<hr/>

<h2 id="derivation-relation-shapes">9. Derivation Relation Shapes</h2>

<p>
Base v0.1 distinguishes four broad derivation relation shapes:
</p>

<ul>
  <li><strong>1 -&gt; 1 direct preservation</strong> — one validated execution-relevant contributor derives to one primary IR object,</li>
  <li><strong>1 -&gt; n expansion</strong> — one validated contributor derives to one primary IR object plus one or more support objects,</li>
  <li><strong>n -&gt; 1 restricted aggregation</strong> — multiple validated contributors derive to one support object, but only when all contributors remain explicitly attributable,</li>
  <li><strong>1 -&gt; 0 non-primary derivation</strong> — one source-visible family remains relevant to validation, attribution, or correspondence, but does not become a primary execution object.</li>
</ul>

<p>
The first three shapes apply to execution-facing derived content. The fourth applies to content that remains respected at the boundary without becoming a primary execution object.
</p>

<p>
Examples:
</p>

<pre><code>validated arithmetic primitive
   -&gt; one primary execution object

validated explicit state construct
   -&gt; one primary execution object
   + support object(s)

multiple validated contributors
   -&gt; one aggregated support object
   only if all contributors remain attributable

validated source-visible declaration
   -&gt; no primary execution object
   but correspondence remains explicit</code></pre>

<p>
A conforming implementation MUST NOT use an accidental omission where an explicit <code>1 -&gt; 0</code> non-primary relation is required for recoverability.
</p>

<p>
A conforming implementation MUST NOT aggregate multiple contributors into one object if that aggregation destroys contributor attribution, role distinctions, or category-safe recoverability.
</p>

<p>
Typical valid <code>1 -&gt; 0</code> outcomes may include:
</p>

<ul>
  <li>front-panel layout declarations,</li>
  <li>pure visual styling declarations,</li>
  <li>widget realization-package detail,</li>
  <li>SVG layer detail,</li>
  <li>IDE-only metadata.</li>
</ul>

<hr/>

<h2 id="source-to-ir-family-mapping">10. Source-to-IR Family Mapping</h2>

<p>
This section defines the base v0.1 correspondence posture from validated execution-relevant families to canonical open IR families.
</p>

<h3>10.1 Primitive execution contributors</h3>

<ul>
  <li>Validated intrinsic primitive invocations MUST derive to primary execution-facing IR objects.</li>
  <li>The derived object MUST preserve primitive-family identity and enough typing/interface information for later construction and lowering compatibility.</li>
</ul>

<h3>10.2 Sub-FROG invocation contributors</h3>

<ul>
  <li>Validated sub-FROG invocations MUST derive to primary execution-facing IR objects or equivalent callable execution-facing boundary objects.</li>
  <li>The callable boundary MUST remain recoverable as callable boundary participation, not collapsed into ordinary local primitive identity.</li>
</ul>

<h3>10.3 Public interface contributors</h3>

<ul>
  <li>Validated public interface participation that is execution-relevant MUST derive to explicit public-interface-boundary participation in the canonical Execution IR.</li>
  <li>Public interface participation MUST NOT collapse into front-panel participation, layout, or UI declaration presence.</li>
</ul>

<h3>10.4 Widget-related contributors</h3>

<ul>
  <li><code>widget_value</code> participation MUST derive as <code>widget_value</code> participation.</li>
  <li><code>widget_reference</code> participation MUST derive as <code>widget_reference</code> participation.</li>
  <li>Standardized UI-object primitive operations MUST derive as standardized UI-object primitive operations.</li>
  <li>These families MUST remain distinct.</li>
</ul>

<h3>10.5 Structured control contributors</h3>

<ul>
  <li>Validated structure families such as case-like or loop-like forms MUST derive to structure-preserving IR families with explicit regions and explicit boundary roles where applicable.</li>
  <li>Structured control MUST NOT be flattened into ordinary unordered node sets if region or boundary semantics would be lost.</li>
</ul>

<h3>10.6 State contributors</h3>

<ul>
  <li>Validated explicit local-memory contributors MUST derive as explicit local-memory participation.</li>
  <li>Initialization carriers MUST remain explicit when semantically required.</li>
  <li>State participation MUST NOT be rewritten as inferred persistence.</li>
</ul>

<h3>10.7 Non-primary source-visible contributors</h3>

<ul>
  <li>Some source-visible contributors MAY remain non-primary at the canonical execution boundary.</li>
  <li>When they still matter for declaration linkage, attribution, recoverability, or validation explanation, their correspondence MUST remain explicit.</li>
</ul>

<h3>10.8 Widget-package and realization contributors</h3>

<ul>
  <li>Validated use of widget package content MAY influence execution-facing legality and addressing consequences.</li>
  <li>Widget package contents do not automatically derive to primary execution objects.</li>
  <li>Realization-package detail, visual assets, SVG anchors, and host-private hints SHOULD normally remain non-primary at the open-IR boundary unless a future profile explicitly standardizes a narrower execution-facing family.</li>
</ul>

<hr/>

<h2 id="identity-and-attribution-rules">11. Identity and Attribution Rules</h2>

<p>
Derivation MUST remain compatible with <code>IR/Identity and Mapping.md</code>.
</p>

<p>
Accordingly:
</p>

<ul>
  <li>every primary execution-facing object MUST be attributable to validated contributor or contributor-group meaning permitted by the specification,</li>
  <li>support objects MUST remain attributable,</li>
  <li>non-primary outcomes MUST remain distinguishable from accidental loss,</li>
  <li>declaration-reference correspondence MUST remain distinguishable from primary execution-object identity,</li>
  <li>multi-contributor attribution MUST remain distinguishable from ordinary one-to-one attribution where that distinction matters.</li>
</ul>

<p>
Derivation MUST preserve enough information so that the canonical open IR can answer:
</p>

<ul>
  <li>which validated contributor or contributors gave rise to this object,</li>
  <li>whether the object is primary, support, or correspondence-only,</li>
  <li>which category it belongs to,</li>
  <li>which boundary role it participates in where applicable.</li>
</ul>

<p>
Preferred canonical JSON posture in base v0.1:
</p>

<ul>
  <li><code>unit.source_map</code> carries attribution-oriented mapping records,</li>
  <li><code>unit.correspondence</code> carries declaration-reference and non-primary outcome correspondence records,</li>
  <li>objects MAY also carry inline attribution-compatible anchors where permitted by construction and schema rules.</li>
</ul>

<pre><code>primary object
   -&gt; attribution required

support object
   -&gt; attribution required

non-primary outcome
   -&gt; correspondence required where relevant

accidental omission
   -/&gt; acceptable substitute for correspondence</code></pre>

<p>
For widget-related execution-facing objects, attribution MUST remain able to distinguish at least:
</p>

<ul>
  <li>widget identity attribution,</li>
  <li>member-address attribution where applicable,</li>
  <li>operation-family attribution for UI primitives,</li>
  <li>non-primary realization-package correspondence where preserved.</li>
</ul>

<hr/>

<h2 id="connectivity-derivation">12. Connectivity Derivation</h2>

<p>
Validated dependency structure MUST derive to explicit execution-facing connectivity.
</p>

<p>
Connectivity derivation MUST preserve:
</p>

<ul>
  <li>directionality,</li>
  <li>typed compatibility as already validated upstream,</li>
  <li>the difference between ordinary connectivity and boundary participation,</li>
  <li>the attributable relation between connected contributors and derived execution objects.</li>
</ul>

<p>
Connectivity derivation MUST NOT treat:
</p>

<ul>
  <li>layout adjacency,</li>
  <li>visual ordering,</li>
  <li>editor grouping,</li>
  <li>rendering proximity</li>
</ul>

<p>
as substitutes for validated dependency structure.
</p>

<p>
Allowed derivation-time explicitness includes:
</p>

<ul>
  <li>making port-level or terminal-level endpoints explicit,</li>
  <li>introducing support-side connection carriers,</li>
  <li>materializing boundary-crossing relations explicitly.</li>
</ul>

<p>
Forbidden outcomes include:
</p>

<ul>
  <li>rewriting ordinary connectivity as public-interface participation,</li>
  <li>rewriting ordinary connectivity as structure-boundary participation,</li>
  <li>rewriting ordinary connectivity as explicit UI sequencing,</li>
  <li>losing endpoint attribution through opaque connection collapse.</li>
</ul>

<p>
The same rule applies to UI-related execution connectivity:
</p>

<ul>
  <li>execution ordering or dependencies involving UI primitive operations must remain attributable as such,</li>
  <li>they must not be rewritten as host-render-order assumptions or visual adjacency.</li>
</ul>

<hr/>

<h2 id="structured-control-derivation">13. Structured Control Derivation</h2>

<p>
Validated structured control MUST derive as structured control.
</p>

<p>
This means derivation MUST preserve, where relevant:
</p>

<ul>
  <li>structure family identity,</li>
  <li>region ownership,</li>
  <li>region boundaries,</li>
  <li>structure-terminal roles,</li>
  <li>entry, exit, selector, iteration, or condition participation according to the validated structure family.</li>
</ul>

<p>
Allowed explicitness includes:
</p>

<ul>
  <li>making implicit region carriers explicit in the IR representation,</li>
  <li>introducing structure-owned support objects,</li>
  <li>making structure-boundary crossing explicit.</li>
</ul>

<p>
Forbidden outcomes include:
</p>

<ul>
  <li>flattening structure families into ordinary node bags when region semantics would be lost,</li>
  <li>deriving apparent visual grouping as though it were a validated structure family,</li>
  <li>replacing explicit structure terminals with inferred scheduler behavior,</li>
  <li>erasing structure ownership before lowering.</li>
</ul>

<pre><code>validated structure
   -&gt; structure-preserving IR family
   -&gt; explicit regions where applicable
   -&gt; explicit terminal / boundary roles where applicable

never
   -&gt; ordinary graph only
   if structured semantics would be lost</code></pre>

<hr/>

<h2 id="interface-and-ui-derivation">14. Interface and UI Derivation</h2>

<p>
Public interface and UI families are both execution-relevant in some cases, but they are not the same thing and MUST NOT collapse during derivation.
</p>

<h3>14.1 Public interface derivation</h3>

<ul>
  <li>Validated public interface participation MUST derive to explicit public-interface-boundary participation.</li>
  <li>It MUST remain recoverable as public boundary participation rather than widget presence or visual declaration convenience.</li>
</ul>

<h3>14.2 <code>widget_value</code> derivation</h3>

<ul>
  <li>Validated <code>widget_value</code> participation MUST derive as <code>widget_value</code> participation.</li>
  <li>It MUST remain distinct from property-based access to a member named <code>value</code>.</li>
</ul>

<h3>14.3 <code>widget_reference</code> derivation</h3>

<ul>
  <li>Validated <code>widget_reference</code> participation MUST derive as <code>widget_reference</code> participation.</li>
  <li>It MUST remain distinct from standardized UI-object primitive operation.</li>
</ul>

<h3>14.4 Standardized UI-object primitive operation derivation</h3>

<ul>
  <li>Validated UI-object primitive operations such as property read, property write, or method invocation MUST derive as explicit standardized UI-object primitive operations.</li>
  <li>These operation families MUST remain distinct from widget declaration identity and from widget-reference participation identity.</li>
</ul>

<h3>14.5 Explicit UI sequencing</h3>

<ul>
  <li>Where validated meaning establishes explicit UI sequencing, derivation MUST preserve it as explicit UI sequencing.</li>
  <li>Explicit UI sequencing MUST remain distinct from ordinary connectivity, structure-boundary participation, explicit state participation, and public-interface-boundary participation.</li>
</ul>

<p>
Forbidden category collapses include:
</p>

<ul>
  <li>public interface participation = widget participation,</li>
  <li><code>widget_value</code> = <code>widget_reference</code>,</li>
  <li><code>widget_reference</code> = UI-object operation,</li>
  <li>property read = property write,</li>
  <li>method invocation = property write,</li>
  <li>property read = method invocation,</li>
  <li><code>ui_in</code> = <code>ui_out</code>.</li>
</ul>

<hr/>

<h2 id="widget-object-derivation-corridor">15. Widget Object Derivation Corridor</h2>

<p>
FROG v0.1 already distinguishes:
</p>

<ul>
  <li>widget instance declaration in canonical source,</li>
  <li>class-level widget law,</li>
  <li>diagram-side widget interaction,</li>
  <li>widget-oriented package definitions,</li>
  <li>runtime interpretation,</li>
  <li>host realization.</li>
</ul>

<p>
Derivation MUST preserve that architectural split while still producing an execution-facing IR that is useful for later lowering and runtime consumption.
</p>

<h3>15.1 Widget instance participation</h3>

<ul>
  <li>Validated widget instance participation MAY give rise to execution-facing identities and participation carriers in the IR.</li>
  <li>The mere existence of a widget instance declaration does not require every panel-side attribute to become a primary IR object.</li>
</ul>

<h3>15.2 Class-law consequences</h3>

<ul>
  <li>Validated class-law consequences such as legal member addressing MAY influence derivation.</li>
  <li>Class-law documents or packages do not themselves automatically derive as execution objects.</li>
</ul>

<h3>15.3 Realization-package consequences</h3>

<ul>
  <li>Realization packages MAY matter downstream to runtime and host realization.</li>
  <li>Their detailed contents SHOULD normally remain non-primary at the canonical open-IR boundary.</li>
</ul>

<h3>15.4 Member-address preservation</h3>

<ul>
  <li>When validated widget interaction addresses a member or part, derivation MUST preserve enough information to keep that addressed target recoverable.</li>
  <li>The preservation target is execution-facing addressing, not host-private node addressing.</li>
</ul>

<h3>15.5 Event-family preservation</h3>

<ul>
  <li>When validated meaning includes widget-related event categories relevant to execution-facing form, derivation MAY preserve them as execution-facing event categories or support-side descriptors.</li>
  <li>Such preservation MUST NOT silently become a host-native event taxonomy.</li>
</ul>

<h3>15.6 What must remain downstream</h3>

<p>
The following remain downstream from derivation and MUST NOT be absorbed as open-IR truth:
</p>

<ul>
  <li>host widget trees,</li>
  <li>SVG layer graphs,</li>
  <li>platform-native control classes,</li>
  <li>render-order optimizations,</li>
  <li>focus-loop implementation details,</li>
  <li>runtime-private object tables.</li>
</ul>

<pre><code>validated widget interaction meaning
        -&gt;
derivation preserves execution-facing widget identities
        + addressed access consequences
        + operation-family distinctions
        -/&gt; host widget tree
        -/&gt; SVG render graph
        -/&gt; runtime-private UI storage</code></pre>

<hr/>

<h2 id="state-and-cycle-preservation">16. State and Cycle Preservation</h2>

<p>
Validated explicit local memory MUST derive as explicit local memory.
</p>

<p>
Derivation MUST preserve:
</p>

<ul>
  <li>the existence of explicit state,</li>
  <li>the distinction between state and ordinary dataflow,</li>
  <li>initialization carriers when required,</li>
  <li>read-side versus write-side state participation where semantically relevant,</li>
  <li>the legality boundary that allowed the cycle or state construct upstream.</li>
</ul>

<p>
Derivation MUST NOT:
</p>

<ul>
  <li>invent hidden state where no validated explicit state existed,</li>
  <li>erase validated explicit state into inferred persistence,</li>
  <li>replace explicit initialization with guessed defaulting,</li>
  <li>present scheduler timing guesses as state semantics.</li>
</ul>

<pre><code>validated explicit state
   -&gt; explicit state in canonical IR

validated initialization
   -&gt; explicit initialization carrier where required

never
   -&gt; inferred hidden persistence as a substitute</code></pre>

<p>
If a cycle was accepted only because explicit local memory made it legal, the derived canonical IR MUST continue to show the explicit state participation that makes that legality recoverable.
</p>

<hr/>

<h2 id="allowed-normalization-during-derivation">17. Allowed Normalization During Derivation</h2>

<p>
Derivation MAY introduce additional explicitness when all of the following remain true:
</p>

<ul>
  <li>validated meaning is preserved,</li>
  <li>attribution is preserved,</li>
  <li>recoverability is preserved,</li>
  <li>category distinctions are preserved,</li>
  <li>the result remains a canonical open-IR artifact rather than a lowered private form.</li>
</ul>

<p>
Allowed normalization includes:
</p>

<ul>
  <li>making ports, terminal roles, or directionality explicit,</li>
  <li>making regions explicit,</li>
  <li>introducing support objects,</li>
  <li>introducing canonical object identifiers,</li>
  <li>introducing explicit attribution and correspondence carriers,</li>
  <li>making boundary participation explicit,</li>
  <li>normalizing equivalent validated forms into one canonical execution-facing representation.</li>
</ul>

<p>
Normalization MUST remain conservative:
</p>

<pre><code>allowed normalization
   =
more explicit open IR
without
semantic change
or
recoverability loss</code></pre>

<p>
Lowering-readiness MAY motivate additional explicitness. It MUST NOT justify early erasure of required open-IR distinctions.
</p>

<p>
For widget-related execution structure, allowed normalization may include:
</p>

<ul>
  <li>introducing explicit member-address descriptors,</li>
  <li>introducing explicit operation-family tags for UI primitives,</li>
  <li>introducing explicit widget identity anchors.</li>
</ul>

<p>
It MUST NOT include:
</p>

<ul>
  <li>converting validated widget interaction into one target's native widget API calls,</li>
  <li>expanding a widget interaction into host-private visual-node operations,</li>
  <li>embedding SVG runtime behavior as if it were open IR structure.</li>
</ul>

<hr/>

<h2 id="forbidden-derivation-outcomes">18. Forbidden Derivation Outcomes</h2>

<p>
The following derivation outcomes are forbidden:
</p>

<ul>
  <li>loss of semantic faithfulness,</li>
  <li>loss of source attribution for execution-facing objects,</li>
  <li>loss of required correspondence for non-primary outcomes,</li>
  <li>collapse of public interface participation and UI participation,</li>
  <li>collapse of <code>widget_value</code> and <code>widget_reference</code>,</li>
  <li>collapse of <code>widget_reference</code> and standardized UI-object primitive operation,</li>
  <li>collapse of ordinary connectivity and boundary participation,</li>
  <li>collapse of public-interface-boundary participation, structure-boundary participation, explicit state participation, and explicit UI sequencing into one undifferentiated boundary category,</li>
  <li>flattening structured control into ordinary graph content where semantics would be lost,</li>
  <li>rewriting explicit state as inferred persistence,</li>
  <li>promoting non-execution source content into execution semantics,</li>
  <li>importing editor-only geometry, styling, or presentation state as execution truth,</li>
  <li>introducing runtime-private scheduler policy as though it were canonical IR law,</li>
  <li>introducing backend-family partitions as though they were canonical open-IR categories,</li>
  <li>using derivation to perform lowering, target adaptation, or ABI commitment too early,</li>
  <li>promoting widget realization packages, SVG assets, anchor maps, or host-private hints into primary execution categories without explicit language-level standardization.</li>
</ul>

<pre><code>forbidden
   =
anything that changes validated meaning
or destroys the recoverable open-IR boundary</code></pre>

<hr/>

<h2 id="relation-with-construction-schema-lowering-and-backend-contract">19. Relation with Construction, Schema, Lowering, and Backend Contract</h2>

<p>
Derivation is only one stage in the IR corridor. It is upstream from construction, schema validation, lowering, and backend-facing handoff.
</p>

<pre><code>validated meaning
   -&gt; derivation
   -&gt; canonical IR construction
   -&gt; canonical JSON validation where applicable
   -&gt; lowering
   -&gt; backend-facing contract</code></pre>

<p>
Accordingly:
</p>

<ul>
  <li>derivation defines what must correspond from validated meaning to canonical IR,</li>
  <li>construction defines how those correspondences are materially built into a canonical IR payload,</li>
  <li>schema defines how the canonical JSON shape is machine-checked,</li>
  <li>lowering defines later target-oriented specialization,</li>
  <li>backend contract defines later backend-facing handoff.</li>
</ul>

<p>
Derivation MUST therefore produce an IR that is:
</p>

<ul>
  <li>construction-compatible,</li>
  <li>schema-compatible in principle,</li>
  <li>identity-compatible,</li>
  <li>fit for later lowering,</li>
  <li>still distinct from lowered form and backend-facing contract.</li>
</ul>

<p>
This distinction matters directly to an industrial compiler corridor. A future LLVM-oriented consumer remains downstream from:
</p>

<ul>
  <li>validated meaning,</li>
  <li>canonical derivation,</li>
  <li>canonical open IR identity and correspondence law.</li>
</ul>

<p>
LLVM or any other backend family MUST NOT become the place where missing derivation law is invented retroactively.
</p>

<p>
The same rule applies to UI-capable execution targets:
</p>

<ul>
  <li>target adaptation for Python, Rust, C/C++, native front-panel hosts, or mixed compute/UI runtimes remains downstream,</li>
  <li>derivation must already have preserved the relevant execution-facing distinctions correctly before those targets consume the IR.</li>
</ul>

<hr/>

<h2 id="out-of-scope-for-v01">20. Out of Scope for v0.1</h2>

<p>
The following are out of scope for this document in base v0.1:
</p>

<ul>
  <li>multi-document derivation sets,</li>
  <li>multi-unit canonical IR documents,</li>
  <li>backend-family-specific derivation branches,</li>
  <li>runtime activation identities,</li>
  <li>private scheduler token models,</li>
  <li>ABI-level lowering commitments,</li>
  <li>full debug-information emission standards,</li>
  <li>cross-build persistent identity guarantees,</li>
  <li>one mandatory execution-facing event protocol for host UI systems,</li>
  <li>one mandatory runtime UI object ABI.</li>
</ul>

<p>
These may be specified later. They do not weaken the current derivation obligations at the canonical open-IR boundary.
</p>

<hr/>

<h2 id="summary">21. Summary</h2>

<p>
This document defines the normative derivation rules from validated FROG program meaning to the canonical Execution IR Document.
</p>

<p>
Its core rules are:
</p>

<ul>
  <li>derivation starts only from validated meaning,</li>
  <li>derivation preserves semantic faithfulness,</li>
  <li>derivation preserves attribution and recoverability,</li>
  <li>derivation preserves structured control, explicit state, validated dependency structure, and execution-category distinctions,</li>
  <li>derivation may add explicitness but may not perform downstream specialization too early,</li>
  <li>derivation yields one canonical Execution IR Document containing one execution unit in base v0.1.</li>
</ul>

<p>
The canonical open IR is therefore not an arbitrary implementation artifact. It is the normalized, attributable, recoverable execution-facing representation derived from validated meaning.
</p>

<p>
A serious downstream compilation path depends on this discipline. FROG may later lower toward backend-facing consumers, including LLVM-oriented routes. That downstream route must still begin from a canonical Execution IR whose derivation law was already satisfied correctly.
</p>

<p>
The same discipline now applies explicitly to widget-related execution structure:
</p>

<ul>
  <li>validated widget interaction meaning may derive into execution-facing IR carriers,</li>
  <li>but widget-package ownership, realization-package detail, SVG assets, and host-private visual realization remain outside the ownership of derivation at the canonical open-IR boundary.</li>
</ul>
