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
It exists to make the published source-to-FIR corridor executable and checkable for the bounded Examples 01–05 surface without claiming general compiler completeness.
</p>

<p>
The supported source-to-FIR targets cover the published numbered examples from <code>01</code> through <code>05</code>.
Rule selection is now based on explicit source-pattern recognition rather than trusting <code>metadata.name</code> as semantic authority.
</p>

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
The generic <code>derive_fir.py</code> entry point is the preferred path for Examples 01–05.
</p>

<hr/>

<h2>Supported Rule Files</h2>

<ul>
  <li><a href="./rules/example01_fir_derivation.md">Example 01 FIR derivation rules</a></li>
  <li><a href="./rules/example02_fir_derivation.md">Example 02 FIR derivation rules</a></li>
  <li><a href="./rules/example03_fir_derivation.md">Example 03 FIR derivation rules</a></li>
  <li><a href="./rules/example04_fir_derivation.md">Example 04 FIR derivation rules</a></li>
  <li><a href="./rules/example05_fir_derivation.md">Example 05 FIR derivation rules</a></li>
</ul>

<hr/>

<h2>Current Recognized Rule IDs</h2>

<ul>
  <li><code>pure_public_addition</code></li>
  <li><code>ui_value_roundtrip</code></li>
  <li><code>ui_property_write</code></li>
  <li><code>stateful_feedback_delay</code></li>
  <li><code>bounded_ui_accumulator</code></li>
</ul>

<hr/>

<h2>Boundary</h2>

<p>
This deriver is downstream from source and validation.
It does not own language semantics, widget law, runtime behavior, lowering, backend contracts, or LLVM behavior.
It is a reference implementation tool that protects the current published source-to-FIR corridor.
</p>
