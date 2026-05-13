# Example 08 Enum Realization Package

This folder owns the Example 08 `.wfrog` realization package.

The `.frog` source owns the front-panel instance: widget positions, enum item vocabulary, caption placement, and per-instance visual parameters.

The `.wfrog` package in this folder owns the realization linkage: Default Enum realization references, host requirements, and SVG asset references. Runtime code must combine the `.frog` instance with these `.wfrog` assets instead of duplicating enum widgets or rendering a fallback.
