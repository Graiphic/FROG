<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example 02 — UI Value Roundtrip</h1>

<p align="center">
  <strong>Minimal front-panel value participation example using widget_value nodes</strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Published Files</h2>

<pre><code>Examples/02_ui_value_roundtrip/
├── Readme.md
├── main.frog
├── main.fir.json
└── main.lowering.json
</code></pre>

<hr/>

<h2>Source Shape</h2>

<pre><code>widget_value(ctrl_a) ---\
                         &gt; frog.core.add ---&gt; widget_value(ind_result)
widget_value(ctrl_b) ---/
</code></pre>

<hr/>

<h2>Reference Checks</h2>

<pre><code>python Implementations/Reference/Deriver/derive_fir.py --source Examples/02_ui_value_roundtrip/main.frog --expected Examples/02_ui_value_roundtrip/main.fir.json --check
python Implementations/Reference/Lowerer/lower_fir.py --fir Examples/02_ui_value_roundtrip/main.fir.json --expected Examples/02_ui_value_roundtrip/main.lowering.json --check
</code></pre>

<hr/>

<h2>Boundary</h2>

<p>This example is a widget-value derivation and lowering slice. It does not currently claim backend contract, runtime acceptance, or LLVM closure.</p>
