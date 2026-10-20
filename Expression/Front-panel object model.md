<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Front-Panel Object Model</h1>

<p align="center">
  <strong>Normative architectural model for front-panel objects, widget classes, widget instances, and runtime-facing embodiment boundaries</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#why-this-document-exists">2. Why This Document Exists</a></li>
  <li><a href="#scope">3. Scope</a></li>
  <li><a href="#architectural-position">4. Architectural Position</a></li>
  <li><a href="#core-design-rule">5. Core Design Rule</a></li>
  <li><a href="#front-panel-object-taxonomy">6. Front-Panel Object Taxonomy</a></li>
  <li><a href="#baseline-standardization-strategy">7. Baseline Standardization Strategy</a></li>
  <li><a href="#the-six-required-separations">8. The Six Required Separations</a></li>
  <li><a href="#front-panel-object-class-model">9. Front-Panel Object Class Model</a></li>
  <li><a href="#front-panel-object-instance-model">10. Front-Panel Object Instance Model</a></li>
  <li><a href="#common-minimal-contract-for-all-standard-widgets">11. Common Minimal Contract for All Standard Widgets</a></li>
  <li><a href="#minimum-object-surface-requirement">12. Minimum Object-Surface Requirement</a></li>
  <li><a href="#baseline-family-intent">13. Baseline Family Intent</a></li>
  <li><a href="#property-vs-method-vs-event-vs-part-vs-realization-vs-runtime-state">14. Property vs Method vs Event vs Part vs Realization vs Runtime State</a></li>
  <li><a href="#relation-with-frogui-primitives">15. Relation with <code>frog.ui.*</code> Primitives</a></li>
  <li><a href="#relation-with-realization-families">16. Relation with Realization Families</a></li>
  <li><a href="#custom-widget-extension-model">17. Custom Widget Extension Model</a></li>
  <li><a href="#runtime-contract-posture">18. Runtime Contract Posture</a></li>
  <li><a href="#v1-design-decisions">19. V1 Design Decisions</a></li>
  <li><a href="#deferred-surfaces">20. Deferred Surfaces</a></li>
  <li><a href="#validation-posture">21. Validation Posture</a></li>
  <li><a href="#summary">22. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the architectural object model for front-panel widgets in FROG.
</p>

<p>
Its purpose is to provide one coherent normative foundation for:
</p>

<ul>
  <li>standard intrinsic widgets,</li>
  <li>future standardized widgets,</li>
  <li>developer-defined custom widgets,</li>
  <li>front-panel widget instances in canonical source,</li>
  <li>object-style widget interaction in diagrams,</li>
  <li>runtime-side embodiment across multiple host families.</li>
</ul>

<p>
The front-panel object model must remain:
</p>

<ul>
  <li>small enough to standardize,</li>
  <li>strong enough to prevent runtime drift,</li>
  <li>clear enough to support machine-readable publication,</li>
  <li>open enough to allow custom widget ecosystems later.</li>
</ul>

<p>
This document therefore serves as the architectural bridge between the generic widget doctrine and the concrete standardized widget baseline.
</p>

<hr/>

<h2 id="why-this-document-exists">2. Why This Document Exists</h2>

<p>
The widget corridor is only stable if FROG answers the following questions explicitly:
</p>

<ul>
  <li>What is a front-panel object in architectural terms?</li>
  <li>What belongs to widget class law?</li>
  <li>What belongs to widget instances in canonical source?</li>
  <li>What belongs to semantic value participation?</li>
  <li>What belongs to realization?</li>
  <li>What belongs only to runtime-private embodiment?</li>
</ul>

<p>
Without such a document, several kinds of drift appear quickly:
</p>

<ul>
  <li>widgets collapse into passive values plus skins,</li>
  <li>runtime implementations become the de facto owner of widget behavior and object surfaces,</li>
  <li>property and method surfaces become inconsistent across widget families,</li>
  <li>dynamic public text collapses into SVG or toolkit-private ownership,</li>
  <li>custom widget publication becomes impossible to validate coherently.</li>
</ul>

<p>
This document exists to prevent that drift.
</p>

<hr/>

<h2 id="scope">3. Scope</h2>

<p>
This document defines:
</p>

<ul>
  <li>the architectural taxonomy of front-panel objects,</li>
  <li>the separation between class, instance, value, realization, and runtime state,</li>
  <li>the common minimal object contract expected from standard widgets,</li>
  <li>the minimum object-surface requirement for intrinsic standard widgets,</li>
  <li>the extension posture for custom widgets,</li>
  <li>the v1 standardization posture for the initial baseline.</li>
</ul>

<p>
This document does not define:
</p>

<ul>
  <li>the full source syntax of widget instances,</li>
  <li>the full generic widget class contract schema,</li>
  <li>the full <code>.wfrog</code> publication grammar,</li>
  <li>one mandatory realization family,</li>
  <li>one mandatory host toolkit,</li>
  <li>one mandatory runtime UI object model.</li>
</ul>

<hr/>

<h2 id="architectural-position">4. Architectural Position</h2>

<p>
The front-panel object model occupies the following architectural position:
</p>

<pre><code>Expression/
  - source architecture and doctrinal boundaries

Libraries/Widgets/
  - intrinsic standardized widget classes

Libraries/UI.md
  - intrinsic executable widget interaction primitives

Libraries/Realizations/
  - official realization families

.wfrog publication
  - machine-readable widget and realization publication

Implementations/
  - runtime families and host embodiment
</code></pre>

<p>
This document is architectural and cross-cutting.
It is therefore upstream from realization families and downstream from the general widget architecture.
</p>

<hr/>

<h2 id="core-design-rule">5. Core Design Rule</h2>

<p>
The core design rule of the FROG front-panel object model is:
</p>

<blockquote>
  A standard widget is not merely a value and not merely a visual control.
  A standard widget is a published object class with a minimal but real public surface.
</blockquote>

<p>
Accordingly, each intrinsic standard widget must expose, at minimum:
</p>

<ul>
  <li>a stable semantic value posture when applicable,</li>
  <li>a small but real property surface,</li>
  <li>a small but real method surface,</li>
  <li>a small but real event surface,</li>
  <li>a stable public part model for realization targeting.</li>
</ul>

<p>
This rule is deliberately close in spirit to mature graphical systems such as LabVIEW:
the baseline must remain small, but baseline widgets must still be object-like and inspectable rather than passive values decorated by runtime-private UI.
</p>

<hr/>

<h2 id="front-panel-object-taxonomy">6. Front-Panel Object Taxonomy</h2>

<p>
FROG front-panel objects are organized into the following conceptual families.
</p>

<h3>6.1 Value widgets</h3>

<p>
Value widgets expose a primary semantic value as their main public surface.
</p>

<ul>
  <li>numeric</li>
  <li>boolean</li>
  <li>string</li>
  <li>path</li>
  <li>enum</li>
</ul>

<h3>6.2 Structured widgets</h3>

<p>
Structured widgets expose collections, aggregates, or structured composition surfaces.
</p>

<ul>
  <li>array</li>
  <li>cluster</li>
  <li>table</li>
  <li>tree</li>
  <li>tab</li>
</ul>

<h3>6.3 Visualization widgets</h3>

<p>
Visualization widgets expose rendered or projected data surfaces rather than simple scalar display.
</p>

<ul>
  <li>chart</li>
  <li>graph</li>
  <li>picture</li>
</ul>

<h3>6.4 Command and selection widgets</h3>

<p>
Command and selection widgets are oriented toward activation, state change, or user selection.
</p>

<ul>
  <li>button</li>
  <li>switch</li>
  <li>listbox</li>
</ul>

<h3>6.5 Decorative and support widgets</h3>

<p>
Decorative and support widgets provide support, annotation, grouping, or guidance rather than a primary business value.
</p>

<ul>
  <li>frame</li>
  <li>label</li>
  <li>line</li>
</ul>

<p>
This taxonomy is architectural.
It does not mean all of these classes must enter the intrinsic baseline at once.
</p>

<hr/>

<h2 id="baseline-standardization-strategy">7. Baseline Standardization Strategy</h2>

<p>
The intrinsic standard baseline of FROG must remain intentionally small.
</p>

<p>
The recommended standardization strategy is:
</p>

<h3>7.1 Intrinsic baseline v1</h3>

<ul>
  <li><code>frog.widgets.numeric_control</code></li>
  <li><code>frog.widgets.numeric_indicator</code></li>
  <li><code>frog.widgets.boolean_control</code></li>
  <li><code>frog.widgets.boolean_indicator</code></li>
  <li><code>frog.widgets.string_control</code></li>
  <li><code>frog.widgets.string_indicator</code></li>
  <li><code>frog.widgets.button</code></li>
  <li><code>frog.widgets.waveform_chart</code></li>
  <li>support widgets such as label and frame, if standardized as intrinsic baseline objects</li>
</ul>

<h3>7.2 Near-core standardized candidates</h3>

<ul>
  <li>enum</li>
  <li>path</li>
  <li>cluster</li>
  <li>array</li>
</ul>

<h3>7.3 Deferred standardization</h3>

<ul>
  <li>listbox</li>
  <li>switch</li>
  <li>table</li>
  <li>tree</li>
  <li>tab</li>
  <li>waveform graph</li>
  <li>XY graph</li>
  <li>intensity graph</li>
  <li>picture-rich or canvas-like object families</li>
</ul>

<p>
This strategy preserves a credible minimum without overloading the intrinsic baseline.
</p>

<hr/>

<h2 id="the-six-required-separations">8. The Six Required Separations</h2>

<p>
The front-panel object model requires six explicit separations.
</p>

<h3>8.1 Class versus instance</h3>

<ul>
  <li><strong>class</strong> defines what a widget is,</li>
  <li><strong>instance</strong> defines one placed occurrence in a concrete front panel.</li>
</ul>

<h3>8.2 Instance versus semantic value</h3>

<ul>
  <li><strong>instance</strong> is the placed object,</li>
  <li><strong>semantic value</strong> is the business value it carries or displays.</li>
</ul>

<h3>8.3 Semantic value versus visual realization</h3>

<ul>
  <li><strong>semantic value</strong> belongs to class law and diagram meaning,</li>
  <li><strong>visual realization</strong> belongs to downstream embodiment.</li>
</ul>

<h3>8.4 Public object surface versus runtime-private state</h3>

<ul>
  <li><strong>public object surface</strong> is inspectable and standardizable,</li>
  <li><strong>runtime-private state</strong> is implementation detail.</li>
</ul>

<h3>8.5 Behavior surface versus host toolkit behavior</h3>

<ul>
  <li><strong>public behavior surface</strong> belongs to the class and bounded behavior doctrine,</li>
  <li><strong>toolkit behavior</strong> belongs to runtime embodiment and compatibility strategy.</li>
</ul>

<h3>8.6 Interaction primitive versus realization surface</h3>

<ul>
  <li><strong><code>frog.ui.*</code></strong> accesses legal public class surfaces,</li>
  <li><strong>anchors, text regions, resource layers, and visual states</strong> belong to realization publication unless explicitly promoted to class law.</li>
</ul>

<hr/>

<h2 id="front-panel-object-class-model">9. Front-Panel Object Class Model</h2>

<p>
A front-panel widget class defines a portable object surface.
</p>

<p>
At minimum, a class should define:
</p>

<ul>
  <li><code>class_id</code>,</li>
  <li>category,</li>
  <li>role posture such as control, indicator, or support,</li>
  <li>primary value posture where applicable,</li>
  <li>legal properties,</li>
  <li>legal methods,</li>
  <li>legal events,</li>
  <li>stable public parts,</li>
  <li>minimal intrinsic behavior expectations,</li>
  <li>minimal realization expectations.</li>
</ul>

<p>
A widget class therefore defines what a conforming runtime must preserve as public meaning.
It does not define one single realization or one single toolkit embodiment.
</p>

<hr/>

<h2 id="front-panel-object-instance-model">10. Front-Panel Object Instance Model</h2>

<p>
A front-panel widget instance is a canonical source-side placed occurrence of a widget class.
</p>

<p>
An instance is responsible for:
</p>

<ul>
  <li>instance identity,</li>
  <li>placement in the front panel,</li>
  <li>role participation in the source program,</li>
  <li>instance-local property initialization where legal,</li>
  <li>source-visible references and bindings.</li>
</ul>

<p>
An instance does not own the full class law.
It instantiates a class that already exists.
</p>

<hr/>

<h2 id="common-minimal-contract-for-all-standard-widgets">11. Common Minimal Contract for All Standard Widgets</h2>

<p>
Every intrinsic standard widget should follow a common minimal contract.
</p>

<h3>11.1 Identity</h3>

<ul>
  <li><code>class_id</code></li>
  <li>category</li>
  <li>role posture</li>
</ul>

<h3>11.2 Value posture</h3>

<ul>
  <li>whether a primary value exists,</li>
  <li>the primary value type,</li>
  <li>whether the value is readable, writable, or indicator-only,</li>
  <li>whether natural <code>widget_value</code> participation exists.</li>
</ul>

<h3>11.3 Public object surface</h3>

<ul>
  <li>properties</li>
  <li>methods</li>
  <li>events</li>
  <li>parts</li>
</ul>

<h3>11.4 Interaction posture</h3>

<ul>
  <li>visibility</li>
  <li>enabled posture when applicable</li>
  <li>focus posture when applicable</li>
  <li>tooltip or help posture when applicable</li>
</ul>

<h3>11.5 Layout posture</h3>

<ul>
  <li>root layout participation</li>
  <li>minimum stable part structure</li>
  <li>container participation when applicable</li>
</ul>

<h3>11.6 Realization posture</h3>

<ul>
  <li>which parts are expected to be realized,</li>
  <li>which dynamic surfaces require host rendering,</li>
  <li>which realization notes are mandatory for portability.</li>
</ul>

<h3>11.7 Runtime posture</h3>

<ul>
  <li>minimum runtime expectations,</li>
  <li>fallback posture when realization specialization is unavailable,</li>
  <li>interaction semantics that must be preserved.</li>
</ul>

<hr/>

<h2 id="minimum-object-surface-requirement">12. Minimum Object-Surface Requirement</h2>

<p>
Each intrinsic standard widget in the baseline must expose a minimum object surface comparable in spirit to mature graphical object systems.
</p>

<p>
The required minimum is:
</p>

<ul>
  <li>at least one meaningful public property beyond pure identity when applicable,</li>
  <li>at least one meaningful public method when the class is interactive or stateful,</li>
  <li>at least one meaningful public event when the class produces observable interaction or rendering change,</li>
  <li>a stable public part model for realization targeting.</li>
</ul>

<p>
This requirement exists to prevent the baseline from collapsing into passive value carriers with no credible property-node or method-node posture.
</p>

<p>
At the same time, the object surface must remain minimal.
The goal is not to copy an entire mature proprietary palette into the intrinsic core.
The goal is to standardize a small but usable object surface for each baseline class.
</p>

<hr/>

<h2 id="baseline-family-intent">13. Baseline Family Intent</h2>

<h3>13.1 Numeric family</h3>

<p>
Numeric widgets are value widgets with a primary numeric value and a minimal object surface including value access, label access, focus, and step-style operations when the control posture exposes them.
</p>

<h3>13.2 Boolean family</h3>

<p>
Boolean widgets are value widgets with a primary boolean value and a minimal object surface including value access, label access, focus, and boolean-state action methods such as toggle or set-true / set-false when legal.
</p>

<h3>13.3 String family</h3>

<p>
String widgets are value widgets with a primary string value and a minimal object surface including value access, label access, focus, and minimal observable text-oriented rendering or editing posture.
</p>

<h3>13.4 Button family</h3>

<p>
Buttons are command-oriented widgets with at least a semantic label surface, a command activation posture, and a small object surface rather than a passive decorative control.
</p>

<h3>13.5 Chart family</h3>

<p>
The intrinsic chart baseline is intentionally limited to one minimal waveform-chart-style indicator with a small but credible property and method surface rather than a full graphing ecosystem.
</p>

<h3>13.6 Support widgets</h3>

<p>
Support widgets such as label and frame, if standardized in the intrinsic baseline, must still expose a coherent minimal object contract even if they do not carry rich business value.
</p>

<hr/>

<h2 id="property-vs-method-vs-event-vs-part-vs-realization-vs-runtime-state">14. Property vs Method vs Event vs Part vs Realization vs Runtime State</h2>

<h3>14.1 Property</h3>

<p>
A property is a legal public surface of a widget class that may be read or written according to class law.
</p>

<p>
Examples:
</p>

<ul>
  <li><code>value</code></li>
  <li><code>label.text</code></li>
  <li><code>history.capacity</code></li>
  <li><code>axes.x.visible</code></li>
</ul>

<h3>14.2 Method</h3>

<p>
A method is a legal public action surface of a widget class.
</p>

<p>
Examples:
</p>

<ul>
  <li><code>focus()</code></li>
  <li><code>increment()</code></li>
  <li><code>toggle()</code></li>
  <li><code>append_sample(sample)</code></li>
</ul>

<h3>14.3 Event</h3>

<p>
An event is a legal public observable occurrence emitted by a widget class.
</p>

<p>
Examples:
</p>

<ul>
  <li><code>value_changed</code></li>
  <li><code>value_rendered</code></li>
  <li><code>focus_gained</code></li>
  <li><code>history_cleared</code></li>
</ul>

<h3>14.4 Part</h3>

<p>
A part is a stable public realization target owned by the widget class law.
</p>

<p>
Examples:
</p>

<ul>
  <li><code>label</code></li>
  <li><code>value_display</code></li>
  <li><code>text_display</code></li>
  <li><code>state_face</code></li>
  <li><code>plot_area</code></li>
  <li><code>x_axis</code></li>
  <li><code>y_axis</code></li>
</ul>

<h3>14.5 Realization surface</h3>

<p>
A realization surface is a downstream embodiment structure such as:
</p>

<ul>
  <li>anchor,</li>
  <li>text region,</li>
  <li>resource layer,</li>
  <li>state-specific asset,</li>
  <li>host-native placement surface.</li>
</ul>

<p>
A realization surface is not automatically a public class property or public class part unless explicitly published as such.
</p>

<h3>14.6 Runtime-private state</h3>

<p>
Runtime-private state includes toolkit handles, buffers, caret state, cached layout structures, dispatch tables, and other implementation machinery.
</p>

<p>
Runtime-private state is not part of class law and not part of the standard public widget surface unless explicitly standardized elsewhere.
</p>

<hr/>

<h2 id="relation-with-frogui-primitives">15. Relation with <code>frog.ui.*</code> Primitives</h2>

<p>
The object model defined here is consumed by <code>frog.ui.*</code> primitives.
</p>

<p>
The rule is:
</p>

<pre><code>class law
    - what public properties, methods, events, and parts exist

frog.ui.*
    - how executable diagrams interact with those public surfaces
</code></pre>

<p>
This means:
</p>

<ul>
  <li>the class owns the legality and meaning of <code>value</code>, <code>label.text</code>, and similar members,</li>
  <li><code>frog.ui.property_read</code> and <code>frog.ui.property_write</code> access those legal members,</li>
  <li><code>frog.ui.method_invoke</code> invokes legal public methods,</li>
  <li><code>frog.ui.event_observe</code> observes legal public events.</li>
</ul>

<p>
Realization-side anchors, text regions, skin layers, and host embodiment helpers must not be addressed through <code>frog.ui.*</code> as if they were public class members unless the class law explicitly promotes them into the public object model.
</p>

<hr/>

<h2 id="relation-with-realization-families">16. Relation with Realization Families</h2>

<p>
The realization layer is downstream from the front-panel object model.
</p>

<p>
Accordingly:
</p>

<ul>
  <li>the class defines semantic meaning and public object surfaces,</li>
  <li>the realization defines visual embodiment and placement,</li>
  <li>assets provide concrete embodiment resources,</li>
  <li>the runtime renders or approximates the realization.</li>
</ul>

<p>
This is especially important for dynamic public surfaces:
</p>

<ul>
  <li><code>label.text</code> remains class-owned semantic text,</li>
  <li><code>value_display</code> or <code>text_display</code> remain class-owned dynamic public surfaces,</li>
  <li>their anchors, text regions, layer maps, and asset-backed decorative containers remain realization-side publication.</li>
</ul>

<p>
The asset layer therefore does not become the semantic owner of dynamic public text or value-bearing meaning.
</p>

<hr/>

<h2 id="custom-widget-extension-model">17. Custom Widget Extension Model</h2>

<p>
The front-panel object model must allow custom widget publication later.
</p>

<h3>17.1 Extension categories</h3>

<ul>
  <li>intrinsic standard widget</li>
  <li>later standardized widget</li>
  <li>developer-defined custom widget</li>
</ul>

<h3>17.2 Minimum publication requirements for custom widgets</h3>

<p>
A custom widget should publish at minimum:
</p>

<ul>
  <li><code>class_id</code>,</li>
  <li>role posture,</li>
  <li>primary value posture or explicit absence of one,</li>
  <li>public properties,</li>
  <li>public methods,</li>
  <li>public events,</li>
  <li>public parts,</li>
  <li>minimal realization expectations,</li>
  <li>minimal runtime expectations.</li>
</ul>

<h3>17.3 What custom widgets may extend</h3>

<ul>
  <li>properties</li>
  <li>methods</li>
  <li>events</li>
  <li>parts</li>
  <li>bounded behavior</li>
  <li>realization publication</li>
</ul>

<h3>17.4 What custom widgets must not break</h3>

<ul>
  <li>the distinction between property, method, event, part, and realization surface,</li>
  <li>the separation between class law and runtime-private embodiment,</li>
  <li>the requirement that legal public surfaces remain inspectable and publishable,</li>
  <li>the ability of runtimes to determine the object contract without guessing from assets.</li>
</ul>

<p>
The extension posture must therefore be open, but not anarchic.
</p>

<hr/>

<h2 id="runtime-contract-posture">18. Runtime Contract Posture</h2>

<p>
A runtime implementing the front-panel object model must preserve the published public object contract for every widget surface it claims to support.
</p>

<p>
A runtime may differ in:
</p>

<ul>
  <li>host toolkit,</li>
  <li>rendering engine,</li>
  <li>layout internals,</li>
  <li>asset consumption strategy,</li>
  <li>event bridge implementation.</li>
</ul>

<p>
A runtime must not differ in:
</p>

<ul>
  <li>class identity,</li>
  <li>public meaning of properties, methods, events, and parts,</li>
  <li>the distinction between semantic value and visual realization,</li>
  <li>the distinction between public object contract and runtime-private embodiment state.</li>
</ul>

<hr/>

<h2 id="v1-design-decisions">19. V1 Design Decisions</h2>

<p>
The following design decisions should be treated as immediate baseline decisions for v1:
</p>

<ul>
  <li>the intrinsic widget baseline remains small,</li>
  <li>every intrinsic standard widget must have a real minimal object surface,</li>
  <li>numeric, boolean, string, button, and minimal waveform chart form the core value-and-command baseline,</li>
  <li>support widgets such as label and frame may enter the baseline only if they also follow a disciplined minimal object contract,</li>
  <li>realization-specific placement and asset structures remain downstream from class law,</li>
  <li>custom widgets are allowed later through explicit publication rather than runtime-private invention.</li>
</ul>

<hr/>

<h2 id="deferred-surfaces">20. Deferred Surfaces</h2>

<p>
The following should generally be deferred rather than overloaded into the intrinsic v1 core:
</p>

<ul>
  <li>full graph families beyond the minimal waveform chart baseline,</li>
  <li>rich legend systems,</li>
  <li>cursor systems,</li>
  <li>annotation systems,</li>
  <li>heavy table and tree interaction models,</li>
  <li>complex tab ecosystems,</li>
  <li>full picture and canvas ecosystems,</li>
  <li>toolkit-specific style systems.</li>
</ul>

<p>
Deferral is not rejection.
It is an architectural discipline rule intended to keep v1 coherent.
</p>

<hr/>

<h2 id="validation-posture">21. Validation Posture</h2>

<p>
Validators should be able to diagnose at least:
</p>

<ul>
  <li>widgets that claim to be standard but do not publish a coherent public object surface,</li>
  <li>use of public members, methods, or events not published by the active class law,</li>
  <li>confusion between public class surfaces and realization-only surfaces,</li>
  <li>attempts to treat runtime-private embodiment state as public widget contract,</li>
  <li>custom widgets that fail to publish the minimum interoperable object contract.</li>
</ul>

<p>
Validation must preserve the recoverable distinction between:
</p>

<ul>
  <li>semantic value,</li>
  <li>public class member,</li>
  <li>public part,</li>
  <li>realization surface,</li>
  <li>runtime-private embodiment state.</li>
</ul>

<hr/>

<h2 id="summary">22. Summary</h2>

<p>
The FROG front-panel object model defines the architectural rule that a widget is a published object class rather than a passive value plus skin.
</p>

<p>
It organizes the widget corridor through explicit separations between:
</p>

<ul>
  <li>class,</li>
  <li>instance,</li>
  <li>semantic value,</li>
  <li>public object surface,</li>
  <li>realization,</li>
  <li>runtime-private embodiment.</li>
</ul>

<p>
Its key v1 rule is:
</p>

<ul>
  <li>keep the intrinsic widget core small,</li>
  <li>but require each intrinsic standard widget to expose a minimum real object surface with properties, methods, events, and parts.</li>
</ul>

<p>
This keeps the baseline coherent, portable, extensible, and compatible with future custom widget publication without turning one runtime implementation into the true owner of front-panel object meaning.
</p>

## Studio reconciliation — 14 October 2026

The [Studio authoring profile](Studio%20authoring%20document%20profile%20v1.md) records complete nested widget schemas, stable Cluster field identities and true Array data shape separately from visible previews. Symbolic reference-node adaptation is qualified separately from new reference-value widgets currently being developed. Object shape, source acceptance and runtime provider support must remain distinct.
