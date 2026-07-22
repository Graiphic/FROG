# Context Help

Context Help is an IDE discoverability surface. It explains the source-visible
object under the pointer without changing program meaning or interrupting the
active editor.

## Expected Behavior

A FROG-compatible IDE MAY provide a modeless Context Help window that:

- follows the widget, terminal, node, port, wire, annotation, or other
  source-visible object under the pointer;
- presents the object's stable title, concise description, value type, role,
  and relevant connector information;
- can be frozen so the user can inspect or follow a documentation link;
- can link to more detailed public or implementation-specific documentation;
- remains usable in Front Panel, Diagram, and palette-oriented workflows.

An IDE SHOULD update this surface only when the semantic hover target changes.
Context Help should not require a polling loop or redefine the hovered object.

## Source And Catalog Ownership

Intrinsic FROG functions and standard widgets should obtain their descriptions
from stable catalogs keyed by canonical primitive IDs or widget class IDs.

External functions and custom widgets MAY carry author-supplied documentation
metadata. A concise description and a documentation URL are presentation and
discoverability metadata. They do not change ports, types, validation,
execution, scheduling, or dataflow semantics.

Implementations may preserve metadata equivalent to:

```json
"doc": {
  "summary": "Adds two application-specific values.",
  "url": "https://docs.example.test/functions/add"
}
```

Widget-instance property maps may carry the common fields
`documentation.description`, `documentation.tip_strip`, and an optional
`documentation.url`. Unknown documentation metadata should be
preserved where the applicable source-preservation contract requires it.

## Security

An IDE must treat documentation links as untrusted external destinations. It
should display or launch only explicitly supported URL schemes and should not
interpret documentation text as executable content.

## Boundary

Context Help belongs to IDE discoverability. The language specification owns
the meaning of the object being described; the help window does not.
