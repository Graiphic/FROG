<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Standard Frame Widget</h1>

<p align="center">
  <strong>Normative baseline for the standardized simple frame support widget</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="./Readme.md">Widgets index</a></li>
  <li><a href="../../Expression/Widget.md">Expression widget instances</a></li>
  <li><a href="../../Expression/Widget%20realization.md">Expression widget realization</a></li>
  <li><a href="../../Expression/Widget%20package%20(.wfrog).md">Widget package publication</a></li>
  <li><a href="../../Libraries/Realizations/Default/Frame.md">Default frame realization</a></li>
  <li><a href="../../Libraries/Realizations/Default/frame.default.wfrog">Default frame realization manifest</a></li>
</ul>

<hr/>

<h2>Overview</h2>

<p>
This document defines <code>frog.widgets.frame</code>, the standardized simple frame support widget.
</p>

<p>
A Frame is a front-panel support rectangle. It provides only a background and a border so a source can visually
group, separate, or emphasize a region of the front panel.
</p>

<p>
The simple Frame is intentionally not a value control, not an indicator, not a subpanel, not a scrollable container,
and not a method-bearing object. It does not load another <code>.frog</code> and it does not own the widgets visually placed
inside its bounds.
</p>

<hr/>

<h2>Class Defined Here</h2>

<ul>
  <li><code>frog.widgets.frame</code></li>
</ul>

<hr/>

<h2>Frame Part versus Frame Widget</h2>

<p>
FROG distinguishes:
</p>

<pre><code>frame
    - a public part inside another widget realization

frog.widgets.frame
    - standalone simple support rectangle placed on the front panel
</code></pre>

<p>
A numeric widget may have a <code>frame</code> part. A button may have a <code>frame</code> part.
That is not the same thing as the standalone <code>frog.widgets.frame</code> class.
</p>

<hr/>

<h2>Class Posture</h2>

<ul>
  <li><strong>class_id:</strong> <code>frog.widgets.frame</code></li>
  <li><strong>family:</strong> <code>support_widget</code></li>
  <li><strong>compatible role:</strong> <code>support</code></li>
  <li><strong>primary business value:</strong> absent</li>
  <li><strong>natural <code>widget_value</code> participation:</strong> absent</li>
  <li><strong>standard behavior:</strong> visual background and border only</li>
</ul>

<p>
The Frame is source-owned front-panel decoration. Position and size live in the <code>.frog</code> instance layout.
Default visual properties live in the Default realization manifest and may be overridden by the <code>.frog</code> instance.
</p>

<hr/>

<h2>Public Part Model</h2>

<ul>
  <li><code>root</code></li>
  <li><code>background</code></li>
  <li><code>border</code></li>
</ul>

<p>
No caption, scrollbar, focus ring, value surface, child host, or content-region ownership is part of the simple Frame baseline.
A title can be composed with a separate <code>frog.widgets.label</code> instance when needed.
</p>

<hr/>

<h2>Standard Property Surface</h2>

<h3>Layout</h3>

<ul>
  <li><code>layout.x : length</code></li>
  <li><code>layout.y : length</code></li>
  <li><code>layout.width : length</code></li>
  <li><code>layout.height : length</code></li>
</ul>

<h3>Background</h3>

<ul>
  <li><code>style.background.visible : bool</code></li>
  <li><code>style.background.fill_color : frog.color.rgba8</code></li>
</ul>

<h3>Border</h3>

<ul>
  <li><code>style.border.visible : bool</code></li>
  <li><code>style.border.color : frog.color.rgba8</code></li>
  <li><code>style.border.width : length</code></li>
</ul>

<p>
Border width is a single global thickness value. It must remain visually uniform on all four sides regardless of
Frame size or aspect ratio. The simple Frame baseline uses a continuous rectangular border only; dashed, dotted,
textured, or side-specific border styles are not part of this widget.
</p>

<hr/>

<h2>Non-Goals</h2>

<ul>
  <li>No <code>caption.*</code> surface. Use a separate Label widget for text.</li>
  <li>No Frame-specific method-node surface.</li>
  <li>No Frame-specific property-node behavior beyond ordinary source-owned visual style data.</li>
  <li>No scrollbars. A scrollable region is a separate container concept, not this simple Frame.</li>
  <li>No child <code>.frog</code> reference. Use Subpanel for embedded front panels.</li>
  <li>No hidden ownership of widgets placed visually inside the rectangle.</li>
</ul>

<hr/>

<h2>Default Realization Posture</h2>

<p>
The Default realization exposes one rectangular SVG template with:
</p>

<ul>
  <li>a <code>background</code> part,</li>
  <li>a <code>border</code> part,</li>
  <li><code>vector-effect="non-scaling-stroke"</code> on the border to preserve uniform stroke thickness while resizing.</li>
</ul>

<p>
The SVG provides geometry and visual hooks only. It does not own grouping semantics, child layout, or execution behavior.
</p>

<hr/>

<h2>Validation Expectations</h2>

<p>
Validators SHOULD diagnose:
</p>

<ul>
  <li>attempts to treat <code>frog.widgets.frame</code> as a scalar value-carrying widget,</li>
  <li>attempts to use Frame as a Subpanel or child <code>.frog</code> host,</li>
  <li>attempts to attach caption, scrollbar, method, or execution semantics to the simple Frame baseline,</li>
  <li>use of a non-uniform border thickness after resizing,</li>
  <li>attempts to expose dashed, dotted, textured, or side-specific borders on the simple Frame baseline,</li>
  <li>runtime hardcoding of background color, border color, or border width instead of consuming source/default realization data.</li>
</ul>

<hr/>

<h2>Summary</h2>

<p>
<code>frog.widgets.frame</code> is the simplest visual grouping support object in the FROG baseline:
a resizable rectangle with configurable background and border.
</p>
