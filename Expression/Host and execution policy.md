# FROG Host and Execution Policy

This document defines the optional source-carried host and execution-policy
sections used by a FROG program. These sections are deliberately separate from
`metadata`, the executable `diagram`, and IDE-only preferences.

## 1. Ownership boundary

- `metadata` owns descriptive identity and documentation only.
- `host` owns how an external host launches and presents the front panel.
- `execution_policy` owns portable execution-control policy explicitly defined
  by FROG profiles.
- `diagram` and the validated language semantics remain the authority for
  executable program meaning.
- `ide` owns authoring preferences that execution-facing systems must ignore.

Neither `host` nor `execution_policy` may redefine node behavior, dataflow,
types, graph dependencies, scheduling order, or observable program results.

## 2. Top-level shape

Both sections are optional. Their version fields are required whenever the
corresponding section is present.

```json
{
  "host": {
    "version": 1,
    "launch": {
      "trigger": "manual"
    },
    "front_panel_window": {
      "style": "standard",
      "title_mode": "program_name",
      "custom_title": "",
      "initial_position": "centered",
      "minimum_width": 0,
      "minimum_height": 0,
      "allow_close": true,
      "allow_resize": true,
      "allow_minimize": true,
      "menu_bar_visible": true,
      "toolbar_visible": true,
      "vertical_scrollbar_visible": true,
      "horizontal_scrollbar_visible": true,
      "scroll_to_origin": false
    }
  },
  "execution_policy": {
    "version": 1,
    "debugging_allowed": true
  }
}
```

Unknown later-version fields must be preserved by lossless authoring tools when
possible and must not be guessed by execution-facing systems.

## 3. Host version 1

### 3.1 Launch trigger

`host.launch.trigger` accepts:

- `manual`: loading or opening the source does not start execution;
- `on_open`: an application host may start the FROG after successful load,
  validation, and runtime preparation.

The presence of a `front_panel` alone never implies `on_open`.

### 3.2 Front-panel window

`style` accepts `standard` or `dialog`. It is a presentation request; it does
not alter the program graph.

`title_mode` accepts `program_name` or `custom`. When `program_name` is used,
the host derives the title from `metadata.name`. `custom_title` is used only
when `title_mode` is `custom`.

`initial_position` accepts `unchanged`, `centered`, `maximized`, `minimized`,
or `custom`. Version 1 stores the common modes; custom coordinates are reserved
for a later compatible extension.

`minimum_width` and `minimum_height` are non-negative host pixels. Zero means
that no source-defined minimum is requested.

The `allow_*` fields constrain user window actions. Visibility fields request
host chrome but do not require a host that has no corresponding facility to
invent one.

## 4. Execution policy version 1

`debugging_allowed` controls whether an IDE or compatible execution host may
attach interactive debugging controls to this program. It does not change
execution meaning. A value of `false` is a tooling policy, not encryption,
source protection, or a security boundary.

Version 1 intentionally defines no scheduler priority, execution-system hint,
reentrancy/clone mode, inlining request, or implicit error-handling behavior.
Those concepts may be added only after their portable semantics and runtime
capability negotiation are specified.

## 5. Validation and defaults

When the sections are absent, conforming tools use these effective defaults:

- manual launch;
- standard, centered front-panel window;
- title derived from the program name;
- close, resize, minimize, menu bar, toolbar, and scrollbars allowed/visible;
- no forced scroll-to-origin;
- interactive debugging allowed.

Invalid enum values must produce a source-validation diagnostic. Negative
minimum dimensions are invalid. A tool must not silently reinterpret an
unsupported policy as a different policy.

## 6. Explicit exclusions

The following are not part of this contract:

- password-based diagram hiding or source locking;
- revision history (source control remains the authority);
- compiler memory estimates and derived diagnostics;
- print settings;
- IDE layout preferences;
- private scheduler and compiler optimization hints.

Derived diagnostics may be displayed by an IDE but must not be serialized as
authoritative host or execution policy.
