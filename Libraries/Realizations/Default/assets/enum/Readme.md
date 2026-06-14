<p align="center">
  <img src="../../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Enum Realization Assets</h1>

<p align="center">
  <strong>SVG template assets for the default enum realization variant</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="../../Enum.md">Default enum realization posture</a></li>
  <li><a href="../../enum.default.wfrog">Machine-readable enum realization manifest</a></li>
  <li><a href="../../Package.md">Default realization package posture</a></li>
  <li><a href="../../../../Widgets/Enum.md">Enum widget class law</a></li>
</ul>

<hr/>

<h2>1. Published Assets</h2>

<ul>
  <li><code>templates/enum_rectangular_ring.svg</code> - compact rectangular enum value template.</li>
  <li><code>templates/enum_dropdown_list.svg</code> - opened dropdown list skin used by the host surface.</li>
</ul>

<hr/>

<h2>2. Closed Value Markers</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>value_face</code></li>
  <li><code>value_display</code></li>
  <li><code>focus_ring</code></li>
  <li><code>spinner</code> when increment/decrement is shown</li>
  <li><code>increment_up</code> when increment/decrement is shown</li>
  <li><code>increment_down</code> when increment/decrement is shown</li>
</ul>

<p>
The closed value skin no longer publishes a selector face or arrow. The
<code>focus_ring</code> is public so the SVG owns the closed value focus
boundary while the runtime only toggles and styles it.
</p>

<h2>3. Dropdown List Markers</h2>

<ul>
  <li><code>root</code></li>
  <li><code>list_panel</code></li>
  <li><code>option_row</code></li>
  <li><code>option_text</code></li>
</ul>

<p>
The dropdown list is still a host surface because item count and interaction
state are dynamic. Its visible panel, row, and text skin are published in the
dropdown SVG so the runtime can repeat <code>option_row</code> and
<code>option_text</code> for the enum item inventory.
</p>

<p>
Dropdown placement and sizing are declared in the realization
<code>.wfrog</code>, not in runtime-specific code. The Default Enum dropdown
surface anchors to <code>value_face</code>, uses its outer width with a
declared source-width outset, and centers under the body.
</p>
