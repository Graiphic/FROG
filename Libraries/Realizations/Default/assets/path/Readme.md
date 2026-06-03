<p align="center">
  <img src="../../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Path Realization Assets</h1>

<p align="center">
  <strong>SVG template asset for the default path realization variant</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="../../Path.md">Default path realization posture</a></li>
  <li><a href="../../path.default.wfrog">Machine-readable path realization manifest</a></li>
  <li><a href="../../Package.md">Default realization package posture</a></li>
  <li><a href="../../../../Widgets/Path.md">Path widget class law</a></li>
</ul>

<hr/>

<h2>1. Published Asset</h2>

<ul>
  <li><code>templates/path_rectangular_field.svg</code> — rectangular path-field realization template.</li>
</ul>

<hr/>

<p>
Latest asset review: <time datetime="2026-06-03">2026-06-03</time>. The review
covers the semantic SVG skin parts, anchors, resize/solidarity posture, and
Example 09 C++ browser-host rendering evidence.
</p>

<h2>2. Required Template Markers</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>path_face</code></li>
  <li><code>path_display</code></li>
  <li><code>browse_button</code></li>
  <li><code>browse_button_text</code></li>
  <li><code>path_icon</code></li>
</ul>

<p>
The template intentionally publishes the rectangular field parts used by the
Default Path realization. Hosts may provide focus handling through overlays or
platform behavior, but this asset does not publish a separate
<code>focus_ring</code> part.
</p>

<hr/>

<h2>3. Resize And Solidarity Rules</h2>

<table>
  <thead>
    <tr>
      <th>Surface</th>
      <th>Rule</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>path_face</code></td>
      <td>Stretch with the widget field; preserve one uniform visible border thickness.</td>
    </tr>
    <tr>
      <td><code>path_icon</code></td>
      <td>Keep the folder as one indivisible proportional group; it is left-pinned inside the field and may be hidden.</td>
    </tr>
    <tr>
      <td><code>path_display</code></td>
      <td>Stretch horizontally between the left text inset and the right field/button boundary; host text overlays align to <code>path_display.left_center</code>.</td>
    </tr>
    <tr>
      <td><code>browse_button</code> + <code>browse_button_text</code></td>
      <td>Move as a right-pinned solidary group; the text remains centered in the button.</td>
    </tr>
    <tr>
      <td><code>caption</code></td>
      <td>Anchor text to <code>caption.anchor</code>; do not treat SVG preview text as final runtime text.</td>
    </tr>
  </tbody>
</table>

<p>
The template may contain preview text so vector editors remain readable, but a
runtime must hide or replace preview text when it projects live widget values.
</p>

<hr/>

<h2>4. Raw SVG Audit Expectations</h2>

<ul>
  <li>Every required part must keep a stable <code>id</code> and <code>data-frog-part</code> marker.</li>
  <li>The caption and path display anchors must remain readable through <code>data-frog-anchor</code>.</li>
  <li>The folder icon must remain grouped under <code>path_icon</code>; editing the folder artwork must not split it into unrelated runtime surfaces.</li>
  <li>The field and browse button borders must not create double frames or unintended inner gaps.</li>
  <li>Any restyled Path SVG that breaks these rules must be published as a new realization variant instead of replacing the Default baseline silently.</li>
</ul>

<hr/>

<h2>5. Semantic Metadata Vocabulary</h2>

<p>
The SVG root publishes <code>data-frog-skin-contract="semantic_svg_skin_v1"</code>
and a <code>metadata</code> block named
<code>frog_semantic_skin_contract</code>. Individual SVG parts may also publish
the following attributes so a host can discover geometry intent without
private coordinate guesses:
</p>

<ul>
  <li><code>data-frog-role</code> — human/runtime-readable purpose of the surface.</li>
  <li><code>data-frog-member</code> — primary widget property or semantic member represented by the part.</li>
  <li><code>data-frog-bind</code> — property set that may affect the part.</li>
  <li><code>data-frog-anchor</code> — named anchor used by host overlays.</li>
  <li><code>data-frog-resize</code> — resize policy for the part.</li>
  <li><code>data-frog-solidary-group</code> — named group that moves or hides together.</li>
  <li><code>data-frog-visibility-member</code> — source property controlling optional visibility.</li>
  <li><code>data-frog-border-policy</code> — border thickness rule for border-like surfaces.</li>
  <li><code>data-frog-crop-reference</code> — marks the part usable as a compact embedded-widget crop region.</li>
  <li><code>data-frog-runtime-surface</code> — host overlay role for live runtime rendering.</li>
</ul>
