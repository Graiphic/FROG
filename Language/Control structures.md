<p align="center">
  <img src="../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">🐸 FROG Control Structures Specification</h1>

<p align="center">
Normative execution semantics for control structures in FROG programs<br/>
<em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#goals">2. Goals</a></li>
  <li><a href="#relation-with-other-specifications">3. Relation with Other Specifications</a></li>
  <li><a href="#functions-vs-control-structures">4. Functions vs Control Structures</a></li>
  <li><a href="#scope-for-v01">5. Scope for v0.1</a></li>
  <li><a href="#standard-control-structures-for-v01">6. Standard Control Structures for v0.1</a></li>
  <li><a href="#semantic-execution-model">7. Semantic Execution Model</a></li>
  <li><a href="#case-structure-semantics">8. Case Structure Semantics</a></li>
  <li><a href="#for-loop-structure-semantics">9. For Loop Structure Semantics</a></li>
  <li><a href="#while-loop-structure-semantics">10. While Loop Structure Semantics</a></li>
  <li><a href="#event-structure-semantics">11. Event Structure Semantics</a></li>
  <li><a href="#loop-output-semantics">12. Loop Output Semantics</a></li>
  <li><a href="#scheduler-independence">13. Scheduler Independence</a></li>
  <li><a href="#relation-with-local-memory-and-cycles">14. Relation with Local Memory and Cycles</a></li>
  <li><a href="#validation-rules">15. Validation Rules</a></li>
  <li><a href="#examples">16. Examples</a></li>
  <li><a href="#out-of-scope-for-v01">17. Out of Scope for v0.1</a></li>
  <li><a href="#summary">18. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document defines the normative execution semantics of control structures in FROG.
A control structure is a language-level executable construct whose meaning cannot be reduced
to an ordinary primitive call.
</p>

<p>
In FROG v0.1, control structures govern:
</p>

<ul>
  <li>selection of exactly one executable region among several candidates,</li>
  <li>counted repetition of one executable region,</li>
  <li>condition-governed repetition of one executable region,</li>
  <li>dispatch of exactly one region for a declared event or timeout.</li>
</ul>

<p>
This document defines what validated control structures mean at execution time.
It does not own the canonical serialized source representation of structure nodes, boundaries,
terminals, or regions. Those source-facing topics belong to <code>Expression/</code>.
</p>

<p>
FROG v0.1 keeps concrete control forms explicit. Therefore, the language standardizes
<code>case</code>, <code>for_loop</code>, <code>while_loop</code>, <code>event_structure</code>, <code>disabled_structure</code>, and <code>conditional_disable_structure</code> as distinct semantic
families rather than collapsing them into one generic hidden form.
</p>

<p>
In FROG, boolean conditional branching and multi-branch textual selection are both represented
by the same semantic family: <code>case</code>. A boolean <code>case</code> is the base v0.1
semantic equivalent of a traditional <code>if / else</code>. There is no separate standardized
<code>if</code> structure family in base v0.1.
</p>

<p>
More generally, authoring-facing labels such as <em>If</em>, <em>If / Else</em>, <em>Else If</em>,
or <em>Switch</em> do not define additional semantic families by themselves.
When such views are exposed by tooling, they remain authoring projections of the standardized
control-structure families defined here.
</p>

<hr/>

<h2 id="goals">2. Goals</h2>

<ul>
  <li><strong>Clarity</strong> — define what standardized control structures mean at execution time.</li>
  <li><strong>Determinism</strong> — require stable observable meaning independent of backend scheduling details.</li>
  <li><strong>Structural explicitness</strong> — preserve the distinction between structural control and ordinary primitive evaluation.</li>
  <li><strong>Architectural cleanliness</strong> — keep semantic ownership in <code>Language/</code> while preserving source ownership in <code>Expression/</code>.</li>
  <li><strong>Compatibility with dataflow</strong> — define control behavior without abandoning the dataflow execution model.</li>
  <li><strong>Extensibility</strong> — allow future control-structure families without redefining the semantic foundations.</li>
</ul>

<hr/>

<h2 id="relation-with-other-specifications">3. Relation with Other Specifications</h2>

<p>
This document complements the following specifications:
</p>

<ul>
  <li><code>Language/Readme.md</code> — defines the role of <code>Language/</code> as the normative execution-semantics layer.</li>
  <li><code>Expression/Control structures.md</code> — defines the canonical source representation of structure nodes, boundaries, terminals, regions, and source-facing validation shape.</li>
  <li><code>Expression/Diagram.md</code> — defines the executable graph structure, node kinds, diagram scopes, and graph-level source representation.</li>
  <li><code>Expression/Type.md</code> — defines type syntax and compatibility.</li>
  <li><code>Expression/State and cycles.md</code> — defines the source-facing representation of explicit local memory and cycle-facing source constraints.</li>
  <li><code>Language/State and cycles.md</code> — defines the normative validity rule for cyclic graphs and the semantic meaning of explicit local memory.</li>
  <li><code>Libraries/Core.md</code> — defines ordinary built-in functions such as <code>frog.core.add</code> and <code>frog.core.delay</code>.</li>
  <li><code>IDE/Readme.md</code> — defines the Program Model and the authoring boundary between editable views and canonical source.</li>
  <li><code>IDE/Palette.md</code> — defines how structures are presented to users in authoring environments.</li>
</ul>

<p>
Ownership is intentionally split as follows:
</p>

<ul>
  <li><code>Expression/</code> owns the canonical source representation of structure nodes, boundaries, terminals, and regions,</li>
  <li><code>Language/</code> owns the normative execution meaning of the standardized structure families,</li>
  <li><code>Libraries/</code> owns ordinary primitive identity and primitive-local semantics used inside structure regions,</li>
  <li><code>IDE/</code> owns authoring, visualization, observability, and debugging behavior.</li>
</ul>

<p>
When both <code>Expression/Control structures.md</code> and this document discuss the same structure family,
they MUST remain aligned. However, ownership remains separated between source representation and execution semantics.
</p>

<p>
When a conflict exists:
</p>

<ul>
  <li><code>Expression/</code> remains authoritative for canonical source form,</li>
  <li><code>Language/</code> remains authoritative for normative execution semantics.</li>
</ul>

<p>
Derived authoring forms exposed by IDE tooling MUST NOT be interpreted as new semantic families
unless and until such families are explicitly standardized by <code>Language/</code>.
</p>

<hr/>

<h2 id="functions-vs-control-structures">4. Functions vs Control Structures</h2>

<p>
FROG distinguishes between:
</p>

<ul>
  <li><strong>functions</strong> — callable primitive operations such as <code>frog.core.add</code> or <code>frog.core.delay</code>,</li>
  <li><strong>control structures</strong> — language-level structural constructs such as <code>case</code>, <code>for_loop</code>, <code>while_loop</code>, <code>event_structure</code>, <code>disabled_structure</code>, and <code>conditional_disable_structure</code>.</li>
</ul>

<p>
A function is evaluated according to its primitive signature and primitive semantics.
A control structure governs one or more owned executable regions and defines how those regions are selected,
repeated, or terminated.
</p>

<p>
Example:
</p>

<ul>
  <li><code>frog.core.select</code> is a primitive that selects one value among inputs,</li>
  <li>a <code>case</code> structure selects one executable region among multiple branches.</li>
</ul>

<p>
This distinction is normative and fundamental.
A control structure is part of the language model itself and MUST NOT be treated as a disguised library call.
</p>

<hr/>

<h2 id="scope-for-v01">5. Scope for v0.1</h2>

<p>
FROG v0.1 standardizes the following control structures:
</p>

<ul>
  <li><code>case</code></li>
  <li><code>for_loop</code></li>
  <li><code>while_loop</code></li>
  <li><code>event_structure</code></li>
  <li><code>disabled_structure</code></li>
  <li><code>conditional_disable_structure</code></li>
</ul>

<p>
FROG v0.1 does not standardize general timed structures, parallel-region structures,
pattern-match structures beyond the base case forms, or exception-handling structures.
Those MAY be introduced later as distinct semantic families.
</p>

<hr/>

<h2 id="standard-control-structures-for-v01">6. Standard Control Structures for v0.1</h2>

<p>
The standardized control-structure families for FROG v0.1 are:
</p>

<ul>
  <li><code>case</code></li>
  <li><code>for_loop</code></li>
  <li><code>while_loop</code></li>
  <li><code>event_structure</code></li>
  <li><code>disabled_structure</code></li>
  <li><code>conditional_disable_structure</code></li>
</ul>

<p>
These are language structures, not library functions.
Tools SHOULD present them as dedicated structural elements in the diagram editor.
</p>

<p>
For usability, tools MAY present a boolean <code>case</code> as <em>If / Else</em> in the UI.
More generally, tools MAY expose authoring-facing derived forms such as <em>If</em>, <em>If / Else</em>,
<em>Else If</em>, or <em>Switch</em>, provided that those views preserve the standardized semantic family.
However, the standardized family remains <code>case</code>.
</p>

<p>
Accordingly:
</p>

<ul>
  <li>base v0.1 standardizes <code>case</code>, not a separate <code>if</code> family,</li>
  <li>base v0.1 standardizes <code>case</code>, not a separate <code>switch</code> family,</li>
  <li>authoring convenience MUST NOT fragment the language into multiple semantic families with equivalent meaning.</li>
</ul>

<hr/>

<h2 id="semantic-execution-model">7. Semantic Execution Model</h2>

<p>
This document defines the observable execution meaning of validated control structures.
It does not redefine the serialized source fields that identify structure nodes.
</p>

<p>
For the purposes of this document, a <strong>structure activation</strong> is one semantic participation
of a validated structure instance in program execution.
The exact internal scheduler implementation may vary across runtimes,
but the observable result of a structure activation MUST remain semantically equivalent.
</p>

<p>
At a high level:
</p>

<ul>
  <li>a <code>case</code> activation executes exactly one selected region,</li>
  <li>a <code>for_loop</code> activation executes one body region exactly <code>N</code> times for some resolved non-negative count <code>N</code>,</li>
  <li>a <code>while_loop</code> activation executes one body region one or more times according to the standardized continuation rule.</li>
</ul>

<p>
A control structure remains part of the dataflow graph.
It does not introduce arbitrary instruction ordering outside the standardized rules defined here.
Internal execution inside a selected or repeated region remains constrained by ordinary dataflow meaning.
</p>

<p>
Derived authoring views MAY change how a structure is edited or displayed.
They MUST NOT change the semantic meaning of a validated structure activation.
</p>

<hr/>

<h2 id="case-structure-semantics">8. Case Structure Semantics</h2>

<h3>8.1 Purpose</h3>

<p>
A <code>case</code> structure selects exactly one executable region for one activation.
Non-selected regions do not execute for that activation.
</p>

<h3>8.2 Selector categories for v0.1</h3>

<p>
In base v0.1, the standardized selector categories are:
</p>

<ul>
  <li><code>bool</code></li>
  <li><code>string</code></li>
</ul>

<p>
No other selector category is standardized by base v0.1.
</p>

<h3>8.3 Derived authoring forms do not create new semantics</h3>

<p>
A tool MAY expose user-facing views such as <em>If</em>, <em>If / Else</em>, <em>Else If</em>, or <em>Switch</em>.
Those views do not introduce independent semantic families in base v0.1.
They remain authoring projections of the standardized <code>case</code> family.
</p>

<p>
Accordingly:
</p>

<ul>
  <li><em>If</em> and <em>If / Else</em> map to boolean <code>case</code> semantics,</li>
  <li><em>Else If</em> remains a derived authoring form rather than a distinct standardized family,</li>
  <li><em>Switch</em> remains a derived authoring form over exact-match <code>case</code> semantics in the selector categories standardized by base v0.1.</li>
</ul>

<h3>8.4 Selector resolution</h3>

<p>
For each <code>case</code> activation, the selector value MUST be resolved and one branch-selection rule MUST be applied.
Branch selection MUST be deterministic for a given validated source structure and a given selector value.
</p>

<h3>8.5 Boolean case semantics</h3>

<p>
A boolean <code>case</code> is the base v0.1 semantic equivalent of a classic <code>if / else</code>.
</p>

<p>
If the selector category is <code>bool</code>:
</p>

<ul>
  <li>exactly two executable regions are semantically relevant,</li>
  <li>the <code>true</code> region MUST execute when the selector value is <code>true</code>,</li>
  <li>the <code>false</code> region MUST execute when the selector value is <code>false</code>,</li>
  <li>no third branch and no default branch participate in base v0.1 boolean-case semantics.</li>
</ul>

<p>
A tool MAY visually minimize or present one branch differently for authoring purposes.
That presentation MUST NOT redefine which branch is semantically selected.
</p>

<h3>8.6 String case semantics</h3>

<p>
If the selector category is <code>string</code>, the structure performs exact string-based branch selection.
</p>

<ul>
  <li>if the selector value exactly matches one declared string branch, that matching branch MUST execute,</li>
  <li>otherwise, the unique default branch MUST execute,</li>
  <li>branch selection MUST be deterministic and MUST NOT depend on branch ordering heuristics.</li>
</ul>

<p>
A tool MAY present such a structure as a <em>Switch</em>-like form in the UI.
That presentation does not create a distinct standardized semantic family.
</p>

<h3>8.7 Outputs</h3>

<p>
For one <code>case</code> activation:
</p>

<ul>
  <li>only the selected region contributes region-local execution for that activation,</li>
  <li>the structure outputs are the outputs produced by the selected region for that activation,</li>
  <li>non-selected regions MUST NOT produce competing outputs for that activation.</li>
</ul>

<h3>8.8 No implicit merging semantics beyond the selected branch</h3>

<p>
A <code>case</code> structure does not execute multiple branches and later merge them.
Its meaning is single-branch execution per activation.
</p>

<h3>8.9 Future extensibility</h3>

<p>
Future revisions or stricter profiles MAY add selector categories such as integers, enums, or pattern-oriented matching.
Such extensions MUST preserve deterministic branch selection and MUST remain compatible with the ownership split between
<code>Expression/</code> and <code>Language/</code>.
</p>

<hr/>

<h2 id="for-loop-structure-semantics">9. For Loop Structure Semantics</h2>

<h3>9.1 Purpose</h3>

<p>
A <code>for_loop</code> governs counted repetition of one body region.
</p>

<h3>9.2 Counted iteration</h3>

<p>
For each <code>for_loop</code> activation, the loop count MUST resolve to a non-negative integer value.
In base v0.1, the standardized count type is <code>i64</code>.
</p>

<p>
If the resolved count is <code>N</code>:
</p>

<ul>
  <li>if <code>N = 0</code>, the body region executes zero times,</li>
  <li>if <code>N &gt; 0</code>, the body region executes exactly <code>N</code> times.</li>
</ul>

<p>
Negative counts are invalid in base v0.1.
</p>

<h3>9.3 Iteration index</h3>

<p>
In base v0.1, the standardized loop index progresses deterministically as:
</p>

<pre><code>0, 1, 2, ..., N - 1</code></pre>

<p>
for the successive body activations of one <code>for_loop</code> activation with resolved count <code>N &gt; 0</code>.
</p>

<h3>9.4 Region semantics</h3>

<p>
A <code>for_loop</code> governs one repeated body region.
That body region executes once per iteration and remains subject to ordinary dataflow meaning during each iteration.
</p>

<h3>9.5 Completion</h3>

<p>
A <code>for_loop</code> activation completes after the required number of iterations has completed
and after loop outputs have been resolved according to the standardized loop-output rules.
</p>

<h3>9.6 Distinction from ordinary functions</h3>

<p>
A <code>for_loop</code> is not semantically reducible to one ordinary primitive call.
It governs repeated execution of a structural region.
</p>

<hr/>

<h2 id="while-loop-structure-semantics">10. While Loop Structure Semantics</h2>

<h3>10.1 Purpose</h3>

<p>
A <code>while_loop</code> governs condition-driven repetition of one body region.
</p>

<h3>10.2 Standardized continuation rule for v0.1</h3>

<p>
Base v0.1 standardizes a continue-while-true rule with post-test loop behavior:
</p>

<pre><code>execute body once
resolve condition
continue while condition is true</code></pre>

<p>
Therefore, a base v0.1 <code>while_loop</code> activation executes the body at least once.
A standardized pre-test while variant is not part of base v0.1.
</p>

<h3>10.3 Condition resolution</h3>

<p>
After each completed body activation, the continuation condition MUST resolve to a boolean value.
</p>

<ul>
  <li>if the resolved condition is <code>true</code>, another body activation MUST occur,</li>
  <li>if the resolved condition is <code>false</code>, the loop MUST terminate.</li>
</ul>

<h3>10.4 Iteration index</h3>

<p>
In base v0.1, the standardized loop index for <code>while_loop</code> progresses deterministically
from <code>0</code> for the first body activation and increments by <code>1</code> for each further iteration.
</p>

<h3>10.5 Region semantics</h3>

<p>
A <code>while_loop</code> governs one repeated body region.
That body region remains subject to ordinary dataflow meaning during each iteration.
</p>

<h3>10.6 Completion</h3>

<p>
A <code>while_loop</code> activation completes only after one body activation has occurred,
the continuation condition has resolved to <code>false</code>, and loop outputs have been resolved
according to the standardized loop-output rules.
</p>

<h3>10.7 Distinction from for loop</h3>

<p>
A <code>while_loop</code> is semantically distinct from a <code>for_loop</code>.
A <code>for_loop</code> communicates counted repetition.
A <code>while_loop</code> communicates condition-governed repetition under the standardized post-test rule.
Both remain distinct structure families in v0.1.
</p>

<hr/>

<h2 id="event-structure-semantics">11. Event Structure Semantics</h2>

<h3>11.1 Purpose</h3>

<p>
An <code>event_structure</code> waits for one of its declared event occurrences and
executes exactly one matching event-case region. Event labels are authoring text;
selection is determined only by each region's canonical <code>event</code> descriptor.
</p>

<h3>11.2 Timeout resolution</h3>

<p>
The <code>timeout</code> terminal resolves to an <code>i64</code> number of milliseconds
before waiting begins. A value of <code>-1</code> disables timeout generation, a
non-negative value defines the timeout duration, and values below <code>-1</code> are
invalid. Exactly one region MUST declare <code>event.kind: "timeout"</code>.
</p>

<p>
If a matching non-timeout event occurs at or before the computed timeout deadline,
that event wins. Otherwise, expiration generates the timeout occurrence. This rule
keeps the selected region independent from scheduler polling intervals.
</p>

<h3>11.3 Event matching</h3>

<ul>
  <li><code>value_change</code> regions MUST identify their source.</li>
  <li><code>user</code> regions MAY identify a source when the event producer is scoped.</li>
  <li>Non-timeout descriptor pairs of <code>kind</code> and <code>source</code> MUST be unique within the structure.</li>
  <li>One occurrence MUST activate exactly one matching region.</li>
</ul>

<h3>11.4 Event Data</h3>

<p>
Before the selected region executes, the runtime MUST project its Event Data fields
as read-only values: <code>source</code> and <code>type</code> are <code>string</code>, and
<code>time</code> is <code>u64</code>. A region MAY expose only a selected subset, but
every selected field id MUST come from the public Event Data field catalogue.
</p>

<h3>11.5 Completion</h3>

<p>
An event-structure activation completes after the selected event-case region and
its boundary outputs complete. A later event occurrence begins a new activation;
it MUST NOT cause a second region to execute in the already completed activation.
</p>

<h2 id="disabled-structure-semantics">11.6 Diagram Disable Structure Semantics</h2>

<p>
A <code>disabled_structure</code> retains one Enabled region and one Disabled
region but activates only the Enabled region. The Disabled region MUST NOT
schedule nodes, perform effects, publish boundary outputs, or otherwise alter
runtime behavior. Retaining and displaying its source is an authoring concern.
</p>

<p>
The structure has no selector, condition, or other family-specific terminal.
Changing the region visible in an editor MUST NOT change which region executes.
Boundary-output completeness is evaluated against the Enabled region only.
Syntax, schema, and local type validation MAY still diagnose malformed retained
source inside the Disabled region without making that region executable.
</p>

<h2 id="conditional-disable-semantics">11.7 Conditional Disable Semantics</h2>

<p>
A <code>conditional_disable_structure</code> resolves exactly one owned region
before runtime from a Compilation Context of case-sensitive string symbols.
Built-in symbols form the base context, Project symbols replace matching
Built-in names, and Target symbols replace matching Project names. Runtime
data, controls, wires, and enumerators MUST NOT modify this selection.
</p>

<p>
Condition regions are evaluated in canonical source order. Comparisons use
exact string equality or inequality; comparisons inside one <code>all</code>
group are AND-ed, and <code>any</code> groups are OR-ed. The first true region
wins. If no explicit region matches, the unique Default region wins. Reordering
regions can therefore change program meaning.
</p>

<p>
Only the resolved region participates in executable graph validation,
scheduling, side effects, and boundary-output resolution for the selected
target. Other regions remain source variants for other Compilation Contexts.
Lowering MAY replace the structure with the resolved graph, but source tooling
MUST preserve every region and its order.
</p>

<hr/>

<h2 id="loop-output-semantics">12. Loop Output Semantics</h2>

<p>
Loop outputs define how values become observable after loop termination.
In base v0.1, the only standardized loop-output mode is:
</p>

<ul>
  <li><code>mode: "last_value"</code></li>
</ul>

<h3>12.1 last_value</h3>

<p>
If a loop output uses <code>mode: "last_value"</code>:
</p>

<ul>
  <li>when one or more iterations execute, the loop output value MUST be the value produced by the final executed iteration for that output,</li>
  <li>when zero iterations are possible and zero iterations occur, the loop output value MUST be the declared <code>zero_iteration_value</code>.</li>
</ul>

<h3>12.2 Zero-iteration consequence</h3>

<p>
For <code>for_loop</code>, zero iterations are possible when the resolved count is <code>0</code>.
Therefore, any <code>last_value</code> output that can observe zero iterations requires a deterministic zero-iteration meaning.
</p>

<p>
For base v0.1 <code>while_loop</code>, zero iterations do not occur under the standardized post-test continue-while-true rule.
Therefore, <code>zero_iteration_value</code> is not required for that reason.
</p>

<h3>12.3 No additional standardized loop-output modes in v0.1</h3>

<p>
Collection, generalized reduction, profile-defined accumulation, and other advanced output modes are out of scope for base v0.1.
</p>

<hr/>

<h2 id="scheduler-independence">13. Scheduler Independence</h2>

<p>
The exact internal scheduler implementation may vary across runtimes, but the observable semantics of control structures
MUST remain equivalent.
</p>

<p>
Therefore, runtimes MAY differ in:
</p>

<ul>
  <li>internal ready-node scheduling strategy,</li>
  <li>queueing mechanisms,</li>
  <li>buffer layouts,</li>
  <li>memory allocation strategies,</li>
  <li>backend execution-order details that do not change observable meaning.</li>
</ul>

<p>
However, runtimes MUST NOT differ in:
</p>

<ul>
  <li>which <code>case</code> region is selected for a given selector value,</li>
  <li>whether a boolean <code>case</code> executes exactly one of the <code>true</code> or <code>false</code> branches,</li>
  <li>whether a string <code>case</code> falls back to the unique default region when no explicit match exists,</li>
  <li>whether a <code>for_loop</code> with resolved count <code>N</code> executes exactly <code>N</code> iterations,</li>
  <li>whether a <code>while_loop</code> uses the standardized post-test continue-while-true rule,</li>
  <li>which event-case region executes for a given event descriptor, source, and timeout deadline,</li>
  <li>whether a <code>disabled_structure</code> executes anything other than its Enabled region,</li>
  <li>which ordered <code>conditional_disable_structure</code> region is selected for an identical resolved Compilation Context,</li>
  <li>how <code>last_value</code> loop outputs are resolved,</li>
  <li>whether directed cycles inside structure regions still require explicit local memory.</li>
</ul>

<p>
This separation allows backend freedom without semantic fragmentation of the language.
</p>

<hr/>

<h2 id="relation-with-local-memory-and-cycles">14. Relation with Local Memory and Cycles</h2>

<p>
Control structures do not weaken the general cycle-validity rule of FROG.
A directed cycle is valid only when explicit local memory exists in that cycle.
</p>

<p>
Therefore:
</p>

<ul>
  <li>a cycle in an outer diagram remains subject to the normal explicit-local-memory rule,</li>
  <li>a cycle inside a <code>case</code> region remains subject to the same rule,</li>
  <li>a cycle inside a <code>for_loop</code> body remains subject to the same rule,</li>
  <li>a cycle inside a <code>while_loop</code> body remains subject to the same rule,</li>
  <li>a cycle inside an <code>event_structure</code> region remains subject to the same rule.</li>
</ul>

<p>
Loop repetition is not itself a substitute for explicit local memory in a graph cycle.
Iteration and stored state are related concepts, but they are not the same semantic mechanism.
</p>

<p>
In base v0.1, <code>frog.core.delay</code> is the minimal standard primitive used to make local memory explicit.
The normative cycle-validity rule remains owned by <code>Language/State and cycles.md</code>.
</p>

<hr/>

<h2 id="validation-rules">15. Validation Rules</h2>

<p>
Implementations MUST enforce the following semantic rules:
</p>

<ul>
  <li>every validated control structure MUST belong to a standardized or profile-authorized structure family,</li>
  <li>a <code>case</code> activation MUST execute exactly one selected region,</li>
  <li>a boolean <code>case</code> MUST provide deterministic selection between the <code>true</code> and <code>false</code> branches,</li>
  <li>a string <code>case</code> MUST provide deterministic exact-match selection and a deterministic default fallback when no explicit match exists,</li>
  <li>derived authoring views such as <em>If</em>, <em>If / Else</em>, <em>Else If</em>, or <em>Switch</em> MUST NOT alter the semantic family of the validated structure,</li>
  <li>a <code>for_loop</code> count MUST resolve to a non-negative <code>i64</code> value in base v0.1,</li>
  <li>a <code>for_loop</code> with resolved count <code>N</code> MUST execute exactly <code>N</code> iterations,</li>
  <li>a <code>while_loop</code> condition MUST resolve to a <code>bool</code> value after each body activation,</li>
  <li>a base v0.1 <code>while_loop</code> MUST use the standardized post-test continue-while-true rule,</li>
  <li>an <code>event_structure</code> timeout MUST resolve to an <code>i64</code> value greater than or equal to <code>-1</code>,</li>
  <li>an <code>event_structure</code> MUST define exactly one timeout region,</li>
  <li>every event-case region MUST define a canonical event descriptor,</li>
  <li>every <code>value_change</code> event descriptor MUST identify its source,</li>
  <li>non-timeout event descriptors MUST be unique by kind and source within one structure,</li>
  <li>one event occurrence MUST execute exactly one matching event-case region,</li>
  <li>a <code>disabled_structure</code> MUST define exactly one Enabled region and one Disabled region,</li>
  <li>a <code>disabled_structure</code> activation MUST execute only its Enabled region and MUST ignore its authoring-visible region,</li>
  <li>a <code>conditional_disable_structure</code> MUST have no runtime selector, MUST evaluate AND before OR, MUST select the first true ordered condition, and MUST fall back to its unique Default region,</li>
  <li>loop outputs MUST have deterministic semantic meaning after termination,</li>
  <li>if <code>mode: "last_value"</code> is used and zero iterations are possible, a deterministic zero-iteration meaning MUST exist,</li>
  <li>cycles inside structure-owned regions MUST satisfy the same explicit-local-memory rule as any other directed cycle.</li>
</ul>

<p>
Tools SHOULD additionally warn when:
</p>

<ul>
  <li>loop output meaning is valid but hard to understand,</li>
  <li>structure behavior is technically valid but difficult to inspect,</li>
  <li>feedback inside a structure region is valid yet visually unclear.</li>
</ul>

<hr/>

<h2 id="examples">16. Examples</h2>

<h3>16.1 Boolean case</h3>

<p>
Assume one validated boolean <code>case</code> activation:
</p>

<ul>
  <li>if the selector resolves to <code>true</code>, the <code>true</code> region executes and the <code>false</code> region does not execute,</li>
  <li>if the selector resolves to <code>false</code>, the <code>false</code> region executes and the <code>true</code> region does not execute.</li>
</ul>

<p>
An IDE MAY present this structure as <em>If</em> or <em>If / Else</em>.
That presentation does not change the semantic family or the branch-selection rule.
</p>

<h3>16.2 String case with default fallback</h3>

<p>
Assume a string <code>case</code> with explicit matches <code>"start"</code> and <code>"stop"</code> plus one default region:
</p>

<ul>
  <li>selector = <code>"start"</code> → the <code>"start"</code> region executes,</li>
  <li>selector = <code>"stop"</code> → the <code>"stop"</code> region executes,</li>
  <li>selector = <code>"pause"</code> → the default region executes.</li>
</ul>

<p>
An IDE MAY present this structure as a <em>Switch</em>-like form.
That presentation does not create a distinct standardized semantic family.
</p>

<h3>16.3 for_loop with count 3</h3>

<p>
If the resolved count is <code>3</code>, the body executes exactly three times and the standardized index sequence is:
</p>

<pre><code>0, 1, 2</code></pre>

<h3>16.4 for_loop with count 0</h3>

<p>
If the resolved count is <code>0</code>, the body executes zero times.
If a loop output uses <code>mode: "last_value"</code>, its observable result is the declared
<code>zero_iteration_value</code>.
</p>

<h3>16.5 while_loop under the standardized post-test rule</h3>

<p>
A base v0.1 <code>while_loop</code> always executes its body once.
After that first body activation:
</p>

<ul>
  <li>if the condition resolves to <code>true</code>, another iteration occurs,</li>
  <li>if the condition resolves to <code>false</code>, the loop terminates.</li>
</ul>

<h3>16.6 Event structure with a 100 ms timeout</h3>

<p>
Assume one <code>value_change</code> region for source
<code>widget:temperature</code> and one timeout region. If a matching value-change
occurrence arrives within 100 ms, the value-change region executes. Otherwise,
the timeout region executes. In both cases exactly one region executes.
</p>

<h3>16.7 Cycle inside a loop body</h3>

<p>
A feedback path inside a loop body is not automatically valid just because the enclosing structure repeats.
If that feedback path is a directed cycle, it still requires explicit local memory such as <code>frog.core.delay</code>.
</p>

<hr/>

<h2 id="out-of-scope-for-v01">17. Out of Scope for v0.1</h2>

<ul>
  <li>timed or clock-driven structures,</li>
  <li>parallel-region control structures,</li>
  <li>pattern-oriented branch selection beyond the standardized boolean and string case forms,</li>
  <li>exception-handling structures,</li>
  <li>alternative standardized while-loop continuation policies,</li>
  <li>advanced loop-output modes such as generalized reduction or implicit collection,</li>
  <li>hidden automatic memory insertion inside structure regions,</li>
  <li>derived authoring labels becoming independent semantic families without explicit language standardization.</li>
</ul>

<hr/>

<h2 id="summary">18. Summary</h2>

<p>
FROG treats structural control as an explicit language-level concept, not as a disguised library call.
This document defines the normative execution semantics of that structural control.
</p>

<ul>
  <li><code>case</code>, <code>for_loop</code>, <code>while_loop</code>, <code>event_structure</code>, <code>disabled_structure</code>, and <code>conditional_disable_structure</code> are the standardized control-structure families of base v0.1.</li>
  <li>A boolean <code>case</code> is the semantic equivalent of <code>if / else</code>, but the standardized family remains <code>case</code>.</li>
  <li>A string <code>case</code> performs deterministic exact-match selection with a required default fallback.</li>
  <li>Authoring-facing derived forms such as <em>If</em>, <em>If / Else</em>, <em>Else If</em>, and <em>Switch</em> do not introduce separate semantic families in base v0.1.</li>
  <li>A <code>for_loop</code> performs counted repetition with deterministic index progression.</li>
  <li>A base v0.1 <code>while_loop</code> uses standardized post-test continue-while-true semantics.</li>
  <li>An <code>event_structure</code> executes exactly one region for a declared event occurrence or deterministic timeout.</li>
  <li>A <code>disabled_structure</code> retains both regions but executes only its Enabled region.</li>
  <li>A <code>conditional_disable_structure</code> resolves the first matching compile-time variant, or Default, before runtime.</li>
  <li>Loop outputs have standardized deterministic meaning through <code>mode: "last_value"</code>.</li>
  <li>Control structures do not create hidden memory and do not weaken the explicit-local-memory rule for cycles.</li>
</ul>

<p>
This gives FROG a clear, durable, and semantically explicit foundation for language-level structural execution.
</p>
