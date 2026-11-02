#pragma once

#include <cstddef>
#include <span>
#include <string_view>

namespace frog::native_engine {

enum class DiagramShapeStandard {
    Intrinsic,
    Triangle,
    Square,
    Rectangle30,
    LogicAnd,
    LogicOr,
    LogicXor,
    HalfSquare,
    Capsule,
    RectangleVertical,
    HalfHorizontalRounded,
    CapsuleStraight,
    FixedConstant
};

enum class DiagramShapeResizePolicy {
    Fixed,
    Uniform,
    Horizontal,
    Vertical,
    BothAxes
};

enum class DiagramShapeAuraPolicy {
    SvgOutline,
    Rectangular
};

enum class DiagramElementStandard {
    Function,
    FunctionRectangle30,
    FunctionSelect120,
    ExplicitConversion,
    WidgetTerminal,
    NumericConstant,
    FixedConstant,
    EnumConstant,
    RingConstant
};

enum class DiagramExtensionStandard {
    None,
    Triangle,
    BooleanAnd,
    BooleanOr,
    BooleanXor,
    BooleanNand,
    BooleanNor,
    BooleanXnor,
    StringConcatenate,
    RetainFirstError
};

struct DiagramShapeGeometry {
    double canvasSide = 48.0;
    double bodyLeft = 0.0;
    double bodyTop = 0.0;
    double bodyRight = 48.0;
    double bodyBottom = 48.0;
    double inputAnchorX = 0.0;
    double outputAnchorX = 1.0;
    double canvasHeight = 48.0;
};

struct DiagramShapeDescriptor {
    DiagramShapeStandard standard = DiagramShapeStandard::Intrinsic;
    std::string_view id;
    std::string_view displayName;
    std::string_view editorAssetPath;
    DiagramShapeGeometry geometry;
    DiagramShapeResizePolicy resizePolicy = DiagramShapeResizePolicy::Fixed;
    DiagramShapeAuraPolicy auraPolicy = DiagramShapeAuraPolicy::SvgOutline;
    bool editorVisible = false;
};

struct DiagramElementGeometry {
    double naturalWidth = 48.0;
    double naturalHeight = 48.0;
    double minimumWidth = 48.0;
    double minimumHeight = 48.0;
};

struct DiagramElementDescriptor {
    DiagramElementStandard standard = DiagramElementStandard::Function;
    std::string_view id;
    std::string_view displayName;
    DiagramElementGeometry geometry;
    DiagramShapeResizePolicy resizePolicy = DiagramShapeResizePolicy::Fixed;
    DiagramShapeAuraPolicy auraPolicy = DiagramShapeAuraPolicy::SvgOutline;
    bool preserveEndAdornmentSize = false;
};

inline constexpr std::size_t kDiagramExtensionBaseInputCount = 2U;
inline constexpr std::size_t kDiagramExtensionMaximumInputCount = 16U;
inline constexpr double kDiagramExtensionInputPitch = 24.0;

struct DiagramExtensionDescriptor {
    DiagramExtensionStandard standard = DiagramExtensionStandard::None;
    std::string_view id;
    std::string_view displayName;
    DiagramShapeStandard headShape = DiagramShapeStandard::Intrinsic;
    std::size_t baseInputCount = kDiagramExtensionBaseInputCount;
    std::size_t maximumInputCount = kDiagramExtensionMaximumInputCount;
    double inputPitch = kDiagramExtensionInputPitch;
    double baseInputAnchorX = 4.0;
    double extendedInputAnchorX = 4.0;
    double outputAnchorX = 45.0;
};

[[nodiscard]] constexpr DiagramShapeGeometry diagramShapeGeometry(
    DiagramShapeStandard standard) noexcept {
    switch (standard) {
    case DiagramShapeStandard::Triangle:
        return DiagramShapeGeometry{
            48.0, 4.0, 3.5, 45.0, 45.0, 4.0 / 48.0, 45.0 / 48.0
        };
    case DiagramShapeStandard::Square:
        return DiagramShapeGeometry{
            48.0, 1.0, 1.0, 47.0, 47.0, 1.0 / 48.0, 47.0 / 48.0
        };
    case DiagramShapeStandard::Rectangle30:
        return DiagramShapeGeometry{
            48.0, 1.0, 1.3, 47.0, 61.1, 1.0 / 48.0, 47.0 / 48.0, 62.4
        };
    case DiagramShapeStandard::LogicAnd:
    case DiagramShapeStandard::LogicOr:
    case DiagramShapeStandard::LogicXor:
        return DiagramShapeGeometry{
            48.0, 4.0, 12.0, 44.0, 36.0, 4.0 / 48.0, 44.0 / 48.0
        };
    case DiagramShapeStandard::HalfSquare:
        return DiagramShapeGeometry{
            48.0, 1.0, 12.0, 47.0, 36.0, 1.0 / 48.0, 47.0 / 48.0
        };
    case DiagramShapeStandard::Capsule:
        return DiagramShapeGeometry{
            48.0, 1.0, 12.5, 47.0, 35.5, 1.0 / 48.0, 47.0 / 48.0
        };
    case DiagramShapeStandard::RectangleVertical:
        return DiagramShapeGeometry{
            48.0, 1.0, 1.3, 47.0, 61.1, 1.0 / 48.0, 47.0 / 48.0, 62.4
        };
    case DiagramShapeStandard::HalfHorizontalRounded:
    case DiagramShapeStandard::CapsuleStraight:
        return DiagramShapeGeometry{
            48.0, 1.0, 12.5, 47.0, 35.5, 1.0 / 48.0, 47.0 / 48.0
        };
    case DiagramShapeStandard::FixedConstant:
        return DiagramShapeGeometry{
            48.0, 6.75, 12.5, 41.25, 35.5, 6.75 / 48.0, 41.25 / 48.0
        };
    case DiagramShapeStandard::Intrinsic:
    default:
        return DiagramShapeGeometry{};
    }
}

[[nodiscard]] constexpr bool diagramShapeIsLogicGate(
    DiagramShapeStandard standard) noexcept {
    return standard == DiagramShapeStandard::LogicAnd ||
        standard == DiagramShapeStandard::LogicOr ||
        standard == DiagramShapeStandard::LogicXor;
}

[[nodiscard]] constexpr DiagramElementGeometry diagramElementGeometry(
    DiagramElementStandard standard) noexcept {
    constexpr double editableConstantLegacyWidth = 84.0;
    constexpr double editableConstantLegacyHeight = 38.0;
    constexpr double editableConstantLegacyMinimumWidth = 48.0;
    constexpr double editableConstantHeight = 29.75;
    constexpr double editableConstantScale =
        editableConstantHeight / editableConstantLegacyHeight;
    switch (standard) {
    case DiagramElementStandard::FunctionRectangle30:
        return DiagramElementGeometry{ 48.0, 62.4, 48.0, 62.4 };
    case DiagramElementStandard::FunctionSelect120:
        return DiagramElementGeometry{ 57.6, 57.6, 57.6, 57.6 };
    case DiagramElementStandard::ExplicitConversion:
        return DiagramElementGeometry{ 48.0, 48.0, 48.0, 48.0 };
    case DiagramElementStandard::WidgetTerminal:
        return DiagramElementGeometry{ 66.3, 29.75, 66.3, 29.75 };
    case DiagramElementStandard::NumericConstant:
        return DiagramElementGeometry{
            editableConstantLegacyWidth * editableConstantScale,
            editableConstantHeight,
            editableConstantLegacyMinimumWidth * editableConstantScale,
            editableConstantHeight
        };
    case DiagramElementStandard::FixedConstant:
        return DiagramElementGeometry{ 48.0, 48.0, 48.0, 48.0 };
    case DiagramElementStandard::EnumConstant:
        return DiagramElementGeometry{ 136.0, 34.0, 76.0, 34.0 };
    case DiagramElementStandard::RingConstant:
        return DiagramElementGeometry{ 136.0, 34.0, 64.0, 34.0 };
    case DiagramElementStandard::Function:
    default:
        return DiagramElementGeometry{};
    }
}

[[nodiscard]] std::span<const DiagramShapeDescriptor> diagramShapeManifest() noexcept;
[[nodiscard]] const DiagramShapeDescriptor *findDiagramShapeDescriptor(
    DiagramShapeStandard standard) noexcept;
[[nodiscard]] const DiagramShapeDescriptor *findDiagramShapeDescriptor(
    std::string_view id) noexcept;
[[nodiscard]] DiagramShapeStandard diagramShapeStandardForFunction(
    std::string_view catalogId,
    std::string_view parentCatalogId) noexcept;
[[nodiscard]] std::span<const DiagramElementDescriptor> diagramElementManifest() noexcept;
[[nodiscard]] const DiagramElementDescriptor *findDiagramElementDescriptor(
    DiagramElementStandard standard) noexcept;
[[nodiscard]] const DiagramElementDescriptor *findDiagramElementDescriptor(
    std::string_view id) noexcept;
[[nodiscard]] DiagramElementStandard diagramElementStandardForFunction(
    std::string_view catalogId,
    std::string_view parentCatalogId) noexcept;
[[nodiscard]] std::span<const DiagramExtensionDescriptor> diagramExtensionManifest() noexcept;
[[nodiscard]] const DiagramExtensionDescriptor *findDiagramExtensionDescriptor(
    DiagramExtensionStandard standard) noexcept;
[[nodiscard]] const DiagramExtensionDescriptor *findDiagramExtensionDescriptor(
    std::string_view id) noexcept;
[[nodiscard]] DiagramExtensionStandard diagramExtensionStandardForFunction(
    std::string_view catalogId) noexcept;
[[nodiscard]] double diagramExtensionInputAnchorX(
    DiagramExtensionStandard standard,
    std::size_t inputIndex) noexcept;
[[nodiscard]] double diagramExtensionOutputAnchorX(
    DiagramExtensionStandard standard) noexcept;

} // namespace frog::native_engine
