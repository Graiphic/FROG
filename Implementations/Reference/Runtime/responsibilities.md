<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Reference Runtime Responsibilities</h1>

<p align="center">
  <strong>Ownership map for parent-level runtime-family material and language-specific runtime consumers in the non-normative FROG reference implementation</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#why-this-file-exists">2. Why This File Exists</a></li>
  <li><a href="#current-runtime-family-scope">3. Current Runtime-Family Scope</a></li>
  <li><a href="#parent-level-ownership">4. Parent-Level Ownership</a></li>
  <li><a href="#parent-level-non-ownership">5. Parent-Level Non-Ownership</a></li>
  <li><a href="#python-consumer-responsibilities">6. Python Consumer Responsibilities</a></li>
  <li><a href="#rust-consumer-responsibilities">7. Rust Consumer Responsibilities</a></li>
  <li><a href="#cpp-consumer-responsibilities">8. C/C++ Consumer Responsibilities</a></li>
  <li><a href="#shared-runtime-obligations">9. Shared Runtime Obligations</a></li>
  <li><a href="#example-05-corridor-responsibilities">10. Example 05 Corridor Responsibilities</a></li>
  <li><a href="#rendered-ui-boundary">11. Rendered UI Boundary</a></li>
  <li><a href="#llvm-boundary">12. LLVM Boundary</a></li>
  <li><a href="#design-rules">13. Design Rules</a></li>
  <li><a href="#summary">14. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This file explains responsibility boundaries inside:
</p>

<pre><code>Implementations/Reference/Runtime/</code></pre>

<p>
The purpose of this file is not to restate language semantics.
Its purpose is to make ownership explicit between:
</p>

<ul>
  <li>parent-level runtime-family material,</li>
  <li>the Python runtime consumer,</li>
  <li>the Rust runtime consumer,</li>
  <li>the C/C++ runtime consumer,</li>
  <li>and the shared Example 05 corridor they are expected to consume.</li>
</ul>

<p>
This matters because the reference runtime is now visibly multi-language at repository level,
but those language-specific consumers do not all own the same things and do not currently close the corridor at the same maturity level.
</p>

<hr/>

<h2 id="why-this-file-exists">2. Why This File Exists</h2>

<p>
When a runtime family grows across several implementation languages,
ambiguity appears easily:
</p>

<ul>
  <li>parent-level helpers may be mistaken for the whole runtime,</li>
  <li>one language realization may be mistaken for the definition of FROG,</li>
  <li>rendered UI work may be mistaken for canonical widget law,</li>
  <li>compiler-family work may be mistaken for runtime-family work.</li>
</ul>

<p>
This file exists to prevent those collapses.
It keeps the ownership map explicit and makes the runtime-family corridor easier to audit.
</p>

<hr/>

<h2 id="current-runtime-family-scope">3. Current Runtime-Family Scope</h2>

<p>
The first published runtime family is:
</p>

<pre><code>reference_host_runtime_ui_binding</code></pre>

<p>
At the current repository state, the runtime-family directory contains:
</p>

<ul>
  <li>parent-level runtime-family material,</li>
  <li>a Python consumer family,</li>
  <li>a Rust consumer family,</li>
  <li>a C/C++ consumer family.</li>
</ul>

<p>
The corridor currently targeted most concretely is:
</p>

<pre><code>Examples/05_bounded_ui_accumulator/</code></pre>

<hr/>

<h2 id="parent-level-ownership">4. Parent-Level Ownership</h2>

<p>
The parent runtime directory owns:
</p>

<ul>
  <li>the runtime-family posture for the reference runtime family,</li>
  <li>parent-level explanations of accepted-contract execution,</li>
  <li>parent-level shared helpers used by the bounded corridor,</li>
  <li>parent-level coordination across runtime-language consumers,</li>
  <li>the repository-visible reference point for the shared contract-driven corridor.</li>
</ul>

<p>
At parent level, the key responsibilities are:
</p>

<ul>
  <li>make the backend-family consumer posture explicit,</li>
  <li>state what all consumers are expected to preserve,</li>
  <li>avoid turning one language-specific implementation into semantic truth,</li>
  <li>keep contract-family expectations aligned across Python, Rust, and C/C++.</li>
</ul>

<hr/>

<h2 id="parent-level-non-ownership">5. Parent-Level Non-Ownership</h2>

<p>
The parent runtime directory does not own:
</p>

<ul>
  <li>the FROG language definition,</li>
  <li>the canonical <code>.frog</code> source model,</li>
  <li>the semantic acceptance boundary,</li>
  <li>the FIR architecture,</li>
  <li>the lowering boundary,</li>
  <li>the backend contract emission boundary,</li>
  <li>widget class law,</li>
  <li><code>.wfrog</code> package law,</li>
  <li>the LLVM-oriented compiler-family path.</li>
</ul>

<p>
The parent directory is downstream from those layers.
It consumes their outputs.
It must not redefine them.
</p>

<pre><code>validated meaning != runtime-family implementation
backend contract != runtime-private structures
reference runtime != universal FROG runtime
</code></pre>

<hr/>

<h2 id="python-consumer-responsibilities">6. Python Consumer Responsibilities</h2>

<p>
The Python consumer currently has the most operational closure.
Its responsibilities include:
</p>

<ul>
  <li>consume the published Example 05 contract corridor,</li>
  <li>provide direct contract-driven execution,</li>
  <li>provide the first bounded rendered UI corridor,</li>
  <li>load the example-local <code>.wfrog</code> package for the rendered slice,</li>
  <li>preserve the same bounded accumulation behavior as the shared corridor,</li>
  <li>preserve explicit widget-value and widget-reference behavior for the supported subset.</li>
</ul>

<p>
The Python consumer does not thereby become the owner of widget semantics.
It remains only one consumer realization with the strongest current operational proof path.
</p>

<hr/>

<h2 id="rust-consumer-responsibilities">7. Rust Consumer Responsibilities</h2>

<p>
The Rust consumer currently has the strongest secondary proof posture after Python.
Its responsibilities include:
</p>

<ul>
  <li>load and validate the published Example 05 contract,</li>
  <li>preserve the same bounded execution behavior,</li>
  <li>preserve the same final observable result,</li>
  <li>make the Rust-side runtime split explicit through dedicated source files,</li>
  <li>stay aligned with the same contract-family assumptions as Python.</li>
</ul>

<p>
At the current stage, its strongest proof surface remains test-driven.
That is acceptable for the bounded corridor, but it is not yet the same thing as a fully polished example-facing runner posture.
</p>

<hr/>

<h2 id="cpp-consumer-responsibilities">8. C/C++ Consumer Responsibilities</h2>

<p>
The C/C++ consumer is now structurally present in the repository.
Its responsibilities are:
</p>

<ul>
  <li>provide a third language-level runtime-family consumer posture,</li>
  <li>stay aligned with the same contract-family assumptions as Python and Rust,</li>
  <li>preserve the same bounded Example 05 behavior,</li>
  <li>make contract loading, execution, and UI binding boundaries explicit at header and source level,</li>
  <li>close the same corridor without inventing a parallel semantic story.</li>
</ul>

<p>
At the current stage, the C/C++ consumer is structurally real but not yet operationally closed at the same example-facing level as Python.
</p>

<hr/>

<h2 id="shared-runtime-obligations">9. Shared Runtime Obligations</h2>

<p>
All runtime-language consumers are expected to preserve the same shared obligations for the bounded corridor:
</p>

<ul>
  <li>accept or reject the published contract explicitly,</li>
  <li>preserve explicit-state meaning,</li>
  <li>preserve bounded-loop meaning,</li>
  <li>preserve public input and output binding obligations,</li>
  <li>preserve indicator publication obligations,</li>
  <li>preserve the bounded object-style UI interaction subset declared by the corridor,</li>
  <li>reject unsupported operations explicitly rather than silently reinterpret them.</li>
</ul>

<p>
Private helper structures may differ.
Observed contract behavior must not drift.
</p>

<hr/>

<h2 id="example-05-corridor-responsibilities">10. Example 05 Corridor Responsibilities</h2>

<p>
For Example 05, responsibility boundaries should be read as follows:
</p>

<ul>
  <li><strong><code>main.frog</code></strong> owns canonical program meaning and widget participation.</li>
  <li><strong><code>accumulator_panel.wfrog</code></strong> owns peripheral widget-oriented realization packaging.</li>
  <li><strong><code>main.fir.json</code></strong> owns example-local execution-facing readability.</li>
  <li><strong><code>main.lowering.json</code></strong> owns example-local lowered posture.</li>
  <li><strong>the emitted contract artifact</strong> owns the runtime-family handoff surface.</li>
  <li><strong>the runtime-family consumers</strong> own private execution of the accepted handoff surface.</li>
</ul>

<p>
That separation must remain recoverable in every runtime-language implementation.
</p>

<hr/>

<h2 id="rendered-ui-boundary">11. Rendered UI Boundary</h2>

<p>
Rendered UI work belongs to the runtime-consumer realization layer, not to canonical language ownership.
</p>

<p>
Accordingly:
</p>

<ul>
  <li>a rendered host window is a runtime-side proof surface,</li>
  <li>widget private objects are runtime-private realization structures,</li>
  <li>visual assets remain downstream realization resources,</li>
  <li>rendered UI success does not redefine widget class law.</li>
</ul>

<p>
At the current repository state, the rendered-host proof path is strongest on the Python side.
That does not make Python the owner of widget truth.
</p>

<hr/>

<h2 id="llvm-boundary">12. LLVM Boundary</h2>

<p>
The LLVM-oriented path belongs to a separate compiler-family corridor.
It is downstream from FIR and lowering, but it is not part of the runtime-family ownership surface.
</p>

<p>
That means:
</p>

<ul>
  <li>runtime-family consumers execute accepted backend contracts,</li>
  <li>compiler-family consumers transform lowered artifacts toward native executables,</li>
  <li>those two downstream families must remain conceptually separate even when they target the same example.</li>
</ul>

<hr/>

<h2 id="design-rules">13. Design Rules</h2>

<ul>
  <li>Keep parent-level runtime-family posture separate from language-specific consumer implementation details.</li>
  <li>Keep one language realization from becoming semantic truth by convenience.</li>
  <li>Keep rendered UI realization separate from widget class law.</li>
  <li>Keep compiler-family LLVM work separate from runtime-family ownership.</li>
  <li>Keep Example 05 corridor artifacts attributable to their proper upstream layers.</li>
  <li>Prefer explicit rejection over silent reinterpretation when a consumer does not support a declared surface.</li>
</ul>

<hr/>

<h2 id="summary">14. Summary</h2>

<p>
This file exists to make runtime-family ownership explicit inside the reference implementation.
The parent runtime directory coordinates a shared contract-consumption posture.
Python, Rust, and C/C++ are parallel consumers of that posture with different current maturity levels.
</p>

<p>
The key architectural rule is simple:
</p>

<pre><code>shared corridor truth
    !=
one language-specific runtime implementation
</code></pre>

<p>
That distinction is what keeps the runtime family modular, auditable, and aligned with the broader FROG architecture.
</p>
