<h1 align="center">🐸 FROG I/O Library Specification</h1>

<p align="center">
Definition of the minimal standard <strong>frog.io</strong> library for FROG v0.1<br/>
<em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#goals">2. Goals</a></li>
  <li><a href="#relation-with-other-specifications">3. Relation with Other Specifications</a></li>
  <li><a href="#role-of-frog-io">4. Role of <code>frog.io</code></a></li>
  <li><a href="#naming-and-namespace">5. Naming and Namespace</a></li>
  <li><a href="#scope-for-v01">6. Scope for v0.1</a></li>
  <li><a href="#path-model">7. Path Model</a></li>
  <li><a href="#typing-model">8. Typing Model</a></li>
  <li><a href="#path-functions">9. Path Functions</a></li>
  <li><a href="#filesystem-query-functions">10. Filesystem Query Functions</a></li>
  <li><a href="#resource-read-functions">11. Resource Read Functions</a></li>
  <li><a href="#diagram-representation">12. Diagram Representation</a></li>
  <li><a href="#validation-rules">13. Validation Rules</a></li>
  <li><a href="#examples">14. Examples</a></li>
  <li><a href="#out-of-scope">15. Out of Scope for v0.1</a></li>
  <li><a href="#summary">16. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the minimal standard <strong>frog.io</strong> library for FROG v0.1.
</p>

<p>
The <code>frog.io</code> library provides a first standardized set of primitives for:
</p>

<ul>
  <li>path manipulation,</li>
  <li>filesystem existence and kind queries,</li>
  <li>directory listing,</li>
  <li>whole-resource text reading,</li>
  <li>whole-resource byte reading.</li>
</ul>

<p>
The goal of this first version is to provide immediately useful file- and path-oriented I/O primitives without requiring a broader standardized model for generic side-effect sequencing, stream handles, mutation, or structured error transport.
</p>

<p>
For FROG v0.1, <code>frog.io</code> is intentionally conservative and primarily read-oriented.
</p>

<hr/>

<h2 id="goals">2. Goals</h2>

<ul>
  <li><strong>Usefulness</strong> — provide a practical first standard I/O surface for ordinary programs.</li>
  <li><strong>Portability</strong> — keep primitive identity and purpose stable across conforming implementations.</li>
  <li><strong>Conservatism</strong> — standardize only the I/O primitives that fit cleanly within the current FROG source and type model.</li>
  <li><strong>Clarity</strong> — define stable names, stable port models, and explicit success signaling where failure is possible.</li>
  <li><strong>Extensibility</strong> — leave room for later libraries or later revisions covering mutation, streams, networking, hardware I/O, and richer transport semantics.</li>
</ul>

<hr/>

<h2 id="relation-with-other-specifications">3. Relation with Other Specifications</h2>

<p>
This document complements the following specifications:
</p>

<ul>
  <li><strong>Expression/Diagram.md</strong> — defines how library primitives are serialized as diagram nodes.</li>
  <li><strong>Expression/Type.md</strong> — defines built-in types and array types used by I/O primitives.</li>
  <li><strong>Expression/Control structures.md</strong> — defines structures that MAY be used to branch on success or failure results from I/O primitives.</li>
  <li><strong>Libraries/Core.md</strong> — defines the minimal core primitive library, which remains distinct from I/O functionality.</li>
</ul>

<p>
This document defines the standardized primitive catalog for <code>frog.io</code>.
It does not redefine the graph model, the type system, or general execution semantics.
</p>

<p>
This document also does not define a general cross-library sequencing model for external effects.
Where a primitive can fail, v0.1 standardizes explicit success signaling at the value level instead.
</p>

<hr/>

<h2 id="role-of-frog-io">4. Role of <code>frog.io</code></h2>

<p>
The <code>frog.io</code> library provides standardized primitives for interacting with path-identified external resources and for reading external resource content into ordinary FROG values.
</p>

<p>
In serialized diagrams, calls to these primitives are represented as <code>primitive</code> nodes whose <code>type</code> field uses the <code>frog.io.*</code> namespace.
</p>

<p>
Examples:
</p>

<pre>
frog.io.path_join
frog.io.exists
frog.io.read_text
frog.io.read_bytes
</pre>

<p>
The <code>frog.io</code> library is distinct from:
</p>

<ul>
  <li><code>frog.core</code>, which contains minimal always-available computational primitives,</li>
  <li><code>frog.ui</code>, which covers widget interaction,</li>
  <li><code>frog.connectivity</code>, which is the appropriate family for language interop, runtime bindings, database access, or external service integration,</li>
  <li>future libraries for networking, hardware access, serialization formats, or runtime coordination.</li>
</ul>

<p>
In other words, <code>frog.io</code> covers foundational file- and path-oriented resource access in v0.1, not general interop or connectivity.
</p>

<hr/>

<h2 id="naming-and-namespace">5. Naming and Namespace</h2>

<p>
FROG library primitives use namespace-qualified identifiers:
</p>

<pre>
frog.&lt;library&gt;.&lt;primitive&gt;
</pre>

<p>
For this document:
</p>

<ul>
  <li><code>frog</code> identifies the language namespace,</li>
  <li><code>io</code> identifies the standard I/O library namespace,</li>
  <li>the final segment identifies the primitive name.</li>
</ul>

<p>
Examples:
</p>

<pre>
frog.io.path_join
frog.io.path_normalize
frog.io.list_directory
frog.io.read_text
frog.io.read_bytes
</pre>

<p>
Primitive names in <code>frog.io</code> SHOULD use lowercase snake_case where multiple words are needed.
</p>

<hr/>

<h2 id="scope-for-v01">6. Scope for v0.1</h2>

<p>
FROG v0.1 standardizes the following minimal <code>frog.io</code> primitive families:
</p>

<ul>
  <li>path manipulation primitives,</li>
  <li>filesystem query primitives,</li>
  <li>directory listing,</li>
  <li>whole-resource read primitives for text and raw bytes.</li>
</ul>

<p>
FROG v0.1 does <strong>not</strong> attempt to define:
</p>

<ul>
  <li>file mutation primitives such as write, copy, move, rename, or delete,</li>
  <li>stream- or handle-based I/O,</li>
  <li>partial reads, buffered readers, or cursor-based access,</li>
  <li>network sockets, HTTP primitives, or remote transport protocols,</li>
  <li>hardware I/O primitives,</li>
  <li>language interop primitives such as Python, C/C++, or .NET calls,</li>
  <li>database access primitives such as SQL queries, connections, or transactions,</li>
  <li>structured path types distinct from <code>string</code>,</li>
  <li>advanced structured error objects or exception transport.</li>
</ul>

<hr/>

<h2 id="path-model">7. Path Model</h2>

<p>
In FROG v0.1, paths used by <code>frog.io</code> primitives are represented as values of type <code>string</code>.
</p>

<p>
No distinct built-in <code>path</code> type is standardized in v0.1.
Accordingly:
</p>

<ul>
  <li>all path inputs and outputs in this document use <code>string</code>,</li>
  <li>the exact platform path syntax is not standardized by the source language alone,</li>
  <li>the active execution profile defines supported path syntax, normalization behavior, and platform-specific conventions.</li>
</ul>

<p>
The source-level meaning of a path primitive remains stable even when platform details differ.
</p>

<hr/>

<h2 id="typing-model">8. Typing Model</h2>

<p>
All <code>frog.io</code> primitives are typed according to <strong>Expression/Type.md</strong>.
</p>

<p>
In v0.1, the following built-in types are especially relevant to this library:
</p>

<ul>
  <li><code>string</code> — used for paths and textual content,</li>
  <li><code>bool</code> — used for success and predicate-style outputs,</li>
  <li><code>array&lt;string&gt;</code> — used for directory entry lists,</li>
  <li><code>array&lt;u8&gt;</code> — used for raw byte content.</li>
</ul>

<p>
Unless stated otherwise:
</p>

<ul>
  <li>all required inputs MUST exist and be type-compatible,</li>
  <li>all outputs MUST always be present,</li>
  <li>primitives that can fail MUST expose a <code>success</code> output of type <code>bool</code>,</li>
  <li>when <code>success = false</code>, any data output MUST still be well-typed and MUST take its defined fallback value.</li>
</ul>

<p>
Fallback values in this document are standardized as follows:
</p>

<ul>
  <li>empty string: <code>""</code>,</li>
  <li>empty dynamic byte array: <code>array&lt;u8&gt;</code> with zero elements,</li>
  <li>empty dynamic string array: <code>array&lt;string&gt;</code> with zero elements.</li>
</ul>

<hr/>

<h2 id="path-functions">9. Path Functions</h2>

<h3>9.1 <code>frog.io.path_join</code></h3>

<p>
Combines a base path and a child path fragment into a combined path string according to the active execution profile.
</p>

<ul>
  <li>input ports: <code>base</code>, <code>child</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>
Both inputs and the output are of type <code>string</code>.
</p>

<h3>9.2 <code>frog.io.path_normalize</code></h3>

<p>
Normalizes a path string according to the active execution profile.
</p>

<ul>
  <li>input port: <code>path</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>
Both the input and the output are of type <code>string</code>.
</p>

<h3>9.3 <code>frog.io.path_parent</code></h3>

<p>
Returns the parent path of the given path string according to the active execution profile.
</p>

<ul>
  <li>input port: <code>path</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>
Both the input and the output are of type <code>string</code>.
</p>

<h3>9.4 <code>frog.io.path_name</code></h3>

<p>
Returns the final name component of the given path string.
</p>

<ul>
  <li>input port: <code>path</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>
Both the input and the output are of type <code>string</code>.
</p>

<hr/>

<h2 id="filesystem-query-functions">10. Filesystem Query Functions</h2>

<h3>10.1 <code>frog.io.exists</code></h3>

<p>
Returns whether the given path currently resolves to an existing filesystem entry under the active execution profile.
</p>

<ul>
  <li>input port: <code>path</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>path: string</code></li>
  <li><code>result: bool</code></li>
</ul>

<h3>10.2 <code>frog.io.is_file</code></h3>

<p>
Returns whether the given path currently resolves to a regular file under the active execution profile.
</p>

<ul>
  <li>input port: <code>path</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>path: string</code></li>
  <li><code>result: bool</code></li>
</ul>

<h3>10.3 <code>frog.io.is_directory</code></h3>

<p>
Returns whether the given path currently resolves to a directory under the active execution profile.
</p>

<ul>
  <li>input port: <code>path</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>path: string</code></li>
  <li><code>result: bool</code></li>
</ul>

<h3>10.4 <code>frog.io.list_directory</code></h3>

<p>
Lists the immediate entries of a directory.
</p>

<ul>
  <li>input port: <code>path</code></li>
  <li>output ports: <code>entries</code>, <code>success</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>path: string</code></li>
  <li><code>entries: array&lt;string&gt;</code></li>
  <li><code>success: bool</code></li>
</ul>

<p>
Rules:
</p>

<ul>
  <li>if <code>success = true</code>, <code>entries</code> contains the immediate entry names produced for that directory by the active execution profile,</li>
  <li>if <code>success = false</code>, <code>entries</code> MUST be the empty array,</li>
  <li>the primitive MUST NOT perform recursive traversal in v0.1,</li>
  <li>the returned ordering MUST be deterministic within the active execution profile for a stable directory snapshot.</li>
</ul>

<hr/>

<h2 id="resource-read-functions">11. Resource Read Functions</h2>

<h3>11.1 <code>frog.io.read_text</code></h3>

<p>
Reads the entire textual content of a resource identified by a path.
</p>

<ul>
  <li>input port: <code>path</code></li>
  <li>output ports: <code>content</code>, <code>success</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>path: string</code></li>
  <li><code>content: string</code></li>
  <li><code>success: bool</code></li>
</ul>

<p>
Rules:
</p>

<ul>
  <li>if <code>success = true</code>, <code>content</code> contains the decoded textual content defined by the active execution profile,</li>
  <li>if <code>success = false</code>, <code>content</code> MUST be the empty string,</li>
  <li>the exact supported text encodings and newline normalization behavior are profile-defined in v0.1.</li>
</ul>

<h3>11.2 <code>frog.io.read_bytes</code></h3>

<p>
Reads the entire raw byte content of a resource identified by a path.
</p>

<ul>
  <li>input port: <code>path</code></li>
  <li>output ports: <code>content</code>, <code>success</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>path: string</code></li>
  <li><code>content: array&lt;u8&gt;</code></li>
  <li><code>success: bool</code></li>
</ul>

<p>
Rules:
</p>

<ul>
  <li>if <code>success = true</code>, <code>content</code> contains the full byte sequence read from the referenced resource,</li>
  <li>if <code>success = false</code>, <code>content</code> MUST be the empty byte array.</li>
</ul>

<hr/>

<h2 id="diagram-representation">12. Diagram Representation</h2>

<p>
Calls to <code>frog.io</code> primitives are serialized as <code>primitive</code> nodes in the diagram.
</p>

<p>
Examples:
</p>

<pre><code>{
  "id": "join_1",
  "kind": "primitive",
  "type": "frog.io.path_join"
}</code></pre>

<pre><code>{
  "id": "exists_1",
  "kind": "primitive",
  "type": "frog.io.exists"
}</code></pre>

<pre><code>{
  "id": "read_text_1",
  "kind": "primitive",
  "type": "frog.io.read_text"
}</code></pre>

<p>
The exact port existence, direction, and typing of these nodes are resolved from this specification together with the type system and the graph rules.
</p>

<hr/>

<h2 id="validation-rules">13. Validation Rules</h2>

<p>
Implementations MUST enforce the following rules:
</p>

<ul>
  <li>every <code>frog.io</code> primitive reference MUST identify a valid standardized I/O primitive name,</li>
  <li>all required input ports MUST exist and be type-compatible,</li>
  <li>all produced output ports MUST match the primitive definition,</li>
  <li>all path inputs in this document MUST use type <code>string</code> in v0.1,</li>
  <li>all array outputs in this document MUST use the exact standardized element type declared by the primitive definition.</li>
</ul>

<p>
For query-style predicates:
</p>

<ul>
  <li><code>frog.io.exists</code>, <code>frog.io.is_file</code>, and <code>frog.io.is_directory</code> MUST produce outputs of type <code>bool</code>.</li>
</ul>

<p>
For result-bearing read and listing primitives:
</p>

<ul>
  <li>a <code>success</code> output MUST exist,</li>
  <li><code>success</code> MUST be of type <code>bool</code>,</li>
  <li>fallback values for unsuccessful results MUST match the definitions in this specification.</li>
</ul>

<hr/>

<h2 id="examples">14. Examples</h2>

<h3>14.1 Join a base directory and filename</h3>

<pre><code>{
  "id": "join_1",
  "kind": "primitive",
  "type": "frog.io.path_join"
}</code></pre>

<p>
Conceptual ports:
</p>

<pre>
base, child → result
</pre>

<h3>14.2 Read a text file</h3>

<pre><code>{
  "id": "read_text_1",
  "kind": "primitive",
  "type": "frog.io.read_text"
}</code></pre>

<p>
Conceptual ports:
</p>

<pre>
path → content, success
</pre>

<h3>14.3 Branch on read success</h3>

<pre><code>{
  "interface": {
    "inputs": [
      { "id": "path", "type": "string" }
    ],
    "outputs": [
      { "id": "content", "type": "string" },
      { "id": "ok", "type": "bool" }
    ]
  },
  "diagram": {
    "nodes": [
      { "id": "input_path", "kind": "interface_input", "interface_port": "path" },
      { "id": "read_text_1", "kind": "primitive", "type": "frog.io.read_text" },
      { "id": "output_content", "kind": "interface_output", "interface_port": "content" },
      { "id": "output_ok", "kind": "interface_output", "interface_port": "ok" }
    ],
    "edges": [
      { "id": "e1", "from": { "node": "input_path", "port": "value" }, "to": { "node": "read_text_1", "port": "path" } },
      { "id": "e2", "from": { "node": "read_text_1", "port": "content" }, "to": { "node": "output_content", "port": "value" } },
      { "id": "e3", "from": { "node": "read_text_1", "port": "success" }, "to": { "node": "output_ok", "port": "value" } }
    ]
  }
}</code></pre>

<h3>14.4 Read raw bytes</h3>

<pre><code>{
  "id": "read_bytes_1",
  "kind": "primitive",
  "type": "frog.io.read_bytes"
}</code></pre>

<p>
Conceptual ports:
</p>

<pre>
path → content, success
</pre>

<hr/>

<h2 id="out-of-scope">15. Out of Scope for v0.1</h2>

<p>
The following are outside the strict scope of <code>frog.io</code> in v0.1:
</p>

<ul>
  <li>file creation, writing, copying, moving, renaming, or deletion,</li>
  <li>stream handles, cursors, buffered readers, or partial-range reads,</li>
  <li>directory creation or mutation,</li>
  <li>network I/O, HTTP, sockets, or remote resource transports,</li>
  <li>hardware I/O, DAQ, serial, fieldbus, or instrument drivers,</li>
  <li>watchers, notifications, or asynchronous event-driven resource monitoring,</li>
  <li>language interop such as Python, C/C++, .NET, or foreign-function calls,</li>
  <li>database connectivity such as SQL connections, queries, or transactions,</li>
  <li>structured error objects, error codes, or exception payload types,</li>
  <li>a dedicated built-in <code>path</code> type distinct from <code>string</code>.</li>
</ul>

<hr/>

<h2 id="summary">16. Summary</h2>

<p>
The <code>frog.io</code> library defines a minimal standard I/O primitive set for FROG v0.1.
</p>

<p>
It provides:
</p>

<ul>
  <li>basic path composition and normalization,</li>
  <li>filesystem existence and kind queries,</li>
  <li>directory listing,</li>
  <li>whole-resource text reads,</li>
  <li>whole-resource byte reads.</li>
</ul>

<p>
This first version is intentionally conservative.
Its purpose is to establish a useful and portable initial I/O vocabulary while leaving mutation, streams, network transports, hardware access, interop, database access, and richer error transport to later standardization work.
</p>

<hr/>

<p align="center">
End of FROG I/O Library Specification
</p>
