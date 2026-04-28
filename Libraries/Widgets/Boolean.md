<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Standard Boolean Widgets</h1>

<p align="center">
  <strong>Normative baseline for standardized boolean control and boolean indicator widget classes</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="./Readme.md">Widgets index</a></li>
  <li><a href="../../Expression/Widget.md">Expression widget instances</a></li>
  <li><a href="../../Expression/Widget%20interaction.md">Expression widget interaction</a></li>
  <li><a href="../../Expression/Widget%20realization.md">Expression widget realization</a></li>
  <li><a href="../../Expression/Widget%20package%20(.wfrog).md">Widget package publication</a></li>
  <li><a href="../../Libraries/UI.md">Executable UI primitives</a></li>
  <li><a href="../../Profiles/UI%20Widget%20Classes.md">Profile posture for widget-class adoption</a></li>
</ul>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#classes-defined-here">2. Classes Defined Here</a></li>
  <li><a href="#boolean-class-versus-realization">3. Boolean Class versus Realization</a></li>
  <li><a href="#surface-naming-posture">4. Surface Naming Posture</a></li>
  <li><a href="#common-family-posture">5. Common Family Posture</a></li>
  <li><a href="#public-visual-part-model">6. Public Visual Part Model</a></li>
  <li><a href="#standard-property-surface">7. Standard Property Surface</a></li>
  <li><a href="#frogwidgetsboolean_control">8. <code>frog.widgets.boolean_control</code></a></li>
  <li><a href="#frogwidgetsboolean_indicator">9. <code>frog.widgets.boolean_indicator</code></a></li>
  <li><a href="#default-svg-realization-posture">10. Default SVG Realization Posture</a></li>
  <li><a href="#rectangular-and-circular-realization-variants">11. Rectangular and Circular Realization Variants</a></li>
  <li><a href="#diagram-interaction-posture">12. Diagram Interaction Posture</a></li>
  <li><a href="#behavior-expectations">13. Behavior Expectations</a></li>
  <li><a href="#validation-expectations">14. Validation Expectations</a></li>
  <li><a href="#compatibility-with-bounded-flat-slices">15. Compatibility with Bounded Flat Slices</a></li>
  <li><a href="#summary">16. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the intrinsic standardized baseline for boolean widgets in FROG.
</p>

<p>
The boolean family provides the standard widget surfaces used for true/false interaction and true/false display.
It is intentionally small, portable, inspectable, and strong enough to serve as part of the reusable front-panel baseline.
</p>

<p>
The standard boolean family is defined as a real object surface with:
</p>

<ul>
  <li>a primary boolean value posture,</li>
  <li>a portable property surface,</li>
  <li>a minimal method surface,</li>
  <li>a minimal event surface,</li>
  <li>a public part model for realization targeting,</li>
  <li>a default SVG-oriented realization posture suitable for IDE manipulation and runtime interpretation.</li>
</ul>

<p>
The intrinsic boolean baseline is strictly true/false.
It does not standardize a tri-state boolean model in the intrinsic core.
</p>

<p>
This file defines the boolean class law.
It does not make a particular SVG asset, host toolkit, runtime implementation, or IDE editor operation the semantic owner of the widget.
</p>

<hr/>

<h2 id="classes-defined-here">2. Classes Defined Here</h2>

<p>
This document defines the following standardized widget classes:
</p>

<ul>
  <li><code>frog.widgets.boolean_control</code></li>
  <li><code>frog.widgets.boolean_indicator</code></li>
</ul>

<p>
Both classes share one boolean family posture.
They differ by their role:
the control is user-editable in the portable baseline,
while the indicator is display-oriented and receives its value from diagram/runtime publication.
</p>

<hr/>

<h2 id="boolean-class-versus-realization">3. Boolean Class versus Realization</h2>

<p>
A boolean class is not the same thing as its visible embodiment.
</p>

<p>
The class owns:
</p>

<ul>
  <li>the boolean value meaning,</li>
  <li>the control-versus-indicator distinction,</li>
  <li>the public property inventory,</li>
  <li>the public method inventory,</li>
  <li>the public event inventory,</li>
  <li>the public part model,</li>
  <li>the legality of object-style access through <code>frog.ui.*</code>.</li>
</ul>

<p>
The realization owns:
</p>

<ul>
  <li>rectangular, circular, checkbox-like, switch-like, toggle-like, LED-like, or other visual embodiment choices,</li>
  <li>SVG assets or host-native drawing resources,</li>
  <li>part-to-visual bindings,</li>
  <li>state-to-visual mappings,</li>
  <li>text placement and anchoring,</li>
  <li>skin resources and compatible visual variants.</li>
</ul>

<p>
A different shape does not automatically create a different boolean class.
A rectangular boolean and a circular boolean remain <code>frog.widgets.boolean_control</code> or <code>frog.widgets.boolean_indicator</code> as long as their public contract remains the same.
</p>

<p>
A distinct standardized class should be introduced only when the public contract itself changes:
different value semantics,
different properties,
different methods,
different events,
different public parts,
or different standardized behavior meaning.
</p>

<hr/>

<h2 id="surface-naming-posture">4. Surface Naming Posture</h2>

<p>
The canonical boolean widget surface uses hierarchical member names.
</p>

<p>
The preferred public naming posture is:
</p>

<ul>
  <li><code>value</code> for the semantic boolean value,</li>
  <li><code>label.*</code> for the external widget label,</li>
  <li><code>state_text.*</code> for optional text displayed inside the boolean face,</li>
  <li><code>interaction.*</code> for interaction and visibility posture,</li>
  <li><code>style.*</code> for portable visual configuration,</li>
  <li><code>realization.*</code> for compatible realization selection.</li>
</ul>

<p>
The external widget label and the internal state text are intentionally separate:
</p>

<pre><code>label.*
    - external label owned by the widget instance

state_text.*
    - optional true/false text displayed inside the boolean state face
</code></pre>

<p>
This separation prevents ambiguity between the text naming the widget and the text displayed as part of the true/false embodiment.
</p>

<hr/>

<h2 id="common-family-posture">5. Common Family Posture</h2>

<p>
The boolean family has the following common posture:
</p>

<ul>
  <li>family: scalar boolean widget family</li>
  <li>primary value: present</li>
  <li>value type: <code>bool</code></li>
  <li>primary value mirror property: <code>value</code></li>
  <li>natural value participation: yes</li>
  <li>object-style access surface: yes</li>
  <li>external label surface: <code>label.*</code></li>
  <li>internal state text surface: <code>state_text.*</code></li>
  <li>interaction surface: <code>interaction.*</code></li>
  <li>portable style surface: <code>style.*</code></li>
  <li>portable realization-selection surface: <code>realization.*</code></li>
</ul>

<p>
The boolean family also follows these architectural rules:
</p>

<ul>
  <li><code>value</code> is class-owned semantic boolean data.</li>
  <li><code>label.text</code> is class-owned external label text.</li>
  <li><code>state_text.true_text</code> and <code>state_text.false_text</code> are optional state text surfaces, not the semantic source of the boolean value.</li>
  <li><code>outer_face</code> and <code>inner_face</code> are stable public visual parts, not runtime-private decorations.</li>
  <li>the visual embodiment of true, false, focus, disabled, or pressed posture belongs downstream to realization.</li>
</ul>

<hr/>

<h2 id="public-visual-part-model">6. Public Visual Part Model</h2>

<p>
The boolean family exposes the following stable public parts:
</p>

<ul>
  <li><code>root</code> — root widget surface and hit-test container,</li>
  <li><code>label</code> — external widget label surface,</li>
  <li><code>outer_face</code> — outside boolean visual structure, frame, or body,</li>
  <li><code>inner_face</code> — inside boolean state surface or state marker,</li>
  <li><code>state_text</code> — optional internal true/false text surface,</li>
  <li><code>focus_ring</code> — optional focus visualization surface,</li>
  <li><code>frame</code> — optional outer frame surface when a realization separates frame from outer face.</li>
</ul>

<p>
The following distinction is normative:
</p>

<pre><code>public part
    - stable class-visible target for realization mapping

realization substructure
    - asset-private layer, anchor, helper, group, gradient, mask, or toolkit detail
</code></pre>

<p>
A realization may contain many SVG groups, asset layers, masks, gradients, text anchors, hit regions, or host toolkit structures.
Those details do not become public boolean parts unless they are explicitly published by the class law.
</p>

<hr/>

<h2 id="standard-property-surface">7. Standard Property Surface</h2>

<p>
The boolean family exposes a small but real public property surface.
</p>

<h3>7.1 Semantic value properties</h3>

<ul>
  <li><code>value : bool</code> — primary boolean value.</li>
</ul>

<h3>7.2 External label properties</h3>

<ul>
  <li><code>label.visible : bool</code> — whether the external label is visible.</li>
  <li><code>label.text : string</code> — external widget label text.</li>
  <li><code>label.placement : enum</code> — preferred external label placement, such as <code>top</code>, <code>bottom</code>, <code>left</code>, or <code>right</code>, when supported by the active realization.</li>
  <li><code>label.padding : length</code> — preferred spacing between the external label and the boolean face, when supported.</li>
  <li><code>label.style.text_color : frog.color.rgba8</code></li>
  <li><code>label.style.font_family : string</code></li>
  <li><code>label.style.font_size : length</code></li>
  <li><code>label.style.font_weight : enum</code></li>
  <li><code>label.style.font_style : enum</code></li>
  <li><code>label.style.text_decoration : enum</code></li>
  <li><code>label.style.text_alignment : enum</code></li>
</ul>

<h3>7.3 Internal state-text properties</h3>

<ul>
  <li><code>state_text.visible : bool</code> — whether internal true/false text is visible.</li>
  <li><code>state_text.true_text : string</code> — text displayed inside the boolean face when <code>value</code> is true.</li>
  <li><code>state_text.false_text : string</code> — text displayed inside the boolean face when <code>value</code> is false.</li>
  <li><code>state_text.placement : enum</code> — preferred placement inside the boolean face, such as <code>center</code>, when supported.</li>
  <li><code>state_text.style.text_color.true : frog.color.rgba8</code></li>
  <li><code>state_text.style.text_color.false : frog.color.rgba8</code></li>
  <li><code>state_text.style.font_family : string</code></li>
  <li><code>state_text.style.font_size : length</code></li>
  <li><code>state_text.style.font_weight : enum</code></li>
  <li><code>state_text.style.font_style : enum</code></li>
  <li><code>state_text.style.text_decoration : enum</code></li>
  <li><code>state_text.style.text_alignment : enum</code></li>
</ul>

<p>
The state-text properties do not redefine the boolean value.
They only define optional internal text representation for true and false states.
</p>

<h3>7.4 Interaction properties</h3>

<ul>
  <li><code>interaction.visible : bool</code> — whether the widget is visible.</li>
  <li><code>interaction.enabled : bool</code> — whether the widget accepts user interaction where applicable.</li>
  <li><code>interaction.focusable : bool</code> — whether the widget may receive focus where supported.</li>
  <li><code>interaction.pressed : bool</code> — readable transient pressed posture for controls when exposed.</li>
</ul>

<h3>7.5 Outer-face style properties</h3>

<ul>
  <li><code>style.outer.fill_color.true : frog.color.rgba8</code></li>
  <li><code>style.outer.fill_color.false : frog.color.rgba8</code></li>
  <li><code>style.outer.border_color.true : frog.color.rgba8</code></li>
  <li><code>style.outer.border_color.false : frog.color.rgba8</code></li>
  <li><code>style.outer.border_width : length</code></li>
  <li><code>style.outer.corner_radius : length</code> — used by rectangular variants and ignored or constrained by circular variants.</li>
  <li><code>style.outer.opacity : number</code></li>
</ul>

<h3>7.6 Inner-face style properties</h3>

<ul>
  <li><code>style.inner.visible.true : bool</code></li>
  <li><code>style.inner.visible.false : bool</code></li>
  <li><code>style.inner.fill_color.true : frog.color.rgba8</code></li>
  <li><code>style.inner.fill_color.false : frog.color.rgba8</code></li>
  <li><code>style.inner.border_color.true : frog.color.rgba8</code></li>
  <li><code>style.inner.border_color.false : frog.color.rgba8</code></li>
  <li><code>style.inner.border_width : length</code></li>
  <li><code>style.inner.inset : length</code> — preferred inset between <code>outer_face</code> and <code>inner_face</code>.</li>
  <li><code>style.inner.corner_radius : length</code> — used by rectangular variants and ignored or constrained by circular variants.</li>
  <li><code>style.inner.opacity : number</code></li>
</ul>

<h3>7.7 Focus and disabled style properties</h3>

<ul>
  <li><code>style.focus_ring.visible : bool</code></li>
  <li><code>style.focus_ring.color : frog.color.rgba8</code></li>
  <li><code>style.focus_ring.width : length</code></li>
  <li><code>style.disabled.opacity : number</code></li>
</ul>

<h3>7.8 Realization-selection properties</h3>

<ul>
  <li><code>realization.family : string</code> — compatible realization family identifier.</li>
  <li><code>realization.variant : string</code> — compatible realization variant identifier, such as <code>rectangular</code> or <code>circular</code>.</li>
  <li><code>realization.skin_id : string</code> — compatible skin identity.</li>
</ul>

<p>
These realization-selection properties select compatible embodiment.
They do not create a new widget class and do not redefine class-owned semantics.
</p>

<hr/>

<h2 id="frogwidgetsboolean_control">8. <code>frog.widgets.boolean_control</code></h2>

<h3>8.1 Class identity</h3>

<ul>
  <li><strong>class_id:</strong> <code>frog.widgets.boolean_control</code></li>
  <li><strong>family:</strong> <code>boolean_widget</code></li>
  <li><strong>compatible role:</strong> <code>control</code></li>
</ul>

<h3>8.2 Primary value posture</h3>

<ul>
  <li>primary value: present</li>
  <li>value type: <code>bool</code></li>
  <li>natural value participation: yes</li>
  <li>user-mutable: yes</li>
  <li>diagram-mutable: yes</li>
  <li>runtime-mutable: yes</li>
  <li>mirrored property: <code>value</code></li>
</ul>

<h3>8.3 Standard properties</h3>

<p>
The boolean control exposes the common boolean property surface defined in this document.
At minimum, a portable implementation should preserve:
</p>

<ul>
  <li><code>value</code></li>
  <li><code>label.visible</code></li>
  <li><code>label.text</code></li>
  <li><code>state_text.visible</code></li>
  <li><code>state_text.true_text</code></li>
  <li><code>state_text.false_text</code></li>
  <li><code>interaction.visible</code></li>
  <li><code>interaction.enabled</code></li>
  <li><code>style.outer.*</code> surfaces declared by the active publication corridor</li>
  <li><code>style.inner.*</code> surfaces declared by the active publication corridor</li>
  <li><code>realization.*</code> surfaces declared by the active publication corridor</li>
</ul>

<h3>8.4 Standard methods</h3>

<ul>
  <li><code>focus()</code> — request focus on the widget where supported.</li>
  <li><code>toggle()</code> — invert the current boolean value when the widget is enabled.</li>
  <li><code>set_true()</code> — set <code>value</code> to true when legal.</li>
  <li><code>set_false()</code> — set <code>value</code> to false when legal.</li>
  <li><code>reset_to_default()</code> — restore the default value and portable visual defaults published by the active package where those defaults exist.</li>
</ul>

<h3>8.5 Standard events</h3>

<ul>
  <li><code>value_changed</code> — emitted when the primary boolean value changes.</li>
  <li><code>pressed</code> — emitted for supported press interaction posture.</li>
  <li><code>released</code> — emitted for supported release interaction posture.</li>
  <li><code>focus_gained</code> — emitted when focus is acquired where focus is supported.</li>
  <li><code>focus_lost</code> — emitted when focus is lost where focus is supported.</li>
</ul>

<h3>8.6 Standard parts</h3>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>outer_face</code></li>
  <li><code>inner_face</code></li>
  <li><code>state_text</code></li>
  <li><code>focus_ring</code></li>
  <li><code>frame</code> when present</li>
</ul>

<hr/>

<h2 id="frogwidgetsboolean_indicator">9. <code>frog.widgets.boolean_indicator</code></h2>

<h3>9.1 Class identity</h3>

<ul>
  <li><strong>class_id:</strong> <code>frog.widgets.boolean_indicator</code></li>
  <li><strong>family:</strong> <code>boolean_widget</code></li>
  <li><strong>compatible role:</strong> <code>indicator</code></li>
</ul>

<h3>9.2 Primary value posture</h3>

<ul>
  <li>primary value: present</li>
  <li>value type: <code>bool</code></li>
  <li>natural value participation: yes</li>
  <li>user-mutable: no in the standard portable posture</li>
  <li>diagram-mutable: yes</li>
  <li>runtime-mutable: yes</li>
  <li>mirrored property: <code>value</code></li>
</ul>

<h3>9.3 Standard properties</h3>

<p>
The boolean indicator exposes the same public boolean visual surface as the boolean control where legal, but it does not accept user-originated value toggling in the standard portable posture.
</p>

<p>
At minimum, a portable implementation should preserve:
</p>

<ul>
  <li><code>value</code></li>
  <li><code>label.visible</code></li>
  <li><code>label.text</code></li>
  <li><code>state_text.visible</code></li>
  <li><code>state_text.true_text</code></li>
  <li><code>state_text.false_text</code></li>
  <li><code>interaction.visible</code></li>
  <li><code>style.outer.*</code> surfaces declared by the active publication corridor</li>
  <li><code>style.inner.*</code> surfaces declared by the active publication corridor</li>
  <li><code>realization.*</code> surfaces declared by the active publication corridor</li>
</ul>

<p>
The presence of <code>interaction.enabled</code> on an indicator is optional and host-facing.
If published, it controls host interaction affordances only and MUST NOT imply user mutability of the boolean value.
</p>

<h3>9.4 Standard methods</h3>

<ul>
  <li><code>focus()</code> when supported by the host and active publication corridor.</li>
  <li><code>reset_to_default()</code> when default value or style defaults exist and the active publication corridor exposes them.</li>
</ul>

<h3>9.5 Standard events</h3>

<ul>
  <li><code>value_rendered</code> — emitted when the visible boolean state has been refreshed where event observation is supported.</li>
  <li><code>focus_gained</code> — emitted when focus is acquired where focus is supported.</li>
  <li><code>focus_lost</code> — emitted when focus is lost where focus is supported.</li>
</ul>

<h3>9.6 Standard parts</h3>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>outer_face</code></li>
  <li><code>inner_face</code></li>
  <li><code>state_text</code></li>
  <li><code>focus_ring</code></li>
  <li><code>frame</code> when present</li>
</ul>

<hr/>

<h2 id="default-svg-realization-posture">10. Default SVG Realization Posture</h2>

<p>
The boolean family should have a default SVG-oriented realization posture.
This does not mean that the SVG file owns boolean semantics.
It means that a standard realization package may publish SVG-backed assets and part mappings that consume the public boolean property surface.
</p>

<p>
A default SVG realization SHOULD support:
</p>

<ul>
  <li>resizing through the front-panel layout rectangle,</li>
  <li>mapping of <code>root</code>, <code>label</code>, <code>outer_face</code>, <code>inner_face</code>, <code>state_text</code>, and <code>focus_ring</code> to visible SVG or host-rendered surfaces,</li>
  <li>state-dependent color application for true and false values,</li>
  <li>state-dependent visibility of the inner face when published,</li>
  <li>external label visibility and styling,</li>
  <li>internal state-text visibility and styling,</li>
  <li>configurable outer and inner border widths,</li>
  <li>configurable outer and inner fill colors,</li>
  <li>configurable focus ring display where supported.</li>
</ul>

<p>
The preferred realization publication model is parameter binding, not arbitrary direct SVG mutation.
</p>

<pre><code>widget property
    -> realization parameter
        -> SVG part/style application
</code></pre>

<p>
For example:
</p>

<ul>
  <li><code>style.outer.border_width</code> may bind to the stroke width of the <code>outer_face</code> SVG part.</li>
  <li><code>style.outer.fill_color.true</code> may bind to the fill color of <code>outer_face</code> when <code>value</code> is true.</li>
  <li><code>style.inner.fill_color.false</code> may bind to the fill color of <code>inner_face</code> when <code>value</code> is false.</li>
  <li><code>state_text.true_text</code> may bind to the text content of the <code>state_text</code> part when <code>value</code> is true.</li>
</ul>

<p>
A SVG asset may use implementation-specific element identifiers, but a FROG realization package SHOULD publish stable part bindings.
A host runtime should not need to infer boolean semantics from raw SVG geometry alone.
</p>

<h3>10.1 Recommended SVG part markers</h3>

<p>
When a default SVG asset is authored for FROG, it SHOULD make the intended part mapping easy to recover.
A realization may do this through package metadata, SVG element identifiers, or explicit data attributes.
</p>

<p>
Recommended symbolic markers include:
</p>

<ul>
  <li><code>frog-part:root</code></li>
  <li><code>frog-part:outer_face</code></li>
  <li><code>frog-part:inner_face</code></li>
  <li><code>frog-part:state_text</code></li>
  <li><code>frog-part:focus_ring</code></li>
</ul>

<p>
These markers are realization aids.
They do not replace the class law and do not replace the <code>.wfrog</code> package as the machine-readable publication family.
</p>

<h3>10.2 Width, height, and layout ownership</h3>

<p>
Widget width, height, and position belong primarily to front-panel layout.
The boolean class does not own absolute pixel placement.
</p>

<p>
The default SVG realization should scale to the host-provided layout rectangle.
Properties such as <code>style.outer.border_width</code>, <code>style.inner.inset</code>, and <code>style.outer.corner_radius</code> influence realization geometry within that rectangle, but they do not replace the front-panel layout system.
</p>

<hr/>

<h2 id="rectangular-and-circular-realization-variants">11. Rectangular and Circular Realization Variants</h2>

<p>
The standard boolean class may be realized through rectangular and circular variants.
These are realization variants, not separate widget classes.
</p>

<p>
The recommended variant identifiers are:
</p>

<ul>
  <li><code>rectangular</code></li>
  <li><code>circular</code></li>
</ul>

<p>
A realization package MAY implement those variants through:
</p>

<ul>
  <li>two separate SVG assets, such as one rectangular asset and one circular asset,</li>
  <li>one SVG asset containing multiple named variant groups,</li>
  <li>a host-native drawing realization driven by the same public property surface,</li>
  <li>a mixed realization strategy.</li>
</ul>

<p>
The class law does not require two separate SVG files.
However, a repository-visible default realization is easier to inspect when rectangular and circular variants are published as two clearly named assets or two clearly named variant entries.
</p>

<h3>11.1 Rectangular variant</h3>

<p>
The rectangular variant maps <code>outer_face</code> and <code>inner_face</code> to rectangle-like surfaces.
It SHOULD support:
</p>

<ul>
  <li>scalable width and height from the front-panel layout rectangle,</li>
  <li><code>style.outer.border_width</code>,</li>
  <li><code>style.outer.corner_radius</code>,</li>
  <li><code>style.inner.inset</code>,</li>
  <li><code>style.inner.corner_radius</code>,</li>
  <li>true and false colors for outer and inner surfaces,</li>
  <li>optional centered <code>state_text</code>.</li>
</ul>

<h3>11.2 Circular variant</h3>

<p>
The circular variant maps <code>outer_face</code> and <code>inner_face</code> to circle-like or ellipse-like surfaces.
It SHOULD support:
</p>

<ul>
  <li>scaling from the front-panel layout rectangle,</li>
  <li>circle-preserving behavior when width and height are equal,</li>
  <li>ellipse-compatible behavior when the host allows non-square layout,</li>
  <li><code>style.outer.border_width</code>,</li>
  <li><code>style.inner.inset</code>,</li>
  <li>true and false colors for outer and inner surfaces,</li>
  <li>optional centered <code>state_text</code>.</li>
</ul>

<p>
For circular variants, corner-radius properties are either ignored, clamped, or treated as already implied by the circular geometry.
A runtime MUST NOT reinterpret ignored corner-radius properties as a change in boolean semantics.
</p>

<h3>11.3 Variant selection</h3>

<p>
Variant selection should be expressed through <code>realization.variant</code>.
</p>

<pre><code>{
  "class_ref": "frog.widgets.boolean_control",
  "props": {
    "value": false,
    "label.text": "Enabled",
    "label.visible": true,
    "state_text.visible": true,
    "state_text.true_text": "ON",
    "state_text.false_text": "OFF",
    "realization.family": "svg_boolean_basic",
    "realization.variant": "rectangular"
  }
}</code></pre>

<p>
Changing <code>realization.variant</code> from <code>rectangular</code> to <code>circular</code> changes the embodiment, not the boolean class.
</p>

<hr/>

<h2 id="diagram-interaction-posture">12. Diagram Interaction Posture</h2>

<p>
The boolean family supports:
</p>

<ul>
  <li>natural value participation through <code>widget_value</code>,</li>
  <li>property access through <code>frog.ui.property_read</code> and <code>frog.ui.property_write</code>,</li>
  <li>method invocation through <code>frog.ui.method_invoke</code> where legal,</li>
  <li>event observation through <code>frog.ui.event_observe</code> where legal.</li>
</ul>

<p>
When the program intent is ordinary boolean dataflow, the natural value path SHOULD be preferred.
Object-style access remains available for richer widget interaction.
</p>

<p>
Typical legal object-style surfaces include:
</p>

<ul>
  <li><code>value</code></li>
  <li><code>label.visible</code></li>
  <li><code>label.text</code></li>
  <li><code>state_text.visible</code></li>
  <li><code>state_text.true_text</code></li>
  <li><code>state_text.false_text</code></li>
  <li><code>interaction.enabled</code></li>
  <li><code>interaction.visible</code></li>
  <li><code>style.outer.*</code></li>
  <li><code>style.inner.*</code></li>
  <li><code>realization.*</code> when publicly exposed.</li>
</ul>

<p>
Realization-only anchors, SVG helper groups, state maps, gradients, masks, resource layers, track internals, and toolkit-private structures remain outside the public boolean class surface unless explicitly standardized.
</p>

<hr/>

<h2 id="behavior-expectations">13. Behavior Expectations</h2>

<p>
The intrinsic behavior baseline of the boolean family includes at least:
</p>

<ul>
  <li>the primary value remains boolean,</li>
  <li>boolean controls accept user-originated toggling only when enabled,</li>
  <li>boolean indicators do not accept user-originated value mutation in the portable baseline,</li>
  <li>boolean value transitions may emit <code>value_changed</code> on controls,</li>
  <li>indicator realizations may emit <code>value_rendered</code> when their visible state is refreshed,</li>
  <li>the visible state surfaces follow the current boolean value posture,</li>
  <li>state text, if visible, follows <code>state_text.true_text</code> or <code>state_text.false_text</code> according to <code>value</code>.</li>
</ul>

<p>
Pressed posture, focus posture, host activation feedback, state-specific colors, and SVG layer visibility remain interaction and realization concerns unless explicitly published as portable class surfaces.
They do not redefine the boolean value.
</p>

<hr/>

<h2 id="validation-expectations">14. Validation Expectations</h2>

<p>
Validators SHOULD diagnose at least:
</p>

<ul>
  <li>non-boolean primary value type on boolean widgets,</li>
  <li>role/class mismatches,</li>
  <li>user-originated mutation attempts on <code>frog.widgets.boolean_indicator</code> where forbidden,</li>
  <li>unknown boolean family properties, methods, events, or parts,</li>
  <li>attempts to treat rectangular or circular realization variants as separate class identities by default,</li>
  <li>attempts to treat raw SVG element identifiers as public class members without publication through class law or realization binding metadata,</li>
  <li>attempts to use styling or skin-selection surfaces to imply a distinct boolean class contract without explicit class publication,</li>
  <li>attempts to treat <code>state_text.true_text</code> or <code>state_text.false_text</code> as the semantic owner of the boolean value.</li>
</ul>

<p>
Validators MAY allow bounded package-specific realization properties only when they are explicitly published by the active realization package and do not conflict with the standard boolean class law.
</p>

<hr/>

<h2 id="compatibility-with-bounded-flat-slices">15. Compatibility with Bounded Flat Slices</h2>

<p>
Some frozen bounded example corridors may preserve flat property names for compatibility with an already-published slice.
</p>

<p>
Such surfaces may include names such as:
</p>

<ul>
  <li><code>label</code></li>
  <li><code>visible</code></li>
  <li><code>enabled</code></li>
  <li><code>foreground_color</code></li>
</ul>

<p>
Those flat names may remain valid for a frozen corridor that explicitly publishes them.
They are not the preferred canonical naming posture for newly defined boolean widget classes.
</p>

<p>
The preferred canonical boolean surface for new widget work is:
</p>

<pre><code>label.text
label.visible
interaction.visible
interaction.enabled
style.outer.*
style.inner.*
state_text.*
realization.*
</code></pre>

<p>
This rule allows Example-style frozen slices to remain stable while letting the general widget baseline move toward a more scalable canonical surface.
</p>

<hr/>

<h2 id="summary">16. Summary</h2>

<p>
The boolean widget family defines the intrinsic standardized true/false widget baseline of FROG:
</p>

<ul>
  <li><code>frog.widgets.boolean_control</code></li>
  <li><code>frog.widgets.boolean_indicator</code></li>
</ul>

<p>
These classes provide portable boolean interaction and display surfaces with a real minimal object model:
properties, methods, events, parts, and realization mapping.
</p>

<p>
The baseline explicitly separates:
</p>

<ul>
  <li>semantic boolean value from visual state embodiment,</li>
  <li>external widget label from internal state text,</li>
  <li>class law from SVG realization,</li>
  <li>rectangular or circular variants from class identity,</li>
  <li>IDE-editable visual parameters from runtime-private implementation details.</li>
</ul>

<p>
A rectangular boolean and a circular boolean therefore remain the same standard boolean class when their public contract is the same.
They are realization variants selected through the realization surface, interpreted by the IDE and runtime through published part bindings and properties.
</p>
