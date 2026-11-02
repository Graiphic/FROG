#include "FrogDocumentZstdCodec.hpp"

#include "zstd.h"

#include <memory>

namespace frog::native_engine::document_internal {

DocumentZstdDecodeResult decodeDocumentZstdChunk(
    std::span<const std::uint8_t> encoded, std::span<std::uint8_t> decoded) {
    if (ZSTD_getFrameContentSize(encoded.data(), encoded.size()) != decoded.size() ||
        ZSTD_findFrameCompressedSize(encoded.data(), encoded.size()) != encoded.size())
        return DocumentZstdDecodeResult::InvalidFrameSize;
    std::unique_ptr<ZSTD_DCtx, decltype(&ZSTD_freeDCtx)> context(ZSTD_createDCtx(), ZSTD_freeDCtx);
    if (!context || ZSTD_isError(ZSTD_DCtx_setParameter(context.get(), ZSTD_d_windowLogMax, 20)))
        return DocumentZstdDecodeResult::DecoderUnavailable;
    const auto actual = ZSTD_decompressDCtx(
        context.get(), decoded.data(), decoded.size(), encoded.data(), encoded.size());
    if (ZSTD_isError(actual) || actual != decoded.size())
        return DocumentZstdDecodeResult::CorruptChunk;
    return DocumentZstdDecodeResult::Ok;
}

} // namespace frog::native_engine::document_internal
