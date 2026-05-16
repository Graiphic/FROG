<h1>Example 12 - Button Switch When Released</h1>

<p>
Example 12 is a bounded Button widget acceptance slice for
<code>behavior.mechanical_action=switch_when_released</code>.
It uses one Button control labelled <code>Trigger</code> and one read-only
Boolean indicator labelled <code>Switched</code>.
</p>

<p>
The Button is a command/value widget: the press edge arms the visible Button
interaction, the release edge toggles the stored Button value, and the switched
value remains visible until the next completed press/release gesture. The
Boolean indicator consumes that stored value through the normal widget-value
publication path.
</p>

<pre><code>trigger_button.value -&gt; switched_indicator.value
trigger_button.value -&gt; public output switched</code></pre>

<p>
The visible UI must consume the Default Button SVG and Default Boolean SVG
assets through the <code>.frog</code> instance data and the
<code>.wfrog</code> realization package. The runtime may provide host input
overlays, but those overlays must align to the SVG-published Button face part
and must not define a fallback Button visual.
</p>

<p>
The C++ behavior was accepted first, then the Python and Rust reference runtime
source surfaces were aligned to the same Button event contract. The reference
runtimes can execute the Button value copy through the manifest-declared native
bool kernel artifact while still consuming the <code>.wfrog</code> package and
Default SVG realizations. LLVM remains a backend producer of the native
artifact; it is not the runtime identity.
</p>
