<p align="center">
  <img src="../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Default Realization Package</h1>

<p align="center">
  <strong>Machine-readable publication posture for the official <code>Default</code> widget realization family</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Navigation</h2>

<ul>
  <li><a href="../Readme.md">Realizations index</a></li>
  <li><a href="../../Widgets/Readme.md">Standard widgets</a></li>
  <li><a href="../../Widgets/Chart.md">Chart widget class law</a></li>
  <li><a href="./Chart.md">Default chart realization</a></li>
  <li><a href="./chart.default.wfrog">Chart realization manifest</a></li>
</ul>

<hr/>

<h2>Chart realization note</h2>

<p>
The chart realization uses a subobject composition posture.
The top-level shell defines slots, and separate SVG assets define subobjects such as plot area, grid, scales, legends, graph palette, scrollbars, cursor layer, and plot-image layers.
</p>

<p>
This is the correct posture for composite widgets whose subobjects have independent public properties and runtime behavior.
</p>
