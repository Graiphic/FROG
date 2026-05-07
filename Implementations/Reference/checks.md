<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference Workspace Checks</h1>

<p align="center">
  <strong>Repository-quality checks for the current FROG reference implementation workspace</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Primary Command</h2>

<pre><code>python Implementations/Reference/check_reference_workspace.py</code></pre>

<p>
This command runs the Examples 01–05 full pipeline.
Widget-layer validation is included by default through the pipeline.
</p>

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

<h2>Full Python Test Command</h2>

<pre><code>python -m pip install pytest
python Implementations/Reference/check_reference_workspace.py --include-pytest</code></pre>

<p>
This additionally runs the pytest suites for artifact checks, derivation, lowering, contract emission, generic runtime contract execution, LLVM, and pipeline coordination.
</p>

<p>
The current pytest surface protects, among other things:
</p>

<ul>
  <li>source-pattern FIR derivation selection,</li>
  <li>FIR-unit-kind lowering selection,</li>
  <li>lowered-unit-kind LLVM emission selection,</li>
  <li>unsupported pattern / unsupported kind failure behavior.</li>
</ul>

<hr/>

<h2>Runtime Contract Executor Check</h2>

<pre><code>python Implementations/Reference/Runtime/check_examples01_05_runtime_acceptance.py
python Implementations/Reference/Runtime/execute_reference_contract.py \
  --acceptance Implementations/Reference/Runtime/acceptance/example01_pure_addition.acceptance.json \
  --check
</code></pre>

<hr/>

<h2>LLVM Native Build Proof</h2>

<pre><code>python Implementations/Reference/check_reference_workspace.py --include-llvm-build</code></pre>

<p>
This requires <code>clang</code>.
It verifies the native LLVM proof dossiers for Examples 01–05.
Widget-layer validation remains enabled unless explicitly skipped.
</p>

<hr/>

<h2>Continuous Integration</h2>

<p>
The repository publishes a GitHub Actions workflow at:
</p>

<pre><code>.github/workflows/reference-workspace.yml</code></pre>

<p>
The workflow currently runs three jobs:
</p>

<ul>
  <li><code>Reference workspace</code> — runs the primary Examples 01–05 workspace check.</li>
  <li><code>Reference pytest</code> — installs <code>pytest</code> and runs <code>check_reference_workspace.py --include-pytest</code>.</li>
  <li><code>Widget layer only</code> — runs the widget-layer validator directly.</li>
</ul>

<hr/>

<h2>Explicit Widget-Validator Skip</h2>

<pre><code>python Implementations/Reference/check_reference_workspace.py --skip-widget-validator</code></pre>

<p>
This skips widget-layer validation inside the Examples 01–05 pipeline.
It should be used only for narrow debugging of unrelated pipeline stages.
The default repository-quality posture is to keep widget validation enabled.
</p>
