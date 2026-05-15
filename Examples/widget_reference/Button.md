<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Button Widget Example Reference</h1>

<p align="center">
  <strong>Current Example 10 Button command and Boolean indicator proof surface</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Scope</h2>

<p>
The stabilized Button slice is <code>Examples/10_button_press_to_boolean</code>.
It proves one <code>frog.widgets.button</code> command control driving one
<code>frog.widgets.boolean_indicator</code>.
</p>

<pre><code>trigger_button.pressed
  -&gt; pressed_indicator.value
  -&gt; public pressed
</code></pre>

<p>
The example is intentionally small: the Button publishes a momentary pressed value, the
diagram copies that value, and the Boolean indicator displays the current pressed state.
This corridor was accepted on 2026-05-15 after browser-host visual inspection
of the Default Button rectangular SVG asset and the Default Boolean circular indicator.
</p>

<hr/>

<h2>FROG-Owned Instance Data</h2>

<p>
The <code>.frog</code> source owns the diagram and the front-panel instance data:
</p>

<ul>
  <li><code>Examples/10_button_press_to_boolean/main.frog</code></li>
  <li>front panel canvas: <code>420 x 170</code> panel pixels</li>
  <li>button control: <code>trigger_button</code>, <code>frog.widgets.button</code>, rectangular variant</li>
  <li>Boolean indicator: <code>pressed_indicator</code>, <code>frog.widgets.boolean_indicator</code>, circular variant</li>
  <li>widget layout, caption placement, initial values, state text, mechanical action, and per-instance visual states live in the <code>.frog</code> widget instances</li>
</ul>

<p>
Current instance-level properties proven by the example include:
</p>

<ul>
  <li><code>caption.text</code>, caption anchors, caption font size, and caption visibility</li>
  <li><code>state_text.true_text</code>, <code>state_text.false_text</code>, <code>state_text.visible</code>, and <code>state_text.style.*</code></li>
  <li><code>style.face.fill_color.*</code>, <code>style.face.border_color.*</code>, and <code>style.face.border_width</code></li>
  <li><code>style.pressed.inset</code> and hover/pressed visual states for the Button body</li>
  <li><code>style.inner.*</code> state colors for the read-only Boolean indicator</li>
  <li><code>interaction.enabled</code> for the Button and read-only indicator behavior</li>
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
      <td>Button caption</td>
      <td><code>caption.text</code>, <code>caption.anchor.x/y</code>, <code>caption.align.horizontal</code></td>
      <td>Place the external label from instance data and keep it visually separated from the Button body.</td>
    </tr>
    <tr>
      <td>Button face</td>
      <td><code>style.face.*</code>, <code>hover_*</code>, <code>pressed_*</code></td>
      <td>Apply normal, hover, and pressed states through the SVG-published <code>face</code> part rather than through a hardcoded HTML shell.</td>
    </tr>
    <tr>
      <td>Button state text</td>
      <td><code>state_text.true_text</code>, <code>state_text.false_text</code>, <code>state_text.style.*</code></td>
      <td>Show <code>ON</code> while pressed and <code>OFF</code> when released according to instance data.</td>
    </tr>
    <tr>
      <td>Host input overlay</td>
      <td><code>interaction.enabled</code>, <code>behavior.mechanical_action</code></td>
      <td>Align the browser input overlay to the SVG-published Button face bounds and drive a momentary pressed value.</td>
    </tr>
    <tr>
      <td>Boolean indicator</td>
      <td><code>style.inner.*</code>, <code>state_text.*</code></td>
      <td>Render the read-only pressed result through the Default Boolean circular realization.</td>
    </tr>
  </tbody>
</table>

<p>
The Default Button SVG template exposes these public parts:
<code>caption</code>, <code>face</code>, <code>focus_ring</code>, <code>frame</code>,
<code>label</code>, <code>root</code>, <code>state_face</code>, and <code>state_text</code>.
</p>

<hr/>

<h2>WFROG-Owned Realization Data</h2>

<p>
The example package is:
</p>

<pre><code>Examples/10_button_press_to_boolean/ui/button_panel.wfrog</code></pre>

<p>
It owns only the realization references, SVG asset references, and host requirements.
</p>

<ul>
  <li>Default Button package: <code>Libraries/Realizations/Default/button.default.wfrog</code></li>
  <li>Default Boolean package: <code>Libraries/Realizations/Default/boolean.default.wfrog</code></li>
  <li>button SVG asset: <code>Libraries/Realizations/Default/assets/button/templates/button_rectangular.svg</code></li>
  <li>indicator SVG asset: <code>Libraries/Realizations/Default/assets/boolean/templates/boolean_circular.svg</code></li>
  <li>asset ids consumed by runtimes: <code>button_rectangular_svg</code>, <code>boolean_circular_svg</code></li>
</ul>

<hr/>

<h2>Runtime Expectations</h2>

<ul>
  <li>Example 10 validates only <code>behavior.mechanical_action=switch_until_released</code>.</li>
  <li>The Button control must publish a momentary pressed value while the host pointer is down.</li>
  <li>The Boolean indicator is read-only and must return to false when the Button is released.</li>
  <li>Normal, hover, pressed, text, border, and indicator colors are instance-configurable.</li>
  <li>The visible skins must come from Default Button and Boolean SVG assets, not from a hardcoded HTML card.</li>
  <li>The runtime must reject fallback markers that replace the Button or Boolean bodies with local HTML/CSS widgets.</li>
  <li>The other Button mechanical actions are class-law vocabulary until each one has its own bounded runtime acceptance.</li>
</ul>

<hr/>

<h2>Native Kernel Posture</h2>

<ul>
  <li>native manifest: <code>Implementations/Reference/LLVM/examples/10_button_press_to_boolean/native_kernel_manifest.json</code></li>
  <li>LLVM kernel: <code>Implementations/Reference/LLVM/examples/10_button_press_to_boolean/kernel.ll</code></li>
  <li>native ABI entry: <code>frog_example10_run</code></li>
  <li>runtime languages validated: C++, Python, Rust</li>
</ul>

<hr/>

<h2>Validation Notes</h2>

<ul>
  <li>The Button press and release path must be tested in the browser-host UI.</li>
  <li>The visible UI must expose the expected asset routes for the Button and Boolean indicator.</li>
  <li>Button state text must switch between <code>OFF</code> and <code>ON</code> according to the pressed state.</li>
  <li>There must be no local duplicated Button SVG under the example directory.</li>
  <li>The accepted scope is bounded to Example 10 and must not be used to claim generalized runtime completeness.</li>
</ul>
