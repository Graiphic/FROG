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
import warnings
import zlib


MAX_SOURCE_BYTES = 16 * 1024 * 1024
MAX_RESOURCE_BYTES = 512 * 1024
MAX_RESOURCES = 256
MAX_SECTIONS = 2048
PARSER_REVISION = "5f20e23de6a386021eb955c825cc43d221f09ff1"
FILE_TYPES = {".vi": "VI", ".ctl": "Control", ".llb": "LLB"}
HEAPS = {b"BDHP", b"BDHb", b"BDHc", b"FPHP", b"FPHb", b"FPHc"}
SEMANTIC_RESOURCES = {b"LVSR", b"vers", b"BDHb", b"FPHb", b"VCTP", b"DTHP", b"TM80", b"DSTM", b"DFDS"}


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
    vi = container.VI(options, text_encoding="mac_roman")
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
    class Bounded(base):
        def getData(self, section_num=None, use_coding=None):
            section = self.getSection(section_num)
            raw = section.raw_data
            coding = use_coding if use_coding is not None else getattr(section, "block_coding", blocks.BLOCK_CODING.NONE)
            if coding in (blocks.BLOCK_CODING.ZLIB, blocks.BLOCK_CODING.COMP):
                if len(raw) < 4:
                    raise EOFError("Compressed resource has no length header.")
                size = int.from_bytes(raw[:4], "big")
                if size > MAX_RESOURCE_BYTES:
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
    for ident in (b"LVSR", b"vers", b"VCTP", b"DTHP", b"TM80", b"DSTM", b"DFDS", b"BDHb", b"FPHb"):
        block = vi.blocks.get(ident)
        if block is None or (ident == b"vers" and report.get("labview_version")):
            continue
        entry = inventory[ident]
        warnings = ReaderMessages()
        try:
            if len(block.sections) > (32 if ident == b"vers" else 1):
                raise ValueError("The resource contains more sections than this decoder profile admits.")
            with contextlib.redirect_stdout(warnings), contextlib.redirect_stderr(warnings):
                for number in block.sections:
                    if hasattr(block, "setDefaultEncoding"):
                        block.setDefaultEncoding(number)
                    section = block.getParsedSection(number)
                    if getattr(section, "parse_failed", False):
                        raise ValueError("The upstream reader retained a binary section.")
                    if ident in HEAPS:
                        validate_heap(section, heap)
                block.setActiveSectionNum(block.defaultSectionNumber())
                if ident in (b"LVSR", b"vers"):
                    version = block.getVersion()
                    if version.get("major"):
                        report["labview_version"] = version
            entry["state"] = "decoded_heap" if ident in HEAPS else "decoded_metadata"
            if ident in HEAPS:
                entry["heap_records"] = sum(len(section.objects) for section in block.sections.values())
        except (ArithmeticError, AttributeError, EOFError, LookupError, OSError, RuntimeError, TypeError, ValueError, zlib.error) as error:
            entry["state"] = "decode_failed"
            report["diagnostics"].append(Refusal("resource_decode", str(error), entry["id"]).diagnostic)
        if warnings.messages:
            entry["reader_messages"] = warnings.messages.strip()


def inspect(data, source, report):
    with warnings.catch_warnings():
        warnings.filterwarnings("ignore", message="invalid escape sequence", category=SyntaxWarning)
        from pylabview import LVblock, LVheap, LVrsrcontainer

    try:
        vi, inventory = index_resources(data, source, LVrsrcontainer, LVblock)
    except (EOFError, OSError) as error:
        raise Refusal("invalid_rsrc", str(error)) from error
    report.update(container_type=vi.ftype.name, rsrc_format=vi.fmtver, resources=list(inventory.values()))
    decode_resources(vi, inventory, LVheap, report)
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
            raise Refusal("type_or_value_unsupported", f"Cannot verify the original types or values without decoded {name}.", name)
    return vi


def format_report(report):
    lines = [f"LabVIEW source\n{report['source']}"]
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
            lines.append(f"  {entry['id']} — {entry['state']}, {entry['sections']} sections, {entry['stored_bytes']} bytes{records}")
    lines.extend(("", "Eligible for editable FROG import." if report["compatible"] else "Editable FROG import unavailable for this file."))
    if report["compatible"]:
        projection = report["projection"]
        lines.append(f"The editable projection contains {projection['nodes']} diagram nodes and {projection['wires']} wires.")
        if any(detail["kind"] == "loaded_shift_register" for detail in report.get("adaptations", ())):
            lines.append("The imported register retains its state across completed executions of the loaded document.")
        if report.get("frontend_validated"):
            lines.append("The common FROG Frontend validated the editable projection.")
    for diagnostic in report["diagnostics"]:
        lines.append(f"  [{diagnostic['code']}] {diagnostic['resource']} — {diagnostic['message']}")
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


def validate_frontend(frontend, document, stage):
    model = stage / "model.frog"
    diagnostics = stage / "frontend-diagnostics.json"
    write_json(model, document)
    result = subprocess.run([str(frontend.resolve(strict=True)), "validate", "--source", str(model),
                             "--source-uri", "import:/labview/model.frog", "--document-id", "labview:studio:draft",
                             "--fir-document-id", "labview:studio:fir", "--source-format", "frog.document.draft",
                             "--source-version", str(document["draft_revision"]), "--fir-version", "0.1",
                             "--diagnostics-output", str(diagnostics)],
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


def publish(document, mapping, report, args):
    output = args.output.absolute()
    if output.exists() or output.is_symlink():
        raise Refusal("destination_exists", "Choose a new import destination.")
    stage = Path(tempfile.mkdtemp(prefix=".labview-import-", dir=output.parent))
    try:
        validate_frontend(args.frontend, document, stage)
        write_json(stage / "labview-mapping.json", mapping)
        report["frontend_validated"] = True
        report["report"] = format_report(report)
        write_json(stage / "import-analysis.json", report)

        def commit():
            if output.exists() or output.is_symlink():
                raise Refusal("destination_exists", "Choose a new import destination.")
            if hashlib.sha256(read_snapshot(args.source)).hexdigest() != report["source_sha256"]:
                raise Refusal("source_changed", "The selected file changed during conversion. Analyze it again.")
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
              "compatible": False, "diagnostics": []}
    try:
        data = read_snapshot(args.source)
        digest = report["source_sha256"] = hashlib.sha256(data).hexdigest()
        if args.expected_sha256 and digest != args.expected_sha256.lower():
            raise Refusal("source_changed", "The selected file differs from the analyzed bytes. Analyze it again.")
        vi = inspect(data, args.source, report)
        from import_labview_profile import ProfileRefusal, project

        try:
            document, mapping = project(vi, args.source, digest)
        except ProfileRefusal as error:
            raise Refusal(error.code, str(error), error.resource) from error
        if args.analyze and args.frontend:
            scratch_parent = args.report.parent if args.report else None
            with tempfile.TemporaryDirectory(prefix=".labview-analysis-", dir=scratch_parent) as scratch:
                validate_frontend(args.frontend, document, Path(scratch))
            report["frontend_validated"] = True
        report.update(compatible=True, projection=projection_size(document["diagram"]), profile=mapping["profile"],
                      adaptations=mapping.get("adaptations", []))
        if not args.analyze:
            publish(document, mapping, report, args)
            return 0
    except Refusal as error:
        report["diagnostics"].append(error.diagnostic)
    except (ImportError, OSError, subprocess.TimeoutExpired) as error:
        report["diagnostics"].append(Refusal("reader_or_io", str(error)).diagnostic)
    except (ArithmeticError, AttributeError, LookupError, RuntimeError, TypeError, ValueError, zlib.error) as error:
        report["diagnostics"].append(Refusal("invalid_rsrc", str(error)).diagnostic)

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
