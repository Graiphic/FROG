<p align="center">
  <img src="../../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Array Realization Assets</h1>

<p align="center">
  <strong>SVG shell and subobject assets for the default array realization</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Published Assets</h2>

<ul>
  <li><code>templates/array_shell.svg</code> — shell and layout slots.</li>
  <li><code>subobjects/index_display.svg</code> — index display subobject.</li>
  <li><code>subobjects/element_region.svg</code> — repeated element viewport.</li>
  <li><code>subobjects/element_slot.svg</code> — generic element slot placeholder.</li>
  <li><code>subobjects/vertical_scrollbar.svg</code> — vertical scrollbar.</li>
  <li><code>subobjects/horizontal_scrollbar.svg</code> — horizontal scrollbar.</li>
</ul>

<p>
The actual element behavior is owned by the element widget class, not by these array realization assets.
The <code>.frog</code> instance owns rank, shape, viewport size, scrollbar gap,
index values, element records, and per-instance style overrides.
The Default SVG assets publish parts and baseline geometry only.
</p>
