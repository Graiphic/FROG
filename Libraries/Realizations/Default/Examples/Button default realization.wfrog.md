<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example — Default Button Realization Package</h1>

<p align="center">
  <strong>Illustrative machine-readable <code>.wfrog</code> publication example for the default realization of <code>frog.widgets.button</code></strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#purpose-of-this-example">2. Purpose of this Example</a></li>
  <li><a href="#architectural-focus">3. Architectural Focus</a></li>
  <li><a href="#example-json">4. Example JSON</a></li>
  <li><a href="#reading-notes">5. Reading Notes</a></li>
  <li><a href="#why-this-example-matters">6. Why This Example Matters</a></li>
  <li><a href="#summary">7. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document provides an illustrative machine-readable <code>.wfrog</code> publication example for the default realization of <code>frog.widgets.button</code>.
</p>

<p>
It is intentionally small, but already complete enough to demonstrate:
</p>

<ul>
  <li>package identity,</li>
  <li>target-class declaration,</li>
  <li>realization declaration,</li>
  <li>state inventory,</li>
  <li>structural part bindings,</li>
  <li>resource publication,</li>
  <li>state-to-resource mapping,</li>
  <li>anchor publication for dynamic text placement,</li>
  <li>fallback posture.</li>
</ul>

<p>
This example is not presented as the only possible encoding shape of all future realization packages.
It is presented as the first coherent reference pattern for the default family.
</p>

<p>
It is also intentionally conservative.
It demonstrates the shared button realization corridor first, before adding variant-specific or skin-specific specialization layers.
</p>

<hr/>

<h2 id="purpose-of-this-example">2. Purpose of this Example</h2>

<p>
This example is not the generic <code>.wfrog</code> specification.
It is a concrete publication example built on top of the realization-package posture already defined elsewhere.
</p>

<p>
Its purpose is to show how the default official realization family may publish one standard widget embodiment in a machine-readable way without collapsing class law, realization publication, resources, and runtime implementation into the same layer.
</p>

<p>
It also serves a narrower purpose:
to prove that the default button corridor can already be published in a way that keeps dynamic public text portable and inspectable.
</p>

<p>
Because the button is the clearest first proof case for dynamic semantic text versus realization-owned placement, it is the correct place to establish the publication pattern that later numeric, string, boolean, and chart examples should follow.
</p>

<hr/>

<h2 id="architectural-focus">3. Architectural Focus</h2>

<p>
This example preserves the architectural split between:
</p>

<ul>
  <li>the semantic button label owned by the widget class through <code>label.text</code>,</li>
  <li>the realization-side placement surface used to display that text,</li>
  <li>the state-sensitive visual resources used for the button face,</li>
  <li>the runtime-side responsibility for rendering live text into the published placement surface.</li>
</ul>

<p>
That split is the most important rule demonstrated by this example.
</p>

<p>
The button <code>face</code> is treated as a state-sensitive visual surface.
The button <code>label</code> is treated as a dynamic semantic public part whose placement is realization-owned but whose content remains class-owned.
</p>

<p>
This means the example is designed to prove three things simultaneously:
</p>

<ul>
  <li><code>state_maps</code> are the preferred mechanism for state-sensitive visual embodiment,</li>
  <li><code>part_bindings</code> are the preferred mechanism for stable structural correspondence,</li>
  <li>anchors are the preferred mechanism for dynamic host-rendered button text placement.</li>
</ul>

<p>
The example is also family-generic rather than variant-specific.
It does not attempt to prove a specialized flat, raised, rounded, or skin-specific button posture yet.
That later specialization remains possible, but it should be layered on top of the base publication pattern shown here rather than replacing it.
</p>

<hr/>

<h2 id="example-json">4. Example JSON</h2>

<pre><code>{
  "wfrog_version": "1",
  "package_kind": "widget_realization_library",
  "package": {
    "id": "frog.realizations.default.button",
    "name": "FROG Default Button Realization",
    "namespace": "frog.realizations.default",
    "publisher": "FROG",
    "summary": "Default official realization for frog.widgets.button"
  },
  "imports": [],
  "exports": {
    "realizations": [
      "frog.realizations.default.button"
    ],
    "resources": [
      "button.face.normal.svg",
      "button.face.disabled.svg",
      "button.face.focused.svg",
      "button.face.pressed.svg",
      "button.label.anchor_map"
    ],
    "anchors": [
      "button.label.center"
    ]
  },
  "targets": [
    {
      "class_id": "frog.widgets.button"
    }
  ],
  "realizations": [
    {
      "id": "frog.realizations.default.button",
      "family": "default",
      "target_class": "frog.widgets.button",
      "parts": [
        "root",
        "face",
        "label",
        "frame"
      ],
      "states": [
        "normal",
        "disabled",
        "focused",
        "pressed"
      ],
      "part_bindings": [
        {
          "target_class": "frog.widgets.button",
          "part": "face",
          "binding_kind": "resource_layer",
          "binding_target": "face"
        },
        {
          "target_class": "frog.widgets.button",
          "part": "label",
          "binding_kind": "anchor",
          "binding_target": "button.label.center",
          "placement_role": "text_anchor",
          "fallback": "host_centered_text_region"
        },
        {
          "target_class": "frog.widgets.button",
          "part": "frame",
          "binding_kind": "resource_layer",
          "binding_target": "frame"
        }
      ],
      "state_maps": [
        {
          "target_class": "frog.widgets.button",
          "target_part": "face",
          "state": "normal",
          "resource_refs": [
            "button.face.normal.svg"
          ]
        },
        {
          "target_class": "frog.widgets.button",
          "target_part": "face",
          "state": "disabled",
          "resource_refs": [
            "button.face.disabled.svg"
          ],
          "fallback": "normal"
        },
        {
          "target_class": "frog.widgets.button",
          "target_part": "face",
          "state": "focused",
          "resource_refs": [
            "button.face.focused.svg"
          ],
          "fallback": "normal"
        },
        {
          "target_class": "frog.widgets.button",
          "target_part": "face",
          "state": "pressed",
          "resource_refs": [
            "button.face.pressed.svg"
          ],
          "fallback": "normal"
        }
      ],
      "fallbacks": {
        "focused": "normal",
        "pressed": "normal",
        "disabled": "normal"
      },
      "host_hints": {
        "supports_host_focus_ring": true,
        "supports_host_pressed_feedback": true,
        "preferred_scaling": "uniform"
      }
    }
  ],
  "resources": [
    {
      "id": "button.face.normal.svg",
      "kind": "svg",
      "path": "./assets/button/face/normal.svg",
      "target_class": "frog.widgets.button",
      "target_part": "face",
      "target_state": "normal"
    },
    {
      "id": "button.face.disabled.svg",
      "kind": "svg",
      "path": "./assets/button/face/disabled.svg",
      "target_class": "frog.widgets.button",
      "target_part": "face",
      "target_state": "disabled"
    },
    {
      "id": "button.face.focused.svg",
      "kind": "svg",
      "path": "./assets/button/face/focused.svg",
      "target_class": "frog.widgets.button",
      "target_part": "face",
      "target_state": "focused"
    },
    {
      "id": "button.face.pressed.svg",
      "kind": "svg",
      "path": "./assets/button/face/pressed.svg",
      "target_class": "frog.widgets.button",
      "target_part": "face",
      "target_state": "pressed"
    },
    {
      "id": "button.label.anchor_map",
      "kind": "anchor_map",
      "path": "./assets/button/anchors/label.json",
      "target_class": "frog.widgets.button",
      "target_part": "label",
      "role": "text_anchor"
    }
  ],
  "anchors": [
    {
      "id": "button.label.center",
      "target_class": "frog.widgets.button",
      "target_part": "label",
      "source_resource": "button.label.anchor_map",
      "anchor_kind": "text_anchor",
      "horizontal_alignment": "center",
      "vertical_alignment": "center",
      "padding": {
        "left": 10,
        "right": 10,
        "top": 6,
        "bottom": 6
      }
    }
  ]
}</code></pre>

<hr/>

<h2 id="reading-notes">5. Reading Notes</h2>

<p>
This example shows a clean four-layer split:
</p>

<ul>
  <li>the widget class already exists in <code>Libraries/Widgets/Button.md</code>,</li>
  <li>the default realization family already exists in <code>Libraries/Realizations/Default/Button.md</code>,</li>
  <li>this package publishes a machine-readable realization of that family,</li>
  <li>the assets remain realization resources rather than semantic truth.</li>
</ul>

<p>
The example also shows that:
</p>

<ul>
  <li><code>face</code> is the main state-driven visual part and therefore uses <code>state_maps</code>,</li>
  <li><code>label</code> is not published as baked final text inside state-specific SVG assets,</li>
  <li><code>label</code> is structurally bound to a published anchor so that the runtime can render live <code>label.text</code>,</li>
  <li><code>pressed</code> and <code>focused</code> may fall back to <code>normal</code>,</li>
  <li>host hints remain hints rather than semantic law.</li>
</ul>

<p>
The example intentionally keeps <code>frame</code> in the published part model even though no dedicated frame-specific resource is listed here.
That is acceptable because the example is focused on the minimum button publication corridor rather than on demonstrating every optional visual layer at once.
</p>

<p>
In other words:
</p>

<ul>
  <li>the published part model may be broader than the minimal demonstrated resource set,</li>
  <li>the example still remains coherent because the main demonstrated split concerns <code>face</code> and <code>label</code>,</li>
  <li>future examples may extend the same corridor with explicit frame resources or frame-specific state maps.</li>
</ul>

<p>
The example also intentionally does not publish a dedicated <code>variants</code> array or a dedicated <code>skin_id</code> specialization layer.
That omission is deliberate:
</p>

<ul>
  <li>the current goal is to prove the shared default button publication corridor first,</li>
  <li>variant-aware and skin-aware specialization can be layered later without changing the architectural rule already demonstrated here,</li>
  <li>the base example therefore stays small enough to remain the canonical first proof case.</li>
</ul>

<hr/>

<h2 id="why-this-example-matters">6. Why This Example Matters</h2>

<p>
This distinction is important because the standard button class defines <code>label.text</code> as a portable semantic surface.
The realization may define where that text is placed, clipped, aligned, or decorated, but it does not become the semantic owner of the label content itself.
</p>

<p>
Without this split, a runtime could easily drift into one of two broken postures:
</p>

<ul>
  <li>treating SVG files as the hidden owner of visible button text,</li>
  <li>treating runtime-private placement rules as the only real realization contract.</li>
</ul>

<p>
This example avoids both failures by making the following inspectable at publication time:
</p>

<ul>
  <li>the state-sensitive visual resources for the button face,</li>
  <li>the structural binding of the semantic label part,</li>
  <li>the explicit anchor used for host-rendered label placement,</li>
  <li>the fallback posture used when specialized resources or placement surfaces are unavailable.</li>
</ul>

<p>
That is why this example is the first normative proof case for the default realization example corridor.
It is small, but it already demonstrates the central architectural rule that later widget families must preserve.
</p>

<p>
It also establishes the correct extension direction:
future variant-specific or skin-specific button examples must specialize this base corridor rather than replacing it with a different implicit architecture.
</p>

<hr/>

<h2 id="summary">7. Summary</h2>

<p>
This example provides a complete first machine-readable publication model for the default realization of <code>frog.widgets.button</code> using:
</p>

<ul>
  <li>state-sensitive visual resources for <code>face</code>,</li>
  <li>a published anchor map for <code>label</code>,</li>
  <li>explicit structural bindings,</li>
  <li>inspectable fallback posture.</li>
</ul>

<p>
It is suitable as a reference pattern for other widget realization packages in the same family, especially when a realization must preserve the distinction between public semantic text and realization-side visual placement.
</p>

<p>
The next most coherent file to handle after this one is:
</p>

<ul>
  <li><code>Libraries/Realizations/Default/Examples/Button package and assets example.md</code></li>
</ul>

<p>
That file should now close the concrete side of the same proof corridor by aligning the example prose above with a simulated asset tree, concrete resource organization, and placement-support files.
</p>
