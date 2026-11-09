<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1>Conformance Case — Invalid: Layout Grouping or Apparent Nesting Must Not Be Treated as Structure Boundary</h1>

<p><strong>Invalid conformance case for collapsing visual grouping, framing, enclosure, or apparent nesting into explicit structure boundary semantics in FROG v0.1</strong><br>
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

<p>This invalid case covers a source shape or tool behavior that treats visual grouping, graphical framing, enclosure, editor grouping, or apparent nesting as though they were sufficient to create an explicit structure boundary, structure region, or structure membership.</p>

<p>Typical anti-patterns include:</p>

<ul>
  <li>treating a node as structurally inside a control structure because it appears visually enclosed by a frame,</li>
  <li>treating a grouped set of nearby nodes as though they formed an implicit structure region,</li>
  <li>treating apparent nesting in the drawing as though it created semantic nesting in source,</li>
  <li>promoting editor grouping or bounding-box overlap into structure membership,</li>
  <li>using visual enclosure as a substitute for explicit structure declaration and explicit region assignment.</li>
</ul>

<p>Conceptually, the forbidden collapse is:</p>

<pre><code>layout grouping or apparent nesting
                 ==
explicit structure boundary
</code></pre>

<p>Base FROG v0.1 does not allow that equivalence.</p>

<hr>

<h2 id="expected-status">2. Expected Status</h2>

<p><strong>Expected:</strong> invalid</p>

<hr>

<h2 id="intended-anti-pattern">3. Intended Anti-Pattern</h2>

<p>The invalid intent is any source shape, validator behavior, derivation behavior, or lowering behavior that requires visual grouping or apparent nesting to act as structure law.</p>

<p>A minimal conceptual anti-pattern is:</p>

<pre><code>diagram layout

  +--------------------------------------+
  | visual frame only                    |
  |                                      |
  |   frog.core.add                      |
  |                                      |
  +--------------------------------------+

        frog.core.multiply
        (overlapping or visually near)

tool interpretation:
  "the frame implies a structure boundary"

tool interpretation:
  "the enclosed node is therefore inside a case-like or loop-like structure"
</code></pre>

<p>Another invalid interpretation is:</p>

<pre><code>visual enclosure exists
    therefore
structure boundary exists
</code></pre>

<p>or:</p>

<pre><code>apparent nesting exists
    therefore
region membership may be inferred
</code></pre>

<p>without an explicit published rule establishing such structure semantics.</p>

<hr>

<h2 id="boundaries-exercised">4. Boundaries Exercised</h2>

<ul>
  <li>explicit structure declaration,</li>
  <li>explicit structure-region membership,</li>
  <li>explicit structure-boundary ownership,</li>
  <li>structure semantics versus graphical grouping,</li>
  <li>declared nesting versus apparent nesting,</li>
  <li>rejection of semantic collapse across structure law and layout presentation.</li>
</ul>

<hr>

<h2 id="why-this-case-is-invalid">5. Why this Case Is Invalid</h2>

<p>This case is invalid because base FROG v0.1 publishes these roles as distinct:</p>

<ul>
  <li>structure boundaries arise from explicit structure declaration,</li>
  <li>structure regions arise from explicit structure-owned region definition,</li>
  <li>structure membership arises from explicit canonical membership,</li>
  <li>layout grouping belongs to presentation, readability, and recoverability,</li>
  <li>apparent nesting does not create semantic nesting,</li>
  <li>graphical enclosure does not create structure ownership.</li>
</ul>

<p>Treating grouping or apparent nesting as structure law therefore contradicts the published architecture.</p>

<p>The required separation is:</p>

<pre><code>explicit structure law
          !=
visual grouping or enclosure
</code></pre>

<hr>

<h2 id="expected-validation-outcome">6. Expected Validation Outcome</h2>

<p>A conforming validator should reject the case when the source or tool behavior requires visual grouping, enclosure, or apparent nesting to be accepted as structure boundary or structure membership.</p>

<p>It should report that at least one of the following boundaries has been violated:</p>

<ul>
  <li>explicit structure declaration versus visual grouping,</li>
  <li>explicit region membership versus apparent nesting,</li>
  <li>semantic structure boundary versus editor or diagram enclosure,</li>
  <li>declared structure ownership versus presentation-level grouping.</li>
</ul>

<p>If the case attempts to justify the collapse through editor convenience, readability heuristics, bounding-box recovery, or backend preference, that should still be rejected. None of those creates language law.</p>

<hr>

<h2 id="why-derivation-is-forbidden">7. Why Derivation Is Forbidden</h2>

<p>Execution IR derivation is forbidden for this invalid case because validation has not established a legitimate structure boundary or legitimate structure membership.</p>

<p>There is no conforming basis for deriving:</p>

<ul>
  <li>a structure object from a visual frame alone,</li>
  <li>a structure region from apparent grouping alone,</li>
  <li>a structure-owned node from enclosure alone,</li>
  <li>or a backend-facing contract that pretends visually grouped nodes were structurally nested all along.</li>
</ul>

<p>Derivation would first require an unpublished semantic reinterpretation. That is not allowed.</p>

<hr>

<h2 id="why-silent-repair-is-wrong">8. Why Silent Repair Is Wrong</h2>

<p>A conforming toolchain must not silently “repair” this case by:</p>

<ul>
  <li>creating synthetic structure boundaries from visual frames,</li>
  <li>assigning nodes to regions because they appear enclosed,</li>
  <li>rewriting apparent nesting into declared nesting,</li>
  <li>treating grouping metadata as structure metadata,</li>
  <li>resolving ambiguous layout by inventing hidden structure ownership while still claiming conformance.</li>
</ul>

<p>Such behavior would erase distinctions that the published source-to-meaning boundary requires to remain explicit and recoverable.</p>

<p>The forbidden repair pattern is:</p>

<pre><code>visual grouping exists
    -/-> structure boundary exists

apparent nesting exists
    -/-> structure membership exists

layout enclosure
    -/-> structure law
</code></pre>

<hr>

<h2 id="summary">9. Summary</h2>

<p>This case must be rejected because FROG v0.1 treats explicit structure boundaries and visual grouping or apparent nesting as distinct published categories.</p>

<p>A conforming implementation must reject any source or tool behavior that depends on treating layout grouping, enclosure, or apparent nesting as though they created a structure boundary or structure membership.</p>

<p>The essential rule is:</p>

<pre><code>layout grouping or apparent nesting
    does not create
structure boundary
</code></pre>
