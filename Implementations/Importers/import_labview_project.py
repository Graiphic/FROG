# SPDX-License-Identifier: Apache-2.0
"""Import the explicit local target of an XML LabVIEW project."""

import hashlib
import json
import os
from pathlib import Path, PureWindowsPath
import subprocess
import tempfile
from urllib.parse import unquote
import uuid
import xml.etree.ElementTree as ET

from import_labview_heap_graph import HeapTypes, members
from import_labview_panel import project_panel
from import_labview_profile import ProfileRefusal, field, heap_root, identity, kind, project


def import_project(args, data, report, reader):
    root = args.source.resolve().parent
    report["project_members"] = []

    def reject(message, location=None):
        raise reader.Refusal("project", message, str(location or args.source.name))

    if len(data) > 2 * 1024 * 1024 or b"<!DOCTYPE" in data.upper() or b"\0" in data:
        reject("Expected a bounded UTF-8 LabVIEW project without a DTD.")
    try:
        lv = ET.fromstring(data)
    except ET.ParseError as error:
        reject(f"Invalid project XML: {error}")
    targets = lv.findall("Item")
    if lv.tag != "Project" or lv.get("Type") != "Project" or len(targets) != 1 or targets[0].get("Type") != "My Computer":
        reject("Project import requires one local My Computer target.")
    if sum(1 for _ in lv.iter()) > 4096:
        reject("The project exceeds the supported item count.")

    manifest = ET.Element("Project", xmlns="urn:graiphic:studio:project:1", formatVersion="1",
                          id=str(uuid.uuid5(uuid.NAMESPACE_URL, args.source.name + "/" + report["source_sha256"])),
                          name=args.source.stem, defaultTargetId="target-local")
    ET.SubElement(ET.SubElement(manifest, "SourceRoots"), "Root", id="root-project", path=".")
    files = ET.SubElement(manifest, "Files")
    tree = ET.SubElement(manifest, "Tree")
    entries, outputs, fingerprints = {}, {}, {}
    total = 0

    def snapshot(path):
        nonlocal total
        payload = reader.read_snapshot(path)
        total += len(payload)
        if total > 64 * 1024 * 1024:
            reject("The local project exceeds 64 MiB.")
        fingerprints[path] = hashlib.sha256(payload).hexdigest()
        return payload

    def local(url):
        value = unquote(url).replace("\\", "/")
        if not value or PureWindowsPath(value).anchor or ":" in value or "\0" in value:
            reject(f"Expected a local project-relative URL: {url}")
        # NI project items conventionally use ../File.vi relative to the
        # project item itself. Also accept plain paths relative to its folder.
        rel = value.removeprefix("../")
        path = (root / rel).resolve()
        if not path.is_relative_to(root) or not path.is_file():
            reject(f"Missing or external project file: {url}")
        return path

    seen_rows = 0
    def collect(parent, dest, depth=0):
        nonlocal seen_rows
        if depth > 32:
            reject("Project tree is too deeply nested.")
        for item in parent.findall("Item"):
            category, name = item.get("Type"), item.get("Name", "")
            seen_rows += 1
            row = f"item-{seen_rows}"
            if category in ("Folder", "Dependencies"):
                folder = ET.SubElement(dest, "Folder", id=row, name=name, mode="virtual")
                collect(item, folder, depth + 1)
            elif category == "Build" and not item.findall("Item"):
                continue
            elif category in ("VI", "Document") and item.get("URL"):
                path = local(item.attrib["URL"])
                ext = path.suffix.lower()
                if category == "VI" and ext not in (".vi", ".ctl"):
                    reject(f"Unsupported VI project member: {name}")
                if path not in entries:
                    rel = path.relative_to(root)
                    out = rel.with_suffix({".vi": ".frog", ".ctl": ".ftype"}[ext]) if category == "VI" else Path("Resources") / rel
                    if out in outputs or out.name.casefold() in {"import-analysis.json", "labview-mapping.json"}:
                        reject(f"Conflicting project destination: {out}")
                    file_id = f"file-{len(entries)+1}"
                    member_kind = {".vi": "frog", ".ctl": "type-definition"}.get(ext, "resource") if category == "VI" else "resource"
                    entries[path] = (out, file_id, member_kind)
                    outputs[out] = None
                    ET.SubElement(files, "File", id=file_id, kind=member_kind, path=out.as_posix())
                    report["project_members"].append({"source": rel.as_posix(), "output": out.as_posix()})
                ET.SubElement(dest, "FileRef", id=row, fileId=entries[path][1])
            else:
                reject(f"Unsupported project item {name} ({category}).")

    collect(targets[0], tree)
    programs = [path for path, (_, _, k) in entries.items() if k == "frog"]
    if not programs:
        reject("The project contains no local VI.")
    ET.SubElement(ET.SubElement(manifest, "Targets"), "Target", id="target-local", kind="local", name="My Computer")
    ET.SubElement(ET.SubElement(manifest, "Configurations"), "Configuration", id="config-debug", name="Debug")
    runs = ET.SubElement(manifest, "RunConfigurations")
    for path in programs:
        run_id = "run-" + entries[path][1]
        ET.SubElement(runs, "Run", id=run_id, name=path.stem, entryFileId=entries[path][1],
                      targetId="target-local", configurationId="config-debug")
        if len(programs) == 1:
            manifest.set("defaultRunId", run_id)
    ET.SubElement(manifest, "BuildSpecifications")
    ET.SubElement(manifest, "Extensions")
    mapping = {"profile": "labview-local-project-v1", "programs": {}, "source_sha256": report["source_sha256"]}
    if not args.frontend:
        reject("Project analysis requires the FROG Frontend to validate the members and their Type Definitions.")
    from pylabview import LVblock, LVheap, LVrsrcontainer
    scratch_parent = args.report.parent if args.report else args.output.parent if args.output else None
    with tempfile.TemporaryDirectory(prefix=".labview-project-", dir=scratch_parent) as temporary:
        scratch = Path(temporary).resolve()
        definitions = {}
        for path, (out, _, category) in entries.items():
            if category == "frog":
                continue
            payload = snapshot(path)
            if category == "resource":
                outputs[out] = payload
                continue
            vi, inventory = reader.index_resources(payload, path, LVrsrcontainer, LVblock)
            details = {"diagnostics": []}
            reader.decode_resources(vi, inventory, LVheap, details)
            widgets, issues = project_panel(vi)
            if details["diagnostics"] or issues or len(widgets) != 1:
                reject(f"Could not translate the saved control schema: {path.name}")
            definition = {"format": "frog.type_definition.draft", "draft_revision": 1, "name": path.stem,
                          "id": str(uuid.uuid5(uuid.NAMESPACE_URL, args.source.name + "/" + out.as_posix())),
                          "revision": 1, "icon_svg": "", "root": next(iter(widgets.values()))}
            definitions[path] = definition
            outputs[out] = definition
            # Keep native scratch files separate from names in the input tree.
            reader.write_json(scratch / (entries[path][1] + ".ftype"), definition)
        for path in programs:
            payload = snapshot(path)
            details = {"diagnostics": []}
            vi = reader.inspect(payload, path, details)
            try:
                doc, evidence = project(vi, path, fingerprints[path])
            except ProfileRefusal as error:
                reject(f"{path.name}: {error}", error.resource)
            out = entries[path][0]
            controls = {}
            for link in details["declared_links"]:
                if link["kind"] != "VICC":
                    continue
                parts = link["path_components"]
                if not parts or link["path_type"] != 1:
                    reject(f"Unresolved control link in {path.name}: {parts}")
                control = (path.parent / Path(*[part for part in parts if part])).resolve()
                if control not in definitions:
                    reject(f"Missing project control for {path.name}: {control.name}")
                key = tuple(link["qualified_name"])
                if key in controls and controls[key] != control:
                    reject(f"Ambiguous control identity in {path.name}: {key}")
                controls[key] = control
            deps = {}
            widgets = {w["id"]: w for w in doc["frontPanel"]["widgets"]}
            types = HeapTypes(vi)
            pane = field(field(heap_root(vi, b"FPHb"), "root", "FPHb"), "paneHierarchy", "FPHb")
            for obj in members(pane, "zPlaneList", "FPHb"):
                if kind(obj) != "fPDCO":
                    continue
                td = types.describe(types.heap(obj, "typeDesc", "FPHb"), "FPHb")
                if td.family != "TypeDef":
                    continue
                key = tuple(part.decode(vi.textEncoding) for part in td.definition)
                control = controls.get(key)
                if control is None:
                    reject(f"Unresolved Type Definition in {path.name}: {key}")
                ctl_out, ctl_id, _ = entries[control]
                ref = Path(os.path.relpath(ctl_out, out.parent)).as_posix()
                widget = widgets[f"lv-widget-{identity(obj, 'FPHb')}"]
                reader.write_json(scratch / "widget.json", widget)
                model = scratch / (ctl_id + ".ftype")
                linked = subprocess.run([args.frontend.resolve(), "typedef-link", model, scratch / "widget.json", ref],
                                        capture_output=True, timeout=20)
                if linked.returncode:
                    reject(f"{path.name}, {widget['label']}: {linked.stderr.decode('utf-8', 'replace')[:500]}")
                widget["props"]["type_definition"] = json.loads(linked.stdout)
                link = widget["props"]["type_definition"]
                revision = 12 if "tab_members" in link or isinstance(link["baseline_widget"], dict) else 8
                doc["draft_revision"] = max(revision, doc["draft_revision"])
                deps[ref] = {"kind": 2, "reference": ref, "source_uri": model.as_uri(),
                             "identity": definitions[control]["id"], "version": "1", "path": str(model)}
            lock = scratch / "dependencies.json"
            reader.write_json(lock, {"format": "frog.frontend.dependencies", "version": "1", "dependencies": list(deps.values())})
            reader.validate_frontend(args.frontend, doc, scratch, lock)
            outputs[out] = doc
            mapping["programs"][out.as_posix()] = evidence
    ET.indent(manifest)
    outputs[Path(args.source.stem + ".frprj")] = ET.tostring(manifest, encoding="utf-8", xml_declaration=True)
    mapping["source_files"] = {p.relative_to(root).as_posix(): h for p, h in fingerprints.items()}
    report.update(compatible=True, editable=True, execution_checks="complete", frontend_validated=True,
                  profile=mapping["profile"], member_sha256=mapping["source_files"])
    if not args.analyze:
        reader.publish(None, mapping, report, args, outputs, fingerprints)
