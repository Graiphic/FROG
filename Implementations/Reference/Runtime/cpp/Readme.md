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
│   ├── main_llvm_kernel.cpp
│   ├── runtime.cpp
│   └── ui.cpp
└── tests/
    ├── test_slice05.cpp
    └── test_slice05_llvm_kernel.cpp</code></pre>

<hr/>

<h2>Role</h2>

<p>
This directory contains the C/C++ consumer for the published Example 05 runtime corridor.
It accepts the emitted backend contract, loads the published <code>.wfrog</code> package, resolves the referenced SVG assets, exposes a browser-host UI, and can run either the default contract-executor path or the optional manifest-declared native-kernel bridge path.
</p>

<p>
This directory is a runtime-family consumer.
It is not a language-definition layer, not a compiler-family layer, and not a substitute for canonical source, FIR, lowering, or backend compilation.
</p>

<hr/>

<h2>Standard Runtime Executable</h2>

<pre><code>frog_reference_runtime_cpp</code></pre>

<p>
The standard executable supports:
</p>

<ul>
  <li>headless execution by default or through <code>run</code>,</li>
  <li>browser-host UI through <code>ui</code>.</li>
</ul>

<p>
It remains available without LLVM or <code>clang</code>.
It executes the bounded Example 05 backend contract and produces the published runtime artifact.
</p>

<pre><code>cmake -S Implementations/Reference/Runtime/cpp -B build/frog_runtime_cpp
cmake --build build/frog_runtime_cpp
ctest --test-dir build/frog_runtime_cpp

build/frog_runtime_cpp/frog_reference_runtime_cpp 3
build/frog_runtime_cpp/frog_reference_runtime_cpp ui --host 127.0.0.1 --port 8080 --no-open-browser</code></pre>

<hr/>

<h2>Native-Kernel Runtime Executable</h2>

<pre><code>frog_reference_runtime_cpp_llvm_kernel</code></pre>

<p>
The optional native-kernel executable is built when <code>FROG_RUNTIME_CPP_ENABLE_LLVM_KERNEL_BRIDGE=ON</code>.
CMake compiles <code>Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/kernel.ll</code> with <code>clang</code>, links the resulting object into the executable, and uses <code>native_kernel_manifest.json</code> to validate and call the ABI surface.
</p>

<p>
This executable is the bounded Example 05 LabVIEW-like proof path:
</p>

<pre><code>.frog source
  -&gt; FIR
  -&gt; lowering
  -&gt; LLVM-produced kernel.ll
  -&gt; native kernel manifest

.wfrog front panel
  -&gt; C++ browser-host renderer
  -&gt; SVG skins and anchor/value overlays

runtime C++
  -&gt; NativeKernelBridge
  -&gt; frog_example05_run(input_value, out_result)
  -&gt; public result
  -&gt; ind_result.value
  -&gt; runtime snapshot / state.json
</code></pre>

<pre><code>python Implementations/Reference/Runtime/check_example05_cpp_native_kernel_bridge.py

build/frog_runtime_cpp_native_kernel_bridge/frog_reference_runtime_cpp_llvm_kernel 3
build/frog_runtime_cpp_native_kernel_bridge/frog_reference_runtime_cpp_llvm_kernel ui --host 127.0.0.1 --port 8080 --no-open-browser</code></pre>

<hr/>

<h2>Browser-Host UI Fidelity</h2>

<p>
The current C++ browser-host renderer consumes the Example 05 <code>.wfrog</code> package as the panel layout authority.
It renders:
</p>

<ul>
  <li>the front panel in <code>panel_pixels</code>,</li>
  <li>the widget <code>x</code>, <code>y</code>, <code>width</code>, and <code>height</code> declarations,</li>
  <li>the numeric SVG skins as the widget bodies,</li>
  <li><code>label_anchor</code>, <code>value_anchor</code>, and <code>value_box</code> overlays,</li>
  <li>dynamic widget values and labels,</li>
  <li>contract-driven <code>foreground_color</code> property writes.</li>
</ul>

<p>
The Example 05 panel uses <code>500x170</code> panel bounds and two <code>220x88</code> numeric widgets, matching the natural geometry of the published SVG assets.
This is a bounded Example 05 fidelity pass, not a complete generalized <code>.wfrog</code> renderer.
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

<h2>Tests</h2>

<p>The standard C/C++ test target checks:</p>

<ul>
  <li>headless execution with input <code>3</code> and final result <code>15</code>,</li>
  <li>overflow rejection behavior,</li>
  <li>native-kernel bridge behavior with an ABI-compatible stub,</li>
  <li>browser-host HTML rendering with both SVG asset routes,</li>
  <li><code>.wfrog</code> / SVG geometry coherence for the Example 05 numeric widgets.</li>
</ul>

<p>The optional LLVM-produced native-kernel bridge target checks:</p>

<ul>
  <li>manifest loading, entry-symbol validation, ABI validation, and error mapping,</li>
  <li>direct calls through an object compiled from <code>kernel.ll</code>,</li>
  <li>runtime snapshot publication through <code>execute_with_native_kernel_bridge(...)</code>,</li>
  <li>browser UI runtime execution through <code>BrowserUiRuntime</code> with a native kernel bridge.</li>
</ul>

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

<p>
Read this directory as the C/C++ proof path for the current runtime family:
</p>

<pre><code>contract + .wfrog + SVG assets
=&gt; C/C++ runtime core
=&gt; headless result or browser-host UI

native kernel manifest + linked ABI-compatible kernel
=&gt; C++ NativeKernelBridge
=&gt; same runtime result / diagnostic / snapshot surface

kernel.ll + clang + optional CMake target
=&gt; native-kernel runtime executable
=&gt; hosted .wfrog UI calling a compiled diagram kernel
</code></pre>
