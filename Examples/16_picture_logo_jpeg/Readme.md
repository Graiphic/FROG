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
A private Graiphic runtime implementation may use a small codec dependency such
as <code>stb_image</code> behind that contract, but that dependency is not the public
semantic owner of the FROG program.
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
This example is not part of the public reference runtime closure. Public
runtime source in this repository remains bounded through Example 15. Runtime
implementation and parity validation for Example 16 continue in Graiphic's
proprietary <code>Graiphic/FROG-Runtime</code> repository unless this example is
explicitly promoted later as public reference material.
</p>
