<h1 align="center">🐸 FROG Collections Library Specification</h1>

<p align="center">
Definition of the standard <strong>frog.collections</strong> library for FROG v0.1<br/>
<em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#goals">2. Goals</a></li>
  <li><a href="#relation-with-other-specifications">3. Relation with Other Specifications</a></li>
  <li><a href="#role-of-frog-collections">4. Role of <code>frog.collections</code></a></li>
  <li><a href="#naming-and-namespace">5. Naming and Namespace</a></li>
  <li><a href="#scope-for-v01">6. Scope for v0.1</a></li>
  <li><a href="#collection-model-for-v01">7. Collection Model for v0.1</a></li>
  <li><a href="#typing-model">8. Typing Model</a></li>
  <li><a href="#structural-query-functions">9. Structural Query Functions</a></li>
  <li><a href="#element-access-and-update-functions">10. Element Access and Update Functions</a></li>
  <li><a href="#array-growth-and-combination-functions">11. Array Growth and Combination Functions</a></li>
  <li><a href="#subarray-functions">12. Subarray Functions</a></li>
  <li><a href="#diagram-representation">13. Diagram Representation</a></li>
  <li><a href="#validation-rules">14. Validation Rules</a></li>
  <li><a href="#examples">15. Examples</a></li>
  <li><a href="#out-of-scope">16. Out of Scope for v0.1</a></li>
  <li><a href="#summary">17. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the standard <strong>frog.collections</strong> library for FROG v0.1.
</p>

<p>
The <code>frog.collections</code> library provides a first standardized set of collection-oriented primitives for executable FROG diagrams.
</p>

<p>
For FROG v0.1, this library is intentionally limited to <strong>array-oriented primitives</strong>.
It standardizes a compact but useful set of operations for:
</p>

<ul>
  <li>array size inspection,</li>
  <li>emptiness checks,</li>
  <li>element access,</li>
  <li>element replacement,</li>
  <li>array growth,</li>
  <li>array concatenation,</li>
  <li>subarray extraction.</li>
</ul>

<p>
This library extends the practical expressiveness of FROG without expanding the minimal mandatory kernel defined by <code>frog.core</code>.
</p>

<hr/>

<h2 id="goals">2. Goals</h2>

<ul>
  <li><strong>Practicality</strong> — provide a useful first standard collection library for everyday graphical programming.</li>
  <li><strong>Minimality</strong> — keep the v0.1 surface small enough to remain durable and easy to implement.</li>
  <li><strong>Clarity</strong> — assign every primitive a clear, stable role and a stable port model.</li>
  <li><strong>Portability</strong> — ensure collection primitives have portable source-level meaning across conforming implementations.</li>
  <li><strong>Extensibility</strong> — leave room for future collection families beyond arrays without forcing them into v0.1.</li>
</ul>

<hr/>

<h2 id="relation-with-other-specifications">3. Relation with Other Specifications</h2>

<p>
This document complements the following specifications:
</p>

<ul>
  <li><strong>Expression/Diagram.md</strong> — defines how library primitives are serialized as executable diagram nodes.</li>
  <li><strong>Expression/Type.md</strong> — defines built-in types, including array types, and the compatibility and coercion rules that apply to primitive ports.</li>
  <li><strong>Expression/Control structures.md</strong> — defines standardized language structures, which remain distinct from ordinary primitive functions.</li>
  <li><strong>Libraries/Core.md</strong> — defines the minimal built-in primitive kernel on top of which higher-level library families may be added.</li>
</ul>

<p>
This document defines the standardized collection primitive catalog.
It does not redefine the graph model, the type system, or the standardized control-structure model.
</p>

<hr/>

<h2 id="role-of-frog-collections">4. Role of <code>frog.collections</code></h2>

<p>
The <code>frog.collections</code> library provides standardized collection-oriented executable primitives for FROG diagrams.
</p>

<p>
In language terms, these are library-level primitive functions.
In the serialized diagram representation defined by <code>Expression/Diagram.md</code>, calls to these functions are represented as nodes of kind <code>primitive</code>.
</p>

<p>
For example:
</p>

<ul>
  <li><code>frog.collections.length</code> is a standardized collection primitive,</li>
  <li>in a diagram, that function call appears as a <code>primitive</code> node with <code>type = "frog.collections.length"</code>.</li>
</ul>

<p>
This library is a standard sibling of <code>frog.core</code>, not an expansion of it.
</p>

<hr/>

<h2 id="naming-and-namespace">5. Naming and Namespace</h2>

<p>
FROG uses the following general namespace pattern for standardized primitives:
</p>

<pre>
frog.&lt;library&gt;.&lt;primitive&gt;
</pre>

<p>
For this document:
</p>

<ul>
  <li><code>frog</code> identifies the language namespace,</li>
  <li><code>collections</code> identifies the standard collections library,</li>
  <li>the final segment identifies the primitive name.</li>
</ul>

<p>
Examples:
</p>

<pre>
frog.collections.length
frog.collections.get
frog.collections.set
frog.collections.append
frog.collections.concat
frog.collections.slice
</pre>

<p>
Primitive names in <code>frog.collections</code> SHOULD use lowercase snake_case where multiple words are needed.
</p>

<hr/>

<h2 id="scope-for-v01">6. Scope for v0.1</h2>

<p>
FROG v0.1 standardizes the following initial collection primitives:
</p>

<ul>
  <li>structural query functions,</li>
  <li>element access and update functions,</li>
  <li>array growth and combination functions,</li>
  <li>subarray extraction.</li>
</ul>

<p>
In v0.1, these functions apply only to standardized FROG array types.
</p>

<p>
This document does <strong>not</strong> yet standardize:
</p>

<ul>
  <li>maps,</li>
  <li>sets,</li>
  <li>dictionaries,</li>
  <li>heterogeneous tuples or records,</li>
  <li>higher-order collection functions such as <code>map</code>, <code>filter</code>, or <code>reduce</code>,</li>
  <li>sorting, searching, or advanced reshaping catalogs.</li>
</ul>

<hr/>

<h2 id="collection-model-for-v01">7. Collection Model for v0.1</h2>

<p>
For FROG v0.1, the only standardized collection family is the built-in array family defined by <strong>Expression/Type.md</strong>.
</p>

<p>
Accordingly:
</p>

<ul>
  <li><code>array&lt;T&gt;</code> is a valid collection type with dynamic size,</li>
  <li><code>array&lt;T, N&gt;</code> is a valid collection type with fixed size,</li>
  <li>nested arrays are valid when the element type is itself an array type.</li>
</ul>

<p>
All collection primitives in this document operate on homogeneous arrays.
</p>

<p>
When a primitive in this document returns an array whose size depends on runtime values rather than a compile-time fixed type expression, the output type is the dynamic array form <code>array&lt;T&gt;</code>.
</p>

<hr/>

<h2 id="typing-model">8. Typing Model</h2>

<p>
All <code>frog.collections</code> primitives are typed according to <strong>Expression/Type.md</strong>.
</p>

<p>
Unless stated otherwise:
</p>

<ul>
  <li>array inputs MUST have valid FROG array types,</li>
  <li>element ports MUST be type-compatible with the array element type,</li>
  <li>index and length ports in this document MUST use type <code>u64</code>,</li>
  <li>all implicit coercions MUST follow the standard FROG coercion rules,</li>
  <li>all functions in this document are stateless and side-effect-free.</li>
</ul>

<p>
For primitives that require an index to be in range, the index validity condition is part of the primitive contract.
If an implementation can prove a violation statically, it MUST reject the graph as invalid.
If the violation is only detectable at execution time, the active execution profile MUST define the corresponding runtime failure behavior.
</p>

<hr/>

<h2 id="structural-query-functions">9. Structural Query Functions</h2>

<h3>9.1 <code>frog.collections.length</code></h3>

<p>
Returns the number of elements in an array.
</p>

<ul>
  <li>input port: <code>array</code></li>
  <li>output port: <code>length</code></li>
</ul>

<p>
Rules:
</p>

<ul>
  <li>the input MUST have type <code>array&lt;T&gt;</code> or <code>array&lt;T, N&gt;</code>,</li>
  <li>the output type is <code>u64</code>.</li>
</ul>

<p>
For a fixed-size input array, the returned value is the fixed declared size.
For a dynamic array, the returned value is the runtime element count.
</p>

<h3>9.2 <code>frog.collections.is_empty</code></h3>

<p>
Returns whether an array contains zero elements.
</p>

<ul>
  <li>input port: <code>array</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>
Rules:
</p>

<ul>
  <li>the input MUST have type <code>array&lt;T&gt;</code> or <code>array&lt;T, N&gt;</code>,</li>
  <li>the output type is <code>bool</code>.</li>
</ul>

<hr/>

<h2 id="element-access-and-update-functions">10. Element Access and Update Functions</h2>

<h3>10.1 <code>frog.collections.get</code></h3>

<p>
Returns the element stored at a given index.
</p>

<ul>
  <li>input ports: <code>array</code>, <code>index</code></li>
  <li>output port: <code>value</code></li>
</ul>

<p>
Rules:
</p>

<ul>
  <li><code>array</code> MUST have type <code>array&lt;T&gt;</code> or <code>array&lt;T, N&gt;</code>,</li>
  <li><code>index</code> MUST have type <code>u64</code>,</li>
  <li><code>value</code> has type <code>T</code>,</li>
  <li>the index MUST satisfy <code>index &lt; length(array)</code>.</li>
</ul>

<h3>10.2 <code>frog.collections.set</code></h3>

<p>
Returns an array equal to the input array except at one index, where the value is replaced.
</p>

<ul>
  <li>input ports: <code>array</code>, <code>index</code>, <code>value</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>
Rules:
</p>

<ul>
  <li><code>array</code> MUST have type <code>array&lt;T&gt;</code> or <code>array&lt;T, N&gt;</code>,</li>
  <li><code>index</code> MUST have type <code>u64</code>,</li>
  <li><code>value</code> MUST be type-compatible with <code>T</code>,</li>
  <li>the index MUST satisfy <code>index &lt; length(array)</code>.</li>
</ul>

<p>
Output typing:
</p>

<ul>
  <li>if the input type is <code>array&lt;T&gt;</code>, the output type is <code>array&lt;T&gt;</code>,</li>
  <li>if the input type is <code>array&lt;T, N&gt;</code>, the output type is <code>array&lt;T, N&gt;</code>.</li>
</ul>

<hr/>

<h2 id="array-growth-and-combination-functions">11. Array Growth and Combination Functions</h2>

<h3>11.1 <code>frog.collections.append</code></h3>

<p>
Returns an array formed by appending one element to the end of the input array.
</p>

<ul>
  <li>input ports: <code>array</code>, <code>value</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>
Rules:
</p>

<ul>
  <li><code>array</code> MUST have type <code>array&lt;T&gt;</code> or <code>array&lt;T, N&gt;</code>,</li>
  <li><code>value</code> MUST be type-compatible with <code>T</code>.</li>
</ul>

<p>
Output typing:
</p>

<ul>
  <li>the output type is <code>array&lt;T&gt;</code>.</li>
</ul>

<p>
This output is dynamic in v0.1 even if the input array has fixed size, because the result length differs from the input length.
</p>

<h3>11.2 <code>frog.collections.concat</code></h3>

<p>
Returns an array formed by concatenating two arrays of compatible element type.
</p>

<ul>
  <li>input ports: <code>a</code>, <code>b</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>
Rules:
</p>

<ul>
  <li><code>a</code> MUST have type <code>array&lt;T1&gt;</code> or <code>array&lt;T1, N1&gt;</code>,</li>
  <li><code>b</code> MUST have type <code>array&lt;T2&gt;</code> or <code>array&lt;T2, N2&gt;</code>,</li>
  <li><code>T1</code> and <code>T2</code> MUST be identical or implicitly coercible to a common element type <code>T</code>.</li>
</ul>

<p>
Output typing:
</p>

<ul>
  <li>the output type is <code>array&lt;T&gt;</code>.</li>
</ul>

<hr/>

<h2 id="subarray-functions">12. Subarray Functions</h2>

<h3>12.1 <code>frog.collections.slice</code></h3>

<p>
Returns a contiguous subarray starting at a given index and spanning a given number of elements.
</p>

<ul>
  <li>input ports: <code>array</code>, <code>start</code>, <code>length</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>
Rules:
</p>

<ul>
  <li><code>array</code> MUST have type <code>array&lt;T&gt;</code> or <code>array&lt;T, N&gt;</code>,</li>
  <li><code>start</code> MUST have type <code>u64</code>,</li>
  <li><code>length</code> MUST have type <code>u64</code>,</li>
  <li>the requested slice MUST satisfy <code>start + length &lt;= length(array)</code>.</li>
</ul>

<p>
Output typing:
</p>

<ul>
  <li>the output type is <code>array&lt;T&gt;</code>.</li>
</ul>

<p>
The result type is dynamic in v0.1 because the slice length is provided as a runtime value.
</p>

<hr/>

<h2 id="diagram-representation">13. Diagram Representation</h2>

<p>
Calls to <code>frog.collections</code> primitives are serialized as <code>primitive</code> nodes in the diagram.
</p>

<p>
Example:
</p>

<pre><code>{
  "id": "len_1",
  "kind": "primitive",
  "type": "frog.collections.length"
}</code></pre>

<p>
Another example:
</p>

<pre><code>{
  "id": "get_1",
  "kind": "primitive",
  "type": "frog.collections.get"
}</code></pre>

<p>
The exact port existence, direction, and typing of these nodes are resolved from this specification together with the type system and the graph rules.
</p>

<hr/>

<h2 id="validation-rules">14. Validation Rules</h2>

<p>
Implementations MUST enforce the following rules:
</p>

<ul>
  <li>every <code>frog.collections</code> primitive reference MUST identify a valid standardized primitive name defined in this document,</li>
  <li>all required input ports for the referenced primitive MUST exist,</li>
  <li>all produced output ports MUST match the primitive definition,</li>
  <li>all array inputs MUST use valid FROG array types,</li>
  <li>all implicit coercions MUST follow <strong>Expression/Type.md</strong>.</li>
</ul>

<p>
For <code>frog.collections.get</code> and <code>frog.collections.set</code> specifically:
</p>

<ul>
  <li>the <code>index</code> input MUST have type <code>u64</code>,</li>
  <li>the referenced element position MUST be in range.</li>
</ul>

<p>
For <code>frog.collections.slice</code> specifically:
</p>

<ul>
  <li><code>start</code> and <code>length</code> MUST both have type <code>u64</code>,</li>
  <li>the requested slice interval MUST be valid for the input array.</li>
</ul>

<p>
For <code>frog.collections.append</code>, <code>frog.collections.concat</code>, and <code>frog.collections.set</code>:
</p>

<ul>
  <li>value inputs MUST be type-compatible with the resolved array element type.</li>
</ul>

<hr/>

<h2 id="examples">15. Examples</h2>

<h3>15.1 Array length</h3>

<pre><code>{
  "id": "len_1",
  "kind": "primitive",
  "type": "frog.collections.length"
}</code></pre>

<p>
Conceptual ports:
</p>

<pre>
array → length
</pre>

<h3>15.2 Element access</h3>

<pre><code>{
  "id": "get_1",
  "kind": "primitive",
  "type": "frog.collections.get"
}</code></pre>

<p>
Conceptual ports:
</p>

<pre>
array, index → value
</pre>

<h3>15.3 Element replacement</h3>

<pre><code>{
  "id": "set_1",
  "kind": "primitive",
  "type": "frog.collections.set"
}</code></pre>

<p>
Conceptual ports:
</p>

<pre>
array, index, value → result
</pre>

<h3>15.4 Array append</h3>

<pre><code>{
  "id": "append_1",
  "kind": "primitive",
  "type": "frog.collections.append"
}</code></pre>

<p>
Conceptual ports:
</p>

<pre>
array, value → result
</pre>

<h3>15.5 Array slice</h3>

<pre><code>{
  "id": "slice_1",
  "kind": "primitive",
  "type": "frog.collections.slice"
}</code></pre>

<p>
Conceptual ports:
</p>

<pre>
array, start, length → result
</pre>

<h3>15.6 Concatenate two arrays</h3>

<pre><code>{
  "diagram": {
    "nodes": [
      { "id": "input_a", "kind": "interface_input", "interface_port": "a" },
      { "id": "input_b", "kind": "interface_input", "interface_port": "b" },
      { "id": "concat_1", "kind": "primitive", "type": "frog.collections.concat" },
      { "id": "output_result", "kind": "interface_output", "interface_port": "result" }
    ],
    "edges": [
      { "id": "e1", "from": { "node": "input_a", "port": "value" }, "to": { "node": "concat_1", "port": "a" } },
      { "id": "e2", "from": { "node": "input_b", "port": "value" }, "to": { "node": "concat_1", "port": "b" } },
      { "id": "e3", "from": { "node": "concat_1", "port": "result" }, "to": { "node": "output_result", "port": "value" } }
    ]
  }
}</code></pre>

<hr/>

<h2 id="out-of-scope">16. Out of Scope for v0.1</h2>

<p>
The following are outside the strict scope of <code>frog.collections</code> in v0.1:
</p>

<ul>
  <li>maps, sets, dictionaries, and key-value collection families,</li>
  <li>heterogeneous tuple, record, or cluster collection primitives,</li>
  <li>sorting and searching catalogs,</li>
  <li>insert-at and remove-at catalogs,</li>
  <li>reshape, transpose, or multidimensional array algebra catalogs beyond what is expressible through ordinary nested arrays,</li>
  <li>higher-order collection primitives such as <code>map</code>, <code>filter</code>, <code>reduce</code>, or <code>fold</code>,</li>
  <li>streaming and lazy collection models,</li>
  <li>text-oriented collection semantics, which belong in a dedicated text library.</li>
</ul>

<hr/>

<h2 id="summary">17. Summary</h2>

<p>
The <code>frog.collections</code> library defines the first standardized collection-oriented primitive set for FROG v0.1.
</p>

<p>
It provides:
</p>

<ul>
  <li>array size inspection,</li>
  <li>emptiness checks,</li>
  <li>element access,</li>
  <li>element replacement,</li>
  <li>array append,</li>
  <li>array concatenation,</li>
  <li>subarray extraction.</li>
</ul>

<p>
This library is intentionally compact.
Its purpose is to provide a durable and practical first collection vocabulary aligned with the current FROG v0.1 type system while leaving room for richer collection families in later revisions.
</p>

<hr/>

<p align="center">
End of FROG Collections Library Specification
</p>
