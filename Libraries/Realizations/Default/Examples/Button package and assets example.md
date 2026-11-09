<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example — Default Button Package and Asset Tree</h1>

<p align="center">
  <strong>Complete illustrative example of a default button realization package together with a simulated asset tree</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#purpose">2. Purpose</a></li>
  <li><a href="#what-this-example-demonstrates">3. What This Example Demonstrates</a></li>
  <li><a href="#simulated-tree">4. Simulated Tree</a></li>
  <li><a href="#files-included">5. Files Included</a></li>
  <li><a href="#publication-alignment">6. Publication Alignment</a></li>
  <li><a href="#reading-notes">7. Reading Notes</a></li>
  <li><a href="#summary">8. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document provides a complete illustrative example for the default realization of <code>frog.widgets.button</code>.
</p>

<p>
It combines:
</p>

<ul>
  <li>a machine-readable <code>.wfrog</code> realization package,</li>
  <li>a normalized asset tree,</li>
  <li>a resource manifest,</li>
  <li>a layer map,</li>
  <li>an anchor publication resource for dynamic label placement.</li>
</ul>

<p>
The example is intentionally small, but it is structured to preserve the doctrinal split between widget semantic law, realization publication, asset resources, and runtime rendering responsibility.
</p>

<p>
This file is therefore the concrete asset-facing companion to the machine-readable example package.
It shows how the package-level publication corridor may be materialized on disk without introducing semantic drift.
</p>

<p>
It also serves as the first concrete proof that the button realization corridor can be published in a form that is both human-readable and machine-readable while remaining compatible with later runtime implementations in different host environments.
</p>

<hr/>

<h2 id="purpose">2. Purpose</h2>

<p>
The purpose of this example is to show one coherent end-to-end publication unit for a standard widget realization without confusing:
</p>

<ul>
  <li>class law,</li>
  <li>realization-family posture,</li>
  <li>package publication,</li>
  <li>asset files,</li>
  <li>runtime implementation.</li>
</ul>

<p>
In particular, this example preserves the distinction between:
</p>

<ul>
  <li>the semantic button label owned by the widget class through <code>label.text</code>,</li>
  <li>the realization-side placement surface used to display that text,</li>
  <li>the state-sensitive visual resources used for the button face,</li>
  <li>optional decorative or structural layers such as a frame,</li>
  <li>the runtime-side responsibility for drawing live text into the published placement surface.</li>
</ul>

<p>
This distinction is the main reason the example package and the asset tree must be shown together.
The package explains the realization contract.
The asset tree shows how that contract is concretely supported.
</p>

<p>
The example is intentionally family-generic rather than variant-specific.
It proves the shared default button publication corridor first.
Later examples may specialize that corridor for compatible realization variants or skin bundles, but they should not replace the architectural split demonstrated here.
</p>

<hr/>

<h2 id="what-this-example-demonstrates">3. What This Example Demonstrates</h2>

<p>
This example is intended to prove at least the following:
</p>

<ul>
  <li>the asset tree can remain aligned with the machine-readable realization package,</li>
  <li>the state-sensitive button face can be published through explicit state resources,</li>
  <li>the semantic button label can remain class-owned while still receiving realization-defined placement support,</li>
  <li>optional structural layers such as <code>frame</code> can be published without becoming new widget semantics,</li>
  <li>a runtime can consume the same publication corridor without relying on undocumented filename heuristics or runtime-private layout rules.</li>
</ul>

<p>
The example also demonstrates the preferred split between:
</p>

<ul>
  <li><code>state_maps</code> for visual state embodiment,</li>
  <li><code>part_bindings</code> for stable structural correspondence,</li>
  <li>anchor publication for dynamic host-rendered label placement.</li>
</ul>

<p>
More concretely, this example is meant to show that:
</p>

<ul>
  <li>the asset tree is not merely decorative documentation,</li>
  <li>resource placement on disk remains part of the inspectable publication corridor,</li>
  <li>the runtime should consume published realization structure rather than inventing its own hidden realization contract.</li>
</ul>

<hr/>

<h2 id="simulated-tree">4. Simulated Tree</h2>

<pre><code>Libraries/Realizations/Default/Examples/ButtonPackage/
├── default_button_realization.wfrog.json
└── assets/
    └── button/
        ├── asset_manifest.json
        ├── face/
        │   ├── normal.svg
        │   ├── disabled.svg
        │   ├── focused.svg
        │   ├── pressed.svg
        │   └── layer_map.json
        ├── anchors/
        │   └── label.json
        └── frame/
            ├── normal.svg
            └── focused.svg
</code></pre>

<p>
This tree intentionally does <strong>not</strong> publish state-specific SVG assets for <code>label</code>.
The button label remains a dynamic public surface, so the realization publishes a placement surface for that label rather than baking final text ownership into SVG files.
</p>

<p>
This tree is also intentionally conservative:
</p>

<ul>
  <li><code>face/</code> carries the main state-sensitive visual embodiment,</li>
  <li><code>anchors/</code> carries dynamic placement support for host-rendered semantic text,</li>
  <li><code>frame/</code> illustrates an optional supporting visual layer,</li>
  <li>no private runtime-only folders are treated as if they were part of the published realization contract.</li>
</ul>

<p>
The tree also shows an important discipline rule for future realization families:
variant-specific or skin-specific material should only be introduced when it is published explicitly as such, not smuggled into the asset structure as an undocumented runtime convention.
</p>

<hr/>

<h2 id="files-included">5. Files Included</h2>

<ul>
  <li><code>default_button_realization.wfrog.json</code></li>
  <li><code>asset_manifest.json</code></li>
  <li><code>layer_map.json</code></li>
  <li><code>label.json</code> under <code>anchors/</code></li>
  <li><code>normal.svg</code>, <code>disabled.svg</code>, <code>focused.svg</code>, and <code>pressed.svg</code> under <code>face/</code></li>
  <li><code>normal.svg</code> and <code>focused.svg</code> under <code>frame/</code></li>
</ul>

<p>
The SVG files are represented here as paths and placeholders only.
Their visual drawing content is intentionally left out at this stage.
</p>

<p>
The expected role of each file is as follows:
</p>

<ul>
  <li><code>default_button_realization.wfrog.json</code> publishes the machine-readable realization package, including target class, parts, bindings, state maps, and fallbacks,</li>
  <li><code>asset_manifest.json</code> provides a compact inventory of the resources shipped with this example package,</li>
  <li><code>layer_map.json</code> describes how visual face layers are composed or interpreted when the runtime loads face resources,</li>
  <li><code>anchors/label.json</code> publishes the placement surface used by the runtime to render live <code>label.text</code>,</li>
  <li><code>face/*.svg</code> publishes state-specific visual embodiment for the button face,</li>
  <li><code>frame/*.svg</code> publishes an optional supporting visual layer for outer emphasis or focus posture.</li>
</ul>

<p>
In a fuller publication corridor, the same pattern may later be extended with:
</p>

<ul>
  <li>variant-specific subtrees,</li>
  <li>skin-specific resource groups,</li>
  <li>style-token support artifacts,</li>
  <li>additional frame or density resources.</li>
</ul>

<p>
However, none of those extensions are required for this first proof case.
The example deliberately remains small enough to keep the core architectural split legible.
</p>

<hr/>

<h2 id="publication-alignment">6. Publication Alignment</h2>

<p>
The important alignment rule of this example is:
</p>

<ul>
  <li>the package JSON identifies the realization record and its <code>state_maps</code>,</li>
  <li>the resource inventory identifies the concrete resource files,</li>
  <li>the asset tree places those files in a normalized directory structure,</li>
  <li>the runtime consumes the published placement and state information rather than inventing its own hidden contract.</li>
</ul>

<p>
For the button corridor specifically, the expected alignment is:
</p>

<ul>
  <li><code>face</code> resources align with state-sensitive visual publication such as <code>normal</code>, <code>disabled</code>, <code>focused</code>, and <code>pressed</code>,</li>
  <li><code>frame</code> resources align with optional supporting visual publication when the family exposes it,</li>
  <li><code>anchors/label.json</code> aligns with the published placement surface used for dynamic label rendering,</li>
  <li>the package-level <code>part_bindings</code> align the public parts with those realization-side surfaces.</li>
</ul>

<p>
This means the tree is not merely decorative.
It is part of the inspectable realization-publication corridor.
</p>

<p>
The alignment rule also implies the reverse:
</p>

<ul>
  <li>the asset tree must not imply semantics that the package does not publish,</li>
  <li>the package must not reference resources that the tree does not actually make available,</li>
  <li>the runtime must not rely on silent assumptions that neither the package nor the tree expose explicitly.</li>
</ul>

<hr/>

<h2 id="reading-notes">7. Reading Notes</h2>

<p>
This example is intentionally conservative:
</p>

<ul>
  <li>the package targets one class,</li>
  <li>the assets are scoped by structural realization role,</li>
  <li>the state mapping is explicit for the main state-driven visual part,</li>
  <li>fallback remains explicit,</li>
  <li>the maps remain realization support, not semantic truth.</li>
</ul>

<p>
The most important architectural point is that <code>face</code> and <code>label</code> do not follow the same publication posture:
</p>

<ul>
  <li><code>face</code> is a state-driven visual part, so it is represented through state-sensitive SVG resources,</li>
  <li><code>label</code> is a semantic public text-bearing part, so it is represented through a placement resource rather than through baked final text assets.</li>
</ul>

<p>
This means that:
</p>

<ul>
  <li>the widget class remains the owner of the semantic text surface,</li>
  <li>the realization remains the owner of the placement geometry and visual embodiment,</li>
  <li>the runtime remains responsible for rendering the live text using the published placement surface,</li>
  <li>the asset tree does not silently become the owner of button-text semantics.</li>
</ul>

<p>
The optional <code>frame/</code> subtree is shown here to illustrate that a realization family may publish additional visual layers without changing widget-class meaning.
Whether a given runtime chooses to materialize <code>frame</code> as a distinct host layer or flatten it into a composed drawing pipeline remains an implementation matter, provided that the published realization contract is respected.
</p>

<p>
This example also demonstrates a useful discipline rule for future widget families:
</p>

<ul>
  <li>when a public surface is fundamentally dynamic and host-rendered, prefer publishing a placement resource,</li>
  <li>when a public surface is fundamentally a state-sensitive visual embodiment, prefer publishing explicit state resources,</li>
  <li>do not let the asset tree blur that difference.</li>
</ul>

<p>
The same discipline will matter later for:
</p>

<ul>
  <li>numeric <code>value_display</code> surfaces,</li>
  <li>string <code>text_display</code> surfaces,</li>
  <li>boolean <code>label</code> surfaces,</li>
  <li>chart labels and other dynamic chart-associated text-bearing surfaces.</li>
</ul>

<hr/>

<h2 id="summary">8. Summary</h2>

<p>
This example provides a complete first publication block for a standard widget realization package in the default family.
</p>

<p>
It demonstrates a coherent asset-publication posture in which:
</p>

<ul>
  <li>state-sensitive SVG resources are used for the button face,</li>
  <li>dynamic label placement is published through an anchor resource,</li>
  <li>machine-readable package publication and concrete assets remain aligned,</li>
  <li>semantic ownership does not collapse into the asset tree.</li>
</ul>

<p>
It is designed to serve as the reference pattern for the next families such as numeric, boolean, string, and chart, especially when those widgets also expose dynamic public text or other dynamic public surfaces that must remain portable across runtimes.
</p>

<p>
The next most coherent file to handle after this one is:
</p>

<ul>
  <li><code>Libraries/Realizations/Default/Examples/ButtonPackage/default_button_realization.wfrog.json</code></li>
</ul>

<p>
That file is the natural next step because the prose corridor is now coherent and the next closure point is the concrete machine-readable artifact that the simulated tree claims to publish.
</p>
