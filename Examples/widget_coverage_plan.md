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
The widget examples described here are initially focused on front-panel package publication, runtime value binding, user-facing rendering posture, and snapshot diagnostics.
</p>

<hr/>

<h2>Current Boundary</h2>

<ul>
  <li>Do not create Examples 07–10 before the Boolean pilot is validated.</li>
  <li>Do not generalize the Deriver, Lowerer, or ContractEmitter broadly in this wave.</li>
  <li>Do not add LLVM coverage for the new scalar widget examples in this wave.</li>
  <li>Do not weaken Examples 01–05 or Example 05 native-kernel bridge coverage.</li>
  <li>Keep each new example bounded, explicit, and independently checkable.</li>
</ul>

<hr/>

<h2>Official Widget-Example Validation Rule</h2>

<p>
A widget example is not valid merely because an HTML page contains the expected widget IDs, layout numbers, and runtime values.
The visible front panel must be derived from the published <code>.wfrog</code> package and from the realization assets or templates declared by that package and its referenced default realization manifest.
</p>

<p>
An official widget-example checker must fail when a visible widget declares <code>visual.asset_ref = asset:&lt;id&gt;</code> but the rendered front panel does not visibly use that asset route or asset content.
The checker must also fail when a <code>.wfrog</code> package declares <code>realization_refs</code> but the referenced realization manifest is missing, does not target the widget class IDs used by the example, or does not publish the assets required by the example package.
</p>

<p>
A fallback renderer may exist for narrow debugging, but it must be explicitly marked as a fallback and must not pass official widget-example validation.
The following are not sufficient for official validation:
</p>

<ul>
  <li>hardcoded CSS-only widget faces,</li>
  <li>handcrafted cards that bypass <code>visual.asset_ref</code>,</li>
  <li>native controls used as the visible widget body when the <code>.wfrog</code> instance declares an SVG asset,</li>
  <li>snapshot-only validation without front-panel asset fidelity checks.</li>
</ul>

<p>
Runtime snapshots and diagnostics remain useful, but they must be exposed through debug surfaces such as <code>/state.json</code>.
Raw runtime snapshots must not be visible in the normal user-facing front panel by default.
</p>

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
      <td>runtime/UI-binding only</td>
      <td>pilot</td>
    </tr>
    <tr>
      <td><code>07_string_value_roundtrip</code></td>
      <td><code>frog.widgets.string_control</code> → <code>frog.widgets.string_indicator</code></td>
      <td>copy text value</td>
      <td>future runtime/UI-binding only</td>
      <td>planned</td>
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
  <li>The rendered front panel visibly uses the assets declared by <code>visual.asset_ref</code>.</li>
  <li>The rendered front panel exposes the expected asset routes for the declared Boolean assets.</li>
  <li>The user-facing front panel is clean and does not show the runtime snapshot by default.</li>
  <li>A <code>state.json</code>-style diagnostic surface may remain available for validation and debug.</li>
  <li>No LLVM artifact is required for this pilot.</li>
</ul>

<hr/>

<h2>Compiler and Runtime Posture</h2>

<p>
The scalar widget pilot examples do not make the runtime LLVM-only and do not move LLVM coverage away from Example 05.
They validate front-panel package consumption, widget value binding, visible realization-asset use, and bounded runtime behavior.
If later scalar kernels receive native compiler-family coverage, that work should be introduced through the same manifest-driven boundary used by Example 05.
</p>

<hr/>

<h2>Future Work</h2>

<ul>
  <li>Promote Example 06 into the shared pipeline only after its dedicated check is stable.</li>
  <li>Add String, Enum, Path, and Button examples one at a time.</li>
  <li>Only then consider a narrow scalar widget runtime abstraction.</li>
  <li>Keep Array, Cluster, Table, Tree, Tab, Listbox, Picture, Chart, Splitter, Panel/SubPanel, Frame, Label, and Decorations for later waves.</li>
</ul>
