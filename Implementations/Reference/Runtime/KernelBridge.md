<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference Runtime Bridge</h1>

<p align="center">
  <strong>Compiler-agnostic bridge posture for the non-normative FROG reference runtime</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This document records the boundary between the FROG reference runtime and backend-produced compiled artifacts.
The runtime hosts execution and UI. Backends compile lowered units. Explicit manifests and stable call surfaces connect both worlds.
</p>

<p>
This document is non-normative. Version governance remains centralized in <code>Versioning/Readme.md</code> and repository history.
</p>

<hr/>

<h2>Current Status</h2>

<p>
The C++ reference runtime currently hosts the Example 05 UI and executes the backend contract.
The LLVM proof path separately validates native-oriented compiler output from the lowered unit.
The browser renderer is currently an Example 05 bounded fidelity pass, not a generalized faithful <code>.wfrog</code> renderer.
</p>

<p>
The first bridge publication surface is now present as a manifest plus ABI-oriented LLVM artifact.
The C++ runtime bridge code that loads or links that manifest-declared kernel is not yet published.
</p>

<hr/>

<h2>Core Rule</h2>

<pre><code>FROG runtime hosts execution and UI.
FROG backends compile lowered units.
Explicit manifests and stable ABI surfaces connect both worlds.
</code></pre>

<hr/>

<h2>Published Example 05 Bridge Artifacts</h2>

<pre><code>Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/native_kernel_manifest.json
Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/kernel.ll
Implementations/Reference/Runtime/check_example05_native_kernel_bridge.py
</code></pre>

<p>
The manifest declares <code>frog_example05_run</code> with ABI <code>frog_u16_to_result_status</code>.
The ABI artifact returns a compact result status with <code>ok</code>, <code>result</code>, and <code>error_code</code> fields.
The publication checker validates the manifest, source lowering reference, ABI declaration, exported symbol shape, and overflow diagnostic mapping.
</p>

<p>
The checker can be run directly with:
</p>

<pre><code>python Implementations/Reference/Runtime/check_example05_native_kernel_bridge.py</code></pre>

<hr/>

<h2>Example 05 Target</h2>

<p>
The first implementation target remains <code>Examples/05_bounded_ui_accumulator/</code>.
The runtime should consume a manifest-declared compiled artifact instead of owning the Example 05 algorithm internally.
</p>

<p>
The first success case remains <code>input_value = 3</code> and <code>result = 15</code>.
Overflow should be reported with <code>error_code = 1</code> and mapped by the runtime to <code>final_state must remain in the u16 domain.</code>
</p>

<hr/>

<h2>Non-Goals</h2>

<ul>
  <li>The runtime does not compile diagrams.</li>
  <li>The runtime does not depend on LLVM.</li>
  <li>This bridge does not claim a complete production runtime.</li>
  <li>This bridge does not introduce Example 06 or new widget classes.</li>
</ul>
