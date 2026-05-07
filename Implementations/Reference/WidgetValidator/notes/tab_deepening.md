<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Tab Widget Deepening Notes</h1>

<p align="center">
  <strong>Current qualitative deepening pass for the FROG Tab widget family</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>What Changed</h2>

<p>
The Tab family now has a fuller public surface for page navigation and page hosting:
</p>

<ul>
  <li>structured <code>frog.tab_selection</code> value model,</li>
  <li>stable page identity,</li>
  <li>selected page posture,</li>
  <li>tab header layout posture,</li>
  <li>page region and page container surfaces,</li>
  <li>overflow and tab scrolling surfaces,</li>
  <li>optional close and reorder posture,</li>
  <li>tab item icon / label / close-button parts,</li>
  <li>selected-page container and content parts.</li>
</ul>

<hr/>

<h2>Boundary</h2>

<p>
Tab remains a page navigation and container widget family.
It does not own child-widget values, hidden page lifecycle internals, host-native tab handles, or IDE-private page editors.
</p>
