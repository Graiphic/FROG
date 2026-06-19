<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 22 - Listbox Selection Roundtrip</h1>

<p align="center">
  <strong>Listbox numeric selected value through Default SVG assets</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This example introduces a dedicated Listbox widget after the current public
reference runtime closure. It keeps the public source, FIR/lowering,
realization package, Default SVG asset reference, and native manifest surface
visible while Graiphic runtime implementation continues privately.
</p>

<p>
The front panel contains one <code>frog.widgets.listbox_control</code> and one
<code>frog.widgets.listbox_indicator</code>. The control lets the user select
one source-owned item. The visible text is a listbox row label, while the
diagram-facing value is the selected item's numeric <code>u16</code> value.
The explicit Execute action publishes that numeric value through the
manifest-backed native proof and updates the read-only Listbox indicator.
</p>

<hr/>

<h2>Source / Realization Boundary</h2>

<ul>
  <li><code>.frog</code> owns the Listbox instances, item ids, item labels, numeric item values, selected value, selection mode, viewport, captions, layout, bindings, and instance-level style overrides.</li>
  <li><code>.wfrog</code> references the Default Listbox realization package, SVG asset, host capability declarations, and reusable default widget properties for the accepted Listbox visual posture.</li>
  <li>The Default Listbox SVG asset publishes <code>placement_bounds</code>, <code>main_body</code>, <code>item_region</code>, <code>item_row</code>, <code>item_icon</code>, <code>item_label</code>, <code>selection_face</code>, <code>hover_face</code>, <code>disabled_face</code>, <code>vertical_scrollbar</code>, <code>horizontal_scrollbar</code>, <code>caption</code>, <code>label</code>, and <code>focus_ring</code> parts.</li>
  <li>The runtime consumes the manifest-backed native <code>u16</code> artifact and binds host overlays to the published SVG parts.</li>
</ul>

<hr/>

<h2>Native Proof</h2>

<p>
The LLVM/native artifact is intentionally a numeric identity proof:
</p>

<pre><code>selected_item_value = item_value</code></pre>

<p>
The Listbox widget owns selected-item UI state and item records. The diagram
sees the selected numeric value, while the runtime maps that value back to the
matching source-owned Listbox item for display.
</p>

<hr/>

<h2>Runtime Status</h2>

<p>
This example is repository-visible public specification material beyond the
current public reference runtime closure. Its private implementation is aligned
across the C++, Python, and Rust families in <code>Graiphic/FROG-Runtime</code>
after C++ visual and behavioral acceptance.
</p>
