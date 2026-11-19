"""Build the redistributable source-document fixture with the existing codec.

Run from a FROG checkout with its SDK Python dependencies. No Studio, compiler,
installation, semantic adapter or user archive is invoked by this producer.
"""
import argparse
import hashlib
import json
from pathlib import Path
import tempfile
import sys

if __package__ in (None, ""):
    sys.path.insert(0, str(Path(__file__).resolve().parents[4]))

from sdk.ecosystem.archive import build_archive
from sdk.ecosystem.contract import ROOT as SDK_ROOT
from sdk.ecosystem.library_resolution import LocalLibraryArchive
from sdk.ecosystem.source_library import capture_source_library


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--origin", default="local:graiphic-examples")
    parser.add_argument("--consumer", type=Path)
    args = parser.parse_args()
    if args.consumer is not None and (not args.consumer.parent.is_dir() or args.consumer.exists()):
        parser.error("The consumer must be a new file in an existing explicit output directory.")
    output = args.output.resolve()
    if not output.parent.is_dir():
        parser.error("The explicit output parent directory must already exist.")
    source = (Path(__file__).parent / "src/Addition.frog").read_bytes()
    license_bytes = (SDK_ROOT.parents[1] / "LICENSE").read_bytes()
    digest = hashlib.sha256(source).hexdigest()
    manifest = {"schema": "graiphic.distribution", "format_version": "0.1", "profile": "library",
                "identity": {"publisher": "org.graiphic.examples", "package": "source-addition", "version": "1.0.0"},
                "content_mode": "source",
                "files": [{"path": "src/Addition.frog", "sha256": digest, "size": len(source), "role": "source"},
                          {"path": "LICENSE", "sha256": hashlib.sha256(license_bytes).hexdigest(), "size": len(license_bytes), "role": "license"}],
                "contributions": [{"id": "addition", "kind": "export", "payload": "src/Addition.frog",
                                   "contract": {"path": "src/Addition.frog", "sha256": digest, "symbol": "addition.entry"}}],
                "dependencies": [], "license": {"expression": "Apache-2.0", "path": "LICENSE"}, "required_rights": ["build"]}
    # The codec still assembles the final ZIP on the output volume. Source and
    # verification scratch belong in the OS temporary area, outside synced
    # author folders; cleanup must complete before a success receipt is printed.
    with tempfile.TemporaryDirectory(prefix="frog-source-library-") as temp:
        root = Path(temp)
        (root / "src").mkdir()
        (root / "src/Addition.frog").write_bytes(source)
        (root / "LICENSE").write_bytes(license_bytes)
        descriptor = root / "manifest.json"
        descriptor.write_text(json.dumps(manifest, indent=2, sort_keys=True) + "\n", encoding="utf-8")
        build_archive(descriptor, root, output)
        with capture_source_library(LocalLibraryArchive(output, args.origin), root) as captured:
            if args.consumer is not None:
                # Reuse the actual Studio-authored connector/ports and wires.
                # This only binds its explicit call reference to this package;
                # no interface inference, FROG validation or FIR is duplicated.
                template = json.loads((Path(__file__).parent / "consumer-template.frog").read_text(encoding="utf-8"))
                calls = [x for x in template["diagram"]["nodes"] if x.get("kind") == "frog_call"]
                if len(calls) != 1:
                    raise ValueError("The authored consumer must retain its single callable entry.")
                calls[0]["call"]["document_path"] = captured.exports[0].reference.encode()
                with args.consumer.open("x", encoding="utf-8") as stream:
                    stream.write(json.dumps(template, ensure_ascii=False, indent=2) + "\n")
            result = {"archive": str(output), "sha256": hashlib.sha256(output.read_bytes()).hexdigest(),
                      "manifest_sha256": captured.manifest_sha256,
                      "reference": captured.exports[0].reference.encode(),
                      "source_format": "frog.document.draft", "source_version": "2",
                      "installed": False, "frontend_invoked": False, "execution_qualified": False}
            if args.consumer is not None:
                result["consumer"] = str(args.consumer.resolve())
                result["consumer_sha256"] = hashlib.sha256(args.consumer.read_bytes()).hexdigest()
    print(json.dumps(result, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
