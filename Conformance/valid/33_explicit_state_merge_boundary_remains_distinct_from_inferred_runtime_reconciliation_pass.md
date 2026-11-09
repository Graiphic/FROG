<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 33</h1>

<p align="center">
  <strong>Explicit state merge boundary remains distinct from inferred runtime reconciliation pass</strong><br>
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

<p><strong>Case:</strong> <code>33_explicit_state_merge_boundary_remains_distinct_from_inferred_runtime_reconciliation_pass</code></p>

<hr>

<h2 id="expected">2. Expected</h2>

<p><strong>Expected:</strong> valid</p>

<p><strong>Expected meaning:</strong> language-level meaning is established.</p>

<p><strong>Expected preservation:</strong> an explicit state merge boundary remains semantically distinct from any merge, reconciliation, coalescing, or convergence pass that an implementation might infer from runtime coordination or backend-private state management.</p>

<hr>

<h2 id="why">3. Why</h2>

<p>This case exists to make one boundary explicit:</p>

<pre><code>explicit state merge boundary
            !=
inferred runtime reconciliation pass
</code></pre>

<p>A conforming implementation may use internal reconciliation logic, scheduler-side coalescing, update consolidation, conflict resolution, or backend-private merge passes.</p>

<p>Those mechanisms do not become the source of semantic truth when the published program meaning already establishes an explicit state merge boundary.</p>

<p>If the merge boundary is explicit in validated program meaning, it must not later be replaced by whatever reconciliation strategy happens to be convenient for one runtime.</p>

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
  <li>a state merge boundary established by explicit language meaning, and</li>
  <li>a boundary merely inferred from one runtime reconciliation or implementation-side convergence mechanism.</li>
</ul>

<hr>

<h2 id="scenario">5. Scenario</h2>

<p>A FROG program contains legal explicit state participation and one legal boundary at which state contributions, branches, or stateful paths are semantically merged according to the validated program structure and state semantics.</p>

<p>The program is otherwise valid:</p>
<ul>
  <li>its graph is legal,</li>
  <li>its state usage is legal,</li>
  <li>its structure semantics are legal,</li>
  <li>its execution dependencies are explicit where required.</li>
</ul>

<p>The important property of this case is that the merge boundary is semantically attributable to validated program meaning itself. It is not left to one implementation’s reconciliation or coalescing convenience.</p>

<hr>

<h2 id="what-makes-the-case-valid">6. What Makes the Case Valid</h2>

<p>This case is valid because it does not rely on hidden runtime reconciliation behavior to establish meaning.</p>

<p>Instead:</p>
<ul>
  <li>state participation is explicit where required,</li>
  <li>the merge boundary occurs within a valid semantic context,</li>
  <li>the relevant distinction between pre-merge and post-merge state meaning is established by the validated program structure and state rules,</li>
  <li>the case does not require a conforming implementation to invent merge semantics from runtime coordination convenience.</li>
</ul>

<p>Accordingly, validation succeeds and language-level meaning is established.</p>

<hr>

<h2 id="expected-meaning">7. Expected Meaning</h2>

<p>If this case validates, the established meaning includes at least the following:</p>
<ul>
  <li>there is legal explicit state participation,</li>
  <li>there is a legal semantic merge boundary where the published rules make that distinction meaningful,</li>
  <li>the semantic significance of that merge boundary is part of validated program meaning,</li>
  <li>that boundary is not semantically interchangeable with whatever reconciliation pass one runtime happens to execute internally.</li>
</ul>

<p>The semantic reading is therefore:</p>

<pre><code>explicit state semantics
        +
explicit execution meaning
        -&gt;
validated state merge boundary
</code></pre>

<p>Not:</p>

<pre><code>runtime happens to reconcile several internal states
        -&gt;
therefore that becomes language merge truth
</code></pre>

<hr>

<h2 id="expected-preservation">8. Expected Preservation</h2>

<p>A conforming implementation must preserve the distinction that this case establishes.</p>

<p>At minimum, later layers must not silently collapse:</p>
<ul>
  <li>explicit semantically established state merge boundary, into</li>
  <li>inferred runtime reconciliation pass.</li>
</ul>

<p>This does not require one frozen runtime reconciliation architecture.</p>

<p>It does require semantic faithfulness.</p>

<p>In particular:</p>
<ul>
  <li>derivation must not erase merge-relevant state distinctions that the validated meaning established,</li>
  <li>lowering must not reinterpret explicit merge semantics as mere runtime coordination convenience,</li>
  <li>backend contract generation must not silently replace semantic merge boundaries with unspecified reconciliation behavior,</li>
  <li>runtime-private merge, coalescing, or convergence passes must not become the hidden owner of state merge truth.</li>
</ul>

<hr>

<h2 id="what-must-not-happen">9. What Must Not Happen</h2>

<p>A conforming implementation must <strong>not</strong> do any of the following:</p>

<ul>
  <li>treat an explicit state merge boundary as semantically equivalent to one observed reconciliation pass,</li>
  <li>discard merge-relevant semantic distinctions during derivation and later substitute internal convergence logic,</li>
  <li>claim semantic correctness merely because one runtime exposes a plausible merged result,</li>
  <li>reinterpret source-established merge relationships as backend-private state-management detail,</li>
  <li>use runtime reconciliation behavior to retroactively define language meaning.</li>
</ul>

<p>The forbidden collapse is:</p>

<pre><code>explicit state merge boundary
        -/-> inferred runtime reconciliation pass
</code></pre>

<hr>

<h2 id="rationale">10. Rationale</h2>

<p>This distinction matters because state merging is one of the easiest places for hidden implementation behavior to silently redefine a language.</p>

<p>If inferred runtime reconciliation can replace explicit merge semantics, then:</p>
<ul>
  <li>program meaning becomes implementation-dependent,</li>
  <li>validation loses authority,</li>
  <li>backend-private coordination leaks upward into language truth,</li>
  <li>independent implementations may diverge while all appearing to produce “the same merged state”.</li>
</ul>

<p>FROG must keep the architectural rule explicit:</p>

<pre><code>validated semantic merge boundaries
            remain
above runtime reconciliation convenience
</code></pre>

<p>This is necessary for inspectability, portability, reproducibility, and future independent implementations.</p>

<hr>

<h2 id="minimum-conformance-reading">11. Minimum Conformance Reading</h2>

<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>language-level meaning is established,</li>
  <li>the state merge boundary is semantically established where the published rules require it,</li>
  <li>that boundary remains distinct from inferred runtime reconciliation pass,</li>
  <li>later layers must preserve that distinction instead of silently laundering it away.</li>
</ul>

<p>The public truth asserted by this case is:</p>

<pre><code>explicit state merge boundary
            remains distinct from
inferred runtime reconciliation pass
</code></pre>
