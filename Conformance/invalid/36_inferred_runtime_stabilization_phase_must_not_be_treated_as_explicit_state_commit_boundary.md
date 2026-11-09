<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 36</h1>

<p align="center">
  <strong>Inferred runtime stabilization phase must not be treated as explicit state commit boundary</strong><br/>
  FROG — Free Open Graphical Language
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#1-case-name">1. Case Name</a></li>
  <li><a href="#2-expected">2. Expected</a></li>
  <li><a href="#3-why">3. Why</a></li>
  <li><a href="#4-boundary-being-violated">4. Boundary Being Violated</a></li>
  <li><a href="#5-scenario">5. Scenario</a></li>
  <li><a href="#6-what-makes-the-case-invalid">6. What Makes the Case Invalid</a></li>
  <li><a href="#7-why-runtime-stabilization-is-not-enough">7. Why Runtime Stabilization Is Not Enough</a></li>
  <li><a href="#8-what-must-not-be-preserved-as-truth">8. What Must Not Be Preserved as Truth</a></li>
  <li><a href="#9-expected-conformance-result">9. Expected Conformance Result</a></li>
  <li><a href="#10-rationale">10. Rationale</a></li>
  <li><a href="#11-minimum-conformance-reading">11. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="1-case-name">1. Case Name</h2>

<p>
  Case:
  <code>36_inferred_runtime_stabilization_phase_must_not_be_treated_as_explicit_state_commit_boundary</code>
</p>

<hr/>

<h2 id="2-expected">2. Expected</h2>

<p><strong>Expected:</strong> invalid</p>

<p><strong>Expected reason:</strong> rejected as a semantic substitution.</p>

<p>
  <strong>Expected rejection basis:</strong>
  the case attempts to promote an inferred runtime stabilization phase, settling phase,
  quiescence point, convergence wait, deferred flush completion, or backend-private
  “state is now stable” moment into an explicit state commit boundary, even though that
  commit boundary is not established as language-level meaning.
</p>

<hr/>

<h2 id="3-why">3. Why</h2>

<p>This case exists to reject the forbidden collapse:</p>

<pre>inferred runtime stabilization phase
            ->
explicit state commit boundary</pre>

<p>
  A runtime may internally perform stabilization work. It may drain queues, synchronize
  workers, flush buffers, complete deferred propagation, or wait for an internal fixed
  point before considering its own execution machinery settled.
</p>

<p>
  Those implementation details do not by themselves define when state becomes semantically
  committed in the language.
</p>

<p>
  If a case depends on such inferred runtime behavior to establish an explicit commit
  boundary, it must be rejected.
</p>

<hr/>

<h2 id="4-boundary-being-violated">4. Boundary Being Violated</h2>

<p>This case violates the normative separation between:</p>

<ul>
  <li>state commit semantics established by validated program meaning, and</li>
  <li>runtime-private stabilization behavior inferred from one implementation strategy.</li>
</ul>

<p>The invalid substitution is:</p>

<pre>validated semantic commit boundary
            !=
runtime stabilization convenience</pre>

<p>
  The case attempts to derive language truth from implementation quiescence instead of
  from published semantic structure.
</p>

<hr/>

<h2 id="5-scenario">5. Scenario</h2>

<p>
  A program, derivation path, implementation note, backend interpretation, or conformance
  claim asserts that state should be considered semantically committed because one runtime
  eventually reaches a stable point after its internal settling logic completes.
</p>

<p>In that scenario, the claimed commit boundary is not grounded in:</p>

<ul>
  <li>explicit validated state semantics,</li>
  <li>published execution meaning,</li>
  <li>an explicit semantic boundary defined by the language,</li>
  <li>or a preserved distinction already established upstream.</li>
</ul>

<p>Instead, the claim relies on observations such as:</p>

<ul>
  <li>the runtime stopped changing values,</li>
  <li>all deferred updates appear flushed,</li>
  <li>the executor reports a stable snapshot,</li>
  <li>the backend reached a quiescent phase,</li>
  <li>the implementation now considers the state “settled enough.”</li>
</ul>

<p>
  That is not sufficient to establish an explicit state commit boundary in FROG.
</p>

<hr/>

<h2 id="6-what-makes-the-case-invalid">6. What Makes the Case Invalid</h2>

<p>This case is invalid because it replaces semantic ownership with runtime convenience.</p>

<p>More precisely, it is invalid because it does at least one of the following:</p>

<ul>
  <li>treats an inferred stabilization phase as if it were a published commit boundary,</li>
  <li>derives commit truth from backend-private settling behavior,</li>
  <li>assumes that runtime quiescence automatically defines semantic visibility,</li>
  <li>uses implementation convergence as a surrogate for validated commit meaning,</li>
  <li>collapses a language-level boundary into one executor’s internal coordination phase.</li>
</ul>

<p>
  A conforming implementation may use stabilization internally, but it must not promote
  that internal phase into language law.
</p>

<hr/>

<h2 id="7-why-runtime-stabilization-is-not-enough">7. Why Runtime Stabilization Is Not Enough</h2>

<p>
  Runtime stabilization is an implementation concern. It may vary across schedulers,
  targets, runtimes, optimization strategies, buffering models, or backend contracts.
</p>

<p>That makes it unsuitable as the owner of semantic commit truth.</p>

<p>If stabilization were enough, then:</p>

<ul>
  <li>different runtimes could expose different commit points while all claiming correctness,</li>
  <li>semantic meaning would drift with executor design,</li>
  <li>validation would lose authority over state boundaries,</li>
  <li>backend-private coordination would silently redefine the language.</li>
</ul>

<p>
  FROG cannot permit that substitution.
</p>

<hr/>

<h2 id="8-what-must-not-be-preserved-as-truth">8. What Must Not Be Preserved as Truth</h2>

<p>
  Later layers may preserve runtime-private stabilization mechanisms as implementation
  machinery, but they must not preserve them as semantic truth in place of an explicit
  commit boundary.
</p>

<p>In particular, a conforming pipeline must not preserve as normative truth:</p>

<ul>
  <li>“the runtime became stable, therefore commit occurred,”</li>
  <li>“the backend reached quiescence, therefore the language commit boundary is here,”</li>
  <li>“the executor finished settling, therefore explicit commit semantics are satisfied,”</li>
  <li>“the state looks stable now, therefore semantic commit has been established.”</li>
</ul>

<p>The rejected promotion is:</p>

<pre>runtime-private stabilization fact
        -/-> explicit semantic commit boundary</pre>

<hr/>

<h2 id="9-expected-conformance-result">9. Expected Conformance Result</h2>

<p>
  This case must be rejected as invalid.
</p>

<p>The rejection result is not:</p>

<ul>
  <li>“runtime stabilization is forbidden,”</li>
  <li>“implementations may not settle internally,”</li>
  <li>“quiescence logic is disallowed.”</li>
</ul>

<p>The actual rejection result is:</p>

<ul>
  <li>runtime stabilization may exist internally,</li>
  <li>but it must not be treated as an explicit semantic state commit boundary unless the language meaning already establishes that boundary.</li>
</ul>

<hr/>

<h2 id="10-rationale">10. Rationale</h2>

<p>
  This rejection matters because commit boundaries are exactly the kind of semantic edge
  that can be quietly stolen by implementation details if the repository is not explicit.
</p>

<p>
  Independent implementations need freedom to use different stabilization mechanisms
  internally. That freedom is compatible with conformance only if those mechanisms stay
  below semantic ownership.
</p>

<p>
  Once a runtime-specific stabilization phase is allowed to define commit meaning, the
  architectural stack collapses:
</p>

<pre>source
  ->
validated meaning
  ->
derived representation
  ->
runtime-private convenience
  ->
hidden language law</pre>

<p>
  FROG must reject that collapse.
</p>

<p>The language-level rule remains:</p>

<pre>explicit semantic commit boundary
            remains distinct from
inferred runtime stabilization phase</pre>

<hr/>

<h2 id="11-minimum-conformance-reading">11. Minimum Conformance Reading</h2>

<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the case attempts to infer commit semantics from runtime stabilization behavior,</li>
  <li>that behavior is implementation-private and may vary,</li>
  <li>it therefore cannot be promoted to explicit language-level commit truth,</li>
  <li>the case must be rejected as invalid.</li>
</ul>

<p>The public truth asserted by this rejection is:</p>

<pre>inferred runtime stabilization phase
            must not be treated as
explicit state commit boundary</pre>
