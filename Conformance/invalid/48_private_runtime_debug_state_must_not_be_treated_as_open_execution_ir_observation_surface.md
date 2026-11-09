<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 48</h1>

<p align="center">
  <strong>Private runtime debug state must not be treated as open execution IR observation surface</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#case-name">1. Case Name</a></li>
  <li><a href="#expected">2. Expected</a></li>
  <li><a href="#why">3. Why</a></li>
  <li><a href="#boundary-being-violated">4. Boundary Being Violated</a></li>
  <li><a href="#scenario">5. Scenario</a></li>
  <li><a href="#what-makes-the-case-invalid">6. What Makes the Case Invalid</a></li>
  <li><a href="#why-private-debug-state-is-not-enough">7. Why Private Debug State Is Not Enough</a></li>
  <li><a href="#what-must-not-be-preserved-as-truth">8. What Must Not Be Preserved as Truth</a></li>
  <li><a href="#expected-conformance-result">9. Expected Conformance Result</a></li>
  <li><a href="#rationale">10. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">11. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="case-name">1. Case Name</h2>
<p>Case: <code>48_private_runtime_debug_state_must_not_be_treated_as_open_execution_ir_observation_surface</code></p>

<hr/>

<h2 id="expected">2. Expected</h2>
<p><strong>Expected:</strong> invalid</p>

<p><strong>Expected reason:</strong> rejected as an architectural collapse.</p>

<p><strong>Expected rejection basis:</strong> the case attempts to treat runtime-private debug buffers, probe state, snapshot state, watch caches, trace records, diagnostic mirrors, or executor-private inspection machinery as if they were the public observation surface of the open execution-facing IR.</p>

<hr/>

<h2 id="why">3. Why</h2>
<p>This case exists to reject the forbidden substitution:</p>

<pre><code>private runtime debug state
            -&gt;
open execution IR observation surface
</code></pre>

<p>A runtime may introduce many forms of private debug machinery:</p>

<ul>
  <li>probe buffers,</li>
  <li>watch caches,</li>
  <li>trace records,</li>
  <li>snapshot stores,</li>
  <li>diagnostic mirrors,</li>
  <li>debugger-owned temporary state,</li>
  <li>executor-private inspection data,</li>
  <li>backend-specific introspection structures.</li>
</ul>

<p>Those structures may exist for implementation convenience. They do not become the public observation surface of the open execution-facing IR merely because one runtime relies on them for debugging, tracing, or inspection.</p>

<p>If a case attempts that promotion, it must be rejected.</p>

<hr/>

<h2 id="boundary-being-violated">4. Boundary Being Violated</h2>
<p>This case violates the normative separation between:</p>

<ul>
  <li>open execution-facing observation structure that belongs to the public derived IR layer, and</li>
  <li>private runtime debug state that belongs only to one implementation-owned realization.</li>
</ul>

<p>The invalid substitution is:</p>

<pre><code>open execution IR observation surface
                !=
private runtime debug state
</code></pre>

<p>The case attempts to let downstream diagnostic machinery replace the public execution-facing observation layer.</p>

<hr/>

<h2 id="scenario">5. Scenario</h2>
<p>A valid source program is validated and represented in an open execution-facing form.</p>

<p>That derived execution-facing form may expose observation-relevant structure at the public layer, such as:</p>

<ul>
  <li>recoverable execution-facing entities,</li>
  <li>inspectable participation boundaries,</li>
  <li>observable state involvement,</li>
  <li>publicly attributable execution-facing structure that remains above runtime realization.</li>
</ul>

<p>A runtime then realizes that execution-facing form and introduces private debugging and inspection machinery for tracing, watch support, probing, snapshotting, diagnostics, or executor-local introspection.</p>

<p>The conformance failure occurs when that runtime-private machinery is then treated as:</p>

<ul>
  <li>the canonical public observation surface of the open execution-facing IR,</li>
  <li>the semantic owner of execution-facing observation meaning,</li>
  <li>the only observation layer that must be preserved across implementations,</li>
  <li>the required explanation of what the open IR exposes for observation.</li>
</ul>

<p>In that scenario, private runtime realization is silently replacing a public derived layer.</p>

<hr/>

<h2 id="what-makes-the-case-invalid">6. What Makes the Case Invalid</h2>
<p>This case is invalid because it promotes downstream private debug machinery into public execution-facing observation truth.</p>

<p>More precisely, it is invalid because it does at least one of the following:</p>

<ul>
  <li>treats probe or watch state as if it were the canonical public observation surface of the open IR,</li>
  <li>makes public inspection meaning depend on access to implementation-private debug structures,</li>
  <li>collapses the open execution-facing observation layer into runtime-private diagnostics,</li>
  <li>assumes one implementation’s debugger model is the required public observation truth surface,</li>
  <li>turns diagnostic convenience into architectural ownership.</li>
</ul>

<p>A conforming implementation may use private debug state. It must not promote that state into the public observation surface of the open execution-facing IR.</p>

<hr/>

<h2 id="why-private-debug-state-is-not-enough">7. Why Private Debug State Is Not Enough</h2>
<p>Private runtime debug state is not enough because it is downstream, implementation-owned, and potentially executor-specific.</p>

<p>Different runtimes may realize the same open execution-facing observation surface using different probe buffers, snapshot models, trace systems, watch implementations, or diagnostic caches. That freedom is acceptable only if the public observation layer remains independent from those private choices.</p>

<p>If private debug state were enough, then:</p>

<ul>
  <li>the open execution-facing IR would stop being a meaningful public observation layer,</li>
  <li>independent implementations would become harder to compare,</li>
  <li>conformance would drift toward debugger-specific behavior,</li>
  <li>public inspectability would depend on implementation-private tooling machinery,</li>
  <li>the architecture would lose a critical IR / runtime boundary.</li>
</ul>

<p>FROG must reject that substitution.</p>

<hr/>

<h2 id="what-must-not-be-preserved-as-truth">8. What Must Not Be Preserved as Truth</h2>
<p>Later layers may preserve private runtime debug state as implementation machinery, but they must not preserve it as the public truth surface of open execution-facing observation.</p>

<p>In particular, a conforming pipeline must not preserve as normative truth:</p>

<ul>
  <li>“the debugger snapshot store is the canonical observation surface of the open IR,”</li>
  <li>“the runtime watch cache is the required public inspection model,”</li>
  <li>“the probe buffer layout is the canonical execution-facing observation structure,”</li>
  <li>“because the runtime can inspect it, that diagnostic state is now the public observation surface.”</li>
</ul>

<p>The rejected promotion is:</p>

<pre><code>private runtime debug state
            -/-> open execution IR observation surface
</code></pre>

<hr/>

<h2 id="expected-conformance-result">9. Expected Conformance Result</h2>
<p>This case must be rejected as invalid.</p>

<p>The rejection result is not:</p>

<ul>
  <li>“runtime debugging is forbidden,”</li>
  <li>“implementations may not use probe buffers,”</li>
  <li>“executors may not maintain watch caches or traces.”</li>
</ul>

<p>The actual rejection result is:</p>

<ul>
  <li>runtime-private debug state may exist,</li>
  <li>but it must not be treated as the public observation surface of the open execution-facing IR.</li>
</ul>

<hr/>

<h2 id="rationale">10. Rationale</h2>
<p>This rejection matters because FROG explicitly separates:</p>

<pre><code>validated meaning
        -&gt;
open execution-facing IR
        -&gt;
open execution observation surface
        -&gt;
private runtime realization
        -&gt;
private debug machinery
</code></pre>

<p>If runtime-private debug state can replace the open execution-facing observation surface, that stack collapses into:</p>

<pre><code>validated meaning
        -&gt;
runtime debugging behavior
        -&gt;
hidden public observation truth
</code></pre>

<p>That is precisely what the repository is trying to avoid.</p>

<p>Independent implementations need freedom to realize the same open execution-facing observation layer in different ways. They do not need freedom to redefine the public observation surface as private debugger state.</p>

<p>The preserved rule remains:</p>

<pre><code>private runtime debug state
            must remain distinct from
open execution IR observation surface
</code></pre>

<hr/>

<h2 id="minimum-conformance-reading">11. Minimum Conformance Reading</h2>
<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the case relies on private runtime debug state,</li>
  <li>that state is downstream and implementation-owned,</li>
  <li>it is being promoted to the public open-IR observation layer,</li>
  <li>that promotion breaks the open-IR / private-runtime boundary,</li>
  <li>the case must therefore be rejected as invalid.</li>
</ul>

<p>The public truth asserted by this rejection is:</p>

<pre><code>private runtime debug state
            must not be treated as
open execution IR observation surface
</code></pre>
