"""Finite S090 qualification through public CLI, schemas and bridge IPC only.

No Studio/Runtime/LLVM API, Source evaluator or FIR interpreter is imported.
Requires Python's stdlib and an already available jsonschema installation.
The integrating build runs this script; creating it does not qualify binaries.
"""
from __future__ import annotations

import argparse
import ctypes
from ctypes import wintypes
from dataclasses import dataclass
from decimal import Decimal
import hashlib
import json
import os
from pathlib import Path, PureWindowsPath
import queue
import re
import shutil
import struct
import subprocess
import sys
import threading
import time
from urllib.parse import urljoin
import uuid

sys.dont_write_bytecode = True
from check_outputs import check_outputs, load_output

MAX_FRAME = 16 * 1024 * 1024
MAX_STDERR = 65536
DEADLINE = 90
SELECTION = {"engine_id": "graiphic.reference-c", "target_id": "windows-x64", "profile_id": "native-fast"}


def need(condition, message):
    if not condition:
        raise AssertionError(message)


def digest(data):
    return hashlib.sha256(data).hexdigest()


def file_digest(path):
    h = hashlib.sha256()
    with Path(path).open("rb") as source:
        for block in iter(lambda: source.read(65536), b""):
            h.update(block)
    return h.hexdigest()


def unique(pairs):
    out = {}
    for key, value in pairs:
        need(key not in out, f"Duplicate JSON member: {key}")
        out[key] = value
    return out


def forbidden_number(token):
    raise AssertionError(f"Non-JSON numeric token: {token}")


DECODER = json.JSONDecoder(object_pairs_hook=unique, parse_int=int, parse_float=Decimal,
                           parse_constant=forbidden_number)


def strict(data):
    if isinstance(data, bytes):
        need(len(data) <= MAX_FRAME, "JSON exceeds16MiB")
        data = data.decode("utf-8", "strict")
    need(len(data.encode("utf-8", "strict")) <= MAX_FRAME, "JSON exceeds16MiB")
    value = DECODER.decode(data)
    pending, count = [(value, 0)], 0
    while pending:
        item, depth = pending.pop()
        count += 1
        need(depth <= 128 and count <= 1000000, "JSON nesting/value budget exceeded")
        if isinstance(item, str):
            item.encode("utf-8", "strict")  # Reject isolated escaped surrogates.
        elif type(item) is dict:
            pending.extend((entry, depth + 1) for pair in item.items() for entry in pair)
        elif type(item) is list:
            pending.extend((entry, depth + 1) for entry in item)
    return value


@dataclass(frozen=True)
class Raw:
    text: str


def encoded(value):
    """Preserve already admitted provenance/parameter tokens, never float-coerce."""
    if isinstance(value, Raw):
        return value.text
    if isinstance(value, Decimal):
        need(value.is_finite(), "Nonfinite Decimal is outside JSON")
        return str(value)
    if type(value) is dict:
        return "{" + ",".join(json.dumps(k, ensure_ascii=False) + ":" + encoded(v) for k, v in value.items()) + "}"
    if type(value) is list:
        return "[" + ",".join(encoded(v) for v in value) + "]"
    return json.dumps(value, ensure_ascii=False, allow_nan=False, separators=(",", ":"))


def raw_member(document, name):
    """Find a token using stdlib raw_decode; do not re-encode hashed values."""
    need(type(strict(document)) is dict, "Raw member owner is not an object")
    index = 0

    def whitespace(at):
        while at < len(document) and document[at] in " \t\r\n":
            at += 1
        return at

    index = whitespace(index)
    need(document[index] == "{", "Object opening token absent")
    index += 1
    while True:
        index = whitespace(index)
        if document[index] == "}":
            raise AssertionError(f"Missing raw member: {name}")
        key, index = DECODER.raw_decode(document, index)
        index = whitespace(index)
        need(document[index] == ":", "Object member colon absent")
        start = whitespace(index + 1)
        _, end = DECODER.raw_decode(document, start)
        if key == name:
            return document[start:end]
        index = whitespace(end)
        need(document[index] in ",}", "Object member delimiter absent")
        if document[index] == "}":
            raise AssertionError(f"Missing raw member: {name}")
        index += 1


def save_json(path, value):
    Path(path).write_text(encoded(value) + "\n", encoding="utf-8", newline="\n")


class Schemas:
    def __init__(self, sdk):
        try:
            from jsonschema import Draft202012Validator
            from referencing import Registry, Resource
        except ImportError as error:
            raise RuntimeError("Environment unqualified: install-free run requires an already available jsonschema/referencing runtime") from error
        self.validator = Draft202012Validator
        self.documents = {}
        folder = sdk / "schemas"
        for path in sorted(folder.glob("*.schema.json")):
            self.documents[path.name] = strict(path.read_bytes())
        need(self.documents, "Public SDK schema directory is absent")
        resources = {}
        for name, document in self.documents.items():
            resource = Resource.from_contents(document)
            resources[(folder / name).as_uri()] = resource
            if "$id" in document:
                resources[document["$id"]] = resource
        # Same public owners as SDK/tests/validate_frontend_results.py:
        # source-map/correspondence reference the real IR schemas by their IDs.
        ir_folder = sdk.parents[1] / "IR" / "schema"
        ir_paths = sorted(ir_folder.glob("*.schema.json"))
        need(ir_paths, "The public kit's IR/schema closure is absent")
        for path in ir_paths:
            document = strict(path.read_bytes())
            need("$id" in document and document["$id"] not in resources, "Missing or duplicate public IR schema identity")
            resources[document["$id"]] = Resource.from_contents(document)
        for name, document in self.documents.items():
            resource = Resource.from_contents(document)
            # Relative public references resolve locally at each schema's ID.
            for base in self.documents.values():
                if "$id" in base:
                    resources.setdefault(urljoin(base["$id"], name), resource)

        def no_network(uri):
            raise RuntimeError(f"Unresolved public schema reference; network forbidden: {uri}")

        self.registry = Registry(retrieve=no_network).with_resources(resources.items())

    def check(self, name, value):
        need(name in self.documents, f"Required public schema is missing: {name}")
        self.validator(self.documents[name], registry=self.registry).validate(value)

    def message(self, message):
        self.check("bridge-message.schema.json", message)
        payload = message["payload"]
        if message["method"] == "describe" and message["kind"] == "response" and message.get("status") == "ok":
            self.check("bridge-describe-0.2.schema.json", payload)
        elif not payload and (message["method"] in ("describe", "probe") or
                              message["kind"] == "response" and message.get("status") != "ok"):
            return
        else:
            self.check("bridge-payload.schema.json", payload)
            need(payload["method"] == message["method"], "Payload method changed")
            phase = {"request": "request", "response": "response", "progress": "progress"}[message["kind"]]
            need(payload["phase"] == phase, "Payload phase changed")


class Job:
    """Test-local kill-on-close containment, assigned before any peer request."""
    def __init__(self, process):
        self.api = ctypes.WinDLL("kernel32", use_last_error=True)

        class Basic(ctypes.Structure):
            _fields_ = [("process_time", ctypes.c_int64), ("job_time", ctypes.c_int64),
                        ("flags", wintypes.DWORD), ("minimum", ctypes.c_size_t),
                        ("maximum", ctypes.c_size_t), ("active", wintypes.DWORD),
                        ("affinity", ctypes.c_size_t), ("priority", wintypes.DWORD), ("schedule", wintypes.DWORD)]

        class IO(ctypes.Structure):
            _fields_ = [(key, ctypes.c_uint64) for key in ("read_ops", "write_ops", "other_ops", "read_bytes", "write_bytes", "other_bytes")]

        class Limits(ctypes.Structure):
            _fields_ = [("basic", Basic), ("io", IO), ("process_memory", ctypes.c_size_t),
                        ("job_memory", ctypes.c_size_t), ("peak_process", ctypes.c_size_t), ("peak_job", ctypes.c_size_t)]

        self.api.CreateJobObjectW.argtypes = [ctypes.c_void_p, wintypes.LPCWSTR]
        self.api.CreateJobObjectW.restype = wintypes.HANDLE
        self.api.SetInformationJobObject.argtypes = [wintypes.HANDLE, ctypes.c_int, ctypes.c_void_p, wintypes.DWORD]
        self.api.AssignProcessToJobObject.argtypes = [wintypes.HANDLE, wintypes.HANDLE]
        self.api.OpenProcess.argtypes = [wintypes.DWORD, wintypes.BOOL, wintypes.DWORD]
        self.api.OpenProcess.restype = wintypes.HANDLE
        self.api.QueryFullProcessImageNameW.argtypes = [wintypes.HANDLE, wintypes.DWORD, wintypes.LPWSTR, ctypes.POINTER(wintypes.DWORD)]
        self.api.QueryFullProcessImageNameW.restype = wintypes.BOOL
        self.api.GetProcessTimes.argtypes = [wintypes.HANDLE, ctypes.POINTER(wintypes.FILETIME), ctypes.POINTER(wintypes.FILETIME),
                                             ctypes.POINTER(wintypes.FILETIME), ctypes.POINTER(wintypes.FILETIME)]
        self.api.GetProcessTimes.restype = wintypes.BOOL
        self.api.GetSystemDirectoryW.argtypes = [wintypes.LPWSTR, wintypes.UINT]
        self.api.GetSystemDirectoryW.restype = wintypes.UINT
        self.system_console = None
        self.api.CloseHandle.argtypes = [wintypes.HANDLE]
        self.api.TerminateJobObject.argtypes = [wintypes.HANDLE, wintypes.UINT]
        self.api.QueryInformationJobObject.argtypes = [wintypes.HANDLE, ctypes.c_int, ctypes.c_void_p, wintypes.DWORD, ctypes.c_void_p]
        self.handle = self.api.CreateJobObjectW(None, None)
        if not self.handle:
            process.kill()
            process.wait(timeout=3)
            raise AssertionError("Cannot create qualification process Job")
        limits = Limits()
        limits.basic.flags = 0x2000  # JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE
        child = self.api.OpenProcess(0x0100 | 0x0001, False, process.pid)  # SET_QUOTA | TERMINATE
        try:
            need(self.api.SetInformationJobObject(self.handle, 9, ctypes.byref(limits), ctypes.sizeof(limits)), "Cannot set qualification Job limits")
            need(child and self.api.AssignProcessToJobObject(self.handle, child), "Cannot contain qualification process tree")
        except BaseException:
            self.close()
            process.kill()
            process.wait(timeout=3)
            raise
        finally:
            if child:
                self.api.CloseHandle(child)

    def process_identity(self, pid):
        result = {"pid": pid}
        process = self.api.OpenProcess(0x1000, False, pid)  # PROCESS_QUERY_LIMITED_INFORMATION only.
        if not process:
            result["open_process_error"] = ctypes.get_last_error()
            return result
        try:
            image = ctypes.create_unicode_buffer(32768)
            length = wintypes.DWORD(len(image))
            if self.api.QueryFullProcessImageNameW(process, 0, image, ctypes.byref(length)):
                result["image_path"] = image[:length.value]
                try:
                    result["image_sha256"] = file_digest(result["image_path"])
                except OSError as error:
                    result["image_hash_error"] = str(error)
            else:
                result["image_query_error"] = ctypes.get_last_error()
            created, exited, kernel, user = (wintypes.FILETIME() for _ in range(4))
            if self.api.GetProcessTimes(process, ctypes.byref(created), ctypes.byref(exited), ctypes.byref(kernel), ctypes.byref(user)):
                result["creation_filetime_100ns"] = (int(created.dwHighDateTime) << 32) | int(created.dwLowDateTime)
            else:
                result["creation_time_query_error"] = ctypes.get_last_error()
        finally:
            self.api.CloseHandle(process)
        return result

    def snapshot(self):
        class Accounting(ctypes.Structure):
            _fields_ = [("user", ctypes.c_int64), ("kernel", ctypes.c_int64),
                        ("period_user", ctypes.c_int64), ("period_kernel", ctypes.c_int64),
                        ("faults", wintypes.DWORD), ("total", wintypes.DWORD),
                        ("active", wintypes.DWORD), ("terminated", wintypes.DWORD)]
        value = Accounting()
        returned = wintypes.DWORD()
        need(self.api.QueryInformationJobObject(self.handle, 1, ctypes.byref(value), ctypes.sizeof(value), ctypes.byref(returned)), "Cannot query Job basic accounting")
        offsets = {name: getattr(Accounting, name).offset for name, _ in Accounting._fields_}
        expected_offsets = {"user": 0, "kernel": 8, "period_user": 16, "period_kernel": 24,
                            "faults": 32, "total": 36, "active": 40, "terminated": 44}
        pointer_bytes = ctypes.sizeof(ctypes.c_size_t)
        pid_buffer = ctypes.create_string_buffer(8 + pointer_bytes * 64)
        pid_returned = wintypes.DWORD()
        need(self.api.QueryInformationJobObject(self.handle, 3, pid_buffer, ctypes.sizeof(pid_buffer), ctypes.byref(pid_returned)), "Cannot query Job basic process-ID list")
        assigned, listed = struct.unpack_from("<II", pid_buffer.raw)
        need(listed <= 64, "Job process-ID list exceeded its finite observation capacity")
        pids = [int.from_bytes(pid_buffer.raw[8+i*pointer_bytes:8+(i+1)*pointer_bytes], "little") for i in range(listed)]
        if self.system_console is None:
            system = ctypes.create_unicode_buffer(32768)
            length = self.api.GetSystemDirectoryW(system, len(system))
            need(0 < length < len(system), "Cannot identify the exact Windows system directory")
            path = Path(system[:length]) / "conhost.exe"
            self.system_console = {"image_path": str(path), "image_sha256": file_digest(path)}
        return {"information_class": 1, "buffer_bytes": ctypes.sizeof(value), "return_length": int(returned.value),
                "field_offsets": offsets, "layout_matches_windows_sdk": offsets == expected_offsets and ctypes.sizeof(value) == 48 and returned.value == 48,
                "raw_hex": bytes(value).hex(), "fields": {name: int(getattr(value, name)) for name, _ in Accounting._fields_},
                "pid_list_information_class": 3, "pid_list_return_length": int(pid_returned.value),
                "pid_list_raw_hex": pid_buffer.raw[:8+listed*pointer_bytes].hex(),
                "assigned_processes": assigned, "process_ids_in_list": listed, "active_process_ids": sorted(pids),
                "active_process_metadata": [self.process_identity(pid) for pid in sorted(pids)],
                "expected_system_console": self.system_console,
                "monotonic_ns": time.monotonic_ns()}

    def count(self):
        # This is Windows job accounting, not a count of distinct observed PIDs.
        return self.snapshot()["fields"]["total"]

    def terminate(self):
        if self.handle:
            self.api.TerminateJobObject(self.handle, 2)

    def close(self):
        if getattr(self, "handle", None):
            self.api.CloseHandle(self.handle)
            self.handle = None


def run_frontend(argv, folder):
    save_json(folder / "frontend-command.json", {"schema": "frog.reference-c.public_command", "schema_version": "0.1",
              "argv": list(map(str, argv)), "executable_sha256": file_digest(argv[0]), "deadline_seconds": DEADLINE})
    start = time.monotonic()
    with (folder / "frontend.stdout").open("wb") as out, (folder / "frontend.stderr").open("wb") as err:
        process = subprocess.Popen(list(map(str, argv)), stdin=subprocess.DEVNULL, stdout=out, stderr=err,
                                   shell=False, creationflags=subprocess.CREATE_NO_WINDOW)
        job = Job(process)
        try:
            while process.poll() is None:
                need(time.monotonic() - start < DEADLINE, "Public Frontend CLI deadline exceeded")
                need((folder / "frontend.stdout").stat().st_size <= 1048576 and
                     (folder / "frontend.stderr").stat().st_size <= MAX_STDERR, "Public Frontend CLI log limit exceeded")
                time.sleep(0.02)
            need(process.returncode == 0, f"Public Frontend CLI refused Source (exit {process.returncode}); see retained logs")
            need((folder / "frontend.stdout").stat().st_size <= 1048576 and
                 (folder / "frontend.stderr").stat().st_size <= MAX_STDERR, "Public Frontend CLI final log limit exceeded")
        finally:
            if process.poll() is None:
                job.terminate()
            process.wait(timeout=3)
            job.close()
    save_json(folder / "frontend-result.json", {"exit_code": process.returncode, "elapsed_ms": int((time.monotonic()-start)*1000),
              "stdout_sha256": file_digest(folder / "frontend.stdout"), "stderr_sha256": file_digest(folder / "frontend.stderr")})


def exact_read(stream, size):
    out = bytearray()
    while len(out) < size:
        part = stream.read(size - len(out))
        if not part:
            need(not out, "Truncated public bridge frame")
            return None
        out.extend(part)
    return bytes(out)


class Session:
    def __init__(self, args, schemas, folder, label):
        self.schemas, self.folder = schemas, folder
        self.identity = {"session_id": "public-corpus-" + label + "-" + uuid.uuid4().hex, "generation": 1}
        self.sequence, self.progress_limit = 0, 8
        self.frames, self.stderr, self.workers = queue.Queue(maxsize=64), bytearray(), []
        self.error = None
        self.process = subprocess.Popen([str(args.peer), "--compiler-config", args.compiler_config], stdin=subprocess.PIPE,
                                        stdout=subprocess.PIPE, stderr=subprocess.PIPE, bufsize=0, shell=False,
                                        creationflags=subprocess.CREATE_NO_WINDOW)
        self.job = Job(self.process)
        save_json(folder / "peer-launch.json", {"argv": [str(args.peer), "--compiler-config", args.compiler_config],
                  "peer_sha256": file_digest(args.peer), **self.identity})
        try:
            self.baseline_job = self.capture_job("baseline")
        except BaseException:
            self.job.terminate()
            self.process.wait(timeout=3)
            self.job.close()
            for stream in (self.process.stdin, self.process.stdout, self.process.stderr):
                stream.close()
            raise

        def receive():
            try:
                total, count = 0, 0
                while True:
                    prefix = exact_read(self.process.stdout, 4)
                    if prefix is None:
                        self.frames.put(None, timeout=1)
                        return
                    size, = struct.unpack("<I", prefix)
                    need(0 < size <= MAX_FRAME, "Peer frame exceeds public16MiB limit")
                    total, count = total + size + 4, count + 1
                    need(total <= 32*1024*1024 and count <= 64, "Peer lifetime stdout/frame budget exceeded")
                    data = exact_read(self.process.stdout, size)
                    need(data is not None, "Peer ended within a frame")
                    self.frames.put(data, timeout=1)
            except BaseException as error:
                self.error = error
                self.job.terminate()
                try:
                    self.frames.put_nowait(error)
                except queue.Full:
                    pass

        def receive_stderr():
            try:
                while True:
                    data = self.process.stderr.read(4096)
                    if not data:
                        return
                    need(len(self.stderr)+len(data) <= MAX_STDERR, "Peer stderr exceeded64KiB")
                    self.stderr.extend(data)
            except BaseException as error:
                self.error = error
                self.job.terminate()
                try:
                    self.frames.put_nowait(error)
                except queue.Full:
                    pass

        for target in (receive, receive_stderr):
            worker = threading.Thread(target=target, daemon=True)
            worker.start()
            self.workers.append(worker)

    def capture_job(self, phase):
        observation = self.job.snapshot()
        save_json(self.folder / f"{phase}.job.json", {"schema": "frog.reference-c.job_accounting", "schema_version": "0.1",
                  "phase": phase, "peer_pid": self.process.pid, **observation})
        self.latest_job = observation
        need(observation["layout_matches_windows_sdk"], "Job accounting layout/returned size differs from Windows SDK; inspect retained raw bytes")
        return observation

    def exchange(self, method, payload):
        self.sequence += 1
        request = {"schema": "frog.bridge.message", "protocol_version": "0.1", "request_id": f"request-{self.sequence}",
                   **self.identity, "kind": "request", "method": method, "payload": payload}
        data = encoded(request).encode("utf-8")
        need(0 < len(data) <= MAX_FRAME, "Request exceeds16MiB")
        self.schemas.message(strict(data))
        prefix = self.folder / f"{self.sequence:02d}-{method}"
        prefix.with_suffix(".request.json").write_bytes(data)
        prefix.with_suffix(".request.frame").write_bytes(struct.pack("<I", len(data))+data)
        end, written = time.monotonic()+DEADLINE, queue.Queue(maxsize=1)

        def write():
            try:
                pending = memoryview(struct.pack("<I", len(data))+data)
                while pending:
                    count = self.process.stdin.write(pending)
                    need(count, "Bridge stdin closed before the request was written")
                    pending = pending[count:]
                written.put(None)
            except BaseException as error:
                written.put(error)

        writer = threading.Thread(target=write, daemon=True)
        self.workers.append(writer)
        writer.start()
        try:
            result = written.get(timeout=max(0.01, end-time.monotonic()))
            if result:
                raise result
            progress = 0
            while True:
                if self.error:
                    raise self.error
                raw = self.frames.get(timeout=max(0.01, end-time.monotonic()))
                if isinstance(raw, BaseException):
                    raise raw
                need(raw is not None, "Peer closed before its final response")
                response = strict(raw)
                self.schemas.message(response)
                need(all(response[key] == request[key] for key in ("request_id", "session_id", "generation", "method", "protocol_version")), "Peer returned an obsolete or substituted tuple")
                if response["kind"] == "progress":
                    progress += 1
                    need(progress <= self.progress_limit and response["progress"]["completed"] <= response["progress"]["total"], "Peer progress limit/total violated")
                    (self.folder / f"{self.sequence:02d}-{method}.progress-{progress}.json").write_bytes(raw)
                    continue
                need(response["kind"] == "response", "Peer returned neither progress nor a final response")
                prefix.with_suffix(".response.json").write_bytes(raw)
                prefix.with_suffix(".response.frame").write_bytes(struct.pack("<I", len(raw))+raw)
                self.capture_job(f"{self.sequence:02d}-{method}")
                return response, raw.decode("utf-8")
        except queue.Empty as error:
            self.job.terminate()
            raise AssertionError(f"Public bridge {method} deadline exceeded") from error

    def close(self):
        try:
            if self.process.stdin:
                self.process.stdin.close()
            try:
                self.process.wait(timeout=3)
            except subprocess.TimeoutExpired:
                self.job.terminate()
                self.process.wait(timeout=3)
        finally:
            self.job.close()
            for worker in self.workers:
                worker.join(timeout=2)
            for stream in (self.process.stdout, self.process.stderr):
                stream.close()
            (self.folder / "peer.stderr").write_bytes(self.stderr)
        need(not any(worker.is_alive() for worker in self.workers), "A qualification I/O thread survived process cleanup")
        if self.error:
            raise self.error
        need(self.process.returncode == 0, f"Peer exited abnormally: {self.process.returncode}")


def successful(response):
    need(response["status"] == "ok" and not any(d["severity"] == "error" for d in response["diagnostics"]),
         f"Bridge refused public request: {encoded(response.get('diagnostics', []))}")
    return response["payload"]


def payload(method, **extra):
    return {"schema": "frog.bridge.payload", "schema_version": "0.1", "method": method, "phase": "request", **extra}


def process_identities(observation):
    records = observation["active_process_metadata"]
    need(sorted(row["pid"] for row in records) == observation["active_process_ids"], "Process identity records lost an active Job PID")
    identities = set()
    for row in records:
        need(type(row.get("creation_filetime_100ns")) is int and row["creation_filetime_100ns"] > 0 and
             type(row.get("image_path")) is str and re.fullmatch(r"[0-9a-f]{64}", row.get("image_sha256", "")),
             "An active PID's image/hash/creation identity could not be qualified")
        identities.add((row["pid"], row["creation_filetime_100ns"], os.path.normcase(os.path.normpath(row["image_path"])), row["image_sha256"]))
    need(len(identities) == len(records), "Repeated process identity in Job snapshot")
    return identities


def qualify_startup(session):
    before, after = session.baseline_job, session.latest_job
    expected = after["expected_system_console"]
    console_path = os.path.normcase(os.path.normpath(expected["image_path"]))
    need(before["expected_system_console"] == expected, "Windows system console identity changed during startup")
    identities = []
    for observation in (before, after):
        current = process_identities(observation)
        need(observation["fields"]["total"] == len(current) and observation["fields"]["active"] == len(current),
             "Startup Job accounting includes an unobserved or transient process")
        need(sum(row[0] == session.process.pid for row in current) == 1, "The actual peer PID is absent from its startup Job")
        others = {row for row in current if row[0] != session.process.pid}
        need(len(others) <= 1 and all(row[2] == console_path and row[3] == expected["image_sha256"] for row in others),
             "Startup launched a process other than the exact Windows system conhost.exe")
        identities.append(current)
    need(identities[0] <= identities[1], "A startup process disappeared or changed identity before Describe")
    added = identities[1] - identities[0]
    need(len(added) <= 1 and all(row[0] != session.process.pid and row[2] == console_path and row[3] == expected["image_sha256"] for row in added),
         "Startup growth is not solely one still-active system console host")
    need(after["fields"]["total"] - before["fields"]["total"] == len(added), "Startup accounting growth differs from the identified console PID")
    session.described_job = after
    session.described_identities = identities[1]
    session.startup_console_growth = len(added)
    save_json(session.folder / "startup-validation.json", {"status": "passed", "expected_system_console": expected,
              "baseline_total_processes": before["fields"]["total"], "post_describe_total_processes": after["fields"]["total"],
              "allowed_console_growth": len(added), "active_process_metadata": after["active_process_metadata"]})


def check_description(session):
    description = successful(session.exchange("describe", {})[0])
    need(description["bridge_id"] == "graiphic.reference-c" and description["engine_id"] == SELECTION["engine_id"], "Unexpected public bridge identity")
    need(description["operations_exhaustive"] is False and description["constraints"]["deferToValidate"] is True, "Discovery unexpectedly replaced full-graph validation")
    target = next((t for t in description["targets"] if t["id"] == SELECTION["target_id"]), None)
    need(target and target["backend_id"] == "msvc-c17-19.44" and target["hardware_id"] == "cpu:x86_64" and "native-fast" in target["profiles"], "Exact C17 realization target is unavailable")
    need(set(("describe", "validate", "run", "observe", "stop")) <= set(description["methods"]), "Finite public method set is incomplete")
    session.progress_limit = min(8, description["limits"]["maximum_progress_messages"])
    qualify_startup(session)


def decoded_command_line(value):
    need(re.fullmatch(r"(?:[0-9a-f]{2})+", value) is not None, "Command line is not exact hexadecimal bytes")
    data = bytes.fromhex(value)
    need(len(data) % 2 == 0 and data.endswith(b"\0\0"), "Command line omits its UTF16LE terminal NUL")
    text = data.decode("utf-16-le", "strict")
    need(text[-1:] == "\0" and "\0" not in text[:-1], "Command line has interior NUL")
    api = ctypes.WinDLL("shell32", use_last_error=True)
    api.CommandLineToArgvW.argtypes = [wintypes.LPCWSTR, ctypes.POINTER(ctypes.c_int)]
    api.CommandLineToArgvW.restype = ctypes.POINTER(wintypes.LPWSTR)
    kernel = ctypes.WinDLL("kernel32", use_last_error=True)
    kernel.LocalFree.argtypes = [ctypes.c_void_p]
    count = ctypes.c_int()
    argv = api.CommandLineToArgvW(text[:-1], ctypes.byref(count))
    need(argv, "Windows could not decode the recorded command line")
    try:
        return [argv[i] for i in range(count.value)]
    finally:
        kernel.LocalFree(ctypes.cast(argv, ctypes.c_void_p))


def retain_artifacts(observations, folder, values_bytes, config_sha):
    artifacts = observations["artifacts"]
    need(type(artifacts) is list and len(artifacts) <= 64, "Artifact count exceeded finite contract")
    folder.mkdir()
    raw, total = {}, 0
    for row in artifacts:
        need(type(row) is dict and set(row) == {"name", "sha256", "encoding", "data"}, "Artifact entry changed")
        name = row["name"]
        need(type(name) is str and re.fullmatch(r"[A-Za-z0-9_.-]{1,160}", name) and name not in (".", "..") and name not in raw, "Unsafe or duplicate artifact name")
        need(row["encoding"] in ("utf8", "hex") and type(row["data"]) is str, "Unknown artifact encoding")
        if row["encoding"] == "utf8":
            data = row["data"].encode("utf-8", "strict")
        else:
            need(re.fullmatch(r"(?:[0-9a-f]{2})*", row["data"]) is not None, "Artifact hex is not canonical")
            data = bytes.fromhex(row["data"])
        total += len(data)
        need(total <= 4*1024*1024 and digest(data) == row["sha256"], "Artifact bytes exceeded the budget or its SHA256 changed")
        (folder / name).write_bytes(data)
        raw[name] = data
    need("kernel.c" in raw and "kernel.exe" in raw, "Qualification omitted generated C or the actual executable")
    need(digest(raw["kernel.c"]) == observations["generated_c_sha256"] and digest(raw["kernel.exe"]) == observations["executable_sha256"], "Source/executable evidence differs from its admitted artifact identity")
    need(raw["kernel.exe"][:2] == b"MZ", "Generated executable is not a PE artifact")
    need(observations["compiler_config_sha256"] == config_sha and observations["kernel_exit_code"] == 0 and
         observations["execution"] == "compiled-c17" and observations["values_sha256"] == digest(values_bytes), "Actual execution evidence does not bind the exact inputs/compiler/results")
    roles = {}
    for name, data in raw.items():
        if not name.endswith(".command.json"):
            continue
        command = strict(data)
        role = command["role"]
        need(role in ("cl", "link", "kernel") and role not in roles, "Missing or duplicate exact compiler/kernel command")
        need(command["schema"] == "frog.bridge.reference_c.command" and command["schema_version"] == "0.1" and
             command["compiler_config_sha256"] == config_sha and command["includes_terminal_nul"] is True, "Command evidence contract changed")
        need(decoded_command_line(command["command_line_utf16le_hex"]) == command["argv"] and
             command["argv"][0] == command["executable"], "Recorded argv differs from the exact invoked Windows command line")
        expected = {"cl": "cl.exe", "link": "link.exe", "kernel": "kernel.exe"}[role]
        need(PureWindowsPath(command["executable"]).name == expected, "Command invoked an unexpected executable")
        stem = name[:-len(".command.json")]
        need(stem + ".stdout" in raw and stem + ".stderr" in raw and stem + ".result.json" in raw, "Command lost exact stdout/stderr/result evidence")
        result = strict(raw[stem + ".result.json"])
        need(result["schema"] == "frog.bridge.reference_c.process_result" and result["schema_version"] == "0.1" and result["role"] == role and
             result["exit_code"] == 0 and result["command_sha256"] == digest(data) and
             result["stdout_sha256"] == digest(raw[stem + ".stdout"]) and result["stderr_sha256"] == digest(raw[stem + ".stderr"]), "Actual process result/log hashes differ")
        if role == "kernel":
            need(raw[stem + ".stdout"] == values_bytes, "Kernel stdout was replaced or re-encoded before publication")
        roles[role] = {"command": name, "exit_code": result["exit_code"]}
    need(set(roles) == {"cl", "link", "kernel"}, "Qualification did not retain all three actual invocation commands")
    save_json(folder / "verified-manifest.json", {"original_bytes": total, "files": [{"name": k, "sha256": digest(v), "bytes": len(v)} for k, v in raw.items()], "roles": roles})
    return {"files": len(raw), "bytes": total, "roles": roles}


def copy_sdk(sdk, target):
    target.mkdir()
    files = []
    for root in (sdk / "include", sdk / "src", sdk / "schemas"):
        need(root.is_dir(), f"Required public SDK source owner is missing: {root}")
        files.extend(path for path in root.rglob("*") if path.is_file())
    files.extend(path for path in (sdk / "CMakeLists.txt", sdk / "contract.py", sdk / "README.md") if path.is_file())
    files.extend(path for path in sdk.glob("LICENSE*") if path.is_file())
    manifest, total = [], 0
    for path in sorted(set(files)):
        relative = path.relative_to(sdk)
        total += path.stat().st_size
        need(total <= 32*1024*1024 and len(manifest) < 2048, "Public SDK source snapshot exceeded its finite evidence budget")
        out = target / relative
        out.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(path, out)
        manifest.append({"path": relative.as_posix(), "sha256": file_digest(out), "bytes": out.stat().st_size})
    save_json(target / "snapshot-manifest.json", {"schema": "frog.reference-c.sdk-source", "schema_version": "0.1", "files": manifest})
    return digest(encoded(manifest).encode("utf-8"))


def derive(args, schemas, examples, metadata, source_hash):
    source = (examples / metadata["source"]).resolve()
    need(source.is_relative_to(examples) and source.is_file() and file_digest(source) == source_hash, "Corpus Source bytes changed")
    stem = source.stem
    folder = args.output_root / "derived" / stem
    folder.mkdir(parents=True)
    captured = folder / "source.frog"
    shutil.copyfile(source, captured)
    paths = {name: folder / f"{stem}.{name}.json" for name in ("fir", "provenance", "maps", "diagnostics")}
    argv = [args.frontend, "derive", "--source", captured, "--source-uri", metadata["source_uri"],
            "--document-id", metadata["source_document_id"], "--fir-document-id", metadata["fir_document_id"],
            "--source-format", "frog.source", "--source-version", "0.1", "--fir-version", "0.1",
            "--output", paths["fir"], "--provenance-output", paths["provenance"],
            "--source-maps-output", paths["maps"], "--diagnostics-output", paths["diagnostics"]]
    run_frontend(argv, folder)
    fir = paths["fir"].read_bytes()
    provenance_file = paths["provenance"].read_bytes()
    provenance = provenance_file.decode("utf-8").strip(" \t\r\n")
    context = strict(provenance)
    schemas.check("frontend-context-0.3.schema.json", context)
    schemas.check("frontend-source-maps.schema.json", strict(paths["maps"].read_bytes()))
    diagnostics = strict(paths["diagnostics"].read_bytes())
    need(type(diagnostics) is list, "Public Frontend diagnostics must be an array")
    for diagnostic in diagnostics:
        schemas.check("diagnostic-0.2.schema.json", diagnostic)
    need(diagnostics == [], "Public Frontend emitted unexpected diagnostics")
    need(context["closure_complete"] is True and context["source"]["sha256"] == source_hash and
         context["source"]["uri"] == metadata["source_uri"] and context["source"]["document_id"] == metadata["source_document_id"] and
         context["fir"] == {"document_id": metadata["fir_document_id"], "family": "frog.execution_ir", "version": "0.1"}, "Public Frontend changed exact Source/FIR identity")
    artifact = strict(fir)
    need(artifact["schema_family"] == "frog.execution_ir" and artifact["schema_version"] == "0.1" and artifact["document_id"] == metadata["fir_document_id"], "CLI emitted a different FIR format/identity")
    save_json(folder / "captured-identities.json", {"source_sha256": source_hash, "fir_sha256": digest(fir), "provenance_file_sha256": digest(provenance_file),
              "context_token_sha256": digest(provenance.encode("utf-8")), "context_outer_whitespace_removed_only": True,
              "source": context["source"], "fir": context["fir"], "frontend": context["frontend"]})
    return {"fir": fir.decode("utf-8"), "provenance": provenance, "source_sha256": source_hash, "folder": folder}


def graph_fields(graph):
    return {"fir": {"family": "frog.execution_ir", "version": "0.1", "sha256": digest(graph["fir"].encode("utf-8")), "utf8": graph["fir"]},
            "provenance": Raw(graph["provenance"]), "selection": SELECTION}


def activate(args, schemas, graph, case, examples, config_sha):
    folder = args.output_root / "activations" / case["id"]
    folder.mkdir(parents=True)
    for name, hash_key in (("inputs", "inputs_sha256"), ("expected", "expected_sha256")):
        source = (examples / case[name]).resolve()
        need(source.is_relative_to(examples) and file_digest(source) == case[hash_key], "Static corpus inputs/oracle changed")
        shutil.copyfile(source, folder / f"{name}.json")
    session = Session(args, schemas, folder, case["id"])
    try:
        check_description(session)
        admitted = successful(session.exchange("validate", payload("validate", **graph_fields(graph)))[0])["receipt"]
        need(admitted["fir_sha256"] == digest(graph["fir"].encode("utf-8")) and admitted["context_sha256"] == digest(graph["provenance"].encode("utf-8")) and
             all(admitted[k] == v for k, v in SELECTION.items()), "Admission receipt changed exact FIR/context/selection")
        parameters = (folder / "inputs.json").read_text(encoding="utf-8")
        strict(parameters)
        response, raw = session.exchange("run", payload("run", **graph_fields(graph), receipt=admitted, command="start", parameters=Raw(parameters)))
        successful(response)
        values = raw_member(raw_member(raw, "payload"), "values").encode("utf-8")
        actual = folder / "actual-values.json"
        actual.write_bytes(values)
        count = check_outputs(load_output(actual), load_output(folder / "expected.json"), case["id"])
        handle = response["payload"]["run_handle"]
        need(handle["session_id"] == session.identity["session_id"] and handle["generation"] == session.identity["generation"], "Actual run handle changed generation")
        observation = successful(session.exchange("observe", payload("observe", run_handle=handle, parameters={"include_artifacts": True}))[0])
        need(observation["run_handle"] == handle, "Observe substituted the completed run handle")
        evidence = retain_artifacts(observation["observations"], folder / "artifacts", values, config_sha)
        need(observation["observations"]["fir_sha256"] == admitted["fir_sha256"] and observation["observations"]["context_sha256"] == admitted["context_sha256"], "Execution lost admitted graph authority")
        processes = session.latest_job["fields"]["total"]
        baseline = session.described_job["fields"]["total"]
        need(processes >= baseline + 3, "No compiler/linker/kernel job-accounting growth accompanied the three retained actual invocation records")
        stopped = successful(session.exchange("stop", payload("stop", run_handle=handle))[0])
        need(stopped["run_handle"] == handle, "Stop substituted the current completed handle")
        result = {"id": case["id"], "status": "passed", "outcome": response["payload"]["values"]["outcome"],
                  "checked_outputs": count, "actual_values_sha256": digest(values), "job_total_processes": processes,
                  "job_post_describe_total_processes": baseline, "job_total_processes_growth_after_describe": processes-baseline,
                  "active_process_ids": session.latest_job["active_process_ids"], "artifacts": evidence}
        save_json(folder / "result.json", result)
        return result
    finally:
        session.close()


def reject(args, schemas, graph, case):
    folder = args.output_root / "refusals" / case["id"]
    folder.mkdir(parents=True)
    session = Session(args, schemas, folder, case["id"])
    try:
        check_description(session)
        after_describe = session.latest_job
        response, _ = session.exchange("validate", payload("validate", **graph_fields(graph)))
        need(response["status"] == "error" and response["payload"] == {}, "Unsupported graph obtained a receipt or success status")
        expected = case["expected_diagnostic"]
        diagnostic = next((d for d in response["diagnostics"] if d["code"] == expected["code"]), None)
        need(diagnostic and diagnostic["severity"] == "error" and diagnostic["source"]["object_id"] == expected["object_id"] and
             diagnostic["source"]["pointer"] == expected["source_pointer"] and diagnostic["source"]["uri"] == case["source_uri"], "Finite refusal lost its exact code/FIR object/Source pointer")
        after_validate = session.latest_job
        baseline = after_describe["fields"]["total"]
        processes = after_validate["fields"]["total"]
        need(processes == baseline, "Unsupported graph caused Job TotalProcesses growth after Describe; inspect retained raw accounting")
        need(process_identities(after_validate) == session.described_identities and after_validate["fields"]["active"] == len(session.described_identities),
             "Unsupported graph changed an active PID/image/creation identity after Describe")
        result = {"id": case["id"], "status": "passed", "wire_status": "error", "diagnostic": diagnostic,
                  "job_total_processes": processes, "job_post_describe_total_processes": baseline, "job_total_processes_growth_after_describe": 0,
                  "startup_console_growth": session.startup_console_growth,
                  "active_process_ids": after_validate["active_process_ids"], "compiler_started": False, "fallback_used": False}
        save_json(folder / "result.json", result)
        return result
    finally:
        session.close()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--frontend", type=Path, required=True, help="Explicit standalone public Frontend CLI executable")
    parser.add_argument("--peer", type=Path, required=True, help="Explicit standalone installed reference C peer executable")
    parser.add_argument("--compiler-config", required=True, help="Safe path relative to that peer executable")
    parser.add_argument("--sdk-root", type=Path, required=True, help="Public SDK ecosystem source root containing schemas")
    parser.add_argument("--output-root", type=Path, required=True, help="New durable evidence directory")
    parser.add_argument("--corpus", type=Path, default=Path(__file__).resolve().parents[1] / "examples" / "corpus.json")
    args = parser.parse_args()
    need(os.name == "nt", "This finite MSVC qualification requires Windows")
    for name in ("frontend", "peer", "sdk_root", "output_root", "corpus"):
        path = getattr(args, name)
        if name in ("frontend", "peer"):
            need(path.is_absolute(), f"--{name} must be an explicit absolute executable path")
        setattr(args, name, path.resolve())
    need(args.frontend.is_file() and args.peer.is_file(), "Explicit Frontend/peer executable is unavailable")
    relative = PureWindowsPath(args.compiler_config)
    need(not relative.is_absolute() and not relative.drive and ".." not in relative.parts and relative.parts and
         not any(":" in part for part in relative.parts), "Compiler configuration must be safe and relative to the installed peer")
    config = (args.peer.parent / args.compiler_config).resolve()
    need(config.is_relative_to(args.peer.parent), "Compiler configuration escaped installed peer directory")
    need(not args.output_root.exists(), "Evidence directory must be new; existing proof is never overwritten")
    args.output_root.mkdir(parents=True)
    report = {"schema": "frog.reference-c.public-corpus", "schema_version": "0.1", "status": "running", "sources": [], "activations": [], "refusals": [],
              "public_only": True, "uses_studio_runtime_llvm_imports": False, "graph_evaluator": False}
    try:
        schemas = Schemas(args.sdk_root)
        report["sdk_source_snapshot_sha256"] = copy_sdk(args.sdk_root, args.output_root / "sdk-source")
        shutil.copyfile(Path(__file__), args.output_root / "run_public_corpus.py")
        shutil.copyfile(Path(__file__).with_name("check_outputs.py"), args.output_root / "check_outputs.py")
        shutil.copyfile(args.corpus, args.output_root / "corpus.json")
        shutil.copyfile(config, args.output_root / "compiler-config.json")
        config_sha = file_digest(config)
        report["executables"] = {"frontend": {"path": str(args.frontend), "sha256": file_digest(args.frontend)},
                                 "peer": {"path": str(args.peer), "sha256": file_digest(args.peer)}}
        report["compiler_config_sha256"] = config_sha
        report["python"] = {"executable": sys.executable, "version": sys.version}
        corpus = strict(args.corpus.read_bytes())
        need(corpus["schema"] == "frog.reference-c.corpus" and corpus["schema_version"] == "0.1" and
             len(corpus["sources"]) == 6 and len(corpus["cases"]) == 12 and len(corpus["negative_sources"]) == 2, "The bounded public corpus changed")
        ids = [row["id"] for row in corpus["cases"] + corpus["negative_sources"]]
        need(len(set(ids)) == 14 and all(re.fullmatch(r"[a-z0-9][a-z0-9-]{0,95}", name) for name in ids), "Unsafe or repeated bounded corpus identity")
        examples, graphs = args.corpus.parent, {}
        for source in corpus["sources"]:
            metadata = next(c for c in corpus["cases"] if c["source"] == source["path"])
            graph = derive(args, schemas, examples, metadata, source["sha256"])
            need(digest(graph["fir"].encode("utf-8")) == source["probe_fir_sha256"], "Fresh public CLI FIR differs from the frozen common corpus artifact")
            graphs[source["path"]] = graph
            report["sources"].append({"source": source["path"], "sha256": source["sha256"], "fir_sha256": source["probe_fir_sha256"]})
        for case in corpus["negative_sources"]:
            graph = derive(args, schemas, examples, case, case["source_sha256"])
            need(digest(graph["fir"].encode("utf-8")) == case["probe_fir_sha256"], "Fresh negative FIR differs from the actual public Frontend corpus")
            graphs[case["source"]] = graph
            report["sources"].append({"source": case["source"], "sha256": case["source_sha256"], "fir_sha256": case["probe_fir_sha256"]})
        for case in corpus["cases"]:
            report["activations"].append(activate(args, schemas, graphs[case["source"]], case, examples, config_sha))
            save_json(args.output_root / "report.json", report)
        for case in corpus["negative_sources"]:
            report["refusals"].append(reject(args, schemas, graphs[case["source"]], case))
            save_json(args.output_root / "report.json", report)
        need(sum(row["outcome"] == "completed" for row in report["activations"]) == 10 and
             sum(row["outcome"] == "fault" for row in report["activations"]) == 2, "Actual finite corpus outcomes changed")
        report["status"] = "passed"
        report["counts"] = {"sources": 8, "activations": 12, "completed": 10, "runtime_faults": 2, "localized_admission_refusals": 2}
        save_json(args.output_root / "report.json", report)
        print(encoded({"status": "passed", "report": str(args.output_root / "report.json"), **report["counts"]}))
        return 0
    except BaseException as error:
        report["status"], report["error"] = "failed", f"{type(error).__name__}: {error}"
        save_json(args.output_root / "report.json", report)
        raise


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (Exception, KeyboardInterrupt) as error:
        print(f"PUBLIC_CORPUS_FAIL {type(error).__name__}: {error}", file=sys.stderr)
        sys.exit(1)
