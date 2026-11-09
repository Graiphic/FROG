<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 31</h1>

<p align="center">
  <strong>Explicit state version boundary remains distinct from inferred runtime update epoch</strong><br>
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

<p><strong>Case:</strong> <code>31_explicit_state_version_boundary_remains_distinct_from_inferred_runtime_update_epoch</code></p>

<hr>

<h2 id="expected">2. Expected</h2>

<p><strong>Expected:</strong> valid</p>

<p><strong>Expected meaning:</strong> language-level meaning is established.</p>

<p><strong>Expected preservation:</strong> an explicit state version boundary remains semantically distinct from any epoch, tick, generation, or update phase that an implementation might infer from runtime scheduling or backend-private update bookkeeping.</p>

<hr>

<h2 id="why">3. Why</h2>

<p>This case exists to make one boundary explicit:</p>

<pre><code>explicit state version boundary
            !=
inferred runtime update epoch
</code></pre>

<p>A conforming implementation may maintain internal update counters, commit epochs, generation numbers, scheduler rounds, or backend-private phase markers.</p>

<p>Those implementation mechanisms do not become the source of semantic truth when the published program meaning already establishes an explicit boundary between state versions.</p>

<p>If the state version boundary is explicit in validated program meaning, it must not later be replaced by whatever epoching strategy happens to be convenient for one runtime.</p>

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
  <li>a state version boundary established by explicit language meaning, and</li>
  <li>a boundary merely inferred from one runtime update epoch or implementation-side generation mechanism.</li>
</ul>

<hr>

<h2 id="scenario">5. Scenario</h2>

<p>A FROG program contains legal explicit state participation and one legal boundary at which one state version is distinguished from another according to the validated program structure and state semantics.</p>

<p>The program is otherwise valid:</p>
<ul>
  <li>its graph is legal,</li>
  <li>its state usage is legal,</li>
  <li>its structure semantics are legal,</li>
  <li>its execution dependencies are explicit where required.</li>
</ul>

<p>The important property of this case is that the version boundary is semantically attributable to validated program meaning itself. It is not left to one implementation’s internal update epoch or generation bookkeeping.</p>

<hr>

<h2 id="what-makes-the-case-valid">6. What Makes the Case Valid</h2>

<p>This case is valid because it does not rely on hidden runtime phase-tracking behavior to establish meaning.</p>

<p>Instead:</p>
<ul>
  <li>state participation is explicit where required,</li>
  <li>the version boundary occurs within a valid semantic context,</li>
  <li>the relevant distinction between state versions is established by the validated program structure and state rules,</li>
  <li>the case does not require a conforming implementation to invent version semantics from runtime update convenience.</li>
</ul>

<p>Accordingly, validation succeeds and language-level meaning is established.</p>

<hr>

<h2 id="expected-meaning">7. Expected Meaning</h2>

<p>If this case validates, the established meaning includes at least the following:</p>
<ul>
  <li>there is legal explicit state participation,</li>
  <li>there is a legal version boundary between state observations or updates where the published rules make that distinction meaningful,</li>
  <li>the semantic significance of that version boundary is part of validated program meaning,</li>
  <li>that boundary is not semantically interchangeable with whatever epoch, round, or generation one runtime happens to use internally.</li>
</ul>

<p>The semantic reading is therefore:</p>

<pre><code>explicit state semantics
        +
explicit execution meaning
        -&gt;
validated state version boundary
</code></pre>

<p>Not:</p>

<pre><code>runtime happens to increment one internal epoch
        -&gt;
therefore that becomes language version truth
</code></pre>

<hr>

<h2 id="expected-preservation">8. Expected Preservation</h2>

<p>A conforming implementation must preserve the distinction that this case establishes.</p>

<p>At minimum, later layers must not silently collapse:</p>
<ul>
  <li>explicit semantically established state version boundary, into</li>
  <li>inferred runtime update epoch.</li>
</ul>

<p>This does not require one frozen runtime phasing architecture.</p>

<p>It does require semantic faithfulness.</p>

<p>In particular:</p>
<ul>
  <li>derivation must not erase version-relevant state distinctions that the validated meaning established,</li>
  <li>lowering must not reinterpret explicit version semantics as mere runtime bookkeeping convenience,</li>
  <li>backend contract generation must not silently replace semantic version boundaries with unspecified update epochs,</li>
  <li>runtime-private generation or epoch counters must not become the hidden owner of state version truth.</li>
</ul>

<hr>

<h2 id="what-must-not-happen">9. What Must Not Happen</h2>

<p>A conforming implementation must <strong>not</strong> do any of the following:</p>

<ul>
  <li>treat an explicit state version boundary as semantically equivalent to one observed runtime update epoch,</li>
  <li>discard version-relevant semantic distinctions during derivation and later substitute internal generation bookkeeping,</li>
  <li>claim semantic correctness merely because one runtime exposes a plausible epoch model,</li>
  <li>reinterpret source-established version relationships as backend-private update detail,</li>
  <li>use runtime epoch behavior to retroactively define language meaning.</li>
</ul>

<p>The forbidden collapse is:</p>

<pre><code>explicit state version boundary
        -/-> inferred runtime update epoch
</code></pre>

<hr>

<h2 id="rationale">10. Rationale</h2>

<p>This distinction matters because state versioning is one of the easiest places for hidden implementation behavior to silently redefine a language.</p>

<p>If inferred runtime epochs can replace explicit version semantics, then:</p>
<ul>
  <li>program meaning becomes implementation-dependent,</li>
  <li>validation loses authority,</li>
  <li>backend-private bookkeeping leaks upward into language truth,</li>
  <li>independent implementations may diverge while all appearing to track “the same state version”.</li>
</ul>

<p>FROG must keep the architectural rule explicit:</p>

<pre><code>validated semantic version boundaries
            remain
above runtime epoch convenience
</code></pre>

<p>This is necessary for inspectability, portability, reproducibility, and future independent implementations.</p>

<hr>

<h2 id="minimum-conformance-reading">11. Minimum Conformance Reading</h2>

<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>language-level meaning is established,</li>
  <li>the state version boundary is semantically established where the published rules require it,</li>
  <li>that boundary remains distinct from inferred runtime update epoch,</li>
  <li>later layers must preserve that distinction instead of silently laundering it away.</li>
</ul>

<p>The public truth asserted by this case is:</p>

<pre><code>explicit state version boundary
            remains distinct from
inferred runtime update epoch
</code></pre>
