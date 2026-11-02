#pragma once

#include <algorithm>
#include <atomic>
#include <cmath>
#include <optional>
#include <string>
#include <string_view>

namespace frog::native_engine {

inline constexpr double kDocumentTextBasePointSize = 15.0;

[[nodiscard]] inline bool validDocumentTextPointSize(double size) noexcept {
    return std::isfinite(size) && size >= 6.0 && size <= 144.0;
}

[[nodiscard]] inline bool validDocumentTextFontFamily(std::string_view family) noexcept {
    if (family.empty() || family.size() > 256U || family.find('\0') != std::string_view::npos)
        return false;
    // Match the portable UTF-8 reader's scalar-value rules, including private
    // historical drafts whose other byte strings may use a legacy adapter.
    for (std::size_t index = 0; index < family.size();) {
        const auto first = static_cast<unsigned char>(family[index++]);
        if (first < 0x80U) continue;
        const unsigned int extra = first >= 0xc2U && first <= 0xdfU ? 1U :
            first >= 0xe0U && first <= 0xefU ? 2U : first >= 0xf0U && first <= 0xf4U ? 3U : 0U;
        if (extra == 0U || extra > family.size() - index) return false;
        for (unsigned int part = 0; part < extra; ++part) {
            const auto byte = static_cast<unsigned char>(family[index++]);
            if (byte < 0x80U || byte > 0xbfU || (part == 0U &&
                ((first == 0xe0U && byte < 0xa0U) || (first == 0xedU && byte >= 0xa0U) ||
                 (first == 0xf0U && byte < 0x90U) || (first == 0xf4U && byte > 0x8fU)))) return false;
        }
    }
    return true;
}

[[nodiscard]] inline double normalizedDocumentTextPointSize(double size) noexcept {
    return std::isfinite(size) ? std::clamp(size, 6.0, 144.0) : kDocumentTextBasePointSize;
}

// Application preference and fallback for legacy documents without authored
// text defaults. Document persistence stores its explicit defaults separately.
// Portable engine clients retain the historical baseline until configured.
inline std::atomic<double> applicationDocumentTextPointSize{kDocumentTextBasePointSize};

struct DocumentTextDefaultsBinding {
    const std::optional<double> *pointSize = nullptr;
    const std::optional<std::string> *fontFamily = nullptr;
};

// Bind the active document for the duration of its message/operation, including
// nested cross-document sends. Pointers address the optional members themselves
// so loading/Undo may replace their values without invalidating the binding.
inline thread_local DocumentTextDefaultsBinding activeDocumentTextDefaults;

[[nodiscard]] inline DocumentTextDefaultsBinding bindDocumentTextDefaults(
    DocumentTextDefaultsBinding binding) noexcept {
    const auto previous = activeDocumentTextDefaults;
    activeDocumentTextDefaults = binding;
    return previous;
}

class ScopedDocumentTextDefaultsBinding final {
public:
    explicit ScopedDocumentTextDefaultsBinding(DocumentTextDefaultsBinding binding) noexcept
        : previous_(bindDocumentTextDefaults(binding)) {}
    ~ScopedDocumentTextDefaultsBinding() { static_cast<void>(bindDocumentTextDefaults(previous_)); }
    ScopedDocumentTextDefaultsBinding(const ScopedDocumentTextDefaultsBinding &) = delete;
    ScopedDocumentTextDefaultsBinding &operator=(const ScopedDocumentTextDefaultsBinding &) = delete;
private:
    DocumentTextDefaultsBinding previous_;
};

[[nodiscard]] inline double documentDefaultTextPointSize() noexcept {
    if (activeDocumentTextDefaults.pointSize && *activeDocumentTextDefaults.pointSize)
        return **activeDocumentTextDefaults.pointSize;
    return applicationDocumentTextPointSize.load(std::memory_order_relaxed);
}

[[nodiscard]] inline std::string_view documentDefaultTextFontFamily() noexcept {
    if (activeDocumentTextDefaults.fontFamily && *activeDocumentTextDefaults.fontFamily)
        return **activeDocumentTextDefaults.fontFamily;
    return {};
}

inline void setDocumentDefaultTextPointSize(double size) noexcept {
    applicationDocumentTextPointSize.store(normalizedDocumentTextPointSize(size),
        std::memory_order_relaxed);
}

} // namespace frog::native_engine
