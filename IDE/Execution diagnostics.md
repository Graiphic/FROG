# Execution diagnostics in Graiphic Studio

Revision 1.1 — 2026-09-16. This is the IDE presentation contract, not a new
language profile. Semantic authority belongs to
[Execution eligibility](../Language/Execution%20eligibility.md).

<a id="lecture-et-blocages"></a>

## Run and blocking states

- The **Run** button retains its triangle. A red prohibition symbol is **overlaid** when validation or compilation is blocked or unsupported. Runtime unavailability alone does not draw this symbol. The vector overlay must surround the triangle without replacing it or overflowing, and remain antialiased at actual toolbar sizes.
- The second button, **Run continuously**, keeps its normal icon. It is not an error indicator and does not bypass validation.
- Run remains clickable to inspect diagnostics, including for a public source whose validator/lowerer is unavailable.
- Clicking blocked Run opens an explanation window; it starts neither an old artifact nor an incomplete program.
- Front Panel and Block Diagram use the same result for the current document.

<a id="fenêtre-de-diagnostics"></a>

## Diagnostics window

Each row states its impact: compilation, execution, both, or non-blocking. Two distinct states appear at the top: compilation validation and execution availability. “Validation succeeded” does not mean an artifact has already been generated, loaded or executed. Selection shows the reason, stable code and, when available, affected terminal, structure and region.

Blocking reasons remain visible even with warning severity. The non-blocking-message checkbox must never hide a refusal reason. An unavailable target is explicitly distinguished from an incorrect wire. When no runtime is included, one global diagnostic is sufficient: per-primitive execution-capability warnings must not suggest that Add or constants are invalid functions. This grouping must not remove any compilation-blocking reason.

The window follows the Properties / Tools → Options style: custom title, IDE font and metrics, themed buttons and selection, and light/dark colors. The list shows subject and reason over several lines; full details are copyable and wrap. The window is resizable. Tab, Enter and Escape remain usable.

**Show Error**, or double-clicking a locatable row, reveals the affected region and centers/selects the object. Navigation changes neither wires, default values nor compilation selection. A global diagnostic, such as a missing runtime, invents no diagram location.

The window is reused per document and refreshes after editing, undo/redo or model changes. A stale row must be refreshed before navigation. Opening it releases the toolbar button's mouse capture so the list is immediately interactive.

<a id="interprétation"></a>

## Interpretation

The symbol means **validation / compilation blocked or unsupported**, not merely “runtime absent.” A Studio version without a runtime may validate and edit a diagram without executing it: Run then keeps its normal triangle and explains the execution limit when clicked, without pretending to run code.

A new empty editing document is a no-operation (void) program, valid if no interface obligation remains unsatisfied. Two numeric constants wired to Add's required inputs are also valid when its ordinary output is unused. However, an empty editing projection does not prove validity of a public source whose validation is unavailable.

The official rules belong to the language and selected profile. A function's presence in a palette does not guarantee target implementation. Results actually verified are tracked in the [Studio coverage matrix](../docs/execution-eligibility-studio-coverage.md).

<a id="évolution"></a>

## Evolution

Changes to this presentation must preserve access to causes, the severity/blocking distinction and the distinction between a program error and a target limitation. At minimum, test both panels, public sources, light/dark themes, icon sizes, navigation into a hidden region, refresh after a correction and document independence.
