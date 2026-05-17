<h1 align="center">FROG Image Library Specification</h1>

<p align="center">
Definition of the minimal standard <strong>frog.image</strong> library for FROG<br/>
<em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Overview</h2>

<p>
This document defines the first minimal standard <code>frog.image</code>
primitive surface. Its role is to provide ordinary image-data functions that
can feed Picture widgets without making the Picture widget itself responsible
for file reading or codec behavior.
</p>

<p>
The initial library is intentionally small. It standardizes a portable image
buffer value shape and one path-based decode primitive suitable for basic
FROG programs and base IDE distributions.
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

<hr/>

<h2>Out of Scope for the Initial Image Library</h2>

<ul>
  <li>image mutation and drawing primitives,</li>
  <li>image encoding and file writing,</li>
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
