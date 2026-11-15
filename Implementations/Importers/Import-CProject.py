# SPDX-License-Identifier: Apache-2.0
"""Translate an existing CMake C target's reachable scalar functions into editable FROG."""

import argparse
from collections import defaultdict
import hashlib
from html import escape
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import sys
from uuid import uuid4


class Refusal(ValueError):
    pass


def refuse(code, location, detail):
    raise Refusal(f"{location}: c_project.{code}: {detail}")


def read_json(path):
    try:
        with path.open("rb") as source:
            data = source.read(32 * 1024 * 1024 + 1)
        if len(data) > 32 * 1024 * 1024:
            refuse("metadata_limit", path, "JSON exceeds 32 MiB")
        return json.loads(data)
    except (OSError, json.JSONDecodeError) as error:
        refuse("metadata", path, error)


def reply_file(reply, name):
    if not re.fullmatch(r"[A-Za-z0-9_.-]+\.json", name):
        refuse("metadata", reply, f"Invalid reply file name {name!r}")
    return reply / name


def cmake_target(database, configuration, target):
    reply = database.parent / ".cmake/api/v1/reply"
    indices = sorted(reply.glob("index-*.json"))
    if not indices:
        refuse("codemodel_missing", database,
               "Configure the project explicitly with a codemodel-v2 File API query and CMAKE_EXPORT_COMPILE_COMMANDS=ON")
    index = read_json(indices[-1])
    model = index.get("reply", {}).get("codemodel-v2")
    if model is None:
        refuse("codemodel_missing", indices[-1], "Request codemodel-v2 before configuring")
    model = read_json(reply_file(reply, model["jsonFile"]))
    if model["version"]["major"] != 2:
        refuse("codemodel_version", reply, "Codemodel major version 2 is required")
    if Path(model["paths"]["build"]).resolve() != database.parent:
        refuse("build_directory", database, "The codemodel belongs to a different CMake build directory")
    generator = index.get("cmake", {}).get("generator", {})
    if generator.get("name") not in {"Ninja", "Ninja Multi-Config"}:
        refuse("generator", database, f"Expected a Ninja CMake generator, got {generator.get('name')!r}")
    configs = {c["name"]: c for c in model["configurations"]}
    if configuration not in configs:
        refuse("configuration", database, f"Choose a configuration from {sorted(configs)} (use an empty name for an unnamed configuration)")
    entries = configs[configuration]["targets"]
    by_name = {e["name"]: e for e in entries}
    by_id = {e["id"]: e for e in entries}
    if target not in by_name:
        refuse("target", database, f"Target {target!r} is absent in {configuration!r}; available: {', '.join(sorted(by_name))}")
    root = Path(model["paths"]["source"]).resolve()
    files = {}
    visited = set()
    pending = [by_name[target]]
    external_links = set()
    while pending:
        entry = pending.pop()
        if entry["id"] in visited:
            continue
        visited.add(entry["id"])
        model_target = read_json(reply_file(reply, entry["jsonFile"]))
        if model_target["type"] not in {"EXECUTABLE", "STATIC_LIBRARY", "OBJECT_LIBRARY"}:
            refuse("dependency_profile", entry["name"], f"CMake target type {model_target['type']} is outside the static C profile")
        backtrace = model_target.get("backtraceGraph", {})
        linked_ids = []
        for dep in model_target.get("dependencies", []):
            if dep["id"] not in by_id:
                refuse("dependency_missing", entry["name"], dep["id"])
            # Build order alone does not put a definition in the link. In
            # particular, add_dependencies may only prepare a generated header.
            bt = dep.get("backtrace")
            if bt is None:
                refuse("dependency_profile", entry["name"],
                       f"CMake did not expose the link origin of {by_id[dep['id']]['name']}")
            command = backtrace["nodes"][bt].get("command")
            kind = backtrace["commands"][command] if command is not None else None
            if kind == "add_dependencies":
                continue
            if kind != "target_link_libraries":
                refuse("dependency_profile", entry["name"],
                       f"Unsupported CMake dependency declaration {kind!r} for {by_id[dep['id']]['name']}")
            pending.append(by_id[dep["id"]])
            linked_ids.append(dep["id"])
        owned_artifacts = set()
        for dep_id in linked_ids:
            dep = read_json(reply_file(reply, by_id[dep_id]["jsonFile"]))
            owned_artifacts.update(os.path.normcase(a["path"].replace("\\", "/"))
                                   for a in dep.get("artifacts", []))
        for flag in model_target.get("link", {}).get("commandFragments", []):
            if flag.get("role") != "libraries" or "backtrace" not in flag:
                continue
            name = os.path.normcase(flag["fragment"].strip('"').replace("\\", "/"))
            if name not in owned_artifacts:
                external_links.add(flag["fragment"])
        groups = model_target.get("compileGroups", [])
        for source in model_target.get("sources", []):
            if "compileGroupIndex" not in source:
                continue
            if groups[source["compileGroupIndex"]]["language"] != "C":
                refuse("language", source["path"], "Only C translation units are supported")
            path = (root / source["path"]).resolve()
            if path.suffix.lower() != ".c" or path in files:
                refuse("source_profile", path, "Expected one .c compilation per target closure member")
            files[path] = entry["name"]
    if not files or len(files) > 64:
        refuse("target_size", target, "Expected 1 to 64 C translation units")
    return root, files, generator.get("multiConfig", False), sorted(external_links)


def windows_argv(line):
    import ctypes
    from ctypes import wintypes

    shell = ctypes.windll.shell32.CommandLineToArgvW
    shell.argtypes = [wintypes.LPCWSTR, ctypes.POINTER(ctypes.c_int)]
    shell.restype = ctypes.POINTER(wintypes.LPWSTR)
    ctypes.windll.kernel32.LocalFree.argtypes = [wintypes.HLOCAL]
    count = ctypes.c_int()
    args = shell(line, ctypes.byref(count))
    if not args:
        raise OSError("Cannot split the Windows compile command")
    try:
        return args[:count.value]
    finally:
        ctypes.windll.kernel32.LocalFree(args)


def compile_entries(database, files, configuration, multi_config):
    candidates = defaultdict(list)
    for entry in read_json(database):
        cwd = Path(entry["directory"]).resolve()
        source = (cwd / entry["file"]).resolve()
        if source not in files:
            continue
        args = entry.get("arguments")
        if args is None:
            args = windows_argv(entry["command"]) if os.name == "nt" else shlex.split(entry["command"])
        owner = files[source]
        output = entry.get("output", "").replace("\\", "/").split("/")
        if f"{owner}.dir" not in output:
            continue
        # Ninja Multi-Config records its selected configuration in both the
        # object directory and the compiler command. Single-config Ninja omits it.
        config_flags = [a.split("=", 1)[1].strip('"') for a in args
                        if a.startswith(("-DCMAKE_INTDIR=", "/DCMAKE_INTDIR="))]
        suffix = output[output.index(f"{owner}.dir") + 1:]
        if multi_config and (config_flags != [configuration] or not suffix or suffix[0] != configuration):
            continue
        if not multi_config and config_flags:
            continue
        candidates[source].append((cwd, args))
    for path in files:
        found = candidates[path]
        if len(found) != 1:
            refuse("compile_command", path, f"Expected one command for target {files[path]!r}, configuration {configuration!r}; found {len(found)} (Ninja object metadata required)")
    return {p: candidates[p][0] for p in files}


def clang_flags(path, cwd, args):
    driver = Path(args[0]).stem.lower()
    if not re.fullmatch(r"(?:clang(?:-\d+)?|clang-cl)", driver):
        refuse("driver", path, f"Expected CMake commands from Clang or clang-cl, not {args[0]!r}")
    cl = driver == "clang-cl"
    flags = []
    iterator = iter(args[1:])
    for argument in iterator:
        if argument in {"-o", "-MF", "-MT", "-MQ", "/Fo", "/Fd"}:
            next(iterator)
            continue
        if argument in {"--", "-c", "/c", "/TC", "/nologo", "-MMD", "-MP"} or (argument == "-MD" and not cl):
            continue
        if re.fullmatch(r"/showIncludes|/Fo.+|/Fd.+|/FS|-clang:-MD|-clang:-M[TF].+", argument):
            continue
        if not argument.startswith(("-", "@")) and (cwd / argument).resolve() == path:
            continue
        if argument in {"-target", "--target"}:
            value = next(iterator)
            check_target(path, value)
            flags.extend((argument, value))
            continue
        if re.fullmatch(r"(?:--target=|/clang:--target=).+", argument):
            check_target(path, argument.split("=", 1)[1])
            flags.append(argument)
            continue
        if argument in {"-I", "-D", "-U", "-include", "-isystem", "-iquote", "-imacros",
                        "/I", "/D", "/U", "/FI", "/imsvc"}:
            value = next(iterator)
            flags.extend((argument, value))
            continue
        if re.fullmatch(r"-(?:[IDU].+|std=(?:gnu|c)(?:89|90|99|11|17|18|23)|m64|O[0-3szg]|g[0-3]?|W(?:all|extra|error|no-[\w-]+))", argument):
            flags.append(argument)
            continue
        if cl and re.fullmatch(r"(?:/(?:[IDU].+|FI.+|imsvc.+|std:c(?:11|17)|RTC[1su]+|O(?:[12d]|b[0123])|Z[i7I]|W[0-4])|[-/]M[DT]d?)", argument):
            flags.append(argument)
            continue
        refuse("compile_flag", path, f"Unsupported compile option {argument!r}")
    return cl, flags


def check_target(path, target):
    if not re.fullmatch(r"(?:x86_64|amd64)-[\w.-]+", target):
        refuse("target_abi", path, f"Only x86-64 C targets are covered, got {target!r}")


def parse_translation_unit(clang, path, cwd, args):
    cl, flags = clang_flags(path, cwd, args)
    # The provided executable is trusted tooling. Compilation-database
    # commands, wrappers and response files are never executed.
    command = [str(clang), "--driver-mode=" + ("cl" if cl else "gcc"), *flags]
    if cl:
        command += ["/TC", "/clang:-fsyntax-only", "/clang:-Xclang", "/clang:-ast-dump=json", str(path)]
    else:
        command += ["-x", "c", "-fsyntax-only", "-Xclang", "-ast-dump=json", "--", str(path)]
    process = subprocess.run(command, cwd=cwd, capture_output=True, timeout=25)
    if process.returncode:
        detail = process.stderr.decode("utf-8", "replace").strip()[:6000]
        if "file not found" in detail or "cannot open source file" in detail:
            refuse("header_missing", path, detail + "\nPrepare the header using the project's configure or header-generation step before importing.")
        refuse("clang", path, detail)
    if len(process.stdout) > 32 * 1024 * 1024:
        refuse("ast_limit", path, "Clang AST exceeds 32 MiB")
    return json.loads(process.stdout)


def children(node):
    return node.get("inner", [])


def scalar(node, location):
    typ = node.get("type", {})
    name = typ.get("desugaredQualType", typ.get("qualType"))
    if name != "unsigned int":
        refuse("scalar_type", location, f"{name!r} is outside the unsigned int (U32) profile")


def symbol_id(path, name, static, root):
    relative = os.path.relpath(path, root).replace("\\", "/")
    return f"{relative}::{name}" if static else name


class Translation:
    def __init__(self, root, trees, external_links):
        self.root = root
        self.functions = {}
        self.declarations = {}
        self.dll_imports = set()
        self.special_linkage = set()
        self.reachable = {}
        self.source_bytes = {}
        self.external_links = external_links
        for path, tree in trees.items():
            self.index(path, tree)

    def index(self, path, tree):
        # Clang omits repeated file names in locations. Remember the most
        # recent explicit file within this translation unit.
        current_file = path
        for node in children(tree):
            loc = node.get("loc", {})
            if "file" in loc:
                current_file = (path.parent / loc["file"]).resolve()
            if node.get("kind") != "FunctionDecl":
                continue
            static = node.get("storageClass") == "static"
            previous = self.declarations.get((path, node.get("previousDecl")))
            key = previous or symbol_id(path, node["name"], static, self.root)
            self.declarations[path, node["id"]] = key
            if any(n["kind"] == "DLLImportAttr" for n in children(node)):
                self.dll_imports.add((path, node["id"]))
            if any(n["kind"] in {"AsmLabelAttr", "AliasAttr", "WeakAttr", "WeakRefAttr"}
                   for n in children(node)):
                self.special_linkage.add(key)
            if not any(n["kind"] == "CompoundStmt" for n in children(node)):
                continue
            if key in self.functions:
                refuse("duplicate_definition", current_file, key)
            self.functions[key] = (path, current_file, node)

    def function(self, key, trace=()):
        if key in trace:
            refuse("recursive_call", key, "Recursive calls are outside the dataflow profile")
        if key in self.reachable:
            return self.reachable[key]
        if len(self.reachable) + len(trace) >= 48:
            refuse("call_limit", key, "The reachable function limit is 48")
        unit, file, fn = self.functions[key]
        origin = self.location(file, fn["loc"])
        if key in self.special_linkage:
            refuse("linkage_attribute", origin, f"Unsupported linkage annotation on {key}")
        return_type = fn.get("type", {}).get("qualType", "").split(" (", 1)[0]
        if return_type != "unsigned int" or "..." in fn["type"]["qualType"]:
            refuse("signature", origin, f"Unsupported signature for {key}: {fn['type']['qualType']}")
        parameters = [n for n in children(fn) if n["kind"] == "ParmVarDecl"]
        if not 1 <= len(parameters) <= 2:
            refuse("signature", origin, f"{key} requires one or two U32 parameters")
        for parameter in parameters:
            scalar(parameter, origin)
        body = next(n for n in children(fn) if n["kind"] == "CompoundStmt")
        statements = children(body)
        if len(statements) != 1 or statements[0]["kind"] != "ReturnStmt":
            refuse("statement", origin, f"{key} must consist of one return expression")
        graph = Graph(key, origin, file, parameters)
        result = self.expression(unit, graph, children(statements[0])[0], (*trace, key))
        graph.finish(result)
        self.reachable[key] = graph
        return graph

    def location(self, file, loc):
        loc = loc.get("expansionLoc", loc)
        if "file" in loc:
            file = (file.parent / loc["file"]).resolve()
        line = loc.get("line")
        if line is None:
            if file not in self.source_bytes:
                self.source_bytes[file] = file.read_bytes()
            line = self.source_bytes[file][:loc.get("offset", 0)].count(b"\n") + 1
        return f"{os.path.relpath(file, self.root).replace(os.sep, '/')}:{line}"

    def expression(self, unit, graph, node, trace):
        kind = node["kind"]
        args = children(node)
        begin = node.get("range", {}).get("begin")
        at = self.location(graph.source_file, begin) if begin else graph.origin
        if kind == "ParenExpr":
            return self.expression(unit, graph, args[0], trace)
        if kind in {"ImplicitCastExpr", "CStyleCastExpr"}:
            if node.get("castKind") not in {"LValueToRValue", "NoOp", "IntegralCast"}:
                refuse("conversion", at, node.get("castKind"))
            scalar(node, at)
            if args[0]["kind"] == "IntegerLiteral":
                return graph.constant(int(args[0]["value"]) % 2**32)
            return self.expression(unit, graph, args[0], trace)
        if kind == "IntegerLiteral":
            scalar(node, at)
            return graph.constant(int(node["value"]))
        if kind == "DeclRefExpr":
            reference = node["referencedDecl"]
            if reference["id"] not in graph.arguments:
                refuse("read", at, f"Unsupported read of {reference['name']!r}")
            return graph.arguments[reference["id"]]
        if kind == "BinaryOperator" and node["opcode"] in {"+", "-", "*"}:
            scalar(node, at)
            args = [self.expression(unit, graph, arg, trace) for arg in args]
            return graph.binary(node["opcode"], args)
        if kind == "CallExpr":
            scalar(node, at)
            callee = args[0]
            while callee["kind"] in {"ImplicitCastExpr", "ParenExpr"} and len(children(callee)) == 1:
                if callee["kind"] == "ImplicitCastExpr" and callee.get("castKind") != "FunctionToPointerDecay":
                    refuse("indirect_call", at, "Expected a direct function declaration")
                callee = children(callee)[0]
            reference = callee.get("referencedDecl", {})
            if callee["kind"] != "DeclRefExpr" or reference.get("kind") != "FunctionDecl":
                refuse("indirect_call", at, "Expected a direct function declaration")
            key = self.declarations.get((unit, reference["id"]))
            if key is None:
                refuse("call_identity", at, f"Clang declaration for {reference['name']} is not in this translation unit")
            if (unit, reference["id"]) in self.dll_imports:
                refuse("external_call", at, f"DLL import {reference['name']} requires a declared NativeCall contract; it is outside this project profile")
            if key in self.special_linkage:
                refuse("linkage_attribute", at, f"Unsupported linkage annotation on {key}")
            if key not in self.functions:
                if self.external_links:
                    refuse("external_call", at, f"{reference['name']} is not defined in the covered C targets; "
                           f"declared external link inputs: {', '.join(self.external_links)}")
                refuse("missing_definition", at, f"No definition for {reference['name']} in the selected CMake target closure")
            child = self.function(key, trace)
            expected_signature = "unsigned int (" + ", ".join(["unsigned int"] * len(child.inputs)) + ")"
            if reference.get("type", {}).get("qualType") != expected_signature:
                refuse("signature", at, f"Declaration of {key} does not match its U32 definition")
            if len(args) - 1 != len(child.inputs):
                refuse("signature", at, f"Call to {key} does not match its definition")
            values = [self.expression(unit, graph, arg, trace) for arg in args[1:]]
            return graph.call(child, values)
        refuse("operation", at, f"{node.get('opcode', kind)} is outside the expression profile")


def bounds(x, y, width, height):
    return {"x": x, "y": y, "width": width, "height": height}


class Graph:
    def __init__(self, key, origin, file, parameters):
        self.key, self.origin = key, origin
        self.source_file = file
        self.filename = f"{key.split('::')[-1]}-{hashlib.sha256(key.encode()).hexdigest()[:16]}.frog"
        self.inputs = [parameter["name"] for parameter in parameters]
        output = "result"
        while output in self.inputs:
            output += "_out"
        self.output = output
        self.nodes, self.edges, self.widgets = [], [], []
        self.arguments = {p["id"]: self.widget(p["name"], i, True) for i, p in enumerate(parameters)}
        self.column = 0

    def node(self, ident, kind, layout, **data):
        self.nodes.append(dict(id=ident, kind=kind, layout=layout, **data))
        return ident

    def widget(self, name, index, control):
        role = "control" if control else "indicator"
        portkey = "public_input_id" if control else "public_output_id"
        slot = f"zone_{2 * index + 1}" if control else "zone_2"
        ident = f"{role}-{name}"
        self.widgets.append({"id": ident, "classId": "frog.widgets.numeric_" + role, "role": role,
                             "valueType": "u32", "label": name, "bounds": bounds(24 if control else 310, 55 + index * 75, 122, 30),
                             "state": {"value": 0}, "binding": {"mode": "widget_value", portkey: name,
                                        "interface_map_slot": slot, "connection_requirement": "required" if control else "recommended"}})
        x = 80 if control else 250 + self.column * 180
        return self.node(ident, "widget_value", bounds(x, 105 + index * 85, 65, 30),
                         widget=ident, type="u32", label=name), "value"

    def wire(self, start, end, port):
        self.edges.append({"id": f"wire-{len(self.edges)}", "from": {"node": start[0], "port": start[1]},
                           "to": {"node": end, "port": port}})

    def constant(self, value):
        ident = f"constant-{len(self.nodes)}"
        return self.node(ident, "primitive", bounds(180 + self.column * 180, 166 + len(self.nodes) * 34, 68, 28),
                         type="frog.studio.numeric.numeric_constant", value_type="u32", text=str(value)), "value"

    def operation_layout(self):
        self.column += 1
        return bounds(145 + self.column * 180, 90, 55, 60)

    def binary(self, opcode, values):
        type_name, catalog = {"+": ("frog.core.add", "numeric.add"),
                              "-": ("frog.core.sub", "numeric.subtract"),
                              "*": ("frog.core.mul", "numeric.multiply")}[opcode]
        node = self.node(f"arithmetic-{len(self.nodes)}", "primitive", self.operation_layout(),
                         type=type_name, palette_catalog_id=catalog)
        for value, port in zip(values, ("a", "b"), strict=True):
            self.wire(value, node, port)
        return node, "result"

    def call(self, child, values):
        ports = [{"id": name, "source_port_id": name, "direction": direction, "label": name,
                  "value_type": "u32", "connection_requirement": "required" if direction == "input" else "recommended",
                  "interface_map_slot": f"zone_{2 * i + 1}" if direction == "input" else "zone_2",
                  "normalized_x": 0 if direction == "input" else 1,
                  "normalized_y": 0.5 if len(names) == 1 else 0.25 + i * 0.5}
                 for direction, names in (("input", child.inputs), ("output", [child.output]))
                 for i, name in enumerate(names)]
        node = self.node(f"call-{len(self.nodes)}", "frog_call", self.operation_layout(),
                         label=child.key.split("::")[-1], label_visible=True,
                         call={"document_path": child.filename, "document_title": child.key, "ports": ports})
        for name, value in zip(child.inputs, values, strict=True):
            self.wire(value, node, name)
        return node, child.output

    def finish(self, result):
        node, port = self.widget(self.output, 0, False)
        self.wire(result, node, port)

    def draft(self, entry=False):
        interface = {"inputs": [{"id": name, "type": "u32", "connection": "required"} for name in self.inputs],
                     "outputs": [{"id": self.output, "type": "u32"}],
                     "map": {"binding_map_profile": "square_40x40_v1", "layout_pattern_id": "pattern_06",
                             "visual_transforms": []}}
        if entry:
            for widget in self.widgets:
                del widget["binding"]
            interface = {"inputs": [], "outputs": []}
        glyph = escape(self.key.split("::")[-1][:1].upper(), quote=True)
        svg = ('<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 40 40">'
               '<rect x="3" y="3" width="34" height="34" rx="4" fill="#485362" stroke="#aebdcb" stroke-width="2"/>'
               f'<text x="20" y="26" text-anchor="middle" font-size="20" fill="#fff">{glyph}</text></svg>')
        return {"format": "frog.document.draft", "draft_revision": 7, "language": "FROG", "title": self.key,
                "icon": {"svg": svg},
                "metadata": {"name": self.key, "description": self.origin},
                "interface": interface, "frontPanel": {"widgets": self.widgets},
                "diagram": {"nodes": self.nodes, "edges": self.edges}}


def write_json(path, value):
    path.write_text(json.dumps(value, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")


def translate(args):
    database = args.source.resolve(strict=True)
    root, sources, multi_config, external_links = cmake_target(database, args.configuration, args.target)
    entries = compile_entries(database, sources, args.configuration, multi_config)
    clang = args.clang.resolve(strict=True)
    triple = subprocess.run([str(clang), "--driver-mode=gcc", "-print-target-triple"],
                            capture_output=True, text=True, timeout=10, check=True).stdout.strip()
    check_target(database, triple)
    trees = {p: parse_translation_unit(clang, p, *entry) for p, entry in entries.items()}
    translation = Translation(root, trees, external_links)
    if args.entry not in translation.functions:
        refuse("entry", database, f"No externally linked definition of {args.entry!r} in this target")
    main = translation.function(args.entry)
    output = args.output.resolve()
    if output.exists():
        refuse("output_exists", output, "Choose a new output directory")
    # Inherit the destination's normal ACL. On Windows, Python's 0700
    # TemporaryDirectory would otherwise ship a user-only ACL with the result.
    stage = output.parent / f".import-c-project-{uuid4()}"
    stage.mkdir()
    try:
        dependencies = []
        origins = []
        for graph in translation.reachable.values():
            name = "main.frog" if graph is main else graph.filename
            write_json(stage / name, graph.draft(graph is main))
            origins.append({"function": graph.key, "origin": graph.origin, "document": name})
            if graph is not main:
                dependencies.append({"kind": 1, "reference": name, "source_uri": f"import-c:/{name}",
                                     "identity": graph.key, "version": "7", "path": name})
        write_json(stage / "frontend-dependencies.json",
                   {"format": "frog.frontend.dependencies", "version": "1", "dependencies": dependencies})
        command = [str(args.frontend.resolve(strict=True)), "derive", "--source", str(stage / "main.frog"),
                   "--source-uri", "import-c:/main.frog", "--document-id", args.entry, "--fir-document-id",
                   args.entry + ":fir", "--source-format", "frog.document.draft", "--source-version", "7",
                   "--fir-version", "0.1", "--dependencies", str(stage / "frontend-dependencies.json"),
                   "--output", str(stage / "main.fir.json"), "--source-maps-output", str(stage / "source-maps.json")]
        result = subprocess.run(command, capture_output=True, timeout=35)
        if result.returncode:
            detail = result.stderr or result.stdout
            refuse("frontend_refused", database, detail.decode("utf-8", "replace").strip()[:16000] or f"Frontend exited {result.returncode}")
        write_json(stage / "import-map.json", {"target": args.target, "configuration": args.configuration,
                                              "entry": args.entry,
                                              "compile_commands_sha256": hashlib.sha256(database.read_bytes()).hexdigest(),
                                              "external_link_inputs": external_links,
                                              "functions": origins})
        stage.rename(output)
    finally:
        if stage.is_dir():
            shutil.rmtree(stage)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for option in ("source", "output", "clang", "frontend"):
        parser.add_argument("--" + option, type=Path, required=True)
    for option in ("target", "configuration", "entry"):
        parser.add_argument("--" + option, required=True)
    args = parser.parse_args()
    try:
        translate(args)
    except (ValueError, OSError, KeyError, IndexError, TypeError, StopIteration, subprocess.SubprocessError) as error:
        print(f"{args.source}: {error}", file=sys.stderr)
        return 2
    return 0


if __name__ == "__main__":
    sys.exit(main())
