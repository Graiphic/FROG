#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

clang module.ll -o bounded_ui_accumulator_llvm

check_ok_case() {
  local input="$1"
  local expected_value="$2"
  local output
  output="$(./bounded_ui_accumulator_llvm "$input")"
  printf '%s\n' "$output"

  local expected
  expected="$(printf 'final_state=%s\npublic_output=%s\nstatus=ok' "$expected_value" "$expected_value")"

  if [[ "$output" != "$expected" ]]; then
    echo "Unexpected LLVM proof output for input ${input}." >&2
    exit 1
  fi
}

check_error_case() {
  local input="$1"
  local output
  local status

  set +e
  output="$(./bounded_ui_accumulator_llvm "$input")"
  status="$?"
  set -e

  printf '%s\n' "$output"

  local expected
  expected="$(printf 'status=error\nerror=final_state must remain in the u16 domain.')"

  if [[ "$status" != "1" ]]; then
    echo "Expected LLVM proof to reject input ${input} with exit status 1, got ${status}." >&2
    exit 1
  fi

  if [[ "$output" != "$expected" ]]; then
    echo "Unexpected LLVM proof error output for input ${input}." >&2
    exit 1
  fi
}

check_ok_case 0 0
check_ok_case 3 15
check_ok_case 7 35
check_error_case 20000
