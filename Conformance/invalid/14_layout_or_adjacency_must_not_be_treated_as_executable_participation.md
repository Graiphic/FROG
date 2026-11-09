<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1>Conformance Case — Invalid: Layout or Adjacency Must Not Be Treated as Executable Participation</h1>

<p><strong>Invalid conformance case for collapsing diagram layout, proximity, or adjacency into executable participation in FROG v0.1</strong><br>
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

<p>This invalid case covers a source shape or tool behavior that treats diagram layout, spatial proximity, alignment, or adjacency as though they were sufficient to create executable participation or executable dependency.</p>

<p>Typical anti-patterns include:</p>

<ul>
  <li>treating a nearby node as though it were part of a computation without an explicit edge,</li>
  <li>treating visual alignment as though it implied dataflow direction,</li>
  <li>treating graphical neighborhood as though it implied evaluation dependency,</li>
  <li>auto-connecting source objects because they appear visually related,</li>
  <li>using placement or routing convenience as a substitute for explicit executable connectivity.</li>
</ul>

<p>Conceptually, the forbidden collapse is:</p>

<pre><code>layout or adjacency
          ==
executable participation
</code></pre>

<p>Base FROG v0.1 does not allow that equivalence.</p>

<hr>

<h2 id="expected-status">2. Expected Status</h2>

<p><strong>Expected:</strong> invalid</p>

<hr>

<h2 id="intended-anti-pattern">3. Intended Anti-Pattern</h2>

<p>The invalid intent is any source shape, validator behavior, derivation behavior, or lowering behavior that requires layout or adjacency to act as executable graph law.</p>

<p>A minimal conceptual anti-pattern is:</p>

<pre><code>diagram layout

  interface_input(x) ----&gt; frog.core.add ----&gt; interface_output(result)

                 frog.core.multiply
                 (visually nearby, not connected)

tool interpretation:
  "multiply is close to add,
   therefore multiply also participates"
</code></pre>

<p>Another invalid interpretation is:</p>

<pre><code>objects appear aligned
    therefore
dependency exists
</code></pre>

<p>or:</p>

<pre><code>objects appear adjacent
    therefore
an implicit edge may be inferred
</code></pre>

<p>without an explicit published rule establishing such participation.</p>

<hr>

<h2 id="boundaries-exercised">4. Boundaries Exercised</h2>

<ul>
  <li>diagram-side executable participation,</li>
  <li>explicit edge connectivity,</li>
  <li>graph structure versus layout metadata,</li>
  <li>dependency semantics versus visual arrangement,</li>
  <li>rejection of semantic collapse across executable graph law and source presentation.</li>
</ul>

<hr>

<h2 id="why-this-case-is-invalid">5. Why this Case Is Invalid</h2>

<p>This case is invalid because base FROG v0.1 publishes these roles as distinct:</p>

<ul>
  <li>executable participation is established through explicit source-side executable objects and explicit valid connectivity,</li>
  <li>diagram layout belongs to source representation, readability, and recoverability,</li>
  <li>spatial proximity does not create graph edges,</li>
  <li>alignment does not create dataflow,</li>
  <li>adjacency does not create evaluation dependency,</li>
  <li>unconnected nodes do not become participating nodes merely because they are visually suggestive.</li>
</ul>

<p>Treating layout as execution law therefore contradicts the published architecture.</p>

<p>The required separation is:</p>

<pre><code>explicit executable graph
          !=
diagram layout suggestion
</code></pre>

<hr>

<h2 id="expected-validation-outcome">6. Expected Validation Outcome</h2>

<p>A conforming validator should reject the case when the source or tool behavior requires layout, proximity, or adjacency to be accepted as executable participation.</p>

<p>It should report that at least one of the following boundaries has been violated:</p>

<ul>
  <li>explicit connectivity versus inferred connectivity,</li>
  <li>participating node set versus nearby node set,</li>
  <li>dependency semantics versus layout suggestion,</li>
  <li>graph law versus presentation metadata.</li>
</ul>

<p>If the case attempts to justify the collapse through editor convenience, readability heuristics, automatic beautification, routing simplification, or backend preference, that should still be rejected. None of those creates language law.</p>

<hr>

<h2 id="why-derivation-is-forbidden">7. Why Derivation Is Forbidden</h2>

<p>Execution IR derivation is forbidden for this invalid case because validation has not established a legitimate executable graph meaning.</p>

<p>There is no conforming basis for deriving:</p>

<ul>
  <li>an executable edge from layout alone,</li>
  <li>a participating node from adjacency alone,</li>
  <li>a dependency relation from alignment alone,</li>
  <li>or a backend-facing contract that pretends visually related objects were explicitly connected all along.</li>
</ul>

<p>Derivation would first require an unpublished semantic reinterpretation. That is not allowed.</p>

<hr>

<h2 id="why-silent-repair-is-wrong">8. Why Silent Repair Is Wrong</h2>

<p>A conforming toolchain must not silently “repair” this case by:</p>

<ul>
  <li>creating inferred edges from placement,</li>
  <li>pulling nearby nodes into the executable graph,</li>
  <li>rewriting layout adjacency into dataflow dependency,</li>
  <li>treating layout metadata as executable metadata,</li>
  <li>resolving ambiguous source arrangement by inventing hidden graph structure while still claiming conformance.</li>
</ul>

<p>Such behavior would erase distinctions that the published source-to-meaning boundary requires to remain explicit and recoverable.</p>

<p>The forbidden repair pattern is:</p>

<pre><code>nearby node exists
    -/-> executable participation exists

alignment exists
    -/-> dependency exists

layout metadata
    -/-> graph law
</code></pre>

<hr>

<h2 id="summary">9. Summary</h2>

<p>This case must be rejected because FROG v0.1 treats explicit executable participation and diagram layout as distinct published categories.</p>

<p>A conforming implementation must reject any source or tool behavior that depends on treating layout, proximity, or adjacency as though they created executable participation or dependency.</p>

<p>The essential rule is:</p>

<pre><code>layout or adjacency
    does not create
executable participation
</code></pre>
