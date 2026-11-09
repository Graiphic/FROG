#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace frog::native_engine {

// Portable contract shared by persistence and native authoring controllers.
// A shelf field id is durable document data; its label and value type must not
// be redefined independently by a platform renderer.
struct DiagramStructureShelfFieldSpec {
    std::string_view id;
    std::string_view label;
    std::string_view valueType;
};

enum class DiagramTimedLoopTimingNodeRole : std::uint8_t {
    Initial = 0,
    Feedback = 1,
    Update = 2,
    Result = 3
};

struct DiagramTimedLoopTimingNodeSpec {
    DiagramTimedLoopTimingNodeRole role;
    std::string_view id;
    std::string_view semanticRole;
    std::string_view direction;
    std::string_view displayLabel;
    std::string_view portPrefix;
    std::span<const DiagramStructureShelfFieldSpec> fields;
};

inline constexpr std::array<DiagramStructureShelfFieldSpec, 3>
    kDiagramEventStructureDataFields{ {
        { "source", "Source", "string" },
        { "type", "Type", "string" },
        { "time", "Time", "u64" }
    } };

// The same display-only entries used by the authoring schema. Value types
// still come from the actual declared/resolved field, not this caption table.
inline constexpr std::array<DiagramStructureShelfFieldSpec, 2>
    kDiagramEventValueChangeDataFields{{
        { "old_value", "OldVal", "unknown" },
        { "new_value", "NewVal", "unknown" }
    }};
[[nodiscard]] constexpr std::string_view diagramEventDataFieldDisplayLabel(std::string_view id) noexcept {
    for (const auto &field : kDiagramEventStructureDataFields) if (field.id == id) return field.label;
    for (const auto &field : kDiagramEventValueChangeDataFields) if (field.id == id) return field.label;
    return {};
}

inline constexpr std::array<DiagramStructureShelfFieldSpec, 8>
    kDiagramTimedLoopInitialFields{ {
        { "source", "Timing Source", "string" },
        { "period", "Period", "i64" },
        { "offset", "Offset", "i64" },
        { "deadline", "Deadline", "i64" },
        { "priority", "Priority", "i64" },
        { "processor", "Processor", "i64" },
        { "timeout", "Timeout", "i64" },
        { "late_policy", "Late Policy", "string" }
    } };

inline constexpr std::array<DiagramStructureShelfFieldSpec, 14>
    kDiagramTimedLoopFeedbackFields{ {
        { "error", "Error", "error" },
        { "expected_start", "Expected Start", "i64" },
        { "actual_start", "Actual Start", "i64" },
        { "expected_end", "Expected End", "i64" },
        { "actual_end", "Actual End", "i64" },
        { "finished_late", "Finished Late?", "bool" },
        { "iteration_duration", "Iteration Duration", "i64" },
        { "period", "Period", "i64" },
        { "deadline", "Deadline", "i64" },
        { "offset", "Offset", "i64" },
        { "priority", "Priority", "i64" },
        { "processor", "Processor", "i64" },
        { "timeout", "Timeout", "i64" },
        { "wakeup_reason", "Wakeup Reason", "string" }
    } };

inline constexpr std::array<DiagramStructureShelfFieldSpec, 7>
    kDiagramTimedLoopUpdateFields{ {
        { "deadline", "Deadline", "i64" },
        { "mode", "Mode", "string" },
        { "offset", "Offset", "i64" },
        { "period", "Period", "i64" },
        { "priority", "Priority", "i64" },
        { "processor", "Processor", "i64" },
        { "timeout", "Timeout", "i64" }
    } };

inline constexpr std::array<DiagramStructureShelfFieldSpec, 7>
    kDiagramTimedLoopResultFields{ {
        { "error", "Error", "error" },
        { "expected_end", "Expected End", "i64" },
        { "actual_end", "Actual End", "i64" },
        { "finished_late", "Finished Late?", "bool" },
        { "iteration_duration", "Iteration Duration", "i64" },
        { "processor", "Processor", "i64" },
        { "global_end", "Global End", "i64" }
    } };

inline constexpr std::array<DiagramTimedLoopTimingNodeSpec, 4>
    kDiagramTimedLoopTimingNodes{ {
        { DiagramTimedLoopTimingNodeRole::Initial, "initial", "configuration",
            "input", "Initial Timing", "timed.initial.",
            kDiagramTimedLoopInitialFields },
        { DiagramTimedLoopTimingNodeRole::Feedback, "feedback",
            "iteration_feedback", "output", "Timing Feedback",
            "timed.feedback.", kDiagramTimedLoopFeedbackFields },
        { DiagramTimedLoopTimingNodeRole::Update, "update", "next_iteration",
            "input", "Next Iteration", "timed.update.",
            kDiagramTimedLoopUpdateFields },
        { DiagramTimedLoopTimingNodeRole::Result, "result", "completion",
            "output", "Timing Result", "timed.result.",
            kDiagramTimedLoopResultFields }
    } };

[[nodiscard]] constexpr std::span<const DiagramStructureShelfFieldSpec>
diagramEventStructureDataFields() noexcept {
    return kDiagramEventStructureDataFields;
}

[[nodiscard]] constexpr const DiagramStructureShelfFieldSpec *
diagramStructureShelfFieldById(
    std::span<const DiagramStructureShelfFieldSpec> fields,
    std::string_view id) noexcept {
    for (const auto &field : fields) {
        if (field.id == id) return &field;
    }
    return nullptr;
}

enum class DiagramStructureShelfMissingFieldPolicy : std::uint8_t {
    Skip,
    UseFallback
};

// Non-owning, allocation-free projection of persisted shelf field ids onto a
// structure family's canonical catalog. Event and Timed Loop controllers use
// the same resolver in paint, hit-test and port enumeration hot paths.
class DiagramStructureShelfFieldSelection {
public:
    [[nodiscard]] constexpr std::size_t size() const noexcept {
        return resolvedCount_;
    }

    [[nodiscard]] constexpr bool empty() const noexcept {
        return resolvedCount_ == 0U;
    }

    [[nodiscard]] constexpr const DiagramStructureShelfFieldSpec *operator[](
        std::size_t resolvedIndex) const noexcept {
        if (resolvedIndex >= resolvedCount_ || catalog_.empty()) {
            return nullptr;
        }
        if (sourceCount_ == 0U) {
            return &catalog_.front();
        }
        if (missingPolicy_ ==
            DiagramStructureShelfMissingFieldPolicy::UseFallback) {
            const auto *field = diagramStructureShelfFieldById(
                catalog_, visibleIds_[resolvedIndex]);
            return field != nullptr ? field : &catalog_.front();
        }
        std::size_t currentResolvedIndex = 0U;
        for (std::size_t sourceIndex = 0U;
             sourceIndex < sourceCount_;
             ++sourceIndex) {
            const auto *field = diagramStructureShelfFieldById(
                catalog_, visibleIds_[sourceIndex]);
            if (field == nullptr) {
                continue;
            }
            if (currentResolvedIndex == resolvedIndex) {
                return field;
            }
            ++currentResolvedIndex;
        }
        return &catalog_.front();
    }

private:
    friend constexpr DiagramStructureShelfFieldSelection
    resolveDiagramStructureShelfFields(
        std::span<const DiagramStructureShelfFieldSpec>,
        std::span<const std::string>,
        std::size_t,
        DiagramStructureShelfMissingFieldPolicy) noexcept;

    std::span<const DiagramStructureShelfFieldSpec> catalog_;
    std::span<const std::string> visibleIds_;
    std::size_t sourceCount_ = 0U;
    std::size_t resolvedCount_ = 0U;
    DiagramStructureShelfMissingFieldPolicy missingPolicy_ =
        DiagramStructureShelfMissingFieldPolicy::Skip;
};

[[nodiscard]] constexpr DiagramStructureShelfFieldSelection
resolveDiagramStructureShelfFields(
    std::span<const DiagramStructureShelfFieldSpec> catalog,
    std::span<const std::string> visibleIds,
    std::size_t maximumSourceFields,
    DiagramStructureShelfMissingFieldPolicy missingPolicy) noexcept {
    DiagramStructureShelfFieldSelection selection;
    selection.catalog_ = catalog;
    selection.visibleIds_ = visibleIds;
    selection.sourceCount_ =
        (std::min)(maximumSourceFields, visibleIds.size());
    selection.missingPolicy_ = missingPolicy;
    if (catalog.empty()) {
        return selection;
    }
    if (missingPolicy == DiagramStructureShelfMissingFieldPolicy::UseFallback) {
        selection.resolvedCount_ =
            (std::max)(std::size_t{ 1U }, selection.sourceCount_);
        return selection;
    }
    for (std::size_t index = 0U; index < selection.sourceCount_; ++index) {
        if (diagramStructureShelfFieldById(catalog, visibleIds[index]) != nullptr) {
            ++selection.resolvedCount_;
        }
    }
    selection.resolvedCount_ =
        (std::max)(std::size_t{ 1U }, selection.resolvedCount_);
    return selection;
}

[[nodiscard]] constexpr const DiagramTimedLoopTimingNodeSpec &
diagramTimedLoopTimingNodeSpec(DiagramTimedLoopTimingNodeRole role) noexcept {
    const auto index = static_cast<std::size_t>(role);
    return kDiagramTimedLoopTimingNodes[
        index < kDiagramTimedLoopTimingNodes.size() ? index : 0U];
}

[[nodiscard]] constexpr std::span<const DiagramStructureShelfFieldSpec>
diagramTimedLoopTimingFields(DiagramTimedLoopTimingNodeRole role) noexcept {
    return diagramTimedLoopTimingNodeSpec(role).fields;
}

[[nodiscard]] constexpr std::uint32_t
diagramTimedLoopMaximumVisibleRows(DiagramTimedLoopTimingNodeRole role) noexcept {
    return static_cast<std::uint32_t>(diagramTimedLoopTimingFields(role).size());
}

[[nodiscard]] constexpr bool
diagramTimedLoopTimingNodeIsInput(DiagramTimedLoopTimingNodeRole role) noexcept {
    return diagramTimedLoopTimingNodeSpec(role).direction == "input";
}

// Growth chooses unused fields in catalog order. Existing rows (including
// manually selected legacy duplicates) are never normalized or reordered.
[[nodiscard]] inline std::vector<std::string> resizedTimedLoopFields(
    DiagramTimedLoopTimingNodeRole role, std::span<const std::string> original,
    std::size_t requested, std::size_t minimumRetained = 1) {
    const auto catalog = diagramTimedLoopTimingFields(role);
    std::vector<std::string> rows(original.begin(), original.end());
    if (rows.empty() && !catalog.empty()) rows.emplace_back(catalog.front().id);
    const auto keep = (std::max)(minimumRetained, (std::max)(std::size_t{1}, requested));
    if (keep < rows.size()) rows.resize(keep);
    for (const auto &field : catalog) {
        if (rows.size() >= keep) break;
        if (std::find(rows.begin(), rows.end(), field.id) == rows.end())
            rows.emplace_back(field.id);
    }
    return rows;
}

} // namespace frog::native_engine
