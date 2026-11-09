<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 Conformance Case — 02 UI Value Roundtrip</h1>

<p align="center">
  Valid conformance case for natural widget-value participation in FROG v0.1<br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#case-overview">1. Case Overview</a></li>
  <li><a href="#status">2. Expected Status</a></li>
  <li><a href="#source-target">3. Source Target</a></li>
  <li><a href="#boundaries-exercised">4. Boundaries Exercised</a></li>
  <li><a href="#why-this-case-is-valid">5. Why this Case Is Valid</a></li>
  <li><a href="#expected-validation-outcome">6. Expected Validation Outcome</a></li>
  <li><a href="#expected-derivation-preservation">7. Expected Derivation Preservation</a></li>
  <li><a href="#expected-backend-contract-preservation">8. Expected Backend Contract Preservation</a></li>
  <li><a href="#what-must-not-happen">9. What Must Not Happen</a></li>
  <li><a href="#summary">10. Summary</a></li>
</ul>

<hr/>

<h2 id="case-overview">1. Case Overview</h2>

<p>
This case covers a minimal front-panel-driven program using the natural primary-value path:
</p>

<ul>
  <li>two numeric controls,</li>
  <li>one arithmetic primitive,</li>
  <li>one numeric indicator,</li>
  <li>diagram-side <code>widget_value</code> participation only.</li>
</ul>

<p>
The case intentionally excludes object-style widget interaction.
</p>

<hr/>

<h2 id="status">2. Expected Status</h2>

<p><strong>Expected: valid</strong></p>

<hr/>

<h2 id="source-target">3. Source Target</h2>

<p>
Expected source target:
</p>

<pre><code>Examples/02_ui_value_roundtrip/main.frog</code></pre>

<hr/>

<h2 id="boundaries-exercised">4. Boundaries Exercised</h2>

<ul>
  <li>front-panel widget declaration,</li>
  <li>value-carrying widget classes,</li>
  <li>diagram-side <code>widget_value</code> participation,</li>
  <li>primitive execution through <code>frog.core.add</code>,</li>
  <li>clear separation between front-panel declaration and executable participation,</li>
  <li>basic backend-family UI value binding.</li>
</ul>

<hr/>

<h2 id="why-this-case-is-valid">5. Why this Case Is Valid</h2>

<p>
This case is valid because:
</p>

<ul>
  <li>the front-panel widget declarations are well formed,</li>
  <li>the referenced widgets are value-carrying widgets,</li>
  <li>the diagram-side <code>widget_value</code> nodes refer to existing widgets,</li>
  <li>the arithmetic primitive is used over type-compatible values,</li>
  <li>the graph remains acyclic and does not require explicit memory.</li>
</ul>

<hr/>

<h2 id="expected-validation-outcome">6. Expected Validation Outcome</h2>

<p>
A conforming validator should accept the case.
It should confirm at minimum that:
</p>

<ul>
  <li>the front-panel structure is valid,</li>
  <li>widget identifiers are unique,</li>
  <li>the selected standard widget classes are valid,</li>
  <li>the declared <code>value_type</code> is valid for the selected widgets,</li>
  <li>each <code>widget_value</code> node references an existing value-carrying widget,</li>
  <li>control-side and indicator-side participation are used consistently,</li>
  <li>the primitive input and output edges are correct.</li>
</ul>

<hr/>

<h2 id="expected-derivation-preservation">7. Expected Derivation Preservation</h2>

<p>
Execution IR derivation should preserve explicitly:
</p>

<ul>
  <li>widget-value participation for each control widget,</li>
  <li>primitive execution identity for <code>frog.core.add</code>,</li>
  <li>widget-value participation for the indicator widget,</li>
  <li>the validated dependency edges,</li>
  <li>the distinction between front-panel declaration and diagram participation.</li>
</ul>

<p>
The derivation must not reinterpret this case as:
</p>

<ul>
  <li>public interface participation,</li>
  <li><code>widget_reference</code> participation,</li>
  <li>property-based access to a member named <code>value</code>,</li>
  <li>implicit UI event execution.</li>
</ul>

<hr/>

<h2 id="expected-backend-contract-preservation">8. Expected Backend Contract Preservation</h2>

<p>
If this case is lowered and emitted as a backend contract, the contract should still make clear that:
</p>

<ul>
  <li>UI value participation is required by the lowered form,</li>
  <li>the participation is of the natural <code>widget_value</code> kind,</li>
  <li>the contract does not require object-style widget access,</li>
  <li>no first-class event execution model is implied,</li>
  <li>no explicit local-memory semantics are required.</li>
</ul>

<hr/>

<h2 id="what-must-not-happen">9. What Must Not Happen</h2>

<ul>
  <li>The case must not be rewritten as if it required <code>widget_reference</code>.</li>
  <li>The case must not be normalized into property access to widget member <code>value</code>.</li>
  <li>The case must not silently assume a richer event model than the published v0.1 base.</li>
</ul>

<hr/>

<h2 id="summary">10. Summary</h2>

<p>
This is the baseline valid conformance case for natural widget-value participation in FROG v0.1.
A conforming toolchain should accept it,
derive it without losing the distinction between declaration and participation,
and keep UI value binding semantics explicit through backend handoff.
</p>
