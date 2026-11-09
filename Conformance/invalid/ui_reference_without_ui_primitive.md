<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 Conformance Case — Invalid: Widget Reference Without UI Primitive</h1>

<p align="center">
  Invalid conformance case for orphan object-style widget participation in FROG v0.1<br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#case-overview">1. Case Overview</a></li>
  <li><a href="#status">2. Expected Status</a></li>
  <li><a href="#intended-anti-pattern">3. Intended Anti-Pattern</a></li>
  <li><a href="#boundaries-exercised">4. Boundaries Exercised</a></li>
  <li><a href="#why-this-case-is-invalid">5. Why this Case Is Invalid</a></li>
  <li><a href="#expected-validation-outcome">6. Expected Validation Outcome</a></li>
  <li><a href="#why-derivation-is-forbidden">7. Why Derivation Is Forbidden</a></li>
  <li><a href="#why-silent-repair-is-wrong">8. Why Silent Repair Is Wrong</a></li>
  <li><a href="#summary">9. Summary</a></li>
</ul>

<hr/>

<h2 id="case-overview">1. Case Overview</h2>

<p>
This invalid case covers a diagram that introduces a <code>widget_reference</code> node
but does not use that reference through a valid standardized UI interaction primitive.
</p>

<p>
Typical anti-patterns include:
</p>

<ul>
  <li>a floating <code>widget_reference</code> node with no meaningful consumer,</li>
  <li>a <code>widget_reference</code> node routed into an ordinary arithmetic primitive,</li>
  <li>a <code>widget_reference</code> node treated as if it were an unrestricted general-purpose value.</li>
</ul>

<hr/>

<h2 id="status">2. Expected Status</h2>

<p><strong>Expected: invalid</strong></p>

<hr/>

<h2 id="intended-anti-pattern">3. Intended Anti-Pattern</h2>

<p>
Conceptually, the invalid shape is:
</p>

<pre><code>widget_reference(ctrl_gain) ----&gt; frog.core.add
</code></pre>

<p>
or:
</p>

<pre><code>widget_reference(ctrl_gain)
</code></pre>

<p>
with no valid object-style UI interaction primitive using the reference.
</p>

<hr/>

<h2 id="boundaries-exercised">4. Boundaries Exercised</h2>

<ul>
  <li>diagram-side <code>widget_reference</code> participation,</li>
  <li>the boundary between object-style widget access and ordinary valueflow,</li>
  <li>validation of standardized UI interaction usage,</li>
  <li>explicit rejection of semantically ungrounded object references.</li>
</ul>

<hr/>

<h2 id="why-this-case-is-invalid">5. Why this Case Is Invalid</h2>

<p>
This case is invalid because <code>widget_reference</code> is not the natural value path and is not defined as an unrestricted general-purpose value in base v0.1.
Its canonical role is to anchor object-style widget interaction through valid <code>frog.ui.*</code> primitives.
</p>

<p>
Accordingly, a bare or misused widget reference does not define a valid executable meaning by itself.
It does not become valid merely because a widget exists in the front panel.
</p>

<hr/>

<h2 id="expected-validation-outcome">6. Expected Validation Outcome</h2>

<p>
A conforming validator should reject the case.
It should report that at least one of the following is wrong:
</p>

<ul>
  <li>the widget reference is not used in a valid object-style interaction pattern,</li>
  <li>the consuming primitive is not a valid <code>frog.ui.*</code> interaction primitive,</li>
  <li>the graph is attempting to treat widget reference participation as ordinary general-purpose valueflow.</li>
</ul>

<hr/>

<h2 id="why-derivation-is-forbidden">7. Why Derivation Is Forbidden</h2>

<p>
Execution IR derivation is forbidden for this invalid case because validation has not established a legitimate execution meaning.
There is no conforming basis for deriving:
</p>

<ul>
  <li>a valid widget-reference participation object,</li>
  <li>a valid UI-object operation,</li>
  <li>or any valid backend-facing UI interaction contract.</li>
</ul>

<hr/>

<h2 id="why-silent-repair-is-wrong">8. Why Silent Repair Is Wrong</h2>

<p>
A conforming toolchain must not silently “repair” this case by:
</p>

<ul>
  <li>inventing a missing <code>frog.ui.*</code> primitive,</li>
  <li>converting the widget reference into <code>widget_value</code>,</li>
  <li>treating the widget reference as a generic opaque runtime object while still claiming source-level validity.</li>
</ul>

<p>
Such behavior would blur published ownership boundaries and would launder invalid source into unsupported semantics.
</p>

<hr/>

<h2 id="summary">9. Summary</h2>

<p>
This case must be rejected because <code>widget_reference</code> in FROG v0.1 is only valid as the object-style anchor used together with a valid standardized UI interaction primitive.
It is not a free-standing general-purpose value.
</p>
