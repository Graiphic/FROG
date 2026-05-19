<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Source / FIR / Lowering Conformance Matrix</h1>

<p align="center">
  <strong>Public staged matrix for validating source, FIR, lowering, backend contracts, manifests, providers, and runtime-consumption boundaries</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This matrix defines how conformance cases should prove the public execution
corridor without turning one compiler, runtime, provider, or deployment product
into the definition of FROG.
</p>

<pre><code>.frog source
   -&gt; structural validity
   -&gt; semantic validation
   -&gt; canonical Execution IR / FIR
   -&gt; lowering eligibility
   -&gt; backend contract or manifest-compatible handoff
   -&gt; provider / host capability resolution
   -&gt; runtime, launcher, package, or backend-family consumption</code></pre>

<p>
The source-to-FIR matrix remains the focused matrix for canonical source and
FIR derivation. This document extends the public conformance view across the
downstream corridor introduced by lowering and backend contracts.
</p>

<hr/>

<h2>Boundary Rule</h2>

<pre><code>Conformance checks staged ownership.
It does not invent missing language law.
It does not expose Graiphic private runtime implementation.</code></pre>

<p>
Each case must identify the stage that owns the expected result. A case that
fails at semantic validation must not proceed to FIR. A case that fails at
lowering must not be described as invalid source. A case that fails provider
resolution must not redefine the public FIR call identity.
</p>

<hr/>

<h2>Outcome Vocabulary</h2>

<table>
  <thead>
    <tr>
      <th>Outcome</th>
      <th>Stage</th>
      <th>Meaning</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>accept_source</code></td>
      <td>Source</td>
      <td>The file is loadable and structurally valid canonical <code>.frog</code> source.</td>
    </tr>
    <tr>
      <td><code>reject_source</code></td>
      <td>Source</td>
      <td>The file is malformed or structurally invalid before semantic validation.</td>
    </tr>
    <tr>
      <td><code>accept_meaning</code></td>
      <td>Language</td>
      <td>Semantic validation succeeds and validated program meaning exists.</td>
    </tr>
    <tr>
      <td><code>reject_meaning</code></td>
      <td>Language</td>
      <td>Source is structurally valid but semantically invalid.</td>
    </tr>
    <tr>
      <td><code>derive_fir</code></td>
      <td>IR</td>
      <td>Validated meaning derives to canonical Execution IR / FIR.</td>
    </tr>
    <tr>
      <td><code>reject_fir_preservation</code></td>
      <td>IR</td>
      <td>FIR exists but lost required source, type, call, state, widget, or attribution distinctions.</td>
    </tr>
    <tr>
      <td><code>lowerable</code></td>
      <td>Lowering</td>
      <td>The FIR shape has enough explicit meaning and requirements for target-oriented lowering.</td>
    </tr>
    <tr>
      <td><code>reject_lowering</code></td>
      <td>Lowering</td>
      <td>FIR is valid, but the selected lowering profile cannot preserve required semantics or requirements.</td>
    </tr>
    <tr>
      <td><code>emit_contract</code></td>
      <td>Backend contract</td>
      <td>The lowered form can emit a consumer-facing backend contract or manifest-compatible handoff.</td>
    </tr>
    <tr>
      <td><code>reject_contract</code></td>
      <td>Backend contract</td>
      <td>The backend handoff would be ambiguous, incomplete, or incompatible with required assumptions.</td>
    </tr>
    <tr>
      <td><code>resolve_provider</code></td>
      <td>Manifest / provider</td>
      <td>Declared providers, host capabilities, ABI profiles, and dependencies are satisfied for the selected posture.</td>
    </tr>
    <tr>
      <td><code>reject_provider</code></td>
      <td>Manifest / provider</td>
      <td>A required provider, host capability, ABI profile, artifact, or dependency cannot be satisfied.</td>
    </tr>
    <tr>
      <td><code>consume_artifact</code></td>
      <td>Runtime / launcher / backend consumer</td>
      <td>A consumer can validate and consume the declared contract and artifacts without guessing.</td>
    </tr>
    <tr>
      <td><code>reject_consumption</code></td>
      <td>Runtime / launcher / backend consumer</td>
      <td>The consumer rejects explicitly because declared requirements cannot be honored.</td>
    </tr>
    <tr>
      <td><code>unsupported_subset</code></td>
      <td>Implementation</td>
      <td>The construct may be valid FROG, but the implementation does not support it yet and must not invent guessed behavior.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Stage Ownership Matrix</h2>

<table>
  <thead>
    <tr>
      <th>Stage</th>
      <th>Owns conformance check</th>
      <th>Must not be confused with</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Source loadability</td>
      <td>Serialization, parseability, required file envelope.</td>
      <td>Semantic validity.</td>
    </tr>
    <tr>
      <td>Source structural validity</td>
      <td>Required sections, field shape, stable ids, widget instance shape, package reference shape.</td>
      <td>Diagram meaning or runtime execution.</td>
    </tr>
    <tr>
      <td>Semantic validation</td>
      <td>Types, legal nodes, legal edges, structures, widget bindings, library calls, state legality, effect/status rules.</td>
      <td>FIR construction.</td>
    </tr>
    <tr>
      <td>FIR derivation</td>
      <td>Execution-facing objects, typed ports, connections, regions, state, widget value/reference distinction, call identity, source attribution.</td>
      <td>Lowered storage layout or native ABI.</td>
    </tr>
    <tr>
      <td>FIR structural validation</td>
      <td>Canonical JSON IR shape and schema-compatible structure where applicable.</td>
      <td>Proof that semantic derivation was correct by itself.</td>
    </tr>
    <tr>
      <td>Lowering</td>
      <td>Target-oriented specialization, partitioning, state/storage representation, call boundaries, provider/capability boundaries, dependency closure planning.</td>
      <td>New source meaning or private runtime truth.</td>
    </tr>
    <tr>
      <td>Backend contract</td>
      <td>Consumer-facing handoff: artifact kinds, target profile, ABI profile, entrypoints, lifecycle hooks, provider slots, host capabilities, diagnostics.</td>
      <td>FIR identity or private loader implementation.</td>
    </tr>
    <tr>
      <td>Manifest / provider resolution</td>
      <td>Package-relative artifacts, checksums, dependency closure, provider satisfaction, host capability availability.</td>
      <td>Language semantics or public FIR call names.</td>
    </tr>
    <tr>
      <td>Runtime / launcher / backend consumption</td>
      <td>Declared contract validation, artifact binding, value marshalling, state/status handling, UI snapshot behavior where relevant.</td>
      <td>LLVM identity, hidden provider behavior, or unlisted runtime assumptions.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Positive Case Families</h2>

<table>
  <thead>
    <tr>
      <th>Case family</th>
      <th>Source expectation</th>
      <th>FIR expectation</th>
      <th>Lowering/backend expectation</th>
      <th>Manifest/provider expectation</th>
      <th>Runtime-visible expectation</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Pure core primitive</td>
      <td>Valid diagram primitive such as <code>frog.core.add</code> with typed inputs and output.</td>
      <td>Primitive object preserves <code>frog.core.add</code>, typed ports, connections, and attribution.</td>
      <td>May lower directly to backend operations without provider dependency.</td>
      <td>No provider requirement unless the selected backend itself declares one.</td>
      <td>Published output equals the computed value.</td>
    </tr>
    <tr>
      <td>Boolean, string, enum, and path widget value roundtrip</td>
      <td>Valid control and indicator widget instances with source-owned bindings and initial values.</td>
      <td>FIR preserves widget identity, value type, direction, and binding source as <code>widget_value</code> participation.</td>
      <td>Lowering keeps front-panel value boundary distinct from pure program logic.</td>
      <td>Host/UI capability requirements are explicit when the selected posture needs a front-panel host.</td>
      <td>UI snapshot reflects source-owned value state and program output without hardcoded widget visuals.</td>
    </tr>
    <tr>
      <td>Explicit state / delay / feedback</td>
      <td>Valid recurrence uses a published explicit state carrier rather than hidden visual feedback.</td>
      <td>FIR preserves explicit state identity, initialization, and read/write or commit boundary.</td>
      <td>Lowering materializes storage or state slots while preserving initialization semantics.</td>
      <td>State representation crossing artifact boundaries is declared.</td>
      <td>Repeated execution produces the expected stateful sequence.</td>
    </tr>
    <tr>
      <td>Widget value versus widget reference</td>
      <td>Source uses natural widget value flow or object/reference access according to the widget class contract.</td>
      <td>FIR keeps <code>widget_value</code> and <code>widget_reference</code> distinct.</td>
      <td>Lowering may route UI-object operations through host services, but must not collapse them into natural value flow.</td>
      <td>Required UI host capability is explicit where object access crosses the backend boundary.</td>
      <td>Runtime-visible state distinguishes value changes from property/method effects.</td>
    </tr>
    <tr>
      <td>Button mechanical action value</td>
      <td>Source owns widget instances, bindings, initial state, labels, and mechanical-action selection where part of source truth.</td>
      <td>FIR preserves the value participation and relevant source state without absorbing SVG visual behavior as execution logic.</td>
      <td>Lowering treats button behavior as declared widget/front-panel value semantics, not as backend-invented state.</td>
      <td>Front-panel host requirements and state snapshot expectations remain explicit.</td>
      <td>Switch and latch behavior is observable through state snapshots according to the selected mechanical action.</td>
    </tr>
    <tr>
      <td>Provider-backed standard library call</td>
      <td>Valid source uses a namespaced <code>frog.xxx</code> call such as <code>frog.image.decode_file_rgba8</code>.</td>
      <td>FIR preserves namespaced call identity, typed ports, effect/status metadata, and abstract provider requirement.</td>
      <td>Lowering emits a provider/capability boundary and dependency-closure requirement without selecting a private implementation as language law.</td>
      <td>Backend contract or manifest declares provider slots, host capability needs, ABI profile, artifacts, and dependency closure where applicable.</td>
      <td>Runtime or launcher consumes declared artifacts and reports decoded output or declared status/diagnostics.</td>
    </tr>
    <tr>
      <td>Generated launcher or packaged executable posture</td>
      <td>Source remains ordinary FROG and does not encode packaging tricks.</td>
      <td>FIR remains backend-agnostic and does not contain artifact filenames or native symbols.</td>
      <td>Lowering identifies the required program scope, providers, host services, assets, and artifact boundaries.</td>
      <td>Manifest-compatible contract declares only the required dependency closure for the selected package.</td>
      <td>Launcher or package starts from the contract and artifacts without needing a full IDE/debug runtime unless declared.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Negative Case Families</h2>

<table>
  <thead>
    <tr>
      <th>Case family</th>
      <th>Expected rejection stage</th>
      <th>Required rejection reason</th>
      <th>Must not happen</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Malformed or structurally invalid source</td>
      <td><code>reject_source</code></td>
      <td>Missing required section, wrong field type, duplicate ids, or invalid widget/package shape.</td>
      <td>No FIR may be emitted from repaired source.</td>
    </tr>
    <tr>
      <td>Semantic type mismatch</td>
      <td><code>reject_meaning</code></td>
      <td>Illegal port connection, wrong value type, unsupported conversion, or invalid terminal direction.</td>
      <td>The implementation must not coerce silently to make a runtime example pass.</td>
    </tr>
    <tr>
      <td>Unknown primitive or library call</td>
      <td><code>reject_meaning</code> or <code>unsupported_subset</code></td>
      <td>The call identity is unknown, not in the active profile, or valid but unsupported by the implementation subset.</td>
      <td>No guessed FIR call object with invented behavior.</td>
    </tr>
    <tr>
      <td>Invalid widget binding</td>
      <td><code>reject_meaning</code></td>
      <td>Unknown widget id, incompatible widget role, invalid value type, illegal indicator write, or invalid control read posture.</td>
      <td>No host overlay or HTML fallback may replace the missing source-owned binding.</td>
    </tr>
    <tr>
      <td>Visual-only behavior dependency</td>
      <td><code>reject_meaning</code> or <code>reject_fir_preservation</code></td>
      <td>Execution depends on layout, visual order, SVG internals, CSS fallback, or runtime-only widget shell not represented by source meaning.</td>
      <td>Visual convenience must not become execution semantics.</td>
    </tr>
    <tr>
      <td>FIR erases required distinction</td>
      <td><code>reject_fir_preservation</code></td>
      <td>FIR collapses interface and front-panel participation, widget value and widget reference, explicit state and feedback shape, or call identity and provider identity.</td>
      <td>Lowering must not be asked to reconstruct lost meaning from private assumptions.</td>
    </tr>
    <tr>
      <td>Provider-backed call missing provider requirement</td>
      <td><code>reject_lowering</code> or <code>reject_contract</code></td>
      <td>FIR contains a provider-backed call but lowering/backend handoff omits provider/capability/dependency requirements.</td>
      <td>Runtime must not discover hidden dependencies by convention.</td>
    </tr>
    <tr>
      <td>ABI or artifact mismatch</td>
      <td><code>reject_provider</code> or <code>reject_consumption</code></td>
      <td>Manifest declares an unsupported ABI profile, missing artifact, failed checksum, incompatible target, or unavailable host capability.</td>
      <td>Consumer must not reinterpret the artifact with guessed calling conventions.</td>
    </tr>
    <tr>
      <td>LLVM treated as FROG identity</td>
      <td><code>reject_contract</code></td>
      <td>Contract claims that LLVM IR is the public FIR identity or that a runtime must consume LLVM directly to be conforming.</td>
      <td>Backend-family choice must not redefine FROG language truth.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Minimum Case Record Shape</h2>

<p>
Each case that exercises this corridor should state:
</p>

<ul>
  <li>case id and title,</li>
  <li>primary owner stage,</li>
  <li>source features exercised,</li>
  <li>expected source result,</li>
  <li>expected semantic result,</li>
  <li>expected FIR features or preservation facts,</li>
  <li>expected lowering/backend features where applicable,</li>
  <li>manifest, provider, host capability, ABI, or dependency requirements where applicable,</li>
  <li>runtime, launcher, package, or backend-family consumption result where applicable,</li>
  <li>UI snapshot expectation where widgets are involved,</li>
  <li>public reference closure status,</li>
  <li>private continuation status if the case is only exercised by Graiphic-private runtime work.</li>
</ul>

<p>
The case should be narrow enough that a failure can be attributed to one
stage. A broad example may reference multiple narrow conformance cases, but it
should not be the only proof for all stages at once.
</p>

<hr/>

<h2>Concrete Case Files</h2>

<p>
The following concrete case files instantiate this matrix for the first
source/FIR/lowering corpus expansion. They are intentionally narrow records,
not broad runtime implementation recipes.
</p>

<h3>Positive cases</h3>

<ul>
  <li><a href="./valid/compiler/04_provider_backed_standard_library_call_declares_provider_requirement.md"><code>valid/compiler/04_provider_backed_standard_library_call_declares_provider_requirement</code></a> - provider-backed standard-library call with explicit provider/capability/dependency requirements.</li>
  <li><a href="./valid/compiler/05_widget_value_and_reference_are_preserved_across_lowering.md"><code>valid/compiler/05_widget_value_and_reference_are_preserved_across_lowering</code></a> - widget value and widget reference preservation across FIR, lowering, and backend contract boundaries.</li>
</ul>

<h3>Negative cases</h3>

<ul>
  <li><a href="./invalid/compiler/05_provider_backed_call_missing_provider_requirement.md"><code>invalid/compiler/05_provider_backed_call_missing_provider_requirement</code></a> - provider-backed call reaches lowering or contract emission without its required provider boundary.</li>
  <li><a href="./invalid/compiler/06_backend_contract_abi_profile_mismatch.md"><code>invalid/compiler/06_backend_contract_abi_profile_mismatch</code></a> - consumer rejects explicit ABI, target profile, backend family, or representation mismatch.</li>
  <li><a href="./invalid/compiler/07_manifest_missing_artifact_or_failed_checksum.md"><code>invalid/compiler/07_manifest_missing_artifact_or_failed_checksum</code></a> - manifest-compatible handoff fails artifact-reference or checksum validation.</li>
  <li><a href="./invalid/compiler/08_llvm_must_not_be_treated_as_frog_runtime_identity.md"><code>invalid/compiler/08_llvm_must_not_be_treated_as_frog_runtime_identity</code></a> - contract rejects the collapse of FROG/FIR/runtime identity into LLVM identity.</li>
  <li><a href="./invalid/compiler/09_valid_fir_rejected_by_backend_profile_unsupported_subset.md"><code>invalid/compiler/09_valid_fir_rejected_by_backend_profile_unsupported_subset</code></a> - valid FIR is explicitly rejected by a selected backend profile or implementation subset.</li>
</ul>

<p>
These cases cover the first provider, ABI/profile, manifest/artifact,
backend-identity, widget-preservation, and unsupported-subset families called
out by this matrix. Additional families remain open until they gain similarly
focused positive and negative case files.
</p>

<hr/>

<h2>Public Reference Boundary</h2>

<p>
The public reference runtime closure and the conformance matrix are related but
not identical.
</p>

<ul>
  <li>Examples within the current public reference runtime closure may provide executable evidence for a conformance case.</li>
  <li>Repository-visible later examples may provide specification-facing or design-progression evidence without committing Graiphic to publish production runtime implementation details.</li>
  <li>Private Graiphic runtime work may validate additional execution behavior internally, but public conformance wording must remain portable and implementation-independent.</li>
  <li>A runtime-visible snapshot is conformance evidence only for the declared stage and selected execution posture. It is not proof that every upstream or downstream stage is closed.</li>
</ul>

<hr/>

<h2>Promotion Rule</h2>

<p>
A feature family should not be described as systematically covered across the
source/FIR/lowering corridor until the following exist:
</p>

<ul>
  <li>a source-scope rule,</li>
  <li>a semantic validation rule,</li>
  <li>a positive source-to-FIR case,</li>
  <li>a negative rejection case,</li>
  <li>a FIR preservation expectation,</li>
  <li>a lowering/backend expectation where the feature crosses that boundary,</li>
  <li>a manifest/provider/host-capability expectation where the feature needs external or host participation,</li>
  <li>a runtime, launcher, package, or backend-family consumption expectation where observable execution is claimed.</li>
</ul>

<p>
Examples may demonstrate a feature earlier, but examples alone do not close the
conformance surface.
</p>

<hr/>

<h2>Open Items</h2>

<p>
The following families still need concrete case files before broad coverage can
be claimed:
</p>

<ul>
  <li>record / cluster values,</li>
  <li>uniform error/status corridor,</li>
  <li>path typing consistency,</li>
  <li>explicit conversion primitives,</li>
  <li>array construction and loop collection outputs,</li>
  <li>provider-backed standard-library calls beyond the initial image path and its first provider-requirement cases,</li>
  <li>host-capability calls such as time, OS/system, process, and connectivity surfaces,</li>
  <li>external provider calls and policy requirements,</li>
  <li>additional backend-consumer rejection cases beyond the first ABI/profile, artifact, checksum, and target-mismatch anchors.</li>
</ul>

<hr/>

<h2>Summary</h2>

<p>
This matrix gives FROG a staged conformance spine from source to consumer
handoff. It protects the core architecture: <code>.frog</code> source owns
source truth, FIR owns public execution-facing meaning, lowering owns target
specialization, backend contracts and manifests own consumer-facing handoff,
and runtimes or launchers consume declared artifacts without becoming the
definition of the language.
</p>
