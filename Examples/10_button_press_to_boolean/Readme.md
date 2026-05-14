<h1>Example 10 - Button Press To Boolean</h1>

<p>
Example 10 is the bounded Button widget pilot. It keeps the behavior intentionally small:
one Button control publishes its momentary pressed value, the diagram copies that value,
and one Boolean indicator displays the result.
</p>

<pre><code>.frog source
  -&gt; FIR
  -&gt; lowering
  -&gt; LLVM native Button kernel artifact + manifest
  -&gt; C++ / Python / Rust reference runtime
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
