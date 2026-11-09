<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 Conformance Case — 03 UI Property Write</h1>

<p align="center">
  Valid conformance case for widget-reference-driven UI object interaction in FROG v0.1<br/>
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
This case covers minimal object-style UI interaction with:
</p>

<ul>
  <li>one public string input,</li>
  <li>one front-panel widget declaration,</li>
  <li>one diagram-side <code>widget_reference</code>,</li>
  <li>one standardized UI primitive of type <code>frog.ui.property_write</code>.</li>
</ul>

<p>
The case intentionally avoids <code>widget_value</code>.
</p>

<hr/>

<h2 id="status">2. Expected Status</h2>

<p><strong>Expected: valid</strong></p>

<hr/>

<h2 id="source-target">3. Source Target</h2>

<p>
Expected source target:
</p>

<pre><code>Examples/03_ui_property_write/main.frog</code></pre>

<hr/>

<h2 id="boundaries-exercised">4. Boundaries Exercised</h2>

<ul>
  <li>public interface input,</li>
  <li>front-panel widget declaration,</li>
  <li>diagram-side <code>widget_reference</code> participation,</li>
  <li>standardized UI primitive usage through <code>frog.ui.property_write</code>,</li>
  <li>member addressing through <code>widget_member</code>,</li>
  <li>object-style UI interaction in backend handoff.</li>
</ul>

<hr/>

<h2 id="why-this-case-is-valid">5. Why this Case Is Valid</h2>

<p>
This case is valid because:
</p>

<ul>
  <li>the public input is well formed,</li>
  <li>the referenced widget exists,</li>
  <li>the diagram-side <code>widget_reference</code> is valid,</li>
  <li>the primitive reference <code>frog.ui.property_write</code> is valid,</li>
  <li>the addressed widget member is structurally valid,</li>
  <li>the value carried into the property write is type-compatible.</li>
</ul>

<hr/>

<h2 id="expected-validation-outcome">6. Expected Validation Outcome</h2>

<p>
A conforming validator should accept the case.
It should confirm at minimum that:
</p>

<ul>
  <li>the widget declaration is structurally valid,</li>
  <li>the <code>widget_reference</code> node references an existing widget,</li>
  <li>the <code>widget_member</code> descriptor is valid,</li>
  <li>the addressed part and member exist and are writable,</li>
  <li>the <code>ref</code> and <code>value</code> edges into the primitive are valid,</li>
  <li>the source value type is compatible with the targeted property type.</li>
</ul>

<hr/>

<h2 id="expected-derivation-preservation">7. Expected Derivation Preservation</h2>

<p>
Execution IR derivation should preserve explicitly:
</p>

<ul>
  <li>public interface entry participation for the input string,</li>
  <li>widget-reference participation for the referenced widget,</li>
  <li>primitive execution identity for <code>frog.ui.property_write</code>,</li>
  <li>the addressed member role,</li>
  <li>the distinction between widget-reference participation and primitive UI-object operation.</li>
</ul>

<p>
The derivation must not reinterpret this case as:
</p>

<ul>
  <li>natural <code>widget_value</code> participation,</li>
  <li>ordinary unrestricted object storage,</li>
  <li>a generic side effect with no attributable widget interaction origin.</li>
</ul>

<hr/>

<h2 id="expected-backend-contract-preservation">8. Expected Backend Contract Preservation</h2>

<p>
If this case is lowered and emitted as a backend contract, the contract should still make clear that:
</p>

<ul>
  <li>object-style UI interaction support is required,</li>
  <li>widget-reference participation remains distinct,</li>
  <li>the UI-object write operation remains distinct,</li>
  <li>the interaction is property-based rather than natural value participation,</li>
  <li>no first-class event execution model is implied by this case.</li>
</ul>

<hr/>

<h2 id="what-must-not-happen">9. What Must Not Happen</h2>

<ul>
  <li>The case must not be rewritten as if it were ordinary <code>widget_value</code> flow.</li>
  <li>The widget-reference object and the property-write operation must not be collapsed into one untyped generic object.</li>
  <li>The case must not be accepted by inventing unsupported member semantics that were not validated.</li>
</ul>

<hr/>

<h2 id="summary">10. Summary</h2>

<p>
This is the baseline valid conformance case for object-style UI interaction in FROG v0.1.
A conforming toolchain should accept it,
preserve the distinction between widget-reference participation and UI-object primitive operation,
and carry that distinction into backend handoff.
</p>
