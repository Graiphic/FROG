<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1>Conformance Case — 17 Explicit Structure Boundaries Remain Distinct from Layout Grouping or Nesting</h1>

<p><strong>Valid conformance case for preserving that structure boundaries, region membership, and structure ownership arise only from explicit structure declaration and not from visual grouping, framing, enclosure, or apparent nesting in FROG v0.1</strong><br>
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

<p>This conformance case defines a minimal valid program in which one explicit control structure exists while other nearby nodes may appear visually grouped, framed, enclosed, or nested in relation to that structure without becoming members of it unless canonical source explicitly assigns them to the structure boundary or one of its regions.</p>

<p>The purpose of this case is to confirm that a conforming FROG toolchain can accept a program where:</p>

<ul>
  <li>one explicit structure such as <code>case</code>, <code>for_loop</code>, or <code>while_loop</code> is declared in canonical source,</li>
  <li>its boundary, regions, and structure-terminal participation are explicit,</li>
  <li>one or more nearby nodes may look visually associated with the structure,</li>
  <li>only explicit structural membership creates structure semantics,</li>
  <li>visual grouping, framing, enclosure, or apparent nesting does not create hidden structure membership.</li>
</ul>

<p>The conceptual shape is:</p>

<pre><code>diagram layout

  +--------------------------------------+
  | explicit case structure              |
  |                                      |
  |   region A: frog.core.add            |
  |   region B: frog.core.multiply       |
  |                                      |
  +--------------------------------------+

        frog.core.subtract
        (nearby, visually related,
         but not structurally inside)
</code></pre>

<p>In this case, the nearby <code>frog.core.subtract</code> node remains outside the structure unless canonical source explicitly places it inside a declared structure region or boundary participation role.</p>

<p>The required architectural rule is:</p>

<pre><code>explicit structure boundary
            !=
layout grouping or apparent nesting
</code></pre>

<hr>

<h2 id="expected-status">2. Expected Status</h2>

<p><strong>Expected:</strong> valid</p>

<p>A conforming implementation should accept this case because the structural meaning is fully established by explicit structure declaration and explicit region membership rather than by visual grouping or enclosure.</p>

<hr>

<h2 id="illustrative-source-shape">3. Illustrative Source Shape</h2>

<p>This case is defined as a specification-level conformance case.</p>

<p>It is not currently tied to a mandatory published executable slice in <code>Examples/</code>.</p>

<p>An illustrative source shape would include:</p>

<ul>
  <li>one explicit control structure such as <code>case</code>,</li>
  <li>explicit declared regions for that structure,</li>
  <li>one or more primitives explicitly placed inside those declared regions,</li>
  <li>explicit structure terminals where required,</li>
  <li>one additional primitive visually near the structure frame but not declared as a member of any structure region,</li>
  <li>no published rule that allows apparent nesting, frame overlap, enclosure, or editor grouping to substitute for explicit structure membership.</li>
</ul>

<p>The minimal participation model is therefore:</p>

<pre><code>explicit structure declaration
      +
explicit region membership
      +
explicit boundary participation
      =
structure semantics

visual grouping or enclosure
      !=
structure membership
</code></pre>

<hr>

<h2 id="semantic-intent">4. Semantic Intent</h2>

<p>The semantic intent is that the explicit structure alone defines:</p>

<ul>
  <li>which nodes belong to its regions,</li>
  <li>which terminals belong to its boundary,</li>
  <li>which execution alternatives, iterations, or region-local rules are governed by the structure,</li>
  <li>which nearby nodes remain outside the structure.</li>
</ul>

<p>A visually nearby node does not enter the structure merely because it appears inside a rough graphical area, near a frame, inside a drawing box, or visually associated with the structure.</p>

<p>The semantic rule is therefore:</p>

<pre><code>declared structural membership
does create
structure semantics

apparent grouping or enclosure
does not create
structure semantics
</code></pre>

<hr>

<h2 id="boundaries-exercised">5. Boundaries Exercised</h2>

<ul>
  <li>explicit control-structure declaration,</li>
  <li>explicit region membership,</li>
  <li>explicit structure-terminal participation,</li>
  <li>distinction between structural boundary and diagram layout,</li>
  <li>distinction between structure membership and graphical grouping,</li>
  <li>distinction between declared nesting and apparent nesting,</li>
  <li>rejection of hidden structure law inferred from visual arrangement.</li>
</ul>

<p>The key architectural boundary exercised by this case is:</p>

<pre><code>declared structure membership
            !=
visual grouping, enclosure, or framing
</code></pre>

<hr>

<h2 id="why-the-case-is-valid">6. Why the Case Is Valid</h2>

<p>This case is valid because:</p>

<ul>
  <li>the structure is explicitly declared in canonical source,</li>
  <li>its regions are explicitly defined,</li>
  <li>its participating internal nodes are explicitly placed within those regions,</li>
  <li>its structure ownership is determined by declaration rather than by layout,</li>
  <li>the nearby external node remains outside the structure because no explicit structural membership places it inside,</li>
  <li>the case preserves the published rule that structures are semantic objects with explicit boundaries and explicit membership, not editor-drawn grouping hints,</li>
  <li>the program remains meaningful without inventing hidden nesting from layout.</li>
</ul>

<p>The essential point is that visual grouping may help readability and recoverability, but it does not create structure ownership, region membership, or boundary law.</p>

<hr>

<h2 id="expected-validation-outcome">7. Expected Validation Outcome</h2>

<p>A conforming validator should accept this case and establish valid language-level meaning.</p>

<p>At minimum, validation should confirm that:</p>

<ul>
  <li>the source is structurally valid canonical FROG source,</li>
  <li>the control structure is validly declared,</li>
  <li>the participating internal nodes are validly placed within explicit structure regions,</li>
  <li>the structure terminals and boundary participations are valid where used,</li>
  <li>the nearby external node is not required to be treated as part of the structure,</li>
  <li>no rule allows layout grouping, enclosure, frame overlap, or apparent nesting to substitute for explicit structure membership.</li>
</ul>

<p>The validator should therefore establish a valid program in which:</p>

<pre><code>explicit structure membership creates structure law
visual proximity to a structure does not
</code></pre>

<hr>

<h2 id="expected-semantic-preservation">8. Expected Semantic Preservation</h2>

<p>Once validated, the program meaning should preserve at least the following distinctions:</p>

<ul>
  <li>the declared structure remains the sole owner of its regions and terminals,</li>
  <li>internal nodes remain internal because of explicit membership,</li>
  <li>external nearby nodes remain external,</li>
  <li>diagram layout remains presentation and recoverability information,</li>
  <li>structure law remains defined by explicit structural boundaries rather than by visual grouping or apparent enclosure.</li>
</ul>

<p>The preserved distinction is conceptually:</p>

<pre><code>node explicitly in region
     does imply
structure membership

node visually near region
     does not imply
structure membership
</code></pre>

<hr>

<h2 id="expected-ir-and-backend-preservation">9. Expected IR and Backend Preservation</h2>

<p>If execution-facing IR is derived from this case, derivation should preserve explicitly:</p>

<ul>
  <li>the declared structure identity,</li>
  <li>its region membership,</li>
  <li>its structure-terminal participation,</li>
  <li>the fact that nearby but external nodes remain outside the structure.</li>
</ul>

<p>If the case is later lowered into a backend contract, that contract should still preserve that:</p>

<ul>
  <li>structural control semantics come from explicit structure ownership and explicit region membership,</li>
  <li>layout, framing, or grouping metadata may remain useful for authoring or recoverability,</li>
  <li>layout, framing, or grouping metadata must not create structure boundaries or structure membership.</li>
</ul>

<p>The preservation rule remains:</p>

<pre><code>structure declaration
      defines
structure boundary

layout grouping
      does not define
structure boundary
</code></pre>

<hr>

<h2 id="forbidden-reinterpretations">10. Forbidden Reinterpretations</h2>

<p>A conforming implementation must not reinterpret this case in any of the following ways:</p>

<ul>
  <li>as if a nearby node were part of the structure because it appears visually enclosed,</li>
  <li>as if graphical framing or editor grouping implied region membership,</li>
  <li>as if apparent nesting created hidden structure ownership,</li>
  <li>as if layout overlap or boundary proximity were enough to move a node inside a structure,</li>
  <li>as if editor convenience could promote grouping or enclosure into structure law while still preserving conformance.</li>
</ul>

<p>The forbidden collapses can be summarized as:</p>

<pre><code>visual frame        -/-> structure boundary
apparent nesting    -/-> structure membership
grouped layout      -/-> region ownership
nearby node         -/-> internal structure node
</code></pre>

<hr>

<h2 id="summary">11. Summary</h2>

<p>This is the baseline valid conformance case for preserving that explicit structure boundaries remain distinct from layout grouping or nesting.</p>

<p>A conforming toolchain should:</p>

<ul>
  <li>accept the case,</li>
  <li>validate it as meaningful,</li>
  <li>preserve that structure semantics come from explicit structural declaration,</li>
  <li>avoid collapsing layout grouping, enclosure, or apparent nesting into structure law.</li>
</ul>

<p>The essential preservation rule is:</p>

<pre><code>explicit structure boundaries
              !=
layout grouping or nesting
</code></pre>
