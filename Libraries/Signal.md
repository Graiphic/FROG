<h1 align="center">FROG Signal Library Specification</h1>

<p align="center">
Definition of the standard <strong>frog.signal</strong> primitive library for FROG v0.1<br/>
<em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#boundary">2. Boundary</a></li>
  <li><a href="#value-model">3. Value Model</a></li>
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
This document defines the standard <code>frog.signal</code> primitive library.
The library provides value-only operations over one-dimensional numeric sample
arrays. It is intentionally smaller than a full signal-processing toolkit.
</p>

<p>
The initial surface is designed to be useful for ordinary graphical programs
and for future waveform/table examples without hiding computation inside
widgets. A graph that filters or scales data should show that operation as an
explicit <code>frog.signal.*</code> primitive node.
</p>

<hr/>

<h2 id="boundary">2. Boundary</h2>

<p>
<code>frog.signal</code> owns sample-array signal operations.
It does not own waveform records, chart widgets, hardware acquisition, device
timing, streaming buffers, or private runtime scheduling.
</p>

<pre><code>frog.signal
  owns numeric sample-array transforms

frog.waveform
  owns sampled data plus t0/dt timing metadata

frog.table
  owns portable tabular values

widgets
  display or edit values; they do not hide signal computation</code></pre>

<p>
This split lets a program unbundle a waveform, transform its sample array with
<code>frog.signal</code>, and rebuild the waveform with <code>frog.waveform</code>
while preserving the timing metadata explicitly.
</p>

<hr/>

<h2 id="value-model">3. Value Model</h2>

<p>
In this document, a signal is an ordered one-dimensional array of numeric
samples. The published v0.1 signal primitives accept:
</p>

<ul>
  <li><code>array&lt;f32&gt;</code></li>
  <li><code>array&lt;f64&gt;</code></li>
</ul>

<p>
Sample order is significant. Sample spacing, absolute time, units,
multi-channel membership, and acquisition origin are not part of the
<code>frog.signal</code> value model. Those facts belong in explicit inputs or
in a <code>frog.waveform</code> value.
</p>

<p>
Unless stated otherwise, a primitive that consumes an <code>array&lt;f32&gt;</code>
returns <code>f32</code> scalar results and <code>array&lt;f32&gt;</code> array
results. A primitive that consumes an <code>array&lt;f64&gt;</code> returns
<code>f64</code> scalar results and <code>array&lt;f64&gt;</code> array results.
</p>

<hr/>

<h2 id="published-surface">4. Published Surface</h2>

<p>
The v0.1 published <code>frog.signal</code> surface is:
</p>

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
      <td><code>frog.signal.sample_count</code></td>
      <td><code>signal</code></td>
      <td><code>count</code></td>
      <td>Return the number of samples.</td>
    </tr>
    <tr>
      <td><code>frog.signal.mean</code></td>
      <td><code>signal</code></td>
      <td><code>result</code></td>
      <td>Return arithmetic mean.</td>
    </tr>
    <tr>
      <td><code>frog.signal.rms</code></td>
      <td><code>signal</code></td>
      <td><code>result</code></td>
      <td>Return root-mean-square amplitude.</td>
    </tr>
    <tr>
      <td><code>frog.signal.peak</code></td>
      <td><code>signal</code></td>
      <td><code>value</code>, <code>index</code></td>
      <td>Return maximum value and first index.</td>
    </tr>
    <tr>
      <td><code>frog.signal.peak_to_peak</code></td>
      <td><code>signal</code></td>
      <td><code>result</code></td>
      <td>Return maximum minus minimum.</td>
    </tr>
    <tr>
      <td><code>frog.signal.scale</code></td>
      <td><code>signal</code>, <code>gain</code></td>
      <td><code>result</code></td>
      <td>Multiply every sample by a scalar.</td>
    </tr>
    <tr>
      <td><code>frog.signal.offset</code></td>
      <td><code>signal</code>, <code>bias</code></td>
      <td><code>result</code></td>
      <td>Add a scalar to every sample.</td>
    </tr>
    <tr>
      <td><code>frog.signal.normalize_peak</code></td>
      <td><code>signal</code>, <code>target_peak</code></td>
      <td><code>result</code></td>
      <td>Scale so the maximum absolute sample equals <code>target_peak</code>.</td>
    </tr>
    <tr>
      <td><code>frog.signal.moving_average</code></td>
      <td><code>signal</code>, <code>window_size</code></td>
      <td><code>result</code></td>
      <td>Apply a trailing moving-average filter.</td>
    </tr>
    <tr>
      <td><code>frog.signal.fir</code></td>
      <td><code>signal</code>, <code>coefficients</code></td>
      <td><code>result</code></td>
      <td>Apply a causal finite impulse response filter.</td>
    </tr>
    <tr>
      <td><code>frog.signal.decimate</code></td>
      <td><code>signal</code>, <code>factor</code></td>
      <td><code>result</code></td>
      <td>Keep every <code>factor</code>-th sample.</td>
    </tr>
    <tr>
      <td><code>frog.signal.resample_linear</code></td>
      <td><code>signal</code>, <code>output_length</code></td>
      <td><code>result</code></td>
      <td>Resample by piecewise-linear interpolation.</td>
    </tr>
  </tbody>
</table>

<p>
All primitives in this table are standard-library value calls. They are not
core language operators, and they are not widget operations.
</p>

<hr/>

<h2 id="primitive-contracts">5. Primitive Contracts</h2>

<h3><code>frog.signal.sample_count</code></h3>

<ul>
  <li><code>signal</code>: <code>array&lt;f32&gt;</code> or <code>array&lt;f64&gt;</code></li>
  <li><code>count</code>: <code>u64</code></li>
  <li>Empty input is valid and returns <code>0</code>.</li>
</ul>

<h3><code>frog.signal.mean</code></h3>

<ul>
  <li><code>signal</code> MUST be non-empty.</li>
  <li><code>result</code> is the arithmetic mean of all samples.</li>
</ul>

<h3><code>frog.signal.rms</code></h3>

<ul>
  <li><code>signal</code> MUST be non-empty.</li>
  <li><code>result = sqrt(mean(signal[i] * signal[i]))</code>.</li>
</ul>

<h3><code>frog.signal.peak</code></h3>

<ul>
  <li><code>signal</code> MUST be non-empty.</li>
  <li><code>value</code> is the maximum sample value.</li>
  <li><code>index</code> is the first zero-based index whose sample equals <code>value</code>.</li>
  <li><code>index</code> has type <code>u64</code>.</li>
</ul>

<h3><code>frog.signal.peak_to_peak</code></h3>

<ul>
  <li><code>signal</code> MUST be non-empty.</li>
  <li><code>result = max(signal) - min(signal)</code>.</li>
</ul>

<h3><code>frog.signal.scale</code></h3>

<ul>
  <li><code>gain</code> MUST be a numeric scalar compatible with the signal element type.</li>
  <li>The output length equals the input length.</li>
  <li>For every index <code>i</code>, <code>result[i] = signal[i] * gain</code>.</li>
  <li>Empty input is valid and returns an empty result.</li>
</ul>

<h3><code>frog.signal.offset</code></h3>

<ul>
  <li><code>bias</code> MUST be a numeric scalar compatible with the signal element type.</li>
  <li>The output length equals the input length.</li>
  <li>For every index <code>i</code>, <code>result[i] = signal[i] + bias</code>.</li>
  <li>Empty input is valid and returns an empty result.</li>
</ul>

<h3><code>frog.signal.normalize_peak</code></h3>

<ul>
  <li><code>signal</code> MUST be non-empty.</li>
  <li><code>target_peak</code> MUST be a non-negative numeric scalar compatible with the signal element type.</li>
  <li>The maximum absolute input sample MUST be greater than zero.</li>
  <li>For every index <code>i</code>, <code>result[i] = signal[i] * (target_peak / max(abs(signal)))</code>.</li>
</ul>

<h3><code>frog.signal.moving_average</code></h3>

<ul>
  <li><code>window_size</code> MUST be a positive integer.</li>
  <li>The output length equals the input length.</li>
  <li>For sample index <code>i</code>, the output is the mean of samples from <code>max(0, i - window_size + 1)</code> through <code>i</code>.</li>
  <li>Empty input is valid and returns an empty result.</li>
</ul>

<h3><code>frog.signal.fir</code></h3>

<ul>
  <li><code>coefficients</code> MUST be non-empty.</li>
  <li><code>signal</code> and <code>coefficients</code> MUST have the same floating-point element type after any valid coercion before the primitive boundary.</li>
  <li>The output length equals the input signal length.</li>
  <li>The filter is causal: <code>y[i] = sum(k = 0..M-1) coefficients[k] * signal[i-k]</code>.</li>
  <li>For indices below zero, <code>signal[j]</code> is treated as <code>0</code>.</li>
  <li>Empty signal input is valid and returns an empty result.</li>
</ul>

<h3><code>frog.signal.decimate</code></h3>

<ul>
  <li><code>factor</code> MUST be a positive integer greater than or equal to <code>1</code>.</li>
  <li>The result contains input samples at indices <code>0</code>, <code>factor</code>, <code>2 * factor</code>, and so on.</li>
  <li>No implicit anti-alias filtering is performed.</li>
</ul>

<h3><code>frog.signal.resample_linear</code></h3>

<ul>
  <li><code>signal</code> MUST be non-empty.</li>
  <li><code>output_length</code> MUST be a positive integer.</li>
  <li>If <code>output_length = 1</code>, the result contains the first input sample.</li>
  <li>If the input length is <code>1</code>, that value is replicated to the requested output length.</li>
  <li>Otherwise, result samples are obtained by linear interpolation over the input index range <code>0</code> through <code>input_length - 1</code>.</li>
</ul>

<hr/>

<h2 id="effects-status-and-fir">6. Effects, Status, and FIR</h2>

<p>
Every published <code>frog.signal</code> primitive has:
</p>

<ul>
  <li><code>call_class = standard_library_value</code></li>
  <li><code>effect = pure</code></li>
  <li><code>status_model = none</code></li>
  <li>no provider requirement</li>
</ul>

<p>
The published signal primitives do not return local <code>success</code> or
<code>error_code</code> outputs. Preconditions such as non-empty inputs,
positive factors, and valid normalization denominators are part of validation
and execution-profile obligations. If a violation can be proven before
execution, a validator MUST reject the graph. If a violation can only be
observed during execution, the active execution profile owns the diagnostic or
failure behavior until the uniform FROG status corridor is standardized.
</p>

<p>
FIR MUST preserve the namespace-qualified primitive identity, typed ports,
effect metadata, and status posture. A FIR object for
<code>frog.signal.fir</code> therefore remains a public
<code>frog.signal.fir</code> call with explicit <code>signal</code>,
<code>coefficients</code>, and <code>result</code> ports. It does not become a
private runtime filter object, a hidden loop, a native symbol name, or a
backend-specific artifact in FIR.
</p>

<hr/>

<h2 id="candidate-deferrals">7. Candidate Deferrals</h2>

<p>
The following candidates are useful but not part of the published v0.1
<code>frog.signal</code> surface:
</p>

<table>
  <thead>
    <tr>
      <th>Candidate</th>
      <th>Reason deferred</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>frog.signal.generate_sine</code>, <code>generate_square</code>, <code>generate_ramp</code></td>
      <td>Generation needs a settled coordinate contract: sample index only, explicit duration, or <code>frog.waveform</code> timing.</td>
    </tr>
    <tr>
      <td><code>frog.signal.convolve</code> and broader correlation families</td>
      <td>The v0.1 surface intentionally publishes only the causal FIR shape needed for ordinary filtering.</td>
    </tr>
    <tr>
      <td>FFT, DFT, STFT, and spectral analysis</td>
      <td>Complex values, frequency axes, window catalogs, and provider posture need a larger design pass.</td>
    </tr>
    <tr>
      <td>IIR, adaptive, and designed-filter catalogs</td>
      <td>State, stability, coefficient design, and profile requirements are outside this lightweight surface.</td>
    </tr>
    <tr>
      <td>Streaming and multi-channel processing</td>
      <td>Those surfaces depend on execution, buffering, and acquisition semantics not owned by <code>frog.signal</code>.</td>
    </tr>
  </tbody>
</table>

<hr/>

<h2 id="non-goals">8. Non-goals</h2>

<ul>
  <li>Do not make <code>frog.signal</code> a hidden implementation detail of chart, graph, waveform, or table widgets.</li>
  <li>Do not imply compatibility with any third-party binary signal-processing API.</li>
  <li>Do not define hardware acquisition, streaming, or device timing.</li>
  <li>Do not define complex numbers, units-of-measure, or frequency-domain value types in v0.1.</li>
  <li>Do not publish private runtime scheduling, buffering, provider, or optimization strategies as language law.</li>
</ul>

<hr/>

<h2 id="summary">9. Summary</h2>

<p>
<code>frog.signal</code> is the value-only standard-library surface for
one-dimensional numeric sample arrays. It publishes small, deterministic
sample-count, analysis, affine-transform, normalization, filtering, decimation,
and linear-resampling primitives while leaving waveform metadata, table values,
widgets, hardware acquisition, streaming, and advanced spectral processing to
their proper layers.
</p>

<hr/>

<p align="center">
End of FROG Signal Library Specification
</p>
