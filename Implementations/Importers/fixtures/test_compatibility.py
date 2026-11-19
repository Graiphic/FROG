# SPDX-License-Identifier: Apache-2.0
"""Finite importer/Frontend checks. Runtime/UI qualification is a separate test."""
import argparse
import ctypes
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile
from types import SimpleNamespace
import numpy as np
import onnx
from onnx import TensorProto as T, helper as h, numpy_helper
from onnx.reference import ReferenceEvaluator


def load(name, path):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec); spec.loader.exec_module(module)
    return module


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--frontend", type=Path, required=True); p.add_argument("--clang", type=Path, required=True)
    args = p.parse_args(); importers = Path(__file__).resolve().parents[1]
    tool = load("onnx_import", importers / "Import-OnnxGraph.py")
    c = load("c_import", importers / "Import-CProject.py")
    fixtures = load("boolean_fixtures", Path(__file__).with_name("boolean_compatibility.py"))
    cases = 0
    def check(value, label):
        nonlocal cases
        if not value: raise AssertionError(label)
        cases += 1; print("IMPORT_COMPATIBILITY_PASS", label, flush=True)
    def model(shape=(), *, raw=False, ir=9, opset=18, operator="Xor", override=False):
        array = np.ones(shape, dtype=np.bool_)
        initializer = numpy_helper.from_array(array, name="mask") if raw else h.make_tensor("mask", T.BOOL, list(shape), array.reshape(-1).tolist())
        inputs = [h.make_tensor_value_info("x", T.BOOL, list(shape))]
        if override: inputs.append(h.make_tensor_value_info("mask", T.BOOL, list(shape)))
        return h.make_model(h.make_graph([h.make_node(operator, ["x", "mask"], ["result"])], "Logical fixture", inputs,
                                        [h.make_tensor_value_info("result", T.BOOL, list(shape))], [initializer]),
                           ir_version=ir, opset_imports=[h.make_opsetid("", opset)])
    def refusal(m, suffix):
        try: tool.load_model(m.SerializeToString())
        except tool.ImportRefusal as error: return error.diagnostic["code"] == "onnx_import." + suffix
        return False
    for ir in (7, 9, 10, 11, 12, 13):
        for opset in (7, 18, 25):
            m = model(ir=ir, opset=opset); before = m.SerializeToString()
            admitted = tool.load_model(before)
            check(admitted.ir_version == ir and admitted.opset_import[0].version == opset and m.SerializeToString() == before,
                  f"schema_versions_ir{ir}_opset{opset}_original_unchanged")
    for shape in ((), (1,), (3,)):
        for raw in (False, True):
            m = tool.load_model(model(shape, raw=raw, override=True).SerializeToString())
            doc, receipt = tool.convert(m, "a" * 64)
            typed = [w for w in doc["frontPanel"]["widgets"] if w["role"] != "support"]
            check(receipt["initializers"][0]["shape"] == list(shape) and receipt["initializers"][0]["overridable"] and
                  all(("array" in w["valueType"]) == bool(shape) for w in typed), f"bool_encoding_shape_{shape}_raw{raw}")
    for x in (False, True):
        for y in (False, True):
            m = model(override=True)
            actual = ReferenceEvaluator(m).run(None, {"x": np.asarray(x), "mask": np.asarray(y)})[0].item()
            doc, _ = tool.convert(tool.load_model(m.SerializeToString()), "b" * 64)
            check(actual == (x != y) and any(n.get("type") == "frog.core.xor" for n in doc["diagram"]["nodes"]), f"xor_exact_truth_{int(x)}{int(y)}")
    check("viewport_origin" not in doc["diagram"], "neutral_camera_uses_native_writer_omission_for_lossless_save")
    check(refusal(model(ir=14), "profile_unavailable"), "future_ir_refused_without_rewrite")
    check(refusal(model(opset=26), "profile_unavailable"), "future_opset_refused_without_rewrite")
    check(refusal(model((1, 1)), "initializer_unavailable"), "rank2_not_flattened")
    bad = model(); bad.graph.initializer[0].ClearField("int32_data"); bad.graph.initializer[0].raw_data = b""
    check(refusal(bad, "initializer_value"), "malformed_bool_encoding_refused")
    broadcast = h.make_model(h.make_graph([h.make_node("Xor", ["x", "y"], ["z"])], "Unsupported broadcast",
                 [h.make_tensor_value_info("x", T.BOOL, [1]), h.make_tensor_value_info("y", T.BOOL, [3])],
                 [h.make_tensor_value_info("z", T.BOOL, [3])]), ir_version=9, opset_imports=[h.make_opsetid("", 18)])
    check(refusal(broadcast, "broadcast_unavailable"), "array_to_array_broadcast_not_truncated")
    with tempfile.TemporaryDirectory(prefix="frog-import-compatibility-") as temporary:
        root = Path(temporary); fixtures.generate(root)
        external = model(); tensor = external.graph.initializer[0]; tensor.ClearField("int32_data")
        tensor.data_location = T.EXTERNAL; tensor.external_data.add(key="location", value="missing-bool.bin")
        report = tool.analyze(external.SerializeToString(), root)
        check(report["diagnostics"][0]["code"] == "onnx_import.external_resource_missing", "external_resource_missing_localized")
        for file in ("equivalent-adder18.onnx", "equivalent-expanded-adder19.onnx", "equivalent-adder18-vector1.onnx", "xor-broadcast.onnx"):
            source = root / file; output = root / (file + "-frog"); before = source.read_bytes()
            result = subprocess.run([sys.executable, "-I", "-B", str(importers / "Import-OnnxGraph.py"), str(source),
                       "--frontend", str(args.frontend), "--output", str(output)], capture_output=True, text=True, encoding="utf-8", timeout=40)
            check(result.returncode == 0 and (output / "model.frog").is_file() and source.read_bytes() == before,
                  "frontend_admission_"+file+": "+result.stdout+result.stderr)
            report = json.loads((output / "onnx-mapping.json").read_text(encoding="utf-8"))
            check(report["transformations"] == [] and report["onnx_sha256"] == hashlib.sha256(before).hexdigest(), "source_identity_"+file)
        source = root / "equivalent-adder18.onnx"
        changed = subprocess.run([sys.executable, "-I", "-B", str(importers / "Import-OnnxGraph.py"), str(source),
            "--frontend", str(args.frontend), "--output", str(root/"changed"), "--expected-sha256", "0"*64], capture_output=True, text=True)
        check(changed.returncode == 2 and "onnx_import.source_changed" in changed.stdout and not (root/"changed").exists(), "changed_source_refused_atomically")
        header = root / "includes"; header.mkdir(); (header/"shared.h").write_text("typedef unsigned int U32;\n", encoding="utf-8")
        source = root / "functions.c"
        source.write_text('#include "shared.h"\nstatic U32 refine(U32 x){return x+7u;}\nU32 calculate(U32 x,U32 y){return refine(x)*3u+y;}\n_Bool gate(_Bool x,_Bool y){return x && !y;}\n#ifndef FROG_REFERENCE_ONLY\n_Bool effect(_Bool x,_Bool y){return x && y++;}\nfloat floating(float x){return x+1.0f;}\n#endif\n', encoding="utf-8")
        options = SimpleNamespace(source=source, clang=args.clang, include=[header], define=[], expected_sha256="")
        analysis = c.analyze_single_unit(options); entries = {x["name"]: x for x in analysis["functions"]}
        check(entries["calculate"]["compatible"] and entries["gate"]["compatible"] and not entries["effect"]["compatible"] and
              not entries["floating"]["compatible"] and analysis["code_executed"] is False, "clang_signatures_typedefs_effects_and_floats_distinct")
        options.include=[]; missing=c.analyze_single_unit(options)
        check(missing["diagnostics"][0]["code"] == "c_project.header_missing", "missing_header_is_not_missing_function")
        options.include=[header]
        for entry in ("calculate", "gate"):
            output=root/entry
            cmd=[sys.executable,"-I","-B",str(importers/"Import-CProject.py"),"--single-file","--source",str(source),
                 "--clang",str(args.clang),"--include",str(header),"--entry",entry,"--frontend",str(args.frontend),"--output",str(output)]
            result=subprocess.run(cmd,capture_output=True,text=True,encoding="utf-8",timeout=40)
            check(result.returncode==0 and (output/"main.fir.json").is_file() and (output/"original.c").read_bytes()==source.read_bytes(), "pure_c_frontend_"+entry+": "+result.stderr)
        for entry,diagnostic in (("absent","c_project.entry"),("effect","c_project.operation"),("floating","c_project.signature")):
            result=subprocess.run([sys.executable,"-I","-B",str(importers/"Import-CProject.py"),"--single-file","--source",str(source),
                "--clang",str(args.clang),"--include",str(header),"--entry",entry,"--frontend",str(args.frontend),"--output",str(root/entry)],capture_output=True,text=True,encoding="utf-8",timeout=40)
            check(result.returncode==2 and diagnostic in result.stderr and not (root/entry).exists(), "pure_c_refusal_"+entry)
        invalid=root/"invalid.c";invalid.write_text("this is not valid C @@@",encoding="utf-8");options.source=invalid
        report=c.analyze_single_unit(options)
        check(report["status"]=="invalid" and report["diagnostics"][0]["code"]=="c_project.clang", "syntax_error_is_not_missing_function")
        # A deliberately owned, no-entry/no-import reference DLL, never loaded
        # by analysis or by Studio. This executes the original C bodies only
        # during qualification on their checked Windows x64 ABI.
        if os.name=="nt" and ctypes.sizeof(ctypes.c_void_p)==8:
            reference=root/"reference.c";reference.write_text(source.read_text(encoding="utf-8").replace("U32 calculate", "__declspec(dllexport) U32 calculate").replace("_Bool gate", "__declspec(dllexport) _Bool gate"),encoding="utf-8")
            obj=root/"reference.obj";dll=root/"reference.dll"
            subprocess.run([str(args.clang),"/nologo","/TC","/std:c17","/O2","/c","/GS-","/Zl","/DFROG_REFERENCE_ONLY","/I"+str(header),str(reference),"/Fo"+str(obj)],check=True,capture_output=True,timeout=30)
            linked=subprocess.run([str(args.clang.with_name("lld-link.exe")),"/dll","/noentry","/nodefaultlib","/machine:x64","/out:"+str(dll),str(obj)],capture_output=True,timeout=30)
            if linked.returncode: print(linked.stdout.decode(errors="replace"),linked.stderr.decode(errors="replace"),flush=True)
            linked.check_returncode()
            library=ctypes.CDLL(str(dll));library.calculate.argtypes=[ctypes.c_uint32,ctypes.c_uint32];library.calculate.restype=ctypes.c_uint32
            library.gate.argtypes=[ctypes.c_bool,ctypes.c_bool];library.gate.restype=ctypes.c_bool
            rows=[]
            for x,y in [(0,0),(5,11),(29,1),(4294967295,4294967295)]:
                actual=library.calculate(x,y);expected=((x+7)*3+y)&0xffffffff;rows.append([x,y,actual]);check(actual==expected,"compiled_c_u32_reference_"+str(x))
            for x in (False,True):
                for y in (False,True):check(library.gate(x,y)==(x and not y),"compiled_c_bool_reference_"+str(int(x))+str(int(y)))
            kernel=ctypes.WinDLL("kernel32",use_last_error=True);kernel.FreeLibrary.argtypes=[ctypes.c_void_p];kernel.FreeLibrary.restype=ctypes.c_int
            check(bool(kernel.FreeLibrary(library._handle)),"owned_reference_dll_released")
            print("COMPILED_C_REFERENCE_ROWS",json.dumps(rows))
    print("IMPORT_COMPATIBILITY_COMPLETE cases="+str(cases)+" runtime_ui_not_claimed=1")
    return 0


if __name__=="__main__":
    raise SystemExit(main())
