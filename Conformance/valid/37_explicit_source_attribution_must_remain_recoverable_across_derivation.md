<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 37</h1>

<p align="center">
  <strong>Explicit source attribution must remain recoverable across derivation</strong><br/>
  FROG — Free Open Graphical Language
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#1-case-name">1. Case Name</a></li>
  <li><a href="#2-expected">2. Expected</a></li>
  <li><a href="#3-why">3. Why</a></li>
  <li><a href="#4-boundary-being-exercised">4. Boundary Being Exercised</a></li>
  <li><a href="#5-scenario">5. Scenario</a></li>
  <li><a href="#6-what-makes-the-case-valid">6. What Makes the Case Valid</a></li>
  <li><a href="#7-expected-meaning">7. Expected Meaning</a></li>
  <li><a href="#8-expected-preservation">8. Expected Preservation</a></li>
  <li><a href="#9-what-must-not-happen">9. What Must Not Happen</a></li>
  <li><a href="#10-rationale">10. Rationale</a></li>
  <li><a href="#11-minimum-conformance-reading">11. Minimum Conformance Reading</a></li>
</ul>

<hr/>

<h2 id="1-case-name">1. Case Name</h2>

<p>
  Case:
  <code>37_explicit_source_attribution_must_remain_recoverable_across_derivation</code>
</p>

<hr/>

<h2 id="2-expected">2. Expected</h2>

<p><strong>Expected:</strong> valid</p>

<p><strong>Expected meaning:</strong> language-level meaning is established.</p>

<p>
  <strong>Expected preservation:</strong>
  source-established attribution remains recoverable across derivation into execution-facing forms.
</p>

<hr/>

<h2 id="3-why">3. Why</h2>

<p>
  This case exists to make one architectural rule explicit:
</p>

<pre><code>explicit source attribution
            remains recoverable across
derivation
</code></pre>

<p>
  Derivation may introduce execution-facing identities, scheduling artifacts, lowered forms,
  helper nodes, expansion products, or implementation-oriented decomposition.
</p>

<p>
  But those derived forms must not erase the ability to recover which validated source
  element, boundary, structure, interface participation, or semantic owner they came from.
</p>

<p>
  Recoverability does not require that every downstream layer preserves the exact same
  surface syntax.
  It does require that the attribution link to the semantically relevant source origin is
  not silently destroyed.
</p>

<hr/>

<h2 id="4-boundary-being-exercised">4. Boundary Being Exercised</h2>

<p>
  This case exercises the preservation boundary across:
</p>

<pre><code>canonical .frog source
        |
        v
validated program meaning
        |
        v
derived execution-facing representation
</code></pre>

<p>
  The distinction under test is between:
</p>

<ul>
  <li>recoverable attribution to validated source meaning, and</li>
  <li>derived execution identity that becomes detached from source ownership.</li>
</ul>

<hr/>

<h2 id="5-scenario">5. Scenario</h2>

<p>
  A valid FROG program contains explicit source elements that participate in validated meaning,
  such as interface participation, legal state participation, structure ownership, explicit
  connectivity, or explicit primitive participation.
</p>

<p>
  Derivation then produces an execution-facing representation that may:
</p>

<ul>
  <li>normalize structure,</li>
  <li>expand helper forms,</li>
  <li>introduce execution identifiers,</li>
  <li>materialize internal boundaries,</li>
  <li>decompose one source-level construct into several derived elements.</li>
</ul>

<p>
  The case is valid if the resulting derived form still preserves recoverable attribution back
  to the relevant source-established semantic origin.
</p>

<hr/>

<h2 id="6-what-makes-the-case-valid">6. What Makes the Case Valid</h2>

<p>
  This case is valid because derivation does not become semantic amnesia.
</p>

<p>
  More precisely, it is valid because:
</p>

<ul>
  <li>the source is valid,</li>
  <li>the participating source elements contribute to validated meaning,</li>
  <li>derivation is allowed to transform representation,</li>
  <li>the transformed representation still permits recovery of semantically relevant source attribution.</li>
</ul>

<p>
  The case does not require one universal storage format for attribution metadata.
</p>

<p>
  It does require that attribution is not lost in a way that would break inspection,
  explanation, validation traceability, conformance reasoning, or future tooling.
</p>

<hr/>

<h2 id="7-expected-meaning">7. Expected Meaning</h2>

<p>
  If this case validates, the established meaning includes at least the following:
</p>

<ul>
  <li>validated source meaning remains the upstream owner of semantic truth,</li>
  <li>derived execution-facing forms may introduce new identities,</li>
  <li>those derived identities do not replace source ownership,</li>
  <li>recoverable attribution across derivation remains part of a conforming architectural reading.</li>
</ul>

<p>
  The semantic reading is therefore:
</p>

<pre><code>source-established semantic owner
        survives through
recoverable derivation mapping
</code></pre>

<p>Not:</p>

<pre><code>derived execution identity
        replaces
source attribution
</code></pre>

<hr/>

<h2 id="8-expected-preservation">8. Expected Preservation</h2>

<p>
  A conforming implementation must preserve recoverability of explicit source attribution across derivation.
</p>

<p>
  At minimum, later layers must not silently collapse:
</p>

<ul>
  <li>source-established semantic ownership, into</li>
  <li>opaque derived execution identity with no recoverable origin.</li>
</ul>

<p>
  This does not require every implementation to expose attribution in the same debug UI,
  file field, or internal data structure.
</p>

<p>
  It does require architectural recoverability sufficient to avoid turning derivation into
  an irreversible semantic black box.
</p>

<p>
  In particular:
</p>

<ul>
  <li>derivation must not destroy the ability to trace semantically relevant derived elements back to their validated source origin,</li>
  <li>lowering must not sever attribution ownership that remains necessary for explanation or conformance reasoning,</li>
  <li>implementation convenience must not become an excuse for source-origin loss,</li>
  <li>recoverability must remain compatible with independent implementations and future tooling.</li>
</ul>

<hr/>

<h2 id="9-what-must-not-happen">9. What Must Not Happen</h2>

<p>
  A conforming implementation must not do any of the following:
</p>

<ul>
  <li>replace source attribution with derived identifiers that have no recoverable source owner,</li>
  <li>claim that derivation freedom permits irreversible loss of semantic origin,</li>
  <li>treat execution-facing decomposition as permission to forget validated source ownership,</li>
  <li>collapse recoverable meaning into runtime-only opaque machinery,</li>
  <li>make debugging, inspection, validation explanation, or conformance review depend on hidden implementation knowledge.</li>
</ul>

<p>
  The forbidden collapse is:
</p>

<pre><code>recoverable source attribution
        -/-> unrecoverable derived opacity
</code></pre>

<hr/>

<h2 id="10-rationale">10. Rationale</h2>

<p>
  This distinction matters because FROG is not supposed to hide language truth behind one executor,
  one compiler, or one runtime.
</p>

<p>
  If source attribution stops being recoverable once derivation begins, then:
</p>

<ul>
  <li>validation becomes harder to explain,</li>
  <li>conformance becomes harder to test,</li>
  <li>IR becomes harder to inspect,</li>
  <li>tooling becomes dependent on private implementation knowledge,</li>
  <li>independent implementations become harder to compare.</li>
</ul>

<p>
  Recoverability is therefore not a cosmetic tooling preference.
  It is part of keeping the architectural stack inspectable and standardizable.
</p>

<p>
  FROG must preserve the principle:
</p>

<pre><code>source meaning stays upstream owner
even when derived forms become execution-oriented
</code></pre>

<hr/>

<h2 id="11-minimum-conformance-reading">11. Minimum Conformance Reading</h2>

<p>
  A minimal conforming reading of this case is:
</p>

<ul>
  <li>the source is valid,</li>
  <li>language-level meaning is established,</li>
  <li>derivation may transform representation,</li>
  <li>explicit source attribution must remain recoverable across that derivation,</li>
  <li>derived execution identity must not silently replace source ownership.</li>
</ul>

<p>
  The public truth asserted by this case is:
</p>

<pre><code>explicit source attribution
            must remain recoverable across
derivation
</code></pre>
