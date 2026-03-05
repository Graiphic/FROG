<h1 align="center">🐸 FROG Metadata Specification</h1>

<p align="center">
  Metadata definition for <strong>.frog</strong> files<br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2 id="contents">Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#location">2. Location in <code>.frog</code></a></li>
  <li><a href="#structure">3. Metadata structure</a></li>
  <li><a href="#fields">4. Field definitions</a>
    <ul>
      <li><a href="#field-name">4.1 <code>name</code></a></li>
      <li><a href="#field-description">4.2 <code>description</code></a></li>
      <li><a href="#field-author">4.3 <code>author</code></a></li>
      <li><a href="#field-program-version">4.4 <code>program_version</code></a></li>
      <li><a href="#field-tags">4.5 <code>tags</code></a></li>
      <li><a href="#field-created">4.6 <code>created</code></a></li>
      <li><a href="#field-updated">4.7 <code>updated</code></a></li>
      <li><a href="#field-license">4.8 <code>license</code> (optional)</a></li>
    </ul>
  </li>
  <li><a href="#minimal-example">5. Minimal metadata example</a></li>
  <li><a href="#extended-example">6. Extended metadata example</a></li>
  <li><a href="#validation">7. Validation rules</a></li>
  <li><a href="#extensibility">8. Extensibility</a></li>
  <li><a href="#goals">9. Design goals</a></li>
  <li><a href="#summary">10. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
The <strong>metadata</strong> section describes the identity and documentation of a Frog.
It is intended for indexing, documentation, search, and general project organization.
</p>

<p>
The metadata section <strong>does not affect execution semantics</strong>.
Execution behavior MUST be derived exclusively from the <code>diagram</code>, <code>interface</code>, and validation rules defined by the language.
</p>

<p>
Typical consumers of metadata include:
</p>

<ul>
  <li>Development environments (search, browsing, UI)</li>
  <li>Documentation generators</li>
  <li>Example catalogs</li>
  <li>Library indexers</li>
  <li>Version tracking tools</li>
</ul>

<hr/>

<h2 id="location">2. Location in <code>.frog</code></h2>

<p>
Metadata is defined as a top-level JSON object inside a <code>.frog</code> file.
</p>

<pre>{
  "spec_version": "0.1",
  "metadata": { },
  "interface": { },
  "connector": { },
  "diagram": { },
  "front_panel": { }
}</pre>

<p>
Rules:
</p>

<ul>
  <li><code>metadata</code> MUST be present.</li>
  <li>Runtimes MUST ignore metadata fields for execution decisions.</li>
</ul>

<hr/>

<h2 id="structure">3. Metadata structure</h2>

<p>
A typical metadata object:
</p>

<pre>"metadata": {
  "name": "Add",
  "description": "Adds two floating-point numbers.",
  "author": "Author Name",
  "program_version": "1.0.0",
  "tags": ["math", "arithmetic"],
  "created": "2026-03-05T10:00:00Z",
  "updated": "2026-03-05T12:00:00Z",
  "license": "Apache-2.0"
}</pre>

<p>
Only a minimal subset is required. Additional fields are allowed.
</p>

<hr/>

<h2 id="fields">4. Field definitions</h2>

<h3 id="field-name">4.1 <code>name</code></h3>

<p>
Human-readable name of the Frog.
</p>

<pre>"name": "Add"</pre>

<ul>
  <li>MUST be a string.</li>
  <li>SHOULD be concise and descriptive.</li>
  <li>SHOULD be unique within a library or project scope.</li>
</ul>

<p>
Examples:
</p>

<pre>Add
LowPassFilter
PIDController
ReadTemperature</pre>

<hr/>

<h3 id="field-description">4.2 <code>description</code></h3>

<p>
Short explanation of what the Frog does.
</p>

<pre>"description": "Adds two floating-point numbers."</pre>

<ul>
  <li>MUST be a string.</li>
  <li>SHOULD be one or two sentences.</li>
  <li>SHOULD describe behavior, not implementation details.</li>
</ul>

<hr/>

<h3 id="field-author">4.3 <code>author</code></h3>

<p>
Identifies the creator or maintainer.
</p>

<pre>"author": "Example Labs"</pre>

<ul>
  <li>MUST be a string if present.</li>
  <li>MAY include an organization name.</li>
  <li>MAY include multiple names using a single string.</li>
</ul>

<hr/>

<h3 id="field-program-version">4.4 <code>program_version</code></h3>

<p>
Version of the Frog program itself (not the specification version).
</p>

<pre>"program_version": "1.2.0"</pre>

<ul>
  <li>MUST be a string if present.</li>
  <li>SHOULD follow semantic versioning: <code>MAJOR.MINOR.PATCH</code>.</li>
</ul>

<p>
Examples:
</p>

<pre>1.0.0
2.3.1
0.9.0</pre>

<hr/>

<h3 id="field-tags">4.5 <code>tags</code></h3>

<p>
Keywords used for indexing and searching.
</p>

<pre>"tags": ["math", "arithmetic"]</pre>

<ul>
  <li>MUST be an array of strings if present.</li>
  <li>SHOULD contain short, lowercase keywords.</li>
  <li>SHOULD describe the domain and behavior.</li>
</ul>

<p>
Example tags:
</p>

<pre>math
signal
control
robotics
filter
array
example</pre>

<hr/>

<h3 id="field-created">4.6 <code>created</code></h3>

<p>
Timestamp indicating when the Frog was created.
</p>

<pre>"created": "2026-03-05T10:00:00Z"</pre>

<ul>
  <li>MUST be a string if present.</li>
  <li>SHOULD be ISO 8601.</li>
  <li>SHOULD be expressed in UTC using <code>Z</code>.</li>
</ul>

<p>
Recommended format:
</p>

<pre>YYYY-MM-DDTHH:MM:SSZ</pre>

<hr/>

<h3 id="field-updated">4.7 <code>updated</code></h3>

<p>
Timestamp of the last update.
</p>

<pre>"updated": "2026-03-05T12:00:00Z"</pre>

<ul>
  <li>MUST be a string if present.</li>
  <li>SHOULD be ISO 8601 UTC.</li>
  <li>SHOULD be updated automatically by tools.</li>
</ul>

<hr/>

<h3 id="field-license">4.8 <code>license</code> (optional)</h3>

<p>
Informational license string for the Frog.
</p>

<pre>"license": "Apache-2.0"</pre>

<ul>
  <li>MUST be a string if present.</li>
  <li>SHOULD use SPDX identifiers when possible.</li>
  <li>This field is informational and does not enforce legal behavior.</li>
</ul>

<hr/>

<h2 id="minimal-example">5. Minimal metadata example</h2>

<p>
A minimal valid metadata object:
</p>

<pre>"metadata": {
  "name": "Add",
  "description": "Adds two numbers."
}</pre>

<hr/>

<h2 id="extended-example">6. Extended metadata example</h2>

<pre>"metadata": {
  "name": "PIDController",
  "description": "Implements a proportional–integral–derivative controller.",
  "author": "Example Automation",
  "program_version": "1.2.0",
  "tags": ["control", "pid", "automation"],
  "created": "2026-03-05T10:00:00Z",
  "updated": "2026-03-05T14:00:00Z",
  "license": "Apache-2.0"
}</pre>

<hr/>

<h2 id="validation">7. Validation rules</h2>

<p>
Implementations MUST enforce:
</p>

<ul>
  <li><code>metadata</code> MUST exist.</li>
  <li><code>metadata.name</code> MUST exist and MUST be a string.</li>
  <li><code>metadata.description</code> MUST exist and MUST be a string.</li>
  <li>If present, <code>tags</code> MUST be an array of strings.</li>
  <li>If present, timestamps MUST be strings.</li>
</ul>

<p>
Unknown metadata fields:
</p>

<ul>
  <li>Tools MAY introduce additional fields.</li>
  <li>Runtimes MUST ignore unknown metadata fields.</li>
</ul>

<hr/>

<h2 id="extensibility">8. Extensibility</h2>

<p>
Tools MAY extend metadata with additional fields.
</p>

<pre>"metadata": {
  "name": "Example",
  "description": "Example frog",
  "organization": "Example Labs",
  "documentation_url": "https://example.com/docs"
}</pre>

<p>
Such extensions MUST NOT affect execution and MUST be safely ignorable.
</p>

<hr/>

<h2 id="goals">9. Design goals</h2>

<ul>
  <li>Provide clear identity and documentation.</li>
  <li>Enable indexing and discovery.</li>
  <li>Support stable version tracking.</li>
  <li>Remain tool-agnostic and extensible.</li>
  <li>Remain non-executable and non-authoritative for runtime behavior.</li>
</ul>

<hr/>

<h2 id="summary">10. Summary</h2>

<p>
The metadata section provides descriptive information about a Frog without affecting execution.
It enables documentation, discoverability, indexing, and version tracking while preserving a strict separation between
<strong>program description</strong> and <strong>program execution</strong>.
</p>
