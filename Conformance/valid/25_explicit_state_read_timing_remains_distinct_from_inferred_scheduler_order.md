<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 25</h1>

<p align="center">
  <strong>Explicit state read timing remains distinct from inferred scheduler order</strong><br>
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
  <li><a href="#what-makes-the-case-valid">6. What Makes the Case Valid</a></li>
  <li><a href="#expected-meaning">7. Expected Meaning</a></li>
  <li><a href="#expected-preservation">8. Expected Preservation</a></li>
  <li><a href="#what-must-not-happen">9. What Must Not Happen</a></li>
  <li><a href="#rationale">10. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">11. Minimum Conformance Reading</a></li>
</ul>

<hr>

<h2 id="case-name">1. Case Name</h2>

<p><strong>Case:</strong> <code>25_explicit_state_read_timing_remains_distinct_from_inferred_scheduler_order</code></p>

<hr>

<h2 id="expected">2. Expected</h2>

<p><strong>Expected:</strong> valid</p>

<p><strong>Expected meaning:</strong> language-level meaning is established.</p>

<p><strong>Expected preservation:</strong> explicit state read timing remains semantically distinct from any timing that an implementation might infer from scheduler order, execution convenience, or backend-specific sequencing behavior.</p>

<hr>

<h2 id="why">3. Why</h2>

<p>This case exists to make one boundary explicit:</p>

<pre><code>explicit state read timing
            !=
inferred scheduler order
</code></pre>

<p>A conforming implementation may choose one internal scheduler, one queueing model, one executor topology, or one backend-specific execution strategy.</p>

<p>Those implementation choices do not become the source of semantic timing truth when the published program meaning already establishes an explicit state read timing boundary.</p>

<p>If state timing is explicit in the validated program meaning, it must not later be replaced by whatever order happens to emerge from one scheduler.</p>

<hr>

<h2 id="boundary-being-exercised">4. Boundary Being Exercised</h2>

<p>This case exercises the normative boundary:</p>

<pre><code>canonical .frog source
        |
        v
validated program meaning
</code></pre>

<p>It also exercises the preservation obligation across later layers:</p>

<pre><code>validated program meaning
        |
        v
derived execution-facing representation
        |
        v
lowering / backend / runtime realization
</code></pre>

<p>The distinction under test is between:</p>
<ul>
  <li>state read timing established by explicit language meaning, and</li>
  <li>execution order merely inferred from one scheduler or implementation strategy.</li>
</ul>

<hr>

<h2 id="scenario">5. Scenario</h2>

<p>A FROG program contains legal explicit state participation and a legal read of that state whose timing is determined by the validated program structure and state semantics.</p>

<p>The program is otherwise valid:</p>
<ul>
  <li>its graph is legal,</li>
  <li>its state usage is legal,</li>
  <li>its structure semantics are legal,</li>
  <li>its execution dependencies are explicit where required.</li>
</ul>

<p>The important property of this case is that the timing relationship of the state read is semantically attributable to the program meaning itself. It is not left to one implementation’s opportunistic execution order.</p>

<hr>

<h2 id="what-makes-the-case-valid">6. What Makes the Case Valid</h2>

<p>This case is valid because it does not rely on hidden implementation scheduling to establish meaning.</p>

<p>Instead:</p>
<ul>
  <li>state participation is explicit where required,</li>
  <li>the read occurs within a valid semantic context,</li>
  <li>the relevant timing relationship is established by the validated program structure and state rules,</li>
  <li>the case does not require a conforming implementation to invent timing semantics by backend accident.</li>
</ul>

<p>Accordingly, validation succeeds and language-level meaning is established.</p>

<hr>

<h2 id="expected-meaning">7. Expected Meaning</h2>

<p>If this case validates, the established meaning includes at least the following:</p>
<ul>
  <li>there is legal explicit state participation,</li>
  <li>there is a legal state read,</li>
  <li>the timing significance of that read is part of validated program meaning where the published rules make it so,</li>
  <li>the read timing is not semantically interchangeable with whatever one scheduler happens to do.</li>
</ul>

<p>The semantic reading is therefore:</p>

<pre><code>explicit state semantics
        +
explicit execution meaning
        -&gt;
validated state read timing
</code></pre>

<p>Not:</p>

<pre><code>implementation scheduler happens to run something first
        -&gt;
therefore that becomes language timing truth
</code></pre>

<hr>

<h2 id="expected-preservation">8. Expected Preservation</h2>

<p>A conforming implementation must preserve the distinction that this case establishes.</p>

<p>At minimum, later layers must not silently collapse:</p>
<ul>
  <li>explicit semantically established state read timing, into</li>
  <li>inferred scheduler order.</li>
</ul>

<p>This does not require one frozen runtime architecture.</p>

<p>It does require semantic faithfulness.</p>

<p>In particular:</p>
<ul>
  <li>derivation must not erase timing-relevant state distinctions that the validated meaning established,</li>
  <li>lowering must not reinterpret explicit timing semantics as mere execution convenience,</li>
  <li>backend contract generation must not silently replace semantic timing with unspecified scheduling behavior,</li>
  <li>runtime-private execution order must not become the hidden owner of state timing truth.</li>
</ul>

<hr>

<h2 id="what-must-not-happen">9. What Must Not Happen</h2>

<p>A conforming implementation must <strong>not</strong> do any of the following:</p>

<ul>
  <li>treat explicit state read timing as semantically equivalent to one observed scheduler order,</li>
  <li>discard timing-relevant semantic distinctions during derivation and later substitute runtime order,</li>
  <li>claim semantic correctness merely because one executor produces a plausible ordering,</li>
  <li>reinterpret source-established timing relationships as backend-private implementation detail,</li>
  <li>use scheduler behavior to retroactively define language meaning.</li>
</ul>

<p>The forbidden collapse is:</p>

<pre><code>explicit state read timing
        -/-> inferred scheduler order
</code></pre>

<hr>

<h2 id="rationale">10. Rationale</h2>

<p>This distinction matters because stateful execution is one of the easiest places for hidden runtime behavior to silently redefine a language.</p>

<p>If inferred scheduler order can replace explicit state timing semantics, then:</p>
<ul>
  <li>program meaning becomes implementation-dependent,</li>
  <li>validation loses authority,</li>
  <li>backend choices leak upward into language truth,</li>
  <li>independent implementations may diverge while all appearing to “work”.</li>
</ul>

<p>FROG must keep the architectural rule explicit:</p>

<pre><code>validated semantic timing
            remains
above scheduler convenience
</code></pre>

<p>This is necessary for inspectability, portability, reproducibility, and future independent implementations.</p>

<hr>

<h2 id="minimum-conformance-reading">11. Minimum Conformance Reading</h2>

<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>language-level meaning is established,</li>
  <li>the state read timing is semantically established where the published rules require it,</li>
  <li>that timing remains distinct from inferred scheduler order,</li>
  <li>later layers must preserve that distinction instead of silently laundering it away.</li>
</ul>

<p>The public truth asserted by this case is:</p>

<pre><code>explicit state read timing
            remains distinct from
inferred scheduler order
</code></pre>
