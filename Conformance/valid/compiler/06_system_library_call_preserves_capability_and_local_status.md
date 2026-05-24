<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">06 - System Library Call Preserves Capability and Local Status</h1>

<p align="center">
  <strong>Positive source/FIR/lowering case for a bounded <code>frog.system</code> host-capability read</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Case Identifier</h2>

<pre><code>valid/compiler/06_system_library_call_preserves_capability_and_local_status</code></pre>

<h2>Primary Owner Stage</h2>

<pre><code>Primary owner stage: manifest / provider resolution</code></pre>

<p>
The case also depends on semantic validation, FIR library-call preservation,
lowering, backend-contract emission, and host capability resolution.
</p>

<h2>Purpose</h2>

<p>
This case verifies that a published <code>frog.system</code> introspection call
remains explicit across the public execution corridor without turning private
host policy or runtime provider behavior into language law.
</p>

<pre><code>.frog source
   -> semantic validation
   -> FIR host-capability call with local status tuple
   -> lowering host-capability boundary
   -> backend contract or manifest-compatible handoff
   -> provider/host capability resolution without private runtime guessing</code></pre>

<h2>Specification Owners</h2>

<ul>
  <li><a href="../../../Libraries/System.md"><code>Libraries/System.md</code></a> owns the published <code>frog.system</code> primitive catalog, port contracts, local status tuple, fallback behavior, and public capability tokens.</li>
  <li><a href="../../../IR/Library call model.md"><code>IR/Library call model.md</code></a> owns FIR call identity, effect/status metadata, and abstract provider or capability requirements.</li>
  <li><a href="../../../IR/Lowering.md"><code>IR/Lowering.md</code></a> owns target-oriented provider and host-capability boundary preparation.</li>
  <li><a href="../../../IR/Backend contract.md"><code>IR/Backend contract.md</code></a> owns consumer-facing capability requirements, provider slots, manifests, diagnostics, and rejection conditions.</li>
</ul>

<h2>Source Scenario</h2>

<p>
The program uses a published <code>frog.system</code> primitive such as
<code>frog.system.platform_info</code>, <code>frog.system.cpu_logical_count</code>,
<code>frog.system.memory_available_bytes</code>, or
<code>frog.system.environment_get</code>.
</p>

<p>
The source names only the public primitive and its public ports. It does not
name a private provider class, native API, DLL, service, environment policy
table, runtime hook, or host-specific implementation detail.
</p>

<h2>Expected Outcomes</h2>

<pre><code>Expected loadability: loadable
Expected structural validity: valid
Expected semantic acceptance: accepted
Expected IR derivation: derivable
Expected IR schema result: schema-valid
Expected lowering result: lowerable
Expected backend-contract result: emittable
Expected provider resolution: resolvable when the declared host capability is available
Expected backend-family consumption: consumable by a consumer that honors the declared host-capability contract</code></pre>

<h2>Expected FIR Features</h2>

<ul>
  <li>the FIR object preserves the exact <code>frog.system.*</code> call identity,</li>
  <li>all required input and output ports remain explicit and typed according to <code>Libraries/System.md</code>,</li>
  <li>the call class remains <code>host_capability</code> or an equivalent explicit standard-library host-capability class,</li>
  <li>the effect remains <code>host_capability_read</code> or an equivalent read-only effect,</li>
  <li>the status model remains <code>local_result_tuple</code> with <code>success</code> and <code>error_code</code>,</li>
  <li>the relevant public capability token, such as <code>frog.system.platform.read</code> or <code>frog.system.environment.read</code>, remains visible to lowering and backend-contract emission.</li>
</ul>

<h2>Expected Local Status Behavior</h2>

<ul>
  <li>when <code>success = true</code>, <code>error_code</code> is the empty string,</li>
  <li>when <code>success = false</code>, every data output remains present, well typed, and set to the fallback value defined by <code>Libraries/System.md</code>,</li>
  <li>host denial, unsupported subset, unavailable information, invalid input, and unknown failure remain explicit status outcomes rather than successful invented values.</li>
</ul>

<h2>Expected Lowering and Contract Features</h2>

<ul>
  <li>lowering preserves the public host-capability requirement instead of selecting a private provider as language truth,</li>
  <li>the backend contract or manifest-compatible handoff declares the capability and provider boundary required by the selected execution posture,</li>
  <li>consumer-facing diagnostics distinguish unsupported, denied, unavailable, invalid-input, and dependency-resolution failures where the selected posture exposes them,</li>
  <li>the contract remains implementation-neutral and does not require Graiphic's private runtime to be conforming.</li>
</ul>

<h2>Required Preserved Distinctions</h2>

<ul>
  <li>public <code>frog.system</code> call identity remains distinct from concrete provider identity,</li>
  <li>public capability tokens remain distinct from operating-system permission names and private policy tables,</li>
  <li>local status outputs remain part of the published primitive contract,</li>
  <li>host-dependent results remain explicit observations, not source constants or hidden runtime defaults.</li>
</ul>

<h2>Case Classification</h2>

<pre><code>Case classification:
- positive
- source/FIR/lowering
- frog.system
- host-capability read
- local status tuple
- implementation-independent</code></pre>

<h2>Summary</h2>

<p>
A conforming implementation that claims the relevant <code>frog.system</code>
support must accept, derive, lower, contract-emit, and resolve this case when
the declared host capability is available. It must do so without turning one
private runtime provider, operating-system API, or host policy mechanism into
FROG language law.
</p>
