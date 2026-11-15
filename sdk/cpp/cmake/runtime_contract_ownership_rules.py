"""Narrow ownership exceptions; discovery and original CMake reads stay unchanged.

The Frontend snapshot must occupy its declared path and match both its source
origin and canonical SDK owner. Qualified Diagnostic declarations can belong to
the bridge or an explicitly declared private owner; no file is exempted wholesale.
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
DIAGNOSTIC_POLICY_FORMAT = "frog.runtime-contract.diagnostic-owners"
DIAGNOSTIC_POLICY_BYTES = 64 * 1024
DIAGNOSTIC_POLICY_OWNERS = 64
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


def diagnostic_scope(text, namespace):
    # This lexical proof does not run a preprocessor. Fail closed rather than
    # infer namespace ownership through conditionals or macro-generated scopes.
    directives = re.findall(r"^[ \t]*#[ \t]*([A-Za-z_]\w*)", text, re.MULTILINE)
    require(all(directive in ("include", "pragma") for directive in directives),
            "Diagnostic namespace ownership requires unambiguous nonconditional declarations")
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
        require(j >= len(tokens) or tokens[j][0] != "=",
                "Diagnostic namespace ownership cannot infer namespace aliases")
        if j < len(tokens) and tokens[j][0] == "{" and "=" not in names:
            braces[j] = tuple(name for name in names if name != "::") or ("<anonymous>",)
    for i, (token, offset) in enumerate(tokens):
        if token in ("struct", "class"):
            # A qualified, inherited or final definition cannot borrow the
            # permission of another plain Diagnostic declaration in the file.
            j, names = i + 1, []
            if j < len(tokens) and tokens[j][0] == "::":
                j += 1
            if j < len(tokens) and re.fullmatch(r"[A-Za-z_]\w*", tokens[j][0]):
                names.append(tokens[j][0]); j += 1
                while j + 1 < len(tokens) and tokens[j][0] == "::" and re.fullmatch(r"[A-Za-z_]\w*", tokens[j+1][0]):
                    names.append(tokens[j+1][0]); j += 2
            if names and names[-1] == "Diagnostic" and j < len(tokens) and tokens[j][0] in ("{", ":", "final"):
                require(j == i + 2 and tokens[j][0] == "{",
                        "Diagnostic definition requires a plain declaration at its exact namespace scope")
        if token in ("struct", "class") and i + 2 < len(tokens) and tokens[i+1][0] == "Diagnostic" and tokens[i+2][0] == "{":
            if tuple(name for scope in scopes for name in scope) == namespace:
                allowed.add(offset)
        if token == "{":
            scopes.append(braces.get(i, ("<body>",)))
        elif token == "}":
            require(scopes, "unbalanced Diagnostic declaration scopes")
            scopes.pop()
    require(not scopes, "unbalanced Diagnostic declaration scopes")
    matches = list(DECLARATION.finditer(text))
    require(matches and all(match.start() in allowed for match in matches),
            "Diagnostic definition is outside " + "::".join(namespace) + " namespace scope")


def bridge_diagnostic(root, source):
    require(same_path(source, root / BRIDGE), "not the public bridge Diagnostic owner")
    no_redirect(root, BRIDGE)
    diagnostic_scope(read(source).decode("utf-8-sig"), ("frog", "bridge"))


def relative_policy_path(value):
    require(isinstance(value, str) and 0 < len(value.encode("utf-8")) <= 1024 and
            not any(character in value for character in ("\\", ":", "\x00")) and
            all(ord(character) >= 32 and ord(character) != 127 for character in value) and
            all(part not in ("", ".", "..") for part in value.split("/")),
            "Diagnostic owner path must be an exact bounded relative POSIX source path")
    return value


def diagnostic_policy(root, policy):
    root, policy = Path(os.path.abspath(root)), Path(os.path.abspath(policy))
    try:
        relative = policy.relative_to(root).as_posix()
    except ValueError:
        raise ValueError("Diagnostic ownership policy must be inside scan root") from None
    no_redirect(root, relative_policy_path(relative))
    require(stat.S_ISREG(os.stat(io_path(policy)).st_mode), "Diagnostic ownership policy must be a regular file")
    data = json.loads(read(policy, DIAGNOSTIC_POLICY_BYTES).decode("utf-8-sig"), object_pairs_hook=unique)
    require(isinstance(data, dict) and set(data) == {"format", "version", "owners"} and
            data["format"] == DIAGNOSTIC_POLICY_FORMAT and type(data["version"]) is int and data["version"] == 1 and
            isinstance(data["owners"], list) and len(data["owners"]) <= DIAGNOSTIC_POLICY_OWNERS,
            "unsupported Diagnostic ownership policy schema")
    owners, seen_paths, seen_namespaces = {}, set(), set()
    for entry in data["owners"]:
        require(isinstance(entry, dict) and set(entry) == {"path", "namespace"}, "malformed Diagnostic owner declaration")
        path = relative_policy_path(entry["path"])
        qualified = entry["namespace"]
        require(isinstance(qualified, str) and 0 < len(qualified.encode("utf-8")) <= 1024 and
                re.fullmatch(r"[A-Za-z_]\w*(?:::[A-Za-z_]\w*)+", qualified, re.ASCII),
                "Diagnostic owner namespace must be an exact qualified namespace")
        namespace = tuple(qualified.split("::"))
        require(namespace[:3] != ("frog", "sdk", "runtime"), "SDK Runtime namespace cannot be a declared Diagnostic owner")
        require(namespace != ("frog", "bridge") or path == BRIDGE,
                "the public bridge Diagnostic retains its fixed owner path")
        require(path.casefold() not in seen_paths, "duplicate Diagnostic owner path: " + path)
        require(namespace not in seen_namespaces, "duplicate Diagnostic owner namespace: " + qualified)
        seen_paths.add(path.casefold()); seen_namespaces.add(namespace)
        no_redirect(root, path)
        source = root / path
        require(stat.S_ISREG(os.stat(io_path(source)).st_mode), "Diagnostic owner must be a regular source file")
        diagnostic_scope(read(source).decode("utf-8-sig"), namespace)
        owners[path] = namespace
    return owners


def declared_diagnostic(root, source, policy):
    owners = diagnostic_policy(root, policy)
    try:
        relative = Path(os.path.abspath(source)).relative_to(Path(os.path.abspath(root))).as_posix()
    except ValueError:
        raise ValueError("Diagnostic source is outside scan root") from None
    require(relative in owners, "not an exact declared Diagnostic owner path")
    no_redirect(root, relative)
    diagnostic_scope(read(source).decode("utf-8-sig"), owners[relative])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("mode", choices=("snapshot", "bridge-diagnostic", "diagnostic-policy", "declared-diagnostic"))
    parser.add_argument("--scan-root", type=Path, required=True)
    parser.add_argument("--source", type=Path)
    parser.add_argument("--canonical-header", type=Path)
    parser.add_argument("--policy", type=Path)
    args = parser.parse_args()
    if args.mode == "snapshot":
        require(args.source is not None and args.canonical_header is not None, "explicit source and canonical owner required")
        snapshot(args.scan_root, args.source, args.canonical_header)
    elif args.mode == "bridge-diagnostic":
        require(args.source is not None, "explicit bridge source required")
        bridge_diagnostic(args.scan_root, args.source)
    elif args.mode == "diagnostic-policy":
        require(args.policy is not None, "explicit Diagnostic ownership policy required")
        diagnostic_policy(args.scan_root, args.policy)
    else:
        require(args.source is not None and args.policy is not None, "explicit source and Diagnostic ownership policy required")
        declared_diagnostic(args.scan_root, args.source, args.policy)
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, ValueError, RuntimeError) as error:
        print(str(error), file=sys.stderr)
        raise SystemExit(2)
