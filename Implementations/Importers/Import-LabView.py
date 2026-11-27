#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
"""Inspect a LabVIEW RSRC file and convert its qualified editable subset to FROG."""

import argparse
import contextlib
import ctypes
import hashlib
import io
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import time
from types import SimpleNamespace
from urllib.parse import quote
import warnings
import zlib


MAX_SOURCE_BYTES = 16 * 1024 * 1024
MAX_RESOURCE_BYTES = 512 * 1024
MAX_RESOURCES = 256
MAX_SECTIONS = 2048
PARSER_REVISION = "5f20e23de6a386021eb955c825cc43d221f09ff1"
FILE_TYPES = {".vi": "VI", ".ctl": "Control", ".llb": "LLB"}
HEAPS = {b"BDHP", b"BDHb", b"BDHc", b"FPHP", b"FPHb", b"FPHc"}
SEMANTIC_RESOURCES = {b"LVSR", b"vers", b"BDHb", b"FPHb", b"VCTP", b"DTHP", b"TM80", b"DSTM", b"DFDS",
                      b"LIvi", b"LIbd"}


class Refusal(Exception):
    def __init__(self, code, message, resource="/"):
        super().__init__(message)
        self.diagnostic = {"code": "labview_import." + code, "resource": resource,
                           "source": resource, "message": message}


class StrictPayload(io.BytesIO):
    """Prevent implicit zero padding when an upstream field read reaches EOF."""

    def read(self, size=-1):
        value = super().read(size)
        if size >= 0 and len(value) != size:
            raise EOFError("Resource ended inside a field.")
        return value


class ReaderMessages(io.TextIOBase):
    def __init__(self):
        self.messages = ""

    def write(self, text):
        self.messages += text[:max(0, 4096 - len(self.messages))]
        return len(text)


def read_snapshot(source):
    source = publication_path(source)
    if not source.is_file():
        raise Refusal("source_unreadable", "Choose a regular LabVIEW file.")
    with source.open("rb") as stream:
        data = stream.read(MAX_SOURCE_BYTES + 1)
    if len(data) > MAX_SOURCE_BYTES:
        raise Refusal("source_budget", "The maximum LabVIEW source size is 16 MiB.")
    return data


def create_parser_stream(data, header_type):
    class Snapshot(io.BytesIO):
        header_reads = 0

        def readinto(self, buffer):
            if isinstance(buffer, header_type):
                self.header_reads += 1
                if self.header_reads > 4:
                    raise Refusal("header_budget", "Too many linked RSRC headers.")
            return super().readinto(buffer)

    return Snapshot(data)


def initialise_container(data, source, container, blocks):
    options = SimpleNamespace(verbose=0, print_map=None, rsrc=str(source), xml="", raw_connectors=False,
                              typedesc_list_limit=512, array_data_limit=16384, store_as_data_above=4095)
    # Classic VI display strings use the author's system character set. Studio
    # opens untagged Windows documents in the current ANSI code page, as NI does.
    encoding = f"cp{ctypes.windll.kernel32.GetACP()}" if os.name == "nt" else "mac_roman"
    vi = container.VI(options, text_encoding=encoding)
    vi.dataSource = "rsrc"
    vi.src_fname = str(source)
    vi.rsrc_fh = create_parser_stream(data, container.RSRCHeader)
    vi.readRSRCList(vi.rsrc_fh)
    header = vi.rsrc_headers[-1]
    info_start = header.rsrc_info_offset
    info_end = info_start + header.rsrc_info_size
    data_start = header.rsrc_data_offset
    data_end = data_start + header.rsrc_data_size
    info_minimum = (ctypes.sizeof(container.RSRCHeader) + ctypes.sizeof(container.BlockInfoListHeader)
                    + ctypes.sizeof(container.BlockInfoHeader))
    if not (32 <= data_start <= data_end <= info_start < info_end <= len(data)
            and header.rsrc_info_size >= info_minimum):
        raise Refusal("invalid_directory", "The RSRC information or data range exceeds the file.")
    if any(item.ftype != header.ftype or item.rsrc_fmtver != header.rsrc_fmtver for item in vi.rsrc_headers):
        raise Refusal("invalid_directory", "The linked RSRC headers disagree on the file type or format.")
    headers = vi.readRSRCBlockInfo(vi.rsrc_fh)
    if len(headers) > MAX_RESOURCES or sum(int(item.count) + 1 for item in headers) > MAX_SECTIONS:
        raise Refusal("directory_budget", "The RSRC resource or section count exceeds the import limit.")
    entries_start = vi.getPositionOfBlockInfoHeader()
    sections_start = entries_start + ctypes.sizeof(container.BlockInfoHeader) + len(headers) * ctypes.sizeof(blocks.BlockHeader)
    if sections_start > info_end:
        raise Refusal("invalid_directory", "The RSRC resource table exceeds the information range.")
    return vi, headers, entries_start, sections_start, info_end, data_start, data_end


def bounded_reader(base, blocks):
    compressed = (blocks.BLOCK_CODING.ZLIB, blocks.BLOCK_CODING.COMP)

    def decoded_size(raw, coding):
        if coding not in compressed:
            return len(raw)
        if len(raw) < 4:
            raise EOFError("Compressed resource has no length header.")
        return int.from_bytes(raw[:4], "big")

    class Bounded(base):
        def declared_decoded_size(self, section_num):
            section = self.getSection(section_num)
            return decoded_size(section.raw_data, section.block_coding)

        def getData(self, section_num=None, use_coding=None):
            section = self.getSection(section_num)
            raw = section.raw_data
            coding = use_coding if use_coding is not None else getattr(section, "block_coding", blocks.BLOCK_CODING.NONE)
            size = decoded_size(raw, coding)
            if coding in compressed and size > MAX_RESOURCE_BYTES:
                raise ValueError("Decoded resource exceeds 512 KiB.")
            if coding == blocks.BLOCK_CODING.ZLIB:
                decoder = zlib.decompressobj()
                decoded = decoder.decompress(raw[4:], size + 1)
                if len(decoded) != size or not decoder.eof or decoder.unused_data:
                    raise ValueError("Decoded resource does not match its declared size.")
                section.last_plain_data_size = size
                return StrictPayload(decoded)
            if len(raw) > MAX_RESOURCE_BYTES:
                raise ValueError("Resource exceeds 512 KiB.")
            return StrictPayload(super().getData(section_num, coding).read())

    return Bounded


def index_resources(data, source, container, blocks):
    vi, headers, entries_start, sections_start, info_end, data_start, data_end = initialise_container(
        data, source, container, blocks)
    found_type = vi.ftype.name
    if FILE_TYPES.get(source.suffix.lower()) != found_type:
        raise Refusal("file_kind", f"The LabVIEW container is {found_type} and does not match the selected file extension.")
    vi.blocks = {}
    section_size = ctypes.sizeof(blocks.BlockSectionStart)
    data_header_size = ctypes.sizeof(blocks.BlockSectionData)
    inventory = {}
    source_bytes = memoryview(data)
    for header in headers:
        ident = bytes(header.ident)
        name = container.getPrettyStrFromRsrcType(ident)
        start = entries_start + header.offset
        if ident in vi.blocks or not (sections_start <= start <= info_end - (header.count + 1) * section_size):
            raise Refusal("invalid_directory", f"Invalid or duplicate resource directory entry for {name}.", name)
        reader = bounded_reader(getattr(blocks, name), blocks) if ident in SEMANTIC_RESOURCES else blocks.Block
        block = reader(vi, vi.po)
        block.initWithRSRCEarly(header)
        vi.blocks[ident] = block
        byte_count = 0
        for section in block.sections.values():
            position = section.block_pos
            if not data_start <= position <= data_end - data_header_size:
                raise Refusal("invalid_section", f"A {name} section points outside RSRC data.", name)
            size = blocks.BlockSectionData.from_buffer_copy(data, position).size
            payload_start = position + data_header_size
            if size > data_end - payload_start:
                raise Refusal("invalid_section", f"A {name} section extends beyond RSRC data.", name)
            if ident in SEMANTIC_RESOURCES:
                section.raw_data = source_bytes[payload_start:payload_start + size]
                section.raw_data_updated = True
            byte_count += size
        block.size = byte_count
        state = "opaque_heap" if ident in HEAPS and ident not in SEMANTIC_RESOURCES else "indexed"
        inventory[ident] = {"id": name, "sections": len(block.sections), "stored_bytes": byte_count, "state": state}
    return vi, inventory


def validate_heap(block, heap):
    scope = heap.NODE_SCOPE
    roots = 0
    pending = []
    for item in block.objects:
        kind = item.getScopeInfo()
        tag = item.tagEn.value
        if kind == scope.TagClose:
            if not pending or pending.pop() != tag:
                raise ValueError("Heap contains mismatched closing tags.")
            continue
        roots += not pending
        if kind == scope.TagOpen:
            if len(pending) >= 96:
                raise ValueError("Heap nesting exceeds the import limit.")
            pending.append(tag)
        elif kind != scope.TagLeaf:
            raise ValueError("Heap contains an unidentified tag scope.")
    if roots != 1 or pending:
        raise ValueError("Heap must contain one complete root object.")


def decode_resources(vi, inventory, heap, report):
    from import_labview_interfaces import lv20_default_reader
    for ident in (b"LVSR", b"vers", b"VCTP", b"DTHP", b"TM80", b"DSTM", b"DFDS", b"BDHb", b"FPHb",
                  b"LIvi", b"LIbd"):
        block = vi.blocks.get(ident)
        if block is None or (ident == b"vers" and report.get("labview_version")):
            continue
        entry = inventory[ident]
        warnings = ReaderMessages()
        try:
            max_sections = {b"vers": 32, b"TM80": MAX_SECTIONS}.get(ident, 1)
            if len(block.sections) > max_sections:
                raise ValueError("The resource contains more sections than this decoder profile admits.")
            if ident == b"TM80" and len(block.sections) > 1 and 0 not in block.sections:
                raise ValueError("A sectioned type map requires its primary section (0).")
            if ident == b"DFDS" and b"TM80" in inventory and inventory[b"TM80"]["state"] != "decoded_metadata":
                raise ValueError("Stored defaults require a decoded primary type map.")
            with contextlib.redirect_stdout(warnings), contextlib.redirect_stderr(warnings):
                for number in block.sections:
                    if hasattr(block, "setDefaultEncoding"):
                        block.setDefaultEncoding(number)
                if ident == b"TM80":
                    decoded_bytes = sum(block.declared_decoded_size(number) for number in block.sections)
                    if decoded_bytes > MAX_RESOURCE_BYTES:
                        raise ValueError("Decoded type-map sections together exceed 512 KiB.")
                for number in block.sections:
                    with lv20_default_reader(vi) if ident == b"DFDS" else contextlib.nullcontext():
                        section = block.getParsedSection(number)
                    if getattr(section, "parse_failed", False):
                        if ident not in (b"LIvi", b"LIbd"):
                            raise ValueError("The upstream reader retained a binary section.")
                        from import_labview_interfaces import read_dependencies
                        read_dependencies(vi, ident)
                    if ident in HEAPS:
                        validate_heap(section, heap)
                # DFDS consumes the active map, while the other TM80 section
                # can map auxiliary values with no defaults in this resource.
                block.setActiveSectionNum(block.defaultSectionNumber())
                if ident in (b"LVSR", b"vers"):
                    version = block.getVersion()
                    if version.get("major"):
                        report["labview_version"] = version
            entry["state"] = "decoded_heap" if ident in HEAPS else "decoded_metadata"
            if ident in HEAPS:
                entry["heap_records"] = sum(len(section.objects) for section in block.sections.values())
            if ident == b"TM80":
                entry["primary_section"] = block.defaultSectionNumber()
                entry["decoded_bytes"] = decoded_bytes
                entry["type_map_sections"] = [
                    {"section": number, "first_type_id": item.indexShift, "entries": len(item.content)}
                    for number, item in sorted(block.sections.items())
                ]
            elif ident == b"DFDS":
                entry["default_records"] = len(block.getParsedSection(None).content)
        except (ArithmeticError, AttributeError, EOFError, LookupError, OSError, RuntimeError, TypeError, ValueError, zlib.error) as error:
            entry["state"] = "decode_failed"
            report["diagnostics"].append(Refusal("resource_decode", str(error), entry["id"]).diagnostic)
        if warnings.messages:
            entry["reader_messages"] = warnings.messages.strip()


def declared_links(vi, inventory):
    """List the stored link identities and call sites without loading linked code."""
    result = []
    for table in (b"LIvi", b"LIbd"):
        if inventory.get(table, {}).get("state") != "decoded_metadata":
            continue
        from import_labview_interfaces import read_dependencies
        for link in read_dependencies(vi, table):
            if link.kind not in (b"VIVI", b"VICC", b"IUVI"):
                continue
            entry = {"resource": link.resource, "section": next(iter(vi.blocks[table].sections)),
                     "kind": link.kind.decode("ascii"),
                     "qualified_name": [part.decode(vi.textEncoding) for part in link.qualified_name],
                     "path_components": [part.decode(vi.textEncoding) for part in link.path_components],
                     "path_encoding": link.path_encoding.decode("ascii"), "path_type": link.path_type}
            if link.kind == b"IUVI":
                entry["diagram_offsets"] = list(link.diagram_offsets)
            result.append(entry)
    return result


def inspect(data, source, report, *, allow_unread_defaults=False):
    with warnings.catch_warnings():
        warnings.filterwarnings("ignore", message="invalid escape sequence", category=SyntaxWarning)
        from pylabview import LVblock, LVheap, LVrsrcontainer

    try:
        vi, inventory = index_resources(data, source, LVrsrcontainer, LVblock)
    except (EOFError, OSError) as error:
        raise Refusal("invalid_rsrc", str(error)) from error
    report.update(container_type=vi.ftype.name, rsrc_format=vi.fmtver, resources=list(inventory.values()))
    decode_resources(vi, inventory, LVheap, report)
    report["declared_links"] = declared_links(vi, inventory)
    if vi.ftype.name != "VI":
        raise Refusal("inspection_only", "Control and library containers can be inspected. Conversion requires a VI with an editable diagram.")
    if not any(ident in inventory for ident in (b"BDHP", b"BDHb", b"BDHc")):
        raise Refusal("diagram_missing", "The file contains no source Block Diagram heap. Compiled resources are outside the import profile.", "diagram")
    for name in ("BDHb", "FPHb"):
        resource = inventory.get(name.encode())
        if not resource or resource["state"] != "decoded_heap" or resource["sections"] != 1:
            raise Refusal("heap_unsupported", f"The supported import profile requires one decoded {name} heap.", name)
    for name in ("DTHP", "VCTP", "DFDS"):
        resource = inventory.get(name.encode())
        if resource and resource["state"] != "decoded_metadata":
            if name == "DFDS" and allow_unread_defaults:
                vi.unread_panel_defaults = True
                continue
            raise Refusal("type_or_value_unsupported", f"Cannot verify the original types or values without decoded {name}.", name)
    return vi


def record_editing_diagnostics(report, mapping):
    # The executable translator can stop early. The editing pass still read
    # source objects and identified additional operations and unresolved wires.
    diagnostics = report["diagnostics"]
    by_source = {}
    for issue in diagnostics:
        by_source.setdefault(issue["source"], []).append(issue)
    for note in mapping.get("notes", ()):
        code = note.get("code")
        if not code or code == "unwired_boundary_role":
            continue
        source = note.get("resource") or (f"BDHb/uid:{note['node_uid']}" if "node_uid" in note else "diagram")
        message = note.get("message") or note.get("reason", "")
        name = note.get("object") or note.get("name") or note.get("target")
        existing = by_source.get(source, ())
        if name:
            for issue in existing:
                issue.setdefault("object", name)
        if any(issue["message"] == message for issue in existing):
            continue
        detail = Refusal(code, message, source).diagnostic
        if name:
            detail["object"] = name
        diagnostics.append(detail)
        by_source.setdefault(source, []).append(detail)

    from import_labview_structured import OPERATIONS
    source_names = {"aBuild": "Build Array", "forLoop": "For Loop", "whileLoop": "While Loop",
                    "select": "Case Structure", "propNode": "Property Node", "invokeNode": "Invoke Node",
                    "nMux": "Bundle / Unbundle by Name"}
    for uid, node in mapping.get("source_nodes", {}).items():
        source_class = node["class"]
        name = source_names.get(source_class)
        if source_class == "prim":
            fields = node.get("fields", {})
            pair = tuple(fields.get(key, {}).get("value") for key in ("primResID", "primIndex"))
            operation = OPERATIONS.get(pair)
            if operation:
                name = operation[1].rsplit(".", 1)[-1].replace("_", " ").title()
        for issue in by_source.get(f"BDHb/uid:{uid}", ()):
            if name and issue.get("object") in (None, source_class):
                issue["object"] = name


def format_report(report):
    lines = [f"LabVIEW source\n{report['source']}"]
    if "project_members" in report:
        lines += ["LabVIEW XML project · My Computer", f"SHA-256: {report['source_sha256']}"]
        for item in report["project_members"]:
            lines.append(f"  {item['source']} -> {item['output']}")
        lines.extend(d["message"] for d in report["diagnostics"])
        if report.get("frontend_validated"):
            lines.append("The FROG Frontend validated every program with its project dependencies.")
        return "\n".join(lines)
    if report.get("source_sha256"):
        lines.append(f"SHA-256: {report['source_sha256']}")
    if "container_type" in report:
        lines.append(f"{report['container_type']} container (RSRC format {report['rsrc_format']})")
    version = report.get("labview_version")
    if version:
        lines.append(f"LabVIEW {version['major']}.{version['minor']}.{version['bugfix']}")
    resources = report.get("resources", [])
    counts = report["resource_counts"] = {state: sum(item["state"] == state for item in resources)
                                          for state in ("indexed", "opaque_heap", "decode_failed", "decoded_heap", "decoded_metadata")}
    if resources:
        lines.extend((f"{len(resources)} resources, {counts['decoded_heap']} decoded heaps, "
                      f"{counts['opaque_heap']} opaque heaps and {counts['decode_failed']} decode failures.",
                      "", "Resources and sections"))
        for entry in resources:
            records = f", {entry['heap_records']} heap records" if "heap_records" in entry else ""
            if "default_records" in entry:
                records += f", {entry['default_records']} stored defaults"
            if "type_map_sections" in entry:
                records += f", primary section {entry['primary_section']}"
            lines.append(f"  {entry['id']} — {entry['state']}, {entry['sections']} sections, {entry['stored_bytes']} bytes{records}")
            if entry["state"] == "decode_failed" and entry.get("reader_messages"):
                lines.append("    " + entry["reader_messages"])
    links = report.get("declared_links", ())
    if links:
        lines.extend(("", "Declared VI and type definition links"))
        for link in links:
            name = ":".join(link["qualified_name"])
            parts = link["path_components"]
            if link["path_encoding"] == "PTH0" and link["path_type"] == 1 and parts[:1] == [""]:
                components = "./" + "/".join(part or ".." for part in parts[1:])
            else:
                components = " / ".join(part or "<empty>" for part in parts)
            sites = link.get("diagram_offsets")
            suffix = " (diagram references: " + ", ".join(map(str, sites)) + ")" if sites else ""
            lines.append(f"  {link['resource']} {link['kind']} — {name} [{components}]{suffix}")
    if report.get("editable") and not report["compatible"]:
        lines.extend(("", "Editing only — the source diagram can be opened and edited. Run and Build require translation of the remaining LabVIEW operations.",
                      f"{report['source_regions']} diagrams, {report['source_signals']} source signals in the selected VI."))
        if report.get("editable_documents"):
            lines.append(f"Created {report['editable_documents']} editable FROG documents. See labview-mapping.json for source and dependency identities.")
            for member, problem in report.get("unreadable_members", {}).items():
                lines.append(f"  Dependency requiring review: {member} — {problem}")
        lines.extend(("", "Checks from the executable import profile"))
    else:
        lines.extend(("", "Eligible for editable FROG import." if report["compatible"] else "Editable FROG import unavailable for this file."))
    if report["compatible"]:
        projection = report["projection"]
        lines.append(f"The editable projection contains {projection['nodes']} diagram nodes and {projection['wires']} wires.")
        if any(detail["kind"] == "loaded_shift_register" for detail in report.get("adaptations", ())):
            lines.append("The imported register retains its state across completed executions of the loaded document.")
        if report.get("frontend_validated"):
            lines.append("The common FROG Frontend validated the editable projection.")
    if presentation := report.get("presentation"):
        icon = presentation["icon"].get("resource")
        if icon:
            lines.append(f"Saved VI icon: {icon}.")
        else:
            lines.append("The saved VI icon could not be recovered. The imported document uses the fallback icon.")
        connector = presentation["connector"]
        if connector["status"] == "verified":
            lines.append(f"Saved connector: {len(connector['bindings'])} occupied slots out of {connector['slots']}.")
            if connector.get("geometry"):
                lines.append("The original slot identities use the native FROG layout, with different terminal positions.")
            if any(binding["direction"] == "input" for binding in connector["bindings"]):
                policy = connector["input_connection_policy"]
                lines.append(f"FROG input policies: {policy}. The mapping retains the source flags. Unwired inputs are admitted only for matching recommended descriptors and verified widget defaults.")
        else:
            lines.append("Saved connector could not be translated; see the connector diagnostics.")
            for issue in connector["diagnostics"]:
                lines.append(f"  [{issue['code']}] {issue['resource']} — {issue['reason']}")
    for section, entries in report.get("translated_objects", {}).items():
        if entries:
            lines.append(f"Translated {section} objects (LabVIEW UID to FROG binding)")
        for entry in entries:
            details = ", ".join(f"{key}={value}" for key, value in entry.items() if key not in ("uid", "id"))
            lines.append(f"  {section}/uid:{entry['uid']} -> {entry['id']}" + (f" — {details}" if details else ""))
    for diagnostic in report["diagnostics"]:
        lines.append(f"  [{diagnostic['code']}] {diagnostic['resource']} — {diagnostic['message']}")
        if diagnostic.get("object"):
            lines.append("    " + str(diagnostic["object"]))
        for occurrence in diagnostic.get("occurrences", ()):
            source = occurrence["source"]
            region = occurrence.get("diagram_uid")
            details = occurrence.get("property") or ".".join(
                value for value in (occurrence.get("reference_class"), occurrence.get("method")) if value)
            lines.append("    " + source + (f" in diagram {region}" if region is not None else "")
                         + (f" — {details}" if details else ""))
    return "\n".join(lines)


def write_json(path, payload):
    path.write_text(json.dumps(payload, ensure_ascii=False, allow_nan=False, indent=2) + "\n", encoding="utf-8")


def projection_size(diagram):
    nodes = len(diagram["nodes"])
    wires = len(diagram["edges"])
    for node in diagram["nodes"]:
        for region in node.get("regions", ()):
            size = projection_size(region["diagram"])
            nodes += size["nodes"]
            wires += size["wires"]
    return {"nodes": nodes, "wires": wires}


def validate_frontend(frontend, document, stage, filename, dependencies=None):
    model = stage / filename
    diagnostics = stage / "frontend-diagnostics.json"
    write_json(model, document)
    result = subprocess.run([str(frontend.resolve(strict=True)), "validate", "--source", str(model),
                             "--source-uri", "import:/labview/" + quote(filename), "--document-id", "labview:studio:draft",
                             "--fir-document-id", "labview:studio:fir", "--source-format", "frog.document.draft",
                             "--source-version", str(document["draft_revision"]), "--fir-version", "0.1",
                             "--diagnostics-output", str(diagnostics),
                             *(["--dependencies", str(dependencies)] if dependencies else [])],
                            capture_output=True, check=False, timeout=30)
    if result.returncode:
        if diagnostics.is_file():
            with diagnostics.open(encoding="utf-8") as stream:
                detail = stream.read(2048)
        else:
            detail = result.stderr[:2048].decode(errors="replace")
        raise Refusal("frontend_refused", f"The common FROG Frontend rejected the imported document. {detail}")


def retry_windows_sharing(operation):
    deadline = time.monotonic() + 2
    while True:
        try:
            operation()
            return
        except OSError as error:
            if getattr(error, "winerror", None) not in (5, 32, 33) or time.monotonic() >= deadline:
                raise
            time.sleep(0.05)


def publication_path(path):
    """Keep source and staging I/O usable beyond Win32 MAX_PATH."""
    absolute = os.path.abspath(path)
    if os.name == "nt" and not absolute.startswith(("\\\\?\\", "\\\\.\\")):
        absolute = ("\\\\?\\UNC\\" + absolute[2:] if absolute.startswith("\\\\")
                    else "\\\\?\\" + absolute)
    return Path(absolute)


def publish(document, mapping, report, args, documents=None, fingerprints=None):
    from import_labview_profile import vi_document_path

    output = publication_path(args.output)
    if output.exists() or output.is_symlink():
        raise Refusal("destination_exists", "Choose a new import destination.")
    stage = Path(tempfile.mkdtemp(prefix=".labview-import-", dir=output.parent))
    try:
        if document is not None:
            filename = vi_document_path(args.source)
            document["title"] = document["metadata"]["name"] = filename.stem
            report["root_document"] = filename.name
        if documents is None:
            validate_frontend(args.frontend, document, stage, filename.name)
            report["frontend_validated"] = True
        else:
            for relative, member in documents.items():
                target = stage / relative
                if not target.resolve().is_relative_to(stage.resolve()):
                    raise Refusal("destination_path", "A member would leave the chosen import directory.")
                target.parent.mkdir(parents=True, exist_ok=True)
                if isinstance(member, bytes):
                    target.write_bytes(member)
                else:
                    write_json(target, member)
        write_json(stage / "labview-mapping.json", mapping)
        report["report"] = format_report(report)
        write_json(stage / "import-analysis.json", report)

        def commit():
            if output.exists() or output.is_symlink():
                raise Refusal("destination_exists", "Choose a new import destination.")
            if hashlib.sha256(read_snapshot(args.source)).hexdigest() != report["source_sha256"]:
                raise Refusal("source_changed", "The selected file changed during conversion. Analyze it again.")
            for path, expected in (fingerprints or {}).items():
                if hashlib.sha256(read_snapshot(path)).hexdigest() != expected:
                    raise Refusal("source_changed", "A local VI changed during conversion. Analyze it again.", path.name)
            os.rename(stage, output)

        retry_windows_sharing(commit)
    finally:
        retry_windows_sharing(lambda: shutil.rmtree(stage) if stage.exists() else None)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, required=True)
    parser.add_argument("--analyze", action="store_true")
    parser.add_argument("--report", type=Path)
    parser.add_argument("--output", type=Path)
    parser.add_argument("--frontend", type=Path)
    parser.add_argument("--expected-sha256", default="")
    parser.add_argument("--editable-only", action="store_true",
                        help="Preserve the editable source graph and explicitly mark untranslated operations.")
    args = parser.parse_args()
    if (args.analyze and args.output) or (not args.analyze and (not args.output or not args.frontend or args.report)):
        parser.error("Use --analyze [--report PATH], or --output DIR --frontend PATH for conversion.")

    # Python's isolated mode excludes the script directory and current working directory.
    directory = Path(__file__).resolve().parent
    sys.path.insert(0, str(directory))
    vendor = directory / "vendor" / "pylabview"
    if (vendor / "pylabview").is_dir():
        sys.path.insert(0, str(vendor))

    report = {"format": "frog.labview-import.analysis.v1", "source": str(args.source.absolute()),
              "source_sha256": "", "pylabview_revision": PARSER_REVISION,
              "compatible": False, "editable": False, "execution_checks": "not_started", "diagnostics": []}
    try:
        data = read_snapshot(args.source)
        digest = report["source_sha256"] = hashlib.sha256(data).hexdigest()
        if args.expected_sha256 and digest != args.expected_sha256.lower():
            raise Refusal("source_changed", "The selected file differs from the analyzed bytes. Analyze it again.")
        if args.source.suffix.lower() == ".lvproj":
            from import_labview_project import import_project
            import_project(args, data, report, sys.modules[__name__])
            return finish_report(args, report) if args.analyze else 0
        vi = inspect(data, args.source, report)
        from import_labview_profile import ProfileRefusal, project, projection_requirement_diagnostics, vi_document_path

        report["execution_checks"] = "incomplete"
        try:
            document, mapping = project(vi, args.source, digest)
            metadata = document["metadata"]
            metadata["description"] = f"Source (LabVIEW): {args.source.resolve()}\n{metadata['description']}"
        except ProfileRefusal as error:
            primary = Refusal(error.code, str(error), error.resource).diagnostic
            report["diagnostics"].append(primary)
            for diagnostic in projection_requirement_diagnostics(vi):
                if diagnostic["code"] == primary["code"] and any(
                        item["source"] == primary["source"] for item in diagnostic["occurrences"]):
                    primary.update(count=diagnostic["count"], occurrences=diagnostic["occurrences"])
                else:
                    report["diagnostics"].append(diagnostic)
            if args.analyze or args.editable_only:
                from import_labview_editable import project_editable
                hashes = {}
                total_bytes = len(data)

                def member_snapshot(path):
                    nonlocal total_bytes
                    payload = read_snapshot(path)
                    total_bytes += len(payload)
                    if total_bytes > 64 * 1024 * 1024:
                        raise Refusal("dependency_budget", "The local VI project exceeds the import budget.")
                    hashes[path] = hashlib.sha256(payload).hexdigest()
                    return payload

                def load_member(path):
                    return inspect(member_snapshot(path), path, {"diagnostics": []}, allow_unread_defaults=True)

                def load_control(payload, path):
                    from pylabview import LVblock, LVheap, LVrsrcontainer
                    control, inventory = index_resources(payload, path, LVrsrcontainer, LVblock)
                    decode_resources(control, inventory, LVheap, {"diagnostics": []})
                    if (control.ftype.name != "Control" or inventory.get(b"FPHb", {}).get("state") != "decoded_heap" or
                            any(inventory.get(key, {}).get("state") == "decode_failed"
                                for key in (b"DFDS", b"VCTP", b"DTHP", b"TM80", b"DSTM"))):
                        raise Refusal("class_private_data", "Cannot verify the embedded private control.", path.name)
                    return control

                try:
                    documents, mapping = project_editable(args.source, vi, None if args.analyze else load_member,
                                                          (member_snapshot, load_control))
                except ProfileRefusal as projection_error:
                    report["diagnostics"].append(Refusal(projection_error.code, str(projection_error), projection_error.resource).diagnostic)
                else:
                    document = documents[vi_document_path(args.source)]
                    report.update(editable=True, profile=mapping["profile"], projection=projection_size(document["diagram"]),
                                  presentation=mapping["presentation"],
                                  source_regions=len(mapping["diagrams"]), source_signals=len(mapping["signals"]),
                                  frontend_validated=False)
                    record_editing_diagnostics(report, mapping)
                    if not args.analyze:
                        mapping["source_sha256"] = digest
                        mapping["dependency_sha256"] = {p.relative_to(args.source.resolve().parent).as_posix(): h for p, h in hashes.items()}
                        report.update(editable_documents=len(documents), skipped_members=mapping["skipped_members"],
                                      unreadable_members=mapping["unreadable_members"])
                        publish(document, mapping, report, args, documents, hashes)
                        return 0
        else:
            if args.analyze and args.frontend:
                scratch_parent = args.report.parent if args.report else None
                with tempfile.TemporaryDirectory(prefix=".labview-analysis-", dir=scratch_parent) as scratch:
                    validate_frontend(args.frontend, document, Path(scratch), vi_document_path(args.source).name)
                report["frontend_validated"] = True
            report.update(compatible=True, editable=True, execution_checks="complete", projection=projection_size(document["diagram"]), profile=mapping["profile"],
                          adaptations=mapping.get("adaptations", []), presentation=mapping["presentation"])
            report["translated_objects"] = {
                section: [{key: entry[key] for key in ("id", "uid", "source_operation", "primitive_identity",
                                                        "operand_bindings", "class_id", "role", "value_type") if key in entry}
                          for entry in mapping.get(records, ()) if "uid" in entry and "id" in entry]
                for section, records in (("BDHb", "nodes"), ("FPHb", "widgets"))}
            if not args.analyze:
                publish(document, mapping, report, args)
                return 0
    except Refusal as error:
        report["diagnostics"].append(error.diagnostic)
    except (ImportError, OSError, subprocess.TimeoutExpired) as error:
        report["diagnostics"].append(Refusal("reader_or_io", str(error)).diagnostic)
    except (ArithmeticError, AttributeError, LookupError, RuntimeError, TypeError, ValueError, zlib.error) as error:
        report["diagnostics"].append(Refusal("invalid_rsrc", str(error)).diagnostic)

    return finish_report(args, report)


def finish_report(args, report):
    if args.analyze:
        report["report"] = format_report(report)
        if args.report:
            try:
                with args.report.open("x", encoding="utf-8") as stream:
                    json.dump(report, stream, ensure_ascii=False, indent=2)
                    stream.write("\n")
            except OSError as error:
                print(str(error), file=sys.stderr)
                return 2
        else:
            print(json.dumps(report, ensure_ascii=False))
        return 0
    for diagnostic in report["diagnostics"]:
        print(json.dumps(diagnostic, ensure_ascii=False), file=sys.stderr)
    return 2


if __name__ == "__main__":
    raise SystemExit(main())
