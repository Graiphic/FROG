<p align="center">
  <img src="../../FROG logo.svg" alt="FROG logo" width="200" />
</p>

<h1 align="center">FROG Conformance Case 23</h1>

<p align="center">
  <strong>Explicit state initialization remains distinct from inferred default initial value</strong><br>
  FROG — Free Open Graphical Language
</p>

<hr>

<h2>Contents</h2>
<ul>
  <li><a href="#case-name">1. Case Name</a></li>
  <li><a href="#expected">2. Expected</a></li>
  <li><a href="#why">3. Why</a></li>
  <li><a href="#boundary-being-exercised">4. Boundary Being Exercised</a></li>
  <li><a href="#scenario">5. Scenario</a></li>
  <li><a href="#what-makes-the-case-valid">6. What Makes the Case Valid</a></li>
  <li><a href="#expected-meaning">7. Expected Meaning</a></li>
  <li><a href="#expected-preservation">8. Expected Preservation</a></li>
  <li><a href="#what-must-not-happen">9. What Must Not Happen</a></li>
  <li><a href="#rationale">10. Rationale</a></li>
  <li><a href="#minimum-conformance-reading">11. Minimum Conformance Reading</a></li>
</ul>

<hr>

<h2 id="case-name">1. Case Name</h2>

<p><strong>Case:</strong> <code>23_explicit_state_initialization_remains_distinct_from_inferred_default_initial_value</code></p>

<hr>

<h2 id="expected">2. Expected</h2>

<p><strong>Expected:</strong> valid</p>

<p><strong>Expected meaning:</strong> language-level meaning is established.</p>

<p><strong>Expected preservation:</strong> explicitly declared or explicitly attached state initialization remains distinct from any implementation-side, type-side, backend-side, or convenience-side inferred default initial value.</p>

<hr>

<h2 id="why">3. Why</h2>

<p>This case exists to make one boundary explicit:</p>

<pre><code>explicit state initialization
            !=
inferred default initial value
</code></pre>

<p>A conforming implementation may support internal defaults, backend defaults, storage defaults, zero-initialization strategies, or target-family convenience rules.</p>

<p>Those conveniences do not become the language meaning of an explicitly initialized stateful element.</p>

<p>If source-visible state initialization is explicit and valid, that explicit initialization participates in validated program meaning and must not be collapsed into a mere inferred default.</p>

<hr>

<h2 id="boundary-being-exercised">4. Boundary Being Exercised</h2>

<p>This case exercises the normative boundary:</p>

<pre><code>canonical .frog source
        |
        v
validated program meaning
</code></pre>

<p>It especially tests the distinction between:</p>
<ul>
  <li>explicit source-owned state initialization, and</li>
  <li>implementation-side inference of a default initial value.</li>
</ul>

<p>It also exercises the downstream preservation obligation that later derivation must not erase this distinction.</p>

<hr>

<h2 id="scenario">5. Scenario</h2>

<p>A FROG program contains valid explicit state participation whose initial value is explicitly established by the source according to the published language rules.</p>

<p>The program is otherwise valid:</p>
<ul>
  <li>its structure is legal,</li>
  <li>its state participation is legal,</li>
  <li>its typing is legal,</li>
  <li>its cycle legality is satisfied where applicable.</li>
</ul>

<p>The important property of this case is that the initial state is not left to implementation guesswork. It is explicitly established by the source-side program meaning.</p>

<hr>

<h2 id="what-makes-the-case-valid">6. What Makes the Case Valid</h2>

<p>This case is valid because the program does not rely on silent invention of state semantics.</p>

<p>Instead:</p>
<ul>
  <li>state participation is explicit where required,</li>
  <li>the initial state is explicitly established where required,</li>
  <li>the initialization is semantically attributable to the validated source-side program meaning,</li>
  <li>the program does not require a conforming implementation to invent an initial value by fallback inference alone.</li>
</ul>

<p>Accordingly, validation succeeds and language-level meaning is established.</p>

<hr>

<h2 id="expected-meaning">7. Expected Meaning</h2>

<p>If this case validates, the established meaning includes at least the following:</p>
<ul>
  <li>there is legal explicit state participation,</li>
  <li>that state participation has an explicit initialization role where the published rules require it,</li>
  <li>the initialization is part of validated program meaning rather than a backend accident,</li>
  <li>the program meaning does not treat the explicit initialization as interchangeable with a merely inferred default.</li>
</ul>

<p>The semantic reading is therefore:</p>

<pre><code>explicit source-established initialization
        -&gt;
validated state initialization meaning
</code></pre>

<p>Not:</p>

<pre><code>missing initialization
        -&gt;
implementation invents a value
        -&gt;
same meaning
</code></pre>

<hr>

<h2 id="expected-preservation">8. Expected Preservation</h2>

<p>A conforming implementation must preserve the distinction that this case establishes.</p>

<p>At minimum, later layers must not silently collapse:</p>
<ul>
  <li>explicit initialization, into</li>
  <li>default inference.</li>
</ul>

<p>This does not require one frozen internal representation.</p>

<p>It does require that the implementation remain semantically faithful to the published distinction.</p>

<p>In particular:</p>
<ul>
  <li>derivation must not erase the fact that initialization was explicit,</li>
  <li>lowering must not reinterpret explicit initialization as mere target convenience,</li>
  <li>backend contract generation must not silently rewrite explicit initialization into unspecified default behavior,</li>
  <li>runtime-private realization must not become the hidden owner of the initial value.</li>
</ul>

<hr>

<h2 id="what-must-not-happen">9. What Must Not Happen</h2>

<p>A conforming implementation must <strong>not</strong> do any of the following:</p>

<ul>
  <li>treat explicit state initialization as semantically equivalent to a type-derived default,</li>
  <li>treat explicit state initialization as semantically equivalent to zero-initialization convenience,</li>
  <li>discard explicit initialization during derivation and later reintroduce a guessed default,</li>
  <li>claim that explicit initialization is unnecessary merely because one target family could supply a default value,</li>
  <li>reinterpret the program so that explicit initialization loses source attribution.</li>
</ul>

<p>The forbidden collapse is:</p>

<pre><code>explicit initialization
        -/-> inferred default
</code></pre>

<hr>

<h2 id="rationale">10. Rationale</h2>

<p>This distinction matters because stateful behavior is one of the places where language drift appears quickly.</p>

<p>If explicit initialization can be silently replaced by inferred defaults, then:</p>
<ul>
  <li>source meaning becomes target-dependent,</li>
  <li>validation loses authority,</li>
  <li>backend convenience starts redefining the language,</li>
  <li>independent implementations may diverge while all claiming conformance.</li>
</ul>

<p>FROG must keep the semantic rule explicit:</p>

<pre><code>what the user explicitly establishes
            remains
part of validated program meaning
</code></pre>

<p>This is especially important for durable inspectability, reproducible execution meaning, and future independent implementations.</p>

<hr>

<h2 id="minimum-conformance-reading">11. Minimum Conformance Reading</h2>

<p>A minimal conforming reading of this case is:</p>

<ul>
  <li>the source is valid,</li>
  <li>language-level meaning is established,</li>
  <li>explicit state initialization is recognized as explicit,</li>
  <li>explicit state initialization remains distinct from inferred default initial value,</li>
  <li>later layers must preserve that distinction rather than silently laundering it away.</li>
</ul>

<p>The public truth asserted by this case is:</p>

<pre><code>explicit state initialization
            remains distinct from
inferred default initial value
</code></pre>
