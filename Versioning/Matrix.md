<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Version Matrix</h1>

<p align="center">
  <strong>Centralized current-status table for the published FROG specification corpus</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Executable Example Closure Matrix</h2>

<table>
  <thead>
    <tr>
      <th>Example</th>
      <th>Source</th>
      <th>FIR</th>
      <th>Lowering</th>
      <th>Contract</th>
      <th>Runtime acceptance</th>
      <th>LLVM proof</th>
      <th>Current note</th>
    </tr>
  </thead>
  <tbody>
    <tr><td>01 Pure Addition</td><td>Published</td><td>Published</td><td>Published</td><td>Published</td><td>Published</td><td>Published</td><td>Pure arithmetic corridor.</td></tr>
    <tr><td>02 UI Value Roundtrip</td><td>Published</td><td>Published</td><td>Published</td><td>Published</td><td>Published</td><td>Published</td><td>Natural <code>widget_value</code> corridor.</td></tr>
    <tr><td>03 UI Property Write</td><td>Published</td><td>Published</td><td>Published</td><td>Published</td><td>Published</td><td>Published</td><td>Published</td><td>Object-style <code>widget_reference</code> / <code>property_write</code> corridor.</td></tr>
    <tr><td>04 Stateful Feedback Delay</td><td>Published</td><td>Published</td><td>Published</td><td>Published</td><td>Published</td><td>Published</td><td>Published</td><td>Explicit delay-state corridor.</td></tr>
    <tr><td>05 Bounded UI Accumulator</td><td>Published</td><td>Published</td><td>Published</td><td>Published</td><td>Published</td><td>Published</td><td>Published</td><td>Primary applicative UI/state/runtime/native reference corridor.</td></tr>
  </tbody>
</table>

<hr/>

<h2>Widget Baseline Status</h2>

<table>
  <thead>
    <tr><th>Widget family</th><th>Status</th><th>Category</th><th>Role posture</th><th>Current note</th></tr>
  </thead>
  <tbody>
    <tr><td>Boolean</td><td>Published</td><td>core value</td><td>control / indicator</td><td>published baseline</td></tr>
    <tr><td>String</td><td>Published</td><td>core value</td><td>control / indicator</td><td>published baseline</td></tr>
    <tr><td>Button</td><td>Published</td><td>action</td><td>command</td><td>published baseline</td></tr>
    <tr><td>Numeric</td><td>Published</td><td>core value</td><td>control / indicator</td><td>published baseline</td></tr>
    <tr><td>Enum</td><td>Published</td><td>core value</td><td>control / indicator</td><td>published baseline</td></tr>
    <tr><td>Path</td><td>Published</td><td>core value</td><td>control / indicator</td><td>published baseline</td></tr>
    <tr><td>Picture</td><td>Published</td><td>visual data</td><td>control / indicator</td><td>published baseline, needs later qualitative deepening</td></tr>
    <tr><td>Waveform Chart</td><td>Published</td><td>visual data</td><td>indicator</td><td>published composite baseline</td></tr>
    <tr><td>Listbox</td><td>Published</td><td>selection / navigation</td><td>control / indicator</td><td>published baseline, needs later qualitative deepening</td></tr>
    <tr><td>Tab</td><td>Published</td><td>selection / navigation</td><td>control / indicator</td><td>published composite baseline, needs later qualitative deepening</td></tr>
    <tr><td>Tree</td><td>Published</td><td>selection / navigation</td><td>control / indicator</td><td>published baseline, needs later qualitative deepening</td></tr>
    <tr><td>Table</td><td>Published</td><td>structured data</td><td>control / indicator</td><td>published baseline, needs later qualitative deepening</td></tr>
    <tr><td>Array</td><td>Published</td><td>structured data</td><td>structured collection</td><td>published composite baseline</td></tr>
    <tr><td>Cluster</td><td>Published</td><td>structured data</td><td>structured aggregate</td><td>published composite baseline</td></tr>
    <tr><td>Label</td><td>Published</td><td>support</td><td>support</td><td>published baseline</td></tr>
    <tr><td>Frame</td><td>Published</td><td>support</td><td>support</td><td>published baseline</td></tr>
    <tr><td>Decorations</td><td>Published</td><td>support</td><td>decorative support</td><td>published baseline, intentionally non-value</td></tr>
    <tr><td>Splitter</td><td>Published</td><td>layout</td><td>layout</td><td>published baseline</td></tr>
    <tr><td>Panel/SubPanel</td><td>Published</td><td>layout / container</td><td>container</td><td>published baseline</td></tr>
  </tbody>
</table>

<hr/>

<h2>Summary</h2>

<p>
The current published repository state contains both a broad widget baseline and a repository-visible executable Examples 01–05 corridor.
The next priority is to keep the pipeline green, then convert the remaining pattern-specific runtime and LLVM code into more generic contract- and lowered-unit-kind-driven engines.
</p>
