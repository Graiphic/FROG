<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference Workspace Checks</h1>

<p align="center">
  <strong>Repository-quality checks for the current FROG reference implementation workspace</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This document describes the non-normative repository checks used to keep the current Examples 01–05 executable corridor coherent.
These checks do not define FROG semantics.
They verify that the published reference artifacts remain aligned.
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
This additionally runs the pytest suites for:
</p>

<ul>
  <li>artifact preflight checks,</li>
  <li>Deriver,</li>
  <li>Lowerer,</li>
  <li>ContractEmitter,</li>
  <li>Runtime,</li>
  <li>LLVM module emission,</li>
  <li>Pipeline.</li>
</ul>

<hr/>

<h2>LLVM native build proof</h2>

<pre><code>python Implementations/Reference/check_reference_workspace.py --include-llvm-build</code></pre>

<p>
This requires <code>clang</code>.
It verifies the native LLVM proof dossiers for Examples 01–05.
</p>

<hr/>

<h2>Direct corridor command</h2>

<pre><code>python Implementations/Reference/Pipeline/check_examples01_05_full.py --include-widget-validator</code></pre>

<p>
With native builds:
</p>

<pre><code>python Implementations/Reference/Pipeline/check_examples01_05_full.py --include-widget-validator --include-llvm-build</code></pre>

<hr/>

<h2>CI</h2>

<p>
The repository-level GitHub Actions workflow is:
</p>

<pre><code>.github/workflows/reference-pipeline.yml</code></pre>

<p>
It runs the reference workspace checks and the LLVM native proof on Ubuntu.
</p>
