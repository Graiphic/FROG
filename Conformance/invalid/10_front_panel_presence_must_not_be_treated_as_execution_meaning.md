<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1>Conformance Case — 09 Front-Panel Presence Does Not by Itself Define Execution Meaning</h1>

<p><strong>Valid conformance case for preserving that front-panel presence alone does not create execution meaning in FROG v0.1</strong><br>
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

<p>This conformance case defines a minimal valid program in which a front panel is present, but execution meaning is established only by explicit executable participation in the diagram and not by front-panel presence alone.</p>

<p>The purpose of this case is to confirm that a conforming FROG toolchain can accept a program where:</p>

<ul>
  <li>a front panel exists and contains declared UI objects,</li>
  <li>some front-panel content does not participate in execution at all,</li>
  <li>execution meaning is created only where explicit and valid diagram-side participation exists,</li>
  <li>front-panel composition remains distinct from executable meaning.</li>
</ul>

<p>The conceptual shape is:</p>

<pre><code>front panel
  ├── numeric control "gain"      --- explicitly used through widget_value
  └── decorative label "title"    --- not executable

diagram
  interface_input(x) ----\
                          +---- frog.core.add ----> interface_output(result)
  widget_value(gain) ----/
</code></pre>

<p>The required architectural rule is:</p>

<pre><code>front-panel presence
        !=
execution meaning
</code></pre>

<hr>

<h2 id="expected-status">2. Expected Status</h2>

<p><strong>Expected:</strong> valid</p>

<p>A conforming implementation should accept this case because the executable meaning comes from explicit valid participation, not from front-panel existence by itself.</p>

<hr>

<h2 id="illustrative-source-shape">3. Illustrative Source Shape</h2>

<p>This case is defined as a specification-level conformance case.</p>

<p>It is not currently tied to a mandatory published executable slice in <code>Examples/</code>.</p>

<p>An illustrative source shape would include:</p>

<ul>
  <li>one public interface input named <code>x</code>,</li>
  <li>one public interface output named <code>result</code>,</li>
  <li>one front-panel numeric control named <code>gain</code>,</li>
  <li>one front-panel decorative element such as a label, caption, or purely presentational object,</li>
  <li>one diagram-side <code>interface_input</code> participation for <code>x</code>,</li>
  <li>one diagram-side <code>widget_value</code> participation for <code>gain</code>,</li>
  <li>one ordinary arithmetic primitive <code>frog.core.add</code>,</li>
  <li>one diagram-side <code>interface_output</code> participation for <code>result</code>.</li>
</ul>

<p>The minimal participation model is therefore:</p>

<pre><code>front panel contains more than the executable graph uses
only explicitly linked participation contributes to meaning
</code></pre>

<hr>

<h2 id="semantic-intent">4. Semantic Intent</h2>

<p>The semantic intent is:</p>

<pre><code>result = x + gain
</code></pre>

<p>That intent is established only because:</p>

<ul>
  <li><code>x</code> enters through explicit public interface participation,</li>
  <li><code>gain</code> enters through explicit <code>widget_value</code> participation,</li>
  <li>the primitive <code>frog.core.add</code> consumes those values explicitly,</li>
  <li><code>result</code> exits through explicit public output participation.</li>
</ul>

<p>The decorative or otherwise unconnected front-panel content does not add execution meaning merely by existing.</p>

<p>The semantic rule is therefore:</p>

<pre><code>declared front-panel object
does not by itself
become executable participation
</code></pre>

<hr>

<h2 id="boundaries-exercised">5. Boundaries Exercised</h2>

<ul>
  <li>front-panel declaration,</li>
  <li>public interface declaration,</li>
  <li><code>widget_value</code> participation,</li>
  <li>ordinary primitive execution,</li>
  <li>public output participation,</li>
  <li>distinction between presentational front-panel content and executable participation,</li>
  <li>distinction between UI composition and program meaning.</li>
</ul>

<p>The key architectural boundary exercised by this case is:</p>

<pre><code>front-panel composition
          !=
validated execution meaning
</code></pre>

<hr>

<h2 id="why-the-case-is-valid">6. Why the Case Is Valid</h2>

<p>This case is valid because:</p>

<ul>
  <li>the public interface declarations are well formed,</li>
  <li>the front-panel widget declaration for <code>gain</code> is well formed,</li>
  <li>the decorative or otherwise non-executable front-panel content is allowed to exist without needing executable interpretation,</li>
  <li>the diagram-side <code>widget_value</code> participation explicitly and legally references the widget whose value contributes to execution,</li>
  <li>the arithmetic primitive is used over compatible values,</li>
  <li>the graph is acyclic,</li>
  <li>no hidden state is required,</li>
  <li>no hidden event semantics are required,</li>
  <li>the program becomes meaningful only through explicit executable participation, which is exactly what the architecture requires.</li>
</ul>

<p>The essential point is that front-panel presence is allowed, useful, and durable, but it is not automatic executable law.</p>

<hr>

<h2 id="expected-validation-outcome">7. Expected Validation Outcome</h2>

<p>A conforming validator should accept this case and establish valid language-level meaning.</p>

<p>At minimum, validation should confirm that:</p>

<ul>
  <li>the source is structurally valid canonical FROG source,</li>
  <li>the interface input and output declarations are valid,</li>
  <li>the front-panel widget declaration for <code>gain</code> is valid,</li>
  <li>the diagram-side <code>widget_value</code> participation for <code>gain</code> is valid,</li>
  <li>the decorative or otherwise non-executable front-panel content does not need executable interpretation,</li>
  <li>the primitive reference <code>frog.core.add</code> is valid,</li>
  <li>the connected value flow is type-compatible,</li>
  <li>the program meaning depends on explicit diagram participation rather than on front-panel presence alone.</li>
</ul>

<p>The validator should therefore establish a valid program in which:</p>

<pre><code>front panel may exist
front panel may contain non-executable content
execution meaning still comes only from explicit valid participation
</code></pre>

<hr>

<h2 id="expected-semantic-preservation">8. Expected Semantic Preservation</h2>

<p>Once validated, the program meaning should preserve at least the following distinctions:</p>

<ul>
  <li>front-panel composition remains front-panel composition,</li>
  <li>diagram-side executable participation remains executable participation,</li>
  <li>the widget named <code>gain</code> contributes to execution only because it is explicitly referenced through <code>widget_value</code>,</li>
  <li>the decorative or otherwise non-executable front-panel content remains non-executable,</li>
  <li>public interface participation remains distinct from front-panel presence.</li>
</ul>

<p>The preserved distinction is conceptually:</p>

<pre><code>front-panel existence
     does not imply
execution participation

explicit executable participation
     does imply
execution meaning
</code></pre>

<hr>

<h2 id="expected-ir-and-backend-preservation">9. Expected IR and Backend Preservation</h2>

<p>If execution-facing IR is derived from this case, derivation should preserve explicitly:</p>

<ul>
  <li>public interface participation for <code>x</code> and <code>result</code>,</li>
  <li>widget-value participation for <code>gain</code>,</li>
  <li>ordinary arithmetic primitive execution for <code>frog.core.add</code>,</li>
  <li>the fact that decorative or otherwise non-executable front-panel content does not become executable participation merely because it exists in source.</li>
</ul>

<p>If the case is later lowered into a backend contract, that contract should still preserve that:</p>

<ul>
  <li>the executable program contract consists only of the explicitly validated executable participations,</li>
  <li>front-panel composition data may remain relevant for UI realization or recoverability,</li>
  <li>front-panel presence alone does not create extra executable ports, nodes, or dependencies.</li>
</ul>

<p>The preservation rule remains:</p>

<pre><code>UI composition data
      !=
automatic execution contract
</code></pre>

<hr>

<h2 id="forbidden-reinterpretations">10. Forbidden Reinterpretations</h2>

<p>A conforming implementation must not reinterpret this case in any of the following ways:</p>

<ul>
  <li>as if every front-panel object automatically created executable meaning,</li>
  <li>as if decorative or purely presentational front-panel content were executable by default,</li>
  <li>as if front-panel presence alone created interface ports, value flows, or primitive participation,</li>
  <li>as if diagram-side participation could be inferred solely from UI presence without explicit published rules,</li>
  <li>as if backend convenience could silently convert presentational UI content into executable content while still preserving conformance.</li>
</ul>

<p>The forbidden collapses can be summarized as:</p>

<pre><code>front-panel presence   -/-> execution meaning
UI layout              -/-> executable participation
decorative UI object   -/-> value source
front-panel existence  -/-> public interface contract
</code></pre>

<hr>

<h2 id="summary">11. Summary</h2>

<p>This is the baseline valid conformance case for preserving that front-panel presence alone does not define execution meaning.</p>

<p>A conforming toolchain should:</p>

<ul>
  <li>accept the case,</li>
  <li>validate it as meaningful,</li>
  <li>preserve that only explicit valid participation contributes to execution meaning,</li>
  <li>avoid collapsing front-panel composition into executable semantics,</li>
  <li>preserve through derivation and lowering that UI presence alone is not executable law.</li>
</ul>

<p>The essential preservation rule is:</p>

<pre><code>front-panel presence
        !=
execution meaning
</code></pre>
