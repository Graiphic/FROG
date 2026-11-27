# SPDX-License-Identifier: Apache-2.0
"""Read LabVIEW connector and dependency identities without projecting a diagram."""

from collections import deque
from contextlib import contextmanager
from dataclasses import dataclass
import hashlib
import io
import json
from pathlib import Path
from types import MethodType
from xml.etree import ElementTree

from pylabview import LVdatafill, LVdatatype, LVheap, LVlinkinfo, LVmisc

from import_labview_profile import (
    ATTR_COUNT, ATTR_UID, ProfileRefusal, TYPES, Types, children, field, heap_root,
    identity, items, kind, number, one_section, require, valid_windows_component,
)


@dataclass(frozen=True)
class ConnectorType:
    family: str
    label: bytes | None
    definition: tuple[bytes, ...]
    detail: tuple
    children: tuple["ConnectorType", ...]
    complete: bool
    class_id: str | None = None

    def representation(self, *, include_label=False):
        return (self.family, self.label if include_label else None, self.definition,
                self.detail, tuple(child.representation(include_label=True) for child in self.children))

    def resolved_at_connector(self, binding):
        # A cached connector need not declare a library used only inside its callee.
        # Its complete saved descriptor is compared before consulting the actual owner's class.
        if not binding.complete or self.class_id and self.class_id != binding.class_id:
            return False
        if binding.class_id:
            return True
        if self.children:
            return all(a.resolved_at_connector(b) for a, b in zip(self.children, binding.children, strict=True))
        return self.complete

    def matches_connector_type(self, other):
        if self.representation() == other.representation():
            return True
        # NI can cache a different casing of a class filename in a caller.
        # Both local dependency links must independently resolve to the same class.
        return (self.complete and other.complete and self.class_id is not None and self.class_id == other.class_id
                and self.family == other.family == "Refnum" and not self.children and not other.children
                and self.definition == other.definition and self.detail[0] == other.detail[0] == 30
                and self.detail[2:] == other.detail[2:] and len(self.detail[1]) == len(other.detail[1]) == 1
                and self.detail[1][0].lower() == other.detail[1][0].lower())

    def field_label(self):
        if self.label or self.family != "TypeDef" or len(self.children) != 1:
            return self.label
        return self.children[0].field_label()

    def single_user_event_field(self):
        # LV20 EventReg stores the event signature on each client of the refnum.
        # This form registers a cluster containing exactly one live User Event.
        if (self.family != "Refnum" or self.detail !=
                (23, (), (("field0", 0), ("event_clients", ((1, 0, 1000),)))) or len(self.children) != 1):
            return None
        payload = self.children[0]
        while payload.family == "TypeDef" and len(payload.children) == 1:
            payload = payload.children[0]
        if payload.family != "Cluster" or not payload.complete or len(payload.children) != 1:
            return None
        source = payload.children[0]
        if source.field_label() and source.reference_class() == "frog.events.user":
            return source
        return None

    def reference_class(self):
        if self.family != "Refnum":
            return None
        if not self.children:
            for class_code, name in ((1, "frog.application"), (2, "frog.program"), (8, "frog.control.boolean")):
                if self.detail == (8, (), (("ctlflags", class_code), ("hasitem", 0), ("itmident", b"UNKN"))):
                    return name
            return None
        if len(self.children) != 1:
            return None
        if self.detail[1:] == ((), ()):
            return RESOURCE_CLASSES.get(self.detail[0])
        # Strict Array control view, with its saved element schema. Other VI
        # Server reference classes can carry a client descriptor as well.
        if self.detail == (8, (), (("ctlflags", 14), ("hasitem", 0), ("itmident", b"UNKN"))) and self.children[0].family == "Array":
            return "frog.control.array"
        return None

    def value_children(self):
        # The NI Bool VI Server class implies its Boolean value even when the
        # refnum stores no client TD. Keep the saved representation for matching.
        if not self.children and self.reference_class() == "frog.control.boolean":
            return (ConnectorType("Boolean", None, (), (), (), True),)
        return self.children

    def native_type(self, encoding, enums=None, definition=()):
        if not self.complete:
            return None
        if self.class_id:
            return f'object<"{self.class_id}">'
        if self.family in TYPES:
            return TYPES[self.family]
        if self.family in ("LVVariant", "Path"):
            return "variant" if self.family == "LVVariant" else "path"
        if self.family == "MeasureData" and self.detail == (LVdatatype.MEASURE_DATA_FLAVOR.TimeStamp.value,):
            return "timestamp"
        if self.family == "TypeDef":
            return self.children[0].native_type(encoding, enums, self.definition)
        if not self.children and self.reference_class():
            children = self.value_children()
            parameter = "," + children[0].native_type(encoding) if children else ""
            return f'ref<"{self.reference_class()}"{parameter}>'
        if self.family in ("UnitUInt8", "UnitUInt16", "UnitUInt32") and self.detail:
            if any(not isinstance(item[0], bytes) for item in self.detail):
                return None
            labels = [item[0].decode(encoding) for item in self.detail]
            scalar = "u" + self.family.removeprefix("UnitUInt")
            identity = json.dumps(([part.hex() for part in definition], labels, scalar), ensure_ascii=True)
            domain = "labview.enum." + hashlib.sha256(identity.encode()).hexdigest()[:24]
            dtype = f'enum<"{domain}",{scalar}>'
            if enums is not None:
                enums[dtype] = (domain, scalar, labels)
            return dtype
        elements = [child.native_type(encoding, enums) for child in self.children]
        if not elements or None in elements:
            return None
        if self.family == "Cluster":
            labels = [child.field_label() for child in self.children]
            if all(labels) and len(set(labels)) == len(labels):
                fields = [json.dumps(label.decode(encoding), ensure_ascii=False) + ":" + element
                          for label, element in zip(labels, elements)]
                return "cluster<" + ",".join(fields) + ">"
        if self.family == "Map":
            return "map<" + ",".join(elements) + ">"
        if self.family == "Array" and len(self.detail) in (1, 2) and all(
                dim == (255, 0xFFFFFF) for dim in self.detail):
            rank = "" if len(self.detail) == 1 else ",rank=2"
            return f"array<{elements[0]}{rank}>"
        if self.reference_class():
            return f'ref<"{self.reference_class()}",{elements[0]}>'
        return None


@dataclass(frozen=True)
class ConnectorBinding:
    slot: int
    widget_uid: int
    output: bool
    datatype: ConnectorType
    connector_flags: int
    call_flags: int


@dataclass(frozen=True)
class Connector:
    pattern: int
    source_id: int
    slots: int
    bindings: tuple[ConnectorBinding, ...]


@dataclass(frozen=True)
class Dependency:
    resource: str
    record: int
    kind: bytes
    qualified_name: tuple[bytes, ...]
    path_encoding: bytes
    path_type: int
    path_components: tuple[bytes, ...]
    diagram_offsets: tuple[int, ...]
    class_cache_tail: bytes


@dataclass(frozen=True)
class ResolvedDependency:
    caller: Path
    reference: Dependency
    target: Path | None
    status: str
    diagnostic: str | None = None


@dataclass(frozen=True)
class CallTerminal:
    slot: int
    terminal_uid: int
    binding_uid: int
    output: bool
    datatype: ConnectorType
    flags: int


@dataclass(frozen=True)
class CallLink:
    uid: int
    pattern: int
    slots: int
    dependency: Dependency
    specialization: Dependency | None
    terminals: tuple[CallTerminal, ...]


class ExactRead(io.BytesIO):
    def read(self, size=-1):
        result = super().read(size)
        if size >= 0 and len(result) != size:
            raise EOFError("Link table ended inside a field.")
        return result


_CLASS_LINKS = {b"VIPI", b"FPPI", b"DDPI"}
_LINK_TABLES = {b"LIvi": b"LVIN", b"LIbd": b"BDHP"}
_OUTPUT = 0x100
_SCALARS = {"Void", "Boolean", "BooleanU16", "String"}
RESOURCE_CLASSES = {18: "frog.sync.queue", 25: "frog.events.user"}


class EmptyMapDefault(LVdatafill.DataFill):
    def initWithRSRCParse(self, stream):
        InterfaceTypes(self.vi).map_elements(self.td, "DFDS")
        require(stream.read(4) == bytes(4), "map_default", "Expected an empty LV20 Map default.", "DFDS")
        self.value = []


@contextmanager
def lv20_default_reader(owner):
    """Scope the default factory to this document in the one-shot import process."""
    original = LVdatafill.newDataFillObject

    def make(vi, blockref, dtype, subtype, options):
        if vi is owner and dtype == 0x74 and vi.getFileVersion()["major"] == 20:
            return EmptyMapDefault(vi, blockref, dtype, subtype, options)
        return original(vi, blockref, dtype, subtype, options)

    LVdatafill.newDataFillObject = make
    try:
        yield
    finally:
        LVdatafill.newDataFillObject = original


def _read_class_cache_tail(link, stream):
    link._read_original_cache(stream)
    # The pinned parser stops before this LV20 class link cache suffix.
    # Preserve its bytes because their meaning has not been established.
    link.class_cache_tail = stream.read(9)


def _read_lv20_links(vi, resource, block, section_number, table_id):
    location = resource.decode("ascii")
    cursor = 0
    record_kind = None
    try:
        stream = ExactRead(block.getData(section_number).getvalue())
        require(stream.read(2) == b"\0\1" and stream.read(4) == table_id,
                "link_table", "Invalid LabVIEW link table header.", location)
        count = int.from_bytes(stream.read(4), "big")
        require(count <= len(stream.getvalue()) // 6, "link_table", "Invalid link record count.", location)
        records = []
        for index in range(count):
            cursor = stream.tell()
            location = f"{resource.decode('ascii')}/record:{index}@{cursor}"
            record_kind = None
            require(stream.read(2) == b"\0\2", "link_table", "Missing link record marker.", location)
            start = stream.tell()
            record_kind = stream.read(4)
            stream.seek(start)
            link = LVlinkinfo.newLinkObject(vi, (resource, section_number), table_id,
                                           record_kind, vi.po)
            if record_kind in _CLASS_LINKS:
                link._read_original_cache = link.parseUDClassAPILinkCache
                link.parseUDClassAPILinkCache = MethodType(_read_class_cache_tail, link)
            link.parseRSRCData(stream)
            require(stream.tell() > start + 4, "link_table", "The current link record has no decoded body.", location)
            records.append(link)
        cursor = stream.tell()
        record_kind = None
        location = f"{resource.decode('ascii')}/end@{cursor}"
        require(stream.read(2) == b"\0\3" and not stream.read(), "link_table",
                "The saved links do not end at the declared table boundary.", location)
        require(any(getattr(record, "class_cache_tail", b"") for record in records), "link_table",
                "No verified LabVIEW 20 class-cache extension explains the original parse failure.", location)
        return records
    except (EOFError, OSError, AttributeError, IndexError, KeyError, NotImplementedError, TypeError, ValueError) as error:
        if isinstance(error, ProfileRefusal):
            raise
        name = record_kind.decode("ascii", errors="backslashreplace") if record_kind else "header or marker"
        raise ProfileRefusal("link_table", f"Could not decode {name} at byte {cursor}: {error}", location) from error


def read_dependencies(vi, resource: bytes) -> tuple[Dependency, ...]:
    """Read declarations (LIvi) or heap association records (LIbd) in file order."""
    require(resource in _LINK_TABLES, "link_table", "Expected LIvi or LIbd.", "RSRC")
    location = resource.decode("ascii")
    block = vi.blocks.get(resource)
    if block is None:
        return ()
    require(len(block.sections) == 1, "link_table", "Expected one link table section.", location)
    section_number, section = next(iter(block.sections.items()))
    table_id = _LINK_TABLES[resource]
    if resource == b"LIvi" and getattr(getattr(vi, "ftype", None), "name", None) == "Control":
        table_id = b"LVCC"
    records = section.content
    if section.parse_failed:
        ver = vi.getFileVersion()
        require((ver["major"], ver["minor"], ver["bugfix"], ver["stage"]) == (20, 0, 0, 4), "link_table",
                "The link table did not decode and has no verified class-cache adapter.", location)
        records = _read_lv20_links(vi, resource, block, section_number, table_id)
    else:
        require(section.ident == table_id, "link_table",
                "Unexpected link table identity.", location)

    result = []
    for index, record in enumerate(records):
        path = getattr(record, "linkSavePathRef", None)
        result.append(Dependency(
            location, index, record.ident, tuple(getattr(record, "linkSaveQualName", ())),
            getattr(path, "ident", b""), getattr(path, "tpval", -1),
            tuple(getattr(path, "content", ())), tuple(getattr(record, "offsetList", ())),
            getattr(record, "class_cache_tail", b""),
        ))
    return tuple(result)


def resolve_local_path(dependency: Dependency, caller: Path, project_root: Path, encoding: str) -> Path | None:
    """Resolve the verified PTH0 local form under project_root, with no basename fallback."""
    components = dependency.path_components
    if dependency.path_encoding != b"PTH0" or dependency.path_type != 1 or not components or components[0] != b"":
        return None
    location = f"{dependency.resource}/record:{dependency.record}"
    base = caller.parent
    remaining = iter(components[1:])
    for segment in remaining:
        if segment:
            break
        base = base.parent
    else:
        raise ProfileRefusal("dependency_path", "The relative dependency has no file component.", location)
    try:
        parts = [segment.decode(encoding), *(part.decode(encoding) for part in remaining)]
    except (UnicodeError, LookupError) as error:
        raise ProfileRefusal("dependency_path", "Cannot decode the saved dependency path.", location) from error
    for part in parts:
        require(valid_windows_component(part), "dependency_path",
                "Invalid component in the saved relative dependency path.", location)
    candidate = base.joinpath(*parts).resolve()
    require(candidate.is_relative_to(project_root.resolve()), "dependency_path",
            "The saved path points outside the selected project.", location)
    return candidate


def read_class_xml(path, payload):
    location = path.name
    require(len(payload) <= 512 * 1024, "dependency_class",
            "The saved class resource exceeds the XML read budget.", location)
    try:
        xml = payload.decode("utf-8-sig")
        require("<!DOCTYPE" not in xml, "dependency_class", "Class XML contains a document type.", location)
        root = ElementTree.fromstring(xml)
    except (UnicodeError, ElementTree.ParseError) as error:
        raise ProfileRefusal("dependency_class", "Could not decode the saved LabVIEW class resource.", location) from error
    require(root.tag == "LVClass", "dependency_class", "Expected a saved LabVIEW class resource.", location)
    return root


def _class_private_controls(path):
    with path.open("rb") as stream:
        root = read_class_xml(path, stream.read(512 * 1024 + 1))
    embedded = root.find("Property[@Name='NI.LVClass.FlattenedPrivateDataCTL'][@Type='Bin']")
    if embedded is None or not embedded.text:
        return frozenset()
    return frozenset(item.get("Name") for item in root.findall("Item[@Type='Class Private Data']")
                     if item.get("Name") and item.get("URL") == item.get("Name"))


def read_project_dependencies(source: Path, project_root: Path, load_vi, *, max_documents: int = 512,
                              on_member_error=None):
    """Walk only local saved VI links and retain each declaration or use as a separate record."""
    root = project_root.resolve()
    start = source.resolve()
    require(start.is_relative_to(root), "dependency_path", "The source VI is outside the selected project.", "RSRC")
    pending = deque([start])
    scheduled = {start}
    result = {}
    private_controls = {}
    while pending:
        caller = pending.popleft()
        try:
            vi = load_vi(caller)
            declarations = [(resource, read_dependencies(vi, resource)) for resource in _LINK_TABLES]
        except Exception as error:
            if caller == start or on_member_error is None or not on_member_error(caller, error):
                raise
            continue
        links = []
        for resource, references in declarations:
            for reference in references:
                try:
                    target = resolve_local_path(reference, caller, root, vi.textEncoding)
                except ProfileRefusal as error:
                    links.append(ResolvedDependency(caller, reference, None, "invalid", str(error)))
                    continue
                if target is None:
                    status = "external" if reference.path_components else "symbolic"
                elif target.is_file():
                    status = "local"
                    if target.suffix.casefold() == ".vi" and target not in scheduled:
                        require(len(scheduled) < max_documents, "dependency_budget",
                                "The selected project exceeds its VI dependency budget.", resource.decode("ascii"))
                        pending.append(target)
                        scheduled.add(target)
                else:
                    status = "missing"
                    owner = target.parent
                    if (reference.kind in {b"VICC", b"TDCC"} and target.suffix.casefold() == ".ctl" and
                            owner.suffix.casefold() == ".lvclass" and owner.is_file()):
                        if owner not in private_controls:
                            try:
                                private_controls[owner] = _class_private_controls(owner)
                            except (OSError, ProfileRefusal) as error:
                                private_controls[owner] = error
                        controls = private_controls[owner]
                        if isinstance(controls, Exception):
                            links.append(ResolvedDependency(caller, reference, owner, "invalid", str(controls)))
                            continue
                        if target.name in controls:
                            target, status = owner, "embedded"
                links.append(ResolvedDependency(caller, reference, target, status))
        result[caller] = tuple(links)
    return result


class InterfaceTypes(Types):
    def __init__(self, vi):
        super().__init__(vi)
        self.class_ids = getattr(vi, "local_class_ids", {})
        self.encoding = vi.textEncoding

    def _child(self, client):
        return client.nested if client.index == -1 else self.consolidated.getFlatType(client.index)

    def map_elements(self, td, location):
        """Read the two indexed VCTP clients of the LV20 Map descriptor."""
        stream = ExactRead(td.raw_data)
        try:
            code, flags, size = LVdatatype.TDObject.parseRSRCDataHeader(stream)
            require(code == 0x74 and size == len(td.raw_data) and
                    LVmisc.readVariableSizeFieldU2p2(stream) == 2, "map_type",
                    "Invalid LabVIEW Map type header or client count.", location)
            indices = [LVmisc.readVariableSizeFieldU2p2(stream) for _ in range(2)]
            if flags & LVdatatype.TYPEDESC_FLAGS.HasLabel.value:
                length = stream.read(1)[0]
                require(stream.read(length) == td.label, "map_type", "Map label disagrees with its descriptor.", location)
            require(stream.read() in (b"", b"\x00"), "map_type", "Unrecognized Map descriptor suffix.", location)
            return tuple(self.consolidated.getFlatType(index) for index in indices)
        except (EOFError, IndexError) as exc:
            raise ProfileRefusal("map_type", "Truncated Map or invalid key/value type index.", location) from exc

    def heap(self, descriptor, name, location):
        index = number(descriptor, name, location)
        require(1 <= index <= self.slice.tdCount, "type_reference", "Heap TypeID is out of range.", location)
        return self.consolidated.getTopType(self.slice.indexShift + index - 1)

    def describe(self, td, location, depth=0, budget=None):
        if budget is None:
            budget = [1024]
        require(td is not None and depth < 32 and budget[0] > 0, "connector_type",
                "Missing, excessively nested or recursive connector type.", location)
        budget[0] -= 1
        type_code = td.fullType()
        family = type_code.name if hasattr(type_code, "name") else f"Type0x{type_code:02x}"
        label = getattr(td, "label", None)
        definition = ()
        detail = ()
        supported = True
        class_id = None
        children = tuple(self.describe(self._child(client), location, depth + 1, budget)
                         for client in getattr(td, "clients", ()))
        if type_code == 0x74:
            family = "Map"
            children = tuple(self.describe(child, location, depth + 1, budget)
                             for child in self.map_elements(td, location))
        elif family == "TypeDef":
            definition = tuple(td.labels)
            detail = (td.flag1,)
            supported = bool(definition) and len(children) == 1
        elif family in ("Array", "SubArray"):
            detail = tuple((dimension.flags, dimension.fixedSize) for dimension in td.dimensions)
            # SubArray describes a runtime view. It has no serialized panel value.
            supported = family == "Array" and bool(detail) and len(children) == 1
        elif family == "Cluster":
            supported = True
        elif family.startswith(("Num", "Unit")):
            detail = tuple((value.label, value.intval1, value.intval2) for value in td.values)
        elif family == "Refnum":
            detail = (int(td.reftype), tuple(getattr(item, "text", b"") for item in getattr(td, "items", ())),
                      tuple((name, getattr(td, name)) for name in
                            ("field0", "field2", "multiItem", "ctlflags", "hasitem", "itmident") if hasattr(td, name)))
            if detail[0] == 23:
                clients = tuple((c.cfield0, c.cfield2, c.cfield4) for c in td.clients)
                detail = (*detail[:2], (*detail[2], ("event_clients", clients)))
            supported = ConnectorType(family, label, definition, detail, children, False).reference_class() is not None
            if (detail[0] == 30 and not children and len(detail[1]) == 1 and
                    detail[2] == (("field0", 0), ("field2", 0), ("multiItem", 1))):
                class_id = self.class_ids.get(detail[1][0].decode(self.encoding).casefold())
                supported = class_id is not None
        elif family in ("LVVariant", "Path"):
            supported = not children
        elif family == "MeasureData":
            detail = (td.flavor,)
            supported = not children and td.flavor == LVdatatype.MEASURE_DATA_FLAVOR.TimeStamp.value
        elif family not in _SCALARS:
            supported = False
        return ConnectorType(family, label, definition, detail, children,
                             supported and all(child.complete for child in children), class_id)

    def function(self, td, location):
        require(td is not None and getattr(td.fullType(), "name", None) == "Function", "connector_type",
                "The saved connector TypeID does not refer to a function descriptor.", location)
        return td.pattern, tuple((self.describe(self._child(client), location), client.flags)
                                 for client in td.clients)


def _heap_objects(vi, resource, class_name):
    root = heap_root(vi, resource)
    section = one_section(vi, resource)
    nodes = {}
    for item in section.objects:
        if item.getScopeInfo() == LVheap.NODE_SCOPE.TagClose or kind(item) != class_name:
            continue
        # Heap references share the UID but do not carry the object's body.
        if not item.childs:
            continue
        uid = identity(item, resource.decode("ascii"))
        require(uid not in nodes, "heap_uid", f"Multiple definitions for heap UID {uid}.", resource.decode("ascii"))
        nodes[uid] = item
    return root, nodes


def _optional_number(node, name, location):
    return number(node, name, location) if field(node, name, location, optional=True) is not None else 0


def _function_resource(vi, types, resource):
    block = vi.blocks.get(resource)
    location = resource.decode("ascii")
    require(block is not None and len(block.sections) == 1 and block.size == 2, "connector_type",
            "Expected a connector resource with one two-byte TypeID.", location)
    section_number = next(iter(block.sections))
    payload = block.getRawData(section_number)
    require(len(payload) == 2, "connector_type", "Invalid connector TypeID size.", location)
    td = types.consolidated.getTopType(int.from_bytes(payload, "big"))
    return types.function(td, location)


def read_connector(vi) -> Connector:
    """Map source front panel widget UIDs onto the verified connector slots."""
    location = "FPHb/conPane"
    root, widgets = _heap_objects(vi, b"FPHb", "fPDCO")
    pane = field(field(root, "root", location), "conPane", location)
    require(kind(pane) == "conPane", "connector_pattern", "The front panel connector did not decode.", location)
    identities = field(pane, "cons", location)
    extent = identities.attribs.get(ATTR_COUNT)
    pattern_source_id = number(pane, "conId", location)
    types = InterfaceTypes(vi)
    pattern, control_types = _function_resource(vi, types, b"CONP")
    require(pattern is not None and len(control_types) == extent, "connector_pattern",
            "The connector pattern and saved panel connector size differ.", location)
    alternate = _function_resource(vi, types, b"CPC2") if b"CPC2" in vi.blocks else (pattern, control_types)
    call_pattern, call_types = alternate
    require(call_pattern == pattern and len(call_types) == extent, "connector_pattern",
            "The connector resources disagree on pattern or size.", "CPC2")
    result = {}
    for connection in children(identities, "SL__arrayElement"):
        require(kind(connection) == "ConpaneConnection", "connector_binding",
                "Unexpected object in the connector binding list.", location)
        uid = identity(field(connection, "ConnectionDCO", location), location)
        widget_location = f"FPHb/uid:{uid}"
        require(uid in widgets, "connector_binding", "The connector names an undefined panel widget.", widget_location)
        widget = widgets[uid]
        position = _optional_number(widget, "conNum", widget_location)
        require(0 <= position < extent and position not in result, "connector_binding",
                "The widget's connector slot is missing, duplicate or out of range.", widget_location)
        datatype, flags = control_types[position]
        call_datatype, call_flags = call_types[position]
        widget_datatype = types.describe(types.heap(widget, "typeDesc", widget_location), widget_location)
        output = bool(_optional_number(widget, "objFlags", widget_location) & 1)
        require(datatype.family != "Void" and datatype.representation() == call_datatype.representation() and
                datatype.representation() == widget_datatype.representation() and
                output == bool(flags & _OUTPUT) == bool(call_flags & _OUTPUT),
                "connector_binding", f"Connector slot {position} disagrees with its panel widget type or role.", widget_location)
        result[position] = ConnectorBinding(position, uid, output, datatype, flags, call_flags)
    require(result.keys() == {i for i, (datatype, _) in enumerate(control_types) if datatype.family != "Void"} ==
            {i for i, (datatype, _) in enumerate(call_types) if datatype.family != "Void"},
            "connector_binding", "At least one typed connector slot has no matching panel widget binding.", location)
    return Connector(pattern, pattern_source_id, extent, tuple(result[key] for key in sorted(result)))


def _dependencies_by_uid(records, record_kind):
    by_uid = {}
    for record in records:
        if record.kind != record_kind:
            continue
        for uid in record.diagram_offsets:
            require(uid > 0 and uid not in by_uid, "call_link",
                    "An occurrence has multiple incompatible links of the same kind.", f"LIbd/uid:{uid}")
            by_uid[uid] = record
    return by_uid


def read_call_links(vi, dependency_records=None, *, polymorphic=False, occurrence_errors=None) -> dict[int, CallLink]:
    """Reconcile ordinary iUse occurrences against their actual LIbd and pane slots."""
    node_class = "polyIUse" if polymorphic else "iUse"
    _, nodes = _heap_objects(vi, b"BDHb", node_class)
    records = read_dependencies(vi, b"LIbd") if dependency_records is None else dependency_records
    uses = _dependencies_by_uid(records, b"IUVI")
    variants = _dependencies_by_uid(records, b"PUPV")
    types = InterfaceTypes(vi)
    def decode(uid, node):
        location = f"BDHb/uid:{uid}"
        require(not polymorphic or uid in variants, "call_specialization",
                "A polymorphic call needs both the definition and its resolved concrete instance.", location)
        require(uid in uses, "call_link", "The saved iUse occurrence has no IUVI link.", location)
        descriptor = types.heap(node, "connectorTM", location)
        if polymorphic:
            require(descriptor.fullType().name == "Cluster" and len(descriptor.clients) == 2,
                    "call_specialization", "Expected the definition and selected connector pair.", location)
            definition_td, descriptor = (types._child(client) for client in descriptor.clients)
            require(definition_td.fullType().name == "PolyVI", "call_specialization",
                    "The selected connector has no polymorphic definition.", location)
        pattern, expected = types.function(descriptor, location)
        terminals = {}
        seen = set()
        for terminal in items(field(node, "termList", location), location):
            term_uid = identity(terminal, location)
            term_location = f"BDHb/uid:{term_uid}"
            dco = field(terminal, "dco", term_location)
            require(kind(terminal) == "term" and kind(dco) == node_class + "DCO", "call_binding",
                    "Unexpected saved terminal or call binding class.", term_location)
            position = _optional_number(dco, "paramIdx", term_location)
            require(0 <= position < len(expected) and position not in seen, "call_binding",
                    "The saved terminal's connector slot is duplicate or out of range.", term_location)
            seen.add(position)
            datatype, flags = expected[position]
            dco_type = types.describe(types.heap(dco, "typeDesc", term_location), term_location)
            require(datatype.representation() == dco_type.representation(), "call_binding",
                    f"Connector slot {position} disagrees with its diagram terminal type.", term_location)
            if datatype.family == "Void":
                continue
            output = bool(flags & _OUTPUT)
            # The terminal's connection flags can omit 0x8000 for an unwired
            # input. Its DCO records the role whether or not a wire is present.
            require(output == bool(_optional_number(dco, "objFlags", term_location) & 1) and
                    not (output and _optional_number(terminal, "objFlags", term_location) & 0x8000),
                    "call_binding", f"Connector slot {position} disagrees with its diagram terminal direction.", term_location)
            terminals[position] = CallTerminal(position, term_uid, identity(dco, term_location), output, datatype, flags)
        require(len(seen) == len(expected), "call_binding", "The saved call omits connector slots.", location)
        definition = variants.get(uid, uses[uid])
        specialization = uses[uid] if uid in variants else None
        return CallLink(uid, pattern, len(expected), definition, specialization,
                               tuple(terminals[position] for position in sorted(terminals)))

    result = {}
    for uid, node in nodes.items():
        try:
            result[uid] = decode(uid, node)
        except ProfileRefusal as error:
            if occurrence_errors is None:
                raise
            occurrence_errors[uid] = error
    return result


def match_call(call: CallLink, connector: Connector, *, selected_variant=False):
    """Validate a resolved ordinary call and return terminal-to-widget pairs for projection."""
    location = f"BDHb/uid:{call.uid}"
    require(call.specialization is None or selected_variant, "call_specialization",
            "The saved call requires a polymorphic definition and selected variant.", location)
    require((call.pattern, call.slots) == (connector.pattern, connector.slots) and
            tuple(term.slot for term in call.terminals) == tuple(binding.slot for binding in connector.bindings),
            "call_binding", "The caller and callee disagree on connector pattern or occupied slots.", location)
    pairs = tuple(zip(call.terminals, connector.bindings))
    for terminal, binding in pairs:
        require(terminal.output == binding.output and terminal.datatype.matches_connector_type(binding.datatype), "call_binding",
                f"Caller and callee disagree on the type or direction of slot {terminal.slot}.", location)
        require(terminal.datatype.resolved_at_connector(binding.datatype), "call_type",
                f"Full type equivalence of caller and callee slot {terminal.slot} is unverified.", location)
    return pairs
