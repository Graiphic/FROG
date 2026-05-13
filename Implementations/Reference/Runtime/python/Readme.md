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
The Python runtime currently carries the specialized Examples 05, 06, 07, and 08 runtime-family consumers.
It validates the backend contracts and <code>.wfrog</code> front-panel packages, executes the bounded accumulator, Boolean roundtrip, and String roundtrip paths, applies widget property effects, and produces the published runtime snapshots.
</p>

<p>
The Python browser UI is aligned with the shared browser-host fidelity surface for the current published runtime examples.
It consumes the <code>.frog</code> front-panel instance layout and widget coordinates, the <code>.wfrog</code> realization package, Default Numeric/Boolean/String/Enum SVG realizations, and the published realization anchors and value/text surfaces instead of local duplicated widget SVGs.
</p>

<hr/>

<h2>Normalized Contract Surface</h2>

<p>
The specialized runtime now consumes the normalized Example 05, Example 06, Example 07, and Example 08 contract surfaces:
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

<pre><code>python Implementations/Reference/Runtime/python/cli.py run 3
python Implementations/Reference/Runtime/python/cli.py run true --example 06
python Implementations/Reference/Runtime/python/cli.py run "hello world" --example 07
python Implementations/Reference/Runtime/check_python_native_kernel_bridge.py</code></pre>

<hr/>

<h2>Boundary</h2>

<p>
This runtime is non-normative.
It verifies repository-visible runtime closure for the current bounded example corridor.
The Python HTTP UI remains a first-class parity surface for Examples 05, 06, 07, and 08: it must consume <code>.frog</code> front-panel instances, <code>.wfrog</code> realization packages, and Default realization assets with the same no-fallback discipline as the C++ and Rust browser hosts.
</p>
