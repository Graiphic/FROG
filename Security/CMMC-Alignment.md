<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">CMMC Alignment</h1>

<p align="center">
  <strong>Non-normative alignment notes for software provenance, auditability, and artifact integrity</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#purpose">1. Purpose</a></li>
  <li><a href="#important-boundary">2. Important Boundary</a></li>
  <li><a href="#why-cmmc-matters-to-frog">3. Why CMMC Matters to FROG</a></li>
  <li><a href="#where-frog-can-support-cmmc-aligned-evidence">4. Where FROG Can Support CMMC-Aligned Evidence</a></li>
  <li><a href="#software-artifact-evidence-chain">5. Software Artifact Evidence Chain</a></li>
  <li><a href="#ai-generated-and-ai-assisted-code-governance">6. AI-Generated and AI-Assisted Code Governance</a></li>
  <li><a href="#recommended-language">7. Recommended Language</a></li>
  <li><a href="#non-goals">8. Non-Goals</a></li>
  <li><a href="#future-work">9. Future Work</a></li>
</ul>

<hr/>

<h2 id="purpose">1. Purpose</h2>

<p>
This document explains how FROG can support CMMC-aligned software-governance evidence in defense-oriented and security-sensitive environments.
It is non-normative and does not define certification requirements for FROG implementations.
</p>

<p>
CMMC applies to contractor organizations and their information systems when required by applicable U.S. Department of Defense contracts.
FROG is an open graphical language foundation.
The useful relationship between the two is therefore not that FROG is CMMC compliant by itself, but that FROG can help generate technical evidence relevant to CMMC-aligned software assurance, provenance, auditability, configuration control, and integrity-verification practices.
</p>

<hr/>

<h2 id="important-boundary">2. Important Boundary</h2>

<p>
FROG is not a CMMC compliance product.
FROG does not certify an organization, contractor, information system, enclave, cloud environment, development process, or operational deployment.
</p>

<p>
CMMC compliance depends on contractual scope, handled information types, organizational controls, system boundaries, access control, incident response, configuration management, audit and accountability practices, security assessment, and other controls implemented by the organization operating the system.
</p>

<p>
FROG can support a narrower but important technical layer:
<strong>software artifact evidence generation</strong>.
That evidence may be useful to organizations building CMMC-aligned development workflows, especially where software is generated, transformed, reviewed, lowered, compiled, deployed, or executed across multiple tools.
</p>

<hr/>

<h2 id="why-cmmc-matters-to-frog">3. Why CMMC Matters to FROG</h2>

<p>
CMMC is relevant to FROG because defense-oriented software ecosystems increasingly need to show not only that software works, but also where it came from, how it changed, who or what transformed it, how it was reviewed, and which executable artifact was ultimately produced.
</p>

<p>
FROG's architecture is naturally aligned with that type of evidence because it separates:
</p>

<ul>
  <li>canonical <code>.frog</code> source,</li>
  <li>source-object metadata and optional provenance,</li>
  <li>validated language meaning,</li>
  <li>open execution-facing FIR,</li>
  <li>lowering artifacts,</li>
  <li>backend contracts,</li>
  <li>runtime-family consumption,</li>
  <li>compiler-family consumption,</li>
  <li>observed execution and acceptance artifacts.</li>
</ul>

<p>
This separation creates inspectable checkpoints across the software lifecycle.
Those checkpoints can become evidence surfaces for audit, review, integrity verification, and configuration-control workflows.
</p>

<hr/>

<h2 id="where-frog-can-support-cmmc-aligned-evidence">4. Where FROG Can Support CMMC-Aligned Evidence</h2>

<table>
  <thead>
    <tr>
      <th>Governance Concern</th>
      <th>FROG Support Surface</th>
      <th>Evidence Value</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><strong>Software provenance</strong></td>
      <td>Structured <code>.frog</code> source, optional <code>ide.provenance</code>, manifestable artifact lineage.</td>
      <td>Helps identify whether logic is human-authored, AI-generated, AI-assisted, imported, reviewed, stale, invalid, or untrusted.</td>
    </tr>
    <tr>
      <td><strong>Auditability</strong></td>
      <td>Readable graph source, validated meaning, open FIR, lowering records, backend contracts.</td>
      <td>Allows reviewers to inspect the chain from authored intent to execution-facing representation.</td>
    </tr>
    <tr>
      <td><strong>Configuration control</strong></td>
      <td>Versioned source artifacts, generated FIR, generated lowering, generated contracts, acceptance snapshots.</td>
      <td>Supports comparison, review, and controlled evolution of executable logic and derived artifacts.</td>
    </tr>
    <tr>
      <td><strong>Integrity verification</strong></td>
      <td>Hashable and signable source, FIR, lowering, backend contract, and build-output artifacts.</td>
      <td>Supports detection of unauthorized modification across the artifact chain.</td>
    </tr>
    <tr>
      <td><strong>Human review</strong></td>
      <td>Explicit nodes, ports, connections, regions, state boundaries, public interfaces, and UI-facing objects.</td>
      <td>Improves reviewability compared with opaque generated code or backend-private intermediate forms.</td>
    </tr>
    <tr>
      <td><strong>Runtime and compiler accountability</strong></td>
      <td>Backend contracts and runtime/compiler-family separation.</td>
      <td>Helps distinguish language meaning from downstream execution-consumer behavior.</td>
    </tr>
    <tr>
      <td><strong>Generated-code governance</strong></td>
      <td>AI-compatible structured source plus reviewable graph and explicit downstream derivation.</td>
      <td>Supports review and acceptance workflows for AI-generated or AI-assisted system logic.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2 id="software-artifact-evidence-chain">5. Software Artifact Evidence Chain</h2>

<p>
A CMMC-aligned FROG software-governance workflow can treat each transformation stage as an explicit evidence point:
</p>

<pre><code>.frog source
  -> source provenance metadata
  -> semantic validation report
  -> FIR artifact
  -> lowering artifact
  -> backend contract
  -> runtime or compiler consumer
  -> build or execution output
  -> acceptance snapshot
  -> signed or hashed evidence manifest
</code></pre>

<p>
The exact controls belong to the organization operating the workflow.
FROG's role is to provide a language and artifact architecture where those evidence points can be generated, inspected, versioned, compared, hashed, signed, and reviewed.
</p>

<p>
A future evidence manifest could include:
</p>

<ul>
  <li>source artifact identifiers,</li>
  <li>source artifact hashes,</li>
  <li>FIR artifact hashes,</li>
  <li>lowering artifact hashes,</li>
  <li>backend-contract hashes,</li>
  <li>toolchain identifiers,</li>
  <li>runtime or compiler-family identifiers,</li>
  <li>validation results,</li>
  <li>acceptance results,</li>
  <li>review state,</li>
  <li>signature metadata.</li>
</ul>

<hr/>

<h2 id="ai-generated-and-ai-assisted-code-governance">6. AI-Generated and AI-Assisted Code Governance</h2>

<p>
CMMC-oriented environments will increasingly need to manage software artifacts that may be authored, modified, generated, or transformed with AI assistance.
That creates a practical review problem: generated code can be produced faster than humans can comfortably inspect opaque textual output.
</p>

<p>
FROG addresses this problem at the representation layer.
AI-generated or AI-assisted logic can be expressed as structured <code>.frog</code> source, then inspected as graph-level program structure, validated semantically, lowered explicitly, and handed to downstream runtime or compiler consumers through explicit artifacts.
</p>

<p>
The intended benefit is not automatic trust.
The intended benefit is controlled inspection:
humans and organizations can review the structure, provenance, transformations, and acceptance artifacts before the generated logic becomes operational software.
</p>

<hr/>

<h2 id="recommended-language">7. Recommended Language</h2>

<p>
Preferred wording:
</p>

<blockquote>
  FROG is not a CMMC compliance product and does not replace organizational CMMC controls. However, its explicit artifact model can support CMMC-aligned evidence generation for software provenance, auditability, configuration control, and integrity verification.
</blockquote>

<p>
Alternative short wording:
</p>

<blockquote>
  FROG can support CMMC-aligned software-governance evidence by making source, FIR, lowering, backend contracts, runtime/compiler consumption, and acceptance artifacts explicit and inspectable.
</blockquote>

<p>
Wording to avoid:
</p>

<ul>
  <li><code>FROG is CMMC compliant.</code></li>
  <li><code>FROG guarantees CMMC compliance.</code></li>
  <li><code>Using FROG makes an organization CMMC compliant.</code></li>
</ul>

<hr/>

<h2 id="non-goals">8. Non-Goals</h2>

<ul>
  <li>This document does not provide a CMMC assessment.</li>
  <li>This document does not map every CMMC practice to FROG.</li>
  <li>This document does not define a certified FROG security profile.</li>
  <li>This document does not claim that graphical programming is inherently secure.</li>
  <li>This document does not replace secure software development practices.</li>
  <li>This document does not replace organizational controls, policies, procedures, or evidence required by a contract.</li>
</ul>

<hr/>

<h2 id="future-work">9. Future Work</h2>

<p>
The following future work would strengthen FROG's software-governance posture:
</p>

<ul>
  <li>a signed artifact-manifest format,</li>
  <li>a reference <code>frog-verify</code> tool for checking artifact-chain integrity,</li>
  <li>a reference provenance manifest for the published examples,</li>
  <li>a hash manifest for <code>.frog</code>, FIR, lowering, and backend-contract artifacts,</li>
  <li>a security-oriented conformance profile for artifact integrity and provenance preservation,</li>
  <li>an example review workflow for AI-generated or AI-assisted FROG source,</li>
  <li>a threat model covering artifact tampering, stale provenance, untrusted imports, backend substitution, and runtime/compiler mismatch.</li>
</ul>

<p>
Those additions should remain carefully bounded.
They can make FROG stronger as a software-governance foundation, but they should not imply that FROG alone provides organizational compliance.
</p>
