<h1 align="center">FROG Image Library Specification</h1>

<p align="center">
Definition of the minimal standard <strong>frog.image</strong> library for FROG<br/>
<em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Overview</h2>

<p>
This document defines the first minimal standard <code>frog.image</code>
primitive surface and records the base image/picture/plot direction for future
standardization. Its role is to provide ordinary image-data functions that can
feed Picture widgets without making the Picture widget itself responsible for
file reading or codec behavior.
</p>

<p>
The initial published library is intentionally small. It standardizes a
portable image buffer value shape and one path-based decode primitive suitable
for basic FROG programs and base IDE distributions. Additional base functions
are listed below as candidates, not as already accepted primitives.
</p>

<p>
This is a lightweight standard library family. A base FROG distribution MAY
ship an implementation for it, but programs still call it explicitly, FIR still
preserves the primitive identity, and downstream lowering or packaging still
decides which implementation modules and dependencies are required.
</p>

<hr/>

<h2>Layering Rule</h2>

<p>
A Picture widget is a display surface. It consumes image values and renders
them through a realization such as the Default Picture SVG. It does not read
files and it does not own image-decoding semantics.
</p>

<p>
Image file reading and decoding belongs to explicit diagram functions:
</p>

<pre><code>Path control
  -&gt; frog.image.decode_file_rgba8
  -&gt; ImageBufferRGBA8
  -&gt; Picture indicator</code></pre>

<p>
This preserves a LabVIEW-like dataflow model: the user can see the path, the
decode function, the produced image value, and the Picture indicator as
separate program objects.
</p>

<hr/>

<h2>Namespace</h2>

<p>
FROG image primitives use the <code>frog.image.*</code> namespace.
</p>

<pre><code>frog.image.decode_file_rgba8</code></pre>

<p>
The namespace owns generic image value construction, decoding, and simple image
metadata operations. Specialized computer-vision, camera, GPU, neural,
inspection, calibration, or OpenCV-like functionality belongs in optional
libraries, profiles, or implementation-specific packages unless explicitly
standardized later.
</p>

<hr/>

<h2>Available Function Surface</h2>

<p>
The current published <code>frog.image</code> function surface is deliberately
minimal:
</p>

<table>
  <thead>
    <tr>
      <th>Primitive</th>
      <th>Inputs</th>
      <th>Outputs</th>
      <th>Required baseline formats</th>
      <th>Role</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>frog.image.decode_file_rgba8</code></td>
      <td><code>path</code></td>
      <td><code>image</code>, <code>success</code>, <code>error_code</code></td>
      <td><code>image/png</code>, <code>image/jpeg</code></td>
      <td>Decode a path-selected image file into a portable <code>frog.image.buffer_rgba8</code> value.</td>
    </tr>
  </tbody>
</table>

<p>
No other <code>frog.image</code> primitive is currently published by this
specification. Additional image operations, encoders, camera acquisition,
advanced processing, and OpenCV-like functions remain outside the current
standard lightweight image-library surface unless they are explicitly
standardized later.
</p>

<hr/>

<h2>Base Library Direction</h2>

<p>
To stay LabVIEW-like without collapsing advanced vision tooling into the base
standard surface, FROG separates three related standard-library families:
</p>

<table>
  <thead>
    <tr>
      <th>Family</th>
      <th>Responsibility</th>
      <th>Status</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>frog.image</code></td>
      <td>Image buffers, basic graphics file formats, simple metadata, and pixel-buffer utilities.</td>
      <td><code>decode_file_rgba8</code> is published; the remaining base functions are candidates.</td>
    </tr>
    <tr>
      <td><code>frog.picture</code></td>
      <td>Picture construction and drawing primitives that produce a value renderable by a Picture indicator.</td>
      <td>Candidate family; no primitive is published yet.</td>
    </tr>
    <tr>
      <td><code>frog.plot</code></td>
      <td>Simple plot-to-picture functions for waveform, XY, and multi-XY visualization.</td>
      <td>Candidate family; no primitive is published yet.</td>
    </tr>
  </tbody>
</table>

<p>
This split mirrors the practical separation between graphics format I/O,
picture drawing, and plot generation. It also keeps advanced image-processing
families such as thresholding, filtering, OCR, pattern matching, edge
detection, camera acquisition, and OpenCV-like pipelines outside the lightweight
base surface unless explicitly standardized later.
</p>

<hr/>

<h2>Candidate Base Functions</h2>

<p>
The following functions are base-standard candidates. They document the
intended direction, but they are not considered available until promoted by a
specific specification update, example, manifest contract, and
implementation/conformance validation.
</p>

<h3><code>frog.image</code> candidates</h3>

<table>
  <thead>
    <tr>
      <th>Candidate primitive</th>
      <th>Role</th>
      <th>Implementation note</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>frog.image.read_png_file(path)</code></td>
      <td>Read a PNG file into <code>frog.image.buffer_rgba8</code>.</td>
      <td>Can share the same decode backend as <code>decode_file_rgba8</code>.</td>
    </tr>
    <tr>
      <td><code>frog.image.write_png_file(path, image)</code></td>
      <td>Write a <code>frog.image.buffer_rgba8</code> value as PNG.</td>
      <td>Requires an image-writing provider, not only an image decoder.</td>
    </tr>
    <tr>
      <td><code>frog.image.read_jpeg_file(path)</code></td>
      <td>Read a JPEG file into <code>frog.image.buffer_rgba8</code>.</td>
      <td>Can share the same decode backend as <code>decode_file_rgba8</code>.</td>
    </tr>
    <tr>
      <td><code>frog.image.write_jpeg_file(path, image, quality)</code></td>
      <td>Write a <code>frog.image.buffer_rgba8</code> value as JPEG.</td>
      <td>Requires an image-writing provider and an explicit quality contract.</td>
    </tr>
    <tr>
      <td><code>frog.image.read_bmp_file(path)</code></td>
      <td>Read a BMP file into <code>frog.image.buffer_rgba8</code>.</td>
      <td>Candidate baseline graphics-format reader.</td>
    </tr>
    <tr>
      <td><code>frog.image.write_bmp_file(path, image)</code></td>
      <td>Write a <code>frog.image.buffer_rgba8</code> value as BMP.</td>
      <td>Requires an image-writing provider.</td>
    </tr>
    <tr>
      <td><code>frog.image.get_subset(image, x, y, width, height)</code></td>
      <td>Return a rectangular image subset.</td>
      <td>Can be implemented directly over <code>frog.image.buffer_rgba8</code>.</td>
    </tr>
    <tr>
      <td><code>frog.image.width(image)</code>, <code>frog.image.height(image)</code></td>
      <td>Return image dimensions.</td>
      <td>Simple metadata accessors over the image buffer contract.</td>
    </tr>
    <tr>
      <td><code>frog.image.rgb_to_color(r, g, b)</code>, <code>frog.image.color_to_rgb(color)</code></td>
      <td>Convert between RGB channels and a compact color value.</td>
      <td>The exact color scalar representation must be standardized before promotion.</td>
    </tr>
  </tbody>
</table>

<h3><code>frog.picture</code> candidates</h3>

<table>
  <thead>
    <tr>
      <th>Candidate primitive</th>
      <th>Role</th>
    </tr>
  </thead>
  <tbody>
    <tr><td><code>frog.picture.empty()</code></td><td>Create an empty picture value.</td></tr>
    <tr><td><code>frog.picture.draw_point(picture, x, y, color)</code></td><td>Draw one point.</td></tr>
    <tr><td><code>frog.picture.draw_line(picture, x1, y1, x2, y2, color, thickness)</code></td><td>Draw one line segment.</td></tr>
    <tr><td><code>frog.picture.draw_multiple_lines(picture, points, color, thickness)</code></td><td>Draw a polyline or multiple connected line segments.</td></tr>
    <tr><td><code>frog.picture.draw_rect(picture, x, y, width, height, stroke, fill, thickness)</code></td><td>Draw a rectangle.</td></tr>
    <tr><td><code>frog.picture.draw_round_rect(picture, x, y, width, height, radius, stroke, fill, thickness)</code></td><td>Draw a rounded rectangle.</td></tr>
    <tr><td><code>frog.picture.draw_oval(picture, x, y, width, height, stroke, fill, thickness)</code></td><td>Draw an oval or ellipse.</td></tr>
    <tr><td><code>frog.picture.draw_arc(picture, x, y, width, height, start_angle, sweep_angle, color, thickness)</code></td><td>Draw an arc.</td></tr>
    <tr><td><code>frog.picture.draw_text(picture, x, y, text, style)</code></td><td>Draw text using an explicit style contract.</td></tr>
    <tr><td><code>frog.picture.draw_pixmap(picture, x, y, image)</code></td><td>Draw an image buffer into a picture.</td></tr>
    <tr><td><code>frog.picture.to_image(picture)</code></td><td>Rasterize a picture value to <code>frog.image.buffer_rgba8</code>.</td></tr>
  </tbody>
</table>

<p>
The exact picture value representation, clipping behavior, anti-aliasing
rules, text-font contract, flattened pixmap representation, and coordinate
origin are intentionally left for a later <code>frog.picture</code>
specification pass.
</p>

<h3><code>frog.plot</code> candidates</h3>

<table>
  <thead>
    <tr>
      <th>Candidate primitive</th>
      <th>Role</th>
    </tr>
  </thead>
  <tbody>
    <tr><td><code>frog.plot.waveform(data, options)</code></td><td>Render waveform data to a picture value.</td></tr>
    <tr><td><code>frog.plot.xy(x_values, y_values, options)</code></td><td>Render one XY plot to a picture value.</td></tr>
    <tr><td><code>frog.plot.multi_xy(series, options)</code></td><td>Render multiple XY series to a picture value.</td></tr>
    <tr><td><code>frog.plot.legend(series, options)</code></td><td>Render a plot legend component or picture value.</td></tr>
  </tbody>
</table>

<p>
Plot functions are rendering helpers, not advanced signal-processing or chart
history widgets. They may produce a picture value that a Picture indicator can
display, or they may later be paired with dedicated plot/graph widget
realizations.
</p>

<hr/>

<h2>Image Buffer Type</h2>

<p>
The initial portable output type is <code>frog.image.buffer_rgba8</code>.
Conceptually it is a row-major RGBA image buffer:
</p>

<pre><code>{
  "kind": "image_buffer",
  "pixel_format": "rgba8",
  "width": u32,
  "height": u32,
  "stride_bytes": u32,
  "pixels": array&lt;u8&gt;,
  "color_space": "srgb",
  "origin": "top_left"
}</code></pre>

<p>
Rules:
</p>

<ul>
  <li><code>pixels</code> stores rows from top to bottom.</li>
  <li>Each pixel has four bytes in red, green, blue, alpha order.</li>
  <li><code>stride_bytes</code> MUST be at least <code>width * 4</code>.</li>
  <li>For a tightly packed buffer, <code>stride_bytes = width * 4</code>.</li>
  <li>An empty fallback buffer uses <code>width = 0</code>, <code>height = 0</code>, <code>stride_bytes = 0</code>, and an empty <code>pixels</code> array.</li>
</ul>

<hr/>

<h2>Primitive: <code>frog.image.decode_file_rgba8</code></h2>

<p>
Decodes an image file identified by a path into a portable RGBA8 image buffer.
</p>

<ul>
  <li>input port: <code>path</code></li>
  <li>output ports: <code>image</code>, <code>success</code>, <code>error_code</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>path: path</code> or <code>string</code> according to the active source/profile posture</li>
  <li><code>image: frog.image.buffer_rgba8</code></li>
  <li><code>success: bool</code></li>
  <li><code>error_code: string</code></li>
</ul>

<p>
The first standardized format set is:
</p>

<ul>
  <li><code>image/png</code></li>
  <li><code>image/jpeg</code></li>
</ul>

<p>
Implementations MAY support additional formats, but a program that requires
additional formats should declare that requirement explicitly through the
appropriate profile, dependency, or package metadata.
</p>

<h3>Failure Behavior</h3>

<p>
If decoding fails, the primitive MUST return:
</p>

<ul>
  <li><code>success = false</code>,</li>
  <li>an empty <code>frog.image.buffer_rgba8</code>,</li>
  <li>a stable <code>error_code</code>.</li>
</ul>

<p>
Initial error codes:
</p>

<ul>
  <li><code>ok</code></li>
  <li><code>invalid_path</code></li>
  <li><code>not_found</code></li>
  <li><code>unsupported_format</code></li>
  <li><code>decode_failed</code></li>
</ul>

<hr/>

<h2>Implementation Dependency Posture</h2>

<p>
The public FROG specification defines the <code>frog.image</code> primitive
contract. It does not require a specific codec implementation.
</p>

<p>
A base IDE or runtime distribution MAY implement
<code>frog.image.decode_file_rgba8</code> with a small dependency such as
<code>stb_image</code>, and MAY later replace or complement that implementation
with codec-specific libraries such as PNG or JPEG libraries. Such choices are
implementation details as long as the public FROG contract remains satisfied.
</p>

<p>
This separation keeps the dependency replaceable:
</p>

<pre><code>FROG program contract: frog.image.decode_file_rgba8
Implementation choice: stb_image, libpng/libspng, libjpeg-turbo, or another codec stack</code></pre>

<p>
The implementation dependency belongs to the selected runtime, launcher, IDE
distribution, or compiled deployment closure. It does not change the FROG
primitive contract and it does not make image decoding a hidden responsibility
of the Picture widget.
</p>

<h2>Standard Provider and Deployment Closure</h2>

<p>
An implementation MAY realize <code>frog.image.decode_file_rgba8</code> through
a shared library, static library, compiled object, built-in provider, generated
backend artifact, or another equivalent provider mechanism.
</p>

<p>
The public requirement is that the provider satisfies the
<code>frog.image.decode_file_rgba8</code> contract and that the dependency remains
declared in the relevant FIR, lowering, manifest, package, profile, or deployment
closure. The public FROG specification does not require one mandatory runtime
loading mechanism.
</p>

<p>
This keeps deployment modular. A program that does not use
<code>frog.image</code> does not need an image provider. A compiled launcher or
self-contained executable that does use <code>frog.image.decode_file_rgba8</code>
may carry only the image provider needed by that program, together with the
program's other selected dependencies and assets.
</p>

<p>
Implementations MAY keep their provider source code, optimization strategy,
packaging layout, and runtime-loader architecture private, provided the public
primitive contract and declared dependency boundary remain intact.
</p>

<hr/>

<h2>Out of Scope for the Initial Image Library</h2>

<ul>
  <li>any candidate function listed above until it is explicitly promoted,</li>
  <li>camera acquisition,</li>
  <li>GPU image surfaces,</li>
  <li>OpenCV-style image processing,</li>
  <li>region-of-interest algorithms,</li>
  <li>color-management profiles beyond the initial <code>srgb</code> contract,</li>
  <li>streaming or tiled image decode.</li>
</ul>

<p>
Those capabilities may be standardized later or supplied by external FROG
libraries and native dependency packages.
</p>

<hr/>

<p align="center">
End of FROG Image Library Specification
</p>
