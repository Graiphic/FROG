<h1>Example 11 - Button Switch When Pressed</h1>

<p>
Reviewed 2026-06-16: this example consumes the Default Button rectangular semantic SVG skin, its published <code>button_face</code> part, and the current Boolean <code>state_face</code> indicator vocabulary in the C++ browser-host rendering path.
</p>

<p>
Example 11 is a bounded Button widget acceptance slice for
<code>behavior.mechanical_action=switch_when_pressed</code>.
It uses one Button control labelled <code>Trigger</code> and one read-only
Boolean indicator labelled <code>Switched</code>.
</p>

<p>
The Button is a command/value widget: each press edge toggles the stored Button
value, and the switched value remains visible until the next press edge.
The Boolean indicator consumes that stored value through the normal widget-value
publication path.
</p>

<pre><code>trigger_button.value -&gt; switched_indicator.value
trigger_button.value -&gt; public output switched</code></pre>

<p>
The visible UI must consume the Default Button SVG and Default Boolean SVG
assets through the <code>.frog</code> instance data and the
<code>.wfrog</code> realization package. The runtime may provide host input
overlays, but those overlays must align to the SVG-published Button <code>button_face</code> part
and must not define a fallback Button visual.
</p>

<p>
The C++ runtime was accepted first. Python and Rust are then kept at the same
validated behavior level for this bounded action. The reference runtimes can
execute the Button value copy through the manifest-declared native bool kernel
artifact while still consuming the <code>.wfrog</code> package and Default SVG
realizations. LLVM remains a backend producer of the native artifact; it is not
the runtime identity.
</p>
