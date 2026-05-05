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
The Example 05 contract exposes:
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

<h2>Specialized Runtime Status</h2>

<p>
The specialized Python runtime now consumes only the normalized fields above.
A regression test creates a normalized-only copy of the Example 05 contract and verifies that the specialized runtime still produces the published snapshot.
</p>

<hr/>

<h2>Compatibility Surface</h2>

<p>
The following fields may remain temporarily available in the published contract for transition purposes:
</p>

<pre><code>units[].public_interface
units[].ui_binding
units[].state_model
units[].execution_model
units[].property_writes
units[].public_output_publication
</code></pre>

<p>
They are no longer required by the specialized Python runtime.
</p>

<hr/>

<h2>Next Direction</h2>

<p>
The next cleanup step is to remove the legacy compatibility fields from the Example 05 contract and update documentation/tests to treat the normalized surface as the single contract surface.
</p>
