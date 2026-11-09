<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1>Conformance Case — 07 Widget Reference Remains Distinct from Widget Value</h1>

<p><strong>Valid conformance case for preserving the distinction between <code>widget_reference</code> and <code>widget_value</code> in FROG v0.1</strong><br>
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

<p>This conformance case defines a minimal valid program shape in which a widget object reference is used in a UI-authorized context while remaining semantically distinct from the widget’s ordinary primary value.</p>

<p>The purpose of the case is not to introduce a general object model. The purpose is to prove that a conforming FROG toolchain can accept a program in which:</p>

<ul>
  <li>a widget’s primary value remains available through <code>widget_value</code>,</li>
  <li>the widget object itself is available through <code>widget_reference</code>,</li>
  <li>the object reference is consumed only by a standardized UI primitive that is defined to accept such a reference,</li>
  <li>the two participation categories remain distinct throughout validation and derivation.</li>
</ul>

<p>A useful conceptual shape is:</p>

<pre><code>widget object ----------------------&gt; ui primitive expecting widget_reference
      slider_ref                                 |
                                                 v
widget primary value ----------&gt; ordinary computation or display path
      slider_value
</code></pre>

<p>The required preservation rule is:</p>

<pre><code>widget_reference(slider) != widget_value(slider)
</code></pre>

<hr>

<h2 id="expected-status">2. Expected Status</h2>

<p><strong>Expected:</strong> valid</p>

<p>A conforming implementation should accept this case when the widget reference is used only in a published UI-authorized context.</p>

<hr>

<h2 id="illustrative-source-shape">3. Illustrative Source Shape</h2>

<p>This case is defined as a specification-level conformance case.</p>

<p>It is not currently tied to a mandatory published executable slice in <code>Examples/</code>.</p>

<p>An illustrative source shape would include:</p>

<ul>
  <li>one front-panel widget named <code>slider</code>,</li>
  <li>one diagram-side <code>widget_value</code> participation node for <code>slider</code>,</li>
  <li>one diagram-side <code>widget_reference</code> participation node for <code>slider</code>,</li>
  <li>one standardized UI primitive that explicitly accepts a widget reference,</li>
  <li>optionally, one ordinary value-flow path that uses the widget primary value without confusing it with the object reference.</li>
</ul>

<p>The minimal participation model is therefore:</p>

<pre><code>widget object reference   -&gt; UI-authorized primitive
widget primary value      -&gt; ordinary value-flow participation
</code></pre>

<p>The case remains valid only if the same widget may contribute both categories without collapse.</p>

<hr>

<h2 id="semantic-intent">4. Semantic Intent</h2>

<p>The semantic intent is to preserve two different kinds of program participation originating from one declared widget:</p>

<ul>
  <li>its primary value as a data value,</li>
  <li>its object identity as a UI reference.</li>
</ul>

<p>Those are not interchangeable.</p>

<p>The primary value may participate in ordinary value flow.<br>
The reference may participate only in standardized UI-object operations that are explicitly defined to consume a widget reference.</p>

<p>The semantic rule is:</p>

<pre><code>same widget origin
does not imply
same semantic participation category
</code></pre>

<hr>

<h2 id="boundaries-exercised">5. Boundaries Exercised</h2>

<ul>
  <li>front-panel widget declaration,</li>
  <li><code>widget_value</code> participation,</li>
  <li><code>widget_reference</code> participation,</li>
  <li>standardized UI primitive usage,</li>
  <li>type and participation legality for object-style UI interaction,</li>
  <li>preservation of the distinction between object reference participation and value participation.</li>
</ul>

<p>The key architectural boundary exercised by this case is:</p>

<pre><code>widget_value
     !=
widget_reference
</code></pre>

<hr>

<h2 id="why-the-case-is-valid">6. Why the Case Is Valid</h2>

<p>This case is valid because:</p>

<ul>
  <li>the widget declaration is well formed,</li>
  <li>the diagram-side <code>widget_value</code> participation refers to an actual value-carrying widget,</li>
  <li>the diagram-side <code>widget_reference</code> participation refers to an actual widget object in a context where such participation is allowed,</li>
  <li>the consuming UI primitive is standardized and explicitly defined to accept a widget reference,</li>
  <li>the case does not require the reference to be reinterpreted as a value,</li>
  <li>the case does not require the value to be reinterpreted as an object reference,</li>
  <li>the case does not require implicit state,</li>
  <li>the case does not require hidden event semantics,</li>
  <li>the case remains meaningful while preserving the published distinction between the two categories.</li>
</ul>

<p>The essential point is that the program stays valid only because the consuming operation is a valid UI-reference context rather than an ordinary value-flow context.</p>

<hr>

<h2 id="expected-validation-outcome">7. Expected Validation Outcome</h2>

<p>A conforming validator should accept this case and establish valid language-level meaning.</p>

<p>At minimum, validation should confirm that:</p>

<ul>
  <li>the source is structurally valid canonical FROG source,</li>
  <li>the widget declaration is valid,</li>
  <li>the widget supports ordinary primary value participation,</li>
  <li>the diagram-side <code>widget_value</code> node refers to an actual widget,</li>
  <li>the diagram-side <code>widget_reference</code> node refers to an actual widget,</li>
  <li>the consuming UI primitive is a valid standardized UI primitive,</li>
  <li>that primitive explicitly accepts widget-reference participation,</li>
  <li>the case does not require <code>widget_reference</code> to masquerade as a primitive scalar value,</li>
  <li>the case does not require <code>widget_value</code> to masquerade as a UI object handle,</li>
  <li>all connected participations are legal for their declared categories.</li>
</ul>

<p>The validation result should therefore establish a valid program in which:</p>

<pre><code>one widget
may contribute
both a value
and a reference
while those remain distinct
</code></pre>

<hr>

<h2 id="expected-semantic-preservation">8. Expected Semantic Preservation</h2>

<p>Once validated, the program meaning should preserve at least the following distinctions:</p>

<ul>
  <li><code>widget_value(slider)</code> remains ordinary widget-owned value participation,</li>
  <li><code>widget_reference(slider)</code> remains widget-object reference participation,</li>
  <li>the consuming UI primitive remains a reference-consuming UI operation rather than an ordinary arithmetic or scalar operation,</li>
  <li>the two outgoing semantic roles remain attributable to different participation categories even when they originate from the same widget declaration.</li>
</ul>

<p>The preserved distinction is conceptually:</p>

<pre><code>source-side role A: widget_value(slider)
source-side role B: widget_reference(slider)

A != B
A may feed value-oriented logic
B may feed UI-reference-oriented logic
</code></pre>

<hr>

<h2 id="expected-ir-and-backend-preservation">9. Expected IR and Backend Preservation</h2>

<p>If execution-facing IR is derived from this case, derivation should preserve explicitly:</p>

<ul>
  <li>that one participation is widget primary value flow,</li>
  <li>that one participation is widget object reference flow,</li>
  <li>that the reference-consuming operation is a UI-authorized primitive rather than an ordinary value primitive,</li>
  <li>that recoverable attribution still distinguishes the two categories.</li>
</ul>

<p>If the case is later lowered into a backend contract, that contract should still preserve that:</p>

<ul>
  <li>one path carries value participation semantics,</li>
  <li>one path carries widget-reference participation semantics,</li>
  <li>the backend may specialize representation details,</li>
  <li>the backend must not erase the distinction between a widget’s value and the widget object itself.</li>
</ul>

<p>The preservation rule remains:</p>

<pre><code>value contribution
      !=
object reference contribution
</code></pre>

<hr>

<h2 id="forbidden-reinterpretations">10. Forbidden Reinterpretations</h2>

<p>A conforming implementation must not reinterpret this case in any of the following ways:</p>

<ul>
  <li>as if <code>widget_reference</code> were merely another spelling of <code>widget_value</code>,</li>
  <li>as if a widget object reference were an ordinary scalar value,</li>
  <li>as if a widget primary value were sufficient wherever a widget object reference is required,</li>
  <li>as if any primitive could consume a widget reference without being explicitly defined to do so,</li>
  <li>as if the difference between value participation and object participation were only an implementation detail,</li>
  <li>as if backend convenience could erase the distinction while still preserving conformance.</li>
</ul>

<p>The forbidden collapses can be summarized as:</p>

<pre><code>widget_reference   -/-> widget_value
widget_value       -/-> widget_reference
object reference   -/-> scalar value
scalar value       -/-> UI object handle
</code></pre>

<hr>

<h2 id="summary">11. Summary</h2>

<p>This is the baseline valid conformance case for preserving the distinction between:</p>

<ul>
  <li>a widget’s primary value participation, and</li>
  <li>a widget’s object reference participation.</li>
</ul>

<p>A conforming toolchain should:</p>

<ul>
  <li>accept the case,</li>
  <li>validate it as meaningful,</li>
  <li>preserve the distinction between <code>widget_value</code> and <code>widget_reference</code>,</li>
  <li>derive execution-facing representation without collapsing those categories,</li>
  <li>preserve through lowering and backend handoff that value participation and reference participation remain semantically distinct.</li>
</ul>

<p>The essential preservation rule is:</p>

<pre><code>widget_value
     !=
widget_reference
</code></pre>
