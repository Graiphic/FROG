<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 FROG Cache Specification</h1>

<p align="center">
Definition of optional non-authoritative cache data for <strong>.frog</strong> programs<br/>
<em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2 id="contents">Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#scope">2. Scope</a></li>
  <li><a href="#relation-with-other-specifications">3. Relation with Other Specifications</a></li>
  <li><a href="#design-goals">4. Design Goals</a></li>
  <li><a href="#storage-options">5. Cache Storage Options</a></li>
  <li><a href="#external-cache-file">6. External <code>.frog.cache</code> File</a></li>
  <li><a href="#embedded-cache-object">7. Embedded <code>cache</code> Object</a></li>
  <li><a href="#cache-model">8. Cache Model</a></li>
  <li><a href="#cache-object-structure">9. Cache Object Structure</a></li>
  <li><a href="#top-level-fields">10. Top-Level Fields</a></li>
  <li><a href="#common-cache-entry-kinds">11. Common Cache Entry Kinds</a></li>
  <li><a href="#authority-and-correctness-rules">12. Authority and Correctness Rules</a></li>
  <li><a href="#stale-invalid-or-partial-cache-handling">13. Stale, Invalid, or Partial Cache Handling</a></li>
  <li><a href="#security-considerations">14. Security Considerations</a></li>
  <li><a href="#validation-rules">15. Validation Rules</a></li>
  <li><a href="#examples">16. Examples</a>
    <ul>
      <li><a href="#minimal-embedded-example">16.1 Minimal Embedded Cache Object</a></li>
      <li><a href="#extended-embedded-example">16.2 Extended Embedded Cache Object</a></li>
      <li><a href="#external-sidecar-example">16.3 External Sidecar Cache File</a></li>
    </ul>
  </li>
  <li><a href="#summary">17. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
The <code>cache</code> mechanism stores optional derived data associated with a FROG program.
Its purpose is to accelerate tooling workflows such as loading, validation, indexing, analysis, lowering preparation,
compilation preparation, and execution-oriented representation generation.
</p>

<p>
A common use case is caching an execution-oriented intermediate representation derived from the canonical source
<code>.frog</code> file. However, cache is not limited to execution-oriented representations and MAY also contain other derived artifacts useful to tools.
</p>

<p>
Cache data is <strong>non-authoritative</strong>.
The meaning of a FROG program is always defined by canonical source, never by cache content.
</p>

<p>
Accordingly:
</p>

<ul>
  <li>cache data <strong>MUST NOT</strong> define program semantics,</li>
  <li>cache data <strong>MUST NOT</strong> redefine typing, scheduling, structure identity, or observable behavior,</li>
  <li>cache data <strong>MUST</strong> be safe to delete and regenerate.</li>
</ul>

<hr/>

<h2 id="scope">2. Scope</h2>

<p>
This document specifies:
</p>

<ul>
  <li>the role of optional cache artifacts associated with a FROG,</li>
  <li>the boundary between canonical source and derived cache,</li>
  <li>supported cache storage forms,</li>
  <li>a generic cache container structure,</li>
  <li>correctness, validation, and stale-cache handling rules.</li>
</ul>

<p>
This document does not specify:
</p>

<ul>
  <li>the canonical source representation itself,</li>
  <li>the normative execution semantics of the language,</li>
  <li>the definitive standard execution IR,</li>
  <li>the internal architecture of a particular IDE, compiler, or runtime,</li>
  <li>distribution artifacts such as <code>.frogbin</code>.</li>
</ul>

<hr/>

<h2 id="relation-with-other-specifications">3. Relation with Other Specifications</h2>

<p>
The optional top-level <code>cache</code> section belongs to the canonical source specification in <code>Expression/</code> when cache is embedded inside a <code>.frog</code> file.
</p>

<p>
However, the same cache model MAY also be serialized as an external sidecar artifact in a separate <code>.frog.cache</code> file.
</p>

<p>
Its ownership boundary relative to neighboring source and tooling artifacts is:
</p>

<pre>Source and tooling boundary

metadata    - descriptive program identity
interface   - public typed program boundary
diagram     - authoritative executable graph
front_panel - optional user-facing interaction composition
icon        - optional reusable-node visual identity
ide         - optional IDE-facing authoring preferences and recoverability metadata
cache       - optional derived data for tooling acceleration

Canonical source owns meaning.
Cache owns no meaning.</pre>

<p>
In particular:
</p>

<ul>
  <li><code>cache</code> does not define executable logic. That belongs to canonical source, especially <code>diagram</code>.</li>
  <li><code>cache</code> does not define public API. That belongs to <code>interface</code>.</li>
  <li><code>cache</code> does not define descriptive identity. That belongs to <code>metadata</code>.</li>
  <li><code>cache</code> does not define editor recoverability or source-carried authoring preferences. That belongs to <code>ide</code>.</li>
</ul>

<p>
A practical distinction is:
</p>

<pre>If the data is:
- authoritative and needed to understand the program -> canonical source
- useful to reopen or author more comfortably        -> ide
- derived, optional, and safe to regenerate          -> cache</pre>

<hr/>

<h2 id="design-goals">4. Design Goals</h2>

<ul>
  <li>Speed up loading, validation, lowering, and compilation workflows.</li>
  <li>Avoid recomputing derived artifacts when authoritative source has not changed.</li>
  <li>Keep canonical source authoritative and transparent.</li>
  <li>Allow cache to be safely deleted, ignored, or regenerated.</li>
  <li>Prevent cache data from redefining execution semantics.</li>
  <li>Support clean version-control workflows with minimal diff noise.</li>
</ul>

<hr/>

<h2 id="storage-options">5. Cache Storage Options</h2>

<p>
FROG supports two cache storage options:
</p>

<ul>
  <li><strong>Preferred:</strong> a separate sidecar cache file (<code>.frog.cache</code>)</li>
  <li><strong>Allowed but secondary:</strong> an embedded <code>cache</code> object inside the canonical <code>.frog</code> file</li>
</ul>

<p>
The sidecar file is preferred because it keeps canonical source cleaner and reduces noisy diffs,
merge conflicts, and unnecessary version-control churn.
</p>

<p>
Embedded cache is still allowed for workflows where portability, single-file transport, or source-attached derived artifacts are desirable.
</p>

<hr/>

<h2 id="external-cache-file">6. External <code>.frog.cache</code> File</h2>

<p>
A cache sidecar file is associated with a source file by name.
</p>

<pre>example.frog
example.frog.cache</pre>

<p>
The external cache file MAY be generated automatically by tooling and MAY be deleted at any time.
Tools SHOULD regenerate it when needed.
</p>

<p>
The external cache file is a derived artifact.
It is not part of the authoritative source definition of the program.
</p>

<p>
The sidecar form is the preferred storage strategy for routine tooling workflows.
</p>

<hr/>

<h2 id="embedded-cache-object">7. Embedded <code>cache</code> Object</h2>

<p>
In some workflows, tools MAY embed cache data directly inside the canonical <code>.frog</code> file as an optional top-level <code>cache</code> object.
</p>

<p>
If embedded, the <code>cache</code> object:
</p>

<ul>
  <li><strong>MUST</strong> remain optional,</li>
  <li><strong>MUST</strong> remain non-authoritative,</li>
  <li><strong>MUST NOT</strong> be required for correctness,</li>
  <li><strong>MUST</strong> be safe to remove without changing program meaning.</li>
</ul>

<p>
Embedded cache is allowed for convenience, but it is generally less desirable than a sidecar file for collaborative source-control workflows.
</p>

<hr/>

<h2 id="cache-model">8. Cache Model</h2>

<p>
The cache model is intentionally simple:
</p>

<pre>Canonical source and derived cache

authoritative source
     |
     v
+----------------------+
|     example.frog     |
|----------------------|
| metadata             |
| interface            |
| connector            |
| diagram              |
| front_panel          |
| icon                 |
| ide                  |
| cache (optional)     |
+----------+-----------+
           |
           | derived from authoritative source
           v
+----------------------+
| source_fingerprint   |
+----------+-----------+
           |
           v
+----------------------+
| entries              |
|----------------------|
| frog.ir              |
| frog.validation      |
| frog.analysis        |
| tool-specific data   |
+----------------------+

Cache can be:
- embedded inside .frog
- external in .frog.cache

In all cases cache is:
- derived
- optional
- non-authoritative
- safe to delete</pre>

<p>
The cache container is a derived snapshot keyed to some identifiable source state.
When that source state changes, the cache MAY become stale and SHOULD be treated accordingly.
</p>

<hr/>

<h2 id="cache-object-structure">9. Cache Object Structure</h2>

<p>
Recommended cache structure:
</p>

<pre>"cache": {
  "format_version": "0.1",
  "generator": {
    "tool": "frog-ide",
    "version": "0.1.0"
  },
  "source_fingerprint": "sha256:...",
  "entries": {
    "frog.ir": {
      "kind": "execution_ir",
      "schema_version": "0.1",
      "created": "2026-03-05T12:00:00Z",
      "depends_on": ["interface", "diagram", "front_panel"],
      "data": {}
    }
  }
}</pre>

<p>
This structure is generic enough to support multiple derived artifacts while keeping a stable, simple contract.
</p>

<hr/>

<h2 id="top-level-fields">10. Top-Level Fields</h2>

<h3 id="field-format-version">10.1 <code>format_version</code></h3>

<p>
Version of the cache container format.
</p>

<ul>
  <li><strong>MUST</strong> be a string if present.</li>
  <li><strong>SHOULD</strong> identify the cache-container schema version.</li>
</ul>

<hr/>

<h3 id="field-generator">10.2 <code>generator</code></h3>

<p>
Describes the tool that generated the cache.
</p>

<pre>"generator": {
  "tool": "frog-ide",
  "version": "0.1.0"
}</pre>

<ul>
  <li><strong>MUST</strong> be an object if present.</li>
  <li>All generator fields are informational only.</li>
  <li><strong>MUST NOT</strong> affect program meaning.</li>
</ul>

<hr/>

<h3 id="field-source-fingerprint">10.3 <code>source_fingerprint</code></h3>

<p>
Fingerprint of the authoritative source state from which the cache was derived.
</p>

<ul>
  <li><strong>MUST</strong> be a string if present.</li>
  <li><strong>SHOULD</strong> be stable for identical authoritative source content.</li>
  <li><strong>SHOULD</strong> change when authoritative source content changes.</li>
  <li>Tools <strong>MUST</strong> treat cache as stale if the fingerprint does not match the current authoritative source state.</li>
</ul>

<hr/>

<h3 id="field-entries">10.4 <code>entries</code></h3>

<p>
Collection of named cache entries.
</p>

<ul>
  <li><strong>MUST</strong> be an object if present.</li>
  <li>Each property name identifies one cache entry.</li>
  <li>Unknown entries <strong>MUST</strong> be safely ignorable.</li>
</ul>

<p>
Recommended naming:
</p>

<ul>
  <li><code>frog.*</code> for project-defined common entries</li>
  <li>reverse-domain prefixes for tool-specific entries</li>
</ul>

<p>
Examples:
</p>

<ul>
  <li><code>frog.ir</code></li>
  <li><code>frog.validation</code></li>
  <li><code>frog.analysis</code></li>
  <li><code>com.graiphic.ide.symbol_index</code></li>
</ul>

<hr/>

<h3 id="field-entry-structure">10.5 Entry Structure</h3>

<p>
Each entry SHOULD follow this general structure:
</p>

<pre>"frog.ir": {
  "kind": "execution_ir",
  "schema_version": "0.1",
  "created": "2026-03-05T12:00:00Z",
  "depends_on": ["interface", "diagram", "front_panel"],
  "data": {}
}</pre>

<ul>
  <li><code>kind</code> — high-level category of cache entry</li>
  <li><code>schema_version</code> — version of the entry payload schema</li>
  <li><code>created</code> — creation timestamp in ISO-8601 UTC form</li>
  <li><code>depends_on</code> — source sections or stable dependency identifiers</li>
  <li><code>data</code> — opaque JSON payload containing the derived artifact</li>
</ul>

<p>
The <code>data</code> field MAY contain any valid JSON value, but it <strong>MUST</strong> contain only derived information.
</p>

<hr/>

<h2 id="common-cache-entry-kinds">11. Common Cache Entry Kinds</h2>

<h3 id="execution-ir-cache">11.1 Execution IR Cache</h3>

<p>
Stores a cached execution-oriented intermediate representation derived from authoritative source.
</p>

<p>
This is a common and important cache use case.
It is intended to accelerate repeated validation, lowering, compilation preparation, and execution preparation.
</p>

<hr/>

<h3 id="validation-cache">11.2 Validation Cache</h3>

<p>
Stores derived validation outcomes, such as:
</p>

<ul>
  <li>validation status,</li>
  <li>diagnostic counts,</li>
  <li>resolved warning sets,</li>
  <li>last validated profile or target context.</li>
</ul>

<hr/>

<h3 id="analysis-cache">11.3 Analysis Cache</h3>

<p>
Stores derived structural information, such as:
</p>

<ul>
  <li>symbol indexes,</li>
  <li>dependency analysis results,</li>
  <li>graph summaries,</li>
  <li>editor lookup tables.</li>
</ul>

<hr/>

<h3 id="tool-specific-cache">11.4 Tool-Specific Cache</h3>

<p>
Tools MAY store private derived data under their own namespace, provided that the data remains optional,
safe to ignore, and non-authoritative.
</p>

<hr/>

<h2 id="authority-and-correctness-rules">12. Authority and Correctness Rules</h2>

<p>
Cache data <strong>MUST</strong> be treated as an optimization only.
</p>

<ul>
  <li>Runtimes <strong>MUST NOT</strong> require cache data to execute a program correctly.</li>
  <li>Runtimes <strong>MUST NOT</strong> trust cache data as authoritative truth.</li>
  <li>Tools <strong>MUST</strong> be able to rebuild needed cache data from authoritative source.</li>
  <li>If cache conflicts with authoritative source content, authoritative source content <strong>MUST</strong> win.</li>
  <li>Removing cache <strong>MUST NOT</strong> change the meaning of the program.</li>
</ul>

<p>
Embedded cache does not become authoritative merely because it is physically stored inside the <code>.frog</code> file.
Its storage location does not change its semantic status.
</p>

<p>
Source provenance is not cache. An attestation stored under <code>ide.provenance</code> may include a signature, issuer, digest, or human-review claim for a specific source state. Such evidence is source-carried audit metadata and SHOULD NOT be discarded merely because it is non-executable or because a formatter normally removes derived cache entries.
</p>

<pre>Authority rule

canonical source -> authoritative
embedded cache   -> not authoritative
sidecar cache    -> not authoritative

Physical proximity to source
does not create semantic authority.</pre>

<hr/>

<h2 id="stale-invalid-or-partial-cache-handling">13. Stale, Invalid, or Partial Cache Handling</h2>

<p>
Cache data SHOULD be treated as stale when:
</p>

<ul>
  <li><code>source_fingerprint</code> does not match the current authoritative source content,</li>
  <li><code>format_version</code> is not supported,</li>
  <li>an entry <code>schema_version</code> is not supported,</li>
  <li>an entry payload fails validation,</li>
  <li>required dependencies listed in <code>depends_on</code> have changed,</li>
  <li>the cache payload is incomplete or corrupted.</li>
</ul>

<p>
When cache is stale or invalid:
</p>

<ul>
  <li>tools <strong>SHOULD</strong> discard the invalid portion and regenerate it,</li>
  <li>runtimes <strong>MUST</strong> ignore cache for correctness decisions,</li>
  <li>tools <strong>MAY</strong> keep valid entries and regenerate only stale ones.</li>
</ul>

<p>
A malformed or stale cache MUST degrade performance at worst, not correctness.
</p>

<hr/>

<h2 id="security-considerations">14. Security Considerations</h2>

<p>
Cache content MUST NOT be treated as executable truth.
</p>

<ul>
  <li>Tools <strong>SHOULD</strong> validate cache payloads before use.</li>
  <li>Runtimes <strong>MUST</strong> validate source-derived executable representations regardless of cache presence.</li>
  <li>Tools <strong>SHOULD</strong> avoid storing secrets or sensitive information in cache.</li>
  <li>Cache data MAY be treated as untrusted input.</li>
</ul>

<p>
A malformed cache file MUST NOT compromise correctness.
At worst, it should be ignored and regenerated.
</p>

<hr/>

<h2 id="validation-rules">15. Validation Rules</h2>

<p>
Implementations MUST enforce the following rules:
</p>

<ul>
  <li>If present, embedded <code>cache</code> <strong>MUST</strong> be a JSON object.</li>
  <li>If present, external <code>.frog.cache</code> root content <strong>MUST</strong> be a JSON object.</li>
  <li>If present, <code>format_version</code> <strong>MUST</strong> be a string.</li>
  <li>If present, <code>generator</code> <strong>MUST</strong> be an object.</li>
  <li>If present, <code>source_fingerprint</code> <strong>MUST</strong> be a string.</li>
  <li>If present, <code>entries</code> <strong>MUST</strong> be an object.</li>
  <li>Each entry value inside <code>entries</code> <strong>MUST</strong> be an object.</li>
  <li>If present, entry <code>kind</code> <strong>MUST</strong> be a string.</li>
  <li>If present, entry <code>schema_version</code> <strong>MUST</strong> be a string or integer.</li>
  <li>If present, entry <code>created</code> <strong>MUST</strong> be a string.</li>
  <li>If present, entry <code>depends_on</code> <strong>MUST</strong> be an array of strings.</li>
  <li>Entry <code>data</code> <strong>MAY</strong> be any valid JSON value.</li>
  <li>Unknown cache entries <strong>MUST</strong> be ignorable.</li>
  <li>Invalid, unreadable, stale, or incompatible cache <strong>MUST NOT</strong> change correctness.</li>
</ul>

<hr/>

<h2 id="examples">16. Examples</h2>

<h3 id="minimal-embedded-example">16.1 Minimal Embedded Cache Object</h3>

<pre>"cache": {
  "format_version": "0.1",
  "source_fingerprint": "sha256:...",
  "entries": {
    "frog.ir": {
      "kind": "execution_ir",
      "schema_version": "0.1",
      "data": {}
    }
  }
}</pre>

<hr/>

<h3 id="extended-embedded-example">16.2 Extended Embedded Cache Object</h3>

<pre>"cache": {
  "format_version": "0.1",
  "generator": {
    "tool": "frog-ide",
    "version": "0.1.0"
  },
  "source_fingerprint": "sha256:7d1b...c9",
  "entries": {
    "frog.ir": {
      "kind": "execution_ir",
      "schema_version": "0.1",
      "created": "2026-03-05T12:00:00Z",
      "depends_on": ["interface", "diagram", "front_panel"],
      "data": {
        "graph": {},
        "types": {}
      }
    },
    "frog.validation": {
      "kind": "validation",
      "schema_version": "0.1",
      "created": "2026-03-05T12:00:01Z",
      "depends_on": ["interface", "diagram"],
      "data": {
        "valid": true,
        "errors": []
      }
    }
  }
}</pre>

<hr/>

<h3 id="external-sidecar-example">16.3 External Sidecar Cache File</h3>

<pre>{
  "format_version": "0.1",
  "generator": {
    "tool": "frog-cli",
    "version": "0.1.0"
  },
  "source_fingerprint": "sha256:7d1b...c9",
  "entries": {
    "frog.ir": {
      "kind": "execution_ir",
      "schema_version": "0.1",
      "created": "2026-03-05T12:00:00Z",
      "depends_on": ["interface", "diagram", "front_panel"],
      "data": {
        "graph": {}
      }
    }
  }
}</pre>

<hr/>

<h2 id="summary">17. Summary</h2>

<p>
The cache mechanism provides optional acceleration for tooling workflows.
</p>

<ul>
  <li>Preferred storage is a separate <code>.frog.cache</code> file.</li>
  <li>Embedded cache is allowed but secondary.</li>
  <li>Cache data is derived, optional, and non-authoritative.</li>
  <li>Execution-oriented IR is a primary cache use case, but not the only one.</li>
  <li>Authoritative source content always defines program meaning.</li>
</ul>

<p>
In FROG:
</p>

<pre>source is authoritative
cache is derived
cache is optional
cache is disposable</pre>
