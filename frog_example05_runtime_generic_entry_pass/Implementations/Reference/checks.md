<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference Workspace Checks</h1>

<p align="center">
  <strong>Repository-quality checks for the current FROG reference implementation workspace</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Primary command</h2>

<pre><code>python Implementations/Reference/check_reference_workspace.py</code></pre>

<p>
This command runs the Examples 01–05 full pipeline with widget-layer validation enabled.
</p>

<hr/>

<h2>Full Python test command</h2>

<pre><code>python Implementations/Reference/check_reference_workspace.py --include-pytest</code></pre>

<p>
This additionally runs the pytest suites for artifact checks, derivation, lowering, contract emission, generic runtime contract execution, LLVM, and pipeline coordination.
</p>

<hr/>

<h2>Runtime contract executor checks</h2>

<pre><code>python Implementations/Reference/Runtime/check_examples01_05_runtime_acceptance.py
python Implementations/Reference/Runtime/check_examples01_05_runtime_acceptance.py --skip-specialized-example05
python Implementations/Reference/Runtime/execute_reference_contract.py \
  --acceptance Implementations/Reference/Runtime/acceptance/example05_runtime_family.acceptance.json \
  --check
</code></pre>

<hr/>

<h2>LLVM native build proof</h2>

<pre><code>python Implementations/Reference/check_reference_workspace.py --include-llvm-build</code></pre>

<p>
This requires <code>clang</code>.
It verifies the native LLVM proof dossiers for Examples 01–05.
</p>
