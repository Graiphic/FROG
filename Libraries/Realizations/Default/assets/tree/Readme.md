<p align="center">
  <img src="../../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Tree Assets</h1>

<p align="center">
  <strong>SVG assets for the default tree realization family</strong><br/>
  <em>FROG &mdash; Free Open Graphical Language</em>
</p>

<hr/>

<h2>Published Assets</h2>

<ul>
  <li><code>assets/tree/templates/tree_shell.svg</code></li>
  <li><code>assets/tree/item_symbols/check.svg</code></li>
  <li><code>assets/tree/item_symbols/folder_closed.svg</code></li>
  <li><code>assets/tree/item_symbols/folder_open.svg</code></li>
  <li><code>assets/tree/item_symbols/document.svg</code></li>
  <li><code>assets/tree/item_symbols/leaf.svg</code></li>
</ul>

<hr/>

<h2>Static Shell Parts</h2>

<ul>
  <li><code>root</code></li>
  <li><code>label</code></li>
  <li><code>caption</code></li>
  <li><code>node_region</code></li>
  <li><code>vertical_scrollbar</code></li>
  <li><code>horizontal_scrollbar</code></li>
</ul>

<hr/>

<h2>Overlay Contract</h2>

<p>
The Tree shell does not contain hidden rows, hidden backgrounds, hidden buttons, hidden focus rings, or prototype scrollbar artwork.
Dynamic Tree surfaces such as column headers, rows, selection, edit fields, expand/collapse controls, and scrollbar thumbs are host overlays generated from <code>.frog</code> instance data and aligned to the geometry published by this SVG.
Item symbols are separate Default SVG assets resolved through the <code>.wfrog</code> item-symbol registry from source-owned <code>nodes.icon</code> and <code>nodes.collapsed_icon</code> values.
</p>
