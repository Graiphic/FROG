<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">FROG Standard Cluster Widget</h1>

<p align="center">
  <strong>Composite-object baseline for the standardized heterogeneous cluster widget class</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="./Readme.md">Widgets index</a></li>
  <li><a href="../../Expression/Widget.md">Expression widget instances</a></li>
  <li><a href="../../Expression/Widget%20interaction.md">Expression widget interaction</a></li>
  <li><a href="../../Expression/Widget%20realization.md">Expression widget realization</a></li>
  <li><a href="../../Expression/Widget%20package%20(.wfrog).md">Widget package publication</a></li>
  <li><a href="../../Libraries/UI.md">Executable UI primitives</a></li>
  <li><a href="../../Libraries/Realizations/Default/Cluster.md">Default cluster realization</a></li>
  <li><a href="../../Libraries/Realizations/Default/cluster.default.wfrog">Default cluster realization manifest</a></li>
</ul>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#class-defined-here">2. Class Defined Here</a></li>
  <li><a href="#array-versus-cluster">3. Array versus Cluster</a></li>
  <li><a href="#cluster-class-versus-realization">4. Cluster Class versus Realization</a></li>
  <li><a href="#field-model">5. Field Model</a></li>
  <li><a href="#value-and-schema-model">6. Value and Schema Model</a></li>
  <li><a href="#label-caption-and-view-surfaces">7. Label, Caption, and View Surfaces</a></li>
  <li><a href="#public-part-model">8. Public Part Model</a></li>
  <li><a href="#standard-property-surface">9. Standard Property Surface</a></li>
  <li><a href="#standard-methods">10. Standard Methods</a></li>
  <li><a href="#standard-events">11. Standard Events</a></li>
  <li><a href="#default-realization-posture">12. Default Realization Posture</a></li>
  <li><a href="#diagram-interaction-posture">13. Diagram Interaction Posture</a></li>
  <li><a href="#validation-expectations">14. Validation Expectations</a></li>
  <li><a href="#summary">15. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines <code>frog.widgets.cluster</code>, the standardized heterogeneous aggregate widget class of FROG.
</p>

<p>
A cluster is a composite structured widget that groups a fixed set of named fields.
Each field may use a different value type and a different widget class.
This makes the cluster the heterogeneous counterpart of the array.
</p>

<p>
The cluster is therefore not a decorative group box and not merely a layout container.
It is a structured value widget whose aggregate value is derived from its declared field set.
</p>

<hr/>

<h2 id="class-defined-here">2. Class Defined Here</h2>

<ul>
  <li><code>frog.widgets.cluster</code></li>
</ul>

<hr/>

<h2 id="array-versus-cluster">3. Array versus Cluster</h2>

<pre><code>array
    - homogeneous collection
    - repeated element model
    - indexed by position

cluster
    - heterogeneous aggregate
    - fixed named field model
    - addressed by field name or field id
</code></pre>

<p>
An array of numerics repeats the same numeric element.
A cluster may contain one numeric, one boolean, one string, one enum, and one path field in the same aggregate.
</p>

<hr/>

<h2 id="cluster-class-versus-realization">4. Cluster Class versus Realization</h2>

<p>
The cluster class owns:
</p>

<ul>
  <li>aggregate value semantics,</li>
  <li>field inventory and field identity,</li>
  <li>field value type bindings,</li>
  <li>field widget class bindings,</li>
  <li>cluster-level property, method, and event surfaces,</li>
  <li>the public composite part model.</li>
</ul>

<p>
The realization owns:
</p>

<ul>
  <li>field placement,</li>
  <li>content-region drawing,</li>
  <li>field-slot layout,</li>
  <li>frame and caption embodiment,</li>
  <li>spacing and grouping visuals,</li>
  <li>SVG, host-native, or mixed rendering.</li>
</ul>

<p>
The child widget class owns the behavior of an individual field.
The cluster owns the named aggregate structure around those fields.
</p>

<hr/>

<h2 id="field-model">5. Field Model</h2>

<p>
A cluster has a fixed declared field inventory.
</p>

<ul>
  <li><code>fields.count</code></li>
  <li><code>fields[].id</code> — stable field identity.</li>
  <li><code>fields[].name</code> — human-readable field name.</li>
  <li><code>fields[].value_type</code> — semantic value type of the field.</li>
  <li><code>fields[].widget_class_id</code> — optional widget class used to realize the field.</li>
  <li><code>fields[].role</code> — <code>control</code>, <code>indicator</code>, or <code>support</code>.</li>
  <li><code>fields[].default_value</code></li>
  <li><code>fields[].visible</code></li>
  <li><code>fields[].enabled</code></li>
</ul>

<p>
Field order may be meaningful for layout and serialization.
Field identity should remain stable across compatible edits.
</p>

<hr/>

<h2 id="value-and-schema-model">6. Value and Schema Model</h2>

<ul>
  <li><code>value</code> — aggregate value keyed by field identity or field name.</li>
  <li><code>schema.fields[]</code> — field declarations.</li>
  <li><code>schema.fixed : bool</code> — whether runtime schema mutation is forbidden.</li>
  <li><code>schema.order[]</code> — declared field order.</li>
  <li><code>schema.allow_missing_fields : bool</code> — compatibility posture when reading older data.</li>
  <li><code>schema.allow_extra_fields : bool</code> — compatibility posture when reading newer data.</li>
</ul>

<p>
The intrinsic baseline assumes a fixed field inventory.
Runtime mutation of the field schema is outside the core baseline.
</p>

<hr/>

<h2 id="label-caption-and-view-surfaces">7. Label, Caption, and View Surfaces</h2>

<p>
The cluster follows the shared FROG label/caption convention.
</p>

<pre><code>label.*
    - structural / logical widget name surface

caption.*
    - user-facing front-panel title surface

content_region
    - visual child-field region

field_slot
    - realization slot for one declared field
</code></pre>

<hr/>

<h2 id="public-part-model">8. Public Part Model</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>frame</code></li>
  <li><code>content_region</code></li>
  <li><code>field_slot</code></li>
  <li><code>field_label</code></li>
  <li><code>field_gap</code></li>
  <li><code>focus_ring</code></li>
</ul>

<p>
The cluster is a composite widget.
Its Default realization may use a shell plus separate subobject assets for the content region, field slots, and field labels.
</p>

<hr/>

<h2 id="standard-property-surface">9. Standard Property Surface</h2>

<h3>9.1 Aggregate value and schema</h3>

<ul>
  <li><code>value</code></li>
  <li><code>fields.count</code></li>
  <li><code>fields[]</code></li>
  <li><code>fields[].id</code></li>
  <li><code>fields[].name</code></li>
  <li><code>fields[].value_type</code></li>
  <li><code>fields[].widget_class_id</code></li>
  <li><code>fields[].role</code></li>
  <li><code>fields[].default_value</code></li>
  <li><code>schema.fixed</code></li>
  <li><code>schema.order[]</code></li>
  <li><code>schema.allow_missing_fields</code></li>
  <li><code>schema.allow_extra_fields</code></li>
</ul>

<h3>9.2 Field state</h3>

<ul>
  <li><code>fields[].visible</code></li>
  <li><code>fields[].enabled</code></li>
  <li><code>fields[].read_only</code></li>
  <li><code>fields[].caption.text</code></li>
  <li><code>fields[].caption.visible</code></li>
</ul>

<h3>9.3 Layout and view</h3>

<ul>
  <li><code>layout.orientation</code> — <code>vertical</code>, <code>horizontal</code>, or <code>grid</code>.</li>
  <li><code>layout.row_gap</code></li>
  <li><code>layout.column_gap</code></li>
  <li><code>layout.padding</code></li>
  <li><code>layout.field_label_position</code></li>
  <li><code>content_region.scroll_enabled</code></li>
  <li><code>content_region.clip_overflow</code></li>
</ul>

<h3>9.4 Common widget surfaces</h3>

<ul>
  <li><code>label.visible</code></li>
  <li><code>label.text</code></li>
  <li><code>caption.visible</code></li>
  <li><code>caption.text</code></li>
  <li><code>interaction.visible</code></li>
  <li><code>interaction.enabled</code></li>
  <li><code>interaction.read_only</code></li>
  <li><code>interaction.focused</code></li>
  <li><code>style.frame.*</code></li>
  <li><code>style.content_region.*</code></li>
  <li><code>style.field_label.*</code></li>
  <li><code>style.focus_ring.*</code></li>
  <li><code>realization.family</code></li>
  <li><code>realization.variant</code></li>
  <li><code>realization.skin_id</code></li>
</ul>

<hr/>

<h2 id="standard-methods">10. Standard Methods</h2>

<ul>
  <li><code>get_field(id)</code></li>
  <li><code>set_field(id, value)</code></li>
  <li><code>focus_field(id)</code></li>
  <li><code>focus_first_child()</code></li>
  <li><code>reset_field_to_default(id)</code></li>
  <li><code>reset_to_default()</code></li>
  <li><code>set_field_visible(id, visible)</code></li>
  <li><code>set_field_enabled(id, enabled)</code></li>
</ul>

<hr/>

<h2 id="standard-events">11. Standard Events</h2>

<ul>
  <li><code>value_changed</code></li>
  <li><code>field_value_changed</code></li>
  <li><code>field_focus_gained</code></li>
  <li><code>field_focus_lost</code></li>
  <li><code>field_visibility_changed</code></li>
  <li><code>field_enabled_changed</code></li>
  <li><code>focus_gained</code></li>
  <li><code>focus_lost</code></li>
</ul>

<hr/>

<h2 id="default-realization-posture">12. Default Realization Posture</h2>

<p>
The Default realization should expose a cluster shell with separate subobject assets for content region, field slot, field label, and optional frame.
</p>

<p>
The shell owns composition slots.
The content region owns field placement.
Each field widget class owns the field's individual behavior.
</p>

<hr/>

<h2 id="diagram-interaction-posture">13. Diagram Interaction Posture</h2>

<p>
The cluster supports natural aggregate value participation through <code>widget_value</code>, property access through <code>frog.ui.property_read</code> and <code>frog.ui.property_write</code>, method invocation through <code>frog.ui.method_invoke</code>, event observation where legal, and widget reference targeting through <code>widget_reference</code>.
</p>

<hr/>

<h2 id="validation-expectations">14. Validation Expectations</h2>

<p>
Validators SHOULD diagnose aggregate-value shape mismatch, unknown field ids, duplicate field ids, invalid field order, missing required fields, unsupported extra fields, field value-type mismatch, unsupported field widget class, and attempts to treat realization-only field slots as semantic field storage.
</p>

<hr/>

<h2 id="summary">15. Summary</h2>

<p>
<code>frog.widgets.cluster</code> is the standardized heterogeneous aggregate widget of FROG.
It groups named fields of potentially different value types and widget classes into one structured value.
</p>
