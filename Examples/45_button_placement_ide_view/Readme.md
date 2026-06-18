<h1>Example 45 - Button Placement IDE View</h1>

<p>
Reviewed 2026-06-18: this example validates the Default Button IDE placement posture in the C++ private browser host.
</p>

<p>
The Button exposes <code>placement_bounds</code> as its selectable placement aura and
<code>button_face</code> as its visible command body. The default aura band is
<code>4px</code> on every side. The visible grid is only an IDE/review aid; the
same source-owned widget layout remains valid when the grid is hidden.
</p>

<pre><code>.frog source
  -&gt; FIR
  -&gt; lowering
  -&gt; LLVM native Button bool kernel manifest
  -&gt; C++ private runtime
  -&gt; .wfrog front panel binding
  -&gt; Default Button and Boolean realization SVG assets
</code></pre>

<p>
This example must consume <code>ui/button_ide_panel.wfrog</code>, which references
the canonical Default Button and Default Boolean realization packages. The
Button SVG public parts remain <code>root</code>, <code>label</code>,
<code>caption</code>, <code>placement_bounds</code>, <code>button_face</code>, and
<code>state_text</code>.
</p>
