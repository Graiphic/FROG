<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 28</h1>

<p align="center">
  <strong>Inferred runtime flush order must not be treated as explicit state write visibility</strong><br>
  FROG — Free Open Graphical Language
</p>

<hr>

<h2>Contents</h2>
<ul>
  <li><a href="#case-name">1. Case Name</a></li>
  <li><a href="#expected">2. Expected</a></li>
  <li><a href="#why">3. Why</a></li>
  <li><a href="#boundary-being-exercised">4. Boundary Being Exercised</a></li>
  <li><a href="#scenario">5. Scenario</a></li>
  <li><a href="#what-makes-the-case-invalid">6. What Makes the Case Invalid</a></li>
  <li><a href="#expected-rejection">7. Expected Rejection</a></li>
  <li><a href="#what-must-not-be-laundered">8. What Must Not Be Laundered</a></li>
  <li><a href="#rationale">9. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">10. Minimum Conformance Reading</a></li>
</ul>

<hr>

<h2 id="case-name">1. Case Name</h2>

<p><strong>Case:</strong> <code>28_inferred_runtime_flush_order_must_not_be_treated_as_explicit_state_write_visibility</code></p>

<hr>

<h2 id="expected">2. Expected</h2>

<p><strong>Expected:</strong> invalid</p>

<p><strong>Expected meaning:</strong> language-level meaning is not established if a program requires explicit state write visibility semantics but only runtime flush-order inference is available.</p>

<p><strong>Expected rejection:</strong> inferred runtime flush order must not be promoted to explicit state write visibility.</p>

<hr>

<h2 id="why">3. Why</h2>

<p>This case exists to make one rejection rule explicit:</p>

<pre><code>inferred runtime flush order
            -/-&gt;
explicit state write visibility
</code></pre>

<p>A conforming implementation may choose one buffering model, one commit strategy, one propagation mechanism, or one backend-private write-flush discipline.</p>

<p>Those choices do not satisfy a language requirement for explicit semantically established state write visibility.</p>

<p>If the visibility of a state write must be established by the validated program meaning, an implementation must reject the case rather than silently laundering one observed or convenient runtime flush order into semantic truth.</p>

<hr>

<h2 id="boundary-being-exercised">4. Boundary Being Exercised</h2>

<p>This case exercises the normative boundary:</p>

<pre><code>canonical .frog source
        |
        v
validated program meaning
</code></pre>

<p>It specifically tests the failure mode in which:</p>
<ul>
  <li>state participation exists or is being relied upon,</li>
  <li>visibility of a state write matters semantically,</li>
  <li>but the implementation attempts to substitute runtime flush behavior for explicit program meaning.</li>
</ul>

<p>This is a source-to-meaning failure, not a runtime propagation success.</p>

<hr>

<h2 id="scenario">5. Scenario</h2>

<p>A FROG program relies on one state write visibility relationship in a context where the published rules require that visibility to be established by the validated program meaning.</p>

<p>However, the program meaning does not actually establish that visibility explicitly enough.</p>

<p>Instead, a tool attempts to continue by assuming one of the following:</p>
<ul>
  <li>the order in which one runtime flushes writes,</li>
  <li>the order in which one backend commits state updates,</li>
  <li>one buffering implementation habit,</li>
  <li>one runtime convenience propagation strategy,</li>
  <li>some other non-semantic inferred write-visibility behavior.</li>
</ul>

<p>The program is therefore not allowed to claim the same meaning as a program whose state write visibility is explicitly established by the published language rules.</p>

<hr>

<h2 id="what-makes-the-case-invalid">6. What Makes the Case Invalid</h2>

<p>This case is invalid because it attempts to obtain valid state-write visibility meaning without satisfying the source-side and language-side requirement that the visibility be semantically established.</p>

<p>More specifically:</p>
<ul>
  <li>the required visibility fact is not established by validated program meaning,</li>
  <li>the missing semantic fact is being replaced by runtime flush inference,</li>
  <li>the implementation would therefore be inventing meaning rather than validating meaning,</li>
  <li>the resulting behavior would become implementation-defined instead of specification-defined.</li>
</ul>

<p>Accordingly, validation must fail.</p>

<hr>

<h2 id="expected-rejection">7. Expected Rejection</h2>

<p>A conforming implementation must reject this case as failing to establish valid language-level meaning.</p>

<p>The rejection should make clear that:</p>
<ul>
  <li>required explicit state write visibility was not established,</li>
  <li>a merely inferred runtime flush order is not an acceptable substitute,</li>
  <li>derivation must not proceed as though valid visibility meaning already existed.</li>
</ul>

<p>The semantic reading is therefore:</p>

<pre><code>missing required explicit state write visibility
        -&gt;
no valid established visibility meaning
        -&gt;
validation failure
</code></pre>

<hr>

<h2 id="what-must-not-be-laundered">8. What Must Not Be Laundered</h2>

<p>A conforming implementation must <strong>not</strong> do any of the following:</p>

<ul>
  <li>treat one observed runtime flush order as if the source had explicitly established state write visibility,</li>
  <li>treat backend commit convenience as a semantic replacement for explicit visibility meaning,</li>
  <li>derive execution IR as though valid state write visibility meaning already existed,</li>
  <li>emit a backend contract that hides the absence of explicit visibility semantics,</li>
  <li>use runtime-private flush or propagation behavior to retroactively define missing language meaning.</li>
</ul>

<p>The forbidden laundering is:</p>

<pre><code>missing explicit state write visibility
        -/-&gt; inferred runtime flush order
        -/-&gt; treated as valid meaning
</code></pre>

<hr>

<h2 id="rationale">9. Rationale</h2>

<p>This distinction matters because runtime flush behavior is one of the easiest places for hidden implementation decisions to silently redefine a language.</p>

<p>If an implementation may replace missing explicit state visibility with one convenient observed flush order and still claim conformance, then:</p>
<ul>
  <li>program meaning becomes implementation-dependent,</li>
  <li>validation loses force,</li>
  <li>runtime behavior starts replacing published language law,</li>
  <li>independent implementations may diverge while all appearing to “work”.</li>
</ul>

<p>FROG must instead preserve the architectural rule:</p>

<pre><code>required semantic visibility facts
must be established
not guessed from runtime flush behavior
</code></pre>

<p>That rule is necessary for inspectability, portability, reproducibility, and future independent implementations.</p>

<hr>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>

<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is invalid for this purpose,</li>
  <li>language-level meaning is not established,</li>
  <li>an inferred runtime flush order does not count as explicit state write visibility,</li>
  <li>validation must fail rather than silently repairing the case,</li>
  <li>later layers must not launder the missing semantic fact into apparent correctness.</li>
</ul>

<p>The public truth asserted by this case is:</p>

<pre><code>inferred runtime flush order
must not be treated as
explicit state write visibility
</code></pre>
