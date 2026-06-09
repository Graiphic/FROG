<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 41 UI Package</h1>

<p align="center">
  <strong>Default Numeric IDE placement package and SVG realization references</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Overview</h2>

<p>
This directory contains the Example 41 <code>.wfrog</code> package used by the
C++ browser host to inspect Default Numeric placement behavior. The package
references the shared Default Numeric realization and SVG asset; it does not
own Numeric class semantics or execution behavior.
</p>

<hr/>

<h2>Directory Shape</h2>

<pre><code>Examples/41_numeric_placement_ide_view/ui/
├── Readme.md
└── numeric_ide_panel.wfrog

Libraries/Realizations/Default/assets/numeric/templates/
└── numeric_rectangular.svg
</code></pre>

<hr/>

<h2>Package Role</h2>

<pre><code>main.frog
  -> owns panel instances, placement, labels, and selected IDE witness

numeric_ide_panel.wfrog
  -> owns Example 41 package publication
  -> resolves Default Numeric realization package
  -> resolves numeric_rectangular.svg

C++ browser host
  -> renders grid because canvas.grid.visible=true
  -> renders selection aura from placement_bounds
  -> renders label aura from caption
</code></pre>

<hr/>

<h2>Layout Contract</h2>

<pre><code>main_panel:
  width: 500
  height: 240
  coordinate_space: panel_pixels
  presentation_mode: ide_view
  grid.visible: true
  grid.pitch: 16
  grid.origin: { x: 0, y: 0 }
  grid.snap: placement_bounds

right-button control / indicator:
  y: 32

left-button control / indicator:
  y: 96

hidden-button control / indicator:
  y: 160
</code></pre>

<p>
All six widgets use <code>layout.origin=placement_bounds.top_left</code> and
<code>layout.bounds_ref=placement_bounds</code>. This is the portable aura
whose top-left snaps to the shared placement grid. Its width is controlled by
grid point-to-point movement; its height follows the visible body plus the
declared uniform aura band. The visible Numeric body remains inside that aura
and is aligned through the SVG semantic parts.
</p>

<hr/>

<h2>SVG Parts Consumed</h2>

<ul>
  <li><code>placement_bounds</code> - portable widget aura, top-left grid-snapped and width grid-controlled,</li>
  <li><code>caption</code> - source-owned label/caption anchor and IDE label aura,</li>
  <li><code>control_body</code> - centered visible body for controls,</li>
  <li><code>indicator_body</code> - centered visible body for indicators,</li>
  <li><code>value_face</code> - visible value surface and border,</li>
  <li><code>text_value</code> - runtime value text anchor,</li>
  <li><code>increment_up</code> and <code>increment_down</code> - optional spinner buttons,</li>
  <li><code>focus_ring</code> - runtime focus state, distinct from placement aura and IDE selection.</li>
</ul>

<hr/>

<h2>Boundary</h2>

<p>
The selection overlays are host overlays derived from published SVG parts.
They are not additional SVG public parts. They also do not replace the
<code>focus_ring</code>, which remains the widget's own focus interaction part.
</p>
