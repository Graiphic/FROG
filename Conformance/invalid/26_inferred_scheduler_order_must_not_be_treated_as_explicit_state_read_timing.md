<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 26</h1>

<p align="center">
  <strong>Inferred scheduler order must not be treated as explicit state read timing</strong><br>
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

<p><strong>Case:</strong> <code>26_inferred_scheduler_order_must_not_be_treated_as_explicit_state_read_timing</code></p>

<hr>

<h2 id="expected">2. Expected</h2>

<p><strong>Expected:</strong> invalid</p>

<p><strong>Expected meaning:</strong> language-level meaning is not established if a program requires explicit state read timing semantics but only scheduler-order inference is available.</p>

<p><strong>Expected rejection:</strong> inferred scheduler order must not be promoted to explicit state read timing.</p>

<hr>

<h2 id="why">3. Why</h2>

<p>This case exists to make one rejection rule explicit:</p>

<pre><code>inferred scheduler order
            -/-&gt;
explicit state read timing
</code></pre>

<p>A conforming implementation may choose one internal scheduler, one execution engine, one queue topology, or one backend-oriented sequencing strategy.</p>

<p>Those choices do not satisfy a language requirement for explicit semantically established state read timing.</p>

<p>If the timing of a state read must be established by the validated program meaning, an implementation must reject the case rather than silently laundering one observed or convenient scheduler order into semantic truth.</p>

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
  <li>timing of a state read matters semantically,</li>
  <li>but the implementation attempts to substitute scheduler behavior for explicit program meaning.</li>
</ul>

<p>This is a source-to-meaning failure, not a runtime scheduling success.</p>

<hr>

<h2 id="scenario">5. Scenario</h2>

<p>A FROG program relies on one state read timing relationship in a context where the published rules require that timing to be established by the validated program meaning.</p>

<p>However, the program meaning does not actually establish that timing explicitly enough.</p>

<p>Instead, a tool attempts to continue by assuming one of the following:</p>
<ul>
  <li>the order in which one executor currently visits nodes,</li>
  <li>the order produced by one scheduler implementation,</li>
  <li>one backend-family sequencing habit,</li>
  <li>one runtime convenience ordering,</li>
  <li>some other non-semantic inferred execution order.</li>
</ul>

<p>The program is therefore not allowed to claim the same meaning as a program whose state read timing is explicitly established by the published language rules.</p>

<hr>

<h2 id="what-makes-the-case-invalid">6. What Makes the Case Invalid</h2>

<p>This case is invalid because it attempts to obtain valid state-read timing meaning without satisfying the source-side and language-side requirement that the timing be semantically established.</p>

<p>More specifically:</p>
<ul>
  <li>the required timing fact is not established by validated program meaning,</li>
  <li>the missing semantic fact is being replaced by scheduler inference,</li>
  <li>the implementation would therefore be inventing meaning rather than validating meaning,</li>
  <li>the resulting behavior would become implementation-defined instead of specification-defined.</li>
</ul>

<p>Accordingly, validation must fail.</p>

<hr>

<h2 id="expected-rejection">7. Expected Rejection</h2>

<p>A conforming implementation must reject this case as failing to establish valid language-level meaning.</p>

<p>The rejection should make clear that:</p>
<ul>
  <li>required explicit state read timing was not established,</li>
  <li>a merely inferred scheduler order is not an acceptable substitute,</li>
  <li>derivation must not proceed as though valid timing meaning already existed.</li>
</ul>

<p>The semantic reading is therefore:</p>

<pre><code>missing required explicit state read timing
        -&gt;
no valid established timing meaning
        -&gt;
validation failure
</code></pre>

<hr>

<h2 id="what-must-not-be-laundered">8. What Must Not Be Laundered</h2>

<p>A conforming implementation must <strong>not</strong> do any of the following:</p>

<ul>
  <li>treat one observed scheduler order as if the source had explicitly established state read timing,</li>
  <li>treat backend sequencing convenience as a semantic replacement for explicit timing meaning,</li>
  <li>derive execution IR as though valid state read timing meaning already existed,</li>
  <li>emit a backend contract that hides the absence of explicit timing semantics,</li>
  <li>use runtime-private scheduling behavior to retroactively define missing language meaning.</li>
</ul>

<p>The forbidden laundering is:</p>

<pre><code>missing explicit state read timing
        -/-&gt; inferred scheduler order
        -/-&gt; treated as valid meaning
</code></pre>

<hr>

<h2 id="rationale">9. Rationale</h2>

<p>This distinction matters because scheduler behavior is one of the easiest places for hidden implementation decisions to silently redefine a language.</p>

<p>If an implementation may replace missing explicit state timing with one convenient observed order and still claim conformance, then:</p>
<ul>
  <li>program meaning becomes implementation-dependent,</li>
  <li>validation loses force,</li>
  <li>runtime behavior starts replacing published language law,</li>
  <li>independent implementations may diverge while all appearing to “work”.</li>
</ul>

<p>FROG must instead preserve the architectural rule:</p>

<pre><code>required semantic timing facts
must be established
not guessed from execution order
</code></pre>

<p>That rule is necessary for inspectability, portability, reproducibility, and future independent implementations.</p>

<hr>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>

<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is invalid for this purpose,</li>
  <li>language-level meaning is not established,</li>
  <li>an inferred scheduler order does not count as explicit state read timing,</li>
  <li>validation must fail rather than silently repairing the case,</li>
  <li>later layers must not launder the missing semantic fact into apparent correctness.</li>
</ul>

<p>The public truth asserted by this case is:</p>

<pre><code>inferred scheduler order
must not be treated as
explicit state read timing
</code></pre>
