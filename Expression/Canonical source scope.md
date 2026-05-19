<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Canonical Source Scope</h1>

<p align="center">
  <strong>Public scope matrix for what belongs in a canonical <code>.frog</code> source file</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This document closes the first source-boundary question for the public FROG
specification:
</p>

<pre><code>What is canonical .frog source?
What is required?
What is optional?
What is source metadata?
What belongs to the diagram?
What belongs to the front panel?
What is a binding?
What must remain outside executable source meaning?</code></pre>

<p>
It is a scope guide for canonical source ownership. It does not define the full
semantic validation algorithm, the FIR object model, or any runtime behavior.
</p>

<hr/>

<h2>Core Rule</h2>

<pre><code>.frog is the canonical program source.
.wfrog is referenced widget package source.
FIR is derived later from validated program meaning.
Runtime-private state is not canonical source.</code></pre>

<p>
The canonical <code>.frog</code> file may contain both executable and
non-executable source facts. Their presence in the source file does not make
all of them execution semantics.
</p>

<hr/>

<h2>Scope Classifications</h2>

<table>
  <thead>
    <tr>
      <th>Classification</th>
      <th>Meaning</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>required_source</code></td>
      <td>Must be present in a structurally valid canonical <code>.frog</code> file.</td>
    </tr>
    <tr>
      <td><code>optional_source</code></td>
      <td>May be present and, when present, is still part of canonical source.</td>
    </tr>
    <tr>
      <td><code>descriptive_metadata</code></td>
      <td>Source-carried descriptive information that must not redefine program meaning.</td>
    </tr>
    <tr>
      <td><code>execution_source</code></td>
      <td>Source content that can contribute to later validated program meaning.</td>
    </tr>
    <tr>
      <td><code>presentation_source</code></td>
      <td>Source-owned presentation, layout, or authoring information that is not execution meaning by itself.</td>
    </tr>
    <tr>
      <td><code>binding_source</code></td>
      <td>Explicit source relation connecting two source-owned objects, such as widget value participation or interface participation.</td>
    </tr>
    <tr>
      <td><code>external_reference</code></td>
      <td>Reference to a related artifact such as a <code>.wfrog</code> package or library identity without embedding that artifact as program source.</td>
    </tr>
    <tr>
      <td><code>non_authoritative_cache</code></td>
      <td>Tooling convenience content that must be ignorable without changing source meaning.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Top-Level Source Sections</h2>

<table>
  <thead>
    <tr>
      <th>Section</th>
      <th>Presence</th>
      <th>Primary role</th>
      <th>Source scope</th>
      <th>Execution posture</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>spec_version</code></td>
      <td>Required</td>
      <td>Source-format compatibility anchor.</td>
      <td><code>required_source</code></td>
      <td>Validation input, not execution meaning by itself.</td>
    </tr>
    <tr>
      <td><code>metadata</code></td>
      <td>Required</td>
      <td>Program identity, description, authorship, and descriptive fields.</td>
      <td><code>required_source</code>, <code>descriptive_metadata</code></td>
      <td>Non-executable unless a future explicit rule gives a field execution relevance.</td>
    </tr>
    <tr>
      <td><code>interface</code></td>
      <td>Required</td>
      <td>Public typed inputs and outputs.</td>
      <td><code>required_source</code>, <code>execution_source</code></td>
      <td>Semantic validation input and FIR-relevant when accepted.</td>
    </tr>
    <tr>
      <td><code>diagram</code></td>
      <td>Required</td>
      <td>Authoritative executable graph.</td>
      <td><code>required_source</code>, <code>execution_source</code></td>
      <td>Primary semantic contributor for nodes, edges, structures, state, constants, and calls.</td>
    </tr>
    <tr>
      <td><code>front_panel</code></td>
      <td>Optional</td>
      <td>User-facing widget composition and source-owned instance data.</td>
      <td><code>optional_source</code>, mixed execution and presentation source.</td>
      <td>Execution-relevant only through explicit widget value, event, reference, binding, or initialization participation.</td>
    </tr>
    <tr>
      <td><code>connector</code></td>
      <td>Optional</td>
      <td>Graphical projection of existing interface ports for reuse as a node.</td>
      <td><code>optional_source</code>, <code>presentation_source</code></td>
      <td>Must not create new interface meaning.</td>
    </tr>
    <tr>
      <td><code>icon</code></td>
      <td>Optional</td>
      <td>Reusable-node icon representation.</td>
      <td><code>optional_source</code>, <code>presentation_source</code></td>
      <td>Non-executable.</td>
    </tr>
    <tr>
      <td><code>ide</code></td>
      <td>Optional</td>
      <td>IDE-facing preferences, source provenance, and recoverability hints.</td>
      <td><code>optional_source</code>, <code>descriptive_metadata</code></td>
      <td>Must not redefine execution semantics.</td>
    </tr>
    <tr>
      <td><code>cache</code></td>
      <td>Optional</td>
      <td>Non-authoritative tool cache.</td>
      <td><code>optional_source</code>, <code>non_authoritative_cache</code></td>
      <td>Must be safely ignorable and regenerable.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Diagram Scope</h2>

<p>
The <code>diagram</code> section is the authoritative executable graph of the
FROG source file.
</p>

<p>
Diagram-owned canonical source includes:
</p>

<ul>
  <li>nodes and their stable source identities,</li>
  <li>node family, primitive, structure, function, library, or widget-interaction identity,</li>
  <li>typed ports where source-visible,</li>
  <li>edges and endpoint identities,</li>
  <li>constants that feed executable graph meaning,</li>
  <li>explicit structures and structure-local boundaries,</li>
  <li>explicit local state, delay, feedback, or recurrence carriers,</li>
  <li>diagram-side widget value, event, reference, property, or method participation,</li>
  <li>source attribution and diagnostics anchors where present.</li>
</ul>

<p>
Diagram layout, comments, and authoring placement may be source-owned, but
they do not become execution order, dependency order, or hidden semantic
grouping unless a published structure explicitly says so.
</p>

<hr/>

<h2>Front-Panel Scope</h2>

<p>
The <code>front_panel</code> section is canonical source when present. It is
not a runtime skin and not an IDE-only preview.
</p>

<p>
Front-panel source may include:
</p>

<ul>
  <li>panel canvas and layout facts,</li>
  <li>widget instance identities,</li>
  <li>widget class references,</li>
  <li>widget role, such as control or indicator,</li>
  <li>widget value type, initial value, selected value, or default value where source-owned,</li>
  <li>source-owned labels, captions, and instance-level visual overrides,</li>
  <li>source-owned bindings to diagram or interface participation,</li>
  <li>references to <code>.wfrog</code> realization packages or Default assets.</li>
</ul>

<p>
Only explicit execution-facing participation affects validated meaning.
Pure layout, colors, borders, text placement, hover visuals, and realization
geometry remain front-panel source or realization input. They must not be
promoted into execution semantics or hardcoded in a runtime.
</p>

<hr/>

<h2>Binding Scope</h2>

<p>
A binding is an explicit canonical source relationship. It is not inferred
from visual proximity.
</p>

<p>
Bindings may connect:
</p>

<ul>
  <li>an interface port to diagram participation,</li>
  <li>a widget value to diagram participation,</li>
  <li>a widget event to diagram participation,</li>
  <li>a widget reference to standardized UI-object operations,</li>
  <li>a source value to executable initialization where the published source model allows it.</li>
</ul>

<p>
Bindings that affect execution are semantic validation inputs. Bindings that
only assist host realization or presentation remain support information.
</p>

<hr/>

<h2>Widget Package References</h2>

<p>
A <code>.frog</code> file may reference <code>.wfrog</code> packages and
Default realization assets. Those references are canonical source references,
but the package contents are not embedded into the program source.
</p>

<pre><code>.frog owns:
  widget instance identity
  widget instance layout
  widget role
  widget value participation
  widget bindings
  source-owned initial values
  source-owned instance visual overrides

.wfrog owns:
  widget class/package publication
  realization references
  asset ids
  supported class surfaces
  host capability declarations

Default assets own:
  reusable SVG templates
  public parts and anchors
  default visual behavior
</code></pre>

<p>
This preserves the LabVIEW-like source/user-interface relationship without
turning runtime rendering or SVG geometry into language semantics.
</p>

<hr/>

<h2>Structural Validity Boundary</h2>

<p>
Structural validity checks whether the canonical source shape is acceptable as
source. It does not prove that the program is semantically valid.
</p>

<p>
Structural validation must establish, at minimum:
</p>

<ul>
  <li>required top-level sections are present,</li>
  <li>optional sections have the expected top-level shape when present,</li>
  <li>stable ids are present where required,</li>
  <li>section-local objects have the expected source-owned shape,</li>
  <li>references are syntactically well-formed,</li>
  <li>cache and IDE content do not replace authoritative sections.</li>
</ul>

<p>
Semantic validation remains downstream and is owned by <code>Language/</code>,
<code>Libraries/</code>, and <code>Profiles/</code> as appropriate.
</p>

<hr/>

<h2>Exclusions</h2>

<p>
The following are not canonical source meaning:
</p>

<ul>
  <li>runtime-private state,</li>
  <li>host overlay geometry produced after package resolution,</li>
  <li>compiler-private lowering data,</li>
  <li>IDE-only transient selection state not serialized under the published <code>ide</code> model,</li>
  <li>reconstructed cache data,</li>
  <li>visual hints used as hidden execution dependencies,</li>
  <li>duplicated widget skins that replace Default package references when a Default realization exists.</li>
</ul>

<hr/>

<h2>Summary</h2>

<p>
The canonical <code>.frog</code> file is broader than the executable diagram,
but narrower than a full IDE project or runtime package. It owns the source
program, its public interface, its executable diagram, optional front-panel
instances, explicit bindings, and source-owned metadata. It does not own
runtime-private behavior, compiler-private lowering, or widget realization
assets beyond explicit source references.
</p>
