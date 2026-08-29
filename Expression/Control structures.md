<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Control Structures Specification</h1>

<p align="center">
  <strong>Source-facing representation of control structures in FROG programs</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#goals">2. Goals</a></li>
  <li><a href="#relation-with-other-specifications">3. Relation with Other Specifications</a></li>
  <li><a href="#functions-vs-control-structures">4. Functions vs Control Structures</a></li>
  <li><a href="#scope-for-v01">5. Scope for v0.1</a></li>
  <li><a href="#standard-control-structures-for-v01">6. Standard Control Structures for v0.1</a></li>
  <li><a href="#canonical-structure-node-model">7. Canonical Structure Node Model</a></li>
  <li><a href="#boundary-model">8. Boundary Model</a></li>
  <li><a href="#structure-terminals">9. Structure Terminals</a></li>
  <li><a href="#regions">10. Regions</a></li>
  <li><a href="#case-structure">11. Case Structure</a></li>
  <li><a href="#for-loop-structure">12. For Loop Structure</a></li>
  <li><a href="#while-loop-structure">13. While Loop Structure</a></li>
  <li><a href="#event-structure">14. Event Structure</a></li>
  <li><a href="#source-to-language-alignment">15. Source-to-Language Alignment</a></li>
  <li><a href="#interaction-with-local-memory-and-cycles">16. Interaction with Local Memory and Cycles</a></li>
  <li><a href="#diagram-representation">17. Diagram Representation</a></li>
  <li><a href="#validation-rules">18. Validation Rules</a></li>
  <li><a href="#examples">19. Examples</a></li>
  <li><a href="#out-of-scope-for-v01">20. Out of Scope for v0.1</a></li>
  <li><a href="#summary">21. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
FROG is a graphical dataflow language, but not every program behavior should be represented as an ordinary function call.
Some behaviors are inherently structural: they select one executable region among several, or they repeat a region according to explicit structural rules.
These behaviors are represented by control structures.
</p>

<p>
A control structure is not merely a function with inputs and outputs.
In canonical source, it is a structural region of the diagram with:
</p>

<ul>
  <li>an explicit structure kind,</li>
  <li>an explicit structure family,</li>
  <li>an explicit boundary,</li>
  <li>explicit structure terminals when required,</li>
  <li>one or more owned executable regions,</li>
  <li>a canonical source representation aligned with standardized execution semantics.</li>
</ul>

<p>
FROG v0.1 keeps concrete control forms explicit.
Therefore, the language standardizes <code>case</code>, <code>for_loop</code>, <code>while_loop</code>, <code>event_structure</code>, <code>disabled_structure</code>, and <code>conditional_disable_structure</code> as distinct visible structures rather than collapsing them into one generic hidden form.
</p>

<p>
In FROG, boolean conditional branching and multi-branch textual selection are both represented canonically by the same <code>case</code> structure family.
A boolean <code>case</code> is the canonical source-level equivalent of a traditional <code>if / else</code>.
There is no separate canonical <code>if</code> structure in base v0.1.
</p>

<hr/>

<h2 id="goals">2. Goals</h2>

<ul>
  <li><strong>Clarity</strong> — distinguish ordinary computation from language-level structural control.</li>
  <li><strong>Canonical source stability</strong> — provide one durable source shape for structure nodes.</li>
  <li><strong>Readability</strong> — keep branch selection and loop intent visually explicit.</li>
  <li><strong>Structural explicitness</strong> — make structure boundaries, terminals, and regions first-class in canonical source.</li>
  <li><strong>Extensibility</strong> — allow future structure families without redefining the foundations.</li>
  <li><strong>Compatibility with graphical practice</strong> — remain understandable to users familiar with established graphical dataflow environments.</li>
  <li><strong>Executable-slice closure</strong> — provide a source form precise enough to support one complete published corridor from <code>.frog</code> source to derivation, lowering, backend handoff, and first runtime execution.</li>
</ul>

<hr/>

<h2 id="relation-with-other-specifications">3. Relation with Other Specifications</h2>

<p>
This document complements the following specifications:
</p>

<ul>
  <li><code>Expression/Readme.md</code> — defines the role of <code>Expression/</code> as the canonical source-specification layer.</li>
  <li><code>Language/Readme.md</code> — defines the role of <code>Language/</code> as the normative execution-semantics layer.</li>
  <li><code>Language/Control structures.md</code> — defines the normative execution semantics of control structures.</li>
  <li><code>Expression/Diagram.md</code> — defines the executable graph, diagram scopes, node kinds, and structure-node placement in diagrams.</li>
  <li><code>Expression/Type.md</code> — defines type syntax and compatibility.</li>
  <li><code>Expression/State and cycles.md</code> — defines the source-facing representation of explicit local memory and feedback-cycle formation constraints.</li>
  <li><code>Language/State and cycles.md</code> — defines the normative validity rule for cyclic graphs and local memory.</li>
  <li><code>Libraries/Core.md</code> — defines ordinary built-in functions such as <code>frog.core.add</code> and <code>frog.core.delay</code>.</li>
  <li><code>IDE/Readme.md</code> — defines the Program Model used during authoring and its relation to canonical source.</li>
</ul>

<p>
This document defines the canonical source-level representation of control structures.
It does not redefine ordinary function libraries, and it does not redefine the general diagram node model already defined in <code>Expression/Diagram.md</code>.
</p>

<p>
Accordingly:
</p>

<ul>
  <li><code>Expression/Control structures.md</code> owns the canonical source representation of structure nodes, boundaries, terminals, and regions,</li>
  <li><code>Language/Control structures.md</code> owns the normative execution meaning of those structures,</li>
  <li><code>IDE/</code> MAY expose richer authoring forms, but MUST serialize them back into canonical source structures.</li>
</ul>

<p>
When both documents discuss the same structure family, they MUST remain aligned.
However, ownership remains separated between source representation and execution semantics.
</p>

<p>
When a conflict exists between source shape and execution meaning:
</p>

<ul>
  <li><code>Expression/</code> remains authoritative for canonical source form,</li>
  <li><code>Language/</code> remains authoritative for normative execution semantics.</li>
</ul>

<hr/>

<h2 id="functions-vs-control-structures">4. Functions vs Control Structures</h2>

<p>
FROG distinguishes between:
</p>

<ul>
  <li><strong>functions</strong> — callable operations such as <code>frog.core.add</code> or <code>frog.core.delay</code>,</li>
  <li><strong>control structures</strong> — structural regions such as <code>case</code>, <code>for_loop</code>, <code>while_loop</code>, <code>event_structure</code>, <code>disabled_structure</code>, and <code>conditional_disable_structure</code>.</li>
</ul>

<p>
A function is evaluated according to its port signature and function semantics.
A control structure owns one or more internal executable regions and defines a dedicated source form for structural control.
</p>

<p>
Example:
</p>

<ul>
  <li><code>frog.core.select</code> is a function that selects one value among inputs,</li>
  <li>a <code>case</code> structure selects one executable region among multiple branches.</li>
</ul>

<p>
This distinction is fundamental.
A control structure is part of the language model itself and has a dedicated canonical source representation.
</p>

<hr/>

<h2 id="scope-for-v01">5. Scope for v0.1</h2>

<p>
FROG v0.1 standardizes the following control structures:
</p>

<ul>
  <li><code>case</code></li>
  <li><code>for_loop</code></li>
  <li><code>while_loop</code></li>
  <li><code>event_structure</code></li>
  <li><code>disabled_structure</code></li>
  <li><code>conditional_disable_structure</code></li>
</ul>

<p>
FROG v0.1 does not standardize general timed-loop structures, parallel-region structures, pattern-match structures, or exception-handling structures.
Those MAY be introduced later as their own structure families.
</p>

<hr/>

<h2 id="standard-control-structures-for-v01">6. Standard Control Structures for v0.1</h2>

<p>
The standard structure families for FROG v0.1 are:
</p>

<ul>
  <li><code>case</code></li>
  <li><code>for_loop</code></li>
  <li><code>while_loop</code></li>
  <li><code>event_structure</code></li>
  <li><code>disabled_structure</code></li>
  <li><code>conditional_disable_structure</code></li>
</ul>

<p>
These are language structures, not library functions.
Tools SHOULD present them as dedicated structural elements in the diagram editor rather than as ordinary primitive nodes.
</p>

<p>
For usability, tools MAY present a boolean <code>case</code> as <em>If / Else</em> in the UI.
More generally, tools MAY expose authoring-facing derived forms such as <em>If</em>, <em>If / Else</em>, <em>Else If</em>, or <em>Switch</em>, provided that those forms normalize to the canonical structure family defined in source.
However, the canonical source structure family remains <code>case</code>.
</p>

<p>
Accordingly:
</p>

<ul>
  <li>canonical source MUST serialize standardized structure families such as <code>case</code>, <code>for_loop</code>, <code>while_loop</code>, <code>event_structure</code>, <code>disabled_structure</code>, and <code>conditional_disable_structure</code>,</li>
  <li>canonical source MUST NOT introduce separate structure families such as <code>if</code>, <code>if_else</code>, <code>else_if</code>, or <code>switch</code> in base v0.1,</li>
  <li>tooling MAY preserve authoring intent in editor metadata, but MUST preserve canonical structural meaning when serializing source.</li>
</ul>

<hr/>

<h2 id="canonical-structure-node-model">7. Canonical Structure Node Model</h2>

<p>
Every control structure in canonical source MUST use the following general model:
</p>

<pre><code>{
  "id": "struct_1",
  "kind": "structure",
  "structure_type": "case",
  "boundary": {
    "inputs": [],
    "outputs": []
  },
  "structure_terminals": {},
  "regions": [],
  "layout": {},
  "doc": {},
  "tags": []
}</code></pre>

<p>
Field meaning:
</p>

<ul>
  <li><code>id</code> — unique structure node identifier within the owning diagram scope,</li>
  <li><code>kind</code> — MUST be <code>"structure"</code>,</li>
  <li><code>structure_type</code> — MUST be a valid structure family such as <code>case</code>, <code>for_loop</code>, <code>while_loop</code>, <code>event_structure</code>, <code>disabled_structure</code>, or <code>conditional_disable_structure</code>,</li>
  <li><code>boundary</code> — explicit structure inputs and outputs crossing the structure wall,</li>
  <li><code>structure_terminals</code> — structure-specific terminals such as selector, count, condition, or index,</li>
  <li><code>regions</code> — owned executable regions,</li>
  <li><code>layout</code> — optional editor geometry,</li>
  <li><code>doc</code> — optional structured documentation,</li>
  <li><code>tags</code> — optional structured tags.</li>
</ul>

<p>
The machine-checkable public node contract is published as
<code>Expression/schema/frog.structure-node.schema.json</code>. It validates the
four standardized structure families recursively without attempting to replace
the normative prose in this document.
</p>

<p>
Rules:
</p>

<ul>
  <li><code>id</code> MUST exist.</li>
  <li><code>kind</code> MUST exist and MUST equal <code>"structure"</code>.</li>
  <li><code>structure_type</code> MUST exist.</li>
  <li><code>boundary</code> MUST exist and MUST be valid for the selected structure family.</li>
  <li><code>structure_terminals</code> MUST exist and MUST be valid for the selected structure family.</li>
  <li><code>regions</code> MUST exist and MUST be valid for the selected structure family.</li>
</ul>

<hr/>

<h2 id="boundary-model">8. Boundary Model</h2>

<p>
The <code>boundary</code> object defines the values that cross the structure wall.
Conceptually, these crossings behave like explicit tunnels.
</p>

<pre><code>"boundary": {
  "inputs": [
    { "id": "x", "type": "u16" }
  ],
  "outputs": [
    { "id": "y", "type": "u16" }
  ]
}</code></pre>

<h3>8.1 Boundary inputs</h3>

<p>
A boundary input carries a value from the outer graph into the structure.
The exact structure-family-specific interpretation of that crossing is defined normatively by <code>Language/Control structures.md</code>.
</p>

<ul>
  <li><code>boundary.inputs</code> MUST exist and MUST be an array.</li>
  <li>Each input entry MUST define <code>id</code> and <code>type</code>.</li>
  <li>Boundary input identifiers MUST be unique within the structure boundary.</li>
</ul>

<h3>8.2 Boundary outputs</h3>

<p>
A boundary output carries a value from the structure back to the outer graph.
The exact production rule for that output is defined normatively by <code>Language/Control structures.md</code>.
</p>

<ul>
  <li><code>boundary.outputs</code> MUST exist and MUST be an array.</li>
  <li>Each output entry MUST define <code>id</code> and <code>type</code>.</li>
  <li>Boundary output identifiers MUST be unique within the structure boundary.</li>
</ul>

<h3>8.3 Loop-output metadata</h3>

<p>
For <code>for_loop</code> and <code>while_loop</code>, an output MAY define structure-specific output metadata.
The only standardized loop-output mode in v0.1 is:
</p>

<ul>
  <li><code>mode: "last_value"</code></li>
</ul>

<p>
Example:
</p>

<pre><code>{ "id": "sum_out", "type": "u16", "mode": "last_value", "zero_iteration_value": 0 }</code></pre>

<p>
Source-shape rules for <code>mode: "last_value"</code>:
</p>

<ul>
  <li>the output entry MAY define <code>mode: "last_value"</code>,</li>
  <li>if zero iterations are possible in the active validated profile, the output entry MUST define a type-compatible <code>zero_iteration_value</code>,</li>
  <li>if zero iterations are impossible in the active validated profile, <code>zero_iteration_value</code> MAY be omitted.</li>
</ul>

<p>
Normative interpretation of loop-output modes belongs to <code>Language/Control structures.md</code>.
</p>

<p>
More advanced loop-output modes such as collection, reduction, or profile-defined accumulation are out of scope for base v0.1.
</p>

<h3>8.4 Boundary semantics are explicit</h3>

<p>
A structure boundary is part of canonical executable-graph representation.
The structure wall MUST NOT be treated as a purely visual editor artifact.
</p>

<h3>8.5 Common structure authoring metadata</h3>

<p>
An authoring tool MAY additionally write <code>structure_tunnels</code> to retain
the side and normalized wall position of boundary crossings. That array is
non-semantic placement metadata. When it disagrees with <code>boundary</code>, the
boundary id, type and direction MUST win; tools MUST NOT reinterpret the
executable contract from a stale authoring tunnel record.
</p>

<p>
The public structure-node schema also defines the optional authoring fields used
to preserve visible-case selection, normalized terminal positions, fixed
subdiagram text, background colors, subframe colors and subframe opacity for
the standard families.
Normalized positions range from <code>0</code> to <code>1</code>, colors are packed
24-bit RGB integers (<code>0xRRGGBB</code>) or the transparent sentinel
<code>0xFFFFFFFF</code>, and opacity ranges from
<code>0</code> to <code>100</code>. These fields MUST NOT change execution semantics.
</p>

<ul>
  <li><code>case_structure_*</code> preserves the visible boolean case, selector position, and the separate True/False body and subframe colors,</li>
  <li><code>for_loop_*</code> preserves the movable body index position, For body/subframe colors, and the optional fixed subdiagram label above the body through <code>for_loop_subdiagram_label_visible</code>, <code>for_loop_subdiagram_label_text</code> and the authoring-only clamped <code>for_loop_subdiagram_label_height</code>; the fixed count terminal needs no position field,</li>
  <li><code>while_loop_*</code> preserves the movable condition and index positions and While body/subframe colors,</li>
  <li><code>event_structure_*</code> preserves the visible event-case index, the vertically movable Event Data node, and Event body/subframe colors; the fixed timeout terminal needs no position field,</li>
  <li><code>disabled_structure_*</code> preserves the visible Enabled/Disabled region, body/subframe colors, and the non-semantic veil color and opacity used while authoring the Disabled region.</li>
  <li><code>conditional_disable_*</code> preserves only authoring-visible case selection and presentation colors; compile-time activation is derived from the Compilation Context and MUST NOT be serialized as editor state.</li>
</ul>

<hr/>

<h2 id="structure-terminals">9. Structure Terminals</h2>

<p>
The <code>structure_terminals</code> object contains terminals that are intrinsic to the structure family rather than ordinary boundary tunnels.
</p>

<p>
Examples include:
</p>

<ul>
  <li>a case selector,</li>
  <li>a for-loop count terminal,</li>
  <li>a while-loop continuation terminal,</li>
  <li>an iteration index exposed inside a loop body,</li>
  <li>an event timeout terminal and the typed Event Data field group.</li>
</ul>

<p>
A canonical terminal object SHOULD use the following fields when relevant:
</p>

<pre><code>{
  "type": "bool",
  "outer_visible": true,
  "exposed_in_body": false,
  "read_only": true,
  "role": "selector"
}</code></pre>

<p>
Field meaning:
</p>

<ul>
  <li><code>type</code> — terminal type, required,</li>
  <li><code>outer_visible</code> — whether the terminal appears as an external structure port,</li>
  <li><code>exposed_in_body</code> — whether the terminal is visible inside the owned region body,</li>
  <li><code>read_only</code> — whether body logic may only read the terminal value,</li>
  <li><code>role</code> — optional semantic role label,</li>
  <li><code>outer_port</code> — stable graph port id used on the outer face, when the terminal has one,</li>
  <li><code>body_port</code> — stable graph port id used by the terminal projection inside an owned region, when the terminal has one.</li>
</ul>

<p>
Rules:
</p>

<ul>
  <li>every scalar standardized terminal MUST define <code>type</code>,</li>
  <li>a grouped terminal such as <code>event_data</code> MUST instead define the type of every exposed field,</li>
  <li>terminal meaning is determined by the structure family and owned semantically by <code>Language/</code>,</li>
  <li>terminals MUST NOT be confused with ordinary boundary values.</li>
</ul>

<p>
This document standardizes terminal availability and source representation.
It does not require v0.1 to freeze one universal low-level nested-region reference encoding for consuming those terminals inside region-local diagrams.
</p>

<hr/>

<h2 id="regions">10. Regions</h2>

<p>
The <code>regions</code> array contains the executable regions owned by the structure.
Each region contains a local diagram scope.
</p>

<p>
General region shape:
</p>

<pre><code>{
  "id": "body",
  "diagram": {
    "nodes": [],
    "edges": []
  }
}</code></pre>

<p>
Rules:
</p>

<ul>
  <li><code>regions</code> MUST be an array.</li>
  <li>Each region MUST define <code>id</code>.</li>
  <li>Each region MUST define <code>diagram</code>.</li>
  <li>Each region identifier MUST be unique within the owning structure.</li>
  <li>Each region-local diagram MUST itself be a valid FROG diagram scope.</li>
</ul>

<p>
A structure MAY own one region or multiple regions depending on its family:
</p>

<ul>
  <li>a <code>case</code> structure owns multiple branch regions,</li>
  <li>a <code>for_loop</code> owns one body region,</li>
  <li>a <code>while_loop</code> owns one body region,</li>
  <li>an <code>event_structure</code> owns one or more event-case regions,</li>
  <li>a <code>disabled_structure</code> owns exactly one Enabled region and exactly one Disabled region.</li>
  <li>a <code>conditional_disable_structure</code> owns an ordered list of conditioned regions followed by exactly one Default region.</li>
</ul>

<p>
This document standardizes the canonical source shape of regions.
The rules that determine region participation belong to <code>Language/Control structures.md</code>.
</p>

<hr/>

<h2 id="case-structure">11. Case Structure</h2>

<p>
A <code>case</code> structure selects exactly one executable branch among several internal regions.
This document standardizes how that structure is represented in canonical source.
</p>

<h3>11.1 Purpose</h3>

<p>
A case structure is used when different executable regions must be represented under one structural selection form driven by a selector value.
</p>

<p>
In base v0.1, the standardized selector categories are:
</p>

<ul>
  <li><code>bool</code></li>
  <li><code>string</code></li>
</ul>

<p>
This means that a <code>case</code> structure in v0.1 supports:
</p>

<ul>
  <li>boolean conditional structure selection,</li>
  <li>string-based branch selection.</li>
</ul>

<p>
At the authoring level, tooling MAY expose UI-facing derived forms such as <em>If</em>, <em>If / Else</em>, <em>Else If</em>, or <em>Switch</em>.
Those presentations do not create distinct canonical structure families in source.
They normalize to the canonical <code>case</code> family defined here.
</p>

<h3>11.2 Canonical selector terminal</h3>

<p>
A canonical <code>case</code> MUST define one selector terminal named <code>selector</code>.
</p>

<pre><code>"structure_terminals": {
  "selector": {
    "type": "bool",
    "outer_visible": true,
    "exposed_in_body": true,
    "read_only": true,
    "outer_port": "selector",
    "body_port": "selected_case",
    "role": "selector"
  }
}</code></pre>

<p>
Rules:
</p>

<ul>
  <li><code>selector</code> MUST exist.</li>
  <li><code>selector.type</code> MUST be either <code>bool</code> or <code>string</code> in base v0.1.</li>
  <li><code>selector.outer_visible</code> MUST be <code>true</code>.</li>
  <li><code>selector.exposed_in_body</code> MUST be <code>true</code> so the selected value has a body-side projection.</li>
  <li><code>selector.read_only</code> SHOULD be <code>true</code>.</li>
  <li><code>selector.outer_port</code> MUST be <code>selector</code>.</li>
  <li><code>selector.body_port</code> MUST be <code>selected_case</code>.</li>
</ul>

<h3>11.3 Boolean case</h3>

<p>
A boolean case is the canonical source-level equivalent of a classic <code>if / else</code>.
</p>

<p>
If <code>selector.type</code> is <code>bool</code>:
</p>

<ul>
  <li>the structure MUST define exactly two executable regions,</li>
  <li>exactly one region MUST have <code>match: true</code>,</li>
  <li>exactly one region MUST have <code>match: false</code>,</li>
  <li>no default region is required or expected.</li>
</ul>

<p>
Tools MAY present this form as an <em>If / Else</em> structure in the UI.
The canonical source remains <code>case</code>.
</p>

<h3>11.4 String case</h3>

<p>
If <code>selector.type</code> is <code>string</code>, the structure represents multi-branch selection using string literals.
</p>

<p>
Rules:
</p>

<ul>
  <li>one or more regions MAY define a string <code>match</code> value,</li>
  <li>every explicit string <code>match</code> value MUST be unique within the structure,</li>
  <li>a string case MUST define exactly one default region,</li>
  <li>the default region MUST set <code>default: true</code>.</li>
</ul>

<p>
Canonical shape:
</p>

<pre><code>"regions": [
  {
    "id": "case_start",
    "match": "start",
    "diagram": {
      "nodes": [],
      "edges": []
    }
  },
  {
    "id": "case_stop",
    "match": "stop",
    "diagram": {
      "nodes": [],
      "edges": []
    }
  },
  {
    "id": "default_case",
    "default": true,
    "diagram": {
      "nodes": [],
      "edges": []
    }
  }
]</code></pre>

<p>
Normative branch-selection behavior is defined by <code>Language/Control structures.md</code>.
This document only fixes the canonical source form used to express that behavior.
</p>

<hr/>

<h2 id="for-loop-structure">12. For Loop Structure</h2>

<p>
A <code>for_loop</code> represents a counted-loop structure with one owned body region.
This document standardizes how that structure is represented in canonical source.
</p>

<h3>12.1 Purpose</h3>

<p>
A for loop is used when the loop form is expressed through an explicit count terminal and a single body region.
The source form is intended to support explicit loop-carried computation and explicit local memory inside the body when needed.
</p>

<h3>12.2 Canonical terminals</h3>

<pre><code>"structure_terminals": {
  "count": {
    "type": "i64",
    "outer_visible": true,
    "exposed_in_body": true,
    "read_only": true,
    "outer_port": "count",
    "body_port": "count_value",
    "role": "count"
  },
  "index": {
    "type": "i64",
    "outer_visible": false,
    "exposed_in_body": true,
    "read_only": true,
    "body_port": "iteration",
    "role": "index"
  }
}</code></pre>

<p>
Rules:
</p>

<ul>
  <li><code>count</code> MUST exist.</li>
  <li><code>count.type</code> MUST be <code>i64</code> in base v0.1.</li>
  <li><code>count.outer_visible</code> MUST be <code>true</code>.</li>
  <li><code>count.exposed_in_body</code> MUST be <code>true</code>.</li>
  <li><code>count.outer_port</code> MUST be <code>count</code> and <code>count.body_port</code> MUST be <code>count_value</code>.</li>
  <li><code>index</code> MUST exist.</li>
  <li><code>index.type</code> MUST be <code>i64</code> in base v0.1.</li>
  <li><code>index.exposed_in_body</code> MUST be <code>true</code>.</li>
  <li><code>index.read_only</code> SHOULD be <code>true</code>.</li>
  <li><code>index.body_port</code> MUST be <code>iteration</code>.</li>
</ul>

<h3>12.3 Count representation</h3>

<p>
The canonical <code>for_loop</code> source model represents counted iteration through terminal <code>count</code>.
The resolved count value is provided from the outer graph through the terminal exposed at the structure boundary.
The normative interpretation of the resolved count value belongs to <code>Language/Control structures.md</code>.
</p>

<h3>12.4 Negative counts</h3>

<p>
A canonical source representation MUST be compatible with the rule that negative iteration counts are invalid in base v0.1.
Normative rejection behavior belongs to <code>Language/Control structures.md</code> and to implementation validation.
</p>

<h3>12.5 Zero iterations</h3>

<p>
If zero iterations are possible under the active validated profile, every loop output MUST still have a complete source representation.
If a loop output uses <code>mode: "last_value"</code>, it MUST define a valid <code>zero_iteration_value</code>.
</p>

<h3>12.6 Boundary model for loop-carried values</h3>

<p>
A <code>for_loop</code> MAY use ordinary boundary inputs and outputs to express values that enter or leave the structure.
When a loop computes a final accumulated result, the canonical source representation SHOULD use a boundary output with <code>mode: "last_value"</code>.
</p>

<p>
This source model intentionally does not invent hidden source-level accumulator syntax.
If accumulation requires explicit state, that state MUST still be represented explicitly inside the body region, typically through <code>frog.core.delay</code> in base v0.1.
</p>

<h3>12.7 Regions</h3>

<p>
A canonical <code>for_loop</code> MUST define exactly one region with <code>id: "body"</code>.
</p>

<pre><code>"regions": [
  {
    "id": "body",
    "diagram": {
      "nodes": [],
      "edges": []
    }
  }
]</code></pre>

<h3>12.8 Distinction from ordinary functions</h3>

<p>
A for loop is not reducible to one ordinary function call.
It governs repeated execution of a structural region and therefore requires its own canonical source form.
</p>

<h3>12.9 First executable-slice posture</h3>

<p>
For the first complete executable slice, a conforming source MAY use a <code>for_loop</code> with:
</p>

<ul>
  <li>one boundary input carrying the loop-invariant external value,</li>
  <li>one external count terminal resolved to a constant such as <code>5</code>,</li>
  <li>one explicit local-memory element inside the body region initialized to <code>0</code>,</li>
  <li>one boundary output using <code>mode: "last_value"</code> to expose the final accumulated value.</li>
</ul>

<p>
This keeps loop structure, state, and final result explicit without introducing profile-specific hidden accumulation syntax.
</p>

<hr/>

<h2 id="while-loop-structure">13. While Loop Structure</h2>

<p>
A <code>while_loop</code> represents a condition-governed loop structure with one owned body region.
This document standardizes how that structure is represented in canonical source.
</p>

<h3>13.1 Purpose</h3>

<p>
A while loop is used when repetition is expressed through a dedicated continuation terminal rather than a dedicated count terminal.
</p>

<h3>13.2 Canonical v0.1 loop form</h3>

<p>
The canonical v0.1 source model of <code>while_loop</code> is aligned with the standardized continue-while-true semantic family defined by <code>Language/Control structures.md</code>.
</p>

<p>
In source terms, this alignment is reflected by:
</p>

<ul>
  <li>a body-exposed boolean <code>condition</code> terminal,</li>
  <li>no distinct standardized pre-test <code>while_loop</code> source form in base v0.1,</li>
  <li>a single owned region with <code>id: "body"</code>.</li>
</ul>

<p>
The runtime meaning of that loop family belongs to <code>Language/Control structures.md</code>.
</p>

<h3>13.3 Canonical terminals</h3>

<pre><code>"structure_terminals": {
  "condition": {
    "type": "bool",
    "outer_visible": false,
    "exposed_in_body": true,
    "read_only": false,
    "body_port": "condition",
    "role": "continue_while_true"
  },
  "index": {
    "type": "i64",
    "outer_visible": false,
    "exposed_in_body": true,
    "read_only": true,
    "body_port": "iteration",
    "role": "index"
  }
}</code></pre>

<p>
Rules:
</p>

<ul>
  <li><code>condition</code> MUST exist.</li>
  <li><code>condition.type</code> MUST be <code>bool</code>.</li>
  <li><code>condition.exposed_in_body</code> MUST be <code>true</code>.</li>
  <li><code>condition.outer_visible</code> SHOULD be <code>false</code> in base v0.1.</li>
  <li><code>condition.body_port</code> MUST be <code>condition</code>.</li>
  <li><code>index</code> MUST exist.</li>
  <li><code>index.type</code> MUST be <code>i64</code> in base v0.1.</li>
  <li><code>index.exposed_in_body</code> MUST be <code>true</code>.</li>
  <li><code>index.body_port</code> MUST be <code>iteration</code>.</li>
</ul>

<h3>13.4 Continuation representation</h3>

<p>
The canonical source model represents continuation through terminal <code>condition</code>.
The normative rule that decides whether another iteration occurs belongs to <code>Language/Control structures.md</code>.
</p>

<h3>13.5 Regions</h3>

<p>
A canonical <code>while_loop</code> MUST define exactly one region with <code>id: "body"</code>.
</p>

<pre><code>"regions": [
  {
    "id": "body",
    "diagram": {
      "nodes": [],
      "edges": []
    }
  }
]</code></pre>

<h3>13.6 Loop outputs</h3>

<p>
Loop outputs MUST have a complete and unambiguous source representation after termination.
If a standardized output mode such as <code>last_value</code> is used, its source-level fields MUST be present as required.
Normative output interpretation belongs to <code>Language/Control structures.md</code>.
</p>

<h3>13.7 Distinction from for loop</h3>

<p>
A while loop is source-distinct from a for loop.
A for loop communicates counted repetition.
A while loop communicates condition-governed repetition.
Both remain distinct structure families in canonical v0.1 source.
</p>

<hr/>

<h2 id="event-structure">14. Event Structure</h2>

<p>
An <code>event_structure</code> waits for one of its declared events and executes
exactly one matching event-case region. Its timeout terminal, event descriptors,
typed Event Data fields, boundary and owned regions are public canonical
<code>.frog</code> source; the currently visible case and visual placement remain
non-semantic authoring state.
</p>

<h3>14.1 Canonical terminals</h3>

<pre><code>"structure_terminals": {
  "timeout": {
    "type": "i64",
    "outer_visible": true,
    "exposed_in_body": true,
    "read_only": true,
    "outer_port": "timeout",
    "body_port": "timeout_value",
    "role": "timeout_milliseconds"
  },
  "event_data": {
    "outer_visible": false,
    "exposed_in_body": true,
    "read_only": true,
    "role": "event_data",
    "fields": [
      { "id": "source", "type": "string", "body_port": "event_data.source" },
      { "id": "type", "type": "string", "body_port": "event_data.type" },
      { "id": "time", "type": "u64", "body_port": "event_data.time" }
    ]
  }
}</code></pre>

<p>
The timeout terminal has an outer input and a read-only body projection. The
Event Data terminal is a grouped projection rather than a v0.1 record type;
each field therefore declares its own public scalar type and body port. Base
v0.1 writes the canonical field catalogue in <code>source</code>, <code>type</code>,
then <code>time</code> order.
</p>

<h3>14.2 Event-case regions</h3>

<p>
Every event-case region MUST define <code>id</code>, <code>event</code>,
<code>event_data_fields</code>, and <code>diagram</code>. The event descriptor MUST
define <code>kind</code>. Base v0.1 publishes <code>timeout</code>,
<code>value_change</code>, and <code>user</code>. A <code>value_change</code> case MUST
identify its source; a <code>user</code> case MAY identify a source when scoped.
</p>

<pre><code>"regions": [
  {
    "id": "timeout",
    "event_label": "[0] Timeout",
    "event": { "kind": "timeout" },
    "event_data_fields": ["source", "type", "time"],
    "diagram": { "nodes": [], "edges": [] }
  },
  {
    "id": "temperature-change",
    "event_label": "[1] Temperature: Value Change",
    "event": {
      "kind": "value_change",
      "source": "widget:temperature"
    },
    "event_data_fields": ["source", "time"],
    "diagram": { "nodes": [], "edges": [] }
  }
]</code></pre>

<p>
<code>event_label</code> is authoring text and MUST NOT replace the event
descriptor. <code>event_data_fields</code> selects the rows exposed by the Event
Data node for that region and MUST contain only field ids declared by the
<code>event_data</code> terminal.
</p>

<h3>14.3 Timeout case</h3>

<p>
A canonical Event Structure MUST contain exactly one region whose
<code>event.kind</code> is <code>timeout</code>. The timeout value is resolved through
the <code>timeout</code> outer port and is projected read-only in the selected body
through <code>timeout_value</code>.
</p>

<h3>14.4 Authoring metadata</h3>

<p>
Fields such as <code>event_structure_visible_case_index</code>,
<code>event_structure_data_node_normalized_y</code>, background colors, subframe
colors and their opacity values preserve editor presentation only. They MUST
not affect which event is selected or how the region executes.
</p>

<h2 id="disabled-structure">14.5 Diagram Disable Structure</h2>

<p>
A <code>disabled_structure</code> preserves exactly two owned regions named
<code>enabled</code> and <code>disabled</code>. Each region MUST repeat its semantic
role through <code>activation: "enabled"</code> or
<code>activation: "disabled"</code>. The structure has no selector or condition
terminal, so <code>structure_terminals</code> MUST be an empty object. Ordinary
typed boundary ports and <code>structure_tunnels</code> remain valid.
</p>

<p>
Only the Enabled region is executable. The Disabled region remains canonical
source so an editor can retain and display its diagram, but it does not
participate in execution. <code>disabled_structure_enabled_visible</code>, the
background and subframe colors, and the veil color and opacity are authoring
metadata only and MUST NOT change that rule.
</p>

<h2 id="conditional-disable-structure">14.6 Conditional Disable Structure</h2>

<p>
A <code>conditional_disable_structure</code> is a compile-time conditional
dataflow container. It has no runtime selector terminal, so
<code>structure_terminals</code> MUST be empty. Its <code>regions</code> array is
ordered and MUST contain one or more regions with a structured
<code>condition</code>, followed by exactly one region with
<code>default: true</code>. Every region retains its complete owned diagram in
canonical source.
</p>

<p>
A condition is represented as <code>condition.any[]</code> terms joined by OR;
each term contains <code>all[]</code> comparisons joined by AND. A comparison
contains <code>symbol</code>, <code>operator</code> (<code>==</code> or
<code>!=</code>), and a case-sensitive string <code>value</code>. This normalized
form preserves AND precedence over OR without treating an opaque display
string as semantic source.
</p>

<p>
Resolution evaluates non-Default regions in source order against the resolved
Compilation Context. The first true region wins. If none is true, the Default
region wins. Built-in symbols are overlaid by Project symbols and then Target
symbols; a more specific layer replaces the same symbol name from a less
specific layer. <code>conditional_disable_visible_case_index</code> is
authoring metadata only and MUST NOT influence compile-time selection.
</p>

<hr/>

<h2 id="source-to-language-alignment">15. Source-to-Language Alignment</h2>

<p>
Canonical source representation and normative execution semantics are intentionally separated in FROG.
This section defines the source-level alignment points between the structures declared here and the execution-semantics layer owned by <code>Language/</code>.
</p>

<h3>15.1 Case alignment</h3>

<ul>
  <li>a node with <code>kind: "structure"</code> and <code>structure_type: "case"</code> aligns with the standardized <code>case</code> structure family,</li>
  <li><code>structure_terminals.selector</code> identifies the selector terminal in source,</li>
  <li>the <code>regions</code> array identifies the branch regions in source,</li>
  <li><code>match</code> and <code>default</code> fields provide the source-level branch-selection metadata consumed by the semantic layer.</li>
</ul>

<h3>15.2 For-loop alignment</h3>

<ul>
  <li>a node with <code>structure_type: "for_loop"</code> aligns with the standardized counted-loop family,</li>
  <li><code>structure_terminals.count</code> identifies the source terminal used to express loop count,</li>
  <li><code>structure_terminals.index</code> identifies the source terminal exposed in the body,</li>
  <li>the region <code>body</code> identifies the owned loop body region,</li>
  <li>loop-output metadata such as <code>mode</code> and <code>zero_iteration_value</code> provides source-level data consumed by loop-output semantics,</li>
  <li>explicit local memory inside the body remains represented by ordinary primitives such as <code>frog.core.delay</code> rather than by hidden loop-owned magic state.</li>
</ul>

<h3>15.3 While-loop alignment</h3>

<ul>
  <li>a node with <code>structure_type: "while_loop"</code> aligns with the standardized condition-governed loop family,</li>
  <li><code>structure_terminals.condition</code> identifies the source terminal used for continuation representation,</li>
  <li><code>structure_terminals.index</code> identifies the source terminal exposed in the body,</li>
  <li>the region <code>body</code> identifies the owned loop body region.</li>
</ul>

<h3>15.4 Event-structure alignment</h3>

<ul>
  <li>a node with <code>structure_type: "event_structure"</code> aligns with the standardized event-dispatch family,</li>
  <li><code>structure_terminals.timeout</code> identifies the timeout input and its body projection,</li>
  <li><code>structure_terminals.event_data.fields</code> defines the typed Event Data projections,</li>
  <li>each region's <code>event</code> descriptor defines its trigger independently from its authoring label,</li>
  <li>the region's <code>event_data_fields</code> selects the Event Data rows visible in that region.</li>
</ul>

<h3>15.5 Disabled-structure alignment</h3>

<ul>
  <li>a node with <code>structure_type: "disabled_structure"</code> aligns with the standardized source-exclusion family,</li>
  <li><code>structure_terminals</code> is empty because the structure has no selector input,</li>
  <li>the <code>enabled</code> and <code>disabled</code> regions retain both diagrams in source,</li>
  <li>the semantic layer executes only the region whose <code>activation</code> is <code>enabled</code>.</li>
</ul>

<h3>15.6 Conditional-disable alignment</h3>

<ul>
  <li>a node with <code>structure_type: "conditional_disable_structure"</code> aligns with compile-time graph specialization,</li>
  <li>its empty <code>structure_terminals</code> object forbids a runtime selector wire,</li>
  <li>conditioned regions are evaluated in array order and the first true region wins,</li>
  <li>exactly one Default region provides the fallback when no condition matches,</li>
  <li>all variants remain canonical source even when lowering retains only the selected graph.</li>
</ul>

<h3>15.7 No semantic redefinition in this document</h3>

<p>
This document MUST NOT be interpreted as redefining:
</p>

<ul>
  <li>which branch executes at runtime,</li>
  <li>how many times a loop body executes,</li>
  <li>how loop outputs are produced at runtime,</li>
  <li>how continuation is resolved after a loop-body activation,</li>
  <li>how an event source delivers and orders occurrences,</li>
  <li>how structure behavior participates in global execution semantics.</li>
</ul>

<p>
Those topics are owned normatively by <code>Language/Control structures.md</code>.
</p>

<hr/>

<h2 id="interaction-with-local-memory-and-cycles">16. Interaction with Local Memory and Cycles</h2>

<p>
Control structures interact with local memory and cycles at the source-representation level, but do not redefine the general semantic validity rule for cyclic graphs.
</p>

<p>
Accordingly:
</p>

<ul>
  <li>a cycle in the outer graph remains represented canonically at diagram level,</li>
  <li>a cycle inside a structure region remains represented canonically inside that region-local diagram,</li>
  <li>the presence of a loop structure does not by itself create canonical source evidence of valid local memory,</li>
  <li>explicit local-memory primitives remain the source-facing mechanism for representing memory in cyclic graph paths.</li>
</ul>

<p>
Any directed cycle inside a loop body remains subject to the same representation discipline as any other diagram:
if local memory is intended, it MUST be represented explicitly in canonical source.
</p>

<p>
In base v0.1, <code>frog.core.delay</code> is the minimal standard primitive used to represent such explicit local memory.
The normative cycle-validity rule belongs to <code>Language/State and cycles.md</code>.
</p>

<hr/>

<h2 id="diagram-representation">17. Diagram Representation</h2>

<p>
Control structures are represented in the diagram as nodes of:
</p>

<pre><code>kind = "structure"</code></pre>

<p>
with a valid:
</p>

<pre><code>structure_type = "case" | "for_loop" | "while_loop" | "event_structure" | "disabled_structure" | "conditional_disable_structure"</code></pre>

<p>
Their external ports are resolved from:
</p>

<ul>
  <li><code>boundary.inputs</code>,</li>
  <li><code>boundary.outputs</code>,</li>
  <li>any terminal whose <code>outer_visible</code> value is <code>true</code>.</li>
</ul>

<p>
Their internal source representation is resolved from:
</p>

<ul>
  <li>their owned regions,</li>
  <li>their structure-terminal definitions,</li>
  <li>their family-specific canonical fields.</li>
</ul>

<p>
Tools SHOULD present these structures as dedicated visible structural elements rather than disguising them as ordinary primitive functions.
</p>

<hr/>

<h2 id="validation-rules">18. Validation Rules</h2>

<p>
Implementations MUST enforce the following source-level validation rules:
</p>

<ul>
  <li>every structure node MUST define a valid <code>structure_type</code>,</li>
  <li>every structure node MUST define valid <code>boundary</code>, <code>structure_terminals</code>, and <code>regions</code>,</li>
  <li>all structure boundary crossings MUST be type-compatible,</li>
  <li>every region-local diagram MUST be valid as a local diagram scope,</li>
  <li>a <code>case</code> selector MUST use <code>bool</code> or <code>string</code> in base v0.1,</li>
  <li>if a <code>case</code> selector uses <code>bool</code>, exactly one region MUST match <code>true</code> and exactly one region MUST match <code>false</code>,</li>
  <li>if a <code>case</code> selector uses <code>string</code>, each explicit string <code>match</code> value MUST be unique,</li>
  <li>if a <code>case</code> selector uses <code>string</code>, exactly one region MUST define <code>default: true</code>,</li>
  <li>a <code>case</code> structure MUST define the source representation of every required output for every executable branch,</li>
  <li>canonical source MUST NOT replace <code>case</code> with derived editor-facing kinds such as <code>if</code>, <code>if_else</code>, <code>else_if</code>, or <code>switch</code>,</li>
  <li>a <code>for_loop</code> MUST define a valid count terminal,</li>
  <li>a <code>for_loop</code> count MUST be represented in a way compatible with non-negative counted iteration in base v0.1,</li>
  <li>a <code>for_loop</code> MUST define exactly one body region,</li>
  <li>a <code>while_loop</code> MUST define a valid boolean continuation terminal,</li>
  <li>a <code>while_loop</code> MUST use the canonical source shape aligned with the standardized continue-while-true rule of base v0.1,</li>
  <li>an <code>event_structure</code> MUST define canonical <code>timeout</code> and <code>event_data</code> terminals,</li>
  <li>an <code>event_structure</code> MUST define exactly one timeout event-case region,</li>
  <li>every event-case region MUST define an explicit <code>event</code> descriptor independently from <code>event_label</code>,</li>
  <li>every selected <code>event_data_fields</code> id MUST be declared by the typed <code>event_data</code> terminal,</li>
  <li>a <code>disabled_structure</code> MUST define an empty <code>structure_terminals</code> object,</li>
  <li>a <code>disabled_structure</code> MUST define exactly one <code>enabled</code> region and one <code>disabled</code> region with matching <code>activation</code> values,</li>
  <li>a <code>conditional_disable_structure</code> MUST define an empty <code>structure_terminals</code> object, one or more ordered condition regions, and exactly one Default region,</li>
  <li>every conditional-disable comparison MUST use exact string values and one of <code>==</code> or <code>!=</code>, with AND groups evaluated before OR groups,</li>
  <li>every loop output MUST have a complete source-level meaning,</li>
  <li>if <code>mode: "last_value"</code> is used and zero iterations are possible, <code>zero_iteration_value</code> MUST be present and type-compatible,</li>
  <li>cycles inside structure regions MUST use explicit source representation for local memory when local memory is intended.</li>
</ul>

<p>
Tools SHOULD additionally warn when:
</p>

<ul>
  <li>loop output meaning is technically representable but visually unclear,</li>
  <li>the use of local memory inside a loop is valid in source form but hard to inspect,</li>
  <li>structure representation becomes unnecessarily difficult to understand.</li>
</ul>

<hr/>

<h2 id="examples">19. Examples</h2>

<h3>19.1 Boolean case (canonical if / else)</h3>

<pre><code>{
  "id": "case_1",
  "kind": "structure",
  "structure_type": "case",
  "boundary": {
    "inputs": [
      { "id": "x", "type": "u16" }
    ],
    "outputs": [
      { "id": "y", "type": "u16" }
    ]
  },
  "structure_terminals": {
    "selector": {
      "type": "bool",
      "outer_visible": true,
      "exposed_in_body": true,
      "read_only": true,
      "outer_port": "selector",
      "body_port": "selected_case",
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
}</code></pre>

<h3>19.2 String case</h3>

<pre><code>{
  "id": "case_2",
  "kind": "structure",
  "structure_type": "case",
  "boundary": {
    "inputs": [],
    "outputs": []
  },
  "structure_terminals": {
    "selector": {
      "type": "string",
      "outer_visible": true,
      "exposed_in_body": true,
      "read_only": true,
      "outer_port": "selector",
      "body_port": "selected_case",
      "role": "selector"
    }
  },
  "regions": [
    {
      "id": "case_start",
      "match": "start",
      "diagram": {
        "nodes": [],
        "edges": []
      }
    },
    {
      "id": "case_stop",
      "match": "stop",
      "diagram": {
        "nodes": [],
        "edges": []
      }
    },
    {
      "id": "default_case",
      "default": true,
      "diagram": {
        "nodes": [],
        "edges": []
      }
    }
  ]
}</code></pre>

<h3>19.3 For loop with explicit final-value output</h3>

<pre><code>{
  "id": "loop_accumulate",
  "kind": "structure",
  "structure_type": "for_loop",
  "boundary": {
    "inputs": [
      { "id": "input_value", "type": "u16" }
    ],
    "outputs": [
      { "id": "final_value", "type": "u16", "mode": "last_value", "zero_iteration_value": 0 }
    ]
  },
  "structure_terminals": {
    "count": {
      "type": "i64",
      "outer_visible": true,
      "exposed_in_body": true,
      "read_only": true,
      "outer_port": "count",
      "body_port": "count_value",
      "role": "count"
    },
    "index": {
      "type": "i64",
      "outer_visible": false,
      "exposed_in_body": true,
      "read_only": true,
      "body_port": "iteration",
      "role": "index"
    }
  },
  "regions": [
    {
      "id": "body",
      "diagram": {
        "nodes": [
          { "id": "delay_state", "kind": "primitive", "type": "frog.core.delay" },
          { "id": "add_step", "kind": "primitive", "type": "frog.core.add" }
        ],
        "edges": []
      }
    }
  ]
}</code></pre>

<h3>19.4 While loop</h3>

<pre><code>{
  "id": "loop_2",
  "kind": "structure",
  "structure_type": "while_loop",
  "boundary": {
    "inputs": [
      { "id": "x", "type": "u16" }
    ],
    "outputs": [
      { "id": "y", "type": "u16", "mode": "last_value" }
    ]
  },
  "structure_terminals": {
    "condition": {
      "type": "bool",
      "outer_visible": false,
      "exposed_in_body": true,
      "read_only": false,
      "body_port": "condition",
      "role": "continue_while_true"
    },
    "index": {
      "type": "i64",
      "outer_visible": false,
      "exposed_in_body": true,
      "read_only": true,
      "body_port": "iteration",
      "role": "index"
    }
  },
  "regions": [
    {
      "id": "body",
      "diagram": {
        "nodes": [],
        "edges": []
      }
    }
  ]
}</code></pre>

<hr/>

<h3>19.5 Event structure</h3>

<pre><code>{
  "id": "events_1",
  "kind": "structure",
  "structure_type": "event_structure",
  "boundary": { "inputs": [], "outputs": [] },
  "structure_terminals": {
    "timeout": {
      "type": "i64",
      "outer_visible": true,
      "exposed_in_body": true,
      "read_only": true,
      "outer_port": "timeout",
      "body_port": "timeout_value",
      "role": "timeout_milliseconds"
    },
    "event_data": {
      "outer_visible": false,
      "exposed_in_body": true,
      "read_only": true,
      "role": "event_data",
      "fields": [
        { "id": "source", "type": "string", "body_port": "event_data.source" },
        { "id": "type", "type": "string", "body_port": "event_data.type" },
        { "id": "time", "type": "u64", "body_port": "event_data.time" }
      ]
    }
  },
  "regions": [
    {
      "id": "timeout",
      "event_label": "[0] Timeout",
      "event": { "kind": "timeout" },
      "event_data_fields": ["source", "type", "time"],
      "diagram": { "nodes": [], "edges": [] }
    }
  ]
}</code></pre>

<hr/>

<h2 id="out-of-scope-for-v01">20. Out of Scope for v0.1</h2>

<ul>
  <li>timed or clock-driven structures,</li>
  <li>parallel-region control structures,</li>
  <li>pattern-matching structures beyond the standardized boolean and string case forms,</li>
  <li>exception-handling structures,</li>
  <li>advanced loop output modes such as implicit collection or generalized reduction,</li>
  <li>hidden automatic memory insertion inside structures,</li>
  <li>standardized source-level aliases for authoring forms such as <code>if</code> or <code>switch</code>,</li>
  <li>profile-specific hidden accumulator syntax that would replace explicit local memory in the first executable slice.</li>
</ul>

<hr/>

<h2 id="summary">21. Summary</h2>

<p>
FROG treats structural control as an explicit language-level concept, not as a disguised library call.
This document defines how that structural control is represented in canonical source.
</p>

<ul>
  <li><code>case</code>, <code>for_loop</code>, <code>while_loop</code>, <code>event_structure</code>, <code>disabled_structure</code>, and <code>conditional_disable_structure</code> are the standardized control structures of base v0.1.</li>
  <li>A boolean <code>case</code> is the canonical source-level equivalent of <code>if / else</code>.</li>
  <li>A string <code>case</code> provides canonical multi-branch source representation with an explicit required default region.</li>
  <li>Derived IDE-facing authoring forms such as <em>If</em>, <em>If / Else</em>, <em>Else If</em>, and <em>Switch</em> do not introduce separate canonical structure families in source.</li>
  <li>Loop structures remain source-distinct from ordinary functions.</li>
  <li>Event structures declare their triggers separately from authoring labels and expose typed Event Data field projections.</li>
  <li>Diagram Disable structures retain both diagrams in source while excluding the Disabled region from execution.</li>
  <li>Conditional Disable structures retain every build variant in source and select the first matching condition, or Default, before runtime.</li>
  <li>Structure boundaries, structure terminals, and owned regions are explicit parts of canonical source.</li>
  <li>Explicit local memory inside loop regions remains represented by ordinary source primitives such as <code>frog.core.delay</code>.</li>
  <li>Normative execution semantics remain owned by <code>Language/Control structures.md</code>.</li>
</ul>

<p>
This gives FROG a clear, durable, and visually explicit foundation for structural program control at the source-representation level.
</p>
