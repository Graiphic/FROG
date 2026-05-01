<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Standard Frame Widget</h1>

<p align="center">
  <strong>Normative baseline for the standardized standalone frame support widget</strong><br/>
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
  <li><a href="../../Libraries/Realizations/Default/Frame.md">Default frame realization</a></li>
  <li><a href="../../Libraries/Realizations/Default/frame.default.wfrog">Default frame realization manifest</a></li>
</ul>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#class-defined-here">2. Class Defined Here</a></li>
  <li><a href="#frame-part-versus-frame-widget">3. Frame Part versus Frame Widget</a></li>
  <li><a href="#class-posture">4. Class Posture</a></li>
  <li><a href="#caption-and-content-region-model">5. Caption and Content Region Model</a></li>
  <li><a href="#public-part-model">6. Public Part Model</a></li>
  <li><a href="#standard-property-surface">7. Standard Property Surface</a></li>
  <li><a href="#standard-methods">8. Standard Methods</a></li>
  <li><a href="#standard-events">9. Standard Events</a></li>
  <li><a href="#default-realization-posture">10. Default Realization Posture</a></li>
  <li><a href="#diagram-interaction-posture">11. Diagram Interaction Posture</a></li>
  <li><a href="#validation-expectations">12. Validation Expectations</a></li>
  <li><a href="#summary">13. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines <code>frog.widgets.frame</code>, the standardized standalone frame support widget.
</p>

<p>
A frame widget is a front-panel support object used to visually group related widgets, emphasize a region, mark a section, or provide a bounded content area.
It is not a computational value input or output.
</p>

<p>
The frame widget is intentionally support-oriented, but it is still a real widget class with public properties, methods, events, parts, and a default realization posture.
</p>

<hr/>

<h2 id="class-defined-here">2. Class Defined Here</h2>

<ul>
  <li><code>frog.widgets.frame</code></li>
</ul>

<hr/>

<h2 id="frame-part-versus-frame-widget">3. Frame Part versus Frame Widget</h2>

<p>
FROG distinguishes:
</p>

<pre><code>frame
    - a public part inside another widget realization

frog.widgets.frame
    - standalone support widget placed on the front panel
</code></pre>

<p>
A numeric widget may have a <code>frame</code> part.
A button may have a <code>frame</code> part.
That is not the same thing as <code>frog.widgets.frame</code>.
</p>

<p>
The standalone frame widget may visually group or emphasize other widgets, but it does not automatically own their semantics, execution, or layout unless a front-panel composition model explicitly associates them.
</p>

<hr/>

<h2 id="class-posture">4. Class Posture</h2>

<ul>
  <li><strong>class_id:</strong> <code>frog.widgets.frame</code></li>
  <li><strong>family:</strong> <code>support_widget</code></li>
  <li><strong>compatible role:</strong> <code>support</code></li>
  <li><strong>primary business value:</strong> absent in the intrinsic baseline</li>
  <li><strong>natural <code>widget_value</code> participation:</strong> not required</li>
  <li><strong>preferred access posture:</strong> <code>widget_reference</code> plus <code>frog.ui.*</code></li>
</ul>

<p>
The standardized frame is primarily a front-panel structural support object.
It may be dynamically shown, hidden, styled, titled, or used as a visual grouping region, but it is not a computational value surface.
</p>

<hr/>

<h2 id="caption-and-content-region-model">5. Caption and Content Region Model</h2>

<p>
The frame follows the shared FROG label/caption convention:
</p>

<pre><code>label.*
    - structural / logical widget name surface

caption.*
    - visible frame title or user-facing section text
</code></pre>

<p>
The frame may also expose a <code>content_region</code> part.
This part is a visual and realization-side grouping region.
It does not automatically move ownership of child widgets into the frame class.
Actual front-panel containment, z-order, and grouping semantics belong to the front-panel composition model unless explicitly published elsewhere.
</p>

<hr/>

<h2 id="public-part-model">6. Public Part Model</h2>

<ul>
  <li><code>root</code></li>
  <li><code>background</code></li>
  <li><code>border</code></li>
  <li><code>caption_surface</code></li>
  <li><code>content_region</code></li>
  <li><code>focus_ring</code> when supported</li>
</ul>

<p>
The <code>caption_surface</code> part is the visible title-bearing part when a frame caption is enabled.
The <code>content_region</code> part is a visual grouping region, not a hidden semantic container for executable logic.
</p>

<hr/>

<h2 id="standard-property-surface">7. Standard Property Surface</h2>

<h3>7.1 Logical widget label</h3>

<ul>
  <li><code>label.text : string</code></li>
  <li><code>label.visible : bool</code></li>
</ul>

<h3>7.2 Frame caption</h3>

<ul>
  <li><code>caption.visible : bool</code></li>
  <li><code>caption.text : string</code></li>
  <li><code>caption.placement : enum</code> — <code>top_left</code>, <code>top_center</code>, <code>top_right</code>, <code>bottom_left</code>, <code>bottom_center</code>, <code>bottom_right</code></li>
  <li><code>caption.padding : length</code></li>
  <li><code>caption.style.*</code></li>
</ul>

<h3>7.3 Interaction</h3>

<ul>
  <li><code>interaction.visible : bool</code></li>
  <li><code>interaction.enabled : bool</code> — host affordance only; the frame remains support-oriented.</li>
  <li><code>interaction.focusable : bool</code> when exposed.</li>
  <li><code>interaction.focused : bool</code> when exposed.</li>
</ul>

<h3>7.4 Layout hints</h3>

<ul>
  <li><code>layout.content_inset : length</code></li>
  <li><code>layout.clip_content : bool</code></li>
  <li><code>layout.z_order_hint : integer</code> when exposed by the front-panel model.</li>
</ul>

<h3>7.5 Style</h3>

<ul>
  <li><code>style.background.visible : bool</code></li>
  <li><code>style.background.fill_color : frog.color.rgba8</code></li>
  <li><code>style.border.visible : bool</code></li>
  <li><code>style.border.color : frog.color.rgba8</code></li>
  <li><code>style.border.width : length</code></li>
  <li><code>style.border.corner_radius : length</code></li>
  <li><code>style.border.line_style : enum</code></li>
  <li><code>style.content_region.fill_color : frog.color.rgba8</code></li>
  <li><code>style.focus_ring.*</code> when focus posture is exposed.</li>
</ul>

<h3>7.6 Realization</h3>

<ul>
  <li><code>realization.family : string</code></li>
  <li><code>realization.variant : string</code></li>
  <li><code>realization.skin_id : string</code></li>
</ul>

<hr/>

<h2 id="standard-methods">8. Standard Methods</h2>

<ul>
  <li><code>show()</code></li>
  <li><code>hide()</code></li>
  <li><code>set_caption(text : string)</code></li>
  <li><code>clear_caption()</code></li>
  <li><code>reset_to_default_style()</code></li>
  <li><code>focus()</code> when focus posture is supported</li>
</ul>

<hr/>

<h2 id="standard-events">9. Standard Events</h2>

<ul>
  <li><code>visibility_changed</code></li>
  <li><code>caption_changed</code></li>
  <li><code>value_rendered</code></li>
  <li><code>focus_gained</code> when focus posture is supported</li>
  <li><code>focus_lost</code> when focus posture is supported</li>
</ul>

<hr/>

<h2 id="default-realization-posture">10. Default Realization Posture</h2>

<p>
The Default realization should expose a rectangular frame SVG template with:
</p>

<ul>
  <li>a background part,</li>
  <li>a border part,</li>
  <li>a caption surface,</li>
  <li>a content region,</li>
  <li>an optional focus ring.</li>
</ul>

<p>
The SVG template provides geometry and placement surfaces.
It does not own frame semantics or the semantic caption text.
</p>

<hr/>

<h2 id="diagram-interaction-posture">11. Diagram Interaction Posture</h2>

<p>
The frame supports object-style property access through <code>frog.ui.property_read</code> and <code>frog.ui.property_write</code>, method invocation through <code>frog.ui.method_invoke</code>, event observation where legal, and reference targeting through <code>widget_reference</code>.
</p>

<p>
The frame is not standardized as a natural value-path widget in the intrinsic baseline.
</p>

<hr/>

<h2 id="validation-expectations">12. Validation Expectations</h2>

<p>
Validators SHOULD diagnose:
</p>

<ul>
  <li>attempts to treat <code>frog.widgets.frame</code> as a required scalar value-carrying widget,</li>
  <li>access to unknown members or parts,</li>
  <li>confusion between a widget-local <code>frame</code> part and the standalone <code>frog.widgets.frame</code> class,</li>
  <li>use of SVG-baked title text as semantic caption text,</li>
  <li>attempts to treat <code>content_region</code> as owning child-widget semantics without explicit front-panel composition law.</li>
</ul>

<hr/>

<h2 id="summary">13. Summary</h2>

<p>
<code>frog.widgets.frame</code> is the standalone support grouping widget of the FROG baseline.
Its logical object name is <code>label.text</code>.
Its visible title is <code>caption.text</code>.
Its grouping region is a visual and realization-side surface unless the front-panel composition model explicitly assigns containment semantics.
</p>
