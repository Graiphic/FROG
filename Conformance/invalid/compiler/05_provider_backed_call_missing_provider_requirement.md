<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">05 - Provider-Backed Call Missing Provider Requirement</h1>

<p align="center">
  <strong>Negative source/FIR/lowering case where a provider-backed call loses its required provider boundary</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Case Identifier</h2>

<pre><code>invalid/compiler/05_provider_backed_call_missing_provider_requirement</code></pre>

<h2>Primary Owner Stage</h2>

<pre><code>Primary owner stage: backend-contract emission</code></pre>

<h2>Purpose</h2>

<p>
This case verifies that a provider-backed standard-library call must not reach a backend contract or manifest-compatible handoff without an explicit provider, capability, or dependency requirement.
</p>

<h2>Specification Owners</h2>

<ul>
  <li><a href="../../../IR/Library call model.md"><code>IR/Library call model.md</code></a> owns the FIR-level provider requirement.</li>
  <li><a href="../../../IR/Lowering.md"><code>IR/Lowering.md</code></a> owns the lowering provider/capability boundary.</li>
  <li><a href="../../../IR/Backend contract.md"><code>IR/Backend contract.md</code></a> owns consumer-facing provider slots and rejection conditions.</li>
  <li><a href="../../../Libraries/Image.md"><code>Libraries/Image.md</code></a> owns the published <code>frog.image</code> primitive contract used by the scenario.</li>
</ul>

<h2>Source Scenario</h2>

<p>
The upstream program is structurally valid and semantically accepted. It calls a provider-backed standard-library function such as <code>frog.image.decode_file_rgba8</code>.
</p>

<p>
The invalid artifact is a lowered form, backend contract, or manifest-compatible handoff that carries the call or its lowered consequence but omits the provider requirement, host capability, provider slot, or dependency closure needed to satisfy it.
</p>

<h2>Expected Outcomes</h2>

<pre><code>Expected loadability: loadable
Expected structural validity: valid
Expected semantic acceptance: accepted
Expected IR derivation: derivable
Expected IR schema result: schema-valid
Expected lowering result: rejected or not contract-emittable when the provider boundary is missing
Expected backend-contract result: not emittable
Expected provider resolution: not applicable
Expected backend-family consumption: not applicable</code></pre>

<h2>Expected Rejection Stage</h2>

<pre><code>Expected rejection stage: backend-contract emission</code></pre>

<h2>Expected Rejection Reason</h2>

<pre><code>Expected rejection reason:
the FIR contains a provider-backed public call,
but the lowered/contracted handoff omits the abstract provider requirement,
required capability, provider slot, or dependency closure</code></pre>

<h2>Must Not Happen</h2>

<ul>
  <li>the runtime or launcher must not discover a hidden dependency by filename convention,</li>
  <li>the contract must not silently assume one private provider implementation,</li>
  <li>the missing provider requirement must not be reclassified as valid provider resolution,</li>
  <li>the public FIR call identity must not be rewritten into a private native symbol to avoid declaring the provider.</li>
</ul>

<h2>Required Preserved Distinctions</h2>

<ul>
  <li>FIR call identity remains distinct from concrete provider binding,</li>
  <li>provider requirement remains distinct from runtime-private loader behavior,</li>
  <li>backend-contract failure remains distinct from source or semantic invalidity.</li>
</ul>

<h2>Summary</h2>

<p>
A conforming producer must reject this handoff at the backend-contract boundary. Provider-backed calls are portable only when their public provider and capability requirements survive into the consumer-facing contract or manifest-compatible surface.
</p>
