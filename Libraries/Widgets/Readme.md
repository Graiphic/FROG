<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Standard Widgets</h1>

<p align="center">
  <strong>Normative baseline of intrinsic standardized widget classes for portable front-panel ecosystems</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#why-this-directory-exists">2. Why this Directory Exists</a></li>
  <li><a href="#scope">3. Scope</a></li>
  <li><a href="#architectural-position">4. Architectural Position</a></li>
  <li><a href="#ownership-boundary">5. Ownership Boundary</a></li>
  <li><a href="#what-a-standard-widget-class-is">6. What a Standard Widget Class Is</a></li>
  <li><a href="#minimum-object-surface-rule">7. Minimum Object-Surface Rule</a></li>
  <li><a href="#label-and-caption-convention">8. Label and Caption Convention</a></li>
  <li><a href="#baseline-widget-families">9. Baseline Widget Families</a></li>
  <li><a href="#baseline-standardization-phases">10. Baseline Standardization Phases</a></li>
  <li><a href="#class-versus-realization-rule">11. Class versus Realization Rule</a></li>
  <li><a href="#shared-baseline-conventions">12. Shared Baseline Conventions</a></li>
  <li><a href="#primitive-vs-composite-posture">13. Primitive vs Composite Posture</a></li>
  <li><a href="#relation-with-frog-source">14. Relation with <code>.frog</code> Source</a></li>
  <li><a href="#relation-with-wfrog-publication">15. Relation with <code>.wfrog</code> Publication</a></li>
  <li><a href="#relation-with-frogui-primitives">16. Relation with <code>frog.ui.*</code> Primitives</a></li>
  <li><a href="#relation-with-realization-families">17. Relation with Realization Families</a></li>
  <li><a href="#portability-across-runtimes">18. Portability Across Runtimes</a></li>
  <li><a href="#conformance-posture">19. Conformance Posture</a></li>
  <li><a href="#status">20. Status</a></li>
  <li><a href="#summary">21. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This directory defines the intrinsic standardized widget baseline of FROG.
</p>

<p>
Its role is to publish a small, portable, inspectable set of standard widget classes that can be reused across FROG programs, runtimes, IDEs, and realization families without forcing one private runtime implementation to become the definition of the widget system.
</p>

<p>
This baseline is intentionally modest but concrete.
It defines the first standard widget families that are sufficient to support a credible front-panel ecosystem and serious executable vertical slices.
</p>

<p>
The standard widget baseline does not replace the general widget architecture defined elsewhere.
Instead, it instantiates that architecture through a first published set of reusable classes with explicit public object surfaces.
</p>

<p>
The baseline also assumes that standardized widgets may be styled, skinned, and realized through compatible realization families without turning those realization choices into hidden class splits.
The intrinsic class layer remains semantic and portable.
Realization remains downstream and embodiment-oriented.
</p>

<hr/>

<h2 id="why-this-directory-exists">2. Why this Directory Exists</h2>

<p>
The widget corridor is only credible if FROG can answer two different questions cleanly:
</p>

<ul>
  <li>How does the language represent widgets, widget interaction, widget class law, widget behavior, widget realization, and widget package publication?</li>
  <li>Which standard widget classes actually exist as a reusable baseline for portable programs?</li>
</ul>

<p>
The first question belongs to <code>Expression/</code> and the surrounding architecture documents.
This directory exists to answer the second question.
</p>

<p>
Without a standard widget baseline:
</p>

<ul>
  <li>every runtime would be tempted to invent its own de facto standard widgets,</li>
  <li>examples would become less portable,</li>
  <li>the front-panel corridor would remain too abstract,</li>
  <li>developer-defined widgets would have no stable standard foundation to build on.</li>
</ul>

<p>
This directory therefore publishes the first standardized reusable widget classes on top of the already-defined widget architecture.
</p>

<hr/>

<h2 id="scope">3. Scope</h2>

<p>
This directory defines:
</p>

<ul>
  <li>the intrinsic standardized widget classes of FROG that are published in repository-visible form,</li>
  <li>their class identity and role posture,</li>
  <li>their primary value posture where applicable,</li>
  <li>their standard properties, methods, events, and parts,</li>
  <li>their minimal intrinsic behavior expectations,</li>
  <li>their minimal realization expectations,</li>
  <li>their diagram-interaction posture.</li>
</ul>

<p>
This directory does not define canonical <code>.frog</code> serialization, front-panel composition structure, the full generic widget class contract model, the full <code>.wfrog</code> package format, one mandatory host toolkit, or one mandatory runtime architecture.
</p>

<hr/>

<h2 id="architectural-position">4. Architectural Position</h2>

<pre><code>Expression/                 - widget source model and widget architecture
Libraries/Widgets/          - intrinsic standardized widget classes
Libraries/UI.md             - intrinsic executable UI interaction primitives
Libraries/Realizations/     - official realization families for standardized classes
.wfrog publication          - machine-readable publication of widget and realization artifacts
Implementations/            - runtime families and host realization
</code></pre>

<p>
This separation is intentional:
</p>

<pre><code>frog.widgets.*   - class identities and public class surfaces
frog.ui.*        - executable interaction primitives
realization      - official embodiment posture for published classes
</code></pre>

<hr/>

<h2 id="ownership-boundary">5. Ownership Boundary</h2>

<ul>
  <li><code>Expression/Widget.md</code> owns widget instances in canonical source.</li>
  <li><code>Expression/Widget class contract.md</code> owns the general class-law model.</li>
  <li><code>Expression/Widget behavior.md</code> owns the bounded behavior doctrine.</li>
  <li><code>Expression/Widget realization.md</code> owns the generic realization doctrine.</li>
  <li><code>Expression/Widget package (.wfrog).md</code> owns widget-oriented package publication format.</li>
  <li><code>Libraries/UI.md</code> owns executable widget interaction primitives.</li>
  <li><code>Libraries/Widgets/</code> owns the intrinsic standardized baseline widget classes themselves.</li>
  <li><code>Libraries/Realizations/</code> owns official realization-family posture for those classes.</li>
</ul>

<p>
This directory defines which standard classes exist and what their portable public surfaces mean.
It does not redefine the generic widget architecture, does not replace <code>.wfrog</code>, does not make one runtime the owner of class law, and does not delegate public class meaning to realization assets.
</p>

<hr/>

<h2 id="what-a-standard-widget-class-is">6. What a Standard Widget Class Is</h2>

<p>
A standard widget class in this directory is a reusable, portable, intrinsic class published as part of the FROG baseline.
</p>

<p>
A standard class defines, at minimum:
</p>

<ul>
  <li>a stable <code>class_id</code>,</li>
  <li>a role posture,</li>
  <li>a primary value posture where applicable,</li>
  <li>a stable public property inventory,</li>
  <li>a stable public method inventory,</li>
  <li>a stable public event inventory,</li>
  <li>a stable public part model,</li>
  <li>minimal behavior expectations,</li>
  <li>minimal realization expectations.</li>
</ul>

<p>
A standard class is more than an abstract name and more than a visual control template.
It is a published portable object surface that runtimes may implement and programs may rely on.
</p>

<hr/>

<h2 id="minimum-object-surface-rule">7. Minimum Object-Surface Rule</h2>

<p>
The intrinsic widget baseline follows a strict minimum object-surface rule.
A standard widget in the intrinsic baseline must not collapse into a passive value plus a runtime-private visual shell.
</p>

<p>
At minimum, this means:
</p>

<ul>
  <li>a meaningful public property surface,</li>
  <li>a meaningful public method surface when the class is interactive, stateful, or structurally significant,</li>
  <li>a meaningful public event surface when the class produces observable interaction or visible update,</li>
  <li>a stable public part model for realization targeting.</li>
</ul>

<p>
This rule is intentionally close in spirit to mature graphical object systems such as LabVIEW:
the intrinsic baseline remains small, but standard widgets still have a real minimum of property-node, method-node, and event-observation usefulness.
</p>

<hr/>

<h2 id="label-and-caption-convention">8. Label and Caption Convention</h2>

<p>
The standard widget baseline distinguishes <code>label</code> from <code>caption</code>.
</p>

<pre><code>label.*
    - structural / logical widget name surface
    - useful for diagrams, references, tooling, accessibility, debugging, and program review
    - may be displayed by an IDE or host, but is not primarily the presentation caption

caption.*
    - user-facing presentation text surface
    - intended for front-panel display
    - may be shown, hidden, styled, and positioned by realization
</code></pre>

<p>
This distinction is inspired by mature graphical environments while being made explicit in FROG.
It prevents one text field from serving every purpose at once.
</p>

<p>
The recommended portable surfaces are:
</p>

<ul>
  <li><code>label.text</code> — logical widget name or structural label.</li>
  <li><code>label.visible</code> — whether the logical label is host-visible when the active authoring or realization corridor supports it.</li>
  <li><code>caption.text</code> — user-facing display caption.</li>
  <li><code>caption.visible</code> — whether the caption is visible on the front panel.</li>
  <li><code>caption.placement</code> — preferred caption placement when supported.</li>
  <li><code>caption.style.*</code> — presentation style for the visible caption.</li>
</ul>

<p>
A class may choose not to expose a caption when it is not meaningful.
However, when both a structural label and a user-facing display text are needed, the canonical distinction is <code>label.*</code> versus <code>caption.*</code>.
</p>

<p>
Realization assets may place and style a caption, but they do not become the semantic owner of <code>caption.text</code>.
Likewise, a runtime may display or hide labels and captions, but it must not silently collapse the logical label and the user-facing caption into one private field when both are published.
</p>

<hr/>

<h2 id="baseline-widget-families">9. Baseline Widget Families</h2>

<p>
The intrinsic standardized widget space is organized around a small core plus additional standardized families outside that core.
</p>

<ul>
  <li><code>Numeric.md</code> — numeric control and numeric indicator</li>
  <li><code>Boolean.md</code> — boolean control and boolean indicator</li>
  <li><code>String.md</code> — string control and string indicator</li>
  <li><code>Button.md</code> — push button</li>
  <li><code>Chart.md</code> — minimal waveform chart baseline</li>
</ul>

<p>
These families intentionally form a small but credible front-panel core:
typed editable values, typed displayed values, basic command interaction, and a first structured visual history widget.
</p>

<hr/>

<h2 id="baseline-standardization-phases">10. Baseline Standardization Phases</h2>

<p>
The intrinsic widget baseline should remain intentionally phased.
This helps preserve a credible core while keeping the path open for later expansion.
</p>

<h3>10.1 Intrinsic baseline core</h3>

<ul>
  <li><code>frog.widgets.numeric_control</code></li>
  <li><code>frog.widgets.numeric_indicator</code></li>
  <li><code>frog.widgets.boolean_control</code></li>
  <li><code>frog.widgets.boolean_indicator</code></li>
  <li><code>frog.widgets.string_control</code></li>
  <li><code>frog.widgets.string_indicator</code></li>
  <li><code>frog.widgets.button</code></li>
  <li><code>frog.widgets.waveform_chart</code></li>
</ul>

<h3>10.2 Standardized support widgets outside the intrinsic baseline core</h3>

<ul>
  <li><code>frog.widgets.label</code></li>
  <li><code>frog.widgets.frame</code></li>
</ul>

<h3>10.3 Near-core standardized candidates</h3>

<ul>
  <li><code>frog.widgets.enum_control</code> and <code>frog.widgets.enum_indicator</code></li>
  <li><code>frog.widgets.path_control</code> and <code>frog.widgets.path_indicator</code></li>
  <li><code>frog.widgets.cluster</code></li>
  <li><code>frog.widgets.array</code></li>
</ul>

<h3>10.4 Deferred standardization</h3>

<ul>
  <li>listbox</li>
  <li>table</li>
  <li>tree</li>
  <li>tab</li>
  <li>picture</li>
  <li>waveform graph</li>
  <li>XY graph</li>
  <li>intensity graph</li>
  <li>canvas-like widget families</li>
</ul>

<hr/>

<h2 id="class-versus-realization-rule">11. Class versus Realization Rule</h2>

<p>
A distinct visible embodiment does not, by itself, justify a distinct standardized class.
</p>

<p>
The preferred architecture is:
</p>

<ul>
  <li>small semantic class core,</li>
  <li>multiple compatible realization families or realization variants,</li>
  <li>new standardized classes only when the public contract actually diverges.</li>
</ul>

<p>
A different skin, a different compatible SVG set, a different host-native chrome, or a different compatible realization variant does not automatically create a new class.
A widget remains the same standardized class as long as its public contract remains unchanged.
</p>

<hr/>

<h2 id="shared-baseline-conventions">12. Shared Baseline Conventions</h2>

<ul>
  <li>standard class identifiers use the <code>frog.widgets.*</code> namespace,</li>
  <li>value-carrying classes expose a primary value mirrored as property <code>value</code>,</li>
  <li>structural widget names use <code>label.text</code> when a logical label is part of public class meaning,</li>
  <li>front-panel presentation captions use <code>caption.text</code> when a visible caption is part of public class meaning,</li>
  <li>visibility uses property <code>interaction.visible</code>,</li>
  <li>interactive classes use property <code>interaction.enabled</code> where applicable,</li>
  <li>the root part is named <code>root</code>,</li>
  <li>the outer framing part, when present, is named <code>frame</code>,</li>
  <li>portable styling surfaces, when exposed, use the <code>style.*</code> namespace,</li>
  <li>portable realization-selection surfaces, when exposed, use the <code>realization.*</code> namespace,</li>
  <li>controls typically emit <code>value_changed</code> for primary value mutation,</li>
  <li>indicators typically emit <code>value_rendered</code> for visible refresh-oriented notification.</li>
</ul>

<hr/>

<h2 id="primitive-vs-composite-posture">13. Primitive vs Composite Posture</h2>

<p>
The classes defined in this directory are the initial standardized primitive baseline.
They are intended to serve as a portable reusable widget core, a foundation for serious examples, and a foundation for future standardized or developer-defined composite widgets.
</p>

<hr/>

<h2 id="relation-with-frog-source">14. Relation with <code>.frog</code> Source</h2>

<p>
Canonical <code>.frog</code> source instantiates widgets through widget instances in <code>front_panel</code>.
Those widget instances may reference the standard classes defined in this directory.
</p>

<pre><code>frog.widgets.numeric_control
frog.widgets.boolean_indicator
frog.widgets.string_control
frog.widgets.button
frog.widgets.waveform_chart
</code></pre>

<p>
Canonical source owns the instance.
This directory owns the standardized class being instantiated.
</p>

<hr/>

<h2 id="relation-with-wfrog-publication">15. Relation with <code>.wfrog</code> Publication</h2>

<p>
The classes defined in this directory are intrinsic standardized classes.
They may be published, mirrored, or accompanied by official widget-oriented package artifacts through the <code>.wfrog</code> corridor.
</p>

<pre><code>published class law
    !=
widget-oriented package publication
    !=
realization-family publication
    !=
visual asset
    !=
runtime-private implementation
</code></pre>

<hr/>

<h2 id="relation-with-frogui-primitives">16. Relation with <code>frog.ui.*</code> Primitives</h2>

<p>
The executable object-style interaction surface for widgets is defined by <code>frog.ui.*</code> primitives.
Those primitives operate on widget classes defined by this directory and on compatible developer-defined classes published elsewhere.
</p>

<pre><code>class law
    - what exists

frog.ui.*
    - how execution accesses what exists
</code></pre>

<p>
If a class exposes members such as <code>label.text</code>, <code>caption.text</code>, <code>value</code>, portable <code>style.*</code> surfaces, portable <code>realization.*</code> members, or methods such as <code>clear()</code> or <code>append_text(text)</code>, this directory owns their legality and meaning, while <code>frog.ui.*</code> owns the executable primitive vocabulary used to access them.
</p>

<hr/>

<h2 id="relation-with-realization-families">17. Relation with Realization Families</h2>

<p>
The classes defined here are not identical to one skin and are not identical to one realization resource family.
</p>

<p>
A standard widget class may be accompanied by one or more official realization families, machine-readable realization packages, state-sensitive resource maps, structural part bindings, anchor or text-region publication for dynamic public parts, and compatible realization variants.
</p>

<p>
Those realization layers remain subordinate to the class law.
</p>

<hr/>

<h2 id="portability-across-runtimes">18. Portability Across Runtimes</h2>

<p>
The standard classes defined here are intended to be portable across runtime families such as Python, Rust, and C/C++ implementations.
</p>

<p>
Portability does not require identical host toolkit choice, identical rendering internals, or identical pixel output in every case.
Portability does require stable class identity, stable public properties, methods, events, and parts, stable primary value posture, stable behavior meaning, and stable diagram-interaction meaning.
</p>

<hr/>

<h2 id="conformance-posture">19. Conformance Posture</h2>

<p>
A runtime claiming support for one of the standard classes defined here MUST preserve the published portable class surface of that class for the surfaces it claims to implement.
</p>

<p>
A runtime MUST NOT invent undocumented public members while claiming conformance, silently redefine the meaning of standard parts, silently redefine standard events, or use one private realization strategy as if it were the standard class law itself.
</p>

<p>
A runtime also MUST NOT silently transfer semantic ownership of dynamic public text or value-bearing surfaces into realization assets, anchors, text regions, or toolkit-private layers.
Those structures may embody, place, or render the surface.
They do not redefine its class meaning.
</p>

<hr/>

<h2 id="status">20. Status</h2>

<p>
This directory defines the first intrinsic standardized widget baseline of FROG.
</p>

<p>
The immediate closure direction is to stabilize the intrinsic core, keep the class-versus-realization boundary explicit, preserve the minimum object-surface rule, adopt the shared <code>label</code> / <code>caption</code> convention, and add near-core classes only after the intrinsic core is coherent.
</p>

<hr/>

<h2 id="summary">21. Summary</h2>

<p>
This directory publishes the first intrinsic standardized widget classes of FROG.
</p>

<p>
In short:
</p>

<ul>
  <li><code>Expression/</code> defines how widgets exist in the language architecture,</li>
  <li><code>Libraries/Widgets/</code> defines which standard widget classes exist in the intrinsic baseline and what their public surfaces mean,</li>
  <li><code>Libraries/UI.md</code> defines how execution interacts with those classes,</li>
  <li><code>Libraries/Realizations/</code> defines how official realization families embody them,</li>
  <li><code>.wfrog</code> artifacts publish machine-readable widget and realization artifacts without collapsing those ownership layers.</li>
</ul>
