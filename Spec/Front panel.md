<h1 align="center">🐸 FROG Front Panel Specification</h1>

<p align="center">
Definition of the front panel (interaction layer) for <strong>.frog</strong> programs<br/>
<em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#goals">2. Design goals</a></li>
  <li><a href="#location">3. Location in a .frog file</a></li>
  <li><a href="#structure">4. Front panel structure</a></li>
  <li><a href="#canvas">5. Canvas and coordinate system</a></li>
  <li><a href="#elements">6. Elements</a>
    <ul>
      <li><a href="#controls">6.1 Controls</a></li>
      <li><a href="#indicators">6.2 Indicators</a></li>
      <li><a href="#text">6.3 Text and labels</a></li>
      <li><a href="#comments">6.4 Comments</a></li>
      <li><a href="#shapes">6.5 Shapes</a></li>
      <li><a href="#images">6.6 Images</a></li>
      <li><a href="#groups">6.7 Groups</a></li>
    </ul>
  </li>
  <li><a href="#bindings">7. Bindings</a></li>
  <li><a href="#layout-model">8. Layout model</a></li>
  <li><a href="#style-system">9. Style system</a>
    <ul>
      <li><a href="#theme">9.1 Theme</a></li>
      <li><a href="#fonts">9.2 Fonts</a></li>
      <li><a href="#colors">9.3 Colors and variables</a></li>
      <li><a href="#overrides">9.4 Overrides</a></li>
    </ul>
  </li>
  <li><a href="#ui-libraries">10. UI libraries</a></li>
  <li><a href="#validation">11. Validation rules</a></li>
  <li><a href="#examples">12. Examples</a></li>
  <li><a href="#summary">13. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
The <strong>front_panel</strong> section defines the graphical interaction layer of a Frog.
It describes UI elements such as controls, indicators, text, and graphical decorations.
</p>

<p>
The front panel is optional for execution, but if present it MUST be well-defined and bindable to the diagram.
</p>

<p>
The front panel MUST NOT define execution logic. It only provides interaction and visualization.
</p>

<hr/>

<h2 id="goals">2. Design goals</h2>

<ul>
  <li>Tool-agnostic representation</li>
  <li>Stable layout across implementations</li>
  <li>Clear binding rules to the diagram</li>
  <li>Modular styling (themes, variables, overrides)</li>
  <li>Composable UI via libraries and reusable components</li>
</ul>

<hr/>

<h2 id="location">3. Location in a .frog file</h2>

<pre>{
  "spec_version": "0.1",
  "metadata": {},
  "interface": {},
  "connector": {},
  "diagram": {},
  "front_panel": {}
}</pre>

The <code>front_panel</code> section MUST exist (it may be empty).

<hr/>

<h2 id="structure">4. Front panel structure</h2>

<p>
Recommended structure:
</p>

<pre>"front_panel": {
  "canvas": {},
  "elements": [],
  "bindings": [],
  "style": {},
  "ui_libraries": []
}</pre>

Fields:

<ul>
  <li><code>canvas</code> — coordinate system and background</li>
  <li><code>elements</code> — UI objects (controls, indicators, text, shapes, groups)</li>
  <li><code>bindings</code> — explicit connections to diagram/interface ports</li>
  <li><code>style</code> — theme, fonts, colors, variables</li>
  <li><code>ui_libraries</code> — references to UI widget libraries</li>
</ul>

<hr/>

<h2 id="canvas">5. Canvas and coordinate system</h2>

<p>
The canvas defines the coordinate system used by front panel elements.
</p>

<pre>"canvas": {
  "units": "px",
  "width": 900,
  "height": 600,
  "background": {
    "color": "#FFFFFF"
  }
}</pre>

Rules:

<ul>
  <li>Units SHOULD be <code>px</code> in v0.1.</li>
  <li>Width and height SHOULD be integers.</li>
  <li>Background is purely visual.</li>
</ul>

<hr/>

<h2 id="elements">6. Elements</h2>

<p>
All UI objects are represented as elements.
</p>

<p>
Every element MUST define:
</p>

<ul>
  <li><code>id</code> — unique identifier</li>
  <li><code>kind</code> — element category</li>
  <li><code>layout</code> — position and size</li>
</ul>

Base element structure:

<pre>{
  "id": "el_1",
  "kind": "control",
  "type": "numeric",
  "layout": { "x": 20, "y": 20, "width": 120, "height": 28 },
  "props": {},
  "style_ref": "default.control"
}</pre>

<hr/>

<h3 id="controls">6.1 Controls</h3>

<p>
Controls provide user input to the program.
</p>

Example:

<pre>{
  "id": "ctrl_gain",
  "kind": "control",
  "type": "numeric",
  "layout": { "x": 20, "y": 20, "width": 120, "height": 28 },
  "props": {
    "min": 0.0,
    "max": 10.0,
    "step": 0.1,
    "default": 1.0
  },
  "style_ref": "default.control"
}</pre>

<hr/>

<h3 id="indicators">6.2 Indicators</h3>

<p>
Indicators display program output or state.
</p>

Example:

<pre>{
  "id": "ind_result",
  "kind": "indicator",
  "type": "numeric",
  "layout": { "x": 20, "y": 60, "width": 120, "height": 28 },
  "props": {
    "format": "fixed",
    "precision": 3
  },
  "style_ref": "default.indicator"
}</pre>

<hr/>

<h3 id="text">6.3 Text and labels</h3>

<p>
Text elements display static labels.
</p>

Example:

<pre>{
  "id": "txt_title",
  "kind": "text",
  "layout": { "x": 20, "y": 110, "width": 400, "height": 32 },
  "props": {
    "text": "Example Frog",
    "align": "left"
  },
  "style_ref": "default.title"
}</pre>

<hr/>

<h3 id="comments">6.4 Comments</h3>

<p>
Comments are non-executable annotations.
</p>

Example:

<pre>{
  "id": "cmt_1",
  "kind": "comment",
  "layout": { "x": 20, "y": 150, "width": 500, "height": 80 },
  "props": {
    "text": "This panel demonstrates basic control/indicator binding."
  },
  "style_ref": "default.comment"
}</pre>

<hr/>

<h3 id="shapes">6.5 Shapes</h3>

<p>
Shapes are decorative elements.
</p>

Example:

<pre>{
  "id": "shape_box",
  "kind": "shape",
  "type": "rect",
  "layout": { "x": 10, "y": 10, "width": 160, "height": 90 },
  "props": {
    "radius": 8
  },
  "style_ref": "default.panel_box"
}</pre>

<hr/>

<h3 id="images">6.6 Images</h3>

<p>
Images reference an external resource or embedded SVG.
</p>

Example:

<pre>{
  "id": "img_logo",
  "kind": "image",
  "layout": { "x": 720, "y": 20, "width": 160, "height": 160 },
  "props": {
    "source": "assets/logo.svg"
  }
}</pre>

<hr/>

<h3 id="groups">6.7 Groups</h3>

<p>
Groups allow multiple elements to be manipulated as a unit.
Groups may contain children elements by reference.
</p>

Example:

<pre>{
  "id": "group_1",
  "kind": "group",
  "layout": { "x": 0, "y": 0, "width": 0, "height": 0 },
  "props": {
    "children": ["shape_box", "ctrl_gain", "ind_result"]
  }
}</pre>

<hr/>

<h2 id="bindings">7. Bindings</h2>

<p>
Bindings connect UI elements to diagram or interface ports.
Bindings MUST be explicit and typed.
</p>

Recommended binding structure:

<pre>"bindings": [
  {
    "element": "ctrl_gain",
    "direction": "to_diagram",
    "target": { "node": "input_gain", "port": "value" }
  },
  {
    "element": "ind_result",
    "direction": "from_diagram",
    "target": { "node": "output_result", "port": "value" }
  }
]</pre>

Rules:

<ul>
  <li>Each binding MUST reference an existing element id.</li>
  <li>Each binding MUST reference a valid target node/port.</li>
  <li>Bindings MUST be type-compatible.</li>
</ul>

<hr/>

<h2 id="layout-model">8. Layout model</h2>

<p>
Layout defines element geometry and visual ordering.
</p>

Layout fields:

<ul>
  <li><code>x</code>, <code>y</code> — position</li>
  <li><code>width</code>, <code>height</code> — size</li>
  <li><code>z</code> (optional) — drawing order</li>
  <li><code>anchor</code> (optional) — responsive behavior</li>
</ul>

Example:

<pre>"layout": {
  "x": 20,
  "y": 20,
  "width": 120,
  "height": 28,
  "z": 10,
  "anchor": "top_left"
}</pre>

<hr/>

<h2 id="style-system">9. Style system</h2>

<p>
The style system provides modular visual definitions:
</p>

<ul>
  <li>themes</li>
  <li>fonts</li>
  <li>color palettes</li>
  <li>style references</li>
  <li>local overrides</li>
</ul>

<h3 id="theme">9.1 Theme</h3>

<pre>"style": {
  "theme": "default"
}</pre>

<h3 id="fonts">9.2 Fonts</h3>

<pre>"style": {
  "fonts": {
    "default": { "family": "Inter", "size": 12, "weight": 400 },
    "title": { "family": "Inter", "size": 20, "weight": 700 }
  }
}</pre>

<h3 id="colors">9.3 Colors and variables</h3>

<pre>"style": {
  "colors": {
    "bg": "#FFFFFF",
    "fg": "#111111",
    "accent": "#2ECC71"
  }
}</pre>

<h3 id="overrides">9.4 Overrides</h3>

Elements may override style locally:

<pre>{
  "id": "ctrl_gain",
  "kind": "control",
  "type": "numeric",
  "layout": { "x": 20, "y": 20, "width": 120, "height": 28 },
  "props": { "default": 1.0 },
  "style_ref": "default.control",
  "style_override": { "colors": { "accent": "#FF6600" } }
}</pre>

<hr/>

<h2 id="ui-libraries">10. UI libraries</h2>

<p>
FROG front panels may reference external UI widget libraries.
This enables modular, standardized control/indicator sets.
</p>

Example:

<pre>"ui_libraries": [
  {
    "name": "frog.ui.standard",
    "version": "0.1.0"
  }
]</pre>

Tools MAY resolve these libraries from local or remote registries.
Runtimes MUST ignore UI libraries for execution.

<hr/>

<h2 id="validation">11. Validation rules</h2>

Implementations MUST enforce:

<ul>
  <li>Element identifiers MUST be unique.</li>
  <li>All bindings MUST reference valid elements and valid diagram targets.</li>
  <li>Element layouts MUST contain valid numeric coordinates.</li>
  <li>Unknown element properties MUST NOT affect execution.</li>
</ul>

<hr/>

<h2 id="examples">12. Examples</h2>

<h3>Minimal front panel</h3>

<pre>"front_panel": {
  "canvas": { "units": "px", "width": 600, "height": 400, "background": { "color": "#FFFFFF" } },
  "elements": [],
  "bindings": [],
  "style": {},
  "ui_libraries": []
}</pre>

<h3>Control + indicator + bindings</h3>

<pre>"front_panel": {
  "canvas": { "units": "px", "width": 900, "height": 600, "background": { "color": "#FFFFFF" } },
  "elements": [
    {
      "id": "ctrl_gain",
      "kind": "control",
      "type": "numeric",
      "layout": { "x": 20, "y": 20, "width": 120, "height": 28 },
      "props": { "min": 0.0, "max": 10.0, "step": 0.1, "default": 1.0 },
      "style_ref": "default.control"
    },
    {
      "id": "ind_result",
      "kind": "indicator",
      "type": "numeric",
      "layout": { "x": 20, "y": 60, "width": 120, "height": 28 },
      "props": { "format": "fixed", "precision": 3 },
      "style_ref": "default.indicator"
    },
    {
      "id": "txt_title",
      "kind": "text",
      "layout": { "x": 20, "y": 110, "width": 400, "height": 32 },
      "props": { "text": "Example Frog", "align": "left" },
      "style_ref": "default.title"
    }
  ],
  "bindings": [
    {
      "element": "ctrl_gain",
      "direction": "to_diagram",
      "target": { "node": "input_gain", "port": "value" }
    },
    {
      "element": "ind_result",
      "direction": "from_diagram",
      "target": { "node": "output_result", "port": "value" }
    }
  ],
  "style": {
    "theme": "default",
    "fonts": {
      "default": { "family": "Inter", "size": 12, "weight": 400 },
      "title": { "family": "Inter", "size": 20, "weight": 700 }
    },
    "colors": {
      "bg": "#FFFFFF",
      "fg": "#111111",
      "accent": "#2ECC71"
    }
  },
  "ui_libraries": [
    { "name": "frog.ui.standard", "version": "0.1.0" }
  ]
}</pre>

<hr/>

<h2 id="summary">13. Summary</h2>

<p>
The front panel defines the interaction layer of a Frog through a modular model:
</p>

<ul>
  <li>elements (controls, indicators, text, comments, shapes, images, groups)</li>
  <li>bindings (explicit connections to diagram ports)</li>
  <li>layout (geometry and ordering)</li>
  <li>style (themes, fonts, colors, overrides)</li>
  <li>ui libraries (standard widget sets)</li>
</ul>

<p>
This structure enables tool-independent front panels that remain consistent, extensible, and easy to author.
</p>
