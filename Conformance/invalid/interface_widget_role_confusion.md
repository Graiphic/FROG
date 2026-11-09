<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 Conformance Case — Invalid: Interface / Widget Role Confusion</h1>

<p align="center">
  Invalid conformance case for collapsing public interface participation and widget participation in FROG v0.1<br/>
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
This invalid case covers a program shape that confuses:
</p>

<ul>
  <li>public interface participation,</li>
  <li>natural widget-value participation,</li>
  <li>object-style widget-reference participation.</li>
</ul>

<p>
Typical anti-patterns include:
</p>

<ul>
  <li>treating a front-panel widget declaration as if it automatically created a public interface port,</li>
  <li>using widget participation where an <code>interface_input</code> or <code>interface_output</code> boundary is actually required,</li>
  <li>normalizing <code>widget_value</code> into property access to member <code>value</code>,</li>
  <li>normalizing property access to member <code>value</code> into <code>widget_value</code> participation.</li>
</ul>

<hr/>

<h2 id="status">2. Expected Status</h2>

<p><strong>Expected: invalid</strong></p>

<hr/>

<h2 id="intended-anti-pattern">3. Intended Anti-Pattern</h2>

<p>
Conceptually, the invalid pattern is any source form or validation behavior that collapses distinct published participation categories into one ambiguous endpoint concept.
</p>

<p>
Examples of invalid intent include:
</p>

<pre><code>front_panel widget declaration
   == public interface declaration
</code></pre>

<p>
or:
</p>

<pre><code>widget_value(ctrl_gain)
   == property_write(member = "value")
</code></pre>

<p>
or the reverse.
</p>

<hr/>

<h2 id="boundaries-exercised">4. Boundaries Exercised</h2>

<ul>
  <li>public interface ownership,</li>
  <li>front-panel ownership,</li>
  <li><code>widget_value</code> versus <code>widget_reference</code>,</li>
  <li>natural widget value participation versus object-style property access,</li>
  <li>rejection of semantic collapse across published boundaries.</li>
</ul>

<hr/>

<h2 id="why-this-case-is-invalid">5. Why this Case Is Invalid</h2>

<p>
This case is invalid because base FROG v0.1 publishes these roles as distinct:
</p>

<ul>
  <li>public interface is declared in the interface layer and represented in the diagram through <code>interface_input</code> and <code>interface_output</code>,</li>
  <li>front-panel widgets do not create public interface ports by themselves,</li>
  <li><code>widget_value</code> is the canonical natural path for primary widget value participation,</li>
  <li><code>widget_reference</code> plus <code>frog.ui.*</code> primitives are the canonical object-style interaction path,</li>
  <li>object-style access to widget member <code>value</code> remains semantically distinct from natural <code>widget_value</code> participation.</li>
</ul>

<p>
Any case that collapses those distinctions contradicts the published architecture.
</p>

<hr/>

<h2 id="expected-validation-outcome">6. Expected Validation Outcome</h2>

<p>
A conforming validator should reject the case when the source or tool behavior requires such a collapse in order to “make sense”.
It should report that at least one of the following boundaries has been violated:
</p>

<ul>
  <li>public interface boundary versus widget boundary,</li>
  <li>natural widget-value participation versus object-style interaction,</li>
  <li>declared front-panel ownership versus executable diagram participation.</li>
</ul>

<hr/>

<h2 id="why-derivation-is-forbidden">7. Why Derivation Is Forbidden</h2>

<p>
Execution IR derivation is forbidden for this invalid case because the tool would first need to invent a semantic reinterpretation that is not published.
There is no conforming basis for deriving an execution-facing IR from a source whose meaning depends on collapsing distinct normative roles.
</p>

<hr/>

<h2 id="why-silent-repair-is-wrong">8. Why Silent Repair Is Wrong</h2>

<p>
A conforming toolchain must not silently “repair” this case by:
</p>

<ul>
  <li>creating interface ports automatically from widgets,</li>
  <li>rewriting <code>widget_value</code> as property access to member <code>value</code>,</li>
  <li>rewriting property access to member <code>value</code> as <code>widget_value</code>,</li>
  <li>merging public interface participation and widget participation into one generic endpoint class.</li>
</ul>

<p>
Such behavior would erase distinctions that the published derivation boundary explicitly requires to remain recoverable.
</p>

<hr/>

<h2 id="summary">9. Summary</h2>

<p>
This case must be rejected because FROG v0.1 treats public interface participation, natural widget-value participation, and object-style widget interaction as distinct published categories.
A conforming implementation must reject source or tool behavior that depends on collapsing them.
</p>
