<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Widget Class Contract</h1>

<p align="center">
  <strong>Normative class-level contract for widget classes, primary values, properties, methods, events, parts, object addressing, and bounded behavior surfaces</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#why-this-document-exists">2. Why This Document Exists</a></li>
  <li><a href="#scope">3. Scope</a></li>
  <li><a href="#relation-with-other-specifications">4. Relation with Other Specifications</a></li>
  <li><a href="#ownership-boundary">5. Ownership Boundary</a></li>
  <li><a href="#design-goals">6. Design Goals</a></li>
  <li><a href="#class-contract-purpose">7. Class Contract Purpose</a></li>
  <li><a href="#required-class-surfaces">8. Required Class Surfaces</a></li>
  <li><a href="#class-identity-and-category">9. Class Identity and Category</a></li>
  <li><a href="#primary-value-model">10. Primary Value Model</a></li>
  <li><a href="#property-model">11. Property Model</a></li>
  <li><a href="#method-model">12. Method Model</a></li>
  <li><a href="#event-model">13. Event Model</a></li>
  <li><a href="#part-model">14. Part Model</a></li>
  <li><a href="#object-addressing">15. Object Addressing</a></li>
  <li><a href="#mutability-and-persistence">16. Mutability and Persistence</a></li>
  <li><a href="#design-time-vs-runtime">17. Design-Time vs Runtime</a></li>
  <li><a href="#bounded-behavior-surfaces">18. Bounded Behavior Surfaces</a></li>
  <li><a href="#composite-widget-classes">19. Composite Widget Classes</a></li>
  <li><a href="#developer-defined-widget-classes">20. Developer-Defined Widget Classes</a></li>
  <li><a href="#extension-and-portability-rules">21. Extension and Portability Rules</a></li>
  <li><a href="#conformance">22. Conformance</a></li>
  <li><a href="#example-class-outline">23. Example Class Outline</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
A FROG widget class contract defines the normative class-level law of a widget family.
It specifies the stable semantic surface that portable runtimes, IDEs, validators, and host realizations
are expected to interpret consistently.
</p>

<p>
The class contract exists above any one runtime implementation, above any one UI toolkit, and above any one
visual asset format.
It is the published class-level source of truth for what a widget class is, what it may expose, and how that
exposure is addressed.
</p>

<p>
This document treats a widget class as an object surface with explicitly declared members.
Those members include a possible primary value, named properties, named methods, named events,
and optionally stable parts and subparts.
</p>

<p>
The class contract is therefore the layer that answers what a widget class legally exposes.
It is not the runtime-private code that happens to implement it, not the host toolkit that renders it,
and not the visual asset that skins it.
</p>

<hr/>

<h2 id="why-this-document-exists">2. Why This Document Exists</h2>

<p>
FROG requires a front-panel object system that is:
</p>

<ul>
  <li>portable across runtimes,</li>
  <li>inspectable and auditable,</li>
  <li>extensible beyond a tiny fixed widget inventory,</li>
  <li>explicitly published rather than hidden inside runtime-private code,</li>
  <li>usable by both natural-value interaction and object-style interaction.</li>
</ul>

<p>
Without a class-level contract, widget law would drift into host-specific conventions,
runtime-private helper structures, or visual-layer assumptions.
That would make portability, conformance, and developer-defined widget classes unreliable.
</p>

<p>
This document exists precisely to prevent that drift.
A runtime may implement a class contract.
A host may realize it.
A package may publish it.
But the portable law of the class must remain explicitly declared and inspectable.
</p>

<hr/>

<h2 id="scope">3. Scope</h2>

<p>
This document defines the normative class-level contract for:
</p>

<ul>
  <li>widget class identity and category,</li>
  <li>primary value posture,</li>
  <li>properties,</li>
  <li>methods,</li>
  <li>events,</li>
  <li>parts and subparts,</li>
  <li>object addressing,</li>
  <li>mutability and persistence boundaries,</li>
  <li>design-time versus runtime exposure,</li>
  <li>bounded behavior declaration surfaces,</li>
  <li>developer-defined widget class portability expectations.</li>
</ul>

<p>
This document does not define canonical <code>.frog</code> program syntax, full <code>.wfrog</code> packaging structure,
host rendering internals, or toolkit-private implementation details.
</p>

<p>
It also does not define the executable diagram primitives used to interact with a class instance.
Those belong to the interaction corridor and to the intrinsic UI primitive library.
This document only defines what those interaction forms are allowed to target.
</p>

<hr/>

<h2 id="relation-with-other-specifications">4. Relation with Other Specifications</h2>

<ul>
  <li><strong><code>Expression/Front panel.md</code></strong> defines front-panel composition at program-source level.</li>
  <li><strong><code>Expression/Widget.md</code></strong> defines widget instances as source-visible participants in a <code>.frog</code> program.</li>
  <li><strong><code>Expression/Widget interaction.md</code></strong> defines how diagrams interact with widgets through natural-value and object-style access.</li>
  <li><strong><code>Expression/Widget package (.wfrog).md</code></strong> defines machine-readable packaging of widget classes, composite publication, assets, and bounded behavior modules.</li>
  <li><strong><code>Expression/Widget behavior.md</code></strong> defines the doctrine and limits of portable bounded widget behavior.</li>
  <li><strong><code>Expression/Widget realization.md</code></strong> defines realization boundaries and visual-resource subordination.</li>
  <li><strong><code>Libraries/UI.md</code></strong> defines the intrinsic <code>frog.ui.*</code> primitive library used for executable widget interaction.</li>
</ul>

<p>
This document sits at the class-law layer.
It is the normative contract that packaging, source interaction, runtime interpretation, and host realization must respect.
</p>

<p>
The split is therefore:
</p>

<pre><code>Widget.md
  -&gt; instance-side source object

Widget class contract.md
  -&gt; class-side law

Widget interaction.md
  -&gt; diagram-side access model

Widget package (.wfrog).md
  -&gt; machine-readable publication

Widget behavior.md
  -&gt; bounded behavior doctrine

Widget realization.md
  -&gt; realization and visual-resource boundary
</code></pre>

<hr/>

<h2 id="ownership-boundary">5. Ownership Boundary</h2>

<ul>
  <li><strong>This contract owns:</strong> class-level widget law.</li>
  <li><strong><code>.wfrog</code> owns:</strong> machine-readable publication and packaging of class definitions, bounded behavior declarations, composite publication, and front-panel realization resources.</li>
  <li><strong><code>.frog</code> owns:</strong> canonical program meaning, widget participation, widget identity in program scope, and explicit diagram interaction surfaces.</li>
  <li><strong>Runtime owns:</strong> interpretation of the published widget contract and execution of portable behavior surfaces.</li>
  <li><strong>Host owns:</strong> concrete rendering, input collection, layout realization, and toolkit-private implementation details.</li>
  <li><strong>SVG or other visual assets own:</strong> visual shape and scalable presentation resources only.</li>
</ul>

<p>
A class contract MUST NOT be replaced by runtime-private conventions.
A visual asset MUST NOT become the semantic source of truth for widget law.
</p>

<p>
Likewise, a class contract MUST NOT be silently reconstructed from one particular runtime implementation.
The portable contract is published first.
Implementations conform to it second.
</p>

<hr/>

<h2 id="design-goals">6. Design Goals</h2>

<p>
The FROG widget class model is intended to combine the useful strengths of object-oriented UI systems,
component systems, and LabVIEW-style control/indicator intuition while remaining explicit and disciplined.
</p>

<p>
The design goals are:
</p>

<ul>
  <li>clear object surfaces,</li>
  <li>stable names and stable addressing,</li>
  <li>portable law across Python, Rust, and C/C++ runtimes,</li>
  <li>support for developer-defined widget classes,</li>
  <li>support for composite classes built on portable published surfaces,</li>
  <li>bounded behavior that remains inspectable,</li>
  <li>clean separation between semantic truth and host realization.</li>
</ul>

<p>
The model must be expressive enough to let developers create new widgets,
but constrained enough that the runtime does not become the hidden owner of meaning.
</p>

<hr/>

<h2 id="class-contract-purpose">7. Class Contract Purpose</h2>

<p>
A widget class contract answers the following questions:
</p>

<ul>
  <li>What is this class called?</li>
  <li>Which widget family or category does it belong to?</li>
  <li>Does it expose a primary value surface?</li>
  <li>Which properties are legal?</li>
  <li>Which methods are legal?</li>
  <li>Which events are legal?</li>
  <li>Which parts or subparts are stable and addressable?</li>
  <li>Which members are readable, writable, persistent, ephemeral, or runtime-only?</li>
  <li>Which behavior is portable and declaratively published?</li>
  <li>Which behavior is host-private and therefore non-portable?</li>
</ul>

<p>
A class contract therefore does not merely describe a widget informally.
It defines the legal contract surface that tools, validators, runtimes, and host realizations may rely on.
</p>

<hr/>

<h2 id="required-class-surfaces">8. Required Class Surfaces</h2>

<p>
A widget class contract MUST define:
</p>

<ul>
  <li>a stable <code>class_id</code>,</li>
  <li>a stable category,</li>
  <li>a primary value posture, even if the class has no primary value,</li>
  <li>a property inventory, which MAY be empty,</li>
  <li>a method inventory, which MAY be empty,</li>
  <li>an event inventory, which MAY be empty,</li>
  <li>a part model, either as a hierarchy, a flat inventory, or an explicit statement that the class exposes no addressable parts,</li>
  <li>mutability and persistence posture for exposed members,</li>
  <li>conformance expectations for runtimes claiming support.</li>
</ul>

<p>
A class contract SHOULD also define:
</p>

<ul>
  <li>member documentation,</li>
  <li>constraints and default values,</li>
  <li>design-time versus runtime availability,</li>
  <li>bounded portable behavior surfaces when the class requires more than passive member exposure,</li>
  <li>whether the class is primitive, composite, or externally derived from other published classes.</li>
</ul>

<p>
Empty inventories are allowed.
Ambiguous inventories are not.
A class that exposes no properties, methods, events, or parts MUST still make that posture explicit.
</p>

<hr/>

<h2 id="class-identity-and-category">9. Class Identity and Category</h2>

<p>
Every widget class MUST have a stable <code>class_id</code>.
That identifier is the portable identity of the class and MUST be globally stable within the relevant library or package domain.
</p>

<p>
Every widget class MUST also declare a category.
Typical categories include:
</p>

<ul>
  <li><code>control</code>,</li>
  <li><code>indicator</code>,</li>
  <li><code>container</code>,</li>
  <li><code>composite</code>,</li>
  <li><code>decorative</code>,</li>
  <li><code>hybrid</code>.</li>
</ul>

<p>
The category does not replace the full contract.
It provides a coarse semantic posture for tools, documentation, and library organization.
</p>

<p>
A class MAY also declare a broader lineage posture, such as:
</p>

<ul>
  <li>intrinsic primitive class,</li>
  <li>composite class,</li>
  <li>developer-defined external class,</li>
  <li>profile-gated class.</li>
</ul>

<p>
Such classification helps tools organize the ecosystem, but it MUST NOT replace the explicit declaration of members, parts, and behavior surfaces.
</p>

<hr/>

<h2 id="primary-value-model">10. Primary Value Model</h2>

<p>
A widget class MAY define a primary value surface.
The primary value surface is the natural-value participation path used by <code>widget_value</code> and similar natural-value interaction forms.
</p>

<p>
If a class defines a primary value surface, the contract MUST state:
</p>

<ul>
  <li>its value type,</li>
  <li>its access posture,</li>
  <li>whether user interaction may modify it,</li>
  <li>whether diagram interaction may modify it,</li>
  <li>whether runtime realization may update it,</li>
  <li>whether the surface is persistent or ephemeral,</li>
  <li>whether the primary value is mirrored as a named property such as <code>value</code>.</li>
</ul>

<p>
If a class has no primary value, the contract MUST say so explicitly.
</p>

<p>
The primary value surface MUST NOT be redefined implicitly by one runtime.
If a class uses a primary value, its semantics are part of portable class law.
</p>

<p>
The contract SHOULD also state whether:
</p>

<ul>
  <li>the primary value is the default interaction surface for the class,</li>
  <li>the primary value is representable through object-style property access,</li>
  <li>the primary value may trigger declared events when updated,</li>
  <li>the primary value participates in derived presentation mappings declared by portable behavior surfaces.</li>
</ul>

<hr/>

<h2 id="property-model">11. Property Model</h2>

<p>
A property is a named object member exposed by the widget class contract.
Properties are the standard portable mechanism for object-style state inspection and state modification.
</p>

<p>
FROG does not require a separate normative <em>attribute</em> category at class-law level.
When a member is a published named state surface, it is modeled as a property.
This avoids ambiguity between “attribute” and “property” and keeps portable widget law explicit.
</p>

<p>
Each property declaration SHOULD include:
</p>

<ul>
  <li><code>name</code>,</li>
  <li><code>type</code>,</li>
  <li><code>access</code> — <code>read_only</code>, <code>write_only</code>, or <code>read_write</code>,</li>
  <li><code>mutability</code>,</li>
  <li><code>persistent</code>,</li>
  <li><code>default</code>, when applicable,</li>
  <li><code>nullable</code>, when applicable,</li>
  <li><code>constraints</code>, when applicable,</li>
  <li><code>availability</code> — design-time, runtime, or shared,</li>
  <li><code>applies_to_part</code> when the property is part-local rather than root-local.</li>
</ul>

<p>
A property is legal only if it is:
</p>

<ul>
  <li>published by the class contract, or</li>
  <li>introduced through an explicitly allowed extension model that remains inspectable and non-ambiguous.</li>
</ul>

<p>
A runtime MUST NOT silently invent portable properties that are not declared by the contract.
</p>

<p>
The property model MUST be able to express at least:
</p>

<ul>
  <li>semantic state properties,</li>
  <li>presentation-facing properties,</li>
  <li>interaction-facing properties,</li>
  <li>derived read-only properties,</li>
  <li>runtime-only observable properties when explicitly declared.</li>
</ul>

<p>
But declaring a property does not automatically make it persistent, design-time editable, or portable across all profiles.
Those postures must be stated explicitly.
</p>

<hr/>

<h2 id="method-model">12. Method Model</h2>

<p>
A method is a named action surface exposed by the widget class contract.
Methods are used for explicit object-style invocation when state mutation or behavior triggering is more appropriately modeled as an action than as a property write.
</p>

<p>
Each method declaration SHOULD include:
</p>

<ul>
  <li><code>name</code>,</li>
  <li>typed parameters,</li>
  <li>return type,</li>
  <li>availability boundary,</li>
  <li>declared side-effect posture,</li>
  <li>declared portability posture,</li>
  <li>part scope when the method is part-local.</li>
</ul>

<p>
Method contracts SHOULD make it clear whether a method:
</p>

<ul>
  <li>updates widget state,</li>
  <li>requests host behavior,</li>
  <li>emits events,</li>
  <li>is purely local to the widget object,</li>
  <li>depends on host-private functionality.</li>
</ul>

<p>
A runtime MUST NOT expose undeclared methods as portable class law.
A host-specific method MAY exist, but it MUST be clearly marked as non-portable.
</p>

<p>
Methods are especially important for composite and richer developer-defined widgets,
because they allow published behavior entry points without forcing everything into writable properties.
But method identity, signature, and portability posture must remain explicit.
</p>

<hr/>

<h2 id="event-model">13. Event Model</h2>

<p>
An event is a named observable emission surface exposed by the widget class contract.
Events allow widgets and their parts to publish runtime-observable occurrences without requiring host-private knowledge.
</p>

<p>
Each event declaration SHOULD include:
</p>

<ul>
  <li><code>name</code>,</li>
  <li>payload type or payload schema,</li>
  <li>origin surface,</li>
  <li>delivery posture,</li>
  <li>part scope when the event is emitted by a declared part,</li>
  <li>ordering expectations when relevant,</li>
  <li>portability posture.</li>
</ul>

<p>
Events MAY be emitted by the widget root or by a declared part.
Event names and payload shapes form part of the portable class contract.
</p>

<p>
The class contract does not require one universal event scheduling model for all runtimes,
but it MUST define enough structure that runtimes do not disagree on event identity, payload shape,
or basic source surface.
</p>

<p>
The contract SHOULD also make explicit whether:
</p>

<ul>
  <li>the event is user-driven,</li>
  <li>the event is diagram-triggered,</li>
  <li>the event is realization-driven,</li>
  <li>the event is derived from bounded portable behavior,</li>
  <li>the event is purely advisory and host-private.</li>
</ul>

<hr/>

<h2 id="part-model">14. Part Model</h2>

<p>
A part is a stable addressable sub-surface of a widget object.
Parts allow a widget to expose internal structure without collapsing into one opaque box and without turning host implementation details into semantic truth.
</p>

<p>
A widget class MAY expose no addressable parts.
If it does expose parts, those parts MUST be explicitly declared.
</p>

<p>
A part declaration SHOULD include:
</p>

<ul>
  <li><code>part_id</code>,</li>
  <li><code>kind</code>,</li>
  <li>optional parent part,</li>
  <li>optional child parts,</li>
  <li>optional local properties,</li>
  <li>optional local methods,</li>
  <li>optional local events,</li>
  <li>optional addressing aliases when explicitly supported.</li>
</ul>

<p>
Only declared parts are portable.
Toolkit-private visual fragments are not portable parts unless the class contract declares them as such.
</p>

<p>
A part MUST NOT be declared merely because one runtime happens to expose an internal subtree.
Portable parts are declared because they are part of the published class surface, not because they are convenient internals.
</p>

<hr/>

<h2 id="object-addressing">15. Object Addressing</h2>

<p>
FROG object-style widget interaction requires stable object addressing.
The class contract therefore supports contract-based addressing of:
</p>

<ul>
  <li>the widget root object,</li>
  <li>named root properties,</li>
  <li>named root methods,</li>
  <li>named root events,</li>
  <li>named parts,</li>
  <li>part-local members where allowed by the contract.</li>
</ul>

<p>
Portable addressing MUST be:
</p>

<ul>
  <li>string-stable,</li>
  <li>contract-based,</li>
  <li>independent of host-private object identities,</li>
  <li>independent of toolkit-private node trees.</li>
</ul>

<p>
Illustrative addressing forms include:
</p>

<ul>
  <li><code>root</code></li>
  <li><code>property:value</code></li>
  <li><code>method:reset_to_default_style</code></li>
  <li><code>part:value_text</code></li>
  <li><code>part:value_text/property:foreground_color</code></li>
</ul>

<p>
The exact textual addressing notation used by higher-level source or runtime APIs MAY vary,
but the portable member identity MUST remain derivable from the class contract.
</p>

<p>
The contract must therefore preserve at least:
</p>

<ul>
  <li>stable root-member naming,</li>
  <li>stable part naming,</li>
  <li>stable part-local member naming,</li>
  <li>stable distinction between root and part-local surfaces.</li>
</ul>

<hr/>

<h2 id="mutability-and-persistence">16. Mutability and Persistence</h2>

<p>
The class contract MUST distinguish:
</p>

<ul>
  <li>construction-only members,</li>
  <li>design-time mutable members,</li>
  <li>runtime mutable members,</li>
  <li>persistent members,</li>
  <li>ephemeral members.</li>
</ul>

<p>
This distinction is required so that runtimes, compilers, validators, and IDEs do not guess widget law differently.
</p>

<p>
Mutability posture SHOULD be declared per member.
When a class-level default mutability posture exists, member-level declarations MAY override it.
</p>

<p>
Persistence posture SHOULD indicate whether a member:
</p>

<ul>
  <li>belongs in source-owned persisted state,</li>
  <li>belongs in package-owned defaults,</li>
  <li>exists only during execution,</li>
  <li>is computed on demand and never persisted.</li>
</ul>

<p>
This distinction also governs how authored `.frog` instances, package defaults, and runtime state may interact.
A runtime must not silently persist an ephemeral surface as though it were part of canonical source.
</p>

<hr/>

<h2 id="design-time-vs-runtime">17. Design-Time vs Runtime</h2>

<p>
A widget class contract MAY distinguish:
</p>

<ul>
  <li>design-time surface,</li>
  <li>runtime surface,</li>
  <li>shared surface.</li>
</ul>

<p>
This distinction is important because some members participate in authoring and layout,
while others participate in live execution and user interaction.
</p>

<p>
For example:
</p>

<ul>
  <li>a label string may be design-time and runtime mutable,</li>
  <li>a geometry property may be design-time mutable but runtime read-only,</li>
  <li>a transient render-status property may be runtime-only and ephemeral.</li>
</ul>

<p>
Availability posture should therefore be declared explicitly whenever ambiguity would otherwise arise.
This prevents tools from inventing contradictory assumptions about when a surface may be read, written, invoked, or observed.
</p>

<hr/>

<h2 id="bounded-behavior-surfaces">18. Bounded Behavior Surfaces</h2>

<p>
Some widget classes require more than passive member exposure.
They may require behavior such as validation, normalization, derived state updates,
value-to-presentation mapping, or event emission rules.
</p>

<p>
Portable widget behavior MUST remain bounded and inspectable.
For that reason, a widget class contract MAY declare behavior surfaces in the following categories:
</p>

<ul>
  <li><strong>declarative behavior</strong> — state rules, derived bindings, constraints, mappings, or triggers expressed in a machine-readable portable form,</li>
  <li><strong>bounded pure expressions</strong> — portable expression units without host side effects,</li>
  <li><strong>host-private hooks</strong> — explicitly non-portable behavior entry points owned by a specific runtime or host.</li>
</ul>

<p>
Host-private hooks MUST NOT redefine the portable class contract.
They MAY realize or optimize host integration, but they do not become semantic truth for the widget class.
</p>

<p>
The class contract should therefore make explicit which behavior surfaces are:
</p>

<ul>
  <li>portable and required for conforming support,</li>
  <li>portable but optional,</li>
  <li>non-portable and host-private.</li>
</ul>

<p>
This is the key mechanism that allows richer widget behavior without collapsing the language into unrestricted arbitrary host code.
</p>

<hr/>

<h2 id="composite-widget-classes">19. Composite Widget Classes</h2>

<p>
A widget class MAY be composite.
A composite widget class is a published class whose contract surface is built from explicitly declared parts, member surfaces, and bounded behavior, potentially reusing other published widget classes internally.
</p>

<p>
A composite class MUST still publish its own portable class law explicitly.
It MUST NOT require a runtime to infer its public contract from hidden internal composition alone.
</p>

<p>
A composite class SHOULD therefore publish:
</p>

<ul>
  <li>its stable <code>class_id</code>,</li>
  <li>its category,</li>
  <li>its primary value posture, if any,</li>
  <li>its public properties, methods, and events,</li>
  <li>its stable public parts,</li>
  <li>its bounded behavior surfaces,</li>
  <li>its composition posture when relevant for tooling and inspection.</li>
</ul>

<p>
A runtime may internally realize a composite widget in many ways.
That freedom does not change the obligation to preserve the published composite contract surface.
</p>

<hr/>

<h2 id="developer-defined-widget-classes">20. Developer-Defined Widget Classes</h2>

<p>
FROG is intended to support developer-defined widget classes.
A developer-defined widget class is portable only if its class law is published in an inspectable form through the widget packaging model.
</p>

<p>
A developer-defined class MUST therefore publish:
</p>

<ul>
  <li>a stable <code>class_id</code>,</li>
  <li>its category,</li>
  <li>its primary value posture,</li>
  <li>its property inventory,</li>
  <li>its method inventory,</li>
  <li>its event inventory,</li>
  <li>its part model,</li>
  <li>its bounded portable behavior surfaces, if any.</li>
</ul>

<p>
A runtime may choose not to implement a given developer-defined class,
but it MUST NOT reinterpret that class arbitrarily while claiming conformance.
</p>

<p>
This rule is crucial:
developer freedom is allowed, but only through published portable law.
A developer-defined class is not portable merely because one runtime can render or execute it.
It becomes portable when its class contract is explicit, inspectable, and preserved by implementations that claim support.
</p>

<hr/>

<h2 id="extension-and-portability-rules">21. Extension and Portability Rules</h2>

<p>
A widget class MAY define extension points.
Those extension points MUST be explicit and bounded.
</p>

<p>
Portable extension is allowed only if:
</p>

<ul>
  <li>the extension model is published,</li>
  <li>the extension does not alter the identity of already published members,</li>
  <li>the extension does not silently change primary value semantics,</li>
  <li>the extension remains inspectable to tools and runtimes.</li>
</ul>

<p>
Host-specific extension is allowed only if:
</p>

<ul>
  <li>it is clearly marked as non-portable,</li>
  <li>it does not redefine published class law,</li>
  <li>it does not cause tools to misread the portable contract,</li>
  <li>it does not force one runtime implementation to become the language definition.</li>
</ul>

<p>
Extension therefore exists within a discipline:
it may enlarge a class surface,
but it must not corrupt already published portable meaning.
</p>

<hr/>

<h2 id="conformance">22. Conformance</h2>

<p>
A runtime is conforming for a widget class only if it preserves the published widget class contract
for all portable members and portable behaviors it claims to implement.
</p>

<p>
Conformance therefore requires, at minimum:
</p>

<ul>
  <li>stable interpretation of the class identity,</li>
  <li>stable interpretation of the primary value surface, if any,</li>
  <li>stable interpretation of declared properties, methods, events, and parts,</li>
  <li>stable handling of mutability and persistence posture,</li>
  <li>no silent invention of new portable class law,</li>
  <li>clear marking of non-portable host extensions.</li>
</ul>

<p>
Partial implementation is allowed.
However, a runtime claiming partial implementation MUST identify which portable surfaces it implements and which it does not.
</p>

<p>
Conformance is therefore based on published law and declared support,
not on informal similarity of appearance or on one runtime’s internal convenience model.
</p>

<hr/>

<h2 id="example-class-outline">23. Example Class Outline</h2>

<pre><code>{
  "class_id": "frog.widgets.numeric_indicator",
  "category": "indicator",
  "primary_value": {
    "present": true,
    "type": "float64",
    "access": "read_write",
    "user_mutable": false,
    "diagram_mutable": true,
    "runtime_mutable": true,
    "persistent": false,
    "mirrored_property": "value"
  },
  "properties": [
    {
      "name": "value",
      "type": "float64",
      "access": "read_write",
      "mutability": "runtime",
      "persistent": false,
      "availability": "shared"
    },
    {
      "name": "format",
      "type": "string",
      "access": "read_write",
      "mutability": "design_and_runtime",
      "persistent": true,
      "default": "%.3f",
      "availability": "shared"
    },
    {
      "name": "foreground_color",
      "type": "frog.color.rgba8",
      "access": "read_write",
      "mutability": "design_and_runtime",
      "persistent": true,
      "availability": "shared"
    }
  ],
  "methods": [
    {
      "name": "reset_to_default_style",
      "parameters": [],
      "returns": { "type": "void" },
      "availability": "runtime",
      "side_effect_posture": "widget_local_state_update",
      "portability": "portable"
    }
  ],
  "events": [
    {
      "name": "value_rendered",
      "payload": { "type": "string" },
      "origin_surface": "part:value_text",
      "delivery_posture": "runtime_observable",
      "portability": "portable"
    }
  ],
  "parts": [
    {
      "part_id": "root",
      "kind": "container"
    },
    {
      "part_id": "value_text",
      "kind": "text",
      "parent": "root",
      "local_properties": [
        {
          "name": "foreground_color",
          "type": "frog.color.rgba8",
          "access": "read_write"
        }
      ]
    }
  ],
  "behavior_surfaces": {
    "declarative": [
      {
        "name": "formatted_text_projection",
        "kind": "value_to_text_mapping"
      }
    ],
    "bounded_pure_expressions": [],
    "host_private_hooks": [
      {
        "name": "native_text_rasterization",
        "portability": "non_portable"
      }
    ]
  }
}</code></pre>

<p>
This outline is illustrative rather than exhaustive.
Its role is to show that a portable widget class can expose:
</p>

<ul>
  <li>a stable identity,</li>
  <li>a primary value posture,</li>
  <li>properties, methods, events, and parts,</li>
  <li>bounded portable behavior,</li>
  <li>explicitly non-portable host-private hooks without allowing those hooks to redefine the portable contract.</li>
</ul>
