<p align="center">
  <img src="FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">🐸 FROG — Free Open Graphical Language</h1>

<p align="center">
  <strong>Free Open Graphical Dataflow Programming Language</strong><br/>
  FROG is an open, hardware-agnostic graphical dataflow programming language designed to describe computation as explicit executable graphs while remaining accessible, inspectable, portable, auditable, modular, and scalable across heterogeneous execution targets.
</p>

<p align="center">
  <strong>FROG opens the language layer itself:</strong><br/>
  canonical source, validated meaning, execution-facing FIR, lowering, backend contracts, runtime bridges, and compiler bridges.
</p>

<p align="center">
  In the generative AI era, software production becomes abundant, but accountable understanding becomes scarce.<br/>
  FROG is designed to make generated or human-authored logic structured, graphical, inspectable, controllable, and governable.
</p>

<p align="center">
  <a href="#start-here">Start here</a> •
  <a href="#positioning">Positioning</a> •
  <a href="#execution-architecture">Execution architecture</a> •
  <a href="#repository-map">Repository map</a> •
  <a href="#license">License</a>
</p>

<hr/>

<h2 id="start-here">Start here</h2>

<table>
  <thead>
    <tr>
      <th>Read this</th>
      <th>For</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><a href="./FROG-Strategy.md">FROG Strategy and Positioning</a></td>
      <td>FROG definition, GO HW continuity, campaign priority, AI-era positioning, and dataflow rationale.</td>
    </tr>
    <tr>
      <td><a href="./AI-Generation-Inspectability-Orville.md">AI generation vs inspectability Orville chart</a></td>
      <td>Visible root-level entry point for the AI-era positioning chart.</td>
    </tr>
    <tr>
      <td><a href="./Strategy/AI-Generation-Inspectability-Orville.md">AI Orville chart explanation</a></td>
      <td>Detailed non-normative explanation of why FROG <code>(.frog / JSON)</code> targets both AI generation compatibility and AI inspectability.</td>
    </tr>
    <tr>
      <td><a href="./Strategy/FormatFirstNotIdeFirst.md">Format-first, not IDE-first</a></td>
      <td>Why FROG's durable asset is the public source-to-FIR-to-contract pipeline rather than one IDE product.</td>
    </tr>
    <tr>
      <td><a href="./FROG-Architecture.md">FROG Architecture</a></td>
      <td>Source, FIR, lowering, runtime/compiler, observability, targets, security, interoperability, and language/tooling separation.</td>
    </tr>
    <tr>
      <td><a href="./FROG-Repository-Guide.md">FROG Repository Guide</a></td>
      <td>Repository structure, public specification boundaries, documentation map, governance, and contribution/licensing pointers.</td>
    </tr>
    <tr>
      <td><a href="./FROG-Project-Status.md">FROG Project Status</a></td>
      <td>Published repository state, examples, conformance coverage, library/profile surfaces, and maturity notes.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2 id="positioning">Positioning</h2>

<p>
FROG is a public language specification, not a single IDE, runtime, compiler, device target, or vendor product. It defines a graphical dataflow language layer where source, validated meaning, execution-facing FIR, lowering, runtime contracts, compiler contracts, widgets, libraries, profiles, and conformance can be inspected independently.
</p>

<p align="center">
  <img src="frog-orville-chart.png" alt="FROG positioning chart" width="640" />
</p>

<p align="center">
  <img src="./assets/strategy/frog-ai-generation-inspectability-orville.png" alt="FROG AI generation compatibility vs AI inspectability Orville chart" width="640" />
</p>

<p align="center">
  <em>
    In the generative AI era, FROG targets the upper-right quadrant where machine-generatable structured source meets graph-level inspection, validation, and governance.
  </em>
</p>

<p>
The long-form strategic explanation is maintained in <a href="./FROG-Strategy.md">FROG Strategy and Positioning</a>. The dedicated AI generation compatibility vs AI inspectability chart explanation is maintained in <a href="./Strategy/AI-Generation-Inspectability-Orville.md">AI Generation Compatibility vs AI Inspectability Orville Chart</a>.
</p>

<hr/>

<h2 id="execution-architecture">Execution architecture</h2>

<p>
A conforming FROG ecosystem separates authoring, canonical source, structural validity, validated program meaning, canonical open execution-facing representation, lowering, runtime/compiler consumers, target execution, and source-aligned observability.
</p>

<pre><code>.frog source
  -&gt; loadability
  -&gt; structural validation
  -&gt; semantic validation
  -&gt; validated program meaning
  -&gt; FIR / Execution IR
  -&gt; lowering
  -&gt; backend contract
  -&gt; runtime-family and/or compiler-family consumption</code></pre>

<p>
The reference execution architecture is preserved in <a href="./FROG-Architecture.md#execution-architecture">FROG Architecture</a>. The standalone ASCII pipeline page is available in <a href="./ExecutionPipelineDiagram.md">End-to-End Execution Pipeline Diagram</a>.
</p>

<hr/>

<h2 id="repository-map">Repository map</h2>

<table>
  <thead>
    <tr>
      <th>Path</th>
      <th>Purpose</th>
    </tr>
  </thead>
  <tbody>
    <tr><td><a href="./Expression/Readme.md"><code>Expression/</code></a></td><td>Canonical source and structural validity.</td></tr>
    <tr><td><a href="./Language/Readme.md"><code>Language/</code></a></td><td>Validated program meaning.</td></tr>
    <tr><td><a href="./IR/Readme.md"><code>IR/</code></a></td><td>Open execution-facing representation and downstream handoff posture.</td></tr>
    <tr><td><a href="./Libraries/Readme.md"><code>Libraries/</code></a></td><td>Public libraries, widgets, and Default realizations.</td></tr>
    <tr><td><a href="./Profiles/Readme.md"><code>Profiles/</code></a></td><td>Optional capability/profile surfaces.</td></tr>
    <tr><td><a href="./IDE/Readme.md"><code>IDE/</code></a></td><td>IDE-facing concepts, authoring, observability, and inspection.</td></tr>
    <tr><td><a href="./Examples/Readme.md"><code>Examples/</code></a></td><td>Public examples and validation-oriented dossiers.</td></tr>
    <tr><td><a href="./Conformance/Readme.md"><code>Conformance/</code></a></td><td>Accept / reject / preserve expectations.</td></tr>
    <tr><td><a href="./Implementations/Reference/Readme.md"><code>Implementations/Reference/</code></a></td><td>Non-normative reference implementation workspace.</td></tr>
    <tr><td><a href="./Strategy/Readme.md"><code>Strategy/</code></a></td><td>Non-normative strategic framing layer, including AI-era Orville positioning.</td></tr>
    <tr><td><a href="./Versioning/Readme.md"><code>Versioning/</code></a></td><td>Centralized specification-version governance and current-status reporting.</td></tr>
  </tbody>
</table>

<p>
The detailed repository structure and reading path are maintained in <a href="./FROG-Repository-Guide.md">FROG Repository Guide</a>.
</p>

<hr/>

<h2 id="license">License</h2>

<p>
This project is licensed under the <strong>Apache License 2.0</strong>. See <code>LICENSE</code> for details. External contributions are governed through <code>CONTRIBUTING.md</code> and <code>CLA.md</code>. Repository stewardship and governance are described in <code>GOVERNANCE.md</code>.
</p>

<p align="center">
  <strong>FROG — Free Open Graphical Language</strong><br/>
  Open graphical dataflow programming, specified as a language rather than owned as a product.
</p>

<hr/>
