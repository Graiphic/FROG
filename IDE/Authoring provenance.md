<h1>FROG IDE Authoring Provenance</h1>

<p>IDE-side creation, verification, display, review, and preservation workflows for source-carried FROG provenance.</p>
<p>FROG — Free Open Graphical Language</p>

<hr />

<h2>Contents</h2>

<nav>
  <ol>
    <li><a href="#1-overview">Overview</a></li>
    <li><a href="#2-document-role">Document Role</a></li>
    <li><a href="#3-relationship-with-source-provenance">Relationship with Source Provenance</a></li>
    <li><a href="#4-core-ide-responsibilities">Core IDE Responsibilities</a></li>
    <li><a href="#5-authoring-event-classification">Authoring Event Classification</a></li>
    <li><a href="#6-human-direct-authoring">Human Direct Authoring</a></li>
    <li><a href="#7-ai-assisted-and-ai-generated-authoring">AI-Assisted and AI-Generated Authoring</a></li>
    <li><a href="#8-express-tool-assisted-and-deterministic-guided-authoring">Express, Tool-Assisted, and Deterministic Guided Authoring</a></li>
    <li><a href="#9-snippet-import-and-reuse">Snippet Import and Reuse</a></li>
    <li><a href="#10-human-review-and-acceptance">Human Review and Acceptance</a></li>
    <li><a href="#11-verification-and-display-states">Verification and Display States</a></li>
    <li><a href="#12-signing-keys-and-issuers">Signing, Keys, and Issuers</a></li>
    <li><a href="#13-program-model-integration">Program Model Integration</a></li>
    <li><a href="#14-save-load-and-round-trip-behavior">Save, Load, and Round-Trip Behavior</a></li>
    <li><a href="#15-observability-and-debugging-interaction">Observability and Debugging Interaction</a></li>
    <li><a href="#16-security-and-ux-requirements">Security and UX Requirements</a></li>
    <li><a href="#17-examples">Examples</a></li>
    <li><a href="#18-summary">Summary</a></li>
  </ol>
</nav>

<hr />

<section id="1-overview">
  <h2>1. Overview</h2>

  <p>Authoring provenance is the IDE-facing workflow that creates, verifies, displays, and updates source-carried provenance attestations for <code>.frog</code> programs.</p>

  <p>A FROG IDE may allow users to create graph objects manually, ask an AI assistant to generate or modify logic, insert snippets, use Express guided authoring, import objects from external files, and review generated regions. These paths should remain visible to the user when the source carries enough evidence.</p>

  <p>The IDE authoring provenance layer exists so that a graph can display not only what the program is, but also what is known about how its parts were authored, generated, imported, modified, or accepted.</p>

  <pre><code>authoring action
    -&gt; Program Model change
    -&gt; source object digest
    -&gt; optional signed provenance attestation
    -&gt; visual provenance state</code></pre>

  <p>This document defines IDE behavior. It does not define executable program meaning.</p>
</section>

<hr />

<section id="2-document-role">
  <h2>2. Document Role</h2>

  <p>This document belongs in <code>IDE/</code> because it specifies authoring behavior, review workflows, trust display, and user-facing provenance interaction.</p>

  <p>It is paired with <code>Expression/Source provenance.md</code>, which defines the source-carried object model under <code>ide.provenance</code>.</p>

  <table>
    <thead>
      <tr>
        <th>This document owns</th>
        <th>This document does not own</th>
      </tr>
    </thead>
    <tbody>
      <tr>
        <td>When an IDE should create provenance attestations.</td>
        <td>Canonical executable graph meaning.</td>
      </tr>
      <tr>
        <td>How an IDE should classify authoring actions.</td>
        <td>Language semantics.</td>
      </tr>
      <tr>
        <td>How an IDE should verify and display provenance states.</td>
        <td>Primitive semantics.</td>
      </tr>
      <tr>
        <td>How an IDE should treat AI-generated, human-direct, imported, reviewed, and unknown content.</td>
        <td>IR attribution law as a whole.</td>
      </tr>
      <tr>
        <td>How an IDE should record human review or acceptance.</td>
        <td>Runtime execution behavior.</td>
      </tr>
    </tbody>
  </table>
</section>

<hr />

<section id="3-relationship-with-source-provenance">
  <h2>3. Relationship with Source Provenance</h2>

  <p><code>Expression/Source provenance.md</code> defines the source-carried provenance model:</p>

  <pre><code>ide.provenance
    -&gt; attestations
    -&gt; subjects
    -&gt; digests
    -&gt; claims
    -&gt; issuers
    -&gt; signatures
    -&gt; verification states</code></pre>

  <p>This IDE document defines the workflows that create, update, verify, and render that information.</p>

  <p>The relationship is:</p>

  <pre><code>Expression/Source provenance.md
    defines what can be serialized

IDE/Authoring provenance.md
    defines how an IDE should use it</code></pre>

  <p>An IDE MUST NOT mark an AI-assisted action as <code>human_direct</code> merely because it was executed through the IDE. The authoring channel and the origin of the produced source object are separate facts.</p>
</section>

<hr />

<section id="4-core-ide-responsibilities">
  <h2>4. Core IDE Responsibilities</h2>

  <p>A provenance-aware FROG IDE SHOULD support the following responsibilities:</p>

  <ul>
    <li>classify authoring actions according to source origin and review status;</li>
    <li>create attestations for source objects or changesets when the IDE has authority to do so;</li>
    <li>compute digests over canonicalized source object states;</li>
    <li>sign provenance claims when signing is enabled and the IDE owns or can access an appropriate issuer key;</li>
    <li>preserve existing provenance during safe round-trips;</li>
    <li>detect stale attestations after source changes;</li>
    <li>visually distinguish trusted, untrusted, invalid, stale, unknown, AI-generated, AI-assisted, and human-reviewed states;</li>
    <li>allow explicit human review acceptance or rejection where project policy requires it;</li>
    <li>avoid presenting unverified provenance as trusted.</li>
  </ul>

  <p>The IDE MAY provide stronger policy behavior for enterprise, certified, or safety-relevant workflows, but those policies MUST NOT redefine base language semantics.</p>
</section>

<hr />

<section id="5-authoring-event-classification">
  <h2>5. Authoring Event Classification</h2>

  <p>The IDE SHOULD classify source-producing actions using a stable vocabulary that is compatible with <code>Expression/Source provenance.md</code>.</p>

  <table>
    <thead>
      <tr>
        <th>Classification</th>
        <th>IDE interpretation</th>
      </tr>
    </thead>
    <tbody>
      <tr>
        <td><code>human_direct</code></td>
        <td>The user directly created or changed the source object through a direct authoring action such as placing a node, drawing a wire, moving a port, or editing a property manually.</td>
      </tr>
      <tr>
        <td><code>human_guided</code></td>
        <td>The user configured a guided tool or wizard that deterministically emitted source objects.</td>
      </tr>
      <tr>
        <td><code>ai_generated</code></td>
        <td>An AI system generated the object or region from a prompt, instruction, or inferred intent.</td>
      </tr>
      <tr>
        <td><code>ai_assisted</code></td>
        <td>An AI system suggested, transformed, completed, or modified the object with user participation.</td>
      </tr>
      <tr>
        <td><code>tool_generated</code></td>
        <td>A deterministic non-AI generator produced the object.</td>
      </tr>
      <tr>
        <td><code>imported</code></td>
        <td>The object was imported from another file, package, or external tool.</td>
      </tr>
      <tr>
        <td><code>snippet_inserted</code></td>
        <td>The object was inserted from a reusable snippet.</td>
      </tr>
      <tr>
        <td><code>unknown</code></td>
        <td>The IDE cannot establish a reliable origin.</td>
      </tr>
    </tbody>
  </table>

  <p>The classification SHOULD be applied to the smallest reasonable source object or changeset. A large generated region MAY be attested as a region or changeset when per-node classification would create excessive noise.</p>
</section>

<hr />

<section id="6-human-direct-authoring">
  <h2>6. Human Direct Authoring</h2>

  <p>When a user directly performs a graphical operation, an IDE MAY create a <code>human_direct</code> attestation for the resulting source object or changeset.</p>

  <p>Examples include:</p>

  <ul>
    <li>placing a primitive node from the palette;</li>
    <li>placing a SubFROG node;</li>
    <li>drawing a wire between two ports;</li>
    <li>adding an interface port manually;</li>
    <li>editing a node property through a property panel;</li>
    <li>adding or editing a front-panel widget manually;</li>
    <li>moving or resizing an authoring-presentation object when presentation provenance is tracked.</li>
  </ul>

  <p>A direct graphical action SHOULD NOT automatically produce a semantic attestation for unrelated objects. If the IDE performs auto-layout, auto-wire repair, implicit coercion, or default object creation, those secondary changes SHOULD be classified separately when the distinction matters.</p>

  <pre><code>user places node from palette
    -&gt; create diagram node
    -&gt; compute node digest
    -&gt; claim origin = human_direct
    -&gt; sign if policy allows
    -&gt; display trusted human-direct state if verified</code></pre>
</section>

<hr />

<section id="7-ai-assisted-and-ai-generated-authoring">
  <h2>7. AI-Assisted and AI-Generated Authoring</h2>

  <p>If an AI system creates or modifies source objects, the IDE MUST NOT hide that origin by signing the result as <code>human_direct</code>.</p>

  <p>The IDE SHOULD distinguish:</p>

  <ul>
    <li><code>ai_generated</code> — the AI produced the source object or region;</li>
    <li><code>ai_assisted</code> — the AI participated in a user-driven modification or suggestion;</li>
    <li><code>review_accept</code> — a human later accepted the resulting digest;</li>
    <li><code>review_reject</code> — a human rejected the resulting digest or region.</li>
  </ul>

  <p>The most important IDE state is not simply <code>AI</code> versus <code>human</code>. It is whether the current source state has been accepted by a responsible human reviewer.</p>

  <pre><code>AI generates region
    -&gt; origin = ai_generated
    -&gt; review_state = not_reviewed
    -&gt; human inspects graph
    -&gt; review_accept over same digest
    -&gt; display = AI-generated, human accepted</code></pre>
</section>

<hr />

<section id="8-express-tool-assisted-and-deterministic-guided-authoring">
  <h2>8. Express, Tool-Assisted, and Deterministic Guided Authoring</h2>

  <p>Express nodes, guided authoring surfaces, wizards, and deterministic tools can produce canonical source from user configuration. These workflows SHOULD NOT be collapsed into either purely human-direct or AI-generated states.</p>

  <p>The recommended classification is:</p>

  <ul>
    <li><code>human_guided</code> when a human configured a deterministic authoring flow;</li>
    <li><code>tool_generated</code> when a non-AI tool produced source without direct manual placement of every object;</li>
    <li><code>ai_assisted</code> or <code>ai_generated</code> when an AI system participated in the guided flow.</li>
  </ul>

  <p>Recoverability metadata and provenance have different roles:</p>

  <pre><code>ide.recoverability
    -&gt; helps reopen an authoring flow

ide.provenance
    -&gt; records origin, digest, signature, and review evidence</code></pre>

  <p>Recoverability metadata does not prove provenance. Provenance metadata does not replace recoverability.</p>
</section>

<hr />

<section id="9-snippet-import-and-reuse">
  <h2>9. Snippet Import and Reuse</h2>

  <p>When an IDE inserts a snippet, imported fragment, template, or reusable graph region, it SHOULD preserve valid provenance when preservation is meaningful and safe.</p>

  <p>If the inserted object changes identity, scope, IDs, bindings, or semantic content during insertion, the IDE SHOULD either:</p>

  <ul>
    <li>record the new object as <code>snippet_inserted</code> or <code>imported</code>;</li>
    <li>create a new attestation for the resulting source state;</li>
    <li>preserve original provenance as historical or external evidence only if the relationship remains clear;</li>
    <li>mark copied attestations as stale or not directly applicable when digest matching fails.</li>
  </ul>

  <p>A snippet inserted by a human is not automatically <code>human_direct</code> for every internal object. The insertion action may be human-direct while the internal objects are imported or snippet-derived.</p>
</section>

<hr />

<section id="10-human-review-and-acceptance">
  <h2>10. Human Review and Acceptance</h2>

  <p>Human review is a separate authoring claim. It SHOULD be represented as an attestation over the digest being reviewed.</p>

  <p>The IDE SHOULD provide explicit review actions, for example:</p>

  <ul>
    <li>Accept selected node;</li>
    <li>Accept selected edge;</li>
    <li>Accept generated region;</li>
    <li>Reject generated region;</li>
    <li>Request revision;</li>
    <li>Clear stale review state after modification;</li>
    <li>Require review before build, execution, deployment, or export under a stricter profile.</li>
  </ul>

  <p>A review acceptance SHOULD bind to the current digest. If the object changes after acceptance, the acceptance SHOULD become stale for the modified object.</p>

  <pre><code>review_accept
    subject.digest = current digest
    reviewer_role = project_engineer
    signature = reviewer or IDE issuer signature

later source edit
    digest mismatch
    previous review_accept becomes stale</code></pre>
</section>

<hr />

<section id="11-verification-and-display-states">
  <h2>11. Verification and Display States</h2>

  <p>A provenance-aware IDE SHOULD display verification states in a way that is visible without being confused with runtime execution state.</p>

  <table>
    <thead>
      <tr>
        <th>IDE display state</th>
        <th>Meaning</th>
      </tr>
    </thead>
    <tbody>
      <tr>
        <td>Human direct / trusted</td>
        <td>Valid trusted attestation for direct human authoring.</td>
      </tr>
      <tr>
        <td>AI generated / not reviewed</td>
        <td>AI origin is attested or inferred by the IDE workflow, and no accepted review is present for the current digest.</td>
      </tr>
      <tr>
        <td>AI generated / human accepted</td>
        <td>AI origin exists, and a human review acceptance matches the current digest.</td>
      </tr>
      <tr>
        <td>Imported or snippet-derived</td>
        <td>The object came from external or reusable material.</td>
      </tr>
      <tr>
        <td>Unknown / unattested</td>
        <td>No reliable provenance exists. This MUST NOT be displayed as automatically AI-generated.</td>
      </tr>
      <tr>
        <td>Stale</td>
        <td>The source object changed after the attestation.</td>
      </tr>
      <tr>
        <td>Invalid</td>
        <td>The attestation or signature is malformed or does not verify.</td>
      </tr>
      <tr>
        <td>Untrusted issuer</td>
        <td>The signature verifies but the issuer is not trusted by local policy.</td>
      </tr>
      <tr>
        <td>Unsupported</td>
        <td>The IDE cannot verify the algorithm, schema, subject, or canonicalization rule.</td>
      </tr>
    </tbody>
  </table>

  <p>Display choices may use badges, overlays, property-panel details, audit panes, filters, or graph-level heatmaps. Those UI choices are product behavior, not language semantics.</p>
</section>

<hr />

<section id="12-signing-keys-and-issuers">
  <h2>12. Signing, Keys, and Issuers</h2>

  <p>The IDE may sign attestations with an issuer key. The key may belong to the IDE vendor, a project, an organization, a user, a CI system, a review authority, or another trusted entity.</p>

  <p>The standard source format remains open. The IDE product may implement proprietary key custody, certification, issuer registry, display policy, and review workflows.</p>

  <p>A Graiphic IDE may therefore provide value through:</p>

  <ul>
    <li>secure key management;</li>
    <li>first-class Graiphic issuer recognition;</li>
    <li>enterprise trust-policy configuration;</li>
    <li>certified third-party issuer support;</li>
    <li>review workflow UX;</li>
    <li>visual provenance overlays;</li>
    <li>deployment gating under stricter profiles.</li>
  </ul>

  <p>Other IDEs may emit conforming provenance. Whether those attestations are trusted depends on the verifier policy.</p>
</section>

<hr />

<section id="13-program-model-integration">
  <h2>13. Program Model Integration</h2>

  <p>The IDE Program Model SHOULD track source object identity and provenance subject identity closely enough to support stable attestation and stale-state detection.</p>

  <p>Useful integration points include:</p>

  <ul>
    <li>stable source object IDs;</li>
    <li>changeset grouping;</li>
    <li>source-object canonicalization helpers;</li>
    <li>digest recomputation after edits;</li>
    <li>review-state invalidation when semantic content changes;</li>
    <li>separate semantic and presentation digest scopes;</li>
    <li>source mapping from visual selection to provenance subject.</li>
  </ul>

  <p>The Program Model may cache verification results for UI responsiveness, but cached verification results are not source truth. The serialized source truth remains <code>ide.provenance</code> plus the covered canonical source objects.</p>
</section>

<hr />

<section id="14-save-load-and-round-trip-behavior">
  <h2>14. Save, Load, and Round-Trip Behavior</h2>

  <p>On load, a provenance-aware IDE SHOULD:</p>

  <ul>
    <li>parse <code>ide.provenance</code> if present;</li>
    <li>associate attestations with source objects where possible;</li>
    <li>compute verification states;</li>
    <li>avoid trusting unsupported or unverified claims;</li>
    <li>surface stale or invalid claims to users when relevant.</li>
  </ul>

  <p>On save, a provenance-aware IDE SHOULD:</p>

  <ul>
    <li>preserve existing valid and unknown provenance whenever practical;</li>
    <li>update or add attestations for IDE-owned authoring events when policy enables it;</li>
    <li>avoid silently deleting historical review evidence;</li>
    <li>mark or supersede stale attestations rather than presenting them as current;</li>
    <li>minimize noisy source changes.</li>
  </ul>

  <p>A non-provenance-aware formatter SHOULD preserve unknown <code>ide</code> fields whenever practical.</p>
</section>

<hr />

<section id="15-observability-and-debugging-interaction">
  <h2>15. Observability and Debugging Interaction</h2>

  <p>Provenance overlays are authoring and review overlays. They are distinct from execution observability overlays such as probes, watches, traces, live values, runtime health, and backend diagnostics.</p>

  <p>The IDE MAY combine them in a single cockpit, but it SHOULD keep their meaning distinct:</p>

  <pre><code>execution overlay
    -&gt; what happened at runtime

provenance overlay
    -&gt; what is known about authoring, generation, and review</code></pre>

  <p>A stricter IDE or profile MAY provide workflows such as:</p>

  <ul>
    <li>pause before executing unreviewed AI-generated regions;</li>
    <li>warn before building with stale review attestations;</li>
    <li>filter runtime traces by provenance state;</li>
    <li>highlight untrusted-source regions during debugging.</li>
  </ul>

  <p>Such workflows are policy and product behavior. They do not alter base program semantics.</p>
</section>

<hr />

<section id="16-security-and-ux-requirements">
  <h2>16. Security and UX Requirements</h2>

  <ul>
    <li>The IDE MUST NOT label unknown content as trusted human content.</li>
    <li>The IDE MUST NOT label AI-generated content as <code>human_direct</code> merely because an IDE-hosted AI wrote it.</li>
    <li>The IDE SHOULD make stale attestations visible when the user is relying on provenance.</li>
    <li>The IDE SHOULD expose enough detail for audit: subject, origin, review state, issuer, signature state, digest scope, and timestamp when available.</li>
    <li>The IDE SHOULD avoid overwhelming users with per-object noise when a changeset or region-level view is clearer.</li>
    <li>The IDE SHOULD distinguish review acceptance from safety certification.</li>
    <li>The IDE SHOULD support privacy-aware actor identifiers.</li>
  </ul>
</section>

<hr />

<section id="17-examples">
  <h2>17. Examples</h2>

  <h3>17.1 Palette insertion</h3>

  <pre><code>User drags add primitive from palette.
IDE creates diagram node.
IDE computes semantic digest.
IDE emits create_node / human_direct attestation.
IDE signs if signing is enabled.
IDE displays human-direct trusted state after verification.</code></pre>

  <h3>17.2 AI generation then human acceptance</h3>

  <pre><code>User asks AI assistant to create bounded accumulator region.
IDE creates region and marks origin = ai_generated.
IDE displays generated/unreviewed overlay.
Engineer inspects region, validates semantics, checks FIR path.
Engineer accepts region.
IDE emits review_accept over current digest.
IDE displays AI-generated / human accepted.</code></pre>

  <h3>17.3 Direct JSON edit outside IDE</h3>

  <pre><code>Source file is edited in a text editor.
No valid IDE attestation covers new source state.
IDE reloads file.
Digest mismatch occurs for previous attestation.
IDE displays stale or unknown state.
IDE does not assume AI origin.</code></pre>
</section>

<hr />

<section id="18-summary">
  <h2>18. Summary</h2>

  <p>IDE authoring provenance is the workflow layer that makes source-carried provenance useful to humans. It classifies authoring events, signs claims when appropriate, verifies existing attestations, displays provenance states, and supports explicit human review.</p>

  <p>The core discipline is:</p>

  <pre><code>Human direct authoring is not the same as AI generation.
AI generation is not the same as human review.
Unknown origin is not the same as AI origin.
Valid signature is not the same as trusted policy.
Provenance UX is not executable semantics.</code></pre>
</section>
