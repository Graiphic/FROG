<h1 align="center">🐸 FROG Cache (Optional Execution IR) Specification</h1>

<p align="center">
Optional derived cache data for <strong>.frog</strong> programs<br/>
<em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#purpose">2. Purpose</a></li>
  <li><a href="#design-goals">3. Design goals</a></li>
  <li><a href="#storage-options">4. Cache storage options</a></li>
  <li><a href="#separate-cache-file">5. Separate <code>.frog.cache</code> file</a></li>
  <li><a href="#embedded-cache-object">6. Embedded <code>cache</code> object (optional)</a></li>
  <li><a href="#cache-semantics">7. Cache semantics</a></li>
  <li><a href="#structure">8. Cache object structure</a></li>
  <li><a href="#authoritativeness">9. Non-authoritative rules</a></li>
  <li><a href="#stale-cache">10. Stale or invalid cache handling</a></li>
  <li><a href="#security">11. Security considerations</a></li>
  <li><a href="#validation">12. Validation rules</a></li>
  <li><a href="#examples">13. Examples</a></li>
  <li><a href="#summary">14. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
The <strong>cache</strong> mechanism provides optional, derived, non-authoritative data used to accelerate FROG tooling workflows.
</p>

<p>
A common use case is caching a derived <strong>FROG Execution IR</strong> generated from the source <code>.frog</code> file.
This cache may also include validation results, analysis data, normalized graph information, or other recomputable artifacts.
</p>

<p>
Cache data MUST NOT define program meaning.
The authoritative source of a program remains the <strong>FROG Expression</strong> stored in the <code>.frog</code> file.
</p>

<p>
Implementations MUST treat cache data as optional.
Deleting cache data MUST NOT change program semantics.
</p>

<hr/>

<h2 id="purpose">2. Purpose</h2>

<p>
The cache exists only to improve performance and tooling responsiveness.
</p>

<p>
Typical uses include:
</p>

<ul>
  <li>speeding up program loading</li>
  <li>avoiding repeated validation work</li>
  <li>reusing previously derived execution data</li>
  <li>reducing compile preparation time</li>
  <li>persisting non-authoritative analysis results between sessions</li>
</ul>

<p>
The cache is never the canonical program representation.
</p>

<hr/>

<h2 id="design-goals">3. Design goals</h2>

<ul>
  <li>Accelerate loading, validation, and compilation workflows</li>
  <li>Avoid unnecessary recomputation of derived execution data</li>
  <li>Remain safe to delete at any time</li>
  <li>Prevent cached data from redefining execution semantics</li>
  <li>Support tool interoperability</li>
  <li>Reduce unnecessary noise in version control workflows</li>
</ul>

<hr/>

<h2 id="storage-options">4. Cache storage options</h2>

<p>
FROG supports two cache storage options:
</p>

<ul>
  <li><strong>Preferred:</strong> a separate cache file named <code>.frog.cache</code></li>
  <li><strong>Optional:</strong> an embedded <code>cache</code> object inside the <code>.frog</code> file</li>
</ul>

<p>
A separate cache file is preferred because it avoids unnecessary source diffs and merge conflicts.
</p>

<p>
Embedded cache data is allowed for workflows where a single-file representation is useful, but it remains optional and non-authoritative.
</p>

<hr/>

<h2 id="separate-cache-file">5. Separate <code>.frog.cache</code> file</h2>

<p>
A separate cache file is associated with a source file by name.
</p>

<pre>
example.frog
example.frog.cache
</pre>

<p>
The cache file MAY be generated automatically by IDEs, compilers, or supporting tools.
It MAY be deleted at any time without affecting program meaning.
</p>

<p>
Tools SHOULD regenerate the cache when needed.
</p>

<hr/>

<h2 id="embedded-cache-object">6. Embedded <code>cache</code> object (optional)</h2>

<p>
Some tools MAY embed cache data directly inside the <code>.frog</code> file under a top-level <code>cache</code> object.
</p>

<p>
If embedded, the cache object MUST remain optional and non-authoritative.
It MUST NOT redefine, override, or reinterpret the source program description.
</p>

<p>
Implementations MUST be able to ignore embedded cache content and reconstruct all required derived data from the authoritative source sections.
</p>

<hr/>

<h2 id="cache-semantics">7. Cache semantics</h2>

<p>
The cache stores <strong>derived</strong> data only.
It is not part of the source language semantics.
</p>

<p>
Examples of acceptable cached content include:
</p>

<ul>
  <li>derived FROG Execution IR</li>
  <li>normalized graph forms</li>
  <li>validation summaries</li>
  <li>resolved bindings</li>
  <li>dependency analysis results</li>
  <li>compiler preparation metadata</li>
</ul>

<p>
Examples of content that MUST NOT be treated as authoritative in cache include:
</p>

<ul>
  <li>the canonical source diagram definition</li>
  <li>the canonical interface definition</li>
  <li>the canonical front panel definition</li>
  <li>any source-level meaning that is not recoverable from the Expression</li>
</ul>

<p>
In other words, cache data may describe a derived execution view of the program, but it does not replace the source Expression.
</p>

<hr/>

<h2 id="structure">8. Cache object structure</h2>

<p>
Recommended structure:
</p>

<pre>"cache": {
  "cache_version": "0.1",
  "execution_ir_version": "0.1",
  "generated_by": "frog-ide",
  "generated_at": "2026-03-05T12:00:00Z",
  "source_hash": "sha256:...",
  "program_model_hash": "sha256:...",
  "execution_ir": {}
}</pre>

<h3>Field definitions</h3>

<ul>
  <li><code>cache_version</code> — cache schema version (string)</li>
  <li><code>execution_ir_version</code> — cached execution IR schema version (string)</li>
  <li><code>generated_by</code> — tool identifier (string)</li>
  <li><code>generated_at</code> — ISO 8601 timestamp (string)</li>
  <li><code>source_hash</code> — hash of the canonical source Expression (string)</li>
  <li><code>program_model_hash</code> — optional hash of the normalized internal program model used to generate the cache (string)</li>
  <li><code>execution_ir</code> — cached derived execution representation (object)</li>
</ul>

<p>
The <code>source_hash</code> is the primary mechanism used to detect stale cache content.
</p>

<p>
The <code>program_model_hash</code> is optional and may be used by tools that normalize the source into an internal model before generating execution data.
</p>

<hr/>

<h2 id="authoritativeness">9. Non-authoritative rules</h2>

<p>
Cache data MUST be treated as an optimization only.
</p>

<ul>
  <li>Tools MUST be able to rebuild cache data from the source <code>.frog</code> file.</li>
  <li>Runtimes MUST NOT require cache data in order to execute a program.</li>
  <li>Runtimes MUST NOT trust cache data as the source of correctness.</li>
  <li>Tools MUST ignore cache content that conflicts with authoritative source content.</li>
</ul>

<p>
If cache data conflicts with the source Expression, the source Expression MUST win.
</p>

<hr/>

<h2 id="stale-cache">10. Stale or invalid cache handling</h2>

<p>
Cache data is considered stale or invalid if one or more of the following conditions apply:
</p>

<ul>
  <li><code>source_hash</code> does not match the current source Expression</li>
  <li><code>execution_ir_version</code> is not supported</li>
  <li>the cached payload fails structural validation</li>
  <li>the cached payload is incompatible with the current tool version</li>
  <li>required source sections cannot be matched to the cached derived data</li>
</ul>

<p>
When cache data is stale or invalid:
</p>

<ul>
  <li>Tools SHOULD discard it.</li>
  <li>Tools SHOULD regenerate it automatically when appropriate.</li>
  <li>Runtimes MUST ignore it for correctness decisions.</li>
  <li>Compilation and execution workflows MUST proceed from source-derived data.</li>
</ul>

<hr/>

<h2 id="security">11. Security considerations</h2>

<p>
Cache data MUST be treated as untrusted derived input.
</p>

<ul>
  <li>Tools SHOULD validate cache payloads before use.</li>
  <li>Tools SHOULD avoid storing secrets or sensitive user data in cache files.</li>
  <li>Runtimes MUST validate execution inputs regardless of cache presence.</li>
  <li>Malformed cache data MUST NOT be allowed to redefine source semantics.</li>
</ul>

<p>
A cache file may be corrupted, stale, manually edited, tool-generated by another implementation, or intentionally malformed.
Implementations must therefore treat it defensively.
</p>

<hr/>

<h2 id="validation">12. Validation rules</h2>

<p>
Implementations MUST enforce the following rules:
</p>

<ul>
  <li>If present, <code>cache</code> MUST be a JSON object.</li>
  <li>If present, <code>cache.execution_ir</code> MUST be a JSON object.</li>
  <li>If present, <code>cache.execution_ir_version</code> MUST be a string.</li>
  <li>If present, <code>cache.cache_version</code> MUST be a string.</li>
  <li>If present, <code>cache.source_hash</code> SHOULD be present to detect staleness.</li>
  <li>Implementations MUST be able to ignore cache content completely.</li>
  <li>Runtimes MUST ignore cache data for correctness decisions.</li>
</ul>

<p>
Additional tool-specific validation rules MAY be applied.
</p>

<hr/>

<h2 id="examples">13. Examples</h2>

<h3>13.1 Minimal cache object</h3>

<pre>"cache": {
  "cache_version": "0.1",
  "execution_ir_version": "0.1",
  "source_hash": "sha256:...",
  "execution_ir": {}
}</pre>

<h3>13.2 Extended cache object</h3>

<pre>"cache": {
  "cache_version": "0.1",
  "execution_ir_version": "0.1",
  "generated_by": "frog-ide",
  "generated_at": "2026-03-05T12:00:00Z",
  "source_hash": "sha256:7d1b...c9",
  "program_model_hash": "sha256:3a8d...ef",
  "execution_ir": {
    "graph": {},
    "types": {},
    "validation_state": {
      "valid": true,
      "errors": []
    }
  }
}</pre>

<h3>13.3 Separate cache file example</h3>

<pre>{
  "cache_version": "0.1",
  "execution_ir_version": "0.1",
  "generated_by": "frog-compiler",
  "generated_at": "2026-03-05T12:00:00Z",
  "source_hash": "sha256:7d1b...c9",
  "execution_ir": {
    "graph": {}
  }
}</pre>

<hr/>

<h2 id="summary">14. Summary</h2>

<p>
Cache data provides optional acceleration for tooling workflows.
It is always derived, always optional, and always non-authoritative.
</p>

<ul>
  <li>The authoritative source remains the <code>.frog</code> Expression.</li>
  <li>The preferred storage form is a separate <code>.frog.cache</code> file.</li>
  <li>Embedded cache is allowed but discouraged for source control stability.</li>
  <li>Cache data may include derived FROG Execution IR and related analysis artifacts.</li>
  <li>Cache data is safe to delete and safe to ignore.</li>
</ul>
