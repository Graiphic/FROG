<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 63</h1>

<p align="center">
  <strong>Multi-contributor source attribution must remain explicit in canonical execution IR</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#case-name">1. Case Name</a></li>
  <li><a href="#expected">2. Expected</a></li>
  <li><a href="#why">3. Why</a></li>
  <li><a href="#boundary-being-preserved">4. Boundary Being Preserved</a></li>
  <li><a href="#scenario">5. Scenario</a></li>
  <li><a href="#what-must-remain-true">6. What Must Remain True</a></li>
  <li><a href="#why-single-owner-collapse-is-not-acceptable">7. Why Single-Owner Collapse Is Not Acceptable</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>63_multi_contributor_source_attribution_must_remain_explicit_in_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivable</p>
<p><strong>Expected IR schema result:</strong> schema-valid where applicable</p>
<p><strong>Expected IR architectural result:</strong> valid</p>
<p><strong>Expected preservation:</strong> preserved</p>

<p><strong>Expected preservation basis:</strong> when one canonical execution-facing object is materially attributable to more than one source-side contributor, that plurality must remain explicit in canonical execution IR rather than being silently collapsed into one synthetic owner.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case exists to preserve a stronger attribution rule than simple single-source anchoring.</p>

<p>Some canonical execution IR objects may be derived from one dominant source object. Others may be derived from multiple materially contributing source-side elements.</p>

<p>When multi-contributor attribution exists, the IR must preserve that plurality explicitly.</p>

<p>The preserved rule is:</p>

<pre><code>multiple contributing source-side origins
                    -&gt;
explicit multi-contributor attribution
</code></pre>

<hr/>

<h2 id="boundary-being-preserved">4. Boundary Being Preserved</h2>
<p>This case preserves the distinction between:</p>

<ul>
  <li>a canonical execution object that is attributable to one source-side owner, and</li>
  <li>a canonical execution object that is attributable to multiple source-side contributors.</li>
</ul>

<p>The preserved distinction is:</p>

<pre><code>single-source attribution
            !=
multi-contributor attribution
</code></pre>

<p>The IR may still designate one object as primary where that is architecturally meaningful, but it must not erase the additional contributing sources that remain part of the public derivation explanation.</p>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program contains a local structure whose canonical execution-facing representation depends on multiple source-side contributors.</p>

<p>For example, a derived execution-facing object may depend on:</p>

<ul>
  <li>a primary declaration carrier,</li>
  <li>an explicit structure-boundary participant,</li>
  <li>a state-related source-side contributor,</li>
  <li>another explicit source-side contributor required for the canonical execution interpretation.</li>
</ul>

<p>The canonical execution IR is valid only if a conforming reader can still observe that:</p>

<ul>
  <li>there is more than one contributing source-side origin,</li>
  <li>the plurality is intentional and explicit,</li>
  <li>the result is not merely a runtime-private merge,</li>
  <li>the canonical execution object has not been falsely rewritten as belonging to only one source-side object.</li>
</ul>

<hr/>

<h2 id="what-must-remain-true">6. What Must Remain True</h2>
<p>The following must remain true in a valid reading of this case:</p>

<ul>
  <li>the canonical execution object remains recoverably attributable,</li>
  <li>its attribution is not reduced to one synthetic owner when multiple contributors exist,</li>
  <li>the distinction between primary identity and additional contributors remains readable where both exist,</li>
  <li>multi-contributor derivation remains part of the public canonical IR truth surface.</li>
</ul>

<p>The preserved pattern is:</p>

<pre><code>primary execution identity
+
additional explicit contributors
+
recoverable attribution categories
                -&gt;
valid canonical execution IR
</code></pre>

<hr/>

<h2 id="why-single-owner-collapse-is-not-acceptable">7. Why Single-Owner Collapse Is Not Acceptable</h2>
<p>Single-owner collapse is not acceptable because it weakens the public derivation explanation.</p>

<p>Without explicit multi-contributor attribution:</p>

<ul>
  <li>independent implementations may reconstruct the same object differently,</li>
  <li>later conformance can no longer tell whether additional source-side contributors were preserved,</li>
  <li>non-primary contribution can be confused with accidental omission,</li>
  <li>the canonical IR loses explanatory precision even if runtime behavior appears acceptable.</li>
</ul>

<p>The architecture therefore requires that plurality remain explicit when it exists.</p>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be accepted as valid.</p>

<p>Acceptance means that:</p>

<ul>
  <li>multi-contributor source attribution remains explicit,</li>
  <li>recoverability is preserved at the canonical execution IR layer,</li>
  <li>the IR does not silently collapse materially contributing source-side origins into one owner.</li>
</ul>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This case continues the same conformance direction already established for:</p>

<ul>
  <li>recoverable source attribution,</li>
  <li>intentional non-primary correspondence,</li>
  <li>recoverable source anchoring of primary execution objects.</li>
</ul>

<p>It adds one more local closure:</p>

<pre><code>recoverable source attribution
        -&gt;
not only single-owner anchoring
        -&gt;
but also explicit multi-contributor attribution where present
</code></pre>

<p>That makes the canonical execution IR more inspectable, more comparable across implementations, and less vulnerable to attribution collapse.</p>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>a canonical execution object depends on multiple source-side contributors,</li>
  <li>that plurality remains explicit in the canonical execution IR,</li>
  <li>the case is therefore valid.</li>
</ul>
