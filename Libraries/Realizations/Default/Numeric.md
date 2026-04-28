<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Numeric Realization</h1>

<p align="center">
  <strong>Default realization posture for standardized numeric widgets</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#scope">2. Scope</a></li>
  <li><a href="#target-classes">3. Target Classes</a></li>
  <li><a href="#default-variant-posture">4. Default Variant Posture</a></li>
  <li><a href="#published-parts">5. Published Parts</a></li>
  <li><a href="#state-posture">6. State Posture</a></li>
  <li><a href="#property-binding-posture">7. Property-Binding Posture</a></li>
  <li><a href="#asset-posture">8. Asset Posture</a></li>
  <li><a href="#host-posture">9. Host Posture</a></li>
  <li><a href="#summary">10. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document describes the official Default realization posture for standardized numeric widgets.
</p>

<p>
The current default numeric realization is based on a rectangular SVG-backed posture that can display:
</p>

<ul>
  <li>a numeric value face,</li>
  <li>dynamic value text,</li>
  <li>optional increment/decrement buttons,</li>
  <li>optional label and caption surfaces,</li>
  <li>optional unit label and radix badge.</li>
</ul>

<hr/>

<h2 id="scope">2. Scope</h2>

<p>
This document is realization-side. It does not define the numeric class law.
The normative widget contract remains in <code>../../Widgets/Numeric.md</code>.
</p>

<hr/>

<h2 id="target-classes">3. Target Classes</h2>

<ul>
  <li><code>frog.widgets.numeric_control</code></li>
  <li><code>frog.widgets.numeric_indicator</code></li>
</ul>

<hr/>

<h2 id="default-variant-posture">4. Default Variant Posture</h2>

<p>
The default published variant is <code>rectangular</code>.
</p>

<p>
This variant uses a parameterized SVG template in which the host or runtime can:
</p>

<ul>
  <li>scale width and height to the front-panel layout rectangle,</li>
  <li>apply style colors and border widths,</li>
  <li>show or hide label and caption,</li>
  <li>show or hide unit label and radix badge,</li>
  <li>show or hide the increment/decrement cluster,</li>
  <li>inject dynamic value text.</li>
</ul>

<hr/>

<h2 id="published-parts">5. Published Parts</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>frame</code></li>
  <li><code>value_face</code></li>
  <li><code>text_value</code></li>
  <li><code>spinner</code></li>
  <li><code>increment_up</code></li>
  <li><code>increment_down</code></li>
  <li><code>unit_label</code></li>
  <li><code>radix_badge</code></li>
  <li><code>focus_ring</code></li>
</ul>

<hr/>

<h2 id="state-posture">6. State Posture</h2>

<p>
The default numeric realization recognizes at least the following visual states:
</p>

<ul>
  <li><code>normal</code></li>
  <li><code>focused</code></li>
  <li><code>hovered</code></li>
  <li><code>disabled</code></li>
  <li><code>read_only</code></li>
  <li><code>out_of_range</code></li>
</ul>

<p>
The increment buttons may additionally use a local pressed-state posture.
These are realization states, not new semantic widget values.
</p>

<hr/>

<h2 id="property-binding-posture">7. Property-Binding Posture</h2>

<p>
The default numeric realization is expected to bind at least the following public widget properties:
</p>

<ul>
  <li><code>label.*</code></li>
  <li><code>caption.*</code></li>
  <li><code>value</code></li>
  <li><code>display.format_kind</code></li>
  <li><code>display.format_string</code></li>
  <li><code>display.precision_digits</code></li>
  <li><code>display.radix_visible</code></li>
  <li><code>display.increment_buttons_visible</code></li>
  <li><code>unit_label.*</code></li>
  <li><code>style.frame.*</code></li>
  <li><code>style.value_face.*</code></li>
  <li><code>style.text_value.*</code></li>
  <li><code>style.spinner.*</code></li>
  <li><code>style.focus_ring.*</code></li>
</ul>

<hr/>

<h2 id="asset-posture">8. Asset Posture</h2>

<p>
The default numeric asset corridor is:
</p>

<pre><code>Libraries/Realizations/Default/assets/numeric/templates/numeric_rectangular.svg
</code></pre>

<p>
This SVG is a semantic template.
Its marked parts are intended for IDE and runtime interpretation rather than being a frozen bitmap-like picture.
</p>

<hr/>

<h2 id="host-posture">9. Host Posture</h2>

<p>
A host MAY render text natively instead of relying on baked SVG text.
A host MAY also replace the spinner graphics with equivalent native controls, provided that the published public part model and widget semantics remain preserved.
</p>

<hr/>

<h2 id="summary">10. Summary</h2>

<p>
The Default numeric realization gives FROG one inspectable baseline for numeric controls and numeric indicators while remaining open to runtime-specific rendering strategies.
</p>
