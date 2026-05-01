<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Version Matrix</h1>

<p align="center">
  <strong>Centralized current-status table for the published FROG specification corpus</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#current-repository-wide-values">2. Current Repository-Wide Values</a></li>
  <li><a href="#current-surface-matrix">3. Current Surface Matrix</a></li>
  <li><a href="#current-widget-baseline-status">4. Current Widget Baseline Status</a></li>
  <li><a href="#maintenance-rule">5. Maintenance Rule</a></li>
  <li><a href="#summary">6. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document is the centralized detailed status table for the current published FROG specification corpus.
</p>

<p>
It complements <code>Versioning/Readme.md</code> by giving a compact matrix view of the current repository-wide posture, major repository surfaces, executable corridor closure, and widget-baseline status.
</p>

<hr/>

<h2 id="current-repository-wide-values">2. Current Repository-Wide Values</h2>

<table>
  <thead>
    <tr>
      <th>Field</th>
      <th>Current value</th>
      <th>Notes</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Specification corpus version</td>
      <td><code>0.1-draft</code></td>
      <td>Current repository-wide published posture under consolidation.</td>
    </tr>
    <tr>
      <td>Reference source-format target</td>
      <td><code>.frog spec_version = 0.1</code></td>
      <td>Bounded source compatibility target for the current published subset.</td>
    </tr>
    <tr>
      <td>Current repository-wide status</td>
      <td>Bounded published closure with frozen Example 05 reference slice and active widget-baseline consolidation.</td>
      <td>Example 05 remains frozen while widget definitions and Default realization manifests are extended around it.</td>
    </tr>
    <tr>
      <td>Versioning doctrine</td>
      <td>Additive by default, explicit degraded readability, preservable when safe.</td>
      <td>Governed centrally in <code>Versioning/Readme.md</code>.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2 id="current-surface-matrix">3. Current Surface Matrix</h2>

<table>
  <thead>
    <tr>
      <th>Surface</th>
      <th>Status</th>
      <th>Primary role</th>
      <th>Current posture</th>
      <th>Next coherent closure step</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>Readme.md</code></td>
      <td>Published repository entry surface</td>
      <td>Top-level architecture framing</td>
      <td>Describes source, semantics, FIR, lowering, backend contracts, runtime bridges, compiler bridges, and Example 05 as the current bounded executable anchor.</td>
      <td>Keep synchronized with actual widget-baseline and runtime closure state.</td>
    </tr>
    <tr>
      <td><code>Expression/</code></td>
      <td>Normative, bounded published scope</td>
      <td>Canonical source representation</td>
      <td>Owns source objects, widget instance representation, widget interaction, and source-level structural boundaries.</td>
      <td>Keep widget-source representation aligned with the growing widget class law.</td>
    </tr>
    <tr>
      <td><code>Language/</code></td>
      <td>Normative, bounded published scope</td>
      <td>Validated program meaning</td>
      <td>Owns execution semantics for accepted programs.</td>
      <td>Tighten only where required by currently published executable and widget corridors.</td>
    </tr>
    <tr>
      <td><code>IR/</code></td>
      <td>Normative, bounded published scope</td>
      <td>Execution-facing FIR and lowering</td>
      <td>Owns FIR, derivation, mapping, lowering, and backend contract posture.</td>
      <td>Keep aligned with bounded backend-family and reference-path claims.</td>
    </tr>
    <tr>
      <td><code>Libraries/Widgets/</code></td>
      <td>Published widget-baseline growth front</td>
      <td>Standard widget class law</td>
      <td>Currently covers Boolean, String, Button, Numeric, and Waveform Chart with class-law documents.</td>
      <td>Stabilize the current core before adding additional near-core widget families.</td>
    </tr>
    <tr>
      <td><code>Libraries/Realizations/Default/</code></td>
      <td>Published realization-baseline growth front</td>
      <td>Default realization manifests and assets</td>
      <td>Publishes Default manifests and SVG assets for Boolean, String, Button, Numeric, and Waveform Chart.</td>
      <td>Keep Default manifests synchronized with class law and avoid asset-owned semantics.</td>
    </tr>
    <tr>
      <td><code>Profiles/</code></td>
      <td>Published draft with bounded high-value closure fronts</td>
      <td>Optional capability families</td>
      <td>Provides profile-level posture for optional capability growth.</td>
      <td>Stabilize high-value corridors before widening profile families.</td>
    </tr>
    <tr>
      <td><code>IDE/</code></td>
      <td>Published draft architecture surface</td>
      <td>Authoring, debugging, observability, inspection</td>
      <td>Architecturally meaningful, but not yet a fully frozen ecosystem-wide law surface.</td>
      <td>Keep IDE-facing behavior separate from semantic law.</td>
    </tr>
    <tr>
      <td><code>Examples/05_bounded_ui_accumulator/</code></td>
      <td>Frozen bounded reference corridor</td>
      <td>First source-to-runtime-to-native anchor</td>
      <td>Frozen around source, front panel, FIR, lowering, backend contract, runtime-family acceptance, Python/Rust/C++, and LLVM proof.</td>
      <td>Preserve this corridor while new widget work grows around it.</td>
    </tr>
    <tr>
      <td><code>Implementations/Reference/Runtime/</code></td>
      <td>Published non-normative runtime-family workspace</td>
      <td>Reference runtime-family consumption</td>
      <td>Coordinates Python, Rust, and C/C++ consumers for Example 05.</td>
      <td>Keep acceptance-driven behavior green.</td>
    </tr>
    <tr>
      <td><code>Implementations/Reference/LLVM/</code></td>
      <td>Published narrow native proof path</td>
      <td>First LLVM-oriented native corridor</td>
      <td>Proves the Example 05 bounded accumulator behavior in a first native artifact.</td>
      <td>Do not overclaim general LLVM backend completeness.</td>
    </tr>
    <tr>
      <td><code>Versioning/</code></td>
      <td>Published centralized governance surface</td>
      <td>Version and status governance</td>
      <td>Centralizes version doctrine, current corpus posture, and matrix status.</td>
      <td>Keep synchronized with repo reality.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2 id="current-widget-baseline-status">4. Current Widget Baseline Status</h2>

<table>
  <thead>
    <tr>
      <th>Widget family</th>
      <th>Class-law status</th>
      <th>Default realization status</th>
      <th>Current note</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Boolean</td>
      <td>Published core class law</td>
      <td>Published Default manifest and SVG templates</td>
      <td>Rectangular and circular realization variants exist.</td>
    </tr>
    <tr>
      <td>String</td>
      <td>Published core class law</td>
      <td>Published Default manifest and SVG template</td>
      <td>Rectangular string realization exists.</td>
    </tr>
    <tr>
      <td>Button</td>
      <td>Published core class law</td>
      <td>Published Default manifest and state SVG resources</td>
      <td>Mechanical-action behavior is part of class law.</td>
    </tr>
    <tr>
      <td>Numeric</td>
      <td>Published core class law</td>
      <td>Published Default manifest and SVG template</td>
      <td>Modern numeric representation model exists while Example 05 flat-surface compatibility is preserved.</td>
    </tr>
    <tr>
      <td>Waveform Chart</td>
      <td>Published composite class law</td>
      <td>Published Default manifest with subobject SVG composition</td>
      <td>Composite model includes plot area, plots, scales, legends, palette, scrollbars, optional cursors, and plot-image layers.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2 id="maintenance-rule">5. Maintenance Rule</h2>

<p>
This matrix should be updated when one of the following changes:
</p>

<ul>
  <li>the declared repository-wide corpus posture,</li>
  <li>the freeze posture of Example 05,</li>
  <li>the status of the runtime-family corridor,</li>
  <li>the status of the LLVM proof path,</li>
  <li>the published widget-baseline class law,</li>
  <li>the published Default realization manifests,</li>
  <li>the next coherent closure target for the repository.</li>
</ul>

<hr/>

<h2 id="summary">6. Summary</h2>

<p>
The repository currently has a frozen executable reference corridor and an active standardized widget baseline.
The immediate technical priority is to stabilize the current widget core and continue near-core widget definition without reopening Example 05.
</p>
