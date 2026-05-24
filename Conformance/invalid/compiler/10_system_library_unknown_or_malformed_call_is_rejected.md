<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">10 - System Library Unknown or Malformed Call Is Rejected</h1>

<p align="center">
  <strong>Negative source/FIR/lowering case for invalid <code>frog.system</code> call identity, ports, or inputs</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Case Identifier</h2>

<pre><code>invalid/compiler/10_system_library_unknown_or_malformed_call_is_rejected</code></pre>

<h2>Primary Owner Stage</h2>

<pre><code>Primary owner stage: semantic validation</code></pre>

<h2>Purpose</h2>

<p>
This case verifies that invalid <code>frog.system</code> source usage is rejected
or explicitly reported at the correct stage rather than repaired by runtime
convention.
</p>

<h2>Specification Owners</h2>

<ul>
  <li><a href="../../../Libraries/System.md"><code>Libraries/System.md</code></a> owns the valid primitive ids, required ports, types, local status tuple, environment-name rule, and capability-id rule.</li>
  <li><a href="../../../IR/Library call model.md"><code>IR/Library call model.md</code></a> owns the rule that unknown or unsupported call identity must remain explicit.</li>
  <li><a href="../../../Conformance/Source FIR Lowering conformance matrix.md"><code>Conformance/Source FIR Lowering conformance matrix.md</code></a> owns the staged rejection vocabulary.</li>
</ul>

<h2>Invalid Scenarios</h2>

<ul>
  <li>the source references an unknown primitive such as <code>frog.system.registry_read</code> or <code>frog.system.process_spawn</code>,</li>
  <li>a required input port is missing, such as <code>name</code> for <code>frog.system.environment_get</code>,</li>
  <li>a required input port has the wrong type,</li>
  <li>the environment variable name is the empty string,</li>
  <li>a capability query uses an unknown or malformed capability id.</li>
</ul>

<h2>Expected Outcomes</h2>

<pre><code>Expected loadability: loadable when the file shape itself is well formed
Expected structural validity: valid when only the primitive contract is wrong
Expected semantic acceptance: rejected
Expected IR derivation: not derivable
Expected IR schema result: not applicable
Expected lowering result: not applicable
Expected backend-contract result: not applicable
Expected provider resolution: not applicable
Expected backend-family consumption: not applicable</code></pre>

<h2>Expected Rejection Stage</h2>

<pre><code>Expected rejection stage: semantic validation</code></pre>

<h2>Expected Rejection Reason</h2>

<pre><code>Expected rejection reason:
the source references an unknown or invalid frog.system primitive,
omits a required public port,
uses an incompatible port type,
or violates a published frog.system input rule</code></pre>

<h2>Must Not Happen</h2>

<ul>
  <li>the implementation must not invent a private system primitive to satisfy the call,</li>
  <li>the implementation must not silently substitute a different public primitive,</li>
  <li>the implementation must not repair missing ports from runtime defaults,</li>
  <li>an empty environment variable name must not be treated as a request to enumerate or read arbitrary environment state,</li>
  <li>a malformed capability id must not be interpreted as a host-private permission name.</li>
</ul>

<h2>Required Preserved Distinctions</h2>

<ul>
  <li>semantic rejection remains distinct from runtime execution failure,</li>
  <li>public primitive identity remains distinct from private provider identity,</li>
  <li>invalid input remains distinct from denied or unsupported host capability.</li>
</ul>

<h2>Summary</h2>

<p>
Invalid <code>frog.system</code> source usage must stop before FIR derivation.
Runtime support cannot retroactively make an unknown primitive, missing port,
wrong type, empty environment name, or malformed capability id valid FROG.
</p>
