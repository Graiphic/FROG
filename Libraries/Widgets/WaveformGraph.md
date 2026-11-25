# Waveform Graph — WFG-1-linear profile

Status: limited authoring-profile contract, not full WFG-1 conformance or a runtime ABI. The complete Waveform Graph proposal remains broader than this profile. The independent public package/type validator and full frog.ui member catalogue are not yet integrated for it.

Class `frog.widgets.waveform_graph` is an indicator, distinct from `frog.widgets.waveform_chart`. Its single logical value input has type `frog.waveform_graph_value`. An unwired indicator has an empty default; it has no implicit history or acquisition behavior. A public output binding exports the displayed value and is not the direction of the diagram input.

The dataset contains an ordered list of uniform numeric series. Each has a unique, nonempty stable `id`, descriptive `name`, finite numeric `origin`, strictly positive finite `step`, and Y samples represented logically by f64. `X[k] = origin + k * step`. NaN and infinity are discontinuities, not zeros. A single finite point remains a real single point. Duplicate IDs, invalid X, quotas or unrepresentable spans reject the whole candidate and preserve the last accepted dataset.

Each accepted update replaces the dataset atomically. The input producer cannot later mutate its accepted snapshot. Clear publishes an empty dataset without deleting configuration. Styles bind by series ID rather than array ordinal; default IDs from array adapters are `series_0`, `series_1`, etc.

The linear profile admits 1D and rectangular row-major 2D arrays of f32/f64/i8/u8/i16/u16/i32/u32. Rows are plots; columns are samples. i64/u64 require explicit conversion because f64 does not represent every large integer exactly. Scalars, strings, complex values and arbitrary clusters have no implicit adapter in this profile. Absolute time and other source waveform types require a later explicit contract.

Source geometry uses the existing widget layout. Default size 640 x 400, minimum 240 x 160 and aura 4 are source units. Resizing reflows axes and the plot rectangle without changing samples or stretching font size.

The local instance stores its settings in `props.waveform_graph` with
`profile: "WFG-1-linear"`. The base fields are `x_label`, `y_label`,
`autoscale_x`, `autoscale_y`, `grid_visible`, `legend_visible`, `x_min`,
`x_max`, `y_min`, `y_max`, `plots` and `initial_data`. Optional presentation
fields include the [axis and legend visibility settings](#axis-and-legend-visibility).
Defaults are Time/Amplitude, both axes on autoscale, grid and legend visible,
X [0,100], Y [-10,10], and no plots or data. Each plot override contains
`id`, `name`, `rgb`, `visible`, `lines`, `points` and `width`. Each initial
series contains `id`, `name`, `origin`, `step` and `y`. Encode non-finite Y
values as the JSON strings `NaN`, `Inf` and `-Inf`.

Only explicit initial data/configuration is persisted. Accepted runtime snapshots, revisions, caches, device handles and diagnostics are not defaults. Unknown profile/fields or invalid payloads are preserved and block support claims; they are never silently converted to an empty linear graph.

Effective authoring limits: 64 series, 1 000 000 total samples, RGB styles and line width 0.5–8. An execution target may not infer support from the existence of a local renderer: it must explicitly advertise the profile. No target runtime support, time adapters, zoom/pan, cursors, event API, export or advanced-axis conformance is claimed here.

The sample quota above is the in-memory dataset limit. Persisted initial data also shares the host document's 250 000 JSON-value and 64 MiB source budgets with all other objects and metadata. The host must refuse an unreadable over-budget save without overwriting the previously saved document; it does not truncate samples to fit.

The native editor currently contains separate local class-package and Default-realization descriptors. They are not promoted to a universal canonical `.wfrog` envelope by this document. Harmonization and independent validator coverage are still required before full standard publication.


## Optional authored cursors (local common-Frontend extension, 8 November 2026)

The current common reader/writer also accepts an optional `cursors` array in
`props.waveform_graph`. This presentation extension does not change the logical
value port, sample representation, target admission or runtime ABI above. Its
presence is not a claim of complete WFG or VI Server conformance.

Each cursor has a unique `id`, a `name`, finite `x`/`y`, `plot` (stable series ID),
`index` (sample index), `mode` (0 free, 1 single-plot), `visible`, `show_name`,
`allow_dragging`, RGB `rgb`, `line_style` (0 solid, 1 dash, 2 dot, 3 dash-dot,
4 dash-dot-dot), `line_width` (0 hairline, 1–4 source units), `point_style`
(0–16, the existing XY point-symbol vocabulary), and `style` (0 none,
1 short vertical, 2 vertical, 3 short horizontal, 4 horizontal, 5 short cross,
6 vertical/short horizontal, 7 horizontal/short vertical, 8 full cross).
These are GS presentation indices, not an asserted mapping to VI Server IDs.
At most 256 cursors are admitted; invalid/unknown data is preserved with a
support diagnostic rather than silently discarded.

Free cursors use their data coordinates. A single-plot cursor resolves its
series ID and sample index against the actually displayed immutable dataset;
`x = origin + index * step` and Y is that sample. Missing series, missing indices
and non-finite samples have no fabricated measured value. Dragging selects the
nearest finite sample in X on the associated series. Styles, visibility and
movement permissions never change the graph's data or saved defaults.

The editor retires the former annotation actions for this graph. Legacy
`annotations` (text and X/Y) are read once as named free cursors with a small
plus marker; text and coordinates survive, and a repeated save/open does not
duplicate them. XY annotations remain a separate existing facility.

## Optional independent axes (local common-Frontend extension, 21 November 2026)

The local reader and writer accept `x_axis` and `y_axis` for presentation
overrides. Each axis keeps its own linear or logarithmic mapping, graduation
format, precision, trailing zeros, title, tick labels, major and minor marks,
and major and minor grids. Unset format and major-grid overrides inherit the
shared settings. `y_title.vertical` controls title orientation. Bounds and
autoscale use the existing `x_min`, `x_max`, `y_min`, `y_max`, `autoscale_x`
and `autoscale_y` fields. Manual logarithmic ranges require positive bounds.

Logarithmic projection omits out-of-domain points and breaks the drawn line at
each gap. Autoscale uses plottable coordinates and provides a valid positive
display range when the active mapping has no plottable samples. Mapping and
label formats do not change the original values, sample origin or step. The
local absolute-time format defaults to seconds since 1904-01-01 UTC. Axis
overrides are independent of whether data uses sample indices, explicit
origin and step, or waveform timing. The `WFG-1-linear` source marker is
retained for the local codec. These optional fields extend presentation.

## Axis and legend visibility

Within `props.waveform_graph`, `x_axis.visible` and `y_axis.visible` each
accept a boolean. An absent flag defaults to `true`, including when other
settings for that axis are present. Hiding an axis preserves its title,
range, mapping and graduation settings for the next time it is shown.

`legend_visible` controls the whole plot legend. `legend_rows` is an integer
from 1 to 64 giving the number of visible rows, including unoccupied slots.
Increasing it leaves the dataset unchanged. The optional `legend_position`
object stores the legend's top-left corner as numeric `x` and `y` in logical
units relative to the widget's top-left corner. Omitting it places the legend
beside the right edge. Moving the widget carries its legend with it.
Existing documents without `legend_rows` use four rows. Each plot retains
its own `plots[].visible` and stable ID when the legend is folded or hidden.
The widget label has its own visibility setting outside this graph object.
