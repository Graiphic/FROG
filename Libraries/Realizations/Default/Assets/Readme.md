<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization Assets</h1>

<p align="center">
  <strong>Normative asset-tree posture for the official <code>Default</code> widget realization family</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#why-this-directory-exists">2. Why this Directory Exists</a></li>
  <li><a href="#scope">3. Scope</a></li>
  <li><a href="#asset-tree-principles">4. Asset Tree Principles</a></li>
  <li><a href="#top-level-organization">5. Top-Level Organization</a></li>
  <li><a href="#part-directories-and-placement-directories">6. Part Directories and Placement Directories</a></li>
  <li><a href="#state-organization">7. State Organization</a></li>
  <li><a href="#supported-asset-kinds">8. Supported Asset Kinds</a></li>
  <li><a href="#semantic-svg-skin-posture">9. Semantic SVG Skin Posture</a></li>
  <li><a href="#recommended-tree-shape">10. Recommended Tree Shape</a></li>
  <li><a href="#examples">11. Examples</a></li>
  <li><a href="#fallback-posture">12. Fallback Posture</a></li>
  <li><a href="#validation-posture">13. Validation Posture</a></li>
  <li><a href="#summary">14. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This directory defines the normative asset-tree posture for the official <code>Default</code> realization family.
</p>

<p>
Its purpose is to keep realization resources organized, explicit, and machine-readable-friendly without forcing the asset layer to become the owner of widget semantics.
</p>

<p>
The asset tree is the correct place for:
</p>

<ul>
  <li>state-specific SVG resources,</li>
  <li>part-specific visual resources,</li>
  <li>placement-support resources such as anchors or text regions,</li>
  <li>optional layer maps,</li>
  <li>optional style-token maps.</li>
</ul>

<p>
The asset tree is <strong>not</strong> the correct place to redefine widget-class meaning, public properties, public methods, events, or bounded behavior law.
</p>

<hr/>

<h2 id="why-this-directory-exists">2. Why this Directory Exists</h2>

<p>
Once the default realization family is published and machine-readable realization packages exist, the next source of drift is almost always the asset tree itself.
</p>

<p>
Without a normalized asset tree:
</p>

<ul>
  <li>different realizations use incompatible folder shapes,</li>
  <li>part names drift from class-law names,</li>
  <li>state resources become ambiguous,</li>
  <li>placement surfaces become hidden conventions instead of published resources,</li>
  <li>runtime loaders end up depending on private filename guesses,</li>
  <li>machine-readable publication becomes harder to validate.</li>
</ul>

<p>
This document prevents that drift by defining one official organization posture.
</p>

<hr/>

<h2 id="scope">3. Scope</h2>

<p>
This document defines:
</p>

<ul>
  <li>the official asset-tree organization of the <code>Default</code> realization family,</li>
  <li>how widget-class assets are grouped,</li>
  <li>how part-specific visual assets are grouped,</li>
  <li>how placement-support resources are grouped,</li>
  <li>how state-specific assets are grouped,</li>
  <li>which auxiliary support files may appear.</li>
</ul>

<p>
This document does not define:
</p>

<ul>
  <li>widget class law,</li>
  <li>generic realization doctrine,</li>
  <li>machine-readable package structure in full,</li>
  <li>the content of any specific SVG file,</li>
  <li>one mandatory runtime asset-loading algorithm.</li>
</ul>

<hr/>

<h2 id="asset-tree-principles">4. Asset Tree Principles</h2>

<p>
The asset tree for the default family follows the following principles:
</p>

<ul>
  <li><strong>Class-first organization</strong> — assets are grouped first by target widget class.</li>
  <li><strong>Role-explicit organization</strong> — visual parts, placement resources, and auxiliary support files are not silently mixed together.</li>
  <li><strong>State-explicit organization</strong> — state-sensitive visual assets are grouped under explicit state names.</li>
  <li><strong>No semantic guessing</strong> — runtime loaders SHOULD NOT need to infer widget law from the asset tree.</li>
  <li><strong>Machine-readable friendliness</strong> — paths should align naturally with realization package resource records.</li>
  <li><strong>No semantic capture by SVG</strong> — dynamic public text and similar dynamic public surfaces must not become asset-owned semantics merely because SVG files exist nearby.</li>
</ul>

<hr/>

<h2 id="top-level-organization">5. Top-Level Organization</h2>

<p>
The default family asset tree SHOULD be organized first by standardized widget class family.
</p>

<p>
Recommended top-level shape:
</p>

<pre><code>assets/
  button/
  numeric_control/
  numeric_indicator/
  boolean_control/
  boolean_indicator/
  string_control/
  string_indicator/
  waveform_chart/
</code></pre>

<p>
The folder names SHOULD correspond to the terminal class identifier rather than the fully qualified class name.
</p>

<hr/>

<h2 id="part-directories-and-placement-directories">6. Part Directories and Placement Directories</h2>

<p>
The default family distinguishes at least three different asset scopes:
</p>

<ul>
  <li><strong>part visual directories</strong> — for state-sensitive or otherwise visual resources of realized parts,</li>
  <li><strong>placement directories</strong> — for anchors, text regions, clipping regions, or equivalent placement-support resources,</li>
  <li><strong>widget-level support files</strong> — for manifests, layer maps, or equivalent auxiliary records.</li>
</ul>

<p>
This distinction is important because not every public part should automatically map to a dedicated SVG subtree.
Some public parts are primarily dynamic semantic surfaces and therefore need a placement resource rather than a baked final visual asset subtree.
</p>

<p>
For example, the standard button realization may publish:
</p>

<pre><code>assets/
  button/
    face/
    frame/
    anchors/
</code></pre>

<p>
In this posture:
</p>

<ul>
  <li><code>face/</code> contains state-sensitive visual resources,</li>
  <li><code>frame/</code> contains optional visual framing resources when the realization family exposes them,</li>
  <li><code>anchors/</code> contains placement resources such as <code>label.json</code> for live text placement.</li>
</ul>

<p>
The default button posture therefore does <strong>not</strong> require a <code>label/</code> SVG directory when the semantic button label is rendered dynamically through a published anchor or text-region resource.
</p>

<p>
A part directory such as <code>label/</code> MAY still exist in another widget family or another realization when the published contract truly requires a distinct visual resource layer for that part.
However, such a directory must not be used to smuggle semantic ownership of dynamic public text into the asset layer.
</p>

<hr/>

<h2 id="state-organization">7. State Organization</h2>

<p>
Within a visual part directory, resources SHOULD be grouped by explicit state.
</p>

<p>
Recommended posture:
</p>

<pre><code>assets/
  button/
    face/
      normal.svg
      disabled.svg
      focused.svg
      pressed.svg
</code></pre>

<p>
For families that encode value posture in the state itself, the state name MAY be compound:
</p>

<pre><code>assets/
  boolean_control/
    state_face/
      normal_false.svg
      normal_true.svg
      disabled_false.svg
      disabled_true.svg
</code></pre>

<p>
Placement-support directories do not need to mirror the state structure unless the realization explicitly publishes state-sensitive placement resources.
In the common case, one placement resource is sufficient and the state-specific visual posture remains owned by the corresponding visual part directories.
</p>

<hr/>

<h2 id="supported-asset-kinds">8. Supported Asset Kinds</h2>

<p>
The default family asset tree MAY contain the following file kinds:
</p>

<ul>
  <li><code>.svg</code> — primary scalable visual resources,</li>
  <li><code>.json</code> — auxiliary maps such as anchors, text regions, layers, or style tokens.</li>
</ul>

<p>
Recommended auxiliary support files include:
</p>

<ul>
  <li><code>anchor_map.json</code></li>
  <li><code>text_region_map.json</code></li>
  <li><code>layer_map.json</code></li>
  <li><code>style_token_map.json</code></li>
</ul>

<p>
Realization packages MAY also choose more specific filenames when this improves clarity at the asset level, such as:
</p>

<ul>
  <li><code>anchors/label.json</code>,</li>
  <li><code>anchors/value_display.json</code>,</li>
  <li><code>anchors/title.json</code>.</li>
</ul>

<p>
These files remain realization support artifacts.
They do not become semantic truth.
</p>

<hr/>

<h2 id="semantic-svg-skin-posture">9. Semantic SVG Skin Posture</h2>

<p>
SVG templates in this asset tree should be authored as semantic skins.
A skin may be visually restyled, but its public meaning is carried by stable
markers and realization records rather than by private coordinate conventions.
See <a href="../Semantic SVG Skin Contract.md">Semantic SVG Skin Contract</a>.
</p>

<p>
An asset README or corresponding realization document should identify the
required and optional parts for the widget family, the dynamic text/value
regions, and the resize policy for surfaces that stretch, remain fixed, scale
proportionally, move as solidary groups, repeat, or clip child content.
</p>

<p>
This keeps the asset tree friendly to derived skins: a new visual style can be
introduced by preserving the same semantic markers and resize contract, while a
true contract change becomes visible as a new realization variant or deliberate
documentation update.
</p>

<hr/>

<h2 id="recommended-tree-shape">10. Recommended Tree Shape</h2>

<p>
The recommended official asset-tree shape is:
</p>

<pre><code>assets/
  button/
    asset_manifest.json
    face/
      normal.svg
      disabled.svg
      focused.svg
      pressed.svg
      layer_map.json
    frame/
      normal.svg
      focused.svg
    anchors/
      label.json

  numeric_control/
    asset_manifest.json
    value_display/
      normal.svg
      disabled.svg
      focused.svg
    increment_button/
      normal.svg
      pressed.svg
    decrement_button/
      normal.svg
      pressed.svg
    frame/
      normal.svg
      focused.svg
    anchors/
      label.json
      value.json

  boolean_control/
    asset_manifest.json
    state_face/
      normal_false.svg
      normal_true.svg
      disabled_false.svg
      disabled_true.svg
    anchors/
      label.json

  string_control/
    asset_manifest.json
    text_display/
      normal.svg
      disabled.svg
      focused.svg
    frame/
      normal.svg
      focused.svg
    anchors/
      label.json
      text_value.json

  waveform_chart/
    asset_manifest.json
    plot_area/
      normal.svg
      focused.svg
    x_axis/
      normal.svg
    y_axis/
      normal.svg
    frame/
      normal.svg
      focused.svg
    anchors/
      title.json
      x_axis_label.json
      y_axis_label.json
</code></pre>

<p>
This shape is illustrative but normative in posture:
</p>

<ul>
  <li>visual state-bearing parts are published as visual resource directories,</li>
  <li>dynamic text-bearing or equivalent dynamic public surfaces are preferably published through placement-support resources,</li>
  <li>auxiliary support files remain explicit and inspectable.</li>
</ul>

<hr/>

<h2 id="examples">11. Examples</h2>

<p>
Example resource paths:
</p>

<ul>
  <li><code>./assets/button/face/pressed.svg</code></li>
  <li><code>./assets/button/anchors/label.json</code></li>
  <li><code>./assets/numeric_control/increment_button/pressed.svg</code></li>
  <li><code>./assets/boolean_control/state_face/normal_true.svg</code></li>
  <li><code>./assets/string_control/text_display/focused.svg</code></li>
  <li><code>./assets/waveform_chart/plot_area/normal.svg</code></li>
</ul>

<p>
Example placement-support paths:
</p>

<ul>
  <li><code>./assets/button/anchors/label.json</code></li>
  <li><code>./assets/numeric_control/anchors/value.json</code></li>
  <li><code>./assets/string_control/anchors/text_value.json</code></li>
  <li><code>./assets/waveform_chart/anchors/title.json</code></li>
</ul>

<hr/>

<h2 id="fallback-posture">12. Fallback Posture</h2>

<p>
When a state-specific resource is missing, runtimes MAY apply published fallback rules.
</p>

<p>
However, the asset tree itself SHOULD remain complete enough that default-family realizations do not rely primarily on hidden fallback heuristics.
</p>

<p>
Likewise, when a preferred placement-support resource is unavailable, runtimes MAY apply the published fallback posture of the realization package, but that fallback must remain inspectable at the publication layer rather than becoming a private loader convention.
</p>

<hr/>

<h2 id="validation-posture">13. Validation Posture</h2>

<p>
Validators SHOULD diagnose at least:
</p>

<ul>
  <li>asset paths that do not match declared realization resources,</li>
  <li>directories named with non-published part identifiers or non-published placement roles,</li>
  <li>state resource filenames that do not correspond to declared realization states,</li>
  <li>placement-support files attached to unknown parts,</li>
  <li>visual directories that appear to claim ownership of dynamic public text without a matching realization contract,</li>
  <li>SVG-backed widget assets that lack a reviewable semantic skin contract for required parts, dynamic regions, resize posture, and solidary groups.</li>
</ul>

<hr/>

<h2 id="summary">14. Summary</h2>

<p>
The default-family asset tree is organized by:
</p>

<ul>
  <li>widget class,</li>
  <li>then realization role,</li>
  <li>then explicit state where applicable.</li>
</ul>

<p>
This keeps resources inspectable, stable, and aligned with machine-readable realization publication while preserving the distinction between semantic public surfaces, placement resources, and visual assets.
</p>
