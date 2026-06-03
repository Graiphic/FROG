<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Standard String Widgets</h1>

<p align="center">
  <strong>Normative baseline for standardized string control and string indicator widget classes</strong><br/>
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
  <li><a href="#string-class-versus-realization">3. String Class versus Realization</a></li>
  <li><a href="#label-and-caption-posture">4. Label and Caption Posture</a></li>
  <li><a href="#common-family-posture">5. Common Family Posture</a></li>
  <li><a href="#public-visual-part-model">6. Public Visual Part Model</a></li>
  <li><a href="#standard-property-surface">7. Standard Property Surface</a></li>
  <li><a href="#frogwidgetsstring_control">8. <code>frog.widgets.string_control</code></a></li>
  <li><a href="#frogwidgetsstring_indicator">9. <code>frog.widgets.string_indicator</code></a></li>
  <li><a href="#default-svg-realization-posture">10. Default SVG Realization Posture</a></li>
  <li><a href="#diagram-interaction-posture">11. Diagram Interaction Posture</a></li>
  <li><a href="#behavior-expectations">12. Behavior Expectations</a></li>
  <li><a href="#validation-expectations">13. Validation Expectations</a></li>
  <li><a href="#summary">14. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the intrinsic standardized baseline for string widgets in FROG.
</p>

<p>
The string family provides the standard widget surfaces used for editable textual input and displayed textual output.
It is intentionally small, portable, inspectable, and strong enough to serve as part of the reusable front-panel baseline.
</p>

<p>
The standard string family is defined as a real object surface with:
</p>

<ul>
  <li>a primary string value posture,</li>
  <li>a portable property surface,</li>
  <li>a minimal method surface,</li>
  <li>a minimal event surface,</li>
  <li>a public part model for realization targeting,</li>
  <li>a default SVG-oriented realization posture suitable for IDE manipulation and runtime interpretation.</li>
</ul>

<p>
The intrinsic string baseline is a text-value widget family.
It does not standardize a full rich-text editor, code editor, terminal widget, document editor, or full text-processing component in the intrinsic core.
</p>

<hr/>

<h2 id="classes-defined-here">2. Classes Defined Here</h2>

<ul>
  <li><code>frog.widgets.string_control</code></li>
  <li><code>frog.widgets.string_indicator</code></li>
</ul>

<p>
Both classes share one string family posture.
The control is user-editable in the portable baseline.
The indicator is display-oriented and receives its value from diagram/runtime publication.
</p>

<hr/>

<h2 id="string-class-versus-realization">3. String Class versus Realization</h2>

<p>
A string class is not the same thing as a host text box, a browser input, a toolkit label, or a SVG rectangle containing text.
</p>

<p>
The class owns:
</p>

<ul>
  <li>the string value meaning,</li>
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
  <li>rectangular text-box embodiment, compact text display, multiline embodiment, scroll affordances, or host-native text rendering,</li>
  <li>SVG assets or host-native drawing resources,</li>
  <li>part-to-visual bindings,</li>
  <li>text placement and anchoring,</li>
  <li>skin resources and compatible visual variants.</li>
</ul>

<p>
A different visible text box skin does not automatically create a different string class.
A single-line and a multiline embodiment may remain realization or property-driven variants of the same class as long as the public contract remains the same.
</p>

<hr/>

<h2 id="label-and-caption-posture">4. Label and Caption Posture</h2>

<p>
The string family follows the shared FROG label/caption convention.
</p>

<pre><code>label.*
    - structural / logical widget name surface
    - useful for diagrams, references, tooling, accessibility, debugging, and program review

caption.*
    - front-panel presentation text
    - user-facing display caption placed by realization
</code></pre>

<p>
This distinction is useful for string widgets because the main displayed string value is already owned by <code>value</code>.
A string widget may therefore have:
</p>

<ul>
  <li>a structural label used by tooling,</li>
  <li>a visible caption shown on the front panel,</li>
  <li>a current string value displayed or edited inside the text region,</li>
  <li>placeholder text shown when the value is empty.</li>
</ul>

<p>
Those four text surfaces must remain distinct.
</p>

<hr/>

<h2 id="common-family-posture">5. Common Family Posture</h2>

<ul>
  <li>family: scalar string widget family</li>
  <li>primary value: present</li>
  <li>value type: <code>string</code></li>
  <li>primary value mirror property: <code>value</code></li>
  <li>natural value participation: yes</li>
  <li>object-style access surface: yes</li>
  <li>structural label surface: <code>label.*</code></li>
  <li>front-panel caption surface: <code>caption.*</code></li>
  <li>placeholder surface: <code>placeholder.*</code></li>
  <li>text behavior surface: <code>text.*</code></li>
  <li>scroll surface: <code>scroll.*</code></li>
  <li>interaction surface: <code>interaction.*</code></li>
  <li>portable style surface: <code>style.*</code></li>
  <li>portable realization-selection surface: <code>realization.*</code></li>
</ul>

<p>
The string family follows these architectural rules:
</p>

<ul>
  <li><code>value</code> is class-owned semantic string data.</li>
  <li><code>label.text</code> is class-owned logical widget-name text.</li>
  <li><code>caption.text</code> is class-owned user-facing caption text.</li>
  <li><code>placeholder.text</code> is presentation fallback text and does not replace <code>value</code>.</li>
  <li><code>text_region</code> and <code>text_value</code> are stable public visual parts, not runtime-private decorations.</li>
  <li>caret, selection, scrollbars, and editor internals remain realization or host details unless explicitly promoted to public class law.</li>
</ul>

<hr/>

<h2 id="public-visual-part-model">6. Public Visual Part Model</h2>

<p>
The string family exposes the following stable public parts:
</p>

<ul>
  <li><code>root</code> — root widget surface and hit-test container,</li>
  <li><code>label</code> — structural label surface when displayed by tooling or host,</li>
  <li><code>caption</code> — user-facing front-panel caption surface,</li>
  <li><code>frame</code> — outer frame or border surface,</li>
  <li><code>text_region</code> — interior text area or editing/display region,</li>
  <li><code>text_value</code> — visible current value text surface,</li>
  <li><code>placeholder</code> — optional placeholder text surface.</li>
</ul>

<p>
Caret, selection highlight, scroll thumb, scroll track, text layout cache, and host editor handles are not public parts in the intrinsic baseline.
They may be realized by a host, but they are not class law unless standardized later.
</p>

<h3>6.1 Default SVG Skin Geometry Contract</h3>

<p>
The Default rectangular String realization is a semantic SVG skin. Hosts use its
published parts and anchors to place live overlays, not private coordinate
guesses or runtime-hardcoded String geometry.
</p>

<table>
  <thead>
    <tr>
      <th>Part</th>
      <th>Resize posture</th>
      <th>Host interpretation</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>root</code></td>
      <td>Scales to the widget layout rectangle.</td>
      <td>Owns the complete String skin coordinate system.</td>
    </tr>
    <tr>
      <td><code>label</code></td>
      <td>Anchored text.</td>
      <td>Logical/tooling label; it remains separate from the user-facing caption.</td>
    </tr>
    <tr>
      <td><code>caption</code></td>
      <td>Anchored text.</td>
      <td>User-facing caption; host overlays align to <code>caption.anchor</code>.</td>
    </tr>
    <tr>
      <td><code>frame</code></td>
      <td>Stretch surface with uniform non-scaling stroke.</td>
      <td>Outer field frame driven by <code>style.frame.*</code>.</td>
    </tr>
    <tr>
      <td><code>text_region</code></td>
      <td>Stretch surface with uniform non-scaling stroke.</td>
      <td>Main text field body; it is the crop/reference rectangle for embedded String renderings.</td>
    </tr>
    <tr>
      <td><code>text_value</code></td>
      <td>Dynamic text region.</td>
      <td>Current semantic <code>value</code>; host overlays align to <code>text_region.left_center</code>.</td>
    </tr>
    <tr>
      <td><code>placeholder</code></td>
      <td>Optional dynamic text region.</td>
      <td>Empty-value fallback only; it never replaces the semantic <code>value</code>.</td>
    </tr>
  </tbody>
</table>

<p>
The Default String skin intentionally has no public <code>focus_ring</code>,
validation marker, overflow marker, caret, or selection part. Those may exist as
host/editor internals, but a runtime must not address them as String class law.
</p>

<hr/>

<h2 id="standard-property-surface">7. Standard Property Surface</h2>

<h3>7.1 Semantic value properties</h3>

<ul>
  <li><code>value : string</code> — primary string value.</li>
</ul>

<h3>7.2 Structural label properties</h3>

<ul>
  <li><code>label.visible : bool</code></li>
  <li><code>label.text : string</code></li>
  <li><code>label.style.text_color : frog.color.rgba8</code></li>
  <li><code>label.style.font_family : string</code></li>
  <li><code>label.style.font_size : length</code></li>
  <li><code>label.style.font_weight : enum</code></li>
  <li><code>label.style.font_style : enum</code></li>
  <li><code>label.style.text_decoration : enum</code></li>
</ul>

<h3>7.3 Caption properties</h3>

<ul>
  <li><code>caption.visible : bool</code></li>
  <li><code>caption.text : string</code></li>
  <li><code>caption.placement : enum</code> — preferred caption placement such as <code>top</code>, <code>bottom</code>, <code>left</code>, or <code>right</code>.</li>
  <li><code>caption.padding : length</code></li>
  <li><code>caption.style.text_color : frog.color.rgba8</code></li>
  <li><code>caption.style.font_family : string</code></li>
  <li><code>caption.style.font_size : length</code></li>
  <li><code>caption.style.font_weight : enum</code></li>
  <li><code>caption.style.font_style : enum</code></li>
  <li><code>caption.style.text_decoration : enum</code></li>
  <li><code>caption.style.text_alignment : enum</code></li>
</ul>

<h3>7.4 Placeholder properties</h3>

<ul>
  <li><code>placeholder.visible : bool</code></li>
  <li><code>placeholder.text : string</code></li>
  <li><code>placeholder.style.text_color : frog.color.rgba8</code></li>
  <li><code>placeholder.style.font_style : enum</code></li>
</ul>

<p>
Placeholder text is displayed only when the current <code>value</code> is empty and the active realization supports placeholder rendering.
It is not the value.
</p>

<h3>7.5 Interaction properties</h3>

<ul>
  <li><code>interaction.visible : bool</code></li>
  <li><code>interaction.enabled : bool</code></li>
  <li><code>interaction.read_only : bool</code></li>
  <li><code>interaction.focusable : bool</code></li>
  <li><code>interaction.focused : bool</code> — readable focus posture when exposed.</li>
</ul>

<h3>7.6 Text behavior properties</h3>

<ul>
  <li><code>text.single_line : bool</code></li>
  <li><code>text.wrap_enabled : bool</code></li>
  <li><code>text.max_length : u32</code> when bounded length is exposed.</li>
  <li><code>text.update_while_typing : bool</code> — whether user edits publish intermediate value changes before commit.</li>
  <li><code>text.horizontal_alignment : enum</code></li>
  <li><code>text.vertical_alignment : enum</code></li>
  <li><code>text.limit_to_single_line : bool</code> — compatibility-oriented posture equivalent to single-line constraint where exposed.</li>
</ul>

<h3>7.7 Scroll properties</h3>

<ul>
  <li><code>scroll.horizontal.visible : bool</code></li>
  <li><code>scroll.vertical.visible : bool</code></li>
  <li><code>scroll.position.x : number</code> when scroll position is publicly exposed.</li>
  <li><code>scroll.position.y : number</code> when scroll position is publicly exposed.</li>
</ul>

<h3>7.8 Frame and text-region style properties</h3>

<ul>
  <li><code>style.frame.fill_color : frog.color.rgba8</code></li>
  <li><code>style.frame.border_color : frog.color.rgba8</code></li>
  <li><code>style.frame.border_width : length</code></li>
  <li><code>style.frame.corner_radius : length</code></li>
  <li><code>style.text_region.fill_color : frog.color.rgba8</code></li>
  <li><code>style.text_region.border_color : frog.color.rgba8</code></li>
  <li><code>style.text_region.border_width : length</code></li>
  <li><code>style.text_region.inset : length</code></li>
</ul>

<h3>7.9 Text value style properties</h3>

<ul>
  <li><code>style.text.color : frog.color.rgba8</code></li>
  <li><code>style.text.font_family : string</code></li>
  <li><code>style.text.font_size : length</code></li>
  <li><code>style.text.font_weight : enum</code></li>
  <li><code>style.text.font_style : enum</code></li>
  <li><code>style.text.text_decoration : enum</code></li>
</ul>

<h3>7.10 Disabled style properties</h3>

<ul>
  <li><code>style.disabled.opacity : number</code></li>
</ul>

<h3>7.11 Realization-selection properties</h3>

<ul>
  <li><code>realization.family : string</code></li>
  <li><code>realization.variant : string</code> — for example <code>rectangular</code>.</li>
  <li><code>realization.skin_id : string</code></li>
</ul>

<hr/>

<h2 id="frogwidgetsstring_control">8. <code>frog.widgets.string_control</code></h2>

<h3>8.1 Class identity</h3>

<ul>
  <li><strong>class_id:</strong> <code>frog.widgets.string_control</code></li>
  <li><strong>family:</strong> <code>string_widget</code></li>
  <li><strong>compatible role:</strong> <code>control</code></li>
</ul>

<h3>8.2 Primary value posture</h3>

<ul>
  <li>primary value: present</li>
  <li>value type: <code>string</code></li>
  <li>natural value participation: yes</li>
  <li>user-mutable: yes</li>
  <li>diagram-mutable: yes</li>
  <li>runtime-mutable: yes</li>
  <li>mirrored property: <code>value</code></li>
</ul>

<h3>8.3 Standard properties</h3>

<p>
The string control exposes the common string property surface defined in this document.
At minimum, a portable implementation should preserve:
</p>

<ul>
  <li><code>value</code></li>
  <li><code>label.text</code></li>
  <li><code>caption.text</code></li>
  <li><code>caption.visible</code></li>
  <li><code>placeholder.text</code></li>
  <li><code>interaction.visible</code></li>
  <li><code>interaction.enabled</code></li>
  <li><code>interaction.read_only</code></li>
  <li><code>text.single_line</code></li>
  <li><code>text.wrap_enabled</code></li>
  <li><code>text.update_while_typing</code></li>
  <li><code>scroll.horizontal.visible</code></li>
  <li><code>scroll.vertical.visible</code></li>
  <li><code>style.frame.*</code> surfaces declared by the active publication corridor</li>
  <li><code>style.text_region.*</code> surfaces declared by the active publication corridor</li>
  <li><code>style.text.*</code> surfaces declared by the active publication corridor</li>
  <li><code>realization.*</code> surfaces declared by the active publication corridor</li>
</ul>

<h3>8.4 Standard methods</h3>

<ul>
  <li><code>focus()</code> — request focus on the widget where supported.</li>
  <li><code>clear()</code> — set <code>value</code> to the empty string when legal.</li>
  <li><code>select_all()</code> — select all visible/editable text when the active host exposes selection.</li>
  <li><code>append_text(text : string)</code> — append text to the current value when legal.</li>
  <li><code>get_line(index : u32) -> string</code> — return the line at the given index when line access is exposed.</li>
  <li><code>size_to_text()</code> — request host sizing based on current text when the active realization supports it.</li>
  <li><code>reset_to_default()</code> — restore the default value and portable visual defaults where those defaults exist.</li>
</ul>

<h3>8.5 Standard events</h3>

<ul>
  <li><code>value_changed</code> — emitted when the primary string value changes.</li>
  <li><code>editing_started</code> — emitted when user editing begins where event observation is supported.</li>
  <li><code>editing_committed</code> — emitted when user editing is committed.</li>
  <li><code>focus_gained</code></li>
  <li><code>focus_lost</code></li>
</ul>

<h3>8.6 Standard parts</h3>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>frame</code></li>
  <li><code>text_region</code></li>
  <li><code>text_value</code></li>
  <li><code>placeholder</code></li>
</ul>

<hr/>

<h2 id="frogwidgetsstring_indicator">9. <code>frog.widgets.string_indicator</code></h2>

<h3>9.1 Class identity</h3>

<ul>
  <li><strong>class_id:</strong> <code>frog.widgets.string_indicator</code></li>
  <li><strong>family:</strong> <code>string_widget</code></li>
  <li><strong>compatible role:</strong> <code>indicator</code></li>
</ul>

<h3>9.2 Primary value posture</h3>

<ul>
  <li>primary value: present</li>
  <li>value type: <code>string</code></li>
  <li>natural value participation: yes</li>
  <li>user-mutable: no in the standard portable posture</li>
  <li>diagram-mutable: yes</li>
  <li>runtime-mutable: yes</li>
  <li>mirrored property: <code>value</code></li>
</ul>

<h3>9.3 Standard properties</h3>

<p>
The string indicator exposes the display-oriented subset of the string surface.
At minimum, a portable implementation should preserve:
</p>

<ul>
  <li><code>value</code></li>
  <li><code>label.text</code></li>
  <li><code>caption.text</code></li>
  <li><code>caption.visible</code></li>
  <li><code>interaction.visible</code></li>
  <li><code>text.single_line</code></li>
  <li><code>text.wrap_enabled</code></li>
  <li><code>scroll.horizontal.visible</code></li>
  <li><code>scroll.vertical.visible</code></li>
  <li><code>style.frame.*</code> surfaces declared by the active publication corridor</li>
  <li><code>style.text_region.*</code> surfaces declared by the active publication corridor</li>
  <li><code>style.text.*</code> surfaces declared by the active publication corridor</li>
  <li><code>realization.*</code> surfaces declared by the active publication corridor</li>
</ul>

<p>
The presence of <code>interaction.enabled</code> on an indicator is optional and host-facing.
If published, it controls host affordances only and MUST NOT imply user mutability of the string value.
</p>

<h3>9.4 Standard methods</h3>

<ul>
  <li><code>focus()</code> when supported by the host and active publication corridor.</li>
  <li><code>size_to_text()</code> when host sizing based on the current value is supported.</li>
  <li><code>reset_to_default_style()</code> when style defaults exist and the active publication corridor exposes them.</li>
</ul>

<h3>9.5 Standard events</h3>

<ul>
  <li><code>value_rendered</code> — emitted when the visible string value has been refreshed where event observation is supported.</li>
  <li><code>focus_gained</code></li>
  <li><code>focus_lost</code></li>
</ul>

<h3>9.6 Standard parts</h3>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>frame</code></li>
  <li><code>text_region</code></li>
  <li><code>text_value</code></li>
</ul>

<hr/>

<h2 id="default-svg-realization-posture">10. Default SVG Realization Posture</h2>

<p>
The string family should have a default SVG-oriented realization posture.
This does not mean that the SVG file owns string semantics.
It means that a standard realization package may publish SVG-backed assets and part mappings that consume the public string property surface.
</p>

<p>
A default SVG realization SHOULD support:
</p>

<ul>
  <li>resizing through the front-panel layout rectangle,</li>
  <li>mapping of <code>root</code>, <code>caption</code>, <code>frame</code>, <code>text_region</code>, <code>text_value</code>, and <code>placeholder</code> to visible SVG or host-rendered surfaces,</li>
  <li>external caption visibility and styling,</li>
  <li>text value rendering or editing inside <code>text_region</code>,</li>
  <li>placeholder visibility when the value is empty,</li>
  <li>configurable frame and text-region border widths,</li>
  <li>configurable frame and text-region fill colors,</li>
  <li>configurable text style surfaces.</li>
</ul>

<p>
The preferred realization publication model is parameter binding, not arbitrary direct SVG mutation.
</p>

<pre><code>widget property
    -> realization parameter
        -> SVG part/style application
</code></pre>

<hr/>

<h2 id="diagram-interaction-posture">11. Diagram Interaction Posture</h2>

<p>
The string family supports:
</p>

<ul>
  <li>natural value participation through <code>widget_value</code>,</li>
  <li>property access through <code>frog.ui.property_read</code> and <code>frog.ui.property_write</code>,</li>
  <li>method invocation through <code>frog.ui.method_invoke</code> where legal,</li>
  <li>event observation through <code>frog.ui.event_observe</code> where legal.</li>
</ul>

<p>
When the program intent is ordinary string dataflow, the natural value path SHOULD be preferred.
Object-style access remains available for richer widget interaction.
</p>

<p>
Typical legal object-style surfaces include:
</p>

<ul>
  <li><code>value</code></li>
  <li><code>label.text</code></li>
  <li><code>caption.text</code></li>
  <li><code>caption.visible</code></li>
  <li><code>placeholder.text</code></li>
  <li><code>interaction.enabled</code></li>
  <li><code>interaction.visible</code></li>
  <li><code>interaction.read_only</code></li>
  <li><code>text.*</code></li>
  <li><code>scroll.*</code></li>
  <li><code>style.*</code></li>
  <li><code>realization.*</code></li>
</ul>

<hr/>

<h2 id="behavior-expectations">12. Behavior Expectations</h2>

<ul>
  <li>the primary value remains of type <code>string</code>,</li>
  <li>string controls accept user editing only when enabled and not read-only,</li>
  <li>user edits update <code>value</code> according to <code>text.update_while_typing</code> and commit behavior,</li>
  <li>control-side value edits may emit <code>value_changed</code> and <code>editing_committed</code>,</li>
  <li>indicator-side visual refresh may emit <code>value_rendered</code>,</li>
  <li>placeholder presentation, when supported, remains realization-facing and does not replace the actual primary value.</li>
</ul>

<hr/>

<h2 id="validation-expectations">13. Validation Expectations</h2>

<p>
Validators SHOULD diagnose at least:
</p>

<ul>
  <li>non-string <code>value_type</code> on string widgets,</li>
  <li>role/class mismatches,</li>
  <li>attempts to use unsupported public members,</li>
  <li>attempts to treat placeholder presentation as the actual primary value,</li>
  <li>attempts to write user-edit-only surfaces on indicator-only classes where forbidden,</li>
  <li>attempts to address realization-only anchors, text regions, caret internals, selection internals, wrapping internals, scroll internals, or skin-private support structures through <code>frog.ui.*</code> as if they were public members,</li>
  <li>attempts to use styling or skin-selection surfaces to imply a distinct string class contract without explicit class publication.</li>
</ul>

<hr/>

<h2 id="summary">14. Summary</h2>

<p>
The string widget family defines the intrinsic standardized text-value baseline of FROG:
</p>

<ul>
  <li><code>frog.widgets.string_control</code></li>
  <li><code>frog.widgets.string_indicator</code></li>
</ul>

<p>
These classes provide the first standard textual value widgets of the portable front-panel baseline.
They expose a real object surface with properties, methods, events, and parts while keeping realization ownership and runtime-private embodiment clearly separated from class meaning.
</p>
