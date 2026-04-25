<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 FROG IDE Snippet Specification</h1>

<p align="center">
Definition of image-backed IDE snippet artifacts for FROG authoring workflows<br/>
<em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#scope-for-v01">2. Scope for v0.1</a></li>
  <li><a href="#architectural-position-and-dependencies">3. Architectural Position and Dependencies</a></li>
  <li><a href="#design-principles">4. Design Principles</a></li>
  <li><a href="#what-a-snippet-is">5. What a Snippet Is</a></li>
  <li><a href="#snippet-vs-full-frog">6. Snippet vs Full FROG</a></li>
  <li><a href="#snippet-kinds">7. Snippet Kinds</a></li>
  <li><a href="#selection-scope-model">8. Selection Scope Model</a></li>
  <li><a href="#diagram-snippet-content">9. Diagram Snippet Content</a></li>
  <li><a href="#front-panel-snippet-content">10. Front Panel Snippet Content</a></li>
  <li><a href="#composite-snippets">11. Composite Snippets</a></li>
  <li><a href="#image-backed-snippet-model">12. Image-Backed Snippet Model</a></li>
  <li><a href="#embedded-payload-model">13. Embedded Payload Model</a></li>
  <li><a href="#boundary-endpoints-and-open-connections">14. Boundary Endpoints and Open Connections</a></li>
  <li><a href="#reference-closure-and-dependencies">15. Reference Closure and Dependencies</a></li>
  <li><a href="#preview-and-presentation">16. Preview and Presentation</a></li>
  <li><a href="#export-semantics">17. Export Semantics</a></li>
  <li><a href="#import-paste-and-drag-and-drop-semantics">18. Import, Paste, and Drag-and-Drop Semantics</a></li>
  <li><a href="#validation-and-safety-rules">19. Validation and Safety Rules</a></li>
  <li><a href="#illustrative-examples">20. Illustrative Examples</a></li>
  <li><a href="#out-of-scope-for-v01">21. Out of Scope for v0.1</a></li>
  <li><a href="#summary">22. Summary</a></li>
  <li><a href="#license">23. License</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the snippet model of a FROG IDE.
A snippet is a portable, image-backed IDE artifact that captures a reusable fragment of authoring content so that it may be:
</p>

<ul>
  <li>copied,</li>
  <li>shared,</li>
  <li>previewed as an ordinary image,</li>
  <li>dragged into a FROG IDE,</li>
  <li>or inserted into another editing context.</li>
</ul>

<p>
A snippet is not a full FROG program.
It is a source-aligned fragment of IDE-managed content derived from the FROG Program Model and packaged inside an image carrier that remains visually meaningful outside the IDE.
</p>

<p>
The core intent of a FROG snippet is therefore dual:
</p>

<ul>
  <li><strong>human-visible artifact</strong> — the snippet is viewable as an image in generic tools, file browsers, documentation, chat systems, and websites,</li>
  <li><strong>IDE-readable artifact</strong> — the same image contains structured embedded snippet payload data that a conforming FROG IDE can decode and insert through import, paste, or drag-and-drop.</li>
</ul>

<p>
This gives FROG a snippet workflow intentionally aligned with image-backed graphical-language reuse:
a user can see the snippet as an image everywhere, while a conforming FROG IDE can recover the actual structured reusable content from it.
</p>

<pre><code>Snippet model

authoring fragment
    -&gt; structured snippet payload
    -&gt; image-backed carrier
    -&gt; human preview outside IDE
    -&gt; structured recovery inside IDE

Never:

snippet image
    -&gt; pixel interpretation only
    -&gt; guessed semantics
</code></pre>

<hr/>

<h2 id="scope-for-v01">2. Scope for v0.1</h2>

<p>
For FROG v0.1, this document standardizes:
</p>

<ul>
  <li>what a snippet is,</li>
  <li>which kinds of authoring fragments may be captured,</li>
  <li>how snippet boundaries are represented,</li>
  <li>how references and required supporting objects are carried,</li>
  <li>that a standalone snippet artifact MUST be image-backed,</li>
  <li>that the image carrier MUST contain structured embedded snippet payload data readable by a conforming FROG IDE,</li>
  <li>how import, paste, and drag-and-drop SHOULD behave at the source-model level,</li>
  <li>how optional non-authoritative recoverability metadata MAY accompany carried content.</li>
</ul>

<p>
FROG v0.1 does <strong>not</strong> standardize:
</p>

<ul>
  <li>a mandatory clipboard MIME type,</li>
  <li>a mandatory operating-system drag-and-drop protocol,</li>
  <li>a mandatory online marketplace or package manager for snippet libraries,</li>
  <li>execution directly from a snippet,</li>
  <li>automatic semantic repair of invalid pasted fragments beyond defined validation rules,</li>
  <li>one universal binary embedding mechanism that every image technology must share.</li>
</ul>

<p>
Base v0.1 standardizes the <strong>image-backed requirement</strong> and the <strong>logical embedded payload model</strong>.
Stricter transport conventions MAY later freeze exact carrier encodings, chunk conventions, metadata slots,
integrity metadata, or file-extension rules.
</p>

<hr/>

<h2 id="architectural-position-and-dependencies">3. Architectural Position and Dependencies</h2>

<h3>3.1 Repository position</h3>

<p>
This document belongs in <code>IDE/</code> because it defines snippet capture, transport, preview, paste,
drag-and-drop, and reinsertion behavior as IDE authoring workflows.
It does <strong>not</strong> belong to <code>Expression/</code> because it does not define the canonical persisted form of a full FROG source unit.
</p>

<h3>3.2 Dependencies</h3>

<p>
This document complements the following specifications:
</p>

<ul>
  <li><code>IDE/Readme.md</code> — defines the role of the Program Model and the distinction between IDE artifacts and canonical source,</li>
  <li><code>IDE/Palette.md</code> — defines the active canonical insertable space into which carried snippet content is reinserted,</li>
  <li><code>IDE/Express.md</code> — defines Express authoring, canonical target identity, and IDE-side recoverability expectations for guided authoring,</li>
  <li><code>Expression/Readme.md</code> — defines the canonical source model and the optional source-level <code>ide</code> section,</li>
  <li><code>Expression/IDE preferences.md</code> — defines IDE-facing preferences and recoverability metadata carried in the optional source-level <code>ide</code> section,</li>
  <li><code>Expression/Diagram.md</code> — defines executable graph objects, layout metadata, annotations, and diagram-scope dependency references,</li>
  <li><code>Expression/Control structures.md</code> — defines structure nodes, owned regions, and the whole-structure representation that snippet capture must preserve when structures are selected,</li>
  <li><code>Expression/Front panel.md</code> — defines front-panel composition and widget-tree structure,</li>
  <li><code>Expression/Widget.md</code> — defines widget identity and widget object structure,</li>
  <li><code>Expression/Widget interaction.md</code> — defines the source-facing representation of <code>widget_value</code>, <code>widget_reference</code>, and object-style widget interaction,</li>
  <li><code>Libraries/UI.md</code> — defines the standardized <code>frog.ui.*</code> primitive identities and primitive-local behavior used by widget-related diagram content,</li>
  <li><code>Profiles/Readme.md</code> — defines the architectural role of optional standardized capability families that may surface in transported snippet content.</li>
</ul>

<h3>3.3 Ownership boundary</h3>

<p>
This document does not redefine the canonical <code>.frog</code> source format.
It defines how the IDE may serialize, embed, transport, preview, and reinsert fragments of source-aligned authoring content through an image-backed snippet artifact.
</p>

<p>
Accordingly:
</p>

<ul>
  <li><code>Expression/</code> remains authoritative for the carried source-level object model,</li>
  <li><code>Libraries/</code> remains authoritative for intrinsic primitive identity and primitive-local behavior,</li>
  <li><code>Profiles/</code> remains authoritative for optional profile-defined capability families,</li>
  <li><code>IDE/Palette.md</code> remains authoritative for the canonical insertable space of the target editor,</li>
  <li><code>IDE/Snippet.md</code> remains authoritative only for snippet transport, preview, embedding, and reinsertion behavior.</li>
</ul>

<pre><code>Ownership reminder

Expression/     -&gt; carried source-level object model
Libraries/      -&gt; intrinsic primitive identity
Profiles/       -&gt; optional capability families
IDE/Palette     -&gt; target insertable space
IDE/Snippet     -&gt; snippet transport and reinsertion behavior
</code></pre>

<hr/>

<h2 id="design-principles">4. Design Principles</h2>

<h3>4.1 Snippets are IDE artifacts</h3>

<p>
A snippet belongs to the IDE and authoring-workflow layer.
It is not the canonical persisted form of a full FROG program.
</p>

<h3>4.2 Snippets are source-aligned</h3>

<p>
Although snippets are IDE artifacts, their embedded contents MUST remain expressible in terms of source-level objects already defined elsewhere, such as:
</p>

<ul>
  <li>diagram nodes and edges,</li>
  <li>diagram annotations,</li>
  <li>structure-owned regions when a structure is carried,</li>
  <li>front-panel widgets,</li>
  <li>layout metadata,</li>
  <li>documentation metadata.</li>
</ul>

<h3>4.3 Standalone snippets must be image-backed</h3>

<p>
A standalone snippet artifact MUST be an image that:
</p>

<ul>
  <li>can be rendered by generic image-capable tools, and</li>
  <li>contains embedded structured snippet payload data readable by a conforming FROG IDE.</li>
</ul>

<h3>4.4 Embedded payload is authoritative</h3>

<p>
The visible image is essential for human workflow and browsing, but the IDE-authoritative meaning of a snippet is defined by its embedded structured payload, not by pixel interpretation alone.
</p>

<h3>4.5 Snippets must be paste-safe</h3>

<p>
A snippet MUST carry enough information for a target IDE to either:
</p>

<ul>
  <li>insert it deterministically,</li>
  <li>or reject it deterministically with an explicit reason.</li>
</ul>

<h3>4.6 Snippets do not invent semantics</h3>

<p>
A snippet MUST NOT introduce ad hoc execution semantics that do not already exist in the FROG language or Program Model.
It transports content; it does not redefine the language.
</p>

<h3>4.7 Reinsertion targets canonical content</h3>

<p>
A snippet MUST reinsert canonical content into the target Program Model.
It is a transport path into the same canonical insertable space, not a competing insertion language.
</p>

<h3>4.8 Recoverability is optional</h3>

<p>
A snippet MAY carry non-authoritative recoverability metadata related to Express-authored instances or other guided authoring flows.
However:
</p>

<ul>
  <li>such metadata MUST remain optional,</li>
  <li>such metadata MUST remain non-authoritative for execution semantics,</li>
  <li>loss of such metadata MUST NOT invalidate the carried canonical snippet content.</li>
</ul>

<h3>4.9 Capability support is not implied by transport</h3>

<p>
A snippet MAY carry intrinsic, profile-defined, or other implementation-supported content.
Transporting that content does not imply that every target IDE supports it.
Import validation remains responsible for checking whether required capabilities are actually available.
</p>

<h3>4.10 Minimal but extensible</h3>

<p>
FROG v0.1 defines a minimal snippet core.
Future transport conventions MAY define richer carrier encodings, previews, galleries, library conventions,
signing models, or cross-tool interchange standards.
</p>

<hr/>

<h2 id="what-a-snippet-is">5. What a Snippet Is</h2>

<p>
A snippet is a serialized authoring fragment captured from one editing context so that it may be inserted into another editing context through an image-backed transport artifact.
</p>

<p>
Conceptually, a snippet contains:
</p>

<ul>
  <li>a snippet kind,</li>
  <li>a payload containing source-aligned fragment content,</li>
  <li>boundary information for open connections or incomplete surroundings,</li>
  <li>presentation metadata used to render the snippet as an image,</li>
  <li>optional non-authoritative recoverability metadata,</li>
  <li>an image carrier that embeds the structured snippet payload.</li>
</ul>

<p>
A snippet is usually derived from a user selection, but the exact UI gesture used to create that selection is outside the scope of this document.
</p>

<hr/>

<h2 id="snippet-vs-full-frog">6. Snippet vs Full FROG</h2>

<p>
A full FROG program is a complete source-level unit described by the canonical FROG Expression.
A snippet is only a fragment.
</p>

<p>
Therefore, a snippet:
</p>

<ul>
  <li>MAY be incomplete relative to a full program,</li>
  <li>MAY have open boundaries,</li>
  <li>MAY omit unrelated program sections,</li>
  <li>MAY carry non-authoritative recoverability metadata,</li>
  <li>MUST NOT be treated as a complete executable FROG unless a future convention explicitly wraps it into a complete FROG.</li>
</ul>

<p>
A snippet is not a substitute for:
</p>

<ul>
  <li>a reusable library FROG,</li>
  <li>a dependency package,</li>
  <li>a canonical source file,</li>
  <li>an execution artifact.</li>
</ul>

<hr/>

<h2 id="snippet-kinds">7. Snippet Kinds</h2>

<p>
The canonical snippet kinds for FROG v0.1 are:
</p>

<ul>
  <li><code>diagram_snippet</code> — a fragment of one diagram scope,</li>
  <li><code>front_panel_snippet</code> — a fragment of a front-panel widget tree,</li>
  <li><code>composite_snippet</code> — a snippet carrying both diagram and front-panel content when they are semantically linked.</li>
</ul>

<p>
A <code>diagram_snippet</code> is the primary snippet kind for v0.1.
</p>

<hr/>

<h2 id="selection-scope-model">8. Selection Scope Model</h2>

<h3>8.1 Single-scope rule</h3>

<p>
A v0.1 snippet payload MUST be rooted in exactly one source scope for each carried content family.
For example:
</p>

<ul>
  <li>a diagram snippet is rooted in one diagram scope,</li>
  <li>a front-panel snippet is rooted in one widget-tree scope.</li>
</ul>

<h3>8.2 No mixed diagram scopes in one diagram payload</h3>

<p>
A single <code>diagram_snippet</code> payload MUST NOT directly mix executable objects taken from multiple diagram scopes.
For example, it MUST NOT combine objects from both:
</p>

<ul>
  <li>a parent diagram scope, and</li>
  <li>a nested structure-region scope</li>
</ul>

<p>
inside one flat diagram payload.
</p>

<h3>8.3 Whole-structure capture</h3>

<p>
If a selection includes a structure node as a selected object, the snippet MUST carry that structure node together with its owned regions and their source-aligned content.
</p>

<h3>8.4 Region-local snippet</h3>

<p>
A future stricter convention MAY allow creating a snippet from content inside one structure-owned region without selecting the containing structure itself.
In that case, the snippet root is that region-local diagram scope rather than the parent scope.
</p>

<hr/>

<h2 id="diagram-snippet-content">9. Diagram Snippet Content</h2>

<p>
A <code>diagram_snippet</code> MAY contain:
</p>

<ul>
  <li>selected nodes,</li>
  <li>selected internal edges between those nodes,</li>
  <li>selected annotations,</li>
  <li>node-local documentation fields,</li>
  <li>node tags,</li>
  <li>layout metadata relevant to pasted placement,</li>
  <li>snippet-local dependency references needed by selected <code>subfrog</code> nodes,</li>
  <li>optional non-authoritative recoverability metadata relevant to guided authoring reconstruction.</li>
</ul>

<p>
A diagram snippet MUST preserve source-level node kinds and kind-specific fields.
It MUST NOT rewrite a selected node into a different node kind merely for snippet transport.
</p>

<h3>9.1 Internal edges</h3>

<p>
An edge is internal to the snippet if both its source endpoint and destination endpoint are carried by the snippet payload.
Internal edges SHOULD be carried directly as ordinary snippet-owned edges.
</p>

<h3>9.2 Partial-edge crossings</h3>

<p>
An edge that crosses the snippet selection boundary is not fully internal.
Such a crossing MUST be represented through snippet boundary endpoints as defined later in this document.
</p>

<hr/>

<h2 id="front-panel-snippet-content">10. Front Panel Snippet Content</h2>

<p>
A <code>front_panel_snippet</code> MAY contain:
</p>

<ul>
  <li>one or more widget instances,</li>
  <li>widget subtrees,</li>
  <li>layout and composition metadata,</li>
  <li>styling metadata,</li>
  <li>widget-local configuration,</li>
  <li>optional non-authoritative recoverability metadata relevant to guided authoring reconstruction.</li>
</ul>

<p>
For v0.1, a front-panel snippet SHOULD preserve widget-tree structure rather than flattening it into unrelated widgets.
If a selected widget has selected descendants, their parent-child relationship MUST remain preserved.
</p>

<p>
A snippet MUST NOT silently break a widget subtree in a way that makes the resulting payload structurally ambiguous.
An IDE MAY therefore require subtree closure for certain widget selections.
</p>

<p>
A front-panel snippet MUST preserve widget instances using the canonical widget-instance field model defined by the front-panel and widget specifications rather than inventing a snippet-specific widget object shape.
</p>

<hr/>

<h2 id="composite-snippets">11. Composite Snippets</h2>

<p>
A <code>composite_snippet</code> carries both:
</p>

<ul>
  <li>a diagram fragment, and</li>
  <li>a related front-panel fragment.</li>
</ul>

<p>
This is especially useful when the diagram fragment contains:
</p>

<ul>
  <li><code>widget_value</code> nodes,</li>
  <li><code>widget_reference</code> nodes,</li>
  <li><code>frog.ui.*</code> widget-interaction primitives whose meaning depends on referenced widgets.</li>
</ul>

<p>
If a snippet carries widget-related diagram nodes and is intended to be portable across documents, it SHOULD include the required referenced widget definitions in a front-panel fragment rather than assuming they already exist in the target FROG.
</p>

<p>
When a composite snippet carries both diagram and front-panel content, all internal widget references between those carried content families MUST remain resolvable inside the snippet payload.
</p>

<p>
If a composite snippet also carries recoverability metadata, that metadata MUST remain subordinate to the carried canonical diagram and front-panel content.
</p>

<hr/>

<h2 id="image-backed-snippet-model">12. Image-Backed Snippet Model</h2>

<h3>12.1 General rule</h3>

<p>
A standalone FROG snippet artifact MUST be image-backed.
The image carrier is not optional in the base standalone workflow.
</p>

<h3>12.2 Required dual nature</h3>

<p>
The snippet image carrier MUST satisfy both of the following:
</p>

<ul>
  <li><strong>generic visibility</strong> — it MUST be displayable as an image outside the FROG IDE,</li>
  <li><strong>structured recoverability</strong> — it MUST contain embedded snippet payload data that a conforming FROG IDE can recover without relying on manual visual interpretation.</li>
</ul>

<h3>12.3 Authoritative meaning</h3>

<p>
The embedded structured payload is authoritative for import, drag-and-drop insertion, validation, and reconstruction.
The rendered image is authoritative for visual presentation only.
</p>

<h3>12.4 Image-carrier intent</h3>

<p>
The image carrier exists so that a snippet can be:
</p>

<ul>
  <li>dropped into chats, emails, wikis, presentations, and documents as an image,</li>
  <li>browsed visually in folders and asset collections,</li>
  <li>dragged from a generic image-aware environment into a FROG IDE,</li>
  <li>decoded by the IDE into structured reusable authoring content.</li>
</ul>

<h3>12.5 Carrier convention freedom</h3>

<p>
Base v0.1 requires an image-backed carrier but does not require one unique binary embedding technique across all environments.
A stricter transport convention MAY freeze:
</p>

<ul>
  <li>the exact raster format,</li>
  <li>the exact file extension convention,</li>
  <li>the exact metadata or chunk location for embedded payloads,</li>
  <li>signature or integrity metadata.</li>
</ul>

<hr/>

<h2 id="embedded-payload-model">13. Embedded Payload Model</h2>

<h3>13.1 Conceptual top-level payload</h3>

<p>
The embedded payload of a snippet image is conceptually:
</p>

<pre><code class="language-json">{
  "kind": "frog_snippet",
  "version": "0.1",
  "snippet_kind": "diagram_snippet | front_panel_snippet | composite_snippet",
  "diagram_fragment": { },
  "front_panel_fragment": { },
  "boundaries": { },
  "dependencies": { },
  "preview": { },
  "metadata": { },
  "recoverability": { },
  "provenance": { }
}</code></pre>

<h3>13.2 Rules</h3>

<ul>
  <li><code>kind</code> MUST be <code>"frog_snippet"</code>,</li>
  <li><code>version</code> MUST identify the snippet-format version,</li>
  <li><code>snippet_kind</code> MUST identify the snippet category,</li>
  <li><code>diagram_fragment</code> MUST be present for <code>diagram_snippet</code> and <code>composite_snippet</code>,</li>
  <li><code>front_panel_fragment</code> MUST be present for <code>front_panel_snippet</code> and <code>composite_snippet</code>.</li>
</ul>

<p>
Auxiliary sections such as <code>preview</code>, <code>metadata</code>, and <code>recoverability</code> MAY be present provided that they remain non-authoritative with respect to source-aligned snippet meaning.
</p>

<p>
A snippet payload MAY also carry provenance evidence when the relationship between the original source objects and the transported fragment remains clear. During insertion, the IDE SHOULD either preserve valid provenance with explicit imported context, mark the inserted objects as <code>snippet_inserted</code> or <code>imported</code>, or require a fresh human-review attestation after the fragment is placed in the target program.
</p>

<h3>13.3 Embedded payload requirement</h3>

<p>
A conforming standalone snippet image MUST embed a recoverable payload equivalent in information content to the logical model above.
An image that merely resembles a snippet visually but lacks recoverable structured payload is not a valid standalone FROG snippet artifact.
</p>

<h3>13.4 Payload vs rendered image</h3>

<p>
The rendered image MAY be regenerated from the payload.
The payload MUST NOT be reconstructed by lossy visual analysis of the rendered image.
</p>

<h3>13.5 Recoverability metadata</h3>

<p>
If an embedded snippet payload carries recoverability metadata:
</p>

<ul>
  <li>that metadata MUST remain optional,</li>
  <li>that metadata MUST remain non-authoritative for canonical meaning,</li>
  <li>its absence or loss MUST NOT invalidate the carried canonical snippet content,</li>
  <li>it SHOULD help a conforming IDE reopen or preserve guided authoring presentation only when that is safely possible.</li>
</ul>

<hr/>

<h2 id="boundary-endpoints-and-open-connections">14. Boundary Endpoints and Open Connections</h2>

<h3>14.1 Purpose</h3>

<p>
A snippet is often created from a larger graph fragment that was connected to surrounding content.
Those external relationships must be represented without pretending that the snippet is a full closed diagram.
</p>

<h3>14.2 Boundary endpoints</h3>

<p>
FROG v0.1 defines conceptual snippet boundary endpoints:
</p>

<ul>
  <li><code>snippet_input</code> — a value or sequencing dependency expected from outside the snippet,</li>
  <li><code>snippet_output</code> — a value or sequencing dependency produced by the snippet for use outside the snippet.</li>
</ul>

<p>
These are snippet-level transport concepts, not canonical node kinds of the FROG source language.
They exist only inside the snippet payload model.
</p>

<h3>14.3 Meaning</h3>

<p>
A snippet boundary endpoint represents an open connection between the carried fragment and content that was not included in the snippet.
When the snippet is imported or dropped, the IDE MAY:
</p>

<ul>
  <li>leave the corresponding boundary as an unwired open endpoint,</li>
  <li>offer reconnection assistance,</li>
  <li>or map it to a suitable target connection if the insertion operation explicitly defines one.</li>
</ul>

<h3>14.4 Type information</h3>

<p>
When type information is resolvable from the source fragment, a snippet boundary endpoint SHOULD carry that type information so that insertion validation can remain deterministic.
</p>

<hr/>

<h2 id="reference-closure-and-dependencies">15. Reference Closure and Dependencies</h2>

<h3>15.1 General rule</h3>

<p>
A snippet SHOULD carry the minimum supporting information needed to remain meaningful after transport.
</p>

<h3>15.2 Primitive references</h3>

<p>
Primitive nodes are carried by their canonical type identifiers such as <code>frog.core.add</code> or <code>frog.ui.property_read</code>.
A snippet does not inline primitive-library definitions.
</p>

<h3>15.3 Sub-FROG dependencies</h3>

<p>
If a snippet contains <code>subfrog</code> nodes, it SHOULD carry the relevant dependency references needed to keep those <code>subfrog.ref</code> identifiers meaningful at insertion time.
A snippet does not inline the full referenced dependent FROGs.
</p>

<h3>15.4 Profile-defined capabilities</h3>

<p>
If a snippet contains profile-defined entries, it SHOULD carry enough capability-identification information to allow the target IDE to validate whether the required profile support is available.
A snippet does not convert optional profile-defined content into unconditional core content.
</p>

<h3>15.5 Widget references</h3>

<p>
If a snippet contains <code>widget_value</code> or <code>widget_reference</code> nodes and is intended to remain portable across documents,
it SHOULD carry the required widget definitions through a front-panel fragment or equivalent widget payload.
</p>

<h3>15.6 Composite internal closure</h3>

<p>
If a composite snippet carries both diagram-side widget references and front-panel widget definitions,
those internal references MUST remain consistent after identifier regeneration and insertion.
</p>

<h3>15.7 No unrelated closure</h3>

<p>
A snippet SHOULD NOT capture unrelated surrounding program content merely to avoid open boundaries.
Closure should be minimal and semantically relevant.
</p>

<h3>15.8 Optional recoverability closure</h3>

<p>
A snippet MAY carry recoverability metadata that helps preserve or restore the relationship between a carried fragment and an Express-authored presentation.
Such metadata MUST NOT replace the requirement to carry the actual canonical content needed for insertion.
</p>

<hr/>

<h2 id="preview-and-presentation">16. Preview and Presentation</h2>

<h3>16.1 Preview role</h3>

<p>
Because the snippet itself is image-backed, preview is not an optional afterthought.
The visible image is a first-class workflow surface of the snippet artifact.
</p>

<h3>16.2 Visible image expectations</h3>

<p>
The rendered snippet image SHOULD make the snippet identifiable to a human viewer.
Depending on snippet kind, this MAY include:
</p>

<ul>
  <li>a diagram thumbnail or cropped fragment view,</li>
  <li>a front-panel thumbnail,</li>
  <li>a composite preview of diagram and front panel,</li>
  <li>a compact title or label,</li>
  <li>lightweight visual hints for open boundaries.</li>
</ul>

<h3>16.3 Human-visible but non-authoritative rendering</h3>

<p>
The rendered image SHOULD be visually useful and SHOULD correspond to the embedded payload.
However, if any conflict exists between the visible rendering and the embedded payload, the embedded payload remains authoritative.
</p>

<h3>16.4 No semantic dependence on OCR or computer vision</h3>

<p>
A conforming IDE MUST NOT require OCR, image understanding, or manual tracing to recover snippet meaning from a valid snippet image.
The structured embedded payload is mandatory precisely to avoid that ambiguity.
</p>

<hr/>

<h2 id="export-semantics">17. Export Semantics</h2>

<h3>17.1 Export operation</h3>

<p>
When an IDE exports a snippet, it MUST:
</p>

<ul>
  <li>capture the selected source-aligned fragment,</li>
  <li>normalize it into one of the canonical snippet kinds,</li>
  <li>compute boundary endpoints for cut external connections,</li>
  <li>attach minimal required dependencies,</li>
  <li>generate a human-visible rendered image,</li>
  <li>embed the structured snippet payload into that image-backed carrier.</li>
</ul>

<p>
If the exported selection also contains optional recoverability state, the IDE MAY embed corresponding non-authoritative recoverability metadata in the snippet payload.
</p>

<h3>17.2 Stable rendering intent</h3>

<p>
The visible rendering SHOULD remain stable enough that repeated exports of the same snippet produce recognizably similar images, subject to convention-defined rendering policies.
</p>

<h3>17.3 Export safety</h3>

<p>
An IDE MUST NOT export an image as a valid snippet artifact if:
</p>

<ul>
  <li>the structured payload is missing,</li>
  <li>the payload is incomplete for the chosen snippet kind,</li>
  <li>required dependencies are intentionally omitted in a way that makes deterministic import impossible.</li>
</ul>

<hr/>

<h2 id="import-paste-and-drag-and-drop-semantics">18. Import, Paste, and Drag-and-Drop Semantics</h2>

<h3>18.1 General rule</h3>

<p>
A conforming FROG IDE SHOULD treat a dropped or imported image as a candidate snippet artifact when the environment supplies an image object or file.
</p>

<h3>18.2 Decode-first behavior</h3>

<p>
On snippet import, paste, or drag-and-drop, the IDE MUST first attempt to decode embedded structured snippet payload from the image-backed carrier.
</p>

<p>
If decoding succeeds and validation succeeds, the IDE SHOULD proceed with snippet insertion semantics.
If decoding fails, the IDE MUST NOT pretend that the image is a valid snippet.
It MAY instead:
</p>

<ul>
  <li>reject the operation,</li>
  <li>treat the dropped object as an ordinary image asset if the editor context supports that,</li>
  <li>report that the image is viewable but not a valid FROG snippet.</li>
</ul>

<h3>18.3 Insertion semantics</h3>

<p>
When inserting a valid snippet payload, the IDE SHOULD:
</p>

<ul>
  <li>reinsert canonical carried content into the target Program Model,</li>
  <li>regenerate local identifiers as required to avoid collisions,</li>
  <li>preserve internal connectivity and internal ownership relationships,</li>
  <li>preserve internal cross-references between carried content families,</li>
  <li>preserve carried layout relationships as far as practical,</li>
  <li>leave external boundaries open unless the insertion operation provides an explicit reconnection rule,</li>
  <li>preserve semantic distinction between diagram content, front-panel content, dependencies, and non-authoritative recoverability metadata.</li>
</ul>

<p>
If optional recoverability metadata is present, the IDE MAY use it to restore or preserve guided authoring presentation.
If such metadata is absent, ignored, or cannot safely be applied, the IDE MUST still insert the canonical carried content deterministically when the canonical content itself is valid.
</p>

<h3>18.4 Target-scope validation</h3>

<p>
Snippet insertion MUST still validate against the target scope and active insertable space.
A snippet may carry valid source-aligned content and still be rejected if the target editing context does not support the required canonical target family or required optional profile support.
</p>

<p>
Snippet insertion MUST NOT silently turn copied or imported provenance into a fresh <code>human_direct</code> claim. A direct human drag-and-drop action is evidence about the insertion action; it is not automatically evidence that the carried canonical objects were originally authored manually.
</p>

<h3>18.5 Drag-and-drop equivalence</h3>

<p>
A drag-and-drop workflow using a snippet image SHOULD be semantically equivalent to explicit snippet import from the same carrier.
The gesture differs; the recovered payload meaning does not.
</p>

<h3>18.6 Paste from clipboard</h3>

<p>
If a clipboard operation carries an image-backed snippet artifact or equivalent embedded-payload image data,
the IDE SHOULD decode and treat it using the same rules as file import or drag-and-drop.
</p>

<hr/>

<h2 id="validation-and-safety-rules">19. Validation and Safety Rules</h2>

<p>
A snippet payload MUST be rejected if any of the following is true:
</p>

<ul>
  <li>the image carrier does not contain recoverable embedded payload data,</li>
  <li>the payload kind is missing or invalid,</li>
  <li>required fragment sections are missing for the declared snippet kind,</li>
  <li>internal references are inconsistent,</li>
  <li>the payload violates source-level invariants of the carried content family,</li>
  <li>the payload depends on unsupported required profile-defined capabilities in the target IDE.</li>
</ul>

<p>
Optional non-authoritative recoverability metadata MUST NOT by itself determine whether the canonical carried content is valid.
It MAY be ignored, warned about, or dropped without invalidating an otherwise valid snippet payload.
</p>

<p>
A target IDE SHOULD additionally warn when:
</p>

<ul>
  <li>the snippet is valid but requires external dependencies that are unavailable,</li>
  <li>the snippet is valid but references profile-defined capabilities that are not supported in the current environment,</li>
  <li>the snippet is structurally valid but contains unresolved boundary endpoints,</li>
  <li>the visible image appears stale relative to regenerated preview expectations,</li>
  <li>optional recoverability metadata is present but cannot be safely applied,</li>
  <li>the snippet was produced by a newer incompatible snippet-transport revision.</li>
</ul>

<p>
A snippet image MUST NOT be trusted merely because it looks correct visually.
Validation MUST be performed on the embedded structured payload.
</p>

<hr/>

<h2 id="illustrative-examples">20. Illustrative Examples</h2>

<h3>20.1 Diagram snippet payload</h3>

<pre><code class="language-json">{
  "kind": "frog_snippet",
  "version": "0.1",
  "snippet_kind": "diagram_snippet",
  "diagram_fragment": {
    "nodes": [
      { "id": "add_1", "kind": "primitive", "type": "frog.core.add" }
    ],
    "edges": []
  },
  "boundaries": {
    "inputs": [
      { "id": "b_in_1", "kind": "snippet_input", "type": "f64" }
    ],
    "outputs": [
      { "id": "b_out_1", "kind": "snippet_output", "type": "f64" }
    ]
  },
  "metadata": {
    "title": "Adder fragment"
  }
}</code></pre>

<h3>20.2 Composite snippet payload</h3>

<pre><code class="language-json">{
  "kind": "frog_snippet",
  "version": "0.1",
  "snippet_kind": "composite_snippet",
  "diagram_fragment": {
    "nodes": [
      { "id": "widget_temp", "kind": "widget_value", "widget": "temperature_numeric" }
    ],
    "edges": []
  },
  "front_panel_fragment": {
    "widgets": [
      {
        "id": "temperature_numeric",
        "role": "indicator",
        "widget": "frog.ui.standard.numeric_indicator",
        "value_type": "f64",
        "props": {
          "label": "Temperature"
        },
        "children": []
      }
    ]
  },
  "boundaries": {
    "inputs": [],
    "outputs": []
  },
  "metadata": {
    "title": "Temperature indicator snippet"
  }
}</code></pre>

<h3>20.3 Diagram snippet with optional recoverability metadata</h3>

<pre><code class="language-json">{
  "kind": "frog_snippet",
  "version": "0.1",
  "snippet_kind": "diagram_snippet",
  "diagram_fragment": {
    "nodes": [
      { "id": "read_text_1", "kind": "subfrog", "ref": "frog.io.read_text_file.basic" }
    ],
    "edges": []
  },
  "dependencies": {
    "diagram_dependencies": [
      {
        "name": "frog.io.read_text_file.basic",
        "path": "lib/io/read_text_file/basic.frog"
      }
    ]
  },
  "recoverability": {
    "express_bindings": [
      {
        "instance_id": "expr_001",
        "express_id": "frog.ide.express.read_text_file",
        "canonical_target_kind": "subfrog",
        "canonical_target_ref": "frog.io.read_text_file.basic",
        "visible_optional_terminals": ["error_in", "error_out"],
        "config": {
          "encoding": "utf8"
        }
      }
    ]
  }
}</code></pre>

<p>
In this example, the carried canonical content remains the authoritative snippet meaning.
The <code>recoverability</code> section is optional and non-authoritative.
</p>

<h3>20.4 Conceptual image-backed artifact</h3>

<pre><code>+---------------------------------------------+
| visible snippet image                       |
|  - diagram or panel preview                 |
|  - optional title or label                  |
|  - optional boundary hints                  |
+---------------------------------------------+
| embedded structured payload                 |
|  kind = frog_snippet                        |
|  version = 0.1                              |
|  snippet_kind = diagram/front_panel/...     |
|  source-aligned fragment content            |
|  optional non-authoritative recoverability  |
+---------------------------------------------+</code></pre>

<h3>20.5 Drag-and-drop behavior</h3>

<pre><code>user drags snippet image into FROG IDE
            |
            v
IDE detects image-backed candidate artifact
            |
            v
IDE decodes embedded structured snippet payload
            |
      +-----+-----+
      |           |
      v           v
   success      failure
      |           |
      v           v
validate     reject as snippet
      |
      v
insert fragment deterministically</code></pre>

<hr/>

<h2 id="out-of-scope-for-v01">21. Out of Scope for v0.1</h2>

<ul>
  <li>automatic semantic adaptation across incompatible language or profile environments,</li>
  <li>execution directly from snippet images,</li>
  <li>a mandatory cross-platform clipboard MIME registry,</li>
  <li>a mandatory OS-level drag-and-drop binary protocol,</li>
  <li>online snippet marketplaces,</li>
  <li>cryptographic signing requirements for snippet images,</li>
  <li>pixel-only recovery of snippet semantics without embedded structured payload,</li>
  <li>making optional recoverability metadata required for valid snippet insertion.</li>
</ul>

<hr/>

<h2 id="summary">22. Summary</h2>

<p>
A FROG snippet is an IDE-layer reusable fragment artifact, but unlike an ordinary detached fragment file,
it is intentionally defined as an <strong>image-backed snippet carrier</strong>.
</p>

<ul>
  <li>it is viewable as an ordinary image outside the IDE,</li>
  <li>it contains embedded structured snippet payload data,</li>
  <li>a conforming FROG IDE can recover that payload during import, paste, or drag-and-drop,</li>
  <li>the embedded payload is authoritative for snippet meaning,</li>
  <li>the visible image is authoritative for presentation only,</li>
  <li>optional recoverability metadata MAY be transported but MUST remain non-authoritative,</li>
  <li>reinsertion targets canonical carried content rather than a separate snippet-specific language,</li>
  <li>the snippet remains a fragment, not a full canonical <code>.frog</code> program.</li>
</ul>

<p>
This gives FROG a modern, portable, and visually shareable snippet model that preserves the usability advantage of image-based graphical-language reuse while remaining structurally rigorous.
</p>

<hr/>

<h2 id="license">23. License</h2>

<p>
This specification is part of the FROG repository and is governed by the repository license and contribution rules.
</p>
