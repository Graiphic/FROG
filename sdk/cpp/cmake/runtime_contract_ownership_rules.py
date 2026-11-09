"""Narrow ownership exceptions; discovery and original CMake reads stay unchanged.

The Frontend snapshot must occupy its declared path and match both its source
origin and canonical SDK owner. Only the bridge owner's qualified Diagnostic
definition is distinct from the Runtime contract; no file is exempted wholesale.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import stat
import sys
from runtime_contract_source_inventory import io_path


SNAPSHOT = "Implementations/Frontend/snapshot/sdk/include/frog/sdk/runtime_contract.hpp"
ORIGIN = "Implementations/Frontend/source-origin.json"
CANONICAL = "sdk/cpp/include/frog/sdk/runtime_contract.hpp"
BRIDGE = "sdk/ecosystem/include/frog/bridge.hpp"
DECLARATION = re.compile(r"(struct|class)[ \t\r\n]+Diagnostic[ \t\r\n]*\{")
# Preserve offsets while ignoring comments and ordinary/raw string literals.
TOKENS = re.compile(
    r'^[ \t]*\#(?:\\\r?\n|[^\n])*|//[^\n]*|/\*[\s\S]*?\*/|(?:u8|u|U|L)?R"([^ ()\\\t\r\n]{0,16})\([\s\S]*?\)\1"'
    r'|(?:u8|u|U|L)?"(?:\\[\s\S]|[^"\\])*"|\'(?:\\[\s\S]|[^\'\\])*\''
    r'|[A-Za-z_]\w*|::|[^\s]', re.MULTILINE)


def require(condition, message):
    if not condition:
        raise ValueError(message)


def same_path(left, right):
    return os.path.normcase(os.path.abspath(left)) == os.path.normcase(os.path.abspath(right))


def read(path, limit=4 * 1024 * 1024):
    with open(io_path(path), "rb") as stream:
        data = stream.read(limit + 1)
    require(len(data) <= limit, "ownership input exceeds its bounded size: " + str(path))
    return data


def no_redirect(root, relative):
    path = root
    for part in Path(relative).parts:
        path = path / part
        info = os.lstat(io_path(path))
        # Cloud placeholders are not directory aliases. Reject actual symlinks
        # and mount-point/junction redirects in an exception's fixed path.
        require(not stat.S_ISLNK(info.st_mode) and
                getattr(info, "st_reparse_tag", 0) not in (0xA0000003, 0xA000000C),
                "ownership exception path is redirected: " + str(path))


def unique(pairs):
    result = {}
    for key, value in pairs:
        require(key not in result, "duplicate source-origin member: " + key)
        result[key] = value
    return result


def snapshot(root, source, canonical):
    require(same_path(source, root / SNAPSHOT), "not the declared Frontend snapshot path")
    require(same_path(canonical, root / CANONICAL), "snapshot upstream is not the selected canonical SDK owner")
    for relative in (SNAPSHOT, ORIGIN, CANONICAL):
        no_redirect(root, relative)
    origin = json.loads(read(root / ORIGIN).decode("utf-8"), object_pairs_hook=unique)
    require(isinstance(origin, dict) and origin.get("format") == "frog.frontend.source-origin" and
            type(origin.get("version")) is int and origin["version"] == 1 and
            isinstance(origin.get("sources"), list), "unsupported Frontend source-origin manifest")
    declared = "snapshot/sdk/include/frog/sdk/runtime_contract.hpp"
    entries, seen = [], set()
    for entry in origin["sources"]:
        require(isinstance(entry, dict) and isinstance(entry.get("path"), str), "malformed source-origin entry")
        name = entry["path"]
        require(name.casefold() not in seen, "duplicate source-origin path: " + name)
        seen.add(name.casefold())
        if name.casefold() == declared.casefold():
            entries.append(entry)
    require(len(entries) == 1, "missing or ambiguous declared Runtime snapshot")
    entry = entries[0]
    require(entry["path"] == declared and entry.get("upstream") == "FROG/" + CANONICAL and
            entry.get("ownership") == "snapshot", "inconsistent Runtime snapshot provenance")
    digest, size = entry.get("sha256"), entry.get("bytes")
    require(isinstance(digest, str) and re.fullmatch(r"[0-9a-f]{64}", digest) and
            type(size) is int and 0 < size <= 1024 * 1024, "invalid Runtime snapshot fingerprint or byte count")
    for path in (source, canonical):
        data = read(path, 1024 * 1024)
        require(len(data) == size and hashlib.sha256(data).hexdigest() == digest,
                "Runtime snapshot/canonical bytes differ from declared provenance: " + str(path))


def bridge_diagnostic(root, source):
    require(same_path(source, root / BRIDGE), "not the public bridge Diagnostic owner")
    no_redirect(root, BRIDGE)
    text = read(source).decode("utf-8-sig")
    # This lexical proof does not run a preprocessor. Fail closed rather than
    # infer namespace ownership through conditionals or macro-generated scopes.
    directives = re.findall(r"^[ \t]*#[ \t]*([A-Za-z_]\w*)", text, re.MULTILINE)
    require(all(directive in ("include", "pragma") for directive in directives),
            "bridge namespace ownership requires unambiguous nonconditional declarations")
    tokens = [(m.group(), m.start()) for m in TOKENS.finditer(text)
              if m.group() not in ("",) and not m.group().lstrip().startswith(("#", "//", "/*")) and
              not ('"' in m.group() or "'" in m.group())]
    scopes, braces, allowed = [], {}, set()
    # Mark namespace bodies before walking all braces. Generic bodies prevent
    # a local or nested class named Diagnostic from acquiring namespace scope.
    for i, (token, _) in enumerate(tokens):
        if token != "namespace":
            continue
        names, j = [], i + 1
        while j < len(tokens) and (re.fullmatch(r"[A-Za-z_]\w*", tokens[j][0]) or tokens[j][0] == "::"):
            names.append(tokens[j][0]); j += 1
        if j < len(tokens) and tokens[j][0] == "{" and "=" not in names:
            braces[j] = tuple(name for name in names if name != "::") or ("<anonymous>",)
    for i, (token, offset) in enumerate(tokens):
        if token in ("struct", "class") and i + 2 < len(tokens) and tokens[i+1][0] == "Diagnostic" and tokens[i+2][0] == "{":
            if tuple(name for scope in scopes for name in scope) == ("frog", "bridge"):
                allowed.add(offset)
        if token == "{":
            scopes.append(braces.get(i, ("<body>",)))
        elif token == "}":
            require(scopes, "unbalanced bridge declaration scopes")
            scopes.pop()
    require(not scopes, "unbalanced bridge declaration scopes")
    matches = list(DECLARATION.finditer(text))
    require(matches and all(match.start() in allowed for match in matches),
            "Diagnostic definition is outside frog::bridge namespace scope")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("mode", choices=("snapshot", "bridge-diagnostic"))
    parser.add_argument("--scan-root", type=Path, required=True)
    parser.add_argument("--source", type=Path, required=True)
    parser.add_argument("--canonical-header", type=Path)
    args = parser.parse_args()
    if args.mode == "snapshot":
        require(args.canonical_header is not None, "explicit canonical owner required")
        snapshot(args.scan_root, args.source, args.canonical_header)
    else:
        bridge_diagnostic(args.scan_root, args.source)
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, ValueError, RuntimeError) as error:
        print(str(error), file=sys.stderr)
        raise SystemExit(2)
