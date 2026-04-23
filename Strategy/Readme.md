<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">🐸 FROG Strategy</h1>

<p align="center">
  Non-normative strategic framing layer for the long-term purpose, positioning, ecosystem logic, and industrial relevance of FROG<br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<p align="center">
  FROG is not only a graphical language initiative. It is a strategic attempt to open the foundational language layer of executable graphical programming.
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#why-this-directory-exists">1. Why this Directory Exists</a></li>
  <li><a href="#what-strategy-owns">2. What Strategy Owns</a></li>
  <li><a href="#what-strategy-does-not-own">3. What Strategy Does Not Own</a></li>
  <li><a href="#why-frog-needs-a-strategic-layer">4. Why FROG Needs a Strategic Layer</a></li>
  <li><a href="#what-makes-frog-strategically-different">5. What Makes FROG Strategically Different</a></li>
  <li><a href="#frog-in-the-ai-era">6. FROG in the AI Era</a></li>
  <li><a href="#industrial-security-and-technological-sovereignty">7. Industrial Security and Technological Sovereignty</a></li>
  <li><a href="#hardware-agnosticism-and-open-fir-modularity">8. Hardware Agnosticism and Open FIR Modularity</a></li>
  <li><a href="#relation-with-the-rest-of-the-repository">9. Relation with the Rest of the Repository</a></li>
  <li><a href="#relation-with-roadmap-and-versioning">10. Relation with Roadmap and Versioning</a></li>
  <li><a href="#current-strategic-entry-point">11. Current Strategic Entry Point</a></li>
  <li><a href="#strategic-boundaries-to-preserve">12. Strategic Boundaries to Preserve</a></li>
  <li><a href="#summary">13. Summary</a></li>
</ul>

<hr/>

<h2 id="why-this-directory-exists">1. Why this Directory Exists</h2>

<p>
This directory exists to hold the <strong>strategic framing layer</strong> of FROG.
It explains why the project matters, what category gap it targets, why that gap is important now, and what long-term ecosystem consequences would follow if FROG succeeds.
</p>

<p>
This layer is intentionally <strong>non-normative</strong>.
It does not define language truth.
It does not define source validity.
It does not define execution semantics.
It does not define IR derivation rules.
It does not define current corpus-version truth.
</p>

<p>
Its purpose is to make the larger logic of the project explicit:
why an open graphical language matters,
why the repository is structured the way it is,
why the project should be understood as more than a local tooling exercise,
and why the category itself needs to change.
</p>

<p>
That category-level change is the central strategic point.
FROG is not only trying to make graphical programming more convenient.
It is trying to make executable graphical programming more open, more inspectable, more hardware-agnostic, more modular, and more governable than historical vendor-bound graphical ecosystems.
</p>

<hr/>

<h2 id="what-strategy-owns">2. What Strategy Owns</h2>

<p>
The Strategy layer owns repository-visible documents whose role is to explain:
</p>

<ul>
  <li>the strategic problem FROG addresses,</li>
  <li>the ecosystem gap FROG targets,</li>
  <li>the expected impact of success,</li>
  <li>the relevance of openness, inspectability, portability, modularity, and auditability,</li>
  <li>the long-term industrial and technological significance of the project,</li>
  <li>the role of open FIR as a public bridge surface toward runtime families, compiler families, and hardware ecosystems,</li>
  <li>the framing used for external explanation, funding logic, ecosystem communication, or mission-level positioning.</li>
</ul>

<p>
This is the layer where FROG can explain not only <strong>what it is</strong>, but also <strong>why it matters</strong>.
</p>

<p>
The strongest strategic reading of FROG is not “another graphical editor”.
The strongest strategic reading is that FROG tries to open the language and execution-facing bridge layer underneath graphical system programming.
</p>

<hr/>

<h2 id="what-strategy-does-not-own">3. What Strategy Does Not Own</h2>

<p>
The Strategy layer does <strong>not</strong> own:
</p>

<ul>
  <li>canonical source representation,</li>
  <li>source-shape/schema posture,</li>
  <li>structural validity,</li>
  <li>validated language meaning,</li>
  <li>intrinsic primitive behavior,</li>
  <li>profile-owned capability behavior,</li>
  <li>canonical execution-facing IR definition,</li>
  <li>lowering rules,</li>
  <li>backend contract rules,</li>
  <li>conformance expectations,</li>
  <li>reference implementation behavior,</li>
  <li>roadmap sequencing,</li>
  <li>current specification corpus version and current detailed repository-surface status.</li>
</ul>

<p>
Those ownership boundaries remain elsewhere in the repository:
</p>

<pre><code>Expression/                 -&gt; canonical source and structural validity
Language/                   -&gt; validated program meaning
Libraries/                  -&gt; intrinsic primitive surface
Profiles/                   -&gt; optional standardized capability families
IR/                         -&gt; canonical open execution-facing representation
Conformance/                -&gt; public accept / reject / preserve expectations
Implementations/Reference/  -&gt; non-normative executable workspace
Roadmap/                    -&gt; sequencing and closure order
Versioning/                 -&gt; corpus-version governance and current-status reporting</code></pre>

<p>
Strategy may explain why those layers matter,
but it must not silently replace them.
</p>

<p>
This separation is itself strategic.
FROG’s credibility depends on not collapsing vision, roadmap, version status, normative law, and reference implementation into one ambiguous document layer.
</p>

<hr/>

<h2 id="why-frog-needs-a-strategic-layer">4. Why FROG Needs a Strategic Layer</h2>

<p>
FROG is not merely a technical document set.
It is an attempt to open a category that has historically remained fragmented, opaque, and too tightly coupled to proprietary execution ecosystems.
</p>

<p>
A strategic layer is therefore needed because the project has to explain more than local technical design choices.
It has to explain:
</p>

<ul>
  <li>why open graphical programming matters,</li>
  <li>why graphical system programming should not remain vendor-bound by default,</li>
  <li>why inspectable source and inspectable execution-facing artifacts matter,</li>
  <li>why a public FIR bridge surface matters for hardware portability, runtime modularity, and compiler modularity,</li>
  <li>why the repository is built as a layered language stack rather than as one monolithic product,</li>
  <li>why the ecosystem significance of FROG is larger than one implementation.</li>
</ul>

<p>
Without this strategic framing, the repository could be misread as:
</p>

<ul>
  <li>just another graphical editor idea,</li>
  <li>just a runtime experiment,</li>
  <li>just an implementation workspace,</li>
  <li>just a niche format proposal,</li>
  <li>or just an open-source imitation of historical graphical tools.</li>
</ul>

<p>
That would be an incomplete reading.
FROG is trying to establish an open language basis for graphical system programming that can support future validators, IDEs, runtimes, compilers, profiles, conformance growth, and industrial ecosystem participation.
</p>

<p>
The strategic claim is therefore broader:
FROG attacks the structural lock-in of graphical industrial programming by opening the language layer itself:
source, semantics, FIR, lowering, backend contracts, runtime-family boundaries, and compiler-family boundaries.
</p>

<hr/>

<h2 id="what-makes-frog-strategically-different">5. What Makes FROG Strategically Different</h2>

<p>
The strategic novelty of FROG does not come from any single isolated property.
Graphical programming exists.
Dataflow programming exists.
Multi-target execution exists.
Intermediate representations exist.
AI-assisted tooling exists.
</p>

<p>
The strategic novelty of FROG is the way these ideas are combined into one open graphical language architecture.
</p>

<p>
FROG is strategically different because it combines:
</p>

<ul>
  <li><strong>graphical executable structure</strong> — computation is represented as explicit dataflow graphs rather than syntax-first instruction sequences,</li>
  <li><strong>open canonical source</strong> — the saved program is intended to remain structured, inspectable, machine-friendly, and independent from one IDE product,</li>
  <li><strong>open execution-facing FIR</strong> — the intermediate execution-facing representation is not treated as a hidden backend-private artifact,</li>
  <li><strong>runtime/compiler modularity</strong> — runtime families and compiler families are downstream consumers rather than definitions of the language,</li>
  <li><strong>hardware agnosticism</strong> — the language aims to remain upstream from any one processor family, operating system, device class, or vendor stack,</li>
  <li><strong>AI-era reviewability</strong> — the same structured artifacts that help machines generate and transform programs can also help humans inspect and audit them,</li>
  <li><strong>security through reduced opacity</strong> — the project’s credible security angle is traceability and auditability, not a claim that graphical form automatically guarantees safety.</li>
</ul>

<p>
The resulting strategic proposition is:
</p>

<blockquote>
  <p>
  FROG is an attempt to open the foundational language layer of executable graphical programming, with an inspectable FIR able to serve as a bridge toward multiple runtimes, compilers, and hardware targets.
  </p>
</blockquote>

<p>
That is the category-level difference.
FROG should not be reduced to an IDE, a runtime, a compiler target, a UI layer, or one reference implementation.
Its strategic value is precisely that those downstream layers can remain modular while the upstream language basis stays open and inspectable.
</p>

<hr/>

<h2 id="frog-in-the-ai-era">6. FROG in the AI Era</h2>

<p>
FROG matters even more in the AI era than it would have in a purely manual-programming era.
</p>

<p>
Software artifacts are increasingly:
</p>

<ul>
  <li>generated,</li>
  <li>rewritten,</li>
  <li>transformed,</li>
  <li>assisted,</li>
  <li>refactored,</li>
  <li>explained,</li>
  <li>reviewed,</li>
  <li>validated.</li>
</ul>

<p>
through AI-assisted tooling or AI-adjacent pipelines.
</p>

<p>
That changes the strategic value of representation.
A serious language infrastructure now needs to be:
</p>

<ul>
  <li>structured enough for machine generation and transformation,</li>
  <li>explicit enough for human review,</li>
  <li>open enough for independent tooling,</li>
  <li>layered enough to preserve meaning across validation and derivation stages,</li>
  <li>stable enough to remain governable across version evolution.</li>
</ul>

<p>
FROG is well aligned with that requirement because:
</p>

<ul>
  <li>the canonical <code>.frog</code> source is structured JSON,</li>
  <li>the primary program structure is graphically reviewable,</li>
  <li>the execution-facing FIR remains open and inspectable,</li>
  <li>the backend/compiler path remains downstream rather than becoming hidden language truth,</li>
  <li>the runtime path remains downstream rather than becoming hidden language truth,</li>
  <li>the repository centralizes corpus-version posture and version doctrine rather than scattering it across unrelated documents.</li>
</ul>

<p>
This does not mean that textual languages cannot be audited.
It means that FROG is designed to make structural review more direct by keeping the executable graph explicit across the language stack.
</p>

<p>
That is a strategic advantage, not just a cosmetic one.
It reduces the auditability gap between:
</p>

<ul>
  <li>what was generated,</li>
  <li>what was saved,</li>
  <li>what was validated,</li>
  <li>what was derived,</li>
  <li>what is handed downstream,</li>
  <li>what is executed or compiled,</li>
  <li>and what current version posture the published repository actually claims.</li>
</ul>

<p>
For AI-assisted development, this is critical.
The more software is generated or transformed by tools, the more important it becomes to preserve inspectable structure across every stage of the pipeline.
FROG is designed for that world.
</p>

<hr/>

<h2 id="industrial-security-and-technological-sovereignty">7. Industrial Security and Technological Sovereignty</h2>

<p>
FROG is also strategically relevant because industrial systems increasingly depend on software that must remain:
</p>

<ul>
  <li>reviewable,</li>
  <li>portable,</li>
  <li>traceable,</li>
  <li>durable,</li>
  <li>vendor-independent enough to remain governable over time.</li>
</ul>

<p>
That matters for industrial security.
Critical logic should not have to disappear into opaque saved formats, opaque private execution layers, or one vendor-defined tooling stack in order to remain usable.
</p>

<p>
That also matters for technological sovereignty.
A language ecosystem is more sovereign when:
</p>

<ul>
  <li>the source representation is open,</li>
  <li>the execution-facing representation remains inspectable,</li>
  <li>multiple actors can implement compatible tooling,</li>
  <li>compiler, runtime, IDE, and backend paths are not collapsed into one private authority,</li>
  <li>version posture and transition doctrine remain publicly readable rather than hidden in one vendor lifecycle.</li>
</ul>

<p>
FROG therefore has strategic significance beyond developer ergonomics.
It can support an open, inspectable, portable, and governable programming foundation for industrial and strategic software ecosystems.
</p>

<p>
The security claim must remain disciplined.
FROG should not claim that graphical form automatically guarantees security.
Its stronger and more credible claim is that open source artifacts, explicit graph structure, inspectable FIR, explicit backend contracts, and readable governance reduce structural opacity and make review more direct.
</p>

<hr/>

<h2 id="hardware-agnosticism-and-open-fir-modularity">8. Hardware Agnosticism and Open FIR Modularity</h2>

<p>
FROG’s hardware-agnostic ambition is not only a portability slogan.
It is tied to the architectural role of FIR.
</p>

<p>
The open FIR is the intended bridge surface between the upstream graphical language and downstream execution families.
Because that bridge surface is public and inspectable, downstream implementations can target different execution strategies without redefining the language itself.
</p>

<pre><code>canonical .frog source
        |
        v
validated program meaning
        |
        v
open execution-facing FIR
        |
        +-----------------------------+-----------------------------+
        |                             |                             |
        v                             v                             v
runtime-family bridge        compiler-family bridge        hybrid bridge
        |                             |                             |
        v                             v                             v
operational runtimes          native artifacts              target-specific mixes
Python / Rust / C++           LLVM / vendor compilers       runtime + compiled execution
        |                             |                             |
        +-----------------------------+-----------------------------+
                                      |
                                      v
                         heterogeneous hardware targets</code></pre>

<p>
This is why FROG can credibly speak about hardware agnosticism and hyper-modularity.
The strategic idea is not that one runtime magically runs everywhere.
The idea is that one open graphical language stack can preserve an inspectable execution-facing representation from which multiple runtime, compiler, and hardware bridges can be built.
</p>

<p>
This resembles the role that durable intermediate and system-level representations have played in other ecosystems:
not because FROG is the same kind of language as C,
but because FROG aims to create a stable upstream layer from which many downstream hardware strategies can be reached.
</p>

<p>
That distinction is essential.
FROG should not overclaim that every target is already supported.
It should claim, more precisely, that its architecture is designed so that target-specific support can be added without turning any one target into the definition of the language.
</p>

<hr/>

<h2 id="relation-with-the-rest-of-the-repository">9. Relation with the Rest of the Repository</h2>

<p>
The Strategy layer should be read together with, but distinctly from, the other repository layers.
</p>

<pre><code>Readme.md                   -&gt; repository-wide architectural entry point
Strategy/                   -&gt; why the project matters
Roadmap/                    -&gt; in what order closure should happen
Versioning/                 -&gt; what the current corpus version is and how version transitions are governed
Expression/                 -&gt; what is saved and structurally valid
Language/                   -&gt; what validated programs mean
IR/                         -&gt; what validated meaning becomes in execution-facing form
Conformance/                -&gt; what should be accepted, rejected, and preserved
Implementations/Reference/  -&gt; how a bounded subset is currently exercised</code></pre>

<p>
This separation matters because FROG needs:
</p>

<ul>
  <li>architecture,</li>
  <li>strategy,</li>
  <li>roadmap,</li>
  <li>versioning,</li>
  <li>conformance,</li>
  <li>reference proof</li>
</ul>

<p>
without allowing any one of those layers to silently replace the others.
</p>

<p>
The Strategy layer can explain why Example 05 matters as a frozen bounded corridor, why the runtime-family acceptance path matters, and why LLVM proof paths are strategically relevant.
It must still leave the actual current status of those surfaces to the owning documentation and to <code>Versioning/</code>.
</p>

<hr/>

<h2 id="relation-with-roadmap-and-versioning">10. Relation with Roadmap and Versioning</h2>

<p>
The repository contains three distinct repository-wide framing and governance surfaces that answer three different questions:
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
      <td>What is the current published specification corpus version, what is the additive-evolution doctrine, and what is the current detailed per-surface status?</td>
      <td>Normative technical ownership, strategic rationale, or milestone sequencing</td>
    </tr>
  </tbody>
</table>

<p>
This means:
</p>

<ul>
  <li>Strategy explains the <strong>why</strong>,</li>
  <li>Roadmap explains the <strong>next</strong>,</li>
  <li>Versioning explains the <strong>current published version posture</strong>.</li>
</ul>

<p>
The strategic layer may refer to the current published baseline, but it should not become the authoritative source for the current corpus version or for the detailed status of repository surfaces.
When that information is needed, the authoritative repository-visible entry points are:
</p>

<ul>
  <li><code>Versioning/Readme.md</code>,</li>
  <li><code>Versioning/Matrix.md</code>.</li>
</ul>

<hr/>

<h2 id="current-strategic-entry-point">11. Current Strategic Entry Point</h2>

<p>
At the current published repository state, the main strategic framing document is:
</p>

<pre><code>Strategy/Heilmeier/Readme.md</code></pre>

<p>
That document explains the technological purpose, gap, expected impact, risks, and long-term program logic of FROG in a structured mission-oriented form.
</p>

<p>
The role of this directory-level <code>Readme.md</code> is different.
It serves as the strategic entry point for the <code>Strategy/</code> layer as a whole and clarifies how strategic framing should coexist with the normative specification, the roadmap, and the centralized versioning surface.
</p>

<p>
Together, these strategy documents should make one message unavoidable:
FROG is not only a graphical programming proposal.
It is an attempt to open the language and bridge infrastructure underneath executable graphical programming.
</p>

<hr/>

<h2 id="strategic-boundaries-to-preserve">12. Strategic Boundaries to Preserve</h2>

<p>
The Strategy layer should preserve the following distinctions:
</p>

<ul>
  <li>strategy vs normative specification,</li>
  <li>strategy vs roadmap sequencing,</li>
  <li>strategy vs version-governance truth,</li>
  <li>strategic motivation vs semantic ownership,</li>
  <li>AI compatibility vs AI dependence,</li>
  <li>auditability improvement vs exaggerated security claims,</li>
  <li>hardware agnosticism vs claims of already supporting every hardware target,</li>
  <li>open FIR as a bridge surface vs one backend becoming language truth,</li>
  <li>open language vs one implementation,</li>
  <li>technological sovereignty vs branding control.</li>
</ul>

<p>
In particular:
</p>

<ul>
  <li>FROG should not claim that graphical form automatically guarantees security,</li>
  <li>FROG should not claim that textual languages cannot be reviewed,</li>
  <li>FROG should not claim that all runtime and compiler targets are already complete,</li>
  <li>FROG should not claim that strategy prose is a substitute for current repository status reporting,</li>
  <li>FROG should claim, more narrowly and more credibly, that its architecture reduces opacity by combining structured source, explicit graph reviewability, inspectable execution-facing representation, open downstream bridge boundaries, and centralized readable version posture.</li>
</ul>

<p>
This discipline makes the strategic claim stronger.
FROG does not need exaggerated promises to be ambitious.
Its real ambition is already large: opening the foundational layer of executable graphical programming.
</p>

<hr/>

<h2 id="summary">13. Summary</h2>

<p>
The <code>Strategy/</code> layer exists to explain why FROG matters as a long-term technological program.
</p>

<p>
Its role is to make explicit that FROG is not only:
</p>

<ul>
  <li>an open graphical language,</li>
  <li>a dataflow architecture,</li>
  <li>a compiler/runtime preparation effort.</li>
</ul>

<p>
It is also:
</p>

<ul>
  <li>a response to opaque graphical ecosystems,</li>
  <li>a response to vendor-bound industrial programming stacks,</li>
  <li>a response to AI-era auditability needs,</li>
  <li>a response to industrial reviewability requirements,</li>
  <li>a response to technological sovereignty concerns in strategic software systems,</li>
  <li>and a proposal for an open graphical language layer whose FIR can become a public bridge surface toward multiple runtimes, compilers, and hardware targets.</li>
</ul>

<p>
That is why the Strategy layer exists, and why it must remain explicit, non-normative, clearly separated from the specification layers, clearly separated from roadmap sequencing, and clearly separated from centralized corpus-version reporting.
</p>

<p>
The strategic message should remain simple:
</p>

<blockquote>
  <p>
  FROG is an open attempt to make executable graphical programming inspectable, hardware-agnostic, AI-compatible, and modular across runtime and compiler families by opening the language layer and its FIR bridge surface.
  </p>
</blockquote>
