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
The runtime hosts execution and UI.
Backends compile lowered units.
Explicit manifests and stable call surfaces connect both worlds.
</p>

<p>
This document is non-normative.
Version governance remains centralized in <code>Versioning/Readme.md</code> and repository history.
</p>

<hr/>

<h2>Current Status</h2>

<p>
Examples 05, 06, 07, 08, and 09 now have published native-kernel bridge surfaces.
The LLVM-oriented backend path publishes <code>kernel.ll</code> and <code>native_kernel_manifest.json</code> for those slices.
The C++ / Python / Rust runtime bridges load the manifest, validate the declared ABI, call linked or dynamically loaded ABI-compatible kernel entry points, map <code>error_code</code> to diagnostics where applicable, and project the result onto the existing runtime snapshot surface.
</p>

<p>
The standard runtime paths still keep the backend-contract executors available.
The optional native-kernel runtime paths consume LLVM-produced kernel artifacts through manifest-declared ABI surfaces and run the same <code>.frog</code> / <code>.wfrog</code> front-panel posture through the browser-host runtimes.
</p>

<p>
The browser renderers are current bounded fidelity passes for Examples 05 through 09, not complete generalized faithful <code>.wfrog</code> renderers.
</p>

<hr/>

<h2>Core Rule</h2>

<pre><code>FROG runtime hosts execution and UI.
FROG backends compile lowered units.
Explicit manifests and stable ABI surfaces connect both worlds.
</code></pre>

<hr/>

<h2>Published Example 05-09 Bridge Artifacts</h2>

<pre><code>Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/native_kernel_manifest.json
Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/kernel.ll
Implementations/Reference/LLVM/examples/06_boolean_value_roundtrip/native_kernel_manifest.json
Implementations/Reference/LLVM/examples/06_boolean_value_roundtrip/kernel.ll
Implementations/Reference/LLVM/examples/07_string_value_roundtrip/native_kernel_manifest.json
Implementations/Reference/LLVM/examples/07_string_value_roundtrip/kernel.ll
Implementations/Reference/LLVM/examples/08_enum_value_roundtrip/native_kernel_manifest.json
Implementations/Reference/LLVM/examples/08_enum_value_roundtrip/kernel.ll
Implementations/Reference/LLVM/examples/09_path_value_roundtrip/native_kernel_manifest.json
Implementations/Reference/LLVM/examples/09_path_value_roundtrip/kernel.ll
Implementations/Reference/Runtime/check_example05_native_kernel_bridge.py
Implementations/Reference/Runtime/check_example05_cpp_native_kernel_bridge.py
Implementations/Reference/Runtime/check_example06_native_kernel_bridge.py
Implementations/Reference/Runtime/check_example06_cpp_native_kernel_bridge.py
Implementations/Reference/Runtime/check_python_native_kernel_bridge.py
Implementations/Reference/Runtime/check_rust_native_kernel_bridge.py
Implementations/Reference/Runtime/cpp/include/kernel_bridge.hpp
Implementations/Reference/Runtime/cpp/src/kernel_bridge.cpp
Implementations/Reference/Runtime/cpp/src/main_llvm_kernel.cpp
Implementations/Reference/Runtime/cpp/tests/test_slice05_llvm_kernel.cpp
Implementations/Reference/Runtime/cpp/tests/test_slice06_llvm_kernel.cpp
Implementations/Reference/Runtime/cpp/tests/test_slice07_llvm_kernel.cpp
Implementations/Reference/Runtime/cpp/tests/test_slice08_llvm_kernel.cpp
Implementations/Reference/Runtime/cpp/tests/test_slice09_llvm_kernel.cpp
</code></pre>

<p>
The Example 05 manifest declares <code>frog_example05_run</code> with ABI <code>frog_u16_to_result_status_outptr</code>.
The Example 06 manifest declares <code>frog_example06_run</code> with ABI <code>frog_bool_to_result_status_outptr</code>.
The Example 07 and Example 09 manifests use UTF-8 copy surfaces.
The Example 08 manifest uses the enum value surface.
The ABI uses an explicit out-parameter carrier:
</p>

<pre><code>void frog_example05_run(uint16_t input_value, FrogRunResult* out_result)
void frog_example06_run(uint8_t input_value, FrogBoolRunResult* out_result)
void frog_example07_run(const uint8_t* input_ptr, uint32_t input_len, FrogStringRunResult* out_result)
void frog_example08_run(uint16_t mode_value, FrogRunResult* out_result)
void frog_example09_run(const uint8_t* input_ptr, uint32_t input_len, FrogStringRunResult* out_result)</code></pre>

<p>
The result-status payload contains <code>ok</code>, <code>result</code>, and <code>error_code</code> fields.
The publication checker validates the manifest, source lowering reference, ABI declaration, exported symbol shape, out-parameter result layout, and overflow diagnostic mapping.
The C++ bridge consumes the same manifest and calls a linked entry point through a compiler-agnostic function pointer surface.
</p>

<hr/>

<h2>Native Runtime Closure</h2>

<p>
The optional native-kernel C++ runtime executable is:
</p>

<pre><code>frog_reference_runtime_cpp_llvm_kernel</code></pre>

<p>
It is built only when <code>FROG_RUNTIME_CPP_ENABLE_LLVM_KERNEL_BRIDGE=ON</code>.
That build compiles the published Example 05 through Example 09 <code>kernel.ll</code> artifacts with <code>clang</code>, links them into the executable, loads the selected manifest, and routes headless and browser UI execution through the native kernel bridge.
</p>

<pre><code>cmake -S Implementations/Reference/Runtime/cpp \
  -B build/frog_runtime_cpp_native_kernel_bridge \
  -DFROG_RUNTIME_CPP_ENABLE_LLVM_KERNEL_BRIDGE=ON

cmake --build build/frog_runtime_cpp_native_kernel_bridge \
  --target frog_reference_runtime_cpp_llvm_kernel

build/frog_runtime_cpp_native_kernel_bridge/frog_reference_runtime_cpp_llvm_kernel 3
build/frog_runtime_cpp_native_kernel_bridge/frog_reference_runtime_cpp_llvm_kernel run true --example 06
build/frog_runtime_cpp_native_kernel_bridge/frog_reference_runtime_cpp_llvm_kernel run "hello world" --example 07
build/frog_runtime_cpp_native_kernel_bridge/frog_reference_runtime_cpp_llvm_kernel run fault --example 08
build/frog_runtime_cpp_native_kernel_bridge/frog_reference_runtime_cpp_llvm_kernel run "C:/FROG/hello_world.txt" --example 09
build/frog_runtime_cpp_native_kernel_bridge/frog_reference_runtime_cpp_llvm_kernel ui --no-open-browser
build/frog_runtime_cpp_native_kernel_bridge/frog_reference_runtime_cpp_llvm_kernel ui --example 09 --no-open-browser
</code></pre>

<hr/>

<h2>Example 05 Target</h2>

<p>
The first implementation target remains <code>Examples/05_bounded_ui_accumulator/</code>.
Examples 06 through 09 extend that bridge discipline to current scalar widget slices.
The native-kernel runtime path consumes manifest-declared kernel entry surfaces instead of owning the diagram algorithms internally.
</p>

<p>
The first success case remains <code>input_value = 3</code> and <code>result = 15</code>.
Overflow is reported with <code>error_code = 1</code> and mapped by the runtime to <code>final_state must remain in the u16 domain.</code>
</p>

<hr/>

<h2>Non-Goals</h2>

<ul>
  <li>The runtime does not compile diagrams at runtime.</li>
  <li>The baseline runtime build does not depend on LLVM.</li>
  <li>This bridge does not claim a complete production runtime.</li>
  <li>This bridge does not make the current widget slices a complete widget-system implementation.</li>
  <li>The optional LLVM-produced bridge test does not make LLVM the conceptual runtime authority.</li>
  <li>The bounded Example 05-09 <code>.wfrog</code> renderers do not claim to be a full general renderer.</li>
</ul>
