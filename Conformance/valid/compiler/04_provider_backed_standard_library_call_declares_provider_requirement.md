<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">04 - Provider-Backed Standard-Library Call Declares Provider Requirement</h1>

<p align="center">
  <strong>Positive source/FIR/lowering case for a provider-backed standard-library call with explicit dependency closure</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Case Identifier</h2>

<pre><code>valid/compiler/04_provider_backed_standard_library_call_declares_provider_requirement</code></pre>

<h2>Primary Owner Stage</h2>

<pre><code>Primary owner stage: manifest / provider resolution</code></pre>

<p>
The case also has required prerequisites in semantic validation, FIR library-call preservation, lowering, and backend-contract emission.
</p>

<h2>Purpose</h2>

<p>
This case verifies that a validated provider-backed standard-library call remains explicit across the public execution corridor.
</p>

<pre><code>.frog source
   -&gt; semantic validation
   -&gt; FIR call object with abstract provider requirement
   -&gt; lowering provider boundary
   -&gt; backend contract or manifest-compatible handoff
   -&gt; provider resolution without private runtime guessing</code></pre>

<h2>Specification Owners</h2>

<ul>
  <li><a href="../../../Libraries/Image.md"><code>Libraries/Image.md</code></a> owns the published <code>frog.image.decode_file_rgba8</code> primitive contract.</li>
  <li><a href="../../../IR/Library call model.md"><code>IR/Library call model.md</code></a> owns FIR call identity, effect/status metadata, and abstract provider requirements.</li>
  <li><a href="../../../IR/Lowering.md"><code>IR/Lowering.md</code></a> owns target-oriented provider and dependency-closure preparation.</li>
  <li><a href="../../../IR/Backend contract.md"><code>IR/Backend contract.md</code></a> owns consumer-facing provider slots, capability requirements, artifacts, ABI assumptions, and rejection conditions.</li>
</ul>

<h2>Source Scenario</h2>

<p>
The program uses a published <code>frog.image</code> read primitive such as <code>frog.image.decode_file_rgba8</code> to decode a path value into an image buffer value with local status outputs.
</p>

<p>
The source does not name a private codec library, native symbol, loader path, or runtime implementation. It uses the public namespaced primitive and any explicit profile or package posture needed by the selected execution route.
</p>

<h2>Expected Outcomes</h2>

<pre><code>Expected loadability: loadable
Expected structural validity: valid
Expected semantic acceptance: accepted
Expected IR derivation: derivable
Expected IR schema result: schema-valid
Expected lowering result: lowerable
Expected backend-contract result: emittable
Expected provider resolution: resolvable when the declared provider/capability is available
Expected backend-family consumption: consumable by a consumer that honors the declared provider requirements</code></pre>

<h2>Expected FIR Features</h2>

<ul>
  <li>the FIR object preserves <code>kind = frog.image.decode_file_rgba8</code>,</li>
  <li>typed ports remain explicit, including <code>path</code>, <code>image</code>, <code>success</code>, and <code>error_code</code>,</li>
  <li>the call class remains <code>standard_library_provider_backed</code>,</li>
  <li>the effect remains a declared host/provider capability read rather than an inferred runtime habit,</li>
  <li>the status model remains explicit and local to the published primitive contract,</li>
  <li>the provider requirement names an abstract public requirement, not a concrete private binary.</li>
</ul>

<h2>Expected Lowering and Contract Features</h2>

<ul>
  <li>lowering makes the provider boundary and dependency closure explicit for the selected backend family,</li>
  <li>the backend contract or manifest-compatible handoff declares provider slots and capability requirements,</li>
  <li>artifact, ABI, host capability, and dependency references are explicit where they are part of the selected posture,</li>
  <li>the contract remains a consumer-facing handoff rather than a private runtime loader recipe.</li>
</ul>

<h2>Required Preserved Distinctions</h2>

<ul>
  <li>FIR call identity remains distinct from concrete provider identity,</li>
  <li>provider requirement remains distinct from private implementation mechanism,</li>
  <li>status outputs remain ordinary published outputs unless a future status corridor says otherwise,</li>
  <li>lowering and manifest dependency closure remain downstream from validated FROG meaning.</li>
</ul>

<h2>Case Classification</h2>

<pre><code>Case classification:
- positive
- source/FIR/lowering
- provider-backed standard library
- manifest/provider resolution
- implementation-independent</code></pre>

<h2>Summary</h2>

<p>
A conforming implementation that claims this provider-capable corridor must accept, derive, lower, contract-emit, and resolve this case when the declared provider requirements are satisfied. It must do so without turning one concrete provider or runtime loader into FROG language law.
</p>
