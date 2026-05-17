<h1>Example 16 - Picture Path Image Preview</h1>

<p>
Example 16 is a post-public-runtime-boundary Picture widget slice.
It starts with the repository FROG logo as the default path value, then lets a
conforming host choose another local PNG or JPEG image through a Path control.
The selected path is decoded by the standard <code>frog.image.decode_file_rgba8</code>
primitive and displayed by one read-only Picture indicator.
</p>

<p>
The example exists to validate the public source, library, and realization
shape for basic image display before any production runtime implementation is
accepted. The <code>.frog</code> source owns the Path control, Picture indicator,
layout, labels, initial path value, diagram wiring, and instance-level visual
properties. The <code>.wfrog</code> package owns only the Default Path/Picture
realization references, Default SVG asset references, and host capability
requirements.
</p>

<p>
The lowered unit is backed by the native manifest at
<code>Implementations/Reference/LLVM/examples/16_picture_logo_jpeg/native_kernel_manifest.json</code>
and the LLVM IR boundary at
<code>Implementations/Reference/LLVM/examples/16_picture_logo_jpeg/kernel.ll</code>.
That manifest declares the <code>frog.image</code> provider dependency consumed by
the runtime.
In the native execution corridor, the runtime invokes the manifest-declared
<code>frog_example16_run</code> entry point and that native artifact calls the
<code>frog.image</code> provider.
</p>

<pre><code>image_path.value
  -&gt; frog.image.decode_file_rgba8
  -&gt; preview_picture.value

frog.image.decode_file_rgba8.image       -&gt; public output preview_image
frog.image.decode_file_rgba8.success     -&gt; public output decode_success
frog.image.decode_file_rgba8.error_code  -&gt; public output decode_error_code</code></pre>

<p>
The visible Path and Picture widgets must consume their Default SVG realization
assets through <code>visual.asset_ref</code>. The Picture widget renders the decoded
RGBA8 image buffer inside the published <code>image_surface</code> part within the
<code>picture_region</code> surface. The image file is user data selected through
the Path value; it is not a widget skin and it does not replace the Default
Picture realization.
</p>

<p>
The Picture widget is not an image decoder. Image file decoding is an explicit
diagram primitive supplied by the <code>frog.image</code> standard library contract.
A conforming implementation may realize that primitive through a dynamic
provider, static link, compiled object, or equivalent deployment dependency,
but that provider is not the public semantic owner of the FROG program.
</p>

<p>
For browser-style hosts, a file picker cannot portably expose the user's
absolute local filesystem path. Such hosts may therefore import the selected
file into a host-accessible temporary path, update the Path control value to
that imported path, and then execute the same diagram contract:
<code>Path -&gt; frog.image.decode_file_rgba8 -&gt; Picture</code>. The host import
step is a front-panel convenience for path selection; it does not turn Picture
into a decoder and it does not replace the explicit <code>frog.image</code>
primitive.
</p>

<p>
This example therefore also exercises the desired dependency posture for later
deployment work: the program declares that it needs
<code>frog.image.decode_file_rgba8</code>, and a runtime, launcher, or compiled
deployment may satisfy that need by carrying only the selected provider required
by the program.
</p>

<p>
This example is not part of the public reference runtime closure. Public
runtime source in this repository remains bounded through Example 15. The
repository-visible source, FIR, lowering, LLVM/native boundary, manifest,
<code>.wfrog</code>, and Default SVG realization material remain public.
Runtime implementation and parity validation for Example 16 continue in
Graiphic's proprietary <code>Graiphic/FROG-Runtime</code> repository unless this
example is explicitly promoted later as public reference material.
</p>
