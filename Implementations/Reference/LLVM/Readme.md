<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference LLVM-Oriented Path</h1>

<p align="center">
  <strong>Downstream compiler-family posture for native executable closure in the non-normative FROG reference implementation</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Current LLVM Example Surface</h2>

<ul>
  <li><code>examples/01_pure_addition/</code></li>
  <li><code>examples/02_ui_value_roundtrip/</code></li>
  <li><code>examples/03_ui_property_write/</code></li>
  <li><code>examples/04_stateful_feedback_delay/</code></li>
  <li><code>examples/05_bounded_ui_accumulator/</code></li>
</ul>

<hr/>

<h2>Generic Lowering-to-LLVM Entry Point</h2>

<pre><code>python Implementations/Reference/LLVM/tools/emit_lowering_to_llvm.py \
  --lowering Examples/01_pure_addition/main.lowering.json \
  --expected Implementations/Reference/LLVM/examples/01_pure_addition/module.ll \
  --check
</code></pre>

<p>
The generic emitter dispatches by <code>lowered_units[0].kind</code>.
</p>

<p>
Supported lowered unit kinds:
</p>

<ul>
  <li><code>pure_addition_kernel</code></li>
  <li><code>ui_value_roundtrip_kernel</code></li>
  <li><code>ui_property_write_effect_unit</code></li>
  <li><code>stateful_feedback_delay_kernel</code></li>
  <li><code>bounded_accumulator_kernel_with_ui_bindings</code></li>
</ul>

<hr/>

<h2>Compatibility Wrappers</h2>

<pre><code>python Implementations/Reference/LLVM/tools/emit_examples01_04_llvm_modules.py --check
python Implementations/Reference/LLVM/tools/emit_llvm_module.py --check
</code></pre>

<p>
These wrappers are kept for continuity, but both now delegate to the generic lowered-unit-kind emitter.
</p>

<hr/>

<h2>Native Build</h2>

<pre><code>python Implementations/Reference/Pipeline/check_examples01_05_full.py --include-llvm-build</code></pre>

<hr/>

<p>
LLVM remains downstream from FROG.
These examples are native proof dossiers, not a generalized production backend.
</p>
