<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Boolean Widget Example Reference</h1>

<p align="center">
  <strong>Current Example 06 Boolean control and indicator proof surface</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Scope</h2>

<p>
The stabilized Boolean slice is <code>Examples/06_boolean_value_roundtrip</code>.
It proves one <code>frog.widgets.boolean_control</code> and one
<code>frog.widgets.boolean_indicator</code>.
</p>

<pre><code>bool_input.value
  -&gt; bool_result.value
  -&gt; public result
</code></pre>

<p>
The example is intentionally small: one control value is copied to one indicator and one public output.
</p>

<hr/>

<h2>FROG-Owned Instance Data</h2>

<p>
The <code>.frog</code> source owns the diagram and the front-panel instance data:
</p>

<ul>
  <li><code>Examples/06_boolean_value_roundtrip/main.frog</code></li>
  <li>front panel canvas: <code>420 x 150</code> panel pixels</li>
  <li>control widget: <code>bool_input</code>, <code>frog.widgets.boolean_control</code>, rectangular variant</li>
  <li>indicator widget: <code>bool_result</code>, <code>frog.widgets.boolean_indicator</code>, circular variant</li>
  <li>widget layout, caption placement, initial values, state text, and per-instance visual states live in the <code>.frog</code> widget instances</li>
</ul>

<p>
Current instance-level properties proven by the example include:
</p>

<ul>
  <li><code>caption.text</code>, centered caption anchors, and caption visibility</li>
  <li><code>state_text.true_text</code>, <code>state_text.false_text</code>, and <code>state_text.visible</code></li>
  <li><code>style.inner.fill_color.false</code>, <code>true</code>, <code>hover_*</code>, and <code>pressed_*</code></li>
  <li><code>style.outer.border_color.* = transparent</code> for the accepted visual shape</li>
  <li><code>style.focus_ring.*</code> on the control</li>
  <li><code>style.inner.left</code>, <code>top</code>, <code>width</code>, and <code>height</code> for the circular indicator size</li>
  <li><code>interaction.enabled</code> and read-only indicator behavior</li>
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
      <td>Align labels from the instance anchors and keep them visually centered over their widgets.</td>
    </tr>
    <tr>
      <td>Boolean face</td>
      <td><code>style.outer.*</code>, <code>style.inner.*</code>, <code>style.frame.visible</code>, <code>style.frame.fill_color</code>, <code>style.frame.opacity</code></td>
      <td>Apply false/true visual states and optional frame posture through the SVG parts rather than through a hardcoded HTML switch.</td>
    </tr>
    <tr>
      <td>State text</td>
      <td><code>state_text.true_text</code>, <code>state_text.false_text</code>, <code>state_text.visible</code>, <code>state_text.style.*</code></td>
      <td>Show or hide state text according to instance data; the indicator may hide text while the LED color carries state.</td>
    </tr>
    <tr>
      <td>Interaction states</td>
      <td><code>hover_*</code>, <code>pressed_*</code>, <code>style.pressed.inset</code>, <code>style.transition.*</code></td>
      <td>Expose normal, hover, and pressed states without widening the hit target beyond the widget body.</td>
    </tr>
    <tr>
      <td>Indicator geometry</td>
      <td><code>style.inner.left/top/width/height</code></td>
      <td>Size the circular LED from instance properties so the IDE can later resize it predictably.</td>
    </tr>
  </tbody>
</table>

<p>
The Default Boolean SVGs currently expose these public parts:
<code>caption</code>, <code>focus_ring</code>, <code>frame</code>, <code>inner_face</code>,
<code>label</code>, <code>outer_face</code>, <code>root</code>, and <code>state_text</code>.
</p>

<hr/>

<h2>WFROG-Owned Realization Data</h2>

<p>
The example package is:
</p>

<pre><code>Examples/06_boolean_value_roundtrip/ui/boolean_panel.wfrog</code></pre>

<p>
It owns only the realization references, SVG asset references, and host requirements.
</p>

<ul>
  <li>Default package: <code>Libraries/Realizations/Default/boolean.default.wfrog</code></li>
  <li>control SVG asset: <code>Libraries/Realizations/Default/assets/boolean/templates/boolean_rectangular.svg</code></li>
  <li>indicator SVG asset: <code>Libraries/Realizations/Default/assets/boolean/templates/boolean_circular.svg</code></li>
  <li>asset ids consumed by runtimes: <code>boolean_rectangular_svg</code>, <code>boolean_circular_svg</code></li>
</ul>

<hr/>

<h2>Runtime Expectations</h2>

<ul>
  <li>The Boolean control must toggle through runtime interpretation of the <code>.frog</code> instance state map.</li>
  <li>The indicator is read-only and must not expose a hidden editable control.</li>
  <li>Normal, hover, and pressed colors are instance-configurable.</li>
  <li>The visible skin must come from the Default Boolean SVG assets, not from a hardcoded HTML card.</li>
  <li>The runtime must reject fallback markers such as <code>boolean-card</code> or a visible native checkbox used as the widget body.</li>
</ul>

<hr/>

<h2>Native Kernel Posture</h2>

<ul>
  <li>native manifest: <code>Implementations/Reference/LLVM/examples/06_boolean_value_roundtrip/native_kernel_manifest.json</code></li>
  <li>LLVM kernel: <code>Implementations/Reference/LLVM/examples/06_boolean_value_roundtrip/kernel.ll</code></li>
  <li>native ABI entry: <code>frog_example06_run</code></li>
  <li>runtime languages validated: C++, Python, Rust</li>
</ul>

<hr/>

<h2>Validation Notes</h2>

<ul>
  <li>Both <code>true</code> and <code>false</code> paths must be tested.</li>
  <li>The visible UI must expose the expected asset routes for rectangular control and circular indicator.</li>
  <li>State text may be visible on the control and hidden on the indicator according to <code>.frog</code> instance properties.</li>
  <li>There must be no local duplicated Boolean SVG under the example directory.</li>
</ul>
