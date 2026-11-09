#pragma once

#include <cstdint>
#include <span>
#include <vector>

namespace frog::native_engine::document_internal {

// Shared document compression boundary. The document owner retains format versions,
// chunk sizes, Base64, CRC, resource budgets and user-facing diagnostics.
enum class DocumentZstdDecodeResult {
    Ok,
    InvalidFrameSize,
    DecoderUnavailable,
    CorruptChunk
};

// Existing one-shot level-3 writer, true only when smaller than raw.
[[nodiscard]] bool compressDocumentZstdChunk(
    std::span<const std::uint8_t> raw,std::vector<std::uint8_t> &scratch);

[[nodiscard]] DocumentZstdDecodeResult decodeDocumentZstdChunk(
    std::span<const std::uint8_t> encoded, std::span<std::uint8_t> decoded);

} // namespace frog::native_engine::document_internal
