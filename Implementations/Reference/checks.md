<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference Workspace Checks</h1>

<p align="center">
  <strong>Repository-quality checks for the current FROG reference implementation workspace</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Fresh Clone Prerequisites</h2>

<p>
The Git repository carries the reference sources, expected artifacts, runtime code, <code>.wfrog</code> realization packages, SVG assets, LLVM proof material, and validation scripts.
A clean clone still needs local development tools before every check can run.
</p>

<table>
  <thead>
    <tr>
      <th>Tool</th>
      <th>Required for</th>
      <th>Notes</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>python</code> + <code>pip</code></td>
      <td>Reference workspace checks and Python runtime tests</td>
      <td>Install <code>pytest</code> with <code>python -m pip install pytest</code>.</td>
    </tr>
    <tr>
      <td><code>cmake</code> + C++ toolchain</td>
      <td>C++ runtime build and CTest surface</td>
      <td>The standard C++ runtime build does not require LLVM.</td>
    </tr>
    <tr>
      <td><code>cargo</code> / Rust toolchain</td>
      <td>Rust runtime tests</td>
      <td>Used to validate the Rust runtime-family consumer.</td>
    </tr>
    <tr>
      <td><code>clang</code></td>
      <td>LLVM native proof and native-kernel bridge checks</td>
      <td>Optional for baseline checks; required for native LLVM-backed closure.</td>
    </tr>
  </tbody>
</table>

<p>
Recommended clean-clone smoke sequence:
</p>

<pre><code>python -m pip install pytest
python Implementations/Reference/check_reference_workspace.py --include-pytest

cmake -S Implementations/Reference/Runtime/cpp -B build/frog_runtime_cpp
cmake --build build/frog_runtime_cpp
ctest --test-dir build/frog_runtime_cpp --output-on-failure

cargo test --manifest-path Implementations/Reference/Runtime/rust/Cargo.toml
</code></pre>

<p>
When <code>clang</code> is available, add the native checks:
</p>

<pre><code>python Implementations/Reference/check_reference_workspace.py --include-llvm-build
python Implementations/Reference/check_reference_workspace.py --include-native-kernel-bridge
</code></pre>

<hr/>

<h2>Primary Command</h2>

<pre><code>python Implementations/Reference/check_reference_workspace.py</code></pre>

<p>
This command runs the Examples 01–05 full pipeline.
Widget-layer validation is included by default through the pipeline.
</p>

<hr/>

<h2>Full Python Test Command</h2>

<pre><code>python -m pip install pytest
python Implementations/Reference/check_reference_workspace.py --include-pytest</code></pre>

<p>
This additionally runs the pytest suites for artifact checks, derivation, lowering, contract emission, generic runtime contract execution, Python runtime UI coverage for the current widget examples, LLVM, and pipeline coordination.
</p>

<hr/>

<h2>LLVM Native Build Proof</h2>

<pre><code>python Implementations/Reference/check_reference_workspace.py --include-llvm-build</code></pre>

<p>
This requires <code>clang</code>.
It verifies the native LLVM proof dossiers for Examples 01–05.
Widget-layer validation remains enabled unless explicitly skipped.
</p>

<hr/>

<h2>Native-Kernel Runtime Bridge</h2>

<pre><code>python Implementations/Reference/check_reference_workspace.py --include-native-kernel-bridge</code></pre>

<p>
This requires <code>cmake</code>, a C++ toolchain, and <code>clang</code>.
It verifies:
</p>

<ul>
  <li>the Example 05 native-kernel manifest publication surface,</li>
  <li>the ABI declaration for <code>frog_example05_run</code>,</li>
  <li>the LLVM <code>kernel.ll</code> result-status out-parameter layout,</li>
  <li>the C++ <code>NativeKernelBridge</code> path,</li>
  <li>the Example 06 native Boolean manifest and C++ LLVM-produced native bridge surface,</li>
  <li>the Python and Rust dynamic native-kernel bridge surfaces for the current published widget examples,</li>
  <li>runtime snapshot preservation through the bridge,</li>
  <li>overflow diagnostic mapping,</li>
  <li>the optional native-kernel runtime executable linked against compiled LLVM artifacts.</li>
</ul>

<hr/>

<h2>C++ Runtime Checks</h2>

<pre><code>cmake -S Implementations/Reference/Runtime/cpp -B build/frog_runtime_cpp
cmake --build build/frog_runtime_cpp
ctest --test-dir build/frog_runtime_cpp --output-on-failure</code></pre>

<p>
The standard C++ checks do not require LLVM or <code>clang</code>.
They validate the contract runtime path, the current Examples 05–10 browser-host UI rendering surfaces, <code>.wfrog</code> / SVG geometry coherence, and native-kernel bridge paths using ABI-compatible linked stubs.
</p>

<p>
The optional LLVM-produced native-kernel runtime closure is checked with:
</p>

<pre><code>python Implementations/Reference/check_reference_workspace.py --include-native-kernel-bridge</code></pre>

<hr/>

<h2>Runtime Contract Executor Check</h2>

<pre><code>python Implementations/Reference/Runtime/check_examples01_05_runtime_acceptance.py
python Implementations/Reference/Runtime/execute_reference_contract.py \
  --acceptance Implementations/Reference/Runtime/acceptance/example01_pure_addition.acceptance.json \
  --check
</code></pre>

<hr/>

<h2>Widget-Layer Only Command</h2>

<pre><code>python Implementations/Reference/check_reference_workspace.py --widget-layer-only</code></pre>

<p>
This runs only the non-normative widget-layer repository hygiene validator:
</p>

<pre><code>python Implementations/Reference/WidgetValidator/validate_widget_layer.py</code></pre>

<p>
Use this command when changing widget class-law documents, Default realization documents, <code>*.default.wfrog</code> manifests, SVG resources, public part names, package bindings, or validation expectations.
</p>

<hr/>

<h2>Continuous Integration</h2>

<p>
The repository publishes a GitHub Actions workflow at:
</p>

<pre><code>.github/workflows/reference-workspace.yml</code></pre>

<p>
The workflow runs:
</p>

<ul>
  <li><code>Reference workspace</code> — primary Examples 01–05 workspace check.</li>
  <li><code>Reference pytest</code> — installs <code>pytest</code> and runs <code>check_reference_workspace.py --include-pytest</code>.</li>
  <li><code>Widget layer only</code> — runs the widget-layer validator directly.</li>
  <li><code>Reference native kernel bridge</code> — installs native build tooling and runs <code>check_reference_workspace.py --include-native-kernel-bridge</code>.</li>
</ul>

<hr/>

<h2>Explicit Widget-Validator Skip</h2>

<pre><code>python Implementations/Reference/check_reference_workspace.py --skip-widget-validator</code></pre>

<p>
This skips widget-layer validation inside the Examples 01–05 pipeline.
It should be used only for narrow debugging of unrelated pipeline stages.
The default repository-quality posture is to keep widget validation enabled.
</p>
