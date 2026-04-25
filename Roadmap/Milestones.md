<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Milestones</h1>

<p align="center">
  <strong>Compact milestone-tracking surface for the non-normative FROG closure roadmap</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#what-this-file-tracks">2. What This File Tracks</a></li>
  <li><a href="#what-this-file-does-not-track">3. What This File Does Not Track</a></li>
  <li><a href="#relationship-with-versioning">4. Relationship with Versioning</a></li>
  <li><a href="#status-legend">5. Status Legend</a></li>
  <li><a href="#current-milestone-bias">6. Current Milestone Bias</a></li>
  <li><a href="#milestone-table">7. Milestone Table</a></li>
  <li><a href="#reading-guidance">8. Reading Guidance</a></li>
  <li><a href="#maintenance-rule">9. Maintenance Rule</a></li>
  <li><a href="#summary">10. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>
<p>
This document is the compact milestone-tracking surface for the non-normative FROG roadmap.
</p>

<p>
Its purpose is to give a concise view of the main closure milestones that organize the project’s sequencing logic without turning milestone tracking into a competing source of specification truth, semantic ownership, or current corpus-version reporting.
</p>

<p>
This file is intentionally shorter and more operational than <code>Roadmap/Readme.md</code>.
It is a navigation surface for closure progress, not a replacement for the architectural roadmap narrative.
</p>

<hr/>

<h2 id="what-this-file-tracks">2. What This File Tracks</h2>
<p>
This file tracks:
</p>

<ul>
  <li>the major closure milestones of the FROG project,</li>
  <li>their rough sequencing logic,</li>
  <li>their high-level status,</li>
  <li>their relation to the repository-visible closure corridor.</li>
</ul>

<p>
The milestones here are project-level closure markers.
They are intended to remain compact and readable.
</p>

<hr/>

<h2 id="what-this-file-does-not-track">3. What This File Does Not Track</h2>
<p>
This file does not track:
</p>

<ul>
  <li>the current published specification corpus version,</li>
  <li>the current detailed status of every repository surface,</li>
  <li>the cross-version additive-evolution doctrine,</li>
  <li>the detailed compatibility posture of source, semantic, IR, or profile layers,</li>
  <li>the normative law of the language.</li>
</ul>

<p>
Those responsibilities belong elsewhere:
</p>

<ul>
  <li><code>Versioning/Readme.md</code> — current corpus-version doctrine and transition policy,</li>
  <li><code>Versioning/Matrix.md</code> — detailed current-status table by repository surface,</li>
  <li>the six core specification families — normative technical ownership.</li>
</ul>

<hr/>

<h2 id="relationship-with-versioning">4. Relationship with Versioning</h2>
<p>
Milestones and versions are related, but they must not be confused.
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
      <td><code>Roadmap/Milestones.md</code></td>
      <td>Which closure steps matter most, and in what rough order?</td>
      <td>Current corpus-version truth</td>
    </tr>
    <tr>
      <td><code>Versioning/Readme.md</code></td>
      <td>What is the current published specification corpus version and what doctrine governs version transitions?</td>
      <td>Milestone sequencing</td>
    </tr>
    <tr>
      <td><code>Versioning/Matrix.md</code></td>
      <td>What is the detailed current status of each major repository surface?</td>
      <td>Milestone planning</td>
    </tr>
  </tbody>
</table>

<p>
This means:
</p>

<ul>
  <li>a milestone may be active even when the current corpus version has not changed,</li>
  <li>a corpus version may be clarified centrally without changing milestone order,</li>
  <li>this file should never be used as the authoritative answer to “what version is the published specification currently at?”</li>
</ul>

<p>
For that question, the authoritative repository-visible entry points are:
</p>

<ul>
  <li><code>Versioning/Readme.md</code>,</li>
  <li><code>Versioning/Matrix.md</code>.</li>
</ul>

<hr/>

<h2 id="status-legend">5. Status Legend</h2>
<ul>
  <li><strong>[x]</strong> completed or already established in the published baseline</li>
  <li><strong>[~]</strong> active, partially established, or under consolidation</li>
  <li><strong>[ ]</strong> planned future milestone</li>
</ul>

<hr/>

<h2 id="current-milestone-bias">6. Current Milestone Bias</h2>
<p>
The current milestone bias remains:
</p>

<ol>
  <li>preserve repository-entry coherence,</li>
  <li>preserve source / semantics / IR / conformance alignment,</li>
  <li>preserve the bounded executable reference corridor,</li>
  <li>preserve widget-object corridor coherence,</li>
  <li>preserve centralized version-governance clarity,</li>
  <li>preserve source provenance and human-review attestation boundaries without turning them into execution semantics,</li>
  <li>only then broaden backend families, runtime families, deployment families, or ecosystem claims.</li>
</ol>

<p>
In other words, the project should continue to prefer coherent corridor closure over premature ecosystem widening.
</p>

<hr/>

<h2 id="milestone-table">7. Milestone Table</h2>
<table>
  <thead>
    <tr>
      <th>Milestone</th>
      <th>Status</th>
      <th>Meaning</th>
      <th>Current practical focus</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>M0 — Repository foundation established</td>
      <td>[x]</td>
      <td>Core specification families, support areas, framing layers, and centralized version-governance surfaces exist</td>
      <td>Preserve coherence rather than reopening the foundation</td>
    </tr>
    <tr>
      <td>M1 — Canonical source, source-schema, and provenance closure</td>
      <td>[~]</td>
      <td>The canonical <code>.frog</code> source model and its structural-validity posture are made increasingly precise and stable</td>
      <td>Tighten source-shape, section ownership, structural validation boundaries, and optional <code>ide.provenance</code> preservation rules</td>
    </tr>
    <tr>
      <td>M2 — Validated semantic closure</td>
      <td>[~]</td>
      <td>Validated program meaning becomes increasingly explicit and architecture-coherent</td>
      <td>Keep control, state, interface, widget, and primitive/profile semantics aligned</td>
    </tr>
    <tr>
      <td>M3 — IR and compiler-corridor closure</td>
      <td>[~]</td>
      <td>The open execution-facing IR corridor remains explicit from derivation through lowering and backend contract</td>
      <td>Preserve clear distinction between open IR, lowering, backend handoff, and private realization</td>
    </tr>
    <tr>
      <td>M4 — Bounded execution-start and reference-path proof</td>
      <td>[~]</td>
      <td>A narrow but real path from source to observable execution is repository-visible and credible</td>
      <td>Keep examples, conformance, profiles, and reference implementation aligned on the bounded slice</td>
    </tr>
    <tr>
      <td>M5 — Widget-object and UI corridor closure</td>
      <td>[~]</td>
      <td>The widget-object model becomes serious enough to sustain long-term IDE/runtime/UI-host reasoning</td>
      <td>Keep <code>widget_value</code>, <code>widget_reference</code>, class contracts, and UI primitives coherent</td>
    </tr>
    <tr>
      <td>M6 — Conformance growth and profile broadening</td>
      <td>[ ]</td>
      <td>The public truth surface and capability-family coverage broaden from a stable bounded core</td>
      <td>Expand only after current corridor closure is solid enough</td>
    </tr>
    <tr>
      <td>M7 — Broader ecosystem and industrial-standard credibility</td>
      <td>[ ]</td>
      <td>FROG sustains credible multi-implementation, profile, and industrial ecosystem growth</td>
      <td>Widen from proof to durable ecosystem posture without losing architectural discipline</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2 id="reading-guidance">8. Reading Guidance</h2>
<p>
Readers should use this file for a quick answer to:
</p>

<p>
<strong>Which closure milestones matter right now?</strong>
</p>

<p>
Readers should use other files for different questions:
</p>

<ul>
  <li><strong><code>Roadmap/Readme.md</code></strong> — for the fuller narrative and phase logic,</li>
  <li><strong><code>Versioning/Readme.md</code></strong> — for the current published corpus version and cross-version doctrine,</li>
  <li><strong><code>Versioning/Matrix.md</code></strong> — for the current detailed status of each major repository surface.</li>
</ul>

<hr/>

<h2 id="maintenance-rule">9. Maintenance Rule</h2>
<p>
This file should be updated when:
</p>

<ul>
  <li>a major closure milestone is reached,</li>
  <li>a major milestone changes status from planned to active,</li>
  <li>the current milestone bias changes materially,</li>
  <li>the project reorders major closure fronts.</li>
</ul>

<p>
This file should <strong>not</strong> be updated merely to restate the current corpus-version truth if that truth is already maintained centrally in <code>Versioning/</code>.
</p>

<p>
Milestones should remain compact, readable, and sequencing-oriented.
They should not become a second version-status matrix.
</p>

<hr/>

<h2 id="summary">10. Summary</h2>
<p>
This file is the compact milestone tracker of the non-normative FROG roadmap.
</p>

<p>
It should answer:
</p>

<ul>
  <li>which closure fronts are already established,</li>
  <li>which ones are currently active,</li>
  <li>which ones come next in the project logic.</li>
</ul>

<p>
It should not answer, by itself:
</p>

<ul>
  <li>what the current published specification corpus version is,</li>
  <li>what the detailed current status of each repository surface is,</li>
  <li>what the full cross-version doctrine is.</li>
</ul>

<p>
Those responsibilities remain centralized in:
</p>

<ul>
  <li><code>Versioning/Readme.md</code>,</li>
  <li><code>Versioning/Matrix.md</code>.</li>
</ul>
