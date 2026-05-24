<h1 align="center">🐸 FROG Math Library Specification</h1>

<p align="center">
Definition of the standard <strong>frog.math</strong> library for FROG v0.1<br/>
<em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#goals">2. Goals</a></li>
  <li><a href="#relation-with-other-specifications">3. Relation with Other Specifications</a></li>
  <li><a href="#role-of-frog-math">4. Role of <code>frog.math</code></a></li>
  <li><a href="#naming-and-namespace">5. Naming and Namespace</a></li>
  <li><a href="#scope-for-v01">6. Scope for v0.1</a></li>
  <li><a href="#library-categories">7. Library Categories</a></li>
  <li><a href="#typing-model">8. Typing Model</a></li>
  <li><a href="#numeric-utility-functions">9. Numeric Utility Functions</a></li>
  <li><a href="#rounding-functions">10. Rounding Functions</a></li>
  <li><a href="#power-root-and-logarithmic-functions">11. Power, Root, and Logarithmic Functions</a></li>
  <li><a href="#trigonometric-functions">12. Trigonometric Functions</a></li>
  <li><a href="#hyperbolic-functions">13. Hyperbolic Functions</a></li>
  <li><a href="#diagram-representation">14. Diagram Representation</a></li>
  <li><a href="#validation-rules">15. Validation Rules</a></li>
  <li><a href="#examples">16. Examples</a></li>
  <li><a href="#out-of-scope">17. Out of Scope for v0.1</a></li>
  <li><a href="#summary">18. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the standard <strong>frog.math</strong> library for FROG v0.1.
</p>

<p>
The <code>frog.math</code> library complements the minimal <code>frog.core</code> library by defining a first standard set of scalar mathematical primitives beyond the built-in core vocabulary.
</p>

<p>
In v0.1, this library focuses on portable and commonly expected scalar mathematics, including:
</p>

<ul>
  <li>numeric utility functions,</li>
  <li>rounding functions,</li>
  <li>power, root, exponential, and logarithmic functions,</li>
  <li>trigonometric functions,</li>
  <li>hyperbolic functions.</li>
</ul>

<p>
This document does not attempt to define full linear algebra, statistics, optimization, signal processing, tensor computation, or symbolic mathematics.
</p>

<p>
This document also records the public promotion boundary for the staged math
surface. The existing flat scalar <code>frog.math.*</code> v0.1 names remain
valid. Candidate names such as <code>frog.numeric</code> and deeper
<code>frog.math.*</code> domain namespaces are documented here as staged
surfaces, not as implemented v0.1 APIs unless this document explicitly marks
them as published primitives.
</p>

<hr/>

<h2 id="goals">2. Goals</h2>

<ul>
  <li><strong>Usefulness</strong> — provide a first standard mathematical library that is broadly useful in practical programs.</li>
  <li><strong>Separation of concerns</strong> — keep minimal language primitives in <code>frog.core</code> while placing broader scalar mathematics in a dedicated library.</li>
  <li><strong>Portability</strong> — ensure that the defined function names and port models remain stable across conforming implementations.</li>
  <li><strong>Clarity</strong> — define each primitive with an explicit name, role, and expected typing model.</li>
  <li><strong>Extensibility</strong> — leave room for staged future namespaces such as <code>frog.numeric</code>, <code>frog.math.linalg</code>, <code>frog.math.stats</code>, and related domain surfaces without treating them as implemented v0.1 APIs prematurely.</li>
</ul>

<hr/>

<h2 id="relation-with-other-specifications">3. Relation with Other Specifications</h2>

<p>
This document complements the following specifications:
</p>

<ul>
  <li><strong>Libraries/Core.md</strong> — defines the minimal standard primitive core, including basic arithmetic and comparison primitives.</li>
  <li><strong>Libraries/Signal.md</strong>, <strong>Libraries/Waveform.md</strong>, and <strong>Libraries/Table.md</strong> — own signal, waveform, and tabular value semantics that must not be absorbed by generic math.</li>
  <li><strong>Expression/Diagram.md</strong> — defines how library functions are serialized as diagram nodes.</li>
  <li><strong>Expression/Type.md</strong> — defines built-in types, type compatibility, and implicit coercion rules.</li>
  <li><strong>Expression/Control structures.md</strong> — defines standardized language structures, which remain distinct from ordinary primitive functions.</li>
  <li><strong>IR/Library call model.md</strong>, <strong>IR/Lowering.md</strong>, and <strong>IR/Backend contract.md</strong> — define how published math call identity, provider requirements, status behavior, and backend handoff remain explicit downstream.</li>
</ul>

<p>
This document defines the standard mathematical primitive vocabulary of <code>frog.math</code>.
It does not redefine the graph model, the type system, or language structures.
</p>

<hr/>

<h2 id="role-of-frog-math">4. Role of <code>frog.math</code></h2>

<p>
The <code>frog.math</code> library provides standardized mathematical primitives that go beyond the minimal built-in core vocabulary.
</p>

<p>
In language terms, these are library functions.
In the serialized diagram representation defined by <code>Expression/Diagram.md</code>, calls to these functions appear as <code>primitive</code> nodes.
</p>

<p>
Therefore:
</p>

<ul>
  <li><code>frog.math.sqrt</code> is a standard mathematical function,</li>
  <li>in a diagram, that function call appears as a <code>primitive</code> node with <code>type = "frog.math.sqrt"</code>.</li>
  <li>in FIR, that call remains a standard-library value call rather than an intrinsic core operator or a private runtime operation.</li>
</ul>

<p>
Basic arithmetic and comparison operations that belong to
<code>frog.core</code> remain intrinsic core primitives. The presence of
<code>frog.math</code> does not move addition, subtraction, multiplication,
division, comparison, boolean logic, selection, or explicit state carriers out
of the core language corridor.
</p>

<hr/>

<h2 id="naming-and-namespace">5. Naming and Namespace</h2>

<p>
FROG uses the following general namespace pattern for built-in and library-defined primitives:
</p>

<pre>
frog.&lt;library&gt;.&lt;primitive&gt;
</pre>

<p>
For this document:
</p>

<ul>
  <li><code>frog</code> identifies the language namespace,</li>
  <li><code>math</code> identifies the standard scalar mathematics library,</li>
  <li>the final segment identifies the primitive name.</li>
</ul>

<p>
Examples:
</p>

<pre>
frog.math.sqrt
frog.math.pow
frog.math.sin
frog.math.round
</pre>

<p>
Primitive names in <code>frog.math</code> SHOULD use lowercase snake_case where multiple words are needed.
</p>

<hr/>

<h2 id="scope-for-v01">6. Scope for v0.1</h2>

<p>
FROG v0.1 standardizes the following mathematical families in <code>frog.math</code>:
</p>

<ul>
  <li>numeric utility functions,</li>
  <li>rounding functions,</li>
  <li>power and root functions,</li>
  <li>exponential and logarithmic functions,</li>
  <li>trigonometric functions,</li>
  <li>hyperbolic functions.</li>
</ul>

<p>
In v0.1, <code>frog.math</code> is intentionally limited to scalar mathematics.
Array-wide, matrix-wide, tensor-wide, and domain-specific mathematical libraries are outside the strict scope of this document.
</p>

<h3>6.1 Compatibility of the Flat Scalar Surface</h3>

<p>
The published v0.1 scalar surface uses flat names such as
<code>frog.math.sqrt</code>, <code>frog.math.sin</code>,
<code>frog.math.atan2</code>, and <code>frog.math.clamp</code>. Those names
remain the public v0.1 primitive identities. A future organization layer such
as <code>frog.math.elementary</code> may improve discovery or palette grouping,
but it must not silently break or reinterpret the published flat names.
</p>

<h3>6.2 Conservative <code>frog.numeric</code> Candidate</h3>

<p>
For T048, <code>frog.numeric</code> is intentionally introduced as a
conservative candidate section inside this document rather than as a separate
public library page. This avoids publishing a broad numeric API before the
source type model, conversion posture, and conformance cases are ready.
</p>

<p>
The candidate scope for <code>frog.numeric</code> is limited to core-adjacent
numeric support:
</p>

<ul>
  <li>numeric constants such as implementation-independent mathematical constants where their precision and type rules are explicit,</li>
  <li>scalar classification such as finite, NaN, infinity, signbit, and comparable representation-aware predicates,</li>
  <li>explicit conversions and coercion helpers once the public source/type posture defines their legality,</li>
  <li>machine-representation helpers such as epsilon only when their target/profile dependency is stated clearly.</li>
</ul>

<p>
Until those rules are promoted explicitly, a source file must not assume that
<code>frog.numeric.*</code> is a published v0.1 primitive family merely because
the candidate namespace is documented here.
</p>

<h3>6.3 Staged Future Math Namespaces</h3>

<p>
The following namespaces are staged for future public work. They are not
implemented v0.1 primitive catalogs unless a future revision adds exact
primitive ids, ports, type rules, status behavior, and conformance cases.
</p>

<table>
  <thead>
    <tr>
      <th>Namespace</th>
      <th>Current public posture</th>
      <th>Boundary</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>frog.math.elementary</code></td>
      <td>Candidate organization layer.</td>
      <td>May group scalar utility, rounding, power/root/log, trigonometric, hyperbolic, and selected special functions without replacing flat v0.1 names.</td>
    </tr>
    <tr>
      <td><code>frog.math.complex</code></td>
      <td>Candidate staged surface.</td>
      <td>Requires a public complex value/type model before primitive promotion.</td>
    </tr>
    <tr>
      <td><code>frog.math.random</code></td>
      <td>Candidate staged surface.</td>
      <td>Must use explicit random state or explicit entropy/capability inputs; hidden global random state is not a valid public contract.</td>
    </tr>
    <tr>
      <td><code>frog.math.linalg</code></td>
      <td>Deferred provider-aware surface.</td>
      <td>Matrix/vector values, shape rules, status behavior, and provider/capability requirements must be explicit before promotion.</td>
    </tr>
    <tr>
      <td><code>frog.math.stats</code></td>
      <td>Split staged surface.</td>
      <td>Simple descriptive statistics may become value-only; distributions and statistical tests need status and numeric edge rules.</td>
    </tr>
    <tr>
      <td><code>frog.math.interpolate</code></td>
      <td>Candidate staged surface.</td>
      <td>Requires precondition and status rules for dimensions, ordered axes, duplicate samples, and out-of-range policy.</td>
    </tr>
    <tr>
      <td><code>frog.math.fit</code></td>
      <td>Deferred provider-aware surface.</td>
      <td>Requires model shape, status, provider, and later callable/function-reference policy.</td>
    </tr>
    <tr>
      <td><code>frog.math.optimize</code></td>
      <td>Deferred provider-aware surface.</td>
      <td>Requires callable/function-reference, status, cancellation/timeout, convergence, and provider rules.</td>
    </tr>
    <tr>
      <td><code>frog.math.calculus</code></td>
      <td>Deferred surface.</td>
      <td>Requires function-reference, step/error controls, status, cancellation, and provider posture for advanced solvers.</td>
    </tr>
  </tbody>
</table>

<h3>6.4 Boundaries With Other Libraries</h3>

<ul>
  <li><code>frog.signal</code> owns signal sample-array operations, filtering, decimation, resampling, and future signal-specific analysis.</li>
  <li><code>frog.waveform</code> owns sampled waveform values with timing semantics such as <code>t0</code>, <code>dt</code>, and sample arrays.</li>
  <li><code>frog.table</code> owns portable rectangular table values and table transforms, not statistical modeling or dataframe engines.</li>
  <li><code>frog.time</code> owns clocks, durations, waits, formatting/parsing, and profiling marks, not numerical calculus time-step policy.</li>
</ul>

<hr/>

<h2 id="library-categories">7. Library Categories</h2>

<p>
The standard <code>frog.math</code> library is organized into the following categories:
</p>

<ul>
  <li><strong>Numeric Utility</strong></li>
  <li><strong>Rounding</strong></li>
  <li><strong>Powers, Roots, and Logarithms</strong></li>
  <li><strong>Trigonometric</strong></li>
  <li><strong>Hyperbolic</strong></li>
</ul>

<p>
This categorization is semantic.
It does not impose a mandatory palette layout, but it provides a stable foundation for IDE organization.
</p>

<hr/>

<h2 id="typing-model">8. Typing Model</h2>

<p>
All <code>frog.math</code> functions are typed according to <strong>Expression/Type.md</strong>.
</p>

<p>
Unless stated otherwise:
</p>

<ul>
  <li>all input and output ports MUST use valid FROG types,</li>
  <li>all compatibility checks MUST follow the standard FROG type rules,</li>
  <li>all implicit coercions MUST follow the standard FROG coercion rules,</li>
  <li>all functions in this document are stateless and side-effect-free,</li>
  <li>all functions in this document operate on scalar values in v0.1.</li>
</ul>

<p>
Required support for v0.1:
</p>

<ul>
  <li>every conforming implementation that supports <code>frog.math</code> MUST support <code>float32</code> and <code>float64</code> for all primitives in this document,</li>
  <li>support for additional numeric scalar types MAY be defined by the active execution profile,</li>
  <li>behavior for domain errors, infinities, NaNs, overflow, underflow, and implementation-defined numeric edge cases MUST be defined by the active execution profile for each supported numeric family.</li>
</ul>

<p>
This document standardizes primitive identity, port models, and source-level meaning.
It does not require a single universal floating-point exception model across all runtimes.
</p>

<hr/>

<h2 id="numeric-utility-functions">9. Numeric Utility Functions</h2>

<h3>9.1 <code>frog.math.min</code></h3>

<p>
Returns the smaller of two numeric scalar inputs.
</p>

<ul>
  <li>input ports: <code>a</code>, <code>b</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>
The two inputs MUST be type-compatible under the FROG type rules.
The output type is the resolved common result type.
</p>

<h3>9.2 <code>frog.math.max</code></h3>

<p>
Returns the larger of two numeric scalar inputs.
</p>

<ul>
  <li>input ports: <code>a</code>, <code>b</code></li>
  <li>output port: <code>result</code></li>
</ul>

<h3>9.3 <code>frog.math.clamp</code></h3>

<p>
Constrains a numeric scalar input to a closed interval.
</p>

<ul>
  <li>input ports: <code>value</code>, <code>min_value</code>, <code>max_value</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>
Rules:
</p>

<ul>
  <li>all three inputs MUST be type-compatible,</li>
  <li><code>min_value</code> SHOULD be less than or equal to <code>max_value</code>,</li>
  <li>the output type is the resolved common result type.</li>
</ul>

<h3>9.4 <code>frog.math.sign</code></h3>

<p>
Returns the sign classification of a numeric scalar input.
</p>

<ul>
  <li>input port: <code>in</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>
In v0.1, the output value MUST be negative, zero, or positive according to the active execution profile for the supported numeric family.
The output type is the same as the resolved input type unless the active execution profile defines a stricter standard result type.
</p>

<h3>9.5 <code>frog.math.square</code></h3>

<p>
Returns the square of a numeric scalar input.
</p>

<ul>
  <li>input port: <code>in</code></li>
  <li>output port: <code>result</code></li>
</ul>

<h3>9.6 <code>frog.math.reciprocal</code></h3>

<p>
Returns the reciprocal of a numeric scalar input.
</p>

<ul>
  <li>input port: <code>in</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>
Division-by-zero and non-finite behavior MUST be defined by the active execution profile.
</p>

<hr/>

<h2 id="rounding-functions">10. Rounding Functions</h2>

<h3>10.1 <code>frog.math.floor</code></h3>

<p>
Returns the greatest integral value not greater than the input.
</p>

<ul>
  <li>input port: <code>in</code></li>
  <li>output port: <code>result</code></li>
</ul>

<h3>10.2 <code>frog.math.ceil</code></h3>

<p>
Returns the smallest integral value not less than the input.
</p>

<ul>
  <li>input port: <code>in</code></li>
  <li>output port: <code>result</code></li>
</ul>

<h3>10.3 <code>frog.math.trunc</code></h3>

<p>
Returns the integral value obtained by truncation toward zero.
</p>

<ul>
  <li>input port: <code>in</code></li>
  <li>output port: <code>result</code></li>
</ul>

<h3>10.4 <code>frog.math.round</code></h3>

<p>
Returns the rounded integral value nearest to the input according to the active execution profile.
</p>

<ul>
  <li>input port: <code>in</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>
Tie-breaking behavior MUST be defined by the active execution profile.
</p>

<hr/>

<h2 id="power-root-and-logarithmic-functions">11. Power, Root, and Logarithmic Functions</h2>

<h3>11.1 <code>frog.math.pow</code></h3>

<p>
Raises a base value to a power.
</p>

<ul>
  <li>input ports: <code>base</code>, <code>exponent</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>
The input operands MUST be numeric scalar values.
The output type is the resolved result type under the active execution profile.
</p>

<h3>11.2 <code>frog.math.sqrt</code></h3>

<p>
Returns the square root of a numeric scalar input.
</p>

<ul>
  <li>input port: <code>in</code></li>
  <li>output port: <code>result</code></li>
</ul>

<h3>11.3 <code>frog.math.exp</code></h3>

<p>
Returns the natural exponential of a numeric scalar input.
</p>

<ul>
  <li>input port: <code>in</code></li>
  <li>output port: <code>result</code></li>
</ul>

<h3>11.4 <code>frog.math.ln</code></h3>

<p>
Returns the natural logarithm of a numeric scalar input.
</p>

<ul>
  <li>input port: <code>in</code></li>
  <li>output port: <code>result</code></li>
</ul>

<h3>11.5 <code>frog.math.log10</code></h3>

<p>
Returns the base-10 logarithm of a numeric scalar input.
</p>

<ul>
  <li>input port: <code>in</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>
Domain constraints, NaN handling, and behavior on unsupported values MUST be defined by the active execution profile.
</p>

<hr/>

<h2 id="trigonometric-functions">12. Trigonometric Functions</h2>

<h3>12.1 <code>frog.math.sin</code></h3>

<p>
Returns the sine of an angle input.
</p>

<ul>
  <li>input port: <code>angle</code></li>
  <li>output port: <code>result</code></li>
</ul>

<h3>12.2 <code>frog.math.cos</code></h3>

<p>
Returns the cosine of an angle input.
</p>

<ul>
  <li>input port: <code>angle</code></li>
  <li>output port: <code>result</code></li>
</ul>

<h3>12.3 <code>frog.math.tan</code></h3>

<p>
Returns the tangent of an angle input.
</p>

<ul>
  <li>input port: <code>angle</code></li>
  <li>output port: <code>result</code></li>
</ul>

<h3>12.4 <code>frog.math.asin</code></h3>

<p>
Returns the inverse sine of a numeric scalar input.
</p>

<ul>
  <li>input port: <code>in</code></li>
  <li>output port: <code>result</code></li>
</ul>

<h3>12.5 <code>frog.math.acos</code></h3>

<p>
Returns the inverse cosine of a numeric scalar input.
</p>

<ul>
  <li>input port: <code>in</code></li>
  <li>output port: <code>result</code></li>
</ul>

<h3>12.6 <code>frog.math.atan</code></h3>

<p>
Returns the inverse tangent of a numeric scalar input.
</p>

<ul>
  <li>input port: <code>in</code></li>
  <li>output port: <code>result</code></li>
</ul>

<h3>12.7 <code>frog.math.atan2</code></h3>

<p>
Returns the two-argument inverse tangent.
</p>

<ul>
  <li>input ports: <code>y</code>, <code>x</code></li>
  <li>output port: <code>result</code></li>
</ul>

<p>
In v0.1, angle units are profile-defined.
A conforming execution profile that supports <code>frog.math</code> SHOULD make the angle convention explicit, and radians SHOULD be the default unless another convention is stated.
</p>

<hr/>

<h2 id="hyperbolic-functions">13. Hyperbolic Functions</h2>

<h3>13.1 <code>frog.math.sinh</code></h3>

<p>
Returns the hyperbolic sine of a numeric scalar input.
</p>

<ul>
  <li>input port: <code>in</code></li>
  <li>output port: <code>result</code></li>
</ul>

<h3>13.2 <code>frog.math.cosh</code></h3>

<p>
Returns the hyperbolic cosine of a numeric scalar input.
</p>

<ul>
  <li>input port: <code>in</code></li>
  <li>output port: <code>result</code></li>
</ul>

<h3>13.3 <code>frog.math.tanh</code></h3>

<p>
Returns the hyperbolic tangent of a numeric scalar input.
</p>

<ul>
  <li>input port: <code>in</code></li>
  <li>output port: <code>result</code></li>
</ul>

<hr/>

<h2 id="diagram-representation">14. Diagram Representation</h2>

<p>
Calls to <code>frog.math</code> functions are serialized as <code>primitive</code> nodes in the diagram.
</p>

<p>
Example:
</p>

<pre><code>{
  "id": "sqrt_1",
  "kind": "primitive",
  "type": "frog.math.sqrt"
}</code></pre>

<p>
Another example:
</p>

<pre><code>{
  "id": "sin_1",
  "kind": "primitive",
  "type": "frog.math.sin"
}</code></pre>

<p>
The exact port existence, direction, and typing of these nodes are resolved from this specification together with the type system and the graph rules.
</p>

<p>
When a published <code>frog.math.*</code> call reaches FIR, FIR must preserve the
public call identity, typed ports, pure value-call classification, source
attribution, and any profile-defined numeric edge policy needed by downstream
lowering. FIR must not collapse the call into a private runtime helper name,
native symbol, provider symbol, or backend-family operation unless a later
lowering stage records that specialization separately.
</p>

<hr/>

<h2 id="validation-rules">15. Validation Rules</h2>

<p>
Implementations MUST enforce the following rules:
</p>

<ul>
  <li>every <code>frog.math</code> function reference MUST identify a valid standardized <code>frog.math</code> function name,</li>
  <li>all required input ports for the referenced function MUST exist and be type-compatible,</li>
  <li>all produced output ports MUST match the function definition,</li>
  <li>all implicit coercions MUST follow <strong>Expression/Type.md</strong>,</li>
  <li>all functions in this document MUST be treated as stateless and side-effect-free.</li>
  <li>published <code>frog.math.*</code> calls MUST remain distinct from <code>frog.core.*</code> intrinsic operators in source validation and FIR,</li>
  <li>candidate or deferred namespaces such as <code>frog.numeric</code>, <code>frog.math.random</code>, <code>frog.math.interpolate</code>, and <code>frog.math.linalg</code> MUST NOT be accepted as published v0.1 primitives until exact primitive contracts are promoted.</li>
</ul>

<p>
For v0.1 scalar support:
</p>

<ul>
  <li>all functions in this document MUST operate on scalar inputs only,</li>
  <li>array, matrix, tensor, map, set, and cluster-wide lifting semantics are not defined by this document,</li>
  <li>conforming implementations that claim support for <code>frog.math</code> MUST support <code>float32</code> and <code>float64</code> for every primitive in this document.</li>
</ul>

<p>
For numeric edge cases:
</p>

<ul>
  <li>domain violations, overflow, underflow, NaN propagation, and infinity handling MUST be defined by the active execution profile,</li>
  <li>these profile-defined runtime details MUST NOT change primitive identity or source-level meaning.</li>
</ul>

<h3>15.1 Conformance Promotion Plan</h3>

<p>
The public conformance surface for math is staged as follows:
</p>

<ul>
  <li>current positive cases should verify that published flat scalar <code>frog.math.*</code> calls are accepted, remain pure value standard-library calls, derive to FIR, and preserve call identity through lowering,</li>
  <li>current negative cases should reject unknown <code>frog.math.*</code> names, missing or mismatched ports, nonscalar lifting, and premature use of candidate namespaces,</li>
  <li>future <code>frog.numeric</code> cases should cover numeric classification and explicit conversions only after the candidate surface is promoted to exact public primitive ids,</li>
  <li>future <code>frog.math.random</code> cases should require explicit random state or explicit entropy/capability inputs and reject hidden global RNG state,</li>
  <li>future <code>frog.math.interpolate</code> cases should test dimension, ordering, duplicate-sample, and out-of-range preconditions with explicit status behavior,</li>
  <li>future provider-aware <code>frog.math.linalg</code> cases should verify manifest/provider requirements and reject contracts that hide provider dependencies or turn BLAS/LAPACK-like choices into language law.</li>
</ul>

<p>
These conformance cases must remain implementation-neutral. They may require
provider or capability declarations where a future surface needs them, but they
must not name Graiphic private runtime providers, loader mechanisms, binaries,
or local paths.
</p>

<hr/>

<h2 id="examples">16. Examples</h2>

<h3>16.1 Square root</h3>

<pre><code>{
  "id": "sqrt_1",
  "kind": "primitive",
  "type": "frog.math.sqrt"
}</code></pre>

<p>
Conceptual ports:
</p>

<pre>
in → result
</pre>

<h3>16.2 Power</h3>

<pre><code>{
  "id": "pow_1",
  "kind": "primitive",
  "type": "frog.math.pow"
}</code></pre>

<p>
Conceptual ports:
</p>

<pre>
base, exponent → result
</pre>

<h3>16.3 Clamp</h3>

<pre><code>{
  "id": "clamp_1",
  "kind": "primitive",
  "type": "frog.math.clamp"
}</code></pre>

<p>
Conceptual ports:
</p>

<pre>
value, min_value, max_value → result
</pre>

<h3>16.4 Two-argument arctangent</h3>

<pre><code>{
  "id": "atan2_1",
  "kind": "primitive",
  "type": "frog.math.atan2"
}</code></pre>

<p>
Conceptual ports:
</p>

<pre>
y, x → result
</pre>

<hr/>

<h2 id="out-of-scope">17. Out of Scope for v0.1</h2>

<p>
The following are outside the strict scope of <code>frog.math</code> in v0.1:
</p>

<ul>
  <li>vectorized or broadcast semantics over arrays, matrices, or tensors,</li>
  <li><code>frog.numeric</code> as a published v0.1 primitive catalog,</li>
  <li><code>frog.math.elementary</code> as a breaking replacement for flat <code>frog.math.*</code> v0.1 names,</li>
  <li>linear algebra primitives,</li>
  <li>probability and statistics primitives,</li>
  <li>hidden global random-number state,</li>
  <li>curve fitting, optimization, and numerical solvers,</li>
  <li>signal-processing primitives,</li>
  <li>symbolic or algebraic manipulation,</li>
  <li>units-of-measure systems,</li>
  <li>special-function catalogs beyond the functions explicitly standardized here.</li>
</ul>

<hr/>

<h2 id="summary">18. Summary</h2>

<p>
The <code>frog.math</code> library defines a first standard scalar mathematics library for FROG v0.1.
</p>

<p>
It provides:
</p>

<ul>
  <li>numeric utility functions,</li>
  <li>rounding functions,</li>
  <li>power, root, exponential, and logarithmic functions,</li>
  <li>trigonometric functions,</li>
  <li>hyperbolic functions.</li>
</ul>

<p>
This library is intentionally broader than <code>frog.core</code> but still bounded.
Its purpose is to provide a durable and useful standard foundation for scalar mathematical programming in FROG while leaving richer numerical domains to future sibling libraries.
</p>

<hr/>

<p align="center">
End of FROG Math Library Specification
</p>
