<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 34</h1>

<p align="center">
  <strong>Inferred runtime reconciliation pass must not be treated as explicit state merge boundary</strong><br>
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

<p><strong>Case:</strong> <code>34_inferred_runtime_reconciliation_pass_must_not_be_treated_as_explicit_state_merge_boundary</code></p>

<hr>

<h2 id="expected">2. Expected</h2>

<p><strong>Expected:</strong> invalid</p>

<p><strong>Expected meaning:</strong> language-level meaning is not established if a program requires an explicit state merge boundary but only a runtime-inferred reconciliation pass is available.</p>

<p><strong>Expected rejection:</strong> inferred runtime reconciliation pass must not be promoted to explicit state merge boundary.</p>

<hr>

<h2 id="why">3. Why</h2>

<p>This case exists to make one rejection rule explicit:</p>

<pre><code>inferred runtime reconciliation pass
            -/-&gt;
explicit state merge boundary
</code></pre>

<p>A conforming implementation may maintain internal reconciliation logic, scheduler-side coalescing, conflict resolution, convergence passes, or backend-private state merge phases.</p>

<p>Those implementation mechanisms do not satisfy a language requirement for an explicit semantically established state merge boundary.</p>

<p>If a state merge boundary must be established by validated program meaning, an implementation must reject the case rather than silently laundering one observed runtime reconciliation pass into semantic truth.</p>

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
  <li>a state merge boundary matters semantically,</li>
  <li>but the implementation attempts to substitute runtime reconciliation behavior for explicit program meaning.</li>
</ul>

<p>This is a source-to-meaning failure, not a runtime coordination success.</p>

<hr>

<h2 id="scenario">5. Scenario</h2>

<p>A FROG program relies on one state merge boundary in a context where the published rules require that boundary to be established by validated program meaning.</p>

<p>However, the program meaning does not actually establish that boundary explicitly enough.</p>

<p>Instead, a tool attempts to continue by assuming one of the following:</p>
<ul>
  <li>the point at which one runtime happens to reconcile several internal state contributions,</li>
  <li>the pass at which one backend coalesces state updates,</li>
  <li>one scheduler-side convergence phase,</li>
  <li>one runtime-private conflict-resolution mechanism,</li>
  <li>some other non-semantic inferred reconciliation pass.</li>
</ul>

<p>The program is therefore not allowed to claim the same meaning as a program whose state merge boundary is explicitly established by the published language rules.</p>

<hr>

<h2 id="what-makes-the-case-invalid">6. What Makes the Case Invalid</h2>

<p>This case is invalid because it attempts to obtain valid state-merge meaning without satisfying the source-side and language-side requirement that the merge boundary be semantically established.</p>

<p>More specifically:</p>
<ul>
  <li>the required merge fact is not established by validated program meaning,</li>
  <li>the missing semantic fact is being replaced by runtime reconciliation inference,</li>
  <li>the implementation would therefore be inventing meaning rather than validating meaning,</li>
  <li>the resulting behavior would become implementation-defined instead of specification-defined.</li>
</ul>

<p>Accordingly, validation must fail.</p>

<hr>

<h2 id="expected-rejection">7. Expected Rejection</h2>

<p>A conforming implementation must reject this case as failing to establish valid language-level meaning.</p>

<p>The rejection should make clear that:</p>
<ul>
  <li>required explicit state merge boundary was not established,</li>
  <li>a merely inferred runtime reconciliation pass is not an acceptable substitute,</li>
  <li>derivation must not proceed as though valid merge meaning already existed.</li>
</ul>

<p>The semantic reading is therefore:</p>

<pre><code>missing required explicit state merge boundary
        -&gt;
no valid established merge meaning
        -&gt;
validation failure
</code></pre>

<hr>

<h2 id="what-must-not-be-laundered">8. What Must Not Be Laundered</h2>

<p>A conforming implementation must <strong>not</strong> do any of the following:</p>

<ul>
  <li>treat one observed runtime reconciliation pass as if the source had explicitly established a state merge boundary,</li>
  <li>treat backend-private coalescing logic as a semantic replacement for explicit merge meaning,</li>
  <li>derive execution IR as though valid state merge meaning already existed,</li>
  <li>emit a backend contract that hides the absence of explicit merge semantics,</li>
  <li>use runtime-private reconciliation behavior to retroactively define missing language meaning.</li>
</ul>

<p>The forbidden laundering is:</p>

<pre><code>missing explicit state merge boundary
        -/-&gt; inferred runtime reconciliation pass
        -/-&gt; treated as valid meaning
</code></pre>

<hr>

<h2 id="rationale">9. Rationale</h2>

<p>This distinction matters because runtime reconciliation behavior is one of the easiest places for hidden implementation decisions to silently redefine a language.</p>

<p>If an implementation may replace missing explicit merge boundaries with one convenient observed reconciliation pass and still claim conformance, then:</p>
<ul>
  <li>program meaning becomes implementation-dependent,</li>
  <li>validation loses force,</li>
  <li>runtime coordination starts replacing published language law,</li>
  <li>independent implementations may diverge while all appearing to produce “the same merged state”.</li>
</ul>

<p>FROG must instead preserve the architectural rule:</p>

<pre><code>required semantic merge facts
must be established
not guessed from runtime reconciliation behavior
</code></pre>

<p>That rule is necessary for inspectability, portability, reproducibility, and future independent implementations.</p>

<hr>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>

<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is invalid for this purpose,</li>
  <li>language-level meaning is not established,</li>
  <li>an inferred runtime reconciliation pass does not count as an explicit state merge boundary,</li>
  <li>validation must fail rather than silently repairing the case,</li>
  <li>later layers must not launder the missing semantic fact into apparent correctness.</li>
</ul>

<p>The public truth asserted by this case is:</p>

<pre><code>inferred runtime reconciliation pass
must not be treated as
explicit state merge boundary
</code></pre>
