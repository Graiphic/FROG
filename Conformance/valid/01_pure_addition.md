<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 Conformance Case — 01 Pure Addition</h1>

<p align="center">
  Valid conformance case for the first executable public-interface arithmetic slice in FROG v0.1<br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#case-overview">1. Case Overview</a></li>
  <li><a href="#expected-status">2. Expected Status</a></li>
  <li><a href="#source-target">3. Source Target</a></li>
  <li><a href="#boundaries-exercised">4. Boundaries Exercised</a></li>
  <li><a href="#why-this-case-is-valid">5. Why this Case Is Valid</a></li>
  <li><a href="#expected-validation-outcome">6. Expected Validation Outcome</a></li>
  <li><a href="#expected-derivation-preservation">7. Expected Derivation Preservation</a></li>
  <li><a href="#expected-lowering-and-contract-preservation">8. Expected Lowering and Backend Contract Preservation</a></li>
  <li><a href="#expected-reference-runtime-outcome">9. Expected Reference Runtime Outcome</a></li>
  <li><a href="#what-must-not-happen">10. What Must Not Happen</a></li>
  <li><a href="#summary">11. Summary</a></li>
</ul>

<hr/>

<h2 id="case-overview">1. Case Overview</h2>

<p>
This case covers the smallest useful executable FROG program for the first reference slice:
</p>

<ul>
  <li>two public inputs,</li>
  <li>one ordinary core arithmetic primitive,</li>
  <li>one public output.</li>
</ul>

<p>
The case is intentionally free of UI participation, structured control, and explicit local memory.
It is the baseline valid case for proving that the published architecture can support a complete source-to-execution path without introducing additional concerns too early.
</p>

<hr/>

<h2 id="expected-status">2. Expected Status</h2>

<p><strong>Expected: valid</strong></p>

<p>
A conforming toolchain should accept this source as a valid program for the relevant published slice.
</p>

<hr/>

<h2 id="source-target">3. Source Target</h2>

<p>
Expected source target:
</p>

<pre><code>Examples/01_pure_addition/main.frog</code></pre>

<p>
This conformance case is tied to that published example and should be interpreted against that source.
</p>

<hr/>

<h2 id="boundaries-exercised">4. Boundaries Exercised</h2>

<ul>
  <li>canonical source loading,</li>
  <li>public interface declaration,</li>
  <li>diagram-side <code>interface_input</code> participation,</li>
  <li>diagram-side <code>interface_output</code> participation,</li>
  <li>primitive execution through <code>frog.core.add</code>,</li>
  <li>basic edge validity,</li>
  <li>Execution IR derivation for a minimal executable unit,</li>
  <li>lowering for a first backend family,</li>
  <li>backend-contract emission for a callable execution unit,</li>
  <li>runtime-side consumption of that contract.</li>
</ul>

<hr/>

<h2 id="why-this-case-is-valid">5. Why this Case Is Valid</h2>

<p>
This case is valid because it expresses a simple acyclic executable graph with:
</p>

<ul>
  <li>well-formed public interface declarations,</li>
  <li>valid diagram-side public-boundary participation,</li>
  <li>a valid intrinsic primitive reference,</li>
  <li>valid edge endpoints,</li>
  <li>no required UI capability,</li>
  <li>no required explicit-memory capability.</li>
</ul>

<p>
No additional profile capability is required for this case.
It is intentionally the narrowest published program that still exercises the full architectural chain.
</p>

<hr/>

<h2 id="expected-validation-outcome">6. Expected Validation Outcome</h2>

<p>
A conforming validator should accept the case.
It should confirm at minimum that:
</p>

<ul>
  <li>the file is structurally valid canonical source,</li>
  <li>the required top-level sections are present,</li>
  <li>the interface port declarations are well formed and unambiguous,</li>
  <li>the declared types are valid for the accepted subset,</li>
  <li>the <code>frog.core.add</code> primitive usage is valid,</li>
  <li>the primitive input and output edges are correctly connected,</li>
  <li>the graph is acyclic for this case,</li>
  <li>no UI semantics are required to validate this program,</li>
  <li>no explicit local-memory semantics are required to validate this program.</li>
</ul>

<hr/>

<h2 id="expected-derivation-preservation">7. Expected Derivation Preservation</h2>

<p>
Execution IR derivation should preserve explicitly:
</p>

<ul>
  <li>one execution unit for the whole validated FROG program,</li>
  <li>public entry participation for input <code>a</code>,</li>
  <li>public entry participation for input <code>b</code>,</li>
  <li>primitive execution identity for <code>frog.core.add</code>,</li>
  <li>public exit participation for output <code>result</code>,</li>
  <li>the validated dependency edges between those objects,</li>
  <li>recoverable attribution back to the source boundaries and primitive use.</li>
</ul>

<p>
The derivation should not introduce:
</p>

<ul>
  <li>UI participation,</li>
  <li><code>widget_value</code> objects,</li>
  <li><code>widget_reference</code> objects,</li>
  <li>structured regions,</li>
  <li>explicit memory objects,</li>
  <li>hidden scheduler-private meaning not justified by the source and the published IR boundary.</li>
</ul>

<hr/>

<h2 id="expected-lowering-and-contract-preservation">8. Expected Lowering and Backend Contract Preservation</h2>

<p>
If this case is lowered and emitted as a backend contract, the result should still make clear that:
</p>

<ul>
  <li>one consumable execution unit is being offered,</li>
  <li>the unit has two public inputs and one public output,</li>
  <li>the computation is ordinary arithmetic addition,</li>
  <li>the public-boundary role of the values is preserved,</li>
  <li>no UI participation is required,</li>
  <li>no explicit local-memory semantics are required.</li>
</ul>

<p>
A backend family may specialize argument passing, evaluation realization, or contract shape, but it must not erase the public-boundary origin of the values and must not reinterpret this case as a UI, stateful, or runtime-private-only program.
</p>

<hr/>

<h2 id="expected-reference-runtime-outcome">9. Expected Reference Runtime Outcome</h2>

<p>
A minimal reference implementation for the first executable slice should be able to:
</p>

<ul>
  <li>load this file,</li>
  <li>validate it,</li>
  <li>derive a corresponding Execution IR,</li>
  <li>lower it for the selected backend family,</li>
  <li>emit a backend contract,</li>
  <li>execute it as a callable computation or testable execution unit.</li>
</ul>

<p>
A simple runtime-facing interpretation is:
</p>

<pre><code>result = a + b</code></pre>

<p>
For a concrete test run, a reference implementation may choose example values such as:
</p>

<pre><code>a = 2.25
b = 3.75
result = 6.0</code></pre>

<p>
These values are illustrative test data for the conformance case.
They do not redefine the language semantics.
</p>

<hr/>

<h2 id="what-must-not-happen">10. What Must Not Happen</h2>

<ul>
  <li>The case must not be rejected as if UI support were required.</li>
  <li>The case must not be rejected as if explicit local memory were required.</li>
  <li>The case must not be rewritten into a hidden runtime-private callable form with lost public-boundary attribution.</li>
  <li>The case must not gain implicit state.</li>
  <li>The case must not gain hidden scheduler-private semantics that are not justified by the published layers.</li>
  <li>The case must not be reinterpreted as a widget-driven, object-reference-driven, or structured-control program.</li>
  <li>The implementation must not bypass validation, derivation, lowering, and backend handoff by silently jumping from source directly to private execution while claiming conformance for the whole chain.</li>
</ul>

<hr/>

<h2 id="summary">11. Summary</h2>

<p>
This is the baseline valid conformance case for the first executable public-interface arithmetic slice in FROG v0.1.
A conforming toolchain should accept it, derive it cleanly, preserve its public-boundary and primitive-execution meaning through lowering and backend handoff, and allow a minimal reference runtime to produce the expected arithmetic result.
</p>

<p>
If a toolchain cannot handle this case coherently, it is not ready to move on to UI participation, widget references, or explicit local memory.
</p>
