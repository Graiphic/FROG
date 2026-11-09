<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Widget Behavior</h1>

<p align="center">
  <strong>Normative behavior boundary for widget classes, bounded package-published reaction rules, and host-private realization support</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#why-this-document-exists">2. Why This Document Exists</a></li>
  <li><a href="#scope">3. Scope</a></li>
  <li><a href="#what-widget-behavior-means-in-frog">4. What Widget Behavior Means in FROG</a></li>
  <li><a href="#ownership-boundary">5. Ownership Boundary</a></li>
  <li><a href="#behavior-levels">6. Behavior Levels</a></li>
  <li><a href="#intrinsic-class-behavior">7. Intrinsic Class Behavior</a></li>
  <li><a href="#declarative-behavior-rules">8. Declarative Behavior Rules</a></li>
  <li><a href="#bounded-expressions">9. Bounded Expressions</a></li>
  <li><a href="#host-private-implementation-support">10. Host-Private Implementation Support</a></li>
  <li><a href="#event-emission-and-state-updates">11. Event Emission and State Updates</a></li>
  <li><a href="#mutability-and-safety">12. Mutability and Safety</a></li>
  <li><a href="#interaction-with-properties-methods-events-and-parts">13. Interaction with Properties, Methods, Events, and Parts</a></li>
  <li><a href="#what-must-not-happen">14. What Must Not Happen</a></li>
  <li><a href="#publication-through-wfrog">15. Publication Through <code>.wfrog</code></a></li>
  <li><a href="#portability-across-runtimes">16. Portability Across Runtimes</a></li>
  <li><a href="#status">17. Status</a></li>
  <li><a href="#license">18. License</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the normative boundary for widget behavior in FROG.
</p>

<p>
Widget behavior concerns how a widget reacts over time to:
</p>

<ul>
  <li>value updates,</li>
  <li>property changes,</li>
  <li>method invocation,</li>
  <li>part-local interaction,</li>
  <li>event emission conditions,</li>
  <li>bounded internal state transitions.</li>
</ul>

<p>
This document does not define the entire <code>.wfrog</code> package structure. That serialization format is owned by <code>Widget package (.wfrog).md</code>.
</p>

<p>
Instead, this document defines what categories of behavior are allowed, how they are separated architecturally, and which behavior surfaces remain portable and reviewable.
</p>

<p>
Its purpose is not to make behavior disappear.
Its purpose is to prevent behavior from becoming hidden, arbitrary, or runtime-defined by accident.
</p>

<hr/>

<h2 id="why-this-document-exists">2. Why This Document Exists</h2>

<p>
Without an explicit behavior boundary, widget systems tend to collapse into one of two failures:
</p>

<ul>
  <li>all behavior becomes runtime-private and therefore non-portable,</li>
  <li>all behavior becomes unrestricted package-defined arbitrary code and therefore non-auditable.</li>
</ul>

<p>
FROG rejects both extremes.
</p>

<p>
A FROG widget may have rich interaction and rich internal reaction, but the public behavior law of that widget must remain explicit and inspectable.
</p>

<p>
This document therefore exists to keep the behavior corridor disciplined:
</p>

<ul>
  <li>rich enough for real widgets,</li>
  <li>bounded enough for portability and auditability,</li>
  <li>explicit enough that one runtime never becomes the hidden owner of widget behavior law.</li>
</ul>

<hr/>

<h2 id="scope">3. Scope</h2>

<p>
This document defines:
</p>

<ul>
  <li>the allowed categories of widget behavior in FROG,</li>
  <li>the separation between intrinsic behavior, declarative behavior, bounded expressions, and host-private support,</li>
  <li>the relationship between behavior and public object surfaces such as properties, methods, events, and parts,</li>
  <li>the portability boundary across runtime families.</li>
</ul>

<p>
This document does not define:
</p>

<ul>
  <li>the full widget package JSON structure,</li>
  <li>the full widget class contract structure,</li>
  <li>the full host realization resource model,</li>
  <li>the implementation internals of a given runtime toolkit.</li>
</ul>

<p>
It also does not redefine widget class law.
It defines how published widget law may react and evolve without escaping into undocumented runtime behavior or unrestricted host scripting.
</p>

<hr/>

<h2 id="what-widget-behavior-means-in-frog">4. What Widget Behavior Means in FROG</h2>

<p>
In FROG, widget behavior means the normative reaction rules associated with a widget class or composite widget class.
</p>

<p>
Examples include:
</p>

<ul>
  <li>coercing a numeric value into a valid range,</li>
  <li>preventing interaction when <code>interaction.enabled</code> is false,</li>
  <li>emitting <code>value_changed</code> after a committed value update,</li>
  <li>routing a button click to an increment action,</li>
  <li>mapping a changed object property onto part-facing appearance state.</li>
</ul>

<p>
Behavior therefore sits between:
</p>

<ul>
  <li>public object law,</li>
  <li>runtime realization,</li>
  <li>diagram-facing interaction.</li>
</ul>

<p>
It is neither just “visual refresh logic” nor a free-form scripting substrate.
It is the published reaction doctrine that explains how legal widget surfaces evolve and interact over time.
</p>

<hr/>

<h2 id="ownership-boundary">5. Ownership Boundary</h2>

<p>
The following distinctions are normative:
</p>

<pre><code>widget class contract   -&gt; what public object surfaces exist
widget behavior         -&gt; how those surfaces react and evolve
widget realization      -&gt; how those surfaces are rendered or hosted
runtime-private code    -&gt; how one runtime implements support details
</code></pre>

<p>
This means:
</p>

<ul>
  <li>behavior does not create undocumented public properties,</li>
  <li>behavior does not override published class law,</li>
  <li>behavior does not turn SVG into semantic truth,</li>
  <li>runtime-private implementation support does not become the normative source of widget behavior law.</li>
</ul>

<p>
Likewise:
</p>

<pre><code>portable bounded behavior
    !=
arbitrary package-defined host code

realization support
    !=
public behavior law

one runtime implementation
    !=
the behavioral definition of the class
</code></pre>

<hr/>

<h2 id="behavior-levels">6. Behavior Levels</h2>

<p>
FROG widget behavior is divided into four levels.
</p>

<h3>6.1 Intrinsic class behavior</h3>

<p>
This is the built-in reaction law that belongs to the widget class definition itself.
</p>

<h3>6.2 Declarative behavior rules</h3>

<p>
These are explicit published rules that describe reaction without unrestricted arbitrary code.
</p>

<h3>6.3 Bounded expressions</h3>

<p>
These are constrained expression-driven reactions used where declarative mapping alone is insufficient.
</p>

<h3>6.4 Host-private implementation support</h3>

<p>
These are runtime-specific internal details required to realize the published behavior on a host toolkit.
</p>

<p>
Portable widget behavior must remain concentrated in the first three levels. The fourth level exists only to support realization.
</p>

<p>
This hierarchy is fundamental:
portable meaning must live above host-private support,
not be reconstructed from it after the fact.
</p>

<hr/>

<h2 id="intrinsic-class-behavior">7. Intrinsic Class Behavior</h2>

<p>
Intrinsic class behavior is the behavior that a widget class always has by virtue of its published class law.
</p>

<p>
Examples:
</p>

<ul>
  <li>a numeric control may clamp or reject values outside its allowed range according to its value model,</li>
  <li>a button may emit <code>pressed</code> and <code>released</code> when host interaction occurs,</li>
  <li>a disabled widget may reject user-originated edit operations,</li>
  <li>a chart may append samples using a published update method contract.</li>
</ul>

<p>
Intrinsic behavior belongs to the widget definition. It MUST be inspectable from package-published widget content. It MUST NOT exist only as undocumented runtime convention.
</p>

<p>
Intrinsic behavior is the strongest portable behavioral tier.
If a runtime claims support for a class, it must preserve the intrinsic behavior surfaces that are part of the published class law.
</p>

<hr/>

<h2 id="declarative-behavior-rules">8. Declarative Behavior Rules</h2>

<p>
Declarative behavior rules are package-published reaction rules that remain explicit and reviewable.
</p>

<p>
Examples:
</p>

<ul>
  <li>when <code>interaction.enabled</code> is false, suppress user-originated part actions,</li>
  <li>when <code>value</code> changes, refresh <code>value_display</code>,</li>
  <li>when <code>label.text</code> changes, refresh the <code>label</code> part,</li>
  <li>when a button part is activated, invoke a published class method.</li>
</ul>

<p>
Declarative behavior rules SHOULD prefer:
</p>

<ul>
  <li>stable source identifiers,</li>
  <li>stable property and part references,</li>
  <li>explicit event triggers,</li>
  <li>bounded state consequences.</li>
</ul>

<p>
A declarative rule MUST remain understandable without reading one runtime implementation.
</p>

<p>
Declarative rules are therefore the preferred publication surface whenever the intended reaction can be expressed without imperative host-specific logic.
</p>

<hr/>

<h2 id="bounded-expressions">9. Bounded Expressions</h2>

<p>
Some behaviors require more than simple declarative mappings.
</p>

<p>
FROG therefore allows bounded expressions in widget behavior where necessary, but only under strict limits.
</p>

<p>
Bounded expressions MAY be used for:
</p>

<ul>
  <li>value normalization,</li>
  <li>derived appearance state,</li>
  <li>simple computed enablement or visibility,</li>
  <li>simple event payload shaping,</li>
  <li>composition-local routing logic.</li>
</ul>

<p>
Bounded expressions MUST remain:
</p>

<ul>
  <li>inspectable,</li>
  <li>portable,</li>
  <li>deterministic,</li>
  <li>side-effect-bounded,</li>
  <li>non-host-specific in their public meaning.</li>
</ul>

<p>
Bounded expressions MUST NOT become a hidden general-purpose scripting loophole that redefines widget class law.
</p>

<p>
In particular, bounded expressions MUST NOT:
</p>

<ul>
  <li>arbitrarily create new public members,</li>
  <li>call host-private APIs as part of portable public meaning,</li>
  <li>silently mutate forbidden public surfaces,</li>
  <li>depend on one runtime’s internal object graph to preserve meaning.</li>
</ul>

<hr/>

<h2 id="host-private-implementation-support">10. Host-Private Implementation Support</h2>

<p>
A runtime may require private implementation support in order to realize a published widget behavior.
</p>

<p>
Examples:
</p>

<ul>
  <li>connecting host toolkit focus events to published <code>focus_gained</code> and <code>focus_lost</code>,</li>
  <li>mapping platform-native input handling to published button events,</li>
  <li>performing redraw scheduling,</li>
  <li>managing accessibility bridge objects,</li>
  <li>managing toolkit-private caches or retained rendering resources.</li>
</ul>

<p>
These details are allowed and expected.
</p>

<p>
However, host-private support MUST NOT:
</p>

<ul>
  <li>invent undocumented portable public members,</li>
  <li>silently change the legality of published members,</li>
  <li>silently redefine event meaning,</li>
  <li>silently redefine bounded behavior law.</li>
</ul>

<p>
Host-private support therefore exists to implement published behavior,
not to become the hidden definition of that behavior.
</p>

<hr/>

<h2 id="event-emission-and-state-updates">11. Event Emission and State Updates</h2>

<p>
Behavior may cause event emission and state updates, but only in ways consistent with published widget law.
</p>

<p>
In particular:
</p>

<ul>
  <li>an event MUST correspond to a published event identifier,</li>
  <li>an emitted payload MUST remain compatible with the published event payload shape,</li>
  <li>a state update MUST target a published writable surface or an explicitly internal surface,</li>
  <li>behavior MUST NOT mutate read-only public properties through hidden side channels.</li>
</ul>

<p>
The public event and mutation surfaces therefore remain bounded by the class contract, not by runtime convenience.
</p>

<p>
If a class wants a mutation or event to be portable,
that mutation or event must be published as such.
Behavior law cannot smuggle new public semantics through hidden internal transitions.
</p>

<hr/>

<h2 id="mutability-and-safety">12. Mutability and Safety</h2>

<p>
Widget behavior is constrained by published mutability posture.
</p>

<p>
If a property is:
</p>

<ul>
  <li>design-time only, runtime behavior MUST NOT mutate it as a public runtime surface,</li>
  <li>read-only, runtime behavior MUST NOT expose hidden public writes to it,</li>
  <li>internal, the runtime MAY change it privately so long as this does not alter the published public contract.</li>
</ul>

<p>
Behavior rules SHOULD preserve deterministic and reviewable object evolution.
</p>

<p>
Behavior safety therefore depends on respecting:
</p>

<ul>
  <li>member mutability posture,</li>
  <li>member persistence posture,</li>
  <li>declared event contracts,</li>
  <li>declared part boundaries,</li>
  <li>declared portable versus non-portable behavior tiers.</li>
</ul>

<hr/>

<h2 id="interaction-with-properties-methods-events-and-parts">13. Interaction with Properties, Methods, Events, and Parts</h2>

<p>
Widget behavior depends on the published object model and does not replace it.
</p>

<p>
Accordingly:
</p>

<ul>
  <li>properties define stable data-facing surfaces,</li>
  <li>methods define stable operation surfaces,</li>
  <li>events define stable observation surfaces,</li>
  <li>parts define stable substructure surfaces,</li>
  <li>behavior defines how these published surfaces react together.</li>
</ul>

<p>
A behavior rule may therefore:
</p>

<ul>
  <li>read a published property,</li>
  <li>write a published writable property,</li>
  <li>invoke a published internal or public method where allowed,</li>
  <li>emit a published event,</li>
  <li>target a published part.</li>
</ul>

<p>
But behavior MUST NOT bypass the published object model by inventing hidden portable semantics.
</p>

<p>
This means that behavior is subordinate to object law:
it coordinates published surfaces,
but does not create a second invisible object model beside them.
</p>

<hr/>

<h2 id="what-must-not-happen">14. What Must Not Happen</h2>

<p>
The following are prohibited as normative behavior doctrine:
</p>

<ul>
  <li>placing the only true behavior law in runtime-private code,</li>
  <li>placing behavior law only in SVG assets,</li>
  <li>using unrestricted arbitrary package-defined host code as the primary behavioral publication surface,</li>
  <li>using behavior declarations to create undocumented public properties, methods, events, or parts,</li>
  <li>using one runtime's convenience API as the definition of portable widget law.</li>
</ul>

<p>
Likewise, the following are prohibited:
</p>

<ul>
  <li>treating host-private callbacks as if they were portable published behavior law,</li>
  <li>treating realization artifacts as if they owned public reaction semantics,</li>
  <li>treating package-defined imperative escape hatches as the normal portable publication model,</li>
  <li>making support for a widget class depend on undocumented behavior contracts.</li>
</ul>

<hr/>

<h2 id="publication-through-wfrog">15. Publication Through <code>.wfrog</code></h2>

<p>
Behavior surfaces are typically published through <code>.wfrog</code> packages.
</p>

<p>
The package document owns the serialization structure of that publication.
</p>

<p>
This document instead constrains what those package-published behavior surfaces are allowed to mean.
</p>

<p>
In particular, a package may publish:
</p>

<ul>
  <li>intrinsic class behavior declarations,</li>
  <li>declarative rules,</li>
  <li>bounded expressions,</li>
  <li>behavior routing for composite widgets.</li>
</ul>

<p>
A package MUST NOT claim unrestricted opaque behavior as normative portable widget law.
</p>

<p>
Publication through <code>.wfrog</code> therefore enables portability only when the published behavior remains within the bounded and inspectable doctrine defined here.
</p>

<hr/>

<h2 id="portability-across-runtimes">16. Portability Across Runtimes</h2>

<p>
Portable FROG widget behavior must survive across Python, Rust, and C/C++ runtime families.
</p>

<p>
This does not require identical host-internal code.
</p>

<p>
It does require:
</p>

<ul>
  <li>equivalent published public object surfaces,</li>
  <li>equivalent public event meaning,</li>
  <li>equivalent mutability posture,</li>
  <li>equivalent bounded behavior meaning.</li>
</ul>

<p>
Runtime-specific differences in toolkit, rendering backend, event loop integration, or accessibility bridge implementation are acceptable so long as they do not redefine the public behavior contract.
</p>

<p>
Portability therefore means behavioral equivalence at the published contract level,
not identical private implementation strategy.
</p>

<hr/>

<h2 id="status">17. Status</h2>

<p>
This document defines the normative doctrine of widget behavior in FROG.
</p>

<p>
Its closure direction is:
</p>

<ul>
  <li>explicit class-owned behavior,</li>
  <li>bounded portable declarative reaction,</li>
  <li>support for developer-defined composite widgets,</li>
  <li>clear separation from host-private realization support.</li>
</ul>

<p>
Its role in the widget corridor is to keep behavior rich enough for real widgets while remaining portable, inspectable, and bounded.
</p>

<hr/>

<h2 id="license">18. License</h2>

<p>
See the repository-level license information for the licensing terms governing this specification.
</p>
