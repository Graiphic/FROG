<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 24</h1>

<p align="center">
  <strong>Inferred default initial value must not be treated as explicit state initialization</strong><br>
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

<p><strong>Case:</strong> <code>24_inferred_default_initial_value_must_not_be_treated_as_explicit_state_initialization</code></p>

<hr>

<h2 id="expected">2. Expected</h2>

<p><strong>Expected:</strong> invalid</p>

<p><strong>Expected meaning:</strong> language-level meaning is not established if the program requires explicit state initialization but only an inferred default initial value is available.</p>

<p><strong>Expected rejection:</strong> inferred default initial value must not be promoted to explicit state initialization.</p>

<hr>

<h2 id="why">3. Why</h2>

<p>This case exists to make one rejection rule explicit:</p>

<pre><code>inferred default initial value
            -/-> 
explicit state initialization
</code></pre>

<p>A conforming implementation may have internal defaulting behavior, storage initialization behavior, target-family convenience rules, or zero-initialization habits.</p>

<p>Those conveniences do not satisfy a language requirement for explicit source-established state initialization.</p>

<p>If explicit initialization is required by the published rules, an implementation must reject the case rather than silently laundering an inferred default into semantic truth.</p>

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
  <li>explicit initialization is required for valid meaning,</li>
  <li>but the implementation attempts to substitute a guessed or conventional default initial value.</li>
</ul>

<p>This is a source-to-meaning failure, not a backend convenience success.</p>

<hr>

<h2 id="scenario">5. Scenario</h2>

<p>A FROG program relies on stateful behavior in a context where the published rules require explicit source-established initialization.</p>

<p>However, the source does not actually establish that initialization explicitly.</p>

<p>Instead, a tool attempts to continue by assuming one of the following:</p>
<ul>
  <li>a type-derived default value,</li>
  <li>a zero-like initial value,</li>
  <li>a target-family storage default,</li>
  <li>an implementation convenience fallback,</li>
  <li>some other non-source, non-semantic inferred initialization.</li>
</ul>

<p>The program is therefore not allowed to claim the same meaning as a program with explicit state initialization.</p>

<hr>

<h2 id="what-makes-the-case-invalid">6. What Makes the Case Invalid</h2>

<p>This case is invalid because it attempts to obtain valid state-initialization meaning without satisfying the explicit source-side requirement that the published rules impose.</p>

<p>More specifically:</p>
<ul>
  <li>the required initialization is not explicitly established by the source,</li>
  <li>the missing semantic fact is being replaced by inference,</li>
  <li>the implementation would therefore be inventing meaning rather than validating meaning,</li>
  <li>the resulting behavior would become implementation-defined instead of specification-defined.</li>
</ul>

<p>Accordingly, validation must fail.</p>

<hr>

<h2 id="expected-rejection">7. Expected Rejection</h2>

<p>A conforming implementation must reject this case as failing to establish valid language-level meaning.</p>

<p>The rejection should make clear that:</p>
<ul>
  <li>required explicit state initialization was not established,</li>
  <li>a merely inferred default initial value is not an acceptable substitute,</li>
  <li>derivation must not proceed as though valid initialization meaning already existed.</li>
</ul>

<p>The semantic reading is therefore:</p>

<pre><code>missing required explicit initialization
        -&gt;
no valid established state-initialization meaning
        -&gt;
validation failure
</code></pre>

<hr>

<h2 id="what-must-not-be-laundered">8. What Must Not Be Laundered</h2>

<p>A conforming implementation must <strong>not</strong> do any of the following:</p>

<ul>
  <li>treat a type-derived default as if the source had explicitly declared initialization,</li>
  <li>treat zero-initialization convenience as a semantic replacement for explicit initialization,</li>
  <li>derive execution IR as though valid initialization meaning already existed,</li>
  <li>emit a backend contract that hides the absence of explicit initialization,</li>
  <li>use runtime-private startup behavior to retroactively define missing language meaning.</li>
</ul>

<p>The forbidden laundering is:</p>

<pre><code>missing explicit initialization
        -/-> inferred default
        -/-> treated as valid meaning
</code></pre>

<hr>

<h2 id="rationale">9. Rationale</h2>

<p>This distinction matters because state initialization is a common place where hidden implementation behavior can silently redefine a language.</p>

<p>If an implementation may replace missing explicit initialization with a guessed default and still claim conformance, then:</p>
<ul>
  <li>source meaning becomes unstable,</li>
  <li>validation loses force,</li>
  <li>different implementations may accept different behaviors while all claiming correctness,</li>
  <li>backend convenience starts replacing published language law.</li>
</ul>

<p>FROG must instead preserve the architectural rule:</p>

<pre><code>required semantic facts
must be established
not guessed
</code></pre>

<p>That rule is necessary for inspectability, portability, predictable conformance, and future independent implementations.</p>

<hr>

<h2 id="minimum-conformance-reading">10. Minimum Conformance Reading</h2>

<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is invalid for this purpose,</li>
  <li>language-level meaning is not established,</li>
  <li>an inferred default initial value does not count as explicit state initialization,</li>
  <li>validation must fail rather than silently repairing the case,</li>
  <li>later layers must not launder the missing semantic fact into apparent correctness.</li>
</ul>

<p>The public truth asserted by this case is:</p>

<pre><code>inferred default initial value
must not be treated as
explicit state initialization
</code></pre>
