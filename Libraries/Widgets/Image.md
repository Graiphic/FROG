<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Standard Image Widget</h1>

<p align="center">
  <strong>Normative baseline for static pasted image objects on the front panel</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="./Readme.md">Widgets index</a></li>
  <li><a href="../../Expression/Widget.md">Expression widget instances</a></li>
  <li><a href="../../Expression/Widget%20realization.md">Expression widget realization</a></li>
  <li><a href="../../Expression/Widget%20package%20(.wfrog).md">Widget package publication</a></li>
  <li><a href="../../Libraries/Realizations/Default/Image.md">Default Image realization</a></li>
  <li><a href="../../Libraries/Realizations/Default/image.default.wfrog">Default Image realization manifest</a></li>
</ul>

<hr/>

<h2>Overview</h2>

<p>
This document defines <code>frog.widgets.image_static</code>, a static media
widget used when an IDE paste operation places a PNG, JPEG, SVG, or other
supported image asset on the front panel.
</p>

<p>
The Image widget is not the Picture widget. <code>frog.widgets.picture_*</code>
is a value/dataflow image widget whose pixels can be produced by execution.
<code>frog.widgets.image_static</code> is a support/media object: it displays a
source-owned media asset and does not participate in computation by default.
</p>

<hr/>

<h2>Class Defined Here</h2>

<ul>
  <li><code>frog.widgets.image_static</code></li>
</ul>

<hr/>

<h2>Clipboard and Source Model</h2>

<p>
When the user pastes image data, the IDE should materialize the clipboard
payload into a project asset and store a stable reference in the <code>.frog</code>
source. The default canonical form is:
</p>

<pre><code>"media_ref": "./assets/pasted_image.svg"
"media_type": "image/svg+xml"</code></pre>

<p>
Raster formats such as PNG and JPEG keep their original file bytes. SVG keeps
its vector source after the IDE has applied its normal sanitization policy.
Small embedded data URIs may be accepted for temporary or generated sources,
but the normal project posture is an external asset path referenced by
<code>media_ref</code>.
</p>

<hr/>

<h2>Public Part Model</h2>

<ul>
  <li><code>root</code></li>
  <li><code>placement_bounds</code> - the portable IDE aura / placement rectangle.</li>
  <li><code>media_region</code> - the visible body region that displays the image.</li>
  <li><code>media_content</code> - the host-rendered media surface bound to <code>media_ref</code>.</li>
</ul>

<p>
The default Image widget deliberately does not publish focus rings, decorative
markers, selection handles, or editor-only affordances as widget parts.
Selection and hover aura rendering belongs to the IDE/runtime host and uses
<code>placement_bounds</code>.
</p>

<hr/>

<h2>Standard Property Surface</h2>

<ul>
  <li><code>media_ref : string</code> - project-relative path, absolute path, or accepted data URI.</li>
  <li><code>media_type : string</code> - MIME type, for example <code>image/png</code>, <code>image/jpeg</code>, or <code>image/svg+xml</code>.</li>
  <li><code>media.original_format : string</code> - source clipboard or import format.</li>
  <li><code>media.intrinsic_width : u32</code> - original media width when known.</li>
  <li><code>media.intrinsic_height : u32</code> - original media height when known.</li>
  <li><code>media.fit : enum</code> - <code>contain</code>, <code>cover</code>, <code>fill</code>, or <code>none</code>.</li>
  <li><code>media.preserve_aspect_ratio : bool</code> - whether resize preserves the original aspect ratio by default.</li>
  <li><code>media.alt_text : string</code> - accessibility text for the static media.</li>
  <li><code>layout.aura_band_px : length</code> - default <code>4px</code> band between <code>placement_bounds</code> and <code>media_region</code>.</li>
</ul>

<hr/>

<h2>Resize Posture</h2>

<p>
The layout rectangle positions the Image widget by its
<code>placement_bounds</code>. The visible media body is inset by the default
uniform aura band. Resizing changes the placement rectangle; the host then
sizes <code>media_region</code> from that rectangle and the selected fit mode.
</p>

<p>
SVG media scales without raster quality loss. Raster media keeps its original
asset bytes and is visually scaled by the host, matching the expected
PowerPoint-like behavior for pasted images.
</p>

<hr/>

<h2>Default Realization Posture</h2>

<p>
The default realization is a simple rectangular media object. It exposes
semantic parts only and delegates actual image decoding/display to the host
browser/runtime surface.
</p>

<hr/>

<h2>Validation Expectations</h2>

<ul>
  <li>The runtime must load the media from <code>media_ref</code>, not from a hardcoded runtime path.</li>
  <li>The widget must render SVG and raster-compatible media through the same source-owned property surface.</li>
  <li>The IDE aura must be derived from <code>placement_bounds</code> with a default homogeneous <code>4px</code> band around <code>media_region</code>.</li>
  <li>A real browser visual check is required before marking an Image widget example valid.</li>
</ul>

