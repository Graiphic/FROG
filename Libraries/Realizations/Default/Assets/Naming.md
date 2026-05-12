<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization Asset Naming</h1>

<p align="center">
  <strong>Normative naming posture for assets of the official <code>Default</code> widget realization family</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#naming-goals">2. Naming Goals</a></li>
  <li><a href="#widget-directory-naming">3. Widget Directory Naming</a></li>
  <li><a href="#visual-part-directory-naming">4. Visual Part Directory Naming</a></li>
  <li><a href="#placement-directory-naming">5. Placement Directory Naming</a></li>
  <li><a href="#state-file-naming">6. State File Naming</a></li>
  <li><a href="#compound-state-naming">7. Compound State Naming</a></li>
  <li><a href="#auxiliary-file-naming">8. Auxiliary File Naming</a></li>
  <li><a href="#resource-id-alignment">9. Resource-ID Alignment</a></li>
  <li><a href="#prohibited-drift">10. Prohibited Drift</a></li>
  <li><a href="#examples">11. Examples</a></li>
  <li><a href="#summary">12. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the naming posture for assets in the official <code>Default</code> realization family.
</p>

<p>
Its purpose is to ensure that paths, filenames, part names, placement-resource names, and resource identifiers remain stable and predictable across:
</p>

<ul>
  <li>documentation,</li>
  <li>machine-readable realization packages,</li>
  <li>runtime loaders,</li>
  <li>future asset generation workflows.</li>
</ul>

<p>
The naming posture is intentionally designed to preserve the distinction between:
</p>

<ul>
  <li>semantic public surfaces owned by widget class law,</li>
  <li>visual resource layers owned by realization assets,</li>
  <li>placement-support resources such as anchors or text regions,</li>
  <li>runtime-side rendering of live dynamic content.</li>
</ul>

<hr/>

<h2 id="naming-goals">2. Naming Goals</h2>

<p>
The naming posture is designed to be:
</p>

<ul>
  <li>simple,</li>
  <li>stable,</li>
  <li>machine-friendly,</li>
  <li>human-readable,</li>
  <li>aligned with published class and part names,</li>
  <li>explicit about the difference between visual resources and placement resources.</li>
</ul>

<hr/>

<h2 id="widget-directory-naming">3. Widget Directory Naming</h2>

<p>
Widget-class directories SHOULD use the terminal class identifier in lowercase snake_case.
</p>

<p>
Examples:
</p>

<ul>
  <li><code>button</code></li>
  <li><code>numeric_control</code></li>
  <li><code>numeric_indicator</code></li>
  <li><code>boolean_control</code></li>
  <li><code>boolean_indicator</code></li>
  <li><code>string_control</code></li>
  <li><code>string_indicator</code></li>
  <li><code>waveform_chart</code></li>
</ul>

<p>
The asset tree MUST NOT use fully qualified class identifiers as directory names.
It MUST NOT use host-toolkit class names in place of standardized FROG class identifiers.
</p>

<hr/>

<h2 id="visual-part-directory-naming">4. Visual Part Directory Naming</h2>

<p>
Directories that contain visual state-bearing resources SHOULD use the published realized visual-part identifier exactly, in lowercase snake_case when that is the part identifier.
</p>

<p>
Examples:
</p>

<ul>
  <li><code>face</code></li>
  <li><code>frame</code></li>
  <li><code>value_display</code></li>
  <li><code>increment_button</code></li>
  <li><code>decrement_button</code></li>
  <li><code>state_face</code></li>
  <li><code>text_display</code></li>
  <li><code>plot_area</code></li>
  <li><code>x_axis</code></li>
  <li><code>y_axis</code></li>
</ul>

<p>
The asset tree MUST NOT silently rename visual parts away from the published realization or class model.
</p>

<p>
A visual-part directory MUST NOT be introduced merely because a semantic public part exists.
If a public part is realized primarily through a placement-support resource rather than through a distinct visual layer, the asset tree SHOULD use a placement directory instead of inventing a misleading visual subtree.
</p>

<hr/>

<h2 id="placement-directory-naming">5. Placement Directory Naming</h2>

<p>
Directories that contain placement-support resources SHOULD use an explicit role-oriented name rather than pretending to be visual part directories.
</p>

<p>
Recommended placement directory names are:
</p>

<ul>
  <li><code>anchors</code></li>
  <li><code>text_regions</code></li>
  <li><code>clip_regions</code></li>
</ul>

<p>
Examples:
</p>

<ul>
  <li><code>assets/button/anchors/</code></li>
  <li><code>assets/numeric_control/text_regions/</code></li>
  <li><code>assets/waveform_chart/anchors/</code></li>
</ul>

<p>
Within such directories, filenames SHOULD identify the target semantic or realized surface directly.
</p>

<p>
Examples:
</p>

<ul>
  <li><code>label.json</code></li>
  <li><code>value.json</code></li>
  <li><code>title.json</code></li>
  <li><code>x_axis_label.json</code></li>
  <li><code>y_axis_label.json</code></li>
</ul>

<p>
This posture is preferred over hiding placement metadata under unrelated visual directories.
For example, a button label anchor SHOULD be published as <code>assets/button/anchors/label.json</code> rather than as <code>assets/button/face/anchor_map.json</code> when the actual target is the semantic label surface rather than the face visual layer.
</p>

<hr/>

<h2 id="state-file-naming">6. State File Naming</h2>

<p>
A state-specific primary SVG asset SHOULD use:
</p>

<pre><code>&lt;state&gt;.svg</code></pre>

<p>
Examples:
</p>

<ul>
  <li><code>normal.svg</code></li>
  <li><code>disabled.svg</code></li>
  <li><code>focused.svg</code></li>
  <li><code>pressed.svg</code></li>
</ul>

<p>
The state name MUST match a published realization state.
</p>

<p>
State filenames SHOULD remain minimal and SHOULD NOT encode extra unpublished meaning.
</p>

<hr/>

<h2 id="compound-state-naming">7. Compound State Naming</h2>

<p>
When the state expresses more than one published realization dimension, the filename MAY use a compound state name:
</p>

<pre><code>&lt;state_a&gt;_&lt;state_b&gt;.svg</code></pre>

<p>
Examples:
</p>

<ul>
  <li><code>normal_true.svg</code></li>
  <li><code>normal_false.svg</code></li>
  <li><code>disabled_true.svg</code></li>
  <li><code>disabled_false.svg</code></li>
</ul>

<p>
Compound state names SHOULD remain explicit and finite.
They SHOULD NOT become a hidden multi-axis combinatorial system without published realization support.
</p>

<hr/>

<h2 id="auxiliary-file-naming">8. Auxiliary File Naming</h2>

<p>
Auxiliary support files fall into two naming styles, each with its own role.
</p>

<p>
First, generic support-map filenames MAY be used when a directory clearly scopes the file:
</p>

<ul>
  <li><code>layer_map.json</code></li>
  <li><code>style_token_map.json</code></li>
  <li><code>anchor_map.json</code></li>
  <li><code>text_region_map.json</code></li>
</ul>

<p>
Second, target-specific placement filenames SHOULD be preferred when they make the target surface more explicit:
</p>

<ul>
  <li><code>anchors/label.json</code></li>
  <li><code>anchors/value.json</code></li>
  <li><code>anchors/title.json</code></li>
  <li><code>text_regions/text_value.json</code></li>
</ul>

<p>
The default-family preference is:
</p>

<ul>
  <li>use generic names such as <code>layer_map.json</code> for directory-local support files whose scope is already obvious,</li>
  <li>use target-specific names such as <code>anchors/label.json</code> when the file publishes a placement surface for a dynamic public or realized surface.</li>
</ul>

<p>
This helps prevent a generic filename from obscuring which semantic or realized surface is actually being bound.
</p>

<hr/>

<h2 id="resource-id-alignment">9. Resource-ID Alignment</h2>

<p>
Filesystem paths, publication resource identifiers, and realization binding targets SHOULD align naturally, even when they are not textually identical.
</p>

<p>
Recommended posture:
</p>

<ul>
  <li>filesystem path expresses the asset-tree location,</li>
  <li>resource ID expresses the published machine-readable identity,</li>
  <li>binding target expresses the realization-side target surface.</li>
</ul>

<p>
Examples:
</p>

<ul>
  <li>path: <code>./assets/button/face/pressed.svg</code></li>
  <li>resource ID: <code>button.face.pressed.svg</code></li>
  <li>path: <code>./assets/button/anchors/label.json</code></li>
  <li>resource ID: <code>button.label.anchor_map</code></li>
  <li>anchor ID: <code>button.label.center</code></li>
</ul>

<p>
The naming system MUST NOT require the runtime to guess that unrelated names refer to the same published concept.
At the same time, the filesystem does not need to flatten every publication identifier into a literal filename.
Clear structural correspondence is sufficient.
</p>

<hr/>

<h2 id="prohibited-drift">10. Prohibited Drift</h2>

<p>
The following drift patterns SHOULD be avoided:
</p>

<ul>
  <li>renaming published parts only in the asset tree,</li>
  <li>using host-toolkit terminology instead of published FROG terminology,</li>
  <li>encoding unpublished semantic meaning into filenames,</li>
  <li>using multiple competing spellings for the same state,</li>
  <li>using mixed casing without an explicit convention,</li>
  <li>publishing dynamic public text as if it were owned by state-specific SVG label files when the realization contract actually publishes a placement surface,</li>
  <li>storing placement metadata under misleading visual directories whose names suggest a different ownership surface.</li>
</ul>

<hr/>

<h2 id="examples">11. Examples</h2>

<p>
Valid examples:
</p>

<ul>
  <li><code>assets/button/face/pressed.svg</code></li>
  <li><code>assets/button/frame/focused.svg</code></li>
  <li><code>assets/button/anchors/label.json</code></li>
  <li><code>assets/numeric_control/increment_button/pressed.svg</code></li>
  <li><code>assets/boolean_control/state_face/disabled_true.svg</code></li>
  <li><code>assets/string_control/text_display/focused.svg</code></li>
  <li><code>assets/waveform_chart/anchors/title.json</code></li>
</ul>

<p>
Conditionally valid examples:
</p>

<ul>
  <li><code>assets/button/face/anchor_map.json</code> only when the map truly belongs to <code>face</code> as a visual layer rather than to <code>label</code> as a placement surface,</li>
  <li><code>assets/string_control/text_display/text_region_map.json</code> when the published realization contract makes <code>text_display</code> the true owner of that placement support file.</li>
</ul>

<p>
Invalid drift examples:
</p>

<ul>
  <li><code>assets/button/mainSurface/down.svg</code></li>
  <li><code>assets/numeric_control/upArrow/pushed.svg</code></li>
  <li><code>assets/boolean_control/stateFace/onDisabled.svg</code></li>
  <li><code>assets/button/label/normal.svg</code> when the realization contract actually publishes the label through <code>anchors/label.json</code></li>
</ul>

<hr/>

<h2 id="summary">12. Summary</h2>

<p>
The default-family naming posture is simple:
</p>

<ul>
  <li>widget directory = terminal class identifier,</li>
  <li>visual part directory = published realized visual-part identifier,</li>
  <li>placement directory = explicit role directory such as <code>anchors</code> or <code>text_regions</code>,</li>
  <li>state file = published realization state,</li>
  <li>placement file = explicit target surface name,</li>
  <li>auxiliary support files = normalized names when their local scope is already clear.</li>
</ul>

<p>
This keeps the asset layer aligned with the published realization and class layers above it while preserving the distinction between semantic public surfaces, placement resources, and visual assets.
</p>
