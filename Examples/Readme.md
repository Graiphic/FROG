<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Examples</h1>

<p align="center">
  <strong>Executable example dossiers for the published FROG specification</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Overview</h2>

<p>
This directory contains the repository-visible numbered example progression.
Examples are not semantic owners.
They are published slices used to inspect how source, FIR, lowering, contracts, runtimes, and native proof paths relate.
</p>

<hr/>

<h2>Current Example Progression</h2>

<table>
  <thead>
    <tr>
      <th>Example</th>
      <th>Focus</th>
      <th>Starts at</th>
      <th>Current published closure</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>01_pure_addition</code></td>
      <td>Pure public-interface arithmetic</td>
      <td><code>main.frog</code></td>
      <td><code>FIR → lowering → contract → runtime → LLVM</code></td>
    </tr>
    <tr>
      <td><code>02_ui_value_roundtrip</code></td>
      <td>Natural widget-value participation</td>
      <td><code>main.frog</code> with inline front panel</td>
      <td><code>FIR → lowering → contract → runtime → LLVM</code></td>
    </tr>
    <tr>
      <td><code>03_ui_property_write</code></td>
      <td>Object-style widget reference and property write</td>
      <td><code>main.frog</code> with inline front panel</td>
      <td><code>FIR → lowering → contract → runtime → LLVM</code></td>
    </tr>
    <tr>
      <td><code>04_stateful_feedback_delay</code></td>
      <td>Explicit state and feedback through <code>frog.core.delay</code></td>
      <td><code>main.frog</code></td>
      <td><code>FIR → lowering → contract → runtime → LLVM</code></td>
    </tr>
    <tr>
      <td><code>05_bounded_ui_accumulator</code></td>
      <td>Full bounded UI accumulator corridor</td>
      <td><code>main.frog</code> + <code>ui/accumulator_panel.wfrog</code></td>
      <td><code>FIR → lowering → contract → runtime acceptance → LLVM module/native proof</code></td>
    </tr>
    <tr>
      <td><code>06_boolean_value_roundtrip</code></td>
      <td>Boolean control/indicator value roundtrip</td>
      <td><code>main.frog</code> + <code>ui/boolean_panel.wfrog</code></td>
      <td><code>FIR → lowering → contract → LLVM native manifest → C++/Python/Rust runtime bridge → Default Boolean realization</code></td>
    </tr>
    <tr>
      <td><code>07_string_value_roundtrip</code></td>
      <td>String control/indicator hello-world roundtrip</td>
      <td><code>main.frog</code> + <code>ui/string_panel.wfrog</code></td>
      <td><code>FIR → lowering → contract → LLVM native manifest → C++/Python/Rust runtime bridge → Default String realization</code></td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Progression Logic</h2>

<pre><code>01  pure computation
    |
    v
02  widget_value natural UI value path
    |
    v
03  widget_reference + frog.ui.property_write object-style UI path
    |
    v
04  explicit state and valid feedback through delay
    |
    v
05  combined bounded UI accumulator corridor
    |
    v
06  Boolean .wfrog realization and native-kernel runtime bridge
    |
    v
07  String .wfrog realization and native-kernel runtime bridge
</code></pre>

<hr/>

<h2>Directory Shape</h2>

<pre><code>Examples/
├── 01_pure_addition/
│   ├── Readme.md
│   ├── main.frog
│   ├── main.fir.json
│   └── main.lowering.json
├── 02_ui_value_roundtrip/
│   ├── Readme.md
│   ├── main.frog
│   ├── main.fir.json
│   └── main.lowering.json
├── 03_ui_property_write/
│   ├── Readme.md
│   ├── main.frog
│   ├── main.fir.json
│   └── main.lowering.json
├── 04_stateful_feedback_delay/
│   ├── Readme.md
│   ├── main.frog
│   ├── main.fir.json
│   └── main.lowering.json
├── 05_bounded_ui_accumulator/
│   ├── Readme.md
│   ├── Freeze.md
│   ├── main.frog
│   ├── main.fir.json
│   ├── main.lowering.json
│   └── ui/
│       └── accumulator_panel.wfrog
├── 06_boolean_value_roundtrip/
│   ├── Readme.md
│   ├── main.frog
│   ├── main.fir.json
│   ├── main.lowering.json
│   └── ui/
│       └── boolean_panel.wfrog
└── 07_string_value_roundtrip/
    ├── Readme.md
    ├── main.frog
    ├── main.fir.json
    ├── main.lowering.json
    └── ui/
        └── string_panel.wfrog
</code></pre>

<hr/>

<h2>Reference Checks</h2>

<p>
The current reference workspace check across the numbered examples is:
</p>

<pre><code>python Implementations/Reference/check_reference_workspace.py</code></pre>

<p>
To include pytest coverage and optional native proof checks where the local toolchains are available:
</p>

<pre><code>python Implementations/Reference/check_reference_workspace.py --include-pytest
python Implementations/Reference/check_reference_workspace.py --include-native-kernel-bridge
python Implementations/Reference/check_reference_workspace.py --include-llvm-build</code></pre>

<hr/>

<h2>Boundary</h2>

<p>
Examples remain subordinate to the owning specification documents.
They illustrate and verify published corridors.
They do not define FROG semantics.
</p>
