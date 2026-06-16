<h1>Example 10 - Button Switch Until Released</h1>

<p>
Reviewed 2026-06-16: this example consumes the Default Button rectangular semantic SVG skin, its published <code>button_face</code> part, and the current Boolean <code>state_face</code> indicator vocabulary in the C++ browser-host rendering path.
</p>

<p>
Example 10 is the bounded Button widget acceptance slice for the
<code>switch_until_released</code> mechanical action. It keeps the behavior
intentionally small: one Button control is ON while pressed, OFF when released,
the diagram copies that value, and one Boolean indicator displays the result.
</p>

<pre><code>.frog source
  -&gt; FIR
  -&gt; lowering
  -&gt; LLVM native Button kernel artifact + manifest
  -&gt; C++ / Python / Rust reference runtimes
  -&gt; .wfrog front panel binding
  -&gt; Default Button and Boolean realization SVG assets
</code></pre>

<p>
The visible front panel must consume <code>ui/button_panel.wfrog</code>, which points to
<code>Libraries/Realizations/Default/button.default.wfrog</code>,
<code>Libraries/Realizations/Default/boolean.default.wfrog</code>, and their canonical
SVG templates. A fallback HTML-only Button or Boolean widget is not a valid
implementation of this example.
</p>

<p>
The Example 10 visual corridor was accepted on 2026-05-15 for this bounded
Button-to-Boolean behavior. The acceptance is narrow: it freezes the Default
Button rectangular SVG realization as consumed by this example, including
instance-owned labels, ON/OFF state text, <code>button_face</code> border styling, hover/pressed
state properties, host overlay alignment to the published <code>button_face</code>
part, <code>behavior.mechanical_action=switch_until_released</code>, and the
read-only Boolean circular indicator. It does not claim broad runtime
completeness beyond the validated Example 10 corridor.
</p>

<p>
The Button class law names the six LabVIEW-style mechanical actions, but this
example accepts only the momentary <code>switch_until_released</code> behavior.
The other five actions require their own bounded examples before a runtime may
claim support for them.
</p>
