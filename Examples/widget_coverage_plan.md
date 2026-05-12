<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Widget Example Coverage Plan</h1>

<p align="center">
  <strong>Bounded sequencing plan for simple front-panel widget examples</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This document records the conservative plan for adding small widget-focused examples after the current Examples 01–05 corridor.
It is intentionally a sequencing plan, not a broad runtime or compiler generalization claim.
</p>

<p>
The current primary LabVIEW-like proof remains Example 05, where the source diagram can be lowered toward an LLVM-produced native kernel and consumed by a compiler-agnostic runtime bridge.
Examples 06 and 07 extend that discipline to simple scalar widgets while keeping the runtime/compiler boundary explicit: LLVM produces kernel artifacts and manifests; the runtimes consume manifests, ABI surfaces, <code>.wfrog</code> packages, and Default realization assets.
</p>

<hr/>

<h2>Current Boundary</h2>

<ul>
  <li>Do not create Examples 08–10 before the Boolean and String pilots are validated.</li>
  <li>Do not generalize the Deriver, Lowerer, or ContractEmitter broadly in this wave.</li>
  <li>Do not make the runtime LLVM-only; scalar native-kernel coverage must stay behind explicit manifests.</li>
  <li>Do not weaken Examples 01–05 or Example 05 native-kernel bridge coverage.</li>
  <li>Keep each new example bounded, explicit, and independently checkable.</li>
</ul>

<hr/>

<h2>Rendering Fidelity Rule</h2>

<p>
A widget example is not valid if its visible front panel is merely a handcrafted HTML approximation.
The visible front panel must be derived from the published <code>.wfrog</code> layout and must consume the declared realization assets.
</p>

<ul>
  <li>The <code>.wfrog</code> front-panel package is the layout authority.</li>
  <li>The widget class law in <code>Libraries/Widgets/</code> is the class-surface authority.</li>
  <li>The default realization manifest in <code>Libraries/Realizations/Default/</code> is the realization authority for default assets.</li>
  <li>If a widget declares <code>visual.asset_ref = asset:&lt;id&gt;</code>, the rendered front panel must visibly use that asset or fail validation.</li>
  <li>If a package references <code>realization_refs</code>, the checker must verify that the realization manifest exists and targets the widget classes used by the example.</li>
  <li>The visible widget state must be selected from the realization state map. A Boolean example must expose and apply the relevant <code>false</code>, <code>true</code>, <code>hover_*</code>, <code>pressed_*</code>, <code>focused_*</code>, <code>disabled_*</code>, and transition visual states where the class law and realization publish them.</li>
  <li>Fallback rendering may be used only when explicitly marked as fallback and must not pass official widget-example validation.</li>
  <li>Raw runtime snapshots and preformatted debug dumps must not appear in the user-facing front panel by default; a route such as <code>/state.json</code> may remain available for validation.</li>
</ul>

<hr/>

<h2>Wave 1 Candidate Sequence</h2>

<table>
  <thead>
    <tr>
      <th>Candidate</th>
      <th>Widgets</th>
      <th>Behavior</th>
      <th>Initial validation posture</th>
      <th>Status</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>06_boolean_value_roundtrip</code></td>
      <td><code>frog.widgets.boolean_control</code> → <code>frog.widgets.boolean_indicator</code></td>
      <td>copy <code>control.value</code> to <code>indicator.value</code></td>
      <td>runtime/UI-binding plus manifest-driven native-kernel bridge</td>
      <td>published pilot</td>
    </tr>
    <tr>
      <td><code>07_string_value_roundtrip</code></td>
      <td><code>frog.widgets.string_control</code> → <code>frog.widgets.string_indicator</code></td>
      <td>copy text value</td>
      <td>runtime/UI-binding plus manifest-driven native-kernel bridge</td>
      <td>published pilot</td>
    </tr>
    <tr>
      <td><code>08_enum_value_roundtrip</code></td>
      <td><code>frog.widgets.enum_control</code> → <code>frog.widgets.enum_indicator</code></td>
      <td>copy selected enum value</td>
      <td>future runtime/UI-binding only</td>
      <td>planned</td>
    </tr>
    <tr>
      <td><code>09_path_value_roundtrip</code></td>
      <td><code>frog.widgets.path_control</code> → <code>frog.widgets.path_indicator</code></td>
      <td>copy path value</td>
      <td>future runtime/UI-binding only</td>
      <td>planned</td>
    </tr>
    <tr>
      <td><code>10_button_event_to_boolean</code></td>
      <td><code>frog.widgets.button</code> → <code>frog.widgets.boolean_indicator</code></td>
      <td>surface a button event/state as a boolean value</td>
      <td>future runtime/UI-binding only</td>
      <td>planned</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Example 06 Pilot Acceptance Criteria</h2>

<ul>
  <li>One boolean control is placed on the left of the front panel.</li>
  <li>One boolean indicator is placed on the right of the front panel.</li>
  <li>The control value is copied to the indicator value and to the public output.</li>
  <li>The <code>.wfrog</code> package owns panel layout and realization references.</li>
  <li>The check consumes the Boolean widget class law and the default Boolean realization manifest.</li>
  <li>The rendered front panel uses the <code>visual.asset_ref</code> assets declared by the package.</li>
  <li>The rendered Boolean state is materialized through the <code>.wfrog</code> properties and Default Boolean realization <code>state_maps</code>, not through untracked HTML/CSS fallback styling.</li>
  <li>The visible control body is clickable and posts the opposite Boolean value.</li>
  <li>Both <code>true</code> and <code>false</code> states are checked.</li>
  <li>The user-facing front panel is clean and does not show the runtime snapshot by default.</li>
  <li>A <code>state.json</code>-style diagnostic surface may remain available for validation and debug.</li>
  <li>LLVM-native bridge coverage remains optional and manifest-driven for this pilot.</li>
</ul>

<hr/>

<h2>Example 07 Pilot Acceptance Criteria</h2>

<ul>
  <li>One string control is placed on the left of the front panel.</li>
  <li>One string indicator is placed on the right of the front panel.</li>
  <li>The control text is copied to the indicator text and to the public output.</li>
  <li>The <code>.wfrog</code> package owns panel layout, caption anchors, hover styling, and realization references.</li>
  <li>The visible front panel consumes the Default String realization and does not publish a local duplicated SVG.</li>
  <li>The String realization does not publish a <code>focus_ring</code> part; runtimes must not add an untracked host CSS focus ring.</li>
  <li>The C++, Python, and Rust runtimes keep the same source/FIR/lowering/contract/native-manifest/runtime shape for this bounded pilot.</li>
  <li>A <code>state.json</code>-style diagnostic surface may remain available for validation and debug.</li>
</ul>

<hr/>

<h2>Compiler and Runtime Posture</h2>

<p>
The scalar widget pilot examples do not make the runtime LLVM-only.
They validate front-panel package consumption, widget value binding, visible runtime behavior, and, when published, manifest-driven native-kernel bridge consumption.
LLVM remains a backend artifact producer; the runtimes consume explicit manifests and ABI surfaces.
</p>

<hr/>

<h2>Future Work</h2>

<ul>
  <li>Keep Examples 06 and 07 in the shared validation posture once their dedicated checks are stable.</li>
  <li>Add Enum, Path, and Button examples one at a time.</li>
  <li>Only then consider a narrow scalar widget runtime abstraction.</li>
  <li>Keep Array, Cluster, Table, Tree, Tab, Listbox, Picture, Chart, Splitter, Panel/SubPanel, Frame, Label, and Decorations for later waves.</li>
</ul>
