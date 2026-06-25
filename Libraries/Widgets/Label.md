<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Standard Label Widget</h1>

<p align="center">
  <strong>Normative baseline for the standardized standalone label support widget</strong><br/>
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
  <li><a href="../../Libraries/Realizations/Default/Label.md">Default label realization</a></li>
  <li><a href="../../Libraries/Realizations/Default/label.default.wfrog">Default label realization manifest</a></li>
</ul>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#class-defined-here">2. Class Defined Here</a></li>
  <li><a href="#label-property-versus-label-widget">3. Label Property versus Label Widget</a></li>
  <li><a href="#class-posture">4. Class Posture</a></li>
  <li><a href="#text-model">5. Text Model</a></li>
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
This document defines <code>frog.widgets.label</code>, the standardized standalone label support widget.
</p>

<p>
A label widget is a front-panel text object used for titles, instructions, annotations, warnings, section descriptions, and other user-facing support text.
It is not the same thing as the common <code>label.*</code> property present on other widgets.
</p>

<p>
The label widget is intentionally simple: it is static support text with a placement aura.
It is still a real widget class, but its default SVG skin must stay minimal so text does not become entangled with decorative boxes or focus markers.
</p>

<hr/>

<h2 id="class-defined-here">2. Class Defined Here</h2>

<ul>
  <li><code>frog.widgets.label</code></li>
</ul>

<hr/>

<h2 id="label-property-versus-label-widget">3. Label Property versus Label Widget</h2>

<p>
FROG distinguishes:
</p>

<pre><code>label.*
    - common structural / logical widget-name surface
    - available on many widget classes

frog.widgets.label
    - standalone support-text widget
    - placed as an object on the front panel
</code></pre>

<p>
For <code>frog.widgets.label</code>, the displayed support text is not <code>label.text</code>.
The displayed support text is:
</p>

<pre><code>text.value</code></pre>

<p>
This avoids the ambiguity where the widget's logical name and its visible support text would otherwise collide.
</p>

<hr/>

<h2 id="class-posture">4. Class Posture</h2>

<ul>
  <li><strong>class_id:</strong> <code>frog.widgets.label</code></li>
  <li><strong>family:</strong> <code>support_widget</code></li>
  <li><strong>compatible role:</strong> <code>support</code></li>
  <li><strong>primary business value:</strong> absent in the intrinsic baseline</li>
  <li><strong>natural <code>widget_value</code> participation:</strong> not required</li>
  <li><strong>preferred access posture:</strong> <code>widget_reference</code> plus <code>frog.ui.*</code></li>
</ul>

<p>
The standardized label is primarily a front-panel support object.
It may be dynamically updated, but it is not a computational value input or output in the way that numeric, string, boolean, or chart widgets are.
</p>

<hr/>

<h2 id="text-model">5. Text Model</h2>

<p>
The semantic support text is owned by:
</p>

<pre><code>text.value</code></pre>

<ul>
  <li><code>text.value</code> — support text content.</li>
  <li><code>text.visible</code> — visibility of the text content inside the label widget.</li>
  <li><code>text.multiline</code> — whether multiline text is allowed.</li>
  <li><code>text.wrap_enabled</code> — whether text wraps inside the label text region.</li>
  <li><code>text.horizontal_alignment</code> — horizontal alignment.</li>
  <li><code>text.vertical_alignment</code> — vertical alignment.</li>
  <li><code>text.padding</code> — inset from the label content region.</li>
  <li><code>text.clip_overflow</code> — whether overflowing text is clipped.</li>
</ul>

<p>
Text styling belongs to portable style surfaces where exposed.
Text placement belongs to realization.
The SVG asset must not be the semantic owner of the live label text.
</p>

<hr/>

<h2 id="public-part-model">6. Public Part Model</h2>

<ul>
  <li><code>root</code></li>
  <li><code>placement_bounds</code> - non-rendered widget placement aura used by IDE/runtime overlays.</li>
  <li><code>text_surface</code></li>
</ul>

<p>
The default Label contract deliberately does not publish <code>background</code>, <code>frame</code>, or <code>focus_ring</code> parts.
Selection and hover visuals belong to the IDE/runtime overlay aligned to <code>placement_bounds</code>.
Visible boxes, callouts, or grouping frames should be represented by decoration/frame widgets composed with a Label.
</p>

<hr/>

<h2 id="standard-property-surface">7. Standard Property Surface</h2>

<h3>7.1 Logical widget label</h3>

<ul>
  <li><code>label.text</code> — structural / logical name of the label widget.</li>
  <li><code>label.visible</code> — whether the structural label is host-visible where supported.</li>
</ul>

<h3>7.2 Support text</h3>

<ul>
  <li><code>text.value : string</code></li>
  <li><code>text.visible : bool</code></li>
  <li><code>text.multiline : bool</code></li>
  <li><code>text.wrap_enabled : bool</code></li>
  <li><code>text.horizontal_alignment : enum</code></li>
  <li><code>text.vertical_alignment : enum</code></li>
  <li><code>text.padding : length</code></li>
  <li><code>text.clip_overflow : bool</code></li>
</ul>

<h3>7.3 Interaction</h3>

<ul>
  <li><code>interaction.visible : bool</code></li>
  <li><code>interaction.enabled : bool</code> — host affordance only; the label remains support-oriented.</li>
  <li><code>interaction.selected : bool</code> when an IDE/runtime wants to expose selection state.</li>
</ul>

<h3>7.4 Style</h3>

<ul>
  <li><code>style.text.color : frog.color.rgba8</code></li>
  <li><code>style.text.font_family : string</code></li>
  <li><code>style.text.font_size : length</code></li>
  <li><code>style.text.font_weight : enum</code></li>
  <li><code>style.text.font_style : enum</code></li>
  <li><code>style.text.text_decoration : enum</code></li>
</ul>

<h3>7.5 Realization</h3>

<ul>
  <li><code>realization.family : string</code></li>
  <li><code>realization.variant : string</code></li>
  <li><code>realization.skin_id : string</code></li>
</ul>

<hr/>

<h2 id="standard-methods">8. Standard Methods</h2>

<ul>
  <li><code>set_text(text : string)</code></li>
  <li><code>clear_text()</code></li>
  <li><code>append_text(text : string)</code></li>
  <li><code>size_to_text()</code> when supported by the host</li>
  <li><code>reset_to_default_style()</code></li>
</ul>

<hr/>

<h2 id="standard-events">9. Standard Events</h2>

<ul>
  <li><code>text_changed</code></li>
  <li><code>value_rendered</code></li>
</ul>

<hr/>

<h2 id="default-realization-posture">10. Default Realization Posture</h2>

<p>
The Default realization exposes a minimal static-text SVG template with <code>root</code>, <code>placement_bounds</code>, and <code>text_surface</code>.
The default aura band is 4 px. The host may replace the template preview text with native text rendering, but it must keep <code>text.value</code> as the semantic owner of the visible text.
</p>

<hr/>

<h2 id="diagram-interaction-posture">11. Diagram Interaction Posture</h2>

<p>
The label supports object-style property access through <code>frog.ui.property_read</code> and <code>frog.ui.property_write</code>, method invocation through <code>frog.ui.method_invoke</code>, event observation where legal, and reference targeting through <code>widget_reference</code>.
</p>

<p>
The label is not standardized as a natural value-path widget in the intrinsic baseline.
</p>

<hr/>

<h2 id="validation-expectations">12. Validation Expectations</h2>

<p>
Validators SHOULD diagnose attempts to treat <code>frog.widgets.label</code> as a required scalar value-carrying widget, use of SVG-baked text as semantic text, confusion between common <code>label.*</code> and label-widget <code>text.value</code>, and Label SVG skins that publish decorative <code>background</code>, <code>frame</code>, or <code>focus_ring</code> parts instead of the standard <code>placement_bounds</code> aura.
</p>

<hr/>

<h2 id="summary">13. Summary</h2>

<p>
<code>frog.widgets.label</code> is the standalone support-text widget of the FROG baseline.
Its visible text content is owned by <code>text.value</code>.
Its logical object name remains <code>label.text</code>.
Its rendering belongs to realization.
</p>
