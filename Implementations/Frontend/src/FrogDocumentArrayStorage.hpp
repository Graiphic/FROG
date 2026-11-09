#pragma once
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/FrogJson.hpp"
#include <optional>
#include <span>
#include <string>
#include <string_view>

namespace frog::native_engine::document_internal {
struct ArrayStorageWriteContext {
    FrogArrayStorageMode mode = FrogArrayStorageMode::Readable;
    std::size_t remainingBytes = kMaximumFrogDocumentBytes;
    std::size_t remainingCells = kMaximumFrogJsonValueCount;
    FrogDocumentCompressFn compress = nullptr;
};
[[nodiscard]] std::string_view arrayStorageModeToken(FrogArrayStorageMode mode) noexcept;
[[nodiscard]] bool compactNumericArrayType(std::string_view type);
// Null means emit the original readable representation. No numeric conversion.
[[nodiscard]] std::optional<std::string> encodeArrayStorage(
    std::span<const std::string> cells, ArrayStorageWriteContext &context,
    std::string_view profile, std::size_t readableBytes);
// Derived Source retains the same lossless private draft with its existing
// Array encoding when embedding a second scalar tree would exceed the budget.
[[nodiscard]] std::size_t boundedDocumentJsonValueCount(const source::JsonValue &);
[[nodiscard]] bool compactDraftArrayProvenance(source::JsonValue &,std::string &error);
// Expands known private envelopes, with an aggregate decoded resource budget.
[[nodiscard]] bool inflateArrayStorage(source::JsonValue &root, std::string &error,
    bool allowCompact = true);
[[nodiscard]] bool readArrayStorageMode(const source::JsonValue &root,
    FrogArrayStorageMode &mode, std::string &error);
} // namespace frog::native_engine::document_internal
