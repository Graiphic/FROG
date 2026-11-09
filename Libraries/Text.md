<h1 align="center">🐸 FROG Text Library Specification</h1>

<p align="center">
Definition of the standard <strong>frog.text</strong> library for FROG v0.1<br/>
<em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#goals">2. Goals</a></li>
  <li><a href="#relation-with-other-specifications">3. Relation with Other Specifications</a></li>
  <li><a href="#role-of-frog-text">4. Role of <code>frog.text</code></a></li>
  <li><a href="#naming-and-namespace">5. Naming and Namespace</a></li>
  <li><a href="#scope-for-v01">6. Scope for v0.1</a></li>
  <li><a href="#library-categories">7. Library Categories</a></li>
  <li><a href="#typing-model">8. Typing Model</a></li>
  <li><a href="#construction-and-inspection-functions">9. Construction and Inspection Functions</a></li>
  <li><a href="#search-and-predicate-functions">10. Search and Predicate Functions</a></li>
  <li><a href="#segmentation-functions">11. Segmentation Functions</a></li>
  <li><a href="#diagram-representation">12. Diagram Representation</a></li>
  <li><a href="#validation-rules">13. Validation Rules</a></li>
  <li><a href="#examples">14. Examples</a></li>
  <li><a href="#out-of-scope">15. Out of Scope for v0.1</a></li>
  <li><a href="#summary">16. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the standard <strong>frog.text</strong> library for FROG v0.1.
</p>

<p>
The <code>frog.text</code> library contains foundational text-oriented primitive functions for:
</p>

<ul>
  <li>string construction,</li>
  <li>string inspection,</li>
  <li>text search and predicates,</li>
  <li>basic segmentation and recomposition.</li>
</ul>

<p>
These functions form the first standardized text-processing vocabulary of FROG.
They are intended to be portable, deterministic, and sufficient for common string workflows without expanding <code>frog.core</code> beyond its minimal role.
</p>

<hr/>

<h2 id="goals">2. Goals</h2>

<ul>
  <li><strong>Separation of concerns</strong> — keep text processing outside <code>frog.core</code>.</li>
  <li><strong>Practical utility</strong> — provide a small but useful first standard text library.</li>
  <li><strong>Determinism</strong> — define stable observable behavior for valid inputs.</li>
  <li><strong>Portability</strong> — keep semantics independent from any single IDE or runtime implementation.</li>
  <li><strong>Extensibility</strong> — leave room for future formatting, parsing, regex, locale, and encoding libraries.</li>
</ul>

<hr/>

<h2 id="relation-with-other-specifications">3. Relation with Other Specifications</h2>

<p>
This document complements the following specifications:
</p>

<ul>
  <li><strong>Expression/Diagram.md</strong> — defines how primitive functions are serialized as diagram nodes.</li>
  <li><strong>Expression/Type.md</strong> — defines the <code>string</code>, <code>bool</code>, integer, and array type families used by text primitives.</li>
  <li><strong>Libraries/Core.md</strong> — defines the minimal built-in primitive library that remains separate from <code>frog.text</code>.</li>
  <li><strong>IDE/Palette.md</strong> — defines how text entries may be surfaced in the IDE palette.</li>
</ul>

<p>
This document defines the standardized <code>frog.text</code> primitive catalog.
It does not redefine the graph structure, the type system, or the IDE presentation model.
</p>

<hr/>

<h2 id="role-of-frog-text">4. Role of <code>frog.text</code></h2>

<p>
The <code>frog.text</code> library provides standardized text-processing primitives for FROG programs.
It is a sibling library to <code>frog.core</code>, not an extension of the minimal built-in core.
</p>

<p>
In language terms, these are ordinary primitive functions.
In the serialized diagram representation defined by <code>Expression/Diagram.md</code>, calls to these functions are represented as <code>primitive</code> nodes.
</p>

<p>
Therefore:
</p>

<ul>
  <li><code>frog.text.concat</code> is a standardized text-library primitive,</li>
  <li>in a diagram, that function call appears as a <code>primitive</code> node with <code>type = "frog.text.concat"</code>.</li>
</ul>

<hr/>

<h2 id="naming-and-namespace">5. Naming and Namespace</h2>

<p>
FROG uses the following general namespace pattern for standardized library primitives:
</p>

<pre>
frog.&lt;library&gt;.&lt;primitive&gt;
</pre>

<p>
For this document:
</p>

<ul>
  <li><code>frog</code> identifies the language namespace,</li>
  <li><code>text</code> identifies the standardized text library,</li>
  <li>the final segment identifies the primitive name.</li>
</ul>

<p>
Examples:
</p>

<pre>
frog.text.concat
frog.text.length
frog.text.substring
frog.text.contains
frog.text.split
</pre>

<p>
Primitive names in <code>frog.text</code> SHOULD use lowercase snake_case where multiple words are needed.
</p>

<hr/>

<h2 id="scope-for-v01">6. Scope for v0.1</h2>

<p>
FROG v0.1 standardizes the following foundational text primitives:
</p>

<ul>
  <li>construction and inspection functions,</li>
  <li>basic search and predicate functions,</li>
  <li>basic segmentation and recomposition functions.</li>
</ul>

<p>
FROG v0.1 does <strong>not</strong> attempt to standardize:
</p>

<ul>
  <li>locale-aware collation,</li>
  <li>regular expressions,</li>
  <li>advanced formatting,</li>
  <li>numeric or boolean parsing/formatting conversions,</li>
  <li>byte-level encoding conversions,</li>
  <li>markup-oriented text libraries such as XML, JSON, or HTML processing.</li>
</ul>

<hr/>

<h2 id="library-categories">7. Library Categories</h2>

<p>
The standardized <code>frog.text</code> library is organized into the following categories:
</p>

<ul>
  <li><strong>Construction &amp; Inspection</strong></li>
  <li><strong>Search &amp; Predicates</strong></li>
  <li><strong>Segmentation</strong></li>
</ul>

<p>
This categorization is semantic.
It does not impose a mandatory palette layout, but it provides a stable foundation for IDE organization.
</p>

<hr/>

<h2 id="typing-model">8. Typing Model</h2>

<p>
All <code>frog.text</code> primitives are typed according to <strong>Expression/Type.md</strong>.
</p>

<p>
Unless stated otherwise:
</p>

<ul>
  <li>all text inputs and outputs use the canonical <code>string</code> type,</li>
  <li>index and length quantities use <code>u32</code> in v0.1,</li>
  <li>boolean predicate outputs use <code>bool</code>,</li>
  <li>split results use <code>array&lt;string&gt;</code>,</li>
  <li>all functions in this document are stateless and side-effect-free.</li>
</ul>

<p>
No implicit coercion is introduced here beyond the general FROG type rules.
In particular, this document does not define implicit conversions between <code>string</code> and numeric or boolean types.
</p>

<p>
Because <code>Expression/Type.md</code> does not standardize the internal encoding or memory representation of <code>string</code> in v0.1, the primitives in this document operate on the abstract ordered textual content of a <code>string</code> value rather than on raw byte storage.
Implementations MUST remain self-consistent for a given active execution profile.
</p>

<hr/>

<h2 id="construction-and-inspection-functions">9. Construction and Inspection Functions</h2>

<h3>9.1 <code>frog.text.concat</code></h3>

<p>
Concatenates two string values.
</p>

<ul>
  <li>input ports: <code>a</code>, <code>b</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>
Both inputs MUST be of type <code>string</code>.
The output type is <code>string</code>.
</p>

<h3>9.2 <code>frog.text.length</code></h3>

<p>
Returns the length of a string.
</p>

<ul>
  <li>input port: <code>text</code></li>
  <li>output port: <code>count</code></li>
</ul>

<p>
The input MUST be of type <code>string</code>.
The output type is <code>u32</code>.
</p>

<h3>9.3 <code>frog.text.substring</code></h3>

<p>
Extracts a substring from a string.
</p>

<ul>
  <li>input ports: <code>text</code>, <code>start</code>, <code>length</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>
Rules:
</p>

<ul>
  <li><code>text</code> MUST be of type <code>string</code>,</li>
  <li><code>start</code> MUST be of type <code>u32</code>,</li>
  <li><code>length</code> MUST be of type <code>u32</code>,</li>
  <li>the output type is <code>string</code>.</li>
</ul>

<p>
Semantic rules:
</p>

<ul>
  <li>if <code>start</code> is greater than or equal to the length of <code>text</code>, the result MUST be the empty string,</li>
  <li>if <code>start + length</code> exceeds the available suffix length, the result MUST be clipped to the end of <code>text</code>.</li>
</ul>

<hr/>

<h2 id="search-and-predicate-functions">10. Search and Predicate Functions</h2>

<h3>10.1 <code>frog.text.contains</code></h3>

<p>
Returns whether a string contains another string as a contiguous substring.
</p>

<ul>
  <li>input ports: <code>text</code>, <code>needle</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>
Both inputs MUST be of type <code>string</code>.
The output type is <code>bool</code>.
</p>

<p>
An empty <code>needle</code> MUST yield <code>true</code>.
</p>

<h3>10.2 <code>frog.text.starts_with</code></h3>

<p>
Returns whether a string starts with a given prefix.
</p>

<ul>
  <li>input ports: <code>text</code>, <code>prefix</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>
Both inputs MUST be of type <code>string</code>.
The output type is <code>bool</code>.
</p>

<p>
An empty <code>prefix</code> MUST yield <code>true</code>.
</p>

<h3>10.3 <code>frog.text.ends_with</code></h3>

<p>
Returns whether a string ends with a given suffix.
</p>

<ul>
  <li>input ports: <code>text</code>, <code>suffix</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>
Both inputs MUST be of type <code>string</code>.
The output type is <code>bool</code>.
</p>

<p>
An empty <code>suffix</code> MUST yield <code>true</code>.
</p>

<h3>10.4 <code>frog.text.find_first</code></h3>

<p>
Finds the first occurrence of a substring.
</p>

<ul>
  <li>input ports: <code>text</code>, <code>needle</code></li>
  <li>output ports: <code>found</code>, <code>index</code></li>
</ul>

<p>
Rules:
</p>

<ul>
  <li><code>text</code> MUST be of type <code>string</code>,</li>
  <li><code>needle</code> MUST be of type <code>string</code>,</li>
  <li><code>found</code> is of type <code>bool</code>,</li>
  <li><code>index</code> is of type <code>u32</code>.</li>
</ul>

<p>
Semantic rules:
</p>

<ul>
  <li>if <code>needle</code> is empty, <code>found</code> MUST be <code>true</code> and <code>index</code> MUST be <code>0</code>,</li>
  <li>if no occurrence exists, <code>found</code> MUST be <code>false</code> and <code>index</code> MUST be <code>0</code>.</li>
</ul>

<hr/>

<h2 id="segmentation-functions">11. Segmentation Functions</h2>

<h3>11.1 <code>frog.text.split</code></h3>

<p>
Splits a string using a separator substring.
</p>

<ul>
  <li>input ports: <code>text</code>, <code>separator</code></li>
  <li>output port: <code>parts</code></li>
</ul>

<p>
Rules:
</p>

<ul>
  <li><code>text</code> MUST be of type <code>string</code>,</li>
  <li><code>separator</code> MUST be of type <code>string</code>,</li>
  <li><code>parts</code> is of type <code>array&lt;string&gt;</code>.</li>
</ul>

<p>
Semantic rules:
</p>

<ul>
  <li>if <code>separator</code> is empty, the result MUST be a single-element array containing <code>text</code>,</li>
  <li>if no separator occurrence exists, the result MUST be a single-element array containing <code>text</code>,</li>
  <li>empty segments between adjacent separators MUST be preserved.</li>
</ul>

<h3>11.2 <code>frog.text.join</code></h3>

<p>
Joins an array of strings using a separator substring.
</p>

<ul>
  <li>input ports: <code>parts</code>, <code>separator</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>
Rules:
</p>

<ul>
  <li><code>parts</code> MUST be of type <code>array&lt;string&gt;</code>,</li>
  <li><code>separator</code> MUST be of type <code>string</code>,</li>
  <li><code>result</code> is of type <code>string</code>.</li>
</ul>

<p>
Semantic rules:
</p>

<ul>
  <li>joining an empty array MUST produce the empty string,</li>
  <li>joining a single-element array MUST return that element unchanged.</li>
</ul>

<hr/>

<h2 id="diagram-representation">12. Diagram Representation</h2>

<p>
Calls to <code>frog.text</code> primitives are serialized as <code>primitive</code> nodes in the diagram.
</p>

<p>
Example:
</p>

<pre><code>{
  "id": "concat_1",
  "kind": "primitive",
  "type": "frog.text.concat"
}</code></pre>

<p>
Another example:
</p>

<pre><code>{
  "id": "split_1",
  "kind": "primitive",
  "type": "frog.text.split"
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
  <li>every <code>frog.text</code> primitive reference MUST identify a valid standardized text primitive name,</li>
  <li>all required input ports for the referenced primitive MUST exist and be type-compatible,</li>
  <li>all produced output ports MUST match the primitive definition,</li>
  <li>all implicit coercions MUST follow <strong>Expression/Type.md</strong>,</li>
  <li>all primitives in this document MUST be treated as stateless and side-effect-free.</li>
</ul>

<p>
Additional v0.1 constraints:
</p>

<ul>
  <li><code>concat</code>, <code>contains</code>, <code>starts_with</code>, <code>ends_with</code>, and <code>find_first</code> MUST operate on <code>string</code> values,</li>
  <li><code>length</code> MUST return <code>u32</code>,</li>
  <li><code>substring</code> index parameters MUST be <code>u32</code>,</li>
  <li><code>split</code> MUST return <code>array&lt;string&gt;</code>,</li>
  <li><code>join</code> MUST accept <code>array&lt;string&gt;</code>.</li>
</ul>

<hr/>

<h2 id="examples">14. Examples</h2>

<h3>14.1 Concatenation</h3>

<pre><code>{
  "id": "concat_1",
  "kind": "primitive",
  "type": "frog.text.concat"
}</code></pre>

<p>
Conceptual ports:
</p>

<pre>
a, b → result
</pre>

<h3>14.2 Substring extraction</h3>

<pre><code>{
  "id": "substring_1",
  "kind": "primitive",
  "type": "frog.text.substring"
}</code></pre>

<p>
Conceptual ports:
</p>

<pre>
text, start, length → result
</pre>

<h3>14.3 Search</h3>

<pre><code>{
  "id": "find_1",
  "kind": "primitive",
  "type": "frog.text.find_first"
}</code></pre>

<p>
Conceptual ports:
</p>

<pre>
text, needle → found, index
</pre>

<h3>14.4 Split and join</h3>

<pre><code>"diagram": {
  "nodes": [
    { "id": "input_text", "kind": "interface_input", "interface_port": "text" },
    { "id": "split_1", "kind": "primitive", "type": "frog.text.split" },
    { "id": "join_1", "kind": "primitive", "type": "frog.text.join" },
    { "id": "output_text", "kind": "interface_output", "interface_port": "normalized" }
  ],
  "edges": [
    { "id": "e1", "from": { "node": "input_text", "port": "value" }, "to": { "node": "split_1", "port": "text" } },
    { "id": "e2", "from": { "node": "split_1", "port": "parts" }, "to": { "node": "join_1", "port": "parts" } },
    { "id": "e3", "from": { "node": "join_1", "port": "result" }, "to": { "node": "output_text", "port": "value" } }
  ]
}</code></pre>

<hr/>

<h2 id="out-of-scope">15. Out of Scope for v0.1</h2>

<p>
The following are outside the strict scope of <code>frog.text</code> in v0.1:
</p>

<ul>
  <li>locale-aware comparison and collation,</li>
  <li>Unicode normalization policy,</li>
  <li>case folding and locale-aware casing rules,</li>
  <li>regular expressions,</li>
  <li>advanced formatting templates,</li>
  <li>numeric parsing and formatting primitives,</li>
  <li>byte-oriented encoding conversion such as UTF-8/UTF-16 transcoding,</li>
  <li>path semantics, file semantics, and network text I/O,</li>
  <li>markup or schema-oriented text processing such as JSON, XML, or HTML libraries.</li>
</ul>

<hr/>

<h2 id="summary">16. Summary</h2>

<p>
The <code>frog.text</code> library defines the first standardized text-processing primitive set of FROG v0.1.
</p>

<p>
It provides:
</p>

<ul>
  <li>string concatenation,</li>
  <li>string length and substring extraction,</li>
  <li>substring predicates and search,</li>
  <li>basic split and join behavior.</li>
</ul>

<p>
This library is intentionally compact.
Its purpose is to provide a durable and reusable text foundation, on top of which richer parsing, formatting, localization, and serialization libraries may later be defined.
</p>

<hr/>

<p align="center">
End of FROG Text Library Specification
</p>
