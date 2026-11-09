<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 FROG State and Cycles Specification</h1>

<p align="center">
Source-facing representation of local memory and cycle-facing graph constraints in FROG programs<br/>
<em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#goals">2. Goals</a></li>
  <li><a href="#relation-with-other-specifications">3. Relation with Other Specifications</a></li>
  <li><a href="#local-memory">4. Local Memory</a></li>
  <li><a href="#stateless-vs-local-memory-primitives">5. Stateless vs Local-Memory Primitives</a></li>
  <li><a href="#state-ownership-and-scope">6. State Ownership and Scope</a></li>
  <li><a href="#cycles-in-dataflow-graphs">7. Cycles in Dataflow Graphs</a></li>
  <li><a href="#cycle-validity-in-source-form">8. Cycle Validity in Source Form</a></li>
  <li><a href="#frog-core-delay">9. <code>frog.core.delay</code></a></li>
  <li><a href="#initial-state">10. Initial State</a></li>
  <li><a href="#source-to-language-alignment">11. Source-to-Language Alignment</a></li>
  <li><a href="#validation-rules">12. Validation Rules</a></li>
  <li><a href="#examples">13. Examples</a></li>
  <li><a href="#out-of-scope-for-v01">14. Out of Scope for v0.1</a></li>
  <li><a href="#summary">15. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
FROG is a graphical dataflow programming language. In an acyclic graph, execution order can be derived directly from data
dependencies. However, many useful programs require feedback paths, accumulation, filtering, integration, delayed reuse of
values, or other forms of memory.
</p>

<p>
This document defines the source-facing representation of local memory in FROG and the source-level rules that govern how
cycle-facing constructs are represented in executable diagrams.
</p>

<p>
In FROG, directed feedback is only representable in canonical executable source when explicit local memory is present in the
graph. A primitive whose node instance stores a value between activations is called a local-memory primitive.
</p>

<p>
In informal terms, a local-memory primitive is a primitive with local state. The preferred normative wording in this
specification is <strong>local memory</strong>.
</p>

<p>
This document defines how explicit local memory and cyclic dataflow are represented in canonical source.
Normative execution semantics are defined by <code>Language/State and cycles.md</code>.
</p>

<hr/>

<h2 id="goals">2. Goals</h2>

<ul>
  <li><strong>Clarity</strong> — define how local memory is represented in canonical FROG source.</li>
  <li><strong>Source explicitness</strong> — require that feedback be represented through explicit local-memory constructs.</li>
  <li><strong>Deterministic initialization</strong> — require explicit and deterministic source-level initialization of local memory.</li>
  <li><strong>Separation of concerns</strong> — keep local memory distinct from global state, shared state, and runtime services.</li>
  <li><strong>Compatibility with dataflow</strong> — allow feedback while preserving an explicit and verifiable source model.</li>
</ul>

<hr/>

<h2 id="relation-with-other-specifications">3. Relation with Other Specifications</h2>

<p>
This document complements the following specifications:
</p>

<ul>
  <li><code>Expression/Readme.md</code> — defines the role of <code>Expression/</code> as the canonical source-specification layer.</li>
  <li><code>Language/Readme.md</code> — defines the role of <code>Language/</code> as the normative execution-semantics layer.</li>
  <li><code>Expression/Diagram.md</code> — defines the executable graph structure, node kinds, edges, and graph-level source representation.</li>
  <li><code>Expression/Type.md</code> — defines type compatibility for values carried through local-memory primitives.</li>
  <li><code>Expression/Control structures.md</code> — defines structure-owned nested executable regions, which remain subject to the same cycle-facing source constraints.</li>
  <li><code>Language/State and cycles.md</code> — defines the normative execution semantics of local memory and the language-level cycle-validity rule.</li>
  <li><code>Libraries/Core.md</code> — defines standard library primitives, including <code>frog.core.delay</code>.</li>
</ul>

<p>
This document defines the canonical source-level representation of local memory and cycle-facing graph constraints.
It does not redefine the serialized graph structure already specified by <code>Expression/Diagram.md</code>, it does not own
the normative execution semantics of local memory, and it does not own the standardized primitive identity or primitive-local
delayed-value behavior of <code>frog.core.delay</code>.
</p>

<table>
  <tr>
    <th align="left">Layer</th>
    <th align="left">Primary ownership for this topic</th>
  </tr>
  <tr>
    <td><code>Expression/</code></td>
    <td>Canonical source representation of local-memory constructs and cycle-facing source constraints</td>
  </tr>
  <tr>
    <td><code>Language/</code></td>
    <td>Normative execution meaning of local memory and the language-level cycle-validity rule</td>
  </tr>
  <tr>
    <td><code>Libraries/</code></td>
    <td>Standardized primitive identity, ports, required metadata, and primitive-local delayed-value behavior for <code>frog.core.delay</code></td>
  </tr>
  <tr>
    <td><code>IDE/</code></td>
    <td>Authoring, visualization, debugging, and inspection of feedback and stateful constructs</td>
  </tr>
</table>

<p>
Accordingly:
</p>

<ul>
  <li><code>Expression/State and cycles.md</code> owns the canonical source representation of local-memory constructs and cycle-facing source constraints,</li>
  <li><code>Language/State and cycles.md</code> owns the normative execution meaning of local memory and valid cycles,</li>
  <li><code>Libraries/Core.md</code> owns the standardized primitive identity, required ports, required metadata, and primitive-local delayed-value behavior of <code>frog.core.delay</code>.</li>
</ul>

<hr/>

<h2 id="local-memory">4. Local Memory</h2>

<p>
A local-memory primitive is a primitive whose node instance stores an internal value across activations of the same live
FROG instance.
</p>

<p>
In canonical source, local memory is represented explicitly through primitive instances whose standardized identity and required
configuration indicate local-memory behavior.
</p>

<p>
That local memory is:
</p>

<ul>
  <li>attached to the node instance,</li>
  <li>local to that primitive instance,</li>
  <li>not a global variable,</li>
  <li>not implicitly shared with other nodes,</li>
  <li>not a general-purpose shared reference mechanism.</li>
</ul>

<p>
This document defines how such local memory is represented and constrained in canonical source.
Normative observable behavior is defined by <code>Language/State and cycles.md</code>.
</p>

<hr/>

<h2 id="stateless-vs-local-memory-primitives">5. Stateless vs Local-Memory Primitives</h2>

<h3>5.1 Stateless primitive</h3>

<p>
A stateless primitive does not preserve any value between activations.
Its outputs depend only on its current inputs and its fixed configuration.
</p>

<p>Examples:</p>

<ul>
  <li><code>frog.core.add</code></li>
  <li><code>frog.core.mul</code></li>
  <li><code>frog.core.equal</code></li>
  <li><code>frog.math.sin</code></li>
</ul>

<h3>5.2 Local-memory primitive</h3>

<p>
A local-memory primitive preserves an internal value between activations.
Its outputs may depend on that preserved value in addition to its current inputs.
</p>

<p>Examples:</p>

<ul>
  <li><code>frog.core.delay</code></li>
  <li>future register-style primitives</li>
  <li>future integrator-style primitives</li>
  <li>future stateful signal-processing primitives</li>
</ul>

<hr/>

<h2 id="state-ownership-and-scope">6. State Ownership and Scope</h2>

<p>
In canonical source, local memory is represented as belonging to a specific node instance inside a specific live FROG instance.
</p>

<p>Therefore, the source model MUST remain compatible with the following constraints:</p>

<ul>
  <li>two different local-memory nodes do not share state implicitly,</li>
  <li>two different sub-FROG instances do not share local memory implicitly,</li>
  <li>duplicating a node duplicates its local-memory representation,</li>
  <li>instantiating the same sub-FROG multiple times yields distinct local-memory instances for each instantiated graph.</li>
</ul>

<p>
The exact runtime lifetime of a live FROG instance may depend on the active runtime profile.
This document only constrains the canonical source model so that local memory remains explicit, instance-scoped, and non-shared by default.
</p>

<hr/>

<h2 id="cycles-in-dataflow-graphs">7. Cycles in Dataflow Graphs</h2>

<p>
A directed cycle exists when a path of directed edges leads from a node back to itself.
</p>

<p>Examples:</p>

<pre><code>A → B → C → A</code></pre>

<p>or:</p>

<pre><code>A → A</code></pre>

<p>
In a pure dataflow graph, a cycle with no memory creates an unresolved dependency loop.
Such a graph does not define valid executable behavior by itself.
</p>

<p>
Therefore, FROG distinguishes between:
</p>

<ul>
  <li><strong>invalid combinational cycles</strong> — cycles containing no local memory,</li>
  <li><strong>stateful feedback cycles</strong> — cycles containing at least one explicit local-memory primitive.</li>
</ul>

<p>
This distinction applies both to the top-level diagram and to region-local diagrams owned by control structures.
</p>

<hr/>

<h2 id="cycle-validity-in-source-form">8. Cycle Validity in Source Form</h2>

<p>
The base rule of FROG v0.1 is reflected in source form as follows:
</p>

<pre><code>A diagram MAY contain directed cycles.

Every directed cycle MUST contain at least one local-memory primitive.</code></pre>

<p>
Equivalent source-facing interpretation:
</p>

<ul>
  <li>a cycle without local memory is invalid in source validation,</li>
  <li>a cycle containing at least one local-memory primitive is representable as valid in principle, subject to ordinary graph and type validation.</li>
</ul>

<p>
This rule applies to:
</p>

<ul>
  <li>multi-node cycles,</li>
  <li>self-feedback cycles,</li>
  <li>feedback paths through nested diagram regions.</li>
</ul>

<p>
A validator MAY enforce this rule using strongly connected component analysis or any equivalent graph-theoretic method.
</p>

<p>
The rule itself is normative at language level and is owned by <code>Language/State and cycles.md</code>.
This document defines how that rule is reflected in canonical source and in source-level validation.
</p>

<p>
In source terms, a cycle is not made valid by layout, by documentation, or by informal intent.
It is only made valid when the canonical graph representation contains an explicit local-memory primitive in every directed cycle.
</p>

<hr/>

<h2 id="frog-core-delay">9. <code>frog.core.delay</code></h2>

<p>
The minimal standard local-memory primitive for FROG v0.1 is:
</p>

<pre><code>frog.core.delay</code></pre>

<p>
Its source-facing role is to make delayed feedback explicit in canonical graph form.
</p>

<h3>9.1 Standardized identity and required surface</h3>

<p>
A canonical source node using <code>frog.core.delay</code> MUST remain compatible with the primitive definition in
<code>Libraries/Core.md</code>.
</p>

<p>The required standardized surface for base v0.1 includes:</p>

<ul>
  <li>input port: <code>in</code></li>
  <li>output port: <code>out</code></li>
  <li>required metadata: <code>initial</code></li>
</ul>

<h3>9.2 Canonical node shape</h3>

<pre><code class="language-json">{
  "id": "delay_1",
  "kind": "primitive",
  "type": "frog.core.delay",
  "initial": 0.0
}</code></pre>

<p>
This primitive is sufficient to make a feedback path explicit and deterministic in canonical source.
Its canonical node representation is described here, its standardized primitive identity belongs to
<code>Libraries/Core.md</code>, and its normative observable semantics belong to <code>Language/State and cycles.md</code>.
</p>

<hr/>

<h2 id="initial-state">10. Initial State</h2>

<p>
Every local-memory primitive MUST have a deterministic initial state.
</p>

<p>
For <code>frog.core.delay</code> in v0.1, the <code>initial</code> field is mandatory.
</p>

<p>Illustrative node shape:</p>

<pre><code class="language-json">{
  "id": "delay_1",
  "kind": "primitive",
  "type": "frog.core.delay",
  "initial": 0.0
}</code></pre>

<p>Rules:</p>

<ul>
  <li><code>initial</code> MUST be present for every <code>frog.core.delay</code> node in v0.1,</li>
  <li>the value of <code>initial</code> MUST be compatible with the value type carried by the node,</li>
  <li>the initial state MUST be fully defined by source or by a stricter profile that preserves deterministic equivalence.</li>
</ul>

<p>
FROG v0.1 does not permit undefined initial state for <code>frog.core.delay</code>.
</p>

<hr/>

<h2 id="source-to-language-alignment">11. Source-to-Language Alignment</h2>

<p>
Canonical source representation and normative execution semantics are intentionally separated in FROG.
This section defines the source-level alignment points between the forms declared here and the execution-semantics layer owned by <code>Language/</code>.
</p>

<h3>11.1 Local-memory primitive alignment</h3>

<ul>
  <li>a node with <code>kind: "primitive"</code> and <code>type: "frog.core.delay"</code> aligns with the minimal standardized local-memory primitive,</li>
  <li>its required source field <code>initial</code> provides the source-level initialization data associated with that primitive,</li>
  <li>its incoming and outgoing edges make delayed feedback explicit in the graph.</li>
</ul>

<h3>11.2 Cycle alignment</h3>

<ul>
  <li>a directed cycle identified in canonical source aligns with a language-level feedback construct,</li>
  <li>the presence of at least one explicit local-memory primitive in that directed cycle provides the required source evidence of explicit memory,</li>
  <li>the absence of such a primitive leaves the cycle invalid at source-validation level.</li>
</ul>

<h3>11.3 Region-local alignment</h3>

<ul>
  <li>the same source-level cycle analysis applies inside structure-owned regions,</li>
  <li>a loop structure does not by itself constitute local memory in the graph,</li>
  <li>explicit local-memory primitives remain the canonical source mechanism for representing memory in directed feedback paths.</li>
</ul>

<h3>11.4 No semantic redefinition in this document</h3>

<p>
This document MUST NOT be interpreted as redefining:
</p>

<ul>
  <li>runtime scheduling strategy,</li>
  <li>the precise runtime activation model of local-memory primitives,</li>
  <li>the language-level ownership of cycle validity,</li>
  <li>the general observable semantics of delayed state across activations,</li>
  <li>the primitive-local delayed-value behavior owned by <code>Libraries/Core.md</code>.</li>
</ul>

<p>
Those topics are owned normatively by <code>Language/State and cycles.md</code> and, for primitive-local <code>frog.core.delay</code> behavior, by <code>Libraries/Core.md</code>.
</p>

<hr/>

<h2 id="validation-rules">12. Validation Rules</h2>

<p>
Implementations MUST enforce the following source-level validation rules:
</p>

<ul>
  <li>every directed cycle MUST contain at least one explicit local-memory primitive,</li>
  <li>all value connections through local-memory primitives MUST remain type-compatible according to <code>Expression/Type.md</code>,</li>
  <li>local memory MUST be represented as scoped to the node instance,</li>
  <li>local memory MUST NOT be represented as implicitly shared across unrelated nodes or sub-FROG instances,</li>
  <li>the initial state of every local-memory primitive MUST be represented deterministically.</li>
</ul>

<p>For <code>frog.core.delay</code> specifically:</p>

<ul>
  <li>the node MUST define an <code>initial</code> field,</li>
  <li><code>in</code> and <code>out</code> MUST have the same value type,</li>
  <li><code>initial</code> MUST be compatible with that type.</li>
</ul>

<p>
Tools SHOULD additionally warn when:
</p>

<ul>
  <li>a cycle contains more local-memory primitives than necessary and becomes difficult to understand,</li>
  <li>feedback is valid but visually unclear,</li>
  <li>multiple feedback paths make execution hard to inspect even though validation succeeds.</li>
</ul>

<hr/>

<h2 id="examples">13. Examples</h2>

<h3>13.1 Invalid cycle without local memory</h3>

<pre><code class="language-json">{
  "diagram": {
    "nodes": [
      { "id": "a", "kind": "primitive", "type": "frog.core.add" },
      { "id": "b", "kind": "primitive", "type": "frog.core.mul" }
    ],
    "edges": [
      { "id": "e1", "from": { "node": "a", "port": "result" }, "to": { "node": "b", "port": "a" } },
      { "id": "e2", "from": { "node": "b", "port": "result" }, "to": { "node": "a", "port": "a" } }
    ]
  }
}</code></pre>

<p>
This graph is invalid because the cycle contains no local-memory primitive.
</p>

<h3>13.2 Valid cycle with <code>frog.core.delay</code></h3>

<pre><code class="language-json">{
  "diagram": {
    "nodes": [
      { "id": "input_x", "kind": "interface_input", "interface_port": "x" },
      { "id": "add_1", "kind": "primitive", "type": "frog.core.add" },
      { "id": "delay_1", "kind": "primitive", "type": "frog.core.delay", "initial": 0.0 },
      { "id": "output_y", "kind": "interface_output", "interface_port": "y" }
    ],
    "edges": [
      { "id": "e1", "from": { "node": "input_x", "port": "value" }, "to": { "node": "add_1", "port": "a" } },
      { "id": "e2", "from": { "node": "delay_1", "port": "out" }, "to": { "node": "add_1", "port": "b" } },
      { "id": "e3", "from": { "node": "add_1", "port": "result" }, "to": { "node": "delay_1", "port": "in" } },
      { "id": "e4", "from": { "node": "add_1", "port": "result" }, "to": { "node": "output_y", "port": "value" } }
    ]
  }
}</code></pre>

<p>
This graph is valid in principle because the feedback path contains an explicit local-memory primitive.
</p>

<h3>13.3 Self-feedback with explicit memory</h3>

<pre><code class="language-json">{
  "diagram": {
    "nodes": [
      { "id": "delay_1", "kind": "primitive", "type": "frog.core.delay", "initial": 1 },
      { "id": "neg_1", "kind": "primitive", "type": "frog.core.neg" }
    ],
    "edges": [
      { "id": "e1", "from": { "node": "delay_1", "port": "out" }, "to": { "node": "neg_1", "port": "in" } },
      { "id": "e2", "from": { "node": "neg_1", "port": "result" }, "to": { "node": "delay_1", "port": "in" } }
    ]
  }
}</code></pre>

<p>
This is a valid feedback cycle in principle because the self-referential path remains explicit and includes local memory.
</p>

<h3>13.4 Region-local feedback inside a control structure</h3>

<pre><code class="language-json">{
  "id": "loop_1",
  "kind": "structure",
  "structure_type": "while_loop",
  "boundary": {
    "inputs": [],
    "outputs": []
  },
  "structure_terminals": {
    "condition": {
      "type": "bool",
      "outer_visible": false,
      "exposed_in_body": true,
      "read_only": false,
      "role": "continue_while_true"
    },
    "index": {
      "type": "i64",
      "outer_visible": false,
      "exposed_in_body": true,
      "read_only": true,
      "role": "index"
    }
  },
  "regions": [
    {
      "id": "body",
      "diagram": {
        "nodes": [
          { "id": "delay_1", "kind": "primitive", "type": "frog.core.delay", "initial": 0 },
          { "id": "add_1", "kind": "primitive", "type": "frog.core.add" }
        ],
        "edges": [
          { "id": "e1", "from": { "node": "delay_1", "port": "out" }, "to": { "node": "add_1", "port": "a" } },
          { "id": "e2", "from": { "node": "add_1", "port": "result" }, "to": { "node": "delay_1", "port": "in" } }
        ]
      }
    }
  ]
}</code></pre>

<p>
This example illustrates that the same explicit-local-memory rule applies inside a region-local diagram.
The presence of the loop structure does not replace the need for explicit local memory in the feedback path.
</p>

<hr/>

<h2 id="out-of-scope-for-v01">14. Out of Scope for v0.1</h2>

<ul>
  <li>implicit feedback state,</li>
  <li>automatic insertion of hidden memory to repair invalid cycles,</li>
  <li>shared mutable state between unrelated nodes,</li>
  <li>distributed state semantics,</li>
  <li>transactional state models,</li>
  <li>event-specific memory semantics,</li>
  <li>alternative standardized local-memory primitives beyond the minimal base definition.</li>
</ul>

<hr/>

<h2 id="summary">15. Summary</h2>

<p>
FROG allows feedback in executable diagrams, but only when memory is made explicit.
This document defines how that explicit memory and the resulting cycle-facing graph forms are represented in canonical source.
</p>

<ul>
  <li>a directed cycle is representable in canonical source,</li>
  <li>it is only valid in principle when at least one explicit local-memory primitive appears in that directed cycle,</li>
  <li><code>frog.core.delay</code> is the minimal standard local-memory primitive for v0.1,</li>
  <li><code>initial</code> is mandatory for <code>frog.core.delay</code> in v0.1,</li>
  <li>local memory is scoped to the primitive instance and is not implicitly shared,</li>
  <li>normative execution semantics remain owned by <code>Language/State and cycles.md</code>.</li>
</ul>

<p>
This gives FROG a clear, explicit, and verifiable source foundation for local memory and cycle-capable dataflow.
</p>
