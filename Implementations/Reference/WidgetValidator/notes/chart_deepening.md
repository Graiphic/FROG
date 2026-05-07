<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Waveform Chart Widget Deepening Notes</h1>

<p align="center">
  <strong>Current qualitative deepening pass for the FROG Waveform Chart widget family</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>What Changed</h2>

<p>
The Waveform Chart family now has a fuller public surface for instrumentation-style charting:
</p>

<ul>
  <li>structured <code>frog.waveform_chart_value</code> value posture,</li>
  <li>sample payload kinds,</li>
  <li>timebase model,</li>
  <li>history retention model,</li>
  <li>multi-plot posture,</li>
  <li>scale and grid posture,</li>
  <li>cursor and cursor readout posture,</li>
  <li>legend / palette / scrollbar surfaces,</li>
  <li>annotation and threshold overlay surfaces,</li>
  <li>plot-image layer posture.</li>
</ul>

<hr/>

<h2>Boundary</h2>

<p>
Waveform Chart remains the baseline time-ordered numeric chart widget.
XY graphs, intensity graphs, mixed-signal graphs, histograms, advanced annotations, domain-specific plotting engines, and plugin renderers remain downstream profiles or future widget classes.
</p>
