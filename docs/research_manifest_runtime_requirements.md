# Research Manifest Runtime Requirements

This document records the operational requirements that make research-driven campaigns executable and auditable.

## Required behavior

`run_campaign.py` must support legacy manifests and research-driven manifests.

When a manifest includes a `research` block, the runner must verify at least:

```text
research.campaign_type
research.hypothesis_id
research.knowledge_cards
research.blueprint_id
research.main_axis
research.locked_axes
```

Allowed `research.campaign_type` values:

```text
smoke_test
probe
ablation
confirmation
validation
diagnostic_only
```

If `research.campaign_type` is `diagnostic_only`, the campaign must also declare:

```text
campaign.allow_diagnostic_reward = true
```

## Required artifacts

When a research block is present, the run directory must expose a short machine-readable summary:

```text
campaign_research.json
```

`campaign_launch.json` should also include:

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

## Validation command

Until this validation is embedded directly in `run_campaign.py`, Codex must check each research-driven manifest against:

```text
schemas/campaign_manifest.schema.json
```

and report this check in `Botshild-Context`.

## Rule

A research-driven campaign is not considered launch-ready if the `research` block is missing, incomplete, or inconsistent with the selected hypothesis and blueprint.
