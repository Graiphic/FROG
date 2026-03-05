<div align="center">

<h1>FROG IDE Architecture</h1>

<p>
<b>FROG — Free Open Graphical Language</b><br>
Open Source Graphical Dataflow Programming Environment
</p>

</div>

---

<h2>Overview</h2>

<p>
The FROG IDE is a graphical development environment designed to build, edit, and execute dataflow programs based on the FROG language specification.
</p>

<p>
The IDE is intentionally designed as a modular system where the user interface, program representation, compilation, and execution runtime are clearly separated. This architecture ensures that the FROG language itself remains independent from the IDE implementation and can be executed on multiple targets.
</p>

<p>
The development environment focuses on three main user experiences:
</p>

<ul>
<li>graphical programming through a diagram editor</li>
<li>interactive interface design through a front panel editor</li>
<li>visual debugging and program execution</li>
</ul>

---

<h2>High-Level Architecture</h2>

<div align="center">

<pre>

+--------------------------------------------------+
|                    FROG IDE                      |
|                                                  |
|  +-------------------+   +--------------------+  |
|  |  Front Panel      |   |  Diagram Editor    |  |
|  |     Editor        |   |   (Graph Editor)   |  |
|  |       Qt          |   | JavaScript/WebGL   |  |
|  +-------------------+   +--------------------+  |
|            |                        |             |
|            +-----------+------------+             |
|                        |                          |
|                 FROG Expression                   |
|                     (.frog JSON)                  |
+------------------------|--------------------------+
                         |
                         v
                +-------------------+
                |   FROG Compiler   |
                |       Rust        |
                +-------------------+
                         |
                         v
                +-------------------+
                |    FROG Runtime   |
                |         C         |
                +-------------------+
                         |
                         v
                      Execution

</pre>

</div>

---

<h2>Main Components</h2>

<h3>IDE Shell</h3>

<p>
The IDE shell is the main desktop application responsible for managing the global development environment.
</p>

<p>
It provides the structural foundation of the IDE and integrates the different editors and tools used during development.
</p>

<p>
Main responsibilities include:
</p>

<ul>
<li>window management</li>
<li>dockable panels</li>
<li>menu system</li>
<li>toolbars</li>
<li>project management</li>
<li>plugin integration</li>
</ul>

<p>
The initial implementation uses the <b>Qt framework</b>, which provides a stable and mature environment for building complex cross-platform desktop applications.
</p>

---

<h3>Diagram Editor</h3>

<p>
The diagram editor is the graphical programming environment where users create the computational logic of a program.
</p>

<p>
Programs are expressed as dataflow graphs composed of nodes connected by wires that represent the movement of data between operations.
</p>

<p>
Users can:
</p>

<ul>
<li>create nodes</li>
<li>connect wires</li>
<li>configure node parameters</li>
<li>organize graph layouts</li>
<li>inspect execution behavior</li>
</ul>

<p>
The diagram editor is implemented using:
</p>

<ul>
<li><b>JavaScript / TypeScript</b></li>
<li><b>WebGL GPU rendering</b></li>
</ul>

<p>
Using WebGL allows the diagram editor to efficiently render large graphs while maintaining smooth interaction and navigation.
</p>

<p>
The rendering engine supports:
</p>

<ul>
<li>smooth zoom and pan</li>
<li>interactive node manipulation</li>
<li>large graph rendering</li>
<li>execution highlighting</li>
<li>real-time updates</li>
</ul>

<p>
The diagram engine is designed to scale to thousands of nodes without relying on heavy DOM structures.
</p>

---

<h3>Front Panel Editor</h3>

<p>
The front panel editor allows the design of user interfaces associated with a FROG program.
</p>

<p>
The front panel represents the interaction layer between the user and the program logic.
</p>

<p>
Typical elements include:
</p>

<ul>
<li>buttons</li>
<li>numeric controls</li>
<li>indicators</li>
<li>graphs</li>
<li>gauges</li>
<li>interactive displays</li>
</ul>

<p>
The initial implementation relies on <b>Qt Widgets</b> to create native graphical interfaces quickly and reliably.
</p>

<p>
Future versions may introduce additional rendering targets such as:
</p>

<ul>
<li>web-based front panels</li>
<li>remote UI rendering</li>
<li>HTML based dashboards</li>
</ul>

---

<h3>FROG Expression</h3>

<p>
Programs created in the IDE are stored as structured expressions using a JSON-based representation.
</p>

<p>
File extension:
</p>

<pre>
.frog
</pre>

<p>
The expression contains:
</p>

<ul>
<li>diagram definition</li>
<li>node configuration</li>
<li>connections between nodes</li>
<li>front panel layout</li>
</ul>

<p>
The JSON representation is designed to be:
</p>

<ul>
<li>human readable</li>
<li>version control friendly</li>
<li>fully open</li>
<li>portable across environments</li>
</ul>

---

<h3>Compiler</h3>

<p>
The compiler transforms the FROG program expression into an intermediate representation that can be executed efficiently.
</p>

<p>
Responsibilities include:
</p>

<ul>
<li>graph validation</li>
<li>type checking</li>
<li>semantic verification</li>
<li>graph optimization</li>
<li>intermediate representation generation</li>
</ul>

<p>
The compiler is implemented in <b>Rust</b>, providing strong safety guarantees and high performance for complex transformations.
</p>

---

<h3>Runtime</h3>

<p>
The runtime is the execution engine responsible for running compiled FROG programs.
</p>

<p>
It implements the dataflow scheduler that controls when nodes are executed and how data moves through the graph.
</p>

<p>
Responsibilities include:
</p>

<ul>
<li>dataflow scheduling</li>
<li>node execution</li>
<li>parallel execution</li>
<li>memory management</li>
<li>debug instrumentation</li>
</ul>

<p>
The runtime is implemented in <b>C</b> to ensure maximum portability and compatibility with embedded systems.
</p>

<p>
The runtime can therefore be deployed on:
</p>

<ul>
<li>desktop systems</li>
<li>embedded devices</li>
<li>real-time environments</li>
<li>hardware specific targets</li>
</ul>

---

<h2>Execution Flow</h2>

<div align="center">

<pre>

Diagram created in IDE
          ↓
      .frog file
          ↓
   Compiler (Rust)
          ↓
Intermediate Representation
          ↓
      Runtime (C)
          ↓
       Execution

</pre>

</div>

<p>
During debugging sessions, the runtime sends execution events back to the IDE, allowing the diagram editor to highlight active nodes and visualize program behavior.
</p>

---

<h2>Design Principles</h2>

<ul>
<li><b>Language independence</b> — the language specification is separate from the IDE implementation</li>
<li><b>Hardware agnostic execution</b></li>
<li><b>Transparent program representation</b></li>
<li><b>Modular architecture</b></li>
<li><b>GPU accelerated graphical editing</b></li>
<li><b>Open ecosystem</b></li>
</ul>

---

<h2>Future Evolution</h2>

<p>
The architecture of the FROG IDE is designed to evolve over time and support new capabilities.
</p>

<p>
Potential future features include:
</p>

<ul>
<li>plugin architecture</li>
<li>collaborative editing</li>
<li>remote front panels</li>
<li>web based IDE</li>
<li>hardware specific toolchains (RT / FPGA)</li>
</ul>

---

<h2>Repository Structure</h2>

<pre>

FROG/
│
├── IDE/
│   ├── diagram-editor/
│   ├── front-panel-editor/
│   └── ide-shell/
│
├── compiler/
│
├── runtime/
│
└── expression/

</pre>

---

<h2>License</h2>

<p>
FROG is distributed under the Apache 2.0 license and uses a Contributor License Agreement (CLA) for external contributions.
</p>

---

<div align="center">

<p>
FROG — Free Open Graphical Language
</p>

</div>
