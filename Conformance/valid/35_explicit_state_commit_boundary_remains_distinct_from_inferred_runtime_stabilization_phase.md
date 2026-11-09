<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 35</h1>

<p align="center">
  <strong>Explicit state commit boundary remains distinct from inferred runtime stabilization phase</strong><br/>
  FROG — Free Open Graphical Language
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#1-case-name">1. Case Name</a></li>
  <li><a href="#2-expected">2. Expected</a></li>
  <li><a href="#3-why">3. Why</a></li>
  <li><a href="#4-boundary-being-exercised">4. Boundary Being Exercised</a></li>
  <li><a href="#5-scenario">5. Scenario</a></li>
  <li><a href="#6-what-makes-the-case-valid">6. What Makes the Case Valid</a></li>
  <li><a href="#7-expected-meaning">7. Expected Meaning</a></li>
  <li><a href="#8-expected-preservation">8. Expected Preservation</a></li>
  <li><a href="#9-what-must-not-happen">9. What Must Not Happen</a></li>
  <li><a href="#10-rationale">10. Rationale</a></li>
  <li><a href="#11-minimum-conformance-reading">11. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="1-case-name">1. Case Name</h2>

<p>
  Case:
  <code>35_explicit_state_commit_boundary_remains_distinct_from_inferred_runtime_stabilization_phase</code>
</p>

<hr/>

<h2 id="2-expected">2. Expected</h2>

<p><strong>Expected:</strong> valid</p>

<p><strong>Expected meaning:</strong> language-level meaning is established.</p>

<p>
  <strong>Expected preservation:</strong>
  an explicit state commit boundary remains semantically distinct from any stabilization phase,
  settling phase, quiescence pass, convergence wait, or runtime-private “now the state is stable”
  interpretation that an implementation might infer from its own execution machinery.
</p>

<hr/>

<h2 id="3-why">3. Why</h2>

<p>This case exists to make one boundary explicit:</p>

<pre>explicit state commit boundary
            !=
inferred runtime stabilization phase</pre>

<p>
  A conforming implementation may internally use scheduler barriers, settling phases, deferred flushes,
  buffering windows, queue drains, synchronization passes, or backend-private quiescence logic before it
  considers its internal state machinery “stable.”
</p>

<p>
  Those mechanisms do not become the source of semantic truth when the published program meaning already
  establishes an explicit state commit boundary.
</p>

<p>
  If a commit boundary is explicit in validated program meaning, it must not later be replaced by whatever
  stabilization phase happens to be convenient for one runtime, scheduler, executor, or target backend.
</p>

<hr/>

<h2 id="4-boundary-being-exercised">4. Boundary Being Exercised</h2>

<p>This case exercises the normative boundary:</p>

<pre>canonical .frog source
        |
        v
validated program meaning</pre>

<p>It also exercises the preservation obligation across later layers:</p>

<pre>validated program meaning
        |
        v
derived execution-facing representation
        |
        v
lowering / backend / runtime realization</pre>

<p>The distinction under test is between:</p>

<ul>
  <li>a state commit boundary established by explicit language meaning, and</li>
  <li>a boundary merely inferred from one runtime stabilization or quiescence phase.</li>
</ul>

<hr/>

<h2 id="5-scenario">5. Scenario</h2>

<p>
  A FROG program contains legal explicit state participation and one legal boundary at which state updates
  become semantically committed according to the validated program structure and state semantics.
</p>

<p>The program is otherwise valid:</p>

<ul>
  <li>its graph is legal,</li>
  <li>its state usage is legal,</li>
  <li>its structure semantics are legal,</li>
  <li>its execution dependencies are explicit where required.</li>
</ul>

<p>
  The important property of this case is that the commit boundary is semantically attributable to validated
  program meaning itself. It is not left to one implementation’s notion of when its internal execution has
  stabilized or settled enough to expose a result.
</p>

<hr/>

<h2 id="6-what-makes-the-case-valid">6. What Makes the Case Valid</h2>

<p>
  This case is valid because it does not rely on hidden runtime stabilization behavior to establish meaning.
</p>

<p>Instead:</p>

<ul>
  <li>state participation is explicit where required,</li>
  <li>the commit boundary occurs within a valid semantic context,</li>
  <li>the relevant distinction between pre-commit and post-commit state meaning is established by the validated program structure and state rules,</li>
  <li>the case does not require a conforming implementation to invent commit semantics from runtime settling convenience.</li>
</ul>

<p>
  Accordingly, validation succeeds and language-level meaning is established.
</p>

<hr/>

<h2 id="7-expected-meaning">7. Expected Meaning</h2>

<p>If this case validates, the established meaning includes at least the following:</p>

<ul>
  <li>there is legal explicit state participation,</li>
  <li>there is a legal semantic commit boundary where the published rules make that distinction meaningful,</li>
  <li>the semantic significance of that commit boundary is part of validated program meaning,</li>
  <li>that boundary is not semantically interchangeable with whatever stabilization phase one runtime happens to execute internally.</li>
</ul>

<p>The semantic reading is therefore:</p>

<pre>explicit state semantics
        +
explicit execution meaning
        ->
validated state commit boundary</pre>

<p>Not:</p>

<pre>runtime eventually reaches internal stabilization
        ->
therefore that becomes language commit truth</pre>

<hr/>

<h2 id="8-expected-preservation">8. Expected Preservation</h2>

<p>
  A conforming implementation must preserve the distinction that this case establishes.
</p>

<p>At minimum, later layers must not silently collapse:</p>

<ul>
  <li>explicit semantically established state commit boundary, into</li>
  <li>inferred runtime stabilization phase.</li>
</ul>

<p>
  This does not require one frozen runtime stabilization architecture.
</p>

<p>
  It does require semantic faithfulness.
</p>

<p>In particular:</p>

<ul>
  <li>derivation must not erase commit-relevant state distinctions that the validated meaning established,</li>
  <li>lowering must not reinterpret explicit commit semantics as mere runtime settling convenience,</li>
  <li>backend contract generation must not silently replace semantic commit boundaries with unspecified stabilization behavior,</li>
  <li>runtime-private settling, convergence, quiescence, or synchronization phases must not become the hidden owner of state commit truth.</li>
</ul>

<hr/>

<h2 id="9-what-must-not-happen">9. What Must Not Happen</h2>

<p>A conforming implementation must not do any of the following:</p>

<ul>
  <li>treat an explicit state commit boundary as semantically equivalent to one observed runtime stabilization phase,</li>
  <li>discard commit-relevant semantic distinctions during derivation and later substitute internal settling logic,</li>
  <li>claim semantic correctness merely because one runtime exposes a plausible stable state after quiescence,</li>
  <li>reinterpret source-established commit relationships as backend-private synchronization detail,</li>
  <li>use runtime stabilization behavior to retroactively define language meaning.</li>
</ul>

<p>The forbidden collapse is:</p>

<pre>explicit state commit boundary
        -/-> inferred runtime stabilization phase</pre>

<hr/>

<h2 id="10-rationale">10. Rationale</h2>

<p>
  This distinction matters because commit is one of the easiest places for hidden implementation behavior to
  silently redefine a language.
</p>

<p>
  A runtime may need time to settle. It may wait for queues to drain, workers to synchronize, buffers to flush,
  or target-private coordination to complete.
</p>

<p>
  But none of that is automatically the same thing as a language-level commit boundary.
</p>

<p>If inferred stabilization can replace explicit commit semantics, then:</p>

<ul>
  <li>program meaning becomes implementation-dependent,</li>
  <li>validation loses authority,</li>
  <li>backend-private coordination leaks upward into language truth,</li>
  <li>independent implementations may diverge while all appearing to expose a “stable committed state.”</li>
</ul>

<p>FROG must keep the architectural rule explicit:</p>

<pre>validated semantic commit boundaries
            remain
above runtime stabilization convenience</pre>

<p>
  This is necessary for inspectability, portability, reproducibility, and future independent implementations.
</p>

<hr/>

<h2 id="11-minimum-conformance-reading">11. Minimum Conformance Reading</h2>

<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>language-level meaning is established,</li>
  <li>the state commit boundary is semantically established where the published rules require it,</li>
  <li>that boundary remains distinct from inferred runtime stabilization phase,</li>
  <li>later layers must preserve that distinction instead of silently laundering it away.</li>
</ul>

<p>The public truth asserted by this case is:</p>

<pre>explicit state commit boundary
            remains distinct from
inferred runtime stabilization phase</pre>
