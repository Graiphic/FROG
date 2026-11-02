#include "frog_native_engine/WidgetInstance.hpp"

#include "frog_native_engine/ReferenceWidget.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <utility>

namespace frog::native_engine {

namespace {

constexpr std::string_view kPlainTextStyle = "plain";
constexpr std::array<std::string_view, 6> kTextStyles{
    "plain",
    "bold",
    "italic",
    "underline",
    "outline",
    "strikeout"
};
constexpr std::string_view kDefaultLabelHorizontalAlignment = "left";
constexpr std::array<std::string_view, 3> kLabelHorizontalAlignments{
    "left",
    "center",
    "right"
};
constexpr std::string_view kDefaultLabelVerticalAlignment = "middle";
constexpr std::array<std::string_view, 3> kLabelVerticalAlignments{
    "top",
    "middle",
    "bottom"
};

[[nodiscard]] double normalizedFontScale(double scale) noexcept {
    if (!std::isfinite(scale)) {
        return 1.0;
    }

    return std::clamp(scale, 6.0 / kDocumentTextBasePointSize, 144.0 / kDocumentTextBasePointSize);
}

[[nodiscard]] std::optional<double> scaleOverride(double scale) noexcept {
    // An explicit choice must remain explicit, including the baseline size.
    return normalizedFontScale(scale);
}

[[nodiscard]] std::string normalizeTextStyle(std::string_view style) {
    return canonicalTextStyle(style);
}

[[nodiscard]] std::optional<std::string> textStyleOverride(std::string_view style) {
    auto normalized = normalizeTextStyle(style);
    if (normalized == kPlainTextStyle) {
        return std::nullopt;
    }
    return normalized;
}

[[nodiscard]] std::string normalizeHorizontalAlignment(std::string_view alignment) {
    std::string normalized;
    normalized.reserve(alignment.size());
    for (const char ch : alignment) {
        if (ch == '-' || ch == ' ') {
            normalized.push_back('_');
        } else {
            normalized.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
        }
    }

    if (normalized == "middle") {
        return "center";
    }
    for (const auto candidate : kLabelHorizontalAlignments) {
        if (normalized == candidate) {
            return normalized;
        }
    }
    return std::string{ kDefaultLabelHorizontalAlignment };
}

[[nodiscard]] std::optional<std::string> horizontalAlignmentOverride(std::string_view alignment) {
    auto normalized = normalizeHorizontalAlignment(alignment);
    if (normalized == kDefaultLabelHorizontalAlignment) {
        return std::nullopt;
    }
    return normalized;
}

[[nodiscard]] std::string normalizeVerticalAlignment(std::string_view alignment) {
    std::string normalized;
    normalized.reserve(alignment.size());
    for (const char ch : alignment) {
        if (ch == '-' || ch == ' ') {
            normalized.push_back('_');
        } else {
            normalized.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
        }
    }

    if (normalized == "center" || normalized == "centre") {
        return "middle";
    }
    for (const auto candidate : kLabelVerticalAlignments) {
        if (normalized == candidate) {
            return normalized;
        }
    }
    return std::string{ kDefaultLabelVerticalAlignment };
}

[[nodiscard]] std::optional<std::string> verticalAlignmentOverride(std::string_view alignment) {
    auto normalized = normalizeVerticalAlignment(alignment);
    if (normalized == kDefaultLabelVerticalAlignment) {
        return std::nullopt;
    }
    return normalized;
}

[[nodiscard]] std::optional<std::uint32_t> colorOverride(std::optional<std::uint32_t> rgb) noexcept {
    if (rgb.has_value()) {
        if (isTransparentColorModel(rgb.value())) {
            return kTransparentColorModel;
        }
        return rgb.value() & 0x00FFFFFFU;
    }
    return std::nullopt;
}

[[nodiscard]] bool setStateFillColorOverride(
    WidgetInstance &widget,
    bool trueState,
    std::optional<std::uint32_t> rgb) {
    rgb = colorOverride(rgb);

    auto &target = trueState ? widget.stateTrueFillRgb : widget.stateFalseFillRgb;
    if (target == rgb) {
        return false;
    }

    target = rgb;
    return true;
}

[[nodiscard]] bool setStateStrokeColorOverride(
    WidgetInstance &widget,
    bool trueState,
    std::optional<std::uint32_t> rgb) {
    rgb = colorOverride(rgb);

    auto &target = trueState ? widget.stateTrueStrokeRgb : widget.stateFalseStrokeRgb;
    if (target == rgb) {
        return false;
    }

    target = rgb;
    return true;
}

[[nodiscard]] bool setStateTextColorOverride(
    WidgetInstance &widget,
    std::optional<std::uint32_t> rgb) {
    rgb = colorOverride(rgb);

    if (widget.stateTextRgb == rgb) {
        return false;
    }

    widget.stateTextRgb = rgb;
    return true;
}

} // namespace

bool setBooleanWidgetStateFillColor(
    WidgetInstance &widget,
    bool trueState,
    std::optional<std::uint32_t> rgb) {
    if (!isBooleanWidget(widget)) {
        return false;
    }

    return setStateFillColorOverride(widget, trueState, rgb);
}

bool setBooleanWidgetStateStrokeColor(
    WidgetInstance &widget,
    bool trueState,
    std::optional<std::uint32_t> rgb) {
    if (!isBooleanWidget(widget)) {
        return false;
    }

    return setStateStrokeColorOverride(widget, trueState, rgb);
}

bool setBooleanWidgetTextColor(WidgetInstance &widget, std::optional<std::uint32_t> rgb) {
    if (!isBooleanWidget(widget)) {
        return false;
    }

    return setStateTextColorOverride(widget, rgb);
}

bool setButtonWidgetStateFillColor(
    WidgetInstance &widget,
    bool trueState,
    std::optional<std::uint32_t> rgb) {
    if (!isButtonWidget(widget)) {
        return false;
    }

    return setStateFillColorOverride(widget, trueState, rgb);
}

bool setButtonWidgetStateStrokeColor(
    WidgetInstance &widget,
    bool trueState,
    std::optional<std::uint32_t> rgb) {
    if (!isButtonWidget(widget)) {
        return false;
    }

    return setStateStrokeColorOverride(widget, trueState, rgb);
}

bool setButtonWidgetTextColor(WidgetInstance &widget, std::optional<std::uint32_t> rgb) {
    if (!isButtonWidget(widget)) {
        return false;
    }

    return setStateTextColorOverride(widget, rgb);
}

double widgetLabelFontScale(const WidgetInstance &widget) noexcept {
    return normalizedFontScale(widget.labelFontScale.value_or(widget.inheritedTextPointSize / kDocumentTextBasePointSize));
}

double widgetCaptionFontScale(const WidgetInstance &widget) noexcept {
    return normalizedFontScale(widget.captionFontScale.value_or(widget.inheritedTextPointSize / kDocumentTextBasePointSize));
}


double widgetNumericValueFontScale(const WidgetInstance &widget) noexcept {
    return normalizedFontScale(widget.numericValueFontScale.value_or(widget.inheritedTextPointSize / kDocumentTextBasePointSize));
}

double widgetStringValueFontScale(const WidgetInstance &widget) noexcept {
    return normalizedFontScale(widget.stringValueFontScale.value_or(widget.inheritedTextPointSize / kDocumentTextBasePointSize));
}

double widgetStateTextFontScale(const WidgetInstance &widget) noexcept {
    return normalizedFontScale(widget.stateTextFontScale.value_or(widget.inheritedTextPointSize / kDocumentTextBasePointSize));
}

bool setWidgetLabelFontScale(WidgetInstance &widget, double scale) {
    const auto next = scaleOverride(scale);
    if (next == widget.labelFontScale) {
        return false;
    }

    widget.labelFontScale = next;
    return true;
}

bool setWidgetCaptionFontScale(WidgetInstance &widget, double scale) {
    const auto next = scaleOverride(scale);
    if (next == widget.captionFontScale) return false;
    widget.captionFontScale = next;
    return true;
}

bool setWidgetNumericValueFontScale(WidgetInstance &widget, double scale) {
    if (!isNumericWidget(widget)) {
        return false;
    }

    const auto next = scaleOverride(scale);
    if (next == widget.numericValueFontScale) {
        return false;
    }

    widget.numericValueFontScale = next;
    return true;
}

bool setWidgetStringValueFontScale(WidgetInstance &widget, double scale) {
    if (!isStringWidget(widget) &&
        !isPathWidget(widget) &&
        !isRingWidget(widget) &&
        !isEnumWidget(widget) && !isReferenceWidget(widget)) {
        return false;
    }

    const auto next = scaleOverride(scale);
    if (next == widget.stringValueFontScale) {
        return false;
    }

    widget.stringValueFontScale = next;
    return true;
}

bool setWidgetStateTextFontScale(WidgetInstance &widget, double scale) {
    if (!isButtonWidget(widget) && !isBooleanWidget(widget)) {
        return false;
    }

    const double next = normalizedFontScale(scale);
    constexpr double epsilon = 0.001;
    if (widget.stateTextFontScale.has_value() &&
        std::abs(widget.stateTextFontScale.value() - next) < epsilon) {
        return false;
    }

    widget.stateTextFontScale = next;
    return true;
}

bool setWidgetLabelFontFamily(WidgetInstance &widget, std::string family) {
    if (family.empty() || family == "Application Font") {
        if (!widget.labelFontFamily.has_value()) {
            return false;
        }
        widget.labelFontFamily.reset();
        return true;
    }

    if (widget.labelFontFamily == family) {
        return false;
    }

    widget.labelFontFamily = std::move(family);
    return true;
}

bool setWidgetCaptionFontFamily(WidgetInstance &widget, std::string family) {
    const auto next = family.empty() || family == "Application Font"
        ? std::optional<std::string>{} : std::optional<std::string>{std::move(family)};
    if (next == widget.captionFontFamily) return false;
    widget.captionFontFamily = next;
    return true;
}

bool setWidgetNumericValueFontFamily(WidgetInstance &widget, std::string family) {
    if (!isNumericWidget(widget)) {
        return false;
    }
    if (family.empty() || family == "Application Font") {
        if (!widget.numericValueFontFamily.has_value()) {
            return false;
        }
        widget.numericValueFontFamily.reset();
        return true;
    }

    if (widget.numericValueFontFamily == family) {
        return false;
    }

    widget.numericValueFontFamily = std::move(family);
    return true;
}

bool setWidgetStringValueFontFamily(WidgetInstance &widget, std::string family) {
    if (!isStringWidget(widget) &&
        !isPathWidget(widget) &&
        !isRingWidget(widget) &&
        !isEnumWidget(widget) && !isReferenceWidget(widget)) {
        return false;
    }
    if (family.empty() || family == "Application Font") {
        if (!widget.stringValueFontFamily.has_value()) {
            return false;
        }
        widget.stringValueFontFamily.reset();
        return true;
    }

    if (widget.stringValueFontFamily == family) {
        return false;
    }

    widget.stringValueFontFamily = std::move(family);
    return true;
}

bool textStyleHasFlag(std::string_view style, std::string_view flag) noexcept {
    if (flag == "plain") {
        for (const auto candidate : kTextStyles) {
            if (candidate != "plain" && textStyleHasFlag(style, candidate)) {
                return false;
            }
        }
        return true;
    }
    const auto equals = [](std::string_view left, std::string_view right) {
        return left.size() == right.size() && std::equal(left.begin(), left.end(), right.begin(),
            [](unsigned char a, unsigned char b) { return std::tolower(a) == std::tolower(b); });
    };
    constexpr std::string_view separators = " \t\r\n,;+|";
    for (std::size_t start = style.find_first_not_of(separators); start != std::string_view::npos;) {
        const auto end = style.find_first_of(separators, start);
        const auto token = style.substr(start, end == std::string_view::npos ? end : end - start);
        if (equals(token, flag) || (flag == "strikeout" &&
            (equals(token, "line-through") || equals(token, "line_through") ||
             equals(token, "strike-through") || equals(token, "strike_through")))) {
            return true;
        }
        start = end == std::string_view::npos ? end : style.find_first_not_of(separators, end);
    }
    return false;
}

std::string canonicalTextStyle(std::string_view style) {
    std::string result;
    for (const auto flag : kTextStyles) {
        if (flag != "plain" && textStyleHasFlag(style, flag)) {
            if (!result.empty()) { result += ' '; }
            result += flag;
        }
    }
    return result.empty() ? "plain" : result;
}

std::string withTextStyleFlag(std::string_view style, std::string_view flag, bool enabled) {
    if (canonicalTextStyle(flag) == "plain") { return "plain"; }
    std::string result;
    for (const auto candidate : kTextStyles) {
        if (candidate != "plain" && (textStyleHasFlag(flag, candidate)
                ? enabled : textStyleHasFlag(style, candidate))) {
            if (!result.empty()) { result += ' '; }
            result += candidate;
        }
    }
    return result.empty() ? "plain" : result;
}

bool setWidgetStateTextFontFamily(WidgetInstance &widget, std::string family) {
    if (!isBooleanWidget(widget) && !isButtonWidget(widget)) { return false; }
    const auto next = family.empty() || family == "Application Font"
        ? std::optional<std::string>{} : std::optional<std::string>{std::move(family)};
    if (widget.stateTextFontFamily == next) { return false; }
    widget.stateTextFontFamily = next;
    return true;
}

std::string_view widgetStateTextStyle(const WidgetInstance &widget) noexcept {
    return widget.stateTextStyle.has_value() ? std::string_view{*widget.stateTextStyle} : kPlainTextStyle;
}

bool setWidgetStateTextStyle(WidgetInstance &widget, std::string_view style) {
    if (!isBooleanWidget(widget) && !isButtonWidget(widget)) { return false; }
    const auto next = textStyleOverride(style);
    if (widget.stateTextStyle == next) { return false; }
    widget.stateTextStyle = next;
    return true;
}

std::string_view widgetLabelTextStyle(const WidgetInstance &widget) noexcept {
    return widget.labelTextStyle.has_value() ? std::string_view{ widget.labelTextStyle.value() } : kPlainTextStyle;
}

std::string_view widgetNumericValueTextStyle(const WidgetInstance &widget) noexcept {
    return widget.numericValueTextStyle.has_value() ? std::string_view{ widget.numericValueTextStyle.value() } : kPlainTextStyle;
}

std::string_view widgetStringValueTextStyle(const WidgetInstance &widget) noexcept {
    return widget.stringValueTextStyle.has_value() ? std::string_view{ widget.stringValueTextStyle.value() } : kPlainTextStyle;
}

bool setWidgetLabelTextStyle(WidgetInstance &widget, std::string_view style) {
    const auto next = textStyleOverride(style);
    if (widget.labelTextStyle == next) {
        return false;
    }

    widget.labelTextStyle = next;
    return true;
}

bool setWidgetNumericValueTextStyle(WidgetInstance &widget, std::string_view style) {
    if (!isNumericWidget(widget)) {
        return false;
    }

    const auto next = textStyleOverride(style);
    if (widget.numericValueTextStyle == next) {
        return false;
    }

    widget.numericValueTextStyle = next;
    return true;
}

bool setWidgetStringValueTextStyle(WidgetInstance &widget, std::string_view style) {
    if (!isStringWidget(widget) &&
        !isPathWidget(widget) &&
        !isRingWidget(widget) &&
        !isEnumWidget(widget) && !isReferenceWidget(widget)) {
        return false;
    }

    const auto next = textStyleOverride(style);
    if (widget.stringValueTextStyle == next) {
        return false;
    }

    widget.stringValueTextStyle = next;
    return true;
}

std::string_view widgetChoiceTextHorizontalAlignment(const WidgetInstance &widget) noexcept {
    if ((isRingWidget(widget) || isEnumWidget(widget)) &&
        widget.choiceTextHorizontalAlignment.has_value()) {
        const auto &alignment = widget.choiceTextHorizontalAlignment.value();
        for (const auto candidate : kLabelHorizontalAlignments) {
            if (alignment == candidate) {
                return std::string_view{ alignment };
            }
        }
    }
    return kDefaultLabelHorizontalAlignment;
}

std::string_view widgetStringTextHorizontalAlignment(const WidgetInstance &widget) noexcept {
    if (isStringWidget(widget) && !isStringDropdownWidget(widget) && widget.stringTextHorizontalAlignment) {
        for (const auto candidate : kLabelHorizontalAlignments)
            if (*widget.stringTextHorizontalAlignment==candidate) return *widget.stringTextHorizontalAlignment;
    }
    return kDefaultLabelHorizontalAlignment;
}

bool setWidgetStringTextHorizontalAlignment(WidgetInstance &widget,std::string_view alignment) {
    if (!isStringWidget(widget) || isStringDropdownWidget(widget)) return false;
    const auto next=horizontalAlignmentOverride(alignment);
    if (widget.stringTextHorizontalAlignment==next) return false;
    widget.stringTextHorizontalAlignment=next;
    return true;
}

bool setWidgetChoiceTextHorizontalAlignment(
    WidgetInstance &widget,
    std::string_view alignment) {
    if (!isRingWidget(widget) && !isEnumWidget(widget)) {
        return false;
    }

    const auto next = horizontalAlignmentOverride(alignment);
    if (widget.choiceTextHorizontalAlignment == next) {
        return false;
    }

    widget.choiceTextHorizontalAlignment = next;
    return true;
}

bool setWidgetLabelTextColor(WidgetInstance &widget, std::optional<std::uint32_t> rgb) {
    rgb = colorOverride(rgb);

    if (widget.labelTextRgb == rgb) {
        return false;
    }

    widget.labelTextRgb = rgb;
    return true;
}

namespace {

[[nodiscard]] bool supportsNumericAppearanceColors(const WidgetInstance &widget) noexcept {
    return isNumericWidget(widget) || isRingWidget(widget) || isEnumWidget(widget);
}

[[nodiscard]] bool supportsNumericStepAppearanceColors(const WidgetInstance &widget) noexcept {
    return isNumericControlWidget(widget) ||
        isRingControlWidget(widget) ||
        isEnumControlWidget(widget);
}

} // namespace

bool setWidgetNumericValueTextColor(WidgetInstance &widget, std::optional<std::uint32_t> rgb) {
    if (!supportsNumericAppearanceColors(widget)) {
        return false;
    }
    rgb = colorOverride(rgb);

    if (widget.numericValueTextRgb == rgb) {
        return false;
    }

    widget.numericValueTextRgb = rgb;
    return true;
}

bool setWidgetStringValueTextColor(WidgetInstance &widget, std::optional<std::uint32_t> rgb) {
    if (!isStringWidget(widget) && !isPathWidget(widget)) {
        return false;
    }
    rgb = colorOverride(rgb);

    if (widget.stringValueTextRgb == rgb) {
        return false;
    }

    widget.stringValueTextRgb = rgb;
    return true;
}

bool setWidgetNumericBodyFillColor(WidgetInstance &widget, std::optional<std::uint32_t> rgb) {
    if (!supportsNumericAppearanceColors(widget)) {
        return false;
    }

    rgb = colorOverride(rgb);
    if (widget.numericBodyFillRgb == rgb) {
        return false;
    }

    widget.numericBodyFillRgb = rgb;
    return true;
}

bool setWidgetSurfaceColors(WidgetInstance &widget, std::optional<std::uint32_t> fill,
    std::optional<std::uint32_t> stroke, std::optional<bool> displayedBooleanState) {
    fill = colorOverride(fill);
    stroke = colorOverride(stroke);
    const auto assign = [](auto &current, const auto &next) {
        if (current == next) return false;
        current = next;
        return true;
    };
    const auto pair = [&](auto &currentFill, auto &currentStroke) {
        const bool fillChanged = assign(currentFill, fill);
        const bool strokeChanged = assign(currentStroke, stroke);
        return fillChanged || strokeChanged;
    };
    if (isLabelWidget(widget)) return pair(widget.labelSurfaceFillRgb, widget.labelSurfaceStrokeRgb);
    if (isBooleanWidget(widget) || isButtonWidget(widget)) {
        const bool state = displayedBooleanState.value_or(widgetBooleanValue(widget));
        return state ? pair(widget.stateTrueFillRgb, widget.stateTrueStrokeRgb)
            : pair(widget.stateFalseFillRgb, widget.stateFalseStrokeRgb);
    }
    if (supportsNumericAppearanceColors(widget))
        return pair(widget.numericBodyFillRgb, widget.numericBodyStrokeRgb);
    if (isStringWidget(widget) || isPathWidget(widget))
        return pair(widget.stringBodyFillRgb, widget.stringBodyStrokeRgb);
    if (auto *array = arrayContainerState(widget)) {
        const bool frameChanged = pair(array->frameFillRgb, array->frameStrokeRgb);
        const bool regionChanged = pair(array->arrayRegionFillRgb, array->arrayRegionStrokeRgb);
        return frameChanged || regionChanged;
    }
    if (auto *cluster = clusterContainerState(widget))
        return pair(cluster->frameFillRgb, cluster->frameStrokeRgb);
    if (widget.valueType == "variant" || isPictureWidget(widget) || isXYGraphWidget(widget) || isWaveformGraphWidget(widget))
        return pair(widget.surfaceFillRgb, widget.surfaceStrokeRgb);
    return false;
}

bool setWidgetNumericBodyStrokeColor(WidgetInstance &widget, std::optional<std::uint32_t> rgb) {
    if (!supportsNumericAppearanceColors(widget)) {
        return false;
    }

    rgb = colorOverride(rgb);
    if (widget.numericBodyStrokeRgb == rgb) {
        return false;
    }

    widget.numericBodyStrokeRgb = rgb;
    return true;
}

bool setWidgetStringBodyFillColor(WidgetInstance &widget, std::optional<std::uint32_t> rgb) {
    if (!isStringWidget(widget) && !isPathWidget(widget)) {
        return false;
    }

    rgb = colorOverride(rgb);
    if (widget.stringBodyFillRgb == rgb) {
        return false;
    }

    widget.stringBodyFillRgb = rgb;
    return true;
}

bool setWidgetStringBodyStrokeColor(WidgetInstance &widget, std::optional<std::uint32_t> rgb) {
    if (!isStringWidget(widget) && !isPathWidget(widget)) {
        return false;
    }

    rgb = colorOverride(rgb);
    if (widget.stringBodyStrokeRgb == rgb) {
        return false;
    }

    widget.stringBodyStrokeRgb = rgb;
    return true;
}

bool setWidgetNumericStepButtonFillColor(WidgetInstance &widget, std::optional<std::uint32_t> rgb) {
    if (!supportsNumericStepAppearanceColors(widget)) {
        return false;
    }

    rgb = colorOverride(rgb);
    if (widget.numericStepButtonFillRgb == rgb) {
        return false;
    }

    widget.numericStepButtonFillRgb = rgb;
    return true;
}

bool setWidgetNumericStepButtonStrokeColor(WidgetInstance &widget, std::optional<std::uint32_t> rgb) {
    if (!supportsNumericStepAppearanceColors(widget)) {
        return false;
    }

    rgb = colorOverride(rgb);
    if (widget.numericStepButtonStrokeRgb == rgb) {
        return false;
    }

    widget.numericStepButtonStrokeRgb = rgb;
    return true;
}

bool setWidgetNumericStepArrowFillColor(WidgetInstance &widget, std::optional<std::uint32_t> rgb) {
    if (!supportsNumericStepAppearanceColors(widget)) {
        return false;
    }

    rgb = colorOverride(rgb);
    if (widget.numericStepArrowFillRgb == rgb) {
        return false;
    }

    widget.numericStepArrowFillRgb = rgb;
    return true;
}

bool setWidgetNumericStepArrowStrokeColor(WidgetInstance &widget, std::optional<std::uint32_t> rgb) {
    if (!supportsNumericStepAppearanceColors(widget)) {
        return false;
    }

    rgb = colorOverride(rgb);
    if (widget.numericStepArrowStrokeRgb == rgb) {
        return false;
    }

    widget.numericStepArrowStrokeRgb = rgb;
    return true;
}

std::string_view widgetLabelHorizontalAlignment(const WidgetInstance &widget) noexcept {
    if (widget.labelHorizontalAlignment.has_value()) {
        const auto &alignment = widget.labelHorizontalAlignment.value();
        for (const auto candidate : kLabelHorizontalAlignments) {
            if (alignment == candidate) {
                return std::string_view{ alignment };
            }
        }
    }
    return kDefaultLabelHorizontalAlignment;
}

bool setWidgetLabelHorizontalAlignment(WidgetInstance &widget, std::string_view alignment) {
    if (!isLabelWidget(widget)) {
        return false;
    }

    const auto next = horizontalAlignmentOverride(alignment);
    if (widget.labelHorizontalAlignment == next) {
        return false;
    }

    widget.labelHorizontalAlignment = next;
    return true;
}

std::string_view widgetLabelVerticalAlignment(const WidgetInstance &widget) noexcept {
    if (widget.labelVerticalAlignment.has_value()) {
        const auto &alignment = widget.labelVerticalAlignment.value();
        for (const auto candidate : kLabelVerticalAlignments) {
            if (alignment == candidate) {
                return std::string_view{ alignment };
            }
        }
    }
    return kDefaultLabelVerticalAlignment;
}

bool setWidgetLabelVerticalAlignment(WidgetInstance &widget, std::string_view alignment) {
    if (!isLabelWidget(widget)) {
        return false;
    }

    const auto next = verticalAlignmentOverride(alignment);
    if (widget.labelVerticalAlignment == next) {
        return false;
    }

    widget.labelVerticalAlignment = next;
    return true;
}

bool setWidgetLabelSurfaceFillColor(WidgetInstance &widget, std::optional<std::uint32_t> rgb) {
    rgb = colorOverride(rgb);
    if (widget.labelSurfaceFillRgb == rgb) {
        return false;
    }

    widget.labelSurfaceFillRgb = rgb;
    return true;
}

bool setWidgetLabelSurfaceStrokeColor(WidgetInstance &widget, std::optional<std::uint32_t> rgb) {
    rgb = colorOverride(rgb);
    if (widget.labelSurfaceStrokeRgb == rgb) {
        return false;
    }

    widget.labelSurfaceStrokeRgb = rgb;
    return true;
}

bool buttonStateTextLockedInCenter(const WidgetInstance &widget) noexcept {
    return (isButtonWidget(widget) || isBooleanWidget(widget)) &&
        widget.buttonStateTextLockInCenter.value_or(false);
}

bool setButtonStateTextLockedInCenter(WidgetInstance &widget, bool locked) {
    if (!isButtonWidget(widget) && !isBooleanWidget(widget)) {
        return false;
    }

    if (!locked) {
        if (!widget.buttonStateTextLockInCenter.has_value()) {
            return false;
        }
        widget.buttonStateTextLockInCenter.reset();
        return true;
    }

    if (widget.buttonStateTextLockInCenter.value_or(false)) {
        return false;
    }
    widget.buttonStateTextLockInCenter = true;
    return true;
}

bool pathFolderVisible(const WidgetInstance &widget) noexcept {
    return isPathWidget(widget) &&
        widget.role == "control" &&
        widget.pathFolderVisible.value_or(true);
}

bool setPathFolderVisible(WidgetInstance &widget, bool visible) {
    if (!isPathWidget(widget)) {
        return false;
    }

    if (pathFolderVisible(widget) == visible) {
        return false;
    }

    widget.pathFolderVisible = visible ? std::optional<bool>{} : std::optional<bool>{ false };
    return true;
}

bool pathBrowseButtonVisible(const WidgetInstance &widget) noexcept {
    return isPathWidget(widget) &&
        widget.role == "control" &&
        widget.pathBrowseButtonVisible.value_or(true);
}

bool setPathBrowseButtonVisible(WidgetInstance &widget, bool visible) {
    if (!isPathWidget(widget) || widget.role != "control") {
        return false;
    }

    if (pathBrowseButtonVisible(widget) == visible) {
        return false;
    }

    widget.pathBrowseButtonVisible = visible ? std::optional<bool>{} : std::optional<bool>{ false };
    return true;
}

bool pathScrollbarVisible(const WidgetInstance &widget) noexcept {
    return isPathWidget(widget) && widget.pathScrollbarVisible.value_or(false);
}

bool setPathScrollbarVisible(WidgetInstance &widget, bool visible) {
    if (!isPathWidget(widget)) {
        return false;
    }
    const auto next = visible ? std::optional<bool>{ true } : std::optional<bool>{};
    if (widget.pathScrollbarVisible == next) {
        return false;
    }
    widget.pathScrollbarVisible = next;
    return true;
}

std::string_view pathOverflowMode(const WidgetInstance &widget) noexcept {
    if (!isPathWidget(widget) || widget.pathOverflowMode.value_or("clip") != "abbreviate") {
        return "clip";
    }
    return "abbreviate";
}

bool setPathOverflowMode(WidgetInstance &widget, std::string_view mode) {
    if (!isPathWidget(widget)) {
        return false;
    }
    const auto next = mode == "abbreviate"
        ? std::optional<std::string>{ "abbreviate" }
        : std::optional<std::string>{};
    if (widget.pathOverflowMode == next) {
        return false;
    }
    widget.pathOverflowMode = next;
    return true;
}

bool pathSizeToText(const WidgetInstance &widget) noexcept {
    return isPathWidget(widget) && widget.pathSizeToText.value_or(false);
}

bool setPathSizeToText(WidgetInstance &widget, bool enabled) {
    if (!isPathWidget(widget)) {
        return false;
    }
    const auto next = enabled ? std::optional<bool>{ true } : std::optional<bool>{};
    if (widget.pathSizeToText == next) {
        return false;
    }
    widget.pathSizeToText = next;
    return true;
}

std::string_view pathBrowsePrompt(const WidgetInstance &widget) noexcept {
    return isPathWidget(widget) && widget.pathBrowsePrompt.has_value()
        ? std::string_view(widget.pathBrowsePrompt.value())
        : std::string_view{};
}

bool setPathBrowsePrompt(WidgetInstance &widget, std::string prompt) {
    if (!isPathWidget(widget)) {
        return false;
    }
    const auto next = prompt.empty()
        ? std::optional<std::string>{}
        : std::optional<std::string>{ std::move(prompt) };
    if (widget.pathBrowsePrompt == next) {
        return false;
    }
    widget.pathBrowsePrompt = next;
    return true;
}

std::string_view pathBrowsePatternLabel(const WidgetInstance &widget) noexcept {
    return isPathWidget(widget) && widget.pathBrowsePatternLabel.has_value()
        ? std::string_view(widget.pathBrowsePatternLabel.value())
        : std::string_view{};
}

bool setPathBrowsePatternLabel(WidgetInstance &widget, std::string label) {
    if (!isPathWidget(widget)) {
        return false;
    }
    const auto next = label.empty()
        ? std::optional<std::string>{}
        : std::optional<std::string>{ std::move(label) };
    if (widget.pathBrowsePatternLabel == next) {
        return false;
    }
    widget.pathBrowsePatternLabel = next;
    return true;
}

std::string_view pathBrowsePattern(const WidgetInstance &widget) noexcept {
    return isPathWidget(widget) && widget.pathBrowsePattern.has_value()
        ? std::string_view(widget.pathBrowsePattern.value())
        : std::string_view("*.*");
}

bool setPathBrowsePattern(WidgetInstance &widget, std::string pattern) {
    if (!isPathWidget(widget)) {
        return false;
    }
    if (pattern.empty()) {
        pattern = "*.*";
    }
    const auto next = pattern == "*.*"
        ? std::optional<std::string>{}
        : std::optional<std::string>{ std::move(pattern) };
    if (widget.pathBrowsePattern == next) {
        return false;
    }
    widget.pathBrowsePattern = next;
    return true;
}

std::string_view pathBrowseSelectionMode(const WidgetInstance &widget) noexcept {
    if (!isPathWidget(widget) || !widget.pathBrowseSelectionMode.has_value()) {
        return "files";
    }
    const auto &mode = widget.pathBrowseSelectionMode.value();
    return mode == "folders" || mode == "files_or_folders" ? std::string_view(mode) : std::string_view("files");
}

bool setPathBrowseSelectionMode(WidgetInstance &widget, std::string_view mode) {
    if (!isPathWidget(widget)) {
        return false;
    }
    const std::string normalized = mode == "folders" || mode == "files_or_folders"
        ? std::string(mode)
        : std::string("files");
    const auto next = normalized == "files"
        ? std::optional<std::string>{}
        : std::optional<std::string>{ normalized };
    if (widget.pathBrowseSelectionMode == next) {
        return false;
    }
    widget.pathBrowseSelectionMode = next;
    return true;
}

std::string_view pathBrowseExistenceMode(const WidgetInstance &widget) noexcept {
    if (!isPathWidget(widget) || !widget.pathBrowseExistenceMode.has_value()) {
        return "existing";
    }
    const auto &mode = widget.pathBrowseExistenceMode.value();
    return mode == "new" || mode == "new_or_existing" ? std::string_view(mode) : std::string_view("existing");
}

bool setPathBrowseExistenceMode(WidgetInstance &widget, std::string_view mode) {
    if (!isPathWidget(widget)) {
        return false;
    }
    const std::string normalized = mode == "new" || mode == "new_or_existing"
        ? std::string(mode)
        : std::string("existing");
    const auto next = normalized == "existing"
        ? std::optional<std::string>{}
        : std::optional<std::string>{ normalized };
    if (widget.pathBrowseExistenceMode == next) {
        return false;
    }
    widget.pathBrowseExistenceMode = next;
    return true;
}

std::string_view pathBrowseStartPath(const WidgetInstance &widget) noexcept {
    return isPathWidget(widget) && widget.pathBrowseStartPath.has_value()
        ? std::string_view(widget.pathBrowseStartPath.value())
        : std::string_view{};
}

bool setPathBrowseStartPath(WidgetInstance &widget, std::string path) {
    if (!isPathWidget(widget)) {
        return false;
    }
    const auto next = path.empty()
        ? std::optional<std::string>{}
        : std::optional<std::string>{ std::move(path) };
    if (widget.pathBrowseStartPath == next) {
        return false;
    }
    widget.pathBrowseStartPath = next;
    return true;
}

std::uint32_t minimumArrayVisibleAxisCount(std::uint32_t /*rank*/) noexcept {
    // Rank determines how many indices address a value, not how many cells
    // the user must display. Even a 16D array may show a single cell or row.
    return 1U;
}

WidgetEnabledState widgetEnabledState(const WidgetInstance &widget) noexcept {
    return widget.enabledState.value_or(WidgetEnabledState::Enabled);
}

bool setWidgetEnabledState(WidgetInstance &widget, WidgetEnabledState state) {
    const auto next = state == WidgetEnabledState::Enabled
        ? std::optional<WidgetEnabledState>{}
        : std::optional<WidgetEnabledState>{ state };
    if (widget.enabledState == next) {
        return false;
    }

    widget.enabledState = next;
    return true;
}

std::string_view stringDisplayStyle(const WidgetInstance &widget) noexcept {
    if (!isStringWidget(widget)) {
        return "normal";
    }
    if (widget.stringDisplayStyle.has_value()) {
        const auto &style = *widget.stringDisplayStyle;
        if (style == "backslash_codes" || style == "password" || style == "hexadecimal") {
            return style;
        }
    }
    return "normal";
}

bool stringDisplayStyleVisible(const WidgetInstance &widget) noexcept {
    return isStringWidget(widget) && widget.stringDisplayStyleVisible.value_or(false);
}

bool stringLimitToSingleLine(const WidgetInstance &widget) noexcept {
    return !isStringWidget(widget) || widget.stringLimitToSingleLine.value_or(true);
}

bool stringWrapAtWordBreaks(const WidgetInstance &widget) noexcept {
    return isStringWidget(widget) && widget.stringWrapAtWordBreaks.value_or(true);
}

bool stringUpdateWhileTyping(const WidgetInstance &widget) noexcept {
    return !isStringWidget(widget) || widget.stringUpdateWhileTyping.value_or(true);
}

bool stringVerticalScrollbarVisible(const WidgetInstance &widget) noexcept {
    return isStringWidget(widget) && widget.stringVerticalScrollbarVisible.value_or(false);
}

bool stringHorizontalScrollbarVisible(const WidgetInstance &widget) noexcept {
    return isStringWidget(widget) && widget.stringHorizontalScrollbarVisible.value_or(false);
}

bool stringSizeToText(const WidgetInstance &widget) noexcept {
    return isStringWidget(widget) && widget.stringSizeToText.value_or(false);
}

bool setWidgetSupportText(WidgetInstance &widget, std::string text) {
    if (!isLabelWidget(widget)) {
        return false;
    }
    if (widget.supportTextValue.has_value() && widget.supportTextValue.value() == text) {
        return false;
    }
    widget.supportTextValue = std::move(text);
    return true;
}

bool setStringDisplayStyle(WidgetInstance &widget, std::string_view style) {
    if (!isStringWidget(widget)) {
        return false;
    }
    auto normalized = style == "backslash_codes" || style == "password" || style == "hexadecimal"
        ? std::string(style)
        : std::string("normal");
    const auto next = normalized == "normal"
        ? std::optional<std::string>{}
        : std::optional<std::string>{ std::move(normalized) };
    if (widget.stringDisplayStyle == next) {
        return false;
    }
    widget.stringDisplayStyle = next;
    return true;
}

bool setStringDisplayStyleVisible(WidgetInstance &widget, bool visible) {
    if (!isStringWidget(widget)) {
        return false;
    }
    const auto next = visible ? std::optional<bool>{ true } : std::optional<bool>{};
    if (widget.stringDisplayStyleVisible == next) {
        return false;
    }
    widget.stringDisplayStyleVisible = next;
    return true;
}

bool setStringLimitToSingleLine(WidgetInstance &widget, bool enabled) {
    if (!isStringWidget(widget)) {
        return false;
    }
    const auto next = enabled ? std::optional<bool>{} : std::optional<bool>{ false };
    if (widget.stringLimitToSingleLine == next) {
        return false;
    }
    widget.stringLimitToSingleLine = next;
    return true;
}

bool setStringWrapAtWordBreaks(WidgetInstance &widget, bool enabled) {
    if (!isStringWidget(widget)) {
        return false;
    }
    const auto next = enabled ? std::optional<bool>{} : std::optional<bool>{ false };
    if (widget.stringWrapAtWordBreaks == next) {
        return false;
    }
    widget.stringWrapAtWordBreaks = next;
    return true;
}

bool setStringUpdateWhileTyping(WidgetInstance &widget, bool enabled) {
    if (!isStringWidget(widget)) {
        return false;
    }
    const auto next = enabled ? std::optional<bool>{} : std::optional<bool>{ false };
    if (widget.stringUpdateWhileTyping == next) {
        return false;
    }
    widget.stringUpdateWhileTyping = next;
    return true;
}

bool setStringVerticalScrollbarVisible(WidgetInstance &widget, bool visible) {
    if (!isStringWidget(widget)) {
        return false;
    }
    const auto next = visible ? std::optional<bool>{ true } : std::optional<bool>{};
    if (widget.stringVerticalScrollbarVisible == next) {
        return false;
    }
    widget.stringVerticalScrollbarVisible = next;
    return true;
}

bool setStringHorizontalScrollbarVisible(WidgetInstance &widget, bool visible) {
    if (!isStringWidget(widget)) {
        return false;
    }
    const auto next = visible ? std::optional<bool>{ true } : std::optional<bool>{};
    if (widget.stringHorizontalScrollbarVisible == next) {
        return false;
    }
    widget.stringHorizontalScrollbarVisible = next;
    return true;
}

bool setStringSizeToText(WidgetInstance &widget, bool enabled) {
    if (!isStringWidget(widget)) {
        return false;
    }
    const auto next = enabled ? std::optional<bool>{ true } : std::optional<bool>{};
    if (widget.stringSizeToText == next) {
        return false;
    }
    widget.stringSizeToText = next;
    return true;
}

bool widgetLabelVisible(const WidgetInstance &widget) noexcept {
    return widget.labelVisible.value_or(true);
}

bool setWidgetLabelVisible(WidgetInstance &widget, bool visible) {
    if (widgetLabelVisible(widget) == visible) {
        return false;
    }

    widget.labelVisible = visible ? std::optional<bool>{} : std::optional<bool>{ false };
    return true;
}

bool numericRadixVisible(const WidgetInstance &widget) noexcept {
    if (widget.numericColorBox && widget.valueType == "u32") return false;
    return isNumericWidget(widget) && widget.numericRadixVisible.value_or(false);
}

bool setNumericRadixVisible(WidgetInstance &widget, bool visible) {
    if (!isNumericWidget(widget) || numericRadixVisible(widget) == visible) {
        return false;
    }

    widget.numericRadixVisible = visible ? std::optional<bool>{ true } : std::optional<bool>{};
    return true;
}

bool widgetDisabledAppearanceGrayed(const WidgetInstance &widget) noexcept {
    return widgetEnabledState(widget) == WidgetEnabledState::DisabledAndGrayed;
}

} // namespace frog::native_engine
