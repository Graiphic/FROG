<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1>Conformance Case — 15 Explicit Connectivity Remains Distinct from Inferred Evaluation Order</h1>

<p><strong>Valid conformance case for preserving that explicit connectivity defines dependency while evaluation order must not be inferred from diagram reading direction, placement, or visual sequencing in FROG v0.1</strong><br>
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

<p>This conformance case defines a minimal valid program in which explicit connectivity establishes all required executable dependencies, while no additional evaluation order is inferred from visual sequencing, top-to-bottom reading, left-to-right reading, or relative placement in the diagram.</p>

<p>The purpose of this case is to confirm that a conforming FROG toolchain can accept a program where:</p>

<ul>
  <li>multiple executable nodes exist in the same diagram region,</li>
  <li>some nodes are visually arranged in a way that may suggest a human reading order,</li>
  <li>explicit connectivity establishes the actual dependency graph,</li>
  <li>no extra semantic order is created merely because one node appears earlier, higher, lower, left, or right in the layout.</li>
</ul>

<p>The conceptual shape is:</p>

<pre><code>diagram layout

  interface_input(x)
        |
        +----------------------+
        |                      |
        v                      v
  frog.core.add_const    frog.core.multiply_const
      (+1)                    (*2)
        |                      |
        +----------+-----------+
                   |
                   v
             frog.core.add
                   |
                   v
      interface_output(result)
</code></pre>

<p>In this case, the two intermediate branches are both required before the final <code>frog.core.add</code>, but no rule says one branch must execute “first” merely because it appears above, below, left, or right.</p>

<p>The required architectural rule is:</p>

<pre><code>explicit connectivity
        !=
inferred evaluation order
</code></pre>

<hr>

<h2 id="expected-status">2. Expected Status</h2>

<p><strong>Expected:</strong> valid</p>

<p>A conforming implementation should accept this case because the executable meaning is fully established by explicit connectivity and valid primitive participation, without requiring any extra order to be inferred from layout.</p>

<hr>

<h2 id="illustrative-source-shape">3. Illustrative Source Shape</h2>

<p>This case is defined as a specification-level conformance case.</p>

<p>It is not currently tied to a mandatory published executable slice in <code>Examples/</code>.</p>

<p>An illustrative source shape would include:</p>

<ul>
  <li>one public interface input named <code>x</code>,</li>
  <li>one public interface output named <code>result</code>,</li>
  <li>one diagram-side <code>interface_input</code> participation for <code>x</code>,</li>
  <li>one primitive equivalent to adding a constant, such as <code>frog.core.add</code> with an explicit constant operand <code>1</code>,</li>
  <li>one primitive equivalent to multiplying by a constant, such as <code>frog.core.multiply</code> with an explicit constant operand <code>2</code>,</li>
  <li>one final <code>frog.core.add</code> primitive combining the two branch outputs,</li>
  <li>one diagram-side <code>interface_output</code> participation for <code>result</code>,</li>
  <li>explicit edges from <code>x</code> into both branches, and explicit edges from both branches into the final combining node.</li>
</ul>

<p>The minimal participation model is therefore:</p>

<pre><code>explicit dependency edges
      define
required execution dependencies

visual sequencing
      does not define
extra semantic order
</code></pre>

<hr>

<h2 id="semantic-intent">4. Semantic Intent</h2>

<p>The semantic intent is a dependency graph such as:</p>

<pre><code>a = x + 1
b = x * 2
result = a + b
</code></pre>

<p>That semantic intent establishes:</p>

<ul>
  <li>the two branch computations both depend on <code>x</code>,</li>
  <li>the final result depends on both branch results,</li>
  <li>the final combining node cannot complete before both branch outputs are available,</li>
  <li>no additional ordering exists between the two branch computations unless such ordering is introduced explicitly by the language.</li>
</ul>

<p>The semantic rule is therefore:</p>

<pre><code>dependency must be explicit
extra order must not be invented
</code></pre>

<hr>

<h2 id="boundaries-exercised">5. Boundaries Exercised</h2>

<ul>
  <li>diagram-side executable participation,</li>
  <li>explicit connectivity and dependency,</li>
  <li>ordinary primitive execution,</li>
  <li>public interface participation,</li>
  <li>distinction between dependency graph and human reading order,</li>
  <li>distinction between explicit connectivity and inferred sequencing.</li>
</ul>

<p>The key architectural boundary exercised by this case is:</p>

<pre><code>dependency graph law
          !=
layout-implied execution order
</code></pre>

<hr>

<h2 id="why-the-case-is-valid">6. Why the Case Is Valid</h2>

<p>This case is valid because:</p>

<ul>
  <li>the public interface declarations are well formed,</li>
  <li>the participating nodes are explicit and legal,</li>
  <li>the dependency edges are explicit and legal,</li>
  <li>the final result depends on both branch outputs through explicit graph structure,</li>
  <li>the case does not require any hidden sequencing rule beyond the explicit dependency graph,</li>
  <li>the case preserves the architectural rule that execution semantics come from explicit validated connectivity rather than from diagram reading direction.</li>
</ul>

<p>The essential point is that a human reader may look at the top branch first or the left branch first, but that human reading habit does not become language law.</p>

<hr>

<h2 id="expected-validation-outcome">7. Expected Validation Outcome</h2>

<p>A conforming validator should accept this case and establish valid language-level meaning.</p>

<p>At minimum, validation should confirm that:</p>

<ul>
  <li>the source is structurally valid canonical FROG source,</li>
  <li>the public interface declarations are valid,</li>
  <li>the participating primitives are valid,</li>
  <li>the constant participations are valid,</li>
  <li>the explicit connectivity is valid,</li>
  <li>the connected value flows are type-compatible,</li>
  <li>the final combining node explicitly depends on both branch outputs,</li>
  <li>no additional sequencing rule is required in order for the program to be meaningful,</li>
  <li>no rule allows visual ordering alone to create extra dependency.</li>
</ul>

<p>The validator should therefore establish a valid program in which:</p>

<pre><code>explicit dependencies constrain execution
visual order alone does not
</code></pre>

<hr>

<h2 id="expected-semantic-preservation">8. Expected Semantic Preservation</h2>

<p>Once validated, the program meaning should preserve at least the following distinctions:</p>

<ul>
  <li>branch dependency remains explicit dependency,</li>
  <li>final-node dependency remains explicit dependency on both branch results,</li>
  <li>branch placement remains layout information,</li>
  <li>branch ordering in the drawing does not become semantic ordering,</li>
  <li>recoverable attribution still shows the real dependency graph rather than an inferred reading sequence.</li>
</ul>

<p>The preserved distinction is conceptually:</p>

<pre><code>explicit incoming edge
     does imply
dependency

earlier visual appearance
     does not imply
required prior evaluation
</code></pre>

<hr>

<h2 id="expected-ir-and-backend-preservation">9. Expected IR and Backend Preservation</h2>

<p>If execution-facing IR is derived from this case, derivation should preserve explicitly:</p>

<ul>
  <li>entry participation for <code>x</code>,</li>
  <li>both branch computations as explicit executable nodes,</li>
  <li>the explicit branch-to-final-node dependencies,</li>
  <li>the absence of any additional semantic ordering between sibling branches beyond what the dependency graph requires.</li>
</ul>

<p>If the case is later lowered into a backend contract, that contract should still preserve that:</p>

<ul>
  <li>the executable dependency graph comes from explicit validated connectivity,</li>
  <li>backend scheduling may realize the graph in different ways as long as the published dependencies are preserved,</li>
  <li>layout order must not be substituted for semantic dependency order.</li>
</ul>

<p>The preservation rule remains:</p>

<pre><code>explicit connectivity
      defines
dependency obligations

visual sequencing
      does not define
extra semantic order
</code></pre>

<hr>

<h2 id="forbidden-reinterpretations">10. Forbidden Reinterpretations</h2>

<p>A conforming implementation must not reinterpret this case in any of the following ways:</p>

<ul>
  <li>as if the upper branch must execute before the lower branch because it appears first to the eye,</li>
  <li>as if left-to-right placement creates mandatory sequencing between otherwise independent sibling nodes,</li>
  <li>as if top-to-bottom arrangement creates dependency without explicit graph structure,</li>
  <li>as if a human-readable visual order were sufficient to introduce semantic execution order,</li>
  <li>as if backend or editor convenience could promote drawing order into dependency law while still preserving conformance.</li>
</ul>

<p>The forbidden collapses can be summarized as:</p>

<pre><code>visual order        -/-> dependency
top-to-bottom       -/-> required prior execution
left-to-right       -/-> semantic sequencing
layout arrangement  -/-> graph law
</code></pre>

<hr>

<h2 id="summary">11. Summary</h2>

<p>This is the baseline valid conformance case for preserving that explicit connectivity remains distinct from inferred evaluation order.</p>

<p>A conforming toolchain should:</p>

<ul>
  <li>accept the case,</li>
  <li>validate it as meaningful,</li>
  <li>preserve that explicit connectivity defines dependency,</li>
  <li>avoid collapsing visual sequencing into semantic execution order.</li>
</ul>

<p>The essential preservation rule is:</p>

<pre><code>explicit connectivity
        !=
inferred evaluation order
</code></pre>
