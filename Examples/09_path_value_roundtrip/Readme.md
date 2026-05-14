<h1>Example 09 - Path Value Roundtrip</h1>

<p>
Example 09 is the bounded Path widget pilot. It keeps the behavior intentionally small:
one Path control publishes a path value, the diagram copies that value, and one Path
indicator displays the result.
</p>

<pre><code>.frog source
  -&gt; FIR
  -&gt; lowering
  -&gt; LLVM native path kernel artifact + manifest
  -&gt; C++ reference runtime
  -&gt; .wfrog front panel binding
  -&gt; Default Path realization SVG
</code></pre>

<p>
The visible front panel must consume <code>ui/path_panel.wfrog</code>, which points to
<code>Libraries/Realizations/Default/path.default.wfrog</code> and the Default Path SVG
template. A fallback HTML-only Path widget is not a valid implementation of this example.
</p>
