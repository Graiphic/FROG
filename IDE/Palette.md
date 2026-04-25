<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 FROG IDE Palette Specification</h1>

<p align="center">
Definition of the discovery, search, and insertion palette for the FROG IDE<br/>
<em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#scope-for-v01">2. Scope for v0.1</a></li>
  <li><a href="#architectural-position-and-dependencies">3. Architectural Position and Dependencies</a></li>
  <li><a href="#design-principles">4. Design Principles</a></li>
  <li><a href="#one-canonical-insertable-space">5. One Canonical Insertable Space</a></li>
  <li><a href="#palette-access-model">6. Palette Access Model</a></li>
  <li><a href="#primary-palette-view">7. Primary Palette View</a></li>
  <li><a href="#namespace-and-origin-view">8. Namespace and Origin View</a></li>
  <li><a href="#search-model">9. Search Model</a></li>
  <li><a href="#context-aware-insertion">10. Context-Aware Insertion</a></li>
  <li><a href="#entry-kinds-and-presentation-kinds">11. Entry Kinds and Presentation Kinds</a></li>
  <li><a href="#recommended-palette-categories-for-v01">12. Recommended Palette Categories for v0.1</a></li>
  <li><a href="#entry-metadata-and-disclosure">13. Entry Metadata and Disclosure</a></li>
  <li><a href="#express-entries-and-guided-insertion">14. Express Entries and Guided Insertion</a></li>
  <li><a href="#favorites-recent-and-suggested-entries">15. Favorites, Recent, and Suggested Entries</a></li>
  <li><a href="#stability-filtering-and-capability-visibility">16. Stability, Filtering, and Capability Visibility</a></li>
  <li><a href="#extensibility-and-third-party-surfaces">17. Extensibility and Third-Party Surfaces</a></li>
  <li><a href="#validation-and-consistency-rules">18. Validation and Consistency Rules</a></li>
  <li><a href="#examples">19. Examples</a></li>
  <li><a href="#out-of-scope-for-v01">20. Out of Scope for v0.1</a></li>
  <li><a href="#summary">21. Summary</a></li>
  <li><a href="#license">22. License</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
The FROG IDE palette is the primary user-facing mechanism used to discover, search, understand,
and insert entries into a diagram during authoring.
</p>

<p>
The palette exposes the <strong>active canonical insertable space</strong> of the IDE.
That space may include:
</p>

<ul>
  <li>intrinsic standardized primitives defined by <code>Libraries/</code>,</li>
  <li>optional profile-defined primitives defined by active profile specifications,</li>
  <li>standardized structures,</li>
  <li>canonical diagram-node insertions such as interface and widget participation nodes,</li>
  <li>non-executable diagram annotations and documentation entries,</li>
  <li>guided IDE authoring entries such as Express entries,</li>
  <li>implementation-supported third-party namespaces when available.</li>
</ul>

<p>
Its purpose is not only to expose what is available, but to expose it in a way that matches how users
actually think when building a graphical program while preserving canonical identity.
</p>

<p>
The palette therefore serves four complementary roles:
</p>

<ul>
  <li><strong>discovery</strong> — helping users find available constructs and insertion paths,</li>
  <li><strong>navigation</strong> — organizing the active insertable surface in a coherent and scalable way,</li>
  <li><strong>insertion</strong> — allowing rapid placement of primitives, structures, node insertions, and annotations into the diagram,</li>
  <li><strong>guided authoring</strong> — allowing certain common tasks to be inserted through assisted IDE presentations without creating new language constructs.</li>
</ul>

<p>
The palette belongs to the IDE layer.
It exposes constructs owned normatively by other specifications, but it does not redefine their semantics
or canonical source identity.
</p>

<pre><code>Repository architecture

Expression/   -&gt; canonical source form
Language/     -&gt; normative execution semantics
Libraries/    -&gt; intrinsic primitive vocabularies
Profiles/     -&gt; optional standardized capability families
IDE/          -&gt; authoring and discovery surfaces

Palette = discovery and insertion surface
Palette != semantic ownership
</code></pre>

<hr/>

<h2 id="scope-for-v01">2. Scope for v0.1</h2>

<p>
For FROG v0.1, this document standardizes:
</p>

<ul>
  <li>the palette as an IDE-facing discovery and insertion surface,</li>
  <li>the rule that all palette access modes resolve to the same canonical insertable space,</li>
  <li>the distinction between canonical entry kinds and presentation kinds,</li>
  <li>the distinction between intrinsic, profile-defined, and third-party origins,</li>
  <li>the treatment of aliases, authoring views, and guided Express entries,</li>
  <li>the minimum consistency rules for search, browsing, contextual insertion, and guided insertion.</li>
</ul>

<p>
This document does <strong>not</strong> standardize:
</p>

<ul>
  <li>the language semantics of the inserted entries,</li>
  <li>a mandatory ranking algorithm,</li>
  <li>a mandatory palette-window layout,</li>
  <li>a mandatory icon set or visual theme,</li>
  <li>a requirement that every canonical family expose guided entries,</li>
  <li>a requirement that every future palette family already be standardized in v0.1.</li>
</ul>

<hr/>

<h2 id="architectural-position-and-dependencies">3. Architectural Position and Dependencies</h2>

<h3>3.1 Repository position</h3>

<p>
This document belongs in <code>IDE/</code> because it defines how a conforming IDE exposes the active insertable
surface to users.
It does <strong>not</strong> belong to <code>Expression/</code>, <code>Language/</code>, <code>Libraries/</code>,
or <code>Profiles/</code> because it does not own canonical source form, execution meaning,
primitive-local semantics, or profile-owned capability meaning.
</p>

<h3>3.2 Dependencies</h3>

<p>
This document depends on the following specifications:
</p>

<ul>
  <li><code>IDE/Readme.md</code> — architectural role of the palette as an IDE-facing authoring and discovery surface,</li>
  <li><code>IDE/Express.md</code> — guided insertion behavior and normalization to canonical content,</li>
  <li><code>IDE/Snippet.md</code> — insertion and fragment-handling behavior for authoring fragments,</li>
  <li><code>Libraries/Readme.md</code> — intrinsic standard primitive-library taxonomy,</li>
  <li><code>Libraries/Core.md</code> — foundational built-in primitives such as <code>frog.core.add</code> and <code>frog.core.delay</code>,</li>
  <li><code>Libraries/Math.md</code> — numeric primitives in <code>frog.math</code>,</li>
  <li><code>Libraries/Collections.md</code> — collection primitives in <code>frog.collections</code>,</li>
  <li><code>Libraries/Text.md</code> — text primitives in <code>frog.text</code>,</li>
  <li><code>Libraries/IO.md</code> — file, path, byte, and related I/O primitives in <code>frog.io</code>,</li>
  <li><code>Libraries/Signal.md</code> — signal-processing primitives in <code>frog.signal</code>,</li>
  <li><code>Libraries/UI.md</code> — executable widget interaction primitives in <code>frog.ui</code>,</li>
  <li><code>Profiles/Readme.md</code> — architectural role of optional standardized capability families,</li>
  <li><code>Profiles/Interop.md</code> — optional standardized <code>frog.connectivity.*</code> family,</li>
  <li><code>Expression/Control structures.md</code> — canonical structures and the rule that authoring views do not create new canonical structure identities,</li>
  <li><code>Language/Control structures.md</code> — normative semantic families of standardized control structures,</li>
  <li><code>Expression/Diagram.md</code> — canonical node kinds and diagram-level insertion targets,</li>
  <li><code>Expression/Widget interaction.md</code> — widget-facing source paths such as <code>widget_value</code>, <code>widget_reference</code>, property access, and method invocation,</li>
  <li><code>Expression/Front panel.md</code> and <code>Expression/Widget.md</code> — front-panel and widget models informing UI-related palette entries.</li>
</ul>

<h3>3.3 Ownership boundary</h3>

<p>
Accordingly:
</p>

<ul>
  <li><code>Libraries/</code> defines which intrinsic standardized primitives exist,</li>
  <li><code>Profiles/</code> defines which optional standardized capability families exist,</li>
  <li><code>Expression/</code> defines how standardized nodes, structures, and annotations appear in source,</li>
  <li><code>Language/</code> defines the normative execution meaning of language-level structures and cross-cutting runtime rules,</li>
  <li><code>IDE/Palette.md</code> defines how a conforming IDE exposes those entries for discovery and insertion.</li>
</ul>

<p>
If a conflict appears, canonical ownership wins.
The palette may expose many discovery paths, but it MUST NOT create new semantic ownership.
</p>

<pre><code>Ownership summary for palette users

What exists intrinsically?      -&gt; Libraries/
What exists optionally?         -&gt; Profiles/
What is serialized in source?   -&gt; Expression/
What does it mean?              -&gt; Language/
How is it found and inserted?   -&gt; IDE/Palette.md
</code></pre>

<hr/>

<h2 id="design-principles">4. Design Principles</h2>

<h3>4.1 Intent before implementation detail</h3>

<p>
The primary palette SHOULD be organized according to user intent such as:
</p>

<ul>
  <li>do arithmetic,</li>
  <li>compare values,</li>
  <li>branch execution,</li>
  <li>repeat execution,</li>
  <li>work with collections,</li>
  <li>process text,</li>
  <li>process signals,</li>
  <li>interact with a widget,</li>
  <li>read a file,</li>
  <li>insert a public boundary node,</li>
  <li>call an external system through an active optional profile.</li>
</ul>

<h3>4.2 Canonical identity remains singular</h3>

<p>
A construct MAY appear in multiple discovery paths, but it MUST keep one canonical identity.
A search alias, shortcut, contextual presentation, authoring-facing label, or guided presentation
MUST NOT create a second semantic or source-level construct.
</p>

<h3>4.3 Namespace remains visible where namespaces exist</h3>

<p>
Although the primary palette is intent-first, the true standardized namespace model remains important for
library-owned, profile-owned, and third-party primitive entries.
The IDE MUST therefore also provide a namespace- and origin-oriented view.
</p>

<h3>4.4 Search is first-class</h3>

<p>
The palette is not only a tree.
Search is a primary insertion path and MUST be treated as a first-class mechanism.
</p>

<h3>4.5 Canonical kinds and guided presentations remain distinct</h3>

<p>
Ordinary primitives, structures, node insertions, annotations, aliases, authoring views,
and guided Express presentations MUST remain visibly distinguishable in the palette and in search results.
</p>

<h3>4.6 Taxonomy boundaries remain explicit</h3>

<p>
The palette MUST keep the following boundaries explicit:
</p>

<ul>
  <li><strong>Signal</strong> — signal-processing primitives,</li>
  <li><strong>UI</strong> — widget-facing interaction and UI-related insertion paths,</li>
  <li><strong>I/O</strong> — file, path, resource, and byte-oriented I/O,</li>
  <li><strong>Connectivity</strong> — Python, native/shared library, .NET, SQL, and related external-runtime or external-service bindings provided by an active optional profile,</li>
  <li><strong>Structures</strong> — control structures and other canonical structure families,</li>
  <li><strong>Diagram Nodes</strong> — interface boundaries, widget nodes, <code>subfrog</code>, and other direct diagram insertions,</li>
  <li><strong>Annotations</strong> — non-executable diagram documentation elements.</li>
</ul>

<h3>4.7 Context improves speed</h3>

<p>
The IDE SHOULD use type context, wire context, insertion location, and active-profile context
to improve suggestions without changing language semantics.
</p>

<h3>4.8 Express remains an IDE convenience</h3>

<p>
A guided entry MAY improve discoverability, reduce configuration friction, and surface common tasks
in more human terms.
However:
</p>

<ul>
  <li>a guided entry MUST NOT create a new canonical primitive identity by itself,</li>
  <li>a guided entry MUST NOT create a new structure family by itself,</li>
  <li>a guided entry MUST disclose or preserve its canonical target identity,</li>
  <li>execution-facing systems MUST remain independent from guided presentation state.</li>
</ul>

<pre><code>Palette discipline

Many labels
    may point to
one canonical target

One canonical target
    must not become
many semantic constructs
</code></pre>

<hr/>

<h2 id="one-canonical-insertable-space">5. One Canonical Insertable Space</h2>

<p>
All palette access modes MUST resolve to the same underlying <strong>canonical insertable space</strong>.
This is the core consistency invariant of the palette model.
</p>

<p>
That means:
</p>

<ul>
  <li>primary browsing,</li>
  <li>namespace browsing,</li>
  <li>search,</li>
  <li>contextual insertion,</li>
  <li>favorites and recents,</li>
  <li>guided / Express surfaces</li>
</ul>

<p>
are alternative access paths to the same insertable space rather than independent semantic catalogs.
</p>

<p>
Accordingly:
</p>

<ul>
  <li>an alias MUST resolve to a canonical target already present in that space,</li>
  <li>a guided entry MUST resolve to canonical content already valid in that space,</li>
  <li>a contextual suggestion MUST improve ranking, not invent a new construct,</li>
  <li>a view change MUST NOT change the semantic identity of the inserted result.</li>
</ul>

<pre><code>One insertable space
    |
    +--&gt; primary intent view
    +--&gt; namespace / origin view
    +--&gt; search
    +--&gt; contextual insertion
    +--&gt; optional guided / Express lens

All paths must resolve to the same canonical entry space.
</code></pre>

<hr/>

<h2 id="palette-access-model">6. Palette Access Model</h2>

<p>
The FROG IDE palette SHOULD expose multiple complementary access paths:
</p>

<ul>
  <li><strong>Primary palette view</strong> — intent-first hierarchical browsing,</li>
  <li><strong>Namespace and origin view</strong> — direct browsing by intrinsic namespace, optional profile namespace, third-party namespace, and explicit non-library roots,</li>
  <li><strong>Search</strong> — direct textual lookup,</li>
  <li><strong>Contextual insertion</strong> — suggestions derived from current diagram context,</li>
  <li><strong>Optional guided / Express lens</strong> — a task-oriented filtered presentation of the same insertable surface.</li>
</ul>

<p>
These access modes are complementary, not competing.
A conforming implementation MAY emphasize one of them as the default,
but they MUST remain semantically consistent with one another.
</p>

<hr/>

<h2 id="primary-palette-view">7. Primary Palette View</h2>

<p>
The primary palette view is the default palette presented to most users.
It is organized around user intent and common graphical programming tasks.
</p>

<p>
Convenience surfaces such as <strong>Favorites</strong>, <strong>Recent</strong>, <strong>Search</strong>,
and an optional <strong>Guided / Express</strong> lens MAY appear at the top of the palette,
but they are not semantic language categories.
</p>

<p>
Recommended top-level groups that a conforming IDE SHOULD be able to expose are:
</p>

<pre><code>Favorites
Recent
Search
Optional Guided / Express Lens

Program Structure
Flow Control
State &amp; Timing
Math &amp; Logic
Data &amp; Types
Collections
Text
Signal
UI
I/O
</code></pre>

<p>
The following groups are <strong>conditional</strong> rather than baseline intrinsic:
</p>

<pre><code>Connectivity
Additional Profile-Defined Families
Third-Party Families
</code></pre>

<p>
A conforming IDE SHOULD surface such groups only when the corresponding active profile set
or implementation-supported extension set actually provides them.
</p>

<p>
The primary view SHOULD be able to surface both:
</p>

<ul>
  <li>canonical entries directly,</li>
  <li>guided entries that point to those canonical entries or to deterministic canonical fragments.</li>
</ul>

<pre><code>Primary view answers:
- "What do I want to do?"

Namespace / origin view answers:
- "What owns this entry?"
</code></pre>

<hr/>

<h2 id="namespace-and-origin-view">8. Namespace and Origin View</h2>

<p>
The namespace and origin view exposes the actual organization of the active insertable surface.
</p>

<p>
For the current baseline, the intrinsic standardized library namespaces are:
</p>

<pre><code>Intrinsic library namespaces

frog.core.*
frog.math.*
frog.collections.*
frog.text.*
frog.io.*
frog.signal.*
frog.ui.*
</code></pre>

<p>
The current standardized optional profile-owned namespace family is:
</p>

<pre><code>Optional profile namespace families

frog.connectivity.*
    -&gt; owned by the Interop profile
</code></pre>

<p>
Because the palette also exposes non-library constructs, a conforming IDE MUST NOT pretend
that structures or direct diagram-node insertions belong to a primitive namespace when they do not.
</p>

<p>
Accordingly, a namespace-capable IDE MAY also expose explicit non-library roots such as:
</p>

<pre><code>Structures
Diagram Nodes
Annotations
</code></pre>

<p>
This allows the palette to expose:
</p>

<ul>
  <li>intrinsic standardized library primitives through true intrinsic namespaces,</li>
  <li>optional profile-owned primitives through true profile-owned namespaces,</li>
  <li>third-party primitives through true third-party namespaces,</li>
  <li>standardized structures through an explicit structure root,</li>
  <li>standardized node-insertion entries through an explicit diagram-node root,</li>
  <li>non-executable annotation entries through an explicit annotation root.</li>
</ul>

<p>
The namespace/origin view SHOULD make origin explicit.
For example:
</p>

<ul>
  <li><code>frog.core.add</code> — Intrinsic Library</li>
  <li><code>frog.ui.property_write</code> — Intrinsic Library</li>
  <li><code>frog.connectivity.sql.query_text</code> — Profile-defined / Interop</li>
  <li><code>graiphic.vision.detect</code> — Third-Party Library</li>
  <li><code>case</code> — Structure</li>
  <li><code>widget_value</code> — Diagram Node</li>
  <li><code>text annotation</code> — Annotation</li>
</ul>

<p>
The namespace/origin view MAY additionally indicate which entries have guided presentations available.
However, guided presentation MUST remain secondary to canonical origin and identity.
</p>

<hr/>

<h2 id="search-model">9. Search Model</h2>

<p>
Search is a primary palette feature.
Users SHOULD be able to search by:
</p>

<ul>
  <li>primitive name,</li>
  <li>structure name,</li>
  <li>node-insertion name,</li>
  <li>annotation name,</li>
  <li>namespace-qualified name,</li>
  <li>common alias,</li>
  <li>operator symbol,</li>
  <li>natural-language keyword,</li>
  <li>task-oriented phrase,</li>
  <li>common guided-entry wording.</li>
</ul>

<p>
Examples:
</p>

<ul>
  <li>searching <code>add</code> SHOULD find <code>frog.core.add</code>,</li>
  <li>searching <code>+</code> SHOULD also find <code>frog.core.add</code>,</li>
  <li>searching <code>sqrt</code> SHOULD find the relevant <code>frog.math.*</code> entry,</li>
  <li>searching <code>length</code> SHOULD find the relevant <code>frog.collections.*</code> entry,</li>
  <li>searching <code>read text</code> SHOULD find the relevant canonical <code>frog.io.*</code> entry and MAY also surface a guided entry when available,</li>
  <li>searching <code>moving average</code> SHOULD find the relevant <code>frog.signal.*</code> entry and MAY surface a guided signal entry when available,</li>
  <li>searching <code>if</code> SHOULD find the canonical <code>case</code> structure through a boolean conditional authoring view,</li>
  <li>searching <code>switch</code> SHOULD find the canonical <code>case</code> structure through an exact-match authoring view,</li>
  <li>searching <code>for</code> SHOULD find <code>for_loop</code>,</li>
  <li>searching <code>while</code> SHOULD find <code>while_loop</code>,</li>
  <li>searching <code>widget value</code> SHOULD find the <code>widget_value</code> insertion entry,</li>
  <li>searching <code>interface input</code> SHOULD find the <code>interface_input</code> insertion entry,</li>
  <li>searching <code>subfrog</code> SHOULD find sub-FROG insertion,</li>
  <li>searching <code>comment</code> SHOULD find the relevant non-executable annotation entry,</li>
  <li>searching <code>python</code> SHOULD find the relevant connectivity entry only when the corresponding profile support is active,</li>
  <li>searching <code>sql</code> SHOULD find the relevant connectivity entry only when the corresponding profile support is active.</li>
</ul>

<p>
Search SHOULD rank results using at least:
</p>

<ul>
  <li>exact name match,</li>
  <li>namespace-qualified match,</li>
  <li>alias match,</li>
  <li>task-intent match,</li>
  <li>entry-kind relevance,</li>
  <li>guided-entry availability where appropriate,</li>
  <li>contextual relevance,</li>
  <li>recent usage.</li>
</ul>

<p>
The canonical identity of the returned entry MUST remain unchanged.
For example, a search for <code>if</code> returns the canonical <code>case</code> structure rather than
a separate language construct named <code>if</code>.
</p>

<pre><code>Search rule

User query
   -&gt; aliases / symbols / task phrases
   -&gt; ranked results
   -&gt; canonical identity remains visible
</code></pre>

<hr/>

<h2 id="context-aware-insertion">10. Context-Aware Insertion</h2>

<p>
The IDE SHOULD support contextual insertion suggestions derived from the current diagram context.
</p>

<p>
Useful contexts include:
</p>

<ul>
  <li>dragging from a wire,</li>
  <li>inserting near a specific node,</li>
  <li>searching with a selected wire type,</li>
  <li>invoking insertion from a widget reference or widget value node,</li>
  <li>invoking insertion from an empty canvas or structure region,</li>
  <li>invoking insertion while an optional profile is active.</li>
</ul>

<p>
Examples:
</p>

<ul>
  <li>from a <code>bool</code> wire, prefer logic and conditional suggestions,</li>
  <li>from a numeric wire, prefer arithmetic, comparison, and numeric-function suggestions,</li>
  <li>from a collection wire, prefer collection-related suggestions,</li>
  <li>from a signal-like context, prefer relevant <code>frog.signal.*</code> suggestions when type-compatible,</li>
  <li>from a <code>string</code> wire, prefer text operations and string-based <code>case</code> suggestions,</li>
  <li>from a widget reference, prefer property and method interaction suggestions,</li>
  <li>from a value feedback path, prefer <code>frog.core.delay</code>,</li>
  <li>from a path-like or file-related context, prefer I/O suggestions,</li>
  <li>from an interop-related insertion path, prefer Python, native binding, .NET, or SQL suggestions only when the corresponding profile support is active,</li>
  <li>from an empty top-level diagram, surface interface boundary nodes, sub-FROG insertion, annotations, and common structures where appropriate.</li>
</ul>

<p>
When the context strongly suggests a derived authoring form, the IDE MAY present that form directly.
Such context-aware presentations remain authoring conveniences.
They MUST NOT alter the canonical identity or language semantics of the inserted construct.
</p>

<p>
When a user places a node, structure, annotation, or other canonical object through a direct palette interaction, a provenance-aware IDE MAY emit an <code>ide.provenance</code> attestation such as <code>human_direct</code> for the created source object. This evidence is authoring metadata only; it does not alter the canonical insertable space or the meaning of the inserted construct.
</p>

<pre><code>Context helps ranking
Context does not change meaning

wire type / insertion location / active profile
    -&gt; better suggestions

not:
wire type / insertion location
    -&gt; new hidden language construct
</code></pre>

<hr/>

<h2 id="entry-kinds-and-presentation-kinds">11. Entry Kinds and Presentation Kinds</h2>

<p>
The palette MUST visually distinguish between <strong>entry kinds</strong> and <strong>presentation kinds</strong>.
</p>

<h3>11.1 Canonical entry kinds</h3>

<ul>
  <li><strong>primitive entry</strong> — ordinary callable operation defined by an intrinsic library, active profile, or active third-party library,</li>
  <li><strong>structure entry</strong> — diagram region construct with owned subgraphs and explicit boundary semantics,</li>
  <li><strong>node-insertion entry</strong> — canonical source node inserted directly into the diagram,</li>
  <li><strong>annotation entry</strong> — non-executable documentation or organization object.</li>
</ul>

<h3>11.2 Presentation kinds</h3>

<ul>
  <li><strong>direct entry</strong> — canonical entry shown directly,</li>
  <li><strong>alias</strong> — alternate search or display label resolving to one canonical target,</li>
  <li><strong>authoring view</strong> — convenience presentation such as <em>If</em> or <em>Switch</em> resolving to a canonical structure,</li>
  <li><strong>guided entry</strong> — guided IDE presentation resolving to canonical content.</li>
</ul>

<p>
Examples:
</p>

<ul>
  <li><code>frog.core.add</code> is a primitive entry,</li>
  <li><code>frog.core.delay</code> is a stateful primitive entry,</li>
  <li><code>frog.connectivity.sql.query_text</code> is a profile-defined primitive entry when the Interop profile is available,</li>
  <li><code>case</code> is a structure entry,</li>
  <li><code>for_loop</code> is a structure entry,</li>
  <li><code>while_loop</code> is a structure entry,</li>
  <li><code>interface_input</code> is a node-insertion entry,</li>
  <li><code>widget_value</code> is a node-insertion entry,</li>
  <li>a text comment box is an annotation entry,</li>
  <li><em>Read Text File</em> MAY be a guided entry if it resolves to a valid canonical target.</li>
</ul>

<p>
A structure entry MAY additionally expose one or more authoring views or presentation labels.
Such labels are presentation-level affordances.
They MUST remain attached to one canonical structure entry rather than creating additional structure identities.
</p>

<pre><code>Entry kind separation

Primitive       -&gt; callable operation
Structure       -&gt; owned region / control construct
Node insertion  -&gt; canonical source node inserted directly
Annotation      -&gt; non-executable documentation object
Guided entry    -&gt; guided presentation resolving to canonical content
</code></pre>

<hr/>

<h2 id="recommended-palette-categories-for-v01">12. Recommended Palette Categories for v0.1</h2>

<p>
The following palette layout is recommended for base v0.1 and for compatible implementations
that may also activate optional profiles.
</p>

<h3>12.1 Program Structure</h3>

<ul>
  <li>Sub-FROG insertion</li>
  <li>Interface input node insertion</li>
  <li>Interface output node insertion</li>
  <li>Comments</li>
  <li>Documentation annotations</li>
</ul>

<p>
Base v0.1 SHOULD NOT treat structure-owned <code>regions</code> as a standalone palette entry with an independent language identity.
Regions are owned by structures and arise through structure insertion and structure editing.
</p>

<h3>12.2 Flow Control</h3>

<ul>
  <li>Case structure</li>
  <li>Boolean conditional authoring view for <code>case</code> such as <em>If</em> or <em>If / Else</em></li>
  <li>Exact-match authoring view for <code>case</code> such as <em>Switch</em> when appropriate</li>
  <li>For loop structure</li>
  <li>While loop structure</li>
</ul>

<p>
The canonical identity of these entries remains:
</p>

<ul>
  <li><code>case</code></li>
  <li><code>for_loop</code></li>
  <li><code>while_loop</code></li>
</ul>

<h3>12.3 State &amp; Timing</h3>

<ul>
  <li><code>frog.core.delay</code></li>
  <li>additional timing-related entries only when standardized by the active specification set</li>
</ul>

<p>
In base v0.1, this category is effectively centered on explicit local memory through <code>frog.core.delay</code>.
It MUST NOT imply that a broader standardized timing surface already exists when it does not.
</p>

<h3>12.4 Math &amp; Logic</h3>

<ul>
  <li><code>frog.core.add</code></li>
  <li><code>frog.core.sub</code></li>
  <li><code>frog.core.mul</code></li>
  <li><code>frog.core.div</code></li>
  <li><code>frog.core.neg</code></li>
  <li><code>frog.core.abs</code></li>
  <li><code>frog.core.equal</code></li>
  <li><code>frog.core.not_equal</code></li>
  <li><code>frog.core.less</code></li>
  <li><code>frog.core.less_or_equal</code></li>
  <li><code>frog.core.greater</code></li>
  <li><code>frog.core.greater_or_equal</code></li>
  <li><code>frog.core.and</code></li>
  <li><code>frog.core.or</code></li>
  <li><code>frog.core.not</code></li>
  <li><code>frog.core.xor</code></li>
  <li><code>frog.core.select</code></li>
  <li>standardized <code>frog.math.*</code> numeric entries</li>
</ul>

<h3>12.5 Data &amp; Types</h3>

<ul>
  <li>type-conversion entries when standardized by the active specification set</li>
  <li>cast entries when standardized by the active specification set</li>
  <li>future representation-related entries only when explicitly standardized</li>
</ul>

<h3>12.6 Collections</h3>

<ul>
  <li>standardized <code>frog.collections.*</code> entries</li>
  <li>array construction helpers when standardized</li>
  <li>array indexing and slicing helpers when standardized</li>
</ul>

<h3>12.7 Text</h3>

<ul>
  <li>standardized <code>frog.text.*</code> entries</li>
  <li>text comparison entries</li>
  <li>text transformation entries</li>
</ul>

<h3>12.8 Signal</h3>

<ul>
  <li>standardized <code>frog.signal.*</code> entries</li>
  <li>signal filtering entries</li>
  <li>signal measurement and transformation entries</li>
  <li>guided signal-processing entries when available and when they disclose a valid canonical target</li>
</ul>

<h3>12.9 UI</h3>

<ul>
  <li><code>widget_value</code> insertion</li>
  <li><code>widget_reference</code> insertion</li>
  <li><code>frog.ui.property_read</code></li>
  <li><code>frog.ui.property_write</code></li>
  <li><code>frog.ui.method_invoke</code></li>
</ul>

<p>
This group is intended for widget-facing interaction and UI-related insertion paths.
It SHOULD NOT absorb file I/O, database access, external interoperability, or general service bindings.
</p>

<h3>12.10 I/O</h3>

<ul>
  <li>standardized <code>frog.io.*</code> entries</li>
  <li>file read and write entries</li>
  <li>path and resource entries</li>
  <li>byte-oriented I/O entries</li>
  <li>guided I/O entries when available and when they disclose a valid canonical target</li>
</ul>

<p>
This group SHOULD cover operations whose primary role is reading from, writing to, opening, closing,
or transforming a file, path, resource, or byte-oriented I/O value.
It SHOULD NOT absorb Python, native/shared library, .NET, SQL, or general external-runtime bindings.
</p>

<h3>12.11 Connectivity</h3>

<p>
This category is <strong>conditional</strong>.
It SHOULD be surfaced only when the Interop profile, or another future standardized profile defining
connectivity-like capabilities, is active.
It is not part of the baseline intrinsic library surface of base v0.1.
</p>

<ul>
  <li>standardized <code>frog.connectivity.python.*</code> entries when the Interop profile is available</li>
  <li>standardized <code>frog.connectivity.native.*</code> entries when the Interop profile is available</li>
  <li>standardized <code>frog.connectivity.dotnet.*</code> entries when the Interop profile is available</li>
  <li>standardized <code>frog.connectivity.sql.*</code> entries when the Interop profile is available</li>
  <li>guided connectivity entries when available and when they disclose a valid canonical target</li>
</ul>

<h3>12.12 Additional Profile-Defined Families</h3>

<p>
Additional profile-defined groups MAY be surfaced when the active profile set provides them.
When surfaced before baseline standardization in the repository, they SHOULD be clearly marked
as profile-defined, reserved, or future-facing.
</p>

<h3>12.13 Third-Party Families</h3>

<p>
Implementation-supported third-party families MAY be surfaced when available.
They SHOULD remain clearly marked as third-party rather than standardized baseline or profile-defined entries.
</p>

<hr/>

<h2 id="entry-metadata-and-disclosure">13. Entry Metadata and Disclosure</h2>

<p>
Each palette entry SHOULD expose useful metadata to the user.
Recommended metadata includes:
</p>

<ul>
  <li>display name,</li>
  <li>canonical identity,</li>
  <li>namespace-qualified name when applicable,</li>
  <li>canonical origin — intrinsic library, profile-defined library, third-party library, structure, diagram node, annotation, or equivalent,</li>
  <li>canonical entry kind — primitive, structure, node insertion, annotation, or equivalent,</li>
  <li>presentation kind — direct entry, alias, authoring view, guided entry, or equivalent,</li>
  <li>short description,</li>
  <li>type family where relevant,</li>
  <li>statefulness where relevant,</li>
  <li>stability level,</li>
  <li>owning specification or library origin.</li>
</ul>

<p>
When an entry exposes an alias, authoring view, or guided presentation,
the IDE SHOULD also make the canonical identity visible.
</p>

<p>
Examples of useful badges:
</p>

<ul>
  <li>Primitive</li>
  <li>Structure</li>
  <li>Node Insertion</li>
  <li>Annotation</li>
  <li>Guided</li>
  <li>Alias</li>
  <li>Authoring View</li>
  <li>Pure</li>
  <li>Stateful</li>
  <li>Signal</li>
  <li>UI</li>
  <li>I/O</li>
  <li>Connectivity</li>
  <li>Profile-defined</li>
  <li>Third-Party</li>
  <li>Experimental</li>
</ul>

<pre><code>Minimum disclosure rule

If user sees:
- alias
- guided entry
- authoring label

user should still be able to learn:
- canonical identity
- canonical origin
- canonical entry kind
</code></pre>

<hr/>

<h2 id="express-entries-and-guided-insertion">14. Express Entries and Guided Insertion</h2>

<p>
A conforming IDE MAY expose guided entries as assistant-driven palette entries for common tasks.
Their purpose is to reduce authoring friction while preserving the canonical FROG model.
</p>

<p>
A guided entry:
</p>

<ul>
  <li>MAY appear in primary browsing, search results, contextual insertion, or a dedicated guided lens,</li>
  <li>MUST remain attached to a canonical target identity or a deterministic canonical target set,</li>
  <li>SHOULD disclose that target identity to the user,</li>
  <li>MAY expose configurable parameters and optional expandable terminals,</li>
  <li>MUST NOT invent semantically invalid ports or hidden semantically required ports.</li>
</ul>

<p>
A conforming IDE SHOULD support at least the following palette-facing guided lifecycle where such entries exist:
</p>

<ul>
  <li>discover the entry,</li>
  <li>insert and configure the entry,</li>
  <li>reopen the configuration experience,</li>
  <li>preserve stable mapping between the guided presentation and the canonical target,</li>
  <li>discard or detach the guided presentation without invalidating canonical content.</li>
</ul>

<p>
Guided entries SHOULD be especially useful for:
</p>

<ul>
  <li>common I/O tasks,</li>
  <li>common signal-processing tasks,</li>
  <li>common connectivity tasks when the corresponding profile is active,</li>
  <li>other repetitive workflows where guided configuration is materially better than manual low-level assembly.</li>
</ul>

<p>
Guided entries SHOULD NOT be required for ordinary language use.
A user MUST remain able to work directly with canonical primitives, canonical structures,
canonical node insertions, and ordinary annotations.
</p>

<hr/>

<h2 id="favorites-recent-and-suggested-entries">15. Favorites, Recent, and Suggested Entries</h2>

<p>
The palette SHOULD support the following convenience groups:
</p>

<h3>15.1 Favorites</h3>
<p>User-pinned entries for fast access.</p>

<h3>15.2 Recent</h3>
<p>Recently inserted entries.</p>

<h3>15.3 Suggested</h3>
<p>Entries recommended by the IDE based on context, type, structure location, recent activity, or guided-entry availability.</p>

<p>
These convenience groups are optional IDE features.
They MUST NOT alter the canonical identity of the underlying entries.
</p>

<hr/>

<h2 id="stability-filtering-and-capability-visibility">16. Stability, Filtering, and Capability Visibility</h2>

<p>
The palette SHOULD support filtering by stability level.
Recommended levels include:
</p>

<ul>
  <li>Stable</li>
  <li>Advanced</li>
  <li>Experimental</li>
  <li>Profile-defined</li>
  <li>Third-Party</li>
</ul>

<p>
The palette MAY also support filtering by:
</p>

<ul>
  <li>namespace,</li>
  <li>canonical origin,</li>
  <li>canonical entry kind,</li>
  <li>presentation kind,</li>
  <li>type family,</li>
  <li>statefulness,</li>
  <li>owning specification,</li>
  <li>guided-entry availability,</li>
  <li>guided vs direct insertion mode.</li>
</ul>

<p>
Optional profile-defined capability families MUST be surfaced as conditional rather than baseline intrinsic.
Third-party families MUST remain explicitly identifiable as third-party.
</p>

<hr/>

<h2 id="extensibility-and-third-party-surfaces">17. Extensibility and Third-Party Surfaces</h2>

<p>
The palette model MUST support third-party libraries without breaking the built-in organizational model.
</p>

<p>
Third-party entries SHOULD:
</p>

<ul>
  <li>appear in the namespace/origin view under their own namespace,</li>
  <li>appear in the primary palette under the most relevant intent category when appropriate,</li>
  <li>expose clear third-party origin metadata.</li>
</ul>

<p>
Example namespaces:
</p>

<pre><code>graiphic.vision.*
graiphic.control.*
vendor.ai.*
</code></pre>

<p>
A single construct SHOULD have one canonical identity even if the IDE allows multiple discovery paths.
The same construct MUST NOT become semantically duplicated just because it appears in multiple palette views.
</p>

<p>
Third-party ecosystems MAY also provide guided entries.
When they do:
</p>

<ul>
  <li>the guided entry MUST still disclose canonical identity,</li>
  <li>the origin of the underlying library SHOULD remain visible,</li>
  <li>the palette MUST keep third-party guided entries distinguishable from baseline standardized entries and profile-defined entries.</li>
</ul>

<pre><code>Extensibility rule

Many discovery paths
    are allowed

Many canonical identities
for the same thing
    are not allowed
</code></pre>

<hr/>

<h2 id="validation-and-consistency-rules">18. Validation and Consistency Rules</h2>

<p>
Implementations MUST ensure that all palette access modes remain consistent.
</p>

<p>
In particular:
</p>

<ul>
  <li>the primary palette view, namespace/origin view, search results, contextual insertion paths, and guided lens MUST refer to the same canonical insertable space,</li>
  <li>every palette entry MUST resolve to a valid construct known to the active insertable surface,</li>
  <li>the active insertable surface MUST remain consistent with canonical source rules, active standardized libraries, active profile support, and active implementation-supported third-party namespaces where such support exists,</li>
  <li>search aliases MUST map back to a canonical entry identity,</li>
  <li>contextual suggestions MUST NOT invent non-existent constructs,</li>
  <li>profile-defined groups MUST NOT be presented as baseline intrinsic library families unless they actually are,</li>
  <li>third-party namespaces MUST NOT be presented as standardized baseline or standardized profile-defined families unless they actually are,</li>
  <li>authoring-facing labels such as <em>If</em>, <em>If / Else</em>, <em>Else If</em>, or <em>Switch</em> MUST resolve to canonical entries rather than independent language constructs when no such independent constructs are standardized,</li>
  <li>guided entries MUST resolve to valid canonical content and MUST NOT become editor-private semantic constructs.</li>
</ul>

<p>
The palette is an IDE presentation mechanism.
It MUST NOT change the source semantics or source identity of the inserted construct.
</p>

<pre><code>Consistency invariant

primary view
namespace / origin view
search
contextual insertion
guided lens

must all point to
the same canonical insertable space
</code></pre>

<hr/>

<h2 id="examples">19. Examples</h2>

<h3>19.1 Search aliases</h3>

<ul>
  <li><code>if</code> → canonical <code>case</code> structure with boolean selector</li>
  <li><code>if else</code> → canonical <code>case</code> structure with boolean selector</li>
  <li><code>else if</code> → canonical <code>case</code> structure through a derived authoring view</li>
  <li><code>switch</code> → canonical <code>case</code> structure with exact-match branching semantics for the active selector category</li>
  <li><code>for</code> → canonical <code>for_loop</code> structure</li>
  <li><code>while</code> → canonical <code>while_loop</code> structure</li>
  <li><code>+</code> → canonical <code>frog.core.add</code></li>
  <li><code>widget value</code> → canonical <code>widget_value</code> insertion entry</li>
  <li><code>subfrog</code> → canonical sub-FROG insertion entry</li>
</ul>

<h3>19.2 Contextual suggestions from a <code>bool</code> wire</h3>

<ul>
  <li><code>case</code> (boolean conditional authoring view)</li>
  <li><em>If</em> or <em>If / Else</em> presentation for canonical <code>case</code></li>
  <li><code>frog.core.and</code></li>
  <li><code>frog.core.or</code></li>
  <li><code>frog.core.not</code></li>
  <li><code>frog.core.select</code></li>
</ul>

<h3>19.3 Contextual suggestions from a <code>string</code> wire</h3>

<ul>
  <li><code>case</code> (string selector view)</li>
  <li><em>Switch</em> presentation for canonical <code>case</code></li>
  <li>standardized <code>frog.text.*</code> entries</li>
  <li>text comparison entries</li>
</ul>

<h3>19.4 Contextual suggestions from a widget reference</h3>

<ul>
  <li><code>frog.ui.property_read</code></li>
  <li><code>frog.ui.property_write</code></li>
  <li><code>frog.ui.method_invoke</code></li>
</ul>

<h3>19.5 Connectivity discovery examples</h3>

<ul>
  <li><code>python</code> → relevant <code>frog.connectivity.*</code> entry when the Interop profile is available</li>
  <li><code>dll</code> → relevant native/shared-library binding entry when the Interop profile is available</li>
  <li><code>sql</code> → relevant database connectivity entry when the Interop profile is available</li>
  <li><code>.net</code> → relevant .NET connectivity entry when the Interop profile is available</li>
</ul>

<h3>19.6 Guided discovery examples</h3>

<ul>
  <li><code>read text file</code> → guided entry that resolves to a valid canonical I/O target</li>
  <li><code>moving average</code> → canonical signal entry and MAY also surface a guided signal entry when available</li>
  <li><code>python call</code> → canonical connectivity entry and MAY also surface a guided connectivity entry when available</li>
</ul>

<hr/>

<h2 id="out-of-scope-for-v01">20. Out of Scope for v0.1</h2>

<ul>
  <li>full personalization of ranking algorithms,</li>
  <li>remote cloud palette federation,</li>
  <li>marketplace-style commercial discovery UX,</li>
  <li>cross-project recommendation systems,</li>
  <li>automatic insertion of profile-specific hidden helper nodes,</li>
  <li>semantic transformation of one canonical structure into another language construct,</li>
  <li>a requirement that every future palette family already be fully standardized in v0.1,</li>
  <li>turning authoring-facing aliases such as <em>If</em> or <em>Switch</em> into independent canonical entries without corresponding language standardization,</li>
  <li>requiring execution-facing systems to depend on guided presentation state,</li>
  <li>opaque guided entries whose canonical target cannot be determined by a conforming IDE.</li>
</ul>

<hr/>

<h2 id="summary">21. Summary</h2>

<p>
The FROG IDE palette is the primary discovery and insertion mechanism for the active diagram-authoring surface.
</p>

<ul>
  <li>It provides intent-first browsing, namespace/origin browsing, search, contextual insertion, and optional guided discovery.</li>
  <li>It MUST distinguish primitive entries, structure entries, node-insertion entries, annotation entries, and guided presentations.</li>
  <li>It MUST preserve one canonical identity per construct.</li>
  <li>It SHOULD support aliases such as <code>if</code> for boolean <code>case</code>.</li>
  <li>It MAY expose authoring-facing views such as <em>If</em>, <em>If / Else</em>, <em>Else If</em>, and <em>Switch</em>, but those views MUST resolve to canonical standardized entries.</li>
  <li>It MAY expose task-oriented guided entries, but those entries MUST resolve to valid canonical FROG content.</li>
  <li>It SHOULD expose <code>case</code>, <code>for_loop</code>, and <code>while_loop</code> as the standard control structures of v0.1.</li>
  <li>It SHOULD expose the currently standardized intrinsic library families of the active repository stage, including <code>frog.signal.*</code> and <code>frog.ui.*</code>.</li>
  <li>It SHOULD expose Connectivity only when the corresponding optional Interop profile is active.</li>
  <li>It MAY surface additional profile-defined or third-party families, but they SHOULD remain clearly marked when not part of the baseline intrinsic set.</li>
  <li>It MUST remain semantically consistent with canonical source rules, active standardized libraries, active profile support, and the active insertable surface of the IDE.</li>
</ul>

<p>
This gives FROG a palette model that is scalable for experts, clear for implementers, intuitive for users working from intent,
and compatible with guided workflows without creating semantic drift.
</p>

<hr/>

<h2 id="license">22. License</h2>

<p>
This specification is part of the FROG repository and is governed by the repository license and contribution rules.
</p>
