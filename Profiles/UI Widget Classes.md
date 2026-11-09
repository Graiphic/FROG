<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Profile Posture for UI Widget Classes</h1>

<p align="center">
  <strong>How profiles adopt published widget classes without becoming the owner of widget semantics</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="../Libraries/Widgets/Readme.md">Widgets index</a></li>
  <li><a href="../Libraries/Widgets/Numeric.md">Numeric widget law</a></li>
  <li><a href="../Libraries/UI.md">Executable UI primitives</a></li>
  <li><a href="../Examples/05_bounded_ui_accumulator/Readme.md">Example 05 corridor</a></li>
  <li><a href="../Implementations/Reference/Runtime/Readme.md">Reference runtime family</a></li>
  <li><a href="../Versioning/Readme.md">Repository version governance</a></li>
</ul>

<hr/>

<h2>1. Overview</h2>

<p>
Profiles do not define intrinsic widget semantics.
Profiles declare which already-published widget classes, members, and interaction capabilities are required or optional for a given capability family.
</p>

<p>
The active published widget class identifiers used by the current corridor are under <code>frog.widgets.*</code>.
This document therefore adopts that vocabulary and removes the need for a competing <code>frog.ui.standard.*</code> class family.
</p>

<hr/>

<h2>2. Ownership boundary</h2>

<ul>
  <li><code>Libraries/Widgets/</code> owns the normative law of intrinsic widget classes.</li>
  <li><code>Profiles/</code> owns capability selection, adoption posture, and support claims.</li>
  <li><code>.wfrog</code> packages publish corridor-specific panels, assets, and host bindings.</li>
  <li>runtimes implement profile claims; they do not become the owner of class meaning.</li>
</ul>

<p>
Therefore, a profile may say that a runtime family supports <code>frog.widgets.numeric_control</code> with a certain portable member set, but it must not redefine that class under a different identifier or with a conflicting property law.
</p>

<hr/>

<h2>3. Active published class identifiers</h2>

<p>
The active published intrinsic class identifiers used by the current runtime-family corridor are:
</p>

<ul>
  <li><code>frog.widgets.panel</code></li>
  <li><code>frog.widgets.numeric_control</code></li>
  <li><code>frog.widgets.numeric_indicator</code></li>
</ul>

<p>
The first executable UI slice does <strong>not</strong> use a separate <code>frog.ui.standard.numeric_control</code> or <code>frog.ui.standard.numeric_indicator</code> naming family.
</p>

<hr/>

<h2>4. Example 05 profile anchor</h2>

<p>
Example 05 is the current profile anchor for a minimal serious UI runtime-family slice.
A profile claiming support for that slice must support all of the following together:
</p>

<ul>
  <li>the class identifiers <code>frog.widgets.numeric_control</code> and <code>frog.widgets.numeric_indicator</code>,</li>
  <li>the portable property set <code>value</code>, <code>label</code>, <code>visible</code>, <code>enabled</code>, <code>foreground_color</code>,</li>
  <li><code>widget_value</code> participation for the control and indicator value flow,</li>
  <li><code>widget_reference</code> support for portable member access,</li>
  <li><code>frog.ui.property_write</code> on <code>foreground_color</code>,</li>
  <li>the control method <code>focus()</code>,</li>
  <li>the indicator method <code>reset_to_default_style()</code>,</li>
  <li>a browser-host or equivalent host surface that can expose the published panel and assets.</li>
</ul>

<hr/>

<h2>5. Portable member posture for the current slice</h2>

<p>
For the current published slice, the portable member surface is intentionally small and flat.
</p>

<ul>
  <li>portable label member: <code>label</code></li>
  <li>portable visibility member: <code>visible</code></li>
  <li>portable enablement member: <code>enabled</code></li>
  <li>portable presentation member used by the current slice: <code>foreground_color</code></li>
</ul>

<p>
A profile must not reinterpret these members as nested alternatives such as <code>label.text</code>, <code>interaction.visible</code>, or <code>style.foreground_color</code> while still claiming conformance to the currently published slice.
</p>

<hr/>

<h2>6. Host capability claims</h2>

<p>
A profile that claims support for the current reference runtime-family slice must require at least the following host capabilities:
</p>

<ul>
  <li><code>window</code></li>
  <li><code>basic_widget_rendering</code></li>
  <li><code>widget_value_binding</code></li>
  <li><code>widget_reference_binding</code></li>
  <li><code>property_write</code></li>
</ul>

<p>
Additional capabilities such as focus transfer and event observation may be published as optional claims.
</p>

<hr/>

<h2>7. Versioning posture</h2>

<p>
Profile support claims must follow repository-wide version governance.
This document therefore does not publish local hard-coded class-contract or profile versions.
Version and publication status remain governed by <code>Versioning/</code>.
</p>

<hr/>

<h2>8. Future expansion posture</h2>

<p>
As more standardized classes are added, profiles may extend their adoption matrices to boolean, string, button, chart, and other widget families.
The rule remains the same:
</p>

<ul>
  <li>class law stays in <code>Libraries/Widgets/</code>,</li>
  <li>profiles publish support claims and capability selections,</li>
  <li>runtime-private or realization-private names do not become the public class vocabulary.</li>
</ul>

<hr/>

<h2>9. Summary</h2>

<p>
The currently published profile posture is simple: profiles adopt the <code>frog.widgets.*</code> classes already published by the widget library, claim a bounded portable member set, and require the host capabilities needed by the Example 05 runtime-family corridor.
</p>
