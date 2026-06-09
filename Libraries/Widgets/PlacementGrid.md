<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Widget Placement Grid</h1>

<p align="center">
  <strong>Common grid, placement aura, and resize law for front-panel widgets</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
FROG widgets are autonomous objects. A widget owns its class law, skin,
default size, resize constraints, anchors, and interaction parts. The front
panel, IDE, and container widgets such as Array must be able to place different
widgets together without inventing runtime-private geometry. This document
defines the shared placement grid and the placement aura used for that
cohabitation.
</p>

<hr/>

<h2>Grid Rule</h2>

<ul>
  <li>The default visible front-panel grid pitch is <code>16px</code> at 100% design zoom.</li>
  <li>The canonical front-panel coordinate system uses <code>x</code> increasing to the right and <code>y</code> increasing downward.</li>
  <li>The canonical source origin of the front panel is <code>0,0</code> at the top-left of the front-panel canvas.</li>
  <li>When the grid is visible, the <code>0,0</code> origin point must render with a stronger visual marker than ordinary grid points. The default marker is a point twice the standard grid-dot radius, not a large decorative target.</li>
  <li>A widget's canonical placement origin is the top-left corner of its <code>placement_bounds</code>.</li>
  <li>The top-left corner of widget placement bounds snaps to the grid.</li>
  <li>For horizontally resizable widgets, placement width changes point-to-point on the grid while the widget keeps its declared uniform aura band around the main body.</li>
  <li>Placement height follows the widget body plus the declared aura band. It MAY be grid-aligned when the body law naturally lands on the grid, but it is not forced to invent extra bottom space only to hit a grid point.</li>
  <li>Internal skin geometry may use smaller values when required by the realization.</li>
  <li>Runtime hosts consume declared placement properties; they must not hardcode a private grid per widget.</li>
</ul>

<p>
The grid is a placement and inspection surface. It is not a widget skin part
and it is not semantic program data. Examples may show the grid background so
humans can inspect calibration, placement bounds, and contained-widget cells.
Execution examples normally keep the same grid law in source while rendering
the grid hidden.
</p>

<p>
This document defines the common placement vocabulary. It does not assign one
universal aura size to every widget family. Each widget definition and Default
realization manifest must publish its own body, aura padding, resize, and
focus-ring law. The Default Numeric rule below is the first reviewed widget law,
not a template dimension to copy blindly to other widgets.
</p>

<hr/>

<h2>Execution View vs IDE View</h2>

<p>
The same <code>.frog</code> placement law serves two host postures:
</p>

<ul>
  <li><strong>Execution view</strong> - the runtime executes the program and normally hides the grid and IDE selection overlays. Widget placement still uses <code>placement_bounds</code>.</li>
  <li><strong>IDE view</strong> - the host may render the grid, hover/preselection, selected-widget overlays, resize handles, and authoring affordances. Those overlays are host views of source-declared geometry; they are not new widget skin parts.</li>
</ul>

<p>
A front panel may declare this posture with <code>canvas.presentation_mode</code>.
The current public values are <code>execution</code> and <code>ide_view</code>.
In <code>ide_view</code>, <code>canvas.selected_widget_id</code> MAY select an
initial widget for visual validation, and a host MAY update that selection
interactively without changing program semantics.
</p>

<hr/>

<h2>Canonical <code>.frog</code> Layout Syntax</h2>

<p>
In canonical <code>.frog</code> source, widget placement is expressed through
the widget instance <code>layout</code> object. The layout rectangle places the
widget's portable placement aura, not the raw SVG viewBox and not a
runtime-private host control rectangle.
</p>

<pre><code>"front_panel": {
  "canvas": {
    "width": 640,
    "height": 360,
    "coordinate_space": "panel_pixels",
    "grid": {
      "visible": true,
      "pitch": 16,
      "origin": { "x": 0, "y": 0 },
      "snap": "placement_bounds"
    },
    "presentation_mode": "ide_view",
    "selected_widget_id": "numeric_input"
  },
  "widgets": [
    {
      "id": "numeric_input",
      "class_ref": "frog.widgets.numeric_control",
      "role": "control",
      "layout": {
        "x": 32,
        "y": 48,
        "width": 96,
        "height": 32,
        "origin": "placement_bounds.top_left",
        "bounds_ref": "placement_bounds"
      }
    }
  ]
}</code></pre>

<ul>
  <li><code>canvas.grid.visible</code> declares whether the design grid is rendered by examples or IDE front-panel surfaces. When omitted, the default is <code>false</code> for ordinary hosts and MAY be <code>true</code> for calibration examples.</li>
  <li><code>canvas.grid.pitch</code> declares the visible design grid pitch in panel pixels. When omitted, the default is <code>16</code>.</li>
  <li><code>canvas.grid.origin</code> declares the grid origin in the front-panel coordinate space. When omitted, the default is <code>{ "x": 0, "y": 0 }</code>. Runtime and IDE grid renderers must distinguish this origin marker from ordinary repeated grid points, for example with a stronger color or larger point radius.</li>
  <li><code>canvas.grid.snap</code> declares what snaps to the grid. For widgets, the portable value is <code>placement_bounds</code>.</li>
  <li><code>canvas.presentation_mode</code> declares whether the host is rendering an execution surface or an IDE/calibration surface.</li>
  <li><code>canvas.selected_widget_id</code> is optional and only meaningful for IDE/calibration hosts that render selection overlays.</li>
  <li><code>layout.x</code> and <code>layout.y</code> declare the grid-positioned coordinates of <code>layout.origin</code> in the parent coordinate space.</li>
  <li><code>layout.width</code> and <code>layout.height</code> declare the source-owned size of the same <code>layout.bounds_ref</code> rectangle.</li>
  <li><code>layout.origin</code> is optional. When omitted, it is <code>placement_bounds.top_left</code>.</li>
  <li><code>layout.bounds_ref</code> is optional. When omitted for a widget instance, it is <code>placement_bounds</code>.</li>
</ul>

<p>
For a top-level widget, the parent coordinate space is the front-panel canvas.
For a contained widget, the parent coordinate space is the container-owned
placement surface, for example an Array element cell. This keeps the same
source law for standalone widgets, widgets hosted by containers, and future IDE
manipulation: every widget is positioned by its own placement aura inside its
current parent.
</p>

<p>
The top-left canonical origin matches SVG, CSS, canvas, and browser-host
coordinate systems, where <code>y</code> increases downward. IDEs MAY expose
authoring conveniences such as center handles or bottom-left readouts, but
those conveniences must resolve back to the canonical
<code>placement_bounds.top_left</code> layout before serialization.
</p>

<p>
Bottom-left placement is therefore not a canonical source origin in the v0.1
widget placement model. If a future profile adds alternate layout anchors, it
must specify a deterministic conversion back to <code>layout.x</code>,
<code>layout.y</code>, <code>layout.width</code>, and <code>layout.height</code>
over <code>placement_bounds</code>.
</p>

<hr/>

<h2>Bounds Vocabulary</h2>

<ul>
  <li><strong>Skin bounds</strong> - the visible and interactive realization surface of the widget.</li>
  <li><strong>Placement bounds</strong> - the widget aura used to place, select, resize, or contain the widget. Its top-left corner snaps to the grid; its width is grid-controlled; its height follows the widget body plus the declared aura band.</li>
  <li><strong>Main body</strong> - the visible core body of the widget, such as <code>control_body</code> or <code>indicator_body</code>. It sits inside <code>placement_bounds</code> with the widget's declared aura padding. It is not the aura.</li>
  <li><strong>Label aura</strong> - an IDE overlay aligned to the caption/label anchor. It is derived from <code>caption</code> or equivalent label parts.</li>
  <li><strong>Focus ring</strong> - a runtime interaction state owned by the focused widget skin.</li>
  <li><strong>IDE selection overlay</strong> - an editor-only overlay with handles; it is not a runtime widget part.</li>
  <li><strong>Container cell</strong> - a container-owned placement surface, such as an Array element slot.</li>
</ul>

<p>
The placement bounds are the portable aura. They are a declared band around the
widget's main body. The band is uniform for a given posture. The aura's top-left
corner snaps to the common grid and horizontal resizing moves the aura width
from grid point to grid point. The visible body remains centered within that
band and resizes according to the widget class and realization law.
</p>

<hr/>

<h2>Container Rule</h2>

<p>
A container widget that hosts another widget must consume the contained
widget's placement posture. For Array, a widget-backed element cell is the
contained widget's placement surface for that element. Hover, preselection, and
selection belong to the Array cell. The contained widget focus ring appears
only when the contained widget itself receives focus for editing.
</p>

<pre><code>contained widget skin
 declared aura padding
 grid snap
= contained widget placement bounds
= Array element cell when hosted by Array
</code></pre>

<hr/>

<h2>Default Numeric Placement Law</h2>

<p>
The Default Numeric realization is the first reviewed placement-grid witness.
Its compact control posture declares a 4px uniform aura band around the main
body. The compact default aura is 96x32; the visible control body is 88x24 and
is centered inside that aura. The value face and optional increment/decrement
buttons are the visible skin. Width may be resized manually by moving the aura
from grid point to grid point; the visible body stretches while preserving the
uniform band. Height is driven by font and display comfort plus the same band.
The spinner stays anchored to the declared side inside the body. The value text
stays right-aligned and vertically centered.
</p>

<p>
When a Numeric is hosted as an Array element, the Array cell owns hover and
selection. The Numeric focus ring remains a Numeric runtime focus state and
must not be used to represent Array cell selection.
</p>

<hr/>

<h2>Validation Expectations</h2>

<ul>
  <li>Examples that review widget geometry should show the 16px point grid unless the example explicitly tests a plain host surface.</li>
  <li>The visible grid must distinguish the <code>0,0</code> origin marker from ordinary pitch points.</li>
  <li>Standalone widgets must expose their placement posture without changing their visible skin semantics.</li>
  <li>Array cells containing widgets must size to the contained widget placement posture, not to a hardcoded host grid or a fake cell skin.</li>
  <li>Hover/preselection/selection on Array cells must cover the cell placement surface.</li>
  <li>Widget focus rings must cover only the focused widget's public focus part.</li>
</ul>
