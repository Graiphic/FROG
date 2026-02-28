The JSON expression is never executed directly.

It is first validated, secured, and compiled into FROG IR.

The IR is the canonical execution representation.

---

# HARDWARE-AGNOSTIC EXECUTION

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

# SECURITY AND OPTIMIZATION BY DESIGN

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

# INTEROPERABILITY

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

# SEPARATION OF LANGUAGE AND TOOLING

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

# DETERMINISTIC DATAFLOW EXECUTION

FROG uses a true dataflow execution model.

Nodes execute when their inputs become available.

This enables:

- parallel execution by default  
- deterministic behavior  
- efficient hardware utilization  
- scalable performance  

Execution is driven by data flow, not by instruction order.

---

# OPEN BY DESIGN

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

# FROG IS A FOUNDATION

FROG is a foundation for graphical programming where:

- the diagram defines execution  
- the front panel defines interaction  
- the program expression is fully open  
- execution is secure and optimized  
- the language remains independent  

---

# 🐸 FROG — Free Open Graphical Language
