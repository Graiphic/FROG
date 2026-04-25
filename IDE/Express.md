<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 FROG IDE Express Specification</h1>

<p align="center">
Definition of guided Express authoring entries for the FROG IDE<br/>
<em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#scope-for-v01">2. Scope for v0.1</a></li>
  <li><a href="#architectural-position-and-dependencies">3. Architectural Position and Dependencies</a></li>
  <li><a href="#design-principles">4. Design Principles</a></li>
  <li><a href="#what-an-express-entry-is">5. What an Express Entry Is</a></li>
  <li><a href="#what-express-is-not">6. What Express Is Not</a></li>
  <li><a href="#express-kinds">7. Express Kinds</a></li>
  <li><a href="#configuration-model">8. Configuration Model</a></li>
  <li><a href="#configurable-parameters-vs-expandable-terminals">9. Configurable Parameters vs Expandable Terminals</a></li>
  <li><a href="#canonical-normalization">10. Canonical Normalization</a></li>
  <li><a href="#instance-model">11. Instance Model</a></li>
  <li><a href="#palette-integration">12. Palette Integration</a></li>
  <li><a href="#edit-reopen-detach-and-materialize">13. Edit, Reopen, Detach, and Materialize</a></li>
  <li><a href="#program-model-and-optional-source-persistence">14. Program Model and Optional Source Persistence</a></li>
  <li><a href="#validation-rules">15. Validation Rules</a></li>
  <li><a href="#examples">16. Examples</a></li>
  <li><a href="#out-of-scope-for-v01">17. Out of Scope for v0.1</a></li>
  <li><a href="#summary">18. Summary</a></li>
  <li><a href="#license">19. License</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
Express authoring is an <strong>IDE-layer guided authoring capability</strong> that allows a user to insert,
configure, and re-edit common programming tasks through a guided experience rather than through raw
low-level assembly alone.
</p>

<p>
Its purpose is to improve usability, discoverability, speed of insertion, and correctness for recurring
workflows while preserving the canonical FROG model.
</p>

<p>
Express is therefore a <strong>guided authoring presentation mechanism</strong>, not a separate language.
A conforming FROG IDE MAY expose Express entries, but any inserted Express-authored content MUST normalize
to canonical FROG content already valid in the active canonical insertable space of the IDE.
</p>

<p>
This document defines the normative IDE-facing role of Express entries, their lifecycle,
their configuration model, their relationship to canonical source, and the minimum rules required to
preserve stable round-trip authoring.
</p>

<pre><code>One-line model

guided entry
    -&gt; canonical target
    -&gt; canonical content
    -&gt; execution-facing systems

Never:

guided entry
    -&gt; hidden semantic construct
</code></pre>

<hr/>

<h2 id="scope-for-v01">2. Scope for v0.1</h2>

<p>
For FROG v0.1, this document standardizes:
</p>

<ul>
  <li>what an Express entry is,</li>
  <li>what Express is not,</li>
  <li>the IDE-facing lifecycle of Express-authored instances,</li>
  <li>the distinction between configurable parameters and expandable terminals,</li>
  <li>the requirement that Express normalize to canonical content,</li>
  <li>the use of optional non-authoritative recoverability metadata for re-editability.</li>
</ul>

<p>
This document does <strong>not</strong> standardize:
</p>

<ul>
  <li>a new canonical node kind,</li>
  <li>a new semantic structure family,</li>
  <li>new primitive semantics,</li>
  <li>new profile semantics,</li>
  <li>a mandatory UI style for guided editing,</li>
  <li>a requirement that every canonical entry must provide an Express surface,</li>
  <li>a requirement that runtimes understand or consume Express-private state.</li>
</ul>

<hr/>

<h2 id="architectural-position-and-dependencies">3. Architectural Position and Dependencies</h2>

<h3>3.1 Repository position</h3>

<p>
This document belongs in <code>IDE/</code> because it defines a guided authoring behavior of the IDE.
It does <strong>not</strong> belong to <code>Expression/</code>, <code>Language/</code>, <code>Libraries/</code>,
or <code>Profiles/</code> because it does not own canonical source identity, execution meaning,
primitive-local semantics, or profile-owned capability semantics.
</p>

<h3>3.2 Dependencies</h3>

<p>
This document depends on the following specifications:
</p>

<ul>
  <li><code>IDE/Readme.md</code> — architectural role of Express authoring in the IDE layer,</li>
  <li><code>IDE/Palette.md</code> — discovery, search, filtering, insertion, and the rule that all insertion paths resolve to the same canonical insertable space,</li>
  <li><code>IDE/Snippet.md</code> — insertion and fragment-handling behavior for authoring fragments,</li>
  <li><code>Expression/Readme.md</code> — canonical source ownership and the optional source-level <code>ide</code> section,</li>
  <li><code>Expression/IDE preferences.md</code> — non-authoritative IDE-facing recoverability metadata,</li>
  <li><code>Expression/Diagram.md</code> — canonical graph objects and standard node kinds,</li>
  <li><code>Expression/Control structures.md</code> — canonical structure identities and the rule that authoring-facing derived forms do not create new canonical structure families,</li>
  <li><code>Libraries/</code> documents — intrinsic primitive identities and primitive-local semantics,</li>
  <li><code>Profiles/</code> documents — optional standardized capability families and profile-owned capability meaning,</li>
  <li><code>Language/</code> documents — cross-cutting normative execution semantics.</li>
</ul>

<h3>3.3 Ownership boundary</h3>

<p>
If a conflict appears, the following ownership rules apply:
</p>

<ul>
  <li><code>Expression/</code> remains authoritative for canonical source representation,</li>
  <li><code>Language/</code> remains authoritative for normative execution meaning,</li>
  <li><code>Libraries/</code> remain authoritative for intrinsic primitive identity and primitive-local semantics,</li>
  <li><code>Profiles/</code> remain authoritative for optional standardized capability families and profile-owned meaning,</li>
  <li><code>IDE/Express.md</code> remains authoritative only for guided authoring behavior.</li>
</ul>

<pre><code>Ownership reminder

Expression/   -&gt; canonical source representation
Language/     -&gt; normative execution meaning
Libraries/    -&gt; intrinsic primitive identity and local semantics
Profiles/     -&gt; optional standardized capability families
IDE/Express   -&gt; guided authoring behavior
</code></pre>

<p>
If a conflict arises between Express presentation and canonical ownership, canonical ownership wins.
</p>

<hr/>

<h2 id="design-principles">4. Design Principles</h2>

<h3>4.1 Faster authoring without semantic drift</h3>

<p>
Common tasks SHOULD be insertable with less friction than manual low-level assembly.
That convenience MUST NOT create hidden language semantics.
</p>

<h3>4.2 Same canonical insertable space</h3>

<p>
Express authoring MUST resolve to the same canonical insertable space as other insertion paths.
An Express lens is a guided access mode over canonical content, not an alternative semantic universe.
</p>

<h3>4.3 Preserved canonical identity</h3>

<p>
An Express-authored instance MUST resolve to a canonical target whose identity remains knowable.
The user SHOULD be able to discover what canonical primitive, sub-FROG, or canonical fragment
the guided entry actually targets.
</p>

<h3>4.4 Stable re-editability</h3>

<p>
An Express-authored instance SHOULD remain safely reconfigurable after load and reload when the IDE
preserves sufficient non-authoritative recoverability state.
</p>

<h3>4.5 Tooling independence from Express-private state</h3>

<p>
Validation, execution preparation, debugging, probes, watches, snippets, and execution-facing systems
MUST remain grounded in canonical content rather than in Express-private UI state.
</p>

<h3>4.6 Transparency</h3>

<p>
The IDE SHOULD make it possible to inspect:
</p>

<ul>
  <li>the Express entry identity,</li>
  <li>the canonical target identity,</li>
  <li>the current configuration mapping,</li>
  <li>the visible optional terminals if relevant.</li>
</ul>

<h3>4.7 Minimal but extensible</h3>

<p>
FROG v0.1 defines a conservative Express model.
Stronger IDE implementations MAY provide richer guided experiences, provided that canonical normalization,
canonical ownership, and execution-facing independence remain preserved.
</p>

<hr/>

<h2 id="what-an-express-entry-is">5. What an Express Entry Is</h2>

<p>
An <strong>Express entry</strong> is an IDE-defined, guided, task-oriented authoring entry that inserts or edits
canonical FROG content through a guided configuration workflow.
</p>

<p>
An Express entry is characterized by the following properties:
</p>

<ul>
  <li>it is guided,</li>
  <li>it is instance-local,</li>
  <li>it is task-oriented,</li>
  <li>it is normalizable to canonical content,</li>
  <li>it is re-editable when sufficient non-authoritative recoverability state is preserved.</li>
</ul>

<p>
An Express entry MAY represent:
</p>

<ul>
  <li>a guided configuration surface for a single canonical <code>primitive</code> node,</li>
  <li>a guided configuration surface for a canonical <code>subfrog</code> node,</li>
  <li>a guided materialization process for a deterministic canonical fragment.</li>
</ul>

<p>
A conforming IDE SHOULD make the canonical target identity visible somewhere in the guided experience,
entry metadata, or instance detail view.
</p>

<hr/>

<h2 id="what-express-is-not">6. What Express Is Not</h2>

<p>
Express is not:
</p>

<ul>
  <li>a new canonical node kind,</li>
  <li>a new semantic structure family,</li>
  <li>a replacement for canonical source identity,</li>
  <li>a reason for execution-facing systems to depend on editor-private UI state,</li>
  <li>a permission to hide semantically required program content from conforming tooling.</li>
</ul>

<p>
Accordingly:
</p>

<ul>
  <li>an Express entry MUST NOT redefine primitive identity,</li>
  <li>an Express entry MUST NOT redefine profile-owned capability identity,</li>
  <li>an Express entry MUST NOT redefine language semantics,</li>
  <li>an Express entry MUST NOT require a conforming runtime to understand IDE-private Express state,</li>
  <li>an Express entry MUST NOT make canonical execution meaning ambiguous.</li>
</ul>

<hr/>

<h2 id="express-kinds">7. Express Kinds</h2>

<p>
A conforming IDE MAY support one or more of the following Express kinds.
</p>

<h3>7.1 Primitive-backed Express</h3>

<p>
A primitive-backed Express entry configures one canonical <code>primitive</code> node.
This is the most direct Express form and is appropriate when:
</p>

<ul>
  <li>the canonical primitive already exists,</li>
  <li>the primitive has meaningful configurable parameters,</li>
  <li>optional terminals may need to be exposed progressively,</li>
  <li>the guided experience mainly improves usability rather than structural composition.</li>
</ul>

<h3>7.2 Sub-FROG-backed Express</h3>

<p>
A sub-FROG-backed Express entry configures one canonical <code>subfrog</code> node.
This form is appropriate when:
</p>

<ul>
  <li>the guided task is better represented as a reusable program unit,</li>
  <li>the underlying behavior is more complex than a single primitive,</li>
  <li>the IDE should expose a task-oriented entry while preserving a clean canonical diagram identity.</li>
</ul>

<p>
A sub-FROG-backed Express entry MUST preserve the canonical <code>subfrog</code> identity and MUST NOT invent
a boundary different from the one implied by the referenced FROG interface.
</p>

<h3>7.3 Fragment-backed Express</h3>

<p>
A fragment-backed Express entry deterministically materializes a canonical fragment containing multiple canonical objects.
This form SHOULD be used more conservatively because it requires stronger guarantees for:
</p>

<ul>
  <li>stable re-editability,</li>
  <li>deterministic materialization,</li>
  <li>mapping from Express instance state to owned canonical objects,</li>
  <li>safe detach or materialize behavior.</li>
</ul>

<p>
For v0.1, primitive-backed and sub-FROG-backed Express are the preferred baseline.
Fragment-backed Express MAY exist, but it SHOULD remain conservative and predictable.
</p>

<pre><code>Preferred baseline for v0.1

primitive-backed   -&gt; preferred
subfrog-backed     -&gt; preferred
fragment-backed    -&gt; allowed, but conservative
</code></pre>

<hr/>

<h2 id="configuration-model">8. Configuration Model</h2>

<p>
An Express entry SHOULD provide a guided configuration experience appropriate to the target task.
That experience MAY be implemented through:
</p>

<ul>
  <li>a modal dialog,</li>
  <li>a non-modal properties panel,</li>
  <li>an inline popover,</li>
  <li>a staged wizard,</li>
  <li>another equivalent IDE interaction surface.</li>
</ul>

<p>
The configuration experience SHOULD allow the user to:
</p>

<ul>
  <li>understand what the entry does,</li>
  <li>set configuration values,</li>
  <li>review visible optional terminals,</li>
  <li>preview the canonical target identity,</li>
  <li>confirm insertion,</li>
  <li>reopen the same configuration later when re-editing.</li>
</ul>

<p>
A conforming IDE MAY provide task-oriented wording and grouped controls that are more approachable than raw
canonical parameter names, but the mapping from those controls to canonical content MUST remain deterministic.
</p>

<hr/>

<h2 id="configurable-parameters-vs-expandable-terminals">9. Configurable Parameters vs Expandable Terminals</h2>

<p>
If Express authoring is supported, the IDE SHOULD distinguish clearly between:
</p>

<ul>
  <li><strong>configurable parameters</strong> — values or options primarily edited through the guided configuration experience,</li>
  <li><strong>expandable terminals</strong> — optional visible inputs or outputs that the user may choose to expose directly on the diagram,</li>
  <li><strong>canonical target identity</strong> — the actual primitive, sub-FROG, or canonical fragment to which the Express instance resolves.</li>
</ul>

<h3>9.1 Configurable parameters</h3>

<p>
Configurable parameters typically represent:
</p>

<ul>
  <li>modes,</li>
  <li>units,</li>
  <li>defaults,</li>
  <li>policy selections,</li>
  <li>constant values,</li>
  <li>other options best edited through a guided form.</li>
</ul>

<p>
A configurable parameter MAY map to:
</p>

<ul>
  <li>a canonical constant input value,</li>
  <li>a configuration field for a sub-FROG-backed instance,</li>
  <li>a deterministic materialization choice for a fragment-backed instance.</li>
</ul>

<h3>9.2 Expandable terminals</h3>

<p>
Expandable terminals are optional visible terminals that the user may expose or hide for authoring convenience.
The following rules apply:
</p>

<ul>
  <li>an expandable terminal MUST correspond to a port valid for the canonical target,</li>
  <li>an Express presentation MUST NOT invent semantically invalid terminals,</li>
  <li>an Express presentation MUST NOT hide a semantically required terminal in a way that makes the canonical model ambiguous,</li>
  <li>the mapping between configuration state and terminal visibility SHOULD be deterministic.</li>
</ul>

<hr/>

<h2 id="canonical-normalization">10. Canonical Normalization</h2>

<p>
Canonical normalization is the process by which an Express presentation becomes ordinary canonical FROG content.
</p>

<p>
A conforming IDE MUST ensure that:
</p>

<ul>
  <li>an Express entry normalizes to content already valid in the active canonical insertable space,</li>
  <li>normalization preserves the intended source-level meaning,</li>
  <li>execution-facing systems can operate on normalized canonical content without depending on Express UI state.</li>
</ul>

<p>
Canonical normalization MAY target:
</p>

<ul>
  <li>a canonical <code>primitive</code> node,</li>
  <li>a canonical <code>subfrog</code> node,</li>
  <li>a deterministic canonical fragment whose owned objects are all valid under the active insertable space.</li>
</ul>

<p>
The normalization process MUST NOT:
</p>

<ul>
  <li>create a hidden semantic family,</li>
  <li>introduce runtime-required editor-private data,</li>
  <li>change canonical meaning depending on transient editor UI state alone.</li>
</ul>

<pre><code>Normalization rule

guided presentation
    -&gt; canonical target
    -&gt; canonical content

Execution-facing systems consume:
- canonicalized content

They do not consume:
- guided UI state
</code></pre>

<p>
If an Express workflow materializes canonical content through guided user choices, deterministic generation, or AI participation, a provenance-aware IDE SHOULD classify that authoring path explicitly. Direct human configuration, tool generation, and AI-assisted generation SHOULD NOT be collapsed into the same provenance claim merely because all of them normalize to valid canonical content.
</p>

<pre><code>Express provenance posture

guided human configuration  -&gt; human_guided or equivalent claim
deterministic materializer   -&gt; tool_generated or equivalent claim
AI-assisted materializer     -&gt; ai_assisted / ai_generated claim

all cases still normalize to canonical FROG content
</code></pre>

<hr/>

<h2 id="instance-model">11. Instance Model</h2>

<p>
Each Express-authored instance SHOULD have a stable IDE-level identity while the user edits the program.
</p>

<p>
A conforming IDE SHOULD be able to associate the following conceptual elements with an Express-authored instance:
</p>

<ul>
  <li><strong>instance identity</strong> — the IDE-side identity of the Express-authored occurrence,</li>
  <li><strong>express identity</strong> — the Express entry definition used to create or edit the instance,</li>
  <li><strong>canonical target identity</strong> — the primitive, sub-FROG, or fragment target to which the instance resolves,</li>
  <li><strong>configuration state</strong> — non-authoritative recoverability state sufficient to reopen guided editing,</li>
  <li><strong>visible optional terminals</strong> — the current authoring-facing terminal exposure state,</li>
  <li><strong>owned canonical objects</strong> — the canonical object or objects created or edited by the instance.</li>
</ul>

<p>
This instance model is an IDE concern.
It MUST remain reducible to canonical content for execution-facing purposes.
</p>

<hr/>

<h2 id="palette-integration">12. Palette Integration</h2>

<p>
Express entries integrate with the palette as guided insertion entries.
They MAY appear in:
</p>

<ul>
  <li>primary intent-first browsing,</li>
  <li>search results,</li>
  <li>context-aware insertion results,</li>
  <li>a dedicated guided or Express lens.</li>
</ul>

<p>
A conforming IDE SHOULD:
</p>

<ul>
  <li>make Express entries visually distinguishable from direct primitive, structure, node-insertion, and annotation entries,</li>
  <li>disclose canonical target identity in entry metadata or detail view,</li>
  <li>allow the user to reach the corresponding direct canonical entry where that improves transparency.</li>
</ul>

<p>
Express entries MUST remain semantically consistent with the same underlying canonical insertable space as other insertion paths.
An Express lens is therefore a convenience surface, not a distinct semantic catalog.
</p>

<p>
Express entries SHOULD be especially useful for task-oriented workflows such as:
</p>

<ul>
  <li>common I/O tasks,</li>
  <li>common signal-processing tasks,</li>
  <li>common connectivity tasks when relevant profile support is active,</li>
  <li>other repetitive workflows where guided configuration materially improves usability.</li>
</ul>

<p>
Express entries MAY also exist for implementation-supported third-party surfaces.
When they do, the IDE SHOULD keep them visually distinguishable from baseline standardized entries and from standardized profile-defined entries.
</p>

<hr/>

<h2 id="edit-reopen-detach-and-materialize">13. Edit, Reopen, Detach, and Materialize</h2>

<p>
A conforming IDE SHOULD support the following lifecycle operations for Express-authored instances where Express is available:
</p>

<ul>
  <li><strong>insert and configure</strong> — create the initial canonical target through a guided workflow,</li>
  <li><strong>reopen configuration</strong> — reopen guided editing for a previously inserted instance,</li>
  <li><strong>reconfigure</strong> — change configuration and update canonical content deterministically,</li>
  <li><strong>detach or discard Express presentation</strong> — remove the guided layer while preserving valid canonical content,</li>
  <li><strong>materialize</strong> — when supported, convert the guided instance into plain canonical content with no remaining guided editing contract.</li>
</ul>

<h3>13.1 Reopen</h3>

<p>
Reopening configuration SHOULD restore enough guided state to allow meaningful continued editing without reconstructing
the instance from guesswork alone.
</p>

<h3>13.2 Detach</h3>

<p>
Detach removes the Express presentation while leaving the underlying canonical object model intact.
After detach:
</p>

<ul>
  <li>the resulting content MUST remain valid canonical FROG content,</li>
  <li>execution-facing behavior MUST remain grounded in canonical meaning,</li>
  <li>the user MAY continue editing the resulting content directly.</li>
</ul>

<h3>13.3 Materialize</h3>

<p>
Materialize is especially relevant for fragment-backed Express.
When materialization is supported:
</p>

<ul>
  <li>the resulting canonical fragment MUST be deterministic,</li>
  <li>the IDE SHOULD make clear that future reconfiguration may no longer use the original guided contract,</li>
  <li>materialization MUST NOT produce semantically hidden required content.</li>
</ul>

<pre><code>Lifecycle model

insert
   -&gt; configure
   -&gt; normalize to canonical target
   -&gt; reopen / reconfigure if supported
   -&gt; detach safely if desired
   -&gt; materialize if supported
</code></pre>

<hr/>

<h2 id="program-model-and-optional-source-persistence">14. Program Model and Optional Source Persistence</h2>

<h3>14.1 Program Model representation</h3>

<p>
The FROG Program Model MAY preserve non-authoritative Express-related state in memory during editing.
That state SHOULD be sufficient to support:
</p>

<ul>
  <li>stable mapping from an Express instance to owned canonical objects,</li>
  <li>reopen and reconfigure workflows,</li>
  <li>deterministic terminal visibility,</li>
  <li>safe detach or materialize operations,</li>
  <li>round-trip editing without semantic drift.</li>
</ul>

<p>
Typical IDE-side Program Model concerns include:
</p>

<ul>
  <li>preferred guided presentation,</li>
  <li>configuration state,</li>
  <li>per-instance editor hints,</li>
  <li>optional terminal visibility state,</li>
  <li>mapping to canonical target identity.</li>
</ul>

<p>
This state remains IDE-owned and non-authoritative for execution semantics.
</p>

<h3>14.2 Optional source persistence</h3>

<p>
The FROG source format MAY optionally persist IDE-facing recoverability aids through the source-level
<code>ide</code> section owned by the Expression layer.
Within that section, Express-related data SHOULD be treated as recoverability metadata rather than as executable program content.
</p>

<p>
If Express-related state is persisted there:
</p>

<ul>
  <li>it MUST remain optional,</li>
  <li>it MUST remain non-authoritative for execution semantics,</li>
  <li>it MUST NOT be required to determine canonical executable meaning,</li>
  <li>it SHOULD support stable reopen and re-edit workflows,</li>
  <li>it MUST be safely ignorable by runtimes, compilers, and other execution-facing systems.</li>
</ul>

<p>
Examples of persistable Express-related IDE state MAY include:
</p>

<ul>
  <li>Express entry identity,</li>
  <li>guided presentation preferences,</li>
  <li>recoverable configuration values,</li>
  <li>optional terminal visibility state,</li>
  <li>mapping from Express instance identity to owned canonical objects.</li>
</ul>

<p>
A conforming runtime MUST NOT require this state in order to execute the program.
</p>

<pre><code>Source persistence rule

canonical source
    may carry
optional recoverability metadata

Recoverability metadata
    -&gt; helps IDE reopen guided state
    -&gt; does not define execution meaning
</code></pre>

<hr/>

<h2 id="validation-rules">15. Validation Rules</h2>

<p>
A conforming IDE MUST validate Express behavior against canonical ownership and canonical validity.
At minimum:
</p>

<ul>
  <li>an Express entry MUST resolve to valid canonical content,</li>
  <li>the disclosed canonical target identity MUST match the actual normalized target,</li>
  <li>expandable terminals MUST correspond to ports valid for the canonical target,</li>
  <li>reconfiguration MUST update canonical content deterministically,</li>
  <li>detach or materialize MUST leave valid canonical content,</li>
  <li>execution-facing systems MUST be able to ignore Express-private presentation state.</li>
</ul>

<p>
A conforming IDE SHOULD also detect and report:
</p>

<ul>
  <li>broken mappings between Express instances and canonical targets,</li>
  <li>configuration states that no longer match the active insertable space,</li>
  <li>profile mismatches that would make a profile-defined target unavailable or invalid,</li>
  <li>third-party extension mismatches where the referenced target surface is unavailable,</li>
  <li>stale recoverability state that cannot safely be reapplied.</li>
</ul>

<hr/>

<h2 id="examples">16. Examples</h2>

<h3>16.1 Primitive-backed Express example</h3>

<p>
A guided numeric filter entry MAY configure one canonical primitive target.
The guided UI may expose:
</p>

<ul>
  <li>filter family,</li>
  <li>cutoff value,</li>
  <li>units,</li>
  <li>optional error terminals.</li>
</ul>

<p>
After insertion, the canonical target remains an ordinary canonical object known to the active insertable space.
</p>

<h3>16.2 Sub-FROG-backed Express example</h3>

<p>
A task-oriented file-reading entry such as <em>Read Text File</em> MAY configure one canonical <code>subfrog</code> node whose
public contract is easier to present through guided wording than through raw low-level assembly.
</p>

<h3>16.3 Fragment-backed Express example</h3>

<p>
A guided acquisition or connectivity workflow MAY materialize a deterministic fragment containing several canonical objects.
If so, the IDE MUST preserve deterministic mapping between the Express instance and that fragment for as long as guided
re-editing is supported.
</p>

<h3>16.4 Conceptual persisted IDE metadata example</h3>

<pre><code>{
  "ide": {
    "recoverability": {
      "express_bindings": [
        {
          "instance_id": "expr_001",
          "express_id": "frog.ide.express.read_text_file",
          "canonical_target_kind": "subfrog",
          "canonical_target_ref": "frog.io.read_text_file.basic",
          "visible_optional_terminals": ["error_in", "error_out"],
          "config": {
            "encoding": "utf8",
            "line_endings": "auto"
          }
        }
      ]
    }
  }
}</code></pre>

<p>
This example is conceptual.
It illustrates the kind of non-authoritative recoverability state an IDE MAY preserve.
It does not redefine canonical execution meaning.
</p>

<hr/>

<h2 id="out-of-scope-for-v01">17. Out of Scope for v0.1</h2>

<ul>
  <li>turning Express into a new canonical node kind,</li>
  <li>turning Express into a new canonical structure family,</li>
  <li>requiring runtimes to interpret Express-private presentation state,</li>
  <li>opaque Express systems whose canonical target cannot be determined by a conforming IDE,</li>
  <li>automatic hidden semantic transformation beyond deterministic canonical normalization,</li>
  <li>marketplace-specific Express UX or commercial distribution workflows,</li>
  <li>cloud-hosted Express catalogs,</li>
  <li>a requirement that every primitive family already provide Express entries,</li>
  <li>full standardization of fragment-backed Express lifecycle semantics across all future profiles,</li>
  <li>execution-private helper content that cannot be represented as canonical FROG content.</li>
</ul>

<hr/>

<h2 id="summary">18. Summary</h2>

<p>
Express authoring gives the FROG IDE a guided insertion model for common tasks without creating a separate language.
</p>

<ul>
  <li>Express entries belong to the IDE layer.</li>
  <li>They MUST normalize to canonical FROG content.</li>
  <li>They MUST NOT become new semantic node kinds or structure families.</li>
  <li>They SHOULD disclose canonical target identity.</li>
  <li>They MAY preserve non-authoritative recoverability state in the Program Model and, optionally, in the source-level <code>ide</code> section.</li>
  <li>They SHOULD support insert, configure, reopen, reconfigure, and safe detach workflows.</li>
  <li>Execution-facing systems MUST remain grounded in canonicalized content rather than guided UI state.</li>
</ul>

<p>
This gives FROG a modern guided-authoring model compatible with practical IDE workflows while preserving
an open, durable, canonical graphical language representation.
</p>

<hr/>

<h2 id="license">19. License</h2>

<p>
This specification is part of the FROG repository and is governed by the repository license and contribution rules.
</p>
