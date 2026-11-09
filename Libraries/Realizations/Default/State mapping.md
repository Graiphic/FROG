<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization State Mapping</h1>

<p align="center">
  <strong>Normative state and binding model for the official <code>Default</code> widget realization family</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#why-state-mapping-is-explicit">2. Why State Mapping Is Explicit</a></li>
  <li><a href="#state-map-purpose">3. State Map Purpose</a></li>
  <li><a href="#state-map-shape">4. State Map Shape</a></li>
  <li><a href="#part-binding-shape">5. Part Binding Shape</a></li>
  <li><a href="#realization-variant-and-skin-interaction">6. Realization-Variant and Skin Interaction</a></li>
  <li><a href="#state-maps-versus-structural-bindings">7. State Maps versus Structural Bindings</a></li>
  <li><a href="#widget-level-vs-part-level-maps">8. Widget-Level vs Part-Level Maps</a></li>
  <li><a href="#fallback-rules">9. Fallback Rules</a></li>
  <li><a href="#examples">10. Examples</a></li>
  <li><a href="#validation-posture">11. Validation Posture</a></li>
  <li><a href="#summary">12. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the state and binding model used by the official <code>Default</code> realization family.
</p>

<p>
The state-and-binding layer is the machine-readable realization layer that ties:
</p>

<ul>
  <li>public widget parts,</li>
  <li>published realization states,</li>
  <li>published realization resources,</li>
  <li>published anchors, text regions, style-token surfaces, or equivalent realization structures when needed.</li>
</ul>

<p>
Its role is to make realization correspondence explicit without redefining widget class law.
</p>

<p>
The central distinction of this document is simple:
</p>

<ul>
  <li><strong>state maps</strong> describe state-sensitive visual embodiment,</li>
  <li><strong>part bindings</strong> describe stable structural correspondence between a public part and a realization-side surface.</li>
</ul>

<p>
This layer also provides the correct place to express inspectable fallback posture and, when the realization family allows them, compatible realization-variant and compatible skin specializations without turning embodiment choice into hidden class-law drift.
</p>

<hr/>

<h2 id="why-state-mapping-is-explicit">2. Why State Mapping Is Explicit</h2>

<p>
State mapping is made explicit so that:
</p>

<ul>
  <li>runtimes do not need to guess state/resource correspondence,</li>
  <li>assets do not become the only source of realization structure,</li>
  <li>validation can detect missing or ambiguous bindings,</li>
  <li>fallback posture can be inspected rather than hidden in runtime code,</li>
  <li>dynamic parts such as text-bearing parts do not get incorrectly modeled as if they were only static state assets,</li>
  <li>compatible realization variants do not become undocumented runtime-only forks,</li>
  <li>compatible skins do not become undocumented runtime-only theme substitutions.</li>
</ul>

<p>
The default family therefore treats state-sensitive embodiment and structural part binding as related but distinct publication concerns.
</p>

<hr/>

<h2 id="state-map-purpose">3. State Map Purpose</h2>

<p>
A state map associates:
</p>

<ul>
  <li>a target widget class,</li>
  <li>optionally a target widget part,</li>
  <li>optionally a target realization variant,</li>
  <li>optionally a target skin,</li>
  <li>a supported realization state,</li>
  <li>one or more resource references,</li>
  <li>optional fallback posture.</li>
</ul>

<p>
A state map answers the question:
</p>

<p>
<em>Which realization resource or resource group embodies this widget surface in this realization state?</em>
</p>

<p>
State maps are most natural for surfaces such as:
</p>

<ul>
  <li>a button face in <code>normal</code>, <code>focused</code>, or <code>pressed</code>,</li>
  <li>a button frame in <code>focused</code> or <code>disabled</code>,</li>
  <li>a boolean face in state-qualified variants,</li>
  <li>a numeric increment button in <code>pressed</code>.</li>
</ul>

<p>
State maps are generally <strong>not</strong> the preferred primary publication mechanism for dynamic public text-bearing parts whose live content is rendered by the host.
In those cases, the preferred primary mechanism is a structural binding to an anchor, text region, or equivalent placement surface.
</p>

<p>
State maps are also not the preferred primary mechanism for public style semantics.
When skin or styling support exists, state maps should select embodiment resources, while token publication and bounded host styling posture remain separate realization concerns.
</p>

<hr/>

<h2 id="state-map-shape">4. State Map Shape</h2>

<p>
A state map record SHOULD contain:
</p>

<ul>
  <li><code>target_class</code>,</li>
  <li><code>target_part</code> when applicable,</li>
  <li><code>state</code>,</li>
  <li><code>resource_refs</code>,</li>
  <li><code>fallback</code> when applicable.</li>
</ul>

<p>
A state map record MAY also contain:
</p>

<ul>
  <li><code>target_variant</code> when the map is specific to one published realization variant,</li>
  <li><code>target_skin</code> when the map is specific to one compatible published skin,</li>
  <li><code>priority</code> when multiple compatible maps exist under explicit precedence rules,</li>
  <li><code>conditions</code> when the realization family publishes additional bounded selection posture,</li>
  <li><code>host_substitution</code> when a host-native fallback is explicitly declared.</li>
</ul>

<p>
Conceptually:
</p>

<pre><code>{
  "target_class": "frog.widgets.button",
  "target_part": "face",
  "state": "pressed",
  "resource_refs": [
    "button.face.pressed.svg"
  ],
  "fallback": "normal"
}</code></pre>

<p>
A variant-specific example:
</p>

<pre><code>{
  "target_class": "frog.widgets.boolean_control",
  "target_part": "state_face",
  "target_variant": "switch_like",
  "state": "normal_true",
  "resource_refs": [
    "boolean_control.state_face.switch_like.normal_true.svg"
  ],
  "fallback": "normal_true"
}</code></pre>

<p>
A skin-specific example:
</p>

<pre><code>{
  "target_class": "frog.widgets.button",
  "target_part": "face",
  "target_variant": "raised",
  "target_skin": "blue",
  "state": "normal",
  "resource_refs": [
    "button.face.raised.blue.normal.svg"
  ],
  "fallback": "normal"
}</code></pre>

<p>
A state map should identify realization-side embodiment.
It should not be used to smuggle new public widget semantics into a resource-selection table.
</p>

<hr/>

<h2 id="part-binding-shape">5. Part Binding Shape</h2>

<p>
A part binding associates a public part with a realization surface.
</p>

<p>
A part binding record SHOULD contain:
</p>

<ul>
  <li><code>target_class</code>,</li>
  <li><code>part</code>,</li>
  <li><code>binding_kind</code>,</li>
  <li><code>binding_target</code>.</li>
</ul>

<p>
A part binding record MAY also contain:
</p>

<ul>
  <li><code>target_variant</code> when the binding is specific to one published realization variant,</li>
  <li><code>target_skin</code> when the binding is specific to one compatible published skin,</li>
  <li><code>fallback</code> when the binding target can degrade gracefully,</li>
  <li><code>placement_role</code> when the target is a text anchor, clipping region, tokenized placement surface, or equivalent realization surface.</li>
</ul>

<p>
Typical binding kinds include:
</p>

<ul>
  <li><code>resource_layer</code>,</li>
  <li><code>anchor</code>,</li>
  <li><code>text_region</code>,</li>
  <li><code>host_region</code>.</li>
</ul>

<p>
A part binding answers the question:
</p>

<p>
<em>Where does this public part live, visually or structurally, within the realization?</em>
</p>

<p>
Part bindings should remain stable unless the realization explicitly publishes multiple structural postures under inspectable precedence rules.
The default family therefore prefers keeping state variation in <code>state_maps</code> and keeping structural correspondence in <code>part_bindings</code>.
</p>

<p>
When styling support exists, a part binding may remain unchanged while state maps, token maps, or skin-scoped resources vary around it.
That is generally preferable to treating every style change as a new structural binding.
</p>

<hr/>

<h2 id="realization-variant-and-skin-interaction">6. Realization-Variant and Skin Interaction</h2>

<p>
The default family may allow several compatible realization variants for the same standardized class.
It may also allow several compatible skins within one realization corridor.
When that happens, state maps and part bindings must make the specialization explicit rather than leaving it implicit in runtime code.
</p>

<p>
The preferred rule is:
</p>

<ul>
  <li>shared embodiment posture stays at the parent realization level,</li>
  <li>variant-specific embodiment posture is marked explicitly through <code>target_variant</code> or an equivalent inspectable structure,</li>
  <li>skin-specific embodiment posture is marked explicitly through <code>target_skin</code> or an equivalent inspectable structure.</li>
</ul>

<p>
This means:
</p>

<ul>
  <li>a boolean checkbox-like embodiment and a boolean switch-like embodiment may remain two realization variants of the same boolean class,</li>
  <li>a compact chart embodiment and a standard chart embodiment may remain two realization variants of the same chart class,</li>
  <li>a blue button skin and a dark button skin may remain two compatible skins of the same button realization corridor,</li>
  <li>the existence of such variants or skins does not by itself create new widget classes.</li>
</ul>

<p>
A variant-specific or skin-specific state map or part binding MAY specialize:
</p>

<ul>
  <li>resource selection,</li>
  <li>placement posture,</li>
  <li>supporting frame posture,</li>
  <li>tokenized styling posture,</li>
  <li>host-facing fallback preference.</li>
</ul>

<p>
A variant-specific or skin-specific state map or part binding MUST NOT by itself introduce:
</p>

<ul>
  <li>new mandatory public properties,</li>
  <li>new mandatory public methods,</li>
  <li>new mandatory public events,</li>
  <li>a new implicit part model,</li>
  <li>a hidden class split.</li>
</ul>

<p>
A compatible skin is therefore a realization specialization, not a class contract.
A compatible variant is also a realization specialization, not a class contract.
</p>

<hr/>

<h2 id="state-maps-versus-structural-bindings">7. State Maps versus Structural Bindings</h2>

<p>
The default family distinguishes clearly between state maps and structural bindings.
</p>

<ul>
  <li>a <strong>state map</strong> selects state-sensitive realization resources,</li>
  <li>a <strong>part binding</strong> associates a public part with a realization-side surface, region, layer, or anchor.</li>
</ul>

<p>
This distinction is important because not every public part should be modeled as a state-varying asset.
</p>

<p>
For example, in the default button posture:
</p>

<ul>
  <li><code>face</code> is naturally modeled through state maps such as <code>normal</code>, <code>disabled</code>, <code>focused</code>, and <code>pressed</code>,</li>
  <li><code>frame</code> may also be modeled through state maps when the realization family publishes frame variants,</li>
  <li><code>label</code> is naturally modeled through a structural binding to a text anchor, text region, or equivalent host-rendered placement surface.</li>
</ul>

<p>
The label may still vary stylistically across states, variants, or skins, but its primary publication posture remains that of a dynamically rendered part bound to a placement surface rather than a semantic text string baked into a visual asset.
</p>

<p>
Therefore:
</p>

<ul>
  <li>state maps SHOULD be used for state-sensitive embodiment,</li>
  <li>part bindings SHOULD be used for stable structural correspondence,</li>
  <li>dynamic text-bearing parts SHOULD preferably bind to anchors, text regions, or host regions rather than being modeled only through state-scoped SVG files.</li>
</ul>

<p>
This rule applies more broadly than buttons.
It also matters for numeric displays, string value regions, chart titles, axis labels, and other future dynamic public surfaces.
</p>

<p>
The same rule also applies to compatible realization variants and compatible skins:
a variant or skin may change how a part is embodied or where it is placed, but it should still preserve the distinction between state-sensitive embodiment and stable structural correspondence.
</p>

<hr/>

<h2 id="widget-level-vs-part-level-maps">8. Widget-Level vs Part-Level Maps</h2>

<p>
The default family prefers part-level maps when a public part is explicitly realized.
</p>

<p>
Widget-level maps MAY be used when:
</p>

<ul>
  <li>the whole widget surface changes as one unit,</li>
  <li>the realization does not expose differentiated visual sub-surfaces for that state,</li>
  <li>the target class has a minimal part model.</li>
</ul>

<p>
Part-level maps are generally preferable when:
</p>

<ul>
  <li>the class exposes stable parts such as <code>face</code>, <code>label</code>, <code>frame</code>,</li>
  <li>different parts have different realization postures,</li>
  <li>only some parts are state-sensitive,</li>
  <li>the package intends to keep anchor or text-region publication inspectable.</li>
</ul>

<p>
For dynamic text-bearing parts, a part-level structural binding is usually more informative than a widget-level state map because it preserves the distinction between:
</p>

<ul>
  <li>the public semantic surface,</li>
  <li>the realization placement surface,</li>
  <li>the visual resources that may still vary elsewhere in the widget.</li>
</ul>

<p>
For variant-aware or skin-aware publication, part-level mapping is also generally preferable because it allows one variant or one skin to specialize one part without forcing the whole widget to appear as a different class-like object.
</p>

<hr/>

<h2 id="fallback-rules">9. Fallback Rules</h2>

<p>
A state map MAY define an explicit fallback.
</p>

<p>
Fallback SHOULD normally move toward a less specialized state rather than a more specialized one.
</p>

<p>
Examples:
</p>

<ul>
  <li><code>pressed</code> may fall back to <code>normal</code>,</li>
  <li><code>focused</code> may fall back to <code>normal</code> or to a documented host-native focus posture,</li>
  <li><code>disabled_true</code> may fall back to a host-native disabled boolean rendering preserving the true-state distinction.</li>
</ul>

<p>
A part binding MAY also define a fallback posture.
That is especially useful for dynamic placement surfaces.
</p>

<p>
Examples:
</p>

<ul>
  <li>a missing <code>text_region</code> may fall back to a host-native centered region,</li>
  <li>a missing focus-ring resource layer may fall back to a host-native focus ring,</li>
  <li>a missing anchor may fall back to a documented host-region rule.</li>
</ul>

<p>
Fallback must remain inspectable.
It must not silently collapse public part meaning into runtime-private heuristics.
</p>

<p>
The preferred pattern is:
</p>

<ul>
  <li><code>state_maps</code> publish visual-state fallback,</li>
  <li><code>part_bindings</code> publish structural or placement fallback.</li>
</ul>

<p>
When realization variants exist, fallback MAY also move:
</p>

<ul>
  <li>from one specialized state to another less specialized state within the same variant,</li>
  <li>from one specialized placement posture to a host-native compatible placement posture,</li>
  <li>from one embodiment variant to another compatible embodiment variant of the same published class.</li>
</ul>

<p>
When compatible skins exist, fallback MAY also move:
</p>

<ul>
  <li>from one skin-specific resource group to the default skin of the same variant,</li>
  <li>from one skin-specific token set to the base token set of the same realization corridor,</li>
  <li>from one skin-specific placement refinement to the parent placement posture of the same class and variant.</li>
</ul>

<p>
Those cases are acceptable only if:
</p>

<ul>
  <li>the public class meaning remains unchanged,</li>
  <li>the published part meaning remains preserved,</li>
  <li>the fallback remains inspectable in realization publication rather than hidden in runtime-private code.</li>
</ul>

<hr/>

<h2 id="examples">10. Examples</h2>

<pre><code>{
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
      "fallback": "host_native_focus_ring"
    },
    {
      "target_class": "frog.widgets.button",
      "target_part": "face",
      "state": "pressed",
      "resource_refs": [
        "button.face.pressed.svg"
      ],
      "fallback": "normal"
    },
    {
      "target_class": "frog.widgets.button",
      "target_part": "frame",
      "state": "focused",
      "resource_refs": [
        "button.frame.focused.svg"
      ],
      "fallback": "normal"
    },
    {
      "target_class": "frog.widgets.numeric_control",
      "target_part": "increment_button",
      "state": "pressed",
      "resource_refs": [
        "numeric_control.increment_button.pressed.svg"
      ],
      "fallback": "normal"
    },
    {
      "target_class": "frog.widgets.boolean_control",
      "target_part": "state_face",
      "target_variant": "switch_like",
      "state": "normal_true",
      "resource_refs": [
        "boolean_control.state_face.switch_like.normal_true.svg"
      ],
      "fallback": "normal_true"
    },
    {
      "target_class": "frog.widgets.button",
      "target_part": "face",
      "target_variant": "raised",
      "target_skin": "blue",
      "state": "normal",
      "resource_refs": [
        "button.face.raised.blue.normal.svg"
      ],
      "fallback": "normal"
    }
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
    },
    {
      "target_class": "frog.widgets.numeric_control",
      "part": "value_display",
      "binding_kind": "text_region",
      "binding_target": "numeric_control.value.center",
      "placement_role": "text_region",
      "fallback": "host_value_region"
    },
    {
      "target_class": "frog.widgets.boolean_control",
      "part": "label",
      "target_variant": "switch_like",
      "binding_kind": "anchor",
      "binding_target": "boolean_control.switch_like.label.center",
      "placement_role": "text_anchor",
      "fallback": "host_centered_text_region"
    },
    {
      "target_class": "frog.widgets.button",
      "part": "label",
      "target_variant": "raised",
      "target_skin": "blue",
      "binding_kind": "anchor",
      "binding_target": "button.raised.blue.label.center",
      "placement_role": "text_anchor",
      "fallback": "button.label.center"
    }
  ]
}</code></pre>

<p>
In this example:
</p>

<ul>
  <li>the button <code>face</code> is realized through state-sensitive resources,</li>
  <li>the button <code>frame</code> may expose additional state-sensitive embodiment,</li>
  <li>the button <code>label</code> is realized through a structural binding to a published placement surface,</li>
  <li>the numeric <code>value_display</code> is realized through a structural binding to a published text region,</li>
  <li>the boolean <code>switch_like</code> embodiment remains a realization variant of the same boolean class rather than a new class,</li>
  <li>the button <code>raised</code>/<code>blue</code> specialization remains a compatible skin specialization rather than a new class,</li>
  <li>the package remains explicit about fallback posture for both state-sensitive embodiment and dynamic placement.</li>
</ul>

<hr/>

<h2 id="validation-posture">11. Validation Posture</h2>

<p>
Validators SHOULD diagnose at least:
</p>

<ul>
  <li>state maps referencing undeclared classes, parts, states, variants, or skins,</li>
  <li>missing resource references,</li>
  <li>ambiguous duplicate state bindings without explicit precedence,</li>
  <li>fallback chains that resolve to unknown states, unknown variants, unknown skins, or undocumented host substitutions,</li>
  <li>part bindings that target undeclared public parts,</li>
  <li>part bindings whose binding kinds are unknown or incompatible with the referenced binding target,</li>
  <li>dynamic public text-bearing parts modeled only as asset-baked state resources when the realization family claims explicit anchor or text-region support.</li>
</ul>

<p>
Validators MAY also diagnose:
</p>

<ul>
  <li>needlessly coarse widget-level mappings when a stable part-level posture is already published,</li>
  <li>overlapping bindings for the same class and part without explicit precedence,</li>
  <li>state maps that attempt to encode public widget semantics instead of realization-side embodiment,</li>
  <li>examples in which a dynamic public text-bearing part is simultaneously treated as anchor-bound and as the semantic owner of state-scoped SVG label resources without an explicit dual-layer realization contract,</li>
  <li>variant-specific state maps or bindings that appear to introduce a hidden new class split,</li>
  <li>skin-specific state maps or bindings that appear to introduce hidden semantic divergence instead of bounded styling specialization.</li>
</ul>

<hr/>

<h2 id="summary">12. Summary</h2>

<p>
The default realization state-and-binding model makes realization correspondence explicit.
</p>

<p>
It is the correct layer for:
</p>

<ul>
  <li>part/state/resource correspondence,</li>
  <li>structural part bindings,</li>
  <li>fallback rules for state-sensitive embodiment and dynamic placement,</li>
  <li>inspectable specialization of compatible realization variants,</li>
  <li>inspectable specialization of compatible skins inside one realization corridor.</li>
</ul>

<p>
Its preferred architecture is:
</p>

<ul>
  <li><code>state_maps</code> for state-sensitive visual embodiment,</li>
  <li><code>part_bindings</code> for stable structural correspondence,</li>
  <li>placement surfaces for dynamic public parts rendered by the host,</li>
  <li>variant-aware and skin-aware specialization when embodiment choice is published,</li>
  <li>clear separation between realization-side embodiment and widget-class meaning.</li>
</ul>

<p>
It remains fully subordinate to widget class law and realization-family posture.
Its main architectural rule is that state-sensitive visual embodiment and stable structural part binding must remain distinguishable, inspectable, and portable.
</p>
