# State-of-the-Art Decision Matrix

This matrix tells Codex how to combine finance/RL knowledge with observed Context results before choosing a campaign.

## Mandatory decision inputs

Codex must never choose a campaign from literature alone. It must combine:

```text
1. Context evidence
   - latest reports
   - leaderboard
   - experiment_registry
   - decisions
   - rejected/tested ideas
   - current local run state

2. Knowledge priors
   - finance cards
   - trading/risk cards
   - RL state-of-the-art cards
   - campaign blueprints
   - hypotheses

3. Runtime feasibility
   - available data
   - feature coverage
   - training cost
   - current extractor/algo support
   - data quality report
```

## Campaign choice rule

```text
if no clean baseline exists:
    launch or repair PPO/core baseline
elif a result is promising but unconfirmed:
    run confirmation with seeds/folds
elif a known failure exists:
    choose a targeted ablation that addresses the failure
elif knowledge suggests a new mechanism:
    run a probe with one axis changed and a control
else:
    do not launch; write a decision note
```

## RL algorithm choice

| Regime | First choice | Alternative | Avoid first |
| --- | --- | --- | --- |
| Long-only CAC40 continuous allocation, early V0 | PPO | RecurrentPPO | TQC/SAC before baseline |
| Need memory / latent regimes | RecurrentPPO | temporal_lstm/temporal_gru extractor | large transformer first |
| Need architecture ablation | PPO + temporal_cnn control | PPO + temporal_transformer treatment | changing reward at same time |
| Off-policy continuous control | SAC/TD3/TQC | only after memory guardrails | large replay buffer without guardrail |
| Offline trajectory learning | IQL/CQL/Decision Transformer | after trajectory dataset exists | online claims from offline-only test |
| Expensive interactions / simulator scarce | model-based/world-model ideas | Dreamer/TD-MPC2 inspired components | deploying unvalidated world models |

## Finance mechanism choice

| Finance idea | Use as | Required control | Risk |
| --- | --- | --- | --- |
| Markowitz / mean-variance | baseline, teacher, diagnostics | PPO without teacher | future leakage in covariance/returns |
| CAPM/APT/factors | attribution, residual alpha metric | raw return comparison | mistaking beta for alpha |
| Fama efficiency | anti-overfit discipline | Test set holdout | over-iteration on Test set |
| Prospect theory | asymmetric downside reward | cash collapse diagnostics | cash-only policy |
| Limits of arbitrage | risk/liquidity-aware interpretation | turnover/cost report | tolerating drawdown without reason |
| Black-Scholes/Merton/CIR/HJM | risk and regime features, not direct trading signal | simple macro/rates baseline | overengineering before baselines |

## Architecture choice rule

Codex may experiment with transformers only as a controlled architecture ablation:

```text
control: temporal_cnn
variant: temporal_transformer or factor_aware_transformer
locked: data, reward, costs, validation, execution
required diagnostics: param_count, train_speed, eval_test_gap, cash_collapse_score
```

Decision Transformer is not a first V0 campaign. It requires an offline trajectory dataset first.

## Promotion rule

A campaign result cannot become `validated` unless:

```text
multi-seed evidence exists
multi-fold evidence exists or an explicit reason explains why not
Agent vs Equal Weight vs Indice CAC40 is documented
costs, turnover, drawdown and cash exposure are documented
research hypothesis and blueprint are recorded
Test set was not used for iterative tuning
```
