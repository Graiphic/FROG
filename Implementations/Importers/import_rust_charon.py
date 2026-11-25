# SPDX-License-Identifier: Apache-2.0
"""Project Charon 0.1.323 structured Rust bodies into editable FROG graphs."""

from __future__ import annotations

import copy
import json
from pathlib import Path
import re
from dataclasses import dataclass
from types import SimpleNamespace

from import_rust_mir import RustGraph, Value, bounds, _ARITHMETIC, _BOOL_BITWISE, _COMPARISON, _INTEGER_TYPES


_CHARON_VERSION = "0.1.323"
_MAX_AGGREGATE = 8
_MAX_NODES = 1024
_SYMBOL = re.compile(r"[A-Za-z_][A-Za-z_0-9]*")


def _variant(value):
    if isinstance(value, str):
        return value, None
    if isinstance(value, dict) and len(value) == 1:
        return next(iter(value.items()))
    raise ValueError("The LLBC value does not contain one tagged constructor")


def _sequence_index(values, index):
    if type(index) is not int or not 0 <= index < len(values) or values[index] is None:
        raise ValueError(f"LLBC declaration or place {index} is absent")
    return values[index]


def _untagged(value):
    tag, inner = _variant(value)
    if tag != "Untagged":
        raise ValueError("Charon LLBC must be extracted with --no-dedup-serialized-ast")
    return inner


def _path(declaration, ordinal):
    path = declaration.get("item_meta", {}).get("name", [])
    atoms = []
    for segment in path:
        kind, value = _variant(segment)
        if kind == "Ident" and isinstance(value, list) and _SYMBOL.fullmatch(value[0]):
            atoms.append(value[0] + (f"_{value[1]}" if value[1] else ""))
        elif kind == "Impl":
            atoms.append(f"impl_{ordinal}")
    return tuple(atoms)


@dataclass(frozen=True)
class Type:
    kind: str
    name: str = ""
    fields: tuple[tuple[str, Type], ...] = ()
    items: Type | None = None
    count: int = 0

    def contains_array(self):
        return self.kind == "array" or any(field.contains_array() for _, field in self.fields)

    def frog(self):
        if self.kind == "scalar":
            return self.name
        if self.kind == "array":
            return f"array<{self.items.frog()}>"
        if self.kind == "record":
            return 'cluster<' + ','.join(f'"element{i}":{part.frog()}'
                                         for i, (_, part) in enumerate(self.fields, 1)) + '>'
        raise ValueError(f"Rust type {self.name or self.kind} has no value connector")


_UNIT = Type("unit")
_INDEX = Type("index", "usize")


@dataclass(frozen=True)
class Data:
    ty: Type
    endpoint: tuple[str, str] | None = None
    literal: int | bool | None = None

    def scalar(self):
        return Value(self.ty.frog(), self.endpoint)


@dataclass(frozen=True)
class Address:
    local: int
    parts: tuple[int, ...] = ()
    writable: bool = True

    def child(self, ordinal):
        return Address(self.local, (*self.parts, ordinal), self.writable)


@dataclass(frozen=True)
class Borrow:
    ty: Type
    address: Address


class CharonGraph(RustGraph):
    def __init__(self, key, source, parameters, outputs, prototypes):
        self.parameters = parameters
        self.output_specs = outputs
        self.model_inputs = {item["name"]: item["model"] for item in parameters}
        self.model_outputs = dict(outputs)
        self.composite = any(model.kind != "scalar" for model in (*self.model_inputs.values(), *self.model_outputs.values()))
        self.projections = {}
        self.source_nodes = {}
        self.panel_offsets = {"control": 65, "indicator": 65}
        super().__init__(SimpleNamespace(name=key, result=outputs[0][1].frog()),
                         source, parameters, prototypes)
        self.origin = f"Charon LLBC {_CHARON_VERSION} · {source} · {key}"

    def _shape_widget(self, model, ident, role, name):
        if model.kind == "scalar":
            dtype = model.frog()
            widget = copy.deepcopy(self.prototypes[("boolean_" if dtype == "bool" else "numeric_") + role])
            widget.update(id=ident, role=role, valueType=dtype, label=name)
            widget["bounds"] = bounds(0, 0, 84, 34)
            widget["props"].pop("data.default", None)
            widget["props"].update({"label.visible": bool(name), "caption.independent": True, "caption.visible": False})
            if dtype != "bool":
                named = ("Int" if dtype[0] == "i" else "UInt") + dtype[1:]
                widget["props"].update({"data_type.representation": dtype, "data_type.named_numeric_size": named,
                                        "representation.kind": named.lower()})
            widget["state"] = {"value": False if dtype == "bool" else 0}
            return widget
        common = {"id": ident, "role": role, "classId": f"frog.widgets.{'array' if model.kind == 'array' else 'cluster'}_{role}",
                  "valueType": model.frog(), "label": name,
                  "props": {"label.visible": bool(name), "caption.independent": True, "caption.visible": False}}
        if model.kind == "array":
            element = self._shape_widget(model.items, ident + ".element", role, "")
            common["bounds"] = bounds(0, 0, 220, 60)
            common["props"].update({"dimensions.rank": 1, "dimensions.shape": [model.count],
                                    "dimensions.allow_resize": False, "index_display.visible": True,
                                    "viewport.first_indices": [0], "viewport.visible_counts": [1],
                                    "viewport.orientation": "vertical", "element.class_id": element["classId"],
                                    "element.value_type": model.items.frog(), "element.role": role,
                                    "element.template": element})
        else:
            elements = []
            top = 20
            width = 100
            for index, (label, member) in enumerate(model.fields, 1):
                item = self._shape_widget(member, f"{ident}.element{index}", role, label)
                item["bounds"].update(x=15, y=top)
                top += item["bounds"]["height"] + 26
                width = max(width, item["bounds"]["width"] + 28)
                elements.append({"field_id": f"element{index}", "technical_name": label, "logical_order": index-1,
                                 "widget": item})
            common["bounds"] = bounds(0, 0, width, top)
            common["props"]["cluster.elements"] = elements
        return common

    def widget(self, name, index, control):
        role = "control" if control else "indicator"
        model = self.model_inputs[name] if control else self.model_outputs[name]
        ident = f"{role}-{name}"
        widget = self._shape_widget(model, ident, role, name)
        widget["bounds"].update(x=36 if control else 382, y=self.panel_offsets[role])
        self.panel_offsets[role] += widget["bounds"]["height"] + 44
        widget["binding"] = {"mode": "widget_value", "public_input_id" if control else "public_output_id": name,
                             "interface_map_slot": self.slot(index, control),
                             "connection_requirement": "required" if control else "recommended"}
        self.widgets.append(widget)
        x = 85 if control else 270 + self.column*180
        return self.node(ident, "widget_value", bounds(x, 105+index*92, 70, 30),
                         widget=ident, type=model.frog(), label=name), "value"

    def slot(self, index, control):
        if self.three_inputs:
            return ("zone_1", "zone_3", "zone_4")[index] if control else "zone_2"
        return f"zone_{2 * index + (1 if control else 2)}"

    def complete(self, results):
        for index, (identifier, _) in enumerate(self.output_specs):
            target, port = self.widget(identifier, index, False)
            self.wire(results[identifier].endpoint, target, port)

    def draft(self, entry=False):
        document = super().draft(entry)
        if not entry:
            document["interface"]["outputs"] = [{"id": name, "type": model.frog()} for name, model in self.output_specs]
        if self.composite and document["draft_revision"] == 2:
            document["draft_revision"] = 3
        return document

    def subfrog(self, child, values):
        member = child.draft()
        outputs = {}
        ports = []
        for control, items in ((True, child.model_inputs.items()), (False, child.output_specs)):
            for index, (name, model) in enumerate(items):
                height = ((index+0.5)/3 if control else 0.5) if child.three_inputs else (0.25+index*0.5)
                ports.append({"id": name, "source_port_id": name, "direction": "input" if control else "output",
                              "label": name, "value_type": model.frog(),
                              "connection_requirement": "required" if control else "recommended",
                              "interface_map_slot": child.slot(index, control),
                              "normalized_x": 0 if control else 1, "normalized_y": height})
        ident = self.node(f"call-{len(self.nodes)}", "frog_call", self.operation_layout(),
                          label=child.key.split("::")[-1], label_visible=True,
                          doc={"summary": child.origin},
                          call={"document_path": child.filename, "document_title": member["title"],
                                "icon_svg": member["icon"]["svg"], "ports": ports})
        for name, value in zip(child.inputs, values, strict=True):
            self.wire(value.endpoint, ident, name)
        for name, model in child.output_specs:
            outputs[name] = Data(model, (ident, name))
        return outputs

    def collection_node(self, kind, ports, port, model, columns=1, **options):
        self.composite = True
        ident = self.node(f"{kind.rsplit('.', 1)[1]}-{len(self.nodes)}", "primitive", self.operation_layout(),
                          type=f"frog.studio.{kind}", palette_catalog_id=kind, label_visible=False, **options)
        self.nodes[-1]["layout"]["height"] = max(45, columns*17+18)
        for value, destination in ports:
            self.wire(value, ident, destination)
        return Data(model, (ident, port))

    def construct(self, model, fields):
        if model.kind == "record":
            # The template carries the same field identities and visible names
            # as the interface widgets. Every field is supplied by Rust.
            schema = self.record_schema(model)
            ports = [(schema, "cluster"), *((value.endpoint, f"element{i}") for i, value in enumerate(fields, 1))]
            return self.collection_node("cluster.bundle", ports, "result", model, len(ports),
                                        cluster_function_terminals=len(fields))
        names = ("array_a", "array_b", *(f"input_{i}" for i in range(3, len(fields)+1)))
        ports = [(value.endpoint, name) for value, name in zip(fields, names)]
        return self.collection_node("array.build", ports, "result", model, len(ports),
                                    build_array_inputs=len(ports), build_array_concatenate=False)

    def record_schema(self, model):
        root = self.operation_layout()
        root.update(width=105, height=50)

        def children(shape, label, frame, *, owner=None, index=0):
            ident = f"record-schema-{len(self.nodes)}"
            details = {"label": label, "label_visible": owner is not None}
            if owner:
                details.update(constant_owner_node_id=owner, constant_field_id=f"element{index+1}",
                               constant_field_order=index)
            if shape.kind == "record":
                self.node(ident, "primitive", frame, **details, type="frog.studio.cluster.constant",
                          palette_catalog_id="cluster.constant", cluster_constant_collapsed=True)
                for row, (name, member) in enumerate(shape.fields):
                    children(member, name, bounds(frame["x"]+12, frame["y"]+row*24+24, 65, 24), owner=ident, index=row)
            elif shape.kind == "array":
                self.node(ident, "primitive", frame, **details, type="frog.studio.numeric.numeric_array_constant",
                          value_type=shape.items.frog(), text="0",
                          array_constant={"rank": 1, "shape": [shape.count], "values": ["0"]*shape.count,
                                          "first_indices": [0], "visible_counts": [1],
                                          "cell_size": {"width": 36, "height": 27}})
            elif shape.name == "bool":
                self.node(ident, "primitive", frame, **details, type="frog.studio.boolean.false_constant",
                          value_type="bool", text="false")
            else:
                self.node(ident, "primitive", frame, **details, type="frog.studio.numeric.numeric_constant",
                          value_type=shape.frog(), text="0")
            return ident

        return children(model, model.name.split("::")[-1].split("#")[0], root), "value"

    def project(self, aggregate, index):
        key = aggregate.endpoint, index
        if key not in self.projections:
            self.projections[key] = self.project_uncached(aggregate, index)
        return self.projections[key]

    def project_uncached(self, aggregate, index):
        shape = aggregate.ty
        if shape.kind == "record":
            return self.collection_node("cluster.unbundle", [(aggregate.endpoint, "cluster")],
                                        f"element{index+1}", shape.fields[index][1], len(shape.fields),
                                        cluster_function_terminals=len(shape.fields))
        offset = self.typed_constant("i32", index)
        return self.collection_node("array.index", [(aggregate.endpoint, "array"), (offset, "index")],
                                    "element", shape.items, index_array_outputs=1)

    def update(self, aggregate, index, value):
        shape = aggregate.ty
        if shape.kind == "record":
            return self.collection_node("cluster.bundle", [(aggregate.endpoint, "cluster"),
                                        (value.endpoint, f"element{index+1}")], "result", shape,
                                        len(shape.fields), cluster_function_terminals=len(shape.fields))
        offset = self.typed_constant("i32", index)
        return self.collection_node("array.replace_subset", [(aggregate.endpoint, "array"),
                                    (value.endpoint, "subset"), (offset, "index")], "result", shape)


class Crate:
    def __init__(self, export, source):
        self.source = source
        if not isinstance(export, dict) or export.get("charon_version") != _CHARON_VERSION or export.get("has_errors") is not False:
            raise ValueError(f"{source}: expected a complete Charon {_CHARON_VERSION} LLBC export")
        self.krate = export["translated"]
        self.functions = self.krate["fun_decls"]
        self.structs = self.krate["type_decls"]
        self.type_cache = {}
        self.active_types = set()
        self.active_functions = set()
        self.graphs = {}
        self.prototypes = json.loads(Path(__file__).with_name("Import-WidgetPrototypes.json").read_text(encoding="utf-8"))

    def source_location(self, raw):
        try:
            data = _untagged(raw["span"])["data"]
            begin = data["beg"]
            filename = _sequence_index(self.krate["files"], data["file_id"])["name"]["Local"]
            return f"{filename.replace(chr(92), '/')}:{begin['line']}:{begin['col']+1}"
        except (KeyError, TypeError, ValueError):
            return None

    def type(self, raw):
        return self.type_kind(_untagged(raw))

    def type_kind(self, raw):
        tag, payload = _variant(raw)
        if tag == "Scalar":
            tag, payload = _variant(payload)
            if tag == "Bool":
                return Type("scalar", "bool")
            if tag == "Integer":
                tag, bits = _variant(payload)
                if tag == "Unsigned" and bits == "Usize":
                    return _INDEX
                if tag in {"Signed", "Unsigned"} and isinstance(bits, str) and bits.lower() in _INTEGER_TYPES:
                    return Type("scalar", bits.lower())
            raise ValueError(f"Unsupported Charon scalar type {raw!r}")
        if tag == "Adt":
            identifier = payload["id"]
            if payload.get("builtin") == "Tuple" and not payload.get("generics", {}).get("types"):
                return _UNIT
            if payload.get("builtin") or payload.get("generics", {}).get("types"):
                raise ValueError("Only monomorphic Rust structs and unit are admitted")
            return self.record(identifier)
        if tag == "Array":
            nested, count, _ = payload
            n = self.integer_constant(count)
            if not 1 <= n <= _MAX_AGGREGATE:
                raise ValueError(f"Fixed array length {n} is outside 1..{_MAX_AGGREGATE}")
            element = self.type(nested)
            if element.name not in _INTEGER_TYPES:
                raise ValueError("A fixed Rust array needs a fixed-width integer element type in this profile")
            return Type("array", items=element, count=n)
        if tag == "Ref":
            _, nested, kind = payload
            target = self.type(nested)
            if kind in {"Mut", "Shared"} and target.kind in {"scalar", "array", "record"}:
                return Type("reference", kind, items=target)
        raise ValueError(f"Unsupported Charon type {tag}")

    def integer_constant(self, raw):
        inner, _ = _untagged(raw)
        tag, payload = _variant(inner)
        if tag != "Integer":
            raise ValueError("A fixed collection count or index needs an integer literal")
        sign, typed = _variant(payload)
        if sign not in {"Signed", "Unsigned"}:
            raise ValueError("Invalid Charon integer signedness")
        return int(typed[1])

    def record(self, identifier):
        if identifier in self.type_cache:
            return self.type_cache[identifier]
        if identifier in self.active_types or len(self.active_types) >= 12:
            raise ValueError("Recursive or deeply nested Rust structs are outside the profile")
        entry = _sequence_index(self.structs, identifier)
        tag, raw = _variant(entry["kind"])
        if tag != "Struct" or not 1 <= len(raw) <= _MAX_AGGREGATE:
            raise ValueError("Only populated monomorphic Rust structs with up to eight fields are admitted")
        key = "::".join(_path(entry, identifier)) or f"type_{identifier}"
        self.active_types.add(identifier)
        fields = tuple((field["name"], self.type(field["ty"])) for field in raw)
        if any(field.kind not in {"scalar", "array", "record"} for _, field in fields):
            raise ValueError(f"Rust struct {key} contains a field outside the value profile")
        self.active_types.remove(identifier)
        model = Type("record", f"{key}#{identifier}", fields)
        self.type_cache[identifier] = model
        return model

    def call_target(self, raw):
        tag, pointer = _variant(raw)
        if tag != "Regular":
            raise ValueError(f"Unsupported {tag} Rust call")
        kind, reference = _variant(pointer["kind"])
        if kind == "Fun":
            return reference
        if kind == "Trait":
            raw_proof, method = reference
            proof = _untagged(raw_proof)
            tag, impl_ref = _variant(proof["kind"])
            if tag == "TraitImpl":
                implemented = _sequence_index(self.krate["trait_impls"], impl_ref["id"])
                if implemented["impl_trait"]["id"] != proof["trait_decl_ref"]["id"]:
                    raise ValueError("Trait proof and concrete implementation differ")
                declaration = _sequence_index(implemented["methods"], method)["skip_binder"]
                return declaration["id"]
        raise ValueError("Only concrete Charon function or trait method targets are admitted")

    def array_index_intrinsic(self, identifier):
        declaration = _sequence_index(self.functions, identifier)
        tag, origin = _variant(declaration["src"])
        if tag != "TraitImpl":
            return None
        trait = _sequence_index(self.krate["trait_decls"], origin["trait_ref"]["id"])
        language_item = trait["item_meta"].get("lang_item")
        name = _path(trait, origin["trait_ref"]["id"])
        if name[:1] == ("core",) and language_item in {"Index", "IndexMut"}:
            return language_item
        return None

    def entry(self, name):
        candidates = {}
        for index, function in enumerate(self.functions):
            if not function or not isinstance(function["body"], dict) or "Structured" not in function["body"]:
                continue
            path = _path(function, index)
            if not path or path[0] != self.krate.get("crate_name"):
                continue
            if function["src"] == "Normal":
                candidates[index] = "::".join(path)
        if name:
            exact = [i for i, path in candidates.items() if path == name or path.split("::", 1)[-1] == name]
            if not exact and "::" not in name:
                exact = [i for i, path in candidates.items() if path.rsplit("::", 1)[-1] == name]
            if len(exact) == 1:
                return exact[0]
        else:
            called = set()
            for function in self.functions:
                if function is None or not isinstance(function["body"], dict) or "Structured" not in function["body"]:
                    continue
                block = function["body"]["Structured"]["body"]
                for statement in block["statements"]:
                    kind, call = _variant(statement["kind"])
                    if kind == "Call":
                        try:
                            called.add(self.call_target(call["call"]["func"]))
                        except ValueError:
                            continue
            roots = candidates.keys() - called
            if len(roots) == 1:
                return next(iter(roots))
        raise ValueError(f"{self.source}: select one local Rust entry. Available: {', '.join(candidates.values())}")

    def lower(self, identifier, as_entry=False):
        if identifier in self.graphs:
            return self.graphs[identifier]
        record = _sequence_index(self.functions, identifier)
        key = "::".join(_path(record, identifier)) or f"function_{identifier}"
        if identifier in self.active_functions or len(self.active_functions) >= 48:
            raise ValueError(f"{self.source}:{key}: recursive or overly deep Rust call")
        try:
            tag, data = _variant(record["body"])
            if tag != "Structured" or record["signature"]["is_unsafe"] or record["signature"]["abi"] != "Rust":
                raise ValueError("The called function must have a safe Rust structured body")
            locals_ = data["locals"]
            count = locals_["arg_count"]
            if type(count) is not int or not 0 <= count <= 3:
                raise ValueError("At most three Rust argument connectors are supported")
            ordered = locals_["locals"][:count+1]
            if len(ordered) != count+1 or any(not item or item["index"] != i for i, item in enumerate(ordered)):
                raise ValueError("The LLBC return and arguments must retain their actual local IDs")
            if len(record["signature"]["inputs"]) != count:
                raise ValueError("Signature and body disagree on argument arity")
            result_type = self.type(ordered[0]["ty"])
            if result_type != self.type(record["signature"]["output"]):
                raise ValueError("Signature and body disagree on the return type")
            if result_type.kind not in {"unit", "scalar", "record", "array"}:
                raise ValueError("Escaping Rust references and unsupported values cannot be returned")
            parameters = []
            outputs = [("result", result_type)] if result_type != _UNIT else []
            for position, local in enumerate(ordered[1:], 1):
                model = self.type(local["ty"])
                if model != self.type(record["signature"]["inputs"][position-1]):
                    raise ValueError("Signature and body disagree on an argument type")
                if as_entry and model.kind == "reference":
                    raise ValueError("Entry references need an owning Rust caller")
                if as_entry and model.contains_array():
                    raise ValueError("A public Rust input cannot enforce the fixed array length on a FROG connector")
                value_type = model.items if model.kind == "reference" else model
                name = local["name"] if local["name"] and _SYMBOL.fullmatch(local["name"]) else f"arg{position}"
                if name == "result" or any(parameter["name"] == name for parameter in parameters):
                    name += f"_{position}"
                parameters.append({"id": position, "name": name, "type": value_type.frog(), "model": value_type,
                                   "rust_type": model})
                if model.kind == "reference" and model.name == "Mut":
                    outputs.append((f"updated_{name}", model.items))
            if not outputs or len(outputs) > (1 if count == 3 else 2):
                raise ValueError("The Rust result and mutations exceed the editable connector profile")
            graph = CharonGraph(key, self.source, parameters, outputs, self.prototypes)
            if location := self.source_location(record["item_meta"]):
                graph.origin += f" · {location}"
            self.active_functions.add(identifier)
            evaluator = Function(self, graph, locals_["locals"])
            for parameter in parameters:
                local = parameter["id"]
                rust_type = parameter["rust_type"]
                value = Data(parameter["model"], graph.arguments[local])
                if rust_type.kind == "reference":
                    evaluator.values[-local] = value
                    evaluator.values[local] = Borrow(rust_type, Address(-local, writable=rust_type.name == "Mut"))
                else:
                    evaluator.values[local] = value
            evaluator.run(data["body"])
            result = {}
            if result_type != _UNIT:
                result["result"] = evaluator.current(Address(0))
            for parameter in parameters:
                if parameter["rust_type"].name == "Mut":
                    result[f"updated_{parameter['name']}"] = evaluator.current(Address(-parameter["id"]))
            graph.complete(result)
            self.active_functions.remove(identifier)
            self.graphs[identifier] = graph
            return graph
        except (KeyError, IndexError, TypeError) as error:
            raise ValueError(f"{self.source}:{key}: invalid Charon LLBC data ({error})") from error
        except ValueError as error:
            if str(error).startswith(f"{self.source}:"):
                raise
            raise ValueError(f"{self.source}:{key}: {error}") from error


class Function:
    def __init__(self, crate, graph, locals_):
        self.crate = crate
        self.graph = graph
        self.locals = locals_
        self.values = {}

    def type_of(self, place):
        return self.crate.type(place["ty"])

    def address(self, place):
        kind, data = _variant(place["kind"])
        if kind == "Local":
            local = _sequence_index(self.locals, data)
            if self.crate.type(local["ty"]) != self.type_of(place):
                raise ValueError(f"LLBC local {data} disagrees with its declaration")
            return Address(data)
        if kind == "Projection":
            base, projection = data
            source = self.address(base)
            kind, data = _variant(projection)
            if kind == "Deref":
                borrower = self.current(source)
                if isinstance(borrower, Borrow) and borrower.ty.items == self.type_of(place):
                    return borrower.address
                raise ValueError("Deref needs a live typed Rust borrow")
            if kind == "Field":
                variant, index = data
                shape = self.type_of(base)
                if (variant is None and shape.kind == "record" and 0 <= index < len(shape.fields)
                        and shape.fields[index][1] == self.type_of(place)):
                    return source.child(index)
            if kind == "Index":
                if not data["from_end"]:
                    return self.index(source, self.operand(data["offset"]), self.type_of(base), self.type_of(place))
        raise ValueError(f"Unsupported LLBC place projection {kind}")

    def index(self, address, index, shape, target):
        if (shape.kind != "array" or shape.items != target or index.ty != _INDEX
                or type(index.literal) is not int or not 0 <= index.literal < shape.count):
            raise ValueError("A fixed Rust array requires a statically in-bounds usize index")
        return address.child(index.literal)

    def current(self, address):
        value = self.values.get(address.local)
        if value is None:
            raise ValueError(f"Rust local {address.local} has no initialized value")
        for part in address.parts:
            value = self.graph.project(value, part)
        return value

    def write(self, address, value):
        if not address.writable:
            raise ValueError("A shared Rust borrow cannot be written")
        if not address.parts:
            self.values[address.local] = value
            return
        owner = self.values.get(address.local)
        if not isinstance(owner, Data):
            raise ValueError(f"Rust local {address.local} has no initialized aggregate to update")

        def replace(base, parts):
            index = parts[0]
            revised = replace(self.graph.project(base, index), parts[1:]) if len(parts) > 1 else value
            return self.graph.update(base, index, revised)

        self.values[address.local] = replace(owner, address.parts)

    def constant(self, raw):
        inner, raw_type = _untagged(raw)
        model = self.crate.type(raw_type)
        kind, data = _variant(inner)
        if model == _UNIT and kind == "Adt" and data == [None, []]:
            return Data(_UNIT)
        if kind == "Integer" and model.kind in {"index", "scalar"}:
            literal = self.crate.integer_constant(raw)
            if model == _INDEX:
                if not 0 <= literal < 1 << 64:
                    raise ValueError("The Charon usize constant is outside a 64-bit host")
                return Data(model, literal=literal)
            dtype = model.frog()
            if dtype not in _INTEGER_TYPES:
                raise ValueError("The LLBC literal is not an integer type")
            width = int(dtype[1:])
            signed = dtype.startswith("i")
            if not (-(1 << (width-1)) if signed else 0) <= literal < 1 << (width-signed):
                raise ValueError(f"Constant outside Rust {dtype}")
            return Data(model, self.graph.typed_constant(dtype, literal), literal)
        if kind == "Bool" and model.name == "bool" and type(data) is bool:
            return Data(model, self.graph.typed_constant("bool", data), data)
        if kind == "Array" and model.kind == "array":
            return self.aggregate(model, [self.constant(item) for item in data])
        if kind == "Adt" and model.kind == "record" and data[0] is None:
            return self.aggregate(model, [self.constant(item) for item in data[1]])
        raise ValueError(f"Unsupported LLBC constant {kind}")

    def operand(self, raw):
        kind, data = _variant(raw)
        if kind == "Const":
            return self.constant(data)
        if kind in {"Move", "Read", "Copy"}:
            place = data[0] if kind == "Copy" else data
            return self.current(self.address(place))
        raise ValueError(f"Unsupported LLBC operand {kind}")

    def aggregate(self, model, fields):
        if model == _UNIT and not fields:
            return Data(_UNIT)
        expected = [item for _, item in model.fields] if model.kind == "record" else [model.items]*model.count
        if model.kind not in {"record", "array"} or [part.ty for part in fields] != expected:
            raise ValueError("Rust aggregate fields must match their exact declared order and types")
        return self.graph.construct(model, fields)

    def expression(self, raw, expected):
        kind, data = _variant(raw)
        if kind == "Use":
            return self.operand(data[0])
        if kind == "Ref":
            borrow_kind = data["kind"]
            if borrow_kind not in {"Shared", "Mut", "TwoPhaseMut"} or self.operand(data["ptr_metadata"]).ty != _UNIT:
                raise ValueError(f"Unsupported Rust borrow {borrow_kind}")
            place = self.address(data["place"])
            self.current(place)
            mutable = borrow_kind != "Shared"
            if mutable and not place.writable:
                raise ValueError("A mutable Rust borrow requires a writable place")
            target = self.type_of(data["place"])
            return Borrow(Type("reference", "Mut" if mutable else "Shared", items=target),
                          Address(place.local, place.parts, mutable))
        if kind == "Aggregate":
            aggregate, operands = data
            tag, spec = _variant(aggregate)
            if tag == "Adt" and spec[1:] == [None, None] and self.crate.type_kind({"Adt": spec[0]}) == expected:
                return self.aggregate(expected, [self.operand(part) for part in operands])
            if (tag == "Array" and expected.kind == "array" and self.crate.type(spec[0]) == expected.items
                    and self.crate.integer_constant(spec[1]) == expected.count):
                return self.aggregate(expected, [self.operand(part) for part in operands])
        if kind == "Repeat" and expected.kind == "array":
            count = self.crate.integer_constant(data["count"])
            if count == expected.count:
                return self.aggregate(expected, [self.operand(data["operand"])]*count)
        if kind == "BinaryOp":
            (operation, overflow), left, right = _variant(data[0]), self.operand(data[1]), self.operand(data[2])
            if left.ty != right.ty:
                raise ValueError("An LLBC binary operation must retain its exact operand types")
            if operation in _COMPARISON and left.ty.kind in {"scalar", "index"}:
                literal = None
                if left.literal is not None and right.literal is not None:
                    a, b = left.literal, right.literal
                    literal = {"Eq": a == b, "Ne": a != b, "Lt": a < b, "Le": a <= b, "Gt": a > b, "Ge": a >= b}[operation]
                if left.ty.kind == "index":
                    if literal is None:
                        raise ValueError("Dynamic usize comparisons are outside the fixed-index profile")
                    return Data(Type("scalar", "bool"), self.graph.typed_constant("bool", literal), literal)
                result = self.graph.comparison(operation, [left.scalar(), right.scalar()])
                return Data(Type("scalar", "bool"), result.endpoint, literal)
            if operation in _ARITHMETIC and overflow == "Wrap" and left.ty.name in _INTEGER_TYPES:
                return Data(left.ty, self.graph.binary(_ARITHMETIC[operation], [left.endpoint, right.endpoint]))
            if operation in _BOOL_BITWISE and left.ty.name == "bool":
                return Data(left.ty, self.graph.binary(_BOOL_BITWISE[operation], [left.endpoint, right.endpoint]))
            raise ValueError(f"Unsupported LLBC operation {operation} with overflow mode {overflow}")
        if kind == "UnaryOp" and data[0] == "Not":
            value = self.operand(data[1])
            if value.ty.name == "bool":
                return Data(value.ty, self.graph.logical_not(value.endpoint))
        raise ValueError(f"Unsupported LLBC rvalue {kind}")

    def call(self, call, destination):
        if call["safety"] not in {"Inherit", "Safe"}:
            raise ValueError("Unsafe Rust calls are outside this LLBC profile")
        identifier = self.crate.call_target(call["func"])
        arguments = [self.operand(operand) for operand in call["args"]]
        intrinsic = self.crate.array_index_intrinsic(identifier)
        if intrinsic:
            if len(arguments) != 2 or not isinstance(arguments[0], Borrow):
                raise ValueError("Index needs a typed borrowed fixed array")
            owner, index = arguments
            model = self.type_of(destination)
            if model.kind != "reference" or (model.name == "Mut") != (intrinsic == "IndexMut"):
                raise ValueError("Index result has an incompatible Rust borrow type")
            location = self.index(owner.address, index, owner.ty.items, model.items)
            if model.name == "Mut" and not location.writable:
                raise ValueError("IndexMut needs a writable Rust array")
            self.write(self.address(destination), Borrow(model, Address(location.local, location.parts, model.name == "Mut")))
            return
        child = self.crate.lower(identifier)
        if len(arguments) != len(child.parameters):
            raise ValueError("LLBC call and its implementation disagree on argument arity")
        lowered = []
        updates = []
        for parameter, argument in zip(child.parameters, arguments, strict=True):
            model = parameter["rust_type"]
            if argument.ty != model:
                raise ValueError(f"LLBC call argument {parameter['name']} has a different type")
            if isinstance(argument, Borrow):
                lowered.append(self.current(argument.address))
                if model.name == "Mut":
                    address = argument.address
                    if any(previous.local == address.local and
                           previous.parts[:min(len(previous.parts), len(address.parts))] ==
                           address.parts[:min(len(previous.parts), len(address.parts))] for previous, _ in updates):
                        raise ValueError("Overlapping mutable Rust call arguments")
                    updates.append((address, f"updated_{parameter['name']}"))
            else:
                lowered.append(argument)
        outputs = self.graph.subfrog(child, lowered)
        for address, name in updates:
            self.write(address, outputs[name])
        actual = outputs.get("result", Data(_UNIT))
        if self.type_of(destination) != actual.ty:
            raise ValueError("LLBC call destination differs from its actual return type")
        self.write(self.address(destination), actual)

    def run(self, block):
        statements = block["statements"]
        for position, statement in enumerate(statements):
            try:
                if len(self.graph.nodes) > _MAX_NODES:
                    raise ValueError("The editable LLBC graph exceeds the node limit")
                first_generated_node = len(self.graph.nodes)
                kind, data = _variant(statement["kind"])
                if kind in {"StorageLive", "StorageDead"}:
                    self.values.pop(data, None)
                elif kind == "Assign":
                    place, value = data
                    model = self.type_of(place)
                    result = self.expression(value, model)
                    if result.ty != model:
                        raise ValueError("LLBC assignment has a different source and destination type")
                    self.write(self.address(place), result)
                elif kind == "Call":
                    call, destination, unwind = data["call"], data["dest"], data["on_unwind"]
                    if any(_variant(item["kind"])[0] not in {"UnwindResume", "Nop"} for item in unwind["statements"]):
                        raise ValueError("LLBC call has active unwind effects")
                    self.call(call, destination)
                elif kind == "Assert":
                    assertion = data["assert"]
                    test = self.operand(assertion["cond"])
                    if type(test.literal) is not bool or test.literal != assertion["expected"]:
                        raise ValueError("The assertion cannot be proven without Rust panic effects")
                elif kind == "PlaceMention":
                    self.current(self.address(data))
                elif kind in {"Nop", "Borrowck"}:
                    continue
                elif kind == "Return" and position+1 == len(statements):
                    return
                else:
                    raise ValueError(f"Unsupported LLBC statement {kind}")
                if first_generated_node < len(self.graph.nodes):
                    statement_id = statement.get("id", position)
                    location = self.crate.source_location(statement)
                    for node in self.graph.nodes[first_generated_node:]:
                        self.graph.source_nodes[node["id"]] = {"statement_id": statement_id, "location": location}
            except (ValueError, KeyError, IndexError, TypeError, AttributeError) as error:
                if str(error).startswith(f"{self.crate.source}:"):
                    raise
                at = f"{self.crate.source}:{self.graph.key}:LLBC statement {statement.get('id', position)}"
                location = self.crate.source_location(statement)
                if location:
                    at += f" ({location})"
                raise ValueError(f"{at}: {error}") from error
        raise ValueError("The LLBC structured body does not end in Return")


def compile_llbc(crate_json, entry, source_name):
    """Return the selected editable entry and its actual reachable sub-FROGs."""
    try:
        if isinstance(crate_json, (str, bytes)):
            if len(crate_json) > 32 * 1024 * 1024:
                raise ValueError("Charon LLBC exceeds the 32 MiB input limit")
            crate_json = json.loads(crate_json)
        translated = Crate(crate_json, source_name)
        main = translated.lower(translated.entry(entry), as_entry=True)
        return main, {graph.key: graph for graph in translated.graphs.values()}
    except (KeyError, IndexError, TypeError, AttributeError, RecursionError) as error:
        raise ValueError(f"{source_name}: invalid or overly nested Charon LLBC data ({error})") from error
