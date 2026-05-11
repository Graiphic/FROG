<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference Python Runtime</h1>

<p align="center">
  <strong>Python runtime-family consumer for the non-normative FROG reference implementation</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Current Scope</h2>

<p>
The Python runtime currently carries the specialized Example 05 runtime-family consumer.
It validates the Example 05 backend contract and <code>.wfrog</code> front-panel package, executes the bounded accumulator path, applies widget property effects, and produces the published runtime snapshot.
</p>

<p>
The Python browser UI is aligned with the shared Example 05 browser-host fidelity surface.
It consumes the <code>.wfrog</code> panel pixel layout, widget coordinates, the Default Numeric SVG realization, and the published realization anchors such as <code>caption_text</code>, <code>value_face</code>, <code>text_value</code>, <code>increment_up</code>, and <code>increment_down</code>.
</p>

<hr/>

<h2>Normalized Contract Surface</h2>

<p>
The specialized runtime now consumes the normalized Example 05 contract surface:
</p>

<ul>
  <li><code>units[].public_io</code></li>
  <li><code>units[].ui_bindings</code></li>
  <li><code>units[].execution_kernel</code></li>
  <li><code>units[].effects</code></li>
  <li><code>units[].publications</code></li>
</ul>

<p>
It no longer reads the legacy compatibility fields:
</p>

<ul>
  <li><code>public_interface</code></li>
  <li><code>ui_binding</code></li>
  <li><code>state_model</code></li>
  <li><code>execution_model</code></li>
  <li><code>property_writes</code></li>
  <li><code>public_output_publication</code></li>
</ul>

<hr/>

<h2>Command</h2>

<pre><code>python Implementations/Reference/Runtime/python/execute_contract.py 3</code></pre>

<hr/>

<h2>Boundary</h2>

<p>
This runtime is non-normative.
It verifies repository-visible runtime closure for the current bounded example corridor.
The Python HTTP UI remains useful for behavioral and routing checks, while the current C++ browser-host runtime carries the stricter bounded <code>.wfrog</code> visual-fidelity posture for Example 05.
</p>
