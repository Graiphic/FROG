# Knowledge

This folder contains the operational knowledge layer used by Codex before creating or launching Botschild campaigns.

Codex must combine this folder with observed project evidence:

```text
Context reports + decisions + leaderboard + experiment registry
```

and then select:

```text
knowledge card -> hypothesis -> campaign blueprint -> campaign manifest
```

## Mandatory read order

```text
knowledge/STATE_OF_ART_DECISION_MATRIX.md
knowledge/index.md
knowledge/finance/
knowledge/trading/
knowledge/rl/
hypotheses/
campaign_blueprints/
```

## Rule

A literature idea is not a campaign by itself. It becomes actionable only when it is linked to:

```text
- a measurable hypothesis;
- a single main axis;
- locked axes;
- a control/baseline;
- success and invalidation criteria.
```

## Finance coverage

The current V0 knowledge layer covers:

```text
mean-variance allocation
factor risk and alpha attribution
behavioral downside / overreaction
regime-based risk budget
derivatives-inspired risk measures
offline-to-online RL
world-model RL direction
RL safety and robustness
transformer sequence modeling
PPO / RecurrentPPO baselines
```

## Decision rule

Codex must not launch a complex architecture campaign, such as transformer or world-model inspired work, before a clean PPO/core baseline and an appropriate control campaign exist.
