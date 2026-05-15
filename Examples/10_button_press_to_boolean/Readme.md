<h1>Example 10 - Button Press To Boolean</h1>

<p>
Example 10 is the bounded Button widget acceptance slice. It keeps the behavior
intentionally small: one Button control publishes its momentary pressed value,
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
instance-owned labels, ON/OFF state text, face border styling, hover/pressed
state properties, host overlay alignment to the published <code>face</code>
part, and the read-only Boolean circular indicator. It does not claim broad
runtime completeness beyond the validated Example 10 corridor.
</p>
