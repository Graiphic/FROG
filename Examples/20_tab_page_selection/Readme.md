<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 20 - Tab Page Selection</h1>

<p align="center">
  <strong>Tab control page selection through Default SVG assets</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This example introduces a Tab control after the current public reference
runtime closure. It keeps the public source, FIR/lowering, realization package,
Default SVG asset reference, and native manifest surface visible while Graiphic
runtime implementation continues privately.
</p>

<p>
The front panel contains one <code>frog.widgets.tab_control</code> with five
source-owned pages. Each page selector owns its tab-item position and size so
the header can stay on one row when the available Default SVG header region is
wide enough. Wrapping remains a declared constrained-width posture, not a
runtime-private default. Clicking a tab or using the movable page label display
changes the selected page id; the diagram publishes that selected id to the
public output.
</p>

<hr/>

<h2>Source / Realization Boundary</h2>

<ul>
  <li><code>.frog</code> owns the Tab instance, page ids, page titles, selected page id, page content text, per-page tab-item position/size, selected-row placement policy, selected-tab and selected-row join styling, header sizing/wrapping policy, page label display layout/style, and widget layout.</li>
  <li><code>.wfrog</code> references the Default Tab realization package, SVG asset, and host capability declarations.</li>
  <li>The Default Tab SVG asset publishes <code>tab_header_region</code>, <code>tab_item</code>, <code>tab_item_label</code>, <code>selection_face</code>, <code>page_label_display</code>, <code>page_region</code>, <code>page_container</code>, and <code>page_content</code> parts.</li>
  <li>The runtime consumes the manifest-backed native string artifact and binds host overlays to the published SVG parts.</li>
</ul>

<hr/>

<h2>Native Proof</h2>

<p>
The LLVM/native artifact is intentionally a string identity proof:
</p>

<pre><code>selected_page_id_out = selected_page_id</code></pre>

<p>
The Tab widget owns selected-page value semantics. Page content is front-panel
presentation data owned by the <code>.frog</code> instance.
</p>

<p>
The browser host may update the selected page through a small state patch instead
of reloading the whole document. That interaction is declared as a host
capability; it does not change the <code>.frog</code> value contract, the
Default SVG realization, or the native manifest handoff.
</p>

<hr/>

<h2>Runtime Status</h2>

<p>
This example is repository-visible public specification material beyond the
current public reference runtime closure. It is introduced C++-first in
<code>Graiphic/FROG-Runtime</code>. Python and Rust parity should be added only
after the C++ visual and behavioral posture is accepted.
</p>
