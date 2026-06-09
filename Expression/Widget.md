<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Widget Specification</h1>

<p align="center">
  <strong>Normative definition of widget instances in canonical <code>.frog</code> source</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#why-this-document-exists">2. Why This Document Exists</a></li>
  <li><a href="#scope">3. Scope</a></li>
  <li><a href="#relation-with-other-specifications">4. Relation with Other Specifications</a></li>
  <li><a href="#what-a-widget-is">5. What a Widget Is</a></li>
  <li><a href="#architectural-position">6. Architectural Position</a></li>
  <li><a href="#widget-class-vs-value-type">7. Widget Class vs Value Type</a></li>
  <li><a href="#widget-roles">8. Widget Roles</a></li>
  <li><a href="#widget-identity-and-lifecycle">9. Widget Identity and Lifecycle</a></li>
  <li><a href="#ownership-boundary">10. Ownership Boundary</a></li>
  <li><a href="#canonical-widget-instance-shape">11. Canonical Widget Instance Shape</a></li>
  <li><a href="#value-carrying-widgets">12. Value-Carrying Widgets</a></li>
  <li><a href="#widget-reference-model">13. Widget Reference Model</a></li>
  <li><a href="#package-references-and-externalization">14. Package References and Externalization</a></li>
  <li><a href="#source-owned-instance-metadata">15. Source-Owned Instance Metadata</a></li>
  <li><a href="#widget-parts">16. Widget Parts</a></li>
  <li><a href="#properties-methods-and-events">17. Properties, Methods, and Events</a></li>
  <li><a href="#serialization-vs-runtime-state">18. Serialization vs Runtime State</a></li>
  <li><a href="#addressing-model">19. Addressing Model</a></li>
  <li><a href="#validation-rules">20. Validation Rules</a></li>
  <li><a href="#examples">21. Examples</a></li>
  <li><a href="#out-of-scope">22. Out of Scope</a></li>
  <li><a href="#summary">23. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
The FROG front panel is composed of widget instances.
A widget instance is a canonical source-level UI object with stable identity, a declared widget class reference, a declared role, optional primary value semantics, optional source-owned instance metadata, and explicit participation paths toward executable interaction.
</p>

<p>
A widget is not merely a graphical shape.
It is not merely a runtime-private control handle.
It is not itself an executable diagram node.
A widget is a canonical source object that MAY:
</p>

<ul>
  <li>carry a typed primary value,</li>
  <li>accept user input,</li>
  <li>display program state,</li>
  <li>own named parts,</li>
  <li>own child widgets when it is a container,</li>
  <li>expose properties,</li>
  <li>expose methods,</li>
  <li>emit runtime events,</li>
  <li>reference external widget-oriented packages,</li>
  <li>carry source-owned instance metadata.</li>
</ul>

<p>
This document defines the widget instance model used by canonical FROG source.
It standardizes widget instance identity, widget roles, class references at instance level, value-carrying semantics at instance level, source-owned widget configuration, and the distinction between natural value participation and object-style widget access.
</p>

<p>
FROG distinguishes two interaction paths for widgets:
</p>

<ul>
  <li><strong>Natural value path</strong> — the primary value of a value-carrying widget participates in diagram dataflow through <code>widget_value</code>.</li>
  <li><strong>Object-style path</strong> — explicit widget-object access occurs through <code>widget_reference</code> plus widget interaction primitives.</li>
</ul>

<p>
These paths are related but distinct.
The natural value path is the canonical representation for ordinary dataflow participation.
The object-style path is the canonical representation for explicit reads, writes, method invocation, part-scoped access, and richer UI-object interactions.
</p>

<p>
This document defines widget instances as source objects in <code>.frog</code>.
It does not define the full class-side member law for each widget class.
It does not define runtime-private host realization internals.
It does not make SVG, theme files, skins, or host-native controls the normative source of widget semantics.
</p>

<hr/>

<h2 id="why-this-document-exists">2. Why This Document Exists</h2>

<p>
FROG requires a widget system that remains source-visible, portable, inspectable, and compatible with multiple runtimes.
That requires a disciplined separation between:
</p>

<ul>
  <li>canonical widget instances in <code>.frog</code>,</li>
  <li>class-level widget law,</li>
  <li>external widget-oriented package content,</li>
  <li>diagram-side widget interaction,</li>
  <li>runtime interpretation,</li>
  <li>host realization,</li>
  <li>visual assets such as SVG.</li>
</ul>

<p>
Without that separation, widget meaning drifts into runtime-private code, host-private rendering logic, or visual assets that were never meant to own semantics.
This document therefore fixes the source-owned widget instance layer and leaves the other layers to their proper specifications.
</p>

<p>
Its purpose is not to define the whole widget ecosystem in one place.
Its purpose is to define what a widget instance is in canonical source, what canonical source may say about that instance, and where canonical source must stop.
</p>

<hr/>

<h2 id="scope">3. Scope</h2>

<p>
This document standardizes:
</p>

<ul>
  <li>the widget instance model in canonical <code>.frog</code> source,</li>
  <li>widget roles,</li>
  <li>widget instance identity requirements,</li>
  <li>value-carrying widget instance requirements,</li>
  <li>widget references as object-style access anchors,</li>
  <li>instance-level source-owned metadata,</li>
  <li>source-level rules for references to external widget-oriented packages.</li>
</ul>

<p>
This document does not attempt to define a complete industrial UI toolkit.
Its purpose is to define a durable instance-side baseline that can support executable vertical slices and later layered evolution toward richer widget systems.
</p>

<p>
This document keeps the following architectural split explicit:
</p>

<ul>
  <li><code>Widget.md</code> defines widget instances as source objects,</li>
  <li><code>Widget class contract.md</code> defines class-side member legality and class-side contracts,</li>
  <li><code>Widget interaction.md</code> defines diagram-side executable access,</li>
  <li><code>Front panel.md</code> defines front-panel composition and placement rules,</li>
  <li><code>Widget package (.wfrog).md</code> defines the package-level model for external widget-oriented artifacts,</li>
  <li><code>Widget behavior.md</code> defines bounded behavior doctrine,</li>
  <li><code>Widget realization.md</code> defines realization doctrine and visual-resource boundaries.</li>
</ul>

<p>
In the base architecture:
</p>

<ul>
  <li>ordinary widget values are governed by <code>Type.md</code>,</li>
  <li>widget references are object-access anchors rather than ordinary user-declared value types,</li>
  <li>front-panel layout remains distinct from diagram execution,</li>
  <li>source-owned instance metadata remains distinct from executable semantic law,</li>
  <li>standard primitive widget families belong to <code>Libraries/</code>, not to this document.</li>
</ul>

<hr/>

<h2 id="relation-with-other-specifications">4. Relation with Other Specifications</h2>

<ul>
  <li><code>Type.md</code> defines the type system used by value-carrying widgets and typed widget members.</li>
  <li><code>Front panel.md</code> defines front-panel composition, placement, recursive ownership, and layout serialization.</li>
  <li><code>Widget class contract.md</code> defines class-side member contracts, part legality, access legality, capability gating, and object-surface rules that apply to class references used by widget instances.</li>
  <li><code>Widget interaction.md</code> defines executable diagram-side access through <code>widget_reference</code>, <code>frog.ui.property_read</code>, <code>frog.ui.property_write</code>, <code>frog.ui.method_invoke</code>, and related interaction primitives.</li>
  <li><code>Diagram.md</code> defines the executable graph and the node kinds that materialize widget participation in that graph.</li>
  <li><code>Interface.md</code> defines the public program contract and remains distinct from front-panel widgets.</li>
  <li><code>Widget package (.wfrog).md</code> defines the package model for external widget-oriented artifacts referenced by canonical source.</li>
  <li><code>Widget behavior.md</code> defines the bounded behavior doctrine applicable to widget classes and widget packages.</li>
  <li><code>Widget realization.md</code> defines host-facing realization posture and the boundary between semantics and visual realization resources.</li>
  <li><code>Libraries/UI.md</code> defines the standard primitive UI interaction surface and standardized primitive families exposed by the language profile.</li>
  <li><code>Versioning/Readme.md</code> defines centralized version-governance doctrine for the published corpus and source-format compatibility boundaries.</li>
</ul>

<p>
This document defines what a widget instance is in canonical source.
It does not redefine:
</p>

<ul>
  <li>general execution semantics,</li>
  <li>public interface semantics,</li>
  <li>the complete class-side contract surface of every widget class,</li>
  <li>the full realization model of any runtime host,</li>
  <li>the full package grammar beyond what source-level references need to express,</li>
  <li>the standardized widget catalog owned by <code>Libraries/</code>.</li>
</ul>

<hr/>

<h2 id="what-a-widget-is">5. What a Widget Is</h2>

<p>
A widget is an instance of a widget class.
A widget instance exists in canonical source and is owned by the front panel of a FROG program.
</p>

<p>
A widget instance MUST have stable source identity.
Different tools MAY realize the same widget instance differently, but they MUST preserve its canonical source meaning.
</p>

<p>
A widget MAY:
</p>

<ul>
  <li>carry a typed primary value,</li>
  <li>accept or display data,</li>
  <li>own child widgets if it is a container,</li>
  <li>own named parts,</li>
  <li>expose configurable source-owned metadata,</li>
  <li>reference class-side contracts and external widget-oriented packages,</li>
  <li>participate in diagram execution through value and object-style paths.</li>
</ul>

<p>
Canonical widget serialization stores source-relevant widget configuration only.
It MUST NOT depend on arbitrary runtime-only state.
</p>

<p>
A widget is not a public interface port.
A widget is not itself a primitive node.
A widget is not a host-native control definition.
A widget is a source-level UI object that may participate in execution through diagram constructs defined elsewhere.
</p>

<hr/>

<h2 id="architectural-position">6. Architectural Position</h2>

<p>
The widget instance model exists in the middle of a larger architectural stack.
It is neither the whole widget system nor a minor rendering detail.
</p>

<p>
The required distinction is:
</p>

<pre><code>canonical widget instance
    !=
widget class law
    !=
widget-oriented package content
    !=
bounded widget behavior
    !=
runtime-private widget structure
    !=
host rendering implementation
    !=
SVG visual asset</code></pre>

<p>
The source instance identifies the widget and persists the author-owned instance data.
The class contract defines which object surfaces are legal.
The <code>.wfrog</code> package layer carries machine-readable widget-oriented package content.
The behavior layer constrains what package-published widget behavior may mean.
The runtime interprets that content.
The host realizes the live object.
SVG may contribute visual resources, but it does not become the owner of widget meaning.
</p>

<p>
This separation is mandatory because FROG must remain inspectable and portable across runtimes.
No single runtime implementation may become the hidden definition of widget law.
</p>

<hr/>

<h2 id="widget-class-vs-value-type">7. Widget Class vs Value Type</h2>

<h3>7.1 Widget class reference</h3>

<p>
Every widget instance MUST declare a widget class reference.
That class reference identifies the UI category of the widget instance.
Examples:
</p>

<pre><code>frog.widgets.numeric_control
frog.widgets.numeric_indicator
frog.widgets.boolean_indicator
frog.widgets.panel</code></pre>

<p>
At widget-instance level, the declared class reference determines at least:
</p>

<ul>
  <li>the category of UI object the instance belongs to,</li>
  <li>the compatible roles,</li>
  <li>whether a primary value is expected,</li>
  <li>whether containment is allowed,</li>
  <li>which class-side contract applies to object-style access.</li>
</ul>

<p>
The full member surface of the class, including legal properties, methods, events, parts, access modes, and capability gates, belongs normatively to <code>Widget class contract.md</code> and to any associated external widget-oriented class package when such a package is standardized.
</p>

<h3>7.2 Value type</h3>

<p>
A value type identifies the FROG data type carried by a value-carrying widget.
Examples:
</p>

<pre><code>bool
u16
i32
f64
string
array&lt;f64&gt;</code></pre>

<p>
Value types are defined normatively by <code>Type.md</code>.
</p>

<h3>7.3 Consequence</h3>

<p>
A widget is not a type, and a type is not a widget.
</p>

<ul>
  <li><code>u16</code> is a value type,</li>
  <li><code>frog.widgets.numeric_control</code> is a widget class reference,</li>
  <li>a widget instance may be a numeric control carrying a value of type <code>u16</code>.</li>
</ul>

<h3>7.4 Widget reference distinction</h3>

<p>
A widget reference is not a widget class and is not an ordinary value type expression.
It is an object-style access anchor materialized in the diagram by <code>widget_reference</code> and consumed by widget interaction primitives.
</p>

<hr/>

<h2 id="widget-roles">8. Widget Roles</h2>

<p>
Every widget instance MUST declare a role.
</p>

<p>
FROG defines the following standard roles at the architectural level:
</p>

<ul>
  <li><code>control</code></li>
  <li><code>indicator</code></li>
  <li><code>container</code></li>
  <li><code>decoration</code></li>
</ul>

<h3>8.1 <code>control</code></h3>

<p>
A <code>control</code> is a value-carrying widget intended primarily for user-editable interaction.
Its primary value normally flows from front panel toward executable logic.
</p>

<h3>8.2 <code>indicator</code></h3>

<p>
An <code>indicator</code> is a value-carrying widget intended primarily to display program state.
Its primary value normally flows from executable logic toward the front panel.
</p>

<h3>8.3 <code>container</code></h3>

<p>
A <code>container</code> owns child widgets and defines a compositional UI region.
Containers organize structure and ownership even when they do not expose a primary value.
</p>

<h3>8.4 <code>decoration</code></h3>

<p>
A <code>decoration</code> is used primarily for presentation rather than primary program value exchange.
</p>

<h3>8.5 Role compatibility</h3>

<p>
The declared role MUST be compatible with the declared widget class reference.
A tool MUST NOT accept an arbitrary role/class combination unless the active profile explicitly defines it as valid.
</p>

<p>
The legality of a role/class combination is a class-side constraint and therefore depends on the corresponding widget class contract.
</p>

<hr/>

<h2 id="widget-identity-and-lifecycle">9. Widget Identity and Lifecycle</h2>

<p>
FROG distinguishes at least three conceptual levels:
</p>

<ul>
  <li>widget class — the category definition,</li>
  <li>widget instance — the serialized source instance in the front panel,</li>
  <li>runtime widget object — the live object created by a runtime and realized by a host.</li>
</ul>

<p>
Conceptually:
</p>

<pre><code>Widget Class
    ↓ referenced by
Widget Instance (source)
    ↓ interpreted as
Runtime Widget Object
    ↓ realized by
Host UI Object</code></pre>

<p>
The source instance defines author-owned design-time configuration.
The runtime widget object manages live value, transient UI state, package interpretation results, host bindings, and renderer-private state.
</p>

<p>
Widget identifiers MUST remain unique across the entire recursive front-panel widget tree.
This guarantees that diagram-side widget addressing remains unambiguous.
</p>

<p>
The source identity of a widget MUST remain stable enough to support:
</p>

<ul>
  <li>diagram-side <code>widget_value</code> references,</li>
  <li>diagram-side <code>widget_reference</code> references,</li>
  <li>editor tracking,</li>
  <li>tool interoperability,</li>
  <li>canonical source diffs and merges.</li>
</ul>

<p>
This document standardizes source identity and source ownership.
It does not standardize one mandatory runtime object layout or one mandatory host lifecycle implementation.
</p>

<hr/>

<h2 id="ownership-boundary">10. Ownership Boundary</h2>

<p>
The widget instance model exists to make ownership boundaries explicit.
At minimum:
</p>

<ul>
  <li><strong><code>.frog</code> owns</strong> widget instances, instance identity, instance role, instance value typing, instance-level source-owned configuration, composition, and references to external widget-oriented packages.</li>
  <li><strong>Widget class contracts own</strong> member legality, part legality, access legality, and class-side semantic surface.</li>
  <li><strong><code>.wfrog</code> packages own</strong> machine-readable widget-oriented package content such as class-package content, composite widget publication, realization resources, bounded behavior publication, and widget-oriented bundles.</li>
  <li><strong>Widget behavior doctrine owns</strong> the rules under which widget behavior may be published, interpreted, and kept portable.</li>
  <li><strong>Runtime implementations own</strong> live objects, event-loop integration, host control handles, redraw scheduling, focus state, renderer-private caches, and other runtime-private structures.</li>
  <li><strong>Host realization layers own</strong> the concrete rendering strategy, native control binding strategy, and host-private optimization choices.</li>
  <li><strong>Visual assets such as SVG own</strong> visual form, named visual anchors, and scalable graphics, but not normative widget semantics.</li>
</ul>

<p>
Accordingly:
</p>

<pre><code>widget instance meaning
    MUST NOT be hidden in
runtime-private code, host rendering code, or SVG assets</code></pre>

<p>
Likewise, canonical source MUST NOT embed the full widget class law inside each widget instance.
The instance refers to a class; it does not replace the class contract.
</p>

<hr/>

<h2 id="canonical-widget-instance-shape">11. Canonical Widget Instance Shape</h2>

<p>
A canonical widget instance in source form SHOULD follow this general structure:
</p>

<pre><code>{
  "id": "ctrl_input",
  "role": "control",
  "class_ref": "frog.widgets.numeric_control",
  "instance_ref": "ctrl_input",
  "value_type": "u16",
  "layout": {
    "x": 20,
    "y": 24,
    "width": 140,
    "height": 32
  },
  "props": {
    "label": "Input value"
  },
  "parts": {},
  "package_refs": [
    "./ui/accumulator_panel.wfrog"
  ]
}</code></pre>

<p>
Common fields are defined below.
</p>

<h3>11.1 <code>id</code></h3>

<ul>
  <li>MUST be a string.</li>
  <li>MUST be unique across the full recursive front-panel widget tree.</li>
  <li>MUST remain stable enough to support source tracking, diagram references, and tool interoperability.</li>
</ul>

<h3>11.2 <code>role</code></h3>

<ul>
  <li>MUST be a valid widget role under the active specification profile.</li>
  <li>MUST be compatible with the declared widget class reference.</li>
</ul>

<h3>11.3 <code>class_ref</code></h3>

<ul>
  <li>MUST be a string.</li>
  <li>MUST identify a known widget class under the active specification profile.</li>
  <li>MUST be the normative field for widget class identity in canonical source.</li>
</ul>

<p>
For normalized source, <code>class_ref</code> is the canonical field because it makes the ownership boundary explicit: the instance refers to a class definition rather than embedding the class definition.
</p>

<h3>11.4 <code>instance_ref</code></h3>

<p>
A widget instance MAY declare an <code>instance_ref</code> when source wishes to preserve an explicit instance-level reference token distinct from other identifiers.
When omitted, tools MAY treat <code>id</code> as the effective instance reference under the active profile.
</p>

<h3>11.5 <code>value_type</code></h3>

<ul>
  <li>MUST exist for value-carrying widgets.</li>
  <li>MUST NOT be required for non-value widgets.</li>
  <li>MUST be a valid canonical FROG type expression according to <code>Type.md</code>.</li>
</ul>

<h3>11.6 <code>layout</code></h3>

<p>
<code>layout</code> stores design-time placement and size intent.
Its detailed interpretation belongs normatively to <code>Front panel.md</code>.
</p>

<p>
For front-panel widget instances, <code>layout.x</code> and
<code>layout.y</code> are the canonical coordinates of the widget
<code>placement_bounds</code> origin in the parent coordinate space. In the
standard v0.1 placement model, that origin is
<code>placement_bounds.top_left</code>. <code>layout.width</code> and
<code>layout.height</code> describe the same placement-bounds rectangle, not
the raw SVG viewBox, not an IDE selection overlay, and not a runtime-private
host control rectangle.
</p>

<h3>11.7 <code>props</code></h3>

<ul>
  <li>MUST be an object when present.</li>
  <li>MUST contain only source-relevant instance metadata and class-allowed instance-scoped property values.</li>
  <li>MAY contain inherited and class-specific source-owned members allowed by the widget class contract.</li>
</ul>

<p>
Whether a property is legal, readable, writable, design-time-owned, runtime-owned, or profile-gated depends on the corresponding widget class contract.
</p>

<h3>11.8 <code>parts</code></h3>

<ul>
  <li>MUST be an object when present.</li>
  <li>Keys MUST be stable part names.</li>
  <li>Values MUST be valid part objects for the owning widget class or active profile.</li>
</ul>

<p>
The available part surface and part-member legality belong normatively to the corresponding widget class contract.
</p>

<h3>11.9 <code>children</code></h3>

<ul>
  <li>MAY appear on container widgets.</li>
  <li>MUST be an array when present.</li>
  <li>MUST contain valid child widget instances.</li>
</ul>

<p>
Ownership, nesting, and composition rules belong normatively to <code>Front panel.md</code>.
</p>

<h3>11.10 <code>package_refs</code></h3>

<p>
A widget instance MAY reference external widget-oriented packages through <code>package_refs</code>.
Such references are source-owned references, not embedded semantic replacement.
</p>

<p>
Typical examples include references to:
</p>

<ul>
  <li>widget class packages,</li>
  <li>composite widget packages,</li>
  <li>realization-resource packages,</li>
  <li>widget-oriented bundles,</li>
  <li>profile-defined widget-oriented resources.</li>
</ul>

<p>
These references do not transfer ownership of instance semantics away from canonical source.
They identify additional artifacts used to interpret or realize the widget instance within the architectural boundaries defined elsewhere.
</p>

<h3>11.11 <code>style_ref</code> and <code>style_override</code></h3>

<p>
Widget instances MAY include visual style references and visual style overrides.
These fields remain presentation-related and MUST NOT alter widget semantics, public interface semantics, class-side member legality, or executable graph semantics.
</p>

<hr/>

<h2 id="value-carrying-widgets">12. Value-Carrying Widgets</h2>

<p>
Controls and indicators are value-carrying widgets.
</p>

<h3>12.1 Required value semantics</h3>

<ul>
  <li><code>value_type</code> MUST be defined.</li>
  <li>The widget class reference MUST support the declared type.</li>
  <li>The corresponding class contract MUST define a primary value semantic.</li>
</ul>

<h3>12.2 Primary value path</h3>

<p>
Every value-carrying widget conceptually exposes a primary <code>value</code> member.
That primary value is the natural dataflow path between the front panel and the diagram.
</p>

<pre><code>Front Panel Widget
        │
        ▼
widget.value
        │
        ▼
widget_value.value</code></pre>

<p>
In diagram source, this participation is represented by a <code>widget_value</code> node.
</p>

<h3>12.3 Directional intent</h3>

<ul>
  <li>a control normally provides a value to the diagram,</li>
  <li>an indicator normally receives a value from the diagram.</li>
</ul>

<p>
This directional intent does not redefine executable semantics by itself.
The diagram remains authoritative.
</p>

<h3>12.4 Access to <code>value</code> through the object model</h3>

<p>
The primary value MAY also be read or written through the object-style interaction path when the widget class contract exposes <code>value</code> as a readable or writable property.
</p>

<p>
That legality belongs to the widget class contract and MUST NOT be inferred solely from the fact that the widget is value-carrying.
</p>

<h3>12.5 Distinctions that MUST remain explicit</h3>

<pre><code>widget_value access
    !=
object-style property access to value</code></pre>

<p>
They may refer to related semantics, but they are not the same abstraction layer.
</p>

<p>
Likewise:
</p>

<pre><code>ordinary typed value
    !=
widget reference token</code></pre>

<hr/>

<h2 id="widget-reference-model">13. Widget Reference Model</h2>

<p>
A widget reference is the object-style access anchor used by the diagram to target a widget as an object rather than only as a natural value source or sink.
</p>

<p>
Conceptually:
</p>

<pre><code>widget_reference
    ↓
widget object
    ├── property read / write
    ├── method invocation
    └── part-scoped access</code></pre>

<p>
A <code>widget_reference</code> node identifies one widget instance by stable source identity.
</p>

<p>
A widget reference does not by itself define which members are legal.
It only anchors object-style access.
Member legality depends on the widget class contract, and executable access rules depend on <code>Widget interaction.md</code>.
</p>

<p>
In the base architecture, a widget reference is not an ordinary user-declared value type in canonical source.
It is an interaction token used by the diagram-side object-access model.
</p>

<hr/>

<h2 id="package-references-and-externalization">14. Package References and Externalization</h2>

<p>
Canonical <code>.frog</code> source MAY reference external widget-oriented artifacts.
This allows FROG to remain modular without forcing every class-side, behavior-side, and realization-side detail into the canonical program source.
</p>

<p>
The intended architectural direction is:
</p>

<ul>
  <li><code>.frog</code> owns widget instances,</li>
  <li>class-side widget law is defined outside the widget instance shape itself,</li>
  <li>machine-readable widget-oriented package content may be carried by <code>.wfrog</code>,</li>
  <li>runtime implementations interpret those packages without becoming the normative source of widget law.</li>
</ul>

<p>
A standardized widget-oriented format such as <code>.wfrog</code> MAY therefore be used for:
</p>

<ul>
  <li>widget class packages,</li>
  <li>composite widget packages,</li>
  <li>bounded behavior publication,</li>
  <li>realization-resource publication,</li>
  <li>widget-oriented bundles that combine multiple package roles under explicit package-kind rules.</li>
</ul>

<p>
This document does not define the full package grammar of such external formats.
It only defines that canonical widget instances MAY reference external widget-oriented packages and that doing so does not transfer ownership of widget-instance meaning away from <code>.frog</code>.
</p>

<hr/>

<h2 id="source-owned-instance-metadata">15. Source-Owned Instance Metadata</h2>

<p>
A widget instance MAY carry source-owned instance metadata that helps a designer, IDE, compiler, or runtime interpret or realize an authored front face.
This metadata belongs to canonical source serialization, but it does not by itself redefine widget semantics.
</p>

<p>
Typical examples include:
</p>

<ul>
  <li><code>label</code>,</li>
  <li>formatting posture,</li>
  <li>style references,</li>
  <li>theme references,</li>
  <li>other class-allowed source-owned instance members.</li>
</ul>

<h3>15.1 Semantic boundary</h3>

<p>
Source-owned instance metadata remains instance metadata.
It MUST NOT:
</p>

<ul>
  <li>change the widget class reference,</li>
  <li>change the widget role,</li>
  <li>change the widget primary value semantics,</li>
  <li>change public interface meaning,</li>
  <li>replace the diagram as the authoritative executable graph,</li>
  <li>replace the class-side widget contract.</li>
</ul>

<h3>15.2 Visual resources and templates</h3>

<p>
A widget instance MAY indirectly reference visual resources through package references, style references, or other profile-defined means.
However, a visual resource such as SVG remains a visual resource.
It may define skins, scalable graphics, anchors, and decorative surfaces, but it does not become the normative owner of dynamic widget values, dynamic widget text, or object-style widget behavior.
</p>

<p>
Accordingly, a host MAY:
</p>

<ul>
  <li>realize a preferred front face faithfully,</li>
  <li>substitute a compatible native rendering,</li>
  <li>ignore unsupported purely presentational details,</li>
  <li>but MUST preserve widget source meaning.</li>
</ul>

<hr/>

<h2 id="widget-parts">16. Widget Parts</h2>

<p>
A widget MAY expose named parts.
</p>

<p>
A part is a named object-owned sub-surface of a widget, such as:
</p>

<ul>
  <li>a label,</li>
  <li>an axis,</li>
  <li>a legend,</li>
  <li>a cursor,</li>
  <li>an increment button,</li>
  <li>a decrement button.</li>
</ul>

<p>
At instance level, parts allow canonical source to attach source-relevant data to sub-surfaces owned by the widget.
</p>

<p>
However:
</p>

<ul>
  <li>the existence of a part for a given class,</li>
  <li>the cardinality of that part,</li>
  <li>the members exposed by that part,</li>
  <li>the legality of part-scoped access</li>
</ul>

<p>
belong normatively to the corresponding widget class contract.
</p>

<p>
A part is not automatically an independent top-level widget.
It remains owned by its parent widget unless another specification explicitly states otherwise.
</p>

<hr/>

<h2 id="properties-methods-and-events">17. Properties, Methods, and Events</h2>

<p>
Widgets may expose properties, methods, and runtime events.
</p>

<ul>
  <li>A <strong>property</strong> is a named readable and/or writable member.</li>
  <li>A <strong>method</strong> is a named invocable operation.</li>
  <li>An <strong>event</strong> is a named observable occurrence emitted by the widget or one of its parts.</li>
</ul>

<p>
At widget-instance level, these concepts exist so that canonical source and diagram interaction can refer to stable object-surface members.
</p>

<p>
This document does not fully define the legality of every property, method, and event on every class.
That responsibility belongs to <code>Widget class contract.md</code>.
</p>

<p>
Accordingly:
</p>

<ul>
  <li><code>Widget.md</code> defines that widgets may expose such members,</li>
  <li><code>Widget class contract.md</code> defines which members a class actually exposes and under which access conditions,</li>
  <li><code>Widget interaction.md</code> defines how diagram-side executable access uses those members.</li>
</ul>

<p>
This separation is essential: widget instances may participate in an object model without requiring canonical source to redundantly restate the full class-side member law inside each instance.
</p>

<hr/>

<h2 id="serialization-vs-runtime-state">18. Serialization vs Runtime State</h2>

<p>
Canonical widget serialization stores source-relevant information only.
</p>

<p>
Examples of source-relevant information include:
</p>

<ul>
  <li>widget identity,</li>
  <li>declared class reference,</li>
  <li>role,</li>
  <li>value type,</li>
  <li>source-owned property defaults and instance metadata,</li>
  <li>layout intent,</li>
  <li>source-visible part configuration,</li>
  <li>child-widget composition,</li>
  <li>references to external widget-oriented artifacts,</li>
  <li>presentation-related references.</li>
</ul>

<p>
Examples of runtime-only information include:
</p>

<ul>
  <li>current focus ownership,</li>
  <li>live repaint handles,</li>
  <li>host-specific control handles,</li>
  <li>temporary hover state,</li>
  <li>renderer-private caches,</li>
  <li>runtime-generated skin objects,</li>
  <li>native-widget pointers,</li>
  <li>event-loop-private dispatch state.</li>
</ul>

<p>
Canonical source MUST NOT depend on runtime-only data for validity.
</p>

<p>
The design-time versus runtime-owned boundary of individual members is defined more precisely by the corresponding widget class contract.
</p>

<hr/>

<h2 id="addressing-model">19. Addressing Model</h2>

<p>
Widget interaction requires stable addressing.
</p>

<p>
At minimum, addressing distinguishes:
</p>

<ul>
  <li>the widget instance itself,</li>
  <li>the primary value path of value-carrying widgets,</li>
  <li>part-scoped access,</li>
  <li>member-scoped object access.</li>
</ul>

<p>
Conceptually:
</p>

<pre><code>widget
widget.value
widget.part
widget.part.member
widget.member</code></pre>

<p>
The precise legality of a given member path depends on the widget class contract.
The executable form of such access belongs to <code>Widget interaction.md</code>.
</p>

<p>
This document therefore standardizes the existence of stable addressing surfaces at instance level, not the full executable mechanics of each access form.
</p>

<hr/>

<h2 id="validation-rules">20. Validation Rules</h2>

<p>
Validators MUST enforce at least the following rules:
</p>

<ul>
  <li>a widget instance MUST have a stable <code>id</code>,</li>
  <li>a widget instance MUST declare a known <code>class_ref</code>,</li>
  <li>a widget instance MUST declare a compatible <code>role</code>,</li>
  <li>a value-carrying widget MUST declare a valid <code>value_type</code>,</li>
  <li><code>props</code> MUST be an object when present,</li>
  <li><code>parts</code> MUST be an object when present,</li>
  <li><code>children</code> MUST be an array when present,</li>
  <li><code>package_refs</code> MUST be an array when present,</li>
  <li>widget identifiers MUST be unique across the full recursive widget tree.</li>
</ul>

<p>
Validators SHOULD additionally diagnose at least:
</p>

<ul>
  <li>unknown widget class reference,</li>
  <li>invalid role/class combination,</li>
  <li>missing <code>value_type</code> on a value-carrying widget,</li>
  <li>illegal <code>value_type</code> for the class,</li>
  <li>invalid source-owned property shape,</li>
  <li>unknown or invalid part instance shape,</li>
  <li>unsupported external widget-oriented package reference shape,</li>
  <li>attempts to treat visual resource content as the normative owner of widget semantics.</li>
</ul>

<p>
Detailed legality of class members, part members, access modes, profile gates, and host gates belongs to the corresponding widget class contract and to the relevant interaction validation.
</p>

<p>
These checks validate canonical widget-instance structure.
They do not, by themselves, redefine top-level repository version governance.
When version interpretation is required, it belongs to the repository-wide rules defined in <code>Versioning/Readme.md</code>.
</p>

<hr/>

<h2 id="examples">21. Examples</h2>

<h3>21.1 Minimal numeric control widget instance for the first executable slice</h3>

<pre><code>{
  "id": "ctrl_input",
  "role": "control",
  "class_ref": "frog.widgets.numeric_control",
  "instance_ref": "ctrl_input",
  "value_type": "u16",
  "layout": {
    "x": 20,
    "y": 24,
    "width": 140,
    "height": 32
  },
  "props": {
    "label": "Input value"
  },
  "package_refs": [
    "./ui/accumulator_panel.wfrog"
  ]
}</code></pre>

<h3>21.2 Minimal numeric indicator widget instance for the first executable slice</h3>

<pre><code>{
  "id": "ind_result",
  "role": "indicator",
  "class_ref": "frog.widgets.numeric_indicator",
  "instance_ref": "ind_result",
  "value_type": "u16",
  "layout": {
    "x": 240,
    "y": 24,
    "width": 160,
    "height": 32
  },
  "props": {
    "label": "Accumulated result"
  },
  "package_refs": [
    "./ui/accumulator_panel.wfrog"
  ]
}</code></pre>

<h3>21.3 Container widget instance</h3>

<pre><code>{
  "id": "main_panel",
  "role": "container",
  "class_ref": "frog.widgets.panel",
  "layout": {
    "x": 0,
    "y": 0,
    "width": 800,
    "height": 600
  },
  "children": [
    {
      "id": "ctrl_input",
      "role": "control",
      "class_ref": "frog.widgets.numeric_control",
      "value_type": "u16"
    }
  ]
}</code></pre>

<h3>21.4 Value path versus object path</h3>

<pre><code>widget_value(ctrl_input)                     // natural value path
widget_reference(ctrl_input)                 // object-style anchor
frog.ui.property_write(member="label")       // object-style property access</code></pre>

<hr/>

<h2 id="out-of-scope">22. Out of Scope</h2>

<p>
The following are intentionally out of scope for this document:
</p>

<ul>
  <li>a full industrial widget catalog,</li>
  <li>pixel-perfect rendering identity across runtimes,</li>
  <li>one mandatory theme system,</li>
  <li>one mandatory runtime object model,</li>
  <li>the full class-side contract surface for every widget class,</li>
  <li>the full executable semantics of widget interaction primitives,</li>
  <li>a complete UI event-loop specification,</li>
  <li>the complete package grammar beyond the source-reference posture defined here,</li>
  <li>making an SVG face or any other visual asset the normative source of widget semantics,</li>
  <li>the standardized primitive widget catalog owned by <code>Libraries/</code>.</li>
</ul>

<hr/>

<h2 id="summary">23. Summary</h2>

<p>
A FROG widget is a structured source-level UI object with stable identity, a declared class reference, a declared role, an optional primary value, optional parts, optional child widgets, optional source-owned instance metadata, and explicit participation paths toward executable interaction.
</p>

<p>
This document defines widget instances as they exist in canonical <code>.frog</code> source.
It fixes the source-level distinction between:
</p>

<ul>
  <li>natural value participation,</li>
  <li>object-style widget access,</li>
  <li>class-side widget law,</li>
  <li>external widget-oriented package content,</li>
  <li>bounded widget behavior,</li>
  <li>runtime realization,</li>
  <li>visual resources and presentation surfaces.</li>
</ul>

<p>
It therefore does not define:
</p>

<ul>
  <li>front-panel composition ownership in full detail,</li>
  <li>class-side member legality,</li>
  <li>diagram-side executable widget interaction semantics,</li>
  <li>general language execution semantics,</li>
  <li>the full package grammar of widget-oriented artifacts,</li>
  <li>the primitive standardized widget catalog,</li>
  <li>published specification corpus version governance.</li>
</ul>

<p>
That separation allows FROG to support rich widget objects, external widget-oriented package families, skinnable front faces, SVG-backed visual resources, and multi-runtime interpretation without collapsing source instance shape, class contract, package content, behavior doctrine, runtime convenience, host realization, and visual assets into the same layer.
</p>
