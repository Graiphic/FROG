<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">11 - System Library Capability or Subset Must Report Explicitly</h1>

<p align="center">
  <strong>Negative source/FIR/lowering case for denied, unavailable, or unsupported <code>frog.system</code> host capability</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Case Identifier</h2>

<pre><code>invalid/compiler/11_system_library_capability_or_subset_must_report_explicitly</code></pre>

<h2>Primary Owner Stage</h2>

<pre><code>Primary owner stage: manifest / provider resolution</code></pre>

<h2>Purpose</h2>

<p>
This case verifies that a valid <code>frog.system</code> call must be rejected,
reported unsupported, or executed with <code>success = false</code> according to
the published local status contract when the selected host, profile, provider,
or deployment policy cannot satisfy its capability requirement.
</p>

<h2>Specification Owners</h2>

<ul>
  <li><a href="../../../Libraries/System.md"><code>Libraries/System.md</code></a> owns the public capability tokens, partial-support claims, local error codes, fallback outputs, and denied/unsupported posture.</li>
  <li><a href="../../../IR/Library call model.md"><code>IR/Library call model.md</code></a> owns preservation of host-capability requirements in FIR.</li>
  <li><a href="../../../IR/Backend contract.md"><code>IR/Backend contract.md</code></a> owns consumer-facing capability and provider resolution failure posture.</li>
</ul>

<h2>Source Scenario</h2>

<p>
The source is structurally valid and semantically accepted. It calls a
published <code>frog.system</code> primitive such as
<code>frog.system.environment_get</code>,
<code>frog.system.current_directory</code>, or
<code>frog.system.cpu_load_percent</code>.
</p>

<p>
The selected execution profile, package, launcher, or host cannot satisfy the
required capability, explicitly supports only a subset, denies the read, or
cannot provide the requested observation.
</p>

<h2>Expected Outcomes</h2>

<pre><code>Expected loadability: loadable
Expected structural validity: valid
Expected semantic acceptance: accepted
Expected IR derivation: derivable
Expected IR schema result: schema-valid
Expected lowering result: lowerable when the capability requirement is preserved
Expected backend-contract result: emittable only if the unsatisfied capability remains explicit
Expected provider resolution: rejected or reported unsupported/denied/unavailable
Expected backend-family consumption: rejected or status-bearing according to the declared host posture</code></pre>

<h2>Expected Rejection or Report Stage</h2>

<pre><code>Expected rejection/report stage:
manifest / provider resolution
or runtime / launcher / backend consumer status reporting</code></pre>

<h2>Expected Rejection or Report Reason</h2>

<pre><code>Expected reason:
the required public frog.system capability is unsupported,
denied by host policy,
blocked by deployment posture,
or unavailable from the active execution context</code></pre>

<h2>Must Not Happen</h2>

<ul>
  <li>unsupported <code>frog.system</code> calls must not return invented successful values,</li>
  <li>denied environment, process, CPU, memory, platform, or capability reads must not be silently replaced with host defaults,</li>
  <li>a partial-support implementation must not claim full <code>frog.system</code> v0.1 support,</li>
  <li>host-private policy failures must not be encoded as new public primitive ids,</li>
  <li>runtime diagnostics must not change the standardized output ports or fallback behavior.</li>
</ul>

<h2>Required Preserved Distinctions</h2>

<ul>
  <li>valid source remains distinct from unsatisfied host capability,</li>
  <li>unsupported subset remains distinct from unknown primitive identity,</li>
  <li>permission denial remains distinct from invalid input,</li>
  <li>public capability tokens remain distinct from private host permission mechanisms.</li>
</ul>

<h2>Summary</h2>

<p>
A valid <code>frog.system</code> call may still fail under a selected host,
profile, package, or provider posture. That failure must remain explicit
through rejection or the published local status tuple. Silent success with
invented introspection values is non-conforming.
</p>
