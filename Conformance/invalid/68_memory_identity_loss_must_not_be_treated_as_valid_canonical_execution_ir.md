<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 68</h1>

<p align="center">
  <strong>Memory identity loss must not be treated as valid canonical execution IR</strong><br/>
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
  <li><a href="#why-persistence-behavior-is-not-enough">7. Why Persistence Behavior Is Not Enough</a></li>
  <li><a href="#expected-conformance-result">8. Expected Conformance Result</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>68_memory_identity_loss_must_not_be_treated_as_valid_canonical_execution_ir</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected loadability:</strong> loadable</p>
<p><strong>Expected structural validity:</strong> valid</p>
<p><strong>Expected meaning:</strong> established</p>
<p><strong>Expected IR result:</strong> derivation attempted</p>
<p><strong>Expected IR schema result:</strong> may still be schema-valid in shape</p>
<p><strong>Expected IR architectural result:</strong> invalid</p>
<p><strong>Expected preservation:</strong> rejected</p>

<p><strong>Expected rejection basis:</strong> explicit memory identity present in validated meaning is no longer recoverable in canonical execution IR.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case rejects the loss of explicit memory identity.</p>

<p>The rejected collapse is:</p>

<pre><code>explicit memory identity
            -&gt;
anonymous persistence behavior
</code></pre>

<p>That collapse is invalid because canonical execution IR must preserve public execution-facing memory meaning, not merely a runtime impression that “something stateful happens.”</p>

<hr/>

<h2 id="boundary-being-violated">4. Boundary Being Violated</h2>
<p>This case violates the distinction between:</p>

<ul>
  <li>explicit memory identity, and</li>
  <li>unidentified storage or persistence behavior.</li>
</ul>

<p>The rejected substitution is:</p>

<pre><code>recoverable memory entity
            -/-> inferred hidden storage effect
</code></pre>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A validated program contains explicit memory participation.</p>

<p>During canonical execution IR derivation:</p>

<ul>
  <li>the program remains semantically accepted,</li>
  <li>execution-facing stateful behavior may still seem present,</li>
  <li>but the IR no longer provides a recoverable identity for the memory entity itself,</li>
  <li>or memory is represented only through generic effects, timing, or runtime-private storage conventions.</li>
</ul>

<p>The resulting IR no longer preserves the explicit memory boundary.</p>

<hr/>

<h2 id="what-makes-the-case-invalid">6. What Makes the Case Invalid</h2>
<p>This case is invalid when at least one of the following occurs:</p>

<ul>
  <li>memory identity is omitted,</li>
  <li>multiple memory entities are collapsed into anonymous persistence,</li>
  <li>identity is replaced by scheduler, cache, or executor folklore,</li>
  <li>recovery of memory identity depends on runtime-private structures,</li>
  <li>the IR preserves behavior-like symptoms but not the explicit memory entity.</li>
</ul>

<p>Any of those outcomes breaks architectural preservation.</p>

<hr/>

<h2 id="why-persistence-behavior-is-not-enough">7. Why Persistence Behavior Is Not Enough</h2>
<p>Persistence behavior is not enough because public canonical execution IR must explain what kind of execution-facing entities exist, not merely what one runtime happens to do internally.</p>

<p>If memory identity can be erased while behavior remains stateful-looking, then:</p>

<ul>
  <li>distinct memory entities may be confused,</li>
  <li>independent implementations may diverge silently,</li>
  <li>conformance loses the ability to compare memory preservation,</li>
  <li>runtime-private realization starts replacing open IR meaning.</li>
</ul>

<hr/>

<h2 id="expected-conformance-result">8. Expected Conformance Result</h2>
<p>This case must be rejected as invalid.</p>

<p>The rejection is architectural:</p>

<ul>
  <li>source may remain valid,</li>
  <li>meaning may still be established,</li>
  <li>IR may still look executable,</li>
  <li>but explicit memory identity has been lost.</li>
</ul>

<hr/>

<h2 id="rationale">9. Rationale</h2>
<p>This rejection is the natural opposite of the valid memory-identity preservation case.</p>

<p>It closes a precise local gap:</p>

<pre><code>explicit memory
is not preserved
if only stateful behavior remains
but memory identity disappears
</code></pre>

<p>That keeps FROG aligned with its explicit-memory discipline instead of allowing hidden persistence to re-enter through the IR boundary.</p>

<hr/>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>meaning is established,</li>
  <li>explicit memory identity exists upstream,</li>
  <li>the IR no longer preserves that identity,</li>
  <li>the resulting canonical execution IR is invalid,</li>
  <li>the case must be rejected.</li>
</ul>
