<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Widget Engineering Reference Standard</h1>

<p align="center">
  <strong>Public structure for FROG widget class-law documentation</strong><br/>
  <em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Purpose</h2>

<p>
This document defines the recommended public organization for standard FROG
widget engineering references. It is a documentation standard for widget class
law, not an implementation recipe for any private runtime or IDE.
</p>

<p>
Each widget family document should describe what an independent implementation
must preserve: class identity, values, properties, methods, events, source
ownership, realization ownership, public parts, and validated limits.
</p>

<hr/>

<h2>Recommended Sections</h2>

<table>
  <thead>
    <tr>
      <th>Section</th>
      <th>Required content</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Class Identity</td>
      <td>Widget class ids, family name, role category, and supported control/indicator/support forms.</td>
    </tr>
    <tr>
      <td>Role Posture</td>
      <td>Whether the widget is a control, indicator, command, support surface, layout host, or container.</td>
    </tr>
    <tr>
      <td>Value Model</td>
      <td>Diagram-facing value shape, initial/default value posture, read-only posture, and source ownership.</td>
    </tr>
    <tr>
      <td>Public Properties</td>
      <td>Common and family-specific attributes, including visual style, behavior toggles, viewport state, and binding surfaces.</td>
    </tr>
    <tr>
      <td>Methods</td>
      <td>Callable widget operations, if any, with inputs, outputs, side effects, and unsupported surfaces named explicitly.</td>
    </tr>
    <tr>
      <td>Events</td>
      <td>Event surfaces such as value change, selection change, edit commit, page change, or pointer activation.</td>
    </tr>
    <tr>
      <td>Source Ownership</td>
      <td>Which data belongs to <code>.frog</code>: layout, labels, instance values, bindings, behavior settings, and per-instance visual overrides.</td>
    </tr>
    <tr>
      <td>Realization Ownership</td>
      <td>Which data belongs to <code>.wfrog</code>: realization ids, asset references, supported classes, host capabilities, and realization-level defaults.</td>
    </tr>
    <tr>
      <td>Default SVG Parts</td>
      <td>Published parts, anchors, bindings, slots, and repeated regions that a host may target.</td>
    </tr>
    <tr>
      <td>Visual States</td>
      <td>Normal, hover, pressed, selected, disabled, read-only, focused, overflow, or equivalent states relevant to the widget.</td>
    </tr>
    <tr>
      <td>Container Compatibility</td>
      <td>Whether the widget can be embedded in Array, Cluster, Tab, Subpanel, or future container families, and which sizing policy applies.</td>
    </tr>
    <tr>
      <td>Example References</td>
      <td>Accepted examples and reference snapshots that demonstrate the current class-law surface.</td>
    </tr>
    <tr>
      <td>Validated Limitations</td>
      <td>Known unsupported or deferred behavior, without pretending it is already implemented.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Public Property Table Shape</h2>

<p>
Widget documents should use a consistent table when listing properties:
</p>

<table>
  <thead>
    <tr>
      <th>Property</th>
      <th>Type</th>
      <th>Owner</th>
      <th>Applies to</th>
      <th>Meaning</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>value.*</code></td>
      <td>family-specific</td>
      <td><code>.frog</code></td>
      <td>control / indicator</td>
      <td>Primary diagram-facing value or public value mirror.</td>
    </tr>
    <tr>
      <td><code>style.*</code></td>
      <td>family-specific</td>
      <td><code>.frog</code> or realization-published default</td>
      <td>visible widget parts</td>
      <td>Source-owned appearance override, with realization defaults only when the source omits a value.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Method Table Shape</h2>

<table>
  <thead>
    <tr>
      <th>Method</th>
      <th>Inputs</th>
      <th>Outputs</th>
      <th>State changed</th>
      <th>Notes</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>example_method</code></td>
      <td>Typed inputs or none</td>
      <td>Typed outputs or status</td>
      <td>Named widget/front-panel state only</td>
      <td>Document whether it is optional, host-capability-backed, or deferred.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2>Boundary Rule</h2>

<p>
The engineering reference describes the public widget contract. It must not
publish Graiphic private runtime algorithms, private IDE implementation
details, or internal validation checklists. Those belong in private context and
runtime repositories.
</p>

<p>
Conversely, private runtime or IDE behavior must not silently redefine the
public widget class law. If a reusable behavior becomes part of the public
standard, the relevant widget document should be updated deliberately.
</p>
