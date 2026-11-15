# SPDX-License-Identifier: Apache-2.0
"""Create the redistributable ONNX inputs used by the importer checks."""
import argparse
from pathlib import Path
from onnx import TensorProto, checker, helper, save


def make_model(last_operation):
    def scalar(name):
        return helper.make_tensor_value_info(name, TensorProto.BOOL, [])

    graph = helper.make_graph(
        [helper.make_node("And", ["enabled", "mask"], ["filtered"], name="filter"),
         helper.make_node(last_operation, ["filtered", "override"], ["output"], name="combine")],
        "Boolean filter", [scalar("enabled"), scalar("override")], [scalar("output")],
        [helper.make_tensor("mask", TensorProto.BOOL, [], [True]),
         helper.make_tensor("override", TensorProto.BOOL, [], [False])],
        value_info=[scalar("filtered")])
    model = helper.make_model(graph, producer_name="Graiphic FROG importer fixture",
                              ir_version=9, opset_imports=[helper.make_opsetid("", 18)])
    checker.check_model(model, full_check=True)
    return model


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("directory", type=Path)
    directory = parser.parse_args().directory
    directory.mkdir(parents=True, exist_ok=True)
    for filename, operation in (("and-or.onnx", "Or"), ("and-xor.onnx", "Xor")):
        save(make_model(operation), directory / filename)
