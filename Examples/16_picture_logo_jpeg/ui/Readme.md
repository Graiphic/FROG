<h1>Example 16 UI</h1>

<p>
The UI package for Example 16 references the Default Picture realization and
its canonical SVG asset. The <code>.frog</code> source owns the Picture
indicator instance, layout, JPEG picture value, viewport posture, caption
placement, and per-instance style values.
</p>

<p>
The JPEG logo payload is displayed data, not an example-local widget skin. The
runtime must still resolve and consume the Default Picture SVG template through
the <code>.wfrog</code> package before rendering the source-owned image payload
inside the published Picture region.
</p>
