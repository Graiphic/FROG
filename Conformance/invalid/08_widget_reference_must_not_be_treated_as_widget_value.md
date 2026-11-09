<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1>Conformance Case — Invalid: Widget Reference Must Not Be Treated as Widget Value</h1>

<p><strong>Invalid conformance case for collapsing <code>widget_reference</code> into <code>widget_value</code> in FROG v0.1</strong><br>
FROG — Free Open Graphical Language</p>

<hr>

<h2>Contents</h2>
<ul>
  <li><a href="#case-overview">1. Case Overview</a></li>
  <li><a href="#expected-status">2. Expected Status</a></li>
  <li><a href="#intended-anti-pattern">3. Intended Anti-Pattern</a></li>
  <li><a href="#boundaries-exercised">4. Boundaries Exercised</a></li>
  <li><a href="#why-this-case-is-invalid">5. Why this Case Is Invalid</a></li>
  <li><a href="#expected-validation-outcome">6. Expected Validation Outcome</a></li>
  <li><a href="#why-derivation-is-forbidden">7. Why Derivation Is Forbidden</a></li>
  <li><a href="#why-silent-repair-is-wrong">8. Why Silent Repair Is Wrong</a></li>
  <li><a href="#summary">9. Summary</a></li>
</ul>

<hr>

<h2 id="case-overview">1. Case Overview</h2>

<p>This invalid case covers a source shape or tool behavior that treats a widget object reference as though it were the widget’s ordinary primary value.</p>

<p>Typical anti-patterns include:</p>

<ul>
  <li>allowing <code>widget_reference</code> to feed an ordinary arithmetic primitive,</li>
  <li>rewriting <code>widget_reference</code> into <code>widget_value</code> in order to make a graph type-check,</li>
  <li>treating a widget object handle as though it were the widget’s scalar or structured primary value,</li>
  <li>erasing the distinction between object participation and value participation in validation, derivation, or lowering.</li>
</ul>

<p>Conceptually, the forbidden collapse is:</p>

<pre><code>widget_reference(slider)
          ==
widget_value(slider)
</code></pre>

<p>Base FROG v0.1 does not allow that equivalence.</p>

<hr>

<h2 id="expected-status">2. Expected Status</h2>

<p><strong>Expected:</strong> invalid</p>

<hr>

<h2 id="intended-anti-pattern">3. Intended Anti-Pattern</h2>

<p>The invalid intent is any source shape, validator behavior, derivation behavior, or lowering behavior that requires a widget object reference to behave as an ordinary value in order to “make sense”.</p>

<p>A minimal conceptual anti-pattern is:</p>

<pre><code>front panel:
  numeric control "slider"

diagram:
  widget_reference(slider) ----&gt; frog.core.add
</code></pre>

<p>with tool interpretation such as:</p>

<pre><code>"the reference identifies the widget,
 therefore it can stand in for the widget's value"
</code></pre>

<p>Another invalid interpretation is:</p>

<pre><code>widget_reference(slider)
    auto-coerced to
widget_value(slider)
</code></pre>

<p>without an explicit published rule authorizing such a conversion.</p>

<hr>

<h2 id="boundaries-exercised">4. Boundaries Exercised</h2>

<ul>
  <li>front-panel widget ownership,</li>
  <li><code>widget_reference</code> versus <code>widget_value</code>,</li>
  <li>object participation versus ordinary value participation,</li>
  <li>UI-authorized primitive usage versus ordinary primitive usage,</li>
  <li>rejection of semantic collapse across published widget participation categories.</li>
</ul>

<hr>

<h2 id="why-this-case-is-invalid">5. Why this Case Is Invalid</h2>

<p>This case is invalid because base FROG v0.1 publishes these roles as distinct:</p>

<ul>
  <li><code>widget_value</code> denotes the widget’s primary value participation,</li>
  <li><code>widget_reference</code> denotes widget-object reference participation,</li>
  <li>ordinary value-flow primitives consume values according to their declared primitive behavior,</li>
  <li>reference-consuming behavior is allowed only in explicitly authorized standardized UI contexts.</li>
</ul>

<p>A widget object reference is therefore not a free substitute for the widget’s value.</p>

<p>The required separation is:</p>

<pre><code>widget object reference
          !=
widget primary value
</code></pre>

<p>Any case that depends on treating a reference as a value contradicts the published architecture.</p>

<hr>

<h2 id="expected-validation-outcome">6. Expected Validation Outcome</h2>

<p>A conforming validator should reject the case when the source or tool behavior requires <code>widget_reference</code> to be accepted as an ordinary value contribution.</p>

<p>It should report that at least one of the following boundaries has been violated:</p>

<ul>
  <li><code>widget_reference</code> participation versus <code>widget_value</code> participation,</li>
  <li>object-style UI participation versus ordinary value-flow participation,</li>
  <li>authorized UI-reference consumption versus ordinary primitive consumption.</li>
</ul>

<p>If the case attempts to justify the collapse through implementation convenience, implicit coercion, backend preference, or “obvious intent”, that should still be rejected. None of those creates language law.</p>

<hr>

<h2 id="why-derivation-is-forbidden">7. Why Derivation Is Forbidden</h2>

<p>Execution IR derivation is forbidden for this invalid case because validation has not established a legitimate program meaning.</p>

<p>There is no conforming basis for deriving:</p>

<ul>
  <li>a value-flow edge from a widget object reference into an ordinary arithmetic primitive,</li>
  <li>a legitimate scalar or structured value contribution from the reference alone,</li>
  <li>or a backend-facing contract that pretends the reference was always a normal value.</li>
</ul>

<p>Derivation would first require an unpublished semantic reinterpretation. That is not allowed.</p>

<hr>

<h2 id="why-silent-repair-is-wrong">8. Why Silent Repair Is Wrong</h2>

<p>A conforming toolchain must not silently “repair” this case by:</p>

<ul>
  <li>rewriting <code>widget_reference</code> into <code>widget_value</code>,</li>
  <li>extracting a widget primary value from the reference without an explicit published rule,</li>
  <li>treating all widget-originating participations as one generic category,</li>
  <li>allowing ordinary primitives to consume widget references as if they were plain values.</li>
</ul>

<p>Such behavior would erase distinctions that the published source-to-meaning boundary requires to remain explicit and recoverable.</p>

<p>The forbidden repair pattern is:</p>

<pre><code>widget_reference
    -/-> widget_value

object participation
    -/-> ordinary value participation
</code></pre>

<hr>

<h2 id="summary">9. Summary</h2>

<p>This case must be rejected because FROG v0.1 treats widget object reference participation and widget primary value participation as distinct published categories.</p>

<p>A conforming implementation must reject any source or tool behavior that depends on treating <code>widget_reference</code> as though it were <code>widget_value</code>.</p>

<p>The essential rule is:</p>

<pre><code>widget_reference
    does not become
widget_value
</code></pre>
