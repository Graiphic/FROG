<p align="center">
  <img src="FROG logo.png" alt="FROG logo" width="400"/>
</p>

<h1 align="center">🐸 FROG — Free Open Graphical Language</h1>

<p align="center">
  <strong>Free Open Graphical Language</strong><br/>
  An open, hardware-agnostic graphical dataflow language and execution model.
</p>

---

## What is FROG?

FROG is an open, hardware-agnostic graphical dataflow language designed to orchestrate computation through explicit graph execution.

It is built on a simple principle: **the language must be independent of its tools, its runtime, and any single vendor.**

FROG defines an open standard for expressing programs as executable graphs. These graphs describe the flow of data, the operations applied to it, and the interface used to observe and control execution.

A FROG program is composed of two fundamental and complementary parts:

- the **diagram**, which defines the executable dataflow graph  
- the **front panel**, which defines the user interface and interaction layer  

Programs are not opaque binaries. They are structured, readable, and fully open expressions stored in a canonical representation.

---

## Why FROG exists

Graphical dataflow programming has demonstrated unique advantages:

- natural parallelism  
- deterministic execution  
- intuitive system orchestration  
- direct mapping to hardware  

However, existing implementations have historically combined the language, runtime, and development environment into a single proprietary system.

This prevents independent implementations, limits portability, and ties the future of the language to a single entity.

FROG removes this limitation.

**FROG is not an IDE.**  
**FROG is not a product.**  
**FROG is a language specification.**

---

## Core concept: Diagram and Front Panel

Every FROG program is defined by the explicit separation and association of two layers.

### Diagram: the executable graph

The diagram defines the executable logic.

The diagram is a directed dataflow graph composed of:

- nodes, which represent operations  
- edges, which represent data connections  
- types, which define data structure and validity  
- execution metadata, which defines execution behavior  

The diagram is the authoritative definition of computation.

Execution is derived exclusively from the diagram structure and data availability.

### Front panel: the interaction layer

The front panel defines the interaction layer.

The front panel is a structured interface composed of:

- controls, which provide inputs to the diagram  
- indicators, which expose outputs from the diagram  
- visual components, which represent system state  
- layout and graphical properties  

Graphical elements of the front panel use **SVG semantics** for vector definition, layout, and rendering. These SVG definitions are embedded directly within the canonical program expression.

SVG is used as a graphical description model, not as a separate program container.

The program expression remains the single authoritative program representation.

The front panel connects to the diagram through explicit, typed bindings.

This ensures full consistency between execution logic and user interaction.

---

## Programs as open expressions

Every FROG program is stored as a single open expression containing:

- the diagram definition  
- the front panel definition  
- SVG-based graphical element definitions embedded in the expression  
- the type system definition  
- metadata and execution constraints  

This ensures:

- human readability  
- version control compatibility  
- long-term durability  
- tool independence  

The expression defines the program. The runtime executes the validated intermediate representation derived from it.

---

## Execution architecture

FROG defines a clear and secure execution pipeline:

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


The program expression is never executed directly.

It is first validated, secured, and compiled into **FROG IR**.

The IR is the canonical execution representation.

---

## Hardware-agnostic execution

FROG is designed to execute across multiple targets without modifying the program expression.

The language supports three execution profiles:

**FROG Core**  
General-purpose execution on standard operating systems.

**FROG RT**  
Deterministic execution for real-time systems.

**FROG FPGA**  
Compilation targeting programmable logic hardware.

The diagram and front panel remain identical across targets.

Only the compiler and runtime change.

---

## Security and optimization by design

Security and optimization are core properties of the FROG architecture.

FROG enforces:

- strict type validation  
- graph validation before execution  
- controlled and safe execution boundaries  
- optional sandboxed runtime environments  
- deterministic and auditable execution behavior  

Optimization is performed at the IR and compilation levels, including:

- graph simplification  
- dead node elimination  
- constant folding  
- memory optimization and reuse  
- automatic parallel scheduling  
- target-specific optimization  

---

## Interoperability

FROG supports native interoperability with external languages.

External code can be exposed as nodes within the graph while preserving safety and execution integrity.

Supported interoperability includes:

- C and C++  
- Rust  
- Python  
- .NET  
- other languages through stable ABI interfaces  

This allows reuse of existing libraries and integration with external systems.

---

## Separation of language and tooling

FROG explicitly separates:

- the language specification  
- the runtime implementation  
- the development environment  

This enables:

- multiple IDE implementations  
- multiple runtime implementations  
- multiple compilers and targets  
- independent ecosystem evolution  

No single implementation defines the language.

The specification defines the language.

---

## Deterministic dataflow execution

FROG uses a true dataflow execution model.

Nodes execute when their inputs become available.

This enables:

- parallel execution by default  
- deterministic behavior  
- efficient hardware utilization  
- scalable performance  

Execution is driven by data flow, not by instruction order.

---

## Open by design

FROG is fully open and openly specified.

This guarantees:

- vendor independence  
- long-term stability  
- transparent execution  
- community-driven evolution  

Anyone can implement the language.

Anyone can build tools, runtimes, or compilers.

Anyone can extend the ecosystem.

---

## Project status

FROG is currently in early development. The language specification, IR, and reference tooling are under active design.

Discussions, feedback, and contributions are welcome.

---

## License

This project is licensed under the **Apache License 2.0**. See `LICENSE` for details.
