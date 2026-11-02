<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG IR JSON Schema Family</h1>

<p align="center">
  <strong>Machine-checkable schema artifacts for the canonical JSON Execution IR Document</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#why-this-directory-exists">2. Why This Directory Exists</a></li>
  <li><a href="#relation-with-the-rest-of-ir">3. Relation with the Rest of IR/</a></li>
  <li><a href="#schema-family-posture">4. Schema Family Posture</a></li>
  <li><a href="#published-artifacts">5. Published Artifacts</a></li>
  <li><a href="#root-schema">6. Root Schema</a></li>
  <li><a href="#component-schemas">7. Component Schemas</a></li>
  <li><a href="#what-schema-validation-proves">8. What Schema Validation Proves</a></li>
  <li><a href="#what-schema-validation-does-not-prove">9. What Schema Validation Does Not Prove</a></li>
  <li><a href="#widget-object-posture">10. Widget Object Posture</a></li>
  <li><a href="#versioning-posture">11. Versioning Posture</a></li>
  <li><a href="#practical-reading-order">12. Practical Reading Order</a></li>
  <li><a href="#status">13. Status</a></li>
  <li><a href="#license">14. License</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This directory contains the machine-checkable JSON Schema artifacts for the canonical FROG Execution IR Document.
</p>

<p>
These artifacts are the schema-layer companion of the broader IR architecture. They do not define the whole IR by themselves. They define the structural JSON validation surface that a conforming canonical Execution IR payload MUST satisfy for the corresponding schema version.
</p>

<p>
The current schema family is intentionally conservative. It is designed to keep the open Execution IR:
</p>

<ul>
  <li>portable,</li>
  <li>inspectable,</li>
  <li>diffable,</li>
  <li>machine-checkable,</li>
  <li>compatible with later lowering without collapsing into backend-private form.</li>
</ul>

<hr/>

<h2 id="why-this-directory-exists">2. Why This Directory Exists</h2>

<p>
The open Execution IR is not only an architectural concept. It is also a canonical JSON artifact that implementations, conformance material, test suites, inspection tooling, and downstream compiler corridors must be able to exchange and validate.
</p>

<p>
Without a schema directory, the canonical JSON boundary would drift into:
</p>

<ul>
  <li>implementation-private payload conventions,</li>
  <li>unstated omissions,</li>
  <li>non-portable inspection tooling,</li>
  <li>backend-shaped accidental formats,</li>
  <li>runtime-shaped structural shortcuts.</li>
</ul>

<p>
This directory exists to stop that drift.
</p>

<hr/>

<h2 id="relation-with-the-rest-of-ir">3. Relation with the Rest of IR/</h2>

<p>
The ownership split remains:
</p>

<pre><code>IR/Execution IR.md
   -&gt; architectural identity of the canonical open IR

IR/Derivation rules.md
   -&gt; correspondence obligations from validated meaning to IR

IR/Construction rules.md
   -&gt; material construction of conforming IR payloads

IR/Identity and Mapping.md
   -&gt; attribution, correspondence, and recoverability law

IR/Schema.md
   -&gt; schema posture and interpretation doctrine

IR/schema/
   -&gt; machine-checkable JSON Schema artifacts

IR/Lowering.md
   -&gt; later target-oriented specialization

IR/Backend contract.md
   -&gt; later backend-facing handoff</code></pre>

<p>
Accordingly, the files in this directory MUST be read as machine-checkable schema artifacts, not as replacements for architectural law, derivation law, construction law, or recoverability law.
</p>

<hr/>

<h2 id="schema-family-posture">4. Schema Family Posture</h2>

<p>
The schema family posture in base v0.1 is:
</p>

<ul>
  <li>one root schema for the full canonical Execution IR Document,</li>
  <li>optional component schemas for major record families,</li>
  <li>explicit top-level and unit-level categories,</li>
  <li>explicit support for <code>source_map</code> and <code>correspondence</code>,</li>
  <li>structural support for execution-facing widget distinctions without importing host realization.</li>
</ul>

<p>
This posture is deliberately regular and JSON-tool-friendly.
</p>

<hr/>

<h2 id="published-artifacts">5. Published Artifacts</h2>

<p>
The current schema family includes:
</p>

<pre><code>IR/schema/
├── README.md
├── frog.execution-ir.schema.json
└── frog.execution-ir.object.schema.json</code></pre>

<p>
The intended role of these artifacts is:
</p>

<ul>
  <li><code>frog.execution-ir.schema.json</code> — root schema for the full canonical Execution IR Document,</li>
  <li><code>frog.execution-ir.object.schema.json</code> — component schema for execution-unit object records.</li>
</ul>

<p>
Future additions may include component schemas for:
</p>

<ul>
  <li>connections,</li>
  <li>regions,</li>
  <li>source-map records,</li>
  <li>correspondence records,</li>
  <li>widget-related descriptors.</li>
</ul>

<hr/>

<h2 id="root-schema">6. Root Schema</h2>

<p>
The root schema defines the machine-checkable structure of the whole canonical Execution IR Document.
</p>

<p>
It is the schema that a conforming implementation MUST be able to satisfy when it emits a canonical JSON Execution IR payload for the corresponding schema version.
</p>

<p>
The root schema governs the presence and structural arrangement of:
</p>

<ul>
  <li>document identity,</li>
  <li>execution-unit identity,</li>
  <li>objects,</li>
  <li>connections,</li>
  <li>regions,</li>
  <li><code>source_map</code>,</li>
  <li><code>correspondence</code>.</li>
</ul>

<hr/>

<h2 id="component-schemas">7. Component Schemas</h2>

<p>
Component schemas exist so that major record families can be validated, discussed, evolved, and tested independently without losing alignment with the root schema.
</p>

<p>
In base v0.1, the first component schema is the object schema.
</p>

<p>
A component schema:
</p>

<ul>
  <li>does not replace the root schema,</li>
  <li>does not change the canonical document boundary,</li>
  <li>helps keep the schema family readable and maintainable.</li>
</ul>

<hr/>

<h2 id="what-schema-validation-proves">8. What Schema Validation Proves</h2>

<p>
Schema validation proves that a payload satisfies the machine-checkable JSON structural constraints of the claimed schema version.
</p>

<p>
This includes, for example:
</p>

<ul>
  <li>required fields,</li>
  <li>array presence,</li>
  <li>enum values,</li>
  <li>basic structural composition,</li>
  <li>record-family shape.</li>
</ul>

<hr/>

<h2 id="what-schema-validation-does-not-prove">9. What Schema Validation Does Not Prove</h2>

<p>
Schema validation does not, by itself, prove:
</p>

<ul>
  <li>semantic correctness,</li>
  <li>derivation correctness,</li>
  <li>full recoverability correctness,</li>
  <li>architectural sufficiency,</li>
  <li>backend-readiness for every target family.</li>
</ul>

<p>
A payload may be schema-valid and still be architecturally wrong if it silently loses required distinctions or misrepresents validated meaning.
</p>

<hr/>

<h2 id="widget-object-posture">10. Widget Object Posture</h2>

<p>
The schema family is designed so that canonical JSON IR can represent execution-facing widget interaction while staying distinct from host realization.
</p>

<p>
Accordingly, the schema family supports structural carriers for:
</p>

<ul>
  <li><code>widget_value</code>,</li>
  <li><code>widget_reference</code>,</li>
  <li><code>ui_property_read</code>,</li>
  <li><code>ui_property_write</code>,</li>
  <li><code>ui_method_invoke</code>,</li>
  <li>member-address descriptors,</li>
  <li>widget-related attribution and correspondence.</li>
</ul>

<p>
The schema family does not require:
</p>

<ul>
  <li>host widget trees,</li>
  <li>SVG scene graphs,</li>
  <li>toolkit-native control identifiers,</li>
  <li>runtime UI pointers.</li>
</ul>

<hr/>

<h2 id="versioning-posture">11. Versioning Posture</h2>

<p>
The schema family version is part of the canonical JSON boundary.
</p>

<p>
A payload that claims a given schema version MUST satisfy the published schema for that version.
</p>

<p>
Future versions may:
</p>

<ul>
  <li>extend the schema family,</li>
  <li>split more component schemas,</li>
  <li>tighten constraints where allowed by the repository-wide versioning doctrine,</li>
  <li>add future IR families.</li>
</ul>

<p>
Version evolution must remain aligned with the repository-wide versioning posture rather than becoming ad hoc per implementation.
</p>

<hr/>

<h2 id="practical-reading-order">12. Practical Reading Order</h2>

<p>
A practical reading order is:
</p>

<pre><code>1. IR/Readme.md
2. IR/Execution IR.md
3. IR/Derivation rules.md
4. IR/Construction rules.md
5. IR/Identity and Mapping.md
6. IR/Schema.md
7. IR/schema/README.md
8. IR/schema/frog.execution-ir.schema.json
9. IR/schema/frog.execution-ir.object.schema.json</code></pre>

<p>
That order keeps the schema family grounded in architecture before reading the machine-checkable artifacts.
</p>

<hr/>

<h2 id="status">13. Status</h2>

<p>
In base v0.1, this directory provides the first concrete schema family for the canonical open Execution IR JSON boundary.
</p>

<p>
The current posture is intentionally conservative:
</p>

<ul>
  <li>one root schema,</li>
  <li>one first component schema,</li>
  <li>clear room for future decomposition.</li>
</ul>

<hr/>

<h2 id="license">14. License</h2>

<p>
See the repository-level license information for the licensing terms governing these schema artifacts.
</p>
