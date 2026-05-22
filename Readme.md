<div align="center" class="go-pages-link" data-render-target="github">
  <a href="https://graiphic.github.io/FROG/">
    <img src="./assets/open-github-pages-banner.svg" alt="Open the GitHub Pages version" width="640" />
  </a>
</div>

<p align="center">
  <img src="FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">🐸 FROG — Free Open Graphical Language</h1>

<p align="center">
  <strong>Free Open Graphical Dataflow Programming Language</strong><br/>
  FROG is an open, hardware-agnostic graphical dataflow programming language designed to describe computation as explicit executable graphs while remaining accessible, explicit, inspectable, portable, auditable, modular, and scalable across heterogeneous execution targets.
</p>

<p align="center">
  FROG aims to establish itself as the open-source standard foundation for graphical dataflow language infrastructure.
</p>

<p align="center">
  <strong>FROG attacks the structural lock-in of graphical industrial programming by opening the language layer itself:</strong><br/>
  canonical source, validated meaning, execution-facing FIR, lowering, backend contracts, runtime bridges, and compiler bridges.
</p>

<p align="center">
  In the generative AI era, software production becomes abundant, but accountable understanding becomes scarce.<br/>
  FROG is designed to make generated or human-authored logic structured, graphical, inspectable, controllable, and governable.
</p>

<p align="center">
  FROG can also carry optional source provenance through <code>ide.provenance</code>, allowing provenance-aware tools to distinguish unknown, human-direct, AI-generated, AI-assisted, imported, reviewed, stale, invalid, and untrusted source-object states without changing executable meaning.
</p>

<p align="center">
  Specification work initiated: <strong>8 March 2026</strong>
</p>

<p align="center">
  <a href="#start-here">Start here</a> •
  <a href="#positioning">Positioning</a> •
  <a href="#execution-architecture">Execution architecture</a> •
  <a href="#public-specification-and-implementation-boundary">Public boundary</a> •
  <a href="#repository-map">Repository map</a> •
  <a href="#published-repository-state">Published state</a> •
  <a href="#license">License</a>
</p>

<hr/>

<h2 id="start-here">Start here</h2>

<p>
This root README is the public entry point for FROG. It keeps the strategic
positioning, execution architecture reference, public/private boundary, repository
map, and current published state visible without forcing every reader through
the full long-form material.
</p>

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
      <td>Whitepaper links, FROG definition, GO HW continuity, campaign priority, AI-era positioning, and dataflow rationale.</td>
    </tr>
    <tr>
      <td><a href="./FROG-Architecture.md">FROG Architecture</a></td>
      <td>Program representation, FIR posture, runtime/compiler complementarity, original ASCII execution architecture, observability, targets, security, interoperability, and language/tooling separation.</td>
    </tr>
    <tr>
      <td><a href="./FROG-Repository-Guide.md">FROG Repository Guide</a></td>
      <td>What this repository defines, what it does not define, public specification boundaries, repository structure, documentation map, governance, and contribution/licensing pointers.</td>
    </tr>
    <tr>
      <td><a href="./FROG-Project-Status.md">FROG Project Status</a></td>
      <td>Published repository state, examples, conformance coverage, library/profile surfaces, and maturity notes.</td>
    </tr>
    <tr>
      <td><a href="./Examples/Readme.md">Examples</a></td>
      <td>The growing example catalogue and validation-oriented example documentation.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2 id="positioning">Positioning</h2>

<p>
FROG is a public language specification, not a single IDE, runtime, compiler,
device target, or vendor product. It defines a graphical dataflow language
layer where source, validated meaning, execution-facing FIR, lowering, runtime
contracts, compiler contracts, widgets, libraries, profiles, and conformance
can be inspected independently.
</p>

<p>
That distinction matters because graphical industrial programming has often
been locked inside product-specific file formats and execution stacks. FROG
opens the language layer so multiple tools, runtimes, compilers, and deployment
models can share durable public meaning.
</p>

<p align="center">
  <img src="frog-orville-chart.png" alt="FROG positioning chart" width="640" />
</p>

<p>
The long-form strategic explanation, including the whitepaper links and
AI-era accountability framing, is maintained in
<a href="./FROG-Strategy.md">FROG Strategy and Positioning</a>.
</p>

<hr/>

<h2 id="execution-architecture">Execution architecture</h2>

<p>
A conforming FROG ecosystem separates authoring, canonical source, structural
validity, validated program meaning, canonical open execution-facing
representation, lowering, runtime/compiler consumers, target execution, and
source-aligned observability.
</p>

<p>
The original ASCII execution architecture diagram remains the reference view.
It is preserved with the detailed explanation in
<a href="./FROG-Architecture.md#execution-architecture">FROG Architecture</a>.
</p>

<hr/>

<h2 id="public-specification-and-implementation-boundary">Public Specification and Implementation Boundary</h2>

<p>
The public FROG repository defines open language and ecosystem-facing material.
Graiphic may build proprietary runtimes, IDEs, product integrations, and
services around that public specification, but those private implementations do
not become required public language law.
</p>

<table>
  <thead>
    <tr>
      <th>Public FROG repository</th>
      <th>Private Graiphic implementation work</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>.frog</code>, FIR posture, conformance, public examples, public libraries, profiles, Default realizations, and specification-facing docs.</td>
      <td>Graiphic production runtime families, private runtime orchestration details, IDE implementation, product packaging, and internal project coordination.</td>
    </tr>
  </tbody>
</table>

<p>
The current public reference runtime closure remains bounded to Examples
<code>01</code> through <code>15</code>. Later public examples remain
specification-facing, widget-facing, conformance-facing, or design-progression
material unless a later public runtime surface is explicitly promoted.
</p>

<p>
See <a href="./FROG-Repository-Guide.md#public-specification-and-implementation-boundary">Public Specification and Implementation Boundary</a> for the full boundary statement.
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
    <tr>
      <td><a href="./Language/Readme.md"><code>Language/</code></a></td>
      <td>Canonical source and language-facing rules.</td>
    </tr>
    <tr>
      <td><a href="./Expression/Readme.md"><code>Expression/</code></a></td>
      <td>FROG expression/source shape.</td>
    </tr>
    <tr>
      <td><a href="./IR/Readme.md"><code>IR/</code></a></td>
      <td>Execution-facing intermediate representation posture.</td>
    </tr>
    <tr>
      <td><a href="./Libraries/Readme.md"><code>Libraries/</code></a></td>
      <td>Public libraries, widgets, and Default realizations.</td>
    </tr>
    <tr>
      <td><a href="./Profiles/Readme.md"><code>Profiles/</code></a></td>
      <td>Capability/profile surfaces such as interoperability profiles.</td>
    </tr>
    <tr>
      <td><a href="./Examples/Readme.md"><code>Examples/</code></a></td>
      <td>Public examples and validation-oriented dossiers.</td>
    </tr>
    <tr>
      <td><a href="./Conformance/Readme.md"><code>Conformance/</code></a></td>
      <td>Positive and negative conformance cases.</td>
    </tr>
    <tr>
      <td><a href="./Implementations/Reference/Readme.md"><code>Implementations/Reference/</code></a></td>
      <td>Public reference implementation and tooling material within the current public boundary.</td>
    </tr>
    <tr>
      <td><a href="./IDE/Readme.md"><code>IDE/</code></a></td>
      <td>Public IDE-facing specification concepts, not the private Graiphic Studio implementation.</td>
    </tr>
  </tbody>
</table>

<p>
The detailed repository structure and reading path are maintained in
<a href="./FROG-Repository-Guide.md">FROG Repository Guide</a>.
</p>

<hr/>

<h2 id="published-repository-state">Published repository state</h2>

<p>
The public repository currently publishes the open specification surface,
conformance-facing material, public examples through Example <code>24</code>,
Default widget realizations, public library/profile documentation, and the
reference implementation material that belongs inside the public boundary.
</p>

<p>
The examples table will continue to grow in <a href="./Examples/Readme.md">Examples</a>.
The root page intentionally keeps only the orientation-level status, while
<a href="./FROG-Project-Status.md">FROG Project Status</a> preserves the detailed
published-state and maturity notes that used to live here.
</p>

<hr/>

<h2 id="recommended-reading-path">Recommended reading path</h2>

<ol>
  <li>Start with this root README.</li>
  <li>Read <a href="./FROG-Strategy.md">FROG Strategy and Positioning</a> for why the language exists.</li>
  <li>Read <a href="./FROG-Architecture.md">FROG Architecture</a> for source, FIR, lowering, runtime/compiler, and observability posture.</li>
  <li>Read <a href="./FROG-Repository-Guide.md">FROG Repository Guide</a> for the detailed repository map.</li>
  <li>Use <a href="./Examples/Readme.md">Examples</a> and <a href="./Conformance/Readme.md">Conformance</a> for concrete validation material.</li>
  <li>Use <a href="./FROG-Project-Status.md">FROG Project Status</a> for the current published state.</li>
</ol>

<hr/>

<h2 id="license">License</h2>

<p>
This project is licensed under the <strong>Apache License 2.0</strong>.
See <code>LICENSE</code> for details.
</p>

<p>
External contributions are governed through the repository contribution process
and Contributor License Agreement requirements. See <code>CONTRIBUTING.md</code>
and <code>CLA.md</code>. Repository stewardship, governance direction, and ecosystem
positioning are described in <code>GOVERNANCE.md</code>.
</p>

<p align="center">
  <a href="https://cla-assistant.io/Graiphic/FROG">
    <img src="./assets/cla-assistant-badge.svg" alt="CLA Assistant" />
  </a>
</p>

<hr/>

<p align="center">
  <strong>FROG — Free Open Graphical Language</strong><br/>
  Open graphical dataflow programming, specified as a language rather than owned as a product.
</p>

<hr/>
