<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Widget Example Reference Notes</h1>

<p align="center">
  <strong>Implementation notes for the currently stabilized FROG widget examples</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
These notes describe the current repository-proven widget examples.
They are not the normative widget class law.
The class law remains under <code>Libraries/Widgets/</code>, the Default realization law remains under
<code>Libraries/Realizations/Default/</code>, and the numbered examples remain executable proof slices.
</p>

<p>
Each page records the practical contract that must stay true while the examples evolve:
</p>

<ul>
  <li>the <code>.frog</code> source owns diagram logic, front-panel instances, layout, binding, initial values, and instance-level visual customization;</li>
  <li>the example <code>.wfrog</code> package owns realization references, Default SVG asset references, and host capability requirements;</li>
  <li>the runtime consumes the <code>.frog</code> instance data, <code>.wfrog</code> package, Default SVG public parts, and native manifest when the slice is native-backed;</li>
  <li>no accepted widget example may rely on a hardcoded HTML/CSS fallback or duplicated local SVG skin.</li>
</ul>

<hr/>

<h2>Current Stabilized Widgets</h2>

<table>
  <thead>
    <tr>
      <th>Widget family</th>
      <th>Example</th>
      <th>Reference note</th>
      <th>Default realization package</th>
      <th>Default SVG asset</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Numeric</td>
      <td><code>05_bounded_ui_accumulator</code></td>
      <td><a href="./Numeric.md">Numeric.md</a></td>
      <td><code>Libraries/Realizations/Default/numeric.default.wfrog</code></td>
      <td><code>assets/numeric/templates/numeric_rectangular.svg</code></td>
    </tr>
    <tr>
      <td>Boolean</td>
      <td><code>06_boolean_value_roundtrip</code></td>
      <td><a href="./Boolean.md">Boolean.md</a></td>
      <td><code>Libraries/Realizations/Default/boolean.default.wfrog</code></td>
      <td><code>assets/boolean/templates/boolean_rectangular.svg</code>, <code>boolean_circular.svg</code></td>
    </tr>
    <tr>
      <td>String</td>
      <td><code>07_string_value_roundtrip</code></td>
      <td><a href="./String.md">String.md</a></td>
      <td><code>Libraries/Realizations/Default/string.default.wfrog</code></td>
      <td><code>assets/string/templates/string_rectangular.svg</code></td>
    </tr>
    <tr>
      <td>Enum</td>
      <td><code>08_enum_value_roundtrip</code></td>
      <td><a href="./Enum.md">Enum.md</a></td>
      <td><code>Libraries/Realizations/Default/enum.default.wfrog</code></td>
      <td><code>assets/enum/templates/enum_rectangular_ring.svg</code></td>
    </tr>
    <tr>
      <td>Path</td>
      <td><code>09_path_value_roundtrip</code></td>
      <td><a href="./Path.md">Path.md</a></td>
      <td><code>Libraries/Realizations/Default/path.default.wfrog</code></td>
      <td><code>assets/path/templates/path_rectangular_field.svg</code></td>
    </tr>
    <tr>
      <td>Button</td>
      <td><code>10_button_press_to_boolean</code></td>
      <td><a href="./Button.md">Button.md</a></td>
      <td><code>Libraries/Realizations/Default/button.default.wfrog</code></td>
      <td><code>assets/button/templates/button_rectangular.svg</code></td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Runtime Parity Snapshot</h2>

<p>
The stabilized widget examples are expected to run through the same reference-runtime family in every implemented
language. The runtime may be written in C++, Python, or Rust, but it must consume the same source-owned front-panel
instance data, the same <code>.wfrog</code> realization package, and the same native manifest truth when the slice is
native-backed.
</p>

<table>
  <thead>
    <tr>
      <th>Example</th>
      <th>Widget family</th>
      <th>C++</th>
      <th>Python</th>
      <th>Rust</th>
      <th>Execution posture</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>05</code></td>
      <td>Numeric</td>
      <td>validated</td>
      <td>validated</td>
      <td>validated</td>
      <td><code>LLVM native kernel manifest + .frog/.wfrog UI</code></td>
    </tr>
    <tr>
      <td><code>06</code></td>
      <td>Boolean</td>
      <td>validated</td>
      <td>validated</td>
      <td>validated</td>
      <td><code>LLVM native kernel manifest + .frog/.wfrog UI</code></td>
    </tr>
    <tr>
      <td><code>07</code></td>
      <td>String</td>
      <td>validated</td>
      <td>validated</td>
      <td>validated</td>
      <td><code>LLVM native kernel manifest + .frog/.wfrog UI</code></td>
    </tr>
    <tr>
      <td><code>08</code></td>
      <td>Enum</td>
      <td>validated</td>
      <td>validated</td>
      <td>validated</td>
      <td><code>LLVM native kernel manifest + .frog/.wfrog UI</code></td>
    </tr>
    <tr>
      <td><code>09</code></td>
      <td>Path</td>
      <td>validated</td>
      <td>validated</td>
      <td>validated</td>
      <td><code>LLVM native kernel manifest + .frog/.wfrog UI</code></td>
    </tr>
    <tr>
      <td><code>10</code></td>
      <td>Button</td>
      <td>validated</td>
      <td>validated</td>
      <td>validated</td>
      <td><code>LLVM native kernel manifest + .frog/.wfrog UI</code></td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Ownership Boundary</h2>

<table>
  <thead>
    <tr>
      <th>Concern</th>
      <th>Owner</th>
      <th>Validation expectation</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Diagram algorithm</td>
      <td><code>.frog</code></td>
      <td>Derives to FIR, lowers, emits native kernel, and produces the expected runtime output.</td>
    </tr>
    <tr>
      <td>Front-panel widget instances</td>
      <td><code>.frog</code></td>
      <td>Owns widget id, class, role, layout, binding, initial value, label/caption placement, and instance style.</td>
    </tr>
    <tr>
      <td>Published widget realization</td>
      <td><code>.wfrog</code></td>
      <td>Points to the Default realization package and declares which SVG asset ids the runtime must resolve.</td>
    </tr>
    <tr>
      <td>Visual geometry hooks</td>
      <td>Default SVG asset</td>
      <td>Exposes stable <code>data-frog-part</code>, <code>data-frog-anchor</code>, and <code>data-frog-bind</code> surfaces.</td>
    </tr>
    <tr>
      <td>Execution bridge</td>
      <td>Runtime + native manifest</td>
      <td>The runtime loads a manifest-declared ABI artifact and does not become LLVM-only.</td>
    </tr>
    <tr>
      <td>Visible UI host overlays</td>
      <td>Runtime</td>
      <td>May provide host input overlays, but must align them to published SVG parts and source-owned instance data.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Validation Gate</h2>

<p>
Every widget example in this directory must continue to satisfy
<a href="../widget_validation_checklist.md">the widget validation checklist</a>.
The important acceptance rule is:
</p>

<pre><code>.frog diagram + front-panel instances
  -&gt; FIR
  -&gt; lowering
  -&gt; LLVM kernel + native manifest
  -&gt; runtime C++ / Python / Rust
  -&gt; .wfrog realization package
  -&gt; Default SVG asset
  -&gt; faithful visible UI
</code></pre>

<p>
If the visible UI is approximated by a runtime-private widget shell instead of the published <code>.frog</code> /
<code>.wfrog</code> / SVG surfaces, the example is not accepted.
</p>
