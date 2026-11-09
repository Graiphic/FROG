<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">frogc</h1>

<p align="center">
  <strong>Reference pipeline entry point for the first bounded executable corridor in the non-normative FROG reference workspace</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>
<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#non-normative-status">2. Non-Normative Status</a></li>
  <li><a href="#why-frogc-exists">3. Why frogc Exists</a></li>
  <li><a href="#current-closure-target">4. Current Closure Target</a></li>
  <li><a href="#architectural-position">5. Architectural Position</a></li>
  <li><a href="#first-supported-reading">6. First Supported Reading</a></li>
  <li><a href="#published-corridor-anchor">7. Published Corridor Anchor</a></li>
  <li><a href="#recommended-stage-flow">8. Recommended Stage Flow</a></li>
  <li><a href="#minimum-expected-behavior">9. Minimum Expected Behavior</a></li>
  <li><a href="#explicit-non-goals">10. Explicit Non-Goals</a></li>
  <li><a href="#interface-shape">11. Interface Shape</a></li>
  <li><a href="#illustrative-command-posture">12. Illustrative Command Posture</a></li>
  <li><a href="#error-posture">13. Error Posture</a></li>
  <li><a href="#multi-runtime-posture">14. Multi-Runtime Posture</a></li>
  <li><a href="#summary">15. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
<code>frogc</code> is the reference pipeline entry point of the non-normative FROG reference workspace.
Its purpose is to expose one inspectable command-facing route through the currently published bounded executable corridor.
</p>

<p>
In the current published state, <code>frogc</code> should be understood first as the façade for the closed corridor centered on:
</p>

<pre><code>Examples/05_bounded_ui_accumulator/</code></pre>

<p>
It is therefore not merely a generic future compiler name.
It is the first command-facing entry point for a real repository-visible corridor from canonical source to emitted backend contract to reference runtime consumption.
</p>

<hr/>

<h2 id="non-normative-status">2. Non-Normative Status</h2>

<p>
<code>frogc</code> is part of the non-normative reference implementation workspace.
It is not the language definition.
It is not the universal command-line interface for all future FROG implementations.
</p>

<p>
Its role is to orchestrate the reference stages that consume published specification layers without owning them.
That keeps the ownership chain explicit:
</p>

<pre><code>published specification layers
   -&gt; define and bound meaning

frogc
   -&gt; drives a reference-family executable corridor through explicit stages</code></pre>

<hr/>

<h2 id="why-frogc-exists">3. Why frogc Exists</h2>

<p>
A stage-separated reference workspace benefits from one visible orchestration surface.
Without that façade, the repository can still contain all the right stages while remaining harder to inspect as one coherent executable path.
</p>

<p>
<code>frogc</code> therefore exists to:
</p>

<ul>
  <li>load canonical source,</li>
  <li>drive bounded validation,</li>
  <li>drive bounded Execution IR derivation,</li>
  <li>drive backend-family lowering,</li>
  <li>drive backend contract emission,</li>
  <li>and optionally hand the emitted contract to a reference runtime consumer.</li>
</ul>

<p>
For the current closure target, its value is not breadth.
Its value is that one reader can follow one small real corridor through one visible entry point.
</p>

<hr/>

<h2 id="current-closure-target">4. Current Closure Target</h2>

<p>
The current closure target for <code>frogc</code> is the first bounded applicative corridor:
</p>

<pre><code>Examples/05_bounded_ui_accumulator/</code></pre>

<p>
That example is the current anchor because it already combines:
</p>

<ul>
  <li>front-panel value participation,</li>
  <li>minimal object-style UI access,</li>
  <li>bounded structured control,</li>
  <li>explicit state,</li>
  <li>public output publication,</li>
  <li>a published backend contract artifact,</li>
  <li>and published downstream reference runtime consumers.</li>
</ul>

<p>
Accordingly, the first serious reading of <code>frogc</code> is:
</p>

<pre><code>frogc = reference driver for the first closed bounded corridor</code></pre>

<p>
It should not yet be described as a broadly closed front-end for arbitrary FROG programs if the workspace does not publish that broader support.
</p>

<hr/>

<h2 id="architectural-position">5. Architectural Position</h2>

<pre><code>canonical .frog source
      |
      v
Loader
      |
      v
Validator
      |
      v
Deriver
      |
      v
Lowerer
      |
      v
ContractEmitter
      |
      v
Runtime
      ^
      |
    frogc
(reference orchestration entry point)</code></pre>

<p>
The key rule is that <code>frogc</code> orchestrates the stages.
It does not erase them.
It does not collapse lowering, contract emission, and runtime into one hidden action with unclear boundaries.
</p>

<hr/>

<h2 id="first-supported-reading">6. First Supported Reading</h2>

<p>
In the current repository state, the first supported reading of <code>frogc</code> is conservative:
</p>

<ul>
  <li>take one canonical source file for the bounded slice,</li>
  <li>run the reference stages against the supported subset,</li>
  <li>materialize the backend handoff artifact for the selected family,</li>
  <li>and optionally pass that contract to a reference runtime family consumer.</li>
</ul>

<p>
The currently preferred family for that first closed corridor remains:
</p>

<pre><code>reference_host_runtime_ui_binding</code></pre>

<p>
This is the family already used by the published contract artifact and the published reference runtime posture.
</p>

<hr/>

<h2 id="published-corridor-anchor">7. Published Corridor Anchor</h2>

<p>
The first published corridor that <code>frogc</code> should be understood to drive is:
</p>

<pre><code>Examples/05_bounded_ui_accumulator/
      |
      v
Implementations/Reference/ContractEmitter/examples/
05_bounded_ui_accumulator.reference_host_runtime_ui_binding.contract.json
      |
      v
Implementations/Reference/Runtime/</code></pre>

<p>
That matters because <code>frogc</code> is no longer describing a purely hypothetical route.
It can now be read against a source anchor, a published emitted contract, and published runtime-family consumers.
</p>

<hr/>

<h2 id="recommended-stage-flow">8. Recommended Stage Flow</h2>

<p>
The first bounded execution flow for <code>frogc</code> should be read as:
</p>

<ol>
  <li>load <code>Examples/05_bounded_ui_accumulator/main.frog</code>,</li>
  <li>validate the bounded supported subset,</li>
  <li>derive the open execution-facing representation,</li>
  <li>lower for <code>reference_host_runtime_ui_binding</code>,</li>
  <li>emit the backend contract artifact,</li>
  <li>optionally hand the emitted contract to a selected reference runtime realization,</li>
  <li>return explicit success, rejection, or failure with stage attribution.</li>
</ol>

<p>
If the contract is already published and selected directly, a runtime-facing entry may also start at the contract artifact rather than at canonical source.
That does not change the ownership model.
It only changes the entry stage.
</p>

<hr/>

<h2 id="minimum-expected-behavior">9. Minimum Expected Behavior</h2>

<p>
For the current closure target, <code>frogc</code> should be expected to preserve at least:
</p>

<ul>
  <li>the bounded loop identity,</li>
  <li>the exact iteration count of five,</li>
  <li>the explicit local-memory requirement,</li>
  <li>the deterministic initial state of <code>0</code>,</li>
  <li>the sampled numeric control value as input,</li>
  <li>the final public output publication,</li>
  <li>the final indicator publication,</li>
  <li>and the minimal object-style UI property-write surface for <code>face_color</code>.</li>
</ul>

<p>
For this first corridor, the relevant carried value domain is <code>u16</code>.
The reference path driven by <code>frogc</code> should therefore remain coherent with a first bounded scalar surface that includes <code>u16</code>.
</p>

<hr/>

<h2 id="explicit-non-goals">10. Explicit Non-Goals</h2>

<p>
At the current published stage, <code>frogc</code> does not need to claim:
</p>

<ul>
  <li>full-language universal source support,</li>
  <li>all widget families,</li>
  <li>all property / method / event UI interaction forms,</li>
  <li>all backend families,</li>
  <li>all runtime families,</li>
  <li>or universal deployment packaging.</li>
</ul>

<p>
Its current job is narrower and more important:
to make the first closed bounded executable corridor explicit and inspectable.
</p>

<hr/>

<h2 id="interface-shape">11. Interface Shape</h2>

<p>
The exact CLI syntax may evolve.
However, the command-facing contract of <code>frogc</code> should already distinguish clearly between:
</p>

<ul>
  <li>source-oriented entry,</li>
  <li>contract-emission entry,</li>
  <li>and runtime-driving entry where that runtime handoff is explicitly requested.</li>
</ul>

<p>
A conservative conceptual shape is:
</p>

<pre><code>frogc validate &lt;program.frog&gt;
frogc derive-ir &lt;program.frog&gt;
frogc lower --family reference_host_runtime_ui_binding &lt;program.frog&gt;
frogc emit-contract --family reference_host_runtime_ui_binding &lt;program.frog&gt;
frogc run --family reference_host_runtime_ui_binding &lt;program.frog&gt;
frogc run-contract &lt;contract.json&gt;</code></pre>

<p>
This is an interface posture, not a claim that every command is already implemented identically in the published workspace.
The important point is that the stage boundaries remain legible.
</p>

<hr/>

<h2 id="illustrative-command-posture">12. Illustrative Command Posture</h2>

<p>
For the first closed corridor, the most important command-facing readings are:
</p>

<pre><code>frogc emit-contract \
  --family reference_host_runtime_ui_binding \
  Examples/05_bounded_ui_accumulator/main.frog

frogc run \
  --family reference_host_runtime_ui_binding \
  Examples/05_bounded_ui_accumulator/main.frog

frogc run-contract \
  Implementations/Reference/ContractEmitter/examples/05_bounded_ui_accumulator.reference_host_runtime_ui_binding.contract.json</code></pre>

<p>
The third form is especially important for the current published state because the contract artifact is already repository-visible and the runtime-family consumers are already published against that handoff surface.
</p>

<hr/>

<h2 id="error-posture">13. Error Posture</h2>

<p>
<code>frogc</code> should report failures with explicit stage attribution.
A useful minimal model is:
</p>

<ul>
  <li><code>load</code> failure,</li>
  <li><code>validate</code> failure,</li>
  <li><code>derive-ir</code> failure,</li>
  <li><code>lower</code> failure,</li>
  <li><code>emit-contract</code> failure,</li>
  <li><code>runtime-accept</code> rejection,</li>
  <li><code>run</code> failure.</li>
</ul>

<p>
This matters because a closed corridor remains inspectable only if failures remain attributable rather than collapsing into one opaque “compile failed” message.
</p>

<hr/>

<h2 id="multi-runtime-posture">14. Multi-Runtime Posture</h2>

<p>
<code>frogc</code> must not collapse the language into one runtime realization.
For the current corridor, the intended posture is:
</p>

<ul>
  <li>one named source slice,</li>
  <li>one emitted backend contract family,</li>
  <li>one bounded execution meaning,</li>
  <li>and more than one published downstream runtime consumer where available.</li>
</ul>

<p>
For the current published state, that means a Python consumer and a Rust consumer may both remain downstream of the same handoff surface.
<code>frogc</code> should therefore be read as the driver of a corridor that stays open above runtime implementation language.
</p>

<hr/>

<h2 id="summary">15. Summary</h2>

<p>
<code>frogc</code> is the reference orchestration entry point of the non-normative FROG reference workspace.
Its current architectural value is not broad genericity.
Its current value is that it can now be read against one materially closed bounded corridor.
</p>

<p>
That corridor is:
</p>

<pre><code>Examples/05_bounded_ui_accumulator/
  -&gt; ContractEmitter/examples/...contract.json
  -&gt; Runtime/</code></pre>

<p>
Accordingly, <code>frogc</code> should now be described first as the command-facing façade of that first closed source-to-contract-to-runtime reference corridor,
while keeping the stage boundaries and ownership boundaries explicit.
</p>
