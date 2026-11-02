#pragma once

#include "frog_native_engine/FrogJson.hpp"
#include "frog_native_engine/CanonicalStringNative.hpp"

#include <string_view>

namespace frog::native_engine {

// Decoded SVG bytes, inclusive. The JSON reader already owns this portable
// string budget; icon authoring must not introduce a lower, competing ceiling.
// Document, decoded-raster and OLE-transport budgets remain independent.
inline constexpr std::size_t kMaximumFrogIconSvgBytes = kMaximumFrogJsonStringBytes;
inline constexpr std::string_view kFrogIconSvgSizeError =
    "Icon SVG exceeds the maximum JSON string byte length (16 MiB).";
inline constexpr std::string_view kFrogIconSvgNulError =
    "Icon SVG must not contain a NUL character.";
inline constexpr std::string_view kFrogIconSvgUtf8Error =
    "Icon SVG must contain valid UTF-8.";
inline constexpr std::string_view kFrogIconSvgContentError =
    "The retained icon must contain SVG presentation data.";

// Shared admission, not an XML renderer or decoder. Preserve the existing SVG
// marker rule and bytes; never normalize, rasterize or repair an admitted icon.
// Empty denotes the implicit default icon and remains valid.
[[nodiscard]] inline std::string_view frogIconPresentationError(std::string_view svg) noexcept {
    if (svg.size() > kMaximumFrogIconSvgBytes) return kFrogIconSvgSizeError;
    if (svg.find('\0') != svg.npos) return kFrogIconSvgNulError;
    if (!canonicalStringUtf8Valid(svg)) return kFrogIconSvgUtf8Error;
    if (!svg.empty() && svg.find("<svg") == svg.npos) return kFrogIconSvgContentError;
    return {};
}

} // namespace frog::native_engine
