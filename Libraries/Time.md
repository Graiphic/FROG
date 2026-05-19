<h1 align="center">FROG Time Library Specification</h1>

<p align="center">
Definition of the standard <strong>frog.time</strong> timing library for FROG v0.1<br/>
<em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#goals">2. Goals</a></li>
  <li><a href="#relation-with-other-specifications">3. Relation with Other Specifications</a></li>
  <li><a href="#role-of-frog-time">4. Role of <code>frog.time</code></a></li>
  <li><a href="#naming-and-namespace">5. Naming and Namespace</a></li>
  <li><a href="#time-model">6. Time Model</a></li>
  <li><a href="#scope-for-v01">7. Scope for v0.1</a></li>
  <li><a href="#typing-and-status-model">8. Typing and Status Model</a></li>
  <li><a href="#capability-and-effect-model">9. Capability and Effect Model</a></li>
  <li><a href="#wall-clock-functions">10. Wall-Clock Functions</a></li>
  <li><a href="#monotonic-and-elapsed-functions">11. Monotonic and Elapsed Functions</a></li>
  <li><a href="#duration-and-arithmetic-functions">12. Duration and Arithmetic Functions</a></li>
  <li><a href="#formatting-and-parsing-functions">13. Formatting and Parsing Functions</a></li>
  <li><a href="#wait-and-delay-boundaries">14. Wait and Delay Boundaries</a></li>
  <li><a href="#profiling-hooks">15. Profiling Hooks</a></li>
  <li><a href="#diagram-and-fir-representation">16. Diagram and FIR Representation</a></li>
  <li><a href="#determinism-debugging-and-replay">17. Determinism, Debugging, and Replay</a></li>
  <li><a href="#relation-with-signal-waveform-and-chart-surfaces">18. Relation with Signal, Waveform, and Chart Surfaces</a></li>
  <li><a href="#validation-rules">19. Validation Rules</a></li>
  <li><a href="#out-of-scope-for-v01">20. Out of Scope for v0.1</a></li>
  <li><a href="#summary">21. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the standard <strong>frog.time</strong> library for FROG v0.1.
</p>

<p>
The <code>frog.time</code> library provides a compact timing surface for:
</p>

<ul>
  <li>reading wall-clock time,</li>
  <li>reading monotonic time for elapsed-time measurement,</li>
  <li>computing elapsed durations,</li>
  <li>performing duration arithmetic and unit conversion,</li>
  <li>formatting and parsing UTC timestamp text,</li>
  <li>declaring wait/delay boundaries as explicit scheduler-capability calls,</li>
  <li>emitting optional profiling marks without making profiling part of program semantics.</li>
</ul>

<p>
Time is intentionally explicit in FROG. A diagram that depends on a clock, a scheduler wait, or
a profiling hook must call a published primitive. Runtimes, compilers, IDEs, and launchers MUST NOT
hide timing behavior behind private scheduler convention.
</p>

<hr/>

<h2 id="goals">2. Goals</h2>

<ul>
  <li><strong>Separation</strong> - keep wall-clock timestamps, monotonic timestamps, durations, waits, and profiling hooks distinct.</li>
  <li><strong>Portability</strong> - define stable primitive identities and scalar port contracts using the v0.1 type system.</li>
  <li><strong>Scheduler honesty</strong> - expose waits and delays as runtime-hosted capabilities, not as hard real-time guarantees.</li>
  <li><strong>Determinism awareness</strong> - make clock reads and waits visible to FIR, lowering, manifests, replay, and debugging profiles.</li>
  <li><strong>Status discipline</strong> - use explicit local status outputs until the FROG-wide status corridor is standardized.</li>
  <li><strong>Minimality</strong> - avoid timer handles, event-loop APIs, cron-like schedulers, and locale-heavy formatting in the first surface.</li>
</ul>

<hr/>

<h2 id="relation-with-other-specifications">3. Relation with Other Specifications</h2>

<p>
This document complements the following specifications:
</p>

<ul>
  <li><strong>Expression/Diagram.md</strong> - defines how primitive calls are serialized as executable graph nodes.</li>
  <li><strong>Expression/Type.md</strong> - defines the scalar types used by the time primitives.</li>
  <li><strong>Libraries/Core.md</strong> - defines pure computation and explicit local memory; <code>frog.time</code> does not replace <code>frog.core.delay</code>.</li>
  <li><strong>Libraries/System.md</strong> - defines bounded host/system introspection and explicitly excludes time and scheduler control.</li>
  <li><strong>Libraries/Signal.md</strong> - defines signal functions over numeric arrays without owning waveform timestamp metadata.</li>
  <li><strong>IR/Library call model.md</strong> - defines how FIR records host-capability calls, effects, status models, and abstract requirements.</li>
  <li><strong>IR/Lowering.md</strong> and <strong>IR/Backend contract.md</strong> - define downstream specialization and manifest/provider boundaries.</li>
  <li><strong>Language/Execution control and observation boundaries.md</strong> - defines safe observation, pause, debug-stop, and terminal boundaries.</li>
</ul>

<p>
This document defines the <code>frog.time</code> primitive catalog only. It does not define a
runtime scheduler implementation, a real-time profile, an IDE event loop, an operating-system timer
API, a private profiling transport, or a universal status value.
</p>

<hr/>

<h2 id="role-of-frog-time">4. Role of <code>frog.time</code></h2>

<p>
The <code>frog.time</code> library provides explicit timing primitives used by executable diagram
nodes of kind <code>primitive</code>.
</p>

<p>
In serialized diagrams, calls to these primitives use namespace-qualified identifiers such as:
</p>

<pre><code>frog.time.now
frog.time.monotonic_now
frog.time.elapsed_since
frog.time.wait_duration
frog.time.format_iso8601_utc</code></pre>

<p>
The role of <code>frog.time</code> is to make timing dependencies visible in source, FIR,
lowering, backend contracts, manifests, debug tooling, and conformance discussion.
</p>

<hr/>

<h2 id="naming-and-namespace">5. Naming and Namespace</h2>

<p>
FROG library primitives use stable namespace-qualified identifiers:
</p>

<pre><code>frog.&lt;library&gt;.&lt;primitive&gt;</code></pre>

<p>
For this document:
</p>

<ul>
  <li><code>frog</code> identifies the language namespace,</li>
  <li><code>time</code> identifies the standard timing library,</li>
  <li>the final segment identifies the primitive name.</li>
</ul>

<p>
Primitive names in <code>frog.time</code> SHOULD use lowercase snake_case where multiple words are needed.
</p>

<hr/>

<h2 id="time-model">6. Time Model</h2>

<h3>6.1 Wall-clock time</h3>

<p>
A wall-clock timestamp represents civil UTC time. It is suitable for logging, display,
file-like metadata, and timestamped data values.
</p>

<p>
In v0.1, a wall-clock timestamp is carried as:
</p>

<pre><code>wall_time_ns: i64</code></pre>

<p>
The value is signed nanoseconds from the Unix epoch, <code>1970-01-01T00:00:00Z</code>.
The timestamp is UTC. It does not carry a local time zone, locale, calendar system, or leap-second
table.
</p>

<p>
Wall-clock time is not monotonic. It can move forward, pause, or move backward according to host
clock adjustment, network time correction, manual setting, virtualization, or replay policy.
Programs MUST NOT use wall-clock timestamps to measure elapsed durations or drive periodic loops
unless a stricter profile explicitly defines that behavior.
</p>

<h3>6.2 Monotonic time</h3>

<p>
A monotonic timestamp represents an implementation-provided non-decreasing clock suitable for
elapsed-time measurement inside one execution context.
</p>

<p>
In v0.1, a monotonic timestamp is carried as:
</p>

<pre><code>monotonic_time_ns: u64</code></pre>

<p>
The origin is implementation-defined. It has no civil meaning and MUST NOT be serialized as a
portable wall-clock timestamp. Monotonic timestamps are meaningful only when compared with other
monotonic timestamps produced by the same active clock source and compatible execution context.
</p>

<h3>6.3 Duration</h3>

<p>
A duration represents signed elapsed time or signed timestamp offset.
</p>

<p>
In v0.1, a duration is carried as:
</p>

<pre><code>duration_ns: i64</code></pre>

<p>
Negative durations are valid for arithmetic and comparison. Scheduler waits reject negative
durations unless a future profile explicitly gives them a meaning.
</p>

<h3>6.4 No first-class time record type in v0.1</h3>

<p>
FROG v0.1 does not standardize library-defined custom time types, records, structs, or
units-of-measure types. The scalar carrier names above describe primitive port roles and value
semantics, not new source-level type expressions.
</p>

<hr/>

<h2 id="scope-for-v01">7. Scope for v0.1</h2>

<p>
FROG v0.1 standardizes the following <code>frog.time</code> primitives:
</p>

<table>
  <thead>
    <tr>
      <th>Area</th>
      <th>Primitive</th>
      <th>Purpose</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Wall-clock</td>
      <td><code>frog.time.now</code></td>
      <td>Reads the current wall-clock UTC timestamp.</td>
    </tr>
    <tr>
      <td>Monotonic</td>
      <td><code>frog.time.monotonic_now</code></td>
      <td>Reads the current monotonic timestamp for elapsed-time measurement.</td>
    </tr>
    <tr>
      <td>Elapsed</td>
      <td><code>frog.time.elapsed_since</code></td>
      <td>Reads monotonic time and computes elapsed duration since a prior monotonic timestamp.</td>
    </tr>
    <tr>
      <td>Elapsed</td>
      <td><code>frog.time.elapsed_between</code></td>
      <td>Computes duration between two monotonic timestamps.</td>
    </tr>
    <tr>
      <td>Duration</td>
      <td><code>frog.time.duration_from_ms</code></td>
      <td>Converts milliseconds to nanosecond duration.</td>
    </tr>
    <tr>
      <td>Duration</td>
      <td><code>frog.time.duration_to_ms</code></td>
      <td>Converts nanosecond duration to milliseconds with explicit rounding.</td>
    </tr>
    <tr>
      <td>Arithmetic</td>
      <td><code>frog.time.add_duration</code></td>
      <td>Adds a duration to a wall-clock timestamp.</td>
    </tr>
    <tr>
      <td>Arithmetic</td>
      <td><code>frog.time.subtract_duration</code></td>
      <td>Subtracts a duration from a wall-clock timestamp.</td>
    </tr>
    <tr>
      <td>Formatting</td>
      <td><code>frog.time.format_iso8601_utc</code></td>
      <td>Formats a wall-clock timestamp as canonical UTC ISO 8601 text.</td>
    </tr>
    <tr>
      <td>Parsing</td>
      <td><code>frog.time.parse_iso8601_utc</code></td>
      <td>Parses supported ISO 8601 timestamp text into UTC nanoseconds.</td>
    </tr>
    <tr>
      <td>Wait</td>
      <td><code>frog.time.wait_duration</code></td>
      <td>Requests a scheduler wait for at least a duration.</td>
    </tr>
    <tr>
      <td>Wait</td>
      <td><code>frog.time.wait_until_monotonic</code></td>
      <td>Requests a scheduler wait until a target monotonic timestamp.</td>
    </tr>
    <tr>
      <td>Periodic helper</td>
      <td><code>frog.time.periodic_next_target</code></td>
      <td>Computes the next monotonic target for drift-aware periodic loops.</td>
    </tr>
    <tr>
      <td>Profiling</td>
      <td><code>frog.time.profile_mark</code></td>
      <td>Emits an optional source-attributed profiling mark.</td>
    </tr>
  </tbody>
</table>

<p>
Timer-handle APIs, asynchronous callbacks, timed loops, wall-clock alarms, cron-like scheduling,
timer events, cancellation tokens, high-resolution benchmarking suites, locale-heavy formatting,
and real-time guarantees are not part of the v0.1 <code>frog.time</code> surface.
</p>

<hr/>

<h2 id="typing-and-status-model">8. Typing and Status Model</h2>

<p>
All <code>frog.time</code> primitives are typed according to <strong>Expression/Type.md</strong>.
</p>

<p>
In v0.1, this library uses only built-in scalar types:
</p>

<ul>
  <li><code>i64</code> for wall-clock nanoseconds and signed durations,</li>
  <li><code>u64</code> for monotonic nanoseconds and non-negative counts where needed,</li>
  <li><code>u8</code> for fractional digit counts,</li>
  <li><code>bool</code> for success, completion, and acceptance flags,</li>
  <li><code>string</code> for formatted timestamps, rounding modes, labels, and local error codes.</li>
</ul>

<p>
Until FROG standardizes a uniform error/status corridor, primitives in this document that can fail
use a local status tuple:
</p>

<ul>
  <li><code>success: bool</code></li>
  <li><code>error_code: string</code></li>
</ul>

<p>
This local tuple is not the final FROG-wide error/status contract. FIR should represent it as
<code>status_model = "local_result_tuple"</code>.
</p>

<p>
Rules:
</p>

<ul>
  <li>if <code>success = true</code>, <code>error_code</code> MUST be the empty string,</li>
  <li>if <code>success = false</code>, every data output MUST still be present, well-typed, and set to its fallback value,</li>
  <li>integer outputs fall back to <code>0</code>,</li>
  <li>string outputs fall back to the empty string,</li>
  <li>boolean data outputs fall back to <code>false</code>.</li>
</ul>

<p>
Standard local error codes are:
</p>

<ul>
  <li><code>unsupported</code> - the implementation or active execution profile does not support the primitive,</li>
  <li><code>permission_denied</code> - host policy denies the clock, scheduler, or profiling operation,</li>
  <li><code>host_policy_blocked</code> - a sandbox, deployment profile, or capability boundary blocks the operation,</li>
  <li><code>clock_unavailable</code> - the required clock source is unavailable,</li>
  <li><code>scheduler_unavailable</code> - the required wait/scheduler service is unavailable,</li>
  <li><code>profiling_unavailable</code> - the requested profiling hook is unavailable,</li>
  <li><code>invalid_input</code> - an input such as a negative wait duration, invalid rounding mode, invalid fractional digit count, or malformed label is invalid,</li>
  <li><code>overflow</code> - arithmetic or unit conversion exceeds the representable scalar range,</li>
  <li><code>parse_error</code> - timestamp text cannot be parsed by the published grammar,</li>
  <li><code>format_error</code> - timestamp text cannot be formatted under the requested options,</li>
  <li><code>interrupted</code> - a scheduler wait was interrupted by cancellation, abort, teardown, or a profile-defined interruption,</li>
  <li><code>unknown_error</code> - the host failed without a more specific standardized code.</li>
</ul>

<hr/>

<h2 id="capability-and-effect-model">9. Capability and Effect Model</h2>

<p>
The <code>frog.time</code> library contains both value-only transforms and runtime-hosted
host-capability calls.
</p>

<table>
  <thead>
    <tr>
      <th>Primitive group</th>
      <th>Call class</th>
      <th>Effect posture</th>
      <th>Capability token</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Arithmetic and unit conversion</td>
      <td><code>standard_library_value</code></td>
      <td><code>pure</code> unless a status-bearing overflow path is taken</td>
      <td>none</td>
    </tr>
    <tr>
      <td>Formatting and parsing</td>
      <td><code>standard_library_value</code></td>
      <td><code>pure</code></td>
      <td>none</td>
    </tr>
    <tr>
      <td>Wall-clock reads</td>
      <td><code>host_capability</code></td>
      <td><code>host_capability_read</code></td>
      <td><code>frog.time.wall_clock.read</code></td>
    </tr>
    <tr>
      <td>Monotonic reads and elapsed-since helpers</td>
      <td><code>host_capability</code></td>
      <td><code>host_capability_read</code></td>
      <td><code>frog.time.monotonic_clock.read</code></td>
    </tr>
    <tr>
      <td>Scheduler waits</td>
      <td><code>host_capability</code></td>
      <td><code>runtime_orchestration</code></td>
      <td><code>frog.time.scheduler.wait</code></td>
    </tr>
    <tr>
      <td>Profiling hooks</td>
      <td><code>host_capability</code></td>
      <td><code>runtime_orchestration</code></td>
      <td><code>frog.time.profiling.mark</code></td>
    </tr>
  </tbody>
</table>

<p>
The <code>runtime_orchestration</code> effect is valid here only because the source-level primitive is
explicit. A private runtime scheduler service that is not called through a public primitive remains
outside ordinary FIR program calls.
</p>

<hr/>

<h2 id="wall-clock-functions">10. Wall-Clock Functions</h2>

<h3>10.1 <code>frog.time.now</code></h3>

<p>
Reads the current wall-clock UTC timestamp.
</p>

<ul>
  <li>input ports: none</li>
  <li>output ports: <code>wall_time_ns</code>, <code>success</code>, <code>error_code</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>wall_time_ns: i64</code></li>
  <li><code>success: bool</code></li>
  <li><code>error_code: string</code></li>
</ul>

<p>
Rules:
</p>

<ul>
  <li>the timestamp is UTC nanoseconds from the Unix epoch,</li>
  <li>the value may move backward or forward between calls,</li>
  <li>the value MUST NOT be used as evidence of elapsed duration without explicit profile support,</li>
  <li>if the wall-clock source is unavailable, the primitive returns <code>success = false</code> and <code>error_code = "clock_unavailable"</code>.</li>
</ul>

<hr/>

<h2 id="monotonic-and-elapsed-functions">11. Monotonic and Elapsed Functions</h2>

<h3>11.1 <code>frog.time.monotonic_now</code></h3>

<p>
Reads the current monotonic timestamp for elapsed-time measurement.
</p>

<ul>
  <li>input ports: none</li>
  <li>output ports: <code>monotonic_time_ns</code>, <code>success</code>, <code>error_code</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>monotonic_time_ns: u64</code></li>
  <li><code>success: bool</code></li>
  <li><code>error_code: string</code></li>
</ul>

<p>
The origin of <code>monotonic_time_ns</code> is implementation-defined and not portable across execution
contexts. Only differences between compatible monotonic timestamps are meaningful.
</p>

<h3>11.2 <code>frog.time.elapsed_since</code></h3>

<p>
Reads the current monotonic timestamp and computes elapsed duration since a prior monotonic timestamp.
</p>

<ul>
  <li>input port: <code>start_monotonic_ns</code></li>
  <li>output ports: <code>elapsed_ns</code>, <code>current_monotonic_ns</code>, <code>success</code>, <code>error_code</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>start_monotonic_ns: u64</code></li>
  <li><code>elapsed_ns: i64</code></li>
  <li><code>current_monotonic_ns: u64</code></li>
  <li><code>success: bool</code></li>
  <li><code>error_code: string</code></li>
</ul>

<p>
This primitive depends on <code>frog.time.monotonic_clock.read</code>. It fails if the current
monotonic clock is unavailable, if the computed difference exceeds the <code>i64</code> duration range,
or if the current monotonic value is less than <code>start_monotonic_ns</code> under the active clock source.
</p>

<h3>11.3 <code>frog.time.elapsed_between</code></h3>

<p>
Computes the signed duration between two monotonic timestamps.
</p>

<ul>
  <li>input ports: <code>start_monotonic_ns</code>, <code>end_monotonic_ns</code></li>
  <li>output ports: <code>elapsed_ns</code>, <code>success</code>, <code>error_code</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>start_monotonic_ns: u64</code></li>
  <li><code>end_monotonic_ns: u64</code></li>
  <li><code>elapsed_ns: i64</code></li>
  <li><code>success: bool</code></li>
  <li><code>error_code: string</code></li>
</ul>

<p>
This is a value transform. It succeeds when <code>end_monotonic_ns &gt;= start_monotonic_ns</code> and
the difference fits in <code>i64</code>. It does not prove that the two timestamps came from the same
clock source; validators and profiles MAY add stricter provenance checks later.
</p>

<hr/>

<h2 id="duration-and-arithmetic-functions">12. Duration and Arithmetic Functions</h2>

<h3>12.1 <code>frog.time.duration_from_ms</code></h3>

<p>
Converts milliseconds to signed nanoseconds.
</p>

<ul>
  <li>input port: <code>milliseconds</code></li>
  <li>output ports: <code>duration_ns</code>, <code>success</code>, <code>error_code</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>milliseconds: i64</code></li>
  <li><code>duration_ns: i64</code></li>
  <li><code>success: bool</code></li>
  <li><code>error_code: string</code></li>
</ul>

<p>
The primitive fails with <code>overflow</code> if multiplying by <code>1_000_000</code> exceeds the
<code>i64</code> range.
</p>

<h3>12.2 <code>frog.time.duration_to_ms</code></h3>

<p>
Converts signed nanoseconds to milliseconds with explicit rounding.
</p>

<ul>
  <li>input ports: <code>duration_ns</code>, <code>rounding_mode</code></li>
  <li>output ports: <code>milliseconds</code>, <code>success</code>, <code>error_code</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>duration_ns: i64</code></li>
  <li><code>rounding_mode: string</code></li>
  <li><code>milliseconds: i64</code></li>
  <li><code>success: bool</code></li>
  <li><code>error_code: string</code></li>
</ul>

<p>
Valid rounding modes are <code>truncate</code>, <code>floor</code>, <code>ceil</code>, and
<code>nearest</code>. The <code>nearest</code> mode rounds half away from zero.
</p>

<h3>12.3 <code>frog.time.add_duration</code></h3>

<p>
Adds a signed duration to a wall-clock timestamp.
</p>

<ul>
  <li>input ports: <code>wall_time_ns</code>, <code>duration_ns</code></li>
  <li>output ports: <code>result_wall_time_ns</code>, <code>success</code>, <code>error_code</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>wall_time_ns: i64</code></li>
  <li><code>duration_ns: i64</code></li>
  <li><code>result_wall_time_ns: i64</code></li>
  <li><code>success: bool</code></li>
  <li><code>error_code: string</code></li>
</ul>

<p>
The primitive fails with <code>overflow</code> if the result exceeds the <code>i64</code> range.
</p>

<h3>12.4 <code>frog.time.subtract_duration</code></h3>

<p>
Subtracts a signed duration from a wall-clock timestamp.
</p>

<ul>
  <li>input ports: <code>wall_time_ns</code>, <code>duration_ns</code></li>
  <li>output ports: <code>result_wall_time_ns</code>, <code>success</code>, <code>error_code</code></li>
</ul>

<p>
The typing and overflow rules are the same as <code>frog.time.add_duration</code>.
</p>

<hr/>

<h2 id="formatting-and-parsing-functions">13. Formatting and Parsing Functions</h2>

<h3>13.1 <code>frog.time.format_iso8601_utc</code></h3>

<p>
Formats a wall-clock timestamp as canonical UTC ISO 8601 text.
</p>

<ul>
  <li>input ports: <code>wall_time_ns</code>, <code>fractional_digits</code></li>
  <li>output ports: <code>text</code>, <code>success</code>, <code>error_code</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>wall_time_ns: i64</code></li>
  <li><code>fractional_digits: u8</code></li>
  <li><code>text: string</code></li>
  <li><code>success: bool</code></li>
  <li><code>error_code: string</code></li>
</ul>

<p>
Rules:
</p>

<ul>
  <li><code>fractional_digits</code> MUST be in the inclusive range <code>0</code> through <code>9</code>,</li>
  <li>the result MUST use UTC and end with <code>Z</code>,</li>
  <li>the date separator is <code>-</code>, the time separator is <code>:</code>, and the date-time separator is <code>T</code>,</li>
  <li>locale-specific month names, calendars, localized digits, and time-zone names are out of scope.</li>
</ul>

<p>
Example output:
</p>

<pre><code>2026-05-20T13:45:12.123456789Z</code></pre>

<h3>13.2 <code>frog.time.parse_iso8601_utc</code></h3>

<p>
Parses supported ISO 8601 timestamp text into UTC nanoseconds.
</p>

<ul>
  <li>input port: <code>text</code></li>
  <li>output ports: <code>wall_time_ns</code>, <code>success</code>, <code>error_code</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>text: string</code></li>
  <li><code>wall_time_ns: i64</code></li>
  <li><code>success: bool</code></li>
  <li><code>error_code: string</code></li>
</ul>

<p>
The v0.1 parser accepts:
</p>

<ul>
  <li><code>YYYY-MM-DDTHH:MM:SSZ</code>,</li>
  <li><code>YYYY-MM-DDTHH:MM:SS.sssZ</code> with one through nine fractional digits,</li>
  <li>the same forms with numeric UTC offsets such as <code>+02:00</code> or <code>-05:30</code>, converted to UTC.</li>
</ul>

<p>
Leap-second notation such as <code>23:59:60</code>, named time zones, locale formats, week dates,
ordinal dates, relaxed separators, and ambiguous local times are not supported in v0.1.
</p>

<hr/>

<h2 id="wait-and-delay-boundaries">14. Wait and Delay Boundaries</h2>

<p>
Wait primitives are explicit scheduler-capability calls. They request that the active execution host
delay continuation of the relevant execution activity. They do not promise exact wakeup time, hard
real-time behavior, OS-level sleep mechanics, or one implementation strategy.
</p>

<p>
Implementations MUST NOT realize waits as hidden runtime folklore. FIR, lowering, backend contracts,
manifests, launchers, or comparable downstream representations must preserve that the program uses
a scheduler wait capability.
</p>

<h3>14.1 <code>frog.time.wait_duration</code></h3>

<p>
Requests a scheduler wait for at least the supplied duration.
</p>

<ul>
  <li>input port: <code>duration_ns</code></li>
  <li>output ports: <code>completed</code>, <code>actual_elapsed_ns</code>, <code>success</code>, <code>error_code</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>duration_ns: i64</code></li>
  <li><code>completed: bool</code></li>
  <li><code>actual_elapsed_ns: i64</code></li>
  <li><code>success: bool</code></li>
  <li><code>error_code: string</code></li>
</ul>

<p>
Rules:
</p>

<ul>
  <li>negative durations are invalid and MUST produce <code>success = false</code> with <code>error_code = "invalid_input"</code>,</li>
  <li>a zero duration MAY yield at a scheduler boundary but MUST NOT be treated as a hard synchronization barrier,</li>
  <li>when the wait completes normally, <code>completed = true</code> and <code>success = true</code>,</li>
  <li><code>actual_elapsed_ns</code> is measured with the active monotonic clock when available,</li>
  <li>if the wait is interrupted, aborted, blocked by policy, or unsupported, <code>completed = false</code> and <code>success = false</code>.</li>
</ul>

<h3>14.2 <code>frog.time.wait_until_monotonic</code></h3>

<p>
Requests a scheduler wait until a target monotonic timestamp.
</p>

<ul>
  <li>input port: <code>target_monotonic_ns</code></li>
  <li>output ports: <code>completed</code>, <code>actual_monotonic_ns</code>, <code>lateness_ns</code>, <code>success</code>, <code>error_code</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>target_monotonic_ns: u64</code></li>
  <li><code>completed: bool</code></li>
  <li><code>actual_monotonic_ns: u64</code></li>
  <li><code>lateness_ns: i64</code></li>
  <li><code>success: bool</code></li>
  <li><code>error_code: string</code></li>
</ul>

<p>
If the target is already in the past for the active monotonic clock, the primitive completes without
requesting a positive delay. <code>lateness_ns</code> is <code>actual_monotonic_ns - target_monotonic_ns</code>
when the difference fits in <code>i64</code>.
</p>

<h3>14.3 <code>frog.time.periodic_next_target</code></h3>

<p>
Computes the next target for a monotonic periodic loop without accumulating drift from the current
wakeup time.
</p>

<ul>
  <li>input ports: <code>previous_target_monotonic_ns</code>, <code>period_ns</code></li>
  <li>output ports: <code>next_target_monotonic_ns</code>, <code>success</code>, <code>error_code</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>previous_target_monotonic_ns: u64</code></li>
  <li><code>period_ns: i64</code></li>
  <li><code>next_target_monotonic_ns: u64</code></li>
  <li><code>success: bool</code></li>
  <li><code>error_code: string</code></li>
</ul>

<p>
<code>period_ns</code> MUST be positive. The primitive fails with <code>overflow</code> if the next target
does not fit in <code>u64</code>.
</p>

<h3>14.4 Wait boundaries and UI responsiveness</h3>

<p>
A wait may cause an execution activity to yield to a host scheduler, but it must not authorize a
runtime to expose impossible source-level intermediate states. Debugging, pausing, and observation
remain constrained by the safe boundary rules in the language layer.
</p>

<p>
For hosts with front panels or IDE event loops, a wait SHOULD allow the host to remain responsive
when the active profile supports that behavior. This is a host responsiveness expectation, not a
source-level guarantee about event ordering, repaint timing, or UI toolkit mechanics.
</p>

<hr/>

<h2 id="profiling-hooks">15. Profiling Hooks</h2>

<h3>15.1 <code>frog.time.profile_mark</code></h3>

<p>
Emits an optional source-attributed profiling mark.
</p>

<ul>
  <li>input port: <code>label</code></li>
  <li>output ports: <code>accepted</code>, <code>success</code>, <code>error_code</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>label: string</code></li>
  <li><code>accepted: bool</code></li>
  <li><code>success: bool</code></li>
  <li><code>error_code: string</code></li>
</ul>

<p>
The profiling mark is diagnostic. It MUST NOT change the meaning of ordinary dataflow values except
through its explicit status outputs. When profiling is disabled or unavailable, the primitive may
return <code>success = false</code> with <code>error_code = "profiling_unavailable"</code>.
</p>

<p>
Profiling spans, sampled profilers, performance counters, benchmark harnesses, and private trace
formats are out of scope for v0.1.
</p>

<hr/>

<h2 id="diagram-and-fir-representation">16. Diagram and FIR Representation</h2>

<p>
Calls to <code>frog.time</code> primitives are serialized as <code>primitive</code> nodes in the
diagram.
</p>

<p>
Examples:
</p>

<pre><code>{
  "id": "clock_1",
  "kind": "primitive",
  "type": "frog.time.monotonic_now"
}</code></pre>

<pre><code>{
  "id": "wait_1",
  "kind": "primitive",
  "type": "frog.time.wait_duration"
}</code></pre>

<p>
The exact port existence, direction, typing, status posture, effect posture, and capability
requirement of these nodes are resolved from this specification together with the type system,
graph rules, language execution rules, and FIR library-call model.
</p>

<p>
For FIR representation:
</p>

<ul>
  <li>value-only transforms SHOULD use <code>call_class = "standard_library_value"</code>,</li>
  <li>clock reads SHOULD use <code>call_class = "host_capability"</code> and <code>effect = "host_capability_read"</code>,</li>
  <li>wait and profiling calls SHOULD use <code>call_class = "host_capability"</code> and <code>effect = "runtime_orchestration"</code>,</li>
  <li>status-bearing calls SHOULD use <code>status_model = "local_result_tuple"</code>,</li>
  <li>clock, wait, and profiling calls MUST preserve abstract capability tokens rather than private runtime service names.</li>
</ul>

<hr/>

<h2 id="determinism-debugging-and-replay">17. Determinism, Debugging, and Replay</h2>

<p>
Clock reads and waits are not pure deterministic value transforms. They observe or request behavior
from the active execution host. This dependency must remain explicit.
</p>

<p>
Deterministic execution, replay, testing, and simulation profiles MAY provide injected clock and
scheduler providers. Such providers must be declared by the active profile, backend contract,
manifest, test harness, or comparable execution contract. A runtime MUST NOT silently replace
clock reads with invented deterministic values while claiming ordinary host-clock behavior.
</p>

<p>
Debug pause time is profile-sensitive:
</p>

<ul>
  <li>ordinary host-clock profiles may let wall and monotonic clock observations advance while execution is paused,</li>
  <li>replay or simulation profiles may freeze or script clock observations,</li>
  <li>the selected policy must be visible as a profile or host capability claim when it affects source-visible timing results.</li>
</ul>

<p>
Safe observation, pause, debug-stop, and terminal boundaries still apply. A wait primitive may be a
natural place for an implementation to yield, pause, or observe, but it does not permit exposure of
partially committed source-level state.
</p>

<hr/>

<h2 id="relation-with-signal-waveform-and-chart-surfaces">18. Relation with Signal, Waveform, and Chart Surfaces</h2>

<p>
<code>frog.signal</code> v0.1 operates on numeric arrays and does not own timestamp metadata.
</p>

<p>
Future <code>frog.waveform</code> timing fields should use <code>frog.time</code> semantics:
</p>

<ul>
  <li><code>t0</code> for absolute waveform start time should map to <code>wall_time_ns</code> when absolute UTC time is needed,</li>
  <li><code>dt</code> for uniform sample interval should map to <code>duration_ns</code> when integer nanosecond precision is sufficient,</li>
  <li>relative-time waveforms should use duration semantics rather than wall-clock timestamps,</li>
  <li>irregular samples should use explicit x/time arrays or a future waveform-specific representation.</li>
</ul>

<p>
Widget-level chart fields such as <code>timebase.t0</code>, <code>timebase.dt</code>,
<code>timebase.unit</code>, and <code>timebase.sample_rate</code> remain chart value and widget-class
surface until a waveform library publishes a concrete value model. They must not silently define
the source-level <code>frog.time</code> API.
</p>

<hr/>

<h2 id="validation-rules">19. Validation Rules</h2>

<p>
Implementations that support <code>frog.time</code> MUST enforce the following rules:
</p>

<ul>
  <li>every <code>frog.time</code> primitive reference MUST identify a valid standardized primitive name from this document,</li>
  <li>all required input ports MUST exist and be type-compatible,</li>
  <li>all produced output ports MUST match the primitive definition,</li>
  <li>status-bearing primitives MUST expose <code>success</code> and <code>error_code</code> outputs exactly as defined here,</li>
  <li>wall-clock and monotonic timestamp values MUST NOT be used interchangeably,</li>
  <li>wait primitives MUST preserve scheduler capability requirements in FIR, lowering, backend contracts, manifests, or comparable downstream representation,</li>
  <li>unsupported clock, wait, or profiling use MUST be rejected or reported as unsupported rather than guessed from private runtime behavior,</li>
  <li>no implementation may claim hard real-time timing, deterministic replay, or profiling support unless the active profile or host contract explicitly provides it.</li>
</ul>

<hr/>

<h2 id="out-of-scope-for-v01">20. Out of Scope for v0.1</h2>

<p>
The following are outside the strict scope of <code>frog.time</code> in v0.1:
</p>

<ul>
  <li>hard real-time guarantees, deterministic jitter bounds, priority scheduling, or deadline scheduling,</li>
  <li>timed loops or event structures as source-level structure families,</li>
  <li>timer handles, cancellation tokens, async callbacks, promises, futures, or event-loop APIs,</li>
  <li>wall-clock alarms, cron-like scheduling, calendar recurrence, business calendars, or time-zone databases,</li>
  <li>locale-specific formatting, named time zones, leap-second tables, non-Gregorian calendars, and natural-language parsing,</li>
  <li>benchmark frameworks, sampled profilers, performance counters, flame graphs, private trace formats, or IDE-specific profiling transports,</li>
  <li>hardware timer APIs, DAQ timing, audio clocks, GPU clocks, distributed clock synchronization, PTP/NTP control, or multi-machine time coherence,</li>
  <li>using waits as hidden UI event pumps, hidden synchronization primitives, or substitutes for explicit local memory and dataflow dependencies,</li>
  <li>a universal structured error object or FROG-wide error/status corridor.</li>
</ul>

<hr/>

<h2 id="summary">21. Summary</h2>

<p>
The <code>frog.time</code> library defines the first standard FROG timing surface.
</p>

<p>
It provides:
</p>

<ul>
  <li>wall-clock UTC reads through <code>frog.time.now</code>,</li>
  <li>monotonic reads and elapsed-time helpers,</li>
  <li>nanosecond duration conversion and wall-clock arithmetic,</li>
  <li>ISO 8601 UTC formatting and parsing,</li>
  <li>scheduler wait boundaries with explicit capability requirements,</li>
  <li>optional profiling marks that remain diagnostic rather than semantic.</li>
</ul>

<p>
The surface is intentionally explicit, status-bearing where needed, capability-aware, and honest
about scheduler limits. It does not turn one runtime scheduler, one operating-system clock API, or
one profiling implementation into FROG language truth.
</p>

<hr/>

<p align="center">
End of FROG Time Library Specification
</p>
