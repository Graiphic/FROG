<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference Pipeline</h1>

<p align="center">
  <strong>First executable reference pipeline for the non-normative FROG reference implementation workspace</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Overview</h2>

<p>
The current reference workspace now has two complementary check surfaces.
</p>

<ul>
  <li>Examples <code>01</code> through <code>04</code> are reproducible through source-to-FIR and FIR-to-lowering checks.</li>
  <li>Example <code>05</code> remains the full corridor through source, FIR, lowering, backend contract, runtime acceptance, and LLVM module emission.</li>
</ul>

<hr/>

<h2>Example 05 Full Pipeline</h2>

<pre><code>python Implementations/Reference/Pipeline/check_example05_pipeline.py</code></pre>

<hr/>

<h2>Full Workspace Check</h2>

<pre><code>python Implementations/Reference/check_reference_workspace.py --include-pytest</code></pre>

<p>
The pytest pass covers the derivation and lowering slices for Examples <code>01</code> through <code>05</code>.
</p>

<hr/>

<h2>Boundary</h2>

<p>
These checks are non-normative.
They protect repository-visible artifacts and do not define FROG semantics.
</p>
