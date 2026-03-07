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
<li><a href="#structure">4. Connector Structure</a></li>
<li><a href="#perimeter-model">5. Perimeter Slot Model</a></li>
<li><a href="#port-mapping">6. Port Mapping</a></li>
<li><a href="#examples">7. Examples</a></li>
<li><a href="#validation">8. Validation Rules</a></li>
<li><a href="#extensibility">9. Extensibility</a></li>
<li><a href="#summary">10. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
The <strong>connector</strong> section defines how a Frog appears when it is used as a node inside another diagram.
</p>

<p>
While the <strong>interface</strong> defines the logical inputs and outputs of the Frog,
the connector defines their <strong>graphical placement</strong> around the node.
</p>

<p>
Connector ports are positioned along the perimeter of the node using a discrete slot system.
</p>

<hr/>

<h2 id="purpose">2. Purpose of the Connector</h2>

The connector provides:

<ul>
<li>a consistent graphical representation of a Frog when reused as a node</li>
<li>a deterministic mapping between interface ports and visual connection points</li>
<li>a simple and extensible connector layout model</li>
</ul>

The connector ensures that:

<ul>
<li>ports appear in predictable locations</li>
<li>connections remain visually readable</li>
<li>nodes remain visually consistent across development tools</li>
</ul>

<hr/>

<h2 id="location">3. Location in a .frog file</h2>

The connector is defined as a top-level object.

Example structure:

<pre>
{
  "spec_version": "0.1",
  "metadata": {},
  "interface": {},
  "connector": {},
  "diagram": {},
  "front_panel": {}
}
</pre>

The connector section SHOULD exist for Frogs intended to be reused as nodes.

<hr/>

<h2 id="structure">4. Connector Structure</h2>

Example connector definition:

<pre>
"connector": {
  "granularity": 40,
  "ports": [
    { "interface_port": "a", "slot": 34 },
    { "interface_port": "b", "slot": 36 },
    { "interface_port": "result", "slot": 14 }
  ]
}
</pre>

Fields:

<ul>
<li><strong>granularity</strong>: number of discrete connector positions along the node perimeter</li>
<li><strong>ports</strong>: mapping between interface ports and connector slots</li>
</ul>

If <code>granularity</code> is omitted, implementations SHOULD assume the default value of <strong>40</strong>.

<hr/>

<h2 id="perimeter-model">5. Perimeter Slot Model</h2>

<p>
The node is treated as a normalized square.
Connector positions are defined along the perimeter of this square.
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

<p>Conceptual model:</p>

<pre>

                 0   1   2   3   4   5   6   7   8   9
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
</pre>

<p>
Each slot represents a discrete position on the perimeter of the node.
</p>

<p>
The connector specification does not define pixel dimensions.
Rendering systems determine the actual graphical layout.
</p>

<hr/>

<h2 id="port-mapping">6. Port Mapping</h2>

Each entry maps an interface port to a connector slot.

Example:

<pre>
{
  "interface_port": "temperature",
  "slot": 12
}
</pre>

Fields:

<ul>
<li><strong>interface_port</strong>: name of the interface port</li>
<li><strong>slot</strong>: index of the connector position</li>
</ul>

The slot determines where the connection point appears on the node.

<hr/>

<h2 id="examples">7. Examples</h2>

<h3>Basic Arithmetic Node</h3>

<pre>
"connector": {
  "ports": [
    { "interface_port": "a", "slot": 34 },
    { "interface_port": "b", "slot": 36 },
    { "interface_port": "result", "slot": 14 }
  ]
}
</pre>

<h3>Control System Node</h3>

<pre>
"connector": {
  "ports": [
    { "interface_port": "setpoint", "slot": 2 },
    { "interface_port": "measurement", "slot": 6 },
    { "interface_port": "enable", "slot": 1 },
    { "interface_port": "output", "slot": 22 }
  ]
}
</pre>

<hr/>

<h2 id="validation">8. Validation Rules</h2>

Implementations MUST enforce:

<ul>
<li>Each connector port MUST reference an existing interface port</li>
<li>Slot values MUST satisfy <code>0 ≤ slot &lt; granularity</code></li>
<li>Each interface port SHOULD appear at most once in the connector</li>
</ul>

Invalid connector mappings MUST trigger validation errors.

<hr/>

<h2 id="extensibility">9. Extensibility</h2>

Tools MAY extend connector entries with additional properties.

Example:

<pre>
{
  "interface_port": "speed",
  "slot": 8,
  "label": "Speed Input"
}
</pre>

Unknown properties MUST be ignored by runtimes.

<hr/>

<h2 id="summary">10. Summary</h2>

The connector defines how interface ports are positioned when a Frog is used as a reusable node.

It provides:

<ul>
<li>a deterministic connector placement model</li>
<li>a simple and standardized perimeter layout</li>
<li>consistent node representation across tools</li>
</ul>

The connector bridges the logical interface of a Frog with its graphical representation inside diagrams.
