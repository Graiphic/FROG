#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

clang module.ll -o ui_value_roundtrip_llvm

check_case() {
  local args="$1"
  local expected="$2"
  local output
  # shellcheck disable=SC2086
  output="$(./ui_value_roundtrip_llvm $args)"
  output="${output//$'\r'/}"
  printf '%s\n' "$output"
  if [[ "$output" != "$expected" ]]; then
    echo "Unexpected LLVM proof output for 02_ui_value_roundtrip with args '$args'." >&2
    exit 1
  fi
}

check_case "1.0 2.0" $'widget.ind_result.value=3.000000
status=ok'

check_case "4.5 5.5" $'widget.ind_result.value=10.000000
status=ok'
