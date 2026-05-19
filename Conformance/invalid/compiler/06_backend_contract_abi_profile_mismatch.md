<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">06 - Backend Contract ABI Profile Mismatch</h1>

<p align="center">
  <strong>Negative source/FIR/lowering case where a consumer rejects an explicit ABI or profile mismatch</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Case Identifier</h2>

<pre><code>invalid/compiler/06_backend_contract_abi_profile_mismatch</code></pre>

<h2>Primary Owner Stage</h2>

<pre><code>Primary owner stage: runtime / launcher / backend consumer</code></pre>

<h2>Purpose</h2>

<p>
This case verifies that a backend-family consumer must reject a contract or manifest-compatible handoff when the declared ABI, target profile, backend family, or artifact assumptions are incompatible with the selected consumer.
</p>

<h2>Specification Owners</h2>

<ul>
  <li><a href="../../../IR/Backend contract.md"><code>IR/Backend contract.md</code></a> owns backend-family, target profile, ABI, capability, and consumer rejection obligations.</li>
  <li><a href="../../../Profiles/Native CPU LLVM.md"><code>Profiles/Native CPU LLVM.md</code></a> owns the optional <code>native_cpu_llvm</code> profile claim and rejection posture.</li>
  <li><a href="../../../IR/Lowering.md"><code>IR/Lowering.md</code></a> owns target-oriented specialization before contract emission.</li>
</ul>

<h2>Source Scenario</h2>

<p>
The upstream source is structurally valid, semantically accepted, FIR-derivable, lowerable, and contract-emittable. The invalidity is downstream: the emitted contract declares assumptions that the selected consumer cannot honor.
</p>

<p>
Examples include a contract that declares one ABI profile while referencing an artifact built for another ABI profile, a target architecture mismatch, or a backend-family declaration that does not match the selected consumer.
</p>

<h2>Expected Outcomes</h2>

<pre><code>Expected loadability: loadable
Expected structural validity: valid
Expected semantic acceptance: accepted
Expected IR derivation: derivable
Expected IR schema result: schema-valid
Expected lowering result: lowerable
Expected backend-contract result: emittable as an explicit but incompatible handoff
Expected provider resolution: rejected
Expected backend-family consumption: rejected</code></pre>

<h2>Expected Rejection Stage</h2>

<pre><code>Expected rejection stage: backend-family consumer intake</code></pre>

<h2>Expected Rejection Reason</h2>

<pre><code>Expected rejection reason:
the consumer cannot honor the declared ABI profile,
target profile,
backend family,
artifact kind,
or representation assumptions</code></pre>

<h2>Must Not Happen</h2>

<ul>
  <li>the consumer must not reinterpret the artifact with guessed calling conventions,</li>
  <li>the consumer must not silently substitute a different profile claim,</li>
  <li>the mismatch must not be reported as invalid FROG source,</li>
  <li>an implementation-private ABI must not become language law.</li>
</ul>

<h2>Required Preserved Distinctions</h2>

<ul>
  <li>backend-family declaration remains distinct from target profile,</li>
  <li>target profile remains distinct from ABI profile,</li>
  <li>contract emission remains distinct from consumer acceptance,</li>
  <li>consumer rejection remains distinct from semantic rejection.</li>
</ul>

<h2>Summary</h2>

<p>
A conforming backend-family consumer must reject this case explicitly. Compatibility is not established by the mere existence of a lowered artifact; it is established only when the declared contract assumptions match what the consumer can honor.
</p>
