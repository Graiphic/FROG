"""Private discovery and optional original-source prefetch; no contract validation.

OS stat supplies strict volume/file identities, and extended Windows paths allow
real Cloud/junction/long-path IO. Lexical exclusions and physical identities stay
separate. Prefetch reads originals through the very same read_path, discards the
bytes, and retains no content or file copies. Only CMake validates the originals.
"""
import argparse
from concurrent.futures import FIRST_COMPLETED, ThreadPoolExecutor, wait
import json
import os
from pathlib import Path
import re
import sys
import stat
import time
import threading


EXCLUDED_DIRECTORY = re.compile(r"/(build[^/]*)/|/third_party/|/\.git/")
SOURCE_EXTENSION = re.compile(r"\.(h|hpp|c|cc|cpp|cxx)$")


class DiscoveryDiagnostics:
    """Optional real phase timings; stdout inventory and scan admission stay exact."""
    def __init__(self):
        self.started = time.perf_counter_ns()
        self.last_report = self.started
        self.counts = {"directories": 0, "sources": 0, "excluded": 0, "revisited": 0}
        self.nanoseconds = {"identity": 0, "listing_sort": 0, "classification": 0, "record_sort": 0, "prefetch": 0, "serialization": 0}
        self.current = ""
        self.report("discovery_begin", force=True)

    def timed(self, phase, operation, *args, **kwargs):
        started = time.perf_counter_ns()
        try:
            return operation(*args, **kwargs)
        finally:
            self.nanoseconds[phase] += time.perf_counter_ns() - started

    def report(self, phase, force=False):
        now = time.perf_counter_ns()
        if not force and now - self.last_report < 2_000_000_000:
            return
        self.last_report = now
        record = {"phase": phase, "elapsed_ms": (now - self.started) // 1_000_000,
                  **self.counts,
                  "milliseconds": {key: value // 1_000_000 for key, value in self.nanoseconds.items()},
                  "current": self.current[-384:]}
        print("OWNERSHIP_DISCOVERY_DIAGNOSTIC " + json.dumps(record, ensure_ascii=True, separators=(",", ":")),
              file=sys.stderr, flush=True)


def io_path(path):
    native = os.path.abspath(path)
    if os.name != "nt" or native.startswith("\\\\?\\"):
        return native
    if native.startswith("\\\\"):
        return "\\\\?\\UNC\\" + native[2:]
    return "\\\\?\\" + native


def physical_key(path):
    # OS stat follows real junctions and Cloud entries without needing the
    # directory access demanded by GetFinalPathName/Path.resolve. These are
    # actual volume/file identities, never an incomplete lexical fallback.
    information = os.stat(io_path(path), follow_symlinks=True)
    if not information.st_dev or not information.st_ino:
        raise RuntimeError("Physical volume/file identity is unavailable for " + str(path))
    return str(information.st_dev) + ":" + str(information.st_ino)


def discover(scan_root, canonical_header, diagnostics=None):
    lexical_root = Path(os.path.abspath(scan_root))
    canonical = Path(os.path.abspath(canonical_header))
    canonical_physical = (diagnostics.timed("identity", physical_key, canonical)
                          if diagnostics is not None else physical_key(canonical))
    if not stat.S_ISREG(os.stat(io_path(canonical), follow_symlinks=True).st_mode) or not stat.S_ISDIR(os.stat(io_path(lexical_root), follow_symlinks=True).st_mode):
        raise ValueError("The explicit scan root and canonical header must exist")
    pending = [lexical_root]
    visited = set()
    directories = []
    sources = []
    while pending:
        directory = pending.pop()
        lexical = directory.as_posix()
        # The slash is part of the historical directory-boundary regex. Do
        # this before resolution and before reserving a visited physical key.
        if EXCLUDED_DIRECTORY.search(lexical.rstrip("/") + "/"):
            if diagnostics is not None:
                diagnostics.counts["excluded"] += 1
            continue
        if diagnostics is not None:
            diagnostics.current = lexical
            diagnostics.report("discovery")
            physical = diagnostics.timed("identity", physical_key, directory)
        else:
            physical = physical_key(directory)
        if physical in visited:
            if diagnostics is not None:
                diagnostics.counts["revisited"] += 1
            continue
        visited.add(physical)
        directories.append({"kind": "directory", "lexical": lexical, "physical": physical})
        if diagnostics is not None:
            diagnostics.counts["directories"] += 1
            listing_started = time.perf_counter_ns()
        with os.scandir(io_path(directory)) as iterator:
            entries = sorted(iterator, key=lambda entry: entry.name)
        if diagnostics is not None:
            diagnostics.nanoseconds["listing_sort"] += time.perf_counter_ns() - listing_started
        for entry in entries:
            path = directory / entry.name
            # Follow real directory aliases, including junctions and cloud
            # reparse directories. Do not classify ReparsePoint as exclusion.
            is_directory = (diagnostics.timed("classification", entry.is_dir, follow_symlinks=True)
                            if diagnostics is not None else entry.is_dir(follow_symlinks=True))
            if is_directory:
                pending.append(path)
                continue
            extension_name = entry.name.lower() if os.name == "nt" or sys.platform == "darwin" else entry.name
            if SOURCE_EXTENSION.search(extension_name):
                physical = (diagnostics.timed("identity", physical_key, path)
                            if diagnostics is not None else physical_key(path))
                sources.append({"kind": "source", "lexical": path.as_posix(), "physical": physical, "read_path": io_path(path)})
                if diagnostics is not None:
                    diagnostics.counts["sources"] += 1
                    diagnostics.report("discovery")
    if diagnostics is not None:
        record_sort_started = time.perf_counter_ns()
    ordered_directories = sorted(directories, key=lambda record: record["lexical"])
    ordered_sources = sorted(sources, key=lambda record: record["lexical"])
    if diagnostics is not None:
        diagnostics.nanoseconds["record_sort"] += time.perf_counter_ns() - record_sort_started
        diagnostics.report("discovery_complete", force=True)
    yield {"kind": "canonical", "lexical": canonical.as_posix(), "physical": canonical_physical}
    yield from ordered_directories
    # File aliases are intentionally not deduplicated. Their lexical basenames
    # still determine the copied-header assertion in the CMake owner checker.
    yield from ordered_sources



PREFETCH_READERS = 8
PREFETCH_BUFFER_BYTES = 64 * 1024
PREFETCH_OUTSTANDING = 16


def prefetch_original_sources(records, diagnostics=False):
    # These are the existing genuine source records, never materialized copies
    # or content used for ownership decisions. Alias records remain distinct.
    sources = iter(record for record in records if record["kind"] == "source")
    stopped = threading.Event()
    counters_lock = threading.Lock()
    reader_local = threading.local()
    counters = {"active": 0, "active_peak": 0, "outstanding_peak": 0,
                "opened_sources": 0, "completed_sources": 0, "bytes": 0,
                "read_calls": 0, "read_ns_sum": 0, "current": ""}
    started = time.perf_counter_ns()
    last_report = started

    def report(phase, force=False):
        nonlocal last_report
        if not diagnostics:
            return
        now = time.perf_counter_ns()
        if not force and now - last_report < 2_000_000_000:
            return
        last_report = now
        with counters_lock:
            record = dict(counters)
        record["current"] = record["current"][-384:]
        record.update({"phase": phase, "elapsed_ms": (now - started) // 1_000_000,
                       "read_ms_sum": record.pop("read_ns_sum") // 1_000_000,
                       "maximum_readers": PREFETCH_READERS, "buffer_bytes": PREFETCH_BUFFER_BYTES,
                       "outstanding_limit": PREFETCH_OUTSTANDING})
        print("OWNERSHIP_PREFETCH_DIAGNOSTIC " + json.dumps(record, ensure_ascii=True, separators=(",", ":")),
              file=sys.stderr, flush=True)

    def read_original(record):
        # One reusable 64KiB buffer per actual reader, at most eight total.
        if not hasattr(reader_local, "buffer"):
            reader_local.buffer = bytearray(PREFETCH_BUFFER_BYTES)
        count_bytes = 0
        read_calls = 0
        completed = False
        read_started = time.perf_counter_ns()
        with counters_lock:
            counters["active"] += 1
            counters["active_peak"] = max(counters["active_peak"], counters["active"])
            counters["current"] = record["lexical"]
        try:
            if stopped.is_set():
                return
            # Unbuffered original-file IO prevents a second hidden content
            # buffer. No decoding, parsing, hashing or validation of bytes.
            with open(record["read_path"], "rb", buffering=0) as stream:
                with counters_lock:
                    counters["opened_sources"] += 1
                while not stopped.is_set():
                    count = stream.readinto(reader_local.buffer)
                    read_calls += 1
                    if count is None:
                        raise OSError("Blocking original-file read returned no byte count")
                    if count == 0:
                        completed = True
                        break
                    count_bytes += count
        except OSError as error:
            # Preserve the actual lexical source and original OS exception;
            # failed prefetch never emits a partial accepted inventory.
            raise RuntimeError("Original-source prefetch failed for " + record["lexical"] + ": " + str(error)) from error
        finally:
            with counters_lock:
                counters["active"] -= 1
                counters["completed_sources"] += int(completed)
                counters["bytes"] += count_bytes
                counters["read_calls"] += read_calls
                counters["read_ns_sum"] += time.perf_counter_ns() - read_started

    report("prefetch_begin", force=True)
    # Submission is bounded as well as execution: at most sixteen outstanding
    # futures including running readers, never one Future for each source.
    pending = set()
    exhausted = False
    with ThreadPoolExecutor(max_workers=PREFETCH_READERS, thread_name_prefix="frog-ownership-prefetch") as executor:
        try:
            while not exhausted or pending:
                while not exhausted and len(pending) < PREFETCH_OUTSTANDING:
                    record = next(sources, None)
                    if record is None:
                        exhausted = True
                        break
                    pending.add(executor.submit(read_original, record))
                    with counters_lock:
                        counters["outstanding_peak"] = max(counters["outstanding_peak"], len(pending))
                if not pending:
                    break
                done, _ = wait(pending, timeout=0.25, return_when=FIRST_COMPLETED)
                for future in done:
                    future.result()  # Original IO errors are never ignored.
                    pending.remove(future)
                report("prefetch")
        except BaseException:
            stopped.set()
            for future in pending:
                future.cancel()
            # Context shutdown joins only the at-most-eight active readers;
            # they stop between bounded reads and all original handles close.
            raise
    report("prefetch_complete", force=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--scan-root", type=Path, required=True)
    parser.add_argument("--canonical-header", type=Path, required=True)
    parser.add_argument("--diagnostics", action="store_true", help="Emit bounded phase timings on stderr only")
    parser.add_argument("--prefetch", action="store_true", help="Warm original-file IO with bounded readers; never validate contents")
    args = parser.parse_args()
    if sys.version_info < (3, 9):
        raise RuntimeError("The private ownership audit requires the explicitly selected Python 3.9+ interpreter")
    diagnostics = DiscoveryDiagnostics() if args.diagnostics else None
    records = list(discover(args.scan_root, args.canonical_header, diagnostics))
    if args.prefetch:
        prefetch_started = time.perf_counter_ns()
        prefetch_original_sources(records, diagnostics is not None)
        if diagnostics is not None:
            diagnostics.nanoseconds["prefetch"] += time.perf_counter_ns() - prefetch_started
    if diagnostics is not None:
        serialization_started = time.perf_counter_ns()
    # Each record is one JSON line, so CMake parses small records in linear
    # aggregate work. Encode list metacharacters as JSON Unicode escapes before
    # CMake splits lines; decoded source paths remain their exact lexical data.
    for record in records:
        encoded = json.dumps(record, ensure_ascii=True, separators=(",", ":"))
        print(encoded.replace(";", "\\u003b").replace("[", "\\u005b").replace("]", "\\u005d"))
    if diagnostics is not None:
        diagnostics.nanoseconds["serialization"] += time.perf_counter_ns() - serialization_started
        diagnostics.report("inventory_complete", force=True)
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, ValueError, RuntimeError) as error:
        print("Ownership source discovery/prefetch failed: " + str(error), file=sys.stderr)
        raise SystemExit(2)
