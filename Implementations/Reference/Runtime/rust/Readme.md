<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

# Reference Runtime (Rust)

<p>Rust realization of the published <code>reference_host_runtime_ui_binding</code> family.</p>

<p>
  Repository governance and publication state are centralized in
  <a href="../../../../Versioning/Readme.md"><code>Versioning/Readme.md</code></a>.
</p>

<hr/>

<h2>Directory navigation</h2>

<pre><code>Implementations/Reference/Runtime/rust/
├── Readme.md
├── Cargo.toml
├── src/
│   ├── cli.rs
│   ├── contract.rs
│   ├── diagnostics.rs
│   ├── execute.rs
│   ├── lib.rs
│   ├── main.rs
│   ├── runtime.rs
│   └── ui.rs
└── tests/
    ├── slice05_runtime.rs
    ├── slice05_ui.rs
    ├── slice06_ui.rs
    ├── slice07_ui.rs
    ├── slice08_ui.rs
    ├── slice09_ui.rs
    └── slice10_ui.rs</code></pre>

<h2>Role</h2>

<p>
This directory contains the Rust consumer for the published Examples 05, 06, 07, 08, 09, and 10 runtime slices.
It accepts emitted backend contracts, loads published <code>.wfrog</code> packages,
resolves the referenced SVG assets, executes the bounded kernels, and can expose the panels through a minimal browser-host UI.
</p>

<p>
The Rust directory is no longer only a parity or test posture. It publishes both:
</p>

<ul>
  <li>a headless execution path,</li>
  <li>a browser-host UI path.</li>
</ul>

<h2>Current published entry points</h2>

<h3><code>src/cli.rs</code></h3>

<p>
The main command-line surface. It accepts:
</p>

<ul>
  <li>headless execution by default or through <code>run</code>,</li>
  <li>browser-host UI through <code>ui</code>.</li>
</ul>

<p>From <code>Implementations/Reference/Runtime/rust/</code>:</p>

<pre><code>cargo test
cargo run -- 3
cargo run -- ui
cargo run -- ui --host 127.0.0.1 --port 8080 --no-open-browser</code></pre>

<h3><code>src/runtime.rs</code> and <code>src/execute.rs</code></h3>

<p>
These files hold the bounded runtime core and the headless execution path.
The current runtime validates the contract family, package shape, host-binding requirements,
widget classes, property writes, and the Example 05/06/07/08/09/10 execution models before producing runtime result artifacts.
</p>

<h3><code>src/ui.rs</code></h3>

<p>
Browser-host realization for the same runtime core. The current HTML rendering path exposes panel titles,
widget captions, SVG asset routes, and state JSON surfaces while consuming the published <code>.wfrog</code> package and Default realization assets.
</p>

<h2>Current bounded surface</h2>

<ul>
  <li>backend family <code>reference_host_runtime_ui_binding</code>,</li>
  <li>one contract unit named <code>main</code>,</li>
  <li>one public input <code>input_value : u16</code>,</li>
  <li>one public output <code>result : u16</code>,</li>
  <li>one explicit state carrier based on <code>frog.core.delay</code>,</li>
  <li>exactly five loop iterations,</li>
  <li>Example 05 Numeric control/indicator classes, Example 06 Boolean control/indicator classes, Example 07 String control/indicator classes, Example 08 Enum control/indicator classes, Example 09 Path control/indicator classes, and Example 10 Button plus Boolean indicator classes,</li>
  <li>bounded widget properties required by the published <code>.wfrog</code> packages and runtime contracts.</li>
</ul>

<p>
The current accepted slice also requires the host binding to advertise:
<code>window</code>, <code>basic_widget_rendering</code>, <code>property_write</code>,
<code>widget_value_binding</code>, and <code>widget_reference_binding</code>.
The Rust runtime reads those requirements from the emitted contract and the accepted package shape before execution begins.
</p>

<h2>Inputs and outputs</h2>

<h3>Inputs</h3>

<ul>
  <li>The emitted contract artifact under <code>Implementations/Reference/ContractEmitter/examples/</code>.</li>
  <li>The Example 05, Example 06, Example 07, Example 08, Example 09, and Example 10 <code>.wfrog</code> packages.</li>
  <li>The SVG assets referenced by those packages.</li>
  <li>The shared acceptance reading posture under <code>Implementations/Reference/Runtime/acceptance/</code>.</li>
</ul>

<h3>Outputs</h3>

<ul>
  <li>A headless runtime result artifact.</li>
  <li>A browser-host page driven by the same runtime core.</li>
  <li>A runtime snapshot JSON surface for the accepted Example 05, Example 06, Example 07, Example 08, Example 09, and Example 10 slices.</li>
</ul>

<p>
The browser-host path exists to close the first common runtime-family slice with a real visible UI. It does not claim native compiled UI closure.
</p>

<h2>Acceptance-driven tests</h2>

<p>From <code>Implementations/Reference/Runtime/rust/</code>:</p>

<pre><code>cargo test</code></pre>

<p>
The published Rust tests are acceptance-driven and should remain aligned with
<a href="../acceptance/Readme.md"><code>Implementations/Reference/Runtime/acceptance/Readme.md</code></a>.
</p>

<ul>
  <li><code>slice05_runtime.rs</code> checks headless acceptance for the bounded accumulator corridor.</li>
  <li><code>slice05_ui.rs</code> checks browser-host HTML rendering and snapshot acceptance for the numeric corridor.</li>
  <li><code>slice06_ui.rs</code>, <code>slice07_ui.rs</code>, <code>slice08_ui.rs</code>, <code>slice09_ui.rs</code>, and <code>slice10_ui.rs</code> check the Boolean, String, Enum, Path, and Button browser-host surfaces against the <code>.frog</code> instance plus <code>.wfrog</code> realization discipline.</li>
  <li><code>native_kernel_bridge.rs</code> checks dynamic native-kernel bridge parity for the published Example 05, Example 06, Example 07, Example 08, Example 09, and Example 10 surfaces when the local toolchain is available.</li>
</ul>

<h2>Relationship to the other runtime consumers</h2>

<p>
This directory should remain aligned with the Python and C/C++ consumers on:
</p>

<ul>
  <li>contract acceptance,</li>
  <li>package acceptance,</li>
  <li>execution semantics for the bounded accumulator, Boolean roundtrip, String roundtrip, Enum roundtrip, Path roundtrip, and Button press-to-Boolean slices,</li>
  <li>the minimal widget-property surfaces,</li>
  <li>the browser-host UI shapes for Examples 05, 06, 07, 08, 09, and 10,</li>
  <li>the shared acceptance artifacts for the runtime family.</li>
</ul>

<p>
The parent runtime-family definition is documented in
<a href="../Readme.md"><code>Implementations/Reference/Runtime/Readme.md</code></a>.
</p>

<h2>Non-goals</h2>

<ul>
  <li>General runtime support for arbitrary contracts.</li>
  <li>Widget-law ownership.</li>
  <li>Compiler-family responsibilities.</li>
  <li>Native compiled UI closure.</li>
</ul>

<h2>Summary</h2>

<p>
Read this directory as a full Rust consumer for the current runtime family:
</p>

<pre><code>contract + .wfrog + SVG assets
=&gt; Rust runtime core
=&gt; headless result or browser-host UI</code></pre>
