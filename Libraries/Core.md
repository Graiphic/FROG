<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 FROG Core Library Specification</h1>

<p align="center">
  Definition of the minimal intrinsic <code>frog.core</code> primitive library for FROG v0.1<br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#goals">2. Goals</a></li>
  <li><a href="#architectural-position">3. Architectural Position</a></li>
  <li><a href="#relation-with-other-specifications">4. Relation with Other Specifications</a></li>
  <li><a href="#role-of-frogcore">5. Role of <code>frog.core</code></a></li>
  <li><a href="#naming-and-namespace">6. Naming and Namespace</a></li>
  <li><a href="#scope-for-v01">7. Scope for v0.1</a></li>
  <li><a href="#library-categories">8. Library Categories</a></li>
  <li><a href="#typing-model">9. Typing Model</a></li>
  <li><a href="#arithmetic-primitives">10. Arithmetic Primitives</a></li>
  <li><a href="#comparison-primitives">11. Comparison Primitives</a></li>
  <li><a href="#logical-primitives">12. Logical Primitives</a></li>
  <li><a href="#selection-primitive">13. Selection Primitive</a></li>
  <li><a href="#local-memory-primitive">14. Local-Memory Primitive</a></li>
  <li><a href="#diagram-representation">15. Diagram Representation</a></li>
  <li><a href="#validation-rules">16. Validation Rules</a></li>
  <li><a href="#execution-facing-posture">17. Execution-Facing Posture</a></li>
  <li><a href="#examples">18. Examples</a></li>
  <li><a href="#out-of-scope-for-v01">19. Out of Scope for v0.1</a></li>
  <li><a href="#summary">20. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the minimal intrinsic <code>frog.core</code> primitive library for FROG v0.1.
</p>

<p>
The <code>frog.core</code> library contains the smallest standardized primitive set required to express:
</p>

<ul>
  <li>basic arithmetic,</li>
  <li>basic comparison,</li>
  <li>basic boolean logic,</li>
  <li>simple dataflow selection,</li>
  <li>explicit local memory through delayed feedback.</li>
</ul>

<p>
These primitives form the foundational intrinsic executable vocabulary of the language.
They are intended to be stable, portable, and always available in conforming core FROG implementations.
</p>

<p>
This document standardizes:
</p>

<ul>
  <li>primitive identity,</li>
  <li>primitive-local port surface,</li>
  <li>primitive-local metadata requirements,</li>
  <li>primitive-local behavior.</li>
</ul>

<p>
This document does not redefine:
</p>

<ul>
  <li>canonical source structure,</li>
  <li>cross-cutting language semantics,</li>
  <li>Execution IR structure,</li>
  <li>lowering policy,</li>
  <li>backend contract shape,</li>
  <li>runtime-private realization.</li>
</ul>

<hr/>

<h2 id="goals">2. Goals</h2>

<ul>
  <li><strong>Minimality</strong> — define a small but useful intrinsic core.</li>
  <li><strong>Stability</strong> — keep the first standardized primitive surface compact and durable.</li>
  <li><strong>Clarity</strong> — give every primitive a clear role, stable name, and stable port model.</li>
  <li><strong>Portability</strong> — ensure these primitives are expected to exist in every conforming core implementation.</li>
  <li><strong>Composability</strong> — provide a foundation on which broader intrinsic libraries and optional profiles can build.</li>
  <li><strong>Architectural discipline</strong> — keep primitive-local contracts here while preserving surrounding ownership boundaries.</li>
</ul>

<hr/>

<h2 id="architectural-position">3. Architectural Position</h2>

<p>
The position of <code>frog.core</code> in the repository architecture is intentionally narrow and explicit:
</p>

<pre>
Expression/   -&gt; how primitive nodes are written in canonical source
Language/     -&gt; cross-cutting validated-program meaning
Libraries/    -&gt; intrinsic primitive identities and primitive-local contracts
Profiles/     -&gt; optional standardized capability families
IR/           -&gt; open execution-facing representation derived from validated meaning
IDE/          -&gt; authoring, observability, debugging, inspection
</pre>

<p>
Accordingly:
</p>

<ul>
  <li><code>Libraries/Core.md</code> owns the intrinsic identities and primitive-local contracts of <code>frog.core.*</code>,</li>
  <li><code>Expression/</code> owns how those primitives appear in canonical source,</li>
  <li><code>Language/</code> owns cross-cutting rules that remain larger than one primitive,</li>
  <li><code>IR/</code> may later represent, derive, construct, and lower execution-facing objects that reference these primitives,</li>
  <li><code>Profiles/</code> does not redefine the intrinsic core merely by adding optional capability families.</li>
</ul>

<p>
The key rule is:
</p>

<pre>
primitive node in source
    !=
primitive-local contract
    !=
cross-cutting language meaning
    !=
execution-facing derived form
    !=
backend-private realization
</pre>

<hr/>

<h2 id="relation-with-other-specifications">4. Relation with Other Specifications</h2>

<p>
This document complements the following specifications:
</p>

<ul>
  <li><code>Libraries/Readme.md</code> — defines the architectural role of the intrinsic library layer.</li>
  <li><code>Expression/Diagram.md</code> — defines how primitive nodes are serialized in executable diagrams.</li>
  <li><code>Expression/Type.md</code> — defines built-in types, type compatibility, and implicit coercion rules.</li>
  <li><code>Expression/Control structures.md</code> — defines standardized source-facing structure families, which remain distinct from ordinary primitives.</li>
  <li><code>Expression/State and cycles.md</code> — defines the source-facing representation of explicit local memory and feedback-cycle construction.</li>
  <li><code>Language/Expression to validated meaning.md</code> — defines the semantic boundary from canonical source to validated meaning.</li>
  <li><code>Language/State and cycles.md</code> — defines the normative semantics of local memory and the validity rule for directed cycles.</li>
  <li><code>IR/Execution IR.md</code> — defines the open execution-facing representation that may preserve or reference <code>frog.core.*</code> primitives after validation.</li>
  <li><code>IR/Derivation rules.md</code> — defines how validated primitive usage becomes open Execution IR.</li>
  <li><code>IR/Lowering.md</code> — defines later specialization boundaries, not primitive ownership.</li>
  <li><code>IR/Backend contract.md</code> — defines later consumer-facing handoff obligations, not primitive catalog ownership.</li>
  <li><code>IDE/Palette.md</code> — defines how core primitives may be exposed in authoring tools.</li>
</ul>

<p>
This document defines the intrinsic standardized <code>frog.core</code> primitive surface.
It does not redefine graph structure, type-system ownership, canonical source shape, IR construction, lowering strategy, or backend-facing consumption boundaries.
</p>

<table>
  <tr>
    <th align="left">Layer</th>
    <th align="left">Primary ownership for this topic</th>
  </tr>
  <tr>
    <td><code>Expression/</code></td>
    <td>Canonical source representation of primitive nodes and local-memory source form</td>
  </tr>
  <tr>
    <td><code>Language/</code></td>
    <td>Cross-cutting normative semantics for validated meaning, including cycle legality</td>
  </tr>
  <tr>
    <td><code>Libraries/</code></td>
    <td>Intrinsic primitive identities, ports, required metadata, and primitive-local behavior</td>
  </tr>
  <tr>
    <td><code>IR/</code></td>
    <td>Execution-facing derived representation, identity/mapping, lowering, and backend-facing handoff boundaries</td>
  </tr>
  <tr>
    <td><code>IDE/</code></td>
    <td>Palette, authoring, observability, and debugging exposure</td>
  </tr>
</table>

<p>
Accordingly:
</p>

<ul>
  <li><code>Libraries/Core.md</code> owns the standardized identity and primitive-local contract of <code>frog.core.*</code>,</li>
  <li><code>Language/State and cycles.md</code> owns the cross-cutting rule that makes a directed cycle valid only when explicit local memory is present,</li>
  <li><code>Expression/State and cycles.md</code> owns the source-facing representation of that explicit memory in canonical program form,</li>
  <li><code>IR/</code> may later preserve <code>frog.core.delay</code> as an attributable explicit state-bearing execution object, but that later representation does not become the primitive’s normative home.</li>
</ul>

<hr/>

<h2 id="role-of-frogcore">5. Role of <code>frog.core</code></h2>

<p>
The <code>frog.core</code> library is the minimal intrinsic primitive library of the language.
It contains fundamental primitives that do not require optional profile support and do not depend on external ecosystem assumptions.
</p>

<p>
In source terms, uses of these primitives appear as <code>primitive</code> nodes in the executable diagram.
In library terms, the primitive-local contract is owned here.
</p>

<p>
Therefore:
</p>

<ul>
  <li><code>frog.core.add</code> is an intrinsic primitive identity of the FROG core surface,</li>
  <li>in a diagram, a use of that primitive appears as a <code>primitive</code> node with <code>type = "frog.core.add"</code>,</li>
  <li>in validated meaning and later execution-facing derivation, that primitive remains grounded in the primitive-local contract defined here.</li>
</ul>

<p>
The role of <code>frog.core</code> is to define the minimal standardized primitive vocabulary required by essentially all FROG programs.
It is not the place where structure families, widget object modeling, interoperability profiles, IR lowering families, or runtime-service bundles are defined.
</p>

<hr/>

<h2 id="naming-and-namespace">6. Naming and Namespace</h2>

<p>
FROG uses the following general naming pattern for intrinsic primitives:
</p>

<pre><code>frog.&lt;library&gt;.&lt;primitive&gt;</code></pre>

<p>
For this document:
</p>

<ul>
  <li><code>frog</code> identifies the language namespace,</li>
  <li><code>core</code> identifies the minimal intrinsic standard library,</li>
  <li>the final segment identifies the primitive name.</li>
</ul>

<p>
Examples:
</p>

<pre><code>frog.core.add
frog.core.equal
frog.core.select
frog.core.delay</code></pre>

<p>
Primitive names in <code>frog.core</code> SHOULD use lowercase snake_case where multiple words are needed.
</p>

<hr/>

<h2 id="scope-for-v01">7. Scope for v0.1</h2>

<p>
FROG v0.1 standardizes the following minimal intrinsic core primitives:
</p>

<ul>
  <li>arithmetic primitives,</li>
  <li>comparison primitives,</li>
  <li>boolean logic primitives,</li>
  <li>a simple value-selection primitive,</li>
  <li>a single explicit local-memory primitive: <code>frog.core.delay</code>.</li>
</ul>

<p>
FROG v0.1 does not attempt to define a full mathematical library, tensor library, signal-processing library, structure catalog, interop profile surface, lowering family, backend contract taxonomy, or deployment/runtime model in this document.
</p>

<hr/>

<h2 id="library-categories">8. Library Categories</h2>

<p>
The minimal <code>frog.core</code> library is organized into the following categories:
</p>

<ul>
  <li>Arithmetic</li>
  <li>Comparison</li>
  <li>Logic</li>
  <li>Selection</li>
  <li>Local Memory</li>
</ul>

<p>
This categorization is semantic.
It does not impose a mandatory palette layout, IR grouping strategy, lowering family, or runtime layout.
It provides a stable intrinsic basis for tooling and later execution-facing derivation.
</p>

<hr/>

<h2 id="typing-model">9. Typing Model</h2>

<p>
All <code>frog.core</code> primitives are typed according to <code>Expression/Type.md</code>.
</p>

<p>
Unless stated otherwise:
</p>

<ul>
  <li>port types MUST be valid FROG types,</li>
  <li>all type compatibility checks MUST follow the standard FROG type rules,</li>
  <li>all implicit coercions MUST follow the standard FROG coercion rules,</li>
  <li>primitive outputs MUST be deterministic for a given valid input and configuration.</li>
</ul>

<p>
In v0.1:
</p>

<ul>
  <li>all primitives in this document are stateless and side-effect-free, except <code>frog.core.delay</code>,</li>
  <li><code>frog.core.delay</code> is stateful and MUST be treated as an explicit local-memory primitive,</li>
  <li>the primitive-local delayed-value behavior of <code>frog.core.delay</code> is standardized here,</li>
  <li>the general legality of cycles containing <code>frog.core.delay</code> is governed normatively by <code>Language/State and cycles.md</code>.</li>
</ul>

<hr/>

<h2 id="arithmetic-primitives">10. Arithmetic Primitives</h2>

<h3>10.1 <code>frog.core.add</code></h3>

<p>Adds two or more numeric values.</p>

<ul>
  <li>required input ports: <code>a</code>, <code>b</code></li>
  <li>optional ordered input ports: <code>input_3</code>, <code>input_4</code>, ...</li>
  <li>output port: <code>result</code></li>
</ul>

<p>
Each operand MUST be either a numeric scalar or a numeric array. Scalar-only
addition resolves the common numeric scalar type using the FROG numeric
promotion rules. When at least one operand is an array, every array operand
MUST have the same structural shape, including rank, nesting, and fixed versus
dynamic extents. Scalar operands are lifted element by element across that
common shape. FROG Add performs no implicit broadcasting and MUST reject
different array shapes.
</p>

<p>
The result is scalar when every operand is scalar; otherwise it has the common
array shape. Its leaf type is the resolved numeric result type across every
connected operand. Implementations MUST evaluate connected operands in
canonical port order: <code>a</code>, <code>b</code>, then increasing
<code>input_N</code> suffixes, using a canonical left fold. Before each
addition, the operand MUST be coerced to the resolved result leaf type so a
variadic addition remains deterministic.
</p>

<p>
An editor MAY expose unused optional input slots before they are connected.
Those authoring slots do not participate in execution and MUST NOT be inferred
from presentation geometry by a runtime. A connected <code>input_N</code> edge,
not the displayed node height, establishes that the optional operand
participates in the executable graph.
</p>

<p>
The language contract does not impose a maximum optional-input count. An
authoring profile MAY impose a practical cap; the FROG Studio profile currently
uses sixteen total inputs. The machine-readable authority for this primitive is
<code>Libraries/Manifests/frog.core.add.v1.json</code>.
</p>

<h3>10.2 <code>frog.core.sub</code></h3>

<p>Subtracts <code>b</code> from <code>a</code>.</p>

<ul>
  <li>input ports: <code>a</code>, <code>b</code></li>
  <li>output port: <code>result</code></li>
</ul>

<h3>10.3 <code>frog.core.mul</code></h3>

<p>Multiplies two numeric values.</p>

<ul>
  <li>input ports: <code>a</code>, <code>b</code></li>
  <li>output port: <code>result</code></li>
</ul>

<h3>10.4 <code>frog.core.div</code></h3>

<p>Divides <code>a</code> by <code>b</code>.</p>

<ul>
  <li>input ports: <code>a</code>, <code>b</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>
Division by zero behavior MUST be defined by the active execution profile and relevant numeric-type rules.
The intrinsic primitive identity and primitive-local role of <code>frog.core.div</code> remain standardized here.
</p>

<h3>10.5 <code>frog.core.neg</code></h3>

<p>Arithmetic negation of a numeric value.</p>

<ul>
  <li>input port: <code>in</code></li>
  <li>output port: <code>result</code></li>
</ul>

<h3>10.6 <code>frog.core.abs</code></h3>

<p>Absolute value of a numeric input.</p>

<ul>
  <li>input port: <code>in</code></li>
  <li>output port: <code>result</code></li>
</ul>

<hr/>

<h2 id="comparison-primitives">11. Comparison Primitives</h2>

<h3>11.1 <code>frog.core.equal</code></h3>

<p>Returns whether two values are equal.</p>

<ul>
  <li>input ports: <code>a</code>, <code>b</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>
The output type is <code>bool</code>.
In v0.1, equality MUST be supported for built-in scalar types and any other value categories explicitly supported by the active profile or surrounding normative rules.
</p>

<h3>11.2 <code>frog.core.not_equal</code></h3>

<p>Returns whether two values are not equal.</p>

<ul>
  <li>input ports: <code>a</code>, <code>b</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>The output type is <code>bool</code>.</p>

<h3>11.3 <code>frog.core.less</code></h3>

<p>Returns whether <code>a &lt; b</code>.</p>

<ul>
  <li>input ports: <code>a</code>, <code>b</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>
The output type is <code>bool</code>.
In base v0.1, this primitive is defined for numeric operands.
</p>

<h3>11.4 <code>frog.core.less_or_equal</code></h3>

<p>Returns whether <code>a &lt;= b</code>.</p>

<ul>
  <li>input ports: <code>a</code>, <code>b</code></li>
  <li>output port: <code>result</code></li>
</ul>

<h3>11.5 <code>frog.core.greater</code></h3>

<p>Returns whether <code>a &gt; b</code>.</p>

<ul>
  <li>input ports: <code>a</code>, <code>b</code></li>
  <li>output port: <code>result</code></li>
</ul>

<h3>11.6 <code>frog.core.greater_or_equal</code></h3>

<p>Returns whether <code>a &gt;= b</code>.</p>

<ul>
  <li>input ports: <code>a</code>, <code>b</code></li>
  <li>output port: <code>result</code></li>
</ul>

<hr/>

<h2 id="logical-primitives">12. Logical Primitives</h2>

<h3>12.1 <code>frog.core.and</code></h3>

<p>Boolean conjunction.</p>

<ul>
  <li>input ports: <code>a</code>, <code>b</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>Both inputs MUST be <code>bool</code>. The output type is <code>bool</code>.</p>

<h3>12.2 <code>frog.core.or</code></h3>

<p>Boolean disjunction.</p>

<ul>
  <li>input ports: <code>a</code>, <code>b</code></li>
  <li>output port: <code>result</code></li>
</ul>

<h3>12.3 <code>frog.core.not</code></h3>

<p>Boolean negation.</p>

<ul>
  <li>input port: <code>in</code></li>
  <li>output port: <code>result</code></li>
</ul>

<h3>12.4 <code>frog.core.xor</code></h3>

<p>Boolean exclusive OR.</p>

<ul>
  <li>input ports: <code>a</code>, <code>b</code></li>
  <li>output port: <code>result</code></li>
</ul>

<hr/>

<h2 id="selection-primitive">13. Selection Primitive</h2>

<h3>13.1 <code>frog.core.select</code></h3>

<p>
Selects one of two input values based on a boolean condition.
</p>

<ul>
  <li>input ports: <code>condition</code>, <code>true_value</code>, <code>false_value</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>Rules:</p>

<ul>
  <li><code>condition</code> MUST be of type <code>bool</code>,</li>
  <li><code>true_value</code> and <code>false_value</code> MUST be type-compatible,</li>
  <li>the output type is the resolved common result type of the two value inputs.</li>
</ul>

<p>
This primitive is a value-selection primitive.
It is not a general control-structure primitive and it does not replace standardized structure families such as <code>case</code>.
</p>

<hr/>

<h2 id="local-memory-primitive">14. Local-Memory Primitive</h2>

<h3>14.1 <code>frog.core.delay</code></h3>

<p>
<code>frog.core.delay</code> is the minimal standard local-memory primitive for FROG v0.1.
</p>

<p>
It is the only stateful intrinsic primitive defined in this document.
This document defines its primitive identity, port model, required configuration surface, and primitive-local delayed-value behavior.
The normative semantics of local memory as a language concept, and the rule that determines whether a directed cycle is valid, are defined by <code>Language/State and cycles.md</code>.
</p>

<ul>
  <li>input port: <code>in</code></li>
  <li>output port: <code>out</code></li>
</ul>

<p>Required metadata:</p>

<ul>
  <li><code>initial</code></li>
</ul>

<p>Rules:</p>

<ul>
  <li><code>in</code> and <code>out</code> MUST have the same type,</li>
  <li>every <code>frog.core.delay</code> node MUST define an explicit <code>initial</code> value in v0.1,</li>
  <li><code>initial</code> MUST be compatible with the carried value type.</li>
</ul>

<p>
Primitive-local delayed-value model:
</p>

<pre><code>out(t) = state(t)
state(t + 1) = in(t)
state(0) = initial</code></pre>

<p>
This primitive enables deterministic and explicit delayed feedback in dataflow graphs.
However, the presence of a <code>frog.core.delay</code> node inside a graph does not by itself redefine the general language rule for cycles.
Whether a specific cycle is valid remains governed by <code>Language/State and cycles.md</code>.
</p>

<h3>14.2 Role of <code>initial</code></h3>

<p>
The <code>initial</code> field defines the primitive’s initial stored value for the first activation.
In v0.1, this field is mandatory to ensure deterministic initialization of explicit local memory.
</p>

<h3>14.3 Scope of ownership</h3>

<p>
For <code>frog.core.delay</code>, ownership is intentionally split as follows:
</p>

<ul>
  <li><code>Libraries/Core.md</code> owns the primitive name, required ports, required metadata, and primitive-local delayed-value behavior,</li>
  <li><code>Expression/State and cycles.md</code> owns the source-facing representation of explicit local memory in canonical graph form,</li>
  <li><code>Language/State and cycles.md</code> owns the cross-cutting semantic rule that makes a directed cycle valid only when explicit local memory is present,</li>
  <li><code>IR/</code> may later preserve this primitive as an explicit attributable state-bearing execution-facing object, but that later representation does not replace the primitive-local contract defined here.</li>
</ul>

<hr/>

<h2 id="diagram-representation">15. Diagram Representation</h2>

<p>
Uses of <code>frog.core</code> primitives are serialized as <code>primitive</code> nodes in the diagram.
</p>

<p>Example:</p>

<pre><code class="language-json">{
  "id": "add_1",
  "kind": "primitive",
  "type": "frog.core.add"
}</code></pre>

<p>Stateful example:</p>

<pre><code class="language-json">{
  "id": "delay_1",
  "kind": "primitive",
  "type": "frog.core.delay",
  "initial": 0.0
}</code></pre>

<p>
The exact port existence, direction, and typing of these nodes are resolved from this specification together with the type system and the surrounding graph and language rules.
</p>

<p>
Conceptually:
</p>

<pre><code>primitive node
   │
   ├── type = frog.core.&lt;primitive&gt;
   ├── standardized ports
   ├── optional or required metadata
   └── library-defined primitive-local contract
</code></pre>

<hr/>

<h2 id="validation-rules">16. Validation Rules</h2>

<p>
Implementations MUST enforce the following rules:
</p>

<ul>
  <li>every <code>frog.core</code> primitive reference MUST identify a valid standardized core primitive name,</li>
  <li>all required input ports for the referenced primitive MUST exist and be type-compatible,</li>
  <li>all produced output ports MUST match the primitive definition,</li>
  <li>all implicit coercions MUST follow <code>Expression/Type.md</code>,</li>
  <li>all primitives in this document MUST be treated as stateless unless explicitly defined as stateful.</li>
</ul>

<p>For <code>frog.core.delay</code> specifically:</p>

<ul>
  <li>the node MUST define <code>initial</code>,</li>
  <li><code>initial</code> MUST be type-compatible with the delay state type,</li>
  <li><code>in</code> and <code>out</code> MUST have the same type,</li>
  <li>the primitive MUST be treated as an explicit local-memory primitive,</li>
  <li>the graph-level validity of any cycle using that primitive MUST be evaluated according to <code>Language/State and cycles.md</code>.</li>
</ul>

<p>For boolean logic primitives:</p>

<ul>
  <li><code>and</code>, <code>or</code>, <code>not</code>, and <code>xor</code> MUST operate on <code>bool</code> values in v0.1.</li>
</ul>

<p>For comparison primitives:</p>

<ul>
  <li><code>less</code>, <code>less_or_equal</code>, <code>greater</code>, and <code>greater_or_equal</code> MUST operate on supported comparable operands, and in base v0.1 they are defined for numeric operands.</li>
</ul>

<hr/>

<h2 id="execution-facing-posture">17. Execution-Facing Posture</h2>

<p>
This document defines intrinsic primitive-local truth, not one execution-facing representation.
</p>

<p>
Accordingly:
</p>

<ul>
  <li>open Execution IR MAY preserve recognizable primitive execution objects corresponding to <code>frog.core.*</code>,</li>
  <li>derivation MAY add execution-facing explicitness such as resolved ports, directions, or classification records,</li>
  <li>lowering MAY later specialize these primitive-facing objects for backend-oriented use,</li>
  <li>backend contracts MAY declare consumer-facing assumptions about lowered primitive behavior,</li>
  <li>runtime-private realization MAY choose private storage, scheduling, or internal helper objects,</li>
  <li>but none of those later stages redefines the intrinsic primitive identity or primitive-local contract owned here.</li>
</ul>

<p>
This matters especially for <code>frog.core.delay</code>:
</p>

<pre>
intrinsic primitive truth
    -&gt; explicit local-memory primitive

open IR
    -&gt; may preserve explicit attributable state-bearing object

lowered form
    -&gt; may specialize into state read / write / commit patterns

backend contract
    -&gt; may declare explicit state-handling assumptions

private runtime
    -&gt; may realize storage through buffers, cells, registers, or retained objects
</pre>

<p>
The primitive stays the same intrinsic primitive across that corridor.
Only the later representation and realization change.
</p>

<hr/>

<h2 id="examples">18. Examples</h2>

<h3>18.1 Arithmetic primitive</h3>

<pre><code class="language-json">{
  "id": "add_1",
  "kind": "primitive",
  "type": "frog.core.add"
}</code></pre>

<h3>18.2 Selection primitive</h3>

<pre><code class="language-json">{
  "id": "select_1",
  "kind": "primitive",
  "type": "frog.core.select"
}</code></pre>

<h3>18.3 Local-memory primitive</h3>

<pre><code class="language-json">{
  "id": "delay_1",
  "kind": "primitive",
  "type": "frog.core.delay",
  "initial": 0.0
}</code></pre>

<h3>18.4 Delay used in explicit feedback</h3>

<pre><code>value ───────┐
             ▼
        [ frog.core.add ] ───► next_value
             ▲
             │
   [ frog.core.delay ]
             ▲
             │
          feedback</code></pre>

<p>
The exact validity of the feedback cycle is not determined by this sketch alone.
It depends on the language-level cycle-validity rule defined in <code>Language/State and cycles.md</code>.
</p>

<hr/>

<h2 id="out-of-scope-for-v01">19. Out of Scope for v0.1</h2>

<ul>
  <li>a full mathematical standard library,</li>
  <li>tensor operators,</li>
  <li>signal-processing operators,</li>
  <li>domain-specific operator packs,</li>
  <li>interop-oriented primitives that belong in optional profiles,</li>
  <li>implicit memory insertion,</li>
  <li>hidden stateful primitives beyond the standardized <code>frog.core.delay</code>,</li>
  <li>redefinition of structural control through ordinary primitives,</li>
  <li>IR lowering families, backend taxonomies, deployment-mode classes, or runtime-module layouts.</li>
</ul>

<hr/>

<h2 id="summary">20. Summary</h2>

<p>
The <code>frog.core</code> library defines the minimal intrinsic standardized primitive vocabulary of FROG.
</p>

<ul>
  <li>it provides arithmetic, comparison, logical, and simple selection primitives,</li>
  <li>it also defines the minimal explicit local-memory primitive <code>frog.core.delay</code>,</li>
  <li>all primitives in this document are used as <code>primitive</code> nodes in diagrams,</li>
  <li><code>frog.core.delay</code> is the only stateful core primitive in base v0.1,</li>
  <li>primitive identity and required primitive-local configuration are owned here,</li>
  <li>graph-wide local-memory and cycle-validity semantics remain owned by <code>Language/State and cycles.md</code>,</li>
  <li>later execution-facing derivation, lowering, backend-facing contracts, and private realization remain downstream from the primitive-local contracts defined here.</li>
</ul>

<p>
This gives FROG a compact, durable, and implementation-portable intrinsic primitive foundation for v0.1.
</p>
