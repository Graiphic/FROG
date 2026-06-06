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

<hr/>

<h2>Review Status</h2>

<p>
Latest public Path review: <time datetime="2026-06-06">2026-06-06</time>.
The review covers the Example 09 source/package posture, the Default Path
rectangular SVG skin contract, the source-owned <code>focus_ring</code>, and
C++ browser-host rendering of the public example surface.
</p>

<p>
The public repository records the source, <code>.wfrog</code> manifest, Default SVG
asset, and review evidence. Graiphic runtime implementation code remains outside
the public publication scope unless it is deliberately promoted later.
</p>
