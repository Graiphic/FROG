<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1>Conformance Case — Invalid: Widget Must Not Be Promoted to Public Interface</h1>

<p><strong>Invalid conformance case for collapsing widget-owned value participation into public interface participation in FROG v0.1</strong><br>
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

<p>This invalid case covers a program shape or tool behavior that treats a front-panel widget as though it automatically created a public interface port.</p>

<p>Typical anti-patterns include:</p>

<ul>
  <li>accepting a widget declaration as sufficient proof of public interface input existence,</li>
  <li>rewriting a diagram-side <code>widget_value</code> participation into <code>interface_input</code>,</li>
  <li>deriving a callable public argument from a widget without an explicit interface declaration,</li>
  <li>merging public interface participation and widget-owned value participation into one generic endpoint category.</li>
</ul>

<p>Conceptually, the forbidden collapse is:</p>

<pre><code>front-panel widget declaration
          ==
public interface declaration
</code></pre>

<p>or:</p>

<pre><code>widget_value(ctrl_gain)
          ==
interface_input(gain)
</code></pre>

<p>Base FROG v0.1 does not allow that equivalence.</p>

<hr>

<h2 id="expected-status">2. Expected Status</h2>

<p>Expected: invalid</p>

<hr>

<h2 id="intended-anti-pattern">3. Intended Anti-Pattern</h2>

<p>The invalid intent is any source shape, validator behavior, derivation behavior, or lowering behavior that requires a widget to be promoted into a public interface role in order to “make sense”.</p>

<p>A minimal conceptual anti-pattern is:</p>

<pre><code>front panel:
  numeric control "gain"

diagram:
  widget_value(gain) ----&gt; frog.core.add

tool interpretation:
  "gain is therefore also a public input"
</code></pre>

<p>Another invalid interpretation is:</p>

<pre><code>public callable argument list
  auto-generated from widgets
</code></pre>

<p>without an explicit interface declaration establishing that contract.</p>

<hr>

<h2 id="boundaries-exercised">4. Boundaries Exercised</h2>

<ul>
  <li>public interface ownership,</li>
  <li>front-panel ownership,</li>
  <li><code>interface_input</code> versus <code>widget_value</code>,</li>
  <li>public contract participation versus widget-owned value participation,</li>
  <li>rejection of semantic collapse across published participation categories.</li>
</ul>

<hr>

<h2 id="why-this-case-is-invalid">5. Why this Case Is Invalid</h2>

<p>This case is invalid because base FROG v0.1 publishes these roles as distinct:</p>

<ul>
  <li>public interface participation is established through explicit interface declaration and represented in the diagram through <code>interface_input</code> and <code>interface_output</code>,</li>
  <li>front-panel widgets belong to the front-panel/UI surface and do not create public callable contract ports by themselves,</li>
  <li><code>widget_value</code> is the natural value-participation path for a widget’s primary value,</li>
  <li>public interface contract and widget-owned UI participation may coexist, but they are not interchangeable.</li>
</ul>

<p>Any case that depends on automatically promoting a widget into a public interface role contradicts the published architecture.</p>

<p>The required separation is:</p>

<pre><code>public interface boundary
          !=
widget-owned value contribution
</code></pre>

<hr>

<h2 id="expected-validation-outcome">6. Expected Validation Outcome</h2>

<p>A conforming validator should reject the case when the source or tool behavior requires such a promotion in order to establish program meaning.</p>

<p>It should report that at least one of the following boundaries has been violated:</p>

<ul>
  <li>public interface declaration versus front-panel widget declaration,</li>
  <li><code>interface_input</code> participation versus <code>widget_value</code> participation,</li>
  <li>public callable contract versus widget-owned UI state.</li>
</ul>

<p>If the case attempts to justify the promotion through tooling convenience, layout intent, or backend preference, that should still be rejected. None of those creates public interface law.</p>

<hr>

<h2 id="why-derivation-is-forbidden">7. Why Derivation Is Forbidden</h2>

<p>Execution IR derivation is forbidden for this invalid case because validation has not established a legitimate program meaning.</p>

<p>There is no conforming basis for deriving:</p>

<ul>
  <li>a valid public entry participation object for the promoted widget,</li>
  <li>a valid callable program contract automatically inferred from widget presence alone,</li>
  <li>or a valid backend-facing contract that pretends the widget was always a public interface argument.</li>
</ul>

<p>Derivation would first require an unpublished semantic reinterpretation. That is not allowed.</p>

<hr>

<h2 id="why-silent-repair-is-wrong">8. Why Silent Repair Is Wrong</h2>

<p>A conforming toolchain must not silently “repair” this case by:</p>

<ul>
  <li>creating interface ports automatically from widgets,</li>
  <li>rewriting <code>widget_value</code> as <code>interface_input</code>,</li>
  <li>generating a public callable signature from front-panel controls alone while still claiming source-level validity,</li>
  <li>merging interface participation and widget participation into one generic endpoint class.</li>
</ul>

<p>Such behavior would erase distinctions that the published source-to-meaning boundary requires to remain explicit and recoverable.</p>

<p>The forbidden repair pattern is:</p>

<pre><code>widget exists
    -/-> therefore public port exists

widget_value
    -/-> interface_input
</code></pre>

<hr>

<h2 id="summary">9. Summary</h2>

<p>This case must be rejected because FROG v0.1 treats public interface participation and widget-owned value participation as distinct published categories.</p>

<p>A conforming implementation must reject source or tool behavior that depends on automatically promoting a widget into a public interface role.</p>

<p>The essential rule is:</p>

<pre><code>widget declaration
    does not create
public interface declaration
</code></pre>
