<h1>Widget Example Validation Checklist</h1>

<p>
This checklist captures the validation discipline required before a FROG widget
example can be considered accepted. A widget example is not valid merely because
the computation works; it must also prove that the visible front panel is driven
by the FROG source, the published widget realization package, and the Default
SVG assets.
</p>

<h2>Example Scope</h2>

- [ ] The example has a simple, explicit, and testable purpose.
- [ ] The example uses the smallest useful UI surface for the widget under test.
- [ ] The example does not broaden unrelated runtime behavior.
- [ ] The example does not introduce extra widgets unless they are necessary for the behavior being validated.
- [ ] The README states the real validation level without overclaiming a generalized runtime.

<h2>FROG Source Ownership</h2>

- [ ] The `.frog` source defines the diagram logic.
- [ ] The `.frog` source defines the front-panel widget instances.
- [ ] Widget position, size, label, initial value, role, binding, and per-instance visual customization live in `.frog`.
- [ ] Visual properties that an IDE user would customize live as `.frog` instance properties.
- [ ] The `.frog` front panel references the required `.wfrog` package.
- [ ] The `.frog` widget instance references the expected `visual.asset_ref`.

<h2>WFROG Realization Ownership</h2>

- [ ] The `.wfrog` file is a widget realization package, not an example-specific front panel.
- [ ] The `.wfrog` file declares realization references and SVG assets.
- [ ] The `.wfrog` ownership boundary explicitly excludes front-panel widget instance layout, values, bindings, and example behavior.
- [ ] The `.wfrog` file does not duplicate the `.frog` front-panel layout.
- [ ] The `.wfrog` assets point to `Libraries/Realizations/Default/assets/...`.

<h2>SVG Discipline</h2>

- [ ] There are no duplicated local SVG assets under `Examples/<example>/ui/assets`.
- [ ] The visible widget consumes the Default realization SVG asset.
- [ ] The SVG exposes public semantic markers such as `data-frog-part`, `data-frog-anchor`, `data-frog-bind`, or equivalent published markers.
- [ ] Styling hooks needed by the runtime are represented by labeled SVG parts or published realization properties.
- [ ] Marginal SVG edits remain compatible with the published parts and anchors.

<h2>Runtime Rendering</h2>

- [ ] The runtime does not render a hardcoded HTML/CSS fallback disguised as a widget.
- [ ] The runtime renders from `.frog` instance data, `.wfrog` realization data, and the Default SVG asset.
- [ ] The runtime exposes asset routes such as `/asset/<asset_id>`.
- [ ] The rendered surface carries a marker equivalent to `data-frog-visual-law='wfrog-realization-state-map'`.
- [ ] Debug snapshots are not visible in the normal front panel unless the example explicitly requires a debug surface.
- [ ] Runtime defaults are only safety defaults; validated appearance comes from `.frog` / `.wfrog` data.

<h2>Interaction And Visual States</h2>

- [ ] Controls are interactive.
- [ ] Indicators are read-only.
- [ ] Required widget states are modeled: normal, hover, pressed, selected, disabled, or read-only when relevant.
- [ ] State colors, borders, fonts, dimensions, and offsets are configurable through `.frog` instance properties when they are example-specific.
- [ ] Labels are positioned by instance properties, not hidden runtime constants.
- [ ] Text is legible, aligned, and contained inside the widget surface.
- [ ] Dropdowns, selectors, buttons, toggles, text fields, and numeric steppers behave as visible widgets, not decorative images.

<h2>Compilation Corridor</h2>

- [ ] The `.frog` source derives to FIR.
- [ ] FIR lowers to the expected lowered unit.
- [ ] The backend emitter produces the expected contract.
- [ ] The LLVM emitter produces `kernel.ll`.
- [ ] The native manifest declares the ABI, entry symbol, inputs, outputs, and kernel artifact.
- [ ] The runtime consumes the manifest-declared native kernel artifact rather than depending on LLVM directly.

<h2>Runtime Parity</h2>

- [ ] C++ validates the new example first.
- [ ] Python and Rust are then brought to the same behavior level.
- [ ] The same runtime family in each language can run all accepted examples.
- [ ] C++ / Python / Rust produce the same public output and UI output for equivalent inputs.
- [ ] C++ / Python / Rust consume the same `.frog` source, `.wfrog` realization package, and native manifest truth.

<h2>Tests</h2>

- [ ] Tests reject hardcoded fallback markers such as `*-card` or equivalent hand-built widget shells.
- [ ] Tests verify the expected `/asset/...` route.
- [ ] Tests verify that the visible surface uses the `.wfrog` realization state map.
- [ ] Tests verify the important interaction states.
- [ ] Tests verify the expected ABI and native bridge when the example is LLVM-backed.
- [ ] Tests cover at least one non-trivial input value.
- [ ] Tests confirm the indicator output matches the diagram result.

<h2>Validation Commands</h2>

- [ ] `python Implementations/Reference/check_reference_workspace.py --include-pytest` passes.
- [ ] The native kernel bridge check passes for the relevant runtime languages.
- [ ] C++ builds and `ctest` passes.
- [ ] Rust `cargo test` passes.
- [ ] Python runtime tests pass.
- [ ] `git diff --check` passes.
- [ ] Generated cache/build artifacts are not staged.

<h2>Acceptance Rule</h2>

<p>
A widget example is accepted only when it proves the full FROG discipline:
</p>

<pre><code>.frog diagram + front-panel instances
  -&gt; FIR
  -&gt; lowering
  -&gt; LLVM kernel + native manifest
  -&gt; runtime C++ / Python / Rust
  -&gt; .wfrog realization package
  -&gt; Default SVG asset
  -&gt; faithful visible UI
</code></pre>

<p>
If the runtime visually cheats, the example is not valid, even if the computation
itself succeeds.
</p>
