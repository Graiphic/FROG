<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Version Matrix</h1>

<p align="center">
  <strong>Centralized current-status table for the published FROG specification corpus</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This matrix summarizes the current repository-visible status of major FROG specification and reference surfaces.
It does not define document-specific versions.
Version governance remains centralized in this directory and in the repository history.
</p>

<hr/>

<h2>Executable Example Closure Matrix</h2>

<table>
  <thead>
    <tr>
      <th>Example</th>
      <th>Source</th>
      <th>FIR</th>
      <th>Lowering</th>
      <th>Contract</th>
      <th>Runtime acceptance</th>
      <th>LLVM proof</th>
      <th>Current note</th>
    </tr>
  </thead>
  <tbody>
    <tr><td>01 Pure Addition</td><td>Published</td><td>Published</td><td>Published</td><td>Common surface</td><td>Published</td><td>Published</td><td>Pure arithmetic corridor.</td></tr>
    <tr><td>02 UI Value Roundtrip</td><td>Published</td><td>Published</td><td>Published</td><td>Common surface</td><td>Published</td><td>Published</td><td>Natural <code>widget_value</code> corridor.</td></tr>
    <tr><td>03 UI Property Write</td><td>Published</td><td>Published</td><td>Published</td><td>Common surface</td><td>Published</td><td>Published</td><td>Object-style <code>widget_reference</code> / <code>property_write</code> corridor.</td></tr>
    <tr><td>04 Stateful Feedback Delay</td><td>Published</td><td>Published</td><td>Published</td><td>Common surface</td><td>Published</td><td>Published</td><td>Explicit delay-state corridor.</td></tr>
    <tr><td>05 Bounded UI Accumulator</td><td>Published</td><td>Published</td><td>Published</td><td>Common surface</td><td>Published</td><td>Published</td><td>Primary applicative UI/state/runtime/native reference corridor.</td></tr>
  </tbody>
</table>

<hr/>

<h2>Reference Rule-Handoff Matrix</h2>

<table>
  <thead>
    <tr>
      <th>Stage</th>
      <th>Input authority</th>
      <th>Dispatch surface</th>
      <th>Output surface</th>
      <th>Reference documentation</th>
      <th>Current status</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>.frog -&gt; FIR</code></td>
      <td>Canonical source pattern</td>
      <td>recognized source-pattern rule</td>
      <td><code>FIR units[0].kind</code></td>
      <td><code>Implementations/Reference/Deriver/rules/Readme.md</code></td>
      <td>Published and tested</td>
    </tr>
    <tr>
      <td><code>FIR -&gt; lowering</code></td>
      <td><code>frog_fir_unit</code></td>
      <td><code>FIR units[0].kind</code></td>
      <td><code>lowered_units[0].kind</code></td>
      <td><code>Implementations/Reference/Lowerer/rules/Readme.md</code></td>
      <td>Published and tested</td>
    </tr>
    <tr>
      <td><code>lowering -&gt; backend contract</code></td>
      <td><code>frog_lowered_unit</code></td>
      <td><code>lowered_units[0].kind</code></td>
      <td><code>contract.units[0].kind</code></td>
      <td><code>Implementations/Reference/ContractEmitter/</code></td>
      <td>Published and tested</td>
    </tr>
    <tr>
      <td><code>backend contract -&gt; runtime</code></td>
      <td><code>frog_backend_contract</code></td>
      <td><code>contract.units[0].kind</code></td>
      <td>runtime snapshot</td>
      <td><code>Implementations/Reference/Runtime/Readme.md</code></td>
      <td>Published and tested</td>
    </tr>
    <tr>
      <td><code>lowering -&gt; LLVM</code></td>
      <td><code>frog_lowered_unit</code></td>
      <td><code>lowered_units[0].kind</code></td>
      <td><code>module.ll</code></td>
      <td><code>Implementations/Reference/LLVM/rules/Readme.md</code></td>
      <td>Published and tested</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Current FIR, Lowering, Contract, Runtime, and LLVM Kind Matrix</h2>

<table>
  <thead>
    <tr>
      <th>Example</th>
      <th>Derivation rule</th>
      <th>FIR unit kind</th>
      <th>Lowering rule</th>
      <th>Lowered unit kind</th>
      <th>Contract unit kind</th>
      <th>Runtime executor kind</th>
      <th>LLVM proof emitter</th>
    </tr>
  </thead>
  <tbody>
    <tr><td>01 Pure Addition</td><td><code>pure_public_addition</code></td><td><code>pure_dataflow_arithmetic_unit</code></td><td><code>lower_pure_dataflow_arithmetic</code></td><td><code>pure_addition_kernel</code></td><td><code>pure_addition_kernel</code></td><td><code>pure_addition_kernel</code></td><td><code>pure_addition_kernel</code></td></tr>
    <tr><td>02 UI Value Roundtrip</td><td><code>ui_value_roundtrip</code></td><td><code>ui_value_roundtrip_unit</code></td><td><code>lower_ui_value_roundtrip</code></td><td><code>ui_value_roundtrip_kernel</code></td><td><code>ui_value_roundtrip_kernel</code></td><td><code>ui_value_roundtrip_kernel</code></td><td><code>ui_value_roundtrip_kernel</code></td></tr>
    <tr><td>03 UI Property Write</td><td><code>ui_property_write</code></td><td><code>ui_property_write_unit</code></td><td><code>lower_ui_property_write</code></td><td><code>ui_property_write_effect_unit</code></td><td><code>ui_property_write_effect_unit</code></td><td><code>ui_property_write_effect_unit</code></td><td><code>ui_property_write_effect_unit</code></td></tr>
    <tr><td>04 Stateful Feedback Delay</td><td><code>stateful_feedback_delay</code></td><td><code>stateful_feedback_delay_unit</code></td><td><code>lower_stateful_feedback_delay</code></td><td><code>stateful_feedback_delay_kernel</code></td><td><code>stateful_feedback_delay_kernel</code></td><td><code>stateful_feedback_delay_kernel</code></td><td><code>stateful_feedback_delay_kernel</code></td></tr>
    <tr><td>05 Bounded UI Accumulator</td><td><code>bounded_ui_accumulator</code></td><td><code>bounded_stateful_ui_unit</code></td><td><code>lower_bounded_stateful_ui</code></td><td><code>bounded_accumulator_kernel_with_ui_bindings</code></td><td><code>bounded_executable_ui_unit</code></td><td><code>bounded_executable_ui_unit</code></td><td><code>bounded_accumulator_kernel_with_ui_bindings</code></td></tr>
  </tbody>
</table>

<hr/>

<h2>Backend Contract Common Surface</h2>

<p>
Examples <code>01</code> through <code>05</code> share the common backend-contract unit surface:
</p>

<pre><code>public_io
ui_bindings
execution_kernel
effects
publications
</code></pre>

<hr/>

<h2>Widget Class-Law Matrix</h2>

<table>
  <thead>
    <tr>
      <th>Family</th>
      <th>Class-law document</th>
      <th>Classes</th>
      <th>Current status</th>
      <th>Notes</th>
    </tr>
  </thead>
  <tbody>
    <tr><td>Boolean</td><td><code>Libraries/Widgets/Boolean.md</code></td><td><code>frog.widgets.boolean_control</code>, <code>frog.widgets.boolean_indicator</code></td><td>Published</td><td>Scalar control / indicator baseline.</td></tr>
    <tr><td>String</td><td><code>Libraries/Widgets/String.md</code></td><td><code>frog.widgets.string_control</code>, <code>frog.widgets.string_indicator</code></td><td>Published</td><td>Scalar text control / indicator baseline.</td></tr>
    <tr><td>Button</td><td><code>Libraries/Widgets/Button.md</code></td><td><code>frog.widgets.button</code></td><td>Published</td><td>Command widget baseline.</td></tr>
    <tr><td>Numeric</td><td><code>Libraries/Widgets/Numeric.md</code></td><td><code>frog.widgets.numeric_control</code>, <code>frog.widgets.numeric_indicator</code></td><td>Published</td><td>Scalar numeric control / indicator baseline.</td></tr>
    <tr><td>Enum</td><td><code>Libraries/Widgets/Enum.md</code></td><td><code>frog.widgets.enum_control</code>, <code>frog.widgets.enum_indicator</code></td><td>Published</td><td>Scalar enumeration control / indicator baseline.</td></tr>
    <tr><td>Path</td><td><code>Libraries/Widgets/Path.md</code></td><td><code>frog.widgets.path_control</code>, <code>frog.widgets.path_indicator</code></td><td>Published</td><td>Path value control / indicator baseline.</td></tr>
    <tr><td>Picture</td><td><code>Libraries/Widgets/Picture.md</code></td><td><code>frog.widgets.picture_control</code>, <code>frog.widgets.picture_indicator</code></td><td>Published</td><td>Image payload, pixel metadata, viewport, display, overlay, and pointer posture.</td></tr>
    <tr><td>Waveform Chart</td><td><code>Libraries/Widgets/Chart.md</code></td><td><code>frog.widgets.waveform_chart</code></td><td>Published</td><td>Composite chart class law: history, plots, scales, legends, cursors, palette, scrollbars, overlays.</td></tr>
    <tr><td>Listbox</td><td><code>Libraries/Widgets/Listbox.md</code></td><td><code>frog.widgets.listbox_control</code>, <code>frog.widgets.listbox_indicator</code></td><td>Published</td><td>Item identity, selection, active item, viewport, search/filter, and public item parts.</td></tr>
    <tr><td>Tab</td><td><code>Libraries/Widgets/Tab.md</code></td><td><code>frog.widgets.tab_control</code>, <code>frog.widgets.tab_indicator</code></td><td>Published</td><td>Page identity, selected-page value, header posture, overflow, close/reorder, and page surfaces.</td></tr>
    <tr><td>Tree</td><td><code>Libraries/Widgets/Tree.md</code></td><td><code>frog.widgets.tree_control</code>, <code>frog.widgets.tree_indicator</code></td><td>Published</td><td>Finite hierarchy, node identity, selection, expansion, check state, editing, drag/drop, and node parts.</td></tr>
    <tr><td>Table</td><td><code>Libraries/Widgets/Table.md</code></td><td><code>frog.widgets.table_control</code>, <code>frog.widgets.table_indicator</code></td><td>Published</td><td>Flat tabular value, schema, rows, cells, selection, editing, sorting, filtering, and grid parts.</td></tr>
    <tr><td>Array</td><td><code>Libraries/Widgets/Array.md</code></td><td><code>frog.widgets.array</code></td><td>Published</td><td>Homogeneous collection widget baseline.</td></tr>
    <tr><td>Cluster</td><td><code>Libraries/Widgets/Cluster.md</code></td><td><code>frog.widgets.cluster</code></td><td>Published</td><td>Heterogeneous aggregate widget baseline.</td></tr>
    <tr><td>Label</td><td><code>Libraries/Widgets/Label.md</code></td><td><code>frog.widgets.label</code></td><td>Published</td><td>Support widget baseline.</td></tr>
    <tr><td>Frame</td><td><code>Libraries/Widgets/Frame.md</code></td><td><code>frog.widgets.frame</code></td><td>Published</td><td>Support widget baseline.</td></tr>
    <tr><td>Decorations</td><td><code>Libraries/Widgets/Decorations.md</code></td><td><code>frog.widgets.flat_box</code>, <code>frog.widgets.horizontal_line</code>, <code>frog.widgets.vertical_line</code></td><td>Published</td><td>Decorative non-value baseline.</td></tr>
    <tr><td>Splitter</td><td><code>Libraries/Widgets/Splitter.md</code></td><td><code>frog.widgets.horizontal_splitter</code>, <code>frog.widgets.vertical_splitter</code></td><td>Published</td><td>Layout baseline.</td></tr>
    <tr><td>Panel/SubPanel</td><td><code>Libraries/Widgets/Panel.md</code></td><td><code>frog.widgets.panel</code>, <code>frog.widgets.subpanel</code></td><td>Published</td><td>Container baseline.</td></tr>
  </tbody>
</table>

<hr/>

<h2>Default Realization Matrix</h2>

<table>
  <thead>
    <tr>
      <th>Family</th>
      <th>Default document</th>
      <th>Manifest</th>
      <th>Assets</th>
      <th>Current status</th>
    </tr>
  </thead>
  <tbody>
    <tr><td>Boolean</td><td><code>Libraries/Realizations/Default/Boolean.md</code></td><td><code>boolean.default.wfrog</code></td><td>SVG-backed scalar resources</td><td>Published</td></tr>
    <tr><td>String</td><td><code>Libraries/Realizations/Default/String.md</code></td><td><code>string.default.wfrog</code></td><td>SVG-backed scalar resources</td><td>Published</td></tr>
    <tr><td>Button</td><td><code>Libraries/Realizations/Default/Button.md</code></td><td><code>button.default.wfrog</code></td><td>SVG-backed command resources</td><td>Published</td></tr>
    <tr><td>Numeric</td><td><code>Libraries/Realizations/Default/Numeric.md</code></td><td><code>numeric.default.wfrog</code></td><td>SVG-backed scalar resources</td><td>Published</td></tr>
    <tr><td>Enum</td><td><code>Libraries/Realizations/Default/Enum.md</code></td><td><code>enum.default.wfrog</code></td><td>SVG-backed scalar resources</td><td>Published</td></tr>
    <tr><td>Path</td><td><code>Libraries/Realizations/Default/Path.md</code></td><td><code>path.default.wfrog</code></td><td>SVG-backed scalar resources</td><td>Published</td></tr>
    <tr><td>Picture</td><td><code>Libraries/Realizations/Default/Picture.md</code></td><td><code>picture.default.wfrog</code></td><td><code>assets/picture/</code></td><td>Published rectangular image-surface realization.</td></tr>
    <tr><td>Waveform Chart</td><td><code>Libraries/Realizations/Default/Chart.md</code></td><td><code>chart.default.wfrog</code></td><td><code>assets/chart/</code></td><td>Published composite realization with shell, subobjects, composition slots, bindings, and host-native plot allowance.</td></tr>
    <tr><td>Listbox</td><td><code>Libraries/Realizations/Default/Listbox.md</code></td><td><code>listbox.default.wfrog</code></td><td><code>assets/listbox/</code></td><td>Published composite item-list realization with virtual-list host allowance.</td></tr>
    <tr><td>Tab</td><td><code>Libraries/Realizations/Default/Tab.md</code></td><td><code>tab.default.wfrog</code></td><td><code>assets/tab/</code></td><td>Published composite page-navigation realization with page-hosting surfaces.</td></tr>
    <tr><td>Tree</td><td><code>Libraries/Realizations/Default/Tree.md</code></td><td><code>tree.default.wfrog</code></td><td><code>assets/tree/</code></td><td>Published composite hierarchical realization with virtual-tree host allowance.</td></tr>
    <tr><td>Table</td><td><code>Libraries/Realizations/Default/Table.md</code></td><td><code>table.default.wfrog</code></td><td><code>assets/table/</code></td><td>Published composite tabular realization with virtual-grid host allowance.</td></tr>
    <tr><td>Array</td><td><code>Libraries/Realizations/Default/Array.md</code></td><td><code>array.default.wfrog</code></td><td>SVG-backed structured resources</td><td>Published</td></tr>
    <tr><td>Cluster</td><td><code>Libraries/Realizations/Default/Cluster.md</code></td><td><code>cluster.default.wfrog</code></td><td>SVG-backed structured resources</td><td>Published</td></tr>
    <tr><td>Label</td><td><code>Libraries/Realizations/Default/Label.md</code></td><td><code>label.default.wfrog</code></td><td>SVG-backed support resources</td><td>Published</td></tr>
    <tr><td>Frame</td><td><code>Libraries/Realizations/Default/Frame.md</code></td><td><code>frame.default.wfrog</code></td><td>SVG-backed support resources</td><td>Published</td></tr>
    <tr><td>Decorations</td><td><code>Libraries/Realizations/Default/Decorations.md</code></td><td><code>decorations.default.wfrog</code></td><td>SVG-backed decorative resources</td><td>Published</td></tr>
    <tr><td>Splitter</td><td><code>Libraries/Realizations/Default/Splitter.md</code></td><td><code>splitter.default.wfrog</code></td><td>SVG-backed layout resources</td><td>Published</td></tr>
    <tr><td>Panel/SubPanel</td><td><code>Libraries/Realizations/Default/Panel.md</code></td><td><code>panel.default.wfrog</code></td><td>SVG-backed container resources</td><td>Published</td></tr>
  </tbody>
</table>

<hr/>

<h2>Reference Validation Matrix</h2>

<table>
  <thead>
    <tr>
      <th>Surface</th>
      <th>Reference path</th>
      <th>Current status</th>
      <th>Validation role</th>
    </tr>
  </thead>
  <tbody>
    <tr><td>Example corridor checks</td><td><code>Implementations/Reference/Pipeline/check_examples01_05_full.py</code></td><td>Published</td><td>Protects Examples 01–05 source / FIR / lowering / contract / runtime / LLVM corridor.</td></tr>
    <tr><td>Reference workspace checks</td><td><code>Implementations/Reference/check_reference_workspace.py</code></td><td>Published</td><td>Repository-wide reference workspace hygiene check.</td></tr>
    <tr><td>Reference pytest checks</td><td><code>Implementations/Reference/check_reference_workspace.py --include-pytest</code></td><td>Published</td><td>Protects source-pattern derivation, FIR-kind lowering, lowered-kind contract emission, contract-kind runtime execution, lowered-kind LLVM emission, and unsupported-pattern / unsupported-kind failure behavior.</td></tr>
    <tr><td>Widget layer validator</td><td><code>Implementations/Reference/WidgetValidator/validate_widget_layer.py</code></td><td>Published and strengthened</td><td>Protects widget docs, Default docs, manifests, target classes, resources, SVG part markers, composition references, bindings, and public-part alignment.</td></tr>
  </tbody>
</table>

<hr/>

<h2>Current Widget-Layer Boundary</h2>

<pre><code>Libraries/Widgets/
  -&gt; widget class law

Libraries/Realizations/Default/*.md
  -&gt; Default realization explanation

Libraries/Realizations/Default/*.default.wfrog
  -&gt; machine-readable Default realization manifests

Libraries/Realizations/Default/assets/
  -&gt; SVG templates and subobject resources

Implementations/Reference/WidgetValidator/
  -&gt; non-normative repository hygiene validation
</code></pre>

<p>
The widget layer is organized as a standard-facing surface rather than a collection of isolated documents.
The current implementation priority remains to keep the Examples 01–05 executable corridor, reference checks, widget validation, and version-governance posture coherent before broadening into new examples or new widget families.
</p>
