# SPDX-License-Identifier: Apache-2.0
"""Redistributable equivalent cases; these are not the missing video originals."""
import argparse
import json
from pathlib import Path
import numpy as np
from onnx import TensorProto as T, checker, helper as h, numpy_helper, save
from onnx.reference import ReferenceEvaluator


def adder(shape=(), *, ir=9, opset=18):
    nodes = []
    def op(kind, a, b, out):
        nodes.append(h.make_node(kind, [a, b], [out], name=kind + "_" + out))
    op("Xor", "a0", "b0", "sum0_raw")
    op("And", "a0", "b0", "carry0")
    op("Xor", "a1", "b1", "different1")
    op("Xor", "different1", "carry0", "sum1_raw")
    op("And", "a1", "b1", "both1")
    op("And", "different1", "carry0", "propagate1")
    op("Or", "both1", "propagate1", "carry_raw")
    # Add a second, logically significant path using an overridable initializer.
    op("Xor", "sum0_raw", "mask", "masked0")
    # Five exact NOT pairs exercise a nontrivial 18-operation DAG without
    # pretending to reconstruct an unavailable user's source model.
    for source, output in [("masked0", "sum0"), ("sum1_raw", "sum1"),
                           ("carry_raw", "carry_stage"), ("carry_stage", "carry_stage2"),
                           ("carry_stage2", "carry_out")]:
        nodes.extend([h.make_node("Not", [source], [output + "_not"], name="Not_" + output),
                      h.make_node("Not", [output + "_not"], [output], name="Restore_" + output)])
    vi = lambda name: h.make_tensor_value_info(name, T.BOOL, list(shape))
    initial = numpy_helper.from_array(np.zeros(shape, dtype=np.bool_), name="mask")
    model = h.make_model(h.make_graph(nodes, "Equivalent two-bit adder", [vi(x) for x in ["a0", "a1", "b0", "b1", "mask"]],
                                     [vi(x) for x in ["sum0", "sum1", "carry_out"]], [initial]),
                         producer_name="Graiphic redistributable equivalent fixture", ir_version=ir,
                         opset_imports=[h.make_opsetid("", opset)])
    checker.check_model(model, full_check=True)
    return model


def generate(directory):
    directory.mkdir(parents=True, exist_ok=True)
    model = adder()
    save(model, directory / "equivalent-adder18.onnx")
    # An unchanged, already admitted And/Or/Not graph for before/after layout.
    nodes = []
    def binary(kind, a, b, out): nodes.append(h.make_node(kind, [a, b], [out], name=kind+"_"+out))
    def xor(a, b, out):
        nodes.extend([h.make_node("Not", [a], [out+"_not_a"]), h.make_node("Not", [b], [out+"_not_b"])])
        binary("And", a, out+"_not_b", out+"_left")
        binary("And", out+"_not_a", b, out+"_right")
        binary("Or", out+"_left", out+"_right", out)
    xor("a0", "b0", "sum0"); binary("And", "a0", "b0", "carry0")
    xor("a1", "b1", "different1"); xor("different1", "carry0", "sum1")
    binary("And", "a1", "b1", "both1"); binary("And", "different1", "carry0", "propagate1")
    binary("Or", "both1", "propagate1", "carry_out")
    vi = lambda n: h.make_tensor_value_info(n, T.BOOL, [])
    expanded = h.make_model(h.make_graph(nodes, "Equivalent expanded adder", [vi(x) for x in ["a0", "a1", "b0", "b1"]],
                           [vi(x) for x in ["sum0", "sum1", "carry_out"]]),ir_version=9,opset_imports=[h.make_opsetid("",18)])
    checker.check_model(expanded,full_check=True);save(expanded,directory/"equivalent-expanded-adder19.onnx")
    rows = []
    oracle = ReferenceEvaluator(model)
    expanded_oracle = ReferenceEvaluator(expanded)
    for row in range(16):
        values = {name: np.asarray(bool((row >> i) & 1)) for i, name in enumerate(["a0", "a1", "b0", "b1"])}
        result = oracle.run(None, values)
        number = (row & 3) + ((row >> 2) & 3)
        expected = [bool(number & 1), bool(number & 2), bool(number & 4)]
        actual = [bool(x) for x in result]
        if actual != expected: raise AssertionError("Independent integer-adder oracle disagrees with ONNX")
        if [bool(x) for x in expanded_oracle.run(None, values)] != expected:
            raise AssertionError("Expanded adder disagrees with the independent integer oracle")
        rows.append({"inputs": {k: bool(v) for k, v in values.items()}, "expected": dict(zip(["sum0", "sum1", "carry_out"], actual))})
    (directory / "equivalent-adder18-oracles.json").write_text(json.dumps(rows, indent=2) + "\n", encoding="utf-8")
    save(adder((1,), ir=13, opset=25), directory / "equivalent-adder18-vector1.onnx")
    a, b = h.make_tensor_value_info("vector", T.BOOL, [3]), h.make_tensor_value_info("mask", T.BOOL, [])
    z = h.make_tensor_value_info("output", T.BOOL, [3])
    defaults = [numpy_helper.from_array(np.asarray([True, False, True]), name="vector"), h.make_tensor("mask", T.BOOL, [], [False])]
    broadcast = h.make_model(h.make_graph([h.make_node("Xor", ["vector", "mask"], ["output"])], "Scalar to array XOR",
                                        [a, b], [z], defaults), ir_version=13, opset_imports=[h.make_opsetid("", 25)])
    checker.check_model(broadcast, full_check=True); save(broadcast, directory / "xor-broadcast.onnx")
    unsupported = h.make_model(h.make_graph([h.make_node("Equal", ["x", "y"], ["z"])], "Unsupported equality",
                    [h.make_tensor_value_info(x, T.BOOL, []) for x in ["x", "y"]], [h.make_tensor_value_info("z", T.BOOL, [])]),
                    ir_version=9, opset_imports=[h.make_opsetid("", 18)])
    checker.check_model(unsupported, full_check=True); save(unsupported, directory / "outside-profile.onnx")
    (directory / "pure-function.c").write_text("unsigned int calculate(unsigned int x, unsigned int y) { return (x + 7u) * 3u + y; }\n", encoding="utf-8")
    return rows


if __name__ == "__main__":
    p = argparse.ArgumentParser(description=__doc__); p.add_argument("directory", type=Path)
    generate(p.parse_args().directory)
