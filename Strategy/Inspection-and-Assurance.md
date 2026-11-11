# Inspection, execution and evidence

**Supporting guide · reviewed 11 November 2026**

The [project philosophy](Visual-Programming-and-Human-Control.md) focuses on the human capacity to follow and validate machine-generated code. FROG serializes a structured program model in `.frog`; Studio projects it into navigable graphical dataflow. This guide explains the supporting evidence used to understand changes and check their results.

## What each view or check tells you

| Question | Relevant view or evidence | Scope |
| --- | --- | --- |
| How is the program organized? | Connections, hierarchy, types, labels, color, shape and arrangement | Helps expose relationships and locate relevant detail. |
| Is the conversion faithful? | Declared source subset, semantic mappings, identified dependencies and suitable behavior comparisons | A serialized model alone does not establish preservation of behavior. |
| What did generation change? | Identified revisions, stable associations, meaningful differences and impact navigation | A design requirement; the update does not announce a complete change-review feature. |
| Is this document valid? | Structural and semantic validation | Checks language rules rather than whether the chosen algorithm meets the intended need. |
| Can this target execute it? | Admission against the selected backend/provider profile | Applies to the complete admitted graph and declared capabilities. |
| What happened? | Identified Run, Highlight observations, inputs and results | Describes that execution and its conditions. |
| Is the correction acceptable? | Review of the relevant behavior, expected results, tests and revision identity | Establishes only the scope of the review and checks actually performed. |
| What was deployed? | Build result, program revision, dependencies and configuration | Captures the compiled package; execution remains a separate check. |

Source identities and source maps help connect the human view to execution. They are not themselves proof of translation correctness. A relevant change invalidates old conclusions about the behavior affected; an old observation cannot stand for the newly generated candidate.

## A complete example and a meaningful comparison

The calculator illustrates how the architecture of about 1,000 C/H lines can be made accessible on one graphical page, with details in navigable cases. Follow an event, inspect state with Highlight and check a change. The C source includes UI, parser and queue work; the diagram is an abstraction of relevant architecture. This does not establish an automatic importer or a measured speed advantage.

Evaluate whether people identify the consequential change, explain behavior, diagnose a fault and complete a valid correction. Stable visual conventions and hierarchy should support those tasks. Keep requirements and information equivalent in a comparative evaluation.

For machine-performance claims, record algorithms, semantics, data representations, runtime services, inputs and target configuration. FIR/IR and established backends provide a path to optimization; frontend and lowering quality still matter.

## Current scope and optional extensions

The public common Frontend and selected Studio Run/Highlight/LLVM paths provide an existing foundation. Check [current status](../FROG-Project-Status.md), [source/profile compatibility](../docs/studio-source-compatibility.md) and attributed dossiers. General imports and broader change-review capabilities remain development work, not features qualified by this document.

Specialized formal verification can address a particular application's guarantees under explicit assumptions. The separate [DARPA-oriented note](Research/DARPA-Collaboration-Options.md) keeps those candidate tools and demonstrators outside the general product philosophy. This guide adds no verification profile, source field or execution mode.

[Project philosophy](Visual-Programming-and-Human-Control.md) · [Heilmeier framing](Heilmeier/Readme.md) · [Governance and validation](../docs/graiphic-philosophy-and-validation.md)
