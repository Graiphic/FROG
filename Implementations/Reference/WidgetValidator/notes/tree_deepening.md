<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Tree Widget Deepening Notes</h1>

<p align="center">
  <strong>Current qualitative deepening pass for the FROG Tree widget family</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>What Changed</h2>

<p>
The Tree family now has a fuller public surface for hierarchical navigation and structured selection:
</p>

<ul>
  <li>structured <code>frog.tree</code> value model,</li>
  <li>node id and node path posture,</li>
  <li>selection and active node posture,</li>
  <li>expansion state,</li>
  <li>optional checkbox / tri-state posture,</li>
  <li>viewport, search, and filter posture,</li>
  <li>editing posture,</li>
  <li>optional drag/drop posture,</li>
  <li>node row, expander, connector, icon, label, selection, active-node, edit, drag, and drop parts.</li>
</ul>

<hr/>

<h2>Boundary</h2>

<p>
Tree remains a finite hierarchical widget family.
Filesystem APIs, database browsers, object reflection, scene graphs, and IDE-private navigators remain downstream integrations unless explicitly standardized elsewhere.
</p>
