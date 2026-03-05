<div align="center">

<h1>FROG IDE Architecture</h1>

<p>
<b>FROG — Free Open Graphical Language</b><br>
Open Source Graphical Dataflow Programming Environment
</p>

</div>

<hr>

<h2>Overview</h2>

<p>
The FROG IDE is the graphical development environment used to design, edit, debug, and execute programs written in the FROG graphical dataflow language.
</p>

<p>
The IDE is designed as a modular system where the user interface, program representation, compilation, and execution runtime are clearly separated.
</p>

<p>
This architecture ensures that the FROG language itself remains independent from the IDE implementation while enabling execution across multiple targets and environments.
</p>

<p>
The development environment focuses on three main activities:
</p>

<ul>
<li>graphical programming through a diagram editor</li>
<li>user interface design through a front panel editor</li>
<li>visual debugging and execution</li>
</ul>

<hr>

<h2>High-Level Architecture</h2>

<div align="center">

<pre>

+------------------------------------------------------+
|                       FROG IDE                       |
|                                                      |
|  +----------------------+   +----------------------+ |
|  |   Front Panel        |   |   Diagram Editor     | |
|  |   HTML / SVG / JS    |   |   WebGL / JS         | |
|  +----------------------+   +----------------------+ |
|              |                         |              |
|              +------------+------------+              |
|                           |                           |
|                   FROG Expression                     |
|                       (.frog JSON)                   |
+---------------------------|--------------------------+
                            |
                            v
                   +-------------------+
                   |   FROG Compiler   |
                   |       Rust        |
                   +-------------------+
                            |
                            v
                      Intermediate
                     Representation
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

<hr>

<h2>Main Components</h2>

<h3>IDE Shell</h3>

<p>
The IDE shell is the main application responsible for managing the development environment.
</p>

<p>
It integrates all editors and tools and provides the user interface structure.
</p>

<p>
Responsibilities include:
</p>

<ul>
<li>window and layout management</li>
<li>menus and toolbars</li>
<li>project explorer</li>
<li>file management</li>
<li>plugin system</li>
<li>editor integration</li>
</ul>

<p>
The IDE shell is implemented using:
</p>

<ul>
<li><b>JavaScript / TypeScript</b></li>
<li><b>HTML / CSS</b></li>
</ul>

<p>
The IDE is packaged as a desktop application using technologies such as:
</p>

<ul>
<li>Tauri</li>
<li>Electron</li>
</ul>

<p>
This architecture allows the IDE to run consistently across Windows, Linux, and macOS.
</p>

<hr>

<h3>Diagram Editor</h3>

<p>
The diagram editor is the graphical programming environment where users construct dataflow programs.
</p>

<p>
Programs are represented as graphs composed of nodes connected by wires representing data dependencies.
</p>

<p>
Users can:
</p>

<ul>
<li>create nodes</li>
<li>connect wires</li>
<li>configure node properties</li>
<li>organize graph layouts</li>
<li>visualize execution</li>
</ul>

<p>
The diagram editor is implemented using:
</p>

<ul>
<li><b>JavaScript / TypeScript</b></li>
<li><b>WebGL GPU rendering</b></li>
</ul>

<p>
Using WebGL allows the diagram editor to support large graphs with smooth interaction.
</p>

<p>
Capabilities include:
</p>

<ul>
<li>smooth zoom and pan</li>
<li>real-time node manipulation</li>
<li>execution highlighting</li>
<li>efficient rendering of large graphs</li>
</ul>

<hr>

<h3>Front Panel Editor</h3>

<p>
The front panel editor allows developers to design the user interface associated with a FROG program.
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
The front panel editor is implemented using:
</p>

<ul>
<li><b>JavaScript</b></li>
<li><b>HTML</b></li>
<li><b>SVG</b></li>
</ul>

<p>
SVG provides resolution-independent vector rendering and allows interfaces to scale cleanly across displays.
</p>

<p>
This architecture also allows front panels to be rendered directly in web environments.
</p>

<hr>

<h3>FROG Expression</h3>

<p>
Programs created in the IDE are stored as structured expressions using a JSON-based format.
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
This representation is designed to be:
</p>

<ul>
<li>human-readable</li>
<li>version-control friendly</li>
<li>open and portable</li>
</ul>

<hr>

<h3>Compiler</h3>

<p>
The compiler transforms the FROG program expression into an intermediate representation suitable for execution.
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
The compiler is implemented in <b>Rust</b>, providing strong safety guarantees and high performance.
</p>

<hr>

<h3>Incremental Compilation</h3>

<p>
To provide an instantaneous execution experience similar to traditional graphical programming environments, the compiler operates incrementally.
</p>

<p>
Whenever the user modifies the diagram or front panel, only the affected parts of the program are recompiled.
</p>

<p>
This ensures that the intermediate representation remains continuously updated while editing.
</p>

<p>
As a result, program execution can start immediately without requiring a full recompilation.
</p>

<hr>

<h3>Runtime</h3>

<p>
The runtime is the execution engine responsible for running compiled FROG programs.
</p>

<p>
It implements the dataflow scheduler that controls node execution and data propagation.
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
<li>hardware interfaces</li>
</ul>

<hr>

<h2>Execution Flow</h2>

<div align="center">

<pre>

User edits diagram
        ↓
Incremental compilation
        ↓
Intermediate Representation updated
        ↓
User presses Run
        ↓
Runtime executes IR immediately

</pre>

</div>

<hr>

<h2>Design Principles</h2>

<ul>
<li><b>Language independence</b> — the language specification is independent from the IDE</li>
<li><b>Incremental compilation</b> for instant execution</li>
<li><b>Hardware agnostic execution</b></li>
<li><b>Open program representation</b></li>
<li><b>GPU accelerated graphical editing</b></li>
<li><b>Cross-platform development</b></li>
</ul>

<hr>

<h2>Future Evolution</h2>

<p>
The architecture of the FROG IDE is designed to evolve over time.
</p>

<p>
Potential future features include:
</p>

<ul>
<li>plugin architecture</li>
<li>collaborative editing</li>
<li>remote front panels</li>
<li>web-based IDE</li>
<li>hardware-specific toolchains (RT / FPGA)</li>
</ul>

<hr>

<h2>Repository Structure</h2>

<pre>

FROG/
│
├── IDE/
│   ├── ide-shell/
│   ├── diagram-editor/
│   └── front-panel-editor/
│
├── compiler/
│
├── runtime/
│
└── expression/

</pre>

<hr>

<h2>License</h2>

<p>
FROG is distributed under the Apache 2.0 license and uses a Contributor License Agreement (CLA) for external contributions.
</p>

<hr>

<div align="center">

<p>
FROG — Free Open Graphical Language
</p>

</div>
