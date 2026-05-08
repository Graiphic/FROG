#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

clang module.ll -o stateful_feedback_delay_llvm

check_case() {
  local args="$1"
  local expected="$2"
  local output
  # shellcheck disable=SC2086
  output="$(./stateful_feedback_delay_llvm $args)"
  output="${output//$'\r'/}"
  printf '%s\n' "$output"
  if [[ "$output" != "$expected" ]]; then
    echo "Unexpected LLVM proof output for 04_stateful_feedback_delay with args '$args'." >&2
    exit 1
  fi
}

check_case "2.5" $'public_output.y=2.500000
state.delay_1=2.500000
status=ok'

check_case "7.25" $'public_output.y=7.250000
state.delay_1=7.250000
status=ok'
