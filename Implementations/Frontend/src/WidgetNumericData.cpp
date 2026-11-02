// SPDX-License-Identifier: Apache-2.0
#include "frog_native_engine/WidgetInstance.hpp"
#include "frog_native_engine/WidgetNumericData.hpp"
#include "frog_native_engine/NumericTextFormatting.hpp"
#include "frog_native_engine/NumericRadixFormat.hpp"
#include "frog_native_engine/NumericValueLexeme.hpp"
#include "frog_native_engine/WidgetDataOperations.hpp"
#include "frog_native_engine/UnavailableNumericRepresentation.hpp"
#include <algorithm>
#include <array>
#include <charconv>
#include <cmath>
#include <cctype>
#include <iomanip>
#include <limits>
#include <locale>
#include <sstream>

namespace frog::native_engine {
namespace widget_numeric_detail {


constexpr std::array<NumericRepresentationMapping, 28> kNumericRepresentationMappings{{
    // Timestamp retains the historical numeric storage representation for file
    // compatibility; the UI uses its dedicated date/time renderer and editor.
    { "timestamp", "timestamp", "Timestamp" },
    { "float16", "f16", "Float16" },
    { "bfloat16", "bf16", "BFloat16" },
    { "float32", "f32", "Float32" },
    { "float64", "f64", "Float64" },
    { "int8", "i8", "Int8" },
    { "int16", "i16", "Int16" },
    { "int32", "i32", "Int32" },
    { "int64", "i64", "Int64" },
    { "uint8", "u8", "UInt8" },
    { "uint16", "u16", "UInt16" },
    { "uint32", "u32", "UInt32" },
    { "uint64", "u64", "UInt64" },
    { "complex_float32", "c32", "Complex<Float32>" },
    { "complex_float64", "c64", "Complex<Float64>" },
    { "fixed_point", "fxp", "FixedPoint<signed,totalBits,fractionalBits,roundingMode,overflowMode>" },
    { "decimal", "decimal", "Decimal<precision,scale>" },
    { "int4", "i4", "Int4" },
    { "uint4", "u4", "UInt4" },
    { "float8_e4m3", "f8e4m3", "Float8E4M3" },
    { "float8_e5m2", "f8e5m2", "Float8E5M2" },
    { "int128", "i128", "Int128" },
    { "uint128", "u128", "UInt128" },
    { "float80", "f80", "Float80" },
    { "float128", "f128", "Float128" },
    { "big_int", "bigint", "BigInt" },
    { "big_uint", "biguint", "BigUInt" },
    { "big_decimal", "bigdecimal", "BigDecimal" }
}};

[[nodiscard]] std::string lowercaseAscii(std::string_view value) {
    std::string normalized;
    normalized.reserve(value.size());
    for (const char ch : value) {
        normalized.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
    }
    return normalized;
}

[[nodiscard]] const NumericRepresentationMapping *numericRepresentationMapping(std::string_view value) noexcept {
    const std::string normalized = lowercaseAscii(value);
    auto found = std::find_if(
        kNumericRepresentationMappings.begin(),
        kNumericRepresentationMappings.end(),
        [&normalized](const NumericRepresentationMapping &candidate) {
            return candidate.kind == normalized ||
                candidate.valueType == normalized ||
                lowercaseAscii(candidate.namedSize) == normalized;
        });
    if (found != kNumericRepresentationMappings.end()) {
        return &*found;
    }

    // Historical LabVIEW-style names remain accepted when loading older .frog files.
    constexpr std::array<std::pair<std::string_view, std::string_view>, 14> aliases{{
        { "extended", "float80" },
        { "ext", "float80" },
        { "double", "float64" },
        { "dbl", "float64" },
        { "single", "float32" },
        { "sgl", "float32" },
        { "complex_double", "complex_float64" },
        { "cdb", "complex_float64" },
        { "complex_single", "complex_float32" },
        { "csg", "complex_float32" },
        { "f8p", "float8_e4m3" },
        { "f8r", "float8_e5m2" },
        { "fixedpoint", "fixed_point" },
        { "bigdecimal", "big_decimal" }
    }};
    const auto alias = std::find_if(
        aliases.begin(),
        aliases.end(),
        [&normalized](const auto &candidate) { return candidate.first == normalized; });
    if (alias == aliases.end()) {
        return nullptr;
    }
    found = std::find_if(
        kNumericRepresentationMappings.begin(),
        kNumericRepresentationMappings.end(),
        [&alias](const NumericRepresentationMapping &candidate) {
            return candidate.kind == alias->second;
        });
    return found == kNumericRepresentationMappings.end() ? nullptr : &*found;
}

[[nodiscard]] bool numericValueTypeIsIntegral(std::string_view valueType) noexcept {
    const auto *mapping = numericRepresentationMapping(valueType);
    return mapping != nullptr && numericCanonicalValueTypeIsIntegral(mapping->valueType);
}

} // namespace
using namespace widget_numeric_detail;
std::string formatNumericValue(double value) {
    if (std::isnan(value)) {
        return "NaN";
    }
    if (std::isinf(value)) {
        return std::signbit(value) ? "-Inf" : "Inf";
    }
    if (value == 0.0) {
        return "0";
    }
    std::ostringstream output;
    output.imbue(std::locale::classic());
    output << std::setprecision(15) << std::defaultfloat << value;
    return output.str();
}
bool numericCanonicalValueTypeIsComplex(std::string_view type) noexcept {
    return type == "c32" || type == "c64";
}

std::optional<std::string> widgetNumericComplexValueLexeme(const WidgetInstance &widget) {
    if (!isNumericWidget(widget) || !numericCanonicalValueTypeIsComplex(widget.valueType)) return {};
    if (widget.initialValue) {
        const auto value = numeric_value::parseComplexNumber(*widget.initialValue, widget.valueType);
        if (!value || (widget.numericValue && (*widget.numericValue != value->real ||
            (*widget.numericValue == 0 && std::signbit(*widget.numericValue) != std::signbit(value->real))))) return {};
        return widget.initialValue;
    }
    return numeric_value::complexNumberLexeme({widget.numericValue.value_or(0.0), 0.0}, widget.valueType);
}

bool setWidgetNumericComplexValueLexeme(WidgetInstance &widget, std::string_view text) {
    if (!isNumericWidget(widget)) return false;
    const auto value = numeric_value::parseComplexNumber(text, widget.valueType);
    if (!value) return false;
    std::string source(text);
    widget.initialValue = std::move(source);
    widget.numericValue = value->real;
    return true;
}

double widgetNumericValue(const WidgetInstance &widget) noexcept {
    return widget.numericValue.value_or(0.0);
}

bool numericCanonicalValueTypeIsIntegral(std::string_view valueType) noexcept {
    return valueType == "i4" || valueType == "i8" || valueType == "i16" ||
        valueType == "i32" || valueType == "i64" || valueType == "i128" ||
        valueType == "u4" || valueType == "u8" || valueType == "u16" ||
        valueType == "u32" || valueType == "u64" || valueType == "u128" ||
        valueType == "bigint" || valueType == "biguint";
}

double numericMinimumForValueType(std::string_view valueType) noexcept {
    if (valueType == "u4" || valueType == "u8" || valueType == "u16" ||
        valueType == "u32" || valueType == "u64" || valueType == "u128" ||
        valueType == "biguint") {
        return 0.0;
    }
    if (valueType == "i4") {
        return -8.0;
    }
    if (valueType == "i8") {
        return -128.0;
    }
    if (valueType == "i16") {
        return -32768.0;
    }
    if (valueType == "i32") {
        return static_cast<double>((std::numeric_limits<std::int32_t>::min)());
    }
    if (valueType == "i64") {
        return static_cast<double>((std::numeric_limits<std::int64_t>::min)());
    }
    if (valueType == "f16") {
        return -65504.0;
    }
    if (valueType == "f8e4m3") {
        return -448.0;
    }
    if (valueType == "f8e5m2") {
        return -57344.0;
    }
    if (valueType == "bf16" || valueType == "f32") {
        return -static_cast<double>((std::numeric_limits<float>::max)());
    }
    return (std::numeric_limits<double>::lowest)();
}

double numericMaximumForValueType(std::string_view valueType) noexcept {
    if (valueType == "u4") {
        return 15.0;
    }
    if (valueType == "u8") {
        return 255.0;
    }
    if (valueType == "u16") {
        return 65535.0;
    }
    if (valueType == "u32") {
        return static_cast<double>((std::numeric_limits<std::uint32_t>::max)());
    }
    if (valueType == "i4") {
        return 7.0;
    }
    if (valueType == "i8") {
        return 127.0;
    }
    if (valueType == "i16") {
        return 32767.0;
    }
    if (valueType == "i32") {
        return static_cast<double>((std::numeric_limits<std::int32_t>::max)());
    }
    if (valueType == "i64") {
        return static_cast<double>((std::numeric_limits<std::int64_t>::max)());
    }
    if (valueType == "u64") {
        return static_cast<double>((std::numeric_limits<std::uint64_t>::max)());
    }
    if (valueType == "f16") {
        return 65504.0;
    }
    if (valueType == "f8e4m3") {
        return 448.0;
    }
    if (valueType == "f8e5m2") {
        return 57344.0;
    }
    if (valueType == "bf16" || valueType == "f32") {
        return static_cast<double>((std::numeric_limits<float>::max)());
    }
    return (std::numeric_limits<double>::max)();
}

std::optional<double> normalizeNumericEntryValue(
    const WidgetInstance &widget,
    double value) noexcept {
    if ((!isNumericWidget(widget) && !isRingWidget(widget)) || std::isnan(value)) {
        return std::nullopt;
    }

    const auto minimumResponse = widget.numericMinimumResponse.value_or("ignore");
    const auto maximumResponse = widget.numericMaximumResponse.value_or("ignore");
    const bool fixedPoint = numericRepresentationKind(widget) == "fixed_point";
    const auto fixedLimits = fixedPoint
        ? fixedPointLimits(fixedPointConfiguration(widget))
        : FixedPointLimits{};
    const double supportedMinimum = fixedPoint
        ? fixedLimits.minimum
        : numericMinimumForValueType(widget.valueType);
    const double supportedMaximum = fixedPoint
        ? fixedLimits.maximum
        : numericMaximumForValueType(widget.valueType);
    if ((widget.numericMinimum.has_value() && widget.numericMinimum.value() < supportedMinimum) ||
        (widget.numericMaximum.has_value() && widget.numericMaximum.value() > supportedMaximum) ||
        (widget.numericMinimum.has_value() && widget.numericMaximum.has_value() &&
            widget.numericMinimum.value() > widget.numericMaximum.value())) {
        return std::nullopt;
    }
    value = std::clamp(value, supportedMinimum, supportedMaximum);

    const auto coerceConfiguredBounds = [&]() noexcept {
        if (widget.numericMinimum.has_value() && value < widget.numericMinimum.value() &&
            minimumResponse == "coerce") {
            value = widget.numericMinimum.value();
        }
        if (widget.numericMaximum.has_value() && value > widget.numericMaximum.value() &&
            maximumResponse == "coerce") {
            value = widget.numericMaximum.value();
        }
    };

    coerceConfiguredBounds();

    const auto equalsConfiguredBoundary = [value](const std::optional<double> &boundary) noexcept {
        if (!boundary.has_value()) {
            return false;
        }
        const double scale = (std::max)({ 1.0, std::abs(value), std::abs(boundary.value()) });
        return std::abs(value - boundary.value()) <= std::numeric_limits<double>::epsilon() * scale * 8.0;
    };
    const bool preserveConfiguredBoundary =
        equalsConfiguredBoundary(widget.numericMinimum) ||
        equalsConfiguredBoundary(widget.numericMaximum);

    const auto incrementResponse = widget.numericIncrementResponse.value_or("ignore");
    const double increment = widget.numericIncrementStep.value_or(0.0);
    if (!preserveConfiguredBoundary &&
        incrementResponse != "ignore" &&
        std::isfinite(increment) &&
        increment > 0.0) {
        const double origin = widget.numericMinimum.value_or(0.0);
        const double position = (value - origin) / increment;
        if (std::isfinite(position)) {
            if (incrementResponse == "nearest") {
                value = origin + std::round(position) * increment;
            } else if (incrementResponse == "up") {
                value = origin + std::ceil(position) * increment;
            } else if (incrementResponse == "down") {
                value = origin + std::floor(position) * increment;
            }
        }
    }

    if (fixedPoint) {
        value = coerceFixedPointValue(value, fixedPointConfiguration(widget));
    } else if (numericValueTypeIsIntegral(widget.valueType)) {
        if (incrementResponse == "up") {
            value = std::ceil(value);
        } else if (incrementResponse == "down") {
            value = std::floor(value);
        } else {
            value = std::round(value);
        }
    }

    coerceConfiguredBounds();
    if (!std::isfinite(value) || value < supportedMinimum || value > supportedMaximum ||
        (numericValueTypeIsIntegral(widget.valueType) && std::trunc(value) != value)) {
        return std::nullopt;
    }
    return value;
}

std::optional<double> normalizeNumericUserInput(
    const WidgetInstance &widget,
    double value) noexcept {
    return normalizeNumericEntryValue(widget, value);
}

std::string numericRepresentationKind(const WidgetInstance &widget) {
    if(!unavailableComplexNumericIdentity(widget.valueType).empty())return widget.valueType;
    if(widget.numericRepresentationKind&&!unavailableComplexNumericIdentity(*widget.numericRepresentationKind).empty()&&widget.valueType!="c32"&&widget.valueType!="c64")return *widget.numericRepresentationKind;
    if (widget.numericRepresentationKind.has_value() && !widget.numericRepresentationKind->empty()) {
        if (const auto *mapping = numericRepresentationMapping(widget.numericRepresentationKind.value())) {
            return std::string(mapping->kind);
        }
    }
    if (const auto *mapping = numericRepresentationMapping(widget.valueType)) {
        return std::string(mapping->kind);
    }
    return widget.valueType.empty() ? std::string{ "float64" } : widget.valueType;
}

FixedPointConfiguration defaultFixedPointConfiguration() noexcept {
    return {};
}

FixedPointConfiguration normalizedFixedPointConfiguration(
    FixedPointConfiguration configuration) noexcept {
    configuration.wordLength = std::clamp(configuration.wordLength, 1, 128);
    configuration.integerWordLength = std::clamp(configuration.integerWordLength, -256, 256);
    if (configuration.roundingMode != "nearest" &&
        configuration.roundingMode != "up" &&
        configuration.roundingMode != "down" &&
        configuration.roundingMode != "toward_zero") {
        configuration.roundingMode = "nearest";
    }
    if (configuration.overflowMode != "saturate" && configuration.overflowMode != "wrap") {
        configuration.overflowMode = "saturate";
    }
    return configuration;
}

FixedPointConfiguration fixedPointConfiguration(const WidgetInstance &widget) noexcept {
    return normalizedFixedPointConfiguration(
        widget.numericFixedPointConfiguration.value_or(defaultFixedPointConfiguration()));
}

FixedPointLimits fixedPointLimits(const FixedPointConfiguration &configuration) noexcept {
    const auto normalized = normalizedFixedPointConfiguration(configuration);
    const int fractionalBits = normalized.wordLength - normalized.integerWordLength;
    const double delta = std::ldexp(1.0, -fractionalBits);
    FixedPointLimits limits;
    limits.delta = delta;
    limits.fractionalBits = fractionalBits;
    if (normalized.signedEncoding) {
        const double magnitude = std::ldexp(1.0, normalized.integerWordLength - 1);
        limits.minimum = -magnitude;
        limits.maximum = magnitude - delta;
    } else {
        limits.minimum = 0.0;
        limits.maximum = std::ldexp(1.0, normalized.integerWordLength) - delta;
    }
    return limits;
}

double coerceFixedPointValue(
    double value,
    const FixedPointConfiguration &configuration) noexcept {
    const auto normalized = normalizedFixedPointConfiguration(configuration);
    const auto limits = fixedPointLimits(normalized);
    if (!std::isfinite(value) || !std::isfinite(limits.delta) || limits.delta <= 0.0) {
        return 0.0;
    }

    if (normalized.overflowMode == "wrap") {
        const double span = limits.maximum - limits.minimum + limits.delta;
        if (std::isfinite(span) && span > 0.0) {
            value = std::fmod(value - limits.minimum, span);
            if (value < 0.0) {
                value += span;
            }
            value += limits.minimum;
        }
    }
    value = std::clamp(value, limits.minimum, limits.maximum);

    const double scaled = value / limits.delta;
    double quantized;
    if (normalized.roundingMode == "up") {
        quantized = std::ceil(scaled);
    } else if (normalized.roundingMode == "down") {
        quantized = std::floor(scaled);
    } else if (normalized.roundingMode == "toward_zero") {
        quantized = std::trunc(scaled);
    } else {
        quantized = std::round(scaled);
    }
    return std::clamp(quantized * limits.delta, limits.minimum, limits.maximum);
}

bool fixedPointConfigurationsEqual(
    const FixedPointConfiguration &left,
    const FixedPointConfiguration &right) noexcept {
    const auto normalizedLeft = normalizedFixedPointConfiguration(left);
    const auto normalizedRight = normalizedFixedPointConfiguration(right);
    return normalizedLeft.signedEncoding == normalizedRight.signedEncoding &&
        normalizedLeft.complex == normalizedRight.complex &&
        normalizedLeft.wordLength == normalizedRight.wordLength &&
        normalizedLeft.integerWordLength == normalizedRight.integerWordLength &&
        normalizedLeft.roundingMode == normalizedRight.roundingMode &&
        normalizedLeft.overflowMode == normalizedRight.overflowMode;
}

bool setNumericRepresentationKind(WidgetInstance &widget, std::string_view representation) {
    // An unavailable lexical Complex must be recovered/replaced explicitly,
    // never converted from its scalar display cache or invented zero.
    if(!unavailableComplexNumericIdentity(widget.valueType).empty())return false;
    if (!isNumericWidget(widget) || representation.empty()) {
        return false;
    }

    const auto *mapping = numericRepresentationMapping(representation);
    if (mapping == nullptr) {
        return false;
    }

    std::optional<numeric_value::ComplexNumber> complexValue;
    if (numericCanonicalValueTypeIsComplex(widget.valueType)) {
        const auto source = widgetNumericComplexValueLexeme(widget);
        complexValue = source ? numeric_value::parseComplexNumber(*source, widget.valueType) : std::nullopt;
        if (!complexValue || (!numericCanonicalValueTypeIsComplex(mapping->valueType) && complexValue->imaginary != 0)) return false;
    }
    if (numericCanonicalValueTypeIsComplex(mapping->valueType)) {
        const auto source = complexValue ? numeric_value::complexNumberLexeme(*complexValue, mapping->valueType)
            : numeric_value::complexNumberLexeme({widget.numericValue.value_or(0.0), 0.0}, mapping->valueType);
        if (!source) return false;
        complexValue = numeric_value::parseComplexNumber(*source, mapping->valueType);
    }
    const bool previousFixedPoint = numericRepresentationKind(widget) == "fixed_point";
    const auto previousFixedLimits = previousFixedPoint
        ? fixedPointLimits(fixedPointConfiguration(widget))
        : FixedPointLimits{};
    const auto previousSupportedMinimum = previousFixedPoint
        ? previousFixedLimits.minimum
        : numericMinimumForValueType(widget.valueType);
    const auto previousSupportedMaximum = previousFixedPoint
        ? previousFixedLimits.maximum
        : numericMaximumForValueType(widget.valueType);
    const bool minimumTracksRepresentation =
        widget.numericMinimum.has_value() && widget.numericMinimum.value() == previousSupportedMinimum;
    const bool maximumTracksRepresentation =
        widget.numericMaximum.has_value() && widget.numericMaximum.value() == previousSupportedMaximum;
    std::string nextValueType(mapping->valueType);
    std::string nextRepresentation(mapping->kind);
    if (widget.numericRepresentationKind == nextRepresentation &&
        widget.valueType == nextValueType &&
        (nextRepresentation != "fixed_point" || widget.numericFixedPointConfiguration.has_value())) {
        return false;
    }

    std::optional<std::string> convertedDefault;
    if (widget.defaultData) {
        // A saved default is an independently typed value, not the current
        // display cache. Convert it through the same Numeric rules before
        // publishing either change; otherwise Save rejects the old type and
        // silently projects the new type's intrinsic zero instead.
        auto saved=data_operations::decode(*widget.defaultData);
        if (!saved || !isNumericWidget(*saved)) return false;
        // decode sanitizes saved defaults, so this conversion cannot recurse
        // through another default payload or replace it with the live value.
        if (!setNumericRepresentationKind(*saved,mapping->kind) &&
            (saved->valueType!=nextValueType || numericRepresentationKind(*saved)!=mapping->kind)) return false;
        convertedDefault=data_operations::encode(*saved);
        if (convertedDefault->empty()) return false;
    }

    widget.numericRepresentationKind = std::move(nextRepresentation);
    widget.valueType = std::move(nextValueType);
    if (complexValue) {
        widget.numericValue = complexValue->real;
        if (numericCanonicalValueTypeIsComplex(widget.valueType))
            widget.initialValue = *numeric_value::complexNumberLexeme(*complexValue, widget.valueType);
        else {
            std::array<char, 64> buffer{};
            const auto result = std::to_chars(buffer.data(), buffer.data() + buffer.size(), complexValue->real);
            widget.initialValue = std::string(buffer.data(), result.ptr);
        }
    }
    if (widget.valueType != "u32") widget.numericColorBox = false;
    const bool nextFixedPoint = widget.numericRepresentationKind == "fixed_point";
    if (nextFixedPoint) {
        widget.numericFixedPointConfiguration = fixedPointConfiguration(widget);
    }
    const auto nextFixedLimits = nextFixedPoint
        ? fixedPointLimits(fixedPointConfiguration(widget))
        : FixedPointLimits{};
    const auto supportedMinimum = nextFixedPoint
        ? nextFixedLimits.minimum
        : numericMinimumForValueType(widget.valueType);
    const auto supportedMaximum = nextFixedPoint
        ? nextFixedLimits.maximum
        : numericMaximumForValueType(widget.valueType);
    if (minimumTracksRepresentation ||
        (widget.numericMinimum.has_value() && widget.numericMinimum.value() < supportedMinimum)) {
        widget.numericMinimum = supportedMinimum;
    }
    if (maximumTracksRepresentation ||
        (widget.numericMaximum.has_value() && widget.numericMaximum.value() > supportedMaximum)) {
        widget.numericMaximum = supportedMaximum;
    }
    if (widget.numericValue.has_value()) {
        const auto clamped = std::clamp(widget.numericValue.value(), supportedMinimum, supportedMaximum);
        const auto normalized = nextFixedPoint
            ? coerceFixedPointValue(clamped, fixedPointConfiguration(widget))
            : (numericValueTypeIsIntegral(widget.valueType) ? std::trunc(clamped) : clamped);
        if (normalized != widget.numericValue.value()) {
            widget.numericValue = normalized;
            widget.initialValue = formatNumericValue(normalized);
        }
    }
    if (convertedDefault) widget.defaultData=std::move(*convertedDefault);
    return true;
}

bool setWidgetFixedPointConfiguration(
    WidgetInstance &widget,
    const FixedPointConfiguration &configuration) {
    if (!isNumericWidget(widget)) {
        return false;
    }

    const bool wasFixedPoint = numericRepresentationKind(widget) == "fixed_point";
    const auto previousConfiguration = fixedPointConfiguration(widget);
    const auto previousLimits = fixedPointLimits(previousConfiguration);
    if (!wasFixedPoint && !setNumericRepresentationKind(widget, "fixed_point")) {
        return false;
    }

    const auto normalized = normalizedFixedPointConfiguration(configuration);
    const bool configurationChanged =
        !widget.numericFixedPointConfiguration.has_value() ||
        !fixedPointConfigurationsEqual(widget.numericFixedPointConfiguration.value(), normalized);
    if (!configurationChanged && wasFixedPoint) {
        return false;
    }

    const bool minimumTracksConfiguration =
        !widget.numericMinimum.has_value() ||
        widget.numericMinimum.value() == previousLimits.minimum;
    const bool maximumTracksConfiguration =
        !widget.numericMaximum.has_value() ||
        widget.numericMaximum.value() == previousLimits.maximum;
    widget.numericFixedPointConfiguration = normalized;
    widget.numericRepresentationKind = "fixed_point";
    widget.valueType = "fxp";
    widget.numericColorBox = false;
    const auto limits = fixedPointLimits(normalized);
    if (minimumTracksConfiguration || widget.numericMinimum.value_or(limits.minimum) < limits.minimum) {
        widget.numericMinimum = limits.minimum;
    }
    if (maximumTracksConfiguration || widget.numericMaximum.value_or(limits.maximum) > limits.maximum) {
        widget.numericMaximum = limits.maximum;
    }
    if (widget.numericValue.has_value()) {
        widget.numericValue = coerceFixedPointValue(widget.numericValue.value(), normalized);
        widget.initialValue = formatNumericValue(widget.numericValue.value());
    }
    return true;
}

std::string enumRepresentationKind(const WidgetInstance &widget) {
    if (isEnumWidget(widget) && widget.numericRepresentationKind.has_value()) {
        if (const auto *mapping = numericRepresentationMapping(widget.numericRepresentationKind.value());
            mapping != nullptr &&
            (mapping->kind == "uint8" || mapping->kind == "uint16" || mapping->kind == "uint32")) {
            return std::string(mapping->kind);
        }
    }
    return "uint16";
}

std::string enumRepresentationValueType(const WidgetInstance &widget) {
    if (const auto *mapping = numericRepresentationMapping(enumRepresentationKind(widget))) {
        return std::string(mapping->valueType);
    }
    return "u16";
}

bool setEnumRepresentationKind(WidgetInstance &widget, std::string_view representation) {
    if (!isEnumWidget(widget) || representation.empty()) {
        return false;
    }

    const auto *mapping = numericRepresentationMapping(representation);
    if (mapping == nullptr ||
        (mapping->kind != "uint8" && mapping->kind != "uint16" && mapping->kind != "uint32")) {
        return false;
    }

    const std::string nextRepresentation(mapping->kind);
    if (enumRepresentationKind(widget) == nextRepresentation &&
        widget.numericRepresentationKind == nextRepresentation) {
        return false;
    }

    widget.numericRepresentationKind = nextRepresentation;
    return true;
}

bool ringRepresentationSupported(std::string_view representation) noexcept {
    return representation == "float80" ||
        representation == "float64" ||
        representation == "float32" ||
        representation == "int64" ||
        representation == "int32" ||
        representation == "int16" ||
        representation == "int8" ||
        representation == "uint64" ||
        representation == "uint32" ||
        representation == "uint16" ||
        representation == "uint8";
}

std::string ringRepresentationKind(const WidgetInstance &widget) {
    if (isRingWidget(widget) && widget.numericRepresentationKind.has_value()) {
        if (const auto *mapping = numericRepresentationMapping(widget.numericRepresentationKind.value());
            mapping != nullptr && ringRepresentationSupported(mapping->kind)) {
            return std::string(mapping->kind);
        }
    }
    if (isRingWidget(widget)) {
        if (const auto *mapping = numericRepresentationMapping(widget.valueType);
            mapping != nullptr && ringRepresentationSupported(mapping->kind)) {
            return std::string(mapping->kind);
        }
    }
    return "uint16";
}

std::string ringRepresentationValueType(const WidgetInstance &widget) {
    if (const auto *mapping = numericRepresentationMapping(ringRepresentationKind(widget))) {
        return std::string(mapping->valueType);
    }
    return "u16";
}

bool setRingRepresentationKind(WidgetInstance &widget, std::string_view representation) {
    if (!isRingWidget(widget) || representation.empty()) {
        return false;
    }

    const auto *mapping = numericRepresentationMapping(representation);
    if (mapping == nullptr || !ringRepresentationSupported(mapping->kind)) {
        return false;
    }

    const std::string nextRepresentation(mapping->kind);
    const std::string nextValueType(mapping->valueType);
    if (ringRepresentationKind(widget) == nextRepresentation &&
        widget.numericRepresentationKind == nextRepresentation &&
        widget.valueType == nextValueType) {
        return false;
    }

    widget.numericRepresentationKind = nextRepresentation;
    widget.valueType = nextValueType;
    return true;
}

std::optional<double> numericIntegerLexemeProjection(const WidgetInstance &widget,std::string_view text) {
    if(!isNumericWidget(widget) || !numericCanonicalValueTypeIsIntegral(widget.valueType) ||
        text.size()>numeric_value::maximumDecimalNumberBytes)return {};
    const auto consumed=numeric_value::scanDecimalNumber(text);
    if(!consumed || *consumed!=text.size())return {};
    const bool wide=numeric_value::canonicalWideIntegerLexeme("0",widget.valueType).has_value();
    if(wide)return numeric_value::wideIntegerCacheProjection(text,widget.valueType);
    // Fixed-width domains below U64 fit the existing exact i128 transport.
    // Reuse existing type bounds rather than duplicating their table here.
    // BigInt/BigUInt retain their historical double route, outside this proof.
    if(numericMaximumForValueType(widget.valueType)<=numericMaximumForValueType("u32") &&
        !numeric_value::canonicalWideIntegerLexeme(text,"i128"))return {};
    double projection=0;
    const auto result=std::from_chars(text.data(),text.data()+text.size(),projection);
    if(result.ec!=std::errc{} || result.ptr!=text.data()+text.size() || !std::isfinite(projection))return {};
    auto checked=widget;
    if(!setWidgetNumericValue(checked,projection))return {};
    return projection;
}

std::optional<std::string> widgetNumericIntegerValueLexeme(const WidgetInstance &widget) {
    if(!isNumericWidget(widget) || !numericCanonicalValueTypeIsIntegral(widget.valueType))return {};
    if(widget.initialValue) {
        const auto projection=numericIntegerLexemeProjection(widget,*widget.initialValue);
        if(!projection)return {}; // An optional cache cannot validate a malformed source lexeme.
        const bool sameCache=!widget.numericValue ||
            (*projection==*widget.numericValue && (*projection!=0 ||
                std::signbit(*projection)==std::signbit(*widget.numericValue)));
        if(sameCache)return widget.initialValue;
        if(!widget.numericValue)return {};
    }
    const double value=widgetNumericValue(widget);
    if(!std::isfinite(value) || std::trunc(value)!=value)return {};
    std::array<char,64> buffer{};
    const auto result=std::to_chars(buffer.data(),buffer.data()+buffer.size(),value,std::chars_format::general);
    if(result.ec!=std::errc{})return {};
    std::string text(buffer.data(),result.ptr);
    if(!numericIntegerLexemeProjection(widget,text))return {};
    return text;
}

bool setWidgetNumericIntegerValueLexeme(WidgetInstance &widget,std::string_view text) {
    const auto projection=numericIntegerLexemeProjection(widget,text);
    if(!projection)return false;
    // Validate completely before touching either field. The spelling remains
    // authoritative even when the optional display cache cannot represent it.
    std::string lexical{text};
    widget.initialValue=std::move(lexical);widget.numericValue=*projection;
    return true;
}

bool setWidgetNumericValue(WidgetInstance &widget, double value) {
    if (isNumericWidget(widget) && numericCanonicalValueTypeIsComplex(widget.valueType)) {
        const auto text = numeric_value::complexNumberLexeme({value, 0.0}, widget.valueType);
        return text && setWidgetNumericComplexValueLexeme(widget, *text);
    }
    if (!isNumericWidget(widget) || !std::isfinite(value)) {
        return false;
    }

    const bool fixedPoint = numericRepresentationKind(widget) == "fixed_point";
    const auto fixedLimits = fixedPoint
        ? fixedPointLimits(fixedPointConfiguration(widget))
        : FixedPointLimits{};
    const auto minimum = fixedPoint
        ? fixedLimits.minimum
        : numericMinimumForValueType(widget.valueType);
    const auto maximum = fixedPoint
        ? fixedLimits.maximum
        : numericMaximumForValueType(widget.valueType);
    if (value < minimum || value > maximum ||
        (!fixedPoint && numericValueTypeIsIntegral(widget.valueType) && std::trunc(value) != value)) {
        return false;
    }

    const double normalized = fixedPoint
        ? coerceFixedPointValue(value, fixedPointConfiguration(widget))
        : value;
    widget.numericValue = normalized;
    if(widget.valueType=="timestamp") {
        // Persist the actual binary64 seconds without the display formatter's
        //15-digit rounding. This changes no display precision or geometry.
        std::ostringstream initial;initial.imbue(std::locale::classic());
        initial << std::setprecision(std::numeric_limits<double>::max_digits10)
                << std::defaultfloat << normalized;
        widget.initialValue=initial.str();
    } else widget.initialValue = formatNumericValue(normalized);
    return true;
}
bool setWidgetNumericIncrementStep(WidgetInstance &widget, double step) {
    if ((!isNumericWidget(widget) && !isRingWidget(widget)) || !std::isfinite(step) || step < 0.0) {
        return false;
    }
    if (widget.numericIncrementStep.has_value() && std::abs(widget.numericIncrementStep.value() - step) < 0.000001) {
        return false;
    }
    widget.numericIncrementStep = step;
    return true;
}

namespace {

struct NumericCustomFormatSpec {
    std::size_t conversionStart = 0;
    std::size_t conversionEnd = 0;
    int width = 0;
    int precision = -1;
    char code = 'g';
    bool zeroPad = false;
    bool leftAlign = false;
    bool forceSign = false;
    bool hideTrailingZeros = false;
    bool engineering = false;
    bool significantPrecision = false;
};

bool parseNumericCustomFormat(std::string_view format, NumericCustomFormatSpec &spec) noexcept {
    bool conversionFound = false;
    for (std::size_t index = 0; index < format.size();) {
        if (format[index] != '%') {
            ++index;
            continue;
        }
        if (index + 1U < format.size() && format[index + 1U] == '%') {
            index += 2U;
            continue;
        }
        if (conversionFound) {
            return false;
        }
        conversionFound = true;
        spec = NumericCustomFormatSpec{};
        spec.conversionStart = index++;
        bool parsingFlags = true;
        while (index < format.size() && parsingFlags) {
            switch (format[index]) {
            case '-': spec.leftAlign = true; ++index; break;
            case '+': spec.forceSign = true; ++index; break;
            case '#': spec.hideTrailingZeros = true; ++index; break;
            case '^': spec.engineering = true; ++index; break;
            case '0': spec.zeroPad = true; ++index; break;
            default: parsingFlags = false; break;
            }
        }
        while (index < format.size() && std::isdigit(static_cast<unsigned char>(format[index]))) {
            spec.width = (std::min)(64, spec.width * 10 + (format[index] - '0'));
            ++index;
        }
        if (index < format.size() && format[index] == '.') {
            ++index;
            spec.precision = 0;
            if (index >= format.size() || !std::isdigit(static_cast<unsigned char>(format[index]))) {
                return false;
            }
            while (index < format.size() && std::isdigit(static_cast<unsigned char>(format[index]))) {
                spec.precision = (std::min)(18, spec.precision * 10 + (format[index] - '0'));
                ++index;
            }
        } else if (index < format.size() && format[index] == '_') {
            ++index;
            spec.significantPrecision = true;
            spec.precision = 0;
            if (index >= format.size() || !std::isdigit(static_cast<unsigned char>(format[index]))) {
                return false;
            }
            while (index < format.size() && std::isdigit(static_cast<unsigned char>(format[index]))) {
                spec.precision = (std::min)(18, spec.precision * 10 + (format[index] - '0'));
                ++index;
            }
        }
        if (index >= format.size()) {
            return false;
        }
        spec.code = format[index++];
        if (std::string_view{ "fFeEgGduxXobp" }.find(spec.code) == std::string_view::npos) {
            return false;
        }
        spec.conversionEnd = index;
    }
    return conversionFound;
}

std::string unescapeNumericFormatLiteral(std::string_view text) {
    std::string result;
    result.reserve(text.size());
    for (std::size_t index = 0; index < text.size(); ++index) {
        if (text[index] == '%' && index + 1U < text.size() && text[index + 1U] == '%') {
            result.push_back('%');
            ++index;
        } else {
            result.push_back(text[index]);
        }
    }
    return result;
}

std::string trimNumericTrailingZeros(std::string text) {
    return numeric_text::trimTrailingZeros(std::move(text));
}

int fractionalDigitsForSignificantDigits(double value, int digits) noexcept {
    if (value == 0.0) {
        return (std::max)(0, digits - 1);
    }
    const int magnitude = static_cast<int>(std::floor(std::log10(std::abs(value))));
    return std::clamp(digits - magnitude - 1, 0, 18);
}

std::string formatNumericFixed(
    double value,
    int digits,
    bool fractionalPrecision,
    bool hideTrailingZeros) {
    int precision = fractionalPrecision
        ? digits
        : fractionalDigitsForSignificantDigits(value, digits);
    if (!fractionalPrecision && value != 0.0) {
        const int magnitude = static_cast<int>(std::floor(std::log10(std::abs(value))));
        const int integerRoundingPower = magnitude - digits + 1;
        if (integerRoundingPower > 0) {
            const double scale = std::pow(10.0, integerRoundingPower);
            value = std::round(value / scale) * scale;
            precision = 0;
        }
    }
    std::ostringstream output;
    output.imbue(std::locale::classic());
    output << std::fixed << std::setprecision(precision) << value;
    return hideTrailingZeros ? trimNumericTrailingZeros(output.str()) : output.str();
}

std::string formatNumericScientific(
    double value,
    int digits,
    bool fractionalPrecision,
    bool hideTrailingZeros) {
    const int precision = fractionalPrecision ? digits : (std::max)(0, digits - 1);
    std::ostringstream output;
    output.imbue(std::locale::classic());
    output << std::scientific << std::setprecision(precision) << value;
    return hideTrailingZeros ? trimNumericTrailingZeros(output.str()) : output.str();
}

std::pair<double, int> engineeringMantissaAndExponent(double value) noexcept {
    if (value == 0.0) {
        return { 0.0, 0 };
    }
    const int exponent = static_cast<int>(std::floor(std::log10(std::abs(value)) / 3.0)) * 3;
    return { value / std::pow(10.0, exponent), exponent };
}

std::string formatNumericEngineering(
    double value,
    int digits,
    bool fractionalPrecision,
    bool hideTrailingZeros,
    bool appendExponent) {
    const auto [mantissa, exponent] = engineeringMantissaAndExponent(value);
    std::string result = formatNumericFixed(
        mantissa,
        digits,
        fractionalPrecision,
        hideTrailingZeros);
    if (appendExponent && exponent != 0) {
        result += exponent > 0 ? "e+" : "e";
        result += std::to_string(exponent);
    }
    return result;
}

std::string_view siSuffixForExponent(int exponent) noexcept {
    switch (exponent) {
    case -24: return "y";
    case -21: return "z";
    case -18: return "a";
    case -15: return "f";
    case -12: return "p";
    case -9: return "n";
    case -6: return "u";
    case -3: return "m";
    case 0: return "";
    case 3: return "k";
    case 6: return "M";
    case 9: return "G";
    case 12: return "T";
    case 15: return "P";
    case 18: return "E";
    case 21: return "Z";
    case 24: return "Y";
    default: return {};
    }
}

std::string formatNumericSi(
    double value,
    int digits,
    bool fractionalPrecision,
    bool hideTrailingZeros) {
    const auto [mantissa, exponent] = engineeringMantissaAndExponent(value);
    const auto suffix = siSuffixForExponent(exponent);
    if (exponent < -24 || exponent > 24) {
        return formatNumericEngineering(
            value,
            digits,
            fractionalPrecision,
            hideTrailingZeros,
            true);
    }
    std::string result = formatNumericFixed(
        mantissa,
        digits,
        fractionalPrecision,
        hideTrailingZeros);
    result.append(suffix.data(), suffix.size());
    return result;
}

std::string unsignedIntegerInBase(unsigned long long value, unsigned base, bool uppercase) {
    const char *digits = uppercase
        ? "0123456789ABCDEF"
        : "0123456789abcdef";
    std::string result;
    do {
        result.push_back(digits[value % base]);
        value /= base;
    } while (value != 0U);
    std::reverse(result.begin(), result.end());
    return result;
}

struct ExactDisplayInteger {
    std::uint64_t magnitude = 0;
    bool negative = false;
};

std::string formatNumericInteger(double value, unsigned base, bool uppercase, bool unsignedValue,
    const ExactDisplayInteger *exact = nullptr) {
    if (exact) {
        if (unsignedValue && exact->negative) return "0";
        auto result = unsignedIntegerInBase(exact->magnitude, base, uppercase);
        if (exact->negative) result.insert(result.begin(), '-');
        return result;
    }
    if (unsignedValue) {
        const auto converted = value <= 0.0
            ? 0ULL
            : static_cast<unsigned long long>(value);
        return unsignedIntegerInBase(converted, base, uppercase);
    }
    const auto converted = static_cast<long long>(value);
    if (converted >= 0) {
        return unsignedIntegerInBase(static_cast<unsigned long long>(converted), base, uppercase);
    }
    const auto magnitude = static_cast<unsigned long long>(-(converted + 1LL)) + 1ULL;
    return "-" + unsignedIntegerInBase(magnitude, base, uppercase);
}

bool numericValueTypeIsUnsigned(std::string_view valueType) noexcept {
    const auto *mapping = numericRepresentationMapping(valueType);
    return mapping != nullptr &&
        (mapping->kind.starts_with("uint") || mapping->kind == "big_uint");
}

unsigned numericBitWidth(std::string_view valueType) noexcept {
    const auto *mapping = numericRepresentationMapping(valueType);
    if (mapping == nullptr) {
        return 64U;
    }
    const auto kind = mapping->kind;
    const auto digit = std::find_if(kind.begin(), kind.end(), [](unsigned char ch) {
        return std::isdigit(ch) != 0;
    });
    if (digit == kind.end()) {
        return 64U;
    }
    unsigned width = 0U;
    for (auto cursor = digit; cursor != kind.end() && std::isdigit(static_cast<unsigned char>(*cursor)); ++cursor) {
        width = (std::min)(128U, width * 10U + static_cast<unsigned>(*cursor - '0'));
    }
    return width == 0U ? 64U : (std::min)(64U, width);
}

std::string formatNumericRawBits(
    const WidgetInstance &widget,
    double value,
    unsigned base,
    bool uppercase,
    const ExactDisplayInteger *exact = nullptr) {
    const unsigned width = numericBitWidth(widget.valueType);
    std::uint64_t bits = 0U;
    if (exact) {
        bits = exact->negative ? std::uint64_t{0} - exact->magnitude : exact->magnitude;
    } else if (numericValueTypeIsUnsigned(widget.valueType)) {
        bits = value <= 0.0 ? 0U : static_cast<std::uint64_t>(value);
    } else {
        bits = static_cast<std::uint64_t>(static_cast<std::int64_t>(value));
    }
    if (width < 64U) {
        bits &= (std::uint64_t{ 1 } << width) - 1U;
    }
    std::string result = unsignedIntegerInBase(bits, base, uppercase);
    unsigned bitsPerDigit = 1U;
    if (base == 16U) {
        bitsPerDigit = 4U;
    } else if (base == 8U) {
        bitsPerDigit = 3U;
    }
    const std::size_t digitWidth = static_cast<std::size_t>((width + bitsPerDigit - 1U) / bitsPerDigit);
    if (result.size() < digitWidth) {
        result.insert(0U, digitWidth - result.size(), '0');
    }
    return result;
}

std::string groupNumericDigits(std::string text, int groupSize) {
    groupSize = std::clamp(groupSize, 0, 8);
    if (groupSize == 0) {
        return text;
    }
    const std::size_t signOffset = !text.empty() && (text.front() == '-' || text.front() == '+') ? 1U : 0U;
    const std::size_t digitCount = text.size() - signOffset;
    if (digitCount <= static_cast<std::size_t>(groupSize)) {
        return text;
    }
    for (std::size_t position = text.size() - static_cast<std::size_t>(groupSize);
         position > signOffset;
         position -= static_cast<std::size_t>(groupSize)) {
        text.insert(position, 1U, ' ');
        if (position <= signOffset + static_cast<std::size_t>(groupSize)) {
            break;
        }
    }
    return text;
}

std::string applyNumericSign(std::string text, double value, std::string_view signDisplay) {
    if (value < 0.0 || (!text.empty() && (text.front() == '-' || text.front() == '+'))) {
        return text;
    }
    if (signDisplay == "always") {
        text.insert(text.begin(), '+');
    } else if (signDisplay == "space_positive") {
        text.insert(text.begin(), ' ');
    }
    return text;
}

std::string addNumericRadixPrefix(std::string text, std::string_view prefix) {
    const std::size_t insertion = !text.empty() && (text.front() == '-' || text.front() == '+') ? 1U : 0U;
    text.insert(insertion, prefix);
    return text;
}

std::string localizeNumericDecimal(std::string text, std::string_view localeMode) {
    if (localeMode != "system") {
        return text;
    }
    try {
        const std::locale systemLocale("");
        const char decimal = std::use_facet<std::numpunct<char>>(systemLocale).decimal_point();
        if (decimal != '.') {
            const auto exponent = text.find_first_of("eE");
            const auto decimalPoint = text.find('.');
            if (decimalPoint != std::string::npos &&
                (exponent == std::string::npos || decimalPoint < exponent)) {
                text[decimalPoint] = decimal;
            }
        }
    } catch (const std::runtime_error &) {
        // Keep the invariant representation when the host locale is unavailable.
    }
    return text;
}

std::string padNumericField(
    std::string text,
    int width,
    std::string_view padding,
    std::string_view alignment = "right") {
    width = std::clamp(width, 0, 64);
    if (padding == "none" || static_cast<int>(text.size()) >= width) {
        return text;
    }
    const std::size_t count = static_cast<std::size_t>(width - static_cast<int>(text.size()));
    if (padding == "zeros") {
        // A reserved positive sign is a sign position, not ordinary padding.
        std::size_t insertion = !text.empty() &&
            (text.front() == '-' || text.front() == '+' || text.front() == ' ') ? 1U : 0U;
        if (text.size() >= insertion + 2U && text[insertion] == '0' &&
            (text[insertion + 1U] == 'x' || text[insertion + 1U] == 'X' ||
                text[insertion + 1U] == 'b' || text[insertion + 1U] == 'B' ||
                text[insertion + 1U] == 'o' || text[insertion + 1U] == 'O')) {
            insertion += 2U;
        }
        text.insert(insertion, count, '0');
        return text;
    }
    if (padding == "spaces_right") {
        text.append(count, ' ');
    } else if (padding == "spaces_left") {
        text.insert(0U, count, ' ');
    } else if (alignment == "left") {
        text.append(count, ' ');
    } else if (alignment == "center") {
        text.insert(0U, count / 2U, ' ');
        text.append(count - count / 2U, ' ');
    } else {
        text.insert(0U, count, ' ');
    }
    return text;
}

std::string formatNumericCustom(double value, std::string_view format, const NumericCustomFormatSpec &spec,
    const ExactDisplayInteger *exact = nullptr) {
    const int precision = spec.precision >= 0 ? spec.precision : 6;
    std::string valueText;
    switch (spec.code) {
    case 'f':
    case 'F':
        valueText = formatNumericFixed(
            value,
            precision,
            !spec.significantPrecision,
            spec.hideTrailingZeros);
        break;
    case 'e':
    case 'E':
        valueText = spec.engineering
            ? formatNumericEngineering(
                value,
                precision,
                !spec.significantPrecision,
                spec.hideTrailingZeros,
                true)
            : formatNumericScientific(
                value,
                precision,
                !spec.significantPrecision,
                spec.hideTrailingZeros);
        if (spec.code == 'E') {
            std::transform(valueText.begin(), valueText.end(), valueText.begin(), [](unsigned char ch) {
                return static_cast<char>(std::toupper(ch));
            });
        }
        break;
    case 'g':
    case 'G': {
        std::ostringstream output;
        output.imbue(std::locale::classic());
        output << std::setprecision((std::max)(1, precision)) << std::defaultfloat << value;
        valueText = output.str();
        if (spec.engineering) {
            valueText = formatNumericEngineering(
                value,
                precision,
                !spec.significantPrecision,
                spec.hideTrailingZeros,
                true);
        } else if (spec.hideTrailingZeros) {
            valueText = trimNumericTrailingZeros(std::move(valueText));
        }
        if (spec.code == 'G') {
            std::transform(valueText.begin(), valueText.end(), valueText.begin(), [](unsigned char ch) {
                return static_cast<char>(std::toupper(ch));
            });
        }
        break;
    }
    case 'd':
        valueText = formatNumericInteger(value, 10U, false, false, exact);
        break;
    case 'u':
        valueText = formatNumericInteger(value, 10U, false, true, exact);
        break;
    case 'x':
        valueText = formatNumericInteger(value, 16U, false, true, exact);
        break;
    case 'X':
        valueText = formatNumericInteger(value, 16U, true, true, exact);
        break;
    case 'o':
        valueText = formatNumericInteger(value, 8U, false, true, exact);
        break;
    case 'b':
        valueText = formatNumericInteger(value, 2U, false, true, exact);
        break;
    case 'p':
        valueText = formatNumericSi(
            value,
            (std::max)(1, precision),
            !spec.significantPrecision,
            spec.hideTrailingZeros);
        break;
    default:
        valueText = formatNumericValue(value);
        break;
    }
    valueText = applyNumericSign(std::move(valueText), value, spec.forceSign ? "always" : "negative_only");
    valueText = padNumericField(
        std::move(valueText),
        spec.width,
        spec.zeroPad ? "zeros" : "spaces",
        spec.leftAlign ? "left" : "right");
    return unescapeNumericFormatLiteral(format.substr(0U, spec.conversionStart)) +
        valueText +
        unescapeNumericFormatLiteral(format.substr(spec.conversionEnd));
}

} // namespace
namespace {
std::string formatNumericValueImpl(const WidgetInstance &widget, double value,
    const ExactDisplayInteger *exact, const std::string *exact128 = nullptr) {
    if (!std::isfinite(value)) {
        return formatNumericValue(value);
    }
    // Absent persisted options have the same display defaults as Numeric
    // Properties. The scalar-only overload remains the storage formatter.
    const std::string kind = widget.numericDisplayFormat.value_or("automatic");
    const int digits = std::clamp(widget.numericDisplayDigits.value_or(6), 1, 18);
    const bool fractionalPrecision = widget.numericPrecisionType.value_or("significant") == "fractional";
    const bool hideTrailingZeros = widget.numericHideTrailingZeros.value_or(true);
    const bool exponentMultipleOfThree = widget.numericExponentMultipleOfThree.value_or(false);
    const bool integral = numericValueTypeIsIntegral(widget.valueType);
    const bool unsignedType = numericValueTypeIsUnsigned(widget.valueType);
    const bool uppercase = widget.numericRadixUppercase.value_or(true);
    const bool rawBits = widget.numericRadixInterpretation.value_or("numeric") == "raw_bits";
    const int radixGroupSize = std::clamp(widget.numericRadixGroupSize.value_or(0), 0, 8);
    const unsigned radixBase = kind == "binary" ? 2U : kind == "octal" ? 8U : kind == "hexadecimal" ? 16U : 0U;
    std::optional<std::string> wideRadix;
    if (radixBase && (widget.valueType == "i128" || widget.valueType == "u128")) {
        // Actual widget values use their exact lexical source. The explicit
        // double overload remains a preview of its supplied value, never an
        // accidental substitution of the widget's authored default.
        if (exact128) wideRadix = numericRadix128Text(*exact128, widget.valueType, radixBase, rawBits, uppercase);
        else {
            char decimal[128]{};
            const auto converted = std::to_chars(decimal, decimal + sizeof(decimal),
                std::trunc(value), std::chars_format::fixed, 0);
            if (converted.ec == std::errc{})
                wideRadix = numericRadix128Text(std::string_view(decimal, converted.ptr - decimal),
                    widget.valueType, radixBase, rawBits, uppercase);
        }
    }
    std::string result;

    if (wideRadix) {
        result = std::move(*wideRadix);
    } else if (kind == "custom") {
        const std::string format = widget.numericCustomFormatString.value_or("%.6g");
        NumericCustomFormatSpec spec;
        result = parseNumericCustomFormat(format, spec)
            ? formatNumericCustom(value, format, spec, exact)
            : formatNumericValue(value);
    } else if (kind == "fixed") {
        result = formatNumericFixed(value, digits, fractionalPrecision, hideTrailingZeros);
    } else if (kind == "scientific" && !exponentMultipleOfThree) {
        result = formatNumericScientific(value, digits, fractionalPrecision, hideTrailingZeros);
    } else if (kind == "scientific" || kind == "engineering") {
        result = formatNumericEngineering(value, digits, fractionalPrecision, hideTrailingZeros, true);
    } else if (kind == "si") {
        result = formatNumericSi(value, digits, fractionalPrecision, hideTrailingZeros);
    } else if (kind == "hexadecimal") {
        result = rawBits
            ? formatNumericRawBits(widget, value, 16U, uppercase, exact)
            : formatNumericInteger(value, 16U, uppercase, unsignedType, exact);
    } else if (kind == "octal") {
        result = rawBits
            ? formatNumericRawBits(widget, value, 8U, false, exact)
            : formatNumericInteger(value, 8U, false, unsignedType, exact);
    } else if (kind == "binary") {
        result = rawBits
            ? formatNumericRawBits(widget, value, 2U, false, exact)
            : formatNumericInteger(value, 2U, false, unsignedType, exact);
    } else if (kind == "unsigned_decimal") {
        result = formatNumericInteger(value, 10U, false, true, exact);
    } else if (kind == "decimal" || integral) {
        result = formatNumericInteger(value, 10U, false, unsignedType, exact);
    } else {
        const auto automatic = numeric_text::automaticNotation(value, digits, !hideTrailingZeros);
        if (fractionalPrecision) {
            // Keep Automatic's existing notation choice, but count places
            // after the decimal point rather than significant digits.
            result = automatic.find_first_of("eE") == std::string::npos
                ? formatNumericFixed(value, digits, true, hideTrailingZeros)
                : formatNumericScientific(value, digits, true, hideTrailingZeros);
        } else {
            result = hideTrailingZeros ? trimNumericTrailingZeros(automatic) : automatic;
        }
    }

    if (kind != "custom") {
        const bool radix = kind == "hexadecimal" || kind == "octal" || kind == "binary";
        if (radix) {
            result = groupNumericDigits(std::move(result), radixGroupSize);
            if (widget.numericRadixShowPrefix.value_or(false)) {
                const std::string_view radixPrefix = kind == "hexadecimal"
                    ? (uppercase ? "0X" : "0x")
                    : (kind == "binary" ? "0b" : "0o");
                result = addNumericRadixPrefix(std::move(result), radixPrefix);
            }
        }
        result = applyNumericSign(
            std::move(result),
            value,
            widget.numericSignDisplay.value_or("negative_only"));
        result = padNumericField(
            std::move(result),
            widget.numericMinimumFieldWidth.value_or(0),
            widget.numericPadding.value_or("spaces"),
            widget.numericAlignment.value_or("right"));
    }
    result = localizeNumericDecimal(
        std::move(result),
        widget.numericLocale.value_or("invariant"));
    return widget.numericDisplayPrefix.value_or("") +
        result +
        widget.numericDisplaySuffix.value_or("");
}
} // namespace
bool numericFormatStringIsValid(std::string_view format) noexcept {
    NumericCustomFormatSpec spec;
    return parseNumericCustomFormat(format, spec);
}

std::string formatNumericValue(const WidgetInstance &widget, double value) {
    return formatNumericValueImpl(widget, value, nullptr);
}

std::string formatNumericValue(const WidgetInstance &widget) {
    const auto value = widgetNumericValue(widget);
    if (widget.valueType == "i128" || widget.valueType == "u128") {
        if (const auto text = widgetNumericIntegerValueLexeme(widget))
            return formatNumericValueImpl(widget, value, nullptr, &*text);
    }
    // The lexical getter validates the cache against the current source value.
    // Only fixed-width I64/U64 need this path; explicit floating previews keep
    // their existing overload and are never replaced by an authored default.
    if (widget.valueType == "i64" || widget.valueType == "u64") {
        const auto text = widgetNumericIntegerValueLexeme(widget);
        const auto canonical = text ? numeric_value::canonicalWideIntegerLexeme(*text, widget.valueType) : std::nullopt;
        if (canonical) {
            const bool negative = canonical->starts_with('-');
            const auto digits = std::string_view(*canonical).substr(negative ? 1 : 0);
            ExactDisplayInteger exact{0, negative};
            const auto parsed = std::from_chars(digits.data(), digits.data() + digits.size(), exact.magnitude);
            if (parsed.ec == std::errc{} && parsed.ptr == digits.data() + digits.size())
                return formatNumericValueImpl(widget, value, &exact);
        }
    }
    return formatNumericValueImpl(widget, value, nullptr);
}

std::string formatWidgetNumericComplexValue(const WidgetInstance &widget) {
    const auto text = widgetNumericComplexValueLexeme(widget);
    const auto value = text ? numeric_value::parseComplexNumber(*text, widget.valueType) : std::nullopt;
    if (!value) return {};
    auto component = widget;
    component.valueType = widget.valueType == "c32" ? "f32" : "f64";
    component.numericDisplayPrefix.reset();
    component.numericDisplaySuffix.reset();
    component.numericMinimumFieldWidth.reset();
    component.numericPadding.reset();
    component.numericAlignment.reset();
    // Both components use the same Numeric precision/notation policy; the
    // separator owns the imaginary sign. Decorations apply to the pair once.
    const auto real = formatNumericValue(component, value->real);
    component.numericSignDisplay = "negative_only";
    auto imaginary = formatNumericValue(component, std::abs(value->imaginary));
    if (component.numericDisplayFormat == "custom") {
        const auto format = component.numericCustomFormatString.value_or("%.6g");
        NumericCustomFormatSpec spec;
        if (parseNumericCustomFormat(format, spec)) {
            spec.forceSign = false;
            imaginary = localizeNumericDecimal(formatNumericCustom(std::abs(value->imaginary), format, spec),
                component.numericLocale.value_or("invariant"));
        }
    }
    auto pair = real + (std::signbit(value->imaginary) ? "-" : "+") + imaginary + "i";
    pair = padNumericField(std::move(pair), widget.numericMinimumFieldWidth.value_or(0),
        widget.numericPadding.value_or("spaces"), widget.numericAlignment.value_or("right"));
    return widget.numericDisplayPrefix.value_or("") + pair + widget.numericDisplaySuffix.value_or("");
}

std::string fixedPointNamedSize(const FixedPointConfiguration &configuration) {
    const auto normalized = normalizedFixedPointConfiguration(configuration);
    const auto limits = fixedPointLimits(normalized);
    std::ostringstream output;
    output.imbue(std::locale::classic());
    if (normalized.complex) {
        output << "Complex<";
    }
    output << "FixedPoint<"
           << (normalized.signedEncoding ? "signed" : "unsigned")
           << ','
           << normalized.wordLength
           << ','
           << limits.fractionalBits
           << ','
           << normalized.roundingMode
           << ','
           << normalized.overflowMode
           << '>';
    if (normalized.complex) {
        output << '>';
    }
    return output.str();
}

std::string numericNamedSize(const WidgetInstance &widget) {
    if(!unavailableComplexNumericIdentity(numericRepresentationKind(widget)).empty())return numericRepresentationKind(widget);
    if (numericRepresentationKind(widget) == "fixed_point") {
        return fixedPointNamedSize(fixedPointConfiguration(widget));
    }
    if (const auto *mapping = numericRepresentationMapping(numericRepresentationKind(widget))) {
        return std::string(mapping->namedSize);
    }
    return "Float64";
}

std::string enumNamedSize(const WidgetInstance &widget) {
    if (const auto *mapping = numericRepresentationMapping(enumRepresentationKind(widget))) {
        return std::string(mapping->namedSize);
    }
    return "UInt16";
}

std::string ringNamedSize(const WidgetInstance &widget) {
    if (const auto *mapping = numericRepresentationMapping(ringRepresentationKind(widget))) {
        return std::string(mapping->namedSize);
    }
    return "UInt16";
}

bool numericUsesTypeDefaultValueEntry(const WidgetInstance &widget) noexcept {
    if (!isNumericWidget(widget) && !isRingWidget(widget)) {
        return false;
    }
    const bool fixedPoint = numericRepresentationKind(widget) == "fixed_point";
    const auto fixedLimits = fixedPoint
        ? fixedPointLimits(fixedPointConfiguration(widget))
        : FixedPointLimits{};
    const double typeMinimum = fixedPoint
        ? fixedLimits.minimum
        : numericMinimumForValueType(widget.valueType);
    const double typeMaximum = fixedPoint
        ? fixedLimits.maximum
        : numericMaximumForValueType(widget.valueType);
    const bool defaultMinimum = !widget.numericMinimum.has_value() || widget.numericMinimum.value() == typeMinimum;
    const bool defaultMaximum = !widget.numericMaximum.has_value() || widget.numericMaximum.value() == typeMaximum;
    return defaultMinimum &&
        defaultMaximum &&
        !widget.numericMinimumResponse.has_value() &&
        !widget.numericMaximumResponse.has_value() &&
        !widget.numericIncrementResponse.has_value();
}

bool numericUsesTypeDefaultDataEntry(const WidgetInstance &widget) noexcept {
    return numericUsesTypeDefaultValueEntry(widget) &&
        !widget.numericIncrementStep.has_value() &&
        !widget.numericPageStep.has_value();
}

}
