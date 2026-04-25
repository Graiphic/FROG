<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 FROG Metadata Specification</h1>

<p align="center">
Definition of the required <code>metadata</code> section of <strong>.frog</strong> programs<br/>
<em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2 id="contents">Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#why-metadata-exists">2. Why Metadata Exists</a></li>
  <li><a href="#scope">3. Scope</a></li>
  <li><a href="#relation-with-other-specifications">4. Relation with Other Specifications</a></li>
  <li><a href="#location">5. Location in a <code>.frog</code> File</a></li>
  <li><a href="#structure">6. Metadata Structure</a></li>
  <li><a href="#field-definitions">7. Field Definitions</a>
    <ul>
      <li><a href="#field-name">7.1 <code>name</code></a></li>
      <li><a href="#field-summary">7.2 <code>summary</code></a></li>
      <li><a href="#field-description">7.3 <code>description</code></a></li>
      <li><a href="#field-author">7.4 <code>author</code></a></li>
      <li><a href="#field-program-version">7.5 <code>program_version</code></a></li>
      <li><a href="#field-tags">7.6 <code>tags</code></a></li>
      <li><a href="#field-is-example">7.7 <code>is_example</code></a></li>
      <li><a href="#field-created">7.8 <code>created</code></a></li>
      <li><a href="#field-updated">7.9 <code>updated</code></a></li>
      <li><a href="#field-license">7.10 <code>license</code></a></li>
    </ul>
  </li>
  <li><a href="#validation-rules">8. Validation Rules</a></li>
  <li><a href="#extensibility-and-boundary-rules">9. Extensibility and Boundary Rules</a></li>
  <li><a href="#examples">10. Examples</a>
    <ul>
      <li><a href="#minimal-example">10.1 Minimal Metadata Example</a></li>
      <li><a href="#extended-example">10.2 Extended Metadata Example</a></li>
    </ul>
  </li>
  <li><a href="#design-goals">11. Design Goals</a></li>
  <li><a href="#summary">12. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
The top-level <code>metadata</code> section defines the descriptive identity of a FROG program.
</p>

<p>
It carries source-level information such as:
</p>

<ul>
  <li>program identity,</li>
  <li>human-readable description,</li>
  <li>authorship and maintenance information,</li>
  <li>version-tracking information,</li>
  <li>search and discovery hints.</li>
</ul>

<p>
The <code>metadata</code> section is part of the canonical <code>.frog</code> source representation.
It is required for a conforming canonical source file.
</p>

<p>
However, <code>metadata</code> is <strong>non-executable</strong>.
It MUST NOT define, modify, or override execution semantics.
</p>

<p>
Execution behavior MUST be derived from the validated executable source representation of the program, centered on the public interface and the authoritative executable diagram, and interpreted according to the normative language semantics.
</p>

<p>
Runtimes, compilers, and other execution-facing systems MUST ignore metadata fields when determining executable meaning.
</p>

<p>
Version fields also remain separated by role:
</p>

<ul>
  <li><code>metadata.program_version</code> versions one program artifact,</li>
  <li>top-level <code>spec_version</code> versions the source-format compatibility target of the <code>.frog</code> file,</li>
  <li>the published specification corpus version is governed centrally in <code>Versioning/Readme.md</code>.</li>
</ul>

<pre>
Metadata at a glance

metadata
   -> descriptive identity
   -> documentation-facing information
   -> search / discovery support

metadata
   != interface
   != diagram
   != execution semantics
   != source-format compatibility law
   != specification corpus version
</pre>

<hr/>

<h2 id="why-metadata-exists">2. Why Metadata Exists</h2>

<p>
A canonical source file needs more than executable structure.
It also needs a stable descriptive layer that helps humans, repositories, IDEs, search systems, and documentation workflows understand what a FROG is.
</p>

<p>
The <code>metadata</code> section exists to provide that descriptive layer without contaminating executable meaning.
</p>

<p>
It makes the source artifact easier to:
</p>

<ul>
  <li>identify,</li>
  <li>document,</li>
  <li>index,</li>
  <li>search,</li>
  <li>classify,</li>
  <li>version and curate.</li>
</ul>

<p>
At the same time, it preserves a strict architectural rule:
descriptive information must remain separate from executable meaning.
</p>

<pre>
Why metadata exists

Humans and tools need:
- a name
- a description
- descriptive context
- discovery hints
- program artifact tracking

Execution does not need:
- author name
- tags
- summary text
- license label
- timestamps
- program_version
</pre>

<hr/>

<h2 id="scope">3. Scope</h2>

<p>
This document specifies:
</p>

<ul>
  <li>the role of the required top-level <code>metadata</code> section,</li>
  <li>the structure of the metadata object,</li>
  <li>the meaning of standard metadata fields,</li>
  <li>validation rules for those fields,</li>
  <li>compatibility and extensibility rules for tool-added metadata.</li>
</ul>

<p>
This document does not specify:
</p>

<ul>
  <li>the public program interface,</li>
  <li>the executable dataflow graph,</li>
  <li>the optional front-panel composition,</li>
  <li>IDE authoring preferences,</li>
  <li>cache or derived artifacts,</li>
  <li>runtime scheduling, typing, or execution semantics,</li>
  <li>the top-level <code>spec_version</code> compatibility policy,</li>
  <li>the published specification corpus version.</li>
</ul>

<pre>
This document owns:
- descriptive source identity
- standard metadata fields
- metadata validation rules
- metadata extensibility boundaries

This document does not own:
- public API
- executable graph
- IDE recoverability state
- tooling cache
- runtime meaning
- source-format compatibility policy
- specification corpus version policy
</pre>

<hr/>

<h2 id="relation-with-other-specifications">4. Relation with Other Specifications</h2>

<p>
The <code>metadata</code> section belongs to the canonical source specification in <code>Expression/</code>.
It is one of the required top-level source sections of a canonical <code>.frog</code> file.
</p>

<p>
Its boundary with related sections is as follows:
</p>

<pre>Top-level source-section boundary

metadata    -> descriptive source identity and documentation
interface   -> public typed program boundary
diagram     -> authoritative executable graph
front_panel -> optional user-facing interaction layer
ide         -> optional IDE-facing authoring preferences and recoverability aids
cache       -> optional derived non-authoritative tooling data</pre>

<p>
In particular:
</p>

<ul>
  <li><code>metadata</code> does not define the public API of the FROG. That belongs to <code>interface</code>.</li>
  <li><code>metadata</code> does not define executable logic. That belongs to <code>diagram</code>.</li>
  <li><code>metadata</code> does not define front-panel composition. That belongs to <code>front_panel</code> when present.</li>
  <li><code>metadata</code> does not carry IDE recoverability or editor-state preferences. That belongs to <code>ide</code>.</li>
  <li><code>metadata</code> does not carry derived accelerators or regenerated artifacts. That belongs to <code>cache</code>.</li>
  <li><code>metadata.program_version</code> does not define source-format compatibility. That belongs to top-level <code>spec_version</code>.</li>
  <li><code>metadata.program_version</code> does not define the repository-wide published specification corpus version. That belongs to <code>Versioning/Readme.md</code>.</li>
</ul>

<pre>
Non-executable boundary inside Expression/

metadata -> durable descriptive identity
ide      -> IDE-facing authoring preferences and recoverability aids
cache    -> derived tooling accelerators safe to delete and regenerate

Version boundary

metadata.program_version -> one program artifact revision
spec_version             -> source-format compatibility target
Versioning/Readme.md     -> published specification corpus version
</pre>

<hr/>

<h2 id="location">5. Location in a <code>.frog</code> File</h2>

<p>
The <code>metadata</code> section is a required top-level JSON object inside the canonical <code>.frog</code> source file.
</p>

<p>
Minimal canonical source shape:
</p>

<pre>{
  "spec_version": "0.1",
  "metadata": {},
  "interface": {},
  "diagram": {}
}</pre>

<p>
A larger canonical source file MAY additionally include optional top-level sections such as
<code>connector</code>, <code>front_panel</code>, <code>icon</code>, <code>ide</code>, and <code>cache</code>.
</p>

<pre>Canonical .frog structure

example.frog
├─ spec_version
├─ metadata     -> required descriptive source section
├─ interface    -> required public typed boundary
├─ diagram      -> required executable graph
├─ connector    -> optional reusable-node perimeter mapping
├─ front_panel  -> optional UI composition
├─ icon         -> optional node icon
├─ ide          -> optional IDE-facing preferences
└─ cache        -> optional derived tooling data</pre>

<p>
Rules:
</p>

<ul>
  <li><code>metadata</code> MUST exist in a canonical <code>.frog</code> source file.</li>
  <li><code>metadata</code> MUST be represented as a JSON object.</li>
  <li>Execution-facing systems MUST ignore <code>metadata</code> when determining executable behavior.</li>
</ul>

<hr/>

<h2 id="structure">6. Metadata Structure</h2>

<p>
A typical metadata object is:
</p>

<pre>"metadata": {
  "name": "Add",
  "summary": "Simple numeric addition example.",
  "description": "Adds two floating-point numbers and returns the result.",
  "author": "Example Labs",
  "program_version": "1.0.0",
  "tags": ["math", "example"],
  "is_example": true,
  "created": "2026-03-05T10:00:00Z",
  "updated": "2026-03-05T12:00:00Z",
  "license": "Apache-2.0"
}</pre>

<p>
The standard minimal required fields are:
</p>

<ul>
  <li><code>name</code></li>
  <li><code>description</code></li>
</ul>

<p>
All other standard fields are optional unless a profile, repository policy, or toolchain contract explicitly requires them.
</p>

<p>
Tools MAY add additional metadata fields, provided that those fields remain non-executable and safely ignorable by execution-facing systems.
</p>

<p>
Metadata fields such as <code>author</code>, <code>created</code>, and <code>updated</code> are descriptive document-level fields. They do not replace fine-grain source provenance for individual nodes, edges, widgets, regions, or review actions.
</p>

<pre>
Metadata structure model

Required descriptive core
   -> name
   -> description

Optional descriptive enrichments
   -> summary
   -> author
   -> program_version
   -> tags
   -> example flag
   -> timestamps
   -> license
   -> additional non-executable descriptive fields
</pre>

<hr/>

<h2 id="field-definitions">7. Field Definitions</h2>

<h3 id="field-name">7.1 <code>name</code></h3>

<p>
Human-readable program name.
</p>

<pre>"name": "Add"</pre>

<ul>
  <li>MUST be present.</li>
  <li>MUST be a string.</li>
  <li>SHOULD be concise and descriptive.</li>
  <li>SHOULD identify the program clearly within its project, package, or library scope.</li>
</ul>

<hr/>

<h3 id="field-summary">7.2 <code>summary</code></h3>

<p>
Short one-line description suitable for listings, search results, previews, or browser-style views.
</p>

<pre>"summary": "Simple numeric addition example."</pre>

<ul>
  <li>MUST be a string if present.</li>
  <li>SHOULD be a single concise sentence.</li>
  <li>SHOULD complement <code>name</code> rather than repeat it verbatim.</li>
</ul>

<hr/>

<h3 id="field-description">7.3 <code>description</code></h3>

<p>
Longer descriptive text explaining what the FROG does.
</p>

<pre>"description": "Adds two floating-point numbers and returns the result."</pre>

<ul>
  <li>MUST be present.</li>
  <li>MUST be a string.</li>
  <li>SHOULD describe externally meaningful behavior rather than internal implementation details.</li>
  <li>MAY contain multiple sentences.</li>
</ul>

<hr/>

<h3 id="field-author">7.4 <code>author</code></h3>

<p>
Identifies the creator, maintainer, or owning organization.
</p>

<pre>"author": "Example Labs"</pre>

<ul>
  <li>MUST be a string if present.</li>
  <li>MAY name an individual, a team, or an organization.</li>
  <li>SHOULD be treated as descriptive information, not as an access-control mechanism.</li>
</ul>

<hr/>

<h3 id="field-program-version">7.5 <code>program_version</code></h3>

<p>
Version identifier of the FROG program artifact itself.
</p>

<pre>"program_version": "1.2.0"</pre>

<ul>
  <li>MUST be a string if present.</li>
  <li>SHOULD use a stable project-defined versioning convention.</li>
  <li>Semantic versioning (<code>MAJOR.MINOR.PATCH</code>) is RECOMMENDED when appropriate.</li>
  <li>MUST be interpreted as the revision of the authored FROG program artifact, not as the source-format compatibility version of the file.</li>
  <li>MUST NOT be interpreted as the published specification corpus version.</li>
</ul>

<p>
The following distinction MUST remain explicit:
</p>

<pre>
metadata.program_version -> version of one FROG program artifact
spec_version             -> source-format compatibility target of the .frog file
Versioning/Readme.md     -> published specification corpus version
</pre>

<p>
These three notions are related but not interchangeable.
</p>

<hr/>

<h3 id="field-tags">7.6 <code>tags</code></h3>

<p>
Keyword list for search, indexing, filtering, or example browsing.
</p>

<pre>"tags": ["math", "signal", "example"]</pre>

<ul>
  <li>MUST be an array of strings if present.</li>
  <li>Each element SHOULD be a short keyword or short phrase.</li>
  <li>Tags SHOULD avoid meaningless duplication.</li>
  <li>Lowercase normalized tags are RECOMMENDED when practical.</li>
</ul>

<hr/>

<h3 id="field-is-example">7.7 <code>is_example</code></h3>

<p>
Indicates whether the FROG is primarily intended as an example, tutorial artifact, or demonstration program.
</p>

<pre>"is_example": true</pre>

<ul>
  <li>MUST be a boolean if present.</li>
  <li>Tools MAY use this field to populate example browsers or tutorial collections.</li>
</ul>

<hr/>

<h3 id="field-created">7.8 <code>created</code></h3>

<p>
Creation timestamp of the source artifact or of the program record tracked by the owning workflow.
</p>

<pre>"created": "2026-03-05T10:00:00Z"</pre>

<ul>
  <li>MUST be a string if present.</li>
  <li>ISO-8601 UTC format is RECOMMENDED.</li>
  <li>This field is descriptive and MUST NOT be used to derive execution behavior.</li>
</ul>

<hr/>

<h3 id="field-updated">7.9 <code>updated</code></h3>

<p>
Timestamp of the last significant metadata or source update tracked by the owning workflow.
</p>

<pre>"updated": "2026-03-05T12:00:00Z"</pre>

<ul>
  <li>MUST be a string if present.</li>
  <li>ISO-8601 UTC format is RECOMMENDED.</li>
  <li>When both <code>created</code> and <code>updated</code> are present, <code>updated</code> SHOULD NOT represent an earlier instant than <code>created</code>.</li>
</ul>

<hr/>

<h3 id="field-license">7.10 <code>license</code></h3>

<p>
Informational license identifier associated with the program or its distribution context.
</p>

<pre>"license": "Apache-2.0"</pre>

<ul>
  <li>MUST be a string if present.</li>
  <li>SPDX identifiers are RECOMMENDED when applicable.</li>
  <li>This field is informational metadata. It does not alter execution semantics.</li>
</ul>

<hr/>

<h2 id="validation-rules">8. Validation Rules</h2>

<ul>
  <li><code>metadata</code> MUST be present.</li>
  <li><code>metadata</code> MUST be a JSON object.</li>
  <li><code>metadata.name</code> MUST be present and MUST be a string.</li>
  <li><code>metadata.description</code> MUST be present and MUST be a string.</li>
  <li>If present, <code>metadata.summary</code> MUST be a string.</li>
  <li>If present, <code>metadata.author</code> MUST be a string.</li>
  <li>If present, <code>metadata.program_version</code> MUST be a string.</li>
  <li>If present, <code>metadata.tags</code> MUST be an array of strings.</li>
  <li>If present, <code>metadata.is_example</code> MUST be a boolean.</li>
  <li>If present, <code>metadata.created</code> MUST be a string.</li>
  <li>If present, <code>metadata.updated</code> MUST be a string.</li>
  <li>If present, <code>metadata.license</code> MUST be a string.</li>
</ul>

<p>
Unknown metadata fields:
</p>

<ul>
  <li>MAY be present.</li>
  <li>MUST NOT affect executable meaning.</li>
  <li>MUST be ignored by runtimes and other execution-facing systems.</li>
</ul>

<p>
Validation of metadata is structural and descriptive.
A metadata validation failure MUST NOT be interpreted as a dataflow execution rule by itself.
</p>

<pre>
Validation boundary

metadata validation
   -> checks descriptive structure

metadata validation
   != executable graph validation
   != runtime semantic interpretation
   != source-format compatibility validation
</pre>

<hr/>

<h2 id="extensibility-and-boundary-rules">9. Extensibility and Boundary Rules</h2>

<p>
Tools, repositories, or organizations MAY extend the metadata object with additional descriptive fields.
</p>

<p>
Such extensions MUST remain:
</p>

<ul>
  <li>non-executable,</li>
  <li>safely ignorable by runtimes,</li>
  <li>compatible with canonical source transparency.</li>
</ul>

<p>
Example:
</p>

<pre>"metadata": {
  "name": "Example",
  "description": "Example FROG.",
  "organization": "Example Labs",
  "documentation_url": "https://example.com/docs"
}</pre>

<p>
However, transient authoring state SHOULD NOT be stored in <code>metadata</code>.
That kind of information belongs in <code>ide</code> when it is source-carried authoring metadata,
or in <code>cache</code> when it is derived and regenerable tooling data.
</p>

<p>
Authoring-origin evidence, AI-generation markers, signed issuer claims, and human-review attestations belong in <code>ide.provenance</code> when they are serialized with source. They SHOULD NOT be collapsed into <code>metadata.author</code>, because document-level authorship is not the same thing as per-object provenance or review acceptance.
</p>

<pre>
Boundary rule for non-executable data

Durable descriptive identity
    -> metadata

IDE-facing authoring preferences or recoverability
    -> ide

Derived, regenerable tooling accelerators
    -> cache
</pre>

<p>
Metadata extensions SHOULD remain understandable as descriptive source information.
They SHOULD NOT become a hidden transport for execution hints, private lowering directives, runtime policies, editor-only transient state, or source-format compatibility policy.
</p>

<hr/>

<h2 id="examples">10. Examples</h2>

<h3 id="minimal-example">10.1 Minimal Metadata Example</h3>

<pre>"metadata": {
  "name": "Add",
  "description": "Adds two numbers."
}</pre>

<hr/>

<h3 id="extended-example">10.2 Extended Metadata Example</h3>

<pre>"metadata": {
  "name": "PIDController",
  "summary": "Basic PID controller example.",
  "description": "Implements a proportional-integral-derivative controller.",
  "author": "Example Automation",
  "program_version": "1.2.0",
  "tags": ["control", "pid", "automation"],
  "is_example": true,
  "created": "2026-03-05T10:00:00Z",
  "updated": "2026-03-05T14:00:00Z",
  "license": "Apache-2.0"
}</pre>

<hr/>

<h2 id="design-goals">11. Design Goals</h2>

<ul>
  <li>Provide a stable descriptive identity for a FROG program.</li>
  <li>Support documentation, indexing, search, and discovery.</li>
  <li>Support authorship and version-tracking workflows.</li>
  <li>Remain compatible with transparent canonical source representation.</li>
  <li>Remain strictly non-authoritative for execution semantics.</li>
  <li>Preserve a clean architectural boundary with <code>interface</code>, <code>diagram</code>, <code>front_panel</code>, <code>ide</code>, and <code>cache</code>.</li>
  <li>Preserve a clean versioning boundary with top-level <code>spec_version</code> and centralized corpus-version governance.</li>
</ul>

<pre>
Metadata should stay:

- descriptive
- durable
- transparent
- searchable
- ignorable for execution
- distinct from source compatibility law

Metadata should not become:

- executable
- IDE transient state
- cache payload
- hidden runtime policy
- hidden spec_version substitute
- hidden corpus-version substitute
</pre>

<hr/>

<h2 id="summary">12. Summary</h2>

<p>
The required <code>metadata</code> section provides descriptive source information about a FROG program.
It defines identity and documentation-oriented information without changing executable meaning.
</p>

<p>
This preserves a clean long-term separation between:
</p>

<ul>
  <li>descriptive source identity (<code>metadata</code>),</li>
  <li>public program boundary (<code>interface</code>),</li>
  <li>authoritative executable logic (<code>diagram</code>),</li>
  <li>optional user-facing composition (<code>front_panel</code>),</li>
  <li>optional IDE-facing authoring metadata (<code>ide</code>),</li>
  <li>optional derived tooling data (<code>cache</code>),</li>
  <li>program artifact versioning (<code>metadata.program_version</code>),</li>
  <li>source-format compatibility versioning (top-level <code>spec_version</code>),</li>
  <li>published specification corpus version governance (<code>Versioning/Readme.md</code>).</li>
</ul>

<pre>
One-line mental model

metadata tells you what the FROG is
metadata does not tell the FROG how to execute
metadata.program_version does not tell the repository what specification version it is
</pre>
