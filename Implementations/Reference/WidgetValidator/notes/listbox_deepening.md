<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Listbox Widget Deepening Notes</h1>

<p align="center">
  <strong>Current qualitative deepening pass for the FROG Listbox widget family</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>What Changed</h2>

<p>
The Listbox family now has a fuller public surface for finite item-list selection:
</p>

<ul>
  <li>structured <code>frog.listbox_selection</code> value model,</li>
  <li>stable item identity,</li>
  <li>single / multiple / range selection posture,</li>
  <li>active item and hover posture,</li>
  <li>item icon and item label surfaces,</li>
  <li>disabled item and empty state surfaces,</li>
  <li>search/filter view posture,</li>
  <li>vertical and horizontal scrolling posture.</li>
</ul>

<hr/>

<h2>Boundary</h2>

<p>
Listbox remains a finite item-list selection widget family.
Tables, trees, combo boxes, file browsers, database views, virtual item engines, and arbitrary item-template systems remain downstream integrations unless explicitly standardized elsewhere.
</p>
