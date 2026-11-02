<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Execution IR Specification</h1>

<p align="center">
  <strong>Canonical open execution intermediate representation for validated FROG programs</strong><br />
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr />

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#boundary-contract">2. Boundary Contract</a></li>
  <li><a href="#role-of-this-document">3. Role of this Document</a></li>
  <li><a href="#scope">4. Scope</a></li>
  <li><a href="#position-in-the-pipeline">5. Position in the Pipeline</a></li>
  <li><a href="#design-goals">6. Design Goals</a></li>
  <li><a href="#core-invariants">7. Core Invariants</a></li>
  <li><a href="#normative-ir-artifact">8. Normative IR Artifact</a></li>
  <li><a href="#canonical-serialization">9. Canonical Serialization</a></li>
  <li><a href="#execution-ir-object-model">10. Execution IR Object Model</a></li>
  <li><a href="#primary-object-families">11. Primary Object Families</a></li>
  <li><a href="#support-objects-and-non-primary-source-content">12. Support Objects and Non-Primary Source Content</a></li>
  <li><a href="#ports-and-connectivity">13. Ports and Connectivity</a></li>
  <li><a href="#regions-and-structured-control">14. Regions and Structured Control</a></li>
  <li><a href="#interface-and-ui-boundaries">15. Interface and UI Boundaries</a></li>
  <li><a href="#widget-object-preservation-corridor">16. Widget Object Preservation Corridor</a></li>
  <li><a href="#state-local-memory-and-cycles">17. State, Local Memory, and Cycles</a></li>
  <li><a href="#relation-with-identity-and-mapping">18. Relation with Identity and Mapping</a></li>
  <li><a href="#relation-with-derivation-and-construction-rules">19. Relation with Derivation and Construction Rules</a></li>
  <li><a href="#relation-with-lowering-backend-contract-and-runtime-private-forms">20. Relation with Lowering, Backend Contract, and Runtime-Private Forms</a></li>
  <li><a href="#relation-with-cache-and-tooling">21. Relation with Cache and Tooling</a></li>
  <li><a href="#out-of-scope">22. Out of Scope</a></li>
  <li><a href="#summary">23. Summary</a></li>
</ul>

<hr />

<h2 id="overview">1. Overview</h2>

<p>
This document defines the base <strong>canonical open execution intermediate representation</strong> for FROG v0.1.
</p>

<p>
The FROG Execution IR is a <strong>derived, standardized, execution-facing representation</strong> produced from a semantically validated FROG program. It is the first normative execution-layer artifact in the FROG pipeline. It exists to provide a portable, inspectable, attributable, structured representation that remains upstream of lowering, backend specialization, and runtime-private realization.
</p>

<p>
For v0.1, the Execution IR is no longer merely an abstract architectural layer. A conforming implementation MUST be able to produce one <strong>Execution IR Document</strong> for one validated FROG program. That document is the canonical open IR artifact of the program at the execution-IR boundary.
</p>

<ul>
  <li>It is <strong>not</strong> the canonical <code>.frog</code> source.</li>
  <li>It is <strong>not</strong> a private scheduler graph.</li>
  <li>It is <strong>not</strong> a backend-specific lowered form.</li>
  <li>It is <strong>not</strong> a compiled artifact.</li>
  <li>It is <strong>not</strong> a runtime history form.</li>
  <li>It is <strong>not</strong> a host-private widget realization graph.</li>
</ul>

<pre><code>canonical .frog source
      -&gt;
structural validation
      -&gt;
validated program meaning
      -&gt;
canonical Execution IR Document
      -&gt;
lowering / specialization
      -&gt;
backend-facing contract
      -&gt;
runtime-private realization
</code></pre>

<p>
The Execution IR therefore plays the role of the first standardized execution-normalized representation of a validated FROG program. It is the open, inspectable, DFIR-like layer of the language stack.
</p>

<hr />

<h2 id="boundary-contract">2. Boundary Contract</h2>

<p>
The Execution IR is the standardized execution-facing representation produced <strong>after semantic validation</strong> and <strong>before lowering</strong>.
</p>

<p>
It MUST be:
</p>

<ul>
  <li>semantically faithful to validated program meaning,</li>
  <li>execution-facing rather than authoring-facing,</li>
  <li>portable across conforming implementations,</li>
  <li>recoverable for mapping, inspection, diagnostics, and conformance checking,</li>
  <li>free of runtime-private semantic invention,</li>
  <li>materially serializable as a canonical open document.</li>
</ul>

<p>
It is not the place where:
</p>

<ul>
  <li>program validity is decided,</li>
  <li>missing semantics are invented,</li>
  <li>backend-specific specialization becomes normative,</li>
  <li>runtime scheduler policy becomes normative,</li>
  <li>private target realization becomes the language truth.</li>
</ul>

<pre><code>Language/
   answers:
   what the validated program means

Execution IR
   answers:
   what canonical open execution representation exists
   for that validated meaning

Lowering
   answers:
   how that canonical representation is later specialized
</code></pre>

<p>
This boundary applies equally to UI-facing execution structure. Execution IR may preserve validated execution-facing consequences of widget interaction, but it MUST NOT absorb widget package ownership, host realization ownership, or SVG asset ownership as if those were open IR truth.
</p>

<hr />

<h2 id="role-of-this-document">3. Role of this Document</h2>

<p>
This document defines <strong>what the FROG Execution IR is</strong> at the architectural level and at the canonical artifact level. It defines:
</p>

<ul>
  <li>the identity of the Execution IR layer,</li>
  <li>the fact that a validated program produces one canonical Execution IR Document,</li>
  <li>the execution-facing object model,</li>
  <li>the major preserved execution-facing families,</li>
  <li>the invariants that make the IR portable, inspectable, attributable, and suitable for later specialization,</li>
  <li>the rule that the open IR has a canonical normative serialization.</li>
</ul>

<p>
This document is intentionally narrower than:
</p>

<ul>
  <li>the full source-to-IR correspondence rules,</li>
  <li>the detailed procedural rules for constructing concrete IR documents,</li>
  <li>the full identity and recoverability contract across all stages,</li>
  <li>later lowering rules,</li>
  <li>later backend-facing contracts.</li>
</ul>

<pre><code>Execution IR.md
   -&gt; what the canonical open IR is

Identity and Mapping.md
   -&gt; how source-aligned identity stays recoverable

Derivation rules.md
   -&gt; what must correspond

Construction rules.md
   -&gt; how a conforming canonical IR document is built

Lowering.md
   -&gt; how later specialization may begin

Backend contract.md
   -&gt; what a later backend-facing consumer may rely on
</code></pre>

<hr />

<h2 id="scope">4. Scope</h2>

<p>
This document owns:
</p>

<ul>
  <li>the architectural role of the Execution IR,</li>
  <li>its position between validated meaning and later specialization,</li>
  <li>its core invariants,</li>
  <li>its high-level object model,</li>
  <li>the major execution-facing families that remain visible in base v0.1,</li>
  <li>the execution-facing distinctions that the canonical IR MUST preserve,</li>
  <li>the rule that one validated program yields one canonical Execution IR Document,</li>
  <li>the rule that the Execution IR has a canonical normative open serialization,</li>
  <li>the boundary between open Execution IR and downstream private realization.</li>
</ul>

<p>
This document does <strong>not</strong> define:
</p>

<ul>
  <li>the canonical <code>.frog</code> source structure in full,</li>
  <li>the IDE Program Model,</li>
  <li>the complete normative semantics of all FROG execution behavior,</li>
  <li>the full source-to-IR derivation mapping,</li>
  <li>the full procedural construction rules for building IR payloads,</li>
  <li>the full cross-stage identity contract,</li>
  <li>one mandatory runtime scheduler architecture,</li>
  <li>one mandatory compiled artifact format,</li>
  <li>one mandatory lowering pipeline,</li>
  <li>one mandatory backend contract payload shape,</li>
  <li>one mandatory private execution engine architecture,</li>
  <li>the full widget-package architecture,</li>
  <li>the full host-side widget realization model.</li>
</ul>

<pre><code>This document owns:
- canonical open execution identity of the IR layer
- canonical Execution IR Document boundary
- object model
- preserved execution-facing families
- invariants
- canonical serialization rule
- open IR vs downstream realization boundary

This document does not own:
- source serialization
- IDE editing models
- full language semantics
- derivation details
- construction procedure details
- full cross-stage identity contract
- runtime scheduler internals
- compiled artifact formats
- backend-private realization
- widget package definitions
- host realization package definitions
</code></pre>

<hr />

<h2 id="position-in-the-pipeline">5. Position in the Pipeline</h2>

<p>
The base architectural pipeline is:
</p>

<pre><code>canonical .frog source
        |
        v
Expression/
        |
        v
validated source form
        |
        v
validated program meaning
        |
        v
Execution IR Document
        |
        v
lowering / backend preparation
        |
        v
backend contract
        |
        v
runtime-private realization
</code></pre>

<p>
Validated program meaning is the language-level truth of the program. The Execution IR is the canonical open execution-facing document built from that validated meaning.
</p>

<p>
Execution IR therefore begins <strong>after semantic validation</strong> and remains <strong>before lowering</strong>. It is also <strong>before backend contract emission</strong> and <strong>before private realization</strong>.
</p>

<pre><code>semantic truth
      |
      v
canonical open execution document
      |
      v
specialization
      |
      v
standardized consumable handoff
      |
      v
private execution machinery
</code></pre>

<p>
The same pipeline discipline applies to widget-related execution structure:
</p>

<pre><code>widget instance in source
      |
      v
validated widget participation meaning
      |
      v
Execution IR preservation of execution-facing widget roles
      |
      v
lowering / target adaptation
      |
      v
runtime + host realization
</code></pre>

<hr />

<h2 id="design-goals">6. Design Goals</h2>

<p>
The base Execution IR for v0.1 SHOULD be:
</p>

<ul>
  <li>execution-facing,</li>
  <li>source-attributable,</li>
  <li>recoverable for inspection and diagnostics,</li>
  <li>inspectable,</li>
  <li>portable across implementations,</li>
  <li>stable enough to support conformance and tooling,</li>
  <li>strict enough to support later lowering,</li>
  <li>structured enough to serve as a serious compiler-oriented handoff layer,</li>
  <li>conservative enough to avoid freezing one private compiler or runtime design too early.</li>
</ul>

<pre><code>Design balance

open enough      -&gt; inspectable / portable / standard-friendly
faithful enough  -&gt; semantically grounded
attributable     -&gt; recoverable for later tooling
strict enough    -&gt; useful for later lowering
stable enough    -&gt; schema-checkable / comparable
not frozen as    -&gt; one private runtime design
</code></pre>

<p>
For UI and widget-related execution structure, the design goal is similar:
</p>

<ul>
  <li>preserve validated execution-facing widget distinctions,</li>
  <li>do not prematurely collapse them into private object tables or host widgets,</li>
  <li>do not let IR become a surrogate realization package or rendering graph.</li>
</ul>

<hr />

<h2 id="core-invariants">7. Core Invariants</h2>

<p>
The following invariants apply to the canonical open Execution IR of v0.1:
</p>

<ul>
  <li>Every execution-visible IR object MUST be attributable to validated program meaning.</li>
  <li>The Execution IR MUST remain source-aligned at the level of attributable execution-facing objects.</li>
  <li>The Execution IR MUST preserve enough identity and structure to support later recoverable mapping obligations.</li>
  <li>The Execution IR MUST preserve the distinction between primary execution-facing objects and support objects.</li>
  <li>The Execution IR MUST NOT silently change the normative language meaning of the validated program.</li>
  <li>The Execution IR MUST preserve explicit local memory as explicit local memory.</li>
  <li>The Execution IR MUST preserve structured control as structured control in the open IR.</li>
  <li>The Execution IR MUST preserve the distinction between public interface participation and UI participation.</li>
  <li>The Execution IR MUST preserve the distinction between <code>widget_value</code> participation and <code>widget_reference</code> participation.</li>
  <li>The Execution IR MUST preserve the distinction between <code>widget_reference</code> participation and standardized UI-object primitive operation.</li>
  <li>The Execution IR MUST preserve the distinction between <code>widget_value</code> participation and property-based access to member <code>value</code>.</li>
  <li>The Execution IR MUST expose execution-relevant connectivity explicitly.</li>
  <li>The Execution IR MUST be representable as one canonical document for one validated program.</li>
  <li>The canonical document MUST be serializable in the normative open wire format defined for the Execution IR.</li>
  <li>The Execution IR MUST NOT encode editor-only presentation state as execution semantics.</li>
  <li>The Execution IR MUST NOT be treated as a runtime history, event trace, or debugger session log.</li>
  <li>The Execution IR MUST NOT absorb widget realization packages, SVG state graphs, or host-private visual nodes as open IR truth.</li>
</ul>

<pre><code>Core invariants

attributable
source-aligned
mapping-supporting
primary / support distinction preserving
semantically faithful
explicit-memory preserving
structured-control preserving
explicit-connectivity preserving
interface / UI distinction preserving
widget_value / widget_reference distinction preserving
widget_reference / UI-primitive distinction preserving
widget_value / property(value) distinction preserving
canonically documentable
canonically serializable

not editor-state semantics
not runtime history
not host-realization truth
</code></pre>

<hr />

<h2 id="normative-ir-artifact">8. Normative IR Artifact</h2>

<p>
A validated FROG program MUST produce exactly one <strong>Execution IR Document</strong>.
</p>

<p>
That document is the canonical open execution artifact of the program at the Execution IR boundary. It is the normative IR artifact that later stages consume for lowering, transformation, checking, and implementation-specific preparation.
</p>

<p>
In base v0.1, the canonical Execution IR Document MUST contain exactly one top-level execution unit representing one validated program. That execution unit MUST own the execution-facing objects and relationships needed to represent the validated program in canonical open execution form.
</p>

<pre><code>Execution IR Document
└── execution_unit
    ├── identities
    ├── boundaries
    ├── primary_objects
    ├── support_objects
    ├── regions
    ├── ports
    ├── connections
    └── attribution anchors
</code></pre>

<p>
The canonical Execution IR Document is:
</p>

<ul>
  <li>the public open IR artifact,</li>
  <li>the conformance-visible execution IR boundary artifact,</li>
  <li>the normative execution-layer handoff between semantic validation and lowering.</li>
</ul>

<p>
It is not required to match:
</p>

<ul>
  <li>the canonical source file shape,</li>
  <li>the editable Program Model shape,</li>
  <li>a backend-oriented lowered unit,</li>
  <li>a runtime-private scheduled unit,</li>
  <li>a host-private UI object graph.</li>
</ul>

<p>
One source file, one package, or one tool session may contain additional non-authoritative material. That does not change the rule that the validated program yields exactly one canonical Execution IR Document at the open IR boundary.
</p>

<hr />

<h2 id="canonical-serialization">9. Canonical Serialization</h2>

<p>
The FROG Execution IR has a <strong>canonical normative JSON serialization</strong>.
</p>

<p>
For v0.1:
</p>

<ul>
  <li>the architectural Execution IR remains conceptually larger than one transport encoding,</li>
  <li>but the open conformance-visible serialization of that IR MUST be canonical JSON,</li>
  <li>and conforming implementations MUST be able to emit that canonical JSON form.</li>
</ul>

<p>
Accordingly:
</p>

<ul>
  <li>the canonical wire format of the open Execution IR is JSON,</li>
  <li>the canonical payload artifact is the serialized Execution IR Document,</li>
  <li>alternate internal representations MAY exist inside implementations,</li>
  <li>alternate private serializations MAY exist downstream of the open IR boundary,</li>
  <li>but none of those private forms replace the normative status of the canonical open JSON Execution IR Document.</li>
</ul>

<pre><code>normative architectural layer
        |
        v
canonical Execution IR Document
        |
        v
canonical JSON serialization
        |
        v
later private forms MAY exist
</code></pre>

<p>
This rule exists to ensure that the open IR is:
</p>

<ul>
  <li>portable,</li>
  <li>diffable,</li>
  <li>cacheable,</li>
  <li>schema-checkable,</li>
  <li>toolable,</li>
  <li>conformance-testable across implementations.</li>
</ul>

<p>
The complete field-level payload shape is owned by companion IR documents, especially the Construction and Schema material. This document defines the architectural fact that a canonical JSON wire form exists and is mandatory for conforming open IR emission.
</p>

<hr />

<h2 id="execution-ir-object-model">10. Execution IR Object Model</h2>

<p>
The base Execution IR represents one validated FROG program as one <strong>execution unit</strong> containing execution-facing objects and their relationships.
</p>

<p>
Conceptually:
</p>

<pre><code>Execution IR Document
└── execution_unit
    ├── boundary objects
    ├── primary executable objects
    ├── support objects
    ├── region objects
    ├── typed ports / explicit terminals
    ├── directed connections
    ├── execution-facing identities
    └── source attribution anchors
</code></pre>

<p>
This execution unit is the architectural container of the open IR for one validated FROG program in base v0.1.
</p>

<p>
The open Execution IR MAY contain support objects that make execution-facing structure more explicit, but such support objects MUST remain attributable and MUST NOT replace primary validated execution-visible content with opaque generated machinery.
</p>

<pre><code>Support objects are allowed when they are:

attributable
execution-facing
semantically justified
structurally recoverable

not opaque replacement machinery
not hidden semantic redefinition
not premature backend-private invention
</code></pre>

<p>
For widget-related execution structure, the same object-model rule applies:
</p>

<ul>
  <li>execution-facing widget roles may appear as primary or support execution objects when justified by validated meaning,</li>
  <li>but host-private realization objects do not become canonical Execution IR objects merely because an implementation happens to use them.</li>
</ul>

<hr />

<h2 id="primary-object-families">11. Primary Object Families</h2>

<p>
In base v0.1, the Execution IR SHOULD preserve the major validated execution-visible families rather than aggressively lowering them away.
</p>

<p>
Those primary families include:
</p>

<ul>
  <li>primitive execution objects,</li>
  <li>structured execution objects,</li>
  <li>sub-FROG invocation objects,</li>
  <li>public interface boundary objects,</li>
  <li>widget-value participation objects,</li>
  <li>widget-reference participation objects where applicable,</li>
  <li>standardized UI-object primitive execution objects where applicable,</li>
  <li>explicit local-memory-bearing objects where applicable.</li>
</ul>

<p>
This conservative model keeps the open IR readable and attributable while still making execution-facing structure explicit.
</p>

<p>
Base v0.1 therefore favors:
</p>

<pre><code>validated structured meaning
        |
        v
open structured IR
        |
        v
later specialization
</code></pre>

<p>
rather than:
</p>

<pre><code>validated structured meaning
        |
        v
immediate backend-shaped flattening
</code></pre>

<p>
This does not prevent later specialization. It means that specialization belongs to later stages, not to the base open Execution IR itself.
</p>

<p>
For UI-related execution structure, the preserved family distinction is especially important:
</p>

<ul>
  <li><code>widget_value</code> remains a distinct execution-facing participation family,</li>
  <li><code>widget_reference</code> remains a distinct execution-facing participation family,</li>
  <li>UI primitive operations remain distinct from merely carrying a widget reference.</li>
</ul>

<hr />

<h2 id="support-objects-and-non-primary-source-content">12. Support Objects and Non-Primary Source Content</h2>

<p>
Base v0.1 distinguishes clearly between:
</p>

<ul>
  <li><strong>primary execution-facing objects</strong> — objects that directly carry execution-facing roles in the open IR,</li>
  <li><strong>support objects</strong> — objects introduced to make already-validated execution-facing structure more explicit,</li>
  <li><strong>source-visible content that does not become a primary execution object</strong> — source-visible declarations or records that may still matter for correspondence or attribution without becoming primary open-IR objects of their own.</li>
</ul>

<p>
Support objects are legitimate when they help represent:
</p>

<ul>
  <li>explicit regions,</li>
  <li>explicit structure-boundary terminals,</li>
  <li>explicit structure terminals,</li>
  <li>explicit port descriptors,</li>
  <li>explicit source-attribution anchors,</li>
  <li>other execution-facing classification records that do not alter meaning.</li>
</ul>

<p>
Conversely, the existence of source-visible content does <strong>not</strong> imply that it must become a primary execution object in the open IR.
</p>

<p>
This document therefore defines the architectural object model of the open IR, but it does not claim that every source-visible family produces one primary IR object. The exact correspondence rules belong to <code>Derivation rules.md</code>, and the exact recoverability obligations belong to <code>Identity and Mapping.md</code>.
</p>

<pre><code>Execution IR object roles

primary execution-facing object
   -&gt; directly carries execution-facing role

support object
   -&gt; clarifies already-validated execution-facing structure

source-visible content
   -&gt; may remain non-primary at the open-IR boundary
</code></pre>

<p>
Typical non-primary source content at this boundary may include:
</p>

<ul>
  <li>purely authoring-facing front-panel layout detail,</li>
  <li>widget realization package detail,</li>
  <li>SVG layer detail,</li>
  <li>IDE preferences and cache content.</li>
</ul>

<hr />

<h2 id="ports-and-connectivity">13. Ports and Connectivity</h2>

<p>
The Execution IR MUST represent execution-relevant connectivity explicitly.
</p>

<p>
At minimum:
</p>

<ul>
  <li>execution-facing objects MUST expose explicit typed ports or an equivalent explicit terminal interface,</li>
  <li>port direction MUST be explicit,</li>
  <li>directed connections MUST be explicit,</li>
  <li>the resulting IR graph MUST preserve the validated dependency structure of the program,</li>
  <li>connection endpoints MUST remain attributable to validated execution-facing objects and validated ports or terminals.</li>
</ul>

<p>
The open IR MAY be more explicit than source where validation has already resolved execution-relevant facts. That explicitness belongs to open execution-facing representation, not yet to target-specific lowering.
</p>

<pre><code>Connectivity invariants

explicit ports
explicit direction
explicit directed connections
preserved validated dependencies
attributable endpoints
</code></pre>

<p>
Execution IR connectivity MUST therefore remain:
</p>

<ul>
  <li>structurally inspectable,</li>
  <li>directionally unambiguous,</li>
  <li>semantically grounded in validated dependency meaning.</li>
</ul>

<p>
This applies equally to connectivity involving:
</p>

<ul>
  <li>public interface participation,</li>
  <li>widget primary-value participation,</li>
  <li>standardized UI primitive execution objects,</li>
  <li>stateful feedback paths.</li>
</ul>

<hr />

<h2 id="regions-and-structured-control">14. Regions and Structured Control</h2>

<p>
Structured control remains explicit in the base Execution IR.
</p>

<p>
For v0.1, control structures such as <code>case</code>, <code>for_loop</code>, and <code>while_loop</code> SHOULD remain represented as structured execution objects with explicit owned regions and explicit boundary participation rather than being immediately flattened into backend-oriented branch or loop machinery.
</p>

<p>
Accordingly:
</p>

<ul>
  <li>a structured execution object MUST identify its standardized structure family,</li>
  <li>owned regions MUST remain explicit,</li>
  <li>structure-boundary participation MUST remain explicit,</li>
  <li>structure-terminal roles MUST remain explicit or equivalently recoverable,</li>
  <li>region-local execution content MUST remain attributable to source-level region content.</li>
</ul>

<pre><code>validated structured control
        |
        v
explicit structured IR object
        ├── explicit owned regions
        ├── explicit boundary participation
        ├── explicit structure terminals where applicable
        └── attributable region-local content
</code></pre>

<p>
In v0.1, the open Execution IR SHOULD still make it possible to recover:
</p>

<ul>
  <li>which structure family is involved,</li>
  <li>which regions belong to that structure,</li>
  <li>which execution-facing objects are region-local,</li>
  <li>which boundary roles connect outer and inner execution content,</li>
  <li>which roles are structure-intrinsic rather than ordinary boundary values.</li>
</ul>

<hr />

<h2 id="interface-and-ui-boundaries">15. Interface and UI Boundaries</h2>

<p>
The Execution IR MUST preserve the distinction between:
</p>

<ul>
  <li>public interface participation,</li>
  <li>widget primary-value participation,</li>
  <li>widget object-style reference participation,</li>
  <li>standardized UI-object primitive operation.</li>
</ul>

<p>
Therefore:
</p>

<ul>
  <li>public interface entry and exit semantics MUST remain represented explicitly,</li>
  <li>widget primary-value participation MUST remain distinguishable from public interface participation,</li>
  <li>widget-reference access MUST remain distinguishable from ordinary valueflow participation,</li>
  <li>standardized UI-object primitive operation MUST remain distinguishable from widget-reference participation itself,</li>
  <li>the open IR MUST NOT collapse those roles into one undifferentiated generic boundary or generic object-access concept.</li>
</ul>

<p>
In base v0.1, a source-aligned implementation SHOULD preserve recognizable families corresponding to:
</p>

<ul>
  <li><code>interface_input</code>,</li>
  <li><code>interface_output</code>,</li>
  <li><code>widget_value</code>,</li>
  <li><code>widget_reference</code>,</li>
  <li>standardized UI-object primitive execution objects where applicable.</li>
</ul>

<pre><code>Boundary distinction

public interface participation
   !=
widget primary-value participation
   !=
widget object-style reference participation
   !=
standardized UI-object primitive operation
</code></pre>

<p>
This distinction is mandatory for later lowering, because these families may eventually be consumed differently by:
</p>

<ul>
  <li>non-UI execution targets,</li>
  <li>UI-capable runtimes,</li>
  <li>native executable front-panel targets,</li>
  <li>mixed targets that split compute and UI realization.</li>
</ul>

<hr />

<h2 id="widget-object-preservation-corridor">16. Widget Object Preservation Corridor</h2>

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
The Execution IR MUST preserve that separation while still providing an execution-facing corridor for widget-relevant validated meaning.
</p>

<h3>16.1 Preserved execution-facing distinctions</h3>

<p>
The following distinctions MUST remain explicit in the open Execution IR:
</p>

<ul>
  <li><code>widget_value</code> participation versus public interface participation,</li>
  <li><code>widget_reference</code> participation versus <code>widget_value</code> participation,</li>
  <li>UI primitive execution objects versus widget reference carriers,</li>
  <li>root widget access versus addressed member access,</li>
  <li>member access to <code>value</code> versus ordinary primary-value participation.</li>
</ul>

<h3>16.2 What IR may preserve</h3>

<p>
The open Execution IR MAY preserve execution-relevant widget-related information such as:
</p>

<ul>
  <li>stable widget identities needed by execution-facing interaction,</li>
  <li>validated class-resolved addressing consequences where needed by execution,</li>
  <li>widget-reference carrying objects,</li>
  <li>member-address descriptors for standardized UI primitive operations,</li>
  <li>event categories and event payload shapes where execution-facing form needs them,</li>
  <li>attribution back to widget instances and addressed members.</li>
</ul>

<h3>16.3 What IR must not absorb</h3>

<p>
The open Execution IR MUST NOT absorb:
</p>

<ul>
  <li>widget class package ownership,</li>
  <li>widget realization package ownership,</li>
  <li>SVG asset ownership,</li>
  <li>host-private widget composition strategies,</li>
  <li>runtime-private UI object tables,</li>
  <li>one implementation's rendering pipeline as if it were canonical open IR.</li>
</ul>

<h3>16.4 Practical corridor</h3>

<p>
The practical widget-related IR corridor is therefore:
</p>

<pre><code>widget instance in canonical source
        -&gt;
validated widget participation meaning
        -&gt;
validated widget reference / member access meaning
        -&gt;
execution-facing IR preservation of identities and accesses
        -&gt;
later lowering / target adaptation
        -&gt;
runtime and host realization</code></pre>

<p>
This corridor is especially important for:
</p>

<ul>
  <li><code>widget_value</code>,</li>
  <li><code>widget_reference</code>,</li>
  <li><code>frog.ui.property_read</code>,</li>
  <li><code>frog.ui.property_write</code>,</li>
  <li><code>frog.ui.method_invoke</code>.</li>
</ul>

<p>
Open IR may preserve their validated execution-facing structure. Open IR does not become the owner of widget realization itself.
</p>

<hr />

<h2 id="state-local-memory-and-cycles">17. State, Local Memory, and Cycles</h2>

<p>
Explicit local memory remains explicit in the base Execution IR. This is a hard architectural requirement.
</p>

<p>
Accordingly:
</p>

<ul>
  <li>the IR MUST preserve explicit local-memory objects such as <code>frog.core.delay</code>,</li>
  <li>the IR MUST NOT legalize an otherwise invalid combinational cycle by introducing hidden implicit memory,</li>
  <li>the IR MUST NOT erase explicit memory attribution in a valid feedback path,</li>
  <li>cycles represented in the IR MUST remain consistent with validated cycle rules.</li>
</ul>

<p>
The open Execution IR MAY classify explicit memory-bearing objects as execution-relevant state objects, but it MUST remain faithful to the validated FROG rule that valid feedback depends on explicit local memory.
</p>

<pre><code>Cycle discipline

invalid cycle
   != legalized by hidden memory

valid feedback
   requires
explicit local memory

therefore

Execution IR
   must preserve
explicit memory identity
</code></pre>

<p>
State elimination, storage remapping, or state-cell realization belong to later lowering stages, not to the base open Execution IR definition.
</p>

<hr />

<h2 id="relation-with-identity-and-mapping">18. Relation with Identity and Mapping</h2>

<p>
This document defines the open Execution IR as an attributable execution-facing representation. It does not define the full normative mapping contract by which every source-visible object, validated semantic object, and IR object remain recoverably related.
</p>

<p>
That concern belongs primarily to <strong><code>Identity and Mapping.md</code></strong>.
</p>

<p>
The relationship is:
</p>

<ul>
  <li><code>Execution IR.md</code> defines the kinds of execution-facing objects the open IR contains,</li>
  <li><code>Identity and Mapping.md</code> defines how those objects remain connected to validated meaning and source-visible contributors,</li>
  <li><code>Identity and Mapping.md</code> also defines how primary objects, support objects, and non-primary correspondence remain distinguishable where required,</li>
  <li>the open Execution IR MUST preserve enough identity and structure for those mapping obligations to remain satisfiable.</li>
</ul>

<pre><code>Execution IR.md
   -&gt; what execution-facing objects exist

Identity and Mapping.md
   -&gt; how those objects remain recoverably connected
      to validated meaning and source-visible origin
</code></pre>

<p>
For widget-related execution-facing content, this means:
</p>

<ul>
  <li>widget identities used at execution-facing level must remain recoverable,</li>
  <li>member-addressed UI primitive operations must remain attributable,</li>
  <li>the mapping layer must distinguish ordinary valueflow participation from object-style widget access.</li>
</ul>

<hr />

<h2 id="relation-with-derivation-and-construction-rules">19. Relation with Derivation and Construction Rules</h2>

<p>
This document defines the architectural shape of the open Execution IR and the existence of a canonical Execution IR Document. It does not fully define either:
</p>

<ul>
  <li>the normative correspondence between validated source-visible content and IR objects, or</li>
  <li>the full procedural rules for materially building a concrete canonical IR document.</li>
</ul>

<p>
Those concerns belong to companion IR documents:
</p>

<ul>
  <li><strong>Derivation rules</strong> define what must correspond between validated meaning and open Execution IR, including which content derives as primary objects, which may derive through support objects, and which may remain non-primary at the open-IR boundary.</li>
  <li><strong>Construction rules</strong> define how a conforming canonical Execution IR Document is materially built, populated, serialized, and validated.</li>
</ul>

<pre><code>Execution IR.md
   -&gt; what the canonical open IR is

Derivation rules.md
   -&gt; what must correspond

Construction rules.md
   -&gt; how the canonical open IR document is built
</code></pre>

<p>
This separation is important:
</p>

<ul>
  <li>Execution IR SHOULD stay architectural and durable,</li>
  <li>Derivation SHOULD stay about correspondence obligations,</li>
  <li>Construction SHOULD stay about concrete build obligations and payload shape.</li>
</ul>

<hr />

<h2 id="relation-with-lowering-backend-contract-and-runtime-private-forms">20. Relation with Lowering, Backend Contract, and Runtime-Private Forms</h2>

<p>
The open Execution IR sits before lowering and before runtime-private realization. It also sits before the backend-facing contract boundary that later consumers may rely on.
</p>

<p>
Therefore:
</p>

<ul>
  <li>the canonical Execution IR Document MAY serve as the mandatory input to later lowering stages,</li>
  <li>later stages MAY specialize, normalize further, partition, schedule, compile, or transform execution for specific targets,</li>
  <li>a backend contract MAY later define what a lowered form explicitly offers to a downstream consumer,</li>
  <li>runtime-private scheduler structures, compiled objects, target-private state layouts, and equivalent realization forms remain outside the ownership of this document unless later standardized explicitly.</li>
</ul>

<pre><code>open Execution IR
   != private scheduler graph

open Execution IR
   != compiled artifact

open Execution IR
   != runtime trace

open Execution IR
   != backend-facing contract

open Execution IR
   = canonical open execution representation
</code></pre>

<pre><code>canonical open IR
      -&gt;
lowering / specialization
      -&gt;
backend contract
      -&gt;
private realization
</code></pre>

<p>
The architectural reading rule is:
</p>

<ul>
  <li><strong>Execution IR</strong> remains source-shaped enough for inspection and attribution,</li>
  <li><strong>Lowering</strong> is where specialization begins,</li>
  <li><strong>Backend contract</strong> is where a standardized consumable handoff may be declared,</li>
  <li><strong>private realization</strong> is later and is not owned by this document.</li>
</ul>

<p>
For widget-related execution structure, this also means:
</p>

<ul>
  <li>lowering may adapt validated widget interaction toward runtime-usable forms,</li>
  <li>backend contract may describe what a downstream consumer receives,</li>
  <li>runtime-private widget object storage and host rendering remain downstream and private.</li>
</ul>

<hr />

<h2 id="relation-with-cache-and-tooling">21. Relation with Cache and Tooling</h2>

<p>
A tool MAY serialize an Execution IR artifact inside a cache entry such as <code>frog.ir</code>.
</p>

<p>
However:
</p>

<ul>
  <li>the cache container does not own the meaning of Execution IR,</li>
  <li>cache metadata does not affect program semantics,</li>
  <li>cache absence does not invalidate the canonical source artifact,</li>
  <li>cache presence does not make the cached artifact the canonical source of truth.</li>
</ul>

<p>
This document owns the architectural meaning of the Execution IR itself. Cache specifications only define how such derived artifacts may be stored or associated.
</p>

<p>
Likewise, IDEs and other tools MAY inspect, compare, cache, render, or analyze open IR forms without thereby taking ownership of the IR definition itself.
</p>

<pre><code>Execution IR meaning
   lives in
IR/

cached derived artifact
   may live in
frog.ir

cache container
   is not
IR ownership
</code></pre>

<p>
Tooling MAY therefore use the open IR for:
</p>

<ul>
  <li>inspection,</li>
  <li>comparison,</li>
  <li>schema validation,</li>
  <li>conformance checking,</li>
  <li>diagnostic correlation,</li>
  <li>execution preparation.</li>
</ul>

<p>
But such uses MUST NOT redefine the architectural meaning of the open Execution IR as though it were merely a cache convention or an IDE-internal view.
</p>

<hr />

<h2 id="out-of-scope">22. Out of Scope</h2>

<p>
The following topics remain out of scope for this document in v0.1:
</p>

<ul>
  <li>one mandatory SSA-style private form,</li>
  <li>one mandatory scheduler graph,</li>
  <li>one mandatory lowering pipeline,</li>
  <li>one mandatory backend contract payload format,</li>
  <li>distributed execution IR,</li>
  <li>asynchronous execution IR beyond the baseline language model,</li>
  <li>one mandatory runtime state snapshot format,</li>
  <li>one mandatory trace or observability event protocol,</li>
  <li>all field-level JSON rules of the canonical payload shape,</li>
  <li>the full host-side widget realization model,</li>
  <li>the full SVG asset model,</li>
  <li>one mandatory runtime UI object model.</li>
</ul>

<pre><code>Not frozen in v0.1

one universal private execution form
one mandatory SSA private form
one mandatory scheduler graph
one mandatory lowering pipeline
one mandatory backend contract payload
one mandatory trace / observability transport
all field-level schema constraints in this document
one mandatory runtime UI object model
one mandatory host realization model
</code></pre>

<p>
What <strong>is</strong> frozen here is narrower and more important:
</p>

<ul>
  <li>the existence of a canonical Execution IR Document,</li>
  <li>the fact that one validated program yields one such document,</li>
  <li>the fact that the open wire form of that document is canonical JSON,</li>
  <li>the preservation of key execution-facing distinctions required for later lowering and conformance, including widget-related distinctions.</li>
</ul>

<hr />

<h2 id="summary">23. Summary</h2>

<p>
The FROG Execution IR of v0.1 is a <strong>canonical open execution intermediate representation</strong> built from <strong>validated program meaning</strong>. It is the first normative execution artifact of the FROG language stack.
</p>

<p>
A conforming implementation MUST be able to produce:
</p>

<ul>
  <li>one <strong>Execution IR Document</strong> for one validated program,</li>
  <li>with one top-level execution unit,</li>
  <li>serialized in the canonical open JSON wire format of the Execution IR.</li>
</ul>

<p>
The Execution IR preserves:
</p>

<ul>
  <li>major validated execution-facing object families,</li>
  <li>the distinction between primary objects and support objects,</li>
  <li>explicit connectivity,</li>
  <li>structured control and explicit regions,</li>
  <li>public interface versus UI distinctions,</li>
  <li><code>widget_value</code> versus <code>widget_reference</code> distinctions,</li>
  <li><code>widget_reference</code> versus standardized UI-object primitive operation distinctions,</li>
  <li>explicit local memory and valid stateful cycles,</li>
  <li>enough identity and structure to support later recoverability obligations.</li>
</ul>

<p>
It does not define:
</p>

<ul>
  <li>full derivation correspondence,</li>
  <li>full construction procedure,</li>
  <li>full identity and mapping rules,</li>
  <li>full lowering,</li>
  <li>full backend contract,</li>
  <li>runtime-private realization.</li>
</ul>

<pre><code>Execution IR is the canonical open architectural bridge
between validated program meaning
and later specialization for execution realization.

It is the DFIR-like open execution layer of FROG,
while remaining upstream of backend contract,
outside private runtime realization,
and distinct from widget package ownership
and host-side UI realization.
</code></pre>
