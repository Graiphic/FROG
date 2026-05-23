<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Format-first, not IDE-first</h1>

<p>
FROG is intentionally designed as a format-first and pipeline-first graphical
language foundation. The IDE is not the language. The runtime is not the
language. The compiler is not the language. The durable public contract is the
progression from canonical <code>.frog</code> source to validated meaning,
open FIR, lowering boundaries, backend contracts, and explicit runtime or
compiler consumption.
</p>

<p>
This distinction protects the long-term maintainability of FROG programs.
A graphical editor may evolve, be replaced, or disappear without becoming the
owner of the program. A runtime may be versioned, specialized, or replaced
without redefining the source language. A compiler path may change without
turning the compiler into the semantic truth of the ecosystem.
</p>

<p>
The durable asset is therefore not one IDE project file or one private execution
stack. The durable asset is the public chain of artifacts and contracts:
</p>

<pre><code>.frog canonical source
  -&gt; structural validation
  -&gt; semantic validation
  -&gt; validated program meaning
  -&gt; open FIR
  -&gt; lowering boundary
  -&gt; backend contract
  -&gt; runtime-family or compiler-family consumption
</code></pre>

<p>
This avoids the structural risk of graphical programming platforms where the
editor, saved format, execution model, runtime, compiler, and hardware ecosystem
become inseparable. In FROG, an IDE may provide authoring, debugging,
observability, front-panel editing, validation feedback, and deployment tooling,
but it consumes specification-owned artifacts rather than owning the language.
</p>

<p>
The same rule applies downstream. A runtime host can provide live execution,
state management, probes, watches, UI hosting, diagnostics, and operational
orchestration. A compiler path can produce native or optimized artifacts. Both
paths remain consumers of explicit contracts. Neither path becomes the permanent
identity of FROG.
</p>

<p>
For industrial users, this is the practical consequence: existing FROG programs
should remain maintainable over time because their meaning is anchored in open
source artifacts, explicit validation rules, open execution-facing IR posture,
explicit downstream handoff contracts, and conformance expectations rather than
in the continued existence of one monolithic product stack.
</p>

<hr/>

<p align="center">
  <strong>FROG — Free Open Graphical Language</strong><br/>
  Open graphical dataflow programming, specified as a language rather than owned as a product.
</p>
