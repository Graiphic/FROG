<p align="center">
  <img src="../../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Numeric Realization Assets</h1>

<p align="center">
  <strong>SVG template asset for the default numeric realization variant</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="../../Numeric.md">Default numeric realization posture</a></li>
  <li><a href="../../numeric.default.wfrog">Machine-readable numeric realization manifest</a></li>
  <li><a href="../../Package.md">Default realization package posture</a></li>
  <li><a href="../../../../Widgets/Numeric.md">Numeric widget class law</a></li>
</ul>

<hr/>

<h2>1. Published Asset</h2>

<p>
The compact numeric control geometry publishes a value face and two increment/decrement button parts.
The template uses a compact 220 x 88 viewBox so the 24 unit numeric body can preserve the measured LabVIEW NXG-like compact numeric posture when rendered by the current host.
The buttons use the same separated-button proportion measured from the compact NXG numeric reference:
13 unit width, 11 unit height, 2 unit vertical gap, and a 2 unit gap from the value face.
Source properties may place the button cluster on the right, place it on the left, or hide it.
When hidden, the value face may expand to the compact control-body width.
</p>

<ul>
  <li><code>templates/numeric_rectangular.svg</code> — rectangular default numeric realization template.</li>
</ul>

<hr/>

<h2>2. Required Template Markers</h2>

<ul>
  <li><code>root</code></li>
  <li><code>placement_bounds</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>control_body</code></li>
  <li><code>indicator_body</code></li>
  <li><code>value_face</code></li>
  <li><code>text_value</code></li>
  <li><code>spinner</code></li>
  <li><code>increment_up</code></li>
  <li><code>increment_down</code></li>
  <li><code>focus_ring</code></li>
</ul>

<p>
The default compact SVG intentionally does not publish separate
<code>frame</code>, <code>radix_badge</code>, <code>radix_text</code>, or
<code>unit_label</code> parts. The value face owns the visible border, while
radix surfaces are reserved for future variants that explicitly publish those
parts. Unit text for the compact default is rendered inline through
<code>text_value</code>.
The <code>placement_bounds</code> part is the portable placement aura: top-left
grid-snapped, width controlled from grid point to grid point, and height equal
to the main body plus the declared uniform aura band. The
<code>control_body</code> and <code>indicator_body</code> parts describe the
visible body centered inside that aura; they are not the aura. The
<code>focus_ring</code> part is only the widget focus interaction surface. IDE
selection and label auras are host overlays derived from
<code>placement_bounds</code> and <code>caption</code>; they are not extra SVG
public parts.
</p>
