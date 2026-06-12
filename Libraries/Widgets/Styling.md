<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Widget Styling</h1>

<p align="center">
  <strong>Normative styling and skin-customization posture for intrinsic standardized widget classes</strong><br/>
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
  <li><a href="#styling-layers">6. Styling Layers</a></li>
  <li><a href="#portable-style-surface">7. Portable Style Surface</a></li>
  <li><a href="#realization-selection-surface">8. Realization-Selection Surface</a></li>
  <li><a href="#resource-override-surface">9. Resource-Override Surface</a></li>
  <li><a href="#widget-parts-and-style-targeting">10. Widget Parts and Style Targeting</a></li>
  <li><a href="#state-aware-styling">11. State-Aware Styling</a></li>
  <li><a href="#interaction-with-dynamic-public-surfaces">12. Interaction with Dynamic Public Surfaces</a></li>
  <li><a href="#minimum-portable-style-properties">13. Minimum Portable Style Properties</a></li>
  <li><a href="#optional-family-specific-style-surfaces">14. Optional Family-Specific Style Surfaces</a></li>
  <li><a href="#instance-level-vs-class-level-styling">15. Instance-Level vs Class-Level Styling</a></li>
  <li><a href="#runtime-contract">16. Runtime Contract</a></li>
  <li><a href="#validation-posture">17. Validation Posture</a></li>
  <li><a href="#anti-patterns">18. Anti-Patterns</a></li>
  <li><a href="#examples">19. Examples</a></li>
  <li><a href="#summary">20. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the styling and skin-customization posture for intrinsic standardized widget classes in FROG.
</p>

<p>
Its role is to make visual customization possible without collapsing widget meaning into skins, SVG files, or runtime-private conventions.
</p>

<p>
The styling system is intended to support:
</p>

<ul>
  <li>portable widget-visible style properties,</li>
  <li>selection of official realization variants,</li>
  <li>bounded realization-side skin overrides,</li>
  <li>instance-level visual customization,</li>
  <li>portable runtime consumption of those visual choices.</li>
</ul>

<p>
This document therefore defines how a widget may be customized visually in a way that remains inspectable, portable, and compatible with the public class law.
</p>

<hr/>

<h2 id="why-this-document-exists">2. Why This Document Exists</h2>

<p>
A standard widget baseline is not sufficient on its own.
A usable front-panel ecosystem also needs a disciplined way to customize appearance.
</p>

<p>
Without an explicit styling posture:
</p>

<ul>
  <li>runtimes drift toward private theme systems,</li>
  <li>skins become ad hoc file substitutions,</li>
  <li>instance-level visual customization becomes non-portable,</li>
  <li>asset overrides risk becoming hidden semantic changes,</li>
  <li>developers cannot tell which visual choices are portable and which are runtime-private.</li>
</ul>

<p>
This document exists to prevent that drift.
It gives FROG a clean answer to a practical requirement already familiar from systems such as LabVIEW:
a widget should remain semantically stable while still being visually customizable.
</p>

<hr/>

<h2 id="scope">3. Scope</h2>

<p>
This document defines:
</p>

<ul>
  <li>the styling architecture for standard widgets,</li>
  <li>the difference between portable style surfaces and realization-owned skinning,</li>
  <li>how an instance may select a realization family or realization variant,</li>
  <li>how bounded resource overrides may be published,</li>
  <li>the minimum portable style surfaces expected across the intrinsic widget baseline.</li>
</ul>

<p>
This document does not define:
</p>

<ul>
  <li>generic widget class law in full,</li>
  <li>the full realization-package format,</li>
  <li>the full asset-tree format,</li>
  <li>one mandatory theming engine,</li>
  <li>one mandatory SVG authoring rule,</li>
  <li>one mandatory runtime rendering toolkit.</li>
</ul>

<p>
Those ownerships remain defined elsewhere in the repository.
This document defines the styling contract that sits between public widget meaning and realization embodiment.
</p>

<hr/>

<h2 id="architectural-position">4. Architectural Position</h2>

<p>
The styling posture occupies the following architectural position:
</p>

<pre><code>Libraries/Widgets/
  - standard widget classes and public class surfaces

Libraries/Widgets/Styling.md
  - portable styling and skin-customization posture

Libraries/Realizations/
  - official realization families

.wfrog publication
  - machine-readable publication of widgets, realizations, and resources

runtime implementation
  - host-side realization and rendering
</code></pre>

<p>
The intended separation is:
</p>

<pre><code>class law
    - semantic widget meaning

styling
    - widget-visible visual customization surface

realization
    - concrete embodiment corridor

resources
    - SVG, anchors, text regions, layers, tokens

runtime
    - consumption and rendering
</code></pre>

<p>
Styling is therefore neither identical to class law nor identical to realization resources.
It is the controlled visual customization surface exposed to programs and tooling.
</p>

<hr/>

<h2 id="core-design-rule">5. Core Design Rule</h2>

<p>
The core design rule of widget styling in FROG is:
</p>

<p>
<strong>visual customization is allowed, but semantic ownership remains with the widget class.</strong>
</p>

<p>
This means:
</p>

<ul>
  <li>a widget may change colors, visual tokens, realization variant, or skin resources,</li>
  <li>a widget may bind to a different compatible realization embodiment,</li>
  <li>a widget may override realization resources within published compatibility limits,</li>
  <li>but those choices must not silently redefine the public semantic meaning of the widget.</li>
</ul>

<p>
For example:
</p>

<ul>
  <li>a boolean control may become checkbox-like or switch-like visually,</li>
  <li>a button may use a different face skin,</li>
  <li>a numeric control may use a different frame style or text color,</li>
  <li>but those changes do not by themselves create a new class, new public method, or new semantic value model.</li>
</ul>

<hr/>

<h2 id="styling-layers">6. Styling Layers</h2>

<p>
The styling architecture is intentionally layered.
</p>

<p>
A conforming widget-styling system SHOULD distinguish at least the following layers:
</p>

<ul>
  <li><strong>portable style surface</strong> — small, standard, widget-visible style properties,</li>
  <li><strong>realization selection surface</strong> — selection of family, variant, or published skin identity,</li>
  <li><strong>resource override surface</strong> — bounded replacement or override of realization-side resources.</li>
</ul>

<p>
Those layers should not be collapsed into one undifferentiated blob.
The point of the split is:
</p>

<ul>
  <li>portable style remains stable across runtimes,</li>
  <li>realization selection remains inspectable,</li>
  <li>resource overrides remain bounded and explicit.</li>
</ul>

<hr/>

<h2 id="portable-style-surface">7. Portable Style Surface</h2>

<p>
The portable style surface is the smallest standard styling layer shared across the intrinsic widget baseline.
</p>

<p>
Its goal is to provide widget-visible visual customization that is:
</p>

<ul>
  <li>small,</li>
  <li>portable,</li>
  <li>runtime-independent in meaning,</li>
  <li>useful in property-style interaction.</li>
</ul>

<p>
Portable style properties belong to the widget-visible public object surface.
They are therefore valid candidates for:
</p>

<ul>
  <li>instance configuration in source,</li>
  <li>property-node interaction through <code>frog.ui.property_read</code> and <code>frog.ui.property_write</code> where legal,</li>
  <li>serialization in widget instances,</li>
  <li>inspection in IDE tooling.</li>
</ul>

<p>
Portable style does not expose realization-private internals such as raw layer names, SVG element ids, or runtime-private render handles.
</p>

<p>
Design-time interaction styling follows the same rule. IDE hover,
preselection, selected-widget aura, selected-cell aura, label highlight, and
equivalent editor overlays may be rendered by a host, but their reusable colors,
fills, and stroke widths must be declared as source-owned or realization-owned
style tokens. A runtime may expose those values as host CSS variables or native
theme variables while rendering, but the accepted visual colors must not live
only as private runtime constants.
</p>

<p>
For current front-panel IDE/calibration examples, the portable source posture is
to declare these values on the canvas using explicit tokens such as
<code>ide.style.aura.selected.*</code>, <code>ide.style.aura.hover.*</code>,
<code>ide.style.label.*</code>, and <code>ide.style.cell.*</code>. The exact
token set may evolve, but the ownership boundary is stable: geometry and
selection behavior are widget/container contracts; color, fill, opacity, and
stroke-width styling are editable skin/theme tokens.
</p>

<hr/>

<h2 id="realization-selection-surface">8. Realization-Selection Surface</h2>

<p>
A widget instance MAY select a compatible realization embodiment without changing class meaning.
</p>

<p>
That realization-selection surface SHOULD remain explicit.
Typical realization-selection properties may include:
</p>

<ul>
  <li><code>realization.family</code>,</li>
  <li><code>realization.variant</code>,</li>
  <li><code>realization.skin_id</code>.</li>
</ul>

<p>
The intended meaning is:
</p>

<ul>
  <li><code>realization.family</code> chooses the realization family, such as <code>default</code>,</li>
  <li><code>realization.variant</code> chooses one compatible embodiment variant within that family,</li>
  <li><code>realization.skin_id</code> chooses one specific published skin identity when such a skin catalog exists.</li>
</ul>

<p>
These surfaces remain visual or embodiment-oriented.
They must not be used to smuggle semantic class changes into what looks like a pure skin choice.
</p>

<hr/>

<h2 id="resource-override-surface">9. Resource-Override Surface</h2>

<p>
A widget instance or higher-level publication MAY override realization-side resources in a bounded and inspectable way.
</p>

<p>
Typical examples include:
</p>

<ul>
  <li>substituting one SVG face resource for another,</li>
  <li>substituting one frame resource for another,</li>
  <li>substituting one anchor map or text-region map with a compatible replacement,</li>
  <li>substituting one published style-token map with another.</li>
</ul>

<p>
The preferred posture for this surface is an explicit override record rather than raw path mutation.
Conceptually:
</p>

<pre><code>{
  "realization": {
    "family": "default",
    "variant": "flat"
  },
  "style": {
    "foreground_color": "#ffffff",
    "background_color": "#2050d0"
  },
  "resource_overrides": {
    "button.face.normal.svg": "custom.button.face.normal.svg",
    "button.face.pressed.svg": "custom.button.face.pressed.svg"
  }
}</code></pre>

<p>
A resource override must remain compatible with the targeted realization contract.
A replacement resource must not silently break the published part model or placement model.
</p>

<hr/>

<h2 id="widget-parts-and-style-targeting">10. Widget Parts and Style Targeting</h2>

<p>
Style and skin customization SHOULD be able to target stable public parts when useful.
</p>

<p>
For example:
</p>

<ul>
  <li>a button may target <code>face</code>, <code>label</code>, or <code>frame</code>,</li>
  <li>a boolean widget may target <code>state_face</code>, <code>label</code>, or <code>frame</code>,</li>
  <li>a numeric widget may target <code>value_display</code>, <code>increment_button</code>, <code>decrement_button</code>, <code>label</code>, or <code>frame</code>.</li>
</ul>

<p>
However, stable part targeting does not authorize arbitrary styling of realization-private sublayers unless a realization family explicitly publishes that layer as a legal styling target.
</p>

<p>
The safe default rule is:
</p>

<ul>
  <li>widget-visible styling targets public parts,</li>
  <li>realization-private styling targets remain inside the realization layer unless explicitly promoted.</li>
</ul>

<hr/>

<h2 id="state-aware-styling">11. State-Aware Styling</h2>

<p>
A styling system MAY allow state-aware visual customization.
</p>

<p>
Typical examples include:
</p>

<ul>
  <li>different face colors for <code>normal</code> and <code>pressed</code>,</li>
  <li>different text contrast for <code>normal</code> and <code>disabled</code>,</li>
  <li>different boolean emphasis for <code>normal_true</code> and <code>normal_false</code>.</li>
</ul>

<p>
The preferred posture is:
</p>

<ul>
  <li>portable style properties remain mostly state-agnostic or lightly state-scoped,</li>
  <li>strong state-sensitive embodiment remains realization-owned and published through <code>state_maps</code>.</li>
</ul>

<p>
In other words:
</p>

<ul>
  <li>portable style may express simple visual preference,</li>
  <li>state-driven skin embodiment still belongs primarily to realization publication.</li>
</ul>

<hr/>

<h2 id="interaction-with-dynamic-public-surfaces">12. Interaction with Dynamic Public Surfaces</h2>

<p>
Dynamic public surfaces such as:
</p>

<ul>
  <li><code>button.label</code>,</li>
  <li><code>numeric.value_display</code>,</li>
  <li><code>string.text_display</code>,</li>
  <li><code>waveform_chart.label</code></li>
</ul>

<p>
must remain semantically class-owned even when heavily styled.
</p>

<p>
This means:
</p>

<ul>
  <li>text color may be customized,</li>
  <li>font family may be customized,</li>
  <li>text alignment may be customized,</li>
  <li>placement resources may be replaced by compatible overrides,</li>
  <li>but the semantic text or value content does not become owned by an SVG file or style token map.</li>
</ul>

<p>
This rule is critical for LabVIEW-like usability with modern architecture.
The user must be able to customize appearance without losing the portable semantic surface.
</p>

<hr/>

<h2 id="minimum-portable-style-properties">13. Minimum Portable Style Properties</h2>

<p>
The following portable style properties are the recommended minimum shared style surface for the intrinsic widget baseline.
</p>

<h3>13.1 Common widget-level style properties</h3>

<ul>
  <li><code>style.foreground_color</code></li>
  <li><code>style.background_color</code></li>
  <li><code>style.border_color</code></li>
  <li><code>style.border_width</code></li>
  <li><code>style.opacity</code></li>
</ul>

<h3>13.2 Border geometry rule</h3>

<p>
Any visible widget border, frame, selection outline, scrollbar track, cell
outline, or equivalent stroke-like surface MUST keep one visually uniform
thickness on every side of the same bordered surface.
</p>

<p>
This rule applies across all widget families, not only to standalone support
frames. A resized widget must not produce a thicker horizontal edge than
vertical edge, or a thicker left edge than right edge, unless the widget class
explicitly publishes separate side-specific border properties.
</p>

<p>
The preferred portable property is one source-owned width value such as
<code>style.border_width</code>, <code>style.frame.border_width</code>,
<code>style.selection.border_width</code>, or the corresponding family-specific
published property. SVG-backed realizations should use non-scaling stroke
behavior or an equivalent host strategy so resize operations preserve the
declared thickness.
</p>

<p>
Runtime renderers must not repair non-uniform borders by adding private
unpublished frames, hidden duplicate lines, or host-only CSS constants. If a
border is visible, its color, visibility, style, and width must be part of the
source / realization property surface or an explicitly documented generic
fallback for missing optional data.
</p>

<p>
Border-adjacent fill regions are part of the same visual contract. When a
widget face, value face, text region, state face, or equivalent published fill
surface is intended to touch the border, the realization must not introduce a
blank inner strip between that fill and the border. The fill region should meet
the stroke inner edge exactly, or it may extend under a transparent /
stroke-only frame when the border stroke is drawn above it.
</p>

<h3>13.3 Text-oriented style properties</h3>

<ul>
  <li><code>style.font_family</code></li>
  <li><code>style.font_size</code></li>
  <li><code>style.font_weight</code></li>
  <li><code>style.text_color</code></li>
  <li><code>style.text_alignment</code></li>
</ul>

<h3>13.4 Realization-selection properties</h3>

<ul>
  <li><code>realization.family</code></li>
  <li><code>realization.variant</code></li>
  <li><code>realization.skin_id</code></li>
</ul>

<p>
These surfaces should be interpreted conservatively:
</p>

<ul>
  <li>they are public styling controls,</li>
  <li>they do not replace realization resources,</li>
  <li>they influence rendering and embodiment selection,</li>
  <li>they must remain semantically subordinate to the widget class.</li>
</ul>

<hr/>

<h2 id="optional-family-specific-style-surfaces">14. Optional Family-Specific Style Surfaces</h2>

<p>
Individual widget families MAY publish additional style surfaces when they are useful and still portable.
</p>

<p>
Examples:
</p>

<ul>
  <li>boolean widgets MAY later expose a portable <code>style.true_color</code> and <code>style.false_color</code>,</li>
  <li>numeric widgets MAY later expose a portable <code>style.value_alignment</code>,</li>
  <li>chart widgets MAY later expose portable plot, grid, or axis colors,</li>
  <li>button widgets MAY later expose a portable <code>style.corner_radius</code> when that surface is judged stable enough.</li>
</ul>

<p>
Such additions should be made cautiously.
The rule remains:
</p>

<ul>
  <li>if a surface is broadly portable and useful, it may become a widget-visible style property,</li>
  <li>if a surface is mainly realization-specific, it should remain realization-owned.</li>
</ul>

<hr/>

<h2 id="instance-level-vs-class-level-styling">15. Instance-Level vs Class-Level Styling</h2>

<p>
The styling posture must support at least two levels:
</p>

<ul>
  <li><strong>class-compatible instance styling</strong> — a widget instance customizes its own appearance,</li>
  <li><strong>shared realization publication</strong> — a skin or realization package defines a reusable embodiment for many instances.</li>
</ul>

<p>
The preferred rule is:
</p>

<ul>
  <li>instances override or select from published style and realization surfaces,</li>
  <li>shared packages publish reusable skins, variants, and assets,</li>
  <li>neither one silently mutates class law.</li>
</ul>

<p>
This keeps local customization and reusable skin publication compatible with one another.
</p>

<hr/>

<h2 id="runtime-contract">16. Runtime Contract</h2>

<p>
A runtime claiming support for widget styling SHOULD preserve at least the following:
</p>

<ul>
  <li>portable interpretation of shared style properties,</li>
  <li>portable selection of compatible realization family or variant when published,</li>
  <li>explicit handling of compatible resource overrides when supported,</li>
  <li>preservation of dynamic public surfaces such as text and values,</li>
  <li>clear distinction between semantic widget data and realization-side embodiment.</li>
</ul>

<p>
A runtime MAY approximate the exact visual result when host toolkit limitations exist.
However, it MUST NOT:
</p>

<ul>
  <li>reinterpret a skin change as a semantic class change,</li>
  <li>treat resource overrides as permission to invent new public members,</li>
  <li>transfer semantic ownership of live text or value into asset files.</li>
</ul>

<hr/>

<h2 id="validation-posture">17. Validation Posture</h2>

<p>
Validators SHOULD diagnose at least:
</p>

<ul>
  <li>unknown portable style properties,</li>
  <li>realization-family selections incompatible with the targeted widget class,</li>
  <li>resource overrides that target unknown realization resources,</li>
  <li>resource overrides that replace a resource with an incompatible kind,</li>
  <li>styling declarations that imply new public semantics without a corresponding class publication,</li>
  <li>styling declarations that appear to transfer semantic ownership of dynamic public content into assets.</li>
</ul>

<p>
Validators MAY also diagnose:
</p>

<ul>
  <li>style surfaces that are too realization-private to be portable,</li>
  <li>state-aware overrides that are inconsistent with the published state vocabulary,</li>
  <li>part-targeted styling that references non-published public parts.</li>
</ul>

<hr/>

<h2 id="anti-patterns">18. Anti-Patterns</h2>

<p>
The following anti-patterns SHOULD be avoided:
</p>

<ul>
  <li>treating a skin variant as if it were automatically a new widget class,</li>
  <li>treating raw SVG substitution as if it required no compatibility checks,</li>
  <li>letting runtime-private theme keys become the de facto public styling API,</li>
  <li>baking the only visible semantic text or value into state-specific SVG files,</li>
  <li>using style surfaces to smuggle new behavior or new semantic state into a widget.</li>
</ul>

<hr/>

<h2 id="examples">19. Examples</h2>

<h3>19.1 Portable style example</h3>

<pre><code>{
  "style": {
    "foreground_color": "#ffffff",
    "background_color": "#2b6cb0",
    "border_color": "#1a365d",
    "font_family": "Inter",
    "font_size": 13,
    "font_weight": 600,
    "text_alignment": "center"
  }
}</code></pre>

<h3>19.2 Realization-selection example</h3>

<pre><code>{
  "realization": {
    "family": "default",
    "variant": "switch_like",
    "skin_id": "modern_blue"
  }
}</code></pre>

<h3>19.3 Resource-override example</h3>

<pre><code>{
  "realization": {
    "family": "default",
    "variant": "flat"
  },
  "resource_overrides": {
    "button.face.normal.svg": "custom.button.face.normal.svg",
    "button.face.pressed.svg": "custom.button.face.pressed.svg"
  }
}</code></pre>

<h3>19.4 Architectural interpretation</h3>

<p>
In the examples above:
</p>

<ul>
  <li>the widget class remains unchanged,</li>
  <li>the style surface changes appearance only,</li>
  <li>the realization variant changes embodiment choice only,</li>
  <li>the resource override changes compatible realization resources only.</li>
</ul>

<p>
None of those changes, by themselves, define a new widget class.
</p>

<hr/>

<h2 id="summary">20. Summary</h2>

<p>
The FROG widget styling posture exists to make standard widgets visually customizable without sacrificing semantic clarity.
</p>

<p>
Its preferred architecture is:
</p>

<ul>
  <li>portable widget-visible style properties for small shared visual customization,</li>
  <li>explicit realization-family or variant selection for embodiment choice,</li>
  <li>bounded resource overrides for advanced skinning,</li>
  <li>clear preservation of class-owned semantic meaning.</li>
</ul>

<p>
In practical terms, this means FROG can support a LabVIEW-like customization posture:
</p>

<ul>
  <li>changing colors,</li>
  <li>changing fonts and alignment,</li>
  <li>choosing compatible skin variants,</li>
  <li>associating compatible SVG-based embodiment resources,</li>
  <li>while keeping widget semantics stable and portable across runtimes.</li>
</ul>

<p>
The next most coherent file to handle after this new one is:
</p>

<ul>
  <li><code>Libraries/Widgets/Button.md</code></li>
</ul>

<p>
That file should now be aligned with this styling contract so that the button class explicitly distinguishes:
</p>

<ul>
  <li>portable text and color style surfaces,</li>
  <li>realization-owned placement,</li>
  <li>and optional skin or variant selection without class-semantic drift.</li>
</ul>
