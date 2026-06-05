<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 21 - Ring Selection Roundtrip</h1>

<p align="center">
  <strong>Ring numeric selected value through Default SVG assets</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<p>
Reviewed 2026-06-05: the example consumes the Default Ring closed selector SVG
and dropdown list SVG skin. The dropdown list aligns to the value face only,
not the selector arrow, the Ring value face uses a 1px border, and the visible
selected-item text is vertically aligned through source-owned
<code>style.value_display.vertical_offset</code>.
</p>

<p>
The accepted Ring visual posture is now a reusable default:
<code>ui/ring_panel.wfrog</code> declares <code>default_widget_properties</code>
for the control and indicator roles, and the C++ runtime merges those defaults
before any <code>.frog</code> instance-level overrides.
</p>

<hr/>

<h2>Purpose</h2>

<p>
This example introduces a dedicated Ring widget after the current public
reference runtime closure. It keeps the public source, FIR/lowering,
realization package, Default SVG asset/default references, and native manifest surface
visible while Graiphic runtime implementation continues privately.
</p>

<p>
The front panel contains one <code>frog.widgets.ring_control</code> and one
<code>frog.widgets.ring_indicator</code>. The control lets the user select one
source-owned item. The visible text is a Ring string, while the diagram-facing
value is the selected item's numeric <code>u16</code> value. The explicit
Execute action publishes that numeric value through the manifest-backed native
proof and updates the read-only Ring indicator.
</p>

<hr/>

<h2>Source / Realization Boundary</h2>

<ul>
  <li><code>.frog</code> owns the Ring instances, item ids, item labels, numeric item values, selected value, representation, captions, layout, bindings, and instance-level style overrides.</li>
  <li><code>.wfrog</code> references the Default Ring realization package, SVG assets, host capability declarations, dropdown host-surface layout, and reusable default widget properties.</li>
  <li>The Default Ring closed selector SVG publishes <code>root</code>, <code>label</code>, <code>caption</code>, <code>value_face</code>, <code>value_display</code>, and <code>selector_face</code>. The arrow glyph is a private detail owned by <code>selector_face</code>, not a public part.</li>
  <li>The Default Ring dropdown SVG publishes <code>root</code>, <code>list_panel</code>, <code>option_row</code>, <code>option_label</code>, and <code>option_selection_face</code>.</li>
  <li>The runtime consumes the manifest-backed native <code>u16</code> artifact and binds host overlays to the published SVG parts.</li>
</ul>

<hr/>

<h2>Ring Versus Enum</h2>

<p>
Ring and Enum may share a compact selector visual posture, but they are not the
same widget class. An Enum owns a typed symbolic domain and an explicit integer
representation. A Ring owns a source-defined list of display strings and values.
Its LabVIEW-like default posture publishes the selected numeric value while the
item id remains UI identity.
</p>

<hr/>

<h2>Native Proof</h2>

<p>
The LLVM/native artifact is intentionally a numeric identity proof:
</p>

<pre><code>selected_mode_value = mode_value</code></pre>

<p>
The Ring widget owns selected-item UI state and item records. The diagram sees
the selected numeric value, while the runtime maps that value back to the
matching source-owned Ring item for display.
</p>

<hr/>

<h2>Runtime Status</h2>

<p>
This example is repository-visible public specification material beyond the
current public reference runtime closure. Its private implementation is aligned
across the C++, Python, and Rust families in <code>Graiphic/FROG-Runtime</code>
after C++ visual and behavioral acceptance.
</p>
