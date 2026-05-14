<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Path Widget Example Reference</h1>

<p align="center">
  <strong>Current Example 09 Path control and indicator proof surface</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Scope</h2>

<p>
The stabilized Path slice is <code>Examples/09_path_value_roundtrip</code>.
It proves <code>frog.widgets.path_control</code> and <code>frog.widgets.path_indicator</code>
using one normal path row and one preview row with the icon hidden.
</p>

<pre><code>path_input.value
  -&gt; path_result.value
  -&gt; public result_path
</code></pre>

<p>
The secondary no-icon row is a visual-configuration proof.
It demonstrates that hiding the icon can tighten the field without creating a dead empty gap.
</p>

<hr/>

<h2>FROG-Owned Instance Data</h2>

<ul>
  <li><code>Examples/09_path_value_roundtrip/main.frog</code></li>
  <li>front panel canvas: <code>700 x 300</code> panel pixels</li>
  <li>primary control widget: <code>path_input</code>, <code>frog.widgets.path_control</code></li>
  <li>primary indicator widget: <code>path_result</code>, <code>frog.widgets.path_indicator</code></li>
  <li>no-icon preview control: <code>path_input_no_icon</code></li>
  <li>no-icon preview indicator: <code>path_result_no_icon</code></li>
  <li>widget layout, caption placement, value text, icon visibility, browse-button visibility, and visual styling live in <code>.frog</code></li>
</ul>

<p>
Current instance-level properties proven by the example include:
</p>

<ul>
  <li><code>path.kind</code>, <code>path.display_value</code>, and <code>path.validation_state</code></li>
  <li><code>display.icon_visible</code>, <code>display.text_overflow_visible</code>, and <code>display.validation_marker_visible</code></li>
  <li><code>browse.enabled</code> and <code>browse.button_visible</code></li>
  <li><code>style.path_face.*</code> including fill, hover fill, border color, hover border color, and border width</li>
  <li><code>style.path_display.*</code> including color, font size, font weight, padding, and baseline offset</li>
  <li><code>style.path_icon.*</code> including folder colors and highlight color</li>
  <li><code>style.browse_button.*</code> including fill, hover fill, border color, border width, text color, and text size</li>
  <li><code>interaction.enabled</code> and <code>interaction.read_only</code></li>
</ul>

<hr/>

<h2>WFROG-Owned Realization Data</h2>

<p>
The example package is:
</p>

<pre><code>Examples/09_path_value_roundtrip/ui/path_panel.wfrog</code></pre>

<ul>
  <li>Default package: <code>Libraries/Realizations/Default/path.default.wfrog</code></li>
  <li>SVG asset: <code>Libraries/Realizations/Default/assets/path/templates/path_rectangular_field.svg</code></li>
  <li>asset id consumed by runtimes: <code>path_rectangular_field_svg</code></li>
  <li>host capability: <code>optional_file_picker</code></li>
</ul>

<hr/>

<h2>Runtime Expectations</h2>

<ul>
  <li>The folder icon is a labeled SVG part, not a runtime-hardcoded icon.</li>
  <li>The browse button is a labeled widget part and can be visible or hidden by instance properties.</li>
  <li>When the icon is hidden, the value text region must tighten so the field does not keep an unused icon gap.</li>
  <li>Hover effects must apply only over the active field or browse button, not over an invisible oversized frame.</li>
  <li>The file picker updates the relevant control instance only; independent controls must not cross-update each other.</li>
  <li>The runtime must reject fallback markers such as <code>path-card</code>.</li>
</ul>

<hr/>

<h2>Native Kernel Posture</h2>

<ul>
  <li>native manifest: <code>Implementations/Reference/LLVM/examples/09_path_value_roundtrip/native_kernel_manifest.json</code></li>
  <li>LLVM kernel: <code>Implementations/Reference/LLVM/examples/09_path_value_roundtrip/kernel.ll</code></li>
  <li>native ABI entry: <code>frog_example09_run</code></li>
  <li>runtime languages validated: C++, Python, Rust</li>
</ul>

<hr/>

<h2>Validation Notes</h2>

<ul>
  <li>The canonical test value is <code>C:/FROG/hello_world.txt</code>.</li>
  <li>The primary row and no-icon row must stay visually distinct and independently bound.</li>
  <li>The text baseline must keep the path legible and contained inside the field.</li>
  <li>There must be no local duplicated Path SVG under the example directory.</li>
</ul>
