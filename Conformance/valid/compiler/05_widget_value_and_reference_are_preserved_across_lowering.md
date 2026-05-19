<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">05 - Widget Value and Reference Are Preserved Across Lowering</h1>

<p align="center">
  <strong>Positive source/FIR/lowering case for preserving widget value flow separately from widget object access</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Case Identifier</h2>

<pre><code>valid/compiler/05_widget_value_and_reference_are_preserved_across_lowering</code></pre>

<h2>Primary Owner Stage</h2>

<pre><code>Primary owner stage: lowering</code></pre>

<p>
This case tests a lowering-preservation obligation after source, semantic validation, and canonical FIR have already accepted the relevant widget participation.
</p>

<h2>Purpose</h2>

<p>
This case verifies that a valid program using both natural widget value flow and object-style widget access can cross lowering without collapsing the two paths.
</p>

<pre><code>widget_value(widget_id)
   !=
widget_reference(widget_id)
   !=
standardized UI-object primitive operation</code></pre>

<h2>Specification Owners</h2>

<ul>
  <li><a href="../../../Expression/Widget.md"><code>Expression/Widget.md</code></a> owns widget instance identity, roles, value-carrying semantics, and the widget-reference model.</li>
  <li><a href="../../../Expression/Widget interaction.md"><code>Expression/Widget interaction.md</code></a> owns diagram-side object-style widget interaction.</li>
  <li><a href="../../../IR/Execution IR.md"><code>IR/Execution IR.md</code></a> owns FIR preservation of <code>widget_value</code>, <code>widget_reference</code>, and UI-object primitive distinctions.</li>
  <li><a href="../../../IR/Lowering.md"><code>IR/Lowering.md</code></a> owns preservation of those distinctions across target-oriented specialization.</li>
  <li><a href="../../../IR/Backend contract.md"><code>IR/Backend contract.md</code></a> owns any consumer-facing UI host or service capability declarations.</li>
</ul>

<h2>Source Scenario</h2>

<p>
The source contains a value-carrying widget whose primary value participates through a <code>widget_value</code> path, and a separate object-style interaction path that targets the same widget instance through <code>widget_reference</code> and a standardized UI primitive such as a property read, property write, or method invocation.
</p>

<p>
The case is not a visual rendering case. It tests execution-facing category preservation.
</p>

<h2>Expected Outcomes</h2>

<pre><code>Expected loadability: loadable
Expected structural validity: valid
Expected semantic acceptance: accepted
Expected IR derivation: derivable
Expected IR schema result: schema-valid
Expected lowering result: lowerable for a UI-capable or host-service-capable posture
Expected backend-contract result: emittable when required UI host/service capabilities are declared
Expected backend-family consumption: consumable by a consumer that supports the declared UI/service boundary</code></pre>

<h2>Expected FIR Features</h2>

<ul>
  <li>the primary value path remains represented as <code>widget_value</code>,</li>
  <li>the object-style path remains represented as <code>widget_reference</code>,</li>
  <li>the UI primitive operation remains distinct from the widget-reference carrier itself,</li>
  <li>member addressing, direction, and typed value participation remain explicit where they affect execution,</li>
  <li>source attribution to the widget instance and addressed member remains recoverable.</li>
</ul>

<h2>Expected Lowering and Contract Features</h2>

<ul>
  <li>lowering may partition pure computation from UI-mediated service calls,</li>
  <li>lowering may introduce host-service call boundaries for object-style interaction,</li>
  <li>lowering must not route object-style widget access through ordinary primary value flow,</li>
  <li>the backend contract declares any required UI host, widget service, or object-interaction capability,</li>
  <li>a consumer may reject if it lacks the declared UI/service capability, but it must not reinterpret the categories silently.</li>
</ul>

<h2>Required Preserved Distinctions</h2>

<ul>
  <li><code>widget_value</code> remains distinct from <code>widget_reference</code>,</li>
  <li><code>widget_reference</code> remains distinct from UI primitive execution objects,</li>
  <li>primary-value participation remains distinct from object-style member access to <code>value</code>,</li>
  <li>front-panel realization remains distinct from the execution-facing preservation obligation.</li>
</ul>

<h2>Case Classification</h2>

<pre><code>Case classification:
- positive
- source/FIR/lowering
- widget value/reference preservation
- UI host capability boundary
- implementation-independent</code></pre>

<h2>Summary</h2>

<p>
A conforming implementation claiming a UI-capable lowering route must preserve the widget value/reference distinction through FIR, lowering, backend contract, and consumption. It may route object-style access through an explicit host-service boundary, but it must not collapse the categories into one generic UI object.
</p>
