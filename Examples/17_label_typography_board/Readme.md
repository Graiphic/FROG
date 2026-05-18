<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 17 - Label Typography Board</h1>

<p align="center">
  <strong>.frog standalone Label support text + Default Label .wfrog realization asset + C++ private runtime</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This example introduces a simple standalone <code>frog.widgets.label</code> surface.
The front panel contains several Label widgets used as a title, subtitle, LabVIEW-like explanation rectangles, monospace annotations, and styled support notes.
</p>

<p>
The example intentionally keeps Label simple: a Label is support text on the front panel, not a scalar control or indicator.
Visible text is owned by <code>.frog</code> instance property <code>text.value</code>.
The <code>.wfrog</code> package references the Default Label realization and SVG asset.
</p>

<hr/>

<h2>Runtime Boundary</h2>

<p>
This is a repository-visible post-boundary widget progression example.
It does not add public runtime source beyond the current public reference runtime closure.
Runtime implementation for this example continues in <code>Graiphic/FROG-Runtime</code> unless explicitly promoted later.
</p>

<table>
  <tbody>
    <tr>
      <th>Runtime</th>
      <td>C++ Graiphic private runtime</td>
    </tr>
    <tr>
      <th>Execution</th>
      <td>LLVM/native manifest bool proof executor</td>
    </tr>
    <tr>
      <th>Compiler backend</th>
      <td>LLVM/native manifest</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Ownership</h2>

<ul>
  <li><code>.frog</code> owns Label widget instances, layout, text content, typography, backgrounds, frames, and alignment.</li>
  <li><code>.wfrog</code> owns Default Label realization references, asset references, and host capability declarations.</li>
  <li>The Default Label SVG owns reusable parts and anchors such as <code>background</code>, <code>frame</code>, <code>text_surface</code>, and <code>focus_ring</code>.</li>
  <li>The runtime consumes source-owned instance properties and published SVG parts; it does not treat baked SVG preview text as semantic content.</li>
</ul>

<hr/>

<h2>Files</h2>

<ul>
  <li><code>main.frog</code> - canonical source and front-panel Label instances.</li>
  <li><code>main.fir.json</code> - FIR projection of the support-text UI slice.</li>
  <li><code>main.lowering.json</code> - lowering intent with native manifest reference.</li>
  <li><code>ui/label_panel.wfrog</code> - Example 17 package referencing the Default Label realization and SVG asset.</li>
  <li><code>../../Implementations/Reference/LLVM/examples/17_label_typography_board/native_kernel_manifest.json</code> - native proof manifest.</li>
</ul>

<hr/>

<h2>Expected UI</h2>

<p>
The first screen should display a compact white front panel with a large title, a subtitle, two bordered explanation rectangles, and smaller typography samples.
Text must fit its Label rectangles, and all visible styling should come from <code>.frog</code> instance properties interpreted through the Default Label realization.
</p>
