# Runtime Patch Required for Research Manifests

This repository now documents research-driven campaign manifests, but the runtime should also enforce the research contract directly in `scripts/run_campaign.py` or `src/cac40_rl/training/campaign.py`.

## Required runtime behavior

When a campaign manifest includes a `research` block, the runner must reject the campaign unless these fields are present:

```text
campaign_type
hypothesis_id
knowledge_cards
blueprint_id
main_axis
locked_axes
```

Required types:

```text
campaign_type: non-empty string
hypothesis_id: non-empty string
knowledge_cards: non-empty list of strings
blueprint_id: non-empty string
main_axis: non-empty string
locked_axes: non-empty list of strings
```

Allowed campaign types:

```text
smoke_test
probe
ablation
confirmation
validation
diagnostic_only
```

## Required launch artifacts

When a research block is present, the run directory must include:

```text
campaign_research.json
```

and `campaign_launch.json` must include:

```json
{
  "research_contract_status": "passed",
  "research": {
    "campaign_type": "probe",
    "hypothesis_id": "HYP-...",
    "knowledge_cards": ["FIN-...", "RL-..."],
    "blueprint_id": "BP-...",
    "main_axis": "reward",
    "locked_axes": ["data", "model", "costs", "validation"]
  }
}
```

## Why this matters

The research block is the bridge between:

```text
Botshild-Context knowledge/hypotheses/blueprints
```

and:

```text
Botschild executable training runs
```

Without runtime enforcement, a malformed research-driven campaign could still run as a legacy manifest.

## Codex instruction

Before launching the first research-driven campaign, Codex should implement this validation or explicitly confirm that the runner already enforces it.
