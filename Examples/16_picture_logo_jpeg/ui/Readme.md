<h1>Example 16 UI Package</h1>

<p>
The UI package for Example 16 references the Default Path and Default Picture
realizations and their canonical SVG assets. The <code>.frog</code> source owns
the Path control, Picture indicator, front-panel layout, initial path value,
caption placement, viewport posture, and instance-level visual properties.
</p>

<p>
The <code>.wfrog</code> package declares the host capabilities required to bind
the selected path, call the standard <code>frog.image.decode_file_rgba8</code>
primitive, and render the decoded image buffer inside the Default Picture
<code>image_surface</code> part.
</p>

<p>
The <code>.wfrog</code> file does not own the image codec implementation and does
not turn the Picture widget into an implicit file reader. Runtime work for this
post-boundary example remains private unless the example is explicitly promoted
later as public reference material.
</p>
