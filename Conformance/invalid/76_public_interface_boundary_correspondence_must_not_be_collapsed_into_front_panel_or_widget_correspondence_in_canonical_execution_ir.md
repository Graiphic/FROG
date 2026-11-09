<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 76</h1>

<p align="center">
  <strong>Public interface boundary correspondence must not be collapsed into front-panel or widget correspondence in canonical execution IR</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#case-name">1. Case Name</a></li>
  <li><a href="#expected">2. Expected</a></li>
  <li><a href="#why">3. Why</a></li>
  <li><a href="#boundary-being-violated">4. Boundary Being Violated</a></li>
  <li><a href="#scenario">5. Scenario</a></li>
  <li><a href="#what-makes-the-case-invalid">6. What Makes the Case Invalid</a></li>
  <li><a href="#why-ui-side-relation-is-not-an-interface-boundary-substitute">7. Why UI-Side Relation Is Not an Interface-Boundary Substitute</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>76_public_interface_boundary_correspondence_must_not_be_collapsed_into_front_panel_or_widget_correspondence_in_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivation attempted</p>
<p><strong>Expected IR schema result:</strong> may still be schema-valid in shape</p>
<p><strong>Expected IR architectural result:</strong> invalid</p>
<p><strong>Expected preservation:</strong> rejected</p>

<p><strong>Expected rejection basis:</strong> canonical execution IR preserves only front-panel or widget-side correspondence where public interface boundary correspondence should also remain explicitly recoverable as a distinct execution-boundary relation.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case rejects a specific architectural collapse:</p>

<pre><code>public interface boundary correspondence
                -&gt;
front-panel or widget correspondence only
</code></pre>

<p>That collapse is invalid because UI-side relation is not equivalent to public interface boundary anchoring.</p>

<hr/>

<h2 id="boundary-being-violated">4. Boundary Being Violated</h2>
<p>This case violates the distinction between:</p>

<ul>
  <li>execution-facing public boundary relation, and</li>
  <li>interaction-facing UI-side relation.</li>
</ul>

<p>The rejected shortcut is:</p>

<pre><code>a widget-side or front-panel relation exists
        therefore
public interface boundary relation no longer matters
</code></pre>

<p>That shortcut is architecturally invalid.</p>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program contains a public interface boundary and also contains front-panel or widget-side material.</p>

<p>During canonical execution IR construction:</p>

<ul>
  <li>an execution-facing object remains linked to UI-side source material,</li>
  <li>that UI-side link may be well-formed and recoverable,</li>
  <li>but the distinct relation to public interface boundary participation is omitted, erased, or folded into the UI-side relation,</li>
  <li>or the reader is expected to infer interface-boundary anchoring indirectly from UI-side material.</li>
</ul>

<p>The result is an IR that still preserves some source-side relation, but preserves the wrong architectural category.</p>

<hr/>

<h2 id="what-makes-the-case-invalid">6. What Makes the Case Invalid</h2>
<p>This case is invalid when at least one of the following occurs:</p>

<ul>
  <li>public interface boundary correspondence is omitted while widget-side relation remains,</li>
  <li>front-panel correspondence is treated as if it were equivalent to public interface boundary anchoring,</li>
  <li>widget relation is used as the sole preserved source-side anchor for an execution-boundary object,</li>
  <li>interface-boundary recovery depends on UI heuristics or implementation-private assumptions,</li>
  <li>the canonical IR reader cannot distinguish whether an object is anchored to public interface participation or only to UI-side material.</li>
</ul>

<p>Any of those outcomes breaks the required boundary preservation.</p>

<hr/>

<h2 id="why-ui-side-relation-is-not-an-interface-boundary-substitute">7. Why UI-Side Relation Is Not an Interface-Boundary Substitute</h2>
<p>UI-side relation is not an interface-boundary substitute because front panel and widgets are not the public execution boundary of the program.</p>

<p>Even where UI-side objects exist and are useful:</p>

<ul>
  <li>they do not define public interface participation by themselves,</li>
  <li>they do not replace canonical execution-boundary ownership,</li>
  <li>they do not authorize collapse of execution-facing public boundary relation into interaction-facing correspondence.</li>
</ul>

<p>If such a substitution were allowed:</p>

<ul>
  <li>public interface meaning would become UI-dependent,</li>
  <li>non-UI implementations would become harder to compare,</li>
  <li>execution-facing boundary law would drift toward interaction-facing artifacts.</li>
</ul>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be rejected as invalid.</p>

<p>The rejection is architectural:</p>

<ul>
  <li>source may be valid,</li>
  <li>meaning may be established,</li>
  <li>IR may still preserve some source-side correspondence,</li>
  <li>but public interface boundary correspondence has been wrongly collapsed into UI-side correspondence.</li>
</ul>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This rejection is the natural opposite of the valid interface-boundary correspondence distinction case.</p>

<p>It closes the next coherent local gap:</p>

<pre><code>source-side relation survives
but is not correctly preserved
if interface-boundary anchoring
has been replaced by UI-side correspondence
</code></pre>

<p>That keeps canonical execution IR aligned with the repository’s earlier interface/widget/front-panel separations rather than undoing them at the IR correspondence layer.</p>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>UI-side relation remains recoverable,</li>
  <li>public interface boundary correspondence has been collapsed or lost,</li>
  <li>that collapse is invalid,</li>
  <li>the case must be rejected.</li>
</ul>
