<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference Artifact Checks</h1>

<p align="center">
  <strong>Repository-quality preflight checks for the published Examples 01-10 executable corridor</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This directory contains non-normative repository-quality checks.
They verify that the artifacts announced by the examples and reference pipeline are actually present before deeper derivation, lowering, contract, runtime, and LLVM checks run.
</p>

<hr/>

<h2>Command</h2>

<pre><code>python Implementations/Reference/ArtifactChecks/check_examples01_10_artifacts.py</code></pre>

<hr/>

<h2>Boundary</h2>

<p>
These checks do not define FROG.
They protect repository coherence.
</p>
