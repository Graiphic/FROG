<p align="center">
  <img src="FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 FROG Governance</h1>

<p align="center">
  Governance model for the FROG specification repository<br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#governance-goals">2. Governance Goals</a></li>
  <li><a href="#scope-of-governance">3. Scope of Governance</a></li>
  <li><a href="#repository-layers-and-governance-boundaries">4. Repository Layers and Governance Boundaries</a></li>
  <li><a href="#repository-wide-framing-and-governance-surfaces">5. Repository-Wide Framing and Governance Surfaces</a></li>
  <li><a href="#stewardship">6. Stewardship</a></li>
  <li><a href="#open-specification-and-multiple-implementations">7. Open Specification and Multiple Implementations</a></li>
  <li><a href="#decision-model">8. Decision Model</a></li>
  <li><a href="#proposal-and-change-process">9. Proposal and Change Process</a></li>
  <li><a href="#compatibility-versioning-and-deprecation">10. Compatibility, Versioning, and Deprecation</a></li>
  <li><a href="#repository-contributions-and-licensing">11. Repository Contributions and Licensing</a></li>
  <li><a href="#commercial-implementations-and-ecosystem-participation">12. Commercial Implementations and Ecosystem Participation</a></li>
  <li><a href="#support-claims-conformance-and-certification">13. Support Claims, Conformance, and Certification</a></li>
  <li><a href="#trademarks-branding-and-official-claims">14. Trademarks, Branding, and Official Claims</a></li>
  <li><a href="#commercial-vs-non-commercial-certification-direction">15. Commercial vs Non-Commercial Certification Direction</a></li>
  <li><a href="#future-governance-evolution">16. Future Governance Evolution</a></li>
  <li><a href="#non-goals">17. Non-Goals</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
FROG is an open language specification for graphical dataflow programming.
Its purpose is to define a durable, tool-independent, and vendor-independent foundation for executable graphical programs, intrinsic primitive vocabularies, optional standardized capability profiles, and related program representations.
</p>

<p>
This governance document defines how the specification repository is stewarded, how changes are evaluated, how architectural coherence is preserved, and how an open specification may coexist with multiple independent implementations.
</p>

<p>
This document governs the specification repository.
It does not require any implementation to be open source, and it does not prohibit any party from building commercial tools, runtimes, compilers, integrations, services, or full IDE platforms around the FROG specification.
</p>

<p>
This document governs the specification repository and its repository-level governance boundaries.
It does not, by itself, fully define pricing, trademark procedure, detailed certification contracts, or every future business policy.
Those topics MAY be defined in separate policy documents if and when such documents are published.
</p>

<pre>
High-level governance model

open specification
        +
steward-led repository governance
        +
multiple independent implementations
        +
separate conformance / certification / branding authority
</pre>

<hr/>

<h2 id="governance-goals">2. Governance Goals</h2>

<p>
The governance of FROG exists to preserve the following goals:
</p>

<ul>
  <li>keep the specification open, readable, and implementable by independent parties,</li>
  <li>preserve long-term architectural coherence across the repository,</li>
  <li>avoid collapse of the language into one IDE, one runtime, one compiler, one profile implementation, or one hardware vendor,</li>
  <li>allow responsible evolution of the specification over time,</li>
  <li>support a healthy ecosystem of open and commercial implementations,</li>
  <li>keep a clean distinction between the core language, optional standardized profiles, and implementation-specific extensions,</li>
  <li>preserve a clear distinction between normative specification, strategic framing, roadmap sequencing, current corpus-version governance, conformance posture, and reference implementation work,</li>
  <li>protect the integrity of official FROG branding, certification, and compatibility claims.</li>
</ul>

<p>
Governance should also preserve a credible long-term position for FROG as:
</p>

<ul>
  <li>an open language specification rather than a product-bound format,</li>
  <li>a reviewable and inspectable programming foundation,</li>
  <li>a language architecture compatible with AI-era program generation and transformation,</li>
  <li>a language ecosystem compatible with industrial security and technological sovereignty concerns.</li>
</ul>

<hr/>

<h2 id="scope-of-governance">3. Scope of Governance</h2>

<p>
This document applies to the governance of the FROG specification repository, including:
</p>

<ul>
  <li>the language specification,</li>
  <li>source-level program representation,</li>
  <li>execution-related specification layers when published,</li>
  <li>intrinsic standardized primitive-library specifications,</li>
  <li>optional standardized profile specifications,</li>
  <li>IDE architecture specifications,</li>
  <li>repository-level conformance material,</li>
  <li>repository-level strategic framing documents,</li>
  <li>repository-level roadmap documents,</li>
  <li>repository-level versioning-governance documents,</li>
  <li>related architectural and normative documents contained in this repository.</li>
</ul>

<p>
This document does not, by itself, fully define:
</p>

<ul>
  <li>detailed commercial pricing,</li>
  <li>marketplace terms,</li>
  <li>OEM commercial agreements,</li>
  <li>detailed trademark law,</li>
  <li>detailed certification contracts,</li>
  <li>all future certification program procedures.</li>
</ul>

<p>
Those topics MAY be defined more precisely in separate policy documents if and when such documents are published.
</p>

<pre>
Governance scope

This document governs:
- repository stewardship
- editorial direction
- specification evolution
- repository-level architectural authority
- repository-level governance boundaries

This document does not fully govern:
- detailed commercial terms
- fee schedules
- legal trademark procedure
- full certification operations manual
</pre>

<hr/>

<h2 id="repository-layers-and-governance-boundaries">4. Repository Layers and Governance Boundaries</h2>

<p>
The FROG repository contains multiple layers with different roles.
Governance must keep those roles distinct.
</p>

<pre>
Expression/                 -&gt; canonical source and structural validity
Language/                   -&gt; validated program meaning
Libraries/                  -&gt; intrinsic primitive surface
Profiles/                   -&gt; optional standardized capability families
IR/                         -&gt; canonical open execution-facing representation
IDE/                        -&gt; authoring, observability, debugging, inspection

Examples/                   -&gt; named example programs
Conformance/                -&gt; public accept / reject / preserve expectations
Implementations/Reference/  -&gt; non-normative executable workspace

Strategy/                   -&gt; non-normative strategic framing
Roadmap/                    -&gt; non-normative closure sequencing
Versioning/                 -&gt; current corpus-version governance and current-status reporting
</pre>

<p>
These distinctions matter.
In particular:
</p>

<ul>
  <li><strong>Strategy</strong> explains why FROG matters, but does not define language truth.</li>
  <li><strong>Roadmap</strong> explains in what order FROG should be closed, but does not define language truth.</li>
  <li><strong>Versioning</strong> defines current published corpus-version posture and cross-version governance, but does not define language semantics or roadmap ordering.</li>
  <li><strong>Conformance</strong> exposes public expected outcomes, but does not silently replace semantic ownership.</li>
  <li><strong>Implementations/Reference</strong> exercises a bounded executable path, but does not become normative merely by existing.</li>
</ul>

<p>
Governance must therefore preserve not only openness and implementability, but also the internal separation between:
</p>

<ul>
  <li>what is normative,</li>
  <li>what is strategic,</li>
  <li>what is sequencing guidance,</li>
  <li>what is current corpus-version truth,</li>
  <li>what is conformance truth surface,</li>
  <li>what is executable reference proof.</li>
</ul>

<hr/>

<h2 id="repository-wide-framing-and-governance-surfaces">5. Repository-Wide Framing and Governance Surfaces</h2>

<p>
The repository now contains three distinct repository-wide framing and governance surfaces that answer three different questions:
</p>

<table>
  <thead>
    <tr>
      <th>Surface</th>
      <th>Primary question</th>
      <th>What it must not replace</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>Strategy/</code></td>
      <td>Why does FROG matter?</td>
      <td>Normative technical ownership, closure sequencing, or current corpus-version truth</td>
    </tr>
    <tr>
      <td><code>Roadmap/</code></td>
      <td>In what order should FROG be closed?</td>
      <td>Normative technical ownership, strategic rationale, or current corpus-version truth</td>
    </tr>
    <tr>
      <td><code>Versioning/</code></td>
      <td>What is the current published specification corpus version, what doctrine governs version evolution, and what is the current detailed per-surface status?</td>
      <td>Normative technical ownership, strategic rationale, or milestone sequencing</td>
    </tr>
  </tbody>
</table>

<p>
This distinction is governance-relevant because it prevents repository-wide confusion between:
</p>

<ul>
  <li>purpose,</li>
  <li>sequencing,</li>
  <li>current version truth.</li>
</ul>

<p>
Governance should preserve this tri-distinction explicitly.
Repository-wide purpose belongs to <code>Strategy/</code>.
Repository-wide closure order belongs to <code>Roadmap/</code>.
Current published corpus-version truth and current detailed status belong to <code>Versioning/</code>.
</p>

<hr/>

<h2 id="stewardship">6. Stewardship</h2>

<p>
Graiphic is the initial steward of the FROG specification repository.
As steward, Graiphic is responsible for maintaining architectural coherence, reviewing proposed changes, deciding when documents are sufficiently mature for inclusion, and publishing authoritative repository revisions.
</p>

<p>
Stewardship of the repository does not mean that FROG is a vendor-locked product.
The purpose of stewardship is to keep the specification coherent while the language is still being actively defined, cleaned up, and stabilized.
</p>

<p>
The steward SHOULD make decisions in a way that preserves:
</p>

<ul>
  <li>openness of the specification,</li>
  <li>clarity of normative wording,</li>
  <li>implementability by third parties,</li>
  <li>backward compatibility when reasonably possible,</li>
  <li>separation between the standard and any one implementation,</li>
  <li>clear boundaries between intrinsic libraries, profiles, and implementation-specific extensions,</li>
  <li>clear boundaries between specification, strategy, roadmap, versioning, conformance, and reference implementation work.</li>
</ul>

<pre>
Stewardship is not:

- implementation monopoly
- runtime monopoly
- IDE monopoly
- ecosystem exclusivity

Stewardship is:

- repository authority
- editorial authority
- architectural coherence authority
</pre>

<hr/>

<h2 id="open-specification-and-multiple-implementations">7. Open Specification and Multiple Implementations</h2>

<p>
FROG is an open specification.
Any party MAY read, implement, validate, parse, compile, interpret, transform, or otherwise build tooling around the published FROG specification, subject to the repository license and any separately published policies.
</p>

<p>
The FROG specification is intended to support multiple independent implementations, including but not limited to:
</p>

<ul>
  <li>IDEs,</li>
  <li>validators,</li>
  <li>runtimes,</li>
  <li>compilers,</li>
  <li>bridges and interoperability layers,</li>
  <li>hardware integration libraries,</li>
  <li>profile-supporting execution environments,</li>
  <li>testing and conformance tooling.</li>
</ul>

<p>
No implementation is automatically normative merely because it exists.
The repository documents are the normative source of the specification unless a document explicitly states otherwise.
</p>

<p>
An implementation MAY support:
</p>

<ul>
  <li>core FROG only,</li>
  <li>core FROG plus one or more standardized profiles,</li>
  <li>core FROG plus profiles plus additional proprietary or open extensions.</li>
</ul>

<p>
Support for optional profiles is not automatically required for core language support unless a future policy explicitly defines a stricter conformance target.
</p>

<pre>
Open specification does not mean:

one implementation
or
one business model

Open specification means:

many parties may implement
the same published standard
</pre>

<p>
This openness is also one of the reasons FROG can credibly support:
</p>

<ul>
  <li>reviewable and inspectable software pipelines,</li>
  <li>AI-assisted generation and transformation on open artifacts,</li>
  <li>long-term industrial portability,</li>
  <li>technological sovereignty through independent implementation.</li>
</ul>

<p>
Source provenance follows the same governance model. The open specification may define the shape of <code>ide.provenance</code> attestations, but no single IDE is required to be the only possible issuer. Graiphic may maintain proprietary signing infrastructure, certified issuer programs, or product-specific trust policies while the source format remains open to conforming third-party IDEs and organizational trust policies.
</p>

<hr/>

<h2 id="decision-model">8. Decision Model</h2>

<p>
FROG governance currently follows a steward-led model.
Discussion is open, contributions are welcome, and architectural feedback is encouraged, but final editorial and architectural decisions for this repository are made by the steward.
</p>

<p>
In practice:
</p>

<ul>
  <li>contributors MAY propose changes, clarifications, corrections, and new specifications,</li>
  <li>maintainers review proposals for coherence, maturity, and alignment with repository direction,</li>
  <li>the steward decides whether a proposal is accepted, rejected, deferred, split, redirected, or accepted in a narrower form.</li>
</ul>

<p>
The steward SHOULD explain major architectural decisions when such explanation materially helps contributors understand repository direction.
</p>

<p>
Decision-making SHOULD also respect repository layer boundaries.
For example:
</p>

<ul>
  <li>a strategic concern does not automatically justify a semantic change,</li>
  <li>a roadmap desire does not automatically justify normative wording,</li>
  <li>a versioning-governance clarification does not automatically justify semantic change,</li>
  <li>a reference implementation shortcut does not automatically justify a specification change.</li>
</ul>

<pre>
Decision flow

proposal
   ->
review
   ->
architectural evaluation
   ->
accept / revise / split / defer / redirect / reject
</pre>

<hr/>

<h2 id="proposal-and-change-process">9. Proposal and Change Process</h2>

<p>
The normal change path for the specification is:
</p>

<ol>
  <li>identify a problem, ambiguity, inconsistency, or missing specification area,</li>
  <li>discuss the topic through repository issues, pull requests, or equivalent review channels,</li>
  <li>propose concrete wording changes or new documents,</li>
  <li>review the proposal against repository architecture and directly related specifications,</li>
  <li>accept, revise, defer, split, redirect, or reject the change.</li>
</ol>

<p>
Specification changes SHOULD be:
</p>

<ul>
  <li>explicit,</li>
  <li>conservative when modifying existing normative behavior,</li>
  <li>clear about whether they are corrective, additive, or breaking,</li>
  <li>cross-checked against directly related documents,</li>
  <li>clear about whether they affect the core language, an intrinsic library, a profile, an IDE-facing concern, a strategic framing layer, a roadmap layer, or the centralized versioning surface.</li>
</ul>

<p>
Editorial cleanup, factual corrections, and cross-reference alignment MAY be accepted with a lighter process than major semantic changes.
Major semantic changes SHOULD be reviewed more carefully because they may affect multiple specification layers and future implementations.
</p>

<pre>
Change discipline

Small editorial fix
   -> lighter review

Cross-layer semantic change
   -> deeper review
   -> stronger architectural scrutiny
</pre>

<hr/>

<h2 id="compatibility-versioning-and-deprecation">10. Compatibility, Versioning, and Deprecation</h2>

<p>
The FROG specification SHOULD evolve in a way that preserves continuity whenever reasonably possible.
However, early-stage specification work may still require structural correction and architectural refinement.
</p>

<p>
Changes SHOULD be understood under three categories:
</p>

<ul>
  <li><strong>clarification</strong> — wording becomes clearer without changing intended semantics,</li>
  <li><strong>additive change</strong> — new capability is introduced without invalidating existing compliant content,</li>
  <li><strong>breaking change</strong> — previously valid content, assumptions, or interpretations may no longer remain valid.</li>
</ul>

<p>
Breaking changes SHOULD be introduced deliberately and documented clearly.
Where appropriate, the repository SHOULD describe migration direction, replacement constructs, or deprecation rationale.
</p>

<p>
The repository-wide doctrine for current corpus-version governance, additive evolution, degraded readability posture, and current detailed per-surface status belongs centrally in <code>Versioning/</code>.
This governance document recognizes that surface and protects its role, but does not attempt to duplicate its full policy here.
</p>

<pre>
Change classes

clarification -> clearer wording, same intended meaning
additive      -> more capability, existing compliant content remains valid
breaking      -> migration may be required
</pre>

<hr/>

<h2 id="repository-contributions-and-licensing">11. Repository Contributions and Licensing</h2>

<p>
Repository contributions are governed by the repository license, contribution workflow, and contributor license agreement requirements published in the root of the repository.
Contributors MUST follow the contribution process in effect for this repository.
</p>

<p>
Acceptance of a pull request is not automatic.
All contributions remain subject to repository review and steward approval.
</p>

<p>
This governance document does not replace the repository license, contributor license agreement, or contribution policy.
If any conflict appears, the applicable legal and contribution documents govern their own scope.
</p>

<hr/>

<h2 id="commercial-implementations-and-ecosystem-participation">12. Commercial Implementations and Ecosystem Participation</h2>

<p>
Open specification does not imply implementation uniformity.
Open specification also does not prohibit commercial implementations.
</p>

<p>
Any party MAY build open-source or proprietary implementations around the FROG specification, including IDEs, compilers, runtimes, libraries, profiles, integration layers, deployment systems, and commercial services.
Graiphic MAY independently develop proprietary or commercial implementations, enterprise tooling, OEM integrations, plugin ecosystems, marketplaces, hosted services, support offerings, certification programs, and other products around FROG.
</p>

<p>
Such implementations do not redefine the specification merely by existing.
The specification remains governed by the normative repository documents.
</p>

<pre>
Open spec
   does not forbid
commercial implementation

Commercial implementation
   does not redefine
the specification
</pre>

<p>
This coexistence is deliberate.
FROG is intended to support an ecosystem in which:
</p>

<ul>
  <li>the language remains open,</li>
  <li>implementation diversity remains possible,</li>
  <li>commercial participation remains possible,</li>
  <li>official certification and branding remain separately governable.</li>
</ul>

<hr/>

<h2 id="support-claims-conformance-and-certification">13. Support Claims, Conformance, and Certification</h2>

<p>
The governance of FROG SHOULD distinguish clearly between support claims, conformance claims, and certification claims.
Those are related, but they are not the same thing.
</p>

<p>
A future conformance policy SHOULD distinguish at least the following categories:
</p>

<ul>
  <li><strong>implements core FROG</strong> — the implementation claims support for the core language surface only,</li>
  <li><strong>implements core FROG + named profiles</strong> — the implementation claims support for the core language plus one or more explicitly identified standardized profiles,</li>
  <li><strong>conformant implementation</strong> — the implementation satisfies the requirements of the claimed conformance target under an applicable conformance policy,</li>
  <li><strong>certified implementation</strong> — the implementation has been verified and approved under an applicable certification or branding policy.</li>
</ul>

<p>
A claim of profile support SHOULD identify the supported profile explicitly rather than implying universal support for all optional capability families in the repository.
</p>

<p>
Support for a profile MUST NOT, by itself, be treated as support for all profiles.
Likewise, lack of support for a profile MUST NOT, by itself, be treated as failure of core language support unless the claimed target explicitly includes that profile.
</p>

<pre>
Do not collapse these categories

support claim
    != conformance claim
    != certification claim

Example:

"supports core FROG"
    is not the same as
"certified FROG implementation"
</pre>

<p>
These distinctions are especially important because:
</p>

<ul>
  <li>the language must remain open,</li>
  <li>implementation diversity must remain possible,</li>
  <li>public trust in official claims must remain defensible.</li>
</ul>

<hr/>

<h2 id="trademarks-branding-and-official-claims">14. Trademarks, Branding, and Official Claims</h2>

<p>
The openness of the FROG specification does not automatically grant trademark rights.
Names, logos, certification marks, and official compatibility claims are governed by the steward and MAY be subject to separate policies.
</p>

<p>
An implementation MAY implement the FROG specification without being an official Graiphic product.
However, terms such as <em>official</em>, <em>certified</em>, <em>endorsed</em>, <em>FROG-certified</em>, or equivalent claims SHOULD only be used in accordance with applicable trademark, certification, or conformance policies published by the steward.
</p>

<p>
This distinction exists so that:
</p>

<ul>
  <li>the language specification remains open,</li>
  <li>independent implementations remain possible,</li>
  <li>official branding and certification claims remain governable and trustworthy.</li>
</ul>

<pre>
Open specification
    does not automatically grant
official branding rights

Implementing FROG
    does not automatically mean
official / certified / endorsed
</pre>

<p>
This boundary is also important for sovereignty and public trust.
An open language should remain independently implementable without confusing that openness with official steward-controlled marks.
</p>

<hr/>

<h2 id="commercial-vs-non-commercial-certification-direction">15. Commercial vs Non-Commercial Certification Direction</h2>

<p>
The steward MAY define a certification and branding program for implementations that wish to use official FROG certification, official branding, or equivalent steward-controlled marks.
</p>

<p>
The intended direction is:
</p>

<ul>
  <li>commercial implementations that seek official certification, official branding, or equivalent steward-controlled compatibility recognition MAY be subject to paid verification, licensing, or program fees,</li>
  <li>non-commercial implementations MAY be verified and certified free of charge or at minimal cost, subject to steward review and applicable policy,</li>
  <li>the specification itself remains open regardless of whether an implementation participates in any certification or branding program.</li>
</ul>

<p>
This distinction exists to preserve openness of the language specification while allowing the steward to govern the FROG name, branding integrity, and certification responsibility over time.
</p>

<p>
More detailed rules, procedures, fee schedules, eligibility conditions, and branding requirements MAY be defined later in separate policy documents.
</p>

<pre>
Direction of travel

open specification for everyone
        +
optional official certification path
        +
commercial official certification may be paid
        +
non-commercial official certification may be free or low-cost
</pre>

<hr/>

<h2 id="future-governance-evolution">16. Future Governance Evolution</h2>

<p>
The current governance model is intentionally simple and steward-led.
As the ecosystem matures, governance MAY evolve.
</p>

<p>
Possible future evolutions MAY include:
</p>

<ul>
  <li>a documented review board,</li>
  <li>formal change proposals,</li>
  <li>a conformance working group,</li>
  <li>shared editorial maintainership,</li>
  <li>an independent foundation or multi-party governance structure.</li>
</ul>

<p>
Any such change SHOULD preserve the core goals of openness, implementability, architectural coherence, branding clarity, and long-term ecosystem durability.
</p>

<p>
Any future governance evolution SHOULD also preserve the internal repository distinctions between:
</p>

<ul>
  <li>normative ownership,</li>
  <li>strategic framing,</li>
  <li>roadmap sequencing,</li>
  <li>current corpus-version governance,</li>
  <li>conformance truth surface,</li>
  <li>reference implementation proof.</li>
</ul>

<hr/>

<h2 id="non-goals">17. Non-Goals</h2>

<p>
This document does not attempt to:
</p>

<ul>
  <li>freeze the future institutional structure of FROG,</li>
  <li>define detailed trademark law or certification contracts,</li>
  <li>mandate that implementations be open source,</li>
  <li>mandate that implementations be proprietary,</li>
  <li>select a single business model for Graiphic or for the ecosystem,</li>
  <li>force all implementations to support the same optional profiles,</li>
  <li>replace strategy documents with governance,</li>
  <li>replace roadmap sequencing with governance,</li>
  <li>replace versioning governance with governance,</li>
  <li>replace normative specification with governance.</li>
</ul>

<p>
Its purpose is narrower:
to state how the specification repository is governed,
how openness is preserved,
how repository-layer boundaries are protected,
how branding and certification authority are separated from the open language itself,
and how multiple implementations may coexist without collapsing the language into a single vendor product.
</p>

<hr/>

<p align="center">
  <strong>FROG — open specification, durable governance, multiple implementations</strong>
</p>
