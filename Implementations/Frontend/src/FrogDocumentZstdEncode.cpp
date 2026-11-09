#include "FrogDocumentZstdCodec.hpp"
#include "zstd.h"
namespace frog::native_engine::document_internal {
bool compressDocumentZstdChunk(
    std::span<const std::uint8_t> raw, std::vector<std::uint8_t> &scratch) {
    scratch.resize(ZSTD_compressBound(raw.size()));
    const auto size = ZSTD_compress(scratch.data(), scratch.size(), raw.data(), raw.size(), 3);
    const bool compressed = !ZSTD_isError(size) && size < raw.size();
    if (compressed) scratch.resize(size);
    return compressed;
}

}
