Zstandard 1.5.7

Upstream: https://github.com/facebook/zstd/releases/tag/v1.5.7
Source archive: https://codeload.github.com/facebook/zstd/zip/refs/tags/v1.5.7
Archive SHA-256: 7897bc5d620580d9b7cd3539c44b59d78f3657d33663fe97a145e07b4ebd69a4

Subset copied byte-for-byte from the existing Studio vendor: common and
decompression C sources/headers only. The Frontend target compiles ten C
translation units, including FSE/HUF and the Zstandard-namespaced xxHash.
It does not compile compression, dictionary-builder, CLI, threading or
assembly sources. ZSTD_DISABLE_ASM=1 and ZSTD_LEGACY_SUPPORT=0 are explicit
build definitions; the unsupported old Zstd legacy codecs are not bundled.
This does not change the supported Graiphic document revisions.

BSD license selected by the existing vendor. LICENSE.txt and COPYING.txt
preserve the upstream license texts; the alternative GPL text is retained,
without changing the BSD selection. Original attribution remains in each
source, including Meta Platforms and Yann Collet for xxHash. Apache-2.0
for Graiphic original code does not relicense this subset.

The archive URL and SHA above are retained provenance from the existing
vendor. This extraction compared the copied files with that local vendor;
it did not redownload or independently revalidate the upstream archive.
The candidate still requires its exact build and package qualification.
