<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference Deriver</h1>

<p align="center">
  <strong>Non-normative reference derivation workspace for generating FIR from supported FROG source patterns</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This directory contains the current non-normative reference deriver for the FROG implementation workspace.
It exists to make the published source-to-FIR corridor executable and checkable for the bounded Examples 01-15 public reference runtime closure and the post-boundary Example 16 Picture source slice without claiming general compiler completeness.
</p>

<p>
The supported source-to-FIR targets cover the published numbered examples from <code>01</code> through <code>15</code>, plus the public Example <code>16_picture_logo_jpeg</code> source/FIR contract.
Rule selection is based on explicit source-pattern recognition rather than trusting <code>metadata.name</code> as semantic authority.
</p>

<hr/>

<h2>Systematic Coverage Status</h2>

<p>
The public FROG architecture requires the <code>.frog -&gt; FIR</code> boundary to
remain public and inspectable. A complete conforming implementation should be
able to derive a canonical FIR / Execution IR document from any accepted FROG
program for the supported specification version.
</p>

<p>
The Python deriver in this directory is not that complete implementation yet.
It is a bounded reference tool for the currently recognized source-pattern
families. It does not automatically derive FIR for arbitrary valid
<code>.frog</code> graphs, every future source construct, or every
<code>frog.xxx</code> library function. Unsupported source shapes must fail
explicitly instead of guessing a FIR shape.
</p>

<p>
This distinction is intentional:
</p>

<ul>
  <li><code>.frog -&gt; FIR</code> is a public specification boundary.</li>
  <li>This directory contains a non-production reference deriver for the current bounded corridor.</li>
  <li>Broader source syntax closure, general semantic validation, full library-call modeling, and complete source-family coverage remain follow-up standardization work.</li>
</ul>

<hr/>

<h2>Current Derivation Posture</h2>

<pre><code>canonical .frog source
  -&gt; source structure checks
  -&gt; supported source-pattern recognition
  -&gt; bounded source-to-FIR rule
  -&gt; published FIR artifact
</code></pre>

<p>
The deriver remains intentionally narrow.
It recognizes the current published source patterns and emits the current published FIR shapes.
It does not yet implement a general FROG derivation engine.
</p>

<hr/>

<h2>Rule Selection Boundary</h2>

<p>
The reference deriver attempts each registered bounded derivation rule against the source structure.
A source is accepted only when exactly one rule recognizes the source pattern.
</p>

<ul>
  <li>If no rule recognizes the source, derivation fails with diagnostics from each attempted rule.</li>
  <li>If more than one rule recognizes the source, derivation fails as ambiguous.</li>
  <li><code>metadata.name</code> may appear in diagnostics, but it is not the derivation authority.</li>
</ul>

<p>
This is a transitional step from example-specific derivation toward structural derivation:
</p>

<pre><code>previous posture:
  metadata.name / example_id
    -&gt; derivation function

current posture:
  source pattern
    -&gt; recognized bounded rule
    -&gt; FIR artifact

future posture:
  validated semantic structure
    -&gt; general derivation family
    -&gt; FIR artifact
</code></pre>

<hr/>

<h2>Commands</h2>

<h3>Generic source-to-FIR check</h3>

<pre><code>python Implementations/Reference/Deriver/derive_fir.py \
  --source Examples/01_pure_addition/main.frog \
  --expected Examples/01_pure_addition/main.fir.json \
  --check
</code></pre>

<h3>Example 05 legacy helper</h3>

<pre><code>python Implementations/Reference/Deriver/derive_example05_fir.py --check</code></pre>

<p>
The legacy helper remains available for focused Example 05 debugging.
The generic <code>derive_fir.py</code> entry point is the preferred path for supported examples.
</p>

<hr/>

<h2>Rule Family Documentation</h2>

<ul>
  <li><a href="./rules/Readme.md">Reference FIR derivation rule-family index</a></li>
  <li><a href="./rules/example01_fir_derivation.md">Example 01 FIR derivation rules</a></li>
  <li><a href="./rules/example02_fir_derivation.md">Example 02 FIR derivation rules</a></li>
  <li><a href="./rules/example03_fir_derivation.md">Example 03 FIR derivation rules</a></li>
  <li><a href="./rules/example04_fir_derivation.md">Example 04 FIR derivation rules</a></li>
  <li><a href="./rules/example05_fir_derivation.md">Example 05 FIR derivation rules</a></li>
  <li>Examples 06-10 are covered by the current bounded scalar widget derivation rules in <code>fir_deriver.py</code>.</li>
  <li>Examples 11-15 are covered by the current bounded Button mechanical-action derivation rules in <code>fir_deriver.py</code>.</li>
  <li>Example 16 is covered by the bounded Picture path-to-image derivation rule in <code>fir_deriver.py</code>.</li>
</ul>

<hr/>

<h2>Current Recognized Rule IDs</h2>

<ul>
  <li><code>pure_public_addition</code></li>
  <li><code>ui_value_roundtrip</code></li>
  <li><code>ui_property_write</code></li>
  <li><code>stateful_feedback_delay</code></li>
  <li><code>bounded_ui_accumulator</code></li>
  <li><code>boolean_value_roundtrip</code></li>
  <li><code>string_value_roundtrip</code></li>
  <li><code>enum_value_roundtrip</code></li>
  <li><code>path_value_roundtrip</code></li>
  <li><code>button_press_to_boolean</code></li>
  <li><code>button_switch_when_pressed</code></li>
  <li><code>button_switch_when_released</code></li>
  <li><code>button_latch_when_pressed</code></li>
  <li><code>button_latch_when_released</code></li>
  <li><code>button_latch_until_released</code></li>
  <li><code>picture_path_to_image</code></li>
</ul>

<hr/>

<h2>Current Family-to-FIR Mapping</h2>

<table>
  <thead>
    <tr>
      <th>Rule ID</th>
      <th>Family</th>
      <th>Derived FIR unit kind</th>
    </tr>
  </thead>
  <tbody>
    <tr><td><code>pure_public_addition</code></td><td>Pure public-interface arithmetic</td><td><code>pure_dataflow_arithmetic_unit</code></td></tr>
    <tr><td><code>ui_value_roundtrip</code></td><td>Natural widget-value dataflow</td><td><code>ui_value_roundtrip_unit</code></td></tr>
    <tr><td><code>ui_property_write</code></td><td>Object-style UI property effect</td><td><code>ui_property_write_unit</code></td></tr>
    <tr><td><code>stateful_feedback_delay</code></td><td>Explicit delay-backed state</td><td><code>stateful_feedback_delay_unit</code></td></tr>
    <tr><td><code>bounded_ui_accumulator</code></td><td>Bounded UI accumulator</td><td><code>bounded_stateful_ui_unit</code></td></tr>
    <tr><td><code>boolean_value_roundtrip</code></td><td>Boolean widget value roundtrip</td><td><code>boolean_value_roundtrip_ui_unit</code></td></tr>
    <tr><td><code>string_value_roundtrip</code></td><td>String widget value roundtrip</td><td><code>string_value_roundtrip_ui_unit</code></td></tr>
    <tr><td><code>enum_value_roundtrip</code></td><td>Enum widget value roundtrip</td><td><code>enum_value_roundtrip_ui_unit</code></td></tr>
    <tr><td><code>path_value_roundtrip</code></td><td>Path widget value roundtrip</td><td><code>path_value_roundtrip_ui_unit</code></td></tr>
    <tr><td><code>button_press_to_boolean</code></td><td>Button press-to-Boolean publication</td><td><code>button_press_to_boolean_ui_unit</code></td></tr>
    <tr><td><code>button_switch_when_pressed</code></td><td>Button switch mechanical action</td><td><code>button_switch_when_pressed_ui_unit</code></td></tr>
    <tr><td><code>button_switch_when_released</code></td><td>Button switch mechanical action</td><td><code>button_switch_when_released_ui_unit</code></td></tr>
    <tr><td><code>button_latch_when_pressed</code></td><td>Button latch mechanical action</td><td><code>button_latch_when_pressed_ui_unit</code></td></tr>
    <tr><td><code>button_latch_when_released</code></td><td>Button latch mechanical action</td><td><code>button_latch_when_released_ui_unit</code></td></tr>
    <tr><td><code>button_latch_until_released</code></td><td>Button latch mechanical action</td><td><code>button_latch_until_released_ui_unit</code></td></tr>
    <tr><td><code>picture_path_to_image</code></td><td>Path to <code>frog.image.decode_file_rgba8</code> to Picture publication</td><td><code>picture_path_to_image_ui_unit</code></td></tr>
  </tbody>
</table>

<hr/>

<h2>Boundary</h2>

<p>
This deriver is downstream from source and validation.
It does not own language semantics, widget law, runtime behavior, lowering, backend contracts, or LLVM behavior.
It is a reference implementation tool that protects the current published source-to-FIR corridor.
</p>
