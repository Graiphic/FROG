<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1>Conformance Case — 13 Explicit Diagram Participation Remains Distinct from Layout or Adjacency</h1>

<p><strong>Valid conformance case for preserving that executable participation and dependency arise from explicit diagram participation rather than layout, proximity, or adjacency in FROG v0.1</strong><br>
FROG — Free Open Graphical Language</p>

<hr>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#expected-status">2. Expected Status</a></li>
  <li><a href="#illustrative-source-shape">3. Illustrative Source Shape</a></li>
  <li><a href="#semantic-intent">4. Semantic Intent</a></li>
  <li><a href="#boundaries-exercised">5. Boundaries Exercised</a></li>
  <li><a href="#why-the-case-is-valid">6. Why the Case Is Valid</a></li>
  <li><a href="#expected-validation-outcome">7. Expected Validation Outcome</a></li>
  <li><a href="#expected-semantic-preservation">8. Expected Semantic Preservation</a></li>
  <li><a href="#expected-ir-and-backend-preservation">9. Expected IR and Backend Preservation</a></li>
  <li><a href="#forbidden-reinterpretations">10. Forbidden Reinterpretations</a></li>
  <li><a href="#summary">11. Summary</a></li>
</ul>

<hr>

<h2 id="overview">1. Overview</h2>

<p>This conformance case defines a minimal valid program in which multiple diagram objects are visually present in the same region, some of them are close to each other, and yet executable participation and dependency are established only by explicit diagram-side participation and explicit connectivity.</p>

<p>The purpose of this case is to confirm that a conforming FROG toolchain can accept a program where:</p>

<ul>
  <li>diagram objects may be visually near one another,</li>
  <li>layout may suggest possible human interpretation,</li>
  <li>only explicit participation and explicit connectivity create execution meaning,</li>
  <li>layout, proximity, and adjacency remain non-authoritative for executable dependency.</li>
</ul>

<p>The conceptual shape is:</p>

<pre><code>diagram layout

  interface_input(x) ----&gt; frog.core.add ----&gt; interface_output(result)

                 frog.core.multiply
                 (nearby, but unconnected)

  constant(1) ----------/
</code></pre>

<p>In this case, the nearby <code>frog.core.multiply</code> node remains non-participating if it is not explicitly connected into the executable graph.</p>

<p>The required architectural rule is:</p>

<pre><code>layout or adjacency
        !=
executable participation
</code></pre>

<hr>

<h2 id="expected-status">2. Expected Status</h2>

<p><strong>Expected:</strong> valid</p>

<p>A conforming implementation should accept this case because the executable meaning is fully established by explicit source participation and does not depend on visual proximity.</p>

<hr>

<h2 id="illustrative-source-shape">3. Illustrative Source Shape</h2>

<p>This case is defined as a specification-level conformance case.</p>

<p>It is not currently tied to a mandatory published executable slice in <code>Examples/</code>.</p>

<p>An illustrative source shape would include:</p>

<ul>
  <li>one public interface input named <code>x</code>,</li>
  <li>one public interface output named <code>result</code>,</li>
  <li>one constant node carrying the value <code>1</code>,</li>
  <li>one ordinary arithmetic primitive <code>frog.core.add</code>,</li>
  <li>one additional ordinary primitive such as <code>frog.core.multiply</code> placed nearby in the diagram but not connected into the executed path,</li>
  <li>one diagram-side <code>interface_input</code> participation for <code>x</code>,</li>
  <li>one diagram-side <code>interface_output</code> participation for <code>result</code>,</li>
  <li>explicit edges from <code>x</code> and <code>1</code> into <code>frog.core.add</code>, and from <code>frog.core.add</code> into <code>result</code>,</li>
  <li>no executable edge to or from the nearby <code>frog.core.multiply</code> node.</li>
</ul>

<p>The minimal participation model is therefore:</p>

<pre><code>explicit node participation
      +
explicit connectivity
      =
executable meaning

nearby but unconnected nodes
      !=
executable participation
</code></pre>

<hr>

<h2 id="semantic-intent">4. Semantic Intent</h2>

<p>The semantic intent is a simple explicit computation such as:</p>

<pre><code>result = x + 1
</code></pre>

<p>That semantic intent is established only because:</p>

<ul>
  <li><code>x</code> enters through explicit public interface participation,</li>
  <li>the constant <code>1</code> participates explicitly,</li>
  <li><code>frog.core.add</code> is explicitly connected to those inputs,</li>
  <li><code>result</code> is explicitly produced through public output participation.</li>
</ul>

<p>The nearby <code>frog.core.multiply</code> node does not participate in execution merely because it is visually close, aligned, or suggestive in layout.</p>

<p>The semantic rule is therefore:</p>

<pre><code>graphical neighborhood
does not create
semantic dependency
</code></pre>

<hr>

<h2 id="boundaries-exercised">5. Boundaries Exercised</h2>

<ul>
  <li>diagram-side executable participation,</li>
  <li>explicit edge connectivity,</li>
  <li>ordinary primitive execution,</li>
  <li>public interface participation,</li>
  <li>distinction between executable graph structure and diagram layout,</li>
  <li>distinction between explicit dependency and visual adjacency.</li>
</ul>

<p>The key architectural boundary exercised by this case is:</p>

<pre><code>explicit graph structure
          !=
diagram layout suggestion
</code></pre>

<hr>

<h2 id="why-the-case-is-valid">6. Why the Case Is Valid</h2>

<p>This case is valid because:</p>

<ul>
  <li>the public interface declarations are well formed,</li>
  <li>the participating nodes are explicit and legal,</li>
  <li>the actual executable edges are explicit and legal,</li>
  <li>the computation <code>x + 1</code> is fully defined by explicit graph participation,</li>
  <li>the nearby unconnected node remains merely present in the source layout and does not need executable interpretation,</li>
  <li>the case preserves the architectural rule that executable meaning comes from validated graph participation rather than graphical arrangement.</li>
</ul>

<p>The essential point is that diagram layout may support readability and recoverability, but it does not create hidden wires, hidden dependencies, or hidden dataflow.</p>

<hr>

<h2 id="expected-validation-outcome">7. Expected Validation Outcome</h2>

<p>A conforming validator should accept this case and establish valid language-level meaning.</p>

<p>At minimum, validation should confirm that:</p>

<ul>
  <li>the source is structurally valid canonical FROG source,</li>
  <li>the public interface declarations are valid,</li>
  <li>the participating nodes and edges are valid,</li>
  <li>the primitive reference <code>frog.core.add</code> is valid,</li>
  <li>the constant participation is valid,</li>
  <li>the connected value flow is type-compatible,</li>
  <li>the unconnected nearby node does not need to be incorporated into executable meaning,</li>
  <li>no rule allows adjacency, overlap, alignment, or placement to substitute for explicit connectivity.</li>
</ul>

<p>The validator should therefore establish a valid program in which:</p>

<pre><code>explicit connection creates dependency
visual placement alone does not
</code></pre>

<hr>

<h2 id="expected-semantic-preservation">8. Expected Semantic Preservation</h2>

<p>Once validated, the program meaning should preserve at least the following distinctions:</p>

<ul>
  <li>participating nodes remain participating nodes,</li>
  <li>non-participating nearby nodes remain non-participating,</li>
  <li>explicit dependency remains explicit dependency,</li>
  <li>diagram layout remains layout and does not become executable law,</li>
  <li>recoverable attribution still shows which nodes and edges actually participate in execution.</li>
</ul>

<p>The preserved distinction is conceptually:</p>

<pre><code>connected node
     does imply
graph participation

nearby node
     does not imply
graph participation
</code></pre>

<hr>

<h2 id="expected-ir-and-backend-preservation">9. Expected IR and Backend Preservation</h2>

<p>If execution-facing IR is derived from this case, derivation should preserve explicitly:</p>

<ul>
  <li>entry participation for <code>x</code>,</li>
  <li>constant participation for <code>1</code>,</li>
  <li>primitive execution identity for <code>frog.core.add</code>,</li>
  <li>exit participation for <code>result</code>,</li>
  <li>the fact that the nearby unconnected node does not participate in the executable subgraph.</li>
</ul>

<p>If the case is later lowered into a backend contract, that contract should still preserve that:</p>

<ul>
  <li>the executable dependency graph comes from explicit validated connectivity,</li>
  <li>layout metadata may remain useful for tooling or recoverability,</li>
  <li>layout metadata must not create executable nodes, executable edges, or dependency relations.</li>
</ul>

<p>The preservation rule remains:</p>

<pre><code>layout metadata
      !=
dependency semantics
</code></pre>

<hr>

<h2 id="forbidden-reinterpretations">10. Forbidden Reinterpretations</h2>

<p>A conforming implementation must not reinterpret this case in any of the following ways:</p>

<ul>
  <li>as if nearby nodes automatically participated in the same computation,</li>
  <li>as if spatial alignment created implicit dependency,</li>
  <li>as if unconnected nodes could be pulled into execution because they appear visually related,</li>
  <li>as if layout proximity were sufficient to create dataflow, control flow, or evaluation order,</li>
  <li>as if backend or editor convenience could infer hidden graph edges from placement while still preserving conformance.</li>
</ul>

<p>The forbidden collapses can be summarized as:</p>

<pre><code>adjacency          -/-> dependency
alignment          -/-> dataflow edge
layout proximity   -/-> executable participation
diagram placement  -/-> graph law
</code></pre>

<hr>

<h2 id="summary">11. Summary</h2>

<p>This is the baseline valid conformance case for preserving that explicit diagram participation remains distinct from layout or adjacency.</p>

<p>A conforming toolchain should:</p>

<ul>
  <li>accept the case,</li>
  <li>validate it as meaningful,</li>
  <li>preserve that only explicit graph participation contributes to execution meaning,</li>
  <li>avoid collapsing layout or visual suggestion into dependency semantics.</li>
</ul>

<p>The essential preservation rule is:</p>

<pre><code>explicit diagram participation
              !=
layout or adjacency
</code></pre>
