<h1>Widget Example Public Boundary</h1>

<p>
This public repository exposes the specification-facing boundary for widget
examples. It does not publish Graiphic's internal acceptance recipe, visual QA
procedure, release checklist, or private implementation roadmap.
</p>

<p>
Widget examples remain subordinate to the owning specification documents,
schemas, realization packages, conformance cases, and reference checks. They are
not semantic owners of FROG.
</p>

<h2>Public Ownership Boundary</h2>

<ul>
  <li><code>.frog</code> owns diagram logic, front-panel widget instances, widget ids, bindings, initial values, labels, instance layout, and instance-level visual overrides.</li>
  <li><code>.wfrog</code> packages publish realization references, Default asset ids, supported classes, and host capability declarations. They are not example-specific front-panel source.</li>
  <li>Default realization assets own reusable SVG templates, public parts, public anchors, public bindings, internal geometry, and default visual behavior.</li>
  <li>Runtime implementations consume validated source-derived artifacts, backend contracts, native manifests, ABI surfaces, <code>.wfrog</code> packages, and published realization assets. They do not redefine the widget class law.</li>
  <li>A visible skin surface should have one public owner. When a Default SVG part or realization resource owns a border, fill, popup panel, row face, selection face, or equivalent visual surface, a runtime host may position and bind that surface, but should not recreate the same surface as an independent host-only border or fill.</li>
</ul>

<h2>Public Fidelity Expectation</h2>

<p>
A published widget example should make the source / realization / runtime
boundary inspectable. A visible front panel should not be presented as a valid
published widget example if it is only a handcrafted runtime shell unrelated to
the declared <code>.frog</code>, <code>.wfrog</code>, and Default realization assets.
</p>

<p>
The public expectation is architectural, not a Graiphic internal QA checklist:
</p>

<ul>
  <li>front-panel intent is visible in source-owned data,</li>
  <li>realization references resolve through published <code>.wfrog</code> packages,</li>
  <li>Default SVG assets expose public parts, anchors, bindings, or equivalent published markers where the realization requires them,</li>
  <li>host rendering preserves realization ownership of visible skin surfaces rather than layering independent duplicate borders or fills over the same part,</li>
  <li>runtime snapshots and diagnostics remain supporting evidence rather than the user-facing widget itself,</li>
  <li>native-backed examples consume manifests and ABI artifacts rather than treating LLVM as the runtime identity.</li>
</ul>

<h2>Reference Runtime Boundary</h2>

<p>
Examples <code>01</code> through <code>15</code> define the current public reference runtime closure.
Examples beyond that boundary may remain public as specification-facing,
widget-facing, conformance-facing, or design-progression examples. Their
presence does not require Graiphic to publish production runtime implementation
work for those later examples.
</p>

<p>
Graiphic's internal validation recipes, visual acceptance procedures,
development state, and proprietary runtime roadmap are maintained outside this
public repository. They do not redefine the public FROG specification.
</p>
