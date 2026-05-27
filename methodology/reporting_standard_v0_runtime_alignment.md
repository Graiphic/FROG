# Reporting Standard V0 Runtime Alignment

This file resolves the legacy vocabulary ambiguity in `methodology/reporting_standard.md`.

## Current V0 vocabulary

The active V0 orchestration model is:

```text
Codex automation -> campaign manifest -> scripts/run_campaign.py -> Botschild Python trainer -> outputs -> Botshild-Context report and decision
```

The following terms should be used in new reports:

```text
orchestrator_name   = human-readable name for the Codex automation
orchestrator_id     = stable automation identifier
machine_id          = stable machine identifier
runtime             = codex_automation
trainer             = Botschild Python runner
entrypoint          = scripts/run_campaign.py
campaign_manifest   = JSON file in Botschild/campaigns/
```

## Legacy compatibility

Older templates may still contain:

```text
agent_name
agent_id
agent_runtime
```

In V0 reports, these fields are compatibility aliases only. They should not imply a second intelligent CLI agent.

Recommended mapping:

```text
agent_name        -> Codex automation name or trainer process label
agent_id          -> Codex automation id or run process id
agent_runtime     -> codex_automation
```

## Research-driven campaign fields

New reports should include these fields when a campaign manifest contains a `research` block:

```text
hypothesis_id
knowledge_cards
blueprint_id
main_axis
locked_axes
campaign_type
claim_tested
known_risks
research_contract_status
```

## Rule

`Botshild-Context` is the source of truth for knowledge, hypotheses, blueprints, reports and decisions.

`Botschild` is the runtime that executes campaign manifests and produces local artifacts.
