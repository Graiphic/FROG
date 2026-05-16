<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Roadmap</h1>

<p align="center">
  <strong>Non-normative planning layer for the progressive closure of FROG as a language, conformance surface, reference path, compiler corridor, runtime path, deployment path, and future IDE ecosystem</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<p align="center">
  Roadmap start date: <strong>8 March 2026</strong>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#project-map-at-a-glance">1. Project map at a glance</a></li>
  <li><a href="#purpose-of-this-roadmap">2. Purpose of this roadmap</a></li>
  <li><a href="#what-this-roadmap-is-and-is-not">3. What this roadmap is and is not</a></li>
  <li><a href="#relationship-with-versioning">4. Relationship with Versioning</a></li>
  <li><a href="#status-legend">5. Status legend</a></li>
  <li><a href="#guiding-principles">6. Guiding principles</a></li>
  <li><a href="#current-published-baseline">7. Current published baseline</a></li>
  <li><a href="#priority-order">8. Priority order</a></li>
  <li><a href="#high-level-roadmap">9. High-level roadmap</a></li>
  <li><a href="#phase-0-foundation-already-established">10. Phase 0 — Foundation already established</a></li>
  <li><a href="#phase-1-source-and-semantics-closure">11. Phase 1 — Source and semantics closure</a></li>
  <li><a href="#phase-2-ir-and-compiler-corridor-closure">12. Phase 2 — IR and compiler-corridor closure</a></li>
  <li><a href="#phase-3-bounded-execution-start-and-reference-path-closure">13. Phase 3 — Bounded execution-start and reference-path closure</a></li>
  <li><a href="#phase-4-widget-object-and-ui-corridor-closure">14. Phase 4 — Widget-object and UI corridor closure</a></li>
  <li><a href="#phase-5-conformance-growth-and-profile-broadening">15. Phase 5 — Conformance growth and profile broadening</a></li>
  <li><a href="#phase-6-ecosystem-and-industrial-standard-position">16. Phase 6 — Ecosystem and industrial-standard position</a></li>
  <li><a href="#cross-cutting-workstreams">17. Cross-cutting workstreams</a></li>
  <li><a href="#milestone-navigation">18. Milestone navigation</a></li>
  <li><a href="#definition-of-success">19. Definition of success</a></li>
</ul>

<hr/>

<h2 id="project-map-at-a-glance">1. Project map at a glance</h2>

<pre><code>FROG closure sequence

canonical .frog source
        |
        v
loadability
        |
        v
structural validation
        |
        v
validated language meaning
        |
        v
standardized FROG execution IR
        |
        v
backend-specific lowering
        |
        v
backend contract / consumable handoff
        |
        v
runtime or compiler/backend realization
        |
        v
deployable artifact


in parallel

authoring
   -&gt; Program Model
   -&gt; graphical IDE
   -&gt; validation feedback
   -&gt; observability / debugging
   -&gt; reusable industrial workflow


project logic

semantic closure
   -&gt; schema / validation closure
   -&gt; conformance closure
   -&gt; reference execution proof
   -&gt; backend credibility
   -&gt; IDE credibility
   -&gt; ecosystem credibility
</code></pre>

<p>
This roadmap explains the intended closure order.
It does not replace <code>Expression/</code>, <code>Language/</code>, <code>IR/</code>, <code>Libraries/</code>, <code>Profiles/</code>, <code>IDE/</code>, or any other normative ownership layer.
</p>

<hr/>

<h2 id="purpose-of-this-roadmap">2. Purpose of this roadmap</h2>

<p>
This roadmap exists to track the planned closure path of FROG as a full long-term ecosystem effort.
It is a planning layer inside the repository, not the source of normative language truth.
</p>

<p>
Its practical purpose is to answer one project question:
</p>

<p>
<strong>What sequence of concrete closures must be completed to move from the currently published FROG baseline to a serious open graphical programming ecosystem with a credible compiler/runtime path, credible deployment depth, a serious future IDE, and a credible open industrial language position?</strong>
</p>

<p>
That distinction matters:
</p>

<ul>
  <li>the specification layers define what FROG is,</li>
  <li>the roadmap layer defines in what order the project should be closed.</li>
</ul>

<p>
The roadmap also exists to keep the project disciplined.
It helps ensure that strategic ambition does not outrun technical closure, and that new fronts are not opened faster than the current architectural corridor is stabilized.
</p>

<hr/>

<h2 id="what-this-roadmap-is-and-is-not">3. What this roadmap is and is not</h2>

<h3>What it is</h3>

<ul>
  <li>a sequencing layer,</li>
  <li>a closure plan,</li>
  <li>a milestone map,</li>
  <li>a prioritization surface for technical and ecosystem work,</li>
  <li>a public statement of closure order for the repository-visible corridor.</li>
</ul>

<h3>What it is not</h3>

<ul>
  <li>not the normative language definition,</li>
  <li>not the source of semantic truth,</li>
  <li>not a substitute for <code>Expression/</code>, <code>Language/</code>, <code>IR/</code>, or <code>Profiles/</code>,</li>
  <li>not a place where implementation convenience becomes language law,</li>
  <li>not a place where strategy prose silently replaces technical closure,</li>
  <li>not a place where current corpus-version truth is declared,</li>
  <li>not Graiphic's proprietary product implementation plan,</li>
  <li>not a claim that all future phases are already stabilized.</li>
</ul>

<p>
References to runtime, deployment, IDE, or ecosystem maturity in this roadmap describe public specification, conformance, and bounded reference-corridor sequencing.
They do not require Graiphic production runtime or FROG Studio implementation work to be developed inside this public repository.
The public reference runtime material in this repository currently stops at Example <code>15</code>; runtime development for later examples continues in Graiphic's proprietary <code>Graiphic/FROG-Runtime</code> repository unless explicitly promoted later.
</p>

<hr/>

<h2 id="relationship-with-versioning">4. Relationship with Versioning</h2>

<p>
The roadmap and the versioning surface answer different questions and must remain distinct.
</p>

<table>
  <thead>
    <tr>
      <th>Surface</th>
      <th>Main question</th>
      <th>What it must not replace</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>Roadmap/</code></td>
      <td>In what order should FROG be closed?</td>
      <td>Current corpus-version truth or per-surface current-status reporting</td>
    </tr>
    <tr>
      <td><code>Versioning/Readme.md</code></td>
      <td>What is the current published specification corpus version, and what is the cross-version doctrine?</td>
      <td>Closure sequencing</td>
    </tr>
    <tr>
      <td><code>Versioning/Matrix.md</code></td>
      <td>What is the current detailed status of each major repository surface?</td>
      <td>Milestone ordering or long-term planning logic</td>
    </tr>
  </tbody>
</table>

<p>
This means:
</p>

<ul>
  <li>the roadmap states <strong>what should be closed next</strong>,</li>
  <li>the versioning surface states <strong>what is currently published and how version truth is governed</strong>.</li>
</ul>

<p>
When a reader wants to know the current published corpus version or the current detailed repository-wide status table, the authoritative repository-visible entry points are:
</p>

<ul>
  <li><code>Versioning/Readme.md</code>,</li>
  <li><code>Versioning/Matrix.md</code>.</li>
</ul>

<p>
This roadmap may refer to those files, but it must not duplicate their role.
</p>

<hr/>

<h2 id="status-legend">5. Status legend</h2>

<ul>
  <li><strong>[x]</strong> completed or already established in the published baseline</li>
  <li><strong>[~]</strong> partially established, active, or not yet fully closed</li>
  <li><strong>[ ]</strong> planned future work</li>
</ul>

<hr/>

<h2 id="guiding-principles">6. Guiding principles</h2>

<ul>
  <li>Keep the architectural boundaries explicit.</li>
  <li>Prefer small coherent closures over large speculative rewrites.</li>
  <li>Do not let reference implementation convenience become hidden language law.</li>
  <li>Do not collapse open FROG IR into one backend-specific or runtime-private form.</li>
  <li>Do not confuse backend family, target profile, deployment mode, and runtime-private realization.</li>
  <li>Do not confuse roadmap planning with normative ownership.</li>
  <li>Do not confuse roadmap planning with current corpus-version reporting.</li>
  <li>Keep the path from canonical <code>.frog</code> source to deployable execution explicit and inspectable.</li>
  <li>Use conformance as a public truth surface, not as commentary only.</li>
  <li>Keep source-shape/schema closure distinct from later semantic validation.</li>
  <li>Only widen the ecosystem after enough closure exists to support that widening cleanly.</li>
  <li>Keep strategic claims aligned with repository-visible proof.</li>
  <li>Keep AI-era auditability claims tied to real architectural closure rather than slogans.</li>
  <li>Keep versioning governance centralized in <code>Versioning/</code> rather than scattering it into planning documents.</li>
</ul>

<p>
What the project must preserve:
</p>

<pre><code>saved source           -&gt; Expression
validated meaning      -&gt; Language
derived execution form -&gt; IR
backend specialization -&gt; Lowering
consumable handoff     -&gt; Backend contract
private realization    -&gt; Runtime / backend
authoring + debugging  -&gt; IDE
closure sequencing     -&gt; Roadmap
current version truth  -&gt; Versioning
strategic purpose      -&gt; Strategy
</code></pre>

<hr/>

<h2 id="current-published-baseline">7. Current published baseline</h2>

<p>
The project is no longer only conceptual.
A meaningful public foundation already exists.
</p>

<h3>Already established</h3>

<ul>
  <li>[x] FROG is explicitly structured as an open graphical dataflow language rather than a product-bound environment.</li>
  <li>[x] The six core specification families exist: <code>Expression</code>, <code>Language</code>, <code>IR</code>, <code>Libraries</code>, <code>Profiles</code>, <code>IDE</code>.</li>
  <li>[x] The repository contains support areas: <code>Examples/</code>, <code>Conformance/</code>, and <code>Implementations/Reference/</code>.</li>
  <li>[x] A centralized version-governance surface exists through <code>Versioning/Readme.md</code> and <code>Versioning/Matrix.md</code>.</li>
  <li>[x] The first published executable slices already exist: <code>01_pure_addition</code>, <code>02_ui_value_roundtrip</code>, <code>03_ui_property_write</code>, <code>04_stateful_feedback_delay</code>.</li>
  <li>[x] The reference implementation posture is explicitly non-normative.</li>
  <li>[x] The distinction between public interface, front panel, and diagram is already explicit.</li>
  <li>[x] The distinction between <code>widget_value</code> and <code>widget_reference</code> is already explicit.</li>
  <li>[x] The strategic framing layer already exists through <code>Strategy/Heilmeier/</code>.</li>
</ul>

<h3>Already visible but still under closure pressure</h3>

<ul>
  <li>[~] Canonical source and source-schema posture are substantially visible but still need continued tightening.</li>
  <li>[~] Language-semantics closure is already meaningful but still being consolidated.</li>
  <li>[~] IR, lowering, and backend-contract posture are already repository-visible but still part of an active closure front.</li>
  <li>[~] Bounded execution-start and reference-path proof are real, but still intentionally narrow.</li>
  <li>[~] Widget-object closure is already serious, but still a growth front.</li>
  <li>[~] The current corpus version is centrally described, but broader stabilization of the full published subset is still progressing.</li>
</ul>

<p>
This roadmap should therefore be read as the sequencing layer for a repository that already has real published structure, not as a plan for a repository that is still empty or purely hypothetical.
</p>

<hr/>

<h2 id="priority-order">8. Priority order</h2>

<p>
The current closure order should remain biased toward corridor coherence before broad ecosystem expansion.
</p>

<ol>
  <li>keep the repository-level entry surfaces aligned with the real published state,</li>
  <li>keep source, semantic, IR, and conformance boundaries mutually coherent,</li>
  <li>keep the bounded execution-start and reference path credible,</li>
  <li>keep widget-object closure coherent across source, libraries, profiles, and execution-facing reasoning,</li>
  <li>keep version-governance centralized and current,</li>
  <li>only then widen backend families, runtime families, deployment surfaces, or broader ecosystem claims.</li>
</ol>

<hr/>

<h2 id="high-level-roadmap">9. High-level roadmap</h2>

<pre><code>Phase 0  -&gt; foundation already established
Phase 1  -&gt; source and semantics closure
Phase 2  -&gt; IR and compiler-corridor closure
Phase 3  -&gt; bounded execution-start and reference-path closure
Phase 4  -&gt; widget-object and UI corridor closure
Phase 5  -&gt; conformance growth and profile broadening
Phase 6  -&gt; ecosystem and industrial-standard position
</code></pre>

<p>
These phases are not independent silos.
They overlap, but they should still be closed in a disciplined order.
</p>

<hr/>

<h2 id="phase-0-foundation-already-established">10. Phase 0 — Foundation already established</h2>

<ul>
  <li>[x] repository landing surface and global architecture framing</li>
  <li>[x] six core specification families</li>
  <li>[x] examples, conformance, and reference-path support surfaces</li>
  <li>[x] strategic framing layer</li>
  <li>[x] roadmap layer</li>
  <li>[x] centralized version-governance layer</li>
</ul>

<p>
Phase 0 is no longer the active frontier.
It is the already-established repository foundation that later work now builds on.
</p>

<hr/>

<h2 id="phase-1-source-and-semantics-closure">11. Phase 1 — Source and semantics closure</h2>

<ul>
  <li>[~] continue tightening the canonical <code>.frog</code> source shape</li>
  <li>[~] continue tightening source-schema posture and structural validation boundaries</li>
  <li>[~] continue tightening the distinction between structural validity and semantic acceptance</li>
  <li>[~] continue tightening public-interface, front-panel, connector, and diagram ownership boundaries</li>
  <li>[~] continue tightening control-structure and state semantics</li>
  <li>[~] continue tightening library-owned versus profile-owned behavior boundaries</li>
</ul>

<p>
Phase 1 is successful when the repository exposes a clean source-to-meaning boundary for the bounded published subset without relying on implementation folklore.
</p>

<hr/>

<h2 id="phase-2-ir-and-compiler-corridor-closure">12. Phase 2 — IR and compiler-corridor closure</h2>

<ul>
  <li>[~] keep the canonical execution-facing IR explicit and open</li>
  <li>[~] keep derivation rules, construction rules, and identity mapping coherent</li>
  <li>[~] keep canonical JSON IR validation posture explicit where published</li>
  <li>[~] keep lowering architecturally downstream from the open IR</li>
  <li>[~] keep backend-facing contracts explicit and distinct from runtime-private realization</li>
  <li>[~] keep backend-family reasoning bounded and credible</li>
</ul>

<p>
Phase 2 is successful when the repository-visible compiler corridor is explicit enough that the open IR remains clearly upstream from private backend machinery and clearly distinct from one runtime’s internal graph.
</p>

<hr/>

<h2 id="phase-3-bounded-execution-start-and-reference-path-closure">13. Phase 3 — Bounded execution-start and reference-path closure</h2>

<ul>
  <li>[~] keep a first bounded profile-level execution-start contract coherent</li>
  <li>[~] keep named example slices aligned with the actual bounded corridor</li>
  <li>[~] keep conformance material aligned with the bounded executable truth surface</li>
  <li>[~] keep the reference workspace serious but non-normative</li>
  <li>[~] keep executable reference slices inspectable from source to bounded observable execution</li>
</ul>

<p>
Phase 3 is successful when the repository can show a real bounded vertical slice from canonical source through derivation and backend-facing preparation to an observable execution path without pretending that all later runtime families are already standardized.
</p>

<hr/>

<h2 id="phase-4-widget-object-and-ui-corridor-closure">14. Phase 4 — Widget-object and UI corridor closure</h2>

<ul>
  <li>[~] keep front-panel declaration distinct from diagram interaction</li>
  <li>[~] keep <code>widget_value</code> and <code>widget_reference</code> distinct</li>
  <li>[~] keep widget instance and widget class distinct</li>
  <li>[~] keep property read, property write, method invocation, and event exposure distinct</li>
  <li>[~] keep source-level declaration and IDE-synthesized node surfaces distinct</li>
  <li>[~] keep standardized widget-class contracts distinct from runtime-private or toolkit-private reflection mechanisms</li>
</ul>

<p>
Phase 4 is successful when the published repository exposes a coherent UI-object corridor serious enough to support long-term conforming IDE behavior, validator behavior, and runtime-host reasoning without turning one UI host into hidden normative law.
</p>

<hr/>

<h2 id="phase-5-conformance-growth-and-profile-broadening">15. Phase 5 — Conformance growth and profile broadening</h2>

<ul>
  <li>[ ] expand conformance families once the currently published corridor is stable enough</li>
  <li>[ ] broaden capability-family coverage only when ownership boundaries remain clear</li>
  <li>[ ] refine broader backend-family, runtime-family, or deployment-family distinctions where justified</li>
  <li>[ ] widen interop and profile surfaces without collapsing them into the intrinsic core</li>
  <li>[ ] grow preservability and degraded readability expectations where cross-version safety can be made explicit</li>
</ul>

<p>
Phase 5 must not start by scattering unfinished claims.
It should widen only from a sufficiently stable bounded core.
</p>

<hr/>

<h2 id="phase-6-ecosystem-and-industrial-standard-position">16. Phase 6 — Ecosystem and industrial-standard position</h2>

<ul>
  <li>[ ] credible multi-implementation ecosystem posture</li>
  <li>[ ] broader profile ecosystem credibility</li>
  <li>[ ] serious deployment and runtime-family maturity</li>
  <li>[ ] stronger industrial-standard posture</li>
  <li>[ ] clearer conformance, certification, and branding ecosystem separation</li>
</ul>

<p>
Phase 6 is not a marketing phase.
It is the point where enough repository-visible closure exists to sustain a credible long-term open industrial language ecosystem.
</p>

<hr/>

<h2 id="cross-cutting-workstreams">17. Cross-cutting workstreams</h2>

<p>
Source provenance and human-review attestations are a cross-cutting workstream across <code>Expression/</code>, <code>IDE/</code>, <code>Conformance/</code>, <code>Versioning/</code>, and later optional deployment policy profiles. The immediate objective is not to block execution by default, but to make authoring origin, AI participation, import status, review acceptance, stale evidence, and untrusted issuers visible without corrupting source semantics.
</p>

<p>
Some workstreams cut across several phases and must remain continuously aligned:
</p>

<ul>
  <li>repository-entry alignment,</li>
  <li>example / conformance / reference triangulation,</li>
  <li>identity and mapping continuity,</li>
  <li>execution observability and debugging posture,</li>
  <li>widget-object ownership clarity,</li>
  <li>versioning centralization and freshness,</li>
  <li>strategy-to-proof coherence,</li>
  <li>anti-regression documentation discipline.</li>
</ul>

<hr/>

<h2 id="milestone-navigation">18. Milestone navigation</h2>

<p>
Compact milestone tracking is maintained in:
</p>

<pre><code>Roadmap/Milestones.md</code></pre>

<p>
Milestones should remain a compact navigation aid for closure progress.
They should not become a duplicate version-status matrix.
Detailed current repository-surface status belongs centrally in:
</p>

<ul>
  <li><code>Versioning/Readme.md</code>,</li>
  <li><code>Versioning/Matrix.md</code>.</li>
</ul>

<hr/>

<h2 id="definition-of-success">19. Definition of success</h2>

<p>
This roadmap is successful when it helps the repository close in a disciplined order without blurring the distinction between:
</p>

<ul>
  <li>what FROG is,</li>
  <li>what is already published,</li>
  <li>what version state the repository is currently in,</li>
  <li>what should be closed next,</li>
  <li>what remains longer-term ecosystem work.</li>
</ul>

<p>
The roadmap should therefore remain:
</p>

<ul>
  <li>non-normative,</li>
  <li>sequencing-oriented,</li>
  <li>architecturally disciplined,</li>
  <li>explicitly distinct from the version-governance surface,</li>
  <li>explicitly distinct from strategy,</li>
  <li>explicitly distinct from technical ownership layers.</li>
</ul>

<p>
FROG succeeds as a project when the repository can sustain a durable path from canonical source to validated meaning, open execution-facing IR, backend handoff, bounded observable execution, rich widget-object closure, conformance growth, and eventually a credible multi-implementation open industrial ecosystem.
</p>
