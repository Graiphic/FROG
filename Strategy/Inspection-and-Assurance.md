# Inspection, execution and evidence

**Supporting guide · reviewed 11 November 2026**

FROG and Graiphic Studio aim to make graphical programs practical to build, understand, execute and maintain. The [general philosophy](Visual-Programming-and-Human-Control.md) explains that purpose. This guide clarifies the evidence available during development and review.

## What each view or check tells you

| Question | Relevant view or evidence | Scope |
| --- | --- | --- |
| How is the program organized? | Graph connections, structures, types, labels and hierarchy | Helps navigate behavior and locate relevant detail. |
| Is this document valid? | Structural and semantic validation | Checks the applicable language rules, not whether the chosen algorithm meets the user's need. |
| Can this target execute it? | Admission against the selected backend/provider profile | Applies to the complete admitted graph and the declared capabilities. |
| What happened during execution? | Identified Run, Highlight observations, inputs and results | Describes that execution and its conditions. |
| Does a change work as intended? | A specified task, expected results and appropriate tests | Establishes the tested behavior and the limits of that evidence. |
| Was the application packaged? | Build result and captured dependencies/configuration | Identifies what was compiled and packaged; execution remains a separate check. |

These stages help an engineer investigate and correct a program. Keep observations attributable to the version that ran. A change to the program or its relevant dependencies may invalidate an earlier result. Stable identities and source maps help connect views; they do not alone prove preservation of meaning.

## Demonstrate the practical workflow

A complete example should let someone move from an overview to the relevant operation, follow values and check the result of a change. The calculator illustrates this with events, a queue and explicit state. Comparisons should account for the detail and work represented in each implementation.

Measure relevant engineering outcomes when making comparative claims: understanding the behavior correctly, finding the cause of a fault, completing a valid change and the effort required. A compact diagram is useful communication, while review-speed or productivity claims require their own evidence.

## Current scope and optional extensions

The public common Frontend and selected Studio Run/Highlight/LLVM paths provide an existing foundation. Check the [current status](../FROG-Project-Status.md), [source/profile compatibility](../docs/studio-source-compatibility.md) and attributed dossiers for each supported scope. A palette entry or a saved editor document alone does not establish target support.

Formal verification can be useful where a particular application needs a stated guarantee under explicit assumptions. It remains an optional extension with its own supported semantics and trusted components. No verification adapter is required by the general philosophy or introduced by this guide. The specific bounded-model proposal and candidate tools are kept in the separate [collaboration note](Research/DARPA-Collaboration-Options.md).

[Project philosophy](Visual-Programming-and-Human-Control.md) · [General Heilmeier framing](Heilmeier/Readme.md) · [Governance and validation](../docs/graiphic-philosophy-and-validation.md)
