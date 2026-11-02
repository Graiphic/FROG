#pragma once

#include "frog_native_engine/DiagramCommentStyle.hpp"

#include <algorithm>
#include <cmath>
#include <functional>
#include <iterator>
#include <limits>
#include <string_view>
#include <utility>

namespace frog::native_engine {

[[nodiscard]] inline CommentTextFormat commentBaseTextFormat(const DiagramNode &node) {
    return {node.fontFamily, node.textStyle, node.fontPointSize, node.textRgb,
        diagramCommentUsesThemeTextColor(node)};
}

namespace comment_text_internal {

// Invalid UTF-8 bytes count as one replacement scalar. Persisted text already
// passes the JSON reader's strict UTF-8 validation; this also bounds drafts.
inline std::vector<std::size_t> scalarBoundaries(std::string_view text) {
    std::vector<std::size_t> result{0};
    for (std::size_t byte = 0; byte < text.size();) {
        const auto lead = static_cast<unsigned char>(text[byte]);
        std::size_t count = lead < 0x80U ? 1 :
            lead >= 0xC2U && lead <= 0xDFU ? 2 :
            lead >= 0xE0U && lead <= 0xEFU ? 3 :
            lead >= 0xF0U && lead <= 0xF4U ? 4 : 0;
        std::uint32_t scalar = count == 1 ? lead : count == 2 ? lead & 0x1FU :
            count == 3 ? lead & 0x0FU : lead & 0x07U;
        bool valid = count != 0 && count <= text.size() - byte;
        for (std::size_t part = 1; valid && part < count; ++part) {
            const auto next = static_cast<unsigned char>(text[byte + part]);
            valid = (next & 0xC0U) == 0x80U;
            scalar = (scalar << 6U) | (next & 0x3FU);
        }
        valid = valid && (count != 2 || scalar >= 0x80U) &&
            (count != 3 || scalar >= 0x800U) && (count != 4 || scalar >= 0x10000U) &&
            scalar <= 0x10FFFFU && !(scalar >= 0xD800U && scalar <= 0xDFFFU);
        byte += valid ? count : 1;
        result.push_back(result.back() + (valid && scalar > 0xFFFFU ? 2 : 1));
    }
    return result;
}

inline void normalizeRuns(DiagramNode &node, std::size_t length,
    const std::vector<std::size_t> *boundaries = nullptr) {
    const auto base = commentBaseTextFormat(node);
    std::vector<CommentTextRun> prepared;
    prepared.reserve(node.commentTextRuns.size());
    for (auto run : node.commentTextRuns) {
        if (run.start >= length || run.length == 0) continue;
        auto end = run.start + (std::min)(run.length, length - run.start);
        if (boundaries) {
            run.start = *std::lower_bound(boundaries->begin(), boundaries->end(), run.start);
            end = *std::prev(std::upper_bound(boundaries->begin(), boundaries->end(), end));
        }
        if (end <= run.start) continue;
        run.length = end - run.start;
        if (!std::isfinite(run.format.fontPointSize)) run.format.fontPointSize = base.fontPointSize;
        run.format.fontPointSize = std::clamp(run.format.fontPointSize, 6.0, 144.0);
        if (run.format.textStyle.empty()) run.format.textStyle = "plain";
        if (run.format == base) continue;
        prepared.push_back(std::move(run));
    }
    std::stable_sort(prepared.begin(), prepared.end(), [](const auto &left, const auto &right) {
        return left.start < right.start;
    });
    node.commentTextRuns.clear();
    for (auto &run : prepared) {
        if (!node.commentTextRuns.empty()) {
            auto &previous = node.commentTextRuns.back();
            const auto previousEnd = previous.start + previous.length;
            const auto end = run.start + run.length;
            // Earlier source ranges win malformed overlaps; later uncovered
            // portions remain. Valid editing operations never overlap ranges.
            if (run.start < previousEnd) {
                if (end <= previousEnd) continue;
                run.start = previousEnd;
                run.length = end - previousEnd;
            }
            if (run.start == previousEnd && run.format == previous.format) {
                previous.length += run.length;
                continue;
            }
        }
        node.commentTextRuns.push_back(std::move(run));
    }
}

} // namespace comment_text_internal

[[nodiscard]] inline std::size_t commentTextUtf16Length(std::string_view text) {
    return comment_text_internal::scalarBoundaries(text).back();
}

[[nodiscard]] inline CommentTextFormat commentTextFormatAt(const DiagramNode &node, std::size_t index) {
    for (const auto &run : node.commentTextRuns) {
        if (index >= run.start && index - run.start < run.length) return run.format;
    }
    return commentBaseTextFormat(node);
}

// Call against the current UTF-8 text. Ranges are clipped inward to Unicode
// scalar boundaries, so an invalid half-surrogate range cannot style its peer.
inline bool normalizeCommentTextRuns(DiagramNode &node, std::size_t utf16Length) {
    if (node.kind != "comment") return false;
    const auto before = node.commentTextRuns;
    const auto boundaries = comment_text_internal::scalarBoundaries(node.text);
    comment_text_internal::normalizeRuns(node, (std::min)(utf16Length, boundaries.back()), &boundaries);
    return before != node.commentTextRuns;
}

inline bool applyCommentTextFormat(DiagramNode &node, std::size_t first, std::size_t last,
    const std::function<void(CommentTextFormat &)> &mutation) {
    if (node.kind != "comment" || first >= last || !mutation) return false;
    const auto boundaries = comment_text_internal::scalarBoundaries(node.text);
    const auto length = boundaries.back();
    if (first >= length) return false;
    first = *std::lower_bound(boundaries.begin(), boundaries.end(), first);
    last = *std::prev(std::upper_bound(boundaries.begin(), boundaries.end(), (std::min)(last, length)));
    if (first >= last) return false;
    const auto before = node.commentTextRuns;
    comment_text_internal::normalizeRuns(node, length, &boundaries);
    std::vector<CommentTextRun> result;
    std::vector<std::size_t> cuts{first, last};
    for (const auto &run : node.commentTextRuns) {
        const auto end = run.start + run.length;
        if (run.start < first) result.push_back({run.start, (std::min)(end, first) - run.start, run.format});
        if (end > last) result.push_back({(std::max)(run.start, last), end - (std::max)(run.start, last), run.format});
        if (run.start > first && run.start < last) cuts.push_back(run.start);
        if (end > first && end < last) cuts.push_back(end);
    }
    std::sort(cuts.begin(), cuts.end());
    cuts.erase(std::unique(cuts.begin(), cuts.end()), cuts.end());
    for (std::size_t index = 1; index < cuts.size(); ++index) {
        auto format = commentTextFormatAt(node, cuts[index - 1]);
        mutation(format);
        result.push_back({cuts[index - 1], cuts[index] - cuts[index - 1], std::move(format)});
    }
    node.commentTextRuns = std::move(result);
    comment_text_internal::normalizeRuns(node, length, &boundaries);
    return before != node.commentTextRuns;
}

// Call BEFORE changing node.text. Then assign the new UTF-8 text and normalize
// against its new UTF-16 length. This function changes style metadata only.
inline bool replaceCommentTextRuns(DiagramNode &node, std::size_t first, std::size_t removed,
    std::size_t inserted, std::optional<CommentTextFormat> insertionFormat = {}) {
    if (node.kind != "comment" || (removed == 0 && inserted == 0)) return false;
    const auto boundaries = comment_text_internal::scalarBoundaries(node.text);
    const auto length = boundaries.back();
    if (first > length || removed > length - first ||
        !std::binary_search(boundaries.begin(), boundaries.end(), first) ||
        !std::binary_search(boundaries.begin(), boundaries.end(), first + removed) ||
        inserted > (std::numeric_limits<std::size_t>::max)() - (length - removed)) return false;
    const auto before = node.commentTextRuns;
    comment_text_internal::normalizeRuns(node, length, &boundaries);
    if (!insertionFormat) {
        insertionFormat = removed ? commentTextFormatAt(node, first) :
            first ? commentTextFormatAt(node, first - 1) : commentBaseTextFormat(node);
    }
    const auto end = first + removed;
    std::vector<CommentTextRun> result;
    for (const auto &run : node.commentTextRuns) {
        const auto runEnd = run.start + run.length;
        if (run.start < first) result.push_back({run.start, (std::min)(runEnd, first) - run.start, run.format});
        if (runEnd > end) {
            const auto right = (std::max)(run.start, end);
            result.push_back({first + inserted + (right - end), runEnd - right, run.format});
        }
    }
    if (inserted) result.push_back({first, inserted, std::move(*insertionFormat)});
    node.commentTextRuns = std::move(result);
    comment_text_internal::normalizeRuns(node, length - removed + inserted);
    return before != node.commentTextRuns;
}

} // namespace frog::native_engine
