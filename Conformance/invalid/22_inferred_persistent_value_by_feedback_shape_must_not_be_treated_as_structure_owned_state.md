<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1>Conformance Case — Invalid: Inferred Persistent Value by Feedback Shape Must Not Be Treated as Structure-Owned State</h1>

<p><strong>Invalid conformance case for collapsing a feedback-shaped layout into explicit structure-owned persistent state in FROG v0.1</strong><br>
FROG — Free Open Graphical Language</p>

<hr>

<h2>Contents</h2>
<ul>
  <li><a href="#case-overview">1. Case Overview</a></li>
  <li><a href="#expected-status">2. Expected Status</a></li>
  <li><a href="#intended-anti-pattern">3. Intended Anti-Pattern</a></li>
  <li><a href="#boundaries-exercised">4. Boundaries Exercised</a></li>
  <li><a href="#why-this-case-is-invalid">5. Why this Case Is Invalid</a></li>
  <li><a href="#expected-validation-outcome">6. Expected Validation Outcome</a></li>
  <li><a href="#why-derivation-is-forbidden">7. Why Derivation Is Forbidden</a></li>
  <li><a href="#why-silent-repair-is-wrong">8. Why Silent Repair Is Wrong</a></li>
  <li><a href="#summary">9. Summary</a></li>
</ul>

<hr>

<h2 id="case-overview">1. Case Overview</h2>

<p>This invalid case covers a source shape or tool behavior that treats a feedback-shaped wire path, loop-like geometry, or drawn return path as though it were sufficient to create explicit structure-owned persistent state.</p>

<p>Typical anti-patterns include:</p>

<ul>
  <li>treating a wire that visually loops back as though it stored a prior-step value,</li>
  <li>treating apparent feedback shape as though it implied legal persistence across iterations,</li>
  <li>promoting a drawn return path into a state-owning mechanism without explicit declaration,</li>
  <li>using feedback geometry as a substitute for explicit state ownership,</li>
  <li>treating a persistent value as though it existed merely because the diagram visually suggests recurrence.</li>
</ul>

<p>Conceptually, the forbidden collapse is:</p>

<pre><code>inferred persistent value by feedback shape
                    ==
explicit structure-owned state
</code></pre>

<p>Base FROG v0.1 does not allow that equivalence.</p>

<hr>

<h2 id="expected-status">2. Expected Status</h2>

<p><strong>Expected:</strong> invalid</p>

<hr>

<h2 id="intended-anti-pattern">3. Intended Anti-Pattern</h2>

<p>The invalid intent is any source shape, validator behavior, derivation behavior, or lowering behavior that requires a feedback-shaped layout to act as persistent state law.</p>

<p>A minimal conceptual anti-pattern is:</p>

<pre><code>diagram layout

 current input ----&gt; frog.core.add ----&gt; result
                      ^            |
                      |            |
                      +------------+

tool interpretation:
  "the drawn feedback shape means the previous result is stored"

tool interpretation:
  "the loop-like wire therefore creates structure-owned state"
</code></pre>

<p>Another invalid interpretation is:</p>

<pre><code>feedback-looking path exists
    therefore
persistent value exists
</code></pre>

<p>or:</p>

<pre><code>drawn return path exists
    therefore
prior-step state exists
</code></pre>

<p>without an explicit published rule establishing such state semantics.</p>

<hr>

<h2 id="boundaries-exercised">4. Boundaries Exercised</h2>

<ul>
  <li>explicit structure-owned state,</li>
  <li>explicit prior-state and next-state participation,</li>
  <li>legal persistent state versus feedback-shaped layout,</li>
  <li>state semantics versus wire geometry,</li>
  <li>rejection of semantic collapse across explicit memory law and visual recurrence.</li>
</ul>

<hr>

<h2 id="why-this-case-is-invalid">5. Why this Case Is Invalid</h2>

<p>This case is invalid because base FROG v0.1 publishes these roles as distinct:</p>

<ul>
  <li>persistent state arises from explicit semantic state mechanisms,</li>
  <li>drawn feedback geometry belongs to source presentation and recoverability,</li>
  <li>a loop-like wire does not create legal prior-step value semantics,</li>
  <li>apparent recurrence does not create storage ownership,</li>
  <li>layout shape does not become structure-owned state law.</li>
</ul>

<p>Treating feedback shape as persistent state therefore contradicts the published architecture.</p>

<p>The required separation is:</p>

<pre><code>explicit state law
          !=
feedback-shaped layout
</code></pre>

<hr>

<h2 id="expected-validation-outcome">6. Expected Validation Outcome</h2>

<p>A conforming validator should reject the case when the source or tool behavior requires a feedback-shaped layout to be accepted as structure-owned persistent state.</p>

<p>It should report that at least one of the following boundaries has been violated:</p>

<ul>
  <li>explicit state declaration versus inferred state declaration,</li>
  <li>legal persistence versus feedback-shaped geometry,</li>
  <li>prior-step value semantics versus drawn return path,</li>
  <li>memory law versus layout suggestion.</li>
</ul>

<p>If the case attempts to justify the collapse through editor convenience, readability heuristics, loop-like appearance, or backend preference, that should still be rejected. None of those creates language law.</p>

<hr>

<h2 id="why-derivation-is-forbidden">7. Why Derivation Is Forbidden</h2>

<p>Execution IR derivation is forbidden for this invalid case because validation has not established a legitimate persistent state mechanism.</p>

<p>There is no conforming basis for deriving:</p>

<ul>
  <li>a persistent state cell from a loop-shaped wire alone,</li>
  <li>a prior-step value from apparent re-entry alone,</li>
  <li>a legal state transition from feedback geometry alone,</li>
  <li>or a backend-facing contract that pretends a visually recursive path was explicit state ownership all along.</li>
</ul>

<p>Derivation would first require an unpublished semantic reinterpretation. That is not allowed.</p>

<hr>

<h2 id="why-silent-repair-is-wrong">8. Why Silent Repair Is Wrong</h2>

<p>A conforming toolchain must not silently “repair” this case by:</p>

<ul>
  <li>creating synthetic state ownership from feedback geometry,</li>
  <li>rewriting a loop-like wire into an explicit state mechanism,</li>
  <li>treating apparent recurrence as legal persistent storage,</li>
  <li>using layout shape as justification for prior-step semantics,</li>
  <li>resolving ambiguous feedback geometry by inventing hidden structure-owned state while still claiming conformance.</li>
</ul>

<p>Such behavior would erase distinctions that the published source-to-meaning boundary requires to remain explicit and recoverable.</p>

<p>The forbidden repair pattern is:</p>

<pre><code>feedback shape exists
    -/-> persistent state exists

drawn return path exists
    -/-> prior-step value exists

layout geometry
    -/-> structure-owned state law
</code></pre>

<hr>

<h2 id="summary">9. Summary</h2>

<p>This case must be rejected because FROG v0.1 treats explicit structure-owned state and inferred persistent value by feedback shape as distinct published categories.</p>

<p>A conforming implementation must reject any source or tool behavior that depends on treating feedback-shaped layout as though it created structure-owned persistent state.</p>

<p>The essential rule is:</p>

<pre><code>inferred persistent value by feedback shape
    does not create
structure-owned state
</code></pre>
