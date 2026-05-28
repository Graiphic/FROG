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

<p>
The shell <code>frame</code> part is the outer Array container frame. It is
separate from the <code>element_region</code> viewport and from any frame
published by a contained element widget such as Numeric. A visible
<code>index_display</code> is a neighboring Array subcontainer with its own
source-owned surface and a small source-owned gap to the element viewport.
This keeps the LabVIEW-like index display and element viewport visually
separate without hardcoding the contained element visuals.
Its optional container fill, border, border width, and padding are instance
properties under <code>style.index_display.*</code>.
</p>

<p>
The <code>index_display</code> subobject publishes the index value and arrow
surfaces. Per-instance spacing between the arrows and between the arrows and
the value surface is owned by <code>style.index_display.step_gap</code> and
<code>style.index_display.value_gap</code>.
</p>

<p>
For widget-backed arrays, repeated contained widgets may be packed with zero
padding and zero gap. In that posture, adjacent contained widget frames touch
or visually superpose at their shared bounds. The Default Array assets do not
publish a separate per-cell separator stroke for that case; the Array shell
owns only the outer collection frame, viewport slots, scrollbars, and index
display subcontainer.
</p>
