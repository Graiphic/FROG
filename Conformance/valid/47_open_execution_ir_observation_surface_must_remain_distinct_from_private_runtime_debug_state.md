<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 47</h1>

<p align="center">
  <strong>Open execution IR observation surface must remain distinct from private runtime debug state</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

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

<hr/>

<h2 id="case-name">1. Case Name</h2>

<p>
  Case:
  <code>47_open_execution_ir_observation_surface_must_remain_distinct_from_private_runtime_debug_state</code>
</p>

<hr/>

<h2 id="expected">2. Expected</h2>

<p><strong>Expected:</strong> valid</p>

<p><strong>Expected meaning:</strong> language-level meaning is established.</p>

<p>
  <strong>Expected preservation:</strong>
  the observation surface of the open execution-facing IR remains distinct from any private
  runtime debug state, inspection cache, debugger-owned snapshot, probe buffer, trace-only
  structure, or executor-private diagnostic memory introduced during realization.
</p>

<hr/>

<h2 id="why">3. Why</h2>

<p>
  This case exists to make one architectural rule explicit:
</p>

<pre><code>open execution IR observation surface
            !=
private runtime debug state
</code></pre>

<p>
  An open execution-facing IR may expose an observation surface that is meaningful at the public
  derived layer for inspection, reasoning, traceability, or conformance-oriented observation.
</p>

<p>
  A runtime may then introduce additional private debug machinery for implementation convenience:
</p>

<ul>
  <li>debugger-owned snapshots,</li>
  <li>probe buffers,</li>
  <li>inspection caches,</li>
  <li>runtime trace records,</li>
  <li>temporary watch mirrors,</li>
  <li>executor-private diagnostic state,</li>
  <li>backend-specific introspection structures.</li>
</ul>

<p>
  Those private debug structures may exist.
  They do not become the same thing as the public observation surface of the open execution-facing IR.
</p>

<p>
  The public execution-facing observation layer must remain distinguishable from runtime-private debug state.
</p>

<hr/>

<h2 id="boundary-being-exercised">4. Boundary Being Exercised</h2>

<p>
  This case exercises the preservation boundary across:
</p>

<pre><code>validated program meaning
        |
        v
open execution-facing IR observation surface
        |
        v
private runtime debug and diagnostic state
</code></pre>

<p>
  The distinction under test is between:
</p>

<ul>
  <li>observation structure that belongs to the public execution-facing representation, and</li>
  <li>inspection or debug state introduced only by one private runtime realization.</li>
</ul>

<hr/>

<h2 id="scenario">5. Scenario</h2>

<p>
  A valid FROG program is validated and represented in an open execution-facing form.
  That representation includes an observation surface that is meaningful at the public derived layer,
  such as observable execution-facing entities, recoverable boundaries, inspectable state participation,
  or other observation-relevant structure that remains above private runtime realization.
</p>

<p>
  A downstream runtime then realizes that execution-facing form and introduces its own private debug
  machinery for diagnostics, tracing, probing, watch support, snapshotting, or implementation-local inspection.
</p>

<p>
  The case is valid if:
</p>

<ul>
  <li>the open execution-facing observation surface remains meaningful as part of the public derived representation,</li>
  <li>the runtime may still maintain its own private debug state internally,</li>
  <li>the implementation does not collapse public observation structure into runtime-private diagnostic machinery.</li>
</ul>

<hr/>

<h2 id="what-makes-the-case-valid">6. What Makes the Case Valid</h2>

<p>
  This case is valid because it preserves the published stage boundary between open execution-facing observation
  and runtime-private debug state.
</p>

<p>
  More precisely, it is valid because:
</p>

<ul>
  <li>validated meaning is upstream from runtime realization,</li>
  <li>an open execution-facing representation is allowed to carry a public observation surface,</li>
  <li>runtime realization may introduce private diagnostic structures,</li>
  <li>the two are not silently treated as the same ownership layer.</li>
</ul>

<p>
  The case does not require that all runtimes implement debugging in the same way,
  with the same buffers, snapshots, trace models, or inspection caches.
</p>

<p>
  It does require that runtime-private debug state does not overwrite, erase, or become confused with
  the observation surface of the open execution-facing representation.
</p>

<hr/>

<h2 id="expected-meaning">7. Expected Meaning</h2>

<p>
  If this case validates, the established meaning includes at least the following:
</p>

<ul>
  <li>open execution-facing IR may have its own public observation surface,</li>
  <li>private runtime debug state may exist downstream for realization purposes,</li>
  <li>private runtime debug state does not become the owner of open execution-facing observation meaning,</li>
  <li>the distinction between open IR observation surface and runtime-private debug state remains preserved.</li>
</ul>

<p>
  The semantic reading is therefore:
</p>

<pre><code>validated meaning
        ->
open execution-facing observation surface
        ->
private runtime debug state
</code></pre>

<p>Not:</p>

<pre><code>runtime-private debug state
        replaces
open execution-facing observation surface
</code></pre>

<hr/>

<h2 id="expected-preservation">8. Expected Preservation</h2>

<p>
  A conforming implementation must preserve the distinction between open execution IR observation surface
  and private runtime debug state.
</p>

<p>
  At minimum, later layers must not silently collapse:
</p>

<ul>
  <li>public execution-facing observation surface, into</li>
  <li>opaque runtime-private diagnostic machinery.</li>
</ul>

<p>
  This does not require that an implementation publish all of its internal debug state as part of the IR.
</p>

<p>
  It does require that:
</p>

<ul>
  <li>open execution-facing observation structure remains attributable as part of the public derived layer,</li>
  <li>runtime-private debug state remains clearly downstream and implementation-owned,</li>
  <li>the public observation / private diagnostic boundary is not erased for convenience,</li>
  <li>independent implementations remain free to realize the same open observation surface with different private debugging strategies.</li>
</ul>

<hr/>

<h2 id="what-must-not-happen">9. What Must Not Happen</h2>

<p>
  A conforming implementation must not do any of the following:
</p>

<ul>
  <li>treat private runtime debug buffers as if they were the canonical public observation surface of the open execution-facing IR,</li>
  <li>erase public observation structure once runtime tracing or probing begins,</li>
  <li>make conformance or inspection depend on access to private runtime debug state,</li>
  <li>claim that debugger behavior is sufficient explanation of open execution-facing observation structure,</li>
  <li>collapse the public IR / private runtime boundary into one hidden diagnostic layer.</li>
</ul>

<p>
  The forbidden collapse is:
</p>

<pre><code>open execution IR observation surface
        -/-> private runtime debug state
</code></pre>

<hr/>

<h2 id="rationale">10. Rationale</h2>

<p>
  This distinction matters because FROG explicitly aims to keep open execution representation separate
  from runtime-private implementation convenience, including debugging convenience.
</p>

<p>
  If open execution-facing observation were allowed to collapse into runtime-private debug state, then:
</p>

<ul>
  <li>the IR would stop being a meaningful public observation layer,</li>
  <li>independent implementations would become harder to compare,</li>
  <li>conformance would become implementation-dependent,</li>
  <li>debugger-private behavior would drift upward into public truth,</li>
  <li>inspectability and tooling portability would weaken.</li>
</ul>

<p>
  The repository-level architectural rule must remain:
</p>

<pre><code>open execution-facing observation surface
            remains distinct from
private runtime debug machinery
</code></pre>

<p>
  Observation and debugging are one of the easiest places where private tooling state can silently
  replace a public representation.
  This case ensures that it does not.
</p>

<hr/>

<h2 id="minimum-conformance-reading">11. Minimum Conformance Reading</h2>

<p>
  A minimal conforming reading of this case is:
</p>

<ul>
  <li>the source is valid,</li>
  <li>language-level meaning is established,</li>
  <li>an open execution-facing representation may carry a public observation surface,</li>
  <li>a private runtime may introduce downstream debug state,</li>
  <li>the two layers must remain distinct.</li>
</ul>

<p>
  The public truth asserted by this case is:
</p>

<pre><code>open execution IR observation surface
            must remain distinct from
private runtime debug state
</code></pre>
