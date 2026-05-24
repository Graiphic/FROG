<h1 align="center">FROG System Library Specification</h1>

<p align="center">
Definition of the standard <strong>frog.system</strong> introspection library for FROG v0.1<br/>
<em>FROG - Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#goals">2. Goals</a></li>
  <li><a href="#relation-with-other-specifications">3. Relation with Other Specifications</a></li>
  <li><a href="#role-of-frog-system">4. Role of <code>frog.system</code></a></li>
  <li><a href="#naming-and-namespace">5. Naming and Namespace</a></li>
  <li><a href="#scope-for-v01">6. Scope for v0.1</a></li>
  <li><a href="#typing-and-status-model">7. Typing and Status Model</a></li>
  <li><a href="#capability-model">8. Capability Model</a></li>
  <li><a href="#platform-functions">9. Platform Functions</a></li>
  <li><a href="#cpu-functions">10. CPU Functions</a></li>
  <li><a href="#memory-functions">11. Memory Functions</a></li>
  <li><a href="#process-functions">12. Process Functions</a></li>
  <li><a href="#environment-functions">13. Environment Functions</a></li>
  <li><a href="#capability-functions">14. Capability Functions</a></li>
  <li><a href="#diagram-representation">15. Diagram Representation</a></li>
  <li><a href="#validation-rules">16. Validation Rules</a></li>
  <li><a href="#support-and-claims">17. Support and Claims</a></li>
  <li><a href="#conformance-fixtures">18. Conformance Fixtures</a></li>
  <li><a href="#out-of-scope">19. Out of Scope for v0.1</a></li>
  <li><a href="#summary">20. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the standard <strong>frog.system</strong> library for FROG v0.1.
</p>

<p>
The <code>frog.system</code> library provides a compact introspection surface for reading
ordinary host and process information:
</p>

<ul>
  <li>platform and architecture information,</li>
  <li>CPU count and optional current aggregate CPU load,</li>
  <li>memory totals and current availability,</li>
  <li>current process identity and paths,</li>
  <li>environment variable reads,</li>
  <li>FROG capability availability checks.</li>
</ul>

<p>
These primitives read information about the active execution environment. They do not execute
commands, mutate the system, expose arbitrary platform APIs, or make OS introspection mandatory
for minimal embedded profiles.
</p>

<hr/>

<h2 id="goals">2. Goals</h2>

<ul>
  <li><strong>Portability</strong> - define stable primitive identities and port contracts for common system questions.</li>
  <li><strong>Bounded host dependence</strong> - make every host-dependent result explicit and status-bearing.</li>
  <li><strong>Capability awareness</strong> - let validation, lowering, packaging, and execution hosts see the required host capabilities.</li>
  <li><strong>Minimality</strong> - publish only small introspection primitives that fit the current type system.</li>
  <li><strong>Security discipline</strong> - avoid broad environment, process, permission, or platform enumeration surfaces.</li>
</ul>

<hr/>

<h2 id="relation-with-other-specifications">3. Relation with Other Specifications</h2>

<p>
This document complements the following specifications:
</p>

<ul>
  <li><strong>Expression/Diagram.md</strong> - defines how primitive calls are serialized as executable graph nodes.</li>
  <li><strong>Expression/Type.md</strong> - defines built-in scalar and array types used by these primitives.</li>
  <li><strong>Libraries/Core.md</strong> - defines the always-available foundational computational primitives.</li>
  <li><strong>Libraries/IO.md</strong> - defines file, path, resource, and byte-oriented I/O primitives.</li>
  <li><strong>IR/Library call model.md</strong> - defines how FIR represents host-capability and status-bearing calls without encoding private runtime recipes.</li>
  <li><strong>Profiles/Readme.md</strong> - defines optional capability-profile claims outside the intrinsic library core.</li>
</ul>

<p>
This document defines the <code>frog.system</code> primitive catalog only. It does not
redefine the source graph model, the type system, the general execution model, the FIR object
schema, host packaging, runtime scheduling, or private provider implementation.
</p>

<hr/>

<h2 id="role-of-frog-system">4. Role of <code>frog.system</code></h2>

<p>
The <code>frog.system</code> library provides host introspection primitives used by executable
diagram nodes of kind <code>primitive</code>.
</p>

<p>
In serialized diagrams, calls to these primitives use namespace-qualified identifiers such as:
</p>

<pre>
frog.system.platform_info
frog.system.cpu_logical_count
frog.system.memory_available_bytes
frog.system.environment_get
</pre>

<p>
The role of <code>frog.system</code> is to expose a small, explicit, status-bearing system
information vocabulary. It is not a general operating-system API.
</p>

<p>
The namespace <code>frog.os</code> is not published by this v0.1 specification. Future revisions
MAY add aliases or narrower OS-specific profile material, but the normative v0.1 intrinsic
library namespace is <code>frog.system.*</code>.
</p>

<hr/>

<h2 id="naming-and-namespace">5. Naming and Namespace</h2>

<p>
FROG library primitives use stable namespace-qualified identifiers:
</p>

<pre>
frog.&lt;library&gt;.&lt;primitive&gt;
</pre>

<p>
For this document:
</p>

<ul>
  <li><code>frog</code> identifies the language namespace,</li>
  <li><code>system</code> identifies the standard system introspection library,</li>
  <li>the final segment identifies the primitive name.</li>
</ul>

<p>
Primitive names in <code>frog.system</code> SHOULD use lowercase snake_case where multiple
words are needed.
</p>

<hr/>

<h2 id="scope-for-v01">6. Scope for v0.1</h2>

<p>
FROG v0.1 standardizes the following <code>frog.system</code> primitives:
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
      <td>Platform</td>
      <td><code>frog.system.platform_info</code></td>
      <td>Returns platform family, platform id, and architecture.</td>
    </tr>
    <tr>
      <td>CPU</td>
      <td><code>frog.system.cpu_logical_count</code></td>
      <td>Returns the logical CPU count visible to the host context.</td>
    </tr>
    <tr>
      <td>CPU</td>
      <td><code>frog.system.cpu_load_percent</code></td>
      <td>Returns optional current aggregate CPU load as a percentage.</td>
    </tr>
    <tr>
      <td>Memory</td>
      <td><code>frog.system.memory_total_bytes</code></td>
      <td>Returns the total memory bytes visible to the host context.</td>
    </tr>
    <tr>
      <td>Memory</td>
      <td><code>frog.system.memory_available_bytes</code></td>
      <td>Returns currently available memory bytes visible to the host context.</td>
    </tr>
    <tr>
      <td>Process</td>
      <td><code>frog.system.process_id</code></td>
      <td>Returns the current process identifier.</td>
    </tr>
    <tr>
      <td>Process</td>
      <td><code>frog.system.executable_path</code></td>
      <td>Returns the current executable path when disclosed by the host.</td>
    </tr>
    <tr>
      <td>Process</td>
      <td><code>frog.system.current_directory</code></td>
      <td>Returns the current working directory when disclosed by the host.</td>
    </tr>
    <tr>
      <td>Environment</td>
      <td><code>frog.system.environment_get</code></td>
      <td>Reads one named environment variable through host policy.</td>
    </tr>
    <tr>
      <td>Capability</td>
      <td><code>frog.system.capability_available</code></td>
      <td>Checks whether a named FROG capability token is available to the running program.</td>
    </tr>
  </tbody>
</table>

<p>
Every primitive in this document is a host-capability read. Results may vary across hosts, process
contexts, containers, sandboxes, security policy, and time.
</p>

<hr/>

<h2 id="typing-and-status-model">7. Typing and Status Model</h2>

<p>
All <code>frog.system</code> primitives are typed according to <strong>Expression/Type.md</strong>.
</p>

<p>
In v0.1, this library uses only built-in scalar types:
</p>

<ul>
  <li><code>string</code> for identifiers, paths, environment values, capability ids, and local error codes,</li>
  <li><code>bool</code> for success flags, existence flags, and availability flags,</li>
  <li><code>u64</code> for counts, byte totals, and process ids,</li>
  <li><code>f64</code> for CPU load percentage.</li>
</ul>

<p>
Until FROG standardizes a uniform error/status corridor, every primitive in this document uses a
local status tuple:
</p>

<ul>
  <li><code>success: bool</code></li>
  <li><code>error_code: string</code></li>
</ul>

<p>
This local tuple is not the final FROG-wide error/status contract. It is the v0.1
<code>frog.system</code> status shape and should be represented in FIR as an explicit
<code>local_result_tuple</code> status model.
</p>

<p>
Rules:
</p>

<ul>
  <li>if <code>success = true</code>, <code>error_code</code> MUST be the empty string,</li>
  <li>if <code>success = false</code>, every data output MUST still be present, well-typed, and set to its fallback value,</li>
  <li>empty string outputs fall back to <code>""</code>,</li>
  <li><code>u64</code> outputs fall back to <code>0</code>,</li>
  <li><code>f64</code> outputs fall back to <code>0.0</code>,</li>
  <li><code>bool</code> data outputs fall back to <code>false</code>.</li>
</ul>

<p>
Standard local error codes are:
</p>

<ul>
  <li><code>unsupported</code> - the implementation or active execution profile does not support the requested primitive or capability,</li>
  <li><code>permission_denied</code> - host policy denies the read,</li>
  <li><code>host_policy_blocked</code> - a sandbox, embedded host, deployment policy, or capability boundary blocks the read,</li>
  <li><code>unavailable</code> - the information is not available from the host at execution time,</li>
  <li><code>invalid_input</code> - an input such as an environment variable name or capability id is invalid for the primitive,</li>
  <li><code>unknown_error</code> - the host failed the read without a more specific standardized code.</li>
</ul>

<p>
Implementations MAY provide richer diagnostics outside this source-visible primitive contract, but those
diagnostics MUST NOT change the standardized ports or fallback behavior.
</p>

<hr/>

<h2 id="capability-model">8. Capability Model</h2>

<p>
The <code>frog.system</code> library is OS-dependent and runtime-hosted. Validation, FIR,
lowering, backend contracts, and deployment packaging SHOULD preserve the required host capability
for each call.
</p>

<p>
The standard capability tokens for this library are:
</p>

<table>
  <thead>
    <tr>
      <th>Capability token</th>
      <th>Required by</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>frog.system.platform.read</code></td>
      <td><code>platform_info</code></td>
    </tr>
    <tr>
      <td><code>frog.system.cpu.read</code></td>
      <td><code>cpu_logical_count</code>, <code>cpu_load_percent</code></td>
    </tr>
    <tr>
      <td><code>frog.system.memory.read</code></td>
      <td><code>memory_total_bytes</code>, <code>memory_available_bytes</code></td>
    </tr>
    <tr>
      <td><code>frog.system.process.read</code></td>
      <td><code>process_id</code>, <code>executable_path</code>, <code>current_directory</code></td>
    </tr>
    <tr>
      <td><code>frog.system.environment.read</code></td>
      <td><code>environment_get</code></td>
    </tr>
    <tr>
      <td><code>frog.system.capability.query</code></td>
      <td><code>capability_available</code></td>
    </tr>
  </tbody>
</table>

<p>
Capability tokens are public FROG capability ids. They are not operating-system permission names,
native API identifiers, private runtime service names, or security principal names.
</p>

<p>
A minimal embedded implementation MAY reject <code>frog.system</code> primitives as unsupported or
may support only an explicitly claimed subset. It MUST NOT claim full <code>frog.system</code> support
while silently substituting invented values.
</p>

<hr/>

<h2 id="platform-functions">9. Platform Functions</h2>

<h3>9.1 <code>frog.system.platform_info</code></h3>

<p>
Returns basic platform information visible to the running program.
</p>

<ul>
  <li>input ports: none</li>
  <li>output ports: <code>os_family</code>, <code>platform_id</code>, <code>architecture</code>, <code>success</code>, <code>error_code</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>os_family: string</code></li>
  <li><code>platform_id: string</code></li>
  <li><code>architecture: string</code></li>
  <li><code>success: bool</code></li>
  <li><code>error_code: string</code></li>
</ul>

<p>
Rules:
</p>

<ul>
  <li><code>os_family</code> SHOULD use one of <code>windows</code>, <code>macos</code>, <code>linux</code>, <code>unix</code>, <code>wasi</code>, <code>embedded</code>, or <code>unknown</code> when <code>success = true</code>,</li>
  <li><code>platform_id</code> SHOULD be a stable lowercase platform identifier supplied by the active execution profile,</li>
  <li><code>architecture</code> SHOULD use a normalized lowercase architecture identifier such as <code>x86_64</code>, <code>aarch64</code>, <code>arm</code>, <code>x86</code>, <code>wasm32</code>, or <code>unknown</code>,</li>
  <li>when <code>success = false</code>, all string data outputs MUST be the empty string.</li>
</ul>

<hr/>

<h2 id="cpu-functions">10. CPU Functions</h2>

<h3>10.1 <code>frog.system.cpu_logical_count</code></h3>

<p>
Returns the logical CPU count visible to the active host context.
</p>

<ul>
  <li>input ports: none</li>
  <li>output ports: <code>count</code>, <code>success</code>, <code>error_code</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>count: u64</code></li>
  <li><code>success: bool</code></li>
  <li><code>error_code: string</code></li>
</ul>

<p>
The count is the execution context's view, not necessarily the physical machine's total processor
topology. Container, sandbox, scheduler, or deployment policy limits MAY affect the value.
</p>

<h3>10.2 <code>frog.system.cpu_load_percent</code></h3>

<p>
Returns the current aggregate CPU load percentage visible to the active host context.
</p>

<ul>
  <li>input ports: none</li>
  <li>output ports: <code>load_percent</code>, <code>success</code>, <code>error_code</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>load_percent: f64</code></li>
  <li><code>success: bool</code></li>
  <li><code>error_code: string</code></li>
</ul>

<p>
Rules:
</p>

<ul>
  <li>when <code>success = true</code>, <code>load_percent</code> SHOULD be in the inclusive range <code>0.0</code> through <code>100.0</code>,</li>
  <li>the sampling window and host measurement source are implementation-defined unless a future profile constrains them,</li>
  <li>if the host cannot provide a meaningful current aggregate load, <code>success</code> MUST be <code>false</code>.</li>
</ul>

<p>
This primitive is for coarse introspection only. It is not a profiling, scheduling, benchmarking,
or real-time load-control primitive.
</p>

<hr/>

<h2 id="memory-functions">11. Memory Functions</h2>

<h3>11.1 <code>frog.system.memory_total_bytes</code></h3>

<p>
Returns the total memory bytes visible to the active host context.
</p>

<ul>
  <li>input ports: none</li>
  <li>output ports: <code>bytes</code>, <code>success</code>, <code>error_code</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>bytes: u64</code></li>
  <li><code>success: bool</code></li>
  <li><code>error_code: string</code></li>
</ul>

<p>
The value is the host context's memory view. It may represent a sandbox, container, quota, or other
bounded environment rather than physical machine memory.
</p>

<h3>11.2 <code>frog.system.memory_available_bytes</code></h3>

<p>
Returns the currently available memory bytes visible to the active host context.
</p>

<ul>
  <li>input ports: none</li>
  <li>output ports: <code>bytes</code>, <code>success</code>, <code>error_code</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>bytes: u64</code></li>
  <li><code>success: bool</code></li>
  <li><code>error_code: string</code></li>
</ul>

<p>
The value is a momentary host observation. It may change between calls and is not a memory reservation.
</p>

<hr/>

<h2 id="process-functions">12. Process Functions</h2>

<h3>12.1 <code>frog.system.process_id</code></h3>

<p>
Returns the current process identifier visible to the host.
</p>

<ul>
  <li>input ports: none</li>
  <li>output ports: <code>pid</code>, <code>success</code>, <code>error_code</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>pid: u64</code></li>
  <li><code>success: bool</code></li>
  <li><code>error_code: string</code></li>
</ul>

<p>
The identifier is meaningful only inside the active host and profile context. It is not a stable
cross-machine, cross-container, or cross-run identity.
</p>

<h3>12.2 <code>frog.system.executable_path</code></h3>

<p>
Returns the current executable path when disclosed by the host.
</p>

<ul>
  <li>input ports: none</li>
  <li>output ports: <code>path</code>, <code>success</code>, <code>error_code</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>path: string</code></li>
  <li><code>success: bool</code></li>
  <li><code>error_code: string</code></li>
</ul>

<p>
Path syntax follows the active execution profile, as with <code>frog.io</code>.
</p>

<h3>12.3 <code>frog.system.current_directory</code></h3>

<p>
Returns the current working directory when disclosed by the host.
</p>

<ul>
  <li>input ports: none</li>
  <li>output ports: <code>path</code>, <code>success</code>, <code>error_code</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>path: string</code></li>
  <li><code>success: bool</code></li>
  <li><code>error_code: string</code></li>
</ul>

<p>
This primitive reads the host's current directory for the running program. It does not change the
current directory and does not define any filesystem mutation behavior.
</p>

<hr/>

<h2 id="environment-functions">13. Environment Functions</h2>

<h3>13.1 <code>frog.system.environment_get</code></h3>

<p>
Reads one named environment variable through the active host policy.
</p>

<ul>
  <li>input port: <code>name</code></li>
  <li>output ports: <code>value</code>, <code>exists</code>, <code>success</code>, <code>error_code</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>name: string</code></li>
  <li><code>value: string</code></li>
  <li><code>exists: bool</code></li>
  <li><code>success: bool</code></li>
  <li><code>error_code: string</code></li>
</ul>

<p>
Rules:
</p>

<ul>
  <li>if the named variable exists and is permitted, <code>success = true</code>, <code>exists = true</code>, and <code>value</code> contains the value,</li>
  <li>if the named variable does not exist but the read itself is permitted, <code>success = true</code>, <code>exists = false</code>, and <code>value</code> is the empty string,</li>
  <li>if the read is denied, blocked, unsupported, or otherwise fails, <code>success = false</code>, <code>exists = false</code>, and <code>value</code> is the empty string,</li>
  <li>empty variable names are invalid and MUST produce <code>success = false</code> with <code>error_code = "invalid_input"</code>.</li>
</ul>

<p>
This primitive does not enumerate environment variables, write environment variables, expand shell
syntax, or define secret-handling policy. Hosts MAY redact or block environment reads according to
capability policy.
</p>

<hr/>

<h2 id="capability-functions">14. Capability Functions</h2>

<h3>14.1 <code>frog.system.capability_available</code></h3>

<p>
Checks whether a named public FROG capability token is available to the running program.
</p>

<ul>
  <li>input port: <code>capability</code></li>
  <li>output ports: <code>available</code>, <code>success</code>, <code>error_code</code></li>
</ul>

<p>
Types:
</p>

<ul>
  <li><code>capability: string</code></li>
  <li><code>available: bool</code></li>
  <li><code>success: bool</code></li>
  <li><code>error_code: string</code></li>
</ul>

<p>
Rules:
</p>

<ul>
  <li>the input is a public FROG capability token such as <code>frog.system.environment.read</code>,</li>
  <li>the primitive MUST NOT enumerate all capabilities or disclose host-private policy tables,</li>
  <li>unknown or malformed capability ids MUST produce <code>success = false</code> and <code>available = false</code>,</li>
  <li>if the host can answer the query, <code>success = true</code> and <code>available</code> states whether the capability is available to this program,</li>
  <li>if the host blocks capability querying, <code>success = false</code> and <code>available = false</code>.</li>
</ul>

<p>
This primitive is a FROG capability query, not a generic operating-system permission probe.
</p>

<hr/>

<h2 id="diagram-representation">15. Diagram Representation</h2>

<p>
Calls to <code>frog.system</code> primitives are serialized as <code>primitive</code> nodes in the
diagram.
</p>

<p>
Examples:
</p>

<pre><code>{
  "id": "platform_1",
  "kind": "primitive",
  "type": "frog.system.platform_info"
}</code></pre>

<pre><code>{
  "id": "env_1",
  "kind": "primitive",
  "type": "frog.system.environment_get"
}</code></pre>

<p>
The exact port existence, direction, typing, status posture, and capability requirement of these
nodes are resolved from this specification together with the type system, graph rules, and FIR
library-call model.
</p>

<hr/>

<h2 id="validation-rules">16. Validation Rules</h2>

<p>
Implementations that support <code>frog.system</code> MUST enforce the following rules:
</p>

<ul>
  <li>every <code>frog.system</code> primitive reference MUST identify a valid standardized primitive name from this document,</li>
  <li>all required input ports MUST exist and be type-compatible,</li>
  <li>all produced output ports MUST match the primitive definition,</li>
  <li>every primitive MUST expose <code>success</code> and <code>error_code</code> outputs exactly as defined here,</li>
  <li>host capability requirements MUST remain explicit in FIR, lowering, backend contracts, manifests, or comparable downstream representation where those layers are present,</li>
  <li>unsupported <code>frog.system</code> use MUST be rejected or reported as unsupported rather than guessed from private runtime behavior.</li>
</ul>

<p>
For FIR representation, <code>frog.system</code> calls SHOULD use:
</p>

<ul>
  <li><code>call_class = "host_capability"</code>,</li>
  <li><code>effect = "host_capability_read"</code>,</li>
  <li><code>status_model = "local_result_tuple"</code>,</li>
  <li>the relevant <code>frog.system.*</code> capability token from this document.</li>
</ul>

<hr/>

<h2 id="support-and-claims">17. Support and Claims</h2>

<p>
Support for <code>frog.system</code> is optional for minimal embedded profiles and for
implementations that intentionally provide only pure computation or constrained execution.
</p>

<p>
An implementation MAY make one of the following claims:
</p>

<ul>
  <li><code>core FROG only</code> - no <code>frog.system</code> support,</li>
  <li><code>core FROG + frog.system full v0.1</code> - recognizes and implements the full primitive catalog from this document, subject to runtime host policy,</li>
  <li><code>core FROG + frog.system partial v0.1</code> - supports an explicit subset of primitives or capabilities.</li>
</ul>

<p>
A partial implementation MUST identify the supported subset. It MUST NOT present unsupported
introspection values as successful invented defaults.
</p>

<p>
Even under a full support claim, individual calls MAY produce <code>success = false</code> when a
sandbox, deployment host, permission boundary, or current process context denies or cannot provide
the requested information.
</p>

<hr/>

<h2 id="conformance-fixtures">18. Conformance Fixtures</h2>

<p>
The public conformance corpus includes initial <code>frog.system</code> fixture
records under the compiler corridor:
</p>

<ul>
  <li><a href="../Conformance/valid/compiler/06_system_library_call_preserves_capability_and_local_status.md"><code>valid/compiler/06_system_library_call_preserves_capability_and_local_status</code></a> covers valid host-capability reads, local status tuple preservation, and public capability-token preservation.</li>
  <li><a href="../Conformance/invalid/compiler/10_system_library_unknown_or_malformed_call_is_rejected.md"><code>invalid/compiler/10_system_library_unknown_or_malformed_call_is_rejected</code></a> covers unknown primitive ids, missing or mistyped ports, empty environment names, and malformed capability ids.</li>
  <li><a href="../Conformance/invalid/compiler/11_system_library_capability_or_subset_must_report_explicitly.md"><code>invalid/compiler/11_system_library_capability_or_subset_must_report_explicitly</code></a> covers unsupported, denied, blocked, unavailable, or partial-support host-capability posture.</li>
</ul>

<p>
These cases are conformance-facing records. They do not implement providers,
choose private runtime APIs, or expand the <code>frog.system</code> v0.1 primitive
catalog.
</p>

<hr/>

<h2 id="out-of-scope">19. Out of Scope for v0.1</h2>

<p>
The following are outside the strict scope of <code>frog.system</code> in v0.1:
</p>

<ul>
  <li>process creation, command execution, shell execution, or process termination,</li>
  <li>process enumeration, thread enumeration, loaded-module enumeration, or arbitrary process inspection,</li>
  <li>file reads, file writes, path mutation, stream handles, or filesystem watchers,</li>
  <li>network interfaces, sockets, HTTP, service discovery, or remote host probing,</li>
  <li>database access, foreign-function calls, Python/.NET/native interop, Registry access, COM, ActiveX, or other external integration surfaces,</li>
  <li>wall-clock time, monotonic time, timers, waits, profiling timers, or scheduler control,</li>
  <li>hardware sensors, device inventory, drivers, DAQ, serial ports, GPUs, batteries, or power-management APIs,</li>
  <li>user identity, groups, credentials, secrets, tokens, keychains, or permission enumeration,</li>
  <li>environment variable enumeration, mutation, shell expansion, or secret classification,</li>
  <li>host-private runtime service discovery or implementation-specific provider names,</li>
  <li>a universal structured error object or FROG-wide error/status corridor.</li>
</ul>

<hr/>

<h2 id="summary">20. Summary</h2>

<p>
The <code>frog.system</code> library defines a minimal standard system introspection surface for
FROG v0.1.
</p>

<p>
It provides:
</p>

<ul>
  <li>platform and architecture information,</li>
  <li>CPU count and optional current load,</li>
  <li>memory totals and current availability,</li>
  <li>current process id, executable path, and current directory,</li>
  <li>single-variable environment reads,</li>
  <li>bounded FROG capability availability checks.</li>
</ul>

<p>
The surface is intentionally status-bearing, capability-aware, and read-only. It does not turn the
operating system, one runtime, one host policy, or one private implementation strategy into FROG
language truth.
</p>

<hr/>

<p align="center">
End of FROG System Library Specification
</p>
