#pragma once
#include "frog_native_engine/ButtonMechanicalAction.hpp"

#include "frog_native_engine/Geometry.hpp"
#include "frog_native_engine/DocumentTextDefaults.hpp"
#include "frog_native_engine/WidgetCatalog.hpp"
#include "frog_native_engine/SharedBuffer.hpp"
#include "frog_native_engine/ApplicationControlTypes.hpp"
#include "frog_native_engine/CollectionAuthoringState.hpp"
#include "frog_native_engine/HeapOptional.hpp"
#include "frog_native_engine/TypeDefinitionLink.hpp"
#include "frog_native_engine/graph/WaveformGraph.hpp"
#include "frog_native_engine/graph/XYGraph.hpp"

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace frog::native_engine {

constexpr std::uint32_t kTransparentColorModel = 0xFFFFFFFFU;

[[nodiscard]] constexpr bool isTransparentColorModel(std::uint32_t rgb) noexcept {
    return rgb == kTransparentColorModel;
}

enum class NumericIncrementDecrementSide {
    Left,
    Right
};

enum class WidgetEnabledState {
    Enabled,
    Disabled,
    DisabledAndGrayed
};

struct RingItem {
    std::string label;
    std::uint32_t value = 0;
    bool disabled = false;
    std::string id{};
    bool visible = true;
    // Tab pages may present a caption without changing their enum label.
    std::string caption{};
    bool captionMatchesLabel = true;
};

struct WidgetInstance;

// Tab pages reuse RingItem identities and enum choices. Children stay separate
// scene objects, so containing a widget never consumes its Diagram terminal.
struct TabContainerState {
    std::string editorPageId;
    std::uint64_t nextPageSerial = 1;
    std::uint32_t headerFirstVisible = 0;
    bool tabsVisible = true;
    bool pageLabelsDisplayVisible = false;
};

enum class ArrayElementValueKind {
    Number,
    String,
    Boolean,
    Widget
};

struct ArrayElementValue {
    std::vector<std::uint32_t> indices;
    std::string displayValue;
    ArrayElementValueKind kind = ArrayElementValueKind::Number;
    bool defined = true;
    bool enabled = true;
};

struct ArrayContainerState {
    ArrayContainerState() = default;
    ArrayContainerState(const ArrayContainerState &other);
    ArrayContainerState &operator=(const ArrayContainerState &other);
    ArrayContainerState(ArrayContainerState &&) noexcept = default;
    ArrayContainerState &operator=(ArrayContainerState &&) noexcept = default;

    std::uint32_t rank = 1;
    std::vector<std::uint32_t> shape{ 1U };
    std::vector<std::uint32_t> firstIndices{ 0U };
    std::vector<std::uint32_t> visibleCounts{ 1U };
    std::string orientation = "vertical";
    bool allowResize = true;
    // An untyped Array starts with a neutral square slot. Installing an
    // element template replaces both dimensions with that widget's bounds.
    double cellWidth = 96.0;
    double cellHeight = 96.0;
    double elementGap = 0.0;
    bool indexDisplayVisible = true;
    bool verticalScrollbarVisible = false;
    bool horizontalScrollbarVisible = false;
    double scrollbarGap = 6.0;
    double indexDisplayWidth = 54.0;
    double indexBoxHeight = 30.0;
    double indexBoxGap = 4.0;
    double indexStepWidth = 16.0;
    double indexStepGap = 2.0;
    double indexValueGap = 4.0;
    double indexContainerPadding = 4.0;
    double indexContainerBorderWidth = 1.0;
    std::optional<std::uint32_t> indexContainerFillRgb;
    std::optional<std::uint32_t> indexContainerStrokeRgb;
    std::optional<std::uint32_t> indexValueFillRgb;
    std::optional<std::uint32_t> indexValueStrokeRgb;
    std::optional<std::uint32_t> indexStepButtonFillRgb;
    std::optional<std::uint32_t> indexStepButtonStrokeRgb;
    std::optional<std::uint32_t> indexStepArrowFillRgb;
    std::optional<std::uint32_t> indexStepArrowStrokeRgb;
    std::optional<std::uint32_t> frameFillRgb;
    std::optional<std::uint32_t> frameStrokeRgb;
    std::optional<std::uint32_t> arrayRegionFillRgb;
    std::optional<std::uint32_t> arrayRegionStrokeRgb;
    bool widgetComposedElements = false;
    std::string elementClassId;
    std::string elementValueType;
    std::string elementRole;
    std::vector<ArrayElementValue> elements;
    bool elementIcon = false;
    std::shared_ptr<WidgetInstance> elementTemplate;
};

struct ClusterElement {
    std::string fieldId;
    std::string technicalName;
    std::uint32_t logicalOrder = 0U;
    std::shared_ptr<WidgetInstance> widget;
};

enum class ClusterAutoSizing { None, SizeToFit, ArrangeHorizontally, ArrangeVertically };

struct ClusterContainerState {
    ClusterContainerState() = default;
    ClusterContainerState(const ClusterContainerState &other);
    ClusterContainerState &operator=(const ClusterContainerState &other);
    ClusterContainerState(ClusterContainerState &&) noexcept = default;
    ClusterContainerState &operator=(ClusterContainerState &&) noexcept = default;

    double padding = 8.0;
    ClusterAutoSizing autoSizing = ClusterAutoSizing::None;
    std::optional<std::uint32_t> frameFillRgb;
    std::optional<std::uint32_t> frameStrokeRgb;
    std::vector<ClusterElement> elements;
    // Monotonic authoring identity; deletion never makes an old field ID free.
    // The logical order remains independent from this allocation serial.
    std::uint64_t nextFieldSerial = 0;
};

// A Picture Display is deliberately a raw two-dimensional U32 pixel matrix.
// An empty pixel vector means every cell has the numeric value zero.
struct PictureDisplayState {
    std::uint32_t width = 1U;
    std::uint32_t height = 1U;
    SharedBuffer<std::uint32_t> pixels;
};

struct FixedPointConfiguration {
    bool signedEncoding = true;
    bool complex = false;
    int wordLength = 16;
    int integerWordLength = 3;
    std::string roundingMode = "nearest";
    std::string overflowMode = "saturate";
};

struct FixedPointLimits {
    double minimum = 0.0;
    double maximum = 0.0;
    double delta = 1.0;
    int fractionalBits = 0;
};

struct WidgetInstance {
    std::string instanceId;
    std::string classId;
    std::string role;
    std::string valueType;
    Rect bounds;
    std::string label;
    // Presentation only: never used as the terminal name or binding identity.
    std::optional<std::string> caption;
    std::optional<bool> captionVisible;
    std::optional<std::string> supportTextValue;
    std::string sourceExamplePath;
    std::string wfrogPackagePath;
    std::string realizationPackageRef;
    std::string assetRef;
    std::optional<std::string> popupAssetRef;
    std::optional<std::string> defaultSvgAssetPath;
    std::optional<std::string> initialValue;
    std::optional<bool> booleanValue;
    std::optional<double> numericValue;
    std::optional<double> numericMinimum;
    std::optional<double> numericMaximum;
    std::optional<double> numericValueOffsetX;
    std::optional<double> numericValueWidth;
    std::optional<double> labelOffsetX;
    std::optional<double> labelOffsetY;
    std::optional<double> labelFontScale;
    std::optional<double> numericValueFontScale;
    std::optional<double> stringValueFontScale;
    std::optional<double> stateTextFontScale;
    std::optional<std::string> labelFontFamily;
    std::optional<std::string> numericValueFontFamily;
    std::optional<std::string> stringValueFontFamily;
    std::optional<std::string> labelTextStyle;
    std::optional<std::string> numericValueTextStyle;
    std::optional<std::string> stringValueTextStyle;
    std::optional<std::uint32_t> labelTextRgb;
    std::optional<std::uint32_t> numericValueTextRgb;
    std::optional<std::uint32_t> stringValueTextRgb;
    std::optional<std::uint32_t> numericBodyFillRgb;
    std::optional<std::uint32_t> numericBodyStrokeRgb;
    std::optional<std::uint32_t> stringBodyFillRgb;
    std::optional<std::uint32_t> stringBodyStrokeRgb;
    std::optional<std::uint32_t> numericStepButtonFillRgb;
    std::optional<std::uint32_t> numericStepButtonStrokeRgb;
    std::optional<std::uint32_t> numericStepArrowFillRgb;
    std::optional<std::uint32_t> numericStepArrowStrokeRgb;
    std::optional<std::string> labelHorizontalAlignment;
    std::optional<std::string> labelVerticalAlignment;
    std::optional<std::uint32_t> labelSurfaceFillRgb;
    std::optional<std::uint32_t> labelSurfaceStrokeRgb;
    std::optional<bool> labelVisible;
    std::optional<std::string> buttonStateFalseText;
    std::optional<std::string> buttonStateTrueText;
    std::optional<bool> buttonStateTextVisible;
    std::optional<bool> buttonStateTextLockInCenter;
    std::optional<std::uint32_t> stateFalseFillRgb;
    std::optional<std::uint32_t> stateTrueFillRgb;
    std::optional<std::uint32_t> stateFalseStrokeRgb;
    std::optional<std::uint32_t> stateTrueStrokeRgb;
    std::optional<std::uint32_t> stateTextRgb;
    std::optional<std::string> buttonMechanicalAction;
    std::optional<bool> numericIncrementDecrementVisible;
    std::optional<bool> numericRadixVisible;
    std::optional<NumericIncrementDecrementSide> numericIncrementDecrementSide;
    std::optional<bool> pathFolderVisible{};
    std::optional<bool> pathBrowseButtonVisible{};
    std::optional<bool> pathScrollbarVisible{};
    std::optional<std::string> pathOverflowMode{};
    std::optional<bool> pathSizeToText{};
    std::optional<std::string> pathBrowsePrompt{};
    std::optional<std::string> pathBrowsePatternLabel{};
    std::optional<std::string> pathBrowsePattern{};
    std::optional<std::string> pathBrowseSelectionMode{};
    std::optional<std::string> pathBrowseExistenceMode{};
    std::optional<std::string> pathBrowseStartPath{};
    std::optional<std::string> publicInputId{};
    std::optional<std::string> publicOutputId{};
    std::optional<std::string> interfaceMapSlotId{};
    std::optional<std::string> interfaceConnectionRequirement{};
    std::optional<std::string> enumDomainId{};
    std::vector<RingItem> ringItems{};
    std::optional<bool> ringAllowUndefinedValues{};
    std::optional<WidgetEnabledState> enabledState{};
    std::optional<std::string> numericRepresentationKind{};
    std::optional<double> numericIncrementStep{};
    std::optional<std::string> numericMinimumResponse{};
    std::optional<std::string> numericMaximumResponse{};
    std::optional<std::string> numericIncrementResponse{};
    std::optional<double> numericPageStep{};
    // Timestamp presentation is independent of its double-precision UTC value.
    std::optional<std::string> timestampDisplayContent; // date_time, date, time
    std::optional<bool> timestampShowSeconds;
    std::optional<bool> timestampDateFirst;
    std::optional<bool> timestampSingleLine;
    std::optional<bool> timestampClockIconVisible;
    std::optional<std::string> numericDisplayFormat{};
    std::optional<bool> choiceNumericValueVisible{};
    std::optional<int> numericDisplayDigits{};
    std::optional<std::string> numericPrecisionType{};
    std::optional<bool> numericHideTrailingZeros{};
    std::optional<bool> numericExponentMultipleOfThree{};
    std::optional<int> numericMinimumFieldWidth{};
    std::optional<std::string> numericPadding{};
    std::optional<std::string> numericAlignment{};
    std::optional<std::string> numericSignDisplay{};
    std::optional<std::string> numericLocale{};
    std::optional<std::string> numericDisplayPrefix{};
    std::optional<std::string> numericDisplaySuffix{};
    std::optional<bool> numericRadixShowPrefix{};
    std::optional<bool> numericRadixUppercase{};
    std::optional<int> numericRadixGroupSize{};
    std::optional<std::string> numericRadixInterpretation{};
    std::optional<std::string> numericCustomFormatString{};
    std::optional<std::string> contextHelpDescription{};
    std::optional<std::string> contextTip{};
    std::optional<std::string> contextHelpUrl{};
    std::optional<ArrayContainerState> arrayContainer{};
    std::optional<ClusterContainerState> clusterContainer{};
    std::optional<PictureDisplayState> pictureDisplay{};
    std::optional<graph::State> waveformGraph{};
    std::optional<std::string> stringDisplayStyle{};
    std::optional<bool> stringDisplayStyleVisible{};
    std::optional<bool> stringLimitToSingleLine{};
    std::optional<bool> stringWrapAtWordBreaks{};
    std::optional<bool> stringUpdateWhileTyping{};
    std::optional<bool> stringVerticalScrollbarVisible{};
    std::optional<bool> stringHorizontalScrollbarVisible{};
    std::optional<bool> stringSizeToText{};
    std::optional<FixedPointConfiguration> numericFixedPointConfiguration{};
    std::optional<double> placementAuraBand{};
    std::optional<std::string> choiceTextHorizontalAlignment{};
    std::optional<std::string> stateTextFontFamily{};
    std::optional<std::string> stateTextStyle{};
    // Append view preferences to preserve existing aggregate initializers.
    // These never change the opaque Variant payload.
    std::optional<bool> variantShowType{};
    std::optional<bool> variantShowData{};
    std::optional<bool> variantScrollbarVisible{};
    // Typed value snapshot. Separate from the live data and from presentation;
    // empty means the intrinsic default (legacy files are adopted on open).
    std::optional<std::string> defaultData{};
    // U32 presentation only; append to preserve existing aggregate initializers.
    bool numericColorBox = false;
    // String presentation with named choices; the data type remains String.
    bool stringDropdown = false;
    // Append appearance fields to preserve existing aggregate initializers.
    std::optional<std::uint32_t> surfaceFillRgb;
    std::optional<std::uint32_t> surfaceStrokeRgb;
    std::optional<double> captionFontScale;
    // Resolved presentation context; never an explicit per-element override.
    double inheritedTextPointSize = documentDefaultTextPointSize();
    bool defaultTextGeometryInitialized = false;
    // Caption presentation is independent from the binding Label and value.
    std::optional<std::string> captionFontFamily;
    // One-shot realization of a newly created Error palette frame. Transient:
    // loading an authored manual frame must never opt it into initial fitting.
    bool errorPaletteInitialLayoutPending = false;
    HeapOptional<TypeDefinitionLink> typeDefinition;

    // Symbolic reference VALUE, independent of a reference to this widget.
    // Never stores or serializes a native resource handle.
    std::optional<ApplicationControlNode> referenceValue;
    HeapOptional<CollectionAuthoringState> collection;
    // String value presentation, independent of the label and native value.
    std::optional<std::string> stringTextHorizontalAlignment;

    std::optional<TabContainerState> tabContainer;

    // Used only by the Front Panel Label/comment text surface.
    bool commentAutoSize = true;

    // Presentation provenance for Block Diagram literal models only.
    // Front Panel controls do not consume these optional flags.
    std::optional<bool> constantTextBoundsAutomatic;
    std::optional<bool> constantArrayCellBoundsAutomatic;

    // A graph owns typed samples independently from its presentation. Keep the
    // infrequent state off the common WidgetInstance/Array-cell allocation.
    HeapOptional<xy::State> xyGraph;

};

// Shared source-data initializer; placement-only normalization stays in Studio.
[[nodiscard]] WidgetInstance makeWidgetData(std::string instanceId,const WidgetCatalogEntry &,Rect bounds);

[[nodiscard]] WidgetInstance makeWidgetInstance(
    std::string instanceId,
    const WidgetCatalogEntry &entry,
    Rect bounds);

[[nodiscard]] bool isBooleanWidget(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool isWaveformGraphWidget(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool isXYGraphWidget(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setBooleanWidgetIndicatorRole(WidgetInstance &widget, bool indicator);
[[nodiscard]] bool setBooleanWidgetStateFillColor(
    WidgetInstance &widget,
    bool trueState,
    std::optional<std::uint32_t> rgb);
[[nodiscard]] bool setBooleanWidgetStateStrokeColor(
    WidgetInstance &widget,
    bool trueState,
    std::optional<std::uint32_t> rgb);
[[nodiscard]] bool setBooleanWidgetTextColor(WidgetInstance &widget, std::optional<std::uint32_t> rgb);
[[nodiscard]] bool isButtonWidget(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool isButtonIndicatorWidget(const WidgetInstance &widget) noexcept;
[[nodiscard]] std::string_view buttonMechanicalAction(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setButtonMechanicalAction(WidgetInstance &widget, std::string_view action);
[[nodiscard]] bool buttonStateTextLockedInCenter(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setButtonStateTextLockedInCenter(WidgetInstance &widget, bool locked);
[[nodiscard]] bool setButtonWidgetIndicatorRole(WidgetInstance &widget, bool indicator);
[[nodiscard]] bool setButtonWidgetStateText(WidgetInstance &widget, bool trueState, std::string text);
[[nodiscard]] bool setButtonWidgetStateFillColor(
    WidgetInstance &widget,
    bool trueState,
    std::optional<std::uint32_t> rgb);
[[nodiscard]] bool setButtonWidgetStateStrokeColor(
    WidgetInstance &widget,
    bool trueState,
    std::optional<std::uint32_t> rgb);
[[nodiscard]] bool setButtonWidgetTextColor(WidgetInstance &widget, std::optional<std::uint32_t> rgb);
[[nodiscard]] bool widgetBooleanValue(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setWidgetBooleanValue(WidgetInstance &widget, bool value);
[[nodiscard]] bool isNumericWidget(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool isNumericControlWidget(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool isNumericIndicatorWidget(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setNumericWidgetIndicatorRole(WidgetInstance &widget, bool indicator);
[[nodiscard]] bool isStringWidget(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool isStringDropdownWidget(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setStringDropdownSelectedValue(WidgetInstance &widget, std::uint32_t value);
[[nodiscard]] bool setStringWidgetIndicatorRole(WidgetInstance &widget, bool indicator);
[[nodiscard]] bool isPathWidget(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setPathWidgetIndicatorRole(WidgetInstance &widget, bool indicator);
[[nodiscard]] bool pathFolderVisible(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setPathFolderVisible(WidgetInstance &widget, bool visible);
[[nodiscard]] bool pathBrowseButtonVisible(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setPathBrowseButtonVisible(WidgetInstance &widget, bool visible);
[[nodiscard]] bool pathScrollbarVisible(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setPathScrollbarVisible(WidgetInstance &widget, bool visible);
[[nodiscard]] std::string_view pathOverflowMode(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setPathOverflowMode(WidgetInstance &widget, std::string_view mode);
[[nodiscard]] bool pathSizeToText(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setPathSizeToText(WidgetInstance &widget, bool enabled);
[[nodiscard]] std::string_view pathBrowsePrompt(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setPathBrowsePrompt(WidgetInstance &widget, std::string prompt);
[[nodiscard]] std::string_view pathBrowsePatternLabel(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setPathBrowsePatternLabel(WidgetInstance &widget, std::string label);
[[nodiscard]] std::string_view pathBrowsePattern(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setPathBrowsePattern(WidgetInstance &widget, std::string pattern);
[[nodiscard]] std::string_view pathBrowseSelectionMode(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setPathBrowseSelectionMode(WidgetInstance &widget, std::string_view mode);
[[nodiscard]] std::string_view pathBrowseExistenceMode(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setPathBrowseExistenceMode(WidgetInstance &widget, std::string_view mode);
[[nodiscard]] std::string_view pathBrowseStartPath(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setPathBrowseStartPath(WidgetInstance &widget, std::string path);
[[nodiscard]] bool isRingWidget(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool isRingControlWidget(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool isEnumWidget(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool isEnumControlWidget(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool isArrayWidget(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool isClusterWidget(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool isPictureWidget(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool isPictureIndicatorWidget(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setPictureWidgetIndicatorRole(WidgetInstance &widget, bool indicator);
[[nodiscard]] const PictureDisplayState *pictureDisplayState(const WidgetInstance &widget) noexcept;
[[nodiscard]] PictureDisplayState *pictureDisplayState(WidgetInstance &widget) noexcept;
[[nodiscard]] bool resizePictureDisplayToBounds(WidgetInstance &widget, Rect bounds);
[[nodiscard]] bool setPictureDisplayPixels(
    WidgetInstance &widget,
    std::uint32_t sourceWidth,
    std::uint32_t sourceHeight,
    const std::vector<std::uint32_t> &pixels);
[[nodiscard]] std::uint32_t pictureDisplayPixel(
    const WidgetInstance &widget,
    std::uint32_t x,
    std::uint32_t y) noexcept;
[[nodiscard]] bool supportsPublicInterfaceBinding(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setArrayWidgetIndicatorRole(WidgetInstance &widget, bool indicator);
[[nodiscard]] bool isSupportedArrayElementWidget(const WidgetInstance &widget) noexcept;
[[nodiscard]] const ArrayContainerState *arrayContainerState(const WidgetInstance &widget) noexcept;
[[nodiscard]] ArrayContainerState *arrayContainerState(WidgetInstance &widget) noexcept;
[[nodiscard]] const ClusterContainerState *clusterContainerState(
    const WidgetInstance &widget) noexcept;
[[nodiscard]] ClusterContainerState *clusterContainerState(WidgetInstance &widget) noexcept;
[[nodiscard]] bool setClusterWidgetIndicatorRole(WidgetInstance &widget, bool indicator);
[[nodiscard]] bool isSupportedClusterElementWidget(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool appendClusterElement(
    WidgetInstance &cluster,
    const WidgetInstance &element,
    Rect relativeBounds);
[[nodiscard]] std::uint32_t minimumArrayVisibleAxisCount(std::uint32_t rank) noexcept;
[[nodiscard]] std::uint32_t arrayVisibleCountForDimension(
    const ArrayContainerState &state,
    std::size_t dimension) noexcept;
[[nodiscard]] std::uint32_t arrayMaximumFirstIndex(
    const ArrayContainerState &state,
    std::size_t dimension) noexcept;
// Private model operations: the document adapter prepares any composed bytes.
// Initializing has no cells to migrate. Installing accepts explicit cells already
// prepared against the new prototype; neither operation calls a codec.
// Both source schemas are checked for ownership cycles before recursive copying;
// cycles are refused without mutation. Shared acyclic children remain valid.
[[nodiscard]] bool initializeArrayElementTemplate(WidgetInstance &array, const WidgetInstance &element);
[[nodiscard]] bool installPreparedArrayElementTemplate(WidgetInstance &array,
    const WidgetInstance &element, std::vector<ArrayElementValue> preparedElements);
[[nodiscard]] bool setArrayElementTemplateBounds(WidgetInstance &array, Rect bounds);
// Measures the current values once; later value edits do not resize the cells.
[[nodiscard]] bool sizeArrayToWidestElement(
    WidgetInstance &array,
    const std::function<double(const WidgetInstance &, const ArrayElementValue *)> &requiredWidth);
[[nodiscard]] Size minimumArrayBoundsForVisibleCounts(const WidgetInstance &array) noexcept;
[[nodiscard]] bool updateArrayVisibleCountsFromBounds(WidgetInstance &array, Rect bounds);
// Corner-resize policy for multi-dimensional arrays: a diagonal gesture advances
// both visible axes by the same number of cells.  The original and requested
// bounds are converted independently before the shared step is applied.
[[nodiscard]] bool updateArrayVisibleCountsFromCoupledDiagonalBounds(
    WidgetInstance &array,
    Rect originalBounds,
    Rect requestedBounds);
[[nodiscard]] bool setArrayRank(WidgetInstance &array, std::uint32_t rank);
// Display geometry only: never materializes, reshapes, or deletes array values.
[[nodiscard]] bool setArrayVisibleGrid(WidgetInstance &array, std::uint32_t rows, std::uint32_t columns);
[[nodiscard]] bool setArrayFirstIndex(
    WidgetInstance &array,
    std::size_t dimension,
    std::uint32_t value);
[[nodiscard]] bool adjustArrayFirstIndex(
    WidgetInstance &array,
    std::size_t dimension,
    int delta);
[[nodiscard]] bool setArrayNumericElementValue(
    WidgetInstance &array,
    const std::vector<std::uint32_t> &indices,
    double value);
[[nodiscard]] bool setArrayNumericElementUserValue(
    WidgetInstance &array,
    const std::vector<std::uint32_t> &indices,
    double value);
[[nodiscard]] bool adjustArrayNumericElementValue(
    WidgetInstance &array,
    const std::vector<std::uint32_t> &indices,
    int delta);
[[nodiscard]] bool setArrayStringElementValue(
    WidgetInstance &array,
    const std::vector<std::uint32_t> &indices,
    std::string value);
[[nodiscard]] bool setArrayBooleanElementValue(
    WidgetInstance &array,
    const std::vector<std::uint32_t> &indices,
    bool value);
[[nodiscard]] bool setArrayChoiceElementValue(
    WidgetInstance &array,
    const std::vector<std::uint32_t> &indices,
    std::uint32_t value);
[[nodiscard]] bool adjustArrayChoiceElementValue(
    WidgetInstance &array,
    const std::vector<std::uint32_t> &indices,
    int delta);
// Scalar transport and prepared-cell storage are independent of document JSON.
void materializeArrayScalarElementValue(WidgetInstance &widget, const ArrayElementValue *value);
[[nodiscard]] std::optional<ArrayElementValue> prepareArrayScalarElementValue(
    const WidgetInstance &array, const std::vector<std::uint32_t> &indices,
    const WidgetInstance &elementWidget);
[[nodiscard]] bool setPreparedArrayElementValue(WidgetInstance &array, ArrayElementValue value);
[[nodiscard]] bool clearArrayElementValue(
    WidgetInstance &array,
    const std::vector<std::uint32_t> &indices);
[[nodiscard]] bool insertArrayDimensionSliceBefore(
    WidgetInstance &array,
    std::size_t dimension,
    std::uint32_t index);
[[nodiscard]] bool deleteArrayDimensionSlice(
    WidgetInstance &array,
    std::size_t dimension,
    std::uint32_t index);
[[nodiscard]] bool setArrayIndexDisplayVisible(WidgetInstance &array, bool visible);
[[nodiscard]] bool setArrayVerticalScrollbarVisible(WidgetInstance &array, bool visible);
[[nodiscard]] bool setArrayHorizontalScrollbarVisible(WidgetInstance &array, bool visible);
[[nodiscard]] bool setArrayElementGap(WidgetInstance &array, double gap);
[[nodiscard]] bool setRingWidgetIndicatorRole(WidgetInstance &widget, bool indicator);
[[nodiscard]] bool setEnumWidgetIndicatorRole(WidgetInstance &widget, bool indicator);
[[nodiscard]] std::vector<RingItem> defaultRingItems();
[[nodiscard]] std::vector<RingItem> defaultEnumItems();
[[nodiscard]] const std::vector<RingItem> &ringItems(const WidgetInstance &widget) noexcept;
[[nodiscard]] std::string ringSelectedItemLabel(const WidgetInstance &widget);
[[nodiscard]] std::string ringSelectedItemId(const WidgetInstance &widget);
[[nodiscard]] std::size_t ringSelectedItemIndex(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setRingItems(WidgetInstance &widget, std::vector<RingItem> items);
[[nodiscard]] bool choiceNumericValueVisible(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setChoiceNumericValueVisible(WidgetInstance &widget, bool visible);
[[nodiscard]] std::string formatChoiceNumericValue(
    const WidgetInstance &widget,
    std::uint32_t value);
[[nodiscard]] std::string formatChoiceDisplayText(const WidgetInstance &widget);
[[nodiscard]] bool ringAllowUndefinedValues(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setRingAllowUndefinedValues(WidgetInstance &widget, bool allow);
[[nodiscard]] bool setRingSelectedValue(WidgetInstance &widget, std::uint32_t value);
[[nodiscard]] bool setRingItemDisabled(WidgetInstance &widget, std::uint32_t value, bool disabled);
[[nodiscard]] WidgetEnabledState widgetEnabledState(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool widgetInteractionEnabled(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool widgetDisabledAppearanceGrayed(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setWidgetEnabledState(WidgetInstance &widget, WidgetEnabledState state);
[[nodiscard]] bool isLabelWidget(const WidgetInstance &widget) noexcept;
[[nodiscard]] std::string_view widgetSupportTextValue(const WidgetInstance &widget) noexcept;
[[nodiscard]] std::string_view widgetStringValue(const WidgetInstance &widget) noexcept;
[[nodiscard]] std::string_view stringDisplayStyle(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool stringDisplayStyleVisible(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool stringLimitToSingleLine(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool stringWrapAtWordBreaks(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool stringUpdateWhileTyping(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool stringVerticalScrollbarVisible(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool stringHorizontalScrollbarVisible(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool stringSizeToText(const WidgetInstance &widget) noexcept;
[[nodiscard]] std::string_view widgetPathValue(const WidgetInstance &widget) noexcept;
[[nodiscard]] double widgetNumericValue(const WidgetInstance &widget) noexcept;
// Exact canonical scalar spellings only; alias resolution belongs to callers.
[[nodiscard]] bool numericCanonicalValueTypeIsIntegral(std::string_view valueType) noexcept;
[[nodiscard]] double numericMinimumForValueType(std::string_view valueType) noexcept;
[[nodiscard]] double numericMaximumForValueType(std::string_view valueType) noexcept;
[[nodiscard]] std::string formatNumericValue(double value);
[[nodiscard]] std::string formatNumericValue(const WidgetInstance &widget, double value);
// Formats the widget's actual value, preserving validated I64/U64 lexical data.
[[nodiscard]] std::string formatNumericValue(const WidgetInstance &widget);
[[nodiscard]] bool numericFormatStringIsValid(std::string_view format) noexcept;
[[nodiscard]] std::optional<double> normalizeNumericUserInput(
    const WidgetInstance &widget,
    double value) noexcept;
[[nodiscard]] std::string numericRepresentationKind(const WidgetInstance &widget);
[[nodiscard]] std::string numericNamedSize(const WidgetInstance &widget);
[[nodiscard]] bool setNumericRepresentationKind(WidgetInstance &widget, std::string_view representation);
[[nodiscard]] FixedPointConfiguration defaultFixedPointConfiguration() noexcept;
[[nodiscard]] FixedPointConfiguration normalizedFixedPointConfiguration(
    FixedPointConfiguration configuration) noexcept;
[[nodiscard]] FixedPointConfiguration fixedPointConfiguration(const WidgetInstance &widget) noexcept;
[[nodiscard]] FixedPointLimits fixedPointLimits(const FixedPointConfiguration &configuration) noexcept;
[[nodiscard]] std::string fixedPointTypeSignature(const FixedPointConfiguration &configuration);
[[nodiscard]] std::string fixedPointNamedSize(const FixedPointConfiguration &configuration);
[[nodiscard]] double coerceFixedPointValue(
    double value,
    const FixedPointConfiguration &configuration) noexcept;
[[nodiscard]] bool fixedPointConfigurationsEqual(
    const FixedPointConfiguration &left,
    const FixedPointConfiguration &right) noexcept;
[[nodiscard]] bool setWidgetFixedPointConfiguration(
    WidgetInstance &widget,
    const FixedPointConfiguration &configuration);
[[nodiscard]] std::string enumRepresentationKind(const WidgetInstance &widget);
[[nodiscard]] std::string enumRepresentationValueType(const WidgetInstance &widget);
[[nodiscard]] std::string enumNamedSize(const WidgetInstance &widget);
[[nodiscard]] bool setEnumRepresentationKind(WidgetInstance &widget, std::string_view representation);
[[nodiscard]] std::string ringRepresentationKind(const WidgetInstance &widget);
[[nodiscard]] std::string ringRepresentationValueType(const WidgetInstance &widget);
[[nodiscard]] std::string ringNamedSize(const WidgetInstance &widget);
[[nodiscard]] bool setRingRepresentationKind(WidgetInstance &widget, std::string_view representation);
// Value entry keeps range/coercion guards; step/page settings alone govern buttons.
[[nodiscard]] bool numericUsesTypeDefaultValueEntry(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool numericUsesTypeDefaultDataEntry(const WidgetInstance &widget) noexcept;
[[nodiscard]] double widgetNumericIncrementStep(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setWidgetNumericIncrementStep(WidgetInstance &widget, double step);
// Exact integral source/value transfer; numericValue is a derived projection.
// These bypass interactive Data Entry and mutate no identity or presentation.
[[nodiscard]] std::optional<std::string> widgetNumericIntegerValueLexeme(const WidgetInstance &widget);
[[nodiscard]] bool setWidgetNumericIntegerValueLexeme(WidgetInstance &widget, std::string_view text);
// Private complex source transport. initialValue owns both components; numericValue
// is only the optional real-component cache. Invalid edits never mutate either.
[[nodiscard]] bool numericCanonicalValueTypeIsComplex(std::string_view valueType) noexcept;
[[nodiscard]] std::optional<std::string> widgetNumericComplexValueLexeme(const WidgetInstance &widget);
[[nodiscard]] bool setWidgetNumericComplexValueLexeme(WidgetInstance &widget, std::string_view text);
[[nodiscard]] std::string formatWidgetNumericComplexValue(const WidgetInstance &widget);
[[nodiscard]] bool setWidgetNumericValue(WidgetInstance &widget, double value);
[[nodiscard]] bool setWidgetNumericUserValue(WidgetInstance &widget, double value);
[[nodiscard]] bool setWidgetNumericValueGeometry(WidgetInstance &widget, double offsetX, double width);
[[nodiscard]] bool setWidgetLabelOffset(WidgetInstance &widget, double offsetX, double offsetY);
[[nodiscard]] bool setWidgetLabel(WidgetInstance &widget, std::string label);
[[nodiscard]] bool setWidgetSupportText(WidgetInstance &widget, std::string text);
[[nodiscard]] bool setWidgetStringValue(WidgetInstance &widget, std::string text);
[[nodiscard]] bool setStringDisplayStyle(WidgetInstance &widget, std::string_view style);
[[nodiscard]] bool setStringDisplayStyleVisible(WidgetInstance &widget, bool visible);
[[nodiscard]] bool setStringLimitToSingleLine(WidgetInstance &widget, bool enabled);
[[nodiscard]] bool setStringWrapAtWordBreaks(WidgetInstance &widget, bool enabled);
[[nodiscard]] bool setStringUpdateWhileTyping(WidgetInstance &widget, bool enabled);
[[nodiscard]] bool setStringVerticalScrollbarVisible(WidgetInstance &widget, bool visible);
[[nodiscard]] bool setStringHorizontalScrollbarVisible(WidgetInstance &widget, bool visible);
[[nodiscard]] bool setStringSizeToText(WidgetInstance &widget, bool enabled);
[[nodiscard]] bool setWidgetPathValue(WidgetInstance &widget, std::string text);
[[nodiscard]] bool widgetLabelVisible(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setWidgetLabelVisible(WidgetInstance &widget, bool visible);
[[nodiscard]] bool numericIncrementDecrementVisible(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setNumericIncrementDecrementVisible(WidgetInstance &widget, bool visible);
[[nodiscard]] bool numericRadixVisible(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setNumericRadixVisible(WidgetInstance &widget, bool visible);
[[nodiscard]] NumericIncrementDecrementSide numericIncrementDecrementSide(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setNumericIncrementDecrementSide(WidgetInstance &widget, NumericIncrementDecrementSide side);
[[nodiscard]] double widgetLabelFontScale(const WidgetInstance &widget) noexcept;
[[nodiscard]] double widgetCaptionFontScale(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setWidgetCaptionFontScale(WidgetInstance &widget, double scale);
[[nodiscard]] bool setWidgetCaptionFontFamily(WidgetInstance &widget, std::string family);
void resetWidgetTextSizesToDefault(WidgetInstance &widget);
[[nodiscard]] double widgetNumericValueFontScale(const WidgetInstance &widget) noexcept;
[[nodiscard]] double widgetStringValueFontScale(const WidgetInstance &widget) noexcept;
[[nodiscard]] double widgetStateTextFontScale(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setWidgetLabelFontScale(WidgetInstance &widget, double scale);
[[nodiscard]] bool setWidgetNumericValueFontScale(WidgetInstance &widget, double scale);
[[nodiscard]] bool setWidgetStringValueFontScale(WidgetInstance &widget, double scale);
[[nodiscard]] bool setWidgetStateTextFontScale(WidgetInstance &widget, double scale);
[[nodiscard]] bool setWidgetLabelFontFamily(WidgetInstance &widget, std::string family);
[[nodiscard]] bool setWidgetNumericValueFontFamily(WidgetInstance &widget, std::string family);
[[nodiscard]] bool setWidgetStringValueFontFamily(WidgetInstance &widget, std::string family);
[[nodiscard]] bool setWidgetStateTextFontFamily(WidgetInstance &widget, std::string family);
// Styles are a canonical space-separated set; plain denotes the empty set.
[[nodiscard]] bool textStyleHasFlag(std::string_view style, std::string_view flag) noexcept;
[[nodiscard]] std::string canonicalTextStyle(std::string_view style);
[[nodiscard]] std::string withTextStyleFlag(std::string_view style, std::string_view flag, bool enabled);
[[nodiscard]] std::string_view widgetLabelTextStyle(const WidgetInstance &widget) noexcept;
[[nodiscard]] std::string_view widgetNumericValueTextStyle(const WidgetInstance &widget) noexcept;
[[nodiscard]] std::string_view widgetStringValueTextStyle(const WidgetInstance &widget) noexcept;
[[nodiscard]] std::string_view widgetStateTextStyle(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setWidgetLabelTextStyle(WidgetInstance &widget, std::string_view style);
[[nodiscard]] bool setWidgetNumericValueTextStyle(WidgetInstance &widget, std::string_view style);
[[nodiscard]] bool setWidgetStringValueTextStyle(WidgetInstance &widget, std::string_view style);
[[nodiscard]] bool setWidgetStateTextStyle(WidgetInstance &widget, std::string_view style);
[[nodiscard]] std::string_view widgetChoiceTextHorizontalAlignment(const WidgetInstance &widget) noexcept;
[[nodiscard]] std::string_view widgetStringTextHorizontalAlignment(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setWidgetStringTextHorizontalAlignment(WidgetInstance &widget, std::string_view alignment);
[[nodiscard]] bool setWidgetChoiceTextHorizontalAlignment(
    WidgetInstance &widget,
    std::string_view alignment);
[[nodiscard]] bool setWidgetLabelTextColor(WidgetInstance &widget, std::optional<std::uint32_t> rgb);
[[nodiscard]] bool setWidgetNumericValueTextColor(WidgetInstance &widget, std::optional<std::uint32_t> rgb);
[[nodiscard]] bool setWidgetStringValueTextColor(WidgetInstance &widget, std::optional<std::uint32_t> rgb);
[[nodiscard]] bool setWidgetNumericBodyFillColor(WidgetInstance &widget, std::optional<std::uint32_t> rgb);
[[nodiscard]] bool setWidgetSurfaceColors(
    WidgetInstance &widget,
    std::optional<std::uint32_t> fill,
    std::optional<std::uint32_t> stroke,
    std::optional<bool> displayedBooleanState = std::nullopt);
[[nodiscard]] bool setWidgetNumericBodyStrokeColor(WidgetInstance &widget, std::optional<std::uint32_t> rgb);
[[nodiscard]] bool setWidgetStringBodyFillColor(WidgetInstance &widget, std::optional<std::uint32_t> rgb);
[[nodiscard]] bool setWidgetStringBodyStrokeColor(WidgetInstance &widget, std::optional<std::uint32_t> rgb);
[[nodiscard]] bool setWidgetNumericStepButtonFillColor(WidgetInstance &widget, std::optional<std::uint32_t> rgb);
[[nodiscard]] bool setWidgetNumericStepButtonStrokeColor(WidgetInstance &widget, std::optional<std::uint32_t> rgb);
[[nodiscard]] bool setWidgetNumericStepArrowFillColor(WidgetInstance &widget, std::optional<std::uint32_t> rgb);
[[nodiscard]] bool setWidgetNumericStepArrowStrokeColor(WidgetInstance &widget, std::optional<std::uint32_t> rgb);
[[nodiscard]] std::string_view widgetLabelHorizontalAlignment(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setWidgetLabelHorizontalAlignment(WidgetInstance &widget, std::string_view alignment);
[[nodiscard]] std::string_view widgetLabelVerticalAlignment(const WidgetInstance &widget) noexcept;
[[nodiscard]] bool setWidgetLabelVerticalAlignment(WidgetInstance &widget, std::string_view alignment);
[[nodiscard]] bool setWidgetLabelSurfaceFillColor(WidgetInstance &widget, std::optional<std::uint32_t> rgb);
[[nodiscard]] bool setWidgetLabelSurfaceStrokeColor(WidgetInstance &widget, std::optional<std::uint32_t> rgb);
[[nodiscard]] bool adjustWidgetNumericValue(WidgetInstance &widget, int delta);

} // namespace frog::native_engine
