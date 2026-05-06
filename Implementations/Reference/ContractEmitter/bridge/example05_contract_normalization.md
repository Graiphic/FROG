<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 05 Contract Normalization</h1>

<p align="center">
  <strong>Final normalized contract surface for the Example 05 reference runtime-family corridor</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Normalized Surface</h2>

<p>
The Example 05 contract now exposes:
</p>

<pre><code>example_id
artifact_refs
units[].public_io
units[].ui_bindings
units[].execution_kernel
units[].effects
units[].publications
</code></pre>

<hr/>

<h2>Removed Legacy Surface</h2>

<p>
The following compatibility fields are no longer published in the Example 05 contract:
</p>

<pre><code>units[].public_interface
units[].ui_binding
units[].state_model
units[].execution_model
units[].property_writes
units[].public_output_publication
units[].legacy_runtime_shape
</code></pre>

<hr/>

<h2>Runtime Status</h2>

<p>
Both the generic contract executor and the specialized Python runtime consume the normalized surface.
</p>
