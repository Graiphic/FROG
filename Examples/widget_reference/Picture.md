<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Picture Widget Example Reference</h1>

<p align="center">
  <strong>Current Path-to-Picture image preview proof surface</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Scope</h2>

<p>
The current Picture progression is demonstrated by
<code>Examples/16_picture_logo_jpeg</code>. It proves a LabVIEW-like image
display path where a source-owned Path control value is decoded by an explicit
diagram function and displayed by a Picture indicator.
</p>

<pre><code>image_path.value
  -&gt; frog.image.decode_file_rgba8
  -&gt; preview_picture.value
  -&gt; Picture indicator</code></pre>

<p>
The Picture widget displays image data. It does not own file selection, codec
selection, or image decoding. Those behaviors remain diagram/library behavior
and are represented in <code>.frog -&gt; FIR -&gt; lowering -&gt; native artifact</code>.
</p>

<hr/>

<h2>FROG-Owned Instance Data</h2>

<ul>
  <li>Path control value, label, layout, and file-picker posture.</li>
  <li>Picture indicator value, layout, frame, background, viewport, scaling, and image-fit posture.</li>
  <li>Default image path for the bundled FROG logo JPEG.</li>
  <li>Diagram call to the standard <code>frog.image</code> decode surface.</li>
  <li>Bindings that publish the decoded image buffer to the Picture indicator.</li>
</ul>

<hr/>

<h2>Published Parts And Configurable Properties</h2>

<table>
  <thead>
    <tr>
      <th>Surface</th>
      <th>Source or realization data</th>
      <th>Runtime obligation</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Picture frame</td>
      <td><code>style.frame.*</code></td>
      <td>Render the frame through the Default Picture <code>frame</code> part.</td>
    </tr>
    <tr>
      <td>Image region</td>
      <td><code>style.picture_region.*</code>, <code>display.*</code></td>
      <td>Align image drawing to <code>picture_region</code> and <code>image_surface</code>.</td>
    </tr>
    <tr>
      <td>Image value</td>
      <td><code>image.width</code>, <code>image.height</code>, <code>image.pixel_format</code>, <code>image.data</code></td>
      <td>Display the decoded image payload without making the host canvas semantic storage.</td>
    </tr>
    <tr>
      <td>Overlays</td>
      <td><code>overlay_region</code>, <code>roi_layer</code>, <code>crosshair_layer</code></td>
      <td>Reserve public parts for future overlays without making them mandatory in this example.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>WFROG-Owned Realization Data</h2>

<ul>
  <li>Example package: <code>Examples/16_picture_logo_jpeg/ui/picture_panel.wfrog</code></li>
  <li>Default Picture package: <code>Libraries/Realizations/Default/picture.default.wfrog</code></li>
  <li>Default Picture SVG: <code>Libraries/Realizations/Default/assets/picture/templates/picture_rectangular.svg</code></li>
  <li>Default Path package and SVG are also consumed because the example uses a Path command widget.</li>
</ul>

<p>
The bundled JPEG under <code>Examples/16_picture_logo_jpeg/assets/frog_logo.jpg</code>
is example input data. It is not a duplicated Picture skin and it does not
replace the Default Picture realization.
</p>

<hr/>

<h2>Runtime Expectations</h2>

<ul>
  <li>The runtime consumes the public <code>.frog</code>, <code>.wfrog</code>, Default SVG assets, native manifest, and ABI artifact.</li>
  <li>The native artifact invokes the manifest-declared image decode provider; the browser host does not become the image library.</li>
  <li>The visible image surface must align to the SVG-published Picture parts.</li>
  <li>Changing the Path value patches command state. Executing the diagram decodes and publishes the Picture value.</li>
  <li>No HTML/CSS fallback Picture shell or example-local Picture SVG is valid when the Default realization exists.</li>
</ul>

<hr/>

<h2>Reference Snapshots</h2>

<p>
No accepted public reference snapshot is published yet for Example 16. When
published, it should follow <a href="../example_reference_snapshot_standard.md">the example reference snapshot standard</a>
and include the accepted screenshot, state JSON, visual contract, and artifact
index under <code>Examples/16_picture_logo_jpeg/reference/</code>.
</p>
