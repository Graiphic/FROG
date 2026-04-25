<h1>FROG Trusted Authoring Profile</h1>

<p>Optional future profile for stricter provenance, review, issuer-trust, and deployment-gating policies.</p>
<p>FROG — Free Open Graphical Language</p>

<hr />

<section id="status">
  <h2>Status</h2>

  <p>This document is a future-profile stub. It SHOULD NOT be treated as part of the base v0.1 source format unless the repository intentionally promotes it.</p>

  <p>The base FROG source format allows <code>.frog</code> programs without provenance. This optional profile would define stricter organization, certification, or deployment policies for environments that require signed provenance and human review before execution, build, release, or deployment.</p>
</section>

<hr />

<section id="intended-scope">
  <h2>Intended Scope</h2>

  <p>A trusted-authoring profile may eventually define:</p>

  <ul>
    <li>required provenance coverage for selected source object families;</li>
    <li>accepted signature algorithms and issuer categories;</li>
    <li>trusted issuer policy declaration format;</li>
    <li>required human review states before deployment;</li>
    <li>behavior for stale, missing, invalid, unsupported, or untrusted attestations;</li>
    <li>minimum IDE warnings and review workflows;</li>
    <li>conformance cases for provenance preservation and policy rejection.</li>
  </ul>
</section>

<hr />

<section id="boundary">
  <h2>Boundary</h2>

  <p>This profile must remain optional.</p>

  <p>It must not redefine base FROG executable semantics. It may define stricter acceptance gates for a toolchain, organization, product, certification path, or deployment context.</p>

  <pre><code>base .frog validity
    != trusted-authoring deployment eligibility</code></pre>
</section>
