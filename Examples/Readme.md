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

<p>
New examples should follow the
<a href="./example_dossier_standard.md">FROG Example Dossier Standard</a>.
That document defines the shared public organization for widget, operator,
structure, library, compiler, and conformance examples without publishing
Graiphic's internal validation recipe.
</p>

<hr/>

<h2>Public Runtime Boundary</h2>

<p>
Examples <code>01</code> through <code>15</code> define the current public reference runtime closure.
</p>

<p>
Examples beyond this boundary may remain repository-visible as specification-facing, widget-facing, conformance-facing, or design-progression examples.
They should not be interpreted as a commitment to publish Graiphic's production runtime implementation for those examples.
</p>

<p>
Production-grade runtime development for future FROG execution surfaces continues in Graiphic proprietary repositories, including <code>Graiphic/FROG-Runtime</code>, while preserving the public contracts required for independent implementations.
</p>

<h2>Browser-Hosted Inspection Surface</h2>

<p>
When a browser-hosted example is used for validation, the visible page should expose a collapsed-by-default source and execution artifact inspector.
Opening the inspector should show formatted in-page content, while raw routes remain available for direct checks.
The expected inspection surface includes <code>state.json</code>, the owning <code>main.frog</code>, the directly consumed example <code>.wfrog</code> package, and FIR/lowering artifacts when those artifacts exist.
</p>

<p>
This browser presentation is a validation host convention, not the FROG language runtime identity.
Other front-panel hosts may expose the same source, realization, FIR, lowering, manifest, and diagnostic contracts through an equivalent inspection surface.
</p>

<hr/>

<h2>Current Example Progression</h2>

<table>
  <thead>
    <tr>
      <th>Example</th>
      <th>Focus</th>
      <th>Starts at</th>
      <th>Current repository status</th>
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
    <tr>
      <td><code>08_enum_value_roundtrip</code></td>
      <td>Enum control/indicator mode roundtrip</td>
      <td><code>main.frog</code> + <code>ui/enum_panel.wfrog</code></td>
      <td><code>FIR → lowering → contract → LLVM native manifest → C++/Python/Rust runtime bridge → Default Enum realization</code></td>
    </tr>
    <tr>
      <td><code>09_path_value_roundtrip</code></td>
      <td>Path control/indicator value roundtrip</td>
      <td><code>main.frog</code> + <code>ui/path_panel.wfrog</code></td>
      <td><code>FIR → lowering → contract → LLVM native manifest → C++/Python/Rust runtime bridge → Default Path realization</code></td>
    </tr>
    <tr>
      <td><code>10_button_press_to_boolean</code></td>
      <td>Button <code>switch_until_released</code> mechanical action</td>
      <td><code>main.frog</code> + <code>ui/button_panel.wfrog</code></td>
      <td>Current public reference runtime closure: <code>FIR -> lowering -> contract -> LLVM native manifest -> C++/Python/Rust runtime bridge -> Default Button/Boolean realization</code></td>
    </tr>
    <tr>
      <td><code>11_button_switch_when_pressed</code></td>
      <td>Button <code>switch_when_pressed</code> mechanical action</td>
      <td><code>main.frog</code> + <code>ui/button_panel.wfrog</code></td>
      <td>Current public reference runtime closure: <code>FIR -> lowering -> contract -> LLVM native manifest -> C++/Python/Rust runtime bridge -> Default Button/Boolean realization</code></td>
    </tr>
    <tr>
      <td><code>12_button_switch_when_released</code></td>
      <td>Button <code>switch_when_released</code> mechanical action</td>
      <td><code>main.frog</code> + <code>ui/button_panel.wfrog</code></td>
      <td>Current public reference runtime closure: <code>FIR -> lowering -> contract -> LLVM native manifest -> C++/Python/Rust runtime bridge -> Default Button/Boolean realization</code></td>
    </tr>
    <tr>
      <td><code>13_button_latch_when_pressed</code></td>
      <td>Button <code>latch_when_pressed</code> mechanical action</td>
      <td><code>main.frog</code> + <code>ui/button_panel.wfrog</code></td>
      <td>Current public reference runtime closure: <code>FIR -> lowering -> contract -> LLVM native manifest -> C++/Python/Rust runtime bridge -> Default Button/Boolean realization</code></td>
    </tr>
    <tr>
      <td><code>14_button_latch_when_released</code></td>
      <td>Button <code>latch_when_released</code> mechanical action</td>
      <td><code>main.frog</code> + <code>ui/button_panel.wfrog</code></td>
      <td>Current public reference runtime closure: <code>FIR -> lowering -> contract -> LLVM native manifest -> C++/Python/Rust runtime bridge -> Default Button/Boolean realization</code></td>
    </tr>
    <tr>
      <td><code>15_button_latch_until_released</code></td>
      <td>Button <code>latch_until_released</code> mechanical action</td>
      <td><code>main.frog</code> + <code>ui/button_panel.wfrog</code></td>
      <td>Current public reference runtime closure: <code>FIR -> lowering -> contract -> LLVM native manifest -> C++/Python/Rust runtime bridge -> Default Button/Boolean realization</code></td>
    </tr>
    <tr>
      <td><code>16_picture_logo_jpeg</code></td>
      <td>Path-selected PNG/JPEG/BMP decoded by <code>frog.image.decode_file_rgba8</code> and displayed by a Picture indicator</td>
      <td><code>main.frog</code> + <code>ui/picture_panel.wfrog</code> + <code>assets/frog_logo.jpg</code></td>
      <td>Repository-visible post-boundary widget progression: public source/realization/example material only; runtime implementation continues in <code>Graiphic/FROG-Runtime</code> unless explicitly promoted later</td>
    </tr>
    <tr>
      <td><code>17_label_typography_board</code></td>
      <td>Standalone Label support widgets with source-owned typography, frames, backgrounds, and annotation text</td>
      <td><code>main.frog</code> + <code>ui/label_panel.wfrog</code></td>
      <td>Repository-visible post-boundary widget progression: public source/realization/example material only; runtime implementation continues in <code>Graiphic/FROG-Runtime</code> unless explicitly promoted later</td>
    </tr>
    <tr>
      <td><code>18_decoration_board</code></td>
      <td>Standalone Decoration support widgets with one source-owned flat box, one horizontal line, and one vertical line</td>
      <td><code>main.frog</code> + <code>ui/decoration_panel.wfrog</code></td>
      <td>Repository-visible post-boundary widget progression: public source/realization/example material only; runtime implementation continues in <code>Graiphic/FROG-Runtime</code> unless explicitly promoted later</td>
    </tr>
    <tr>
      <td><code>19_subpanel_embedded_front_panel</code></td>
      <td>Subpanel host region displaying a referenced child <code>.frog</code> front panel</td>
      <td><code>main.frog</code> + <code>embedded/embedded_status.frog</code> + <code>ui/subpanel_panel.wfrog</code></td>
      <td>Repository-visible post-boundary widget progression: public source/realization/example material only; runtime implementation continues in <code>Graiphic/FROG-Runtime</code> unless explicitly promoted later</td>
    </tr>
    <tr>
      <td><code>20_tab_page_selection</code></td>
      <td>Tab control page selection with wrapped page headers and a movable page label display</td>
      <td><code>main.frog</code> + <code>ui/tab_panel.wfrog</code></td>
      <td>Repository-visible post-boundary widget progression: public source/realization/example material only; runtime implementation continues in <code>Graiphic/FROG-Runtime</code> unless explicitly promoted later</td>
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
    |
    v
08  Enum .wfrog realization and native-kernel runtime bridge
    |
    v
09  Path .wfrog realization and native-kernel runtime bridge
    |
    v
10-15  Button mechanical actions in the public reference runtime closure
    |
    v
16   Path -> frog.image.decode_file_rgba8 -> Picture as a post-boundary widget progression example
    |
    v
17   standalone Label support text as a post-boundary widget progression example
    |
    v
18   standalone Decorations as a post-boundary widget progression example
    |
    v
19   Subpanel embedding another .frog front panel as a post-boundary widget progression example
    |
    v
20   Tab selected-page value and page region as a post-boundary widget progression example
    |
    v
21+  future examples continue runtime implementation in Graiphic/FROG-Runtime unless explicitly promoted
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
├── 07_string_value_roundtrip/
│   ├── Readme.md
│   ├── main.frog
│   ├── main.fir.json
│   ├── main.lowering.json
│   └── ui/
│       └── string_panel.wfrog
├── 08_enum_value_roundtrip/
│   ├── Readme.md
│   ├── main.frog
│   ├── main.fir.json
│   ├── main.lowering.json
│   └── ui/
│       └── enum_panel.wfrog
├── 09_path_value_roundtrip/
│   ├── Readme.md
│   ├── main.frog
│   ├── main.fir.json
│   ├── main.lowering.json
│   └── ui/
│       └── path_panel.wfrog
├── 10_button_press_to_boolean/
│   ├── Readme.md
│   ├── main.frog
│   ├── main.fir.json
│   ├── main.lowering.json
│   └── ui/
│       └── button_panel.wfrog
├── 11_button_switch_when_pressed/
│   └── same Button example shape
├── 12_button_switch_when_released/
│   └── same Button example shape
├── 13_button_latch_when_pressed/
│   └── same Button example shape
├── 14_button_latch_when_released/
│   └── same Button example shape
├── 15_button_latch_until_released/
│   └── same Button example shape
├── 16_picture_logo_jpeg/
    ├── Readme.md
    ├── main.frog
    ├── main.fir.json
    ├── main.lowering.json
    ├── assets/
    │   └── frog_logo.jpg
    └── ui/
        └── picture_panel.wfrog
├── 17_label_typography_board/
    ├── Readme.md
    ├── main.frog
    ├── main.fir.json
    ├── main.lowering.json
    └── ui/
        └── label_panel.wfrog
├── 18_decoration_board/
    ├── Readme.md
    ├── main.frog
    ├── main.fir.json
    ├── main.lowering.json
    └── ui/
        └── decoration_panel.wfrog
├── 19_subpanel_embedded_front_panel/
    ├── Readme.md
    ├── main.frog
    ├── main.fir.json
    ├── main.lowering.json
    ├── embedded/
    │   ├── embedded_status.frog
    │   └── ui/
    │       └── embedded_status_panel.wfrog
    └── ui/
        └── subpanel_panel.wfrog
└── 20_tab_page_selection/
    ├── Readme.md
    ├── main.frog
    ├── main.fir.json
    ├── main.lowering.json
    └── ui/
        └── tab_panel.wfrog
</code></pre>

<p>
Examples <code>10</code> through <code>15</code> use the same published shape:
each Button mechanical-action slice contains <code>Readme.md</code>,
<code>main.frog</code>, FIR/lowering artifacts, and
<code>ui/button_panel.wfrog</code> referencing the Default Button and Boolean
realization assets.
</p>

<p>
Example <code>16</code> starts the repository-visible post-boundary widget
progression. It contains public source, FIR/lowering shape, a
<code>.wfrog</code> package referencing the Default Path and Default Picture
realizations, and a default FROG logo image path decoded through the
<code>frog.image.decode_file_rgba8</code> standard library contract. It does not
add public runtime source beyond the Example 15 closure.
</p>

<p>
Example <code>17</code> continues the repository-visible post-boundary widget
progression with standalone Label support text. It contains public source,
FIR/lowering shape, a <code>.wfrog</code> package referencing the Default Label
realization, and a small native manifest proof output. It does not add public
runtime source beyond the Example 15 closure.
</p>

<p>
Example <code>18</code> continues the repository-visible post-boundary widget
progression with standalone Decoration support widgets. It contains public
source, FIR/lowering shape, a <code>.wfrog</code> package referencing the Default
Decorations realization, and a small native manifest proof output.
It does not add public runtime source beyond the Example 15 closure.
</p>

<p>
Example <code>19</code> continues the repository-visible post-boundary widget
progression with a Subpanel host region. It contains a parent public source,
a referenced child <code>.frog</code> front panel, FIR/lowering shape, a
<code>.wfrog</code> package referencing the Default Subpanel realization, and a
small native manifest proof output. The parent <code>.frog</code> owns the
Subpanel instance and child reference; the child <code>.frog</code> owns the
widgets displayed inside the Subpanel. It does not add public runtime source
beyond the Example 15 closure.
</p>

<p>
Example <code>20</code> continues the repository-visible post-boundary widget
progression with a Tab control. It contains public source, FIR/lowering shape,
a <code>.wfrog</code> package referencing the Default Tab realization, and a
native manifest string proof output. The <code>.frog</code>
owns page records, selected page value, layout, style, and visible page
content. Its host capability declarations include selected-page state patching
as a realization-facing browser interaction. It does not add public runtime
source beyond the Example 15 closure.
</p>

<hr/>

<h2>Minimum Local Setup</h2>

<p>
Run example validation commands from the repository root.
A fresh clone needs the following local tools before the numbered example corridor can be fully checked:
</p>

<table>
  <thead>
    <tr>
      <th>Tool</th>
      <th>Required for</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>Python</code> + <code>pip</code></td>
      <td>Reference workspace checks, FIR derivation, lowering, contracts, LLVM emission checks, and Python runtime tests.</td>
    </tr>
    <tr>
      <td><code>pytest</code></td>
      <td>Full reference test surface.</td>
    </tr>
    <tr>
      <td><code>CMake</code> + a C++ toolchain</td>
      <td>C++ runtime-family validation for the current widget examples.</td>
    </tr>
    <tr>
      <td><code>Cargo</code> / Rust toolchain</td>
      <td>Rust runtime-family validation for the current widget examples.</td>
    </tr>
    <tr>
      <td><code>clang</code></td>
      <td>Optional LLVM native build and native-kernel bridge checks.</td>
    </tr>
  </tbody>
</table>

<p>
Minimal setup for the Python-backed reference checks:
</p>

<pre><code>python -m pip install pytest
python Implementations/Reference/check_reference_workspace.py --include-pytest</code></pre>

<p>
Runtime-family validation adds the current C++ and Rust surfaces:
</p>

<pre><code>cmake -S Implementations/Reference/Runtime/cpp -B build/frog_runtime_cpp
cmake --build build/frog_runtime_cpp
ctest --test-dir build/frog_runtime_cpp --output-on-failure

cargo test --manifest-path Implementations/Reference/Runtime/rust/Cargo.toml</code></pre>

<p>
Widget examples are framed by the
<a href="./widget_public_boundary.md">Widget Example Public Boundary</a>.
That document records the public source / realization / runtime-consumer
ownership split without publishing Graiphic's internal widget acceptance recipe,
manual visual QA procedure, or private runtime roadmap.
</p>

<p>
All example families are organized by the
<a href="./example_dossier_standard.md">FROG Example Dossier Standard</a>.
Use the widget boundary document as the additional public boundary only when an
example has a front-panel widget or Default realization surface.
</p>

<p>
The currently stabilized widget example surfaces are inventoried in
<a href="./widget_reference/Readme.md">Widget Example Reference Notes</a>.
Those notes document the proven Numeric, Boolean, String, Enum, Path, and Button example surfaces,
including which properties are owned by <code>.frog</code>, which realization packages and SVG assets are consumed through
<code>.wfrog</code>, and which public runtime checks support the published example evidence.
</p>

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
