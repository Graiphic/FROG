<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

# Reference Runtime (Python)

<p>Python realization of the published <code>reference_host_runtime_ui_binding</code> family.</p>

<p>
  Repository governance and publication state are centralized in
  <a href="../../../../Versioning/Readme.md"><code>Versioning/Readme.md</code></a>.
</p>

<hr/>

<h2>Directory navigation</h2>

<pre><code>Implementations/Reference/Runtime/python/
├── Readme.md
├── __init__.py
├── cli.py
├── execute_contract.py
├── run_slice05_ui.py
├── runtime_core.py
├── ui_runtime.py
└── tests/
    ├── __pycache__/                    [generated, not intended source]
    ├── test_runtime_slice05.py        [acceptance-driven]
    ├── test_runtime_ui_slice05.py     [acceptance-driven]
    ├── test_slice05_contract.py       [legacy smoke test]
    └── test_slice05_ui_runtime.py     [legacy smoke test]</code></pre>

<p>
Generated cache directories such as <code>tests/__pycache__/</code> are not part of the intended source surface of this runtime and should not be versioned.
</p>

<h2>Role</h2>

<p>
This directory contains the Python consumer for the published Example 05 runtime corridor.
It accepts the emitted backend contract, loads the published <code>.wfrog</code> package,
resolves the referenced SVG assets, executes the bounded kernel, and can expose the panel through a minimal browser-host UI.
</p>

<p>
This directory is downstream from:
</p>

<pre><code>main.frog
  -&gt; main.fir.json
  -&gt; main.lowering.json
  -&gt; reference_host_runtime_ui_binding contract</code></pre>

<p>
It is not a source parser, not a lowering stage, and not a language-definition layer.
</p>

<h2>Current published entry points</h2>

<h3><code>cli.py</code></h3>

<p>
The main entry point for this directory.
It exposes two modes:
</p>

<ul>
  <li><code>run</code> — headless contract + <code>.wfrog</code> execution.</li>
  <li><code>ui</code> — browser-host UI service for the same runtime core.</li>
</ul>

<p>Examples from the repository root:</p>

<pre><code>python -m Implementations.Reference.Runtime.python.cli run 3
python -m Implementations.Reference.Runtime.python.cli ui
python -m Implementations.Reference.Runtime.python.cli ui --host 127.0.0.1 --port 8080 --no-open-browser</code></pre>

<h3><code>run_slice05_ui.py</code></h3>

<p>
Convenience wrapper that starts the same browser-host UI path as <code>cli.py ui</code>.
It remains useful as a small example-specific launcher, but <code>cli.py</code> is the primary surface.
</p>

<h3><code>execute_contract.py</code> and <code>runtime_core.py</code></h3>

<p>
These files hold the actual execution path. The current runtime core validates the bounded Example 05 contract,
checks the accepted package shape, enforces the bounded execution model, and produces a runtime result artifact.
</p>

<h3><code>ui_runtime.py</code></h3>

<p>
Browser-host realization for the same runtime core. The published tests exercise HTML rendering,
SVG asset routing, and the JSON snapshot endpoint.
</p>

<h2>Current bounded surface</h2>

<p>
The Python runtime is intentionally strict. The currently published bounded surface is:
</p>

<ul>
  <li>backend family <code>reference_host_runtime_ui_binding</code>,</li>
  <li>one contract unit named <code>main</code>,</li>
  <li>one public input <code>input_value : u16</code>,</li>
  <li>one public output <code>result : u16</code>,</li>
  <li>one explicit state carrier based on <code>frog.core.delay</code>,</li>
  <li>exactly five loop iterations,</li>
  <li>two widget classes: <code>frog.widgets.numeric_control</code> and <code>frog.widgets.numeric_indicator</code>,</li>
  <li>five supported widget properties: <code>value</code>, <code>label</code>, <code>visible</code>, <code>enabled</code>, and <code>foreground_color</code>,</li>
  <li>two bounded method accepts: <code>ctrl_input.focus</code> and <code>ind_result.reset_to_default_style</code>.</li>
</ul>

<p>
The current accepted slice also requires the host binding to advertise:
<code>window</code>, <code>basic_widget_rendering</code>, <code>property_write</code>,
<code>widget_value_binding</code>, and <code>widget_reference_binding</code>.
The Python runtime reads those requirements from the emitted contract and the accepted package shape before execution begins.
</p>

<h2>Inputs and outputs</h2>

<h3>Inputs</h3>

<ul>
  <li>The emitted contract artifact under <code>Implementations/Reference/ContractEmitter/examples/</code>.</li>
  <li>The Example 05 package <code>Examples/05_bounded_ui_accumulator/ui/accumulator_panel.wfrog</code>.</li>
  <li>The SVG assets referenced by that package.</li>
  <li>The shared acceptance reading posture under <code>Implementations/Reference/Runtime/acceptance/</code>.</li>
</ul>

<h3>Outputs</h3>

<ul>
  <li>A headless runtime result artifact.</li>
  <li>A browser-host page driven by the same runtime core.</li>
  <li>A runtime snapshot JSON surface for the accepted Example 05 slice.</li>
</ul>

<h2>Acceptance-driven tests</h2>

<p>From the repository root:</p>

<pre><code>python -m pytest Implementations/Reference/Runtime/python/tests/test_runtime_slice05.py
python -m pytest Implementations/Reference/Runtime/python/tests/test_runtime_ui_slice05.py</code></pre>

<p>
The published Python tests are acceptance-driven and should remain aligned with
<a href="../acceptance/Readme.md"><code>Implementations/Reference/Runtime/acceptance/Readme.md</code></a>.
</p>

<ul>
  <li><code>test_runtime_slice05.py</code> checks headless acceptance for the bounded runtime corridor.</li>
  <li><code>test_runtime_ui_slice05.py</code> checks browser-host HTML rendering and snapshot acceptance for the same corridor.</li>
</ul>

<h2>Legacy tests still visible in the current tree</h2>

<p>
The current published Python test directory still contains:
</p>

<ul>
  <li><code>test_slice05_contract.py</code></li>
  <li><code>test_slice05_ui_runtime.py</code></li>
</ul>

<p>
These are legacy pre-acceptance smoke tests. They are useful as transitional checks, but they are not the family-level source of truth anymore.
The acceptance-driven tests above are the primary published line that should remain aligned with the shared acceptance artifacts.
</p>

<h2>Relationship to the other runtime consumers</h2>

<p>
This directory should remain aligned with the Rust and C/C++ consumers on:
</p>

<ul>
  <li>contract acceptance,</li>
  <li>package acceptance,</li>
  <li>execution semantics for the bounded accumulator slice,</li>
  <li>the minimal widget-property surface,</li>
  <li>the browser-host UI shape for Example 05,</li>
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
Read this directory as the Python consumer of the current runtime family:
</p>

<pre><code>contract + .wfrog + SVG assets
=&gt; Python runtime core
=&gt; headless result or browser-host UI</code></pre>
