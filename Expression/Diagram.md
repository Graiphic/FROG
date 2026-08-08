<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 FROG Diagram Specification</h1>

<p align="center">
  Definition of executable graphs in <strong>.frog</strong> programs<br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#purpose">2. Purpose of the Diagram</a></li>
  <li><a href="#relation-with-other-specifications">3. Relation with Other Specifications</a></li>
  <li><a href="#location-in-a-frog-file">4. Location in a <code>.frog</code> File</a></li>
  <li><a href="#diagram-structure">5. Diagram Structure</a></li>
  <li><a href="#diagram-scopes">6. Diagram Scopes</a></li>
  <li><a href="#conceptual-model">7. Conceptual Model</a></li>
  <li><a href="#node-model">8. Node Model</a></li>
  <li><a href="#standard-node-kinds-for-v01">9. Standard Node Kinds for v0.1</a></li>
  <li><a href="#port-model">10. Port Model</a></li>
  <li><a href="#port-resolution-model">11. Port Resolution Model</a></li>
  <li><a href="#edge-model">12. Edge Model</a></li>
  <li><a href="#annotations-documentation-and-tags">13. Annotations, Documentation, and Tags</a></li>
  <li><a href="#layout-information">14. Layout Information</a></li>
  <li><a href="#frog-dependencies">15. FROG Dependencies</a></li>
  <li><a href="#sub-frog-invocation">16. Sub-FROG Invocation</a></li>
  <li><a href="#interface-widget-and-structure-boundaries">17. Interface, Widget, and Structure Boundaries</a></li>
  <li><a href="#widget-interaction-model">18. Widget Interaction Model</a></li>
  <li><a href="#graph-level-participation-of-cycles-and-local-memory">19. Graph-Level Participation of Cycles and Local Memory</a></li>
  <li><a href="#graph-validation">20. Graph Validation</a></li>
  <li><a href="#relation-with-execution-semantics">21. Relation with Execution Semantics</a></li>
  <li><a href="#examples">22. Examples</a></li>
  <li><a href="#summary">23. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
The <code>diagram</code> section defines the executable graph of a FROG.
It is the authoritative structural representation of that graph in canonical source form.
</p>

<p>
A FROG executable unit behaves conceptually like a graphical dataflow program with a public interface,
an internal executable graph, optional front-panel widget participation, and optional nested language structures.
In canonical source form, the <code>diagram</code> is the authoritative representation of that executable graph.
</p>

<p>
The diagram contains:
</p>

<ul>
  <li>the nodes representing executable or structurally meaningful graph elements,</li>
  <li>the edges representing directed connectivity between ports,</li>
  <li>the canonical graph-level participation of public interface ports,</li>
  <li>the canonical graph-level participation of widget values and widget references,</li>
  <li>the invocation sites of other FROGs,</li>
  <li>the explicit local-memory elements required for valid feedback,</li>
  <li>the source-level annotations used for documentation and editor organization.</li>
</ul>

<p>
Interpretation of a validated diagram depends on additional specifications, including:
</p>

<ul>
  <li>the public interface contract,</li>
  <li>the FROG type system,</li>
  <li>the widget model,</li>
  <li>the widget interaction model,</li>
  <li>the normative control-structure semantics,</li>
  <li>the normative local-memory and cycle-validity rules,</li>
  <li>the active primitive catalog or stricter execution profile.</li>
</ul>

<p>
This document defines the graph-level source representation itself.
It does not own the full execution semantics of every graph element.
</p>

<p>
This document also does not own repository-wide version policy.
Top-level <code>spec_version</code> identifies the source-format compatibility target of the containing <code>.frog</code> file, while published specification corpus versioning remains governed centrally in <code>Versioning/Readme.md</code>.
</p>

<hr/>

<h2 id="purpose">2. Purpose of the Diagram</h2>

<p>
The diagram provides:
</p>

<ul>
  <li>the executable logic graph of the FROG,</li>
  <li>the canonical dependency graph of values and explicit effects,</li>
  <li>the canonical link between public interface ports and internal execution logic,</li>
  <li>the canonical link between front-panel widget participation and internal execution logic,</li>
  <li>the hierarchical composition mechanism used to invoke other FROGs,</li>
  <li>the canonical place where valid feedback loops and explicit local memory are represented in source form,</li>
  <li>the canonical source-level space for diagram annotations and organization.</li>
</ul>

<p>
The diagram MUST be sufficient to determine the executable graph shape of the FROG.
Other sections may define semantics needed to interpret that graph, but they do not replace the graph itself.
</p>

<hr/>

<h2 id="relation-with-other-specifications">3. Relation with Other Specifications</h2>

<p>
This document depends on the following FROG specifications:
</p>

<ul>
  <li><code>Expression/Readme.md</code> — defines the role of <code>Expression/</code> as the canonical source-specification layer,</li>
  <li><code>Expression/Type.md</code> — ordinary value typing and compatibility rules,</li>
  <li><code>Expression/Interface.md</code> — public interface declaration and public boundary semantics,</li>
  <li><code>Expression/Front panel.md</code> — serialized UI composition and widget declarations,</li>
  <li><code>Expression/Widget.md</code> — widget classes, primary value model, and widget parts,</li>
  <li><code>Expression/Widget interaction.md</code> — object-style widget interaction primitives and optional UI sequencing,</li>
  <li><code>Expression/Control structures.md</code> — source-facing representation of structure nodes and their serialized form,</li>
  <li><code>Expression/State and cycles.md</code> — source-facing representation of explicit local-memory elements and feedback-cycle formation constraints,</li>
  <li><code>Language/Readme.md</code> — defines the role of <code>Language/</code> as the normative execution-semantics layer,</li>
  <li><code>Language/Control structures.md</code> — normative execution semantics for structure families such as <code>case</code>, <code>for_loop</code>, and <code>while_loop</code>,</li>
  <li><code>Language/State and cycles.md</code> — normative semantics for local memory and cycle validity,</li>
  <li><code>Libraries/Core.md</code> — standard primitive definitions such as <code>frog.core.add</code>, <code>frog.core.mul</code>, and <code>frog.core.delay</code>,</li>
  <li><code>Libraries/UI.md</code> — standard executable UI primitive definitions such as <code>frog.ui.property_read</code>, <code>frog.ui.property_write</code>, and <code>frog.ui.method_invoke</code>,</li>
  <li><code>Versioning/Readme.md</code> — centralized distinction between specification corpus version, top-level <code>spec_version</code>, and program artifact versioning.</li>
</ul>

<p>
This document defines graph-level representation and validation rules.
It does not redefine source ownership or execution semantics already owned by the dedicated specifications above.
</p>

<hr/>

<h2 id="location-in-a-frog-file">4. Location in a <code>.frog</code> File</h2>

<p>
The diagram appears as a top-level object in the canonical <code>.frog</code> source file.
</p>

<pre><code>{
  "spec_version": "0.1",
  "metadata": {},
  "interface": {},
  "connector": {},
  "diagram": {},
  "front_panel": {}
}</code></pre>

<p>
The <code>diagram</code> section MUST exist in a canonical <code>.frog</code> source file.
</p>

<p>
In this source shape:
</p>

<ul>
  <li>top-level <code>spec_version</code> identifies the source-format compatibility target of the file,</li>
  <li><code>diagram</code> defines the authoritative executable graph in canonical source,</li>
  <li>the repository-wide specification corpus version remains governed centrally in <code>Versioning/Readme.md</code>.</li>
</ul>

<hr/>

<h2 id="diagram-structure">5. Diagram Structure</h2>

<p>
The <code>diagram</code> object contains up to four fields:
</p>

<pre><code>"diagram": {
  "nodes": [],
  "edges": [],
  "dependencies": [],
  "annotations": []
}</code></pre>

<p>
Fields:
</p>

<ul>
  <li><code>nodes</code> — required array of graph nodes,</li>
  <li><code>edges</code> — required array of directed graph connections,</li>
  <li><code>dependencies</code> — optional array of referenced external FROGs,</li>
  <li><code>annotations</code> — optional array of visible diagram annotations.</li>
</ul>

<p>
Rules:
</p>

<ul>
  <li><code>nodes</code> MUST exist and MUST be an array.</li>
  <li><code>edges</code> MUST exist and MUST be an array.</li>
  <li><code>dependencies</code> MAY be omitted when unused.</li>
  <li><code>annotations</code> MAY be omitted when unused.</li>
</ul>

<p>
The <code>annotations</code> field belongs to canonical source because it expresses author-created
documentation and visible organization of the diagram.
It MUST NOT affect execution semantics.
</p>

<p>
Diagram objects such as nodes, edges, structure regions, and annotations MAY be referenced by source provenance attestations through <code>ide.provenance</code>. The diagram object itself remains the executable source object; provenance evidence SHOULD be carried outside the object rather than adding mandatory signature or trust fields to every node or edge.
</p>

<hr/>

<h2 id="diagram-scopes">6. Diagram Scopes</h2>

<p>
FROG diagrams are recursive.
The top-level <code>diagram</code> object defines the outer executable graph of the FROG.
Language structures MAY own internal executable regions, and each such region contains its own local diagram scope.
</p>

<p>
Accordingly, FROG distinguishes:
</p>

<ul>
  <li><strong>top-level diagram scope</strong> — the outer executable graph of the FROG,</li>
  <li><strong>region-local diagram scope</strong> — a nested executable graph owned by a structure region.</li>
</ul>

<p>
Each diagram scope has its own local:
</p>

<ul>
  <li>node identifier namespace,</li>
  <li>edge identifier namespace,</li>
  <li>annotation identifier namespace.</li>
</ul>

<p>
Rules defined in this document apply both to the top-level diagram and to region-local diagrams
unless a structure-specific rule explicitly states otherwise.
</p>

<p>
An edge MUST connect nodes within the same diagram scope.
Cross-scope communication MUST occur only through the explicit mechanisms defined by structure boundaries.
</p>

<hr/>

<h2 id="conceptual-model">7. Conceptual Model</h2>

<p>
A validated diagram defines a directed graph whose nodes expose typed ports and whose edges connect compatible ports.
The graph is structural and execution-relevant:
</p>

<ul>
  <li>structural, because it explicitly defines graph boundaries and nesting,</li>
  <li>execution-relevant, because it provides the graph form consumed by the language, primitive, and widget-interaction semantics defined elsewhere.</li>
</ul>

<p>
At conceptual level:
</p>

<ul>
  <li>a node is a graph element that owns ports and participates in execution or graph boundaries,</li>
  <li>a port is a typed participation point on a node,</li>
  <li>an edge is a directed connection from one node port to another node port,</li>
  <li>a diagram scope is a container of nodes, edges, and annotations with its own local namespaces.</li>
</ul>

<p>
The diagram itself does not impose a textual instruction order.
The validated graph participates in execution according to dataflow, structure, local-memory, and widget-interaction semantics owned by other specifications.
</p>

<hr/>

<h2 id="node-model">8. Node Model</h2>

<p>
A node represents an executable or structurally meaningful element of the graph.
Every node owns a set of input and output ports, whether those ports are declared directly in the node,
derived from a referenced primitive signature, derived from a structure family,
derived from an interface boundary, or derived from widget semantics.
</p>

<p>
Every node MUST define:
</p>

<ul>
  <li><code>id</code> — unique node identifier within the owning diagram scope,</li>
  <li><code>kind</code> — node category.</li>
</ul>

<p>
A node MAY also define:
</p>

<ul>
  <li><code>layout</code> — graphical editor position and size metadata,</li>
  <li><code>doc</code> — optional structured documentation,</li>
  <li><code>tags</code> — optional structured tags,</li>
  <li>kind-specific fields defined by this document or by a referenced specification.</li>
</ul>

<h3>8.1 Common node shape</h3>

<pre><code>{
  "id": "node_1",
  "kind": "primitive",
  "type": "frog.core.add",
  "layout": {
    "x": 120,
    "y": 80
  }
}</code></pre>

<p>
Nodes are structural objects.
Their valid ports are resolved from:
</p>

<ul>
  <li>their <code>kind</code>,</li>
  <li>their kind-specific metadata,</li>
  <li>the active primitive catalog or structure family definition,</li>
  <li>other referenced specifications when applicable.</li>
</ul>

<p>
Rules:
</p>

<ul>
  <li>Node identifiers MUST be unique within the owning diagram scope.</li>
  <li>Node identifiers MAY be reused in different nested scopes.</li>
  <li>A node MUST NOT expose hidden semantically required ports outside the canonical resolution rules of its kind.</li>
</ul>

<hr/>

<h2 id="standard-node-kinds-for-v01">9. Standard Node Kinds for v0.1</h2>

<p>
FROG v0.1 defines the following standard node kinds:
</p>

<ul>
  <li><code>primitive</code></li>
  <li><code>structure</code></li>
  <li><code>subfrog</code></li>
  <li><code>interface_input</code></li>
  <li><code>interface_output</code></li>
  <li><code>widget_value</code></li>
  <li><code>widget_reference</code></li>
</ul>

<h3>9.1 <code>primitive</code></h3>

<p>
A <code>primitive</code> node represents a built-in or profile-defined executable primitive.
</p>

<pre><code>{
  "id": "mul_1",
  "kind": "primitive",
  "type": "frog.core.mul",
  "layout": {
    "x": 240,
    "y": 120
  }
}</code></pre>

<p>
Rules:
</p>

<ul>
  <li><code>type</code> MUST exist.</li>
  <li><code>type</code> MUST identify a valid primitive in the active catalog or profile.</li>
  <li>Primitive identifiers SHOULD use canonical namespaces such as <code>frog.core.*</code>, <code>frog.ui.*</code>, or another standardized library namespace.</li>
  <li>Primitive meaning and port signature are defined by the referenced primitive definition.</li>
  <li>A primitive definition MAY expose a canonical extensible port family. Edges MUST address each such port by its stable canonical identifier.</li>
  <li>Editor geometry MAY expose unused extensible-port slots, but layout metadata MUST NOT create executable operands or alter primitive semantics.</li>
</ul>

<p>
For example, <code>frog.core.add</code> always exposes <code>a</code> and
<code>b</code>, and may receive additional ordered operands through
<code>input_3</code>, <code>input_4</code>, and later canonical port identifiers.
Only connected optional ports participate in the validated graph. A node
<code>layout.height</code> value may preserve the editor's expanded shape, but
it is not a substitute for those edge port identifiers.
</p>

<p>
Primitive nodes include ordinary computational functions,
widget interaction primitives,
and local-memory primitives such as <code>frog.core.delay</code>.
</p>

<h3>9.2 <code>structure</code></h3>

<p>
A <code>structure</code> node represents a language-level control structure.
Its canonical source representation is defined by <code>Expression/Control structures.md</code>.
Its normative execution semantics are defined by <code>Language/Control structures.md</code>.
</p>

<pre><code>{
  "id": "case_1",
  "kind": "structure",
  "structure_type": "case",
  "boundary": {
    "inputs": [],
    "outputs": []
  },
  "structure_terminals": {},
  "regions": [],
  "layout": {
    "x": 300,
    "y": 120,
    "width": 260,
    "height": 180
  }
}</code></pre>

<p>
Rules:
</p>

<ul>
  <li><code>structure_type</code> MUST exist.</li>
  <li><code>structure_type</code> MUST identify a valid standardized or profile-defined structure family.</li>
  <li>The canonical standardized structure types for v0.1 are <code>case</code>, <code>for_loop</code>, and <code>while_loop</code>.</li>
  <li><code>boundary</code>, <code>structure_terminals</code>, and <code>regions</code> MUST be valid for the chosen structure type.</li>
  <li>Structure internals and region representation MUST remain aligned with the control-structure specifications and MUST NOT be redefined ad hoc by tools.</li>
</ul>

<p>
There is no separate canonical <code>if</code> structure in v0.1.
Boolean conditional branching is represented canonically as <code>case</code> with selector type <code>bool</code>.
</p>

<h3>9.3 <code>subfrog</code></h3>

<p>
A <code>subfrog</code> node represents an invocation of another FROG.
</p>

<pre><code>{
  "id": "math_add_1",
  "kind": "subfrog",
  "ref": "Math.Add",
  "layout": {
    "x": 420,
    "y": 120
  }
}</code></pre>

<p>
Rules:
</p>

<ul>
  <li><code>ref</code> MUST exist.</li>
  <li><code>ref</code> MUST match a declared dependency identifier.</li>
  <li>The input and output ports of the node are derived from the referenced FROG interface.</li>
</ul>

<h3>9.4 <code>interface_input</code></h3>

<p>
An <code>interface_input</code> node represents the entry point of a declared public interface input inside the executable graph.
</p>

<pre><code>{
  "id": "input_a",
  "kind": "interface_input",
  "interface_port": "a",
  "layout": {
    "x": 40,
    "y": 100
  }
}</code></pre>

<p>
Rules:
</p>

<ul>
  <li><code>interface_port</code> MUST exist.</li>
  <li><code>interface_port</code> MUST reference an existing input declared in the <code>interface</code> section.</li>
  <li>The node exposes exactly one output port named <code>value</code>.</li>
</ul>

<h3>9.5 <code>interface_output</code></h3>

<p>
An <code>interface_output</code> node represents the exit point used to drive a declared public interface output from the executable graph.
</p>

<pre><code>{
  "id": "output_result",
  "kind": "interface_output",
  "interface_port": "result",
  "layout": {
    "x": 760,
    "y": 100
  }
}</code></pre>

<p>
Rules:
</p>

<ul>
  <li><code>interface_port</code> MUST exist.</li>
  <li><code>interface_port</code> MUST reference an existing output declared in the <code>interface</code> section.</li>
  <li>The node exposes exactly one input port named <code>value</code>.</li>
</ul>

<h3>9.6 <code>widget_value</code></h3>

<p>
A <code>widget_value</code> node represents the natural primary-value participation of a front-panel widget in the executable graph.
It materializes the canonical natural widget value path defined by <code>Widget.md</code>.
</p>

<pre><code>{
  "id": "ctrl_gain_value",
  "kind": "widget_value",
  "widget": "ctrl_gain",
  "layout": {
    "x": 100,
    "y": 220
  }
}</code></pre>

<p>
Rules:
</p>

<ul>
  <li><code>widget</code> MUST exist.</li>
  <li><code>widget</code> MUST reference an existing widget declared in the <code>front_panel</code>.</li>
  <li>The referenced widget MUST be value-carrying.</li>
  <li>The node exposes exactly one port named <code>value</code>.</li>
  <li>If the widget role is <code>control</code>, the <code>value</code> port is an output.</li>
  <li>If the widget role is <code>indicator</code>, the <code>value</code> port is an input.</li>
  <li>The port type MUST equal the widget <code>value_type</code>.</li>
</ul>

<p>
A front-panel widget does not define a public interface port by itself.
Public interface semantics remain represented canonically through <code>interface_input</code> and <code>interface_output</code>.
</p>

<h3>9.7 <code>widget_reference</code></h3>

<p>
A <code>widget_reference</code> node represents an explicit object-style reference to a widget.
It is used when the diagram needs to access widget properties, widget parts, or widget methods through the widget interaction model.
</p>

<pre><code>{
  "id": "ctrl_gain_ref",
  "kind": "widget_reference",
  "widget": "ctrl_gain",
  "layout": {
    "x": 80,
    "y": 300
  }
}</code></pre>

<p>
Rules:
</p>

<ul>
  <li><code>widget</code> MUST exist.</li>
  <li><code>widget</code> MUST reference an existing widget declared in the <code>front_panel</code>.</li>
  <li>The node exposes exactly one output port named <code>ref</code>.</li>
  <li>The <code>ref</code> token is an opaque widget-reference token in v0.1.</li>
</ul>

<hr/>

<h2 id="port-model">10. Port Model</h2>

<p>
FROG v0.1 does not require every node to declare an explicit <code>ports</code> array inside the diagram source.
Instead, valid ports are resolved from the node kind and its definition source.
</p>

<p>
A port is a typed participation point on a node.
A port belongs to exactly one node and has a direction relative to that node.
</p>

<p>
For v0.1, a port is characterized conceptually by:
</p>

<ul>
  <li>its owning node,</li>
  <li>its local port name,</li>
  <li>its direction — input or output,</li>
  <li>its type, including opaque sequencing-token or widget-reference types where applicable.</li>
</ul>

<p>
Rules:
</p>

<ul>
  <li>A port name MUST be unique within the resolved port set of its owning node.</li>
  <li>Each input port accepts at most one incoming ordinary driver edge unless a referenced primitive or structure explicitly defines otherwise.</li>
  <li>An output port MAY fan out to multiple consumer ports where type and scope rules allow.</li>
</ul>

<hr/>

<h2 id="port-resolution-model">11. Port Resolution Model</h2>

<p>
Port resolution rules are:
</p>

<ul>
  <li><strong>primitive</strong> — ports are defined by the primitive function signature,</li>
  <li><strong>structure</strong> — ports are defined by the structure family source model, its boundary, and its outer-visible terminals,</li>
  <li><strong>subfrog</strong> — ports are derived from the referenced FROG interface,</li>
  <li><strong>interface_input</strong> — exactly one output port named <code>value</code>,</li>
  <li><strong>interface_output</strong> — exactly one input port named <code>value</code>,</li>
  <li><strong>widget_value</strong> — exactly one port named <code>value</code>, with direction resolved from widget role and type resolved from widget <code>value_type</code>,</li>
  <li><strong>widget_reference</strong> — exactly one output port named <code>ref</code>.</li>
</ul>

<h3>11.1 Primitive port resolution for widget interaction</h3>

<p>
For canonical diagram port resolution in v0.1, the standardized <code>frog.ui.*</code> primitive signatures align with
<code>Libraries/UI.md</code> and <code>Expression/Widget interaction.md</code> as follows:
</p>

<h4>11.1.1 <code>frog.ui.property_read</code></h4>

<ul>
  <li>required input port: <code>ref</code>,</li>
  <li>optional input port: <code>ui_in</code>,</li>
  <li>required output port: <code>value</code>,</li>
  <li>optional output port: <code>ui_out</code>.</li>
</ul>

<p>
The <code>value</code> port type is defined by the addressed widget member.
The <code>ref</code> port is an opaque widget-reference token.
The optional <code>ui_in</code> and <code>ui_out</code> ports are opaque UI sequencing ports.
</p>

<h4>11.1.2 <code>frog.ui.property_write</code></h4>

<ul>
  <li>required input port: <code>ref</code>,</li>
  <li>required input port: <code>value</code>,</li>
  <li>optional input port: <code>ui_in</code>,</li>
  <li>optional output port: <code>ui_out</code>.</li>
</ul>

<p>
The <code>value</code> port type is defined by the addressed widget member.
The <code>ref</code> port is an opaque widget-reference token.
The optional <code>ui_in</code> and <code>ui_out</code> ports are opaque UI sequencing ports.
</p>

<h4>11.1.3 <code>frog.ui.method_invoke</code></h4>

<ul>
  <li>required input port: <code>ref</code>,</li>
  <li>zero or more method parameter input ports, in canonical method order,</li>
  <li>optional input port: <code>ui_in</code>,</li>
  <li>zero or more method result output ports, in canonical method order,</li>
  <li>optional output port: <code>ui_out</code>.</li>
</ul>

<p>
Method parameter and result ports are defined by the addressed widget or widget-part method signature.
The <code>ref</code> port is an opaque widget-reference token.
The optional <code>ui_in</code> and <code>ui_out</code> ports are opaque UI sequencing ports.
</p>

<h3>11.2 Primitive port resolution for local memory</h3>

<p>
The minimal standard local-memory primitive for v0.1 is <code>frog.core.delay</code>.
Its canonical signature is defined by <code>Libraries/Core.md</code>, its source-facing constraints are defined by
<code>Expression/State and cycles.md</code>, and its normative execution semantics are defined by
<code>Language/State and cycles.md</code>.
</p>

<p>
For canonical diagram validation in v0.1:
</p>

<ul>
  <li><code>frog.core.delay</code> MUST define an <code>initial</code> field,</li>
  <li>its input port <code>in</code> and output port <code>out</code> MUST have the same type,</li>
  <li>the <code>initial</code> value MUST be type-compatible with that type.</li>
</ul>

<h3>11.3 Structure port resolution</h3>

<p>
For a <code>structure</code> node, visible ports are derived from the canonical source form of the referenced structure family.
This includes boundary inputs, boundary outputs, selector terminals, loop count terminals, continuation terminals,
loop index terminals, and other outer-visible structure-defined terminals where applicable.
</p>

<h3>11.4 Sub-FROG port resolution</h3>

<p>
For a <code>subfrog</code> node, ports are derived from the declared public interface of the referenced dependency.
</p>

<hr/>

<h2 id="edge-model">12. Edge Model</h2>

<p>
An edge connects one output port to one input port.
Edges define directed graph connectivity.
</p>

<h3>12.1 Common edge shape</h3>

<pre><code>{
  "id": "e1",
  "from": { "node": "input_a", "port": "value" },
  "to":   { "node": "add_1",   "port": "a" },
  "bends": [
    { "x": 180, "y": 104 },
    { "x": 180, "y": 92 }
  ]
}</code></pre>

<p>
Every edge MUST define:
</p>

<ul>
  <li><code>id</code> — unique edge identifier within the owning diagram scope,</li>
  <li><code>from.node</code> — source node identifier,</li>
  <li><code>from.port</code> — source output port identifier,</li>
  <li><code>to.node</code> — destination node identifier,</li>
  <li><code>to.port</code> — destination input port identifier.</li>
</ul>

<p>
An edge MAY define an optional <code>bends</code> array that preserves its editor route.
Each bend MUST contain finite numeric <code>x</code> and <code>y</code> coordinates in diagram space.
Bends are ordered from the source endpoint toward the destination endpoint.
They are layout metadata and MUST NOT alter the directed connectivity or validated execution semantics of the edge.
When <code>bends</code> is absent or empty, an editor MAY choose any route that preserves the same connectivity.
</p>

<h3>12.2 Edge validity</h3>

<p>
An edge is valid only if all of the following hold:
</p>

<ul>
  <li>the source node exists in the same diagram scope,</li>
  <li>the destination node exists in the same diagram scope,</li>
  <li>the source port exists and is an output port,</li>
  <li>the destination port exists and is an input port,</li>
  <li>the source value type is compatible with the destination input type,</li>
  <li>the connection does not violate structure-boundary rules or other dedicated specifications.</li>
</ul>

<h3>12.3 Fan-out and single-driver rule</h3>

<ul>
  <li>An output port MAY drive multiple destination ports when type compatibility holds.</li>
  <li>An input port MUST NOT have more than one incoming edge unless a stricter structure-specific rule explicitly defines otherwise.</li>
  <li>All edges with the same <code>from.node</code> and <code>from.port</code> MUST be interpreted as branches of one logical dataflow net.</li>
  <li>Fan-out remains serialized as one directed edge per destination; it MUST NOT require an executable junction node.</li>
</ul>

<p>
Editors MAY give branches of the same logical net shared route coordinates and render an internal junction marker at a shared point.
Such a marker is a graphical representation of fan-out, not a node and not an additional source of data.
A geometric crossing between edges from different source ports MUST NOT be interpreted as a junction merely because their routes intersect.
</p>

<h3>12.4 Scope locality</h3>

<p>
An edge MUST connect nodes within the same diagram scope.
Cross-scope communication MUST occur only through the explicit mechanisms defined by structure boundaries.
</p>

<hr/>

<h2 id="annotations-documentation-and-tags">13. Annotations, Documentation, and Tags</h2>

<h3>13.1 Diagram annotations</h3>

<p>
Annotations are optional source-level objects used to document and visually organize the diagram.
They do not participate in execution.
</p>

<pre><code>{
  "id": "ann_1",
  "kind": "text",
  "text": "Signal conditioning",
  "layout": {
    "x": 180,
    "y": 40,
    "width": 180,
    "height": 24
  }
}</code></pre>

<p>
The exact annotation vocabulary MAY be extended by editor profiles,
but canonical source MUST preserve the distinction between annotations and executable nodes.
</p>

<h3>13.2 Node documentation</h3>

<p>
A node MAY define a <code>doc</code> object.
This field is source documentation only.
It MUST NOT change execution behavior.
</p>

<h3>13.3 Tags</h3>

<p>
A node MAY define a <code>tags</code> array.
Tags MAY be used by tools for organization, search, or diagnostics,
but MUST NOT alter validated execution semantics.
</p>

<hr/>

<h2 id="layout-information">14. Layout Information</h2>

<p>
Layout information belongs to canonical source because FROG is a graphical language.
</p>

<p>
Typical layout fields include:
</p>

<ul>
  <li><code>x</code>,</li>
  <li><code>y</code>,</li>
  <li><code>width</code>,</li>
  <li><code>height</code>,</li>
  <li><code>bends</code> on an edge for ordered wire-routing points.</li>
</ul>

<p>
Rules:
</p>

<ul>
  <li>Layout metadata MAY be omitted where unnecessary.</li>
  <li>Layout metadata MUST NOT change validated execution semantics.</li>
  <li>Structure layout MAY include size information for editor rendering.</li>
</ul>

<hr/>

<h2 id="frog-dependencies">15. FROG Dependencies</h2>

<p>
When a diagram invokes other FROGs through <code>subfrog</code> nodes, those external FROGs MUST be declared in
<code>diagram.dependencies</code>.
</p>

<pre><code>"dependencies": [
  {
    "name": "Math.Add",
    "path": "lib/math/add.frog"
  }
]</code></pre>

<p>
Dependency fields:
</p>

<ul>
  <li><code>name</code> — logical identifier used by <code>subfrog.ref</code>,</li>
  <li><code>path</code> — normalized portable path to the referenced <code>.frog</code> source.</li>
</ul>

<p>
Rules:
</p>

<ul>
  <li>Dependency names MUST be unique within the diagram.</li>
  <li>A <code>subfrog.ref</code> value MUST match one declared dependency name.</li>
  <li>The referenced dependency MUST resolve to a FROG whose interface can define the node boundary.</li>
  <li><code>path</code> MUST be relative to the directory containing the declaring FROG document.</li>
  <li><code>path</code> MUST use <code>/</code> as its serialized separator and MUST be lexically normalized.</li>
  <li><code>path</code> MUST NOT be an absolute operating-system path, a Windows drive path, a UNC path, a rooted POSIX path, or a <code>file:</code> URI.</li>
  <li>Resolution MUST NOT depend on the process working directory. Tools MUST resolve <code>path</code> from the declaring FROG document location.</li>
  <li>Parent segments such as <code>../shared/filter.frog</code> MAY be used when permitted by the active workspace or package source-root policy.</li>
  <li>When a document is moved or saved under another path, authoring tools MUST rebase <code>path</code> while preserving the logical <code>name</code> used by <code>subfrog.ref</code>.</li>
  <li>An absolute path MAY be accepted only as a legacy authoring input. It MUST be converted to a portable relative path before canonical source is written.</li>
  <li>An unresolved or non-portable dependency MUST produce a diagnostic; a tool MUST NOT silently serialize an absolute fallback.</li>
</ul>

<p>
Non-file dependency sources, including registries, package identities, and network locations, require an explicitly
discriminated source-reference form. They MUST NOT be encoded by overloading <code>path</code> with a URI or a
machine-local locator.
</p>

<hr/>

<h2 id="sub-frog-invocation">16. Sub-FROG Invocation</h2>

<p>
A <code>subfrog</code> node invokes another FROG as a reusable executable unit.
</p>

<p>
Its externally visible ports are derived from the referenced FROG interface:
</p>

<ul>
  <li>each public input of the referenced FROG becomes an input port of the <code>subfrog</code> node,</li>
  <li>each public output of the referenced FROG becomes an output port of the <code>subfrog</code> node.</li>
</ul>

<p>
A <code>subfrog</code> node MUST NOT invent a different boundary shape than the one implied by the referenced interface.
</p>

<hr/>

<h2 id="interface-widget-and-structure-boundaries">17. Interface, Widget, and Structure Boundaries</h2>

<h3>17.1 Public interface boundary</h3>

<p>
Public interface participation in the diagram is represented canonically by:
</p>

<ul>
  <li><code>interface_input</code></li>
  <li><code>interface_output</code></li>
</ul>

<p>
Conceptually:
</p>

<pre><code>external input  → interface_input.value   → internal graph
internal graph  → interface_output.value  → external output</code></pre>

<p>
A front-panel widget does not create a public interface port by itself.
The front panel defines UI composition, not public API semantics.
</p>

<h3>17.2 Front-panel widget boundary</h3>

<p>
Front-panel primary-value participation is represented canonically by:
</p>

<ul>
  <li><code>widget_value</code></li>
</ul>

<p>
Object-style widget access is represented canonically by:
</p>

<ul>
  <li><code>widget_reference</code>,</li>
  <li><code>frog.ui.property_read</code>,</li>
  <li><code>frog.ui.property_write</code>,</li>
  <li><code>frog.ui.method_invoke</code>.</li>
</ul>

<p>
A widget MAY expose its primary value both naturally through <code>widget_value</code>
and as a property through the object-style path when the widget class allows it.
These two paths MUST remain semantically distinct.
</p>

<h3>17.3 Structure boundary</h3>

<p>
A structure boundary is part of the executable graph representation of the program.
Boundary values crossing the structure wall MUST be represented explicitly according to the control-structure model.
</p>

<p>
This document does not redefine structure-boundary internals.
Their canonical source representation is defined by <code>Expression/Control structures.md</code>,
and their normative execution meaning is defined by <code>Language/Control structures.md</code>.
</p>

<hr/>

<h2 id="widget-interaction-model">18. Widget Interaction Model</h2>

<h3>18.1 Natural primary-value path</h3>

<p>
The natural path is:
</p>

<pre><code>widget instance → widget primary value → widget_value node → dataflow graph</code></pre>

<p>
This path SHOULD be used for ordinary control and indicator wiring.
</p>

<h3>18.2 Object-style path</h3>

<p>
The object-style path is:
</p>

<pre><code>widget instance → widget_reference node → frog.ui.* primitive</code></pre>

<p>
This path MUST be used for:
</p>

<ul>
  <li>property reads,</li>
  <li>property writes,</li>
  <li>method invocation,</li>
  <li>object-style access to widget parts.</li>
</ul>

<h3>18.3 Primitive-local member descriptors</h3>

<p>
For canonical diagram representation:
</p>

<ul>
  <li>property interaction primitives MUST use <code>widget_member</code>,</li>
  <li>method interaction primitives MUST use <code>widget_method</code>.</li>
</ul>

<h3>18.4 Optional UI sequencing</h3>

<p>
Widget interaction primitives MAY use optional <code>ui_in</code> and <code>ui_out</code> ports for explicit ordering of UI side effects.
These sequencing ports are opaque and MUST NOT be interpreted as ordinary value ports.
</p>

<hr/>

<h2 id="graph-level-participation-of-cycles-and-local-memory">19. Graph-Level Participation of Cycles and Local Memory</h2>

<p>
Cycles and explicit local memory participate in the diagram as graph-level elements.
This document defines only that participation in canonical source form.
</p>

<p>
In particular:
</p>

<ul>
  <li>directed cycles appear through ordinary graph connectivity inside a diagram scope,</li>
  <li>explicit local memory appears through local-memory primitive nodes such as <code>frog.core.delay</code>,</li>
  <li>graph-level validation of feedback paths MUST remain aligned with <code>Expression/State and cycles.md</code>,</li>
  <li>primitive identity, required ports, and required metadata for <code>frog.core.delay</code> MUST remain aligned with <code>Libraries/Core.md</code>.</li>
</ul>

<p>
Cycle validity and local-memory semantics are defined normatively by <code>Language/State and cycles.md</code>.
The source-facing representation of those constructs in canonical <code>.frog</code> content is defined by
<code>Expression/State and cycles.md</code>.
This document defines only their graph-level participation.
</p>

<hr/>

<h2 id="graph-validation">20. Graph Validation</h2>

<p>
A diagram is valid only if all of the following hold:
</p>

<ul>
  <li>every node identifier is unique within its diagram scope,</li>
  <li>every edge identifier is unique within its diagram scope,</li>
  <li>every annotation identifier is unique within its diagram scope when annotation identifiers are present,</li>
  <li>every node kind is valid,</li>
  <li>every kind-specific required field is present,</li>
  <li>every referenced interface port exists,</li>
  <li>every referenced widget exists,</li>
  <li>every referenced dependency exists,</li>
  <li>every source and destination port resolves successfully,</li>
  <li>every connected value edge is type-compatible,</li>
  <li>every input port has at most one incoming edge unless explicitly allowed by another specification,</li>
  <li>every structure boundary is valid under the control-structure specifications,</li>
  <li>every widget interaction node is valid under the widget interaction and UI library specifications,</li>
  <li>every feedback cycle satisfies the local-memory source rules defined by <code>Expression/State and cycles.md</code>,</li>
  <li>layout, documentation, tags, and annotations do not conflict with executable fields.</li>
</ul>

<h3>20.1 Interface participation checks</h3>

<ul>
  <li>Each declared interface input MUST be consumable by the graph.</li>
  <li>Each declared interface output MUST be producible by the graph.</li>
  <li>Canonical source SHOULD represent each declared public interface port through a dedicated matching boundary node.</li>
</ul>

<h3>20.2 Widget participation checks</h3>

<ul>
  <li>A <code>widget_value</code> node MUST reference a value-carrying widget.</li>
  <li>A <code>widget_reference</code> node MUST reference an existing widget.</li>
  <li>Object-style widget interaction MUST use <code>widget_reference</code> plus a valid <code>frog.ui.*</code> primitive.</li>
</ul>

<h3>20.3 Recommended diagnostics</h3>

<p>
Validators SHOULD diagnose at least the following error classes:
</p>

<ul>
  <li>unknown node kind,</li>
  <li>duplicate node identifier,</li>
  <li>duplicate edge identifier,</li>
  <li>unknown port,</li>
  <li>output-to-output connection,</li>
  <li>input-to-input connection,</li>
  <li>type incompatibility,</li>
  <li>unknown interface port,</li>
  <li>unknown widget,</li>
  <li>unknown dependency reference,</li>
  <li>invalid widget-value use on a non-value-carrying widget,</li>
  <li>invalid feedback cycle without local memory,</li>
  <li>missing mandatory <code>initial</code> on <code>frog.core.delay</code>.</li>
</ul>

<p>
These checks validate the graph-level source form of the artifact.
They do not, by themselves, redefine top-level <code>spec_version</code> policy or repository-wide corpus-version governance.
</p>

<hr/>

<h2 id="relation-with-execution-semantics">21. Relation with Execution Semantics</h2>

<h3>21.1 General principle</h3>

<p>
A validated diagram is the graph-level source form consumed by execution-facing layers.
This document defines that source form.
It does not define the full runtime behavior of every graph element.
</p>

<h3>21.2 Interface participation</h3>

<p>
An <code>interface_input</code> node is the canonical graph-level representation of a public input entering the internal graph.
An <code>interface_output</code> node is the canonical graph-level representation of a public output driven from the internal graph.
</p>

<h3>21.3 Widget participation</h3>

<p>
The natural widget path and the object-style widget path are distinct:
</p>

<ul>
  <li><code>widget_value</code> is the canonical natural representation for primary widget value participation,</li>
  <li><code>widget_reference</code> plus <code>frog.ui.*</code> primitives are the canonical object-style representation.</li>
</ul>

<p>
A widget primary value MAY be accessible through both mechanisms when the widget class allows it,
but tools SHOULD preserve the distinction rather than collapsing both forms into one undifferentiated mechanism.
</p>

<h3>21.4 Structure participation</h3>

<p>
Structure nodes participate in the diagram as explicit graph objects with boundaries, terminals, and owned regions.
Their runtime execution semantics are owned by <code>Language/Control structures.md</code>.
</p>

<h3>21.5 Local-memory participation</h3>

<p>
Local-memory nodes participate in the diagram as explicit primitive nodes in feedback-capable graph paths.
Their source-facing representation is owned by <code>Expression/State and cycles.md</code>.
Their normative execution semantics are owned by <code>Language/State and cycles.md</code>.
Primitive-local delayed-value behavior for <code>frog.core.delay</code> is owned by <code>Libraries/Core.md</code>.
</p>

<h3>21.6 No semantic ownership in this document</h3>

<p>
This document MUST NOT be interpreted as redefining:
</p>

<ul>
  <li>general execution readiness rules,</li>
  <li>structure branch-selection or loop-iteration semantics,</li>
  <li>primitive-local runtime behavior,</li>
  <li>cycle-validity semantics,</li>
  <li>runtime scheduling policy,</li>
  <li>repository-wide version-transition law.</li>
</ul>

<hr/>

<h2 id="examples">22. Examples</h2>

<h3>22.1 Basic public interface graph</h3>

<pre><code>{
  "interface": {
    "inputs": [
      { "id": "a", "type": "f64" },
      { "id": "b", "type": "f64" }
    ],
    "outputs": [
      { "id": "result", "type": "f64" }
    ]
  },
  "diagram": {
    "nodes": [
      {
        "id": "input_a",
        "kind": "interface_input",
        "interface_port": "a"
      },
      {
        "id": "input_b",
        "kind": "interface_input",
        "interface_port": "b"
      },
      {
        "id": "add_node",
        "kind": "primitive",
        "type": "frog.core.add"
      },
      {
        "id": "output_result",
        "kind": "interface_output",
        "interface_port": "result"
      }
    ],
    "edges": [
      {
        "id": "e1",
        "from": { "node": "input_a", "port": "value" },
        "to":   { "node": "add_node", "port": "a" }
      },
      {
        "id": "e2",
        "from": { "node": "input_b", "port": "value" },
        "to":   { "node": "add_node", "port": "b" }
      },
      {
        "id": "e3",
        "from": { "node": "add_node", "port": "result" },
        "to":   { "node": "output_result", "port": "value" }
      }
    ]
  }
}</code></pre>

<h3>22.2 Natural widget-value participation</h3>

<pre><code>{
  "front_panel": {
    "widgets": [
      {
        "id": "ctrl_gain",
        "role": "control",
        "widget": "frog.ui.standard.numeric_control",
        "value_type": "f64"
      },
      {
        "id": "ind_result",
        "role": "indicator",
        "widget": "frog.ui.standard.numeric_indicator",
        "value_type": "f64"
      }
    ]
  },
  "diagram": {
    "nodes": [
      {
        "id": "ctrl_gain_value",
        "kind": "widget_value",
        "widget": "ctrl_gain"
      },
      {
        "id": "input_signal",
        "kind": "interface_input",
        "interface_port": "signal"
      },
      {
        "id": "mul_1",
        "kind": "primitive",
        "type": "frog.core.mul"
      },
      {
        "id": "ind_result_value",
        "kind": "widget_value",
        "widget": "ind_result"
      }
    ],
    "edges": [
      {
        "id": "e1",
        "from": { "node": "input_signal", "port": "value" },
        "to":   { "node": "mul_1", "port": "a" }
      },
      {
        "id": "e2",
        "from": { "node": "ctrl_gain_value", "port": "value" },
        "to":   { "node": "mul_1", "port": "b" }
      },
      {
        "id": "e3",
        "from": { "node": "mul_1", "port": "result" },
        "to":   { "node": "ind_result_value", "port": "value" }
      }
    ]
  }
}</code></pre>

<h3>22.3 Object-style widget property read</h3>

<pre><code>{
  "diagram": {
    "nodes": [
      {
        "id": "ctrl_gain_ref",
        "kind": "widget_reference",
        "widget": "ctrl_gain"
      },
      {
        "id": "read_label_text",
        "kind": "primitive",
        "type": "frog.ui.property_read",
        "widget_member": {
          "part": "label",
          "member": "text"
        }
      }
    ],
    "edges": [
      {
        "id": "e1",
        "from": { "node": "ctrl_gain_ref", "port": "ref" },
        "to":   { "node": "read_label_text", "port": "ref" }
      }
    ]
  }
}</code></pre>

<h3>22.4 Object-style widget property write with public input</h3>

<pre><code>{
  "diagram": {
    "nodes": [
      {
        "id": "status_text",
        "kind": "interface_input",
        "interface_port": "status"
      },
      {
        "id": "ctrl_gain_ref",
        "kind": "widget_reference",
        "widget": "ctrl_gain"
      },
      {
        "id": "write_label_text",
        "kind": "primitive",
        "type": "frog.ui.property_write",
        "widget_member": {
          "part": "label",
          "member": "text"
        }
      }
    ],
    "edges": [
      {
        "id": "e1",
        "from": { "node": "status_text", "port": "value" },
        "to":   { "node": "write_label_text", "port": "value" }
      },
      {
        "id": "e2",
        "from": { "node": "ctrl_gain_ref", "port": "ref" },
        "to":   { "node": "write_label_text", "port": "ref" }
      }
    ]
  }
}</code></pre>

<h3>22.5 Valid feedback cycle with local memory</h3>

<pre><code>{
  "diagram": {
    "nodes": [
      {
        "id": "input_x",
        "kind": "interface_input",
        "interface_port": "x"
      },
      {
        "id": "add_1",
        "kind": "primitive",
        "type": "frog.core.add"
      },
      {
        "id": "delay_1",
        "kind": "primitive",
        "type": "frog.core.delay",
        "initial": 0.0
      },
      {
        "id": "output_y",
        "kind": "interface_output",
        "interface_port": "y"
      }
    ],
    "edges": [
      {
        "id": "e1",
        "from": { "node": "input_x", "port": "value" },
        "to":   { "node": "add_1", "port": "a" }
      },
      {
        "id": "e2",
        "from": { "node": "delay_1", "port": "out" },
        "to":   { "node": "add_1", "port": "b" }
      },
      {
        "id": "e3",
        "from": { "node": "add_1", "port": "result" },
        "to":   { "node": "delay_1", "port": "in" }
      },
      {
        "id": "e4",
        "from": { "node": "add_1", "port": "result" },
        "to":   { "node": "output_y", "port": "value" }
      }
    ]
  }
}</code></pre>

<h3>22.6 Boolean case structure</h3>

<pre><code>{
  "diagram": {
    "nodes": [
      {
        "id": "input_enabled",
        "kind": "interface_input",
        "interface_port": "enabled"
      },
      {
        "id": "case_enabled",
        "kind": "structure",
        "structure_type": "case",
        "boundary": {
          "inputs": [],
          "outputs": [
            { "id": "result", "type": "f64" }
          ]
        },
        "structure_terminals": {
          "selector": {
            "type": "bool",
            "outer_visible": true,
            "exposed_in_body": false,
            "read_only": true,
            "role": "selector"
          }
        },
        "regions": [
          {
            "id": "true_case",
            "match": true,
            "diagram": {
              "nodes": [],
              "edges": []
            }
          },
          {
            "id": "false_case",
            "match": false,
            "diagram": {
              "nodes": [],
              "edges": []
            }
          }
        ]
      }
    ],
    "edges": [
      {
        "id": "e1",
        "from": { "node": "input_enabled", "port": "value" },
        "to":   { "node": "case_enabled", "port": "selector" }
      }
    ]
  }
}</code></pre>

<p>
This illustrates the canonical source-level replacement for a traditional <code>if / else</code> structure.
</p>

<hr/>

<h2 id="summary">23. Summary</h2>

<p>
The FROG <code>diagram</code> is the authoritative executable graph of a FROG in canonical source form.
It is the canonical place where public interface participation, widget participation, structure participation,
sub-FROG invocation, explicit local memory, and graph-level documentation are represented.
</p>

<ul>
  <li>Public interface participation is represented canonically by <code>interface_input</code> and <code>interface_output</code>.</li>
  <li>Natural widget value participation is represented canonically by <code>widget_value</code>.</li>
  <li>Object-style widget access is represented canonically by <code>widget_reference</code> plus <code>frog.ui.*</code> primitives.</li>
  <li>Language structures are explicit graph nodes, not hidden function expansions.</li>
  <li>Cycles and explicit local memory participate directly in the diagram graph.</li>
  <li>The diagram owns graph structure representation; annotations and layout do not change execution semantics.</li>
  <li>Diagram nodes, edges, regions, and annotations may be subjects of source provenance attestations, but provenance does not change graph meaning.</li>
  <li>The diagram does not define source-format compatibility law or published specification corpus versioning.</li>
</ul>

<p>
This keeps FROG understandable as a graphical dataflow language while remaining explicit, canonical, and durable as a long-term language specification.
</p>
