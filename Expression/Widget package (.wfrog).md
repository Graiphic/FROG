<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Widget Package (.wfrog)</h1>

<p align="center">
  <strong>Normative package format for widget classes, widget composition, widget realization resources, and bounded widget behavior published through <code>.wfrog</code> artifacts</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#why-wfrog-exists">2. Why <code>.wfrog</code> Exists</a></li>
  <li><a href="#scope">3. Scope</a></li>
  <li><a href="#ownership-boundary">4. Ownership Boundary</a></li>
  <li><a href="#core-design-goals">5. Core Design Goals</a></li>
  <li><a href="#package-kinds">6. Package Kinds</a></li>
  <li><a href="#top-level-structure">7. Top-Level Structure</a></li>
  <li><a href="#metadata-and-identity">8. Metadata and Identity</a></li>
  <li><a href="#imports-and-dependencies">9. Imports and Dependencies</a></li>
  <li><a href="#exports">10. Exports</a></li>
  <li><a href="#widget-class-definitions">11. Widget Class Definitions</a></li>
  <li><a href="#value-model">12. Value Model</a></li>
  <li><a href="#property-model">13. Property Model</a></li>
  <li><a href="#method-model">14. Method Model</a></li>
  <li><a href="#event-model">15. Event Model</a></li>
  <li><a href="#part-model">16. Part Model</a></li>
  <li><a href="#appearance-model">17. Appearance Model</a></li>
  <li><a href="#behavior-model">18. Behavior Model</a></li>
  <li><a href="#diagram-contract">19. Diagram Contract</a></li>
  <li><a href="#primitive-and-composite-widget-classes">20. Primitive and Composite Widget Classes</a></li>
  <li><a href="#realization-resources">21. Realization Resources</a></li>
  <li><a href="#svg-integration">22. SVG Integration</a></li>
  <li><a href="#bindings-and-host-bridges">23. Bindings and Host Bridges</a></li>
  <li><a href="#relationship-with-frog-source">24. Relationship with <code>.frog</code> Source</a></li>
  <li><a href="#minimal-json-outline">25. Minimal JSON Outline</a></li>
  <li><a href="#structural-validity">26. Structural Validity</a></li>
  <li><a href="#canonical-formatting-and-portability">27. Canonical Formatting and Portability</a></li>
  <li><a href="#status">28. Status</a></li>
  <li><a href="#license">29. License</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the normative package format used by FROG widget-oriented source artifacts with the <code>.wfrog</code> extension.
</p>

<p>
A <code>.wfrog</code> artifact is a structured JSON package used to publish inspectable widget-related content such as:
</p>

<ul>
  <li>widget class definitions,</li>
  <li>widget composition definitions,</li>
  <li>bounded widget behavior declarations,</li>
  <li>widget realization resources,</li>
  <li>host-binding metadata,</li>
  <li>related exported widget-oriented package surfaces.</li>
</ul>

<p>
A <code>.wfrog</code> package is not a second canonical program source file. The canonical program source of FROG remains the <code>.frog</code> file.
</p>

<p>
The purpose of <code>.wfrog</code> is to make widget law, widget composition, widget realization resources, and related package-owned publication surfaces explicit, machine-readable, inspectable, portable, and reviewable without collapsing them into runtime-private code or into opaque visual assets.
</p>

<p>
A <code>.wfrog</code> package therefore exists to publish widget-oriented content, not to replace program source, not to hide semantics inside a runtime, and not to turn realization assets into semantic truth.
</p>

<hr/>

<h2 id="why-wfrog-exists">2. Why <code>.wfrog</code> Exists</h2>

<p>
FROG front-panel widgets are not just drawings and are not just raw values.
</p>

<p>
A widget class may expose:
</p>

<ul>
  <li>a primary value model,</li>
  <li>properties,</li>
  <li>methods,</li>
  <li>events,</li>
  <li>parts and subparts,</li>
  <li>appearance surfaces,</li>
  <li>bounded behavior rules,</li>
  <li>diagram-facing interaction surfaces,</li>
  <li>host-facing realization resources.</li>
</ul>

<p>
That information must not live only in:
</p>

<ul>
  <li>one private runtime implementation,</li>
  <li>one IDE's private editable model,</li>
  <li>an opaque toolkit binding,</li>
  <li>an SVG file,</li>
  <li>one example's ad hoc metadata.</li>
</ul>

<p>
The <code>.wfrog</code> format exists so that widget-oriented content can be published explicitly and consumed consistently across multiple runtimes and toolchains.
</p>

<p>
It is the publication corridor that allows:
</p>

<ul>
  <li>primitive widget classes to be published once and reused,</li>
  <li>composite widget classes to be built without runtime-private reinvention,</li>
  <li>bounded behavior to remain inspectable,</li>
  <li>realization resources to remain subordinate to semantics,</li>
  <li>developers to define new widgets without making one runtime the hidden definition of the class.</li>
</ul>

<hr/>

<h2 id="scope">3. Scope</h2>

<p>
This document defines:
</p>

<ul>
  <li>the top-level structure of a <code>.wfrog</code> package,</li>
  <li>the package kinds allowed in FROG,</li>
  <li>how widget classes are serialized and exported,</li>
  <li>how primitive and composite widget classes are distinguished,</li>
  <li>how package-owned bounded behavior is represented,</li>
  <li>how realization resources are declared,</li>
  <li>how SVG participates as a visual layer,</li>
  <li>how host-facing bindings may be declared without becoming semantic truth,</li>
  <li>how canonical <code>.frog</code> source refers to <code>.wfrog</code> packages.</li>
</ul>

<p>
This document does not define:
</p>

<ul>
  <li>the canonical program source structure of <code>.frog</code>,</li>
  <li>the validated execution meaning of a whole FROG program,</li>
  <li>the full runtime architecture of any one runtime family,</li>
  <li>the private in-memory authoring model of one IDE,</li>
  <li>the full open execution-facing IR,</li>
  <li>the semantics of unrelated primitive families outside the widget corridor.</li>
</ul>

<p>
This document also does not redefine widget instance syntax, diagram-side interaction primitives, or class-law ownership.
It defines the package-owned publication format through which those surfaces may be published and reused.
</p>

<hr/>

<h2 id="ownership-boundary">4. Ownership Boundary</h2>

<p>
The following architectural boundary is normative:
</p>

<pre><code>.frog  -&gt; canonical program source
.wfrog -&gt; widget-oriented package source
SVG    -&gt; visual asset layer
runtime-private code -&gt; host-private realization support
</code></pre>

<p>
More precisely:
</p>

<ul>
  <li><code>.frog</code> owns program meaning at source level, widget participation in the program, and source-side widget identity and use.</li>
  <li><code>.wfrog</code> owns publishable widget-oriented package content such as widget classes, composition, bounded behavior declarations, and realization resources.</li>
  <li>SVG may participate as a visual asset, skin, template, anchor carrier, or scalable decorative layer.</li>
  <li>Runtime-private code may implement host-specific realization details, but it does not become the normative source of widget law.</li>
</ul>

<p>
The following distinctions MUST remain explicit:
</p>

<pre><code>widget instance declaration      != widget class definition
widget class law                 != host-private helper code
widget realization resource      != runtime-private rendering code
visual asset                     != semantic truth
package-owned behavior surface   != unrestricted arbitrary host code
</code></pre>

<p>
Likewise:
</p>

<pre><code>.wfrog publication
    !=
runtime implementation

package-owned class law
    !=
one host toolkit mapping

bounded behavior publication
    !=
arbitrary package-defined host execution
</code></pre>

<hr/>

<h2 id="core-design-goals">5. Core Design Goals</h2>

<p>
The <code>.wfrog</code> format is designed to support all of the following at the same time:
</p>

<ul>
  <li>inspectable widget classes,</li>
  <li>portable widget definitions across Python, Rust, and C/C++ runtimes,</li>
  <li>clear separation between value semantics and visual realization,</li>
  <li>developer-defined front-panel objects,</li>
  <li>bounded behavior surfaces that remain reviewable,</li>
  <li>a modern object model inspired by JavaScript component systems and LabVIEW control intuition,</li>
  <li>a small normative primitive core plus extensible composite widgets.</li>
</ul>

<p>
The format therefore favors:
</p>

<ul>
  <li>explicit properties over hidden mutable fields,</li>
  <li>explicit methods over runtime-only ad hoc callbacks,</li>
  <li>explicit events over undocumented toolkit signals,</li>
  <li>explicit parts over opaque skins,</li>
  <li>bounded declarative behavior over unrestricted package-defined arbitrary execution,</li>
  <li>composition over one closed hardcoded widget list.</li>
</ul>

<p>
It is intentionally a publication format, not a private authoring cache and not a runtime-private intermediate convenience format.
</p>

<hr/>

<h2 id="package-kinds">6. Package Kinds</h2>

<p>
A <code>.wfrog</code> package MUST declare a <code>package_kind</code>.
</p>

<p>
The following kinds are defined:
</p>

<ul>
  <li><code>widget_library</code> — publishes one or more widget classes, typically primitive or reusable classes,</li>
  <li><code>widget_bundle</code> — aggregates multiple imported widget libraries and related assets for convenient distribution,</li>
  <li><code>widget_realization_library</code> — publishes reusable realization resources and realization-side mappings,</li>
  <li><code>widget_composite_library</code> — publishes composite widget classes built from primitive or imported widget classes.</li>
</ul>

<p>
A package MAY export more than one class family, but the package kind MUST still describe the package's dominant publication posture.
</p>

<p>
A package kind does not change the ownership boundary:
</p>

<ul>
  <li>none of these package kinds replaces <code>.frog</code> as canonical program source,</li>
  <li>none of these package kinds turns one runtime implementation into normative widget law.</li>
</ul>

<p>
A package kind therefore helps tools and users understand the intended posture of the package,
but does not alter the portable publication rules of the widget corridor.
</p>

<hr/>

<h2 id="top-level-structure">7. Top-Level Structure</h2>

<p>
A canonical <code>.wfrog</code> package is a JSON object.
</p>

<p>
At minimum, it SHOULD follow this conceptual outline:
</p>

<pre><code>{
  "wfrog_version": "...",
  "package_kind": "...",
  "package": { ... },
  "imports": [ ... ],
  "exports": { ... }
}
</code></pre>

<p>
Depending on package kind, the package MAY additionally contain:
</p>

<ul>
  <li><code>widget_classes</code>,</li>
  <li><code>composite_classes</code>,</li>
  <li><code>realizations</code>,</li>
  <li><code>resources</code>,</li>
  <li><code>bindings</code>,</li>
  <li><code>diagnostics</code>,</li>
  <li><code>examples</code>.</li>
</ul>

<p>
The package remains intentionally explicit. Widget-oriented meaning should not depend on hidden code generation or on private asset-resolution conventions.
</p>

<p>
Top-level sections SHOULD remain stable and diff-friendly.
A package should be inspectable by reading its JSON, not only by executing one implementation.
</p>

<hr/>

<h2 id="metadata-and-identity">8. Metadata and Identity</h2>

<p>
The <code>package</code> object SHOULD identify the package clearly and durably.
</p>

<p>
It SHOULD contain fields such as:
</p>

<ul>
  <li><code>id</code>,</li>
  <li><code>name</code>,</li>
  <li><code>namespace</code>,</li>
  <li><code>publisher</code>,</li>
  <li><code>summary</code>,</li>
  <li><code>description</code>,</li>
  <li><code>license</code>,</li>
  <li><code>homepage</code>,</li>
  <li><code>tags</code>.</li>
</ul>

<p>
Package identity MUST be stable enough that widget references from canonical <code>.frog</code> source remain reviewable and deterministic.
</p>

<p>
Package identity must support:
</p>

<ul>
  <li>stable publication,</li>
  <li>stable import references,</li>
  <li>stable export references,</li>
  <li>clear human inspection,</li>
  <li>tooling diagnostics and conformance reporting.</li>
</ul>

<hr/>

<h2 id="imports-and-dependencies">9. Imports and Dependencies</h2>

<p>
A <code>.wfrog</code> package MAY import other <code>.wfrog</code> packages.
</p>

<p>
Imports are used to:
</p>

<ul>
  <li>reuse widget base classes,</li>
  <li>reuse realization resources,</li>
  <li>reuse enums or shared value-model structures,</li>
  <li>compose higher-level widgets from existing primitives.</li>
</ul>

<p>
Imports SHOULD remain explicit.
</p>

<p>
A package MUST NOT depend on undocumented runtime-private imports in order to define the legality of properties, methods, parts, or events.
</p>

<p>
Dependencies that affect public class law, public composite structure, bounded behavior publication, or realization-resource interpretation SHOULD be represented in a way that remains inspectable and deterministic.
</p>

<hr/>

<h2 id="exports">10. Exports</h2>

<p>
The <code>exports</code> object declares which package-owned symbols are public.
</p>

<p>
Typical exported symbol categories include:
</p>

<ul>
  <li>widget classes,</li>
  <li>composite widget classes,</li>
  <li>shared enums,</li>
  <li>shared appearance tokens,</li>
  <li>realization resources,</li>
  <li>realization profiles.</li>
</ul>

<p>
Only exported classes are intended for stable external reference from <code>.frog</code> source or from other packages.
</p>

<p>
Exported symbols therefore define the stable public publication surface of the package.
Non-exported internal helpers MAY exist, but they MUST NOT be mistaken for public portable contract surfaces.
</p>

<hr/>

<h2 id="widget-class-definitions">11. Widget Class Definitions</h2>

<p>
A widget class definition is the central publication unit of a <code>.wfrog</code> package.
</p>

<p>
Each class definition SHOULD contain, directly or indirectly:
</p>

<ul>
  <li>class identity,</li>
  <li>role model,</li>
  <li>value model,</li>
  <li>property model,</li>
  <li>method model,</li>
  <li>event model,</li>
  <li>part model,</li>
  <li>appearance model,</li>
  <li>behavior model,</li>
  <li>diagram contract,</li>
  <li>optional realization references.</li>
</ul>

<p>
The class definition MUST remain inspectable without requiring users to read one runtime implementation's private code.
</p>

<p>
Conceptually:
</p>

<pre><code>{
  "class_id": "frog.widgets.numeric_control",
  "display_name": "Numeric Control",
  "class_kind": "primitive",
  "widget_role": "control",
  "inherits": "frog.widgets.scalar_widget",
  "value_model": { ... },
  "properties": [ ... ],
  "methods": [ ... ],
  "events": [ ... ],
  "parts": [ ... ],
  "appearance": { ... },
  "behavior": { ... },
  "diagram_contract": { ... },
  "realization_refs": [ ... ]
}
</code></pre>

<p>
The published class definition is therefore the inspectable package-side carrier of class law.
It may reference other package content, but it must not dissolve into runtime-private interpretation.
</p>

<hr/>

<h2 id="value-model">12. Value Model</h2>

<p>
The value model defines the primary data value of a widget class where such a value exists.
</p>

<p>
The value model SHOULD specify:
</p>

<ul>
  <li>the canonical value type,</li>
  <li>default value policy,</li>
  <li>whether the value is required or nullable,</li>
  <li>coercion rules,</li>
  <li>validation constraints,</li>
  <li>range and step constraints where relevant,</li>
  <li>whether the primary value participates naturally in diagram interaction.</li>
</ul>

<p>
Examples:
</p>

<ul>
  <li>a numeric control exposes a scalar numeric primary value,</li>
  <li>a boolean control exposes a boolean primary value,</li>
  <li>a waveform chart may expose a structured series value or append-oriented update contract,</li>
  <li>a container widget may expose no primary value at all.</li>
</ul>

<p>
The primary value model is part of widget class law. It is not merely a visual text field.
</p>

<p>
The value model SHOULD also indicate whether:
</p>

<ul>
  <li>the primary value is mirrored as a property such as <code>value</code>,</li>
  <li>the primary value may be mutated by user interaction,</li>
  <li>the primary value may be mutated by diagram interaction,</li>
  <li>the primary value participates in bounded derived projections such as text or visual state mappings.</li>
</ul>

<hr/>

<h2 id="property-model">13. Property Model</h2>

<p>
A widget class MAY expose properties.
</p>

<p>
Properties are named object members intended for structured inspection and, where allowed, structured mutation.
</p>

<p>
Each property definition SHOULD specify:
</p>

<ul>
  <li>property identifier,</li>
  <li>value type,</li>
  <li>readability,</li>
  <li>writability,</li>
  <li>design-time mutability,</li>
  <li>runtime mutability,</li>
  <li>persistence posture,</li>
  <li>optional owning part,</li>
  <li>description.</li>
</ul>

<p>
Properties SHOULD use stable namespaced identifiers where this improves clarity.
</p>

<p>
Examples:
</p>

<ul>
  <li><code>value</code>,</li>
  <li><code>limits.minimum</code>,</li>
  <li><code>limits.maximum</code>,</li>
  <li><code>interaction.enabled</code>,</li>
  <li><code>interaction.visible</code>,</li>
  <li><code>label.text</code>,</li>
  <li><code>label.visible</code>,</li>
  <li><code>style.text_color</code>,</li>
  <li><code>style.background_fill</code>.</li>
</ul>

<p>
Property law MUST be package-published and inspectable.
</p>

<p>
A runtime MUST NOT silently create portable public properties that are absent from the published class contract.
</p>

<p>
The property model is the package-published extension of class law.
It is not a permission for runtime-specific undisclosed mutable fields to become public portable surfaces.
</p>

<hr/>

<h2 id="method-model">14. Method Model</h2>

<p>
A widget class MAY expose methods.
</p>

<p>
Methods are named object operations that may be invoked through standardized diagram-side interaction.
</p>

<p>
Each method definition SHOULD specify:
</p>

<ul>
  <li>method identifier,</li>
  <li>ordered parameters and their types,</li>
  <li>return type if any,</li>
  <li>allowed invocation context,</li>
  <li>possible emitted events,</li>
  <li>state effects,</li>
  <li>description.</li>
</ul>

<p>
Examples:
</p>

<ul>
  <li><code>focus()</code>,</li>
  <li><code>reset_to_default()</code>,</li>
  <li><code>increment()</code>,</li>
  <li><code>decrement()</code>,</li>
  <li><code>set_range(min, max)</code>.</li>
</ul>

<p>
Methods are part of object law. They are not arbitrary host callbacks.
</p>

<p>
The method model SHOULD make clear whether a method is:
</p>

<ul>
  <li>portable,</li>
  <li>profile-gated,</li>
  <li>host-gated,</li>
  <li>purely local to widget behavior,</li>
  <li>dependent on non-portable realization support.</li>
</ul>

<hr/>

<h2 id="event-model">15. Event Model</h2>

<p>
A widget class MAY expose events.
</p>

<p>
Events are named occurrences emitted by the widget or by one of its parts.
</p>

<p>
Each event definition SHOULD specify:
</p>

<ul>
  <li>event identifier,</li>
  <li>payload shape,</li>
  <li>emission condition,</li>
  <li>owning source or part,</li>
  <li>delivery posture,</li>
  <li>whether the event is observable from standardized diagram interaction,</li>
  <li>description.</li>
</ul>

<p>
Examples:
</p>

<ul>
  <li><code>value_changed</code>,</li>
  <li><code>editing_started</code>,</li>
  <li><code>editing_committed</code>,</li>
  <li><code>focus_gained</code>,</li>
  <li><code>focus_lost</code>,</li>
  <li><code>clicked</code>,</li>
  <li><code>pressed</code>,</li>
  <li><code>released</code>.</li>
</ul>

<p>
The event model is inspired by modern event-target style object systems. Events remain named, inspectable, typed, and explicit.
</p>

<p>
Event publication MUST NOT collapse into undocumented toolkit signals.
If an event is part of the portable public class surface, it must be published as such.
</p>

<hr/>

<h2 id="part-model">16. Part Model</h2>

<p>
A widget class MAY expose named parts.
</p>

<p>
Parts are addressable structural subregions or subobjects of a widget.
</p>

<p>
Parts exist so that:
</p>

<ul>
  <li>appearance can be structured,</li>
  <li>event ownership can be localized,</li>
  <li>part-specific properties can be exposed,</li>
  <li>visual resources can bind to stable anchors,</li>
  <li>diagram-side object access can address stable sub-surfaces when allowed.</li>
</ul>

<p>
A part definition SHOULD specify:
</p>

<ul>
  <li>part identifier,</li>
  <li>part role,</li>
  <li>whether it is public or internal,</li>
  <li>which properties it owns,</li>
  <li>which events it may emit,</li>
  <li>optional realization anchors,</li>
  <li>description.</li>
</ul>

<p>
Examples for a numeric control:
</p>

<ul>
  <li><code>root</code>,</li>
  <li><code>label</code>,</li>
  <li><code>value_display</code>,</li>
  <li><code>increment_button</code>,</li>
  <li><code>decrement_button</code>,</li>
  <li><code>frame</code>,</li>
  <li><code>background</code>.</li>
</ul>

<p>
Portable parts are published parts.
Toolkit-private fragments are not portable parts unless the package declares them as such.
</p>

<hr/>

<h2 id="appearance-model">17. Appearance Model</h2>

<p>
A widget class MAY publish an appearance model.
</p>

<p>
The appearance model SHOULD remain layered:
</p>

<ul>
  <li>abstract appearance tokens,</li>
  <li>layout and sizing rules,</li>
  <li>optional realization references.</li>
</ul>

<p>
The abstract appearance layer SHOULD contain portable style surfaces such as:
</p>

<ul>
  <li>font family,</li>
  <li>font size,</li>
  <li>text color,</li>
  <li>background fill,</li>
  <li>stroke color,</li>
  <li>corner radius,</li>
  <li>padding,</li>
  <li>alignment.</li>
</ul>

<p>
The appearance model MUST NOT collapse widget meaning into one host toolkit skin.
</p>

<p>
The appearance model MUST NOT treat SVG as semantic truth.
</p>

<p>
Appearance publication therefore describes portable appearance-facing surfaces and defaults,
not the full private rendering strategy of a runtime or host.
</p>

<hr/>

<h2 id="behavior-model">18. Behavior Model</h2>

<p>
A widget class MAY publish bounded behavior surfaces.
</p>

<p>
The behavior model exists so that widget reaction and widget internal rules can be described explicitly without collapsing widget law into runtime-private code.
</p>

<p>
The behavior model SHOULD distinguish:
</p>

<ul>
  <li>intrinsic class behavior,</li>
  <li>declarative rules,</li>
  <li>bounded expressions,</li>
  <li>host-private implementation support.</li>
</ul>

<p>
Examples of package-published bounded behavior include:
</p>

<ul>
  <li>coercing a numeric value into its valid range,</li>
  <li>disabling increment and decrement actions when the widget is disabled,</li>
  <li>emitting <code>value_changed</code> after a committed value update,</li>
  <li>updating part-facing appearance state from published object properties.</li>
</ul>

<p>
A <code>.wfrog</code> package MUST NOT require unrestricted arbitrary host code in order to define the public legality of the widget object surface.
</p>

<p>
Behavior publication remains bounded and reviewable.
</p>

<p>
The package SHOULD make explicit which behavior surfaces are:
</p>

<ul>
  <li>portable and normative,</li>
  <li>portable but optional,</li>
  <li>host-private and non-portable.</li>
</ul>

<p>
This is the core rule that allows richer widget publication without turning `.wfrog` into a carrier of arbitrary runtime-defined imperative code.
</p>

<hr/>

<h2 id="diagram-contract">19. Diagram Contract</h2>

<p>
A widget class SHOULD declare how it participates in diagram interaction.
</p>

<p>
This diagram contract SHOULD specify:
</p>

<ul>
  <li>whether the widget has a natural primary value,</li>
  <li>whether that primary value is readable, writable, or both,</li>
  <li>whether <code>widget_reference</code> addressing is allowed,</li>
  <li>which properties are available through standardized property interaction,</li>
  <li>which methods are invocable,</li>
  <li>which events are observable,</li>
  <li>whether specific parts are addressable.</li>
</ul>

<p>
This package-published diagram contract supports portable lowering into later execution-facing representations.
</p>

<p>
It does not replace the interaction model defined elsewhere.
It publishes what the class permits that interaction model to target.
</p>

<hr/>

<h2 id="primitive-and-composite-widget-classes">20. Primitive and Composite Widget Classes</h2>

<p>
FROG supports two major class families in the widget corridor:
</p>

<ul>
  <li><strong>primitive widget classes</strong>,</li>
  <li><strong>composite widget classes</strong>.</li>
</ul>

<h3>20.1 Primitive widget classes</h3>

<p>
Primitive widget classes define the minimal reusable standard object surfaces from which a portable front-panel ecosystem can grow.
</p>

<p>
Examples include:
</p>

<ul>
  <li>numeric control,</li>
  <li>numeric indicator,</li>
  <li>boolean control,</li>
  <li>boolean indicator,</li>
  <li>button,</li>
  <li>string control,</li>
  <li>string indicator,</li>
  <li>waveform chart.</li>
</ul>

<h3>20.2 Composite widget classes</h3>

<p>
Composite widget classes are built from other widget classes and package-published composition rules.
</p>

<p>
A composite widget class SHOULD specify:
</p>

<ul>
  <li>its internal child widget structure,</li>
  <li>its internal layout,</li>
  <li>its exposed public value model if any,</li>
  <li>its public property and method remapping where applicable,</li>
  <li>its internal event-routing rules where applicable,</li>
  <li>its bounded behavior rules.</li>
</ul>

<p>
Composite widget classes are the preferred FROG mechanism for developer-defined higher-level front-panel objects.
</p>

<p>
A composite class remains portable only if its public class law is published explicitly.
A runtime must not be forced to infer the public contract of the composite only from hidden internal composition.
</p>

<hr/>

<h2 id="realization-resources">21. Realization Resources</h2>

<p>
A <code>.wfrog</code> package MAY publish realization resources.
</p>

<p>
These resources MAY include:
</p>

<ul>
  <li>SVG files,</li>
  <li>layer maps,</li>
  <li>anchor maps,</li>
  <li>part-to-visual bindings,</li>
  <li>appearance token maps,</li>
  <li>host realization profiles.</li>
</ul>

<p>
Realization resources are package-published support artifacts for rendering and interaction realization.
</p>

<p>
They MUST remain subordinate to widget class law.
</p>

<p>
Realization resources may help runtimes render or bind a widget consistently,
but they do not redefine what the class means.
They support realization.
They do not own semantics.
</p>

<hr/>

<h2 id="svg-integration">22. SVG Integration</h2>

<p>
SVG may participate as a visual asset layer in a <code>.wfrog</code> package.
</p>

<p>
SVG MAY be used for:
</p>

<ul>
  <li>skins,</li>
  <li>decorative assets,</li>
  <li>scalable visual templates,</li>
  <li>named anchors or layer carriers,</li>
  <li>part-binding guidance.</li>
</ul>

<p>
SVG MUST NOT be treated as the owner of:
</p>

<ul>
  <li>primary widget value semantics,</li>
  <li>public properties,</li>
  <li>public methods,</li>
  <li>public events,</li>
  <li>bounded behavior law.</li>
</ul>

<p>
SVG is a realization aid, not a semantic object model.
</p>

<p>
A runtime MAY faithfully realize SVG-backed assets, substitute a compatible native rendering, or ignore unsupported SVG-specific details, provided that the published class law and published public surfaces remain preserved.
</p>

<hr/>

<h2 id="bindings-and-host-bridges">23. Bindings and Host Bridges</h2>

<p>
A <code>.wfrog</code> package MAY publish host-binding metadata to assist runtime families.
</p>

<p>
Such metadata MAY include:
</p>

<ul>
  <li>preferred realization backends,</li>
  <li>host widget mapping hints,</li>
  <li>capability requirements,</li>
  <li>accessibility hints,</li>
  <li>performance or redraw hints.</li>
</ul>

<p>
These bindings remain non-authoritative with respect to public widget law.
</p>

<p>
A runtime MAY ignore a non-portable convenience hint if doing so preserves the normative public object surface.
</p>

<p>
Bindings and host bridges therefore exist to support realization and deployment convenience.
They MUST NOT silently mutate the published portable contract of a widget class.
</p>

<hr/>

<h2 id="relationship-with-frog-source">24. Relationship with <code>.frog</code> Source</h2>

<p>
Canonical <code>.frog</code> source may reference <code>.wfrog</code> packages through source-owned widget instance structures.
</p>

<p>
A <code>.frog</code> file may therefore identify:
</p>

<ul>
  <li>the widget class it intends to instantiate,</li>
  <li>the package from which that class originates,</li>
  <li>optional realization preferences where allowed by the source model.</li>
</ul>

<p>
However:
</p>

<ul>
  <li>the <code>.frog</code> file remains canonical program source,</li>
  <li>the <code>.frog</code> file does not inline all widget class law,</li>
  <li>the <code>.wfrog</code> file does not replace the program's diagram or source structure.</li>
</ul>

<p>
The relationship is therefore explicit and layered:
</p>

<ul>
  <li><code>.frog</code> declares widget participation in one program,</li>
  <li><code>.wfrog</code> publishes reusable widget-oriented law, composition, behavior, and realization resources,</li>
  <li>runtimes interpret both without collapsing their ownership boundaries.</li>
</ul>

<hr/>

<h2 id="minimal-json-outline">25. Minimal JSON Outline</h2>

<p>
A minimal illustrative outline of a class-publishing <code>.wfrog</code> package is shown below.
</p>

<pre><code>{
  "wfrog_version": "0.1",
  "package_kind": "widget_library",
  "package": {
    "id": "frog.widgets.core",
    "name": "FROG Widget Core",
    "namespace": "frog.widgets"
  },
  "imports": [],
  "exports": {
    "classes": [
      "frog.widgets.numeric_control"
    ]
  },
  "widget_classes": [
    {
      "class_id": "frog.widgets.numeric_control",
      "display_name": "Numeric Control",
      "class_kind": "primitive",
      "widget_role": "control",
      "value_model": {
        "type": { "kind": "float64" },
        "default": 0
      },
      "properties": [
        { "id": "value", "type": { "kind": "float64" }, "read": true, "write": true },
        { "id": "limits.minimum", "type": { "kind": "float64" }, "read": true, "write": true },
        { "id": "limits.maximum", "type": { "kind": "float64" }, "read": true, "write": true },
        { "id": "interaction.enabled", "type": { "kind": "bool" }, "read": true, "write": true }
      ],
      "methods": [
        { "id": "increment", "parameters": [], "returns": null },
        { "id": "decrement", "parameters": [], "returns": null }
      ],
      "events": [
        { "id": "value_changed", "payload": { "type": "object" } }
      ],
      "parts": [
        { "id": "root" },
        { "id": "label" },
        { "id": "value_display" },
        { "id": "increment_button" },
        { "id": "decrement_button" }
      ],
      "appearance": {
        "tokens": {
          "style.text_color": "#000000",
          "style.background_fill": "#ffffff"
        }
      },
      "behavior": {
        "rules": [
          { "kind": "range_coercion", "property": "value" }
        ]
      },
      "diagram_contract": {
        "natural_value": true,
        "widget_reference": true,
        "property_access": true,
        "method_invoke": true,
        "event_observe": true
      }
    }
  ]
}
</code></pre>

<p>
This outline is illustrative. The normative meaning of each section is owned by this document and by the related widget companion documents.
</p>

<p>
It shows the intended posture clearly:
a package publishes explicit widget-oriented law and related resources in a machine-readable form that remains inspectable without reliance on one runtime implementation.
</p>

<hr/>

<h2 id="structural-validity">26. Structural Validity</h2>

<p>
A <code>.wfrog</code> package is structurally valid only if:
</p>

<ul>
  <li>it is loadable as JSON,</li>
  <li>it declares a recognized <code>wfrog_version</code>,</li>
  <li>it declares a recognized <code>package_kind</code>,</li>
  <li>its exported classes resolve to explicit package-published definitions,</li>
  <li>its class definitions are internally coherent,</li>
  <li>its realization resources do not silently replace class law,</li>
  <li>its imports are explicit and non-ambiguous.</li>
</ul>

<p>
Structural validity of a <code>.wfrog</code> package does not, by itself, guarantee that every runtime can realize the package fully. Runtime support claims remain separate from source/package validity.
</p>

<p>
Likewise, structural validity does not by itself guarantee conformance for every exported class.
Conformance depends on whether a runtime preserves the published portable contract surfaces it claims to support.
</p>

<hr/>

<h2 id="canonical-formatting-and-portability">27. Canonical Formatting and Portability</h2>

<p>
A <code>.wfrog</code> package SHOULD remain stable, reviewable, and diff-friendly.
</p>

<p>
Tools SHOULD preserve:
</p>

<ul>
  <li>stable ordering of top-level sections,</li>
  <li>stable ordering of exported symbols,</li>
  <li>stable identifiers for classes, properties, methods, events, and parts,</li>
  <li>explicit imports and realization references,</li>
  <li>explicit bounded behavior declarations.</li>
</ul>

<p>
Portable widget packages SHOULD avoid depending on one host toolkit's private assumptions wherever that would change the public object surface.
</p>

<p>
A portable package should therefore maximize:
</p>

<ul>
  <li>explicit public law,</li>
  <li>explicit public exports,</li>
  <li>bounded behavior publication,</li>
  <li>subordinate realization resources,</li>
  <li>clear marking of host-private conveniences.</li>
</ul>

<hr/>

<h2 id="status">28. Status</h2>

<p>
This document defines the normative package format of FROG widget-oriented <code>.wfrog</code> artifacts.
</p>

<p>
The closure direction for this package family is:
</p>

<ul>
  <li>a small inspectable primitive widget core,</li>
  <li>clear class publication surfaces,</li>
  <li>developer-defined composite widgets,</li>
  <li>portable bounded behavior,</li>
  <li>multi-runtime realization compatibility.</li>
</ul>

<p>
Its role in the repository is to keep widget publication explicit and layered,
so that richer front-panel ecosystems can grow without transferring semantic ownership to runtimes or to visual assets.
</p>

<hr/>

<h2 id="license">29. License</h2>

<p>
See the repository-level license information for the licensing terms governing this specification.
</p>
