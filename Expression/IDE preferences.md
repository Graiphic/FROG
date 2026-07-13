<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 FROG IDE Preferences Specification</h1>

<p align="center">
Definition of the optional <code>ide</code> section of <strong>.frog</strong> programs<br/>
<em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2 id="contents">Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#why-the-ide-section-exists">2. Why the <code>ide</code> Section Exists</a></li>
  <li><a href="#scope">3. Scope</a></li>
  <li><a href="#relation-with-other-specifications">4. Relation with Other Specifications</a></li>
  <li><a href="#location">5. Location in a <code>.frog</code> File</a></li>
  <li><a href="#core-principles">6. Core Principles</a></li>
  <li><a href="#ownership-boundary">7. Ownership Boundary of the <code>ide</code> Section</a></li>
  <li><a href="#recommended-structure">8. Recommended Structure</a></li>
  <li><a href="#diagram-preferences">9. Diagram Preferences</a></li>
  <li><a href="#front-panel-preferences">10. Front Panel Preferences</a></li>
  <li><a href="#execution-oriented-ide-hints">11. Execution-Oriented IDE Hints</a></li>
  <li><a href="#workflow-preferences">12. Workflow Preferences</a></li>
  <li><a href="#recoverability-metadata">13. Recoverability Metadata</a></li>
  <li><a href="#validation-and-compatibility-rules">14. Validation and Compatibility Rules</a></li>
  <li><a href="#extensibility-and-preservation-rules">15. Extensibility and Preservation Rules</a></li>
  <li><a href="#examples">16. Examples</a>
    <ul>
      <li><a href="#minimal-example">16.1 Minimal IDE Metadata</a></li>
      <li><a href="#preferences-example">16.2 Preferences-Focused Example</a></li>
      <li><a href="#recoverability-example">16.3 Preferences Plus Recoverability Example</a></li>
    </ul>
  </li>
  <li><a href="#design-goals">17. Design Goals</a></li>
  <li><a href="#summary">18. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
The optional top-level <code>ide</code> section stores IDE-facing metadata serialized with a FROG program.
</p>

<p>
Its purpose is to support authoring, editing, reopening, and tool-assisted workflows without changing canonical program meaning.
</p>

<p>
The <code>ide</code> section MAY contain:
</p>

<ul>
  <li>diagram authoring preferences,</li>
  <li>front-panel authoring preferences,</li>
  <li>workflow and editor UX preferences,</li>
  <li>tool-interpreted execution-oriented hints,</li>
  <li>non-authoritative recoverability metadata that helps an IDE reopen or restore authoring state.</li>
</ul>

<p>
The <code>ide</code> section is part of the source expression when present, but it is <strong>non-executable</strong>.
It MUST NOT define, modify, or override execution semantics.
</p>

<p>
Runtimes, compilers, and other execution-facing systems MUST ignore the <code>ide</code> section when determining canonical executable meaning.
</p>

<p>
This section belongs to the FROG Expression because a FROG is a durable editable program unit and MAY carry source-level authoring preferences and recoverability aids together with its canonical program content, provided that such metadata remains non-authoritative for execution.
</p>

<pre>
One-line model

ide
   -> source-carried authoring metadata
   -> useful to tools
   -> non-authoritative for execution
</pre>

<hr/>

<h2 id="why-the-ide-section-exists">2. Why the <code>ide</code> Section Exists</h2>

<p>
A canonical source file sometimes needs to preserve authoring intent or reopen-friendly editor state that is useful to IDEs but irrelevant to executable meaning.
That is the role of the optional <code>ide</code> section.
</p>

<p>
Without such a section, tools would either:
</p>

<ul>
  <li>lose useful authoring recoverability,</li>
  <li>store durable editor metadata in the wrong source section,</li>
  <li>or misuse derived cache artifacts for information that should remain source-carried and diff-visible.</li>
</ul>

<p>
The <code>ide</code> section therefore exists to keep a clean separation between:
</p>

<ul>
  <li>descriptive identity,</li>
  <li>authoritative executable content,</li>
  <li>IDE-facing durable preferences and recoverability,</li>
  <li>derived tooling acceleration data.</li>
</ul>

<pre>
Why ide exists

metadata -> what the FROG is
diagram  -> how the FROG executes
ide      -> how editors may reopen and present it
cache    -> regenerated tooling acceleration
</pre>

<hr/>

<h2 id="scope">3. Scope</h2>

<p>
This document specifies:
</p>

<ul>
  <li>the role of the optional top-level <code>ide</code> section,</li>
  <li>the boundary between IDE-facing metadata and canonical executable source,</li>
  <li>the recommended structure of the <code>ide</code> object,</li>
  <li>validation and compatibility rules for IDE metadata,</li>
  <li>recoverability-oriented source metadata that MAY be serialized with a FROG.</li>
</ul>

<p>
This document does not specify:
</p>

<ul>
  <li>the public interface of the FROG,</li>
  <li>the authoritative executable graph,</li>
  <li>the normative execution semantics of the language,</li>
  <li>the runtime scheduler,</li>
  <li>derived cache content,</li>
  <li>a mandatory IDE implementation architecture.</li>
</ul>

<pre>
This document owns:
- source-carried IDE metadata
- authoring preferences
- recoverability metadata
- compatibility rules for ide fields

This document does not own:
- executable graph meaning
- runtime behavior
- cache design
- IDE product architecture as a whole
</pre>

<hr/>

<h2 id="relation-with-other-specifications">4. Relation with Other Specifications</h2>

<p>
The <code>ide</code> section is one of the optional top-level source sections of a canonical <code>.frog</code> file.
It belongs to <code>Expression/</code>.
</p>

<p>
Its ownership boundary relative to neighboring source sections is:
</p>

<pre>Top-level source-section boundary

metadata    -> descriptive program identity and documentation
interface   -> public typed program boundary
diagram     -> authoritative executable graph
front_panel -> optional user-facing interaction composition
icon        -> optional reusable-node visual identity
ide         -> optional IDE-facing authoring preferences and recoverability metadata
cache       -> optional derived tooling data</pre>

<p>
In particular:
</p>

<ul>
  <li><code>ide</code> does not define executable logic. That belongs to <code>diagram</code>.</li>
  <li><code>ide</code> does not define the public API. That belongs to <code>interface</code>.</li>
  <li><code>ide</code> does not replace front-panel composition. That belongs to <code>front_panel</code>.</li>
  <li><code>ide</code> does not replace descriptive identity and documentation. That belongs to <code>metadata</code>.</li>
  <li><code>ide</code> does not replace derived accelerators or regenerated artifacts. That belongs to <code>cache</code>.</li>
</ul>

<pre>
Non-executable source/tooling boundary

metadata -> durable descriptive identity
icon     -> durable reusable-node visual identity
ide      -> source-carried authoring preferences and recoverability aids
cache    -> derived tooling accelerators safe to delete and regenerate
</pre>

<hr/>

<h2 id="location">5. Location in a <code>.frog</code> File</h2>

<p>
The <code>ide</code> object is an optional top-level JSON object inside a canonical <code>.frog</code> file.
</p>

<p>
Example:
</p>

<pre>{
  "spec_version": "0.1",
  "metadata": {},
  "interface": {},
  "diagram": {},
  "ide": {}
}</pre>

<p>
If omitted, tools MAY use implementation-defined defaults and MAY operate without source-carried IDE recoverability metadata.
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
├─ ide          -> optional IDE-facing authoring metadata
└─ cache        -> optional derived tooling data</pre>

<p>
Omitting <code>ide</code> MUST remain safe.
A conforming toolchain MUST still be able to interpret canonical program meaning without any <code>ide</code> data.
</p>

<hr/>

<h2 id="core-principles">6. Core Principles</h2>

<ul>
  <li><strong>Tool-facing but source-carried</strong> — IDE metadata MAY be serialized with the FROG source itself.</li>
  <li><strong>Non-authoritative</strong> — IDE metadata MUST NEVER change canonical program meaning.</li>
  <li><strong>Safe to ignore</strong> — a conforming runtime or other execution-facing system MUST be able to ignore the entire <code>ide</code> section safely.</li>
  <li><strong>Recoverable but optional</strong> — IDE metadata MAY help reopen or restore authoring state, but it MUST NOT be required to interpret the program canonically.</li>
  <li><strong>Stable diffs</strong> — tools SHOULD minimize noisy source changes in this section.</li>
  <li><strong>Inter-tool survivability</strong> — tools SHOULD preserve unknown fields whenever practical.</li>
  <li><strong>Canonical source wins</strong> — if a conflict exists between canonical source sections and <code>ide</code> metadata, canonical source sections MUST win.</li>
</ul>

<pre>
Conflict rule

canonical source sections
    always win

ide metadata
    never overrides
canonical program meaning
</pre>

<hr/>

<h2 id="ownership-boundary">7. Ownership Boundary of the <code>ide</code> Section</h2>

<p>
The <code>ide</code> section is the source-level home for metadata that is meaningful to editors and authoring tools but not authoritative for execution.
</p>

<p>
It MAY contain:
</p>

<ul>
  <li>diagram editing preferences,</li>
  <li>front-panel editing preferences,</li>
  <li>workflow preferences,</li>
  <li>execution-oriented IDE hints,</li>
  <li>non-authoritative recoverability metadata for guided authoring, presentation restoration, or editor reopen flows.</li>
</ul>

<p>
It MUST NOT contain:
</p>

<ul>
  <li>canonical executable structure,</li>
  <li>runtime-required semantic data,</li>
  <li>hidden information needed to understand canonical program meaning,</li>
  <li>data that redefines primitive identity, structure identity, type identity, or language semantics,</li>
  <li>derived cache content whose intended lifecycle is delete-and-regenerate optimization.</li>
</ul>

<p>
A practical decision rule is:
</p>

<pre>If the data is:
- needed only to reopen or author more comfortably -> ide
- derived from source and safe to regenerate       -> cache
- required to understand executable meaning        -> not ide</pre>

<pre>
Boundary rule

Useful for editing and reopening?
    -> ide

Useful only as regenerated accelerator?
    -> cache

Needed for canonical meaning?
    -> not ide
</pre>

<hr/>

<h2 id="recommended-structure">8. Recommended Structure</h2>

<p>
The exact structure of the <code>ide</code> object is extensible.
The following shape is recommended for the standardized v0.1 baseline:
</p>

<pre>"ide": {
  "diagram": {},
  "front_panel": {},
  "execution": {},
  "workflow": {},
  "recoverability": {},
  "provenance": {}
}</pre>

<p>
All sub-objects are optional.
Tools MAY omit any sub-object they do not use.
Tools MAY add additional keys as long as they preserve the non-authoritative status of this section.
</p>

<pre>
Recommended ide structure

ide
├─ diagram
├─ front_panel
├─ execution
├─ workflow
├─ recoverability
└─ provenance
</pre>

<p>
This structure is recommended for clarity, not required as a closed schema for every implementation.
</p>

<p>
The optional <code>provenance</code> sub-object is defined by <code>Expression/Source provenance.md</code>. It may carry authoring, generation, import, modification, signature, issuer, and human-review attestations. It remains part of the <code>ide</code> section because it is authoring and audit metadata, not executable program meaning.
</p>

<hr/>

<h2 id="diagram-preferences">9. Diagram Preferences</h2>

<p>
The <code>diagram</code> sub-object stores preferences for diagram rendering and editing.
</p>

<p>
Example:
</p>

<pre>"diagram": {
  "background_color": "#202020",
  "grid": {
    "enabled": true,
    "size": 10,
    "snap": true
  },
  "wires": {
    "style": "curved",
    "show_dots": false
  },
  "zoom": {
    "default": 1.0
  }
}</pre>

<p>
Suggested fields include:
</p>

<ul>
  <li><code>background_color</code> — string color</li>
  <li><code>grid.enabled</code> — boolean</li>
  <li><code>grid.size</code> — integer</li>
  <li><code>grid.snap</code> — boolean</li>
  <li><code>wires.style</code> — string such as <code>curved</code> or <code>orthogonal</code></li>
  <li><code>wires.show_dots</code> — boolean</li>
  <li><code>zoom.default</code> — number</li>
</ul>

<p>
These preferences affect authoring presentation only.
They MUST NOT affect canonical diagram meaning.
</p>

<pre>
Diagram preferences affect:
- presentation
- editing comfort
- default editor behavior

Diagram preferences do not affect:
- node identity
- wire meaning
- type meaning
- execution meaning
</pre>

<hr/>

<h2 id="front-panel-preferences">10. Front Panel Preferences</h2>

<p>
The <code>front_panel</code> sub-object stores preferences for front-panel editing and rendering.
</p>

<p>
Example:
</p>

<pre>"front_panel": {
  "background_color": "#FFFFFF",
  "background_opacity": 100,
  "grid_color": "#D2D2D2",
  "grid_opacity": 100,
  "alignment_guides": {
    "enabled": true
  },
  "snap": {
    "enabled": true,
    "size": 5
  },
  "zoom_percent": 100,
  "viewport_origin": {
    "x": 0,
    "y": 0
  }
}</pre>

<p>
Suggested fields include:
</p>

<ul>
  <li><code>background_color</code> — string color, or <code>"transparent"</code> for a transparent document-level front-panel background</li>
  <li><code>background_opacity</code> — optional integer percentage from <code>0</code> to <code>100</code>; omitted means fully opaque</li>
  <li><code>grid_color</code> — optional string color for the document-level front-panel grid</li>
  <li><code>grid_opacity</code> — optional integer percentage from <code>0</code> to <code>100</code>; omitted means fully opaque</li>
  <li><code>alignment_guides.enabled</code> — boolean</li>
  <li><code>snap.enabled</code> — boolean</li>
  <li><code>snap.size</code> — integer</li>
  <li><code>zoom_percent</code> — integer percentage for the document's authoring view</li>
  <li><code>viewport_origin.x</code> — number identifying the canvas x-coordinate at the viewport's upper-left corner</li>
  <li><code>viewport_origin.y</code> — number identifying the canvas y-coordinate at the viewport's upper-left corner</li>
</ul>

<p>
The front-panel background preference is document-level IDE presentation.
It may be saved in the <code>.frog</code> so the same source reopens with the same authoring background.
It is distinct from a local IDE default theme, which belongs to tool configuration and not to source.
The zoom and viewport origin are document-level authoring state for reopening the same working view.
An IDE-wide navigation font preference is local tool configuration: it MUST NOT change Front Panel widget typography, bounds, or authored layout.
The useful Front Panel surface dimensions do not belong to <code>ide.front_panel</code>:
they are canonical presentation intent stored as <code>front_panel.canvas.width</code>
and <code>front_panel.canvas.height</code>. They remain independent from the IDE
window position, native chrome, navigation font, zoom, and viewport origin.
When an IDE exposes paired fill/stroke color tools for the front-panel canvas, the fill/front color maps to
<code>background_color</code> and the stroke/back color maps to <code>grid_color</code>.
</p>

<p>
These preferences affect editor UX only.
They MUST NOT redefine widget behavior, front-panel semantics, or public interface meaning.
</p>

<pre>
Front-panel preferences affect:
- editor presentation
- layout assistance
- snap behavior

They do not affect:
- widget semantic identity
- widget value behavior
- public interface
</pre>

<hr/>

<h2 id="execution-oriented-ide-hints">11. Execution-Oriented IDE Hints</h2>

<p>
The <code>execution</code> sub-object stores execution-oriented hints interpreted by IDEs and related authoring tools.
</p>

<p>
These hints MAY influence how an IDE configures:
</p>

<ul>
  <li>launch defaults,</li>
  <li>debugging behavior,</li>
  <li>validation workflows,</li>
  <li>build actions,</li>
  <li>target-selection preferences.</li>
</ul>

<p>
However, they MUST NOT define runtime semantics, scheduling semantics, memory semantics, type semantics, or canonical graph meaning.
</p>

<p>
Example:
</p>

<pre>"execution": {
  "debug_enabled": true,
  "reentrant_hint": false,
  "preferred_profile": "core",
  "run_on_open": false,
  "auto_validate_on_save": true
}</pre>

<p>
Suggested fields include:
</p>

<ul>
  <li><code>debug_enabled</code> — boolean</li>
  <li><code>reentrant_hint</code> — boolean</li>
  <li><code>preferred_profile</code> — preferred tooling profile label for authoring or launch workflows</li>
  <li><code>run_on_open</code> — boolean</li>
  <li><code>auto_validate_on_save</code> — boolean</li>
</ul>

<p>
Runtimes and other execution-facing systems MUST ignore these hints when determining executable semantics.
</p>

<p>
A field such as <code>preferred_profile</code> is only a tooling preference.
It MUST NOT be interpreted as a normative support claim for that profile.
</p>

<pre>
Execution hint rule

Execution hints may influence:
- IDE defaults
- launch choices
- validation workflows

Execution hints may not define:
- runtime semantics
- support claims
- canonical execution meaning
</pre>

<hr/>

<h2 id="workflow-preferences">12. Workflow Preferences</h2>

<p>
The <code>workflow</code> sub-object stores editor UX and authoring workflow preferences.
</p>

<p>
Example:
</p>

<pre>"workflow": {
  "panes": {
    "show_palette": true,
    "show_properties": true
  },
  "search": {
    "default_scope": "project"
  },
  "history": {
    "remember_window_state": true
  }
}</pre>

<p>
Suggested fields include:
</p>

<ul>
  <li><code>panes.show_palette</code> — boolean</li>
  <li><code>panes.show_properties</code> — boolean</li>
  <li><code>search.default_scope</code> — string</li>
  <li><code>history.remember_window_state</code> — boolean</li>
</ul>

<p>
These fields are authoring conveniences only.
They MUST NOT affect canonical source meaning.
</p>

<pre>
Workflow preferences answer:
- how the editor opens
- how panes are shown
- how search behaves by default

They do not answer:
- what the program means
- how the program executes
</pre>

<hr/>

<h2 id="recoverability-metadata">13. Recoverability Metadata</h2>

<p>
The <code>recoverability</code> sub-object stores non-authoritative IDE metadata that helps an editor restore or reopen authoring state that would otherwise be inconvenient to reconstruct from canonical source alone.
</p>

<p>
Recoverability metadata MAY support:
</p>

<ul>
  <li>guided authoring reopen workflows,</li>
  <li>presentation-state restoration,</li>
  <li>stable mapping between IDE-level instances and owned canonical objects,</li>
  <li>other authoring-layer reopen needs that remain non-authoritative for execution.</li>
</ul>

<p>
Recoverability metadata MUST obey all of the following:
</p>

<ul>
  <li>it MUST remain optional,</li>
  <li>it MUST remain non-authoritative for execution semantics,</li>
  <li>it MUST NOT be required to determine canonical executable meaning,</li>
  <li>it MUST be safely ignorable by runtimes, compilers, and other execution-facing systems,</li>
  <li>it SHOULD be preserved by IDEs and formatting tools whenever practical.</li>
</ul>

<p>
One important class of use case is Express authoring recoverability.
For example, an IDE MAY preserve enough state to reopen a guided Express configuration, restore optional terminal visibility, or keep a stable mapping between an Express-authored instance and its canonical target.
</p>

<p>
Example:
</p>

<pre>"recoverability": {
  "express_bindings": [
    {
      "instance_id": "expr_001",
      "express_id": "frog.ide.express.read_text_file",
      "canonical_target_kind": "subfrog",
      "canonical_target_ref": "frog.io.read_text_file.basic",
      "visible_optional_terminals": ["error_in", "error_out"],
      "config": {
        "encoding": "utf8",
        "line_endings": "auto"
      }
    }
  ]
}</pre>

<p>
This example is conceptual.
It illustrates the class of source-level recoverability aids that MAY be stored in <code>ide</code>.
It does not define canonical execution meaning.
</p>

<pre>
Recoverability normalization model

IDE guided authoring state
          |
          v
recoverability metadata in source
          |
          v
canonical source sections remain authoritative
(interface / diagram / front_panel / ...)
          |
          v
execution-facing systems ignore recoverability
</pre>

<pre>
Recoverability rule

Useful to reopen an editor flow?
    -> maybe ide.recoverability

Needed to execute correctly?
    -> not recoverability
</pre>

<hr/>

<h2 id="validation-and-compatibility-rules">14. Validation and Compatibility Rules</h2>

<p>
Implementations MUST enforce the following rules:
</p>

<ul>
  <li>If <code>ide</code> exists, it MUST be a JSON object.</li>
  <li>If present, each known sub-object such as <code>diagram</code>, <code>front_panel</code>, <code>execution</code>, <code>workflow</code>, or <code>recoverability</code> MUST be a JSON object of the expected shape for that tool.</li>
  <li>IDE metadata MUST NOT redefine execution semantics.</li>
  <li>IDE metadata MUST NOT be required for canonical semantic interpretation.</li>
  <li>Unknown IDE fields MUST be safely ignored by runtimes and other execution-facing systems.</li>
  <li>If present, <code>ide.provenance</code> MUST remain non-executable and MUST NOT be required for canonical semantic interpretation.</li>
  <li>Missing <code>ide.provenance</code> MUST be interpreted as unknown or unattested provenance, not as automatically AI-generated source.</li>
  <li>If <code>ide</code> metadata conflicts with canonical source content, canonical source content MUST win.</li>
  <li>Recoverability metadata MUST remain optional and non-authoritative.</li>
</ul>

<p>
Recoverability and provenance have different responsibilities. <code>ide.recoverability</code> helps reopen or reconstruct authoring flows. <code>ide.provenance</code> records source-carried evidence about authoring origin, tool generation, import, signature, issuer trust, and human review. Neither sub-object may redefine executable meaning.
</p>

<p>
Tools SHOULD preserve unknown fields to support interoperability between editors and future extensions of the <code>ide</code> model.
</p>

<pre>
Validation boundary

ide validation
   -> checks authoring metadata structure

ide validation
   != executable validation
   != semantic validation of the program itself
</pre>

<hr/>

<h2 id="extensibility-and-preservation-rules">15. Extensibility and Preservation Rules</h2>

<p>
The <code>ide</code> section is intentionally extensible.
Tools MAY add additional keys, for example:
</p>

<pre>"ide": {
  "diagram": {
    "background_color": "#202020"
  },
  "recoverability": {
    "custom_editor_state": {
      "feature_x_enabled": true
    }
  },
  "custom_tool": {
    "layout_profile": "compact"
  }
}</pre>

<p>
The following extensibility rules apply:
</p>

<ul>
  <li>unknown fields MUST NOT impact execution semantics,</li>
  <li>unknown fields SHOULD be preserved by formatting tools and IDEs whenever practical,</li>
  <li>vendor-specific or tool-specific fields SHOULD be namespaced or clearly grouped when that improves interoperability,</li>
  <li>recoverability-oriented extensions MUST remain non-authoritative for execution,</li>
  <li>derived optimization content SHOULD go to <code>cache</code>, not <code>ide</code>.</li>
</ul>

<pre>
Extensibility rule

New ide fields are allowed
if they stay:
- tool-facing
- non-executable
- safely ignorable
- outside cache territory
</pre>

<hr/>

<h2 id="examples">16. Examples</h2>

<h3 id="minimal-example">16.1 Minimal IDE Metadata</h3>

<pre>"ide": {
  "diagram": {
    "background_color": "#202020"
  }
}</pre>

<hr/>

<h3 id="preferences-example">16.2 Preferences-Focused Example</h3>

<pre>"ide": {
  "diagram": {
    "background_color": "#202020",
    "grid": { "enabled": true, "size": 10, "snap": true },
    "wires": { "style": "curved", "show_dots": false },
    "zoom": { "default": 1.0 }
  },
  "front_panel": {
    "background_color": "#FFFFFF",
    "background_opacity": 100,
    "grid_color": "#D2D2D2",
    "grid_opacity": 100,
    "alignment_guides": { "enabled": true },
    "snap": { "enabled": true, "size": 5 }
  },
  "execution": {
    "debug_enabled": true,
    "reentrant_hint": false,
    "preferred_profile": "core",
    "run_on_open": false,
    "auto_validate_on_save": true
  },
  "workflow": {
    "panes": { "show_palette": true, "show_properties": true },
    "search": { "default_scope": "project" },
    "history": { "remember_window_state": true }
  }
}</pre>

<hr/>

<h3 id="recoverability-example">16.3 Preferences Plus Recoverability Example</h3>

<pre>"ide": {
  "diagram": {
    "background_color": "#202020",
    "grid": { "enabled": true, "size": 10, "snap": true }
  },
  "workflow": {
    "panes": { "show_palette": true, "show_properties": true }
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
}</pre>

<hr/>

<h2 id="design-goals">17. Design Goals</h2>

<ul>
  <li>Allow source-carried IDE preferences without redefining canonical program meaning.</li>
  <li>Support durable authoring workflows and reopen-friendly editing experiences.</li>
  <li>Support optional recoverability metadata for guided authoring flows.</li>
  <li>Preserve a clean architectural boundary between source authority, IDE convenience, and derived cache.</li>
  <li>Remain tool-agnostic, non-executable, and safe to ignore for execution.</li>
  <li>Keep serialized IDE metadata compatible with transparent long-term source control.</li>
</ul>

<pre>
ide should stay:

- tool-facing
- optional
- recoverable
- transparent
- non-authoritative

ide should not become:

- executable
- cache
- hidden runtime contract
- semantic source authority
</pre>

<hr/>

<h2 id="summary">18. Summary</h2>

<p>
The optional <code>ide</code> section stores source-carried IDE-facing metadata for a FROG program.
It MAY store authoring preferences, workflow preferences, execution-oriented IDE hints, and recoverability aids, while remaining strictly non-authoritative for execution.
</p>

<p>
This preserves a clean long-term separation between:
</p>

<ul>
  <li>descriptive source identity (<code>metadata</code>),</li>
  <li>public program boundary (<code>interface</code>),</li>
  <li>authoritative executable logic (<code>diagram</code>),</li>
  <li>optional user-facing composition (<code>front_panel</code>),</li>
  <li>optional IDE-facing authoring metadata (<code>ide</code>),</li>
  <li>optional derived tooling data (<code>cache</code>).</li>
</ul>

<pre>
One-line mental model

ide helps editors reopen and present a FROG
ide does not tell a FROG how to execute
</pre>
