<h1 align="center">FROG Waveform Library Specification</h1>

<p align="center">
Definition of the standard <strong>frog.waveform</strong> value and primitive surface for FROG v0.1<br/>
<em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#boundary">2. Boundary</a></li>
  <li><a href="#value-shape">3. Value Shape</a></li>
  <li><a href="#published-surface">4. Published Surface</a></li>
  <li><a href="#primitive-contracts">5. Primitive Contracts</a></li>
  <li><a href="#effects-status-and-fir">6. Effects, Status, and FIR</a></li>
  <li><a href="#candidate-deferrals">7. Candidate Deferrals</a></li>
  <li><a href="#non-goals">8. Non-goals</a></li>
  <li><a href="#summary">9. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
<code>frog.waveform</code> defines a portable sampled-data value contract and
the first value-only primitives around that contract. A waveform combines a
sample array with timing metadata. It is not a chart widget, an acquisition
session, a streaming buffer, or a runtime-owned history store.
</p>

<p>
This library exists so future graph/chart examples can display waveform data
without making the widget responsible for constructing, timing, or transforming
the data. Computation remains visible as diagram primitives.
</p>

<hr/>

<h2 id="boundary">2. Boundary</h2>

<pre><code>frog.signal
  transforms sample arrays

frog.waveform
  constructs and edits t0/dt/sample waveform values

graph/chart widgets
  display waveform values and expose widget interaction

runtime/acquisition profiles
  own device, stream, and scheduler capabilities</code></pre>

<p>
The standard waveform library is value-only in v0.1. Anything that talks to
hardware, maintains live chart history, reads from a device clock, or buffers a
stream belongs to a later runtime-hosted or profile-owned surface.
</p>

<hr/>

<h2 id="value-shape">3. Value Shape</h2>

<p>
The minimal waveform value shape is:
</p>

<pre><code>{
  "kind": "frog.waveform",
  "sample_type": "f32" | "f64",
  "t0": f64,
  "dt": f64,
  "samples": array&lt;f32&gt; | array&lt;f64&gt;
}</code></pre>

<p>
Field meaning:
</p>

<ul>
  <li><code>t0</code> is the coordinate of the first sample in seconds in the waveform's own time coordinate.</li>
  <li><code>dt</code> is the positive sample interval in seconds.</li>
  <li><code>samples</code> is an ordered one-dimensional numeric sample array.</li>
</ul>

<p>
The v0.1 value shape intentionally does not include first-class attributes.
Attributes, units, channel names, acquisition device identity, absolute
wall-clock timestamps, and richer metadata remain deferred until the type,
record/cluster, and time libraries have a settled public posture.
</p>

<p>
This value contract is a library-owned value shape. It does not make
user-defined records, structs, or arbitrary named aggregate types part of the
base source type system.
</p>

<hr/>

<h2 id="published-surface">4. Published Surface</h2>

<table>
  <thead>
    <tr>
      <th>Primitive</th>
      <th>Inputs</th>
      <th>Outputs</th>
      <th>Role</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>frog.waveform.build</code></td>
      <td><code>t0</code>, <code>dt</code>, <code>samples</code></td>
      <td><code>waveform</code></td>
      <td>Create a waveform value.</td>
    </tr>
    <tr>
      <td><code>frog.waveform.unbundle</code></td>
      <td><code>waveform</code></td>
      <td><code>t0</code>, <code>dt</code>, <code>samples</code></td>
      <td>Expose waveform fields as values.</td>
    </tr>
    <tr>
      <td><code>frog.waveform.samples</code></td>
      <td><code>waveform</code></td>
      <td><code>samples</code></td>
      <td>Return the sample array.</td>
    </tr>
    <tr>
      <td><code>frog.waveform.t0</code></td>
      <td><code>waveform</code></td>
      <td><code>t0</code></td>
      <td>Return the first-sample coordinate.</td>
    </tr>
    <tr>
      <td><code>frog.waveform.dt</code></td>
      <td><code>waveform</code></td>
      <td><code>dt</code></td>
      <td>Return the sample interval.</td>
    </tr>
    <tr>
      <td><code>frog.waveform.sample_count</code></td>
      <td><code>waveform</code></td>
      <td><code>count</code></td>
      <td>Return the number of samples.</td>
    </tr>
    <tr>
      <td><code>frog.waveform.with_samples</code></td>
      <td><code>waveform</code>, <code>samples</code></td>
      <td><code>result</code></td>
      <td>Return a waveform with replacement samples.</td>
    </tr>
    <tr>
      <td><code>frog.waveform.with_timing</code></td>
      <td><code>waveform</code>, <code>t0</code>, <code>dt</code></td>
      <td><code>result</code></td>
      <td>Return a waveform with replacement timing.</td>
    </tr>
    <tr>
      <td><code>frog.waveform.append</code></td>
      <td><code>a</code>, <code>b</code></td>
      <td><code>result</code></td>
      <td>Append two compatible contiguous waveforms.</td>
    </tr>
    <tr>
      <td><code>frog.waveform.time_at_index</code></td>
      <td><code>waveform</code>, <code>index</code></td>
      <td><code>time</code></td>
      <td>Return <code>t0 + index * dt</code>.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2 id="primitive-contracts">5. Primitive Contracts</h2>

<h3><code>frog.waveform.build</code></h3>

<ul>
  <li><code>t0</code>: <code>f64</code></li>
  <li><code>dt</code>: <code>f64</code>, strictly greater than zero</li>
  <li><code>samples</code>: <code>array&lt;f32&gt;</code> or <code>array&lt;f64&gt;</code></li>
  <li><code>waveform</code>: <code>frog.waveform</code></li>
</ul>

<h3><code>frog.waveform.unbundle</code></h3>

<ul>
  <li>Returns the <code>t0</code>, <code>dt</code>, and <code>samples</code> fields without mutation.</li>
  <li>The sample output type matches the waveform sample type.</li>
</ul>

<h3>Accessors</h3>

<ul>
  <li><code>frog.waveform.samples</code> returns the sample array.</li>
  <li><code>frog.waveform.t0</code> returns <code>f64</code>.</li>
  <li><code>frog.waveform.dt</code> returns <code>f64</code>.</li>
  <li><code>frog.waveform.sample_count</code> returns <code>u64</code>.</li>
</ul>

<h3><code>frog.waveform.with_samples</code></h3>

<ul>
  <li>The replacement samples MUST have the same floating-point element type as the input waveform.</li>
  <li>The result preserves <code>t0</code> and <code>dt</code>.</li>
  <li>The result sample count is the replacement sample count.</li>
</ul>

<h3><code>frog.waveform.with_timing</code></h3>

<ul>
  <li><code>dt</code> MUST be strictly greater than zero.</li>
  <li>The result preserves sample type and sample values.</li>
  <li>The result uses the supplied <code>t0</code> and <code>dt</code>.</li>
</ul>

<h3><code>frog.waveform.append</code></h3>

<ul>
  <li><code>a</code> and <code>b</code> MUST have the same sample type.</li>
  <li><code>a.dt</code> and <code>b.dt</code> MUST be equal under the active floating-point equality rules.</li>
  <li><code>b.t0</code> MUST equal <code>a.t0 + sample_count(a) * a.dt</code> under the active floating-point equality rules.</li>
  <li>The result has <code>t0 = a.t0</code>, <code>dt = a.dt</code>, and concatenated samples.</li>
</ul>

<h3><code>frog.waveform.time_at_index</code></h3>

<ul>
  <li><code>index</code> has type <code>u64</code>.</li>
  <li><code>index</code> MUST be less than <code>sample_count(waveform)</code>.</li>
  <li><code>time</code> has type <code>f64</code>.</li>
</ul>

<hr/>

<h2 id="effects-status-and-fir">6. Effects, Status, and FIR</h2>

<p>
Every published <code>frog.waveform</code> primitive has:
</p>

<ul>
  <li><code>call_class = standard_library_value</code></li>
  <li><code>effect = pure</code></li>
  <li><code>status_model = none</code></li>
  <li>no provider requirement</li>
</ul>

<p>
The library uses validation preconditions rather than local status outputs.
Invalid <code>dt</code>, incompatible sample types, non-contiguous append
inputs, and out-of-range indices are validation or execution-profile failures
until the uniform status corridor is standardized.
</p>

<p>
FIR SHOULD preserve waveform calls as named public library calls with
<code>library_id = "frog.waveform"</code>. A FIR value may carry the waveform
fields as an explicit library value object or support object, but it MUST NOT
replace the public waveform contract with private runtime storage, chart
history, acquisition buffers, device handles, or backend-specific layouts.
</p>

<hr/>

<h2 id="candidate-deferrals">7. Candidate Deferrals</h2>

<ul>
  <li>Absolute timestamps and timezone-aware time values, pending <code>frog.time</code>.</li>
  <li>Attributes, units, channel names, and rich metadata, pending the record/cluster value model.</li>
  <li>Multi-channel waveform containers.</li>
  <li>Waveform generation helpers such as sine, square, and ramp generation.</li>
  <li>Waveform resampling that updates <code>dt</code> and status behavior explicitly.</li>
  <li>Acquisition, streaming, chart history, device clock, trigger, and buffer-management surfaces.</li>
</ul>

<hr/>

<h2 id="non-goals">8. Non-goals</h2>

<ul>
  <li>Do not make chart widgets the owner of waveform construction or timing semantics.</li>
  <li>Do not imply compatibility with any external waveform file format or binary API.</li>
  <li>Do not expose runtime handles, device sessions, or streaming buffers as waveform values.</li>
  <li>Do not standardize arbitrary records or metadata maps through this library.</li>
</ul>

<hr/>

<h2 id="summary">9. Summary</h2>

<p>
<code>frog.waveform</code> publishes a small value-only surface for waveform
values with <code>t0</code>, <code>dt</code>, and numeric samples. It is the
timing-aware companion to <code>frog.signal</code>, while acquisition,
streaming, chart history, and rich metadata remain outside v0.1.
</p>

<hr/>

<p align="center">
End of FROG Waveform Library Specification
</p>
