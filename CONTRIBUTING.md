<p align="center">
  <img src="FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 Contributing to FROG</h1>

<p align="center">
Contribution guidelines for the FROG specification and public components<br/>
<em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2 id="contribution-rules">Contribution rules at a glance</h2>

<p>Graiphic leads the official project; participation is open and every merge
requires an explicit maintainer decision. Read the
<a href="./GOVERNANCE.md#official-project-rules">seven project rules</a> before proposing a change.</p>

<ul>
  <li>For a bug, provide a small reproduction, expected and observed behavior, and relevant versions.</li>
  <li>For a substantial feature or architecture change, open an issue first to agree on scope.</li>
  <li>Keep a pull request focused. Explain the change, compatibility impact, checks actually run and remaining limits; preserve licenses and attribution.</li>
  <li>For public code, follow the component's build/test instructions and include a targeted regression test when needed. Shared Studio behavior must be qualified before an official component release.</li>
  <li>Review can lead to acceptance, a revision request, deferral or rejection with a concise reason. Passing tests alone does not guarantee a merge.</li>
</ul>

<p>The existing <a href="./CLA.md">CLA</a> and <a href="./LICENSE">license</a> continue to apply.
No automatic acceptance, new contributor agreement or change of license is introduced by these rules.</p>

<h2>Contents</h2>

<ul>
  <li><a href="#project-status">1. Project Status</a></li>
  <li><a href="#how-you-can-contribute">2. How You Can Contribute</a></li>
  <li><a href="#repository-architecture">3. Repository Architecture</a></li>
  <li><a href="#where-to-start">4. Where to Start</a></li>
  <li><a href="#recommended-workflow">5. Recommended Workflow</a></li>
  <li><a href="#before-you-modify-a-document">6. Before You Modify a Document</a></li>
  <li><a href="#contribution-expectations">7. Contribution Expectations</a></li>
  <li><a href="#change-categories">8. Change Categories</a></li>
  <li><a href="#cross-document-coherence">9. Cross-Document Coherence</a></li>
  <li><a href="#pull-request-expectations">10. Pull Request Expectations</a></li>
  <li><a href="#governance">11. Governance</a></li>
  <li><a href="#contributor-license-agreement">12. Contributor License Agreement</a></li>
  <li><a href="#licensing">13. Licensing</a></li>
  <li><a href="#community-and-collaboration">14. Community and Collaboration</a></li>
</ul>

<hr/>

<h2 id="project-status">1. Project Status</h2>

<p>
FROG defines an open graphical dataflow language and its related architectural layers.
This repository also maintains the common Frontend library and CLI, public SDKs and finite reference bridges.
The proprietary Studio editor and production Runtime remain outside this repository.
</p>

<p>
The repository is still in an active design, cleanup, and stabilization phase.
Many documents are already substantial, but some areas are still being clarified, split, aligned, or structurally refactored.
Contributions in the form of ideas, corrections, issue reports, specification proposals, architectural cleanup, and wording improvements are welcome.
</p>

<p>
Specification contributions SHOULD prioritize:
</p>

<ul>
  <li>clarity,</li>
  <li>precision,</li>
  <li>architectural coherence,</li>
  <li>durable wording,</li>
  <li>implementable semantics.</li>
</ul>

<pre>
This repository includes:
- the language specification and architecture contracts
- public implementation components and SDKs
- attributed examples, tests and qualification material

It does not include the complete Studio IDE or proprietary production Runtime.
</pre>

<hr/>

<h2 id="how-you-can-contribute">2. How You Can Contribute</h2>

<p>
There are several useful ways to contribute to FROG:
</p>

<ul>
  <li>improve documentation,</li>
  <li>clarify ambiguous or inconsistent specification wording,</li>
  <li>propose architectural refinements,</li>
  <li>propose new specifications or missing specification areas,</li>
  <li>identify duplication or ownership conflicts between documents,</li>
  <li>open issues to discuss design questions,</li>
  <li>submit pull requests,</li>
  <li>participate in repository-level architectural discussions.</li>
</ul>

<p>
Early contributions are especially valuable when they improve:
</p>

<ul>
  <li>cross-document consistency,</li>
  <li>terminology alignment,</li>
  <li>ownership clarity between specification layers,</li>
  <li>normative wording quality,</li>
  <li>long-term navigability of the repository.</li>
</ul>

<p>
Useful contributions are not limited to adding new text.
A contribution may also be valuable when it:
</p>

<ul>
  <li>removes ambiguity,</li>
  <li>makes an ownership boundary explicit,</li>
  <li>turns a duplicated document into a redirect or transition stub,</li>
  <li>realigns a document with the actual repository architecture,</li>
  <li>identifies follow-up documents that also need adjustment.</li>
</ul>

<hr/>

<h2 id="repository-architecture">3. Repository Architecture</h2>

<p>
FROG is organized by architectural responsibility.
Contributors SHOULD understand which layer owns which topic before proposing a change.
</p>

<pre><code>FROG/
│
├── Expression/   canonical source specification
├── Language/     normative execution semantics
├── Libraries/    intrinsic standard primitive-library specifications
├── Profiles/     optional standardized capability-family specifications
├── IDE/          authoring, observability, debugging, probes, watch, snippets
│
├── GOVERNANCE.md
├── CLA.md
├── CONTRIBUTING.md
├── LICENSE
└── Readme.md</code></pre>

<p>
Layer ownership in summary:
</p>

<ul>
  <li><strong><code>Expression/</code></strong> — canonical source representation and source-visible program objects,</li>
  <li><strong><code>Language/</code></strong> — normative execution semantics and cross-cutting semantic rules,</li>
  <li><strong><code>Libraries/</code></strong> — intrinsic standardized primitive identities, ports, required metadata, and primitive-level definitions,</li>
  <li><strong><code>Profiles/</code></strong> — optional standardized capability families that extend the usable surface of FROG without becoming part of the intrinsic core,</li>
  <li><strong><code>IDE/</code></strong> — authoring architecture, observability, debugging, probes, watch, and snippet workflows.</li>
</ul>

<p>
If a contribution crosses multiple layers, the contributor SHOULD identify which document is the primary owner of the topic.
If the current repository state is still transitional, the contributor SHOULD distinguish clearly between:
</p>

<ul>
  <li>the current published repository state,</li>
  <li>the proposed architectural direction.</li>
</ul>

<pre>
Ownership rule of thumb

How is it written in source?         -> Expression/
What does it mean when it executes?  -> Language/
What does this intrinsic primitive do? -> Libraries/
What does this optional capability do? -> Profiles/
How does the IDE expose or author it?  -> IDE/
</pre>

<hr/>

<h2 id="where-to-start">4. Where to Start</h2>

<p>
Before contributing, you SHOULD begin by reading:
</p>

<ul>
  <li><code>Readme.md</code> at the repository root,</li>
  <li>the <code>Readme.md</code> of the relevant top-level directory,</li>
  <li>the exact file you intend to modify,</li>
  <li>directly related specification files in the same semantic area,</li>
  <li>existing issues and discussions when relevant.</li>
</ul>

<p>
Typical entry points:
</p>

<ul>
  <li><code>Expression/</code> — canonical source specification</li>
  <li><code>Language/</code> — normative execution semantics</li>
  <li><code>Libraries/</code> — intrinsic standard primitive-library specifications</li>
  <li><code>Profiles/</code> — optional standardized capability-family specifications</li>
  <li><code>IDE/</code> — IDE architecture, authoring workflows, observability, debugging, and snippets</li>
</ul>

<p>
Contributors SHOULD avoid modifying a document in isolation when its topic is semantically connected to neighboring documents.
</p>

<pre>
Minimum reading path before a meaningful change

root Readme.md
      ->
relevant layer Readme.md
      ->
target file
      ->
directly related files
</pre>

<hr/>

<h2 id="recommended-workflow">5. Recommended Workflow</h2>

<p>
The preferred contribution workflow follows the standard GitHub model using forks and pull requests.
</p>

<pre><code>Contributor
     │
     │ Fork
     ▼
Your Fork Repository
     │
     │ Create branch
     │ Commit changes
     ▼
Pull Request
     │
     │ Review by maintainers
     ▼
Graiphic / FROG main repository</code></pre>

<p>
This workflow allows maintainers to review contributions while preserving repository-wide architectural coherence.
</p>

<p>
Recommended step-by-step process:
</p>

<ol>
  <li>Fork the repository.</li>
  <li>Create a branch for your change.</li>
  <li>Re-read the current version of the target document and its directly related documents.</li>
  <li>Identify whether the target document is the primary owner of the topic.</li>
  <li>Identify whether the topic belongs to the intrinsic core, to an optional profile, or to an implementation-facing concern.</li>
  <li>Commit your modifications.</li>
  <li>Open a Pull Request.</li>
  <li>Explain any cross-document conflict, ownership issue, or follow-up cleanup that remains relevant.</li>
  <li>Discuss the proposal with maintainers if clarification is needed.</li>
</ol>

<p>
For larger semantic or architectural changes, opening an issue first is RECOMMENDED.
</p>

<pre>
Recommended large-change flow

issue / discussion
      ->
scope clarification
      ->
document proposal
      ->
pull request
</pre>

<hr/>

<h2 id="before-you-modify-a-document">6. Before You Modify a Document</h2>

<p>
For both specifications and public code, contributors MUST work from the current repository state rather than from memory, assumption, or older draft wording.
</p>

<p>
Before modifying a document, you SHOULD:
</p>

<ul>
  <li>start from the current version of the target file,</li>
  <li>read the repository root <code>Readme.md</code>,</li>
  <li>read the relevant top-level <code>Readme.md</code>,</li>
  <li>read directly related files in the same semantic area,</li>
  <li>identify whether the target document is the primary owner of the topic,</li>
  <li>identify any cross-document conflict before rewriting,</li>
  <li>preserve the distinction between current published state and proposed architectural direction when that distinction matters.</li>
</ul>

<p>
Contributors SHOULD NOT:
</p>

<ul>
  <li>rewrite a document from scratch without taking the current content into account,</li>
  <li>invent repository structure, file names, or ownership assumptions that are not present in the repository,</li>
  <li>move semantic ownership casually from one layer to another without clearly explaining the architectural reason,</li>
  <li>treat an optional profile capability as if it were automatically part of the intrinsic core,</li>
  <li>remove useful existing content merely because a shorter formulation is possible.</li>
</ul>

<pre>
Do not work like this

memory
   ->
assumption
   ->
isolated rewrite

Work like this instead

current repo state
   ->
related documents
   ->
ownership check
   ->
proposed change
</pre>

<hr/>

<h2 id="contribution-expectations">7. Contribution Expectations</h2>

<p>
FROG documents are specification documents.
Contributions SHOULD therefore aim for wording that is:
</p>

<ul>
  <li>explicit,</li>
  <li>stable over time,</li>
  <li>architecturally coherent,</li>
  <li>implementable,</li>
  <li>clear about ownership and scope.</li>
</ul>

<p>
Contributors SHOULD:
</p>

<ul>
  <li>preserve useful existing content when possible,</li>
  <li>prefer conservative rewrites when editing an existing specification,</li>
  <li>improve the document rather than merely shorten it,</li>
  <li>distinguish clearly between current published state and proposed architectural direction when that distinction matters,</li>
  <li>use explicit normative wording when the document is intended to be normative.</li>
</ul>

<p>
Contributors SHOULD call out:
</p>

<ul>
  <li>ownership conflicts,</li>
  <li>duplication between documents,</li>
  <li>ambiguous terminology,</li>
  <li>under-specified behavior,</li>
  <li>outdated references or obsolete repository assumptions.</li>
</ul>

<p>
Where it materially improves clarity, contributors SHOULD add:
</p>

<ul>
  <li>small ASCII diagrams,</li>
  <li>local tree views,</li>
  <li>compact ownership summaries,</li>
  <li>clear transition notes for legacy documents.</li>
</ul>

<pre>
Good specification contributions improve:

- wording
- ownership clarity
- cross-document coherence
- future implementability
- repository readability
</pre>

<hr/>

<h2 id="change-categories">8. Change Categories</h2>

<p>
When proposing a change, contributors SHOULD make clear which category the change belongs to:
</p>

<ul>
  <li><strong>clarification</strong> — wording becomes clearer without changing intended semantics,</li>
  <li><strong>editorial cleanup</strong> — formatting, cross-references, navigation, or readability improve without changing intended meaning,</li>
  <li><strong>additive change</strong> — new capability or new specification area is added without invalidating current compliant content,</li>
  <li><strong>architectural refactor</strong> — ownership, layering, or document boundaries are reorganized for coherence,</li>
  <li><strong>breaking change</strong> — previously valid content, assumptions, or interpretations may no longer remain valid.</li>
</ul>

<p>
Major semantic or architectural changes SHOULD explain:
</p>

<ul>
  <li>what problem they solve,</li>
  <li>which neighboring documents are affected,</li>
  <li>whether the change affects the core language, an intrinsic library, an optional profile, or only an IDE-facing concern,</li>
  <li>whether migration or follow-up cleanup is required.</li>
</ul>

<pre>
Change classification matters

small wording fix
   !=
cross-layer semantic refactor
</pre>

<hr/>

<h2 id="cross-document-coherence">9. Cross-Document Coherence</h2>

<p>
A contribution is stronger when it respects repository-wide coherence rather than improving one file locally while creating inconsistency elsewhere.
</p>

<p>
Contributors SHOULD therefore cross-check:
</p>

<ul>
  <li>top-level ownership boundaries,</li>
  <li>terminology consistency,</li>
  <li>duplicate topic coverage,</li>
  <li>references between layers,</li>
  <li>examples that may have become stale after an architectural change.</li>
</ul>

<p>
Contributors SHOULD be especially careful when a topic could be mistaken for belonging to the wrong layer.
For example:
</p>

<ul>
  <li>source representation belongs to <code>Expression/</code>,</li>
  <li>cross-cutting execution meaning belongs to <code>Language/</code>,</li>
  <li>intrinsic primitive vocabularies belong to <code>Libraries/</code>,</li>
  <li>optional standardized environment-dependent capability families belong to <code>Profiles/</code>,</li>
  <li>authoring, observability, debugging, and inspection belong to <code>IDE/</code>.</li>
</ul>

<p>
If a contributor identifies a conflict but only proposes a local change, the Pull Request SHOULD still explain the broader conflict.
</p>

<pre>
Local improvement is good.
Local improvement that creates a neighboring contradiction is not.

Always ask:
- what else does this change touch?
- which document owns this topic?
- which document may now be stale?
</pre>

<hr/>

<h2 id="pull-request-expectations">10. Pull Request Expectations</h2>

<p>
A strong Pull Request SHOULD make review easier rather than forcing maintainers to reconstruct the architectural impact from the diff alone.
</p>

<p>
A Pull Request description SHOULD therefore identify:
</p>

<ul>
  <li>the primary target document,</li>
  <li>the type of change,</li>
  <li>the ownership boundary involved,</li>
  <li>the directly related documents that were checked,</li>
  <li>any known follow-up cleanup that remains outside the current PR.</li>
</ul>

<p>
For semantic or architectural changes, the Pull Request SHOULD also explain:
</p>

<ul>
  <li>the problem being solved,</li>
  <li>why the chosen document is the correct owner,</li>
  <li>whether the change is clarifying existing intent or changing intended behavior,</li>
  <li>whether any migration or transition note is needed.</li>
</ul>

<pre>
Recommended PR summary shape

- change type
- primary owner document
- directly related documents reviewed
- main architectural rationale
- follow-up documents if any
</pre>

<hr/>

<h2 id="governance">11. Governance</h2>

<p>
FROG is an open specification project with repository-level governance.
Contributors SHOULD remain consistent with the governance model and the current architectural direction of the specification.
</p>

<p>
See:
</p>

<p>
<code>GOVERNANCE.md</code>
</p>

<hr/>

<h2 id="contributor-license-agreement">12. Contributor License Agreement</h2>

<p>
By submitting a contribution, you agree to the Contributor License Agreement.
</p>

<p>
See:
</p>

<p>
<code>CLA.md</code>
</p>

<p>
If CLA signing is required for your Pull Request, the repository workflow may prompt you through the configured CLA process.
</p>

<hr/>

<h2 id="licensing">13. Licensing</h2>

<p>
Repository content is governed by the repository license.
Contributors are responsible for ensuring that they have the legal right to submit their work under the licensing terms used by this repository.
</p>

<p>
See:
</p>

<p>
<code>LICENSE</code>
</p>

<hr/>

<h2 id="community-and-collaboration">14. Community and Collaboration</h2>

<p>
FROG aims to build an open ecosystem around graphical dataflow programming.
Constructive discussions, careful specification proposals, editorial cleanup, and serious architectural reasoning are encouraged.
</p>

<p>
The goal is not only to add more content.
The goal is to build a coherent, durable, and implementable specification that can support multiple independent implementations over time.
</p>

<pre>
Contribution spirit

Be explicit.
Be architectural.
Be conservative with ownership.
Be precise with wording.
Be helpful across document boundaries.
</pre>
