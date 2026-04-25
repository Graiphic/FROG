<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Specification Versioning</h1>

<p align="center">
  <strong>Centralized cross-version governance for the published FROG specification corpus</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#why-this-surface-exists">2. Why This Surface Exists</a></li>
  <li><a href="#scope">3. Scope</a></li>
  <li><a href="#non-goals">4. Non-Goals</a></li>
  <li><a href="#current-published-version">5. Current Published Version</a></li>
  <li><a href="#three-version-notions-that-must-remain-distinct">6. Three Version Notions That Must Remain Distinct</a></li>
  <li><a href="#core-versioning-doctrine">7. Core Versioning Doctrine</a></li>
  <li><a href="#cumulative-version-model">8. Cumulative Version Model</a></li>
  <li><a href="#specification-corpus-version">9. Specification Corpus Version</a></li>
  <li><a href="#frog-spec_version-source-compatibility-version">10. <code>.frog</code> <code>spec_version</code> Source Compatibility Version</a></li>
  <li><a href="#metadata-program_version-program-artifact-version">11. <code>metadata.program_version</code> Program Artifact Version</a></li>
  <li><a href="#governance-principles">12. Governance Principles</a></li>
  <li><a href="#additive-evolution-rule">13. Additive Evolution Rule</a></li>
  <li><a href="#degraded-reading-and-preservation-model">14. Degraded Reading and Preservation Model</a></li>
  <li><a href="#compatibility-levels">15. Compatibility Levels</a></li>
  <li><a href="#safe-behavior-rule">16. Safe Behavior Rule</a></li>
  <li><a href="#format-design-obligations">17. Format Design Obligations</a></li>
  <li><a href="#version-boundary-model">18. Version Boundary Model</a></li>
  <li><a href="#stabilization-classes">19. Stabilization Classes</a></li>
  <li><a href="#cross-repository-versioning-responsibilities">20. Cross-Repository Versioning Responsibilities</a></li>
  <li><a href="#compatibility-posture">21. Compatibility Posture</a></li>
  <li><a href="#version-transition-criteria">22. Version Transition Criteria</a></li>
  <li><a href="#published-version-objectives">23. Published Version Objectives</a></li>
  <li><a href="#central-version-matrix">24. Central Version Matrix</a></li>
  <li><a href="#how-other-documents-should-reference-this-surface">25. How Other Documents Should Reference This Surface</a></li>
  <li><a href="#change-classification-guidance">26. Change Classification Guidance</a></li>
  <li><a href="#minimal-decision-table">27. Minimal Decision Table</a></li>
  <li><a href="#relationship-with-readme-examples-conformance-and-reference-implementation">28. Relationship with Readme, Examples, Conformance, and Reference Implementation</a></li>
  <li><a href="#repository-wide-versioning-diagram">29. Repository-Wide Versioning Diagram</a></li>
  <li><a href="#future-expansion">30. Future Expansion</a></li>
  <li><a href="#summary">31. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>
<p>
This document is the centralized repository-visible governance surface for specification versioning across the published FROG specification corpus.
</p>

<p>
Its purpose is to define how specification versions are identified, how version-to-version closure is evaluated, what becomes normative at each published boundary, what remains draft or deferred, and how the repository should describe transition from one published specification state to the next.
</p>

<p>
This document also defines the core FROG versioning doctrine:
specification evolution should be additive by default,
future source artifacts should remain openable as broadly as possible,
unknown or unsupported sections should be preserved rather than destroyed where safe,
and semantic misunderstanding must be treated as worse than explicit limitation or refusal.
</p>

<hr/>

<h2 id="why-this-surface-exists">2. Why This Surface Exists</h2>
<p>
The published FROG repository already spans multiple architectural layers and repository-level support surfaces:
</p>

<ul>
  <li>canonical source representation,</li>
  <li>structural validation posture,</li>
  <li>validated language meaning,</li>
  <li>execution-facing IR,</li>
  <li>canonical JSON IR posture where applicable,</li>
  <li>lowering,</li>
  <li>backend contract,</li>
  <li>backend-family consumption,</li>
  <li>bounded execution-oriented corridors,</li>
  <li>widget and front-panel object exposure,</li>
  <li>examples,</li>
  <li>conformance material,</li>
  <li>reference implementation workspace,</li>
  <li>strategic framing,</li>
  <li>roadmap sequencing.</li>
</ul>

<p>
Without a centralized specification-versioning surface, version intent would tend to leak into many local documents and become difficult to maintain consistently.
</p>

<p>
This document therefore serves as the single published reference point for:
</p>

<ul>
  <li>cross-version scope,</li>
  <li>stabilization posture,</li>
  <li>transition logic,</li>
  <li>additive-evolution doctrine,</li>
  <li>cumulative version-model policy,</li>
  <li>degraded-but-safe cross-version readability posture.</li>
</ul>

<hr/>

<h2 id="scope">3. Scope</h2>
<p>
This document governs:
</p>

<ul>
  <li>published specification corpus versions,</li>
  <li>what each specification version is intended to stabilize,</li>
  <li>what remains out of scope or explicitly deferred,</li>
  <li>how version transitions should be evaluated,</li>
  <li>how other repository documents should refer to specification-version intent,</li>
  <li>the repository-wide doctrine for additive evolution and safe cross-version readability.</li>
</ul>

<p>
This document does not replace normative ownership of the specification itself.
Normative technical law remains owned by the relevant architectural layers such as <code>Expression/</code>, <code>Language/</code>, <code>IR/</code>, <code>Libraries/</code>, <code>Profiles/</code>, and <code>IDE/</code>.
</p>

<hr/>

<h2 id="non-goals">4. Non-Goals</h2>
<p>
This document does not:
</p>

<ul>
  <li>define one product release train,</li>
  <li>define vendor-specific shipping schedules,</li>
  <li>replace roadmap milestone tracking,</li>
  <li>replace strategic justification,</li>
  <li>replace conformance expectations,</li>
  <li>define one required implementation,</li>
  <li>act as a hidden source of technical semantics,</li>
  <li>guarantee that every older tool can fully edit every future artifact.</li>
</ul>

<p>
Versioning governance is not a substitute for technical specification.
It is the repository-level policy that organizes stabilization across the already-separated technical ownership layers.
</p>

<hr/>

<h2 id="current-published-version">5. Current Published Version</h2>
<p>
This section is the canonical repository-visible place where the current published specification corpus version is declared.
</p>

<table>
  <thead>
    <tr>
      <th>Field</th>
      <th>Current value</th>
      <th>Meaning</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Specification corpus version</td>
      <td><code>0.1-draft</code></td>
      <td>Current published repository-level specification posture under active consolidation</td>
    </tr>
    <tr>
      <td>Status</td>
      <td>Bounded published closure under consolidation</td>
      <td>The repository already exposes a real multi-layer published corridor, but not all surfaces are yet equally stabilized</td>
    </tr>
    <tr>
      <td>Primary closure target</td>
      <td>First coherent end-to-end bounded corpus version</td>
      <td>Canonical source, validated meaning, execution-facing IR, lowering, backend handoff, bounded executable corridor, and bounded widget-object closure</td>
    </tr>
    <tr>
      <td>Reference source-format target</td>
      <td><code>.frog spec_version = 0.1</code></td>
      <td>Source compatibility target for the currently bounded published subset</td>
    </tr>
  </tbody>
</table>

<p>
The values above identify the current repository-wide publication posture.
More detailed per-surface status is centralized in <code>Versioning/Matrix.md</code>.
</p>

<p>
The distinction between <code>0.1-draft</code> and <code>.frog spec_version = 0.1</code> is intentional:
</p>

<ul>
  <li><code>0.1-draft</code> describes the current repository-wide publication posture of the specification corpus,</li>
  <li><code>spec_version = 0.1</code> describes the source-format compatibility target for canonical <code>.frog</code> artifacts within the currently bounded published subset.</li>
</ul>

<p>
The first is repository-governance state.
The second is source-artifact compatibility declaration.
They must remain explicit and non-collapsed.
</p>

<hr/>

<h2 id="three-version-notions-that-must-remain-distinct">6. Three Version Notions That Must Remain Distinct</h2>
<p>
FROG requires three different notions of version to remain explicit and non-collapsed:
</p>

<table>
  <thead>
    <tr>
      <th>Version notion</th>
      <th>What it versions</th>
      <th>Where it belongs</th>
      <th>What it must not be confused with</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Specification corpus version</td>
      <td>The published FROG specification corpus as a whole</td>
      <td>This document and repository-wide version governance</td>
      <td>Source-format compatibility or one program's artifact version</td>
    </tr>
    <tr>
      <td><code>.frog</code> <code>spec_version</code></td>
      <td>The source-format / compatibility target declared by a FROG source file</td>
      <td>Canonical source payload</td>
      <td>The repository's publication state or one program release number</td>
    </tr>
    <tr>
      <td><code>metadata.program_version</code></td>
      <td>One particular program artifact revision chosen by the author</td>
      <td>Program metadata</td>
      <td>Specification version or source-format compatibility law</td>
    </tr>
  </tbody>
</table>

<p>
These three notions are related but not interchangeable.
</p>

<hr/>

<h2 id="core-versioning-doctrine">7. Core Versioning Doctrine</h2>
<p>
FROG specification evolution should be additive by default.
</p>

<p>
Newer specification versions should extend earlier source representations rather than invalidate them without necessity.
A conforming tool should, where possible, be able to open source artifacts targeting newer specification revisions in a degraded but explicit manner, preserving unknown or unsupported sections without silently reinterpreting, discarding, or corrupting them.
</p>

<p>
FROG therefore prefers the following posture:
</p>

<ul>
  <li>open if possible,</li>
  <li>inspect what is understood,</li>
  <li>preserve what is unknown when safe,</li>
  <li>refuse semantic claims or destructive editing when unsafe,</li>
  <li>never silently misinterpret a newer construct as if it were fully understood.</li>
</ul>

<p>
This doctrine is stronger than ordinary backward readability and narrower than a promise of universal full reversibility.
It does not require every older tool to fully understand every future FROG artifact.
It requires the specification and conforming tools to distinguish partial readability from full semantic acceptance, full editability, and executable acceptance.
</p>

<hr/>

<h2 id="cumulative-version-model">8. Cumulative Version Model</h2>
<p>
FROG follows a cumulative version model for specification evolution.
</p>

<p>
In that model, each new specification version should extend the previous one by default rather than replace it.
Older valid source forms should remain valid in later specification versions unless an explicitly documented breaking boundary is declared.
</p>

<p>
The intended reading posture is therefore close in spirit to cumulative language-standard evolution:
</p>

<ul>
  <li>a later version includes the earlier valid language surface,</li>
  <li>later versions add new bounded capabilities, forms, or distinctions,</li>
  <li>earlier authored content should continue to remain meaningful within later specification scope unless a breaking boundary is explicitly declared.</li>
</ul>

<p>
This model does not mean that all tools must support all future constructs equally.
It means that specification evolution should normally behave as:
</p>

<pre><code>later version
    =
earlier version
    +
explicit bounded additions
</code></pre>

<p>
It also means that version evolution should not behave as:
</p>

<pre><code>later version
    =
silent redefinition of the earlier language
</code></pre>

<p>
In practical terms:
</p>

<ul>
  <li>a later corpus version should normally preserve earlier valid forms,</li>
  <li>a later <code>.frog spec_version</code> should normally extend prior accepted source-compatible forms,</li>
  <li>older-capability tools may still need to degrade explicitly when they encounter newer bounded additions,</li>
  <li>unsupported meaning must not be silently guessed or falsely claimed.</li>
</ul>

<p>
This cumulative model is therefore paired with the degraded-reading and preservation model defined later in this document.
Cumulative specification evolution preserves older valid forms.
Preservation-aware tooling behavior governs how older-capability tools handle newer artifacts they do not fully understand.
</p>

<hr/>

<h2 id="specification-corpus-version">9. Specification Corpus Version</h2>
<p>
The specification corpus version is the version of the published FROG specification repository considered as a public specification corpus.
</p>

<p>
It answers questions such as:
</p>

<ul>
  <li>Which architectural surfaces are already considered published and stabilized enough to be referenced together?</li>
  <li>Which closures are required before the next corpus version can be declared?</li>
  <li>Which areas remain draft, provisional, partial, or deferred?</li>
  <li>Which public distinctions are now considered repository-wide policy?</li>
</ul>

<p>
The specification corpus version is a repository-governance notion.
It is not embedded as the version of an individual FROG program artifact.
</p>

<p>
In the current repository posture, the corpus version should be read together with the central matrix:
the version label alone identifies the declared overall publication state,
while <code>Versioning/Matrix.md</code> explains how that state is distributed across the repository surfaces.
</p>

<hr/>

<h2 id="frog-spec_version-source-compatibility-version">10. <code>.frog</code> <code>spec_version</code> Source Compatibility Version</h2>
<p>
The <code>spec_version</code> field in a <code>.frog</code> file identifies the source-format / compatibility target expected by that source artifact.
</p>

<p>
It answers questions such as:
</p>

<ul>
  <li>What canonical source shape is expected?</li>
  <li>Which source-level constructs are admitted?</li>
  <li>Which structural validation rules apply?</li>
  <li>Which source-visible distinctions are required for that file to be accepted as canonical source?</li>
</ul>

<p>
A repository may publish multiple documents that discuss future source-model growth, while a concrete <code>.frog</code> file still declares one source-compatibility target through <code>spec_version</code>.
</p>

<p>
Therefore:
</p>

<ul>
  <li>the repository can evolve beyond one source version,</li>
  <li>a source file can target a specific compatibility boundary,</li>
  <li>the published corpus version and the source file's <code>spec_version</code> remain distinct.</li>
</ul>

<p>
The additive-evolution doctrine means that new <code>spec_version</code> values should, by default, extend prior accepted forms rather than redefine them incompatibly unless a carefully justified boundary change is explicitly declared.
</p>

<p>
Under the cumulative version model, a later <code>spec_version</code> should normally be readable as a later bounded extension of earlier valid source-compatible forms rather than as a replacement language with silent incompatibility.
</p>

<hr/>

<h2 id="metadata-program_version-program-artifact-version">11. <code>metadata.program_version</code> Program Artifact Version</h2>
<p>
The <code>metadata.program_version</code> field versions one particular FROG program artifact as chosen by its author, maintainer, or governing process.
</p>

<p>
It answers questions such as:
</p>

<ul>
  <li>Is this program artifact revision 1.2.0 or 2.0.0?</li>
  <li>Has this authored graph changed from one release of the program to another?</li>
  <li>How should downstream tooling or users distinguish one application-level revision from another?</li>
</ul>

<p>
It does not define specification law, source compatibility law, or repository publication state.
</p>

<hr/>

<h2 id="governance-principles">12. Governance Principles</h2>
<p>
The following principles apply to specification-version governance across the published repository:
</p>

<ol>
  <li><strong>Centralization:</strong> version-transition policy should be maintained here rather than duplicated across many files.</li>
  <li><strong>Explicitness:</strong> what is stabilized, draft, deferred, or out of scope must be stated explicitly.</li>
  <li><strong>Layer preservation:</strong> versioning must not collapse architectural ownership boundaries.</li>
  <li><strong>Non-substitution:</strong> roadmap, strategy, examples, conformance, and reference implementation do not replace specification version governance.</li>
  <li><strong>Repository coherence:</strong> a version claim should refer to a coherent repository-visible state, not one isolated local rewrite.</li>
  <li><strong>Bounded closure before expansion:</strong> a version should close a coherent corridor before broadening into new unfinished fronts.</li>
  <li><strong>No hidden semantic law:</strong> runtime-private implementation convenience must not silently define version semantics.</li>
  <li><strong>Additive by default:</strong> future versions should extend earlier accepted source structures rather than invalidate them without explicit and justified need.</li>
  <li><strong>Cumulative by default:</strong> a later version should normally include earlier valid forms as part of its language surface unless an explicit breaking boundary is declared.</li>
  <li><strong>Preserve when unknown:</strong> unsupported but safely preservable content should not be silently destroyed.</li>
  <li><strong>Refuse when unsafe:</strong> explicit limitation or refusal is preferable to false semantic confidence.</li>
</ol>

<hr/>

<h2 id="additive-evolution-rule">13. Additive Evolution Rule</h2>
<p>
FROG version evolution should be additive by default at the source-representation level and, where possible, at the repository-visible corridor level.
</p>

<p>
This means that a newer version should normally introduce:
</p>

<ul>
  <li>new optional sections,</li>
  <li>new optional fields,</li>
  <li>new bounded capability families,</li>
  <li>new bounded profile closures,</li>
  <li>new bounded widget-object surfaces,</li>
  <li>new bounded executable corridor segments,</li>
  <li>new bounded conformance families.</li>
</ul>

<p>
Source provenance is an example of an additive source-format extension when it is carried under the optional <code>ide.provenance</code> object and remains safely ignorable for execution semantics. Older tools may lack provenance verification, but they SHOULD preserve unknown <code>ide</code> fields when safe and MUST NOT reinterpret missing provenance as AI-generated source.
</p>

<p>
This rule does not mean that any addition is automatically safe for every older tool.
Additivity must be evaluated not only syntactically, but also with respect to degraded semantic readability.
</p>

<p>
An additive change is acceptable by default only when an older tool that does not understand the addition can still behave safely and explicitly.
If a nominally additive construct would cause an older tool to silently misunderstand program meaning, the change must be treated as requiring stricter gating, explicit refusal behavior, or a more carefully delimited compatibility boundary.
</p>

<p>
The additive rule and the cumulative version model reinforce each other:
</p>

<ul>
  <li>the additive rule governs how new capability enters the specification,</li>
  <li>the cumulative model governs how later versions should be interpreted with respect to earlier valid forms.</li>
</ul>

<hr/>

<h2 id="degraded-reading-and-preservation-model">14. Degraded Reading and Preservation Model</h2>
<p>
FROG should support degraded reading of newer artifacts by older-capability tools whenever that degraded reading can remain explicit and safe.
</p>

<p>
The target behavior is not:
</p>

<ul>
  <li>universal full understanding,</li>
  <li>universal full editability,</li>
  <li>universal executable acceptance,</li>
  <li>lossy downgrade disguised as success.</li>
</ul>

<p>
The target behavior is instead:
</p>

<ul>
  <li>open the artifact if structural access is still possible,</li>
  <li>display the subset that is understood,</li>
  <li>identify unsupported sections or capabilities explicitly,</li>
  <li>preserve unknown sections when safe and technically possible,</li>
  <li>restrict editing, validation, derivation, lowering, or execution claims when unsupported features affect those stages.</li>
</ul>

<p>
This doctrine is especially important for long-lived source durability, cross-IDE openness, repository-visible auditability, and avoidance of version-lock behavior.
</p>

<hr/>

<h2 id="compatibility-levels">15. Compatibility Levels</h2>
<p>
When a tool encounters a FROG artifact from a newer capability set or source revision, the specification should distinguish at least the following compatibility levels:
</p>

<table>
  <thead>
    <tr>
      <th>Level</th>
      <th>Meaning</th>
      <th>What it does not imply</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Openable</td>
      <td>The artifact can be opened and parsed sufficiently to expose its known structure</td>
      <td>Semantic acceptance, full editability, derivability, or executability</td>
    </tr>
    <tr>
      <td>Inspectable</td>
      <td>The understood subset can be presented coherently to the user</td>
      <td>That unknown parts are absent or harmless for all operations</td>
    </tr>
    <tr>
      <td>Preservable</td>
      <td>Unknown or unsupported sections can be retained without destructive loss during safe operations</td>
      <td>That the tool fully understands or may freely rewrite those sections</td>
    </tr>
    <tr>
      <td>Fully editable</td>
      <td>The tool understands the relevant artifact semantics well enough to safely edit the whole targeted scope</td>
      <td>That all future constructs are supported</td>
    </tr>
    <tr>
      <td>Semantically acceptable</td>
      <td>The tool may claim validated meaning for the relevant scope</td>
      <td>That all downstream profiles or runtimes are supported</td>
    </tr>
    <tr>
      <td>Executably acceptable</td>
      <td>The tool may claim bounded derivation / lowering / execution acceptance for the relevant scope</td>
      <td>That unsupported features may be ignored without consequence</td>
    </tr>
  </tbody>
</table>

<p>
A conforming tool must not collapse these levels into one undifferentiated “opened successfully” outcome.
</p>

<hr/>

<h2 id="safe-behavior-rule">16. Safe Behavior Rule</h2>
<p>
When a tool encounters a construct, capability, section, or semantic dependency that it does not support, it must follow the safe behavior rule:
</p>

<pre><code>open if possible
inspect what is known
preserve what is unknown when safe
refuse unsafe semantic or executable claims
never silently misinterpret
</code></pre>

<p>
In particular:
</p>

<ul>
  <li>an older tool should not claim full semantic understanding when newer constructs materially affect meaning,</li>
  <li>an older tool should not silently drop unknown data on save when preservation is expected and technically possible,</li>
  <li>an older tool should not present partial editability as if it were full compatibility,</li>
  <li>an older tool should not continue to executable acceptance when unsupported features materially affect derivation, lowering, backend contract, execution-start assumptions, or runtime-observable behavior.</li>
</ul>

<hr/>

<h2 id="format-design-obligations">17. Format Design Obligations</h2>
<p>
The additive and preservable versioning doctrine imposes concrete obligations on the FROG source model and surrounding specification surfaces.
</p>

<h3>17.1 Structured extensibility</h3>
<p>
New capabilities should be introduced through clearly delimited extensibility points rather than by silently changing the meaning of existing fields.
</p>

<h3>17.2 Unknown-field tolerance where intended</h3>
<p>
Where a source area is explicitly extensible, conforming readers should tolerate unknown fields or substructures according to the rules of that area.
</p>

<h3>17.3 Preservation without understanding</h3>
<p>
Where technically feasible and safe, tools should preserve unsupported sections without needing to reinterpret them.
</p>

<h3>17.4 Capability-sensitive acceptance</h3>
<p>
A simple source version number is not always sufficient to determine safe operation.
The repository may therefore define bounded capability families, feature gates, or profile-specific support requirements where necessary.
</p>

<h3>17.5 Explicit criticality</h3>
<p>
Additions should be designed so that tools can distinguish at least:
</p>

<ul>
  <li>decorative or editorial enrichment,</li>
  <li>display-relevant enrichment,</li>
  <li>semantic enrichment,</li>
  <li>derivation-relevant enrichment,</li>
  <li>execution-relevant enrichment.</li>
</ul>

<p>
An older tool may be allowed to ignore decorative or non-authoritative enrichment.
It must not ignore semantic or execution-relevant additions while pretending full acceptance.
</p>

<hr/>

<h2 id="version-boundary-model">18. Version Boundary Model</h2>
<p>
A specification version boundary is the public point at which the repository declares that a specific closure target has become coherent enough to be treated as a stabilized published corpus state.
</p>

<p>
A version boundary should not be declared solely because:
</p>

<ul>
  <li>many files changed,</li>
  <li>one implementation can run one shortcut path,</li>
  <li>a strategy document became stronger,</li>
  <li>a roadmap milestone was renamed,</li>
  <li>one example happened to work in one environment.</li>
</ul>

<p>
A version boundary should instead correspond to a coherent published closure across the relevant target boundary.
</p>

<p>
For example:
</p>

<pre><code>source representation
    -&gt; structural validity posture
    -&gt; validated meaning
    -&gt; execution-facing IR posture
    -&gt; lowering / handoff posture
    -&gt; bounded conformance / example / reference coherence
</code></pre>

<p>
Where a version objective includes execution-oriented closure or widget-object closure, the required repository-visible corridor must be coherent at those boundaries too.
</p>

<p>
Where a version objective includes additive cross-version readability posture, the boundary should also state whether the current published subset is:
</p>

<ul>
  <li>expected to remain openable by older tools,</li>
  <li>expected to remain preservable by older tools,</li>
  <li>expected to require explicit refusal for certain operations.</li>
</ul>

<hr/>

<h2 id="stabilization-classes">19. Stabilization Classes</h2>
<p>
Each major topic discussed in the repository should be readable through one of the following stabilization classes:
</p>

<table>
  <thead>
    <tr>
      <th>Class</th>
      <th>Meaning</th>
      <th>Expected wording posture</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Normative and stabilized for current version scope</td>
      <td>Part of the current published technical law or repository-wide version boundary</td>
      <td>Clear, direct, architecture-bound, non-speculative</td>
    </tr>
    <tr>
      <td>Normative but still narrow in scope</td>
      <td>Published and binding, but only for a bounded subset</td>
      <td>Explicitly bounded and non-overclaimed</td>
    </tr>
    <tr>
      <td>Published draft</td>
      <td>Visible in the repository, but not yet treated as fully stabilized version-boundary law</td>
      <td>Clearly marked as draft, provisional, exploratory, or still under consolidation</td>
    </tr>
    <tr>
      <td>Deferred</td>
      <td>Intentionally not closed for the target version</td>
      <td>Explicitly postponed without ambiguity</td>
    </tr>
    <tr>
      <td>Out of scope</td>
      <td>Not part of the target version objective</td>
      <td>Explicitly excluded</td>
    </tr>
  </tbody>
</table>

<p>
This classification is repository-governance guidance.
Technical meaning remains owned by the relevant technical documents.
</p>

<hr/>

<h2 id="cross-repository-versioning-responsibilities">20. Cross-Repository Versioning Responsibilities</h2>
<p>
Version governance must preserve the published ownership boundaries of the repository.
</p>

<table>
  <thead>
    <tr>
      <th>Repository area</th>
      <th>Primary role</th>
      <th>Versioning responsibility</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>Expression/</code></td>
      <td>Canonical source representation and source-structural posture</td>
      <td>State source-visible rules that correspond to a source compatibility target</td>
    </tr>
    <tr>
      <td><code>Language/</code></td>
      <td>Validated program meaning</td>
      <td>Stabilize semantic law for the target scope</td>
    </tr>
    <tr>
      <td><code>IR/</code></td>
      <td>Execution-facing normalized representation and downstream corridor staging</td>
      <td>Stabilize the published open IR corridor relevant to the target version</td>
    </tr>
    <tr>
      <td><code>Libraries/</code></td>
      <td>Intrinsic primitive catalog law</td>
      <td>Stabilize primitive identities and primitive-local semantics for the target scope</td>
    </tr>
    <tr>
      <td><code>Profiles/</code></td>
      <td>Optional capability-family and bounded backend-family closure</td>
      <td>State which profile-level closures are in scope for a given version</td>
    </tr>
    <tr>
      <td><code>IDE/</code></td>
      <td>Authoring, observability, debugging, inspection, Program Model</td>
      <td>State published IDE-facing boundaries without collapsing into language law</td>
    </tr>
    <tr>
      <td><code>Examples/</code></td>
      <td>Illustrative named slices</td>
      <td>Demonstrate repository-visible bounded slices without becoming semantic law</td>
    </tr>
    <tr>
      <td><code>Conformance/</code></td>
      <td>Public accept / reject / preserve truth surface</td>
      <td>Reflect the public acceptance posture of the stabilized subset</td>
    </tr>
    <tr>
      <td><code>Implementations/Reference/</code></td>
      <td>Non-normative executable workspace</td>
      <td>Exercise bounded reference paths without becoming hidden normative truth</td>
    </tr>
    <tr>
      <td><code>Roadmap/</code></td>
      <td>Non-normative sequencing</td>
      <td>Track intended closure order, not specification law</td>
    </tr>
    <tr>
      <td><code>Strategy/</code></td>
      <td>Non-normative rationale and positioning</td>
      <td>Explain why the program matters, not what version law is</td>
    </tr>
    <tr>
      <td><code>Versioning/</code></td>
      <td>Centralized specification-version governance</td>
      <td>State corpus-level stabilization and transition policy</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2 id="compatibility-posture">21. Compatibility Posture</h2>
<p>
Compatibility claims should be made carefully and at the correct layer.
</p>

<p>
The repository should distinguish at least the following:
</p>

<ul>
  <li><strong>source compatibility</strong> — whether a <code>.frog</code> artifact conforms to the expected source-format boundary,</li>
  <li><strong>degraded readability compatibility</strong> — whether an older-capability tool may still open, inspect, and possibly preserve the artifact safely,</li>
  <li><strong>semantic compatibility</strong> — whether the validated meaning remains equivalent across specification evolution,</li>
  <li><strong>IR compatibility</strong> — whether the published execution-facing representation remains acceptable for the relevant version scope,</li>
  <li><strong>backend-handoff compatibility</strong> — whether downstream consumer-facing contracts remain stable enough for the targeted profile boundary,</li>
  <li><strong>example / conformance / reference alignment compatibility</strong> — whether repository-visible examples, conformance material, and the non-normative reference path still align with the intended published subset.</li>
</ul>

<p>
A compatibility statement should therefore identify:
</p>

<ul>
  <li>the layer concerned,</li>
  <li>the target scope,</li>
  <li>whether the statement is normative, bounded, draft, or deferred,</li>
  <li>whether the statement refers to full support or only to openable / inspectable / preservable degraded handling.</li>
</ul>

<hr/>

<h2 id="version-transition-criteria">22. Version Transition Criteria</h2>
<p>
A transition from one specification corpus version to the next should be considered only when the repository shows a coherent closure across the relevant target boundary.
</p>

<p>
The exact criteria depend on the intended version objective, but should generally include:
</p>

<ol>
  <li>the target scope is explicitly stated,</li>
  <li>the relevant ownership layers are mutually aligned,</li>
  <li>draft-only ambiguity has been reduced to an acceptable bounded level,</li>
  <li>examples and conformance material are aligned where they materially reflect the target subset,</li>
  <li>reference implementation material is not contradicting the published corridor,</li>
  <li>the new version does not silently collapse architectural boundaries.</li>
</ol>

<p>
Where the target version claims bounded executable closure, transition criteria should also include:
</p>

<ul>
  <li>an explicit execution-facing corridor for the bounded profile family,</li>
  <li>a repository-visible path from source to observable execution for the declared subset,</li>
  <li>clear distinction between public contract and runtime-private realization.</li>
</ul>

<p>
Where the target version claims richer widget-object closure, transition criteria should also include:
</p>

<ul>
  <li>alignment between widget declaration, widget interaction, widget class contract, UI library surface, and relevant profile-level class modeling,</li>
  <li>clear ownership of property read, property write, method invocation, and event exposure,</li>
  <li>preservation of the distinction between source declaration and IDE-synthesized node surfaces.</li>
</ul>

<p>
Where the target version claims additive and preservable cross-version handling posture, transition criteria should also include:
</p>

<ul>
  <li>clear identification of extensible source areas,</li>
  <li>clear distinction between safe degraded opening and full semantic acceptance,</li>
  <li>clear refusal behavior when unsupported constructs materially affect semantics or execution,</li>
  <li>clear preservation expectations for unknown sections where preservation is required.</li>
</ul>

<hr/>

<h2 id="published-version-objectives">23. Published Version Objectives</h2>
<p>
This section centralizes the current published version objectives at a repository-governance level.
</p>

<h3>23.1 Current published closure baseline</h3>
<p>
At the current published stage of the repository, the visible closure direction is already beyond architecture-only prose.
</p>

<p>
The repository already exposes a first bounded end-to-end corridor where a controlled subset can be:
</p>

<ul>
  <li>loaded,</li>
  <li>distinguished by loadability,</li>
  <li>structurally validated,</li>
  <li>semantically validated,</li>
  <li>derived into a published execution-facing IR posture,</li>
  <li>lowered toward a backend family,</li>
  <li>emitted as a backend-facing handoff,</li>
  <li>exercised through repository-visible examples, conformance material, and a non-normative reference path.</li>
</ul>

<p>
The repository also already exposes a serious widget-object closure direction through source-visible widget representation, interaction surfaces, class-contract discussion, and profile-level widget class modeling.
</p>

<h3>23.2 Near-term version objective</h3>
<p>
The near-term version objective is to consolidate the first coherent published corpus version around:
</p>

<ul>
  <li>canonical source posture,</li>
  <li>structural validation posture,</li>
  <li>validated semantic posture for the bounded subset,</li>
  <li>open execution-facing IR posture,</li>
  <li>canonical JSON IR posture where published,</li>
  <li>lowering and backend contract posture,</li>
  <li>bounded Native CPU LLVM corridor closure where published,</li>
  <li>bounded observable executable slices,</li>
  <li>coherent widget-object exposure for the currently published UI corridor,</li>
  <li>repository-visible conformance / examples / reference alignment,</li>
  <li>the repository-wide additive versioning and safe degraded readability doctrine,</li>
  <li>the cumulative version model for later specification evolution.</li>
</ul>

<p>
The near-term source closure also includes keeping <code>Expression/Source provenance.md</code> and <code>IDE/Authoring provenance.md</code> aligned with the additive, non-executable, degraded-reading model: provenance may improve inspection and review without changing canonical program meaning.
</p>

<h3>23.3 Explicitly not yet implied by this baseline</h3>
<p>
The current near-term version objective does not automatically imply:
</p>

<ul>
  <li>all possible backend families are stabilized,</li>
  <li>all runtime families are standardized,</li>
  <li>all future widget systems are closed,</li>
  <li>all IDE behaviors are frozen,</li>
  <li>all profile families are complete,</li>
  <li>all interoperability surfaces are final,</li>
  <li>that every older tool can fully edit every future FROG artifact.</li>
</ul>

<p>
Those areas may remain draft, partial, or deferred even if one bounded corpus version is considered coherently stabilized.
</p>

<hr/>

<h2 id="central-version-matrix">24. Central Version Matrix</h2>
<p>
The detailed current status of the repository is centralized in <code>Versioning/Matrix.md</code>.
</p>

<p>
That file should provide:
</p>

<ul>
  <li>the current value for each major repository surface,</li>
  <li>its stabilization status,</li>
  <li>its current version scope,</li>
  <li>its relation to the bounded published corpus objective,</li>
  <li>its next intended closure step where relevant.</li>
</ul>

<p>
This document remains the primary governance entry point.
The matrix remains the primary detailed status table.
</p>

<hr/>

<h2 id="how-other-documents-should-reference-this-surface">25. How Other Documents Should Reference This Surface</h2>
<p>
When another repository document needs versioning guidance, it should keep that guidance short and point back to this document rather than restating full transition policy.
</p>

<p>
Recommended pattern:
</p>

<pre><code>This document participates in the current published FROG specification corpus scope.
Cross-version stabilization targets, additive evolution doctrine, cumulative version model, and transition policy are governed centrally in `Versioning/Readme.md`.
Detailed current status by repository surface is centralized in `Versioning/Matrix.md`.
</code></pre>

<p>
Other documents should not duplicate:
</p>

<ul>
  <li>full version-transition criteria,</li>
  <li>repository-wide stabilization taxonomy,</li>
  <li>cross-version compatibility doctrine,</li>
  <li>multi-version governance policy,</li>
  <li>the central per-surface current-status matrix.</li>
</ul>

<hr/>

<h2 id="change-classification-guidance">26. Change Classification Guidance</h2>
<p>
A proposed repository change should be classified before version claims are made.
</p>

<table>
  <thead>
    <tr>
      <th>Change class</th>
      <th>Typical effect</th>
      <th>Versioning concern</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Clarification</td>
      <td>Makes existing intent more explicit without changing target law</td>
      <td>Usually does not require redefining version scope, but may help close ambiguity</td>
    </tr>
    <tr>
      <td>Additive bounded closure</td>
      <td>Adds a new explicitly bounded published capability or corridor segment</td>
      <td>May expand version scope if cross-layer coherence is reached</td>
    </tr>
    <tr>
      <td>Breaking source-visible change</td>
      <td>Changes canonical source acceptance or required source distinctions</td>
      <td>Must be evaluated against <code>.frog</code> <code>spec_version</code> implications</td>
    </tr>
    <tr>
      <td>Breaking semantic change</td>
      <td>Changes validated meaning for existing accepted constructs</td>
      <td>Requires careful corpus-level transition review</td>
    </tr>
    <tr>
      <td>Downstream corridor clarification</td>
      <td>Clarifies IR, lowering, backend contract, or execution-facing corridor boundaries</td>
      <td>May affect corpus-level stabilization without changing source compatibility</td>
    </tr>
    <tr>
      <td>Implementation-only change</td>
      <td>Changes a reference or experimental implementation without changing published law</td>
      <td>Does not by itself change specification version scope</td>
    </tr>
  </tbody>
</table>

<p>
A nominally additive change must still be reviewed for degraded readability safety.
If an older tool would likely misinterpret the addition semantically, the change must not be described as harmlessly additive without further qualification.
</p>

<hr/>

<h2 id="minimal-decision-table">27. Minimal Decision Table</h2>
<table>
  <thead>
    <tr>
      <th>Question</th>
      <th>If yes</th>
      <th>If no</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Does the change alter canonical source acceptance?</td>
      <td>Review <code>.frog</code> <code>spec_version</code> implications explicitly</td>
      <td>Do not claim source-compatibility impact</td>
    </tr>
    <tr>
      <td>Does the change alter validated semantic meaning?</td>
      <td>Treat as corpus-level semantic transition matter</td>
      <td>Keep semantic version claims narrow</td>
    </tr>
    <tr>
      <td>Does the change only clarify already-intended law?</td>
      <td>Classify as clarification unless another layer is affected</td>
      <td>Assess whether it is additive or breaking</td>
    </tr>
    <tr>
      <td>Does the change widen the bounded executable corridor?</td>
      <td>Check examples, conformance, profiles, and reference path alignment</td>
      <td>Do not overclaim execution closure</td>
    </tr>
    <tr>
      <td>Does the change widen widget-object closure?</td>
      <td>Check Expression / Libraries / Profiles alignment</td>
      <td>Do not imply richer UI-object law than published</td>
    </tr>
    <tr>
      <td>Does the change only affect roadmap or strategy?</td>
      <td>Keep it outside specification-version law</td>
      <td>Evaluate its technical layer implications separately</td>
    </tr>
    <tr>
      <td>Can an older-capability tool still open the artifact safely?</td>
      <td>State whether handling is openable, inspectable, and preservable</td>
      <td>Require explicit refusal behavior</td>
    </tr>
    <tr>
      <td>Can unsupported content be preserved without loss?</td>
      <td>State preservation expectations explicitly</td>
      <td>Restrict save/edit behavior accordingly</td>
    </tr>
    <tr>
      <td>Would an older tool risk semantic misunderstanding?</td>
      <td>Require explicit limitation or refusal</td>
      <td>Do not overconstrain degraded readability</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2 id="relationship-with-readme-examples-conformance-and-reference-implementation">28. Relationship with Readme, Examples, Conformance, and Reference Implementation</h2>
<p>
This document must remain aligned with nearby repository-visible surfaces without duplicating their ownership.
</p>

<table>
  <thead>
    <tr>
      <th>Surface</th>
      <th>Question it answers</th>
      <th>What it must not replace</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>Readme.md</code></td>
      <td>What the published repository currently is and how its layers relate</td>
      <td>Centralized version-transition law</td>
    </tr>
    <tr>
      <td><code>Examples/</code></td>
      <td>Which named illustrative corridors are currently visible</td>
      <td>Normative ownership or corpus-version policy</td>
    </tr>
    <tr>
      <td><code>Conformance/</code></td>
      <td>What is accepted, rejected, or preserved in public repository-visible truth surfaces</td>
      <td>Cross-version governance policy</td>
    </tr>
    <tr>
      <td><code>Implementations/Reference/</code></td>
      <td>How a non-normative executable workspace consumes a bounded corridor</td>
      <td>Normative law or corpus-version truth</td>
    </tr>
  </tbody>
</table>

<p>
The intended reading relation is:
</p>

<pre><code>Readme.md
   -&gt; repository-wide architectural entry point

Examples/
   -&gt; illustrative bounded mirrors

Conformance/
   -&gt; public accept / reject / preserve truth

Implementations/Reference/
   -&gt; non-normative executable consumption path

Versioning/
   -&gt; centralized statement of what current repository-wide version posture means
</code></pre>

<p>
Versioning governance sits adjacent to those surfaces, but it is not reducible to any of them.
</p>

<hr/>

<h2 id="repository-wide-versioning-diagram">29. Repository-Wide Versioning Diagram</h2>
<pre><code>published FROG repository
    |
    +-- technical ownership layers
    |      |
    |      +-- Expression/
    |      +-- Language/
    |      +-- IR/
    |      +-- Libraries/
    |      +-- Profiles/
    |      +-- IDE/
    |
    +-- support and inspection surfaces
    |      |
    |      +-- Examples/
    |      +-- Conformance/
    |      +-- Implementations/Reference/
    |
    +-- framing surfaces
    |      |
    |      +-- Strategy/
    |      +-- Roadmap/
    |
    +-- centralized version governance
           |
           +-- Versioning/Readme.md
           +-- Versioning/Matrix.md

version notions that must remain distinct:
    specification corpus version
        != .frog spec_version
        != metadata.program_version

cross-version handling doctrine:
    open if possible
    inspect what is known
    preserve what is unknown when safe
    refuse unsafe semantic or executable claims
    never silently misinterpret

cumulative version model:
    later version
        =
    earlier version
        +
    explicit bounded additions
</code></pre>

<hr/>

<h2 id="future-expansion">30. Future Expansion</h2>
<p>
This document may later be expanded with:
</p>

<ul>
  <li>a compact version history table,</li>
  <li>explicit version identifiers for successive published corpus states,</li>
  <li>migration notes between corpus versions,</li>
  <li>source-compatibility mapping tables,</li>
  <li>capability-gating guidance where needed,</li>
  <li>release-bound stabilization checklists for major closure fronts.</li>
</ul>

<p>
Such additions should remain centralized here unless a strong repository-architecture reason requires a separate dedicated subdocument set.
</p>

<hr/>

<h2 id="summary">31. Summary</h2>
<p>
FROG needs centralized specification-version governance because the repository now spans multiple technical ownership layers, illustrative example corridors, conformance material, non-normative executable reference work, widget-object closure, and framing layers.
</p>

<p>
This document provides the single repository-visible place where the published specification corpus can state:
</p>

<ul>
  <li>what specification corpus version means,</li>
  <li>how it differs from <code>.frog</code> source compatibility versioning,</li>
  <li>how it differs from program artifact versioning,</li>
  <li>that specification evolution is additive by default,</li>
  <li>that later versions should normally be cumulative extensions of earlier valid forms,</li>
  <li>that degraded cross-version readability should be explicit and safe,</li>
  <li>that unknown supported-preservable content should not be silently destroyed,</li>
  <li>that semantic misunderstanding is worse than explicit refusal,</li>
  <li>what the current published closure target is,</li>
  <li>where the detailed current-status matrix is centralized,</li>
  <li>and how transition to future specification versions should be governed.</li>
</ul>

<p>
All other repository documents should keep version references lightweight and point back here instead of re-scattering cross-version policy.
</p>
