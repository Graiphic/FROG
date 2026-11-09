<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1>Conformance Case — 11 Public Interface Declaration Does Not Require Front-Panel Widget Existence</h1>

<p><strong>Valid conformance case for preserving that public interface declaration does not by itself require corresponding front-panel widget existence in FROG v0.1</strong><br>
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

<p>This conformance case defines a minimal valid program in which a public interface exists and is semantically complete even though no corresponding front-panel widget exists for one or more public ports.</p>

<p>The purpose of this case is to confirm that a conforming FROG toolchain can accept a program where:</p>

<ul>
  <li>a public interface input and output are explicitly declared,</li>
  <li>the executable graph uses those public interface ports directly,</li>
  <li>no front-panel widget is required for those public ports,</li>
  <li>public interface contract remains distinct from front-panel composition.</li>
</ul>

<p>The conceptual shape is:</p>

<pre><code>public interface
  input  x
  output result

front panel
  (optional and unrelated, or absent for x/result)

diagram
  interface_input(x) ----&gt; frog.core.add_constant ----&gt; interface_output(result)
</code></pre>

<p>The required architectural rule is:</p>

<pre><code>public interface declaration
              !=
front-panel widget existence requirement
</code></pre>

<hr>

<h2 id="expected-status">2. Expected Status</h2>

<p><strong>Expected:</strong> valid</p>

<p>A conforming implementation should accept this case because public interface declaration is sufficient to establish the callable program boundary without requiring a corresponding widget on the front panel.</p>

<hr>

<h2 id="illustrative-source-shape">3. Illustrative Source Shape</h2>

<p>This case is defined as a specification-level conformance case.</p>

<p>It is not currently tied to a mandatory published executable slice in <code>Examples/</code>.</p>

<p>An illustrative source shape would include:</p>

<ul>
  <li>one public interface input named <code>x</code>,</li>
  <li>one public interface output named <code>result</code>,</li>
  <li>one diagram-side <code>interface_input</code> participation for <code>x</code>,</li>
  <li>one ordinary primitive that consumes <code>x</code> and a constant, such as <code>frog.core.add</code>,</li>
  <li>one diagram-side <code>interface_output</code> participation for <code>result</code>,</li>
  <li>no requirement that a front-panel widget named <code>x</code> or <code>result</code> exist.</li>
</ul>

<p>The front panel may be:</p>

<ul>
  <li>absent,</li>
  <li>present but unrelated to those public interface ports,</li>
  <li>present with other widgets that do not stand in for the public interface.</li>
</ul>

<p>The minimal participation model is therefore:</p>

<pre><code>public contract may exist on its own
front-panel widget existence is optional and separate
</code></pre>

<hr>

<h2 id="semantic-intent">4. Semantic Intent</h2>

<p>The semantic intent is a normal callable program boundary such as:</p>

<pre><code>result = x + 1
</code></pre>

<p>That intent is established because:</p>

<ul>
  <li><code>x</code> is declared as a public interface input,</li>
  <li>the diagram explicitly consumes <code>x</code> through <code>interface_input</code>,</li>
  <li>the primitive computation is explicit,</li>
  <li><code>result</code> is declared and produced through explicit public output participation.</li>
</ul>

<p>No corresponding front-panel widget is required to make that public contract meaningful.</p>

<p>The semantic rule is therefore:</p>

<pre><code>public interface law
does not depend on
widget existence
</code></pre>

<hr>

<h2 id="boundaries-exercised">5. Boundaries Exercised</h2>

<ul>
  <li>public interface declaration,</li>
  <li>diagram-side <code>interface_input</code> participation,</li>
  <li>diagram-side <code>interface_output</code> participation,</li>
  <li>ordinary primitive execution,</li>
  <li>distinction between public callable contract and front-panel composition,</li>
  <li>distinction between interface existence and UI existence.</li>
</ul>

<p>The key architectural boundary exercised by this case is:</p>

<pre><code>public interface contract
          !=
front-panel widget inventory
</code></pre>

<hr>

<h2 id="why-the-case-is-valid">6. Why the Case Is Valid</h2>

<p>This case is valid because:</p>

<ul>
  <li>the public interface declarations are well formed,</li>
  <li>the diagram-side interface participations are explicit and legal,</li>
  <li>the primitive computation is ordinary executable behavior,</li>
  <li>the graph is acyclic,</li>
  <li>no widget participation is needed to establish the public callable boundary,</li>
  <li>no front-panel object is required in order for the interface to exist as a program contract,</li>
  <li>the architecture explicitly separates public interface meaning from front-panel composition.</li>
</ul>

<p>The essential point is that FROG must allow programs whose public interface is valid even when no UI widget is used to expose or mirror that interface.</p>

<hr>

<h2 id="expected-validation-outcome">7. Expected Validation Outcome</h2>

<p>A conforming validator should accept this case and establish valid language-level meaning.</p>

<p>At minimum, validation should confirm that:</p>

<ul>
  <li>the source is structurally valid canonical FROG source,</li>
  <li>the public interface declarations are valid,</li>
  <li>the diagram-side <code>interface_input</code> and <code>interface_output</code> participations are valid,</li>
  <li>the primitive reference is valid,</li>
  <li>the connected value flow is type-compatible,</li>
  <li>no rule requires corresponding front-panel widgets to exist for public ports,</li>
  <li>the absence of corresponding front-panel widgets does not invalidate the program.</li>
</ul>

<p>The validator should therefore establish a valid program in which:</p>

<pre><code>public interface exists
even when
front-panel widget counterparts do not exist
</code></pre>

<hr>

<h2 id="expected-semantic-preservation">8. Expected Semantic Preservation</h2>

<p>Once validated, the program meaning should preserve at least the following distinctions:</p>

<ul>
  <li>public interface participation remains public interface participation,</li>
  <li>front-panel composition remains optional UI composition,</li>
  <li>absence of widgets does not erase or weaken the public contract,</li>
  <li>the callable program boundary remains defined by interface declaration rather than by UI inventory.</li>
</ul>

<p>The preserved distinction is conceptually:</p>

<pre><code>interface declaration
     does imply
public contract existence

widget existence
     does not imply
or control that contract
</code></pre>

<hr>

<h2 id="expected-ir-and-backend-preservation">9. Expected IR and Backend Preservation</h2>

<p>If execution-facing IR is derived from this case, derivation should preserve explicitly:</p>

<ul>
  <li>public entry participation for <code>x</code>,</li>
  <li>public exit participation for <code>result</code>,</li>
  <li>the primitive execution identity,</li>
  <li>the fact that no widget participation is required for those public ports.</li>
</ul>

<p>If the case is later lowered into a backend contract, that contract should still preserve that:</p>

<ul>
  <li>the callable signature comes from the public interface declaration,</li>
  <li>the callable signature does not depend on front-panel widget existence,</li>
  <li>UI realization remains optional and separate from the public contract.</li>
</ul>

<p>The preservation rule remains:</p>

<pre><code>callable program boundary
        !=
required UI surface
</code></pre>

<hr>

<h2 id="forbidden-reinterpretations">10. Forbidden Reinterpretations</h2>

<p>A conforming implementation must not reinterpret this case in any of the following ways:</p>

<ul>
  <li>as if each public interface input required a corresponding front-panel control,</li>
  <li>as if each public interface output required a corresponding front-panel indicator,</li>
  <li>as if missing widgets invalidated an otherwise valid public interface contract,</li>
  <li>as if interface existence depended on UI composition,</li>
  <li>as if backend convenience could require front-panel widget counterparts while still preserving conformance.</li>
</ul>

<p>The forbidden collapses can be summarized as:</p>

<pre><code>public input declaration   -/-> required front-panel control
public output declaration  -/-> required front-panel indicator
interface law              -/-> UI existence dependency
</code></pre>

<hr>

<h2 id="summary">11. Summary</h2>

<p>This is the baseline valid conformance case for preserving that public interface declaration does not require corresponding front-panel widget existence.</p>

<p>A conforming toolchain should:</p>

<ul>
  <li>accept the case,</li>
  <li>validate it as meaningful,</li>
  <li>preserve that public interface law remains independent from front-panel widget inventory,</li>
  <li>avoid collapsing callable program contract into UI existence requirements.</li>
</ul>

<p>The essential preservation rule is:</p>

<pre><code>public interface declaration
              !=
required front-panel widget existence
</code></pre>
