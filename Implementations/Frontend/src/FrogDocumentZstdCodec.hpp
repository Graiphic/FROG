#pragma once

#include <cstdint>
#include <span>
#include <vector>

namespace frog::native_engine::document_internal {

// Private compression boundary. The document owner retains format versions,
// chunk sizes, Base64, CRC, resource budgets and user-facing diagnostics.
enum class DocumentZstdDecodeResult {
    Ok,
    InvalidFrameSize,
    DecoderUnavailable,
    CorruptChunk
};

[[nodiscard]] DocumentZstdDecodeResult decodeDocumentZstdChunk(
    std::span<const std::uint8_t> encoded, std::span<std::uint8_t> decoded);

} // namespace frog::native_engine::document_internal
