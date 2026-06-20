<h1>Example 47 - Static Image Clipboard Placement</h1>

<p>
Reviewed 2026-06-20: this example introduces
<code>frog.widgets.image_static</code>, a support/media widget for pasted image
assets on the front panel.
</p>

<p>
The example contains two Image widget instances that both reference the same
source-owned SVG media file through <code>media_ref</code>. One instance uses the
natural default placement rectangle; the other is resized to demonstrate that
the media is layout-driven and not hardcoded by the runtime.
</p>

<pre><code>.frog media_ref
  -&gt; project asset path
  -&gt; .wfrog Default Image realization
  -&gt; C++ private browser host
  -&gt; host image element inside media_content</code></pre>

<p>
The public SVG skin parts are <code>root</code>,
<code>placement_bounds</code>, <code>media_region</code>, and
<code>media_content</code>. The default aura band is <code>4px</code> between
<code>placement_bounds</code> and <code>media_region</code>.
</p>

<p>
This is not the Picture widget. Picture remains the dataflow image widget for
runtime-produced image buffers. Image Static is for pasted/imported front-panel
media objects.
</p>

