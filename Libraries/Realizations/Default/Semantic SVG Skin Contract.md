<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Semantic SVG Skin Contract</h1>

<p align="center">
  <strong>Public contract for SVG-backed Default widget skins</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
Default SVG assets are not inert pictures.
They are inspectable widget skins: reusable visual templates whose meaningful
surfaces are named so hosts can project values, labels, overlays, states,
hit targets, and child content without hardcoding example-specific geometry.
</p>

<p>
This document defines the public posture for those SVG-backed skins.
It is downstream from widget class law and from Default realization manifests.
It does not define FROG source semantics, FIR semantics, lowering semantics,
runtime-private algorithms, or IDE-private editing behavior.
</p>

<hr/>

<h2>Core Rule</h2>

<p>
An SVG-backed Default widget is valid only when its functional surfaces are
published through stable semantic markers and matching realization metadata.
The runtime or host may render dynamic overlays, host-native controls, or
optimized surfaces, but those surfaces must align to the published SVG parts,
anchors, slots, and resize policy rather than to private coordinates.
</p>

<pre><code>.frog widget instance
  -> .wfrog realization package
  -> Default SVG asset with semantic markers
  -> host rendering aligned to published parts, anchors, and resize policy
</code></pre>

<hr/>

<h2>Semantic Marker Vocabulary</h2>

<p>
SVG templates should expose meaningful widget surfaces with stable names.
The following marker families are the public vocabulary for SVG-backed skins:
</p>

<table>
  <thead>
    <tr>
      <th>Marker</th>
      <th>Purpose</th>
      <th>Example</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>id</code></td>
      <td>Stable SVG-local identifier, useful for authoring, inspection, and manifest selectors.</td>
      <td><code>id="path_display"</code></td>
    </tr>
    <tr>
      <td><code>data-frog-part</code></td>
      <td>Named widget surface that may be bound, styled, measured, overlaid, hit-tested, or validated.</td>
      <td><code>data-frog-part="path_display"</code></td>
    </tr>
    <tr>
      <td><code>data-frog-anchor</code></td>
      <td>Published placement point or region used for labels, text, overlays, joins, or child alignment.</td>
      <td><code>data-frog-anchor="path_display.left_center"</code></td>
    </tr>
    <tr>
      <td><code>data-frog-bind</code></td>
      <td>Dynamic value, property, or state binding consumed by the host.</td>
      <td><code>data-frog-bind="value.path"</code></td>
    </tr>
    <tr>
      <td><code>data-frog-slot</code></td>
      <td>Composition or child-content placement surface for container and composite widgets.</td>
      <td><code>data-frog-slot="array_element"</code></td>
    </tr>
  </tbody>
</table>

<p>
The marker name is the contract.
Changing the drawn shape behind a marker is allowed when the same public
meaning, resize policy, and host projection posture remain valid.
Removing or renaming a required marker changes the realization contract.
</p>

<hr/>

<h2>Resize And Solidarity Policy</h2>

<p>
Each SVG-backed widget family should publish enough resize information for a
host to resize the widget without guessing which surfaces stretch, stay fixed,
move together, repeat, or clip.
</p>

<p>
The policy may be documented in the widget class-law document, the Default
realization document, the machine-readable <code>.wfrog</code> manifest, the
asset README, or a combination of those surfaces. The publication layer should
name at least:
</p>

<ul>
  <li><strong>stretch surfaces</strong> - parts that grow in width, height, or both, such as text faces, table bodies, array viewports, and plot areas;</li>
  <li><strong>fixed surfaces</strong> - parts that keep a stable size, such as small buttons, increment/decrement affordances, browse buttons, and glyph frames;</li>
  <li><strong>proportional indivisible surfaces</strong> - parts that may scale uniformly but must not be split or distorted, such as icons;</li>
  <li><strong>solidary groups</strong> - surfaces that move together during resize, such as an icon plus its text inset, a scrollbar track plus thumb, or a frame plus joined tab face;</li>
  <li><strong>repeatable regions</strong> - surfaces replicated for rows, columns, cells, items, or array elements;</li>
  <li><strong>clip regions</strong> - viewports whose child content is clipped rather than reshaped;</li>
  <li><strong>text/value regions</strong> - surfaces where dynamic text is projected, including alignment, baseline, padding, and overflow posture.</li>
</ul>

<p>
If a host cannot recover a resize rule for a sensitive part, it may still show
a diagnostic or conservative fallback, but that fallback is not sufficient
evidence for accepting the Default realization.
</p>

<hr/>

<h2>Runtime And Host Consumption</h2>

<p>
Hosts consume a Default skin through the realization manifest and the SVG
markers. A conforming implementation should:
</p>

<ul>
  <li>select widget surfaces by published parts, anchors, slots, and bindings;</li>
  <li>measure current rendered geometry after scaling or resize when placing overlays;</li>
  <li>preserve the class-law value model and public property surface;</li>
  <li>treat host overlays as projections of the SVG contract, not as replacement widget law;</li>
  <li>avoid example-number checks, private coordinate constants, and hidden HTML/CSS shells for accepted Default widgets.</li>
</ul>

<p>
Host-native replacement remains allowed when it preserves the same public class
identity, part model, property surface, interaction posture, state posture, and
validation expectations.
</p>

<hr/>

<h2>Derived Or Restyled Skins</h2>

<p>
A derived skin may change visual design: colors, strokes, corner posture,
fills, icon art, spacing, or decorative shapes.
It remains compatible with the parent realization only when it preserves:
</p>

<ul>
  <li>required semantic part names;</li>
  <li>required anchors, slots, and bindings;</li>
  <li>the published resize and solidarity policy;</li>
  <li>the public state posture;</li>
  <li>the expected dynamic text/value projection regions;</li>
  <li>the same host capability assumptions.</li>
</ul>

<p>
If a restyled SVG changes those surfaces, it should be published as a new
realization variant or as a deliberate contract update, not silently swapped
under an existing Default asset identifier.
</p>

<hr/>

<h2>Widget-Family Documentation Requirement</h2>

<p>
Every SVG-backed widget family should make its skin contract reviewable in the
public repository. The relevant documents should identify:
</p>

<ul>
  <li>required SVG parts and anchors;</li>
  <li>optional SVG parts and when they may be absent;</li>
  <li>dynamic value, label, caption, and state bindings;</li>
  <li>resize, fixed-size, proportional, solidary, repeatable, and clip surfaces;</li>
  <li>host overlay surfaces and their alignment expectations;</li>
  <li>accepted visual states such as normal, hover, pressed, focused, disabled, selected, read-only, and overflow where relevant;</li>
  <li>known limitations or deferred surfaces.</li>
</ul>

<p>
The natural homes are:
</p>

<pre><code>Libraries/Widgets/&lt;Widget&gt;.md
Libraries/Realizations/Default/&lt;Widget&gt;.md
Libraries/Realizations/Default/&lt;widget&gt;.default.wfrog
Libraries/Realizations/Default/assets/&lt;widget&gt;/Readme.md
</code></pre>

<hr/>

<h2>Validation Posture</h2>

<p>
Validation of an SVG-backed skin should include:
</p>

<ul>
  <li>manifest resource resolution;</li>
  <li>presence of required SVG markers;</li>
  <li>alignment between manifest part bindings and SVG markers;</li>
  <li>raw SVG inspection for marker clarity, border posture, and internal geometry;</li>
  <li>rendered example inspection at the final widget size and aspect ratio;</li>
  <li>geometry measurement for overlays that claim to align to SVG parts or anchors;</li>
  <li>state inspection for visible states published by the realization;</li>
  <li>non-regression comparison when a derived example reuses an accepted widget baseline.</li>
</ul>

<hr/>

<h2>Non-Goals</h2>

<ul>
  <li>This document does not require one browser rendering strategy.</li>
  <li>This document does not forbid native widget, canvas, or GPU-backed hosts.</li>
  <li>This document does not make SVG files the owner of widget values or behavior law.</li>
  <li>This document does not publish private Graiphic runtime implementation details.</li>
</ul>
