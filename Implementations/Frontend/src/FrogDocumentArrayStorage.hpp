#pragma once
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/BindingType.hpp"
#include "frog_native_engine/FrogJson.hpp"
#include <optional>
#include <span>
#include <string>
#include <string_view>

namespace frog::native_engine::document_internal {
inline constexpr std::string_view executionArrayStorageBudgetError =
    "FROG array storage: execution projection exceeds the supported JSON representation budget.";
inline constexpr std::string_view numericWordArrayProfile = "numeric-cell-bits-le-v1";
inline constexpr std::string_view exactNarrowNumericWordArrayProfile = "numeric-cell-bits-le-v2";
inline constexpr std::string_view composedNumericArrayField = "studio_numeric_words";
// Numeric envelopes carry U32 decoded byte counts. Bound the complete document
// by that representable byte domain, independently of encoded JSON limits.
// Physical allocations retain ArrayCapacity's current-host admission and their
// strong failure guarantee. This is a logical decode bound, not reserved RAM.
inline constexpr std::size_t maximumDecodedNumericWordsBytes = UINT32_MAX;
struct ArrayStorageWriteContext {
    FrogArrayStorageMode mode = FrogArrayStorageMode::Readable;
    std::size_t remainingBytes = kMaximumFrogDocumentBytes;
    std::size_t remainingCells = kMaximumFrogJsonValueCount;
    std::size_t readableCells = kMaximumFrogJsonValueCount;
    FrogDocumentCompressFn compress = nullptr;
    std::size_t remainingNumericWordsBytes = maximumDecodedNumericWordsBytes;
};
[[nodiscard]] std::string_view arrayStorageModeToken(FrogArrayStorageMode mode) noexcept;
[[nodiscard]] bool compactNumericArrayType(std::string_view type);
[[nodiscard]] bool isPackedExecutionArrayEnvelope(const source::JsonValue &,std::string_view type);
[[nodiscard]] bool decodeComposedPackedNumericArray(const source::JsonValue &,const binding::TypeRef &,
    DiagramArrayValues &,std::vector<std::uint32_t> &,
    std::vector<std::pair<std::size_t,std::uint64_t>> &,std::size_t &remainingWords,std::string &error);
// Null means emit the original readable representation. No numeric conversion.
[[nodiscard]] std::optional<std::string> encodeArrayStorage(
    std::span<const std::string> cells, ArrayStorageWriteContext &context,
    std::string_view profile, std::size_t readableBytes);
[[nodiscard]] std::optional<std::string> encodeDiagramArrayStorage(
    const DiagramArrayValues &, ArrayStorageWriteContext &, bool compactable, std::string_view type = {});
[[nodiscard]] bool decodeDiagramArrayStorage(const source::JsonValue &configuration,
    DiagramArrayValues &, std::string_view type, std::size_t &remainingBytes, std::string &error,
    std::size_t *remainingNumericWordsBytes = nullptr);
// Derived Source retains the same lossless private draft with its existing
// Array encoding when embedding a second scalar tree would exceed the budget.
[[nodiscard]] std::size_t boundedDocumentJsonValueCount(const source::JsonValue &);
[[nodiscard]] bool compactDraftArrayProvenance(source::JsonValue &,std::string &error);
// Expands known private envelopes, with an aggregate decoded resource budget.
[[nodiscard]] bool inflateArrayStorage(source::JsonValue &root, std::string &error,
    bool allowCompact = true, bool preserveDiagramCells = false,
    std::size_t *remainingDecodedBytes = nullptr);
[[nodiscard]] bool inflateExecutionArrayStorage(source::JsonValue &,std::string &,bool allowCompact,bool &keptPackedArrays);
[[nodiscard]] bool readArrayStorageMode(const source::JsonValue &root,
    FrogArrayStorageMode &mode, std::string &error);
} // namespace frog::native_engine::document_internal
