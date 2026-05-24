<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG AI Generation Compatibility vs AI Inspectability Orville Chart</h1>

<p align="center">
  <strong>Non-normative strategic positioning note</strong><br/>
  Why FROG targets the upper-right quadrant where AI-generatable source and AI-inspectable system structure meet.
</p>

<p align="center">
  <img src="../assets/strategy/frog-ai-generation-inspectability-orville.png" alt="FROG AI generation compatibility vs AI inspectability Orville chart" width="640" />
</p>

<hr/>

<h2>Purpose</h2>

<p>
This Orville chart complements the repository's existing positioning chart by focusing on a different AI-era trade-off: <strong>AI generation compatibility</strong> versus <strong>AI inspectability</strong>.
</p>

<p>
The strategic message is narrow and intentional: the post-AI bottleneck is not only the ability to produce more software artifacts. It is the ability to inspect, validate, govern, and evolve what has been produced.
</p>

<hr/>

<h2>Axes</h2>

<table>
  <thead>
    <tr>
      <th>Axis</th>
      <th>Meaning</th>
      <th>High score means</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><strong>AI Generation Compatibility</strong></td>
      <td>How naturally AI systems can generate the software artifact.</td>
      <td>The artifact can be generated directly and structurally by AI-assisted tooling.</td>
    </tr>
    <tr>
      <td><strong>AI Inspectability</strong></td>
      <td>How well generated software can be reviewed, validated, governed, and audited.</td>
      <td>The artifact is structurally reviewable rather than only linearly readable.</td>
    </tr>
  </tbody>
</table>

<p>
All points live in the positive quadrant. The diagonal arrow is the improvement direction: from isolated generation or isolated inspection toward software artifacts that are both generatable and governable.
</p>

<hr/>

<h2>Qualitative coordinates</h2>

<p>
The chart uses qualitative normalized coordinates rather than benchmark scores. The purpose is strategic communication, not empirical ranking.
</p>

<table>
  <thead>
    <tr>
      <th>Technology</th>
      <th>AI Inspectability</th>
      <th>AI Generation Compatibility</th>
      <th>Interpretation</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>C</td>
      <td>Low</td>
      <td>High</td>
      <td>Easy for AI systems to emit as text, but inspection is mostly syntax-first and linear.</td>
    </tr>
    <tr>
      <td>C++</td>
      <td>Low</td>
      <td>High</td>
      <td>Easy to generate as text, but review remains heavily implementation-oriented.</td>
    </tr>
    <tr>
      <td>Rust</td>
      <td>Low to medium</td>
      <td>High</td>
      <td>AI-generatable as text and helped by explicit language constraints, but still syntax-first.</td>
    </tr>
    <tr>
      <td>Python</td>
      <td>Medium</td>
      <td>Very high</td>
      <td>Very natural for AI code generation, but system-level inspection still depends largely on textual review and tests.</td>
    </tr>
    <tr>
      <td>LabVIEW</td>
      <td>High</td>
      <td>Low</td>
      <td>Strong visual inspection properties, but weak direct generation compatibility for general-purpose text-oriented AI coding workflows.</td>
    </tr>
    <tr>
      <td><strong>FROG (.frog / JSON)</strong></td>
      <td><strong>Very high</strong></td>
      <td><strong>Very high</strong></td>
      <td>Designed to combine machine-friendly structured source with graph-level reviewability and open execution-facing artifacts.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Why FROG sits in the upper-right quadrant</h2>

<p>
FROG targets the AI-governable zone because its core artifacts are intended to remain both machine-friendly and human-inspectable.
</p>

<ul>
  <li><strong>Structured graphical logic:</strong> executable behavior is represented as explicit nodes, ports, connections, structures, interfaces, and observability surfaces.</li>
  <li><strong>Machine-friendly source representation:</strong> canonical <code>.frog</code> source is structured JSON, which is naturally compatible with tooling, validation, deterministic serialization, and AI-assisted generation or transformation.</li>
  <li><strong>Open execution-facing IR:</strong> FIR is treated as an inspectable downstream bridge surface rather than a hidden backend-private object.</li>
  <li><strong>Reviewable, inspectable, auditable pipeline:</strong> source, validated meaning, FIR, lowering, backend contracts, runtime/compiler consumption, and observed behavior can be separated and checked.</li>
</ul>

<p>
The claim is not that FROG makes AI-generated systems automatically correct. The claim is that FROG is architected to reduce opacity by pairing AI-generatable structured source with graph-level inspection and open execution-facing artifacts.
</p>

<hr/>

<h2>Strategic reading</h2>

<blockquote>
  <p>
  Textual languages are easy for AI to generate, but harder to inspect as structured systems.
  </p>
</blockquote>

<blockquote>
  <p>
  Historical graphical environments are easier to inspect visually, but weakly aligned with direct general-purpose AI generation.
  </p>
</blockquote>

<blockquote>
  <p>
  FROG (.frog / JSON) is designed to combine both: AI-generatable source and AI-inspectable graphical system structure.
  </p>
</blockquote>

<hr/>

<h2>Boundary</h2>

<p>
This chart is strategic and non-normative. It does not define language semantics, conformance rules, source validity, FIR derivation, runtime behavior, or compiler behavior. Those surfaces remain owned by the corresponding specification layers of the repository.
</p>

<p>
The chart should be read as a communication aid for the AI-era positioning of FROG: a structured language foundation for generated or human-authored logic that still needs accountable inspection, validation, and evolution.
</p>
