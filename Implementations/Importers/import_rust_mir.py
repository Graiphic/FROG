# SPDX-License-Identifier: Apache-2.0
"""Translate the bounded, pure scalar rustc 1.96 MIR profile into editable FROG."""

from __future__ import annotations

import copy
from dataclasses import dataclass
import importlib.util
import json
import math
from pathlib import Path
import re


_C_IMPORTER = Path(__file__).with_name("Import-CProject.py")
_C_SPEC = importlib.util.spec_from_file_location("_frog_rust_graph_support", _C_IMPORTER)
_C_MODULE = importlib.util.module_from_spec(_C_SPEC)
_C_SPEC.loader.exec_module(_C_MODULE)
Graph = _C_MODULE.Graph
bounds = _C_MODULE.bounds

_IDENTIFIER = r"[a-zA-Z_][a-zA-Z_0-9]*"
_SYMBOL = rf"{_IDENTIFIER}(?:::{_IDENTIFIER})*"
_FUNCTION = re.compile(r"^fn (?P<name>\S+?)\((?P<args>.*)\) -> (?P<result>.+) \{$")
_LOCAL = re.compile(r"_(?:0|[1-9][0-9]*)")
_DECLARATION = re.compile(r"let (?:mut )?(_[0-9]+): (.+);")
_DEBUG = re.compile(rf"debug ({_IDENTIFIER}) => (_[0-9]+);")
_BLOCK = re.compile(r"(bb[0-9]+)(?: \(cleanup\))?: \{")
_ASSIGN = re.compile(r"(_[0-9]+) = (.+);")
_CALL = re.compile(r"(.+)\((.*)\) -> \[return: (bb[0-9]+), unwind (?:continue|unreachable)\]")
_GOTO = re.compile(r"goto -> (bb[0-9]+);")
_SWITCH = re.compile(r"switchInt\((.+)\) -> \[0: (bb[0-9]+), otherwise: (bb[0-9]+)\];")
_OPERATION = re.compile(r"([A-Za-z]+)\((.*)\)")
_NUMBER = re.compile(r"(-?[0-9]{1,21})_([iu](?:8|16|32|64))")
_NUMBER_BOUND = re.compile(r"core::num::<impl ([iu](?:8|16|32|64))>::(MIN|MAX)")
_REAL = re.compile(r"(-?[0-9]{1,24}(?:\.[0-9]{1,24})?)([eE][+-]?[0-9]{1,4})?f64")
_COPY = re.compile(r"(?:copy|move) (_[0-9]+)")
_INTEGER_TYPES = frozenset(f"{sign}{width}" for sign in "iu" for width in (8, 16, 32, 64))
_NUMERIC_TYPES = _INTEGER_TYPES | {"f64"}
_TYPES = _NUMERIC_TYPES | {"bool"}
_ARITHMETIC = {"Add": "+", "Sub": "-", "Mul": "*"}
_BOOL_BITWISE = {"BitAnd": "&", "BitOr": "|", "BitXor": "^"}
_COMPARISON = {
    "Eq": "equal", "Ne": "not_equal", "Lt": "less", "Le": "less_or_equal",
    "Gt": "greater", "Ge": "greater_or_equal",
}


@dataclass(frozen=True)
class MirFunction:
    name: str
    arguments: str
    result: str
    header_line: int
    declarations: tuple[tuple[int, str], ...]
    blocks: dict[str, tuple[tuple[int, str], ...]]


@dataclass(frozen=True)
class Value:
    dtype: str
    endpoint: tuple[str, str]


class RustGraph(Graph):
    def __init__(self, function, source_name, parameters, prototypes):
        self.input_types = {parameter["name"]: parameter["type"] for parameter in parameters}
        self.three_inputs = len(parameters) == 3
        self.prototypes = prototypes
        super().__init__(function.name,
                         f"Rust MIR · {source_name} · {function.name} · pure scalar",
                         source_name, parameters, function.result)

    def widget(self, name, index, control):
        role = "control" if control else "indicator"
        port_key = "public_input_id" if control else "public_output_id"
        input_slots = (1, 3, 4) if self.three_inputs else (1, 3)
        slot = f"zone_{input_slots[index]}" if control else "zone_2"
        dtype = self.input_types[name] if control else self.value_type
        ident = f"{role}-{name}"
        widget = copy.deepcopy(self.prototypes[("boolean_" if dtype == "bool" else "numeric_") + role])
        widget.update(id=ident, role=role, valueType=dtype, label=name)
        widget["bounds"].update(x=40 if control else 340, y=80 + index * 90)
        props = widget.setdefault("props", {})
        props.pop("data.default", None)
        props.update({"label.visible": True, "caption.independent": True, "caption.visible": False})
        if dtype != "bool":
            named_size = {"i": "Int", "u": "UInt", "f": "Float"}[dtype[0]] + dtype[1:]
            props.update({"data_type.representation": dtype,
                          "data_type.named_numeric_size": named_size,
                          "representation.kind": named_size.lower()})
        widget["state"] = {"value": False if dtype == "bool" else 0}
        widget["binding"] = {"mode": "widget_value", port_key: name, "interface_map_slot": slot,
                             "connection_requirement": "required" if control else "recommended"}
        props["data.default"] = json.dumps(copy.deepcopy(widget), ensure_ascii=False)
        self.widgets.append(widget)
        x = 80 if control else 250 + self.column * 180
        return self.node(ident, "widget_value", bounds(x, 105 + index * 85, 65, 30),
                         widget=ident, type=dtype, label=name), "value"

    def typed_constant(self, dtype, value):
        ident = f"constant-{len(self.nodes)}"
        x = 180 + self.column * 180
        y = 166 + len(self.nodes) * 34
        if dtype == "bool":
            token = str(value).lower()
            return self.node(ident, "primitive", bounds(x, y, 34, 24),
                             type=f"frog.studio.boolean.{token}_constant",
                             value_type=dtype, text=token), "value"
        return self.node(ident, "primitive", bounds(x, y, 68, 28),
                         type="frog.studio.numeric.numeric_constant",
                         value_type=dtype, text=str(value)), "value"

    def call(self, child, values):
        endpoint = super().call(child, values)
        ports = self.nodes[-1]["call"]["ports"]
        for port in ports:
            if port["direction"] == "input":
                port["value_type"] = child.input_types[port["id"]]
                if child.three_inputs:
                    index = child.inputs.index(port["id"])
                    port["interface_map_slot"] = ("zone_1", "zone_3", "zone_4")[index]
                    port["normalized_y"] = (index + 0.5) / 3
            elif child.three_inputs:
                port["normalized_y"] = 0.5
        return endpoint

    def draft(self, entry=False):
        document = copy.deepcopy(super().draft())
        node_kinds = {node["kind"] for node in self.nodes}
        if "frog_call" not in node_kinds:
            document["draft_revision"] = 3 if "structure" in node_kinds else 2
        if entry:
            for widget in document["frontPanel"]["widgets"]:
                widget.pop("binding")
            document["interface"] = {"inputs": [], "outputs": []}
        else:
            for binding in document["interface"]["inputs"]:
                binding["type"] = self.input_types[binding["id"]]
            if self.three_inputs:
                document["interface"]["map"]["layout_pattern_id"] = "pattern_08"
        return document

    def comparison(self, opcode, values):
        operation = _COMPARISON[opcode]
        ident = self.node(f"comparison-{len(self.nodes)}", "primitive", self.operation_layout(),
                          type=f"frog.studio.comparison.{operation}",
                          palette_catalog_id=f"comparison.{operation}")
        for value, port in zip(values, ("x", "y"), strict=True):
            self.wire(value.endpoint, ident, port)
        return Value("bool", (ident, "result"))

    def select(self, condition, if_true, if_false):
        if if_true == if_false:
            return if_true
        ident = self.node(f"select-{len(self.nodes)}", "primitive", self.operation_layout(),
                          type="frog.studio.structures.select", palette_catalog_id="structures.select")
        for value, port in ((condition, "select"), (if_true, "true"), (if_false, "false")):
            self.wire(value.endpoint, ident, port)
        return Value(if_true.dtype, (ident, "result"))


def _parse_functions(text, source):
    functions = {}
    lines = iter(enumerate(text.splitlines(), 1))
    for number, line in lines:
        header = _FUNCTION.fullmatch(line)
        if header is None:
            continue
        name, args, result = header.group("name", "args", "result")
        if name in functions:
            raise ValueError(f"{source}:{name}:MIR line {number}: duplicate MIR symbol {name}")
        declarations = []
        blocks = {}
        for body_line, body in lines:
            if body == "}":
                break
            item = body.strip()
            if not item:
                continue
            block = _BLOCK.fullmatch(item)
            if block is None:
                declarations.append((body_line, item))
                continue
            label = block[1]
            if label in blocks:
                raise ValueError(f"{source}:{name}:{label}:MIR line {body_line}: duplicate MIR block")
            statements = []
            for statement_line, statement in lines:
                statement = statement.strip()
                if statement == "}":
                    break
                if statement:
                    statements.append((statement_line, statement))
            if "(cleanup)" in item:
                statements.insert(0, (body_line, "(cleanup)"))
            blocks[label] = tuple(statements)
        else:
            raise ValueError(f"{source}:{name}:MIR line {number}: unterminated MIR function")
        functions[name] = MirFunction(name, args, result, number, tuple(declarations), blocks)
    return functions


def _split_arguments(operands):
    return [operand.strip() for operand in operands.split(",")] if operands else []


def _used_symbols(function):
    for statements in function.blocks.values():
        for _, statement in statements:
            parsed = _ASSIGN.fullmatch(statement)
            if parsed:
                call = _CALL.fullmatch(parsed[2])
                if call and re.fullmatch(_SYMBOL, call[1]):
                    yield call[1]


def _successors(statements):
    if not statements:
        return ()
    text = statements[-1][1]
    goto = _GOTO.fullmatch(text)
    if goto:
        return (goto[1],)
    switch = _SWITCH.fullmatch(text)
    if switch:
        return (switch[2], switch[3])
    assignment = _ASSIGN.fullmatch(text)
    if assignment:
        call = _CALL.fullmatch(assignment[2])
        if call:
            return (call[3],)
    return ()


def _has_reachable_cycle(function):
    active = set()
    visited = set()
    pending = [("bb0", False)]
    while pending:
        block, leaving = pending.pop()
        if leaving:
            active.remove(block)
            visited.add(block)
        elif block in active:
            return True
        elif block not in visited:
            active.add(block)
            pending.append((block, True))
            for target in _successors(function.blocks.get(block)):
                pending.append((target, False))
    return False


def _integer_range(dtype):
    width = int(dtype[1:])
    if dtype[0] == "i":
        return -(1 << (width - 1)), (1 << (width - 1)) - 1
    return 0, (1 << width) - 1


class Translator:
    def __init__(self, functions, source):
        self.functions = functions
        self.source = source
        self.graphs = {}
        self.active_functions = set()
        self.prototypes = json.loads(Path(__file__).with_name("Import-WidgetPrototypes.json")
                                     .read_text(encoding="utf-8"))

    def refuse(self, function, line, message, block=None):
        location = f"{self.source}:{function.name}"
        if block is not None:
            location += f":{block}"
        raise ValueError(f"{location}:MIR line {line}: {message}")

    def lower(self, name, call_depth=0):
        if name in self.graphs:
            return self.graphs[name]
        function = self.functions[name]
        header = function.header_line
        if name in self.active_functions:
            self.refuse(function, header, "Recursive calls are outside the pure scalar profile")
        if len(self.active_functions) >= 64 or len(self.graphs) >= 128:
            self.refuse(function, header, "The reachable function graph exceeds the import limit")
        if not re.fullmatch(_SYMBOL, name):
            self.refuse(function, header, "Only named non-generic Rust functions are supported")
        if len(name.split("::")[-1]) > 128:
            self.refuse(function, header, "The symbol exceeds the editable document name limit")
        if function.result not in _TYPES:
            self.refuse(function, header, f"Unsupported return type {function.result}")

        types = {"_0": function.result}
        parameter_ids = []
        for argument in _split_arguments(function.arguments):
            local, separator, dtype = argument.partition(": ")
            if not separator or not _LOCAL.fullmatch(local) or local in types or dtype not in _TYPES:
                self.refuse(function, header, f"Unsupported parameter {argument}")
            types[local] = dtype
            parameter_ids.append(local)
        if len(parameter_ids) > 3:
            self.refuse(function, header, "The editable connector supports at most three scalar inputs")

        debug_names = {}
        for line, declaration in function.declarations:
            local = _DECLARATION.fullmatch(declaration)
            if local:
                if local[1] in types and (local[1] != "_0" or local[2] != function.result):
                    self.refuse(function, line, f"Conflicting declaration of {local[1]}")
                types[local[1]] = local[2]
                continue
            debug = _DEBUG.fullmatch(declaration)
            if debug:
                debug_names.setdefault(debug[2], debug[1])
                continue
            if declaration == "}" or (declaration.startswith("scope ") and declaration.endswith("{")):
                continue
            self.refuse(function, line, f"Unsupported MIR declaration {declaration}")

        parameter_names = set()
        parameters = []
        for local in parameter_ids:
            label = debug_names.get(local, f"arg{local[1:]}")
            while label in parameter_names:
                label += "_"
            parameter_names.add(label)
            parameters.append({"id": local, "name": label, "type": types[local]})

        self.active_functions.add(name)
        graph = RustGraph(function, self.source, parameters, self.prototypes)
        lowerer = FunctionLowerer(self, function, graph, types, call_depth)
        values = {local: Value(types[local], endpoint) for local, endpoint in graph.arguments.items()}
        result = None
        if _has_reachable_cycle(function):
            try:
                from import_rust_loop import lower_counted_loop
            except ModuleNotFoundError as error:
                if error.name != "import_rust_loop":
                    raise
                self.refuse(function, header, "This importer package is missing the Rust counted-loop backend")
            result = lower_counted_loop(lowerer, values)
        if result is None:
            result = lowerer.walk("bb0", values, frozenset())
        graph.finish(result.endpoint)
        if lowerer.uses_f64 and lowerer.first_call:
            line, block, target = lowerer.first_call
            self.refuse(function, line, f"The exact f64 profile does not support a call to {target}", block)
        if lowerer.uses_f64 and _INTEGER_TYPES.intersection(types.values()):
            self.refuse(function, header, "The exact f64 profile supports float and Boolean locals only")
        graph.uses_f64 = lowerer.uses_f64
        self.active_functions.remove(name)
        self.graphs[name] = graph
        return graph


class FunctionLowerer:
    def __init__(self, translator, function, graph, types, call_depth):
        self.translator = translator
        self.function = function
        self.graph = graph
        self.types = types
        self.uses_f64 = "f64" in types.values()
        self.first_call = None
        self.call_depth = call_depth
        self.expanded_blocks = 0
        self.line = function.header_line
        self.block = None

    def refuse(self, message):
        self.translator.refuse(self.function, self.line, message, self.block)

    def require_type(self, value, expected, context):
        if value.dtype != expected:
            self.refuse(f"{context} needs {expected}, found {value.dtype}")

    def operand(self, operand, values):
        if operand in ("const true", "const false"):
            return Value("bool", self.graph.typed_constant("bool", operand == "const true"))
        if operand.startswith("const "):
            literal = operand[6:]
            number = _NUMBER.fullmatch(literal)
            if number:
                dtype = number[2]
                value = int(number[1])
                low, high = _integer_range(dtype)
                if not low <= value <= high:
                    self.refuse(f"Constant {literal} is outside {dtype}")
                return Value(dtype, self.graph.typed_constant(dtype, value))
            bound = _NUMBER_BOUND.fullmatch(literal)
            if bound:
                dtype = bound[1]
                value = _integer_range(dtype)[bound[2] == "MAX"]
                return Value(dtype, self.graph.typed_constant(dtype, value))
            real = _REAL.fullmatch(literal)
            if real:
                mantissa, exponent = real.groups()
                value = float(mantissa + (exponent or ""))
                if not math.isfinite(value) or (value == 0 and any(c in "123456789" for c in mantissa)):
                    self.refuse(f"Constant {literal} is outside the finite f64 literal profile")
                self.uses_f64 = True
                return Value("f64", self.graph.typed_constant("f64", value))
        local = _COPY.fullmatch(operand)
        if local:
            if local[1] not in values:
                self.refuse(f"MIR local {local[1]} has no initialized scalar value on this path")
            return values[local[1]]
        self.refuse(f"Unsupported MIR operand {operand}")

    def expression(self, expression, values):
        operation = _OPERATION.fullmatch(expression)
        if operation is None:
            return self.operand(expression, values)
        op, arguments = operation.groups()
        operands = _split_arguments(arguments)
        if op in _ARITHMETIC or op in _BOOL_BITWISE or op in _COMPARISON:
            if len(operands) != 2:
                self.refuse(f"{op} does not have two operands")
            left, right = [self.operand(operand, values) for operand in operands]
            self.require_type(right, left.dtype, op)
            if op in _COMPARISON:
                if left.dtype == "bool" and op not in ("Eq", "Ne"):
                    self.refuse(f"{op} only supports integers in this profile")
                return self.graph.comparison(op, [left, right])
            if op in _BOOL_BITWISE:
                self.require_type(left, "bool", op)
                token = _BOOL_BITWISE[op]
            else:
                if left.dtype not in _NUMERIC_TYPES:
                    self.refuse(f"{op} only supports admitted numeric types")
                token = _ARITHMETIC[op]
            return Value(left.dtype, self.graph.binary(token, [left.endpoint, right.endpoint]))
        if op == "Not" and len(operands) == 1:
            value = self.operand(operands[0], values)
            self.require_type(value, "bool", op)
            return Value("bool", self.graph.logical_not(value.endpoint))
        if op == "Neg" and len(operands) == 1:
            value = self.operand(operands[0], values)
            if value.dtype not in _INTEGER_TYPES or not value.dtype.startswith("i"):
                self.refuse("Neg only supports signed fixed-width integers")
            zero = self.graph.typed_constant(value.dtype, 0)
            return Value(value.dtype, self.graph.binary("-", [zero, value.endpoint]))
        self.refuse(f"Unsupported MIR operation {op}")

    def call(self, call, values, depth):
        name, operands, continuation = call.groups()
        if name not in self.translator.functions or not re.fullmatch(_SYMBOL, name):
            self.refuse(f"Unsupported call to {name}; only direct calls to imported Rust functions are supported")
        if name in self.translator.active_functions:
            self.refuse(f"Recursive call to {name} is outside the pure scalar profile")
        child = self.translator.lower(name, self.call_depth + depth)
        # The composed scalar callback rejects nonfinite f64 values. Rust f64
        # is unrestricted, so finite examples cannot qualify that call path.
        if self.uses_f64 or child.uses_f64:
            self.refuse(f"The exact f64 profile does not support a call to {name}")
        if self.first_call is None:
            self.first_call = (self.line, self.block, name)
        arguments = [self.operand(operand, values) for operand in _split_arguments(operands)]
        if len(arguments) != len(child.inputs):
            self.refuse(f"Call to {name} has {len(arguments)} arguments, expected {len(child.inputs)}")
        for identifier, actual in zip(child.inputs, arguments, strict=True):
            self.require_type(actual, child.input_types[identifier], f"{name}.{identifier}")
        endpoint = self.graph.call(child, [argument.endpoint for argument in arguments])
        return Value(child.value_type, endpoint), continuation

    def walk(self, block, values, path):
        self.block = block
        self.line = self.function.header_line
        if block in path:
            self.refuse("Loops are outside the pure scalar profile")
        self.expanded_blocks += 1
        if self.call_depth + len(path) >= 128 or self.expanded_blocks > 512:
            self.refuse("The acyclic control flow exceeds the import limit")
        path = path | {block}
        statements = self.function.blocks.get(block)
        if not statements:
            self.refuse("Missing or empty MIR block")
        for index, (self.line, text) in enumerate(statements):
            if len(self.graph.nodes) > 512:
                self.refuse("The editable graph exceeds the node limit")
            terminal = index == len(statements) - 1
            if text == "return;" and terminal:
                if "_0" not in values:
                    self.refuse("The result is uninitialized on this path")
                return values["_0"]
            goto = _GOTO.fullmatch(text)
            if goto and terminal:
                return self.walk(goto[1], values, path)
            switch = _SWITCH.fullmatch(text)
            if switch and terminal:
                condition = self.operand(switch[1], values)
                self.require_type(condition, "bool", "A branch condition")
                # Only total scalar operations and calls are admitted. Evaluating both
                # arms before Select cannot introduce a new trap or side effect.
                if_false = self.walk(switch[2], values.copy(), path)
                if_true = self.walk(switch[3], values.copy(), path)
                return self.graph.select(condition, if_true, if_false)

            assignment = _ASSIGN.fullmatch(text)
            if assignment:
                local, expression = assignment.groups()
                dtype = self.types.get(local)
                if dtype not in _TYPES:
                    self.refuse(f"Unsupported assignment to {local} of type {dtype or 'unknown'}")
                call = _CALL.fullmatch(expression)
                if call and terminal:
                    value, continuation = self.call(call, values, len(path))
                    self.require_type(value, dtype, local)
                    values[local] = value
                    return self.walk(continuation, values, path)
                if not terminal:
                    value = self.expression(expression, values)
                    self.require_type(value, dtype, local)
                    values[local] = value
                    continue
            self.refuse(f"Unsupported MIR statement {text}")
        self.refuse("The block does not terminate")


def compile_graphs(mir_text: str, entry: str | None, source_name: str):
    """Return the chosen entry and its reachable editable sub-FROGs."""
    if len(mir_text) > 4 * 1024 * 1024:
        raise ValueError(f"{source_name}: MIR exceeds the 4 MiB text limit")
    functions = _parse_functions(mir_text, source_name)
    if not functions:
        raise ValueError(f"{source_name}: no Rust function MIR was found")
    available = ", ".join(sorted(functions))
    if entry is None:
        called = {symbol for function in functions.values() for symbol in _used_symbols(function)}
        roots = functions.keys() - called
        if len(roots) != 1:
            raise ValueError(f"{source_name}: select one entry function. Available: {available}")
        entry = next(iter(roots))
    if entry not in functions:
        raise ValueError(f"{source_name}: no MIR function named {entry}. Available: {available}")
    translator = Translator(functions, source_name)
    graph = translator.lower(entry)
    return graph, translator.graphs
