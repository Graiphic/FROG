<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Front Panel</h1>

<p align="center">
  <strong>Normative source model for front-panel composition, widget instances, panel-owned layout intent, and source-visible references to widget-oriented packages</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#why-this-document-exists">2. Why This Document Exists</a></li>
  <li><a href="#scope">3. Scope</a></li>
  <li><a href="#architectural-position">4. Architectural Position</a></li>
  <li><a href="#ownership-boundary">5. Ownership Boundary</a></li>
  <li><a href="#location-in-a-frog-file">6. Location in a <code>.frog</code> File</a></li>
  <li><a href="#front-panel-core-posture">7. Front-Panel Core Posture</a></li>
  <li><a href="#front-panel-structure">8. Front-Panel Structure</a></li>
  <li><a href="#widget-instances">9. Widget Instances</a></li>
  <li><a href="#composition-and-nesting">10. Composition and Nesting</a></li>
  <li><a href="#canvas-and-layout-intent">11. Canvas and Layout Intent</a></li>
  <li><a href="#source-owned-instance-metadata">12. Source-Owned Instance Metadata</a></li>
  <li><a href="#relation-with-widget-class-law">13. Relation with Widget Class Law</a></li>
  <li><a href="#relation-with-wfrog-packages">14. Relation with <code>.wfrog</code> Packages</a></li>
  <li><a href="#widget-package-references">15. Widget Package References</a></li>
  <li><a href="#svg-visual-posture">16. SVG Visual Posture</a></li>
  <li><a href="#relation-with-the-diagram">17. Relation with the Diagram</a></li>
  <li><a href="#runtime-and-host-interpretation-boundary">18. Runtime and Host Interpretation Boundary</a></li>
  <li><a href="#validation-rules">19. Validation Rules</a></li>
  <li><a href="#minimal-posture">20. Minimal Posture</a></li>
  <li><a href="#example">21. Example</a></li>
  <li><a href="#summary">22. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
The <code>front_panel</code> section defines the canonical source-level declaration of the user-facing panel composition of a FROG program.
It is the source home of widget instances, panel composition, containment, layout intent, and source-visible references to widget-oriented package artifacts.
</p>

<p>
The front panel may declare:
</p>

<ul>
  <li>which widget instances participate in the panel,</li>
  <li>which widget class each instance references,</li>
  <li>which panel-owned layout and placement metadata are persisted,</li>
  <li>which source-owned instance metadata is persisted when allowed by the active widget class contract,</li>
  <li>which widget-oriented packages are referenced by the authored source.</li>
</ul>

<p>
The front panel does not define executable graph semantics.
The diagram remains the authoritative executable graph of the FROG program.
</p>

<p>
The front panel also does not define complete widget class law, bounded widget behavior doctrine, runtime-private widget structures, or host rendering internals.
Those concerns belong to distinct specification layers.
</p>

<hr/>

<h2 id="why-this-document-exists">2. Why This Document Exists</h2>

<p>
FROG must preserve a clear architectural distinction between:
</p>

<ul>
  <li>the canonical authored program source,</li>
  <li>the class-level law of widgets,</li>
  <li>the package format used to publish widget-oriented artifacts,</li>
  <li>the bounded behavior doctrine that constrains widget-side behavior publication,</li>
  <li>the runtime interpretation of those artifacts,</li>
  <li>the host-side realization of the live front panel.</li>
</ul>

<p>
Without that distinction, the front panel becomes overloaded.
It starts to absorb class law, behavior law, host realization details, and visual package internals that do not belong to canonical program source.
</p>

<p>
This document therefore defines the front panel as a disciplined source-level composition layer rather than as a catch-all UI definition surface.
Its role is to say which widgets participate in the panel, how they are composed, and which source-visible authored metadata is persisted, while leaving class law, executable interaction, package grammar, and realization internals to their proper homes.
</p>

<hr/>

<h2 id="scope">3. Scope</h2>

<p>
This document standardizes:
</p>

<ul>
  <li>the top-level source shape of the <code>front_panel</code> section,</li>
  <li>source ownership of widget composition and containment,</li>
  <li>source ownership of panel-level layout intent,</li>
  <li>source-visible references to widget-oriented packages,</li>
  <li>the relation between front-panel serialization and widget class law,</li>
  <li>the relation between front-panel serialization and diagram-side executable interaction.</li>
</ul>

<p>
This document does not standardize:
</p>

<ul>
  <li>public interface meaning,</li>
  <li>diagram execution semantics,</li>
  <li>the full class-level legality of widget properties, methods, events, and parts,</li>
  <li>the doctrine for bounded widget behavior publication,</li>
  <li>the standardized primitive widget catalog,</li>
  <li>one mandatory host toolkit,</li>
  <li>pixel-identical rendering across hosts,</li>
  <li>runtime-private UI object structures.</li>
</ul>

<hr/>

<h2 id="architectural-position">4. Architectural Position</h2>

<p>
The front panel is a source-owned panel composition layer.
It sits beside the diagram, not above it, and it does not replace external widget-oriented packages.
</p>

<p>
The required architectural distinction is:
</p>

<pre><code>front_panel source composition
    !=
widget instance law in full
    !=
widget class law
    !=
widget-oriented package content
    !=
bounded widget behavior doctrine
    !=
diagram executable interaction
    !=
runtime-private live widget structures
    !=
host rendering strategy
</code></pre>

<p>
This distinction is mandatory.
If these layers are blurred, source ownership, legality, interpretation, and portability become difficult to reason about and difficult to validate.
</p>

<p>
The front panel owns composition.
It does not own the entire widget system.
</p>

<hr/>

<h2 id="ownership-boundary">5. Ownership Boundary</h2>

<p>
<code>Front panel.md</code> owns:
</p>

<ul>
  <li>source-level panel composition,</li>
  <li>source-level widget containment,</li>
  <li>source-level layout intent and canvas metadata,</li>
  <li>source-owned instance metadata when allowed by widget class law,</li>
  <li>source-visible references to widget-oriented packages.</li>
</ul>

<p>
<code>Front panel.md</code> does not own:
</p>

<ul>
  <li>public interface meaning,</li>
  <li>diagram executable semantics,</li>
  <li>complete widget instance law,</li>
  <li>complete widget class legality,</li>
  <li>object-style interaction primitive law,</li>
  <li>the full machine-readable definition of widget-oriented packages,</li>
  <li>bounded behavior publication law,</li>
  <li>runtime-private live object structure,</li>
  <li>host-private rendering internals,</li>
  <li>the standard primitive widget catalog owned by <code>Libraries/</code>.</li>
</ul>

<p>
Accordingly, the front panel may serialize widget instances and panel-owned authored intent, but it must not become a second widget-class document, a second package-format document, or a second executable-interaction document.
</p>

<hr/>

<h2 id="location-in-a-frog-file">6. Location in a <code>.frog</code> File</h2>

<p>
When present, the front panel appears as an optional top-level section of a <code>.frog</code> file.
</p>

<pre><code>{
  "spec_version": "0.1",
  "metadata": {},
  "interface": {},
  "diagram": {},
  "front_panel": {}
}</code></pre>

<p>
The <code>front_panel</code> section is optional.
When absent, the FROG program is interpreted as having no serialized front-panel composition.
This does not affect the validity of the FROG as an executable program.
</p>

<p>
The presence of <code>front_panel</code> does not make the front panel the owner of executable meaning.
The diagram remains authoritative for execution.
</p>

<hr/>

<h2 id="front-panel-core-posture">7. Front-Panel Core Posture</h2>

<p>
The front panel is the canonical source-level declaration of:
</p>

<ul>
  <li>which widget instances exist in the panel,</li>
  <li>how those instances are composed and nested,</li>
  <li>which panel-owned layout intent is persisted in source,</li>
  <li>which external widget-oriented packages are referenced by source.</li>
</ul>

<p>
The front panel is not the place where full widget class definitions are authored.
It is also not the place where bounded widget behavior is fully defined or where host realization internals are fully authored.
Those concerns belong to widget class contracts, widget behavior doctrine, and <code>.wfrog</code> packages.
</p>

<p>
Accordingly, a front panel is source-visible and canonical, but intentionally incomplete with respect to full class law, full behavior law, and full realization law.
</p>

<hr/>

<h2 id="front-panel-structure">8. Front-Panel Structure</h2>

<p>
The canonical source shape of the front panel object is:
</p>

<pre><code>"front_panel": {
  "canvas": {},
  "package_refs": [],
  "widgets": []
}</code></pre>

<p>
Fields:
</p>

<ul>
  <li><code>canvas</code> — optional panel-level layout or coordinate-space metadata,</li>
  <li><code>package_refs</code> — optional list of referenced widget-oriented packages relevant to this panel,</li>
  <li><code>widgets</code> — top-level widget instances in the panel.</li>
</ul>

<p>
All fields are optional.
If present, they MUST follow the declared structure.
Widget instance metadata belongs inside widget instances rather than in a separate hidden semantic layer.
</p>

<p>
This object shape defines front-panel ownership only.
It does not imply that every field inside every widget instance is defined by this document; widget-instance structure is defined normatively in <code>Widget.md</code>.
</p>

<hr/>

<h2 id="widget-instances">9. Widget Instances</h2>

<p>
Widgets are serialized as source-level instances following the widget instance model defined in <code>Widget.md</code>.
</p>

<p>
Typical widget instance fields include:
</p>

<ul>
  <li><code>id</code>,</li>
  <li><code>role</code>,</li>
  <li><code>class_ref</code>,</li>
  <li><code>instance_ref</code> when panel instance identity is distinguished from diagram identity,</li>
  <li><code>value_type</code> when applicable,</li>
  <li><code>layout</code>,</li>
  <li><code>props</code>,</li>
  <li><code>children</code>,</li>
  <li><code>package_refs</code>.</li>
</ul>

<p>
Example:
</p>

<pre><code>{
  "id": "ctrl_count",
  "role": "control",
  "class_ref": "frog.widgets.numeric_control",
  "instance_ref": "ctrl_count",
  "value_type": "u16",
  "layout": {
    "x": 20,
    "y": 24,
    "width": 140,
    "height": 32
  },
  "props": {
    "label": "Count"
  },
  "package_refs": [
    "./ui/accumulator_panel.wfrog"
  ]
}</code></pre>

<p>
The front panel serializes widget instances as source objects.
It does not redefine widget class semantics, member legality, member accessibility, or class-side part contracts.
</p>

<p>
Likewise, the front panel does not define which primitive widget classes are standardized by the language profile.
That catalog belongs to <code>Libraries/</code>.
</p>

<hr/>

<h2 id="composition-and-nesting">10. Composition and Nesting</h2>

<p>
The front panel defines a source-owned widget tree.
Top-level widgets appear in <code>front_panel.widgets</code>.
Container widgets MAY define child widgets through <code>children</code>.
</p>

<p>
Rules:
</p>

<ul>
  <li>widget identifiers MUST be unique across the full recursive widget tree,</li>
  <li>a widget MUST belong to exactly one container position in the serialized tree,</li>
  <li>container ownership MUST be explicit in source,</li>
  <li>child widgets MUST appear only under widgets whose class and role permit containment.</li>
</ul>

<p>
A runtime or host MAY realize the same source composition using a different internal rendering structure, but that does not change source-owned containment.
</p>

<p>
Composition and nesting therefore belong to canonical source, while legality of containment belongs to widget class law.
</p>

<hr/>

<h2 id="canvas-and-layout-intent">11. Canvas and Layout Intent</h2>

<p>
The optional <code>canvas</code> object carries panel-level layout intent or design-time coordinate-space metadata.
</p>

<p>
Example:
</p>

<pre><code>"canvas": {
  "width": 460,
  "height": 170,
  "coordinate_space": "panel_pixels"
}</code></pre>

<p>
Canvas and layout intent:
</p>

<ul>
  <li>MAY guide design-time layout,</li>
  <li>MAY guide preview sizing,</li>
  <li>MAY guide initial host window sizing,</li>
  <li>MUST NOT define execution semantics by itself.</li>
</ul>

<p>
Likewise, widget <code>layout</code> fields define source-owned placement intent, not executable graph behavior.
</p>

<h3>11.1 Canonical placement grid and origin</h3>

<p>
When a front panel serializes widget geometry in <code>.frog</code>, the
canonical coordinate space is <code>panel_pixels</code>: <code>x</code>
increases to the right, <code>y</code> increases downward, and the canvas
origin <code>0,0</code> is the top-left of the front-panel canvas.
</p>

<p>
The optional <code>canvas.grid</code> object declares the design grid used for
placement, inspection, resize snapping, and container-cell calibration.
If omitted, the active standard widget profile uses a hidden ordinary host
grid, a <code>16px</code> pitch, an origin of
<code>{ "x": 0, "y": 0 }</code>, and <code>placement_bounds</code> snapping.
Examples that calibrate widget placement SHOULD set
<code>canvas.grid.visible</code> to <code>true</code>.
</p>

<pre><code>"canvas": {
  "width": 460,
  "height": 170,
  "coordinate_space": "panel_pixels",
  "grid": {
    "visible": true,
    "pitch": 16,
    "origin": { "x": 0, "y": 0 },
    "snap": "placement_bounds"
  }
}</code></pre>

<p>
For a widget instance, <code>layout.x</code> and <code>layout.y</code> are the
coordinates of the widget placement aura inside its parent coordinate space.
By default, that aura is <code>placement_bounds</code> and the serialized
origin is <code>placement_bounds.top_left</code>.
</p>

<pre><code>"layout": {
  "x": 32,
  "y": 48,
  "width": 112,
  "height": 64,
  "origin": "placement_bounds.top_left",
  "bounds_ref": "placement_bounds"
}</code></pre>

<p>
The short form remains legal when defaults apply:
</p>

<pre><code>"layout": {
  "x": 32,
  "y": 48,
  "width": 112,
  "height": 64
}</code></pre>

<p>
For top-level widgets, the parent coordinate space is the front-panel canvas.
For contained widgets, the parent coordinate space is the container-owned
placement surface, such as an Array element cell. IDEs MAY expose bottom-left,
center, or edge handles to users, but canonical <code>.frog</code> source MUST
serialize the resolved placement back to <code>placement_bounds.top_left</code>
unless a future profile explicitly defines another anchor and its conversion
law.
</p>

<p>
A runtime or host MAY adapt authored layout intent to platform constraints, accessibility requirements, or rendering capabilities, but it MUST preserve the authored source meaning of composition and identity.
</p>

<hr/>

<h2 id="source-owned-instance-metadata">12. Source-Owned Instance Metadata</h2>

<p>
A widget instance MAY contain source-owned instance metadata when the active widget class contract allows it.
</p>

<p>
Typical examples include:
</p>

<ul>
  <li><code>label</code>,</li>
  <li>formatting posture,</li>
  <li>default visual state references,</li>
  <li>instance-level options explicitly allowed by the widget class.</li>
</ul>

<p>
For baseline source serialization, such metadata SHOULD normally appear inside <code>props</code>.
</p>

<p>
Source-owned instance metadata:
</p>

<ul>
  <li>MUST remain subordinate to widget class legality,</li>
  <li>MUST remain distinct from diagram executable interaction,</li>
  <li>MUST remain distinct from host-private realization internals,</li>
  <li>MUST NOT create execution semantics merely by appearing in source.</li>
</ul>

<p>
The following distinction MUST remain explicit:
</p>

<pre><code>widget primary value
    !=
source-owned instance metadata
    !=
panel-owned layout intent
    !=
widget-oriented package reference
</code></pre>

<p>
This distinction is especially important for portable runtimes: a runtime may interpret the same authored metadata differently at realization level while remaining bound to the same canonical source meaning.
</p>

<hr/>

<h2 id="relation-with-widget-class-law">13. Relation with Widget Class Law</h2>

<p>
The front panel serializes widget instances.
The widget class contract defines which members and behaviors are legal for the class referenced by each instance.
</p>

<p>
Accordingly:
</p>

<ul>
  <li>the front panel may persist only instance-side members allowed by the relevant class law,</li>
  <li>the front panel does not invent new properties, methods, events, or parts,</li>
  <li>the front panel does not redefine addressing legality or mutability boundaries,</li>
  <li>the front panel does not redefine the runtime-observable object surface of a class.</li>
</ul>

<p>
This means that front-panel serialization is constrained by class law, but it is not itself the source of class law.
</p>

<hr/>

<h2 id="relation-with-wfrog-packages">14. Relation with <code>.wfrog</code> Packages</h2>

<p>
The front panel MAY reference one or more <code>.wfrog</code> packages.
Those packages remain external widget-oriented artifacts rather than absorbed front-panel content.
</p>

<p>
Typical kinds of content that may be published through <code>.wfrog</code> include:
</p>

<ul>
  <li>widget class publication,</li>
  <li>composite widget publication,</li>
  <li>bounded behavior publication,</li>
  <li>realization-resource publication,</li>
  <li>widget-oriented bundles.</li>
</ul>

<p>
The front panel references these packages but does not absorb their ownership.
It points to external content; it does not become the full serialization format of that content.
</p>

<hr/>

<h2 id="widget-package-references">15. Widget Package References</h2>

<p>
A front panel MAY reference widget-oriented packages through:
</p>

<ul>
  <li>panel-level <code>package_refs</code>,</li>
  <li>widget-level <code>package_refs</code>,</li>
  <li>future source-visible forms defined by later specifications.</li>
</ul>

<p>
Such references are source-owned declarations pointing to artifacts that may define class content, composite definitions, visual assets, bounded behavior publication, or host realization metadata.
They are not themselves executable graph operations.
</p>

<p>
Example:
</p>

<pre><code>"package_refs": [
  "./ui/accumulator_panel.wfrog"
]</code></pre>

<p>
Rules:
</p>

<ul>
  <li>package references MUST remain source-visible and inspectable,</li>
  <li>package references MUST NOT by themselves redefine class identity,</li>
  <li>package references MUST NOT create hidden executable behavior,</li>
  <li>package references MUST NOT collapse canonical <code>.frog</code> source into package-owned meaning,</li>
  <li>unsupported package families MAY be ignored or diagnosed by a runtime or host according to profile and capability claims.</li>
</ul>

<hr/>

<h2 id="svg-visual-posture">16. SVG Visual Posture</h2>

<p>
When a referenced <code>.wfrog</code> package or another permitted widget-oriented artifact references SVG content, that SVG is a visual resource rather than executable truth.
</p>

<p>
Accordingly, SVG-backed realization content:
</p>

<ul>
  <li>MAY provide a visual skin, face template, or scalable panel resource,</li>
  <li>MAY define named visual layers or anchors,</li>
  <li>MAY support part placement, clipping, or scalable decoration,</li>
  <li>MUST NOT define hidden methods, events, or dataflow meaning,</li>
  <li>MUST NOT replace widget class law as the normative owner of widget behavior,</li>
  <li>MUST NOT be treated as the normative owner of dynamic widget values or dynamic widget text.</li>
</ul>

<p>
This means that a static SVG asset MUST NOT be treated as the source of the live numeric value of a numeric control or indicator.
Likewise, a static SVG asset MUST NOT be treated as the normative owner of changing label text, runtime state text, or object-style widget behavior.
</p>

<p>
The required distinction remains:
</p>

<pre><code>front_panel composition
    !=
source-owned instance metadata
    !=
widget class law
    !=
widget-oriented package content
    !=
SVG visual resource content
</code></pre>

<p>
A host MAY:
</p>

<ul>
  <li>render SVG-backed realization faithfully,</li>
  <li>substitute a compatible native realization,</li>
  <li>ignore unsupported SVG-specific visual details,</li>
  <li>or ignore an unsupported realization package entirely,</li>
  <li>but it MUST preserve the source-owned meaning of the widget instance.</li>
</ul>

<hr/>

<h2 id="relation-with-the-diagram">17. Relation with the Diagram</h2>

<p>
The front panel does not define executable behavior.
The diagram remains the authoritative executable graph of the FROG program.
</p>

<p>
Widget participation in execution is expressed in the diagram, not in the front panel itself.
Canonical diagram interaction mechanisms include:
</p>

<ul>
  <li><code>widget_value</code> — natural access to a widget primary value,</li>
  <li><code>widget_reference</code> — object-style access anchor,</li>
  <li><code>frog.ui.property_read</code>, <code>frog.ui.property_write</code>, and <code>frog.ui.method_invoke</code> — executable UI interaction primitives when allowed.</li>
</ul>

<p>
A source-owned package reference, a layout field, or a visual metadata field does not become executable interaction merely because it appears in source.
</p>

<p>
Likewise, composition in the front panel does not replace causality in the diagram.
The front panel says what is present and how it is arranged.
The diagram says what executes and how values flow.
</p>

<hr/>

<h2 id="runtime-and-host-interpretation-boundary">18. Runtime and Host Interpretation Boundary</h2>

<p>
A runtime interprets the front panel together with the relevant widget class content and any referenced widget-oriented packages.
A host realizes the live panel using its rendering toolkit and platform capabilities.
</p>

<p>
Accordingly:
</p>

<ul>
  <li>the runtime interprets source-owned widget identity and referenced package content,</li>
  <li>the runtime constructs live widget objects according to published class and package rules,</li>
  <li>the host realizes those live objects visually and interactively,</li>
  <li>the host does not become the owner of canonical program meaning.</li>
</ul>

<p>
One runtime implementation MUST NOT redefine the front-panel meaning of the language.
One host toolkit MUST NOT become the definition of FROG panel semantics.
</p>

<p>
This is precisely why authored front-panel composition must remain source-visible and inspectable rather than silently dissolved into runtime-private structures.
</p>

<hr/>

<h2 id="validation-rules">19. Validation Rules</h2>

<p>
Validators MUST verify at least that:
</p>

<ul>
  <li><code>front_panel</code>, when present, follows the canonical object shape,</li>
  <li><code>widgets</code>, when present, is an array,</li>
  <li>widget identifiers are unique across the full widget tree,</li>
  <li>each widget instance is structurally valid,</li>
  <li>persisted instance metadata is recognized by the active widget class contract when such validation is available,</li>
  <li>referenced widget-oriented packages follow the published source shape,</li>
  <li>source-owned panel content is not misinterpreted as executable graph semantics.</li>
</ul>

<p>
Validators SHOULD additionally diagnose:
</p>

<ul>
  <li>unsupported package families under the active profile,</li>
  <li>structurally valid but unsupported realization references under the active runtime or host claim,</li>
  <li>attempts to blur source-owned metadata with hidden executable behavior,</li>
  <li>attempts to use static SVG content as the normative owner of dynamic widget values or dynamic widget text.</li>
</ul>

<p>
Detailed legality of widget members, parts, role/class compatibility, and object-surface access remains the responsibility of the corresponding widget class contract and interaction rules.
</p>

<hr/>

<h2 id="minimal-posture">20. Minimal Posture</h2>

<p>
A minimal front panel posture MAY contain:
</p>

<ul>
  <li>one top-level numeric control widget,</li>
  <li>one top-level numeric indicator widget,</li>
  <li>one canvas object,</li>
  <li>source-owned instance metadata in <code>props</code>,</li>
  <li>one or more source-visible references to widget-oriented packages where needed.</li>
</ul>

<p>
For that posture:
</p>

<ul>
  <li>the front panel remains a serious but bounded source composition layer,</li>
  <li>widget class law remains externalized and inspectable,</li>
  <li>package content remains externalized and inspectable,</li>
  <li>runtime interpretation remains distinct from host realization,</li>
  <li>the diagram remains the sole owner of executable graph semantics.</li>
</ul>

<p>
This posture is intentionally small.
It proves the architectural boundary without forcing the front panel document to absorb the full future widget ecosystem.
</p>

<hr/>

<h2 id="example">21. Example</h2>

<pre><code>{
  "front_panel": {
    "canvas": {
      "width": 460,
      "height": 170,
      "coordinate_space": "panel_pixels"
    },
    "package_refs": [
      "./ui/accumulator_panel.wfrog"
    ],
    "widgets": [
      {
        "id": "ctrl_count",
        "role": "control",
        "class_ref": "frog.widgets.numeric_control",
        "instance_ref": "ctrl_count",
        "value_type": "u16",
        "layout": {
          "x": 20,
          "y": 24,
          "width": 140,
          "height": 32
        },
        "props": {
          "label": "Count"
        }
      },
      {
        "id": "ind_total",
        "role": "indicator",
        "class_ref": "frog.widgets.numeric_indicator",
        "instance_ref": "ind_total",
        "value_type": "u16",
        "layout": {
          "x": 220,
          "y": 24,
          "width": 140,
          "height": 32
        },
        "props": {
          "label": "Total"
        }
      }
    ]
  }
}</code></pre>

<hr/>

<h2 id="summary">22. Summary</h2>

<p>
This document defines the canonical source model for front-panel composition.
</p>

<p>
It stabilizes:
</p>

<ul>
  <li>source-owned widget composition,</li>
  <li>source-owned containment and layout intent,</li>
  <li>source-owned instance metadata when allowed by class law,</li>
  <li>source-visible references to widget-oriented packages,</li>
  <li>and a strict separation between front-panel serialization and executable graph meaning.</li>
</ul>

<p>
It also makes explicit that visual assets such as SVG remain visual resources rather than hidden semantic truth.
Dynamic widget values, dynamic widget text, object-style widget access, executable UI behavior, and primitive widget catalog definition remain owned by the language, widget class, package, behavior, runtime, host, and library boundaries defined elsewhere in the specification corpus.
</p>

<p>
The result is a front-panel model that is canonical, inspectable, portable, and disciplined:
it owns composition without trying to own the whole widget system.
</p>
