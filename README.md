<p align="center">
  <img src="FROG logo.png" alt="FROG logo" width="400" />
</p>

<h1 align="center">🐸 FROG — Free Open Graphical Language</h1>

<p align="center">
  <strong>Free Open Graphical Language</strong><br/>
  An open, hardware-agnostic graphical dataflow language and execution model.
</p>

<p align="center">
  <a href="#what-is-frog">What is FROG?</a> •
  <a href="#why-frog-exists">Why FROG exists</a> •
  <a href="#core-concept-diagram-and-front-panel">Core concept</a> •
  <a href="#execution-architecture">Execution</a> •
  <a href="#project-status">Status</a> •
  <a href="#license">License</a>
</p>

<hr/>

<h2 id="what-is-frog">What is FROG?</h2>

<p>
FROG is an open, hardware-agnostic graphical dataflow language designed to orchestrate computation through explicit graph execution.
</p>

<p>
It is built on a simple principle:
<strong>the language must be independent of its tools, its runtime, and any single vendor.</strong>
</p>

<p>
FROG defines an open standard for expressing programs as executable graphs. These graphs describe the flow of data, the operations applied to it, and the interface used to observe and control execution.
</p>

<p>
A FROG program is composed of two fundamental and complementary parts:
</p>

<ul>
  <li><strong>the diagram</strong>, which defines the executable dataflow graph</li>
  <li><strong>the front panel</strong>, which defines the user interface and interaction layer</li>
</ul>

<p>
Programs are not opaque binaries. They are structured, readable, and fully open expressions stored in a canonical representation.
</p>

<hr/>

<h2 id="why-frog-exists">Why FROG exists</h2>

<p>
Graphical dataflow programming has demonstrated unique advantages:
</p>

<ul>
  <li>natural parallelism</li>
  <li>deterministic execution</li>
  <li>intuitive system orchestration</li>
  <li>direct mapping to hardware</li>
</ul>

<p>
However, existing implementations have historically combined the language, runtime, and development environment into a single proprietary system.
This prevents independent implementations, limits portability, and ties the future of the language to a single entity.
</p>

<p>
FROG removes this limitation.
</p>

<p>
<strong>FROG is not an IDE.</strong><br/>
<strong>FROG is not a product.</strong><br/>
<strong>FROG is a language specification.</strong>
</p>

<hr/>

<h2 id="core-concept-diagram-and-front-panel">Core concept: Diagram and Front Panel</h2>

<p>
Every FROG program is defined by the explicit separation and association of two layers.
</p>

<h3>Diagram: the executable graph</h3>

<p>
The diagram defines the executable logic as a directed dataflow graph composed of:
</p>

<ul>
  <li>nodes, which represent operations</li>
  <li>edges, which represent data connections</li>
  <li>types, which define data structure and validity</li>
  <li>execution metadata, which defines execution behavior</li>
</ul>

<p>
The diagram is the authoritative definition of computation.
Execution is derived exclusively from the diagram structure and data availability.
</p>

<h3>Front panel: the interaction layer</h3>

<p>
The front panel defines the user interface and interaction surface, composed of:
</p>

<ul>
  <li>controls, which provide inputs to the diagram</li>
  <li>indicators, which expose outputs from the diagram</li>
  <li>visual components, which represent system state</li>
  <li>layout and graphical properties</li>
</ul>

<p>
Graphical elements of the front panel use <strong>SVG semantics</strong> for vector definition, layout, and rendering.
These SVG definitions are embedded directly within the canonical program expression.
</p>

<p>
SVG is used as a graphical description model, not as a separate program container.
The program expression remains the single authoritative program representation.
</p>

<p>
The front panel connects to the diagram through explicit, typed bindings, ensuring full consistency between execution logic and user interaction.
</p>

<hr/>

<h2>Programs as open expressions</h2>

<p>
Every FROG program is stored as a single open expression containing:
</p>

<ul>
  <li>the diagram definition</li>
  <li>the front panel definition</li>
  <li>SVG-based graphical element definitions embedded in the expression</li>
  <li>the type system definition</li>
  <li>metadata and execution constraints</li>
</ul>

<p>
This ensures:
</p>

<ul>
  <li>human readability</li>
  <li>version control compatibility</li>
  <li>long-term durability</li>
  <li>tool independence</li>
</ul>

<p>
The expression defines the program. The runtime executes the validated intermediate representation derived from it.
</p>

<hr/>

<h2 id="execution-architecture">Execution architecture</h2>

<p>
FROG defines a clear and secure execution pipeline:
</p>

<pre>
FROG IDE
   ↓
FROG Expression
   ↓
FROG IR (Intermediate Representation)
   ↓
FROG Compiler(s)
   ↓
Target runtime / deployment
   ↓
CPU / RT / FPGA / GPU / Embedded
</pre>

<p>
The program expression is never executed directly.
It is first validated, secured, and compiled into <strong>FROG IR</strong>.
The IR is the canonical execution representation.
</p>

<hr/>

<h2>Hardware-agnostic execution</h2>

<p>
FROG is designed to execute across multiple targets without modifying the program expression.
</p>

<p>
The language supports three execution profiles:
</p>

<ul>
  <li><strong>FROG Core</strong> — General-purpose execution on standard operating systems.</li>
  <li><strong>FROG RT</strong> — Deterministic execution for real-time systems.</li>
  <li><strong>FROG FPGA</strong> — Compilation targeting programmable logic hardware.</li>
</ul>

<p>
The diagram and front panel remain identical across targets.
Only the compiler and runtime change.
</p>

<hr/>

<h2>Security and optimization by design</h2>

<p>
Security and optimization are core properties of the FROG architecture.
</p>

<p><strong>FROG enforces:</strong></p>

<ul>
  <li>strict type validation</li>
  <li>graph validation before execution</li>
  <li>controlled and safe execution boundaries</li>
  <li>optional sandboxed runtime environments</li>
  <li>deterministic and auditable execution behavior</li>
</ul>

<p><strong>Optimization is performed at the IR and compilation levels, including:</strong></p>

<ul>
  <li>graph simplification</li>
  <li>dead node elimination</li>
  <li>constant folding</li>
  <li>memory optimization and reuse</li>
  <li>automatic parallel scheduling</li>
  <li>target-specific optimization</li>
</ul>

<hr/>

<h2>Interoperability</h2>

<p>
FROG supports native interoperability with external languages.
External code can be exposed as nodes within the graph while preserving safety and execution integrity.
</p>

<p><strong>Supported interoperability includes:</strong></p>

<ul>
  <li>C and C++</li>
  <li>Rust</li>
  <li>Python</li>
  <li>.NET</li>
  <li>other languages through stable ABI interfaces</li>
</ul>

<p>
This allows reuse of existing libraries and integration with external systems.
</p>

<hr/>

<h2>Separation of language and tooling</h2>

<p>
FROG explicitly separates:
</p>

<ul>
  <li>the language specification</li>
  <li>the runtime implementation</li>
  <li>the development environment</li>
</ul>

<p>
This enables:
</p>

<ul>
  <li>multiple IDE implementations</li>
  <li>multiple runtime implementations</li>
  <li>multiple compilers and targets</li>
  <li>independent ecosystem evolution</li>
</ul>

<p>
No single implementation defines the language.
The specification defines the language.
</p>

<hr/>

<h2>Deterministic dataflow execution</h2>

<p>
FROG uses a true dataflow execution model.
Nodes execute when their inputs become available.
</p>

<p><strong>This enables:</strong></p>

<ul>
  <li>parallel execution by default</li>
  <li>deterministic behavior</li>
  <li>efficient hardware utilization</li>
  <li>scalable performance</li>
</ul>

<p>
Execution is driven by data flow, not by instruction order.
</p>

<hr/>

<h2>Open by design</h2>

<p>
FROG is fully open and openly specified.
</p>

<p><strong>This guarantees:</strong></p>

<ul>
  <li>vendor independence</li>
  <li>long-term stability</li>
  <li>transparent execution</li>
  <li>community-driven evolution</li>
</ul>

<p>
Anyone can implement the language.
Anyone can build tools, runtimes, or compilers.
Anyone can extend the ecosystem.
</p>

<hr/>

<h2 id="project-status">Project status</h2>

<p>
FROG is currently in early development. The language specification, IR, and reference tooling are under active design.
</p>

<p>
Discussions, feedback, and contributions are welcome.
</p>

<hr/>

<h2 id="license">License</h2>

<p>
This project is licensed under the <strong>Apache License 2.0</strong>. See <code>LICENSE</code> for details.
</p>

[![CLA Assistant](https://cla-assistant.io/readme/badge/Graiphic/FROG)](https://cla-assistant.io/Graiphic/FROG)
