# Example 08 Enum Realization Package

This folder owns the Example 08 `.wfrog` realization package.

The `.frog` source owns the front-panel instance: widget positions, enum item vocabulary, caption placement, and intentional per-instance visual overrides.

The `.wfrog` package in this folder owns the realization linkage: Default Enum realization references, host requirements, SVG asset references, reusable `default_widget_properties`, and dropdown host-surface layout. Runtime code must combine the `.frog` instance with these `.wfrog` defaults/assets instead of duplicating enum widgets or rendering a fallback.
