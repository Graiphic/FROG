#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

clang module.ll -o boolean_value_roundtrip_llvm

run_case() {
  local input="$1"
  local expected_input="$2"
  local expected_output="$3"

  output="$(./boolean_value_roundtrip_llvm "$input")"
  grep -qx "input_value=${expected_input}" <<<"$output"
  grep -qx "public_output=${expected_output}" <<<"$output"
  grep -qx "status=ok" <<<"$output"
}

run_case 0 false false
run_case 1 true true

echo "Example 06 LLVM build check: ok"
