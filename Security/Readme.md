<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Security and Compliance Alignment</h1>

<p align="center">
  <strong>Non-normative security, assurance, and compliance-alignment framing for FROG</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#scope">2. Scope</a></li>
  <li><a href="#available-pages">3. Available Pages</a></li>
  <li><a href="#relationship-to-the-specification">4. Relationship to the Specification</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This directory collects non-normative security, assurance, and compliance-alignment material for FROG.
Its purpose is to explain how FROG's explicit artifact architecture can support software-governance discussions without turning the language specification into a compliance product.
</p>

<p>
FROG is designed around inspectable source artifacts, validated meaning, open execution-facing FIR, explicit lowering, backend contracts, reference-consumer artifacts, and acceptance-oriented evidence.
Those properties can be useful in security-sensitive and defense-oriented environments because they reduce opacity across the software creation and execution chain.
</p>

<hr/>

<h2 id="scope">2. Scope</h2>

<p>
The material in this directory is explanatory.
It does not define mandatory conformance requirements by itself.
It does not certify any implementation, organization, product, contractor, or deployment environment.
</p>

<p>
In particular, compliance frameworks such as CMMC, NIST SP 800-171, SBOM programs, secure software development frameworks, or supply-chain security requirements apply to organizations, systems, development environments, operational procedures, and contracts.
FROG can support technical evidence generation for some of those concerns, but it does not replace organizational security controls.
</p>

<hr/>

<h2 id="available-pages">3. Available Pages</h2>

<ul>
  <li>
    <a href="./CMMC-Alignment.md">CMMC Alignment</a> — explains how FROG can support CMMC-aligned evidence generation for software provenance, artifact integrity, configuration control, auditability, and AI-generated-code governance.
  </li>
</ul>

<hr/>

<h2 id="relationship-to-the-specification">4. Relationship to the Specification</h2>

<p>
The authoritative FROG language definition remains distributed across the specification-owned areas of the repository, including <code>Expression/</code>, <code>Language/</code>, <code>IR/</code>, <code>Libraries/</code>, <code>Profiles/</code>, <code>IDE/</code>, <code>Conformance/</code>, and the centralized version-governance surface.
</p>

<p>
This directory should be read as a positioning and assurance-alignment layer.
It explains why FROG's explicit artifact model may be valuable for regulated, security-sensitive, and defense-oriented software ecosystems, while keeping compliance claims bounded and technically defensible.
</p>
