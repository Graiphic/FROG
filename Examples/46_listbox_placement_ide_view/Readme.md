<h1>Example 46 - Listbox Placement IDE View</h1>

<p>
Reviewed 2026-06-19: this example validates the Default Listbox IDE placement
posture in the C++ private browser host.
</p>

<p>
The Listbox exposes <code>placement_bounds</code> as its selectable placement
aura and <code>main_body</code> / <code>item_region</code> as the visible list
body. The default aura band is <code>4px</code> on every side. The visible grid
is only an IDE/review aid; the same source-owned widget layout remains valid
when the grid is hidden.
</p>

<pre><code>.frog source
  -&gt; FIR
  -&gt; lowering
  -&gt; LLVM native Listbox u16 kernel manifest
  -&gt; C++ private runtime
  -&gt; .wfrog front panel binding
  -&gt; Default Listbox realization SVG asset
</code></pre>

<p>
This example consumes <code>ui/listbox_ide_panel.wfrog</code>, which references
the canonical Default Listbox realization package. The Default Listbox SVG
public parts include <code>root</code>, <code>label</code>,
<code>caption</code>, <code>placement_bounds</code>, <code>main_body</code>,
<code>item_region</code>, <code>item_row</code>, <code>item_icon</code>,
<code>item_label</code>, <code>selection_face</code>, <code>hover_face</code>,
<code>disabled_face</code>, <code>vertical_scrollbar</code>,
<code>horizontal_scrollbar</code>, and <code>focus_ring</code>.
</p>
