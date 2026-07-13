<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Numeric Widget Example Reference</h1>

<p align="center">
  <strong>Current Example 05 execution surface and Example 41 placement/IDE surface</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Scope</h2>

<p>
The stabilized executable numeric slice is
<code>Examples/05_bounded_ui_accumulator</code>. It proves one
<code>frog.widgets.numeric_control</code> and one
<code>frog.widgets.numeric_indicator</code> inside a bounded accumulator
corridor. The current placement and IDE-calibration witness is
<code>Examples/41_numeric_placement_ide_view</code>; it shows the 16px
placement grid, <code>placement_bounds</code> aura, right/left/hidden
increment-decrement postures, and the distinction between placement aura,
selection overlay, and <code>focus_ring</code>.
</p>

<pre><code>input_value : u16
  -&gt; five-iteration bounded loop
  -&gt; result : u16
  -&gt; numeric indicator value
</code></pre>

<p>
The current algorithm is a real bounded loop with explicit delay state, not a shortcut multiplication.
For input <code>3</code>, the public result is <code>15</code>.
</p>

<hr/>

<h2>FROG-Owned Instance Data</h2>

<p>
The <code>.frog</code> source owns the executable diagram and the front-panel instance data:
</p>

<ul>
  <li><code>Examples/05_bounded_ui_accumulator/main.frog</code></li>
  <li>front panel canvas: <code>500 x 170</code> panel pixels</li>
  <li>control widget: <code>ctrl_input</code>, <code>frog.widgets.numeric_control</code>, <code>u16</code></li>
  <li>indicator widget: <code>ind_result</code>, <code>frog.widgets.numeric_indicator</code>, <code>u16</code></li>
  <li>widget layout, caption placement, initial values, binding, and per-instance colors live in the <code>.frog</code> widget instances</li>
</ul>

<p>
Current instance-level properties proven by the example include:
</p>

<ul>
  <li><code>caption.text</code>, <code>caption.anchor.x</code>, <code>caption.anchor.y</code>, and <code>caption.align.horizontal</code></li>
  <li><code>data_type.representation = u16</code>, <code>data_type.named_numeric_size = U16</code>, and <code>representation.kind = uint16</code></li>
  <li><code>data_entry.minimum</code>, <code>data_entry.maximum</code>, and <code>data_entry.increment_step</code> on the control</li>
  <li><code>display.increment_buttons_visible</code> on the control and indicator</li>
  <li><code>layout.origin = placement_bounds.top_left</code> and <code>layout.bounds_ref = placement_bounds</code> when placement/aura behavior is being inspected</li>
  <li><code>style.increment_button.*</code> colors for normal and pressed states</li>
  <li><code>foreground_color</code> as the property-write surface used by the diagram</li>
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
      <td><code>caption.text</code>, <code>caption.anchor.x/y</code>, <code>caption.align.horizontal</code>, <code>style.caption.font_weight</code></td>
      <td>Place the visible caption from instance data, not from runtime constants.</td>
    </tr>
    <tr>
      <td>Value field</td>
      <td><code>representation.kind</code>, <code>value</code>, <code>foreground_color</code></td>
      <td>Bind the editable control and read-only indicator to the SVG value face/text parts.</td>
    </tr>
    <tr>
      <td>Increment buttons</td>
      <td><code>display.increment_buttons_visible</code>, <code>data_entry.increment_step</code>, <code>style.increment_button.*</code></td>
      <td>Use the published increment parts and expose normal/pressed states without recoding a separate widget shell.</td>
    </tr>
    <tr>
      <td>Placement aura</td>
      <td><code>layout.placement_bounds.*</code>, <code>layout.grid.*</code>, <code>canvas.grid.*</code> when an IDE or container declares grid inspection</td>
      <td>Use <code>placement_bounds</code> as the portable placement/containment rectangle. It is not focus, not a rendered frame, and not Array cell selection.</td>
    </tr>
    <tr>
      <td>Data limits</td>
      <td><code>data_entry.minimum</code>, <code>data_entry.maximum</code></td>
      <td>Keep runtime input inside the declared <code>u16</code> domain and surface overflow diagnostics.</td>
    </tr>
  </tbody>
</table>

<p>
The Default Numeric SVG currently exposes these public parts:
<code>root</code>, <code>placement_bounds</code>, <code>label</code>,
<code>caption</code>, <code>focus_ring</code>, <code>control_body</code>,
<code>indicator_body</code>, <code>value_face</code>, <code>text_value</code>,
<code>spinner</code>, <code>increment_up</code>, and
<code>increment_down</code>.
</p>

<p>
The compact Default Numeric SVG intentionally does not publish separate
<code>frame</code>, <code>radix_badge</code>, <code>radix_text</code>, or
<code>unit_label</code> parts. The visible compact border belongs to
<code>value_face</code>. Unit text for this compact posture is rendered inline
through <code>text_value</code>; radix/unit variants may publish additional
parts only when a future realization explicitly declares them.
</p>

<hr/>

<h2>WFROG-Owned Realization Data</h2>

<p>
The example package is:
</p>

<pre><code>Examples/05_bounded_ui_accumulator/ui/accumulator_panel.wfrog</code></pre>

<p>
It owns only the realization references, Default SVG asset references, and host requirements.
It explicitly does not own source semantics, front-panel widget layout, values, per-instance visual customization,
FIR/lowering/runtime behavior, or numeric widget class law.
</p>

<ul>
  <li>Default package: <code>Libraries/Realizations/Default/numeric.default.wfrog</code></li>
  <li>SVG asset: <code>Libraries/Realizations/Default/assets/numeric/templates/numeric_rectangular.svg</code></li>
  <li>asset id consumed by runtimes: <code>numeric_rectangular_svg</code></li>
</ul>

<hr/>

<h2>Runtime Expectations</h2>

<ul>
  <li>The runtime reads the <code>.frog</code> front-panel instances and the example <code>.wfrog</code> package.</li>
  <li>The visible UI must route the Default Numeric SVG through <code>/asset/numeric_rectangular_svg</code>.</li>
  <li>The runtime may map SVG public parts to host input overlays, but the geometry and style are driven by the published instance and realization data.</li>
  <li>Increment/decrement controls are part of the published numeric realization and must not be recreated as an unrelated runtime-private widget.</li>
  <li>Debug snapshot content must not be visible in the normal front panel.</li>
</ul>

<hr/>

<h2>Native Kernel Posture</h2>

<ul>
  <li>native manifest: <code>Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/native_kernel_manifest.json</code></li>
  <li>LLVM kernel: <code>Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/kernel.ll</code></li>
  <li>native ABI entry: <code>frog_example05_run</code></li>
  <li>runtime languages validated: C++, Python, Rust</li>
</ul>

<p>
The runtime consumes the manifest-declared native kernel artifact.
It does not consume LLVM directly as a runtime dependency.
</p>

<hr/>

<h2>Validation Notes</h2>

<ul>
  <li>No local duplicated numeric SVG should exist under <code>Examples/05_bounded_ui_accumulator/ui/assets</code>.</li>
  <li>The visible widget must carry the realization-state marker used by the runtime family.</li>
  <li>The C++ standard CLI, C++ native-kernel CLI, Python runtime, and Rust runtime must all produce the same result for equivalent inputs.</li>
  <li>Overflow must produce the published diagnostic: <code>final_state must remain in the u16 domain.</code></li>
</ul>

<p>
Studio editing may select the complete Numeric representation family:
<code>EXT</code>, <code>DBL</code>, <code>SGL</code>, <code>FXP</code>,
signed <code>I64/I32/I16/I8</code>, unsigned <code>U64/U32/U16/U8</code>,
and complex <code>CXT/CDB/CSG</code>. These are source-owned type declarations.
Their presence in a front-panel editor does not by itself claim that every type
has a validated executable lowering/native ABI corridor; each runtime corridor
must still be proven explicitly.
</p>

<h2>Reference Snapshots</h2>

<p>
No standalone public <code>reference/</code> package is currently published for
Example 05 or Example 41. Example 05 remains the executable Numeric baseline,
while Example 41 is the IDE/placement calibration witness. When a public
snapshot is published for either page, it must follow
<a href="../example_reference_snapshot_standard.md">the example reference snapshot standard</a>
and include the accepted screenshot, accepted public state, visual contract,
machine-readable contract, and artifact hash index.
</p>
