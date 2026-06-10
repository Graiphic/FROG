<h1>FROG Project Status</h1>

<p>
Published public repository state, current implementation boundaries, conformance coverage, examples, libraries, profiles, and project maturity notes.
</p>

<p>
This document preserves long-form material that was previously maintained in
the public root README. The root README now acts as a concise orientation page
and links here for detail.
</p>

<hr/>

<h2 id="published-repository-state">Published repository state</h2>

<p>
At the current published state, the repository contains the six core architectural specification families:
<code>Expression/</code>,
<code>Language/</code>,
<code>IR/</code>,
<code>Libraries/</code>,
<code>Profiles/</code>,
and <code>IDE/</code>.
These remain the primary ownership layers of the published language specification.
</p>

<p>
The repository also contains repository-level support areas and repository-level framing / governance layers:
</p>

<ul>
  <li><strong><code>Examples/</code></strong> — illustrative named source slices, executable example dossiers, and bounded compiler-corridor mirrors,</li>
  <li><strong><code>Conformance/</code></strong> — public accept / reject / preserve expectations for the published repository state,</li>
  <li><strong><code>Implementations/Reference/</code></strong> — a non-normative reference implementation workspace used to exercise disciplined execution paths,</li>
  <li><strong><code>Versioning/</code></strong> — centralized specification-governance and current-status reporting for the published specification corpus,</li>
  <li><strong><code>Strategy/</code></strong> — a non-normative strategic framing layer distinct from normative ownership,</li>
  <li><strong><code>Roadmap/</code></strong> — a non-normative closure-sequencing layer distinct from both strategy and specification.</li>
</ul>

<p>
The published numbered example surface now exposes a repository-visible progression under <code>Examples/01_*</code> through <code>Examples/20_*</code>.
Examples <code>01</code> through <code>15</code> define the current public reference runtime closure.
Examples beyond this boundary may remain repository-visible as specification-facing, widget-facing, conformance-facing, or design-progression examples, while Graiphic production runtime work for those later examples continues in the proprietary <code>Graiphic/FROG-Runtime</code> repository unless explicitly promoted later.
All example families are organized by the public
<a href="./Examples/example_dossier_standard.md">FROG Example Dossier Standard</a>,
so future widget, operator, structure, library, compiler, and conformance
examples expose their source, derived artifacts, ownership boundary, validation
posture, and limitations in a consistent way.
For accepted executable examples in the current validation line, that dossier
is expected to expose the full <code>.frog -&gt; FIR -&gt; lowering -&gt; LLVM IR -&gt;
native artifact + manifest -&gt; runtime host</code> corridor. Earlier stopping
points must be explicitly documented as documentation-only, negative
conformance, or exploratory posture rather than implied executable closure.
</p>

<table>
  <thead>
    <tr>
      <th>Example</th>
      <th>Short purpose</th>
      <th>Front-panel / widget surface</th>
      <th>Current validation posture</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>01_pure_addition</code></td>
      <td>Small pure arithmetic proof using public inputs and a public output.</td>
      <td>None; pure dataflow slice.</td>
      <td><code>.frog</code> &rarr; FIR &rarr; lowering &rarr; contract, with separate runtime and LLVM proof consumers.</td>
    </tr>
    <tr>
      <td><code>02_ui_value_roundtrip</code></td>
      <td>First natural widget-value roundtrip from a UI value to a result.</td>
      <td>Inline front-panel value participation.</td>
      <td><code>.frog</code> &rarr; FIR &rarr; lowering &rarr; contract, with separate runtime and LLVM proof consumers.</td>
    </tr>
    <tr>
      <td><code>03_ui_property_write</code></td>
      <td>Object-style widget reference plus <code>frog.ui.property_write</code>.</td>
      <td>Inline front-panel property-write participation.</td>
      <td><code>.frog</code> &rarr; FIR &rarr; lowering &rarr; contract, with separate runtime and LLVM proof consumers.</td>
    </tr>
    <tr>
      <td><code>04_stateful_feedback_delay</code></td>
      <td>Explicit state and valid feedback through <code>frog.core.delay</code>.</td>
      <td>None; stateful dataflow slice.</td>
      <td><code>.frog</code> &rarr; FIR &rarr; lowering &rarr; contract, with separate runtime and LLVM proof consumers.</td>
    </tr>
    <tr>
      <td><code>05_bounded_ui_accumulator</code></td>
      <td>Primary bounded LabVIEW-like accumulator corridor with state, UI binding, and native proof.</td>
      <td>Numeric control and indicator through the Default Numeric realization.</td>
      <td><code>.frog</code> &rarr; FIR &rarr; lowering &rarr; contract &rarr; LLVM kernel manifest &rarr; C++/Python/Rust runtime surfaces.</td>
    </tr>
    <tr>
      <td><code>06_boolean_value_roundtrip</code></td>
      <td>Boolean control-to-indicator roundtrip.</td>
      <td>Boolean control and indicator through the Default Boolean realization.</td>
      <td>Widget runtime slice with published FIR/lowering/contract, LLVM kernel manifest, and C++/Python/Rust runtime coverage.</td>
    </tr>
    <tr>
      <td><code>07_string_value_roundtrip</code></td>
      <td>Hello-world String control-to-indicator roundtrip.</td>
      <td>String control and indicator through the Default String realization.</td>
      <td>Widget runtime slice with published FIR/lowering/contract, LLVM kernel manifest, and C++/Python/Rust runtime coverage.</td>
    </tr>
    <tr>
      <td><code>08_enum_value_roundtrip</code></td>
      <td>Enum mode selection roundtrip.</td>
      <td>Enum control and indicator through the Default Enum realization.</td>
      <td>Widget runtime slice with published FIR/lowering/contract, LLVM kernel manifest, and C++/Python/Rust runtime coverage.</td>
    </tr>
    <tr>
      <td><code>09_path_value_roundtrip</code></td>
      <td>Path control-to-indicator roundtrip, including browse-button UI behavior.</td>
      <td>Path control and indicator through the Default Path realization.</td>
      <td>Widget runtime slice with published FIR/lowering/contract, LLVM kernel manifest, and runtime coverage evidence. Latest Default Path SVG skin review: <time datetime="2026-06-03">2026-06-03</time>; Graiphic runtime implementation code remains outside the public publication scope.</td>
    </tr>
    <tr>
      <td><code>10_button_press_to_boolean</code></td>
      <td>Button <code>switch_until_released</code> mechanical action.</td>
      <td>Button control and Boolean indicator through the Default Button and Boolean realizations.</td>
      <td>Current public reference runtime closure with published FIR/lowering/contract, LLVM kernel manifest, and C++/Python/Rust runtime coverage.</td>
    </tr>
    <tr>
      <td><code>11_button_switch_when_pressed</code></td>
      <td>Button <code>switch_when_pressed</code> mechanical action.</td>
      <td>Button control and Boolean indicator through the Default Button and Boolean realizations.</td>
      <td>Current public reference runtime closure with published FIR/lowering/contract, LLVM kernel manifest, and C++/Python/Rust runtime coverage.</td>
    </tr>
    <tr>
      <td><code>12_button_switch_when_released</code></td>
      <td>Button <code>switch_when_released</code> mechanical action.</td>
      <td>Button control and Boolean indicator through the Default Button and Boolean realizations.</td>
      <td>Current public reference runtime closure with published FIR/lowering/contract, LLVM kernel manifest, and C++/Python/Rust runtime coverage.</td>
    </tr>
    <tr>
      <td><code>13_button_latch_when_pressed</code></td>
      <td>Button <code>latch_when_pressed</code> mechanical action.</td>
      <td>Button control and Boolean indicator through the Default Button and Boolean realizations.</td>
      <td>Current public reference runtime closure with published FIR/lowering/contract, LLVM kernel manifest, and C++/Python/Rust runtime coverage.</td>
    </tr>
    <tr>
      <td><code>14_button_latch_when_released</code></td>
      <td>Button <code>latch_when_released</code> mechanical action.</td>
      <td>Button control and Boolean indicator through the Default Button and Boolean realizations.</td>
      <td>Current public reference runtime closure with published FIR/lowering/contract, LLVM kernel manifest, and C++/Python/Rust runtime coverage.</td>
    </tr>
    <tr>
      <td><code>15_button_latch_until_released</code></td>
      <td>Button <code>latch_until_released</code> mechanical action.</td>
      <td>Button control and Boolean indicator through the Default Button and Boolean realizations.</td>
      <td>Current public reference runtime closure with published FIR/lowering/contract, LLVM kernel manifest, and C++/Python/Rust runtime coverage.</td>
    </tr>
    <tr>
      <td><code>16_picture_logo_jpeg</code></td>
      <td>Path-driven Picture display using the standard <code>frog.image.decode_file_rgba8</code> contract.</td>
      <td>Path control and Picture indicator through the Default Path and Picture realizations.</td>
      <td>Post-boundary public example material; Graiphic private runtime validation continues in <code>Graiphic/FROG-Runtime</code>.</td>
    </tr>
    <tr>
      <td><code>17_label_typography_board</code></td>
      <td>Label typography and support-text surface.</td>
      <td>Label widgets through the Default Label realization.</td>
      <td>Post-boundary public example material; Graiphic private runtime validation continues in <code>Graiphic/FROG-Runtime</code>.</td>
    </tr>
    <tr>
      <td><code>18_decoration_board</code></td>
      <td>Flat box and line decorations for front-panel visual structure.</td>
      <td>Decoration widgets through the Default Decorations realizations.</td>
      <td>Post-boundary public example material; Graiphic private runtime validation continues in <code>Graiphic/FROG-Runtime</code>.</td>
    </tr>
    <tr>
      <td><code>19_subpanel_embedded_front_panel</code></td>
      <td>Subpanel host embedding a referenced child <code>.frog</code> front panel.</td>
      <td>Subpanel host through the Default Subpanel realization plus child front-panel widgets.</td>
      <td>Post-boundary public example material; Graiphic private runtime validation continues in <code>Graiphic/FROG-Runtime</code>.</td>
    </tr>
    <tr>
      <td><code>20_tab_page_selection</code></td>
      <td>Tab page selection with a movable page-label display value.</td>
      <td>Tab control and page-label display through the Default Tab realization.</td>
      <td>Post-boundary public example material; Graiphic private runtime validation continues in <code>Graiphic/FROG-Runtime</code>.</td>
    </tr>
    <tr>
      <td><code>21_ring_selection_roundtrip</code></td>
      <td>Ring numeric selected-value roundtrip.</td>
      <td>Ring control and read-only Ring indicator through the Default Ring realization.</td>
      <td>Post-boundary public example material; Graiphic private runtime validation continues in <code>Graiphic/FROG-Runtime</code>.</td>
    </tr>
    <tr>
      <td><code>22_listbox_selection_roundtrip</code></td>
      <td>Listbox numeric selected-value roundtrip.</td>
      <td>Listbox control and read-only Listbox indicator through the Default Listbox realization.</td>
      <td>Post-boundary public example material; Graiphic private runtime validation continues in <code>Graiphic/FROG-Runtime</code>.</td>
    </tr>
    <tr>
      <td><code>23_table_cell_selection_roundtrip</code></td>
      <td>Table numeric selected-cell roundtrip.</td>
      <td>Table control and read-only Table indicator through the Default Table realization.</td>
      <td>Post-boundary public example material; Graiphic private runtime validation continues in <code>Graiphic/FROG-Runtime</code>.</td>
    </tr>
    <tr>
      <td><code>24_tree_node_selection_roundtrip</code></td>
      <td>Tree numeric selected-node roundtrip.</td>
      <td>Tree control and read-only Tree indicator through the Default Tree realization.</td>
      <td>Post-boundary public example material; Graiphic private runtime validation continues in <code>Graiphic/FROG-Runtime</code>.</td>
    </tr>
  </tbody>
</table>

<p>
Widget examples are framed by the repository-visible
<a href="./Examples/widget_public_boundary.md">Widget Example Public Boundary</a>.
That public boundary keeps the <code>.frog</code> / <code>.wfrog</code> /
Default SVG / runtime-consumer split inspectable without publishing Graiphic's
internal widget acceptance recipe, visual QA procedure, or private runtime
roadmap.
</p>

<p>
The progression is intentionally staged:
</p>

<pre><code>01_pure_addition
  -> pure public-interface arithmetic

02_ui_value_roundtrip
  -> natural widget_value participation

03_ui_property_write
  -> widget_reference and frog.ui.property_write

04_stateful_feedback_delay
  -> explicit delay-backed feedback state

05_bounded_ui_accumulator
  -> combined UI + property write + bounded loop + explicit state corridor

06_boolean_value_roundtrip
  -> Boolean widget value roundtrip

07_string_value_roundtrip
  -> String widget value roundtrip

08_enum_value_roundtrip
  -> Enum widget value roundtrip

09_path_value_roundtrip
  -> Path widget value roundtrip

10_button_press_to_boolean
  -> Button switch_until_released state to Boolean indicator

11_button_switch_when_pressed
  -> Button switch_when_pressed state to Boolean indicator

12_button_switch_when_released
  -> Button switch_when_released state to Boolean indicator

13_button_latch_when_pressed
  -> Button latch_when_pressed state to Boolean indicator

14_button_latch_when_released
  -> Button latch_when_released state to Boolean indicator

15_button_latch_until_released
  -> Button latch_until_released state to Boolean indicator

16_picture_logo_jpeg
  -> Path to frog.image.decode_file_rgba8 to Picture indicator

17_label_typography_board
  -> Label typography, support text, and source-owned visual overrides

18_decoration_board
  -> Flat box plus vertical and horizontal line decorations

19_subpanel_embedded_front_panel
  -> Subpanel host for a referenced child .frog front panel

20_tab_page_selection
  -> Tab selected-page value and page-label display

21_ring_selection_roundtrip
  -> Ring numeric selected-value roundtrip

22_listbox_selection_roundtrip
  -> Listbox numeric selected-value roundtrip

23_table_cell_selection_roundtrip
  -> Table numeric selected-cell roundtrip

24_tree_node_selection_roundtrip
  -> Tree numeric selected-node roundtrip
</code></pre>

<p>
<code>Examples/05_bounded_ui_accumulator/</code> remains the primary applicative vertical-slice anchor because it combines source, front-panel package, FIR, lowering, backend contract, runtime-family acceptance, Python/Rust/C/C++ reference-consumer posture, and an LLVM-oriented native proof path.
Examples <code>01</code> through <code>04</code> are smaller executable proof slices used to keep individual concerns inspectable before they are combined in Example <code>05</code>.
Examples <code>06</code> through <code>15</code> are current widget-focused public reference runtime slices used to keep Boolean, String, Enum, Path, and Button front-panel behavior honest across C++, Python, and Rust.
Example <code>42</code> is a public Boolean IDE/calibration view for the Default Boolean <code>placement_bounds</code> aura, <code>state_face</code> surface, square/round skins, oval/rectangular resizing, and optional centered On/Off text.
Example <code>43</code> is the matching Boolean Array IDE/calibration view: square Boolean control cells, round Boolean indicator cells, and contained-widget <code>placement_bounds</code> cells on a visible review grid.
Runtime development for examples beyond Example <code>15</code> continues in Graiphic's proprietary runtime repository unless explicitly promoted later as public reference material.
</p>

<p>
Examples <code>16</code> through <code>24</code> are the current post-boundary
widget-facing progression examples. They remain public as source, library,
widget, realization, and conformance-facing material for Picture, Label,
Decoration, Subpanel, Tab, Ring, Listbox, Table, and Tree surfaces without adding public runtime
implementation beyond the Example <code>15</code> closure.
</p>

<p>
The correct current statement is therefore:
the repository materially exposes Examples <code>01</code> through <code>24</code>, while Example <code>05</code> remains the richest applicative UI/state/runtime/native reference corridor, Examples <code>06</code> through <code>15</code> define the current bounded public widget-runtime wave, and Examples <code>16</code> through <code>24</code> are public post-boundary widget/design progression material backed by Graiphic private runtime validation.
This does not claim full generalized symmetry across all future examples, all runtime families, or rendered-native front-panel closure.
</p>

<p>
The current reference checks are intentionally repository-visible:
</p>

<pre><code>python -m pip install pytest
python Implementations/Reference/check_reference_workspace.py --include-pytest

cmake -S Implementations/Reference/Runtime/cpp -B build/frog_runtime_cpp
cmake --build build/frog_runtime_cpp
ctest --test-dir build/frog_runtime_cpp --output-on-failure

cargo test --manifest-path Implementations/Reference/Runtime/rust/Cargo.toml
</code></pre>

<p>
Optional native LLVM checks require <code>clang</code>:
</p>

<pre><code>python Implementations/Reference/check_reference_workspace.py --include-llvm-build
python Implementations/Reference/check_reference_workspace.py --include-native-kernel-bridge
</code></pre>

<hr/>

<h2 id="project-status">Project status</h2>

<p>
FROG is currently under active design, cleanup, stabilization, and executable-corridor closure.
The repository already contains substantial material across canonical source representation, source-schema posture, language semantics, execution-facing IR architecture, intrinsic standardized primitive libraries, optional profile architecture, IDE architecture, governance surfaces, strategic framing, roadmap posture, examples, conformance material, and a non-normative reference implementation workspace.
</p>

<p>
At the current published state, the repository has reached a stronger closure milestone:
Examples <code>01</code> through <code>24</code> materially expose repository-visible source, FIR, lowering, backend-contract, widget, and LLVM-oriented proof material.
Examples <code>06</code> through <code>15</code> extend the current public reference workspace with validated widget-front-panel runtime slices for Boolean, String, Enum, Path, and Button.
Examples <code>16</code> through <code>24</code> extend the public post-boundary widget-facing progression for Picture, Label, Decoration, Subpanel, Tab, Ring, Listbox, Table, and Tree while runtime implementation continues privately.
</p>

<p>
The Example <code>05_bounded_ui_accumulator</code> slice remains the primary applicative vertical-slice anchor because it combines front-panel package participation, widget values, widget references, UI property writes, bounded iteration, explicit state, public output, runtime-family acceptance, and LLVM-native proof posture.
Examples <code>01</code> through <code>04</code> provide smaller executable anchors for isolated concerns.
Examples beyond Example <code>15</code> may remain visible as specification-facing, widget-facing, conformance-facing, or design-progression material.
They do not, by presence alone, expand the public runtime implementation maintained in this repository.
</p>

<p>
The current post-boundary set starts with <code>16_picture_logo_jpeg</code>,
which uses a Path control, the standard
<code>frog.image.decode_file_rgba8</code> primitive contract, and a Picture
indicator. It now continues through <code>24_tree_node_selection_roundtrip</code> with
Label, Decoration, Subpanel, Tab, Ring, Listbox, Table, and Tree widget-facing examples. Runtime support
for these examples continues in <code>Graiphic/FROG-Runtime</code> unless
deliberately promoted later.
</p>

<p>
At the same time, the repository has not yet reached:
</p>

<ul>
  <li>full generalized multi-runtime symmetry across all future serious examples,</li>
  <li>a generic contract executor that removes all example-specific runtime acceptance logic,</li>
  <li>a generic LLVM backend driven by lowered-unit kind rather than bounded example patterns,</li>
  <li>full native non-browser rendered front-panel closure,</li>
  <li>or final depth across all observability, debugging, and IDE-facing surfaces.</li>
</ul>

<p>
The current direction is therefore:
</p>

<ul>
  <li><strong>keep the Examples 01-15 executable and widget-runtime surfaces green as the bounded public reference runtime snapshot,</strong></li>
  <li><strong>keep the public reference runtime bounded, non-production, and conformance-oriented,</strong></li>
  <li><strong>continue runtime development for later examples in Graiphic's proprietary <code>Graiphic/FROG-Runtime</code> repository unless explicitly promoted,</strong></li>
  <li><strong>and allow Graiphic production runtime and IDE development to continue privately without redefining the public specification.</strong></li>
</ul>

<p>
The long-term ambition is to establish a durable open graphical programming ecosystem that can scale from experimentation to deeply integrated industrial deployment while remaining inspectable across the source, semantic, execution-facing, observability, and governance layers.
</p>

<p>
In the generative AI era, that ambition also carries a responsibility dimension:
FROG aims to help keep fast-produced software understandable, attributable, controllable, and evolvable by humans and organizations that remain responsible for real execution.
</p>

<hr/>
