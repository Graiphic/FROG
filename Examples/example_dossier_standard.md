<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Example Dossier Standard</h1>

<p align="center">
  <strong>Common organization for public FROG examples</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
A FROG example is a focused, inspectable dossier.
It is not the owner of language semantics.
It demonstrates how a specific language feature, widget surface, operator,
library call, structure, compiler corridor, or runtime-facing contract is
represented through the public FROG artifact chain.
</p>

<p>
Each example should make one primary idea easy to inspect.
If an example combines several already validated ideas, it should name the new
composition being tested and should reuse the accepted baselines instead of
redefining them.
</p>

<hr/>

<h2>Minimum Dossier Shape</h2>

<p>
A numbered example directory should contain the smallest set of files needed to
make its public claim inspectable:
</p>

<table>
  <thead>
    <tr>
      <th>File or directory</th>
      <th>Role</th>
      <th>When expected</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>Readme.md</code></td>
      <td>Human explanation of the example, its focus, artifact path, validation posture, and limitations.</td>
      <td>Always.</td>
    </tr>
    <tr>
      <td><code>main.frog</code></td>
      <td>Canonical FROG source for the example.</td>
      <td>Always, unless the example is explicitly a documentation-only mirror.</td>
    </tr>
    <tr>
      <td><code>main.fir.json</code></td>
      <td>Derived FIR / execution-facing representation of the source.</td>
      <td>Required for accepted executable examples.</td>
    </tr>
    <tr>
      <td><code>main.lowering.json</code></td>
      <td>Lowered execution/backend handoff for the example.</td>
      <td>Required for accepted executable examples.</td>
    </tr>
    <tr>
      <td><code>Implementations/Reference/LLVM/examples/&lt;example&gt;/kernel.ll</code></td>
      <td>LLVM IR proof for the current executable example validation corridor.</td>
      <td>Required for accepted executable examples in the current LLVM/native line.</td>
    </tr>
    <tr>
      <td><code>Implementations/Reference/LLVM/examples/&lt;example&gt;/native_kernel_manifest.json</code></td>
      <td>Native/provider manifest declaring ABI, entry symbols, inputs, outputs, artifacts, and provider dependencies where relevant.</td>
      <td>Required for accepted executable examples in the current LLVM/native line.</td>
    </tr>
    <tr>
      <td><code>ui/*.wfrog</code></td>
      <td>Front-panel realization package references, Default asset references, and host capability declarations.</td>
      <td>When the example has a front panel or widget surface.</td>
    </tr>
    <tr>
      <td><code>assets/</code></td>
      <td>Example-owned input fixtures such as sample images or data files.</td>
      <td>Only for true example data fixtures, not duplicated Default widget skins.</td>
    </tr>
  </tbody>
</table>

<p>
Compiler, conformance, or library examples may add specialized artifacts when
they are the subject of the example.
Those artifacts should be linked from the example README and should not obscure
the primary source-to-artifact story.
</p>

<p>
For the current executable example line, the acceptance corridor is complete
only when it reaches LLVM IR, a native artifact or native artifact build
material, a manifest, and runtime-host consumption. Documentation-only,
negative conformance, or explicitly exploratory examples may stop earlier, but
their README must state that limitation instead of implying executable closure.
This does not make LLVM the identity of FROG; LLVM remains one backend producer
of native artifacts.
</p>

<hr/>

<h2>README Expectations</h2>

<p>
An example README should identify:
</p>

<ul>
  <li>the example title and numbered directory;</li>
  <li>the single primary behavior or concept under test;</li>
  <li>the source artifact path, normally starting at <code>main.frog</code>;</li>
  <li>the public artifact chain for executable examples: <code>.frog -&gt; FIR -&gt; lowering -&gt; LLVM IR -&gt; native artifact + manifest -&gt; runtime host</code>;</li>
  <li>front-panel packages and Default realization assets when widgets are involved;</li>
  <li>the stated validation level, without claiming broader runtime or compiler completeness than the example proves;</li>
  <li>known limits, especially when the example is post-boundary public material whose runtime implementation continues privately.</li>
</ul>

<p>
The README should be useful to a reader who has not seen the chat that created
the example.
It should explain what changed, what is reused, and what evidence the directory
provides.
</p>

<hr/>

<h2>Ownership Boundaries</h2>

<p>
Examples must preserve the public FROG ownership model:
</p>

<ul>
  <li><code>.frog</code> owns diagram logic, source identity, front-panel instances, widget layout, bindings, initial values, labels, and instance-level visual overrides.</li>
  <li><code>.wfrog</code> owns realization package references, Default asset references, supported classes, and host capability declarations.</li>
  <li>Default realization assets own reusable widget visual grammar, SVG templates, public parts, anchors, bindings, internal geometry, and default visual behavior.</li>
  <li>Runtime or host material consumes the published artifacts; it does not become the semantic definition of the example.</li>
</ul>

<p>
For non-widget examples, the same principle applies: the example should show the
source truth, the public derived artifacts, and the handoff boundary instead of
hiding meaning inside an implementation-specific shortcut.
</p>

<p>
For widget examples, visible borders are part of the inspectable realization
contract. Any border, frame, outline, selection rectangle, cell boundary,
scrollbar track, or equivalent stroke-like surface should keep one uniform
thickness on every side after resize unless the widget class deliberately
publishes side-specific border properties. The width, color, visibility, and
style of such surfaces should be source-owned or realization-published, not
hardcoded by the runtime host.
</p>

<p>
When a bordered widget face, value region, text region, state face, or similar
published fill surface is meant to touch its surrounding border, the Default SVG
and rendered host surface must not leave an unintended inner gap. The fill
surface should either meet the stroke inner edge exactly or extend under a
transparent / stroke-only frame so the visible result remains one clean bordered
surface.
</p>

<hr/>

<h2>Browser-Hosted Examples</h2>

<p>
When an example is launched through a browser host for validation, the page
should make the example inspectable without treating the browser as the FROG
runtime identity.
</p>

<p>
The browser-hosted surface should expose:
</p>

<ul>
  <li>the example title and validation badges;</li>
  <li>the visible front panel or equivalent review surface;</li>
  <li>an explicit <code>Execute</code> action when the example requires running the diagram or manifest-backed contract;</li>
  <li><code>Read</code> only when the example specifically validates program-read semantics;</li>
  <li>a collapsed-by-default source and execution artifact inspector with formatted readable content and raw routes for <code>state.json</code>, the owning <code>.frog</code>, directly consumed <code>.wfrog</code> packages, and FIR/lowering artifacts when present.</li>
</ul>

<p>
Widget edits are front-panel state changes.
They should not silently execute the diagram unless the example explicitly
defines that interaction as the execution event.
</p>

<hr/>

<h2>Example Families</h2>

<p>
The same dossier pattern applies beyond widget work:
</p>

<table>
  <thead>
    <tr>
      <th>Example family</th>
      <th>What it should emphasize</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Widget examples</td>
      <td>Source-owned instances, <code>.wfrog</code> realization references, Default SVG assets, public parts, bindings, and visible behavior.</td>
    </tr>
    <tr>
      <td>Operator examples</td>
      <td>Source operator identity, type/value behavior, FIR preservation, lowering behavior, and backend/runtime evidence where in scope.</td>
    </tr>
    <tr>
      <td>Structure examples</td>
      <td>Explicit structure boundaries, regions, terminals, state participation, and preservation across FIR/lowering.</td>
    </tr>
    <tr>
      <td>Library examples</td>
      <td>Explicit <code>frog.xxx</code> function identity, provider or dependency boundary, manifest declaration, and dataflow-visible inputs/outputs.</td>
    </tr>
    <tr>
      <td>Compiler/conformance examples</td>
      <td>Positive and negative cases, expected diagnostics, accepted/rejected source patterns, and artifact preservation claims.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Acceptance Posture</h2>

<p>
An example is complete at the level it honestly proves.
It may be a public specification-facing example, a bounded public reference
runtime example, a compiler corridor example, a widget progression example, or a
future conformance case.
The README and artifacts should state that posture precisely.
</p>

<p>
For the current repository boundary, Examples <code>01</code> through
<code>15</code> define the public reference runtime closure.
Examples after that boundary may remain public as source, specification,
widget, library, conformance, or design-progression material while Graiphic
runtime implementation continues privately unless a later public reference
surface is explicitly promoted.
</p>
