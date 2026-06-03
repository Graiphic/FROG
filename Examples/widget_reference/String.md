<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">String Widget Example Reference</h1>

<p align="center">
  <strong>Current Example 07 String control and indicator proof surface</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Scope</h2>

<p>
The stabilized String slice is <code>Examples/07_string_value_roundtrip</code>.
It proves one <code>frog.widgets.string_control</code> and one
<code>frog.widgets.string_indicator</code> with the standard <code>hello world</code> roundtrip.
</p>

<pre><code>str_input.value
  -&gt; str_result.value
  -&gt; public result_text
</code></pre>

<hr/>

<h2>FROG-Owned Instance Data</h2>

<ul>
  <li><code>Examples/07_string_value_roundtrip/main.frog</code></li>
  <li>front panel canvas: <code>560 x 170</code> panel pixels</li>
  <li>control widget: <code>str_input</code>, <code>frog.widgets.string_control</code></li>
  <li>indicator widget: <code>str_result</code>, <code>frog.widgets.string_indicator</code></li>
  <li>widget layout, caption placement, text values, binding, and text-region styling live in <code>.frog</code></li>
</ul>

<p>
Current instance-level properties proven by the example include:
</p>

<ul>
  <li><code>caption.text</code>, <code>caption.anchor.x</code>, <code>caption.anchor.y</code>, and <code>caption.align.horizontal</code></li>
  <li><code>style.frame.fill_color = transparent</code>, <code>style.frame.border_color = transparent</code>, and <code>style.frame.border_width = 0px</code></li>
  <li><code>style.text_region.fill_color</code>, <code>fill_color.hover</code>, <code>border_color</code>, <code>border_color.hover</code>, and <code>border_width</code></li>
  <li><code>style.text.color</code>, <code>style.text.font_size</code>, and <code>style.text.font_weight</code></li>
  <li><code>placeholder.*</code> on the control</li>
  <li><code>interaction.enabled</code> and <code>interaction.read_only</code></li>
</ul>

<hr/>

<h2>Published Parts And Configurable Properties</h2>

<table>
  <thead>
    <tr>
      <th>Surface</th>
      <th>Current source properties</th>
      <th>Runtime obligation</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Caption</td>
      <td><code>caption.text</code>, <code>caption.anchor.x/y</code>, <code>caption.align.horizontal</code></td>
      <td>Position labels from instance anchors and keep them close enough to read as widget captions.</td>
    </tr>
    <tr>
      <td>Text region</td>
      <td><code>style.text_region.fill_color</code>, <code>fill_color.hover</code>, <code>border_color</code>, <code>border_color.hover</code>, <code>border_width</code></td>
      <td>Map the host input or output overlay to the SVG text region and keep hover styling instance-driven.</td>
    </tr>
    <tr>
      <td>Text glyphs</td>
      <td><code>style.text.color</code>, <code>style.text.font_size</code>, <code>style.text.font_weight</code></td>
      <td>Keep visible text aligned, legible, and inside the region for both control and indicator.</td>
    </tr>
    <tr>
      <td>Frame/focus posture</td>
      <td><code>style.frame.*</code></td>
      <td>Respect the accepted no-external-focus-ring posture for this example.</td>
    </tr>
    <tr>
      <td>Role behavior</td>
      <td><code>interaction.enabled</code>, <code>interaction.read_only</code></td>
      <td>Allow editing only on the control and keep the indicator read-only.</td>
    </tr>
  </tbody>
</table>

<p>
The Default String SVG currently exposes these public parts:
<code>caption</code>, <code>frame</code>, <code>label</code>, <code>placeholder</code>,
<code>root</code>, <code>text_region</code>, and <code>text_value</code>.
It intentionally does not expose a public <code>focus_ring</code>, validation marker,
overflow marker, caret, or selection surface.
</p>

<hr/>

<h2>WFROG-Owned Realization Data</h2>

<p>
The example package is:
</p>

<pre><code>Examples/07_string_value_roundtrip/ui/string_panel.wfrog</code></pre>

<ul>
  <li>Default package: <code>Libraries/Realizations/Default/string.default.wfrog</code></li>
  <li>SVG asset: <code>Libraries/Realizations/Default/assets/string/templates/string_rectangular.svg</code></li>
  <li>asset id consumed by runtimes: <code>string_rectangular_svg</code></li>
</ul>

<hr/>

<h2>Runtime Expectations</h2>

<ul>
  <li>The text field must be rendered from the Default String SVG public parts and anchors.</li>
  <li>Hover and visual state colors must come from the <code>.frog</code> instance properties.</li>
  <li>The accepted visible surface has no external focus ring for the String widget.</li>
  <li>The indicator is read-only and receives the diagram result.</li>
  <li>The runtime must reject fallback markers such as <code>string-card</code>.</li>
</ul>

<hr/>

<h2>Native Kernel Posture</h2>

<ul>
  <li>native manifest: <code>Implementations/Reference/LLVM/examples/07_string_value_roundtrip/native_kernel_manifest.json</code></li>
  <li>LLVM kernel: <code>Implementations/Reference/LLVM/examples/07_string_value_roundtrip/kernel.ll</code></li>
  <li>native ABI entry: <code>frog_example07_run</code></li>
  <li>runtime languages validated: C++, Python, Rust</li>
</ul>

<hr/>

<h2>Validation Notes</h2>

<ul>
  <li>The canonical test value is <code>hello world</code>.</li>
  <li>The visible text must remain aligned and contained inside the text region.</li>
  <li>The control and indicator must consume the same Default String SVG asset through the <code>.wfrog</code> asset route.</li>
  <li>There must be no local duplicated String SVG under the example directory.</li>
</ul>
