<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Standard Picture Widgets</h1>

<p align="center">
  <strong>Normative baseline for standardized picture control and picture indicator widget classes</strong><br/>
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
</ul>

<hr/>

<h2>Classes Defined Here</h2>

<ul>
  <li><code>frog.widgets.picture_control</code></li>
  <li><code>frog.widgets.picture_indicator</code></li>
</ul>

<hr/>
<h2>Overview</h2>

<p>
The Picture family defines standardized image and drawable-surface widgets for FROG. The primary goal is to support industrial image display, RGB/RGBA visualization, grayscale imagery, inspection overlays, viewport control, and bounded pointer interaction without turning one host canvas or image toolkit into the semantic definition of the widget.
</p>

<hr/>

<h2>Relation with <code>frog.image</code></h2>

<p>
Picture widgets display image values. They do not read files, select codecs, or
own image-decoding behavior. A diagram that wants to display a PNG or JPEG from
a path should use an explicit image-library primitive, such as
<code>frog.image.decode_file_rgba8</code>, and wire the resulting image buffer into
the Picture value.
</p>

<pre><code>Path control
  -&gt; frog.image.decode_file_rgba8
  -&gt; Picture indicator</code></pre>

<p>
This keeps file selection, image decoding, and image display as inspectable
program objects instead of hiding decoding inside the widget or the runtime.
</p>

<hr/>

<h2>Picture Data Model</h2>

<ul>
  <li><code>value</code> — class-owned picture payload.</li>
  <li><code>image.data</code> — raw or encoded image payload when image data is represented directly.</li>
  <li><code>image.width</code>, <code>image.height</code> — pixel dimensions.</li>
  <li><code>image.pixel_format</code> — <code>rgb8</code>, <code>rgba8</code>, <code>bgr8</code>, <code>bgra8</code>, <code>gray8</code>, <code>gray16</code>, or profile-supported formats.</li>
  <li><code>image.stride</code> — row stride in bytes when relevant.</li>
  <li><code>image.color_space</code> — <code>srgb</code>, <code>linear_rgb</code>, <code>gray</code>, or profile-supported spaces.</li>
  <li><code>image.origin</code> — pixel-origin convention when exposed.</li>
</ul>

<hr/>

<h2>Viewport and Display Model</h2>

<ul>
  <li><code>viewport.origin</code></li>
  <li><code>viewport.scale</code></li>
  <li><code>viewport.fit_mode</code> — <code>none</code>, <code>fit</code>, <code>fill</code>, or <code>stretch</code>.</li>
  <li><code>display.preserve_aspect_ratio</code></li>
  <li><code>display.interpolation</code> — <code>nearest</code>, <code>linear</code>, or host-compatible.</li>
  <li><code>display.background_color</code></li>
  <li><code>pointer.position</code> when pointer posture is exposed.</li>
</ul>

<hr/>

<h2>Public Parts</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>frame</code></li>
  <li><code>picture_region</code></li>
  <li><code>image_surface</code></li>
  <li><code>overlay_region</code></li>
  <li><code>roi_layer</code></li>
  <li><code>crosshair_layer</code></li>
  <li><code>cursor_readout</code></li>
  <li><code>focus_ring</code></li>
</ul>

<hr/>

<h2>Methods and Events</h2>

<ul>
  <li><code>set_image(image)</code></li>
  <li><code>clear_image()</code></li>
  <li><code>reset_viewport()</code></li>
  <li><code>fit_to_region()</code></li>
  <li><code>set_viewport_origin(origin)</code></li>
  <li><code>set_viewport_scale(scale)</code></li>
</ul>

<ul>
  <li><code>value_changed</code></li>
  <li><code>value_rendered</code></li>
  <li><code>viewport_changed</code></li>
  <li><code>pointer_moved</code></li>
  <li><code>clicked</code></li>
</ul>

<hr/>

<h2>Validation Expectations</h2>

<p>
Validators SHOULD diagnose unsupported pixel formats, inconsistent width/height/stride metadata, invalid viewport scale, unsupported image payload type, role/class mismatch, and attempts to treat realization-private buffers as public picture state.
</p>
