"""Compare typed kernel observations against static independent corpus oracles.

No Source/FIR evaluation, compiler, bridge implementation or backend is present.
JSON number tokens are retained so an I64/U64 can never pass through float.
"""
import argparse
from dataclasses import dataclass
import json
import math
from pathlib import Path
import re
import struct
import sys


@dataclass(frozen=True)
class Number:
    token: str


def require(condition, message):
    if not condition:
        raise AssertionError(message)


def unique_object(pairs):
    result = {}
    for key, value in pairs:
        require(key not in result, f"Duplicate JSON field: {key}")
        result[key] = value
    return result


def reject_non_json_number(token):
    raise AssertionError(f"Non-JSON numeric token: {token}")


def load_output(path):
    data = Path(path).read_bytes()
    require(len(data) <= 16 * 1024 * 1024, "Typed observation exceeds the finite comparison budget")
    return json.loads(data.decode("utf-8"), parse_int=Number, parse_float=Number,
                      parse_constant=reject_non_json_number, object_pairs_hook=unique_object)


def integer(value, low, high, where):
    require(isinstance(value, Number) and re.fullmatch(r"-?(?:0|[1-9][0-9]*)", value.token) is not None,
            f"{where}: an exact integer JSON token is required")
    result = int(value.token)
    require(low <= result <= high, f"{where}: integer outside its exact declared carrier")
    return result


def f64_bits(value, where):
    require(isinstance(value, Number), f"{where}: a JSON numeric token is required for f64")
    result = float(value.token)  # Used only after the declared type has been checked as f64.
    require(math.isfinite(result), f"{where}: this finite corpus has no nonfinite output oracle")
    return struct.pack(">d", result)


INTEGER_RANGES = {
    "i32": (-(1 << 31), (1 << 31) - 1),
    "u32": (0, (1 << 32) - 1),
    "i64": (-(1 << 63), (1 << 63) - 1),
    "u64": (0, (1 << 64) - 1),
}


def compare_value(actual, expected, canonical_type, where):
    if canonical_type in INTEGER_RANGES:
        low, high = INTEGER_RANGES[canonical_type]
        require(integer(actual, low, high, where) == integer(expected, low, high, where + " oracle"),
                f"{where}: exact {canonical_type} output differs")
    elif canonical_type == "bool":
        require(type(actual) is bool and type(expected) is bool and actual is expected,
                f"{where}: bool output differs or was converted to a number")
    elif canonical_type == "f64":
        require(f64_bits(actual, where) == f64_bits(expected, where + " oracle"),
                f"{where}: IEEE f64 output differs (including signed zero)")
    elif canonical_type == "array<f64>":
        for label, value in ((where, actual), (where + " oracle", expected)):
            require(type(value) is dict and set(value) == {"shape", "elements"}, f"{label}: exact Array envelope required")
            require(type(value["shape"]) is list and len(value["shape"]) == 1 and type(value["elements"]) is list,
                    f"{label}: rank-one Array required")
            count = integer(value["shape"][0], 0, 32, label + " shape")
            require(len(value["elements"]) == count, f"{label}: Array shape/element count mismatch")
        require(integer(actual["shape"][0], 0, 32, where) == integer(expected["shape"][0], 0, 32, where + " oracle"),
                f"{where}: exact Array shape differs")
        for index, (value, wanted) in enumerate(zip(actual["elements"], expected["elements"])):
            compare_value(value, wanted, "f64", f"{where}[{index}]")
    else:
        raise AssertionError(f"{where}: no declared corpus oracle for type {canonical_type!r}")


def output_map(document, label):
    require(type(document) is dict, f"{label}: object required")
    required = {"schema", "schema_version", "outputs", "outcome"}
    require(required <= set(document) <= required | {"diagnostic"}, f"{label}: unexpected output-envelope fields")
    require(document["schema"] == "frog.bridge.kernel.outputs" and document["schema_version"] == "0.1",
            f"{label}: unsupported kernel output contract")
    require(document["outcome"] in ("completed", "fault") and type(document["outputs"]) is list,
            f"{label}: explicit outcome and output list required")
    result = {}
    for row in document["outputs"]:
        require(type(row) is dict and set(row) == {"node_id", "port_id", "type", "value"},
                f"{label}: output must retain its exact identity, canonical type and value")
        require(all(type(row[key]) is str and row[key] for key in ("node_id", "port_id", "type")),
                f"{label}: empty or non-string output identity/type")
        identity = (row["node_id"], row["port_id"])
        require(identity not in result, f"{label}: duplicated output {identity!r}")
        result[identity] = row
    return result


def check_outputs(actual, expected, label="actual"):
    observed, wanted = output_map(actual, label), output_map(expected, "oracle")
    require(actual["outcome"] == expected["outcome"], f"{label}: outcome differs from independent oracle")
    require(set(observed) == set(wanted),
            f"{label}: complete output identity set differs; missing={set(wanted)-set(observed)!r}, extra={set(observed)-set(wanted)!r}")
    if expected["outcome"] == "fault":
        require(not observed, f"{label}: a fault must not publish a partial output prefix")
        diagnostic = actual.get("diagnostic")
        require(type(diagnostic) is dict and set(diagnostic) == {"code", "message", "object_id", "source_pointer"},
                f"{label}: attributable fault diagnostic required")
        require(type(diagnostic["message"]) is str and bool(diagnostic["message"]), f"{label}: fault message is absent")
        require(all(diagnostic[key] == expected["diagnostic"][key] for key in ("code", "object_id", "source_pointer")),
                f"{label}: fault code/object/Source pointer differs from oracle")
    else:
        require("diagnostic" not in actual and "diagnostic" not in expected, f"{label}: completed output contains an unexpected fault")
    for identity, row in observed.items():
        oracle = wanted[identity]
        require(row["type"] == oracle["type"], f"{label} {identity!r}: exact canonical type differs")
        compare_value(row["value"], oracle["value"], row["type"], f"{label} {identity!r}")
    return len(wanted)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--expected", type=Path, required=True)
    parser.add_argument("--actual", type=Path, required=True)
    parser.add_argument("--compare", type=Path, help="Optional second backend observation, checked against the same oracle")
    args = parser.parse_args()
    expected = load_output(args.expected)
    count = check_outputs(load_output(args.actual), expected, "first backend")
    if args.compare:
        check_outputs(load_output(args.compare), expected, "second backend")
    print(json.dumps({"status": "passed", "checked_outputs": count, "backends": 2 if args.compare else 1,
                      "outcome": expected["outcome"], "integer_carriers_use_float": False}))


if __name__ == "__main__":
    try:
        main()
    except (AssertionError, ValueError, OSError) as error:
        print(f"typed-output comparison failed: {error}", file=sys.stderr)
        sys.exit(1)
