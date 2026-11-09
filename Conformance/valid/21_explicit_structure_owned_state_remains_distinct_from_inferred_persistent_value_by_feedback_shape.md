<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1>Conformance Case — 21 Explicit Structure-Owned State Remains Distinct from Inferred Persistent Value by Feedback Shape</h1>

<p><strong>Valid conformance case for preserving that persistent structure-owned state arises only from explicit state declaration and explicit state participation, not from a feedback-shaped layout in FROG v0.1</strong><br>
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

<p>This conformance case defines a minimal valid program in which a control structure owns an explicit persistent state across repeated execution, while the diagram may also contain a feedback-shaped visual arrangement whose meaning does not create persistence unless explicit state ownership has been declared.</p>

<p>The purpose of this case is to confirm that a conforming FROG toolchain can accept a program where:</p>

<ul>
  <li>an explicit iterative or state-owning structure is declared,</li>
  <li>the structure explicitly owns a persistent value across repeated execution steps,</li>
  <li>state entry, state update, and state re-use are explicit,</li>
  <li>the diagram may visually resemble a feedback loop,</li>
  <li>persistent meaning comes from explicit structure-owned state rather than from feedback-shaped drawing alone.</li>
</ul>

<p>The conceptual shape is:</p>

<pre><code>structure iteration n

explicit previous-state input
            |
            v
        frog.core.add  &lt;---- current input
            |
            +----&gt; explicit next-state output
            |
            +----&gt; public result

state persistence exists
because the structure explicitly owns the carried value
</code></pre>

<p>The required architectural rule is:</p>

<pre><code>explicit structure-owned state
              !=
inferred persistent value by feedback shape
</code></pre>

<hr>

<h2 id="expected-status">2. Expected Status</h2>

<p><strong>Expected:</strong> valid</p>

<p>A conforming implementation should accept this case because persistence is established by explicit state ownership and explicit state transition participation.</p>

<hr>

<h2 id="illustrative-source-shape">3. Illustrative Source Shape</h2>

<p>This case is defined as a specification-level conformance case.</p>

<p>It is not currently tied to a mandatory published executable slice in <code>Examples/</code>.</p>

<p>An illustrative source shape would include:</p>

<ul>
  <li>one explicit iterative or state-owning structure such as <code>while_loop</code>,</li>
  <li>one explicit structure-owned state slot or equivalent explicit state participation,</li>
  <li>one explicit initial value for that state where required,</li>
  <li>one internal computation consuming both the current external input and the previous state value,</li>
  <li>one explicit next-state value fed back through the declared state-owning mechanism,</li>
  <li>one externally visible result,</li>
  <li>no requirement to infer persistence merely from the drawn shape of an internal feedback path.</li>
</ul>

<p>The minimal participation model is therefore:</p>

<pre><code>explicit state ownership
      +
explicit prior-state participation
      +
explicit next-state participation
      =
persistent structure state

feedback-like drawing
      !=
persistent structure state
</code></pre>

<hr>

<h2 id="semantic-intent">4. Semantic Intent</h2>

<p>The semantic intent is that the structure owns a value that persists from one execution step to the next because the language-level state mechanism explicitly says so.</p>

<p>This means:</p>

<ul>
  <li>the previous state value is available because it belongs to the structure-owned state,</li>
  <li>the next state value becomes persistent because it is explicitly committed to that same state ownership path,</li>
  <li>the result may depend on both the current input and the previous state,</li>
  <li>the visible wire geometry alone does not create or justify persistence.</li>
</ul>

<p>The semantic rule is therefore:</p>

<pre><code>explicit state mechanism
does create
persistent value across steps

feedback shape alone
does not create
persistent value across steps
</code></pre>

<hr>

<h2 id="boundaries-exercised">5. Boundaries Exercised</h2>

<ul>
  <li>explicit structure-owned state,</li>
  <li>explicit previous-state participation,</li>
  <li>explicit next-state participation,</li>
  <li>distinction between legal persistent state and drawn feedback shape,</li>
  <li>distinction between state semantics and layout suggestion,</li>
  <li>rejection of hidden persistence inferred from geometry.</li>
</ul>

<p>The key architectural boundary exercised by this case is:</p>

<pre><code>declared persistent structure state
                !=
feedback-shaped drawing
</code></pre>

<hr>

<h2 id="why-the-case-is-valid">6. Why the Case Is Valid</h2>

<p>This case is valid because:</p>

<ul>
  <li>the structure is explicitly declared,</li>
  <li>the state ownership is explicit,</li>
  <li>the prior-state and next-state roles are explicit,</li>
  <li>the state update path is semantically well formed,</li>
  <li>the program does not rely on hidden persistence inferred from feedback geometry,</li>
  <li>the case preserves the published rule that persistent execution meaning requires explicit state semantics rather than visual shape.</li>
</ul>

<p>The essential point is that persistence exists here because the source explicitly declares state ownership, not because a wire appears to loop back.</p>

<hr>

<h2 id="expected-validation-outcome">7. Expected Validation Outcome</h2>

<p>A conforming validator should accept this case and establish valid language-level meaning.</p>

<p>At minimum, validation should confirm that:</p>

<ul>
  <li>the source is structurally valid canonical FROG source,</li>
  <li>the structure is validly declared,</li>
  <li>the state-owning mechanism is validly declared,</li>
  <li>the prior-state and next-state participations are validly formed,</li>
  <li>the state update path is type-compatible and semantically legal,</li>
  <li>the visible result path is legal,</li>
  <li>no rule requires persistence to be inferred from feedback-shaped geometry.</li>
</ul>

<p>The validator should therefore establish a valid program in which:</p>

<pre><code>explicit state ownership creates legal persistence
feedback-shaped layout alone does not
</code></pre>

<hr>

<h2 id="expected-semantic-preservation">8. Expected Semantic Preservation</h2>

<p>Once validated, the program meaning should preserve at least the following distinctions:</p>

<ul>
  <li>the structure-owned state remains explicit state,</li>
  <li>the prior-state value remains recoverably distinct from ordinary current-step values,</li>
  <li>the next-state commitment remains explicit,</li>
  <li>diagram layout remains presentation and recoverability information,</li>
  <li>persistent execution meaning remains defined by explicit state law rather than by feedback-shaped geometry.</li>
</ul>

<p>The preserved distinction is conceptually:</p>

<pre><code>value admitted through explicit state path
     does imply
persistent state participation

value placed on a feedback-looking wire
     does not imply
persistent state participation
</code></pre>

<hr>

<h2 id="expected-ir-and-backend-preservation">9. Expected IR and Backend Preservation</h2>

<p>If execution-facing IR is derived from this case, derivation should preserve explicitly:</p>

<ul>
  <li>the declared structure identity,</li>
  <li>the explicit state-owning mechanism,</li>
  <li>the distinction between current-step values and persistent state values,</li>
  <li>the explicit prior-state to next-state transition,</li>
  <li>the fact that persistence arises from state semantics rather than from drawn feedback geometry.</li>
</ul>

<p>If the case is later lowered into a backend contract, that contract should still preserve that:</p>

<ul>
  <li>persistent state is carried by an explicit semantic state mechanism,</li>
  <li>layout metadata may remain useful for authoring or recoverability,</li>
  <li>layout metadata must not create synthetic persistence.</li>
</ul>

<p>The preservation rule remains:</p>

<pre><code>explicit state mechanism
      defines
persistent value across steps

feedback shape
      does not define
persistent value across steps
</code></pre>

<hr>

<h2 id="forbidden-reinterpretations">10. Forbidden Reinterpretations</h2>

<p>A conforming implementation must not reinterpret this case in any of the following ways:</p>

<ul>
  <li>as if the feedback-like drawn path itself were the source of persistence,</li>
  <li>as if any loop-shaped wire implied persistent storage,</li>
  <li>as if drawn re-entry into earlier computation automatically created prior-step semantics,</li>
  <li>as if editor convenience could promote feedback geometry into state law,</li>
  <li>as if backend convenience could collapse explicit state ownership into a merely graphical loop while still preserving conformance.</li>
</ul>

<p>The forbidden collapses can be summarized as:</p>

<pre><code>feedback shape        -/-> persistent state
loop-like wire        -/-> prior-step value
drawn return path     -/-> state ownership
layout suggestion     -/-> memory semantics
</code></pre>

<hr>

<h2 id="summary">11. Summary</h2>

<p>This is the baseline valid conformance case for preserving that explicit structure-owned state remains distinct from inferred persistent value by feedback shape.</p>

<p>A conforming toolchain should:</p>

<ul>
  <li>accept the case,</li>
  <li>validate it as meaningful,</li>
  <li>preserve that persistence comes from explicit state semantics,</li>
  <li>avoid collapsing feedback-shaped layout into state ownership.</li>
</ul>

<p>The essential preservation rule is:</p>

<pre><code>explicit structure-owned state
              !=
inferred persistent value by feedback shape
</code></pre>
