<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference Runtime (C/C++)</h1>

<p align="center">
  <strong>C/C++ realization of the published <code>reference_host_runtime_ui_binding</code> family</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<p>
Repository governance and publication state are centralized in
<a href="../../../../Versioning/Readme.md"><code>Versioning/Readme.md</code></a>.
</p>

<hr/>

<h2>Directory Navigation</h2>

<pre><code>Implementations/Reference/Runtime/cpp/
├── Readme.md
├── CMakeLists.txt
├── include/
│   ├── contract.hpp
│   ├── execute.hpp
│   ├── json.hpp
│   ├── kernel_bridge.hpp
│   ├── runtime.hpp
│   └── ui.hpp
├── src/
│   ├── contract.cpp
│   ├── execute.cpp
│   ├── json.cpp
│   ├── kernel_bridge.cpp
│   ├── main.cpp
│   ├── runtime.cpp
│   └── ui.cpp
└── tests/
    ├── test_slice05.cpp
    └── test_slice05_llvm_kernel.cpp</code></pre>

<p>
Generated build directories may appear beside these files in local workspaces.
They are not part of the intended source surface of this runtime.
</p>

<hr/>

<h2>Role</h2>

<p>
This directory contains the C/C++ consumer for the published Example 05 runtime corridor.
It accepts the emitted backend contract, loads the published <code>.wfrog</code> package, resolves the referenced SVG assets, executes the bounded runtime corridor, and can expose the panel through a browser-host UI.
</p>

<p>
The important point is architectural: this directory is a runtime-family consumer, not a language-definition layer, not a compiler-family layer, and not a substitute for canonical source, FIR, lowering, or backend compilation.
</p>

<hr/>

<h2>Current Published Entry Points</h2>

<h3><code>src/main.cpp</code></h3>

<p>The main executable entry point supports:</p>

<ul>
  <li>headless execution by default or through <code>run</code>,</li>
  <li>browser-host UI through <code>ui</code>.</li>
</ul>

<p>From the repository root:</p>

<pre><code>cmake -S Implementations/Reference/Runtime/cpp -B build/frog_runtime_cpp
cmake --build build/frog_runtime_cpp
ctest --test-dir build/frog_runtime_cpp

build/frog_runtime_cpp/frog_reference_runtime_cpp 3
build/frog_runtime_cpp/frog_reference_runtime_cpp ui
build/frog_runtime_cpp/frog_reference_runtime_cpp ui --host 127.0.0.1 --port 8080 --no-open-browser</code></pre>

<h3><code>src/runtime.cpp</code> and <code>src/execute.cpp</code></h3>

<p>
These files hold the bounded runtime core and the headless execution path.
The current default runtime path validates the contract family, package shape, widget classes, property writes, and the Example 05 execution model before producing a runtime result artifact.
</p>

<p>
The runtime also exposes <code>execute_with_native_kernel_bridge(...)</code> for the manifest-declared native-kernel bridge path.
That path consumes a loaded <code>NativeKernelBridge</code>, binds the current control value to the ABI input, calls the native kernel entry point, maps returned status into diagnostics, and publishes the result to the same runtime artifact surface.
</p>

<h3><code>src/kernel_bridge.cpp</code></h3>

<p>
Compiler-agnostic native-kernel bridge helper for Example 05.
It loads <code>native_kernel_manifest.json</code>, validates the manifest-declared entry symbol, ABI name, IO surface, and overflow model, then calls a linked native kernel through a stable function pointer surface.
</p>

<p>
The standard C++ test uses a linked ABI-compatible kernel stub so the baseline CMake build does not depend on LLVM or <code>clang</code>.
The optional bridge test compiles the published LLVM artifact <code>kernel.ll</code> with <code>clang</code> and links it into a dedicated C++ test target.
</p>

<h3><code>src/ui.cpp</code></h3>

<p>
Browser-host realization for the same runtime core.
The current host serves a browser page, the referenced SVG assets, and a runtime snapshot surface.
</p>

<p>
The Example 05 renderer uses the published <code>.wfrog</code> panel layout as the positioning authority, the referenced SVG assets as the visible widget skins, and overlayed label/value surfaces aligned from the SVG anchor and value-box markers.
This is a bounded fidelity pass for the current Example 05 numeric widgets, not a complete generalized <code>.wfrog</code> renderer.
</p>

<hr/>

<h2>Current Bounded Surface</h2>

<ul>
  <li>backend family <code>reference_host_runtime_ui_binding</code>,</li>
  <li>one contract unit named <code>main</code>,</li>
  <li>one public input <code>input_value : u16</code>,</li>
  <li>one public output <code>result : u16</code>,</li>
  <li>one explicit state carrier based on <code>frog.core.delay</code>,</li>
  <li>exactly five loop iterations,</li>
  <li>two widget classes: <code>frog.widgets.numeric_control</code> and <code>frog.widgets.numeric_indicator</code>,</li>
  <li>five supported widget properties: <code>value</code>, <code>label</code>, <code>visible</code>, <code>enabled</code>, and <code>foreground_color</code>,</li>
  <li>panel-pixel layout from <code>Examples/05_bounded_ui_accumulator/ui/accumulator_panel.wfrog</code>,</li>
  <li>SVG skin assets from <code>Examples/05_bounded_ui_accumulator/ui/assets/</code>,</li>
  <li>label/value overlays aligned from <code>label_anchor</code>, <code>value_anchor</code>, and <code>value_box</code> markers where present,</li>
  <li>manifest-declared native-kernel bridge path through <code>kernel_bridge.hpp</code> / <code>kernel_bridge.cpp</code>.</li>
</ul>

<p>
The browser-host closure is intentionally narrow.
It exists to prove that the published contract and the published package are enough for a real visible host runtime in C/C++.
The native-kernel bridge path exists to prove that the runtime can call a manifest-declared kernel through an ABI surface without becoming compiler-specific.
It does not claim native compiled UI closure through LLVM and does not claim a full LabVIEW-like runtime.
</p>

<hr/>

<h2>Native Kernel Bridge Surface</h2>

<pre><code>native_kernel_manifest.json
  -&gt; NativeKernelManifest
  -&gt; NativeKernelBridge
  -&gt; frog_example05_run(input_value)
  -&gt; FrogRunResult { ok, result, error_code }
  -&gt; runtime result / diagnostic / snapshot
</code></pre>

<p>
The runtime consumes a manifest-declared function pointer.
The fact that the first published backend artifact is LLVM-oriented remains manifest metadata, not a runtime-core dependency.
</p>

<hr/>

<h2>Inputs and Outputs</h2>

<h3>Inputs</h3>

<ul>
  <li>The emitted contract artifact under <code>Implementations/Reference/ContractEmitter/examples/</code>.</li>
  <li>The Example 05 package <code>Examples/05_bounded_ui_accumulator/ui/accumulator_panel.wfrog</code>.</li>
  <li>The SVG assets referenced by that package.</li>
  <li>The native-kernel manifest under <code>Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/native_kernel_manifest.json</code> for bridge-specific tests.</li>
  <li>The LLVM kernel artifact under <code>Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/kernel.ll</code> for the optional LLVM-produced bridge test.</li>
</ul>

<h3>Outputs</h3>

<ul>
  <li>A headless runtime result artifact.</li>
  <li>A browser-host page driven by the same runtime core.</li>
  <li>A native-kernel bridge result projected onto the same runtime artifact surface.</li>
  <li>An optional C++ test executable linked against an object compiled from <code>kernel.ll</code>.</li>
</ul>

<hr/>

<h2>Tests</h2>

<p>
The published standard C/C++ test target checks execution, native-kernel bridge behavior with an ABI-compatible stub, and HTML rendering.
After configuring and building, run:
</p>

<pre><code>cmake -S Implementations/Reference/Runtime/cpp -B build/frog_runtime_cpp
cmake --build build/frog_runtime_cpp
ctest --test-dir build/frog_runtime_cpp</code></pre>

<p>
The optional LLVM-produced native-kernel bridge target requires <code>clang</code> and can be run through the repository checker:
</p>

<pre><code>python Implementations/Reference/Runtime/check_example05_cpp_native_kernel_bridge.py
python Implementations/Reference/check_reference_workspace.py --include-native-kernel-bridge</code></pre>

<p>
Or directly with CMake:
</p>

<pre><code>cmake -S Implementations/Reference/Runtime/cpp \
  -B build/frog_runtime_cpp_native_kernel_bridge \
  -DFROG_RUNTIME_CPP_ENABLE_LLVM_KERNEL_BRIDGE=ON
cmake --build build/frog_runtime_cpp_native_kernel_bridge \
  --target frog_reference_runtime_cpp_llvm_kernel_tests
ctest --test-dir build/frog_runtime_cpp_native_kernel_bridge \
  -R frog_reference_runtime_cpp_llvm_kernel_tests \
  --output-on-failure</code></pre>

<p>The current test surfaces check:</p>

<ul>
  <li>headless execution with input <code>3</code> and final result <code>15</code>,</li>
  <li>overflow rejection behavior,</li>
  <li>native-kernel manifest loading, entry-symbol validation, ABI validation, and bridge execution through a linked ABI-compatible kernel stub,</li>
  <li>optional native-kernel bridge execution against an object compiled from <code>kernel.ll</code>,</li>
  <li>native-kernel bridge success with input <code>3</code> and final result <code>15</code>,</li>
  <li>native-kernel bridge overflow mapping to <code>final_state must remain in the u16 domain.</code>,</li>
  <li>indicator publication through the runtime artifact,</li>
  <li>browser-host HTML rendering with both SVG asset routes,</li>
  <li>front-panel dimensions from the <code>.wfrog</code> package,</li>
  <li>widget instance IDs, widget classes, SVG asset routes, and panel-pixel widget coordinates,</li>
  <li>anchor-aware overlay markers for labels and values.</li>
</ul>

<hr/>

<h2>Relationship to the Other Runtime Consumers</h2>

<p>
This directory should remain aligned with the Python and Rust consumers on:
</p>

<ul>
  <li>contract acceptance,</li>
  <li>package acceptance,</li>
  <li>execution semantics for Example 05,</li>
  <li>the minimal widget-property surface,</li>
  <li>the browser-host UI shape for the bounded corridor.</li>
</ul>

<p>
The parent runtime-family definition is documented in
<a href="../Readme.md"><code>Implementations/Reference/Runtime/Readme.md</code></a>.
</p>

<p>
The runtime/compiler native-kernel bridge direction is documented in
<a href="../KernelBridge.md"><code>Implementations/Reference/Runtime/KernelBridge.md</code></a>.
</p>

<hr/>

<h2>Non-Goals</h2>

<ul>
  <li>General runtime support for arbitrary contracts.</li>
  <li>Language or widget-law ownership.</li>
  <li>Compiler-family responsibilities.</li>
  <li>Native compiled UI closure.</li>
  <li>A complete generalized <code>.wfrog</code> renderer.</li>
  <li>A complete LabVIEW-like runtime surface.</li>
  <li>A mandatory runtime dependency on LLVM or <code>clang</code>.</li>
</ul>

<hr/>

<h2>Summary</h2>

<p>Read this directory as the C/C++ proof path for the current runtime family:</p>

<pre><code>contract + .wfrog + SVG assets
=&gt; C/C++ runtime core
=&gt; headless result or browser-host UI

native kernel manifest + linked ABI-compatible kernel
=&gt; C++ NativeKernelBridge
=&gt; same runtime result / diagnostic / snapshot surface

kernel.ll + clang + optional CMake target
=&gt; object linked into C++ bridge test
=&gt; same runtime result / diagnostic / snapshot surface</code></pre>

<p>
The current browser-host renderer is closer to the Example 05 <code>.wfrog</code> package than the earlier HTML-card posture, because it uses the package layout, SVG skins, label/value overlays, and dynamic property writes.
It remains an Example 05 bounded renderer, not the final renderer for the full FROG widget system.
</p>
