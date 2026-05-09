<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference Runtime</h1>

<p align="center">
  <strong>Runtime-family consumers and contract-driven acceptance checks for the non-normative FROG reference implementation</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Overview</h2>

<p>
This directory coordinates runtime-family checks in the non-normative reference implementation.
The runtime consumes emitted backend contracts, validates repository-visible runtime acceptance snapshots, and, for Example 05, also contains a manifest-driven native-kernel bridge path.
</p>

<p>
The default runtime executor dispatches by <code>contract.units[0].kind</code>.
The source example identifier remains preserved as traceability metadata, but it is not the runtime-executor authority.
</p>

<p>
The native-kernel bridge posture is documented in <a href="./KernelBridge.md"><code>KernelBridge.md</code></a>.
That document records the compiler-agnostic runtime/backend boundary for the Example 05 compiled-kernel closure.
</p>

<hr/>

<h2>Current Runtime Surface</h2>

<pre><code>Examples 01-04
  backend contract
    -&gt; contract.units[0].kind
    -&gt; generic contract executor
    -&gt; runtime acceptance snapshot

Example 05 standard runtime
  normalized backend contract + .wfrog package
    -&gt; contract.units[0].kind
    -&gt; bounded contract executor
    -&gt; C++ browser-host UI / Python HTTP smoke UI
    -&gt; runtime acceptance snapshot

Example 05 native-kernel runtime closure
  .frog source
    -&gt; FIR
    -&gt; lowering
    -&gt; LLVM-oriented backend artifact kernel.ll
    -&gt; native kernel manifest

  .wfrog front panel
    -&gt; bounded C++ browser-host renderer
    -&gt; widget input / output binding

  C++ runtime
    -&gt; NativeKernelBridge
    -&gt; frog_example05_run(...)
    -&gt; result / diagnostic
    -&gt; same runtime snapshot surface
</code></pre>

<hr/>

<h2>Current Runtime Executor Kinds</h2>

<table>
  <thead>
    <tr>
      <th>Contract unit kind</th>
      <th>Executor role</th>
      <th>Published examples</th>
    </tr>
  </thead>
  <tbody>
    <tr><td><code>pure_addition_kernel</code></td><td>Pure public-input addition executor.</td><td><code>01_pure_addition</code></td></tr>
    <tr><td><code>ui_value_roundtrip_kernel</code></td><td>Widget-value arithmetic executor.</td><td><code>02_ui_value_roundtrip</code></td></tr>
    <tr><td><code>ui_property_write_effect_unit</code></td><td>Explicit UI property-write effect executor.</td><td><code>03_ui_property_write</code></td></tr>
    <tr><td><code>stateful_feedback_delay_kernel</code></td><td>Explicit delay-backed state-step executor.</td><td><code>04_stateful_feedback_delay</code></td></tr>
    <tr><td><code>bounded_executable_ui_unit</code></td><td>Normalized bounded UI accumulator executor with <code>.wfrog</code> support.</td><td><code>05_bounded_ui_accumulator</code></td></tr>
  </tbody>
</table>

<hr/>

<h2>Example 05 Native-Kernel Runtime Closure</h2>

<p>
Example 05 now has two intentionally distinct C++ runtime paths:
</p>

<ul>
  <li>a standard contract-driven path, which remains available without LLVM or <code>clang</code>;</li>
  <li>an optional native-kernel path, where <code>kernel.ll</code> is compiled by <code>clang</code>, linked into a dedicated runtime executable, and consumed through <code>NativeKernelBridge</code>.</li>
</ul>

<p>
The native-kernel path does not make the runtime LLVM-only.
LLVM is a backend/native-kernel producer.
The runtime consumes a manifest-declared ABI and a linked C-compatible entry point.
</p>

<pre><code>Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/kernel.ll
  -&gt; clang
  -&gt; object linked into frog_reference_runtime_cpp_llvm_kernel

Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/native_kernel_manifest.json
  -&gt; NativeKernelManifest
  -&gt; NativeKernelBridge
  -&gt; frog_example05_run(uint16_t, FrogRunResult*)

Examples/05_bounded_ui_accumulator/ui/accumulator_panel.wfrog
  -&gt; BrowserUiRuntime
  -&gt; panel_pixels + SVG skins + label/value overlays

Browser POST /run
  -&gt; ctrl_input.value
  -&gt; native kernel ABI call
  -&gt; public output result
  -&gt; ind_result.value
  -&gt; state.json / runtime snapshot
</code></pre>

<hr/>

<h2>Normalized Example 05 Runtime Contract</h2>

<p>
The specialized Example 05 runtimes consume the normalized backend-contract surface:
</p>

<pre><code>public_io
ui_bindings
execution_kernel
effects
publications
</code></pre>

<p>
The legacy compatibility fields have been removed from the published Example 05 contract.
</p>

<hr/>

<h2>Commands</h2>

<pre><code>python Implementations/Reference/Runtime/check_examples01_05_runtime_acceptance.py
python Implementations/Reference/Runtime/python/execute_contract.py 3

python Implementations/Reference/Runtime/check_example05_native_kernel_bridge.py
python Implementations/Reference/Runtime/check_example05_cpp_native_kernel_bridge.py
python Implementations/Reference/check_reference_workspace.py --include-native-kernel-bridge
</code></pre>

<hr/>

<h2>Boundary</h2>

<p>
This directory does not define FROG semantics, backend contract law, widget law, UI host law, backend compilation behavior, or compiler-family behavior.
It consumes emitted backend contracts and validates repository-visible runtime behavior for the current bounded reference corridor.
</p>

<p>
The native-kernel bridge preserves this boundary: the runtime hosts execution and UI; backends compile lowered units; explicit manifests and stable ABI surfaces connect both sides.
The Example 05 native-kernel closure is a bounded LabVIEW-like proof corridor, not a generalized production runtime.
</p>
