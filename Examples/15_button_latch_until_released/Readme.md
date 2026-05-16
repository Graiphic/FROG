<h1>Example 15 - Button Latch Until Released</h1>

<p>
Example 15 is a bounded Button widget acceptance slice for
<code>behavior.mechanical_action=latch_until_released</code>.
It uses one Button control labelled <code>Trigger</code> and one read-only
Boolean indicator labelled <code>Latched</code>.
</p>

<p>
The Button is a command/value widget: the press edge latches TRUE, continuous
program reads observe TRUE while the button remains pressed, and the value
resets once the button has been released and the program has read the value.
In this example the visible behavior intentionally resembles
switch-until-released while still preserving the latch reset rule.
</p>

<pre><code>trigger_button.value -&gt; latched_indicator.value
trigger_button.value -&gt; public output latched</code></pre>

<p>
The visible UI must consume the Default Button SVG and Default Boolean SVG
assets through the <code>.frog</code> instance data and the
<code>.wfrog</code> realization package. The runtime may provide host input
overlays, but those overlays must align to the SVG-published Button face part
and must not define a fallback Button visual.
</p>

<p>
The C++ behavior was accepted first, then the Python and Rust reference runtime
source surfaces were aligned to the same Button latch contract. The reference
runtimes can execute the Button value copy through the manifest-declared native
bool kernel artifact while still consuming the <code>.wfrog</code> package and
Default SVG realizations. LLVM remains a backend producer of the native
artifact; it is not the runtime identity.
</p>

