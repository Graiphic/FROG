<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 41 - Numeric Placement IDE View</h1>

<p align="center">
  <strong>Default Numeric grid, placement aura, and compact posture inspection surface</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Overview</h2>

<p>
Reviewed: <code>2026-06-12</code>. This example is a C++ browser-host IDE view
for the Default Numeric widget. It is intentionally separated from
<code>Examples/05_bounded_ui_accumulator</code>: Example 05 remains the
execution slice, while Example 41 shows the 16px placement grid and the
source-declared Numeric placement aura behavior.
</p>

<p>
The view keeps the same compact Numeric proportions validated for execution:
value face, optional increment/decrement buttons on the right or left, hidden
buttons, and indicator body fill. It also lets the host render the selection
aura from <code>placement_bounds</code>. The aura is a runtime/IDE
visualization of source-declared geometry; it is not a new widget skin part, it
is not <code>control_body</code> / <code>indicator_body</code>, and it is not
<code>focus_ring</code>.
</p>

<hr/>

<h2>What This Example Tests</h2>

<ul>
  <li><code>canvas.grid.visible=true</code> with <code>canvas.grid.pitch=16</code>,</li>
  <li><code>canvas.presentation_mode=ide_view</code>,</li>
  <li><code>layout.origin=placement_bounds.top_left</code> and <code>layout.bounds_ref=placement_bounds</code>,</li>
  <li>selection body aura rendered from <code>placement_bounds</code>, with its top-left and top-right corners on the grid,</li>
  <li>right-side Numeric increment/decrement buttons,</li>
  <li>left-side Numeric increment/decrement buttons,</li>
  <li>hidden Numeric increment/decrement buttons,</li>
  <li>Numeric indicator face with distinct light-gray value fill,</li>
  <li>IDE selection aura aligned to <code>placement_bounds</code>,</li>
  <li>IDE label aura aligned to <code>caption</code>,</li>
  <li>the distinction between placement aura, IDE selection overlay, and widget <code>focus_ring</code>.</li>
</ul>

<hr/>

<h2>Published Files</h2>

<pre><code>Examples/41_numeric_placement_ide_view/
├── Readme.md
├── main.frog
├── main.fir.json
├── main.lowering.json
└── ui/
    ├── Readme.md
    └── numeric_ide_panel.wfrog

Implementations/Reference/ContractEmitter/examples/
└── 41_numeric_placement_ide_view.reference_host_runtime_ui_binding.contract.json
</code></pre>

<hr/>

<h2>Panel Posture</h2>

<pre><code>main_panel:
  width: 500
  height: 240
  canvas.grid.visible: true
  canvas.grid.pitch: 16
  canvas.grid.origin: { x: 0, y: 0 }
  canvas.grid.snap: placement_bounds
  canvas.grid.snap_edges: [placement_bounds.top_left, placement_bounds.top_right]
  canvas.grid.width_quantization: grid_pitch_multiple
  canvas.presentation_mode: ide_view
  canvas.selected_widget_id: ctrl_input_left

ctrl_input:
  buttons: right
  layout: x=32 y=32 width=96 height=32

ctrl_input_left:
  buttons: left
  layout: x=32 y=96 width=96 height=32

ctrl_input_plain:
  buttons: hidden
  layout: x=32 y=160 width=96 height=32
</code></pre>

<p>
Each matching indicator keeps the same <code>placement_bounds</code> size. The
visible body sits inside the aura with the default uniform 4px band. The
top-left and top-right corners of each main Numeric placement aura are grid
points; the default <code>96px</code> width is six <code>16px</code> grid
intervals, and manual horizontal resizing must move point-to-point on that
grid. The
selected widget is a source-level view default so visual validation can inspect
selection without clicking first; clicking any Numeric widget in the C++ host
moves the selection overlay.
</p>

<hr/>

<h2>Boundary</h2>

<p>
This example is an IDE/placement witness. It reuses the Slice 05 bounded
contract family only so the current C++ browser host can load the same Numeric
runtime surface. It does not define a new execution semantics, a new native
kernel, or a new widget class.
</p>

<hr/>

<h2>Reference Snapshot</h2>

<p>
No public <code>reference/</code> package is published yet for this IDE view.
When published, it must follow
<a href="../example_reference_snapshot_standard.md">the example reference snapshot standard</a>
and capture the accepted browser-host grid, placement aura, label aura,
right/left/hidden increment-decrement postures, indicator styling, and current
state JSON from the same accepted C++ host session.
</p>
