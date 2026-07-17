<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 FROG Connector Specification</h1>

<p align="center">
Definition of connector mapping for <strong>.frog</strong> programs<br/>
<em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#purpose">2. Purpose of the Connector</a></li>
  <li><a href="#location">3. Location in a .frog file</a></li>
  <li><a href="#relationship-with-interface">4. Relationship with the Interface</a></li>
  <li><a href="#structure">5. Connector Structure</a></li>
  <li><a href="#perimeter-model">6. Perimeter Slot Model</a></li>
  <li><a href="#port-mapping">7. Port Mapping</a></li>
  <li><a href="#examples">8. Examples</a></li>
  <li><a href="#validation">9. Validation Rules</a></li>
  <li><a href="#extensibility">10. Extensibility</a></li>
  <li><a href="#summary">11. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
The <strong>connector</strong> section defines how a Frog is graphically represented when it is instantiated as a node inside another diagram.
</p>

<p>
The connector does <strong>not</strong> define the logical contract of the Frog.
That responsibility belongs to the <strong>interface</strong> section.
</p>

<p>
Instead, the connector provides a deterministic graphical mapping from public interface ports to visible connection points around the node boundary.
</p>

<p>
A Frog may be executable without a connector.
The connector becomes relevant when the Frog is intended to be reused as a graphical node.
</p>

<p>
This document also does not define repository-wide version policy. Top-level <code>spec_version</code> identifies the source-format compatibility target of the containing <code>.frog</code> file, while the published specification corpus version remains governed centrally in <code>Versioning/Readme.md</code>.
</p>

<hr/>

<h2 id="purpose">2. Purpose of the Connector</h2>

<p>
The connector provides:
</p>

<ul>
  <li>a consistent graphical representation of a Frog when reused as a node</li>
  <li>a deterministic mapping between public interface ports and visible connection points</li>
  <li>a simple, tool-independent connector layout model</li>
</ul>

<p>
The connector ensures that:
</p>

<ul>
  <li>ports appear in predictable locations</li>
  <li>graphical reuse remains stable across tools</li>
  <li>the visual shape of a reusable Frog remains independent from its internal diagram layout</li>
</ul>

<hr/>

<h2 id="location">3. Location in a .frog file</h2>

<p>
The connector is defined as a top-level object.
</p>

<p>
Example structure:
</p>

<pre><code>{
  "spec_version": "0.1",
  "metadata": {},
  "interface": {},
  "connector": {},
  "diagram": {},
  "front_panel": {}
}
</code></pre>

<p>
The <code>connector</code> section is <strong>optional</strong>.
</p>

<p>
It SHOULD be present when the Frog is intended to be instantiated as a reusable node inside another Frog.
</p>

<p>
A top-level executable Frog may omit the connector entirely.
</p>

<p>
In this source shape:
</p>

<ul>
  <li>top-level <code>spec_version</code> identifies the source-format compatibility target of the file,</li>
  <li><code>connector</code> defines optional graphical projection of the public contract,</li>
  <li>the published specification corpus version remains governed centrally in <code>Versioning/Readme.md</code>.</li>
</ul>

<hr/>

<h2 id="relationship-with-interface">4. Relationship with the Interface</h2>

<p>
The connector depends on the <strong>interface</strong> section.
</p>

<p>
The interface defines the public logical ports of the Frog.
The connector defines where those ports appear on the node perimeter when the Frog is reused graphically.
</p>

<p>
Therefore:
</p>

<ul>
  <li>the connector MUST NOT introduce ports that do not exist in the interface</li>
  <li>the connector MUST reference only public interface ports</li>
  <li>a Frog may have an interface without a connector</li>
  <li>a Frog MUST NOT define a connector without an interface</li>
</ul>

<p>
In practical terms:
</p>

<ul>
  <li><strong>interface</strong> = public contract</li>
  <li><strong>connector</strong> = graphical projection of that contract</li>
</ul>

<p>
The connector does not define source-format compatibility policy and does not redefine the versioning meaning of top-level <code>spec_version</code>.
</p>

<p>
The connector is also distinct from the source-owned Interface Map defined in
<code>Interface map.md</code>. The Interface Map records an editor-visible
layout pattern and explicit widget-value bindings. A reusable-node connector
projection MAY be derived from that map, but it MUST preserve public port
identity, direction, and type, and MUST NOT depend on transient IDE state.
</p>

<hr/>

<h2 id="structure">5. Connector Structure</h2>

<p>
Example connector definition:
</p>

<pre><code>"connector": {
  "granularity": 40,
  "ports": [
    { "interface_port": "a", "slot": 34 },
    { "interface_port": "b", "slot": 36 },
    { "interface_port": "result", "slot": 14 }
  ]
}
</code></pre>

<p>
Fields:
</p>

<ul>
  <li><strong>granularity</strong>: number of discrete connector positions along the perimeter</li>
  <li><strong>ports</strong>: mapping between public interface port identifiers and perimeter slots</li>
</ul>

<p>
If <code>granularity</code> is omitted, implementations SHOULD assume the default value of <strong>40</strong>.
</p>

<p>
For FROG v0.1, the canonical model is a square node with a perimeter divided into <strong>40 slots</strong>, that is <strong>10 slots per side</strong>.
</p>

<hr/>

<h2 id="perimeter-model">6. Perimeter Slot Model</h2>

<p>
The connector is defined on a normalized square node boundary.
</p>

<p>
Slots are ordered clockwise starting from the <strong>top-left corner</strong>.
</p>

<p>
With the default granularity of 40, the perimeter is divided into four equal sides:
</p>

<ul>
  <li>Top side: slots <code>0..9</code></li>
  <li>Right side: slots <code>10..19</code></li>
  <li>Bottom side: slots <code>20..29</code></li>
  <li>Left side: slots <code>30..39</code></li>
</ul>

<p>
Conceptual model:
</p>

<pre><code>                 0   1   2   3   4   5   6   7   8   9
               ●   ●   ●   ●   ●   ●   ●   ●   ●   ●

          39 ●                                     ● 10
          38 ●                                     ● 11
          37 ●                                     ● 12
          36 ●                                     ● 13
          35 ●                                     ● 14
          34 ●                                     ● 15
          33 ●                                     ● 16
          32 ●                                     ● 17
          31 ●                                     ● 18
          30 ●                                     ● 19

               ●   ●   ●   ●   ●   ●   ●   ●   ●   ●
                29  28  27  26  25  24  23  22  21  20
</code></pre>

<p>
Each slot represents one discrete connection position on the perimeter of the reusable node.
</p>

<p>
The connector model defines logical placement only.
Actual pixel rendering is implementation-dependent.
</p>

<hr/>

<h2 id="port-mapping">7. Port Mapping</h2>

<p>
Each entry in <code>connector.ports</code> maps one public interface port to one connector slot.
</p>

<p>
Example:
</p>

<pre><code>{
  "interface_port": "temperature",
  "slot": 12
}
</code></pre>

<p>
Fields:
</p>

<ul>
  <li><strong>interface_port</strong>: <strong>id</strong> of a public port declared in the interface</li>
  <li><strong>slot</strong>: connector slot index on the node perimeter</li>
</ul>

<p>
The slot determines where the corresponding connection point appears when the Frog is drawn as a reusable node.
</p>

<p>
The connector does not redefine port type, direction, or semantics.
Those properties remain defined by the interface.
</p>

<hr/>

<h2 id="examples">8. Examples</h2>

<h3>Basic Arithmetic Node</h3>

<pre><code>"connector": {
  "granularity": 40,
  "ports": [
    { "interface_port": "a", "slot": 34 },
    { "interface_port": "b", "slot": 36 },
    { "interface_port": "result", "slot": 14 }
  ]
}
</code></pre>

<h3>Control System Node</h3>

<pre><code>"connector": {
  "granularity": 40,
  "ports": [
    { "interface_port": "enable", "slot": 1 },
    { "interface_port": "setpoint", "slot": 3 },
    { "interface_port": "measurement", "slot": 6 },
    { "interface_port": "output", "slot": 22 }
  ]
}
</code></pre>

<h3>Top-Level Executable Frog Without Connector</h3>

<pre><code>{
  "spec_version": "0.1",
  "metadata": {},
  "interface": {
    "inputs": [
      { "id": "file_path", "type": "string" }
    ],
    "outputs": [
      { "id": "status", "type": "bool" }
    ]
  },
  "diagram": {},
  "front_panel": {}
}
</code></pre>

<p>
In this case, the Frog remains executable, but it does not yet define a reusable graphical connector.
</p>

<p>
In this example:
</p>

<ul>
  <li>top-level <code>spec_version</code> identifies the source-format compatibility target of the file,</li>
  <li>the absence of <code>connector</code> means no serialized reusable-node perimeter mapping is provided,</li>
  <li>the published specification corpus version remains governed centrally in <code>Versioning/Readme.md</code>.</li>
</ul>

<hr/>

<h2 id="validation">9. Validation Rules</h2>

<p>
Implementations MUST enforce the following rules:
</p>

<ul>
  <li>the connector MUST be a top-level object when present</li>
  <li>each connector entry MUST reference an existing public interface port</li>
  <li>slot values MUST satisfy <code>0 ≤ slot &lt; granularity</code></li>
  <li>each connector slot MUST be used at most once</li>
  <li>each interface port referenced in the connector MUST appear at most once</li>
</ul>

<p>
Additionally, for a Frog declared or interpreted as a reusable graphical node:
</p>

<ul>
  <li>every public interface input SHOULD have a connector mapping</li>
  <li>every public interface output SHOULD have a connector mapping</li>
</ul>

<p>
Implementations MAY treat incomplete mappings as validation errors when full graphical reusability is required.
</p>

<p>
Invalid connector mappings MUST trigger validation errors.
</p>

<p>
These checks validate optional connector structure and connector-to-interface consistency.
They do not, by themselves, redefine top-level <code>spec_version</code> policy or repository-wide corpus-version governance.
</p>

<hr/>

<h2 id="extensibility">10. Extensibility</h2>

<p>
Tools MAY extend connector entries with additional non-semantic properties.
</p>

<p>
Example:
</p>

<pre><code>{
  "interface_port": "speed",
  "slot": 8,
  "label": "Speed Input"
}
</code></pre>

<p>
Such properties MAY be used for authoring, visualization, or editor hints.
</p>

<p>
Unknown connector properties MUST be ignored by runtimes and by tools that do not support them.
</p>

<p>
Such extensions should remain compatible with the centralized cumulative version model: later source-format versions should normally extend earlier valid connector forms rather than silently replace them, unless an explicit breaking boundary is declared in repository-wide version governance.
</p>

<hr/>

<h2 id="summary">11. Summary</h2>

<p>
The connector defines how a Frog appears when it is reused as a node inside another diagram.
</p>

<p>
It does not define the logical contract of the Frog.
That responsibility belongs to the interface.
</p>

<p>
The connector provides:
</p>

<ul>
  <li>a deterministic connector placement model</li>
  <li>a simple and standardized perimeter slot layout</li>
  <li>a clear separation between logical public ports and graphical node representation</li>
</ul>

<p>
In short:
</p>

<ul>
  <li><strong>interface</strong> defines <em>what</em> the Frog exposes</li>
  <li><strong>connector</strong> defines <em>how</em> that interface appears when the Frog is used as a reusable node</li>
  <li><strong>connector</strong> does not define source-format compatibility law or published specification corpus versioning</li>
  <li><strong>interface.map</strong> records the selected Interface Layout Pattern and widget-value slot correspondence described by <code>Interface map.md</code></li>
</ul>
