<p align="center">
  <img src="FROG logo.png" alt="FROG logo" width="400" />
</p>

<h1 align="center">🐸 FROG — Free Open Graphical Language</h1>

<p align="center">
  <strong>Free Open Graphical Dataflow Programming Language</strong><br/>
  An open, hardware-agnostic graphical dataflow language designed for rapid prototyping and high-performance execution.
</p>

<p align="center">
  <a href="#what-is-frog">What is FROG?</a> •
  <a href="#why-frog-exists">Why FROG exists</a> •
  <a href="#dataflow-programming">Dataflow programming</a> •
  <a href="#from-prototyping-to-critical-systems">From prototyping to critical systems</a> •
  <a href="#core-concept-diagram-and-front-panel">Core concept</a> •
  <a href="#program-representation">Program representation</a> •
  <a href="#execution-architecture">Execution architecture</a> •
  <a href="#hardware-agnostic-execution">Execution targets</a> •
  <a href="#security-and-optimization">Security & Optimization</a> •
  <a href="#interoperability">Interoperability</a> •
  <a href="#separation-of-language-and-tooling">Language separation</a> •
  <a href="#project-status">Status</a> •
  <a href="#license">License</a>
</p>

<hr/>

<h2 id="what-is-frog">What is FROG?</h2>

<p>
FROG is an open, hardware-agnostic <strong>graphical dataflow programming language</strong>
designed to orchestrate computation through explicit dataflow graphs.
</p>

<p>
Instead of writing programs as sequences of instructions,
FROG programs are expressed as <strong>graphs of operations connected by data</strong>.
Execution emerges naturally from the movement and availability of data
through the system.
</p>

<p>
FROG defines an open standard for describing computation as
<strong>executable dataflow graphs</strong> composed of nodes, connections,
types, and interaction interfaces.
</p>

<p>
The language is designed to remain independent from any particular
IDE, compiler, runtime, or vendor ecosystem.
</p>

<p>
This separation ensures long-term stability, interoperability,
and the possibility of multiple independent implementations.
</p>

<hr/>

<h2 id="why-frog-exists">Why FROG exists</h2>

<p>
Graphical dataflow programming has demonstrated powerful advantages
for building complex systems:
</p>

<ul>
<li>natural parallelism</li>
<li>deterministic execution</li>
<li>intuitive system orchestration</li>
<li>strong mapping between software and hardware</li>
<li>high productivity for engineers and scientists</li>
</ul>

<p>
However, most existing graphical dataflow languages have historically been
tightly coupled to proprietary ecosystems where the language,
runtime, and development environment are inseparable.
</p>

<p>
This restricts innovation, limits portability,
and prevents independent implementations.
</p>

<p>
FROG addresses this limitation by defining an <strong>open language specification</strong>
separate from any particular tool.
</p>

<p>
<strong>FROG is not an IDE.</strong><br/>
<strong>FROG is not a product.</strong><br/>
<strong>FROG is a language specification.</strong>
</p>

<hr/>

<h2 id="dataflow-programming">Dataflow programming</h2>

<p>
FROG follows a true <strong>dataflow execution model</strong>.
</p>

<p>
In traditional programming languages, programs are executed
as ordered sequences of instructions.
</p>

<p>
In dataflow programming, operations execute automatically when their
required input data becomes available.
</p>

<pre>
Traditional execution

A → B → C → D


Dataflow execution

   A
  / \
 B   C
  \ /
   D
</pre>

<p>
Execution order therefore emerges from data dependencies rather than
being explicitly defined.
</p>

<p>
This model enables:
</p>

<ul>
<li>automatic parallel execution</li>
<li>deterministic behavior</li>
<li>efficient use of modern hardware</li>
<li>clear representation of system logic</li>
</ul>

<hr/>

<h2 id="from-prototyping-to-critical-systems">From prototyping to critical systems</h2>

<p>
FROG is designed to combine two goals that are often seen as contradictory
in programming languages.
</p>

<ul>
<li>extreme accessibility for rapid prototyping</li>
<li>high execution performance for demanding systems</li>
</ul>

<p>
Graphical programming dramatically lowers the barrier to entry.
By expressing systems visually as dataflow graphs,
engineers and scientists can prototype complex systems rapidly
without the cognitive overhead of traditional syntax-based languages.
</p>

<p>
At the same time, accessibility must not come at the cost of efficiency.
</p>

<p>
Through its intermediate representation and compilation pipeline,
FROG enables deep optimization of execution graphs,
allowing programs to scale from simple experiments
to high-performance deterministic systems.
</p>

<p>
The same programming model can therefore support:
</p>

<ul>
<li>rapid experimentation</li>
<li>scientific computing</li>
<li>industrial automation</li>
<li>embedded systems</li>
<li>real-time control</li>
<li>high-performance computing</li>
<li>safety-critical systems</li>
</ul>

<p>
FROG aims to provide a unified programming model where
<strong>usability, performance, and determinism coexist rather than compete</strong>.
</p>

<p>
The ambition is to contribute to a significant evolution in the design
of programming languages by bringing together graphical programming,
dataflow execution, and modern compiler architectures.
</p>

<hr/>

<h2 id="core-concept-diagram-and-front-panel">Core concept: Diagram and Front Panel</h2>

<p>
Every FROG program combines two complementary layers.
</p>

<h3>Diagram — the executable dataflow graph</h3>

<p>
The diagram defines the program logic as a directed dataflow graph composed of:
</p>

<ul>
<li>nodes representing operations</li>
<li>edges representing data connections</li>
<li>typed ports defining valid data exchange</li>
<li>execution metadata describing scheduling behavior</li>
</ul>

<p>
Execution is derived exclusively from the graph structure and the
availability of data.
</p>

<h3>Front Panel — the interaction layer</h3>

<p>
The front panel defines the user interface used to interact with the program.
</p>

<ul>
<li>controls providing inputs</li>
<li>indicators exposing outputs</li>
<li>visual components representing system state</li>
<li>layout and graphical properties</li>
</ul>

<p>
Front panel elements follow <strong>SVG semantics</strong> for graphical
representation and rendering.
</p>

<p>
These interface elements are connected to the diagram through explicit
typed bindings.
</p>

<hr/>

<h2 id="program-representation">Program representation</h2>

<p>
FROG programs exist in two complementary representations.
</p>

<h3>FROG IR — Intermediate Representation</h3>

<p>
The <strong>FROG IR</strong> is the canonical internal representation
used for execution and compilation.
</p>

<p>
It contains:
</p>

<ul>
<li>nodes and operations</li>
<li>data connections</li>
<li>type information</li>
<li>execution constraints</li>
<li>scheduling metadata</li>
</ul>

<h3>FROG Expression — canonical program format</h3>

<p>
The <strong>FROG Expression</strong> is the serialized program representation.
</p>

<p>
It contains:
</p>

<ul>
<li>diagram definition</li>
<li>front panel definition</li>
<li>embedded SVG interface elements</li>
<li>type system definitions</li>
<li>program metadata</li>
</ul>

<p>
The expression is designed for:
</p>

<ul>
<li>human readability</li>
<li>version control compatibility</li>
<li>tool interoperability</li>
<li>long-term durability</li>
</ul>

<p>
Programs are never executed directly from the expression.
Execution always occurs through the validated intermediate representation.
</p>

<hr/>

<h2 id="execution-architecture">Execution architecture</h2>

<pre>
                FROG IDE
               /        \
              /          \
             ↓            ↓
   FROG Expression     FROG IR
     (storage)      (execution graph)
              \          /
               \        /
                ↓      ↓
               Compiler(s)
                    ↓
             Target runtime
                    ↓
     CPU / RT / FPGA / GPU / Embedded
</pre>

<p>
During development, the IDE maintains the IR while the diagram is edited.
Saving a program serializes the IR into the canonical program expression.
</p>

<hr/>

<h2 id="hardware-agnostic-execution">Hardware-agnostic execution</h2>

<p>
FROG programs are designed to run across multiple targets without modification.
</p>

<ul>
<li><strong>FROG Core</strong> — general-purpose systems</li>
<li><strong>FROG RT</strong> — deterministic real-time execution</li>
<li><strong>FROG FPGA</strong> — programmable logic targets</li>
</ul>

<p>
The diagram and front panel remain identical across targets.
Only the compiler and runtime implementations differ.
</p>

<hr/>

<h2 id="security-and-optimization">Security and optimization by design</h2>

<p>
FROG integrates validation and optimization directly into its architecture.
</p>

<ul>
<li>strict type validation</li>
<li>graph verification before execution</li>
<li>controlled execution boundaries</li>
<li>sandboxed runtime environments</li>
<li>deterministic execution guarantees</li>
</ul>

<p>
Optimization is performed at the IR and compilation levels:
</p>

<ul>
<li>graph simplification</li>
<li>dead node elimination</li>
<li>constant folding</li>
<li>memory reuse optimization</li>
<li>automatic parallel scheduling</li>
<li>target-specific optimizations</li>
</ul>

<hr/>

<h2 id="interoperability">Interoperability</h2>

<p>
FROG integrates with external languages through stable ABI interfaces.
</p>

<ul>
<li>C / C++</li>
<li>Rust</li>
<li>Python</li>
<li>.NET</li>
<li>other languages via ABI bindings</li>
</ul>

<p>
External functions can be exposed as nodes in the dataflow graph
while preserving type safety and execution guarantees.
</p>

<hr/>

<h2 id="separation-of-language-and-tooling">Separation of language and tooling</h2>

<p>
FROG explicitly separates:
</p>

<ul>
<li>language specification</li>
<li>intermediate representation</li>
<li>runtime implementations</li>
<li>development environments</li>
</ul>

<p>
This architecture enables:
</p>

<ul>
<li>multiple IDE implementations</li>
<li>multiple compilers</li>
<li>multiple runtimes</li>
<li>independent ecosystem evolution</li>
</ul>

<p>
The specification defines the language — not a particular implementation.
</p>

<hr/>

<h2 id="project-status">Project status</h2>

<p>
FROG is currently in early development.
The language specification, intermediate representation,
and reference tooling are under active design.
</p>

<p>
Discussions, feedback, and contributions are welcome.
</p>

<hr/>

<h2 id="license">License</h2>

<p>
This project is licensed under the <strong>Apache License 2.0</strong>.
See <code>LICENSE</code> for details.
</p>

<p align="center">
<a href="https://cla-assistant.io/Graiphic/FROG">
<img src="https://cla-assistant.io/readme/badge/Graiphic/FROG" alt="CLA Assistant"/>
</a>
</p>
