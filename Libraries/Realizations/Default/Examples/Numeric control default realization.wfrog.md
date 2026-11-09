<p align="center">
  <img src="../../../../FROG logo.svg" alt="FROG logo" width="140" />
</p>

<h1 align="center">Example — Default Numeric Control Realization Package</h1>

<p align="center">
  <strong>Illustrative machine-readable <code>.wfrog</code> publication example for the default realization of <code>frog.widgets.numeric_control</code></strong><br/>
  <em>FROG — Free Open Graphical Language</em>
</p>

<hr/>

<h2>Contents</h2>

<ul>
  <li><a href="#overview">1. Overview</a></li>
  <li><a href="#purpose-of-this-example">2. Purpose of this Example</a></li>
  <li><a href="#example-json">3. Example JSON</a></li>
  <li><a href="#reading-notes">4. Reading Notes</a></li>
  <li><a href="#summary">5. Summary</a></li>
</ul>

<hr/>

<h2 id="overview">1. Overview</h2>

<p>
This document provides an illustrative machine-readable <code>.wfrog</code> publication example for the default realization of <code>frog.widgets.numeric_control</code>.
</p>

<p>
It demonstrates a richer realization than the button example because it includes:
</p>

<ul>
  <li>a value display surface,</li>
  <li>increment and decrement public parts,</li>
  <li>widget-level states,</li>
  <li>part-level pressed states,</li>
  <li>resource mappings at multiple levels.</li>
</ul>

<hr/>

<h2 id="purpose-of-this-example">2. Purpose of this Example</h2>

<p>
This example exists to show how a value-carrying standard widget with multiple public parts may be realized cleanly through a machine-readable package.
</p>

<p>
It demonstrates that:
</p>

<ul>
  <li>the class law stays upstream,</li>
  <li>part names stay stable,</li>
  <li>state-specific resources may exist for subparts,</li>
  <li>fallback remains explicit,</li>
  <li>the runtime still remains only the interpreter of the publication.</li>
</ul>

<hr/>

<h2 id="example-json">3. Example JSON</h2>

<pre><code>{
  "wfrog_version": "1",
  "package_kind": "widget_realization_library",
  "package": {
    "id": "frog.realizations.default.numeric_control",
    "name": "FROG Default Numeric Control Realization",
    "namespace": "frog.realizations.default",
    "publisher": "FROG",
    "summary": "Default official realization for frog.widgets.numeric_control"
  },
  "imports": [],
  "exports": {
    "realizations": [
      "frog.realizations.default.numeric_control"
    ],
    "resources": [
      "numeric_control.value_display.normal.svg",
      "numeric_control.value_display.disabled.svg",
      "numeric_control.value_display.focused.svg",
      "numeric_control.increment_button.normal.svg",
      "numeric_control.increment_button.pressed.svg",
      "numeric_control.decrement_button.normal.svg",
      "numeric_control.decrement_button.pressed.svg",
      "numeric_control.frame.normal.svg",
      "numeric_control.frame.focused.svg"
    ]
  },
  "targets": [
    {
      "class_id": "frog.widgets.numeric_control"
    }
  ],
  "realizations": [
    {
      "id": "frog.realizations.default.numeric_control",
      "family": "default",
      "target_class": "frog.widgets.numeric_control",
      "parts": [
        "root",
        "label",
        "value_display",
        "increment_button",
        "decrement_button",
        "frame"
      ],
      "states": [
        "normal",
        "disabled",
        "focused"
      ],
      "part_states": [
        {
          "part": "increment_button",
          "states": [
            "normal",
            "pressed"
          ]
        },
        {
          "part": "decrement_button",
          "states": [
            "normal",
            "pressed"
          ]
        }
      ],
      "part_bindings": [
        {
          "part": "value_display",
          "binding_kind": "resource_layer",
          "binding_target": "value_display"
        },
        {
          "part": "increment_button",
          "binding_kind": "resource_layer",
          "binding_target": "increment_button"
        },
        {
          "part": "decrement_button",
          "binding_kind": "resource_layer",
          "binding_target": "decrement_button"
        },
        {
          "part": "frame",
          "binding_kind": "resource_layer",
          "binding_target": "frame"
        }
      ],
      "state_maps": [
        {
          "target_part": "value_display",
          "state": "normal",
          "resource_refs": [
            "numeric_control.value_display.normal.svg"
          ]
        },
        {
          "target_part": "value_display",
          "state": "disabled",
          "resource_refs": [
            "numeric_control.value_display.disabled.svg"
          ],
          "fallback": "normal"
        },
        {
          "target_part": "value_display",
          "state": "focused",
          "resource_refs": [
            "numeric_control.value_display.focused.svg"
          ],
          "fallback": "normal"
        },
        {
          "target_part": "increment_button",
          "state": "normal",
          "resource_refs": [
            "numeric_control.increment_button.normal.svg"
          ]
        },
        {
          "target_part": "increment_button",
          "state": "pressed",
          "resource_refs": [
            "numeric_control.increment_button.pressed.svg"
          ],
          "fallback": "normal"
        },
        {
          "target_part": "decrement_button",
          "state": "normal",
          "resource_refs": [
            "numeric_control.decrement_button.normal.svg"
          ]
        },
        {
          "target_part": "decrement_button",
          "state": "pressed",
          "resource_refs": [
            "numeric_control.decrement_button.pressed.svg"
          ],
          "fallback": "normal"
        },
        {
          "target_part": "frame",
          "state": "normal",
          "resource_refs": [
            "numeric_control.frame.normal.svg"
          ]
        },
        {
          "target_part": "frame",
          "state": "focused",
          "resource_refs": [
            "numeric_control.frame.focused.svg"
          ],
          "fallback": "normal"
        }
      ],
      "fallbacks": {
        "disabled": "normal",
        "focused": "normal",
        "increment_button.pressed": "increment_button.normal",
        "decrement_button.pressed": "decrement_button.normal"
      },
      "host_hints": {
        "supports_host_focus_ring": true,
        "supports_host_step_buttons": true,
        "preferred_scaling": "uniform"
      }
    }
  ],
  "resources": [
    {
      "id": "numeric_control.value_display.normal.svg",
      "kind": "svg",
      "path": "./assets/numeric_control/value_display/normal.svg",
      "target_class": "frog.widgets.numeric_control",
      "target_part": "value_display",
      "target_state": "normal"
    },
    {
      "id": "numeric_control.value_display.disabled.svg",
      "kind": "svg",
      "path": "./assets/numeric_control/value_display/disabled.svg",
      "target_class": "frog.widgets.numeric_control",
      "target_part": "value_display",
      "target_state": "disabled"
    },
    {
      "id": "numeric_control.value_display.focused.svg",
      "kind": "svg",
      "path": "./assets/numeric_control/value_display/focused.svg",
      "target_class": "frog.widgets.numeric_control",
      "target_part": "value_display",
      "target_state": "focused"
    },
    {
      "id": "numeric_control.increment_button.normal.svg",
      "kind": "svg",
      "path": "./assets/numeric_control/increment_button/normal.svg",
      "target_class": "frog.widgets.numeric_control",
      "target_part": "increment_button",
      "target_state": "normal"
    },
    {
      "id": "numeric_control.increment_button.pressed.svg",
      "kind": "svg",
      "path": "./assets/numeric_control/increment_button/pressed.svg",
      "target_class": "frog.widgets.numeric_control",
      "target_part": "increment_button",
      "target_state": "pressed"
    },
    {
      "id": "numeric_control.decrement_button.normal.svg",
      "kind": "svg",
      "path": "./assets/numeric_control/decrement_button/normal.svg",
      "target_class": "frog.widgets.numeric_control",
      "target_part": "decrement_button",
      "target_state": "normal"
    },
    {
      "id": "numeric_control.decrement_button.pressed.svg",
      "kind": "svg",
      "path": "./assets/numeric_control/decrement_button/pressed.svg",
      "target_class": "frog.widgets.numeric_control",
      "target_part": "decrement_button",
      "target_state": "pressed"
    },
    {
      "id": "numeric_control.frame.normal.svg",
      "kind": "svg",
      "path": "./assets/numeric_control/frame/normal.svg",
      "target_class": "frog.widgets.numeric_control",
      "target_part": "frame",
      "target_state": "normal"
    },
    {
      "id": "numeric_control.frame.focused.svg",
      "kind": "svg",
      "path": "./assets/numeric_control/frame/focused.svg",
      "target_class": "frog.widgets.numeric_control",
      "target_part": "frame",
      "target_state": "focused"
    }
  ]
}</code></pre>

<hr/>

<h2 id="reading-notes">4. Reading Notes</h2>

<p>
This example shows a richer realization posture than the button example.
</p>

<p>
In particular:
</p>

<ul>
  <li><code>value_display</code> has widget-level states <code>normal</code>, <code>disabled</code>, and <code>focused</code>,</li>
  <li><code>increment_button</code> and <code>decrement_button</code> each have their own pressed-state realization,</li>
  <li><code>frame</code> supports focused posture,</li>
  <li>fallback remains explicit at both widget and part level.</li>
</ul>

<p>
This is exactly the kind of case that justifies a dedicated realization-publication layer rather than runtime-only conventions.
</p>

<hr/>

<h2 id="summary">5. Summary</h2>

<p>
This example provides a complete first machine-readable publication model for the default realization of <code>frog.widgets.numeric_control</code>.
</p>

<p>
It is suitable as the reference pattern for other value-carrying widgets with stateful public parts.
</p>
