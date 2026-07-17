<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Interface Map Specification</h1>

<p align="center">
Source-owned layout and Front Panel value bindings for public FROG ports
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#ownership">2. Ownership Boundaries</a></li>
  <li><a href="#source-shape">3. Canonical Source Shape</a></li>
  <li><a href="#patterns">4. Layout Patterns and Slots</a></li>
  <li><a href="#widget-bindings">5. Widget Bindings</a></li>
  <li><a href="#direction-and-types">6. Direction and Type Rules</a></li>
  <li><a href="#authoring-lifecycle">7. Authoring Lifecycle</a></li>
  <li><a href="#pattern-changes">8. Pattern Changes and Migration</a></li>
  <li><a href="#array-composition">9. Array Composition</a></li>
  <li><a href="#connection-policy">10. Connection Policy</a></li>
  <li><a href="#validation">11. Validation Rules</a></li>
  <li><a href="#example">12. Complete Example</a></li>
  <li><a href="#summary">13. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
The <strong>Interface Map</strong> is the source-owned visual map that relates
public FROG interface ports to value-carrying Front Panel widgets.
It records which layout pattern is active, how that pattern is transformed,
and which widget value occupies each map slot.
</p>

<p>
The Interface Map is an authoring and public-boundary projection. It does not
create hidden execution semantics. The Diagram remains the authority for
executable dataflow, while <code>interface.inputs[]</code> and
<code>interface.outputs[]</code> remain the authority for the public logical
contract.
</p>

<p>
An editor MAY offer a compact graphical navigator for this model. The visible
state of that navigator is not authoritative unless the corresponding
relationship is serialized in the <code>.frog</code> source described here.
</p>

<hr/>

<h2 id="ownership">2. Ownership Boundaries</h2>

<ul>
  <li><strong><code>interface.inputs[]</code> and <code>interface.outputs[]</code></strong> own public port identity, direction, type, and connection policy.</li>
  <li><strong><code>interface.map</code></strong> owns the selected layout pattern and its visual transforms.</li>
  <li><strong>Front Panel widget bindings</strong> own the explicit relationship between one widget primary value, one public port, and one Interface Map slot.</li>
  <li><strong>The Diagram</strong> owns executable dataflow and public boundary participation.</li>
  <li><strong>The connector projection</strong>, when emitted for reusable-node presentation, MUST remain consistent with the public interface and MUST NOT invent ports.</li>
  <li><strong>IDE transient state</strong>, including hovered, armed, pressed, or swap-preview slots, is not source state.</li>
</ul>

<p>
A Front Panel widget does not become public merely because it is visible. A
public port is created or selected explicitly, and the widget value is then
bound to that port through source-owned data.
</p>

<hr/>

<h2 id="source-shape">3. Canonical Source Shape</h2>

<p>
The selected Interface Map belongs inside the top-level
<code>interface</code> object:
</p>

<pre><code>"interface": {
  "inputs": [],
  "outputs": [],
  "map": {
    "layout_pattern_id": "pattern_33",
    "visual_transforms": []
  }
}</code></pre>

<h3>3.1 <code>layout_pattern_id</code></h3>

<p>
The <code>layout_pattern_id</code> field identifies the selected Interface
Layout Pattern. It MUST resolve in the active pattern catalog or profile.
Pattern identifiers are opaque stable identifiers; consumers MUST NOT derive
geometry from the numeric-looking suffix of an identifier.
</p>

<h3>3.2 <code>visual_transforms</code></h3>

<p>
The optional <code>visual_transforms</code> array records transforms in the
order in which they are applied. Standard transform tokens are:
</p>

<ul>
  <li><code>rotate_90_clockwise</code></li>
  <li><code>flip_horizontal</code></li>
  <li><code>flip_vertical</code></li>
</ul>

<p>
Transforms change slot geometry, not slot identity. A binding attached to
<code>zone_4</code> remains attached to <code>zone_4</code> while that zone
moves with the transformed pattern.
</p>

<hr/>

<h2 id="patterns">4. Layout Patterns and Slots</h2>

<p>
An Interface Layout Pattern defines a finite ordered set of rectangular slots.
Each slot has:
</p>

<ul>
  <li>a stable slot identifier such as <code>zone_1</code>,</li>
  <li>normalized or grid-relative rectangular geometry,</li>
  <li>a deterministic order used for binding migration.</li>
</ul>

<p>
Slot geometry is authoritative for hit testing and display. Editors MUST use
the actual transformed rectangle of the slot; they MUST NOT substitute an
approximate selection band or stale geometry from a previous pattern.
</p>

<p>
The exact pattern catalog is profile- or tool-distributed. A <code>.frog</code>
document stores the selected stable pattern id and transforms, not a hidden
editor index.
</p>

<hr/>

<h2 id="widget-bindings">5. Widget Bindings</h2>

<p>
A value-carrying Front Panel widget MAY contain a <code>binding</code> object:
</p>

<pre><code>"binding": {
  "mode": "widget_value",
  "public_input_id": "gain",
  "interface_map_slot": "zone_4",
  "connection_requirement": "recommended"
}</code></pre>

<p>
An indicator uses <code>public_output_id</code> instead of
<code>public_input_id</code>.
</p>

<h3>5.1 Required fields</h3>

<ul>
  <li><code>mode</code> MUST be <code>widget_value</code> for primary-value participation.</li>
  <li>Exactly one of <code>public_input_id</code> or <code>public_output_id</code> MUST be present.</li>
  <li><code>interface_map_slot</code> MUST identify a slot in the selected and transformed Interface Layout Pattern.</li>
</ul>

<h3>5.2 Public-port origin</h3>

<p>
When a public port is created from an explicit Interface Map authoring gesture,
the port SHOULD preserve its source relationship:
</p>

<pre><code>{
  "id": "gain",
  "type": "f64",
  "binding_origin": "widget.numeric_control.value"
}</code></pre>

<p>
The origin is correspondence metadata. It does not make the widget the owner
of the public interface and does not replace Diagram boundary semantics.
</p>

<hr/>

<h2 id="direction-and-types">6. Direction and Type Rules</h2>

<ul>
  <li>A control widget primary value binds to a public input.</li>
  <li>An indicator widget primary value binds to a public output.</li>
  <li>A label, decoration, empty Array container, or other object without a resolved primary value type MUST NOT be bound.</li>
  <li>The public port type MUST match the widget primary value type.</li>
  <li>Changing a numeric representation MUST update the public port type, Diagram terminal projection, and binding color immediately.</li>
  <li>Changing control/indicator role MUST update direction and terminal projection atomically.</li>
</ul>

<p>
Color is a type visualization, not connection policy. Required, recommended,
and optional semantics remain explicit source tokens even when an editor adds
a separate required-edge treatment.
</p>

<hr/>

<h2 id="authoring-lifecycle">7. Authoring Lifecycle</h2>

<p>
Editors implementing direct Interface Map binding SHOULD follow this lifecycle:
</p>

<ol>
  <li>Selecting an empty slot arms that slot for one binding action.</li>
  <li>While armed, eligible widgets receive an explicit binding-target preview.</li>
  <li>Selecting one eligible widget creates or updates the public port and widget binding, then exits binding mode.</li>
  <li><code>Escape</code> cancels the armed state without changing existing bindings.</li>
  <li>Selecting an already bound slot selects its widget and MAY arm the slot for replacement.</li>
  <li>Rebinding a widget that was already bound clears its old slot before assigning the new slot.</li>
  <li>Deleting a bound widget removes its public value binding and clears the slot.</li>
</ol>

<p>
At most one widget may occupy one Interface Map slot, and one widget primary
value may occupy at most one Interface Map slot in this base model.
</p>

<h3>7.1 Swap and move</h3>

<p>
An editor MAY provide a modified-click swap operation. Swapping two occupied
slots exchanges their widget bindings. Swapping an occupied slot with an empty
slot moves the binding. The operation MUST be atomic: source must never expose
an intermediate state with duplicated ownership or a silently lost port.
</p>

<h3>7.2 Disconnect</h3>

<p>
Disconnecting one terminal clears the corresponding widget binding and slot.
Disconnecting all terminals clears every Interface Map widget binding. These
operations do not delete unrelated widgets.
</p>

<hr/>

<h2 id="pattern-changes">8. Pattern Changes and Migration</h2>

<p>
Changing a pattern, adding a terminal, or removing a terminal MUST preserve as
many existing bindings as the target pattern can represent.
</p>

<ol>
  <li>Collect existing bindings in deterministic source-pattern slot order.</li>
  <li>Order target slots deterministically by geometry and stable slot id.</li>
  <li>Assign existing bindings to target slots in that order.</li>
  <li>If the target has fewer slots, remove overflow bindings explicitly.</li>
</ol>

<p>
An editor MUST report removed overflow bindings; it MUST NOT leave public ports
or widget bindings pointing to unavailable slots.
</p>

<p>
Rotation and reflection are different from selecting another pattern. They
preserve slot identity and move the bound slot with its transformed geometry.
</p>

<hr/>

<h2 id="array-composition">9. Array Composition</h2>

<p>
An empty Array container has no resolved element type and therefore cannot be
bound. When a bound scalar widget is encapsulated by an Array:
</p>

<ul>
  <li>the Array inherits the scalar widget value direction, element type, public port, Interface Map slot, and connection requirement,</li>
  <li>the contained element template no longer carries a duplicate public binding,</li>
  <li>the public port type becomes the corresponding Array type,</li>
  <li>the Diagram terminal and Interface Map type color update immediately.</li>
</ul>

<p>
Removing the contained widget returns the Array to an untyped and unbindable
state and clears the now-invalid public binding.
</p>

<hr/>

<h2 id="connection-policy">10. Connection Policy</h2>

<p>
The accepted values are <code>required</code>, <code>recommended</code>, and
<code>optional</code>. Their semantics are defined by
<code>Interface.md</code>.
</p>

<p>
An editor MAY visualize a required terminal with a stronger external-facing
edge. At a corner, only the dataflow-facing left or right edge SHOULD receive
that treatment unless the slot exists exclusively on the top or bottom edge.
This is an editor visualization rule and does not replace the serialized
connection requirement.
</p>

<hr/>

<h2 id="validation">11. Validation Rules</h2>

<p>A conforming validator MUST reject:</p>

<ul>
  <li>an unknown <code>layout_pattern_id</code> under the active pattern profile,</li>
  <li>an unsupported visual transform token when strict validation is requested,</li>
  <li>a binding that references an unavailable transformed slot,</li>
  <li>two widgets bound to the same slot,</li>
  <li>one widget value bound to multiple slots,</li>
  <li>a control bound through <code>public_output_id</code>,</li>
  <li>an indicator bound through <code>public_input_id</code>,</li>
  <li>a missing or type-incompatible public port,</li>
  <li>a binding on an object without a resolved primary value type,</li>
  <li>a stale scalar binding left on an element template after Array encapsulation.</li>
</ul>

<p>
Unknown non-semantic editor extensions MAY be ignored, but they MUST NOT be
used to repair a missing canonical binding silently.
</p>

<hr/>

<h2 id="example">12. Complete Example</h2>

<pre><code>{
  "interface": {
    "inputs": [
      {
        "id": "numeric_control",
        "type": "f64",
        "connection": "recommended",
        "binding_origin": "widget.numeric_control.value"
      }
    ],
    "outputs": [],
    "map": {
      "layout_pattern_id": "pattern_33",
      "visual_transforms": ["rotate_90_clockwise"]
    }
  },
  "front_panel": {
    "widgets": [
      {
        "id": "numeric_control",
        "role": "control",
        "widget": "frog.widgets.numeric_control",
        "value_type": "f64",
        "binding": {
          "mode": "widget_value",
          "public_input_id": "numeric_control",
          "interface_map_slot": "zone_4",
          "connection_requirement": "recommended"
        }
      }
    ]
  }
}</code></pre>

<hr/>

<h2 id="summary">13. Summary</h2>

<ul>
  <li>The public contract remains in <code>interface.inputs[]</code> and <code>interface.outputs[]</code>.</li>
  <li>The selected visual pattern remains in <code>interface.map</code>.</li>
  <li>Widget-to-port-to-slot correspondence remains explicit in widget source.</li>
  <li>Transforms move slot geometry without changing slot identity.</li>
  <li>Pattern changes migrate bindings deterministically and remove only overflow.</li>
  <li>Type, role, Array composition, Diagram terminals, and binding visualization remain synchronized.</li>
  <li>No editor-only state may substitute for the serialized relationship.</li>
</ul>
