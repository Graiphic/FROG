<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1>Conformance Case — Invalid: Inferred Evaluation Order Must Not Be Treated as Dependency or Connectivity</h1>

<p><strong>Invalid conformance case for collapsing inferred evaluation order into explicit dependency or connectivity in FROG v0.1</strong><br>
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

<p>This invalid case covers a source shape or tool behavior that treats a visually inferred, editor-inferred, or reader-inferred evaluation order as though it were sufficient to create semantic dependency or executable connectivity.</p>

<p>Typical anti-patterns include:</p>

<ul>
  <li>treating the node that appears earlier in the drawing as though it must execute first,</li>
  <li>treating left-to-right or top-to-bottom visual reading as though it created dependency,</li>
  <li>inserting hidden sequencing because one subgraph is drawn “before” another,</li>
  <li>rewriting visually ordered sibling computations into an ordered chain without explicit graph structure,</li>
  <li>using editor traversal order, serialization order, or display order as a substitute for explicit dependency.</li>
</ul>

<p>Conceptually, the forbidden collapse is:</p>

<pre><code>inferred evaluation order
            ==
explicit dependency or connectivity
</code></pre>

<p>Base FROG v0.1 does not allow that equivalence.</p>

<hr>

<h2 id="expected-status">2. Expected Status</h2>

<p><strong>Expected:</strong> invalid</p>

<hr>

<h2 id="intended-anti-pattern">3. Intended Anti-Pattern</h2>

<p>The invalid intent is any source shape, validator behavior, derivation behavior, or lowering behavior that requires inferred evaluation order to act as executable graph law.</p>

<p>A minimal conceptual anti-pattern is:</p>

<pre><code>diagram layout

  branch A: x + 1
  branch B: x * 2

  final: add(branch A, branch B)

tool interpretation:
  "branch A is drawn above branch B,
   therefore branch A must execute first"

tool interpretation:
  "branch A appears earlier in reading order,
   therefore branch B depends on branch A"
</code></pre>

<p>Another invalid interpretation is:</p>

<pre><code>visual sequencing exists
    therefore
dependency exists
</code></pre>

<p>or:</p>

<pre><code>display order exists
    therefore
hidden connectivity may be inferred
</code></pre>

<p>without an explicit published rule establishing such dependency.</p>

<hr>

<h2 id="boundaries-exercised">4. Boundaries Exercised</h2>

<ul>
  <li>explicit connectivity and dependency,</li>
  <li>evaluation dependency versus visual sequencing,</li>
  <li>graph law versus editor or reader interpretation,</li>
  <li>sibling branch independence versus imposed hidden order,</li>
  <li>rejection of semantic collapse across explicit graph structure and inferred sequencing.</li>
</ul>

<hr>

<h2 id="why-this-case-is-invalid">5. Why this Case Is Invalid</h2>

<p>This case is invalid because base FROG v0.1 publishes these roles as distinct:</p>

<ul>
  <li>explicit connectivity establishes dependency obligations,</li>
  <li>validated graph structure establishes what must precede what,</li>
  <li>visual sequencing does not create new edges,</li>
  <li>reader interpretation does not create hidden ordering constraints,</li>
  <li>display order, serialization order, and editor traversal order do not become language law.</li>
</ul>

<p>Treating inferred order as dependency therefore contradicts the published architecture.</p>

<p>The required separation is:</p>

<pre><code>explicit dependency graph
          !=
inferred execution order
</code></pre>

<hr>

<h2 id="expected-validation-outcome">6. Expected Validation Outcome</h2>

<p>A conforming validator should reject the case when the source or tool behavior requires inferred evaluation order to be accepted as semantic dependency or executable connectivity.</p>

<p>It should report that at least one of the following boundaries has been violated:</p>

<ul>
  <li>explicit dependency versus inferred sequencing,</li>
  <li>graph structure versus reading order,</li>
  <li>sibling independence versus imposed hidden ordering,</li>
  <li>connectivity law versus presentation or traversal convention.</li>
</ul>

<p>If the case attempts to justify the collapse through editor convenience, readability heuristics, deterministic rendering order, serialization order, or backend preference, that should still be rejected. None of those creates language law.</p>

<hr>

<h2 id="why-derivation-is-forbidden">7. Why Derivation Is Forbidden</h2>

<p>Execution IR derivation is forbidden for this invalid case because validation has not established a legitimate semantic dependency graph for the imposed order.</p>

<p>There is no conforming basis for deriving:</p>

<ul>
  <li>a dependency edge from visual order alone,</li>
  <li>a sequencing constraint from display order alone,</li>
  <li>a connectivity relation from editor traversal alone,</li>
  <li>or a backend-facing contract that pretends visually earlier nodes were semantically prior all along.</li>
</ul>

<p>Derivation would first require an unpublished semantic reinterpretation. That is not allowed.</p>

<hr>

<h2 id="why-silent-repair-is-wrong">8. Why Silent Repair Is Wrong</h2>

<p>A conforming toolchain must not silently “repair” this case by:</p>

<ul>
  <li>creating hidden sequencing edges from visual order,</li>
  <li>rewriting sibling branches into an ordered chain,</li>
  <li>using editor traversal order as dependency law,</li>
  <li>using serialization order as execution dependency,</li>
  <li>resolving ambiguous graph understanding by inventing hidden connectivity while still claiming conformance.</li>
</ul>

<p>Such behavior would erase distinctions that the published source-to-meaning boundary requires to remain explicit and recoverable.</p>

<p>The forbidden repair pattern is:</p>

<pre><code>visual order exists
    -/-> dependency exists

display order exists
    -/-> connectivity exists

serialization order
    -/-> semantic execution order
</code></pre>

<hr>

<h2 id="summary">9. Summary</h2>

<p>This case must be rejected because FROG v0.1 treats explicit connectivity and inferred evaluation order as distinct published categories.</p>

<p>A conforming implementation must reject any source or tool behavior that depends on treating inferred evaluation order as though it created semantic dependency or executable connectivity.</p>

<p>The essential rule is:</p>

<pre><code>inferred evaluation order
    does not create
dependency or connectivity
</code></pre>
