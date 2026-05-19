<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">08 - LLVM Must Not Be Treated as FROG Runtime Identity</h1>

<p align="center">
  <strong>Negative source/FIR/lowering case where a backend contract incorrectly collapses FROG identity into LLVM identity</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Case Identifier</h2>

<pre><code>invalid/compiler/08_llvm_must_not_be_treated_as_frog_runtime_identity</code></pre>

<h2>Primary Owner Stage</h2>

<pre><code>Primary owner stage: backend contract</code></pre>

<h2>Purpose</h2>

<p>
This case verifies that an LLVM-oriented backend family may exist downstream, but LLVM IR, LLVM tooling, or LLVM runtime assumptions must not be presented as the public identity of FROG, FIR, or runtime conformance.
</p>

<pre><code>FROG source
   -&gt;
validated meaning
   -&gt;
canonical FROG Execution IR
   -&gt;
lowering
   -&gt;
backend contract
   -&gt;
LLVM-oriented consumer

not:

FROG FIR = LLVM IR
FROG runtime identity = LLVM</code></pre>

<h2>Specification Owners</h2>

<ul>
  <li><a href="../../../IR/Execution IR.md"><code>IR/Execution IR.md</code></a> owns canonical FROG Execution IR identity.</li>
  <li><a href="../../../IR/Lowering.md"><code>IR/Lowering.md</code></a> owns the boundary that keeps LLVM-oriented routes downstream from canonical FIR.</li>
  <li><a href="../../../IR/Backend contract.md"><code>IR/Backend contract.md</code></a> owns the consumer-facing handoff boundary.</li>
  <li><a href="../../../Profiles/Native CPU LLVM.md"><code>Profiles/Native CPU LLVM.md</code></a> owns the optional <code>native_cpu_llvm</code> profile without redefining FROG itself.</li>
</ul>

<h2>Invalid Contract Scenario</h2>

<p>
The upstream program may be valid and may have a valid LLVM-oriented downstream route. The invalidity is a backend contract, profile assertion, or consumer declaration that claims one of the following:
</p>

<ul>
  <li>canonical FROG Execution IR is LLVM IR,</li>
  <li>a runtime or launcher must directly consume LLVM IR to be conforming to FROG,</li>
  <li>LLVM object identity replaces FROG source, FIR, or backend-contract identity,</li>
  <li>failure to support LLVM is a FROG language semantic rejection rather than an optional profile limitation.</li>
</ul>

<h2>Expected Outcomes</h2>

<pre><code>Expected source result: upstream source may be valid
Expected semantic result: upstream meaning may be accepted
Expected IR result: upstream FIR may be valid and derivable
Expected lowering result: LLVM-oriented lowering may be valid when explicitly declared
Expected backend-contract result: rejected when it collapses FROG/FIR/runtime identity into LLVM identity
Expected provider resolution: not applicable
Expected backend-family consumption: not applicable after contract rejection</code></pre>

<h2>Expected Rejection Stage</h2>

<pre><code>Expected rejection stage: backend-contract validation</code></pre>

<h2>Expected Rejection Reason</h2>

<pre><code>Expected rejection reason:
the contract or profile assertion treats a downstream LLVM-oriented route
as the public identity of FROG,
canonical FIR,
runtime conformance,
or launcher conformance</code></pre>

<h2>Must Not Happen</h2>

<ul>
  <li>LLVM must not replace canonical FROG Execution IR,</li>
  <li>an LLVM consumer must not become the definition of FROG runtime conformance,</li>
  <li>a contract must not require all conforming launchers to parse or depend on LLVM directly,</li>
  <li>profile-specific rejection must not be back-projected as semantic invalidity.</li>
</ul>

<h2>Required Preserved Distinctions</h2>

<ul>
  <li>FROG source remains distinct from canonical FIR,</li>
  <li>canonical FIR remains distinct from LLVM IR,</li>
  <li>lowering remains distinct from backend consumption,</li>
  <li>backend-family support remains distinct from language conformance,</li>
  <li>runtime or launcher consumption remains manifest/contract-driven, not LLVM-identity-driven.</li>
</ul>

<h2>Summary</h2>

<p>
A conforming implementation may support an LLVM-oriented route, but it must keep LLVM downstream from FROG source, validated meaning, canonical FIR, lowering, and backend contract. Any contract that turns LLVM into FROG identity must be rejected.
</p>
