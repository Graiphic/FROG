#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

clang module.ll -o ui_property_write_llvm

check_case() {
  local args="$1"
  local expected="$2"
  local output
  # shellcheck disable=SC2086
  output="$(./ui_property_write_llvm $args)"
  output="${output//$'\r'/}"
  printf '%s\n' "$output"
  if [[ "$output" != "$expected" ]]; then
    echo "Unexpected LLVM proof output for 03_ui_property_write with args '$args'." >&2
    exit 1
  fi
}

check_case "Ready" $'widget=ctrl_gain
member=label.text
value=Ready
status=ok'

check_case "Running" $'widget=ctrl_gain
member=label.text
value=Running
status=ok'
