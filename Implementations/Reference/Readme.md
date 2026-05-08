<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Reference Implementation Workspace</h1>

<p align="center">
  <strong>Non-normative executable workspace for exercising published FROG corridors from source to runtime and compiler-family proof paths</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Overview</h2>

<p>
This directory contains the non-normative reference implementation workspace for FROG.
It exists to make selected repository-visible corridors executable and inspectable without turning implementation convenience into normative language law.
</p>

<p>
The workspace protects a numbered progression of examples and the published widget / Default realization layer:
</p>

<pre><code>Libraries/Widgets/
Libraries/Realizations/Default/
  -&gt; WidgetValidator

Examples 01–05
  .frog
    -&gt; source-pattern FIR derivation
    -&gt; FIR unit.kind
    -&gt; lowering rule
    -&gt; lowered_unit.kind
    -&gt; backend contract unit.kind
    -&gt; runtime executor kind
    -&gt; runtime acceptance snapshot
    -&gt; LLVM proof
</code></pre>

<p>
Example 05 remains the primary applicative UI/state/runtime/native corridor.
Examples 01–04 provide smaller executable proof slices for pure arithmetic, widget values, UI property writes, and explicit feedback state.
</p>

<hr/>

<h2>Current Reference Corridor Discipline</h2>

<p>
The current reference workspace is organized around explicit rule handoffs rather than example-name dispatch:
</p>

<pre><code>.frog source pattern
  -&gt; FIR unit.kind
  -&gt; lowered_unit.kind
  -&gt; backend contract unit.kind
  -&gt; runtime executor kind
  -&gt; runtime snapshot

lowered_unit.kind
  -&gt; LLVM proof emitter
  -&gt; module.ll
</code></pre>

<table>
  <thead>
    <tr>
      <th>Stage</th>
      <th>Dispatch surface</th>
      <th>Reference documentation</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Deriver</td>
      <td>Recognized source pattern</td>
      <td><code>Implementations/Reference/Deriver/rules/Readme.md</code></td>
    </tr>
    <tr>
      <td>Lowerer</td>
      <td><code>FIR units[0].kind</code></td>
      <td><code>Implementations/Reference/Lowerer/rules/Readme.md</code></td>
    </tr>
    <tr>
      <td>ContractEmitter</td>
      <td><code>lowered_units[0].kind</code></td>
      <td><code>Implementations/Reference/ContractEmitter/</code></td>
    </tr>
    <tr>
      <td>Runtime</td>
      <td><code>contract.units[0].kind</code></td>
      <td><code>Implementations/Reference/Runtime/Readme.md</code></td>
    </tr>
    <tr>
      <td>LLVM proof path</td>
      <td><code>lowered_units[0].kind</code></td>
      <td><code>Implementations/Reference/LLVM/rules/Readme.md</code></td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Published Workspace Shape</h2>

<pre><code>Implementations/Reference/
├── ArtifactChecks/
├── CLI/
├── ContractEmitter/
├── Deriver/
├── Loader/
├── Lowerer/
├── Runtime/
├── UIHost/
├── LLVM/
├── Validator/
├── WidgetValidator/
├── check_reference_workspace.py
├── checks.md
├── pipeline.md
└── Readme.md
</code></pre>

<hr/>

<h2>Stage Responsibilities</h2>

<ul>
  <li><code>ArtifactChecks/</code> verifies that the artifacts announced by the executable corridor exist and have the expected high-level identity.</li>
  <li><code>WidgetValidator/</code> protects the published widget class-law and Default realization layer.</li>
  <li><code>Deriver/</code> derives FIR from supported canonical <code>.frog</code> source patterns.</li>
  <li><code>Lowerer/</code> lowers supported FIR unit kinds into backend- or compiler-facing lowered unit kinds.</li>
  <li><code>ContractEmitter/</code> emits backend contracts from published lowered unit kinds.</li>
  <li><code>Runtime/</code> checks contract-unit-kind-driven runtime acceptance.</li>
  <li><code>LLVM/</code> checks lowered-unit-kind-to-native proof modules.</li>
</ul>

<hr/>

<h2>Primary Checks</h2>

<pre><code>python Implementations/Reference/check_reference_workspace.py
python Implementations/Reference/check_reference_workspace.py --widget-layer-only
python Implementations/Reference/check_reference_workspace.py --include-pytest
python Implementations/Reference/check_reference_workspace.py --include-llvm-build
</code></pre>

<p>
The default workspace command includes widget-layer validation through the Examples 01–05 pipeline.
The <code>--widget-layer-only</code> command exists for fast validation when editing widget class-law documents, Default realization documents, manifests, or SVG resources.
The <code>--include-pytest</code> command additionally protects source-pattern derivation, FIR-unit-kind lowering, lowered-unit-kind contract emission, contract-unit-kind runtime execution, lowered-unit-kind LLVM emission, and unsupported-pattern / unsupported-kind failure behavior.
</p>

<hr/>

<h2>Continuous Integration</h2>

<p>
The repository publishes a GitHub Actions workflow for the reference workspace:
</p>

<pre><code>.github/workflows/reference-workspace.yml</code></pre>

<p>
The workflow runs the full reference workspace check, the reference pytest suite, and the widget-layer-only check on relevant repository changes.
</p>

<hr/>

<h2>Ownership Boundary</h2>

<p>
This workspace consumes the published specification layers.
It does not define FROG source law, semantic law, FIR law, widget law, realization law, runtime law, backend contract law, or LLVM backend law.
</p>

<pre><code>reference implementation
  !=
language definition
</code></pre>
