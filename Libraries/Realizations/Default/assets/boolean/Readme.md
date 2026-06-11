<p align="center">
  <img src="../../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Boolean Realization Assets</h1>

<p align="center">
  <strong>SVG template assets for the default boolean realization variants</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="../../Boolean.md">Default boolean realization posture</a></li>
  <li><a href="../../boolean.default.wfrog">Machine-readable boolean realization manifest</a></li>
  <li><a href="../../Package.md">Default realization package posture</a></li>
  <li><a href="../../../../Widgets/Boolean.md">Boolean widget class law</a></li>
</ul>

<hr/>

<h2>1. Purpose</h2>

<p>
This directory contains SVG skins for the default Boolean realization family.
The assets do not define the Boolean classes; the class law belongs to
<code>Libraries/Widgets/Boolean.md</code>.
</p>

<hr/>

<h2>2. Published Assets</h2>

<ul>
  <li><code>templates/boolean_rectangular.svg</code> - square/rectangular LED skin.</li>
  <li><code>templates/boolean_circular.svg</code> - round/oval LED skin.</li>
</ul>

<p>
This directory also includes review-only colorized overlays:
</p>

<ul>
  <li><code>templates/boolean_rectangular.review.svg</code> - semantic-part review overlay for the rectangular variant.</li>
  <li><code>templates/boolean_circular.review.svg</code> - semantic-part review overlay for the circular variant.</li>
</ul>

<p>
The <code>*.review.svg</code> files are human inspection helpers. They keep the
same public <code>data-frog-part</code> labels and geometry as the production
templates, but colorize each semantic part so reviewers can inspect the skin
contract quickly. They are not referenced by <code>boolean.default.wfrog</code>
and must not be consumed by production runtimes.
</p>

<hr/>

<h2>3. Public Template Parts</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>placement_bounds</code> - non-rendered IDE/container placement aura.</li>
  <li><code>state_face</code> - visible Boolean true/false state face.</li>
  <li><code>state_text</code> - optional centered On/Off text.</li>
  <li><code>focus_ring</code> - optional outline following <code>state_face</code>.</li>
</ul>

<p>
The production assets intentionally do not publish <code>outer_face</code>,
<code>inner_face</code>, or <code>frame</code>.
</p>

<p>
Both production SVGs use the same default geometry contract:
<code>placement_bounds</code> is 72 x 72 source units, <code>state_face</code>
is 64 x 64 source units, and the nominal aura band is 4 source units around
the visible Boolean body. This matches the Default Numeric aura standard.
The review overlays colorize those sections for human inspection only; runtime
binding continues to use the production SVG parts above.
</p>

<hr/>

<h2>4. Realization Role</h2>

<p>
Both assets target <code>frog.widgets.boolean_control</code> and
<code>frog.widgets.boolean_indicator</code>. They are realization variants,
not separate widget classes.
</p>

<hr/>

<h2>5. Ownership Boundary</h2>

<p>
The assets may define geometry, template markers, SVG groups, CSS variables,
and fallback visual defaults. They must not redefine the meaning of
<code>value</code>, the control-versus-indicator distinction, or the public
Boolean property, method, and event inventory.
</p>
