<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">01 — Language-Valid but Profile-Rejected</h1>

<p align="center">
  <strong>Negative compiler-corridor case where the program is valid FROG but outside the declared native CPU LLVM profile surface</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Case Identifier</h2>

<pre><code>invalid/compiler/01_language_valid_but_profile_rejected</code></pre>

<hr/>

<h2>Purpose</h2>

<p>
This case verifies that a program may be fully valid at the core FROG language level while still being rejected by a declared compilation profile.
</p>

<pre><code>language-valid
   !=
profile-valid</code></pre>

<hr/>

<h2>Source Scenario</h2>

<p>
The program is assumed to be:
</p>

<ul>
  <li>structurally valid canonical FROG source,</li>
  <li>semantically accepted by core FROG,</li>
  <li>but dependent on a capability surface intentionally outside the conservative first <code>native_cpu_llvm</code> subset.</li>
</ul>

<hr/>

<h2>Expected Loadability</h2>

<pre><code>Expected loadability: loadable</code></pre>

<h2>Expected Structural Validity</h2>

<pre><code>Expected structural validity: valid</code></pre>

<h2>Expected Semantic Acceptance</h2>

<pre><code>Expected semantic acceptance: accepted</code></pre>

<h2>Expected IR Derivation</h2>

<pre><code>Expected IR derivation: derivable</code></pre>

<h2>Expected Canonical JSON IR Result</h2>

<pre><code>Expected IR schema result: schema-valid</code></pre>

<h2>Declared Profile Scope</h2>

<pre><code>Declared profile scope: native_cpu_llvm</code></pre>

<h2>Expected Lowering Result</h2>

<pre><code>Expected lowering result: rejected</code></pre>

<h2>Expected Backend-Contract Result</h2>

<pre><code>Expected backend-contract result: not applicable</code></pre>

<h2>Expected Backend-Family Consumption</h2>

<pre><code>Expected backend-family consumption: not applicable</code></pre>

<h2>Expected Rejection Stage</h2>

<pre><code>Expected rejection stage: profile-gated lowering entry</code></pre>

<h2>Expected Rejection Reason</h2>

<pre><code>Expected rejection reason:
program is valid FROG,
but outside the conservative accepted subset of native_cpu_llvm</code></pre>

<h2>Required Preserved Distinctions</h2>

<ul>
  <li>language validity remains distinct from profile validity,</li>
  <li>canonical IR derivability remains distinct from profile lowerability,</li>
  <li>profile rejection remains distinct from semantic rejection.</li>
</ul>

<h2>Summary</h2>

<p>
This case proves that the repository can reject a compilation-profile claim without pretending that the underlying program is semantically invalid as FROG.
</p>
