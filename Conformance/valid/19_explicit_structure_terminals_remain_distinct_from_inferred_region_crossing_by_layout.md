<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1>Conformance Case — 19 Explicit Structure Terminals Remain Distinct from Inferred Region Crossing by Layout</h1>

<p><strong>Valid conformance case for preserving that structure-terminal usage arises only from explicit structure-terminal declaration and explicit terminal crossing, not from visual line placement or apparent region crossing in FROG v0.1</strong><br>
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

<p>This conformance case defines a minimal valid program in which data crosses a declared structure boundary only through explicit structure-terminal usage, while other nearby wires or nodes may visually appear to cross into or out of a structure region without acquiring structure-terminal semantics.</p>

<p>The purpose of this case is to confirm that a conforming FROG toolchain can accept a program where:</p>

<ul>
  <li>an explicit control structure is declared,</li>
  <li>one or more explicit structure terminals are declared and used,</li>
  <li>actual region crossing occurs only through those explicit terminals,</li>
  <li>other nearby layout elements may appear visually close to the structure boundary,</li>
  <li>visual crossing or apparent penetration of a region boundary does not create hidden structure-terminal usage.</li>
</ul>

<p>The conceptual shape is:</p>

<pre><code>diagram layout

outside value ----&gt; [explicit input terminal] ----&gt; structure region

outside decorative wire path
     visually near the frame
     but not actually crossing through a declared terminal

structure region ----&gt; [explicit output terminal] ----&gt; outside consumer
</code></pre>

<p>The required architectural rule is:</p>

<pre><code>explicit structure terminal usage
                !=
inferred region crossing by layout
</code></pre>

<hr>

<h2 id="expected-status">2. Expected Status</h2>

<p><strong>Expected:</strong> valid</p>

<p>A conforming implementation should accept this case because boundary crossing is fully established by explicit structure-terminal declaration and explicit terminal usage rather than by visual placement.</p>

<hr>

<h2 id="illustrative-source-shape">3. Illustrative Source Shape</h2>

<p>This case is defined as a specification-level conformance case.</p>

<p>It is not currently tied to a mandatory published executable slice in <code>Examples/</code>.</p>

<p>An illustrative source shape would include:</p>

<ul>
  <li>one explicit control structure such as <code>case</code> or <code>while_loop</code>,</li>
  <li>one or more explicit structure input terminals or output terminals,</li>
  <li>an internal region computation that consumes data entering through an explicit terminal,</li>
  <li>an internal or external consumer receiving data exiting through an explicit terminal,</li>
  <li>one additional nearby edge or node visually close to the structure frame but not connected through any declared structure terminal,</li>
  <li>no published rule that allows apparent line crossing or frame proximity to substitute for explicit terminal usage.</li>
</ul>

<p>The minimal participation model is therefore:</p>

<pre><code>explicit structure declaration
      +
explicit structure terminals
      +
explicit terminal crossing
      =
structure-boundary data movement

visual crossing impression
      !=
structure-terminal usage
</code></pre>

<hr>

<h2 id="semantic-intent">4. Semantic Intent</h2>

<p>The semantic intent is that data enters or exits the structure only where explicit structure-terminal participation exists.</p>

<p>This means:</p>

<ul>
  <li>structure boundary crossing is owned by declared terminals,</li>
  <li>internal region values and external values remain distinct unless a terminal explicitly relates them,</li>
  <li>apparent geometric overlap with a structure frame does not create a crossing,</li>
  <li>layout alone does not authorize data movement across a structural boundary.</li>
</ul>

<p>The semantic rule is therefore:</p>

<pre><code>declared terminal crossing
does create
structure-boundary dataflow

apparent visual crossing
does not create
structure-boundary dataflow
</code></pre>

<hr>

<h2 id="boundaries-exercised">5. Boundaries Exercised</h2>

<ul>
  <li>explicit control-structure declaration,</li>
  <li>explicit structure-terminal declaration,</li>
  <li>explicit terminal-mediated region crossing,</li>
  <li>distinction between structural crossing and diagram layout,</li>
  <li>distinction between explicit terminal usage and inferred frame crossing,</li>
  <li>rejection of hidden boundary crossing inferred from visual arrangement.</li>
</ul>

<p>The key architectural boundary exercised by this case is:</p>

<pre><code>declared structure-terminal usage
                !=
visual region crossing impression
</code></pre>

<hr>

<h2 id="why-the-case-is-valid">6. Why the Case Is Valid</h2>

<p>This case is valid because:</p>

<ul>
  <li>the structure is explicitly declared,</li>
  <li>its terminals are explicitly declared,</li>
  <li>all actual boundary crossings occur through those explicit terminals,</li>
  <li>the internal computation uses only explicitly admitted structure inputs,</li>
  <li>the external consumer receives only explicitly emitted structure outputs,</li>
  <li>the nearby visually suggestive layout element remains semantically irrelevant to structure-boundary crossing,</li>
  <li>the case preserves the published rule that structure boundaries are crossed only through explicit terminal law.</li>
</ul>

<p>The essential point is that a line looking as though it touches or passes a frame is not equivalent to a declared structure terminal.</p>

<hr>

<h2 id="expected-validation-outcome">7. Expected Validation Outcome</h2>

<p>A conforming validator should accept this case and establish valid language-level meaning.</p>

<p>At minimum, validation should confirm that:</p>

<ul>
  <li>the source is structurally valid canonical FROG source,</li>
  <li>the structure is validly declared,</li>
  <li>the structure terminals are validly declared and used,</li>
  <li>all admitted cross-boundary value flow occurs through explicit terminals,</li>
  <li>the nearby visually suggestive edge or node is not required to be treated as structure-terminal usage,</li>
  <li>no rule allows frame contact, overlap, or apparent crossing to substitute for explicit terminal crossing.</li>
</ul>

<p>The validator should therefore establish a valid program in which:</p>

<pre><code>explicit terminal usage creates legal boundary crossing
visual frame crossing impression does not
</code></pre>

<hr>

<h2 id="expected-semantic-preservation">8. Expected Semantic Preservation</h2>

<p>Once validated, the program meaning should preserve at least the following distinctions:</p>

<ul>
  <li>declared structure terminals remain the only legal boundary-crossing points,</li>
  <li>internal values remain internal unless explicitly crossed through a terminal,</li>
  <li>external values remain external unless explicitly crossed through a terminal,</li>
  <li>diagram layout remains presentation and recoverability information,</li>
  <li>structure-boundary dataflow remains defined by explicit terminal semantics rather than by visual path placement.</li>
</ul>

<p>The preserved distinction is conceptually:</p>

<pre><code>value connected through terminal
     does imply
legal region crossing

value visually near a frame
     does not imply
legal region crossing
</code></pre>

<hr>

<h2 id="expected-ir-and-backend-preservation">9. Expected IR and Backend Preservation</h2>

<p>If execution-facing IR is derived from this case, derivation should preserve explicitly:</p>

<ul>
  <li>the declared structure identity,</li>
  <li>its explicit terminal identities,</li>
  <li>the internal and external sides of each legal crossing,</li>
  <li>the fact that visually nearby but non-terminal layout elements do not participate in structure-boundary crossing.</li>
</ul>

<p>If the case is later lowered into a backend contract, that contract should still preserve that:</p>

<ul>
  <li>cross-boundary structure dataflow is defined by explicit structure terminals,</li>
  <li>layout metadata may remain useful for authoring or recoverability,</li>
  <li>layout metadata must not create synthetic structure-terminal crossings.</li>
</ul>

<p>The preservation rule remains:</p>

<pre><code>structure terminals
      define
legal region crossing

layout geometry
      does not define
legal region crossing
</code></pre>

<hr>

<h2 id="forbidden-reinterpretations">10. Forbidden Reinterpretations</h2>

<p>A conforming implementation must not reinterpret this case in any of the following ways:</p>

<ul>
  <li>as if a wire visually touching a frame were a structure terminal,</li>
  <li>as if apparent passage across a frame created a legal region crossing,</li>
  <li>as if editor routing or drawing convenience implied terminal semantics,</li>
  <li>as if a visually nearby edge could be promoted into a structure input or output,</li>
  <li>as if backend or editor convenience could promote visual crossing into terminal law while still preserving conformance.</li>
</ul>

<p>The forbidden collapses can be summarized as:</p>

<pre><code>visual frame crossing   -/-> structure terminal usage
line near boundary      -/-> legal region crossing
layout routing          -/-> terminal semantics
apparent crossing       -/-> structure-boundary dataflow
</code></pre>

<hr>

<h2 id="summary">11. Summary</h2>

<p>This is the baseline valid conformance case for preserving that explicit structure terminals remain distinct from inferred region crossing by layout.</p>

<p>A conforming toolchain should:</p>

<ul>
  <li>accept the case,</li>
  <li>validate it as meaningful,</li>
  <li>preserve that structure-boundary crossing occurs only through explicit terminal semantics,</li>
  <li>avoid collapsing visual path geometry into legal region crossing.</li>
</ul>

<p>The essential preservation rule is:</p>

<pre><code>explicit structure terminals
              !=
inferred region crossing by layout
</code></pre>
