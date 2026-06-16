<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization — Numeric Widgets</h1>

<p align="center">
  <strong>Default realization posture for standardized numeric widgets</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="./Package.md">Default realization package posture</a></li>
  <li><a href="./numeric.default.wfrog">Machine-readable numeric realization manifest</a></li>
  <li><a href="./assets/numeric/Readme.md">Numeric default realization assets</a></li>
  <li><a href="../../Widgets/Numeric.md">Numeric widget class law</a></li>
  <li><a href="../../UI.md">Executable UI primitives</a></li>
</ul>

<hr/>

<h2>1. Overview</h2>

<p>
This document describes the default realization posture for standardized numeric widgets:
</p>

<ul>
  <li><code>frog.widgets.numeric_control</code></li>
  <li><code>frog.widgets.numeric_indicator</code></li>
</ul>

<p>
It does not define numeric widget semantics.
The class law belongs to <code>Libraries/Widgets/Numeric.md</code>.
This document describes how the default realization family can embody that law through SVG template resources, part bindings, state maps, and host hints.
</p>

<hr/>

<h2>2. Published Default Variant</h2>

<ul>
  <li><code>rectangular</code></li>
</ul>

<p>
The rectangular variant is the initial default numeric realization variant.
</p>

<hr/>

<h2>3. Machine-Readable Manifest</h2>

<pre><code>Libraries/Realizations/Default/numeric.default.wfrog</code></pre>

<hr/>

<h2>4. Public Parts Realized</h2>

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

<hr/>

<h2>5. SVG Template Asset</h2>

<ul>
  <li><code>assets/numeric/templates/numeric_rectangular.svg</code></li>
</ul>

<p>
The template is intended to be interpreted by an IDE or runtime through published part markers and property bindings.
Its compact control body uses a LabVIEW NXG-like 24 unit <code>main_body</code>
centered in a 96 x 32 <code>placement_bounds</code> aura inside a compact
220 x 88 viewBox: a 73 unit value face, a 2 unit value-to-button gap,
13 x 11 increment/decrement button surfaces separated by a 2 unit vertical
gap, and a uniform 4 unit <code>aura_band</code> around
<code>main_body</code>.
The default caption anchor is positioned above the placement aura so the IDE
label aura remains visually separated from the body aura.
The button cluster can be placed on the right, placed on the left, or hidden through source-owned display properties.
When hidden, the value face may expand to the compact <code>control_body</code> width.
The button proportion intentionally matches the measured compact NXG numeric posture.
The compact indicator uses the same value-face geometry but a distinct indicator fill color.
The compact default does not publish a separate frame, radix badge, or unit label;
the visible value border belongs to <code>value_face</code>, future unit text is rendered inline through <code>text_value</code>, and
<code>placement_bounds</code> is the invisible placement aura. Its top-left
corner is the canonical placement origin, its width and height are the portable
layout rectangle, and its height follows <code>main_body</code> plus the declared
uniform <code>aura_band</code>. When source or container policy declares grid snap, this same
<code>placement_bounds</code> rectangle supplies the top-left/top-right edges
and width used for snap and quantization. IDE hosts render the selection aura
from <code>placement_bounds</code>; the
<code>control_body</code> and <code>indicator_body</code> parts describe the
Numeric mappings to <code>main_body</code>. Those overlays are not additional
SVG parts. The <code>focus_ring</code> remains the widget focus interaction
part and must not be used as the placement aura.
</p>

<hr/>

<h2>6. Default Compact U8 Control Profile</h2>

<p>
The Default Numeric realization also names one standard compact U8 control posture:
<code>frog.realizations.default.numeric.profile.compact_u8_control</code>.
This is not a separate widget class or a new widget concept. It is an ordinary
<code>frog.widgets.numeric_control</code> using the Default Numeric SVG, the
<code>96 x 32</code> <code>placement_bounds</code> posture, U8 representation,
the standard caption anchor, and the accepted LabVIEW NXG-like neutral palette.
</p>

<p>
Examples and containers that need compact Numeric controls such as Array
dimension count, visible row count, or visible column count must use this
profile rather than inventing a bespoke mini numeric surface. Instance-local
data may change the widget id, label text, value, binding, and placement
coordinates. It must not reintroduce a separate frame, private mini-control
geometry, or local colors that diverge from the Default Numeric palette.
</p>

<hr/>

<h2>7. Summary</h2>

<p>
The default numeric realization provides one inspectable SVG-backed baseline for numeric controls and indicators while keeping the class law in <code>Libraries/Widgets/Numeric.md</code>.
</p>
