<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 27</h1>

<p align="center">
  <strong>Explicit state write visibility remains distinct from inferred runtime flush order</strong><br>
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

<p><strong>Case:</strong> <code>27_explicit_state_write_visibility_remains_distinct_from_inferred_runtime_flush_order</code></p>

<hr>

<h2 id="expected">2. Expected</h2>

<p><strong>Expected:</strong> valid</p>

<p><strong>Expected meaning:</strong> language-level meaning is established.</p>

<p><strong>Expected preservation:</strong> explicit state write visibility remains semantically distinct from any visibility that an implementation might infer from runtime flush order, buffering behavior, commit timing, or backend-specific write propagation strategy.</p>

<hr>

<h2 id="why">3. Why</h2>

<p>This case exists to make one boundary explicit:</p>

<pre><code>explicit state write visibility
            !=
inferred runtime flush order
</code></pre>

<p>A conforming implementation may choose one buffering strategy, one commit discipline, one memory update mechanism, or one runtime propagation policy.</p>

<p>Those implementation choices do not become the source of semantic visibility truth when the published program meaning already establishes an explicit state write visibility boundary.</p>

<p>If visibility of a state write is explicit in the validated program meaning, it must not later be replaced by whatever timing happens to emerge from one runtime flush strategy.</p>

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
  <li>state write visibility established by explicit language meaning, and</li>
  <li>visibility merely inferred from one runtime flush order or implementation-side propagation behavior.</li>
</ul>

<hr>

<h2 id="scenario">5. Scenario</h2>

<p>A FROG program contains legal explicit state participation and a legal state write whose visibility relationship is determined by the validated program structure and state semantics.</p>

<p>The program is otherwise valid:</p>
<ul>
  <li>its graph is legal,</li>
  <li>its state usage is legal,</li>
  <li>its structure semantics are legal,</li>
  <li>its execution dependencies are explicit where required.</li>
</ul>

<p>The important property of this case is that the visibility relationship of the write is semantically attributable to the program meaning itself. It is not left to one implementation’s buffering or flush behavior.</p>

<hr>

<h2 id="what-makes-the-case-valid">6. What Makes the Case Valid</h2>

<p>This case is valid because it does not rely on hidden runtime propagation behavior to establish meaning.</p>

<p>Instead:</p>
<ul>
  <li>state participation is explicit where required,</li>
  <li>the write occurs within a valid semantic context,</li>
  <li>the relevant visibility relationship is established by the validated program structure and state rules,</li>
  <li>the case does not require a conforming implementation to invent visibility semantics by runtime accident.</li>
</ul>

<p>Accordingly, validation succeeds and language-level meaning is established.</p>

<hr>

<h2 id="expected-meaning">7. Expected Meaning</h2>

<p>If this case validates, the established meaning includes at least the following:</p>
<ul>
  <li>there is legal explicit state participation,</li>
  <li>there is a legal state write,</li>
  <li>the visibility significance of that write is part of validated program meaning where the published rules make it so,</li>
  <li>that write visibility is not semantically interchangeable with whatever one runtime happens to flush first.</li>
</ul>

<p>The semantic reading is therefore:</p>

<pre><code>explicit state semantics
        +
explicit execution meaning
        -&gt;
validated state write visibility
</code></pre>

<p>Not:</p>

<pre><code>runtime happens to flush or expose one write first
        -&gt;
therefore that becomes language visibility truth
</code></pre>

<hr>

<h2 id="expected-preservation">8. Expected Preservation</h2>

<p>A conforming implementation must preserve the distinction that this case establishes.</p>

<p>At minimum, later layers must not silently collapse:</p>
<ul>
  <li>explicit semantically established state write visibility, into</li>
  <li>inferred runtime flush order.</li>
</ul>

<p>This does not require one frozen runtime architecture.</p>

<p>It does require semantic faithfulness.</p>

<p>In particular:</p>
<ul>
  <li>derivation must not erase visibility-relevant state distinctions that the validated meaning established,</li>
  <li>lowering must not reinterpret explicit visibility semantics as mere runtime convenience,</li>
  <li>backend contract generation must not silently replace semantic visibility with unspecified propagation behavior,</li>
  <li>runtime-private flush or commit order must not become the hidden owner of state visibility truth.</li>
</ul>

<hr>

<h2 id="what-must-not-happen">9. What Must Not Happen</h2>

<p>A conforming implementation must <strong>not</strong> do any of the following:</p>

<ul>
  <li>treat explicit state write visibility as semantically equivalent to one observed flush order,</li>
  <li>discard visibility-relevant semantic distinctions during derivation and later substitute runtime commit order,</li>
  <li>claim semantic correctness merely because one runtime produces a plausible propagation order,</li>
  <li>reinterpret source-established visibility relationships as backend-private implementation detail,</li>
  <li>use runtime flush behavior to retroactively define language meaning.</li>
</ul>

<p>The forbidden collapse is:</p>

<pre><code>explicit state write visibility
        -/-> inferred runtime flush order
</code></pre>

<hr>

<h2 id="rationale">10. Rationale</h2>

<p>This distinction matters because stateful execution is one of the easiest places for hidden runtime behavior to silently redefine a language.</p>

<p>If inferred runtime flush order can replace explicit state visibility semantics, then:</p>
<ul>
  <li>program meaning becomes implementation-dependent,</li>
  <li>validation loses authority,</li>
  <li>backend and runtime choices leak upward into language truth,</li>
  <li>independent implementations may diverge while all appearing to “work”.</li>
</ul>

<p>FROG must keep the architectural rule explicit:</p>

<pre><code>validated semantic visibility
            remains
above runtime flush convenience
</code></pre>

<p>This is necessary for inspectability, portability, reproducibility, and future independent implementations.</p>

<hr>

<h2 id="minimum-conformance-reading">11. Minimum Conformance Reading</h2>

<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>language-level meaning is established,</li>
  <li>the state write visibility is semantically established where the published rules require it,</li>
  <li>that visibility remains distinct from inferred runtime flush order,</li>
  <li>later layers must preserve that distinction instead of silently laundering it away.</li>
</ul>

<p>The public truth asserted by this case is:</p>

<pre><code>explicit state write visibility
            remains distinct from
inferred runtime flush order
</code></pre>
