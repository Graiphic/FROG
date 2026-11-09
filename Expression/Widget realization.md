<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Widget Realization</h1>

<p align="center">
  <strong>Normative realization boundary for widget rendering, visual resources, part-to-visual mapping, and host-facing realization support</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#why-this-document-exists">2. Why This Document Exists</a></li>
  <li><a href="#scope">3. Scope</a></li>
  <li><a href="#what-realization-means-in-frog">4. What Realization Means in FROG</a></li>
  <li><a href="#ownership-boundary">5. Ownership Boundary</a></li>
  <li><a href="#realization-layers">6. Realization Layers</a></li>
  <li><a href="#abstract-appearance-layer">7. Abstract Appearance Layer</a></li>
  <li><a href="#layout-and-sizing-layer">8. Layout and Sizing Layer</a></li>
  <li><a href="#visual-resource-layer">9. Visual Resource Layer</a></li>
  <li><a href="#part-to-visual-binding">10. Part-to-Visual Binding</a></li>
  <li><a href="#svg-integration">11. SVG Integration</a></li>
  <li><a href="#runtime-host-bridges">12. Runtime Host Bridges</a></li>
  <li><a href="#interaction-with-behavior">13. Interaction with Behavior</a></li>
  <li><a href="#interaction-with-widget-class-law">14. Interaction with Widget Class Law</a></li>
  <li><a href="#what-realization-must-not-do">15. What Realization Must Not Do</a></li>
  <li><a href="#publication-through-wfrog">16. Publication Through <code>.wfrog</code></a></li>
  <li><a href="#portability-across-runtimes">17. Portability Across Runtimes</a></li>
  <li><a href="#status">18. Status</a></li>
  <li><a href="#license">19. License</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the normative realization boundary for FROG widgets.
</p>

<p>
Realization concerns how a published widget class is rendered and hosted visually.
</p>

<p>
This includes:
</p>

<ul>
  <li>appearance tokens,</li>
  <li>layout and sizing posture,</li>
  <li>visual assets,</li>
  <li>part-to-visual mapping,</li>
  <li>host-side realization metadata.</li>
</ul>

<p>
This document does not define the whole <code>.wfrog</code> package structure. That serialization format is owned by <code>Widget package (.wfrog).md</code>.
</p>

<p>
Instead, this document defines what realization is allowed to do, what it is not allowed to do, and how it remains subordinate to published widget class law.
</p>

<p>
Realization is therefore the visual and host-facing embodiment layer of a widget, not the semantic source of truth for what the widget is.
</p>

<hr/>

<h2 id="why-this-document-exists">2. Why This Document Exists</h2>

<p>
Graphical widgets need rendering and host integration, but rendering is not the same thing as object law.
</p>

<p>
Without a clear realization boundary, widget systems often drift into one of two failures:
</p>

<ul>
  <li>the visual asset becomes the hidden owner of widget meaning,</li>
  <li>one runtime's private toolkit mapping becomes the true widget definition.</li>
</ul>

<p>
FROG rejects both failures.
</p>

<p>
A widget may be rendered richly, skinned flexibly, and realized differently on different hosts, but its public object model must remain stable and inspectable independently of realization details.
</p>

<p>
This document therefore exists to preserve a disciplined layering:
</p>

<ul>
  <li>class law defines the public surface,</li>
  <li>behavior defines reaction,</li>
  <li>realization defines embodiment,</li>
  <li>runtime-private support defines implementation convenience only.</li>
</ul>

<hr/>

<h2 id="scope">3. Scope</h2>

<p>
This document defines:
</p>

<ul>
  <li>the realization boundary around widget classes,</li>
  <li>the layering between abstract appearance, layout, visual resources, and host support,</li>
  <li>how parts bind to visual assets,</li>
  <li>how SVG participates in the realization corridor,</li>
  <li>how realization stays subordinate to widget class law and widget behavior law.</li>
</ul>

<p>
This document does not define:
</p>

<ul>
  <li>the full widget package serialization format,</li>
  <li>the full class contract structure,</li>
  <li>the full bounded behavior structure,</li>
  <li>the private rendering internals of any one runtime family.</li>
</ul>

<p>
It also does not define the legal set of public properties, methods, events, or parts.
It defines how those already-published surfaces may be visually and interactively embodied on a host.
</p>

<hr/>

<h2 id="what-realization-means-in-frog">4. What Realization Means in FROG</h2>

<p>
In FROG, realization means the host-facing visual and interactive embodiment of a widget class.
</p>

<p>
Realization answers questions such as:
</p>

<ul>
  <li>how is the widget drawn,</li>
  <li>how are its parts mapped to visual regions,</li>
  <li>which visual assets are used,</li>
  <li>how are appearance tokens resolved,</li>
  <li>how does a host toolkit support focus, redraw, hit testing, and part-local interaction.</li>
</ul>

<p>
Realization does not answer:
</p>

<ul>
  <li>which public properties exist,</li>
  <li>which public methods exist,</li>
  <li>which public events exist,</li>
  <li>which public parts exist,</li>
  <li>what the primary value law of the widget is.</li>
</ul>

<p>
Realization is therefore about embodiment, mapping, and support.
It is not about defining public semantic law.
</p>

<hr/>

<h2 id="ownership-boundary">5. Ownership Boundary</h2>

<p>
The following distinctions are normative:
</p>

<pre><code>widget class contract -&gt; public object law
widget behavior       -&gt; portable reaction law
widget realization    -&gt; visual and host-facing embodiment
runtime-private code  -&gt; internal realization support details
</code></pre>

<p>
Accordingly:
</p>

<ul>
  <li>realization MUST follow published class law,</li>
  <li>realization MUST NOT invent undocumented public members,</li>
  <li>realization MUST NOT replace bounded behavior law,</li>
  <li>realization MUST NOT treat visual assets as semantic truth.</li>
</ul>

<p>
Likewise:
</p>

<pre><code>published parts
    !=
toolkit-private visual fragments

appearance tokens
    !=
full semantic object law

host bridge
    !=
widget definition
</code></pre>

<p>
This ownership boundary is what allows several runtimes to realize one widget class differently while still remaining aligned on the same published meaning.
</p>

<hr/>

<h2 id="realization-layers">6. Realization Layers</h2>

<p>
FROG realization is layered.
</p>

<p>
A well-formed realization posture distinguishes:
</p>

<ul>
  <li>abstract appearance,</li>
  <li>layout and sizing,</li>
  <li>visual resources,</li>
  <li>part-to-visual binding,</li>
  <li>runtime-private host support.</li>
</ul>

<p>
This layered structure keeps semantic truth separate from the skin or toolkit.
</p>

<p>
The layers are related but not interchangeable.
A runtime may merge them internally for efficiency,
but a published realization posture must not collapse them into one opaque implementation artifact.
</p>

<hr/>

<h2 id="abstract-appearance-layer">7. Abstract Appearance Layer</h2>

<p>
The abstract appearance layer defines portable appearance-facing properties and tokens without committing to one host toolkit or one concrete visual resource.
</p>

<p>
Examples:
</p>

<ul>
  <li>text color,</li>
  <li>background fill,</li>
  <li>stroke color,</li>
  <li>font family,</li>
  <li>font size,</li>
  <li>padding,</li>
  <li>corner radius,</li>
  <li>alignment.</li>
</ul>

<p>
These surfaces are part of the public or package-published object model where declared. They remain portable and inspectable.
</p>

<p>
The abstract appearance layer exists so that runtimes can preserve appearance intent even when they do not share one concrete rendering implementation.
</p>

<hr/>

<h2 id="layout-and-sizing-layer">8. Layout and Sizing Layer</h2>

<p>
The layout and sizing layer defines how the widget is spatially structured.
</p>

<p>
Examples:
</p>

<ul>
  <li>minimum size,</li>
  <li>preferred size,</li>
  <li>resizable dimensions,</li>
  <li>label placement,</li>
  <li>button placement,</li>
  <li>internal spacing.</li>
</ul>

<p>
Layout posture may influence realization, but layout posture does not create or remove public object law.
</p>

<p>
This layer may guide host windowing, part placement, scaling, and responsive adaptation,
but it must remain subordinate to the already-published part structure and appearance surfaces.
</p>

<hr/>

<h2 id="visual-resource-layer">9. Visual Resource Layer</h2>

<p>
The visual resource layer contains concrete assets used by realization.
</p>

<p>
Examples:
</p>

<ul>
  <li>SVG files,</li>
  <li>icon fragments,</li>
  <li>vector templates,</li>
  <li>named layers,</li>
  <li>anchor sets.</li>
</ul>

<p>
These resources support rendering.
</p>

<p>
They do not replace the widget class contract.
</p>

<p>
A visual resource may help a runtime render a widget faithfully.
It does not publish new portable properties, methods, events, parts, or behavior.
</p>

<hr/>

<h2 id="part-to-visual-binding">10. Part-to-Visual Binding</h2>

<p>
A widget realization may bind named widget parts to concrete visual regions or anchors.
</p>

<p>
This is the preferred mechanism for connecting:
</p>

<ul>
  <li>public or package-published parts,</li>
  <li>part-facing appearance tokens,</li>
  <li>part-local visual updates,</li>
  <li>part-local interaction hit regions.</li>
</ul>

<p>
For example, a numeric control may bind:
</p>

<ul>
  <li><code>label</code> to a text layer or text anchor,</li>
  <li><code>value_display</code> to a numeric text region,</li>
  <li><code>increment_button</code> to an up-arrow hit region,</li>
  <li><code>decrement_button</code> to a down-arrow hit region,</li>
  <li><code>frame</code> to an outline layer.</li>
</ul>

<p>
The existence and names of these parts come from widget class law, not from the visual asset.
</p>

<p>
Part-to-visual binding therefore maps published semantic parts onto realization regions.
It does not authorize realization to invent the semantic part model retroactively.
</p>

<hr/>

<h2 id="svg-integration">11. SVG Integration</h2>

<p>
SVG may be used as a realization resource in FROG.
</p>

<p>
SVG MAY provide:
</p>

<ul>
  <li>scalable skins,</li>
  <li>decorative layers,</li>
  <li>anchor regions,</li>
  <li>template geometry,</li>
  <li>part-binding hints.</li>
</ul>

<p>
SVG MUST NOT own:
</p>

<ul>
  <li>the primary value semantics of the widget,</li>
  <li>the legality of properties,</li>
  <li>the legality of methods,</li>
  <li>the legality of events,</li>
  <li>the legality of parts,</li>
  <li>the bounded behavior law of the widget.</li>
</ul>

<p>
SVG is therefore a realization asset, not a semantic widget definition language.
</p>

<p>
A runtime MAY:
</p>

<ul>
  <li>render an SVG-backed realization faithfully,</li>
  <li>bind published parts to SVG anchors,</li>
  <li>ignore unsupported SVG-specific detail,</li>
  <li>substitute a compatible native realization.</li>
</ul>

<p>
But it MUST preserve the published public widget surface independently of the SVG asset.
</p>

<hr/>

<h2 id="runtime-host-bridges">12. Runtime Host Bridges</h2>

<p>
Different runtime families may realize the same widget class through different host bridges.
</p>

<p>
Examples include:
</p>

<ul>
  <li>a Python runtime using a Qt-based widget backend,</li>
  <li>a Rust runtime using a retained-mode UI toolkit,</li>
  <li>a C/C++ runtime using a native or embedded rendering backend.</li>
</ul>

<p>
These bridges may differ internally in:
</p>

<ul>
  <li>event-loop integration,</li>
  <li>focus handling,</li>
  <li>redraw scheduling,</li>
  <li>accessibility bridging,</li>
  <li>rendering strategy.</li>
</ul>

<p>
Such variation is allowed.
</p>

<p>
What must remain stable is the published object surface and the portable meaning of the realized widget.
</p>

<p>
Host bridges therefore provide transport from published widget law to host embodiment.
They are implementation bridges, not semantic authorities.
</p>

<hr/>

<h2 id="interaction-with-behavior">13. Interaction with Behavior</h2>

<p>
Realization and behavior are related but distinct.
</p>

<p>
Behavior may request:
</p>

<ul>
  <li>a visual refresh,</li>
  <li>a part-state update,</li>
  <li>focus movement,</li>
  <li>interaction suppression,</li>
  <li>event emission after host interaction.</li>
</ul>

<p>
Realization provides the host-side embodiment needed for these effects to occur visually and interactively.
</p>

<p>
Realization does not define the portable behavior law by itself.
</p>

<p>
A runtime may internally interleave behavior processing and realization updates,
but the published behavioral meaning must remain derivable from behavior and class-law surfaces,
not from realization artifacts alone.
</p>

<hr/>

<h2 id="interaction-with-widget-class-law">14. Interaction with Widget Class Law</h2>

<p>
Realization is downstream of widget class law.
</p>

<p>
This means:
</p>

<ul>
  <li>published properties may influence realization,</li>
  <li>published parts may bind to visual anchors,</li>
  <li>published events may be emitted in response to host interaction,</li>
  <li>published methods may trigger realized state changes.</li>
</ul>

<p>
But realization MUST NOT:
</p>

<ul>
  <li>invent new portable public properties because a toolkit happens to support them,</li>
  <li>rename public parts privately and thereby change source-level addressing meaning,</li>
  <li>replace the published event surface with host-specific undocumented signals,</li>
  <li>replace the primary value model with a visual-text-only convention.</li>
</ul>

<p>
In short, realization may embody published law,
but it may not revise published law.
</p>

<hr/>

<h2 id="what-realization-must-not-do">15. What Realization Must Not Do</h2>

<p>
The following are prohibited as normative realization doctrine:
</p>

<ul>
  <li>treating SVG as the owner of widget semantics,</li>
  <li>treating one runtime toolkit mapping as the widget definition itself,</li>
  <li>placing the only true part structure inside the rendering layer,</li>
  <li>creating hidden portable public members from host convenience,</li>
  <li>silently altering public mutability posture through realization-only conventions.</li>
</ul>

<p>
The following are also prohibited:
</p>

<ul>
  <li>deriving the public part model solely from one skin file,</li>
  <li>using visual-layer aliases to silently replace published member identities,</li>
  <li>treating host hit regions as if they published new portable parts automatically,</li>
  <li>making conformance depend on one private rendering pipeline.</li>
</ul>

<hr/>

<h2 id="publication-through-wfrog">16. Publication Through <code>.wfrog</code></h2>

<p>
Realization resources and realization metadata are typically published through <code>.wfrog</code> packages.
</p>

<p>
The package specification owns how that material is serialized.
</p>

<p>
This document instead constrains what realization publication is allowed to mean.
</p>

<p>
Typical package-published realization content may include:
</p>

<ul>
  <li>resource declarations,</li>
  <li>appearance token maps,</li>
  <li>layout hints,</li>
  <li>part-to-resource bindings,</li>
  <li>host realization profiles.</li>
</ul>

<p>
Publication through <code>.wfrog</code> is therefore the normal packaging corridor for realization-facing material,
but that material remains subordinate to class law and behavior doctrine.
</p>

<hr/>

<h2 id="portability-across-runtimes">17. Portability Across Runtimes</h2>

<p>
Portable realization in FROG does not mean pixel-identical rendering across all runtimes.
</p>

<p>
It means:
</p>

<ul>
  <li>equivalent public object surfaces,</li>
  <li>equivalent part structure,</li>
  <li>equivalent interaction meaning,</li>
  <li>equivalent bounded behavior meaning,</li>
  <li>reasonable preservation of appearance intent where the host supports it.</li>
</ul>

<p>
Different runtimes may therefore realize the same widget differently while still remaining conformant.
</p>

<p>
Portability therefore means semantic stability plus reasonable appearance fidelity,
not identical private toolkit behavior or identical raster output.
</p>

<hr/>

<h2 id="status">18. Status</h2>

<p>
This document defines the normative realization boundary for FROG widgets.
</p>

<p>
Its closure direction is:
</p>

<ul>
  <li>explicit part-to-visual mapping,</li>
  <li>portable appearance layers,</li>
  <li>SVG as a resource rather than semantic truth,</li>
  <li>multi-runtime realizability without semantic drift.</li>
</ul>

<p>
Its role in the widget corridor is to keep realization flexible and rich while preventing semantic ownership from leaking into skins, toolkit bridges, or runtime-private implementation details.
</p>

<hr/>

<h2 id="license">19. License</h2>

<p>
See the repository-level license information for the licensing terms governing this specification.
</p>
