<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Widget Interaction</h1>

<p align="center">
  <strong>Canonical source-level representation of natural-value, object-style, and event-oriented diagram interaction with front-panel widget instances</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#why-this-document-exists">2. Why This Document Exists</a></li>
  <li><a href="#scope">3. Scope</a></li>
  <li><a href="#design-objectives">4. Design Objectives</a></li>
  <li><a href="#relation-with-other-specifications">5. Relation with Other Specifications</a></li>
  <li><a href="#core-model">6. Core Model</a></li>
  <li><a href="#natural-value-path-vs-object-style-path">7. Natural Value Path vs Object-Style Path</a></li>
  <li><a href="#widget-member-addressing">8. Widget Member Addressing</a></li>
  <li><a href="#interaction-node-families">9. Interaction Node Families</a></li>
  <li><a href="#property-read">10. Property Read</a></li>
  <li><a href="#property-write">11. Property Write</a></li>
  <li><a href="#method-invoke">12. Method Invoke</a></li>
  <li><a href="#event-observe">13. Event Observe</a></li>
  <li><a href="#typing-sequencing-and-effect-boundaries">14. Typing, Sequencing, and Effect Boundaries</a></li>
  <li><a href="#diagram-representation">15. Diagram Representation</a></li>
  <li><a href="#validation-rules">16. Validation Rules</a></li>
  <li><a href="#source-to-library-mapping">17. Source-to-Library Mapping</a></li>
  <li><a href="#examples">18. Examples</a></li>
  <li><a href="#out-of-scope">19. Out of Scope</a></li>
  <li><a href="#status">20. Status</a></li>
  <li><a href="#license">21. License</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
FROG front-panel widgets are source-level widget instances declared in <code>front_panel</code>.
They participate in execution through the authoritative <code>diagram</code>, not through the front panel by itself.
</p>

<p>
This document defines the canonical source-level representation of three distinct diagram-side widget interaction paths:
</p>

<ul>
  <li><strong>natural-value participation</strong> through <code>widget_value</code>,</li>
  <li><strong>object-style interaction</strong> through <code>widget_reference</code> together with explicit widget interaction primitives,</li>
  <li><strong>event-oriented observation</strong> through <code>widget_reference</code> together with explicit event observation primitives.</li>
</ul>

<p>
These paths are related but semantically distinct.
</p>

<p>
The natural-value path is the canonical representation of ordinary participation of a widget primary value in typed dataflow.
The object-style path is the canonical representation of explicit member interaction such as:
</p>

<ul>
  <li>property reads,</li>
  <li>property writes,</li>
  <li>method invocation.</li>
</ul>

<p>
The event-oriented path is the canonical representation of observing declared widget or widget-part events through an explicit source-visible primitive rather than through hidden runtime conventions.
</p>

<p>
FROG standardizes the following widget interaction primitive families for the object-style and event-oriented paths:
</p>

<ul>
  <li><code>frog.ui.property_read</code></li>
  <li><code>frog.ui.property_write</code></li>
  <li><code>frog.ui.method_invoke</code></li>
  <li><code>frog.ui.event_observe</code></li>
</ul>

<p>
A value-carrying widget may expose a primary value member such as <code>value</code>.
That member has a dual architectural posture:
</p>

<ul>
  <li>it participates naturally through <code>widget_value</code>, and</li>
  <li>it MAY also be exposed as an ordinary object property when the corresponding widget class contract permits that access.</li>
</ul>

<p>
Accordingly, the same conceptual primary value may be reachable through two different interaction forms,
but those forms MUST remain distinct in source and semantics.
</p>

<p>
This document also preserves the separation between executable value participation,
object-style member interaction, event observation, and source-owned instance metadata.
A widget MAY expose non-semantic presentation members such as <code>style.text_color</code> or <code>interaction.visible</code>,
and a widget instance MAY persist presentation-oriented package or asset references through source-owned metadata.
Those members belong to object-style access or source-owned instance metadata, not to the natural typed value path.
</p>

<hr/>

<h2 id="why-this-document-exists">2. Why This Document Exists</h2>

<p>
A serious widget system requires more than a front-panel declaration.
It also requires a disciplined way to express how a diagram interacts with widgets as values, as objects, and as event sources.
</p>

<p>
Without that distinction, several architectural collapses appear:
</p>

<ul>
  <li>ordinary typed dataflow gets mixed with object-style member access,</li>
  <li>presentation properties get confused with semantic primary values,</li>
  <li>widget references get confused with ordinary typed values,</li>
  <li>events get trapped in runtime-private code,</li>
  <li>runtime convenience gets confused with language truth.</li>
</ul>

<p>
This document therefore fixes a clean interaction corridor:
</p>

<ul>
  <li><code>widget_value</code> for natural primary-value participation,</li>
  <li><code>widget_reference</code> for object-style widget access and event-oriented observation,</li>
  <li><code>frog.ui.*</code> primitives for explicit reads, writes, invocations, and event observation.</li>
</ul>

<p>
It exists so that a runtime may implement widget interaction,
but may not silently redefine the interaction model.
The interaction corridor must remain source-visible, inspectable, and portable across runtime families.
</p>

<hr/>

<h2 id="scope">3. Scope</h2>

<p>
This document specifies:
</p>

<ul>
  <li>the canonical source-level representation of natural-value, object-style, and event-oriented widget interaction in the diagram,</li>
  <li>the semantic boundary between <code>widget_value</code> and <code>widget_reference</code>-based interaction,</li>
  <li>the member-addressing model for widget and widget-part properties, methods, and events,</li>
  <li>the required node-local descriptor fields used by widget interaction primitives,</li>
  <li>the graph-shape constraints that tie widget interaction primitives to <code>widget_reference</code>,</li>
  <li>the source-level validation rules for these interaction forms.</li>
</ul>

<p>
This document does not specify:
</p>

<ul>
  <li>the full widget object model,</li>
  <li>the class-side legality of widget members,</li>
  <li>the canonical declaration and composition of the front panel,</li>
  <li>the full primitive identities and complete primitive signatures beyond their source-facing use here,</li>
  <li>the primitive-local execution semantics of UI interaction operations,</li>
  <li>the general execution semantics of the diagram,</li>
  <li>one mandatory rendering engine or one mandatory realization stack.</li>
</ul>

<p>
This document therefore sits between widget instance declaration and primitive execution semantics:
it defines how interaction is expressed in canonical source, not the full meaning of every class member and not the full behavior of every primitive implementation.
</p>

<hr/>

<h2 id="design-objectives">4. Design Objectives</h2>

<p>
The widget interaction model is designed to satisfy the following objectives:
</p>

<ul>
  <li><strong>Object consistency</strong> — widget interaction MUST follow the widget object model instead of introducing ad hoc UI exceptions.</li>
  <li><strong>Contract consistency</strong> — object-style access and event observation MUST be legal only when permitted by the corresponding widget class contract.</li>
  <li><strong>Dataflow clarity</strong> — ordinary value flow and explicit object access MUST remain distinct.</li>
  <li><strong>Event explicitness</strong> — event observation MUST be source-visible and MUST NOT depend solely on runtime-private conventions.</li>
  <li><strong>Source explicitness</strong> — widget interaction MUST be represented through explicit source constructs rather than hidden editor conventions.</li>
  <li><strong>Type safety</strong> — ordinary value typing and event payload typing MUST remain compatible with the FROG type system.</li>
  <li><strong>Token separation</strong> — ordinary typed values, widget references, event payloads, and UI sequencing tokens MUST remain distinct categories and MUST NOT be silently collapsed.</li>
  <li><strong>Profile extensibility</strong> — widget classes MAY expose different properties, methods, parts, and events under different profiles while preserving one common interaction model.</li>
  <li><strong>Presentation separation</strong> — presentation properties and realization-oriented metadata MUST remain distinct from the natural executable value path.</li>
  <li><strong>Architectural separation</strong> — widget interaction MUST NOT redefine public interface semantics, front-panel composition semantics, widget class contracts, general execution semantics, or the ownership of the standardized <code>frog.ui.*</code> primitive surface.</li>
</ul>

<hr/>

<h2 id="relation-with-other-specifications">5. Relation with Other Specifications</h2>

<p>
This document depends on and complements the following specifications:
</p>

<ul>
  <li><code>Expression/Type.md</code> — ordinary value types, compatibility rules, and implicit coercion rules.</li>
  <li><code>Expression/Widget.md</code> — widget instances, roles, value behavior at instance level, source-visible widget parts, and widget identity.</li>
  <li><code>Expression/Widget class contract.md</code> — class-side member contracts, event contracts, part ownership, member legality, access modes, and class-level object exposure.</li>
  <li><code>Expression/Front panel.md</code> — declaration and serialization of widget instances and source-owned instance metadata.</li>
  <li><code>Expression/Diagram.md</code> — executable graph structure, node categories, canonical widget node kinds, and port-resolution rules.</li>
  <li><code>Libraries/UI.md</code> — the standardized <code>frog.ui.*</code> primitive surface, primitive signatures, sequencing rules, and primitive-local semantics.</li>
  <li><code>Expression/Widget package (.wfrog).md</code> — the architectural role of <code>.wfrog</code> packages when class law, bounded behavior, or realization mappings are resolved through external widget-oriented artifacts.</li>
</ul>

<p>
This document complements, but does not redefine, the following canonical diagram node kinds:
</p>

<ul>
  <li><code>widget_value</code> — natural primary-value participation,</li>
  <li><code>widget_reference</code> — object-style widget access anchor and event observation anchor.</li>
</ul>

<p>
This document also does not redefine public interface semantics.
A front-panel widget does not create a public interface port by itself.
Public interface behavior remains represented canonically through diagram-side interface nodes.
</p>

<p>
Ownership is intentionally split as follows:
</p>

<pre><code>Widget interaction.md
  -&gt; source-level interaction model
  -&gt; boundary between widget_value and widget_reference
  -&gt; member and event descriptor model used by interaction nodes
  -&gt; graph-shape constraints
  -&gt; source-level validation rules for widget interaction forms

Widget.md
  -&gt; widget instance model
  -&gt; widget roles
  -&gt; instance-side value behavior
  -&gt; source-visible widget identity and source-owned metadata

Widget class contract.md
  -&gt; class-side member and event contracts
  -&gt; part ownership
  -&gt; readable / writable / invocable / observable legality
  -&gt; design-time vs runtime access constraints

Front panel.md
  -&gt; widget declaration and front-panel composition
  -&gt; source-owned instance metadata

Diagram.md
  -&gt; executable graph structure
  -&gt; widget_value / widget_reference node kinds
  -&gt; primitive-node structural integration

Libraries/UI.md
  -&gt; frog.ui.* primitive identities
  -&gt; primitive signatures
  -&gt; primitive-local typing and sequencing rules
  -&gt; primitive-local interaction semantics

Widget package (.wfrog).md
  -&gt; external widget-oriented class and realization artifacts
  -&gt; package-level bounded behavior publication
  -&gt; package-level asset and host-binding boundaries
</code></pre>

<hr/>

<h2 id="core-model">6. Core Model</h2>

<p>
A widget interaction node addresses a declared surface of a widget object.
</p>

<p>
A widget surface addressed through this document is one of:
</p>

<ul>
  <li>a widget property,</li>
  <li>a widget-part property,</li>
  <li>a widget method,</li>
  <li>a widget-part method,</li>
  <li>a widget event,</li>
  <li>a widget-part event.</li>
</ul>

<p>
Examples of conceptual interaction:
</p>

<pre><code>ctrl_gain.value
ctrl_gain.interaction.visible
ctrl_gain.style.text_color
ctrl_gain.label.text
ctrl_gain.focus()
graph_1.axes.x.interaction.visible
count_indicator.value_display.value_rendered
</code></pre>

<p>
The object-style interaction path is always represented conceptually as:
</p>

<pre><code>widget_reference -&gt; frog.ui.property_read
widget_reference -&gt; frog.ui.property_write
widget_reference -&gt; frog.ui.method_invoke
widget_reference -&gt; frog.ui.event_observe
</code></pre>

<p>
A widget interaction node does not declare the widget instance.
It operates on a widget instance that already exists in the <code>front_panel</code>.
</p>

<p>
A widget interaction node also does not define which members or events are legal.
It relies on the corresponding widget class contract.
</p>

<p>
When explicit ordering of UI side effects is required,
widget interaction nodes MAY participate in a sequencing chain through <code>ui_in</code> and <code>ui_out</code>.
The primitive-level meaning of those sequencing ports is defined elsewhere.
</p>

<p>
The following distinction MUST remain explicit:
</p>

<pre><code>ordinary typed value
    !=
widget reference token
    !=
UI sequencing token
</code></pre>

<p>
The following distinction MUST also remain explicit:
</p>

<pre><code>natural primary value flow
    !=
object-style property access
    !=
object-style method invocation
    !=
event observation
    !=
source-owned instance metadata
</code></pre>

<p>
The interaction corridor therefore has two anchors and several explicit operations:
</p>

<ul>
  <li><code>widget_value</code> anchors natural value participation,</li>
  <li><code>widget_reference</code> anchors object-style and event-oriented interaction,</li>
  <li>the primitive node identifies the interaction kind,</li>
  <li>the local descriptor identifies the addressed surface,</li>
  <li>the class contract decides whether the interaction is legal.</li>
</ul>

<hr/>

<h2 id="natural-value-path-vs-object-style-path">7. Natural Value Path vs Object-Style Path</h2>

<p>
A value-carrying widget class may expose a primary value member, often named <code>value</code>.
</p>

<p>
FROG recognizes two possible ways to access that member:
</p>

<ul>
  <li>the natural value path through <code>widget_value</code>,</li>
  <li>the object-style path through <code>widget_reference</code> and an explicit interaction primitive.</li>
</ul>

<h3>7.1 Natural Value Path</h3>

<p>
The natural value path is the canonical path for ordinary typed value participation in dataflow.
</p>

<p>
Conceptually:
</p>

<pre><code>widget_value(ctrl_count)
    -&gt; u16
</code></pre>

<p>
This path SHOULD be used when the program intends to:
</p>

<ul>
  <li>read the primary value of a control as an ordinary input to dataflow,</li>
  <li>write the primary value of an indicator as an ordinary output from dataflow,</li>
  <li>participate in normal computation without object-style intent.</li>
</ul>

<h3>7.2 Object-Style Path</h3>

<p>
The object-style path is the canonical path for:
</p>

<ul>
  <li>property reads,</li>
  <li>property writes,</li>
  <li>method invocation,</li>
  <li>addressing named parts,</li>
  <li>accessing presentation properties such as <code>style.text_color</code> or <code>interaction.visible</code>,</li>
  <li>accessing members that have no natural value-path representation.</li>
</ul>

<p>
Conceptually:
</p>

<pre><code>widget_reference(ctrl_count)
    -&gt; frog.ui.property_write { "member": "style.text_color" }
</code></pre>

<h3>7.3 Primary Value through Object-Style Access</h3>

<p>
A widget class MAY also expose its primary value member as an object-style property named <code>value</code>.
</p>

<p>
If the class contract allows it, the following is legal:
</p>

<pre><code>widget_reference(ctrl_count)
    -&gt; frog.ui.property_read { "member": "value" }
</code></pre>

<p>
However, this path MUST remain semantically distinct from <code>widget_value</code>.
When the program intent is ordinary value participation in dataflow, tools SHOULD prefer the natural <code>widget_value</code> form.
</p>

<h3>7.4 Event-Oriented Observation Path</h3>

<p>
A widget class MAY expose named events at the widget root or at declared parts.
Those events are not ordinary typed values and are not properties.
They are observed through the event-oriented path.
</p>

<p>
Conceptually:
</p>

<pre><code>widget_reference(count_indicator)
    -&gt; frog.ui.event_observe { "name": "value_rendered" }
</code></pre>

<p>
If an event payload exists, that payload becomes available through the primitive-level output contract defined elsewhere.
This observation path MUST remain distinct from both <code>widget_value</code> and property access.
</p>

<h3>7.5 Presentation Property Boundary</h3>

<p>
A widget class MAY expose presentation-oriented members such as:
</p>

<ul>
  <li><code>style.text_color</code>,</li>
  <li><code>style.background_fill</code>,</li>
  <li><code>interaction.visible</code>,</li>
  <li>other explicitly declared presentation-facing properties.</li>
</ul>

<p>
These members belong to object-style access or source-owned instance metadata.
They MUST NOT be treated as natural value-path equivalents.
</p>

<p>
In particular:
</p>

<ul>
  <li><code>style.text_color</code> MAY be read or written through object-style property access when the class contract allows it,</li>
  <li>a source-owned asset or template reference MAY exist without becoming an ordinary executable value-flow member.</li>
</ul>

<h3>7.6 Prohibited Collapses</h3>

<p>
The following collapses are invalid unless another specification explicitly states otherwise:
</p>

<ul>
  <li>treating <code>widget_reference</code> as an ordinary typed value,</li>
  <li>treating a presentation property as if it were the natural primary value of the widget,</li>
  <li>treating an event payload channel as if it were the widget primary value path,</li>
  <li>treating a presentation asset reference as executable semantic state by default,</li>
  <li>treating source-owned instance metadata as a substitute for <code>widget_value</code>.</li>
</ul>

<hr/>

<h2 id="widget-member-addressing">8. Widget Member Addressing</h2>

<p>
Widget member addressing identifies:
</p>

<ul>
  <li>the target widget instance,</li>
  <li>optionally a named widget part,</li>
  <li>the addressed property, method, or event.</li>
</ul>

<h3>8.1 Full Conceptual Address</h3>

<p>
A full conceptual property address may be understood as:
</p>

<pre><code>{
  "widget": "ctrl_gain",
  "part": "label",
  "member": "text"
}
</code></pre>

<p>
For methods:
</p>

<pre><code>{
  "widget": "ctrl_gain",
  "part": "label",
  "name": "show"
}
</code></pre>

<p>
For events:
</p>

<pre><code>{
  "widget": "count_indicator",
  "part": "value_display",
  "name": "value_rendered"
}
</code></pre>

<h3>8.2 Primitive-Local Descriptor Model</h3>

<p>
In the canonical diagram model, widget identity is carried by the incoming <code>ref</code> port
rather than embedded directly in the primitive node.
</p>

<p>
Accordingly:
</p>

<ul>
  <li>property interaction nodes MUST declare a <code>widget_member</code> descriptor,</li>
  <li>method interaction nodes MUST declare a <code>widget_method</code> descriptor,</li>
  <li>event interaction nodes MUST declare a <code>widget_event</code> descriptor.</li>
</ul>

<p>
Canonical property descriptors:
</p>

<pre><code>{ "member": "text" }
{ "part": "label", "member": "text" }
</code></pre>

<p>
Canonical method descriptors:
</p>

<pre><code>{ "name": "focus" }
{ "part": "label", "name": "show" }
</code></pre>

<p>
Canonical event descriptors:
</p>

<pre><code>{ "name": "value_rendered" }
{ "part": "value_display", "name": "value_rendered" }
</code></pre>

<h3>8.3 Widget-Level Addressing</h3>

<p>
If <code>part</code> is omitted, the addressed member or event belongs to the widget root object.
</p>

<p>
Examples:
</p>

<pre><code>{ "member": "value" }
{ "member": "interaction.visible" }
{ "member": "style.text_color" }
{ "name": "focus" }
{ "name": "value_changed" }
</code></pre>

<h3>8.4 Part-Level Addressing</h3>

<p>
If <code>part</code> is present, the addressed member or event belongs to the named widget part.
</p>

<p>
Examples:
</p>

<pre><code>{ "part": "label", "member": "text" }
{ "part": "label", "member": "interaction.visible" }
{ "part": "label", "name": "show" }
{ "part": "value_display", "name": "value_rendered" }
</code></pre>

<h3>8.5 Address Validity</h3>

<p>
A widget member or event address is valid only if all of the following hold:
</p>

<ul>
  <li>the referenced widget exists in <code>front_panel</code>,</li>
  <li>the referenced part exists when <code>part</code> is specified,</li>
  <li>the addressed member or event exists for the widget class or widget-part contract under the active profile,</li>
  <li>the addressed surface is compatible with the interaction kind being used,</li>
  <li>the relevant access or observation mode is allowed in the current context,</li>
  <li>profile and host requirements are satisfied when such gates exist.</li>
</ul>

<p>
For example:
</p>

<ul>
  <li><code>frog.ui.property_read</code> MUST target a readable property,</li>
  <li><code>frog.ui.property_write</code> MUST target a writable property,</li>
  <li><code>frog.ui.method_invoke</code> MUST target an invocable method,</li>
  <li><code>frog.ui.event_observe</code> MUST target an observable event.</li>
</ul>

<p>
These legality rules are owned by the corresponding widget class contract, not by this document.
</p>

<hr/>

<h2 id="interaction-node-families">9. Interaction Node Families</h2>

<p>
Widget object-style and event-oriented interactions are represented canonically as <code>primitive</code> nodes in the diagram.
</p>

<p>
The standardized primitive identifiers used by this source model are:
</p>

<ul>
  <li><code>frog.ui.property_read</code></li>
  <li><code>frog.ui.property_write</code></li>
  <li><code>frog.ui.method_invoke</code></li>
  <li><code>frog.ui.event_observe</code></li>
</ul>

<p>
Each widget interaction node MUST satisfy all of the following source-facing constraints:
</p>

<ul>
  <li><code>kind</code> MUST be <code>primitive</code>,</li>
  <li><code>type</code> MUST be one of the standardized primitive identifiers above,</li>
  <li>the node MUST consume a widget reference through required input port <code>ref</code>,</li>
  <li>property interaction nodes MUST declare <code>widget_member</code>,</li>
  <li>method interaction nodes MUST declare <code>widget_method</code>,</li>
  <li>event interaction nodes MUST declare <code>widget_event</code>.</li>
</ul>

<p>
The full primitive signatures, including required and optional ports beyond the source-facing reference model, are owned elsewhere.
</p>

<p>
This document standardizes which interaction families exist at source level and how they attach to widget references.
It does not redefine the intrinsic primitive library as such.
</p>

<hr/>

<h2 id="property-read">10. Property Read</h2>

<p>
A property read interaction represents a read of a property value from a widget or widget part into the diagram.
</p>

<h3>10.1 Standard Primitive Identifier</h3>

<pre><code>frog.ui.property_read
</code></pre>

<h3>10.2 Required Structural Fields</h3>

<pre><code>{
  "id": "read_gain_visible",
  "kind": "primitive",
  "type": "frog.ui.property_read",
  "widget_member": {
    "member": "interaction.visible"
  }
}
</code></pre>

<h3>10.3 Representation Rules</h3>

<ul>
  <li>the node MUST consume a compatible widget reference through <code>ref</code>,</li>
  <li>the node MUST declare <code>widget_member</code>,</li>
  <li>the addressed member MUST be a readable property under the applicable widget class contract and active profile.</li>
</ul>

<h3>10.4 Access to Primary Value as a Property</h3>

<p>
Reading <code>{ "member": "value" }</code> through <code>frog.ui.property_read</code> is valid only when the widget class contract exposes <code>value</code> as a readable property.
</p>

<p>
However, when the intent is ordinary value flow, tools SHOULD prefer the natural <code>widget_value</code> representation.
</p>

<h3>10.5 Access to Presentation Properties</h3>

<p>
Reading a presentation property such as <code>{ "member": "style.text_color" }</code> is valid only when the widget class contract exposes that member as readable.
</p>

<p>
Such a read remains object-style access.
It MUST NOT be confused with the natural primary-value path.
</p>

<hr/>

<h2 id="property-write">11. Property Write</h2>

<p>
A property write interaction represents a write of a diagram value into a writable widget or widget-part property.
</p>

<h3>11.1 Standard Primitive Identifier</h3>

<pre><code>frog.ui.property_write
</code></pre>

<h3>11.2 Required Structural Fields</h3>

<pre><code>{
  "id": "write_gain_label",
  "kind": "primitive",
  "type": "frog.ui.property_write",
  "widget_member": {
    "part": "label",
    "member": "text"
  }
}
</code></pre>

<h3>11.3 Representation Rules</h3>

<ul>
  <li>the node MUST consume a compatible widget reference through <code>ref</code>,</li>
  <li>the node MUST declare <code>widget_member</code>,</li>
  <li>the addressed member MUST be a writable property under the applicable widget class contract and active profile.</li>
</ul>

<h3>11.4 Access to Primary Value as a Property</h3>

<p>
Writing <code>{ "member": "value" }</code> through <code>frog.ui.property_write</code> is valid only when the widget class contract exposes <code>value</code> as a writable property.
</p>

<p>
This object-style write MUST remain semantically distinct from the natural primary value path represented by <code>widget_value</code>.
For ordinary widget primary-value participation in dataflow, tools SHOULD prefer the natural path.
</p>

<h3>11.5 Access to Presentation Properties</h3>

<p>
Writing a presentation property such as <code>{ "member": "style.text_color" }</code> is valid only when the widget class contract exposes that member as writable.
</p>

<p>
A presentation-property write MAY affect host-visible appearance.
It MUST NOT by itself redefine the executable meaning of the program.
</p>

<h3>11.6 Presentation Asset Reference Boundary</h3>

<p>
When a widget surface exposes a source-owned presentation-oriented reference member,
runtime object-style writes are valid only if the corresponding class contract explicitly declares that member runtime-writable.
</p>

<p>
By default, source-owned presentation references do not become runtime-writable executable state merely because they appear in source.
</p>

<hr/>

<h2 id="method-invoke">12. Method Invoke</h2>

<p>
A method invoke interaction represents a method call on a widget or widget part.
</p>

<h3>12.1 Standard Primitive Identifier</h3>

<pre><code>frog.ui.method_invoke
</code></pre>

<h3>12.2 Required Structural Fields</h3>

<pre><code>{
  "id": "invoke_reset",
  "kind": "primitive",
  "type": "frog.ui.method_invoke",
  "widget_method": {
    "name": "reset_to_default"
  }
}
</code></pre>

<h3>12.3 Representation Rules</h3>

<ul>
  <li>the node MUST consume a compatible widget reference through <code>ref</code>,</li>
  <li>the node MUST declare <code>widget_method</code>,</li>
  <li>the addressed member MUST be an invocable method under the applicable widget class contract and active profile.</li>
</ul>

<h3>12.4 Method Invocation and Parts</h3>

<p>
Part-scoped method invocation is represented by including <code>part</code> in the method descriptor.
</p>

<pre><code>{
  "id": "invoke_label_show",
  "kind": "primitive",
  "type": "frog.ui.method_invoke",
  "widget_method": {
    "part": "label",
    "name": "show"
  }
}
</code></pre>

<p>
Whether the targeted part exists, and whether the named method is invocable on that part,
depends on the corresponding widget class contract.
</p>

<p>
Method invocation remains object interaction, not value participation.
Even when a method affects widget appearance or state, it does not become a substitute for <code>widget_value</code>.
</p>

<hr/>

<h2 id="event-observe">13. Event Observe</h2>

<p>
An event observation interaction represents source-visible observation of a declared widget or widget-part event.
</p>

<h3>13.1 Standard Primitive Identifier</h3>

<pre><code>frog.ui.event_observe
</code></pre>

<h3>13.2 Required Structural Fields</h3>

<pre><code>{
  "id": "observe_value_rendered",
  "kind": "primitive",
  "type": "frog.ui.event_observe",
  "widget_event": {
    "name": "value_rendered"
  }
}
</code></pre>

<h3>13.3 Representation Rules</h3>

<ul>
  <li>the node MUST consume a compatible widget reference through <code>ref</code>,</li>
  <li>the node MUST declare <code>widget_event</code>,</li>
  <li>the addressed surface MUST be an observable event under the applicable widget class contract and active profile.</li>
</ul>

<h3>13.4 Event Observation and Parts</h3>

<p>
Part-scoped event observation is represented by including <code>part</code> in the event descriptor.
</p>

<pre><code>{
  "id": "observe_value_display_rendered",
  "kind": "primitive",
  "type": "frog.ui.event_observe",
  "widget_event": {
    "part": "value_display",
    "name": "value_rendered"
  }
}
</code></pre>

<p>
Whether the targeted part exists, and whether the named event is observable on that part,
depends on the corresponding widget class contract.
</p>

<h3>13.5 Event Payload Boundary</h3>

<p>
The event payload, if any, is governed by the event declaration in the widget class contract
and by the primitive-level output contract in <code>Libraries/UI.md</code>.
</p>

<p>
An event payload is neither:
</p>

<ul>
  <li>an ordinary property,</li>
  <li>the widget primary value by default,</li>
  <li>a substitute for widget reference identity.</li>
</ul>

<p>
Event observation therefore introduces an explicit observable occurrence into the diagram.
It does not collapse the event into a property read and does not collapse the payload into the natural value path unless another explicit contract says so.
</p>

<hr/>

<h2 id="typing-sequencing-and-effect-boundaries">14. Typing, Sequencing, and Effect Boundaries</h2>

<p>
Typing rules for widget interaction nodes are not owned entirely by this document.
</p>

<p>
They depend on:
</p>

<ul>
  <li>the FROG type system defined in <code>Type.md</code>,</li>
  <li>the member and event types declared by the widget class contract,</li>
  <li>the primitive-level typing rules owned by <code>Libraries/UI.md</code>.</li>
</ul>

<p>
Similarly, explicit sequencing through <code>ui_in</code> and <code>ui_out</code> is not owned entirely by this document.
</p>

<p>
This document only states that widget interaction nodes MAY participate in explicit UI sequencing when the underlying primitive contract supports it.
</p>

<p>
The following distinction MUST remain explicit:
</p>

<pre><code>member legality
  !=
event legality
  !=
primitive signature
  !=
primitive-local execution semantics
</code></pre>

<p>
Respectively:
</p>

<ul>
  <li>member and event legality belong to the widget class contract layer,</li>
  <li>primitive signature belongs to the intrinsic UI library layer,</li>
  <li>primitive-local execution semantics belong to the intrinsic UI library layer together with the relevant execution-semantics layers.</li>
</ul>

<p>
The following distinction MUST also remain explicit:
</p>

<pre><code>ordinary typed value ports
  !=
widget reference ports
  !=
event payload ports
  !=
UI sequencing ports
</code></pre>

<p>
Accordingly:
</p>

<ul>
  <li>ordinary value ports are governed by <code>Type.md</code>,</li>
  <li><code>ref</code> ports carry widget-reference tokens defined by the widget interaction model and primitive contracts,</li>
  <li>event payload ports, when present, carry event payload values defined by event contracts and primitive contracts,</li>
  <li><code>ui_in</code> and <code>ui_out</code> carry opaque sequencing tokens used only for explicit UI ordering when supported.</li>
</ul>

<p>
The following distinction MUST also remain explicit:
</p>

<pre><code>semantic primary value
  !=
presentation property
  !=
presentation reference
  !=
event payload
</code></pre>

<p>
Accordingly, a diagram that writes <code>style.text_color</code> is not equivalent to a diagram that writes the widget primary value,
and a source-owned presentation reference is not by itself an executable value-flow edge.
Likewise, an observed event payload is not automatically the widget primary value unless the event contract explicitly says so.
</p>

<hr/>

<h2 id="diagram-representation">15. Diagram Representation</h2>

<p>
Widget interaction nodes appear inside the authoritative executable <code>diagram</code> as primitive nodes.
</p>

<p>
Conceptually:
</p>

<pre><code>front_panel widget instance
        |
        +-- widget_value -----------------&gt; ordinary value flow
        |
        +-- widget_reference ------------&gt; object-style and event-oriented interaction
                                             |
                                             +-- frog.ui.property_read
                                             +-- frog.ui.property_write
                                             +-- frog.ui.method_invoke
                                             +-- frog.ui.event_observe
</code></pre>

<p>
The interaction node itself does not carry widget identity directly.
Widget identity is supplied through the incoming <code>ref</code> edge from a <code>widget_reference</code> node.
</p>

<p>
This separation is intentional:
</p>

<ul>
  <li><code>widget_reference</code> identifies the target widget instance,</li>
  <li>the interaction primitive identifies the kind of access or observation,</li>
  <li>the local descriptor identifies the targeted member or event,</li>
  <li>the widget class contract determines whether that access or observation is legal.</li>
</ul>

<p>
Source-owned instance metadata remains outside this executable interaction chain unless:
</p>

<ul>
  <li>it is explicitly addressed through a legal property read or property write, or</li>
  <li>another specification explicitly grants it executable interaction meaning.</li>
</ul>

<p>
This keeps the diagram authoritative without forcing the front panel to absorb executable semantics.
The front panel declares the widget.
The diagram declares the interaction.
</p>

<hr/>

<h2 id="validation-rules">16. Validation Rules</h2>

<p>
Validators MUST enforce at least the following rules:
</p>

<ul>
  <li>a widget interaction node MUST be a <code>primitive</code> node,</li>
  <li>its <code>type</code> MUST be one of the standardized widget interaction primitive identifiers,</li>
  <li>it MUST consume a compatible widget reference through <code>ref</code>,</li>
  <li>property interaction nodes MUST declare <code>widget_member</code>,</li>
  <li>method interaction nodes MUST declare <code>widget_method</code>,</li>
  <li>event interaction nodes MUST declare <code>widget_event</code>,</li>
  <li>the referenced widget MUST exist,</li>
  <li>the referenced part MUST exist when a part is specified,</li>
  <li>the addressed member or event MUST exist on the targeted class or targeted part contract,</li>
  <li>the requested access or observation mode MUST be legal in the current context,</li>
  <li>profile gates and host gates MUST be respected when present.</li>
</ul>

<p>
Validators SHOULD diagnose at least the following error classes:
</p>

<ul>
  <li>missing widget reference input,</li>
  <li>invalid primitive kind,</li>
  <li>unknown widget interaction primitive identifier,</li>
  <li>missing <code>widget_member</code>, <code>widget_method</code>, or <code>widget_event</code> descriptor,</li>
  <li>unknown widget instance,</li>
  <li>unknown widget part,</li>
  <li>unknown member,</li>
  <li>unknown event,</li>
  <li>read of non-readable property,</li>
  <li>write to non-writable property,</li>
  <li>invocation of non-invocable method,</li>
  <li>observation of non-observable event,</li>
  <li>use of profile-gated member outside the required profile,</li>
  <li>use of host-gated member or event without required host capability,</li>
  <li>attempt to use object-style access where only natural value access is intended,</li>
  <li>treating a widget reference token as an ordinary typed value,</li>
  <li>treating a sequencing port as an ordinary typed value port,</li>
  <li>treating a non-semantic presentation property as the natural value path,</li>
  <li>treating an event payload as the widget natural value path without an explicit contract,</li>
  <li>treating a source-owned presentation reference as runtime-writable when the class contract forbids it,</li>
  <li>treating presentation-oriented metadata as executable semantic state without an explicit enabling specification.</li>
</ul>

<p>
These checks validate interaction form and interaction legality.
They do not redefine the full execution semantics of the targeted primitives.
</p>

<hr/>

<h2 id="source-to-library-mapping">17. Source-to-Library Mapping</h2>

<p>
The source-facing interaction model standardized here maps to intrinsic UI primitives in the <code>frog.ui.*</code> namespace.
</p>

<p>
At minimum:
</p>

<ul>
  <li>source-level property read maps to <code>frog.ui.property_read</code>,</li>
  <li>source-level property write maps to <code>frog.ui.property_write</code>,</li>
  <li>source-level method invocation maps to <code>frog.ui.method_invoke</code>,</li>
  <li>source-level event observation maps to <code>frog.ui.event_observe</code>.</li>
</ul>

<p>
This document standardizes the source-facing use of those primitives, not their full primitive definitions.
</p>

<p>
The natural primary-value path remains represented by <code>widget_value</code>, not by those object-style or event-oriented interaction primitives.
</p>

<p>
This mapping is deliberate:
the interaction document defines the source contract of usage,
while <code>Libraries/UI.md</code> defines the intrinsic primitive surface that realizes that contract.
</p>

<hr/>

<h2 id="examples">18. Examples</h2>

<h3>18.1 Read widget property</h3>

<pre><code>{
  "id": "read_gain_visible",
  "kind": "primitive",
  "type": "frog.ui.property_read",
  "widget_member": {
    "member": "interaction.visible"
  }
}
</code></pre>

<h3>18.2 Write part property</h3>

<pre><code>{
  "id": "write_gain_label",
  "kind": "primitive",
  "type": "frog.ui.property_write",
  "widget_member": {
    "part": "label",
    "member": "text"
  }
}
</code></pre>

<h3>18.3 Invoke widget method</h3>

<pre><code>{
  "id": "invoke_reset",
  "kind": "primitive",
  "type": "frog.ui.method_invoke",
  "widget_method": {
    "name": "reset_to_default"
  }
}
</code></pre>

<h3>18.4 Invoke part method</h3>

<pre><code>{
  "id": "invoke_label_show",
  "kind": "primitive",
  "type": "frog.ui.method_invoke",
  "widget_method": {
    "part": "label",
    "name": "show"
  }
}
</code></pre>

<h3>18.5 Observe widget event</h3>

<pre><code>{
  "id": "observe_rendered",
  "kind": "primitive",
  "type": "frog.ui.event_observe",
  "widget_event": {
    "name": "value_rendered"
  }
}
</code></pre>

<h3>18.6 Observe part event</h3>

<pre><code>{
  "id": "observe_value_display_rendered",
  "kind": "primitive",
  "type": "frog.ui.event_observe",
  "widget_event": {
    "part": "value_display",
    "name": "value_rendered"
  }
}
</code></pre>

<h3>18.7 Natural value access</h3>

<pre><code>widget_value(ctrl_count)
    -&gt; u16
</code></pre>

<p>
This is the preferred representation for ordinary primary-value participation in dataflow.
</p>

<h3>18.8 Object-style presentation write</h3>

<pre><code>widget_reference(ctrl_count)
    -&gt; frog.ui.property_write {
         "member": "style.text_color"
       }
</code></pre>

<p>
This is valid only if the active widget class contract declares <code>style.text_color</code> as a writable property.
</p>

<h3>18.9 Event observation</h3>

<pre><code>widget_reference(count_indicator)
    -&gt; frog.ui.event_observe {
         "name": "value_rendered"
       }
</code></pre>

<p>
This is valid only if the active widget class contract declares <code>value_rendered</code> as an observable event.
</p>

<h3>18.10 Invalid collapse example</h3>

<pre><code>widget_value(ctrl_count)
    -&gt; interpreted as style.text_color
</code></pre>

<p>
This is invalid.
The natural value path MUST NOT be reinterpreted as a presentation-property path.
</p>

<h3>18.11 Presentation reference boundary example</h3>

<pre><code>{
  "id": "ctrl_count",
  "role": "control",
  "class_ref": "frog.widgets.numeric_control",
  "value_type": "u16",
  "props": {
    "face_asset_ref": {
      "kind": "resource_ref",
      "path": "./assets/widgets/u16_numeric_control_face.svg"
    }
  }
}
</code></pre>

<p>
This source-owned presentation reference does not by itself create an executable value-flow edge.
It remains source-owned instance metadata unless another specification explicitly grants additional meaning.
</p>

<hr/>

<h2 id="out-of-scope">19. Out of Scope</h2>

<p>
The following are intentionally out of scope for this document:
</p>

<ul>
  <li>a complete event registration and subscription lifecycle model,</li>
  <li>asynchronous UI message delivery guarantees,</li>
  <li>general-purpose widget references as ordinary first-class user values,</li>
  <li>one mandatory UI runtime architecture,</li>
  <li>the full execution semantics of every UI interaction primitive,</li>
  <li>the complete effect model for all UI runtimes and profiles,</li>
  <li>automatic interpretation of presentation assets as executable semantics,</li>
  <li>one mandatory host rendering system for presentation assets.</li>
</ul>

<hr/>

<h2 id="status">20. Status</h2>

<p>
This document defines the canonical source-level widget interaction corridor for the current repository version.
</p>

<p>
Version governance and the current applicable specification version are centralized in <code>Versioning/Readme.md</code>.
</p>

<p>
The closure direction for this document is:
</p>

<ul>
  <li>clear separation between natural value flow and object-style interaction,</li>
  <li>explicit event observation,</li>
  <li>stable addressing of widget properties, methods, events, and parts,</li>
  <li>alignment with <code>.wfrog</code>-published widget class contracts.</li>
</ul>

<hr/>

<h2 id="license">21. License</h2>

<p>
See the repository-level license information for the licensing terms governing this specification.
</p>
