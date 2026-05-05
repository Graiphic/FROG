<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 05 Contract Homogenization</h1>

<p align="center">
  <strong>Compatibility bridge from the legacy Example 05 runtime-family contract shape to the normalized reference contract surface</strong><br/>
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

<h2>Compatibility Surface</h2>

<p>
The following fields remain temporarily available for the specialized Example 05 runtime checker:
</p>

<pre><code>units[].public_interface
units[].ui_binding
units[].state_model
units[].execution_model
units[].property_writes
units[].public_output_publication
</code></pre>

<hr/>

<h2>Next Direction</h2>

<p>
The next cleanup step is to make the specialized Example 05 runtime consume the normalized fields directly, then remove the legacy compatibility fields once the checker no longer needs them.
</p>
