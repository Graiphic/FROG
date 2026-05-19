<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Source To FIR Scope Matrix</h1>

<p align="center">
  <strong>Public matrix for deciding which validated <code>.frog</code> source facts become canonical Execution IR / FIR</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This document answers one practical boundary question:
</p>

<pre><code>When a canonical .frog source file is structurally valid and semantically
accepted, which source-owned facts become FIR, which facts remain attribution
or correspondence, and which facts must stay outside primary FIR?</code></pre>

<p>
The short rule is:
</p>

<pre><code>FIR derives from validated program meaning, not from the diagram alone.</code></pre>

<p>
The diagram is the executable backbone, but public interface declarations,
widget value participation, widget reference participation, widget event
participation, explicit state, constants, library calls, and source attribution
also matter when they affect execution-facing meaning.
</p>

<p>
This matrix is not a replacement for <code>Expression/</code>,
<code>Language/</code>, <code>IR/Execution IR.md</code>, or
<code>IR/Derivation rules.md</code>. It is a compact public reading guide that
keeps source-to-FIR decisions consistent while the reference implementation
grows from bounded source-pattern derivation toward broader derivation
coverage.
</p>

<hr/>

<h2>Reading Legend</h2>

<table>
  <thead>
    <tr>
      <th>Classification</th>
      <th>Meaning</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>primary_fir</code></td>
      <td>The validated fact becomes part of the canonical execution-facing FIR object model.</td>
    </tr>
    <tr>
      <td><code>fir_correspondence</code></td>
      <td>The source fact must remain recoverable for attribution, diagnostics, debugging, conformance, or source mapping, but is not itself a primary execution object.</td>
    </tr>
    <tr>
      <td><code>downstream_reference</code></td>
      <td>The source fact may be carried as an explicit reference for later realization, host binding, packaging, or inspection, without becoming FIR execution semantics.</td>
    </tr>
    <tr>
      <td><code>excluded_from_primary_fir</code></td>
      <td>The source fact must not be treated as canonical FIR meaning. It may remain in source, IDE state, realization packages, or downstream artifacts.</td>
    </tr>
    <tr>
      <td><code>deferred</code></td>
      <td>The project has not closed the full public contract yet. The current reference deriver must fail explicitly for unsupported shapes rather than inventing FIR.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Top-Level Source Section Matrix</h2>

<table>
  <thead>
    <tr>
      <th><code>.frog</code> section or field</th>
      <th>Source role</th>
      <th>FIR posture</th>
      <th>Notes</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>spec_version</code></td>
      <td>Source-format compatibility target.</td>
      <td><code>fir_correspondence</code></td>
      <td>Preserve as source compatibility metadata. It must not define execution behavior by itself.</td>
    </tr>
    <tr>
      <td><code>metadata</code></td>
      <td>Program identity, description, authorship, tags, program artifact version, example status, license.</td>
      <td><code>fir_correspondence</code></td>
      <td>Selected identity fields may appear in document metadata or source references. Metadata must never override diagram, interface, or validated meaning.</td>
    </tr>
    <tr>
      <td><code>interface</code></td>
      <td>Public program boundary: inputs, outputs, declared types, defaults where present, and binding origins where present.</td>
      <td><code>primary_fir</code></td>
      <td>Derives to FIR public interface participation, boundary objects, typed ports, and publication obligations.</td>
    </tr>
    <tr>
      <td><code>diagram</code></td>
      <td>Executable dataflow graph, nodes, edges, constants, structures, local state, widget participation nodes, function/library calls.</td>
      <td><code>primary_fir</code></td>
      <td>This is the executable backbone of FIR, but it is not the only source input to derivation.</td>
    </tr>
    <tr>
      <td><code>front_panel</code></td>
      <td>Source-owned front-panel instances, widget ids, layout, bindings, initial values, labels, captions, visual overrides, package references.</td>
      <td><code>mixed</code></td>
      <td>Only execution-facing widget participation derives to primary FIR. Visual layout and realization details stay outside primary FIR.</td>
    </tr>
    <tr>
      <td><code>connector</code></td>
      <td>Connector-pane or interface-facing visual/authoring source surface when present.</td>
      <td><code>fir_correspondence</code> or <code>excluded_from_primary_fir</code></td>
      <td>Public interface meaning derives from <code>interface</code>. Connector visuals should not become execution semantics unless a future source rule gives them explicit executable meaning.</td>
    </tr>
    <tr>
      <td><code>icon</code></td>
      <td>Program icon or authoring/packaging visual identity.</td>
      <td><code>excluded_from_primary_fir</code></td>
      <td>Icon content does not affect execution-facing FIR.</td>
    </tr>
    <tr>
      <td><code>ide</code></td>
      <td>IDE preferences, authoring state, view state, optional source provenance adjuncts where present.</td>
      <td><code>excluded_from_primary_fir</code></td>
      <td>IDE state must not become canonical execution meaning. Source provenance may be handled by source-validation or trust policy, not FIR execution semantics.</td>
    </tr>
    <tr>
      <td><code>cache</code></td>
      <td>Tooling cache for faster load, validation, analysis, derivation, or execution preparation.</td>
      <td><code>excluded_from_primary_fir</code></td>
      <td>Cache can be invalid, stale, or absent. FIR must be derived from validated source meaning, not trusted cache content.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Diagram Source Matrix</h2>

<table>
  <thead>
    <tr>
      <th>Diagram source fact</th>
      <th>FIR posture</th>
      <th>Derivation expectation</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>diagram.nodes[].id</code></td>
      <td><code>fir_correspondence</code> and sometimes <code>primary_fir</code></td>
      <td>Node identity must remain recoverable. FIR may allocate derived execution identities, but source node identity must remain attributable.</td>
    </tr>
    <tr>
      <td><code>interface_input</code> nodes</td>
      <td><code>primary_fir</code></td>
      <td>Derive to public interface boundary participation and typed input availability.</td>
    </tr>
    <tr>
      <td><code>interface_output</code> nodes</td>
      <td><code>primary_fir</code></td>
      <td>Derive to output publication obligations and typed boundary participation.</td>
    </tr>
    <tr>
      <td><code>primitive</code> nodes for intrinsic operators</td>
      <td><code>primary_fir</code></td>
      <td>Preserve intrinsic operation identity, input/output ports, type facts, and source attribution. Example: <code>frog.core.add</code>.</td>
    </tr>
    <tr>
      <td><code>primitive</code> nodes for <code>frog.xxx</code> libraries</td>
      <td><code>primary_fir</code> with <code>deferred</code> details</td>
      <td>Preserve namespaced function identity and typed ports. Provider, capability, status, effect, and dependency posture follows <code>IR/Library call model.md</code>.</td>
    </tr>
    <tr>
      <td><code>constant</code> nodes</td>
      <td><code>primary_fir</code> support value objects</td>
      <td>Constants that feed executable graph meaning must remain explicit, typed, and attributable. The exact canonical constant family remains a source/schema closure item.</td>
    </tr>
    <tr>
      <td>structure nodes or regions</td>
      <td><code>primary_fir</code></td>
      <td>Valid structure families must preserve recoverable structure identity, boundary ports, region relationships, and control/dataflow participation.</td>
    </tr>
    <tr>
      <td><code>for_loop</code> and other loop structures</td>
      <td><code>primary_fir</code></td>
      <td>Derive explicit regions, carried values, iteration facts, loop output semantics, and source attribution. Array auto-indexing and collection outputs remain follow-up closure where not yet standardized.</td>
    </tr>
    <tr>
      <td><code>frog.core.delay</code> or explicit local memory</td>
      <td><code>primary_fir</code></td>
      <td>Derive explicit state objects, initialization, read/write or commit boundaries, and feedback-cycle legality facts.</td>
    </tr>
    <tr>
      <td><code>widget_value</code> nodes</td>
      <td><code>primary_fir</code></td>
      <td>Derive execution-facing widget value participation, widget identity, addressed member where present, value type, direction, and correspondence to the front-panel instance.</td>
    </tr>
    <tr>
      <td><code>widget_event_value</code> nodes</td>
      <td><code>primary_fir</code></td>
      <td>Derive event payload participation, event identity, value type, and source widget attribution when the event affects execution.</td>
    </tr>
    <tr>
      <td><code>widget_reference</code> nodes</td>
      <td><code>primary_fir</code></td>
      <td>Preserve the distinction between object/reference participation and natural widget value participation.</td>
    </tr>
    <tr>
      <td><code>frog.ui.property_read</code>, <code>frog.ui.property_write</code>, <code>frog.ui.method_invoke</code></td>
      <td><code>primary_fir</code></td>
      <td>Derive explicit UI-object operation meaning without collapsing it into hidden runtime behavior or widget value participation.</td>
    </tr>
    <tr>
      <td><code>subFROG</code> or callable program nodes</td>
      <td><code>primary_fir</code> with <code>deferred</code> details</td>
      <td>Callable identity, dependency identity, typed call ports, and attribution must be explicit. Complete callable packaging and dependency closure remain downstream tasks where not yet closed.</td>
    </tr>
    <tr>
      <td><code>diagram.edges[]</code></td>
      <td><code>primary_fir</code></td>
      <td>Derive directed typed connectivity, source/destination endpoint attribution, connection kind, and any required boundary participation.</td>
    </tr>
    <tr>
      <td>diagram layout, comments, annotations, authoring tags</td>
      <td><code>fir_correspondence</code> or <code>excluded_from_primary_fir</code></td>
      <td>May remain available for diagnostics or source mapping. Must not create execution semantics by visual placement alone.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Front Panel Source Matrix</h2>

<table>
  <thead>
    <tr>
      <th>Front-panel source fact</th>
      <th>FIR posture</th>
      <th>Derivation expectation</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>front_panel.canvas</code></td>
      <td><code>excluded_from_primary_fir</code></td>
      <td>Canvas geometry is front-panel source and host realization context. It does not define execution unless a future explicit source rule says otherwise.</td>
    </tr>
    <tr>
      <td><code>front_panel.package_refs</code></td>
      <td><code>downstream_reference</code></td>
      <td>Package references are needed for front-panel realization and validation, but <code>.wfrog</code> package contents are not primary FIR.</td>
    </tr>
    <tr>
      <td><code>widgets[].id</code></td>
      <td><code>fir_correspondence</code> and sometimes <code>primary_fir</code></td>
      <td>Widget identity must be preserved when diagram nodes or interface bindings participate in execution.</td>
    </tr>
    <tr>
      <td><code>widgets[].role</code></td>
      <td><code>primary_fir</code> when execution-facing</td>
      <td>Control/indicator/read-only behavior may matter for I/O direction and validation when the widget participates in execution.</td>
    </tr>
    <tr>
      <td><code>widgets[].class_ref</code></td>
      <td><code>fir_correspondence</code> and validation input</td>
      <td>Class identity constrains valid widget participation and value shape. Class law remains outside primary FIR ownership.</td>
    </tr>
    <tr>
      <td><code>widgets[].value_type</code></td>
      <td><code>primary_fir</code> when bound</td>
      <td>Value type participates in typed widget I/O when referenced by diagram nodes, interface bindings, or executable initial values.</td>
    </tr>
    <tr>
      <td><code>widgets[].binding</code></td>
      <td><code>primary_fir</code> when executable</td>
      <td>Bindings that connect widget values, widget events, or public I/O to the diagram derive to explicit FIR participation.</td>
    </tr>
    <tr>
      <td><code>widgets[].props.value</code>, selected value, initial value, or default value</td>
      <td><code>primary_fir</code> when it initializes executable state or widget I/O</td>
      <td>Initial executable values must be preserved as typed initialization facts. Pure visual defaults remain source/realization facts.</td>
    </tr>
    <tr>
      <td><code>widgets[].layout</code></td>
      <td><code>excluded_from_primary_fir</code></td>
      <td>Layout belongs to source-owned front-panel representation and host realization, not canonical execution meaning.</td>
    </tr>
    <tr>
      <td>labels, captions, text placement, visual style, colors, border width, hover state, pressed state</td>
      <td><code>excluded_from_primary_fir</code></td>
      <td>These remain source-owned front-panel and realization inputs. They must not be hardcoded in runtime and must not become primary FIR semantics.</td>
    </tr>
    <tr>
      <td><code>widgets[].visual.asset_ref</code></td>
      <td><code>downstream_reference</code></td>
      <td>Asset references support realization validation and host rendering. FIR may preserve recoverable references when useful, but SVG assets are not execution semantics.</td>
    </tr>
    <tr>
      <td>widget-specific public parts, anchors, and realization bindings</td>
      <td><code>excluded_from_primary_fir</code> or <code>downstream_reference</code></td>
      <td>These belong to <code>.wfrog</code> and Default realization packages. Runtime/host consumers may use them, but FIR must not absorb SVG structure.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Boundary Rules</h2>

<ul>
  <li>FIR derives from semantically accepted program meaning, not from raw source shape alone.</li>
  <li>FIR must preserve recoverable source attribution for execution-facing objects, support objects, regions, connections, and correspondences.</li>
  <li>Diagram connectivity is execution-relevant, but visual adjacency is not a substitute for an explicit source edge.</li>
  <li>Widget value, widget event, and widget reference participation can be execution-relevant; widget realization geometry is not FIR ownership.</li>
  <li><code>.wfrog</code> packages, Default SVG assets, runtime overlays, host rendering, and IDE view state must remain outside primary FIR.</li>
  <li>Library calls must remain explicit in FIR. Their public identity, effect, provider, capability, and status posture is defined by <code>IR/Library call model.md</code>.</li>
  <li>Unsupported-but-valid source must be reported as unsupported by a bounded implementation subset, not silently reinterpreted as invalid FROG or guessed into private FIR.</li>
</ul>

<hr/>

<h2>Known Open Items</h2>

<p>
The following items must be closed before the project can claim systematic
<code>.frog -&gt; FIR</code> coverage for the full intended v0.1 surface:
</p>

<ul>
  <li>section-level machine-checkable source schemas for <code>metadata</code>, <code>interface</code>, <code>diagram</code>, and <code>front_panel</code>,</li>
  <li>complete semantic validation rules for supported node families, type compatibility, widget participation, structures, and state,</li>
  <li>first-class schema support for FIR call metadata, provider requirements, and document-level dependency summaries,</li>
  <li>record/cluster value model,</li>
  <li>uniform error/status corridor,</li>
  <li>array construction, indexing, loop collection output, and auto-indexing posture,</li>
  <li>explicit conversion primitive posture,</li>
  <li>path typing consistency,</li>
  <li>source-to-FIR conformance matrix covering both accepted and rejected cases.</li>
</ul>

<hr/>

<h2>Summary</h2>

<p>
FIR is not the diagram serialized differently. FIR is the canonical open
execution-facing representation of a validated FROG program. It is derived from
the executable consequences of the source, including public interface,
diagram, widget I/O participation, explicit state, constants, and library
calls. It preserves source attribution, but it does not absorb front-panel
visual realization, SVG geometry, runtime-private behavior, IDE state, or
backend-specific lowering choices.
</p>
