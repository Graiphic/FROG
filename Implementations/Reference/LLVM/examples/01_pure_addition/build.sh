#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

clang module.ll -o pure_addition_llvm

check_case() {
  local args="$1"
  local expected="$2"
  local output
  # shellcheck disable=SC2086
  output="$(./pure_addition_llvm $args)"
  printf '%s\n' "$output"
  if [[ "$output" != "$expected" ]]; then
    echo "Unexpected LLVM proof output for 01_pure_addition with args '$args'." >&2
    exit 1
  fi
}

check_case "2.25 3.75" $'result=6.000000
status=ok'

check_case "-1.5 2.0" $'result=0.500000
status=ok'
