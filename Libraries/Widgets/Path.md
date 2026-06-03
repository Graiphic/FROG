<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Standard Path Widgets</h1>

<p align="center">
  <strong>Normative baseline for standardized path control and path indicator widget classes</strong><br/>
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
  <li><a href="../../Libraries/Realizations/Default/Path.md">Default path realization</a></li>
  <li><a href="../../Libraries/Realizations/Default/path.default.wfrog">Default path realization manifest</a></li>
</ul>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#classes-defined-here">2. Classes Defined Here</a></li>
  <li><a href="#path-class-versus-realization">3. Path Class versus Realization</a></li>
  <li><a href="#value-and-normalization-model">4. Value and Normalization Model</a></li>
  <li><a href="#label-caption-and-display-surfaces">5. Label, Caption, and Display Surfaces</a></li>
  <li><a href="#common-family-posture">6. Common Family Posture</a></li>
  <li><a href="#public-part-model">7. Public Part Model</a></li>
  <li><a href="#standard-property-surface">8. Standard Property Surface</a></li>
  <li><a href="#frogwidgetspath_control">9. <code>frog.widgets.path_control</code></a></li>
  <li><a href="#frogwidgetspath_indicator">10. <code>frog.widgets.path_indicator</code></a></li>
  <li><a href="#host-browse-posture">11. Host Browse Posture</a></li>
  <li><a href="#default-realization-posture">12. Default Realization Posture</a></li>
  <li><a href="#diagram-interaction-posture">13. Diagram Interaction Posture</a></li>
  <li><a href="#validation-expectations">14. Validation Expectations</a></li>
  <li><a href="#summary">15. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the standardized baseline for path widgets in FROG.
</p>

<p>
The path family provides path-typed widgets for editable path selection and displayed path output.
A path widget looks close to a string widget, but it carries stronger semantic meaning: path kind, path normalization, existence policy, browse posture, and host-specific filesystem boundaries must remain explicit.
</p>

<p>
The intrinsic baseline does not define a filesystem API or one mandatory file-dialog system.
It defines a portable path-value widget family that runtimes and IDEs can implement consistently.
</p>

<h3>Review Status</h3>

<p>
Latest Path widget review:
<time datetime="2026-06-03">2026-06-03</time>. The reviewed public surface
covers the Path widget law, the Default Path realization manifest, the
rectangular SVG skin contract, and Example <code>09_path_value_roundtrip</code>
as the scalar Path reference. This review records public source, manifest,
SVG, and documentation posture; it does not publish Graiphic runtime
implementation code.
</p>

<hr/>

<h2 id="classes-defined-here">2. Classes Defined Here</h2>

<ul>
  <li><code>frog.widgets.path_control</code></li>
  <li><code>frog.widgets.path_indicator</code></li>
</ul>

<hr/>

<h2 id="path-class-versus-realization">3. Path Class versus Realization</h2>

<p>
A path class is not the same thing as a string field, a file picker, a folder picker, a browse dialog, or a host-native path control.
</p>

<p>
The class owns:
</p>

<ul>
  <li>the path value semantics,</li>
  <li>the path kind and selection-mode posture,</li>
  <li>the normalization and validation posture,</li>
  <li>the control-versus-indicator distinction,</li>
  <li>the public property inventory,</li>
  <li>the public method inventory,</li>
  <li>the public event inventory,</li>
  <li>the public part model.</li>
</ul>

<p>
The realization owns visible text-field embodiment, browse-button embodiment, clipping and ellipsis display, icon posture, and host-specific picker interaction.
</p>

<hr/>

<h2 id="value-and-normalization-model">4. Value and Normalization Model</h2>

<p>
The canonical path value is:
</p>

<pre><code>value</code></pre>

<p>
The value is path-typed rather than an arbitrary string.
The canonical public surfaces around it are:
</p>

<ul>
  <li><code>path.kind</code> — <code>file</code>, <code>directory</code>, <code>any</code>, or <code>uri</code>.</li>
  <li><code>path.syntax</code> — <code>platform_native</code>, <code>posix</code>, <code>windows</code>, or <code>uri</code>.</li>
  <li><code>path.normalized_value</code> — normalized path string when exposed.</li>
  <li><code>path.display_value</code> — shortened or formatted display value when exposed.</li>
  <li><code>path.exists</code> — readable existence result when the runtime can check it.</li>
  <li><code>path.validation_state</code> — <code>unknown</code>, <code>valid</code>, <code>missing</code>, <code>invalid</code>, or <code>inaccessible</code>.</li>
</ul>

<p>
A runtime MUST NOT silently treat every string as a valid path when a path widget publishes stricter validation surfaces.
</p>

<hr/>

<h2 id="label-caption-and-display-surfaces">5. Label, Caption, and Display Surfaces</h2>

<p>
The path family follows the shared FROG label/caption convention.
</p>

<pre><code>label.*
    - structural / logical widget name surface

caption.*
    - front-panel presentation caption shown near the path widget

path_display
    - visible path text surface
</code></pre>

<p>
The logical widget name, visible caption, semantic path value, and shortened display string are separate surfaces.
</p>

<hr/>

<h2 id="common-family-posture">6. Common Family Posture</h2>

<ul>
  <li>family: path widget family</li>
  <li>primary value: present</li>
  <li>value type: <code>path</code></li>
  <li>natural value participation: yes</li>
  <li>object-style access: yes</li>
  <li>primary value mirror property: <code>value</code></li>
  <li>path behavior surface: <code>path.*</code></li>
  <li>browse behavior surface: <code>browse.*</code></li>
  <li>display behavior surface: <code>display.*</code></li>
  <li>validation behavior surface: <code>validation.*</code></li>
</ul>

<hr/>

<h2 id="public-part-model">7. Public Part Model</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>path_face</code></li>
  <li><code>path_display</code></li>
  <li><code>browse_button</code> when present</li>
  <li><code>browse_button_text</code> when present</li>
  <li><code>path_icon</code> when present</li>
</ul>

<h3>7.1 Default SVG Skin Geometry Contract</h3>

<p>
The Default rectangular path field is a semantic SVG skin. The SVG is not only
artwork: its published parts and anchors define how a host places editable
text, path display text, browse hit targets, optional icons, and browse
affordances.
</p>

<table>
  <thead>
    <tr>
      <th>Part</th>
      <th>Resize posture</th>
      <th>Solidarity / host interpretation</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>root</code></td>
      <td>Scales to the widget layout rectangle.</td>
      <td>Owns the complete Path skin coordinate system.</td>
    </tr>
    <tr>
      <td><code>caption</code></td>
      <td>Anchored text, not stretched artwork.</td>
      <td>Host overlays align to <code>caption.anchor</code>; the logical label remains separate.</td>
    </tr>
    <tr>
      <td><code>path_face</code></td>
      <td>Stretch surface with one uniform border thickness.</td>
      <td>Primary field body and crop/reference rectangle for embedded Path renderings.</td>
    </tr>
    <tr>
      <td><code>path_icon</code></td>
      <td>Optional proportional indivisible group.</td>
      <td>Moves with the left side of <code>path_face</code>; hiding it must move <code>path_display</code> back to the face inset.</td>
    </tr>
    <tr>
      <td><code>path_display</code></td>
      <td>Dynamic text region stretched horizontally with the field.</td>
      <td>Host value overlays align to <code>path_display.left_center</code>, preserve padding, and clip/ellipsis within the field.</td>
    </tr>
    <tr>
      <td><code>browse_button</code></td>
      <td>Optional fixed-size right-pinned surface.</td>
      <td>Present for controls when browse is visible; absent or hidden for indicators without changing path value semantics.</td>
    </tr>
    <tr>
      <td><code>browse_button_text</code></td>
      <td>Centered glyph/text inside <code>browse_button</code>.</td>
      <td>Moves as part of the browse-button solidary group.</td>
    </tr>
  </tbody>
</table>

<p>
Derived or restyled Path skins may change visual shape and styling, but they
must preserve these public parts, anchors, and resize/solidarity rules unless
they publish a distinct realization variant.
</p>

<hr/>

<h2 id="standard-property-surface">8. Standard Property Surface</h2>

<h3>8.1 Value and path semantics</h3>

<ul>
  <li><code>value : path</code></li>
  <li><code>path.kind : enum</code></li>
  <li><code>path.syntax : enum</code></li>
  <li><code>path.normalized_value : string</code> when exposed</li>
  <li><code>path.display_value : string</code> when exposed</li>
  <li><code>path.exists : bool</code> when the runtime can check it</li>
  <li><code>path.validation_state : enum</code></li>
</ul>

<h3>8.2 Label and caption</h3>

<ul>
  <li><code>label.visible : bool</code></li>
  <li><code>label.text : string</code></li>
  <li><code>label.style.*</code></li>
  <li><code>caption.visible : bool</code></li>
  <li><code>caption.text : string</code></li>
  <li><code>caption.placement : enum</code></li>
  <li><code>caption.style.*</code></li>
</ul>

<h3>8.3 Browse and validation</h3>

<ul>
  <li><code>browse.enabled : bool</code></li>
  <li><code>browse.button_visible : bool</code></li>
  <li><code>browse.dialog_title : string</code></li>
  <li><code>browse.initial_directory : path</code> when exposed</li>
  <li><code>browse.file_filters[]</code> when file filtering is exposed</li>
  <li><code>validation.require_exists : bool</code></li>
  <li><code>validation.allow_create : bool</code></li>
  <li><code>validation.allow_relative : bool</code></li>
  <li><code>validation.allow_empty : bool</code></li>
</ul>

<h3>8.4 Display</h3>

<ul>
  <li><code>display.shorten_mode : enum</code> — <code>none</code>, <code>middle_ellipsis</code>, <code>tail</code>, or <code>name_only</code></li>
  <li><code>display.icon_visible : bool</code></li>
</ul>

<h3>8.5 Interaction</h3>

<ul>
  <li><code>interaction.visible : bool</code></li>
  <li><code>interaction.enabled : bool</code></li>
  <li><code>interaction.read_only : bool</code></li>
  <li><code>interaction.focusable : bool</code></li>
  <li><code>interaction.focused : bool</code></li>
</ul>

<h3>8.6 Style and realization</h3>

<ul>
  <li><code>style.path_face.*</code></li>
  <li><code>style.path_display.*</code></li>
  <li><code>style.browse_button.*</code></li>
  <li><code>realization.family : string</code></li>
  <li><code>realization.variant : string</code></li>
  <li><code>realization.skin_id : string</code></li>
</ul>

<hr/>

<h2 id="frogwidgetspath_control">9. <code>frog.widgets.path_control</code></h2>

<h3>9.1 Class identity</h3>

<ul>
  <li><strong>class_id:</strong> <code>frog.widgets.path_control</code></li>
  <li><strong>family:</strong> <code>path_widget</code></li>
  <li><strong>compatible role:</strong> <code>control</code></li>
</ul>

<h3>9.2 Standard methods</h3>

<ul>
  <li><code>focus()</code></li>
  <li><code>browse()</code></li>
  <li><code>clear()</code></li>
  <li><code>normalize()</code></li>
  <li><code>validate()</code></li>
  <li><code>set_path(path)</code></li>
  <li><code>reset_to_default()</code></li>
</ul>

<h3>9.3 Standard events</h3>

<ul>
  <li><code>value_changed</code></li>
  <li><code>browsing_started</code></li>
  <li><code>browsing_committed</code></li>
  <li><code>browsing_cancelled</code></li>
  <li><code>validation_changed</code></li>
  <li><code>focus_gained</code></li>
  <li><code>focus_lost</code></li>
</ul>

<hr/>

<h2 id="frogwidgetspath_indicator">10. <code>frog.widgets.path_indicator</code></h2>

<h3>10.1 Class identity</h3>

<ul>
  <li><strong>class_id:</strong> <code>frog.widgets.path_indicator</code></li>
  <li><strong>family:</strong> <code>path_widget</code></li>
  <li><strong>compatible role:</strong> <code>indicator</code></li>
</ul>

<h3>10.2 Standard methods</h3>

<ul>
  <li><code>focus()</code> when supported by the host</li>
  <li><code>normalize()</code></li>
  <li><code>validate()</code></li>
  <li><code>reset_to_default_style()</code></li>
</ul>

<h3>10.3 Standard events</h3>

<ul>
  <li><code>value_rendered</code></li>
  <li><code>validation_changed</code></li>
  <li><code>focus_gained</code></li>
  <li><code>focus_lost</code></li>
</ul>

<hr/>

<h2 id="host-browse-posture">11. Host Browse Posture</h2>

<p>
The browse action is host-mediated.
The class law standardizes the public request and resulting value update, not one mandatory native dialog.
</p>

<p>
A runtime may use a native file picker, directory picker, browser-provided file system access mechanism, custom picker, or no picker at all, provided that unsupported browse behavior is explicit and does not redefine path value semantics.
</p>

<hr/>

<h2 id="default-realization-posture">12. Default Realization Posture</h2>

<p>
The Default realization should expose a rectangular path field with a value face, path display, optional browse button, optional path icon, caption and label surfaces.
</p>

<hr/>

<h2 id="diagram-interaction-posture">13. Diagram Interaction Posture</h2>

<p>
The path family supports natural value participation through <code>widget_value</code>, property access through <code>frog.ui.property_read</code> and <code>frog.ui.property_write</code>, method invocation through <code>frog.ui.method_invoke</code>, event observation where legal, and widget reference targeting through <code>widget_reference</code>.
</p>

<hr/>

<h2 id="validation-expectations">14. Validation Expectations</h2>

<p>
Validators SHOULD diagnose non-path values, invalid path syntax, unsupported path kind, selected browse features unsupported by the host, forbidden empty paths, forbidden relative paths, missing paths where existence is required, inaccessible paths, and attempts to treat realization-private browse dialog internals as public members.
</p>

<hr/>

<h2 id="summary">15. Summary</h2>

<p>
The path widget family defines standardized path-value control and display widgets:
</p>

<ul>
  <li><code>frog.widgets.path_control</code></li>
  <li><code>frog.widgets.path_indicator</code></li>
</ul>

<p>
The family is path-typed, browse-aware, validation-aware, and realization-neutral.
</p>
