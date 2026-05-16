<h1>Example 16 - Picture Logo JPEG Display</h1>

<p>
Example 16 is a post-public-runtime-boundary Picture widget slice.
It displays a JPEG rendering of the repository FROG logo through one read-only
Picture indicator.
</p>

<p>
The example exists to validate the public source and realization shape for the
standard Picture widget before any production runtime implementation is
accepted. The <code>.frog</code> source owns the picture value, widget instance,
layout, caption placement, viewport posture, and instance-level visual
properties. The <code>.wfrog</code> package owns only the Default Picture
realization reference, Default SVG asset reference, and host capability
requirements.
</p>

<pre><code>frog_logo_jpeg_literal -&gt; logo_picture.value
frog_logo_jpeg_literal -&gt; public output logo_picture</code></pre>

<p>
The visible Picture widget must consume the Default Picture SVG realization
asset through <code>visual.asset_ref</code> and render the JPEG payload from the
source-owned picture value inside the published <code>image_surface</code>
part within the <code>picture_region</code> surface. The JPEG payload is data
displayed by the widget; it is not a widget skin and does not replace the
Default Picture realization.
</p>

<p>
This example is not part of the public reference runtime closure. Public
runtime source in this repository remains bounded through Example 15. Runtime
implementation and parity validation for Example 16 continue in Graiphic's
proprietary <code>Graiphic/FROG-Runtime</code> repository unless this example is
explicitly promoted later as public reference material.
</p>
