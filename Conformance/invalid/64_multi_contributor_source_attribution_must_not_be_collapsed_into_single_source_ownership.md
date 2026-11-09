<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 64</h1>

<p align="center">
  <strong>Multi-contributor source attribution must not be collapsed into single-source ownership</strong><br/>
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
  <li><a href="#why-primary-owner-alone-is-not-enough">7. Why Primary Owner Alone Is Not Enough</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>64_multi_contributor_source_attribution_must_not_be_collapsed_into_single_source_ownership</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivation attempted</p>
<p><strong>Expected IR schema result:</strong> may still be schema-valid in shape</p>
<p><strong>Expected IR architectural result:</strong> invalid</p>
<p><strong>Expected preservation:</strong> rejected</p>

<p><strong>Expected rejection basis:</strong> a canonical execution object that materially depends on multiple source-side contributors is represented as if it had only one source-side owner.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case rejects attribution collapse.</p>

<p>The failure pattern is:</p>

<pre><code>multiple contributors exist
            -&gt;
IR pretends one source-side owner is enough
</code></pre>

<p>That collapse is invalid because it removes public derivation truth that the canonical execution IR is required to preserve.</p>

<hr/>

<h2 id="boundary-being-violated">4. Boundary Being Violated</h2>
<p>This case violates the distinction between:</p>

<ul>
  <li>primary execution identity, and</li>
  <li>the full explicit contributor set required to explain derivation correctly.</li>
</ul>

<p>The rejected collapse is:</p>

<pre><code>primary owner exists
        therefore
all other contributors may be erased
</code></pre>

<p>That inference is architecturally invalid.</p>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program contains a canonical execution-facing object whose derivation depends on more than one source-side contributor.</p>

<p>During canonical execution IR construction:</p>

<ul>
  <li>one contributor is preserved as visible,</li>
  <li>other materially contributing source-side origins are discarded, hidden, or silently absorbed,</li>
  <li>the resulting IR still looks complete enough for one implementation to proceed,</li>
  <li>but a conforming reader can no longer recover the original plurality of contributors.</li>
</ul>

<p>The failure is not merely informational loss. It is a preservation failure.</p>

<hr/>

<h2 id="what-makes-the-case-invalid">6. What Makes the Case Invalid</h2>
<p>This case is invalid when at least one of the following occurs:</p>

<ul>
  <li>the IR records only one owner when multiple contributors remain architecturally relevant,</li>
  <li>additional contributors are omitted without an explicit intentional category,</li>
  <li>the IR disguises attribution loss as simplification,</li>
  <li>plural contribution is pushed into runtime-private interpretation instead of canonical IR truth,</li>
  <li>the resulting object cannot be publicly understood as multi-contributor anymore.</li>
</ul>

<p>Any of those situations breaks canonical execution IR preservation.</p>

<hr/>

<h2 id="why-primary-owner-alone-is-not-enough">7. Why Primary Owner Alone Is Not Enough</h2>
<p>A primary owner alone is not enough when the architecture requires that multiple contributors remain explicit.</p>

<p>Without that explicit plurality:</p>

<ul>
  <li>derivation review becomes incomplete,</li>
  <li>non-primary participation can no longer be distinguished from accidental omission,</li>
  <li>independent implementations may diverge in how they reconstruct the same object,</li>
  <li>the public IR loses accountability.</li>
</ul>

<p>The preserved rule is not “always store one owner.”</p>

<p>The preserved rule is “store enough explicit attribution to preserve the true derivation boundary.”</p>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be rejected as invalid.</p>

<p>The rejection is specifically architectural:</p>

<ul>
  <li>source may still be valid,</li>
  <li>meaning may still be established,</li>
  <li>IR may still look well-shaped,</li>
  <li>but attribution plurality has been collapsed incorrectly.</li>
</ul>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This rejection extends the repository’s existing protection against:</p>

<ul>
  <li>loss of source attribution,</li>
  <li>disguised identity loss,</li>
  <li>missing source anchors for primary execution objects.</li>
</ul>

<p>It closes the next local gap:</p>

<pre><code>source attribution is not preserved
if plurality is erased
while one owner is left behind
</code></pre>

<p>The architecture must preserve not just the existence of attribution, but the correct shape of attribution when the derivation is multi-contributor.</p>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>multiple source-side contributors materially exist,</li>
  <li>the IR collapses them into one owner,</li>
  <li>that collapse is architecturally invalid,</li>
  <li>the case must be rejected.</li>
</ul>
