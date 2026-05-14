<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Enum Widget Example Reference</h1>

<p align="center">
  <strong>Current Example 08 Enum control and indicator proof surface</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Scope</h2>

<p>
The stabilized Enum slice is <code>Examples/08_enum_value_roundtrip</code>.
It proves one <code>frog.widgets.enum_control</code> and one
<code>frog.widgets.enum_indicator</code> over the <code>example08.mode</code> enum domain.
</p>

<pre><code>mode_input.value
  -&gt; mode_result.value
  -&gt; public result_mode
</code></pre>

<p>
The current enum items are <code>idle</code>, <code>run</code>, and <code>fault</code>,
with display text <code>Idle</code>, <code>Run</code>, and <code>Fault</code>.
</p>

<hr/>

<h2>FROG-Owned Instance Data</h2>

<ul>
  <li><code>Examples/08_enum_value_roundtrip/main.frog</code></li>
  <li>front panel canvas: <code>620 x 180</code> panel pixels</li>
  <li>control widget: <code>mode_input</code>, <code>frog.widgets.enum_control</code></li>
  <li>indicator widget: <code>mode_result</code>, <code>frog.widgets.enum_indicator</code></li>
  <li>widget layout, enum item vocabulary, caption placement, selector visibility, dropdown styling, and binding live in <code>.frog</code></li>
</ul>

<p>
Current instance-level properties proven by the example include:
</p>

<ul>
  <li><code>items[].id</code>, <code>items[].text</code>, <code>items[].numeric_value</code>, and <code>items[].enabled</code></li>
  <li><code>display.selector_visible</code> true on the control and false on the indicator</li>
  <li><code>style.value_face.*</code> and <code>style.value_display.*</code></li>
  <li><code>style.selector_face.*</code> including hover fill, border, symbol color, symbol size, radius, and border width</li>
  <li><code>style.dropdown.*</code> including option fill, text, hover, selected, font, padding, and height</li>
  <li><code>style.scale.reference_width</code> and <code>style.scale.reference_height</code> for scalable geometry</li>
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
      <td>Place labels from the source instance and keep them aligned with the enum field.</td>
    </tr>
    <tr>
      <td>Enum domain</td>
      <td><code>items[].id</code>, <code>items[].text</code>, <code>items[].numeric_value</code>, <code>items[].enabled</code></td>
      <td>Preserve item order, selected value, enabled state, and public id/numeric mapping.</td>
    </tr>
    <tr>
      <td>Value field</td>
      <td><code>style.value_face.*</code>, <code>style.value_display.*</code></td>
      <td>Keep the selected text vertically centered and scaled from the declared reference geometry.</td>
    </tr>
    <tr>
      <td>Selector button</td>
      <td><code>display.selector_visible</code>, <code>style.selector_face.*</code></td>
      <td>Render the button as a published SVG selector part, with normal and hover states from source properties.</td>
    </tr>
    <tr>
      <td>Dropdown</td>
      <td><code>style.dropdown.*</code>, <code>style.dropdown.option.*</code></td>
      <td>Use the declared colors, borders, fonts, option height, hover color, and selected color; dropdown width follows the value field.</td>
    </tr>
    <tr>
      <td>Scalable sizing</td>
      <td><code>style.scale.reference_width</code>, <code>style.scale.reference_height</code>, <code>*_mode = scale_with_widget</code></td>
      <td>Scale offsets, padding, borders, and selector symbol dimensions predictably when the IDE resizes the widget.</td>
    </tr>
  </tbody>
</table>

<p>
The Default Enum SVG currently exposes these public parts:
<code>caption</code>, <code>digital_display</code>, <code>focus_ring</code>, <code>frame</code>,
<code>increment_down</code>, <code>increment_up</code>, <code>label</code>, <code>root</code>,
<code>selector_arrow</code>, <code>selector_face</code>, <code>text_overflow_marker</code>,
<code>value_display</code>, and <code>value_face</code>.
</p>

<hr/>

<h2>WFROG-Owned Realization Data</h2>

<p>
The example package is:
</p>

<pre><code>Examples/08_enum_value_roundtrip/ui/enum_panel.wfrog</code></pre>

<ul>
  <li>Default package: <code>Libraries/Realizations/Default/enum.default.wfrog</code></li>
  <li>SVG asset: <code>Libraries/Realizations/Default/assets/enum/templates/enum_rectangular_ring.svg</code></li>
  <li>asset id consumed by runtimes: <code>enum_rectangular_ring_svg</code></li>
</ul>

<hr/>

<h2>Runtime Expectations</h2>

<ul>
  <li>The selector button is a published part of the Enum realization, not a runtime-private decoration.</li>
  <li>The dropdown width aligns with the value display, not with the selector button area.</li>
  <li>Normal, hover, and selected dropdown colors are instance-configurable through <code>.frog</code>.</li>
  <li>The value text baseline and vertical offset are driven by instance properties.</li>
  <li>The runtime must reject fallback markers such as <code>enum-card</code> and hidden hand-built select shells.</li>
</ul>

<hr/>

<h2>Native Kernel Posture</h2>

<ul>
  <li>native manifest: <code>Implementations/Reference/LLVM/examples/08_enum_value_roundtrip/native_kernel_manifest.json</code></li>
  <li>LLVM kernel: <code>Implementations/Reference/LLVM/examples/08_enum_value_roundtrip/kernel.ll</code></li>
  <li>native ABI entry: <code>frog_example08_run</code></li>
  <li>runtime languages validated: C++, Python, Rust</li>
</ul>

<hr/>

<h2>Validation Notes</h2>

<ul>
  <li>The example must test at least one non-default item such as <code>fault</code>.</li>
  <li>The dropdown must preserve item order and selected item publication.</li>
  <li>The indicator must show the selected value without exposing an interactive selector.</li>
  <li>There must be no local duplicated Enum SVG under the example directory.</li>
</ul>
