<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 FROG Icon Specification</h1>

<p align="center">
Definition of the optional <code>icon</code> section of <strong>.frog</strong> programs<br/>
<em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2 id="contents">Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#why-the-icon-section-exists">2. Why the <code>icon</code> Section Exists</a></li>
  <li><a href="#scope">3. Scope</a></li>
  <li><a href="#relation-with-other-specifications">4. Relation with Other Specifications</a></li>
  <li><a href="#location">5. Location in a <code>.frog</code> File</a></li>
  <li><a href="#purpose">6. Purpose of the Icon Section</a></li>
  <li><a href="#structure">7. Icon Object Structure</a></li>
  <li><a href="#authoring-segmentation">7.1 Optional Authoring Segmentation</a></li>
  <li><a href="#svg-requirements">8. SVG Requirements</a></li>
  <li><a href="#coordinate-system-and-sizing">9. Coordinate System and Sizing</a></li>
  <li><a href="#embedding-and-formatting-rules">10. Embedding and Formatting Rules</a></li>
  <li><a href="#extensibility-and-boundary-rules">11. Extensibility and Boundary Rules</a></li>
  <li><a href="#examples">12. Examples</a>
    <ul>
      <li><a href="#minimal-example">12.1 Minimal Icon</a></li>
      <li><a href="#symbol-example">12.2 Icon with Simple Symbol</a></li>
    </ul>
  </li>
  <li><a href="#validation-rules">13. Validation Rules</a></li>
  <li><a href="#design-goals">14. Design Goals</a></li>
  <li><a href="#summary">15. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
The top-level <code>icon</code> section defines an optional graphical icon associated with a FROG program.
</p>

<p>
This icon is primarily intended to represent the FROG when it is displayed, browsed, or reused as a node in development tools, library browsers, palettes, search results, or diagram editors.
</p>

<p>
The <code>icon</code> section is part of the canonical source representation when present, but it is <strong>non-executable</strong>.
It MUST NOT define, modify, or override execution semantics.
</p>

<p>
Execution behavior MUST be derived from the validated executable source representation of the program, centered on the public interface and the authoritative executable diagram, and interpreted according to the normative language semantics.
</p>

<p>
Runtimes, compilers, and other execution-facing systems MUST ignore the icon when determining executable meaning.
</p>

<pre>
One-line model

icon
   -> source-carried visual identity
   -> useful for reuse and browsing
   -> non-authoritative for execution
</pre>

<hr/>

<h2 id="why-the-icon-section-exists">2. Why the <code>icon</code> Section Exists</h2>

<p>
A reusable graphical program benefits from a compact visual identity that can travel with its canonical source.
That is the role of the optional <code>icon</code> section.
</p>

<p>
Without such a section, tools would have to rely only on:
</p>

<ul>
  <li>generated placeholders,</li>
  <li>tool-private icon assets,</li>
  <li>or non-portable editor state.</li>
</ul>

<p>
The <code>icon</code> section exists so that a FROG can carry a durable reusable-node visual identity while preserving a strict architectural boundary:
visual identity must remain separate from executable meaning and separate from tool-private rendering caches.
</p>

<pre>
Why icon exists

metadata -> what the FROG is
diagram  -> how the FROG executes
icon     -> how the FROG may be recognized visually
ide      -> how an editor may prefer to present or reopen it
cache    -> regenerated tooling acceleration
</pre>

<hr/>

<h2 id="scope">3. Scope</h2>

<p>
This document specifies:
</p>

<ul>
  <li>the role of the optional top-level <code>icon</code> section,</li>
  <li>the structure of the icon object,</li>
  <li>the source representation of embedded SVG icon content,</li>
  <li>validation rules for safe and portable icon content,</li>
  <li>boundary rules between icon data and other source or tooling sections.</li>
</ul>

<p>
This document does not specify:
</p>

<ul>
  <li>the executable dataflow graph,</li>
  <li>the public interface of the FROG,</li>
  <li>front-panel widget composition,</li>
  <li>IDE-specific asset pipelines,</li>
  <li>runtime rendering engines,</li>
  <li>theme systems, icon packs, or vendor-specific visual skins.</li>
</ul>

<pre>
This document owns:
- source-carried reusable-node icon data
- SVG embedding rules
- icon validation boundaries
- icon/tooling separation rules

This document does not own:
- executable graph meaning
- interface meaning
- front-panel composition
- tool-private rendering systems
</pre>

<hr/>

<h2 id="relation-with-other-specifications">4. Relation with Other Specifications</h2>

<p>
The <code>icon</code> section is one of the optional top-level source sections of a canonical <code>.frog</code> file.
It belongs to the source representation defined in <code>Expression/</code>.
</p>

<p>
Its ownership boundary relative to neighboring sections is:
</p>

<pre>Top-level source-section boundary

metadata    -> descriptive program identity and documentation
interface   -> public typed program boundary
diagram     -> authoritative executable graph
front_panel -> optional user-facing interaction surface
icon        -> optional reusable-node visual representation
ide         -> optional IDE-facing authoring preferences and recoverability
cache       -> optional derived tooling data</pre>

<p>
In particular:
</p>

<ul>
  <li><code>icon</code> does not define executable logic. That belongs to <code>diagram</code>.</li>
  <li><code>icon</code> does not define the public API of the FROG. That belongs to <code>interface</code>.</li>
  <li><code>icon</code> is not a replacement for front-panel composition. That belongs to <code>front_panel</code>.</li>
  <li><code>icon</code> is not descriptive program documentation. That belongs to <code>metadata</code>.</li>
  <li><code>icon</code> is not a container for transient editor state or rendering preferences. That belongs to <code>ide</code> or <code>cache</code> depending on ownership.</li>
</ul>

<pre>
Non-executable visual/tooling boundary

metadata -> descriptive identity and documentation
icon     -> durable reusable-node visual identity
ide      -> IDE-facing authoring preferences and recoverability
cache    -> derived rendering or tooling accelerators
</pre>

<hr/>

<h2 id="location">5. Location in a <code>.frog</code> File</h2>

<p>
The <code>icon</code> object is an optional top-level JSON object inside a canonical <code>.frog</code> file.
</p>

<p>
Example:
</p>

<pre>{
  "spec_version": "0.1",
  "metadata": {},
  "interface": {},
  "diagram": {},
  "icon": {}
}</pre>

<p>
A canonical source file MAY omit the <code>icon</code> section entirely.
If omitted, tools MAY render a default icon or another implementation-defined placeholder.
</p>

<pre>Canonical .frog structure

example.frog
├─ spec_version
├─ metadata     -> required descriptive source section
├─ interface    -> required public typed boundary
├─ diagram      -> required executable graph
├─ connector    -> optional reusable-node perimeter mapping
├─ front_panel  -> optional UI composition
├─ icon         -> optional reusable-node visual identity
├─ ide          -> optional IDE-facing preferences
└─ cache        -> optional derived tooling data</pre>

<p>
Omitting <code>icon</code> MUST remain safe.
A conforming toolchain MUST still be able to interpret canonical program meaning without any icon data.
</p>

<hr/>

<h2 id="purpose">6. Purpose of the Icon Section</h2>

<p>
The purpose of the <code>icon</code> section is to provide a compact and recognizable visual representation of a FROG.
</p>

<p>
Typical uses include:
</p>

<ul>
  <li>diagram-node rendering when a FROG is reused as a subprogram,</li>
  <li>palette or library browsing,</li>
  <li>search previews and program pickers,</li>
  <li>visual identity in authoring environments.</li>
</ul>

<p>
The icon is a source-carried visual identifier, not a complete graphical asset system.
It SHOULD remain simple, portable, and stable across tools.
</p>

<pre>
Icon usage model

source-carried icon
        |
        +--> reusable-node rendering
        +--> palette or browser previews
        +--> search results
        +--> library identity
</pre>

<hr/>

<h2 id="structure">7. Icon Object Structure</h2>

<p>
Minimal icon structure:
</p>

<pre>"icon": {
  "size": 40,
  "svg": "&lt;svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 40 40'&gt;...&lt;/svg&gt;"
}</pre>

<p>
Standard fields:
</p>

<ul>
  <li><code>size</code> — logical icon size in source units.</li>
  <li><code>svg</code> — embedded SVG markup as a JSON string.</li>
</ul>

<p>
Field rules:
</p>

<ul>
  <li>If <code>icon</code> is present, <code>icon.svg</code> MUST be present.</li>
  <li>If present, <code>icon.size</code> MUST be an integer.</li>
  <li>If <code>icon.size</code> is omitted, tools MAY assume the standard logical size of <code>40</code>.</li>
</ul>

<p>
The icon object SHOULD remain small and source-friendly.
It is not intended to become a general multimedia container.
</p>

<pre>
Icon structure model

icon
├─ size   -> logical source size
└─ svg    -> embedded self-contained SVG markup
</pre>

<hr/>

<h3 id="authoring-segmentation">7.1 Optional Authoring Segmentation</h3>

<p>
The rendered icon remains the embedded <code>svg</code>. An editor MAY also
preserve durable authoring segmentation so the icon can be reopened without
merging every visible region into one opaque bitmap or one indivisible shape.
</p>

<pre>"icon": {
  "size": 40,
  "svg": "&lt;svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 40 40'&gt;...&lt;/svg&gt;",
  "authoring": {
    "model": "svg-regions-v1",
    "regions": [
      {
        "id": "background",
        "label": "Background",
        "visible": true,
        "locked": false,
        "svg_element_ids": ["background-path"]
      }
    ]
  }
}</pre>

<ul>
  <li><code>authoring.model</code> identifies the durable authoring metadata model.</li>
  <li><code>authoring.regions[]</code> describes independently selectable paint regions or logical groups.</li>
  <li><code>regions[].svg_element_ids[]</code> references stable element ids in <code>icon.svg</code>.</li>
  <li>Region visibility and order MAY be represented in the SVG itself or mirrored in authoring metadata, but both forms MUST agree when present.</li>
</ul>

<p>
When importing SVG, an editor MAY normalize compound artwork into independent
static SVG elements or paint regions so fill, erase, select, move, resize, and
recolor operations target the visible region under the pointer. Such
normalization MUST preserve the rendered result, vector geometry, clipping,
transparency, and logical coordinate system. It MUST NOT rasterize an SVG merely
because the standard preview is 40 x 40.
</p>

<p>
The 40 x 40 value is a logical preview and authoring coordinate convention, not
a raster-resolution requirement. SVG output MUST remain sharp at any supported
render size. Raster imports MAY lose quality when scaled and SHOULD be embedded
or converted through an explicit, source-visible policy.
</p>

<hr/>

<h2 id="svg-requirements">8. SVG Requirements</h2>

<p>
The <code>svg</code> field MUST contain valid SVG markup serialized as a JSON string.
</p>

<p>
The embedded SVG MUST be self-contained.
</p>

<ul>
  <li>It MUST NOT reference remote assets.</li>
  <li>It MUST NOT depend on external images.</li>
  <li>It MUST NOT depend on remote fonts.</li>
  <li>It SHOULD avoid scripting, animation, and other active content.</li>
  <li>It MUST be safe to render in a sandboxed environment.</li>
</ul>

<p>
For portability and predictable rendering, icons SHOULD prefer simple, static SVG content such as:
</p>

<ul>
  <li><code>&lt;path&gt;</code></li>
  <li><code>&lt;rect&gt;</code></li>
  <li><code>&lt;circle&gt;</code></li>
  <li><code>&lt;line&gt;</code></li>
  <li><code>&lt;polyline&gt;</code></li>
  <li><code>&lt;text&gt;</code> when truly needed</li>
</ul>

<p>
Implementations MAY reject unsafe or unsupported SVG subsets for security, portability, or rendering-stability reasons.
</p>

<pre>
Safe SVG rule

Allowed intent:
- static vector description
- self-contained shapes
- stable rendering

Disallowed intent:
- remote dependency
- active behavior
- hidden executable content
</pre>

<hr/>

<h2 id="coordinate-system-and-sizing">9. Coordinate System and Sizing</h2>

<p>
In v0.1, icons are expected to target a <strong>40 x 40 logical grid</strong>.
</p>

<p>
The embedded SVG SHOULD declare either:
</p>

<ul>
  <li><code>viewBox="0 0 40 40"</code>, or</li>
  <li>an equivalent coordinate strategy clearly aligned to a 40-unit logical size.</li>
</ul>

<p>
Recommended form:
</p>

<pre>&lt;svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 40 40"&gt;...&lt;/svg&gt;</pre>

<p>
Tools MAY scale icons for high-DPI or zoomed rendering.
However, the source-level logical coordinate system SHOULD remain stable.
The standard 40 x 40 preview does not require the SVG to be rasterized or
sampled onto forty physical pixels.
</p>

<pre>Logical icon model

source icon
   |
   +-- logical size: 40
   +-- logical grid: 0..40 x 0..40
   |
   +-- SVG viewBox defines source-space geometry
   |
tool rendering
   |
   +-- MAY scale for zoom
   +-- MAY scale for DPI
   +-- MUST NOT change executable meaning</pre>

<hr/>

<h2 id="embedding-and-formatting-rules">10. Embedding and Formatting Rules</h2>

<p>
The SVG is embedded directly as a JSON string inside the <code>icon</code> object.
</p>

<p>
Implementations MUST preserve icon meaning exactly.
JSON escaping is part of normal serialization and does not count as semantic modification.
</p>

<ul>
  <li>Newlines MAY be preserved.</li>
  <li>Whitespace MAY be normalized by formatting tools when that does not alter SVG meaning.</li>
  <li>Formatting tools SHOULD use a stable strategy to avoid noisy diffs.</li>
</ul>

<p>
Implementations SHOULD avoid rewriting the SVG content unnecessarily.
</p>

<pre>
Embedding rule

icon.svg
   -> source-carried SVG string

Formatting may change:
- escaping
- harmless whitespace

Formatting must not change:
- icon geometry meaning
- referenced content model
</pre>

<hr/>

<h2 id="extensibility-and-boundary-rules">11. Extensibility and Boundary Rules</h2>

<p>
Tools MAY add additional icon-related fields for non-executable editing or rendering support.
</p>

<p>
Example:
</p>

<pre>"icon": {
  "size": 40,
  "svg": "&lt;svg&gt;...&lt;/svg&gt;",
  "authoring": {
    "model": "svg-regions-v1",
    "regions": []
  },
  "hint": {
    "category": "math",
    "preferred_bg": "dark"
  }
}</pre>

<p>
Such extensions:
</p>

<ul>
  <li>MAY exist,</li>
  <li>MUST remain non-executable,</li>
  <li>MUST be safely ignorable by runtimes and other execution-facing systems,</li>
  <li>SHOULD be preserved by source-formatting tools when practical.</li>
</ul>

<p>
However, transient editor state or implementation-specific rendering caches SHOULD NOT be stored in <code>icon</code>.
Those belong in <code>ide</code> or <code>cache</code> depending on ownership and durability.
</p>

<pre>
Boundary rule

Durable reusable-node visual identity
    -> icon

Tool-facing authoring preferences or reopen state
    -> ide

Derived rendering or tooling acceleration
    -> cache
</pre>

<p>
Icon extensions SHOULD remain understandable as durable visual-source metadata.
They SHOULD NOT become a hidden transport for executable hints, runtime policies, or editor-private transient state.
</p>

<hr/>

<h2 id="examples">12. Examples</h2>

<h3 id="minimal-example">12.1 Minimal Icon</h3>

<pre>"icon": {
  "size": 40,
  "svg": "&lt;svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 40 40'&gt;&lt;rect x='2' y='2' width='36' height='36' rx='6'/&gt;&lt;/svg&gt;"
}</pre>

<hr/>

<h3 id="symbol-example">12.2 Icon with Simple Symbol</h3>

<pre>"icon": {
  "size": 40,
  "svg": "&lt;svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 40 40'&gt;
    &lt;rect x='2' y='2' width='36' height='36' rx='6' fill='#1f2937'/&gt;
    &lt;path d='M12 20h16' stroke='#ffffff' stroke-width='3' stroke-linecap='round'/&gt;
    &lt;path d='M20 12v16' stroke='#ffffff' stroke-width='3' stroke-linecap='round'/&gt;
  &lt;/svg&gt;"
}</pre>

<hr/>

<h2 id="validation-rules">13. Validation Rules</h2>

<ul>
  <li>If <code>icon</code> is present, it MUST be a JSON object.</li>
  <li>If <code>icon</code> is present, <code>icon.svg</code> MUST be present and MUST be a string.</li>
  <li>If present, <code>icon.size</code> MUST be an integer.</li>
  <li>The SVG SHOULD define a stable logical coordinate system compatible with the standard 40 x 40 grid.</li>
  <li>The SVG MUST be self-contained.</li>
  <li>The SVG MUST NOT alter executable meaning.</li>
  <li>If <code>icon.authoring</code> is present, it MUST remain non-executable and safely ignorable by execution-facing systems.</li>
  <li>Every <code>svg_element_ids</code> reference SHOULD resolve to a stable element id in <code>icon.svg</code>.</li>
  <li>Normalization into independent regions MUST preserve the rendered vector result and MUST NOT rasterize SVG content implicitly.</li>
  <li>Implementations MAY apply stricter safety validation, including rejection of scripts or unsafe constructs.</li>
</ul>

<pre>
Validation boundary

icon validation
   -> checks visual-source structure and SVG safety

icon validation
   != executable validation
   != semantic interpretation of the program
</pre>

<hr/>

<h2 id="design-goals">14. Design Goals</h2>

<ul>
  <li>Provide a compact visual identity for reusable FROG nodes.</li>
  <li>Keep the source representation transparent and version-control-friendly.</li>
  <li>Support stable cross-tool rendering.</li>
  <li>Preserve a clean architectural boundary between visual identity, executable meaning, and IDE/tooling state.</li>
  <li>Remain simple, portable, and non-executable.</li>
</ul>

<pre>
icon should stay:

- visual
- durable
- portable
- small
- non-executable

icon should not become:

- executable
- editor state
- cache payload
- hidden runtime hint channel
</pre>

<hr/>

<h2 id="summary">15. Summary</h2>

<p>
The optional <code>icon</code> section defines a source-carried SVG icon for a FROG program.
It provides durable reusable-node visual identity without affecting executable behavior.
</p>

<p>
This preserves a clean separation between:
</p>

<ul>
  <li>descriptive program identity (<code>metadata</code>),</li>
  <li>public program boundary (<code>interface</code>),</li>
  <li>authoritative executable logic (<code>diagram</code>),</li>
  <li>optional user-facing composition (<code>front_panel</code>),</li>
  <li>optional reusable-node iconography (<code>icon</code>),</li>
  <li>optional IDE-facing authoring metadata (<code>ide</code>),</li>
  <li>optional derived tooling data (<code>cache</code>).</li>
</ul>

<pre>
One-line mental model

icon helps tools recognize a FROG visually
icon does not tell a FROG how to execute
</pre>
