#pragma once
#include "frog_native_engine/NativeProgramOptions.hpp"
#include "frog_native_engine/SharedBuffer.hpp"

#include "frog_native_engine/DiagramShapeCatalog.hpp"
#include "frog_native_engine/DiagramExecutionContract.hpp"
#include "frog_native_engine/InPlaceElementContract.hpp"
#include "frog_native_engine/BooleanArrayConversion.hpp"
#include "frog_native_engine/FrogWidgetData.hpp"
#include "frog_native_engine/FrogDocumentWidgetValues.hpp"
#include "frog_native_engine/FrogJson.hpp"
#include "frog_native_engine/FrogDraftInterface.hpp"
#include "frog_native_engine/HeapOptional.hpp"
#include "frog_native_engine/ContainedWidgetPath.hpp"
#include "frog_native_engine/ApplicationControlTypes.hpp"
#include "frog_native_engine/ConnectivityData.hpp"
#include "frog_native_engine/NativeCall.hpp"
#include "frog_native_engine/DocumentTextDefaults.hpp"
#include "frog_native_engine/CollectionAuthoringState.hpp"

#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace frog::native_engine {

class FrogSourceDocument;

struct FrogFileVersionMetadata {
    std::optional<std::string> createdBy;
    std::optional<std::string> savedBy;
    [[nodiscard]] bool operator==(const FrogFileVersionMetadata &) const noexcept = default;
};

inline constexpr double kDiagramWidgetTerminalWidth =
    diagramElementGeometry(DiagramElementStandard::WidgetTerminal).naturalWidth;
inline constexpr double kDiagramWidgetTerminalHeight =
    diagramElementGeometry(DiagramElementStandard::WidgetTerminal).naturalHeight;
// A For Loop owns one persistent subdiagram. Keeping the region explicit in
// the document avoids inferring encapsulation from visual enclosure and makes
// nested structures round-trip without flattening their body into the parent.
inline constexpr std::string_view kForLoopBodyRegionId = "body";
inline constexpr double kStructureSubdiagramLabelMinimumHeight = 24.0;
inline constexpr double kStructureSubdiagramLabelDefaultHeight = 48.0;
inline constexpr double kStructureSubdiagramLabelMaximumHeight = 160.0;
inline constexpr double kStructureSubdiagramLabelDefaultFontPointSize = 11.0;
inline constexpr std::string_view kWhileLoopBodyRegionId = "body";
inline constexpr std::string_view kTimedLoopBodyRegionId = "body";
inline constexpr std::string_view kEventStructureTimeoutRegionId = "timeout";
inline constexpr std::string_view kDisabledStructureEnabledRegionId = "enabled";
inline constexpr std::string_view kDisabledStructureDisabledRegionId = "disabled";
inline constexpr std::string_view kConditionalDisablePrimaryRegionId =
    "condition_1";
inline constexpr std::string_view kConditionalDisableDefaultRegionId =
    "default";

struct DiagramCallPort {
    std::string id;
    std::string sourcePortId;
    std::string interfaceMapSlotId;
    std::string label;
    std::string valueType;
    bool input = true;
    std::string connectionRequirement = "recommended";
    double normalizedX = 0.0;
    double normalizedY = 0.5;
};

// A Case Structure tunnel is a typed, movable passage through the structure
// border.  The two graph ports share this persisted identity: one belongs to
// the outer diagram and the other to the active case's inner diagram.

struct DiagramCaseStructureTunnel {
    std::string id;
    std::string valueType = "unknown";
    std::string side = "left";
    double normalizedPosition = 0.5;
    bool outputFromStructure = false;
    // Non-empty on the linked faces of one For/While Loop shift register.
    // All faces share one inferred data type; side identifies the input
    // (left) and output (right) presentation. An outer wire is optional:
    // without one, the first inner read uses the resolved type's default;
    // later iterations read the preceding inner write.
    std::string shiftRegisterPairId;
    // Output tunnels require an inner source in each applicable subdiagram.
    // This flag permits a resolved type default in unwired subdiagrams only;
    // wired producers always win. It does not apply to shift registers.
    bool useDefaultIfUnwired = false;
    // Omitted in legacy documents: preserve their non-indexing transport.
    DiagramTunnelMode mode = DiagramTunnelMode::LastValue;
    bool conditional = false;
    // Sequence tunnels have one consuming/producing frame, not a Case selector.
    std::string sequenceFrameId;
    // Editor association on a Case output, separate from the actual per-case
    // wires. Stable input tunnel ID; never enables default values implicitly.
    std::string linkedInputTunnelId;
    // Left stack order, independent of vector/geometry order: 0 reads t-1,
    // 1 reads t-2, etc. A register has contiguous left indices and exactly one
    // right writer at index 0. Legacy pairs omit this field and retain index 0.
    std::size_t shiftRegisterHistoryIndex = 0;
    DiagramTunnelIndexingPolicy indexingPolicy=DiagramTunnelIndexingPolicy::LeadingDimension;
    // Sequence-only: the other face belongs to this frame instead of the
    // parent diagram. Empty preserves the historical external-boundary model.
    std::string sequenceOuterFrameId;
    // Provenance of an explicit editor migration of an exterior Sequence
    // corridor. Its real frame passages preserve the former publication scope
    // when projected for execution; ordinary authored transfers leave it empty.
    std::string sequenceLegacyFrameId;
    std::string sequenceLegacyEdgeId;
};

struct DiagramDisabledRegion {
    std::string id;
    bool enabled = false;
    std::string subdiagramLabelText;
    bool operator==(const DiagramDisabledRegion &) const = default;
};

struct DiagramSequenceFrame {
    std::string id = "frame_0";
    std::string title;
    double width = 240.0;
    std::string subdiagramLabelText;
    bool operator==(const DiagramSequenceFrame &) const = default;
};

struct DiagramEventBinding {
    std::string kind;
    std::string source;
    // Optional Studio authoring metadata. Empty preserves legacy Application
    // (empty source) / widget (nonempty source) identities without migration.
    std::string sourceScope;
    bool operator==(const DiagramEventBinding &) const = default;
};

struct DiagramEventCase {
    std::string id = std::string(kEventStructureTimeoutRegionId);
    std::string label = "[0] Timeout";
    std::vector<std::string> eventDataFields{ "source", "type", "time" };
    // Event identity is independent from its Studio authoring label. The
    // descriptor is preserved separately from private shelf/selection metadata;
    // Studio payload typing does not establish source/FIR conformance.
    std::string eventKind = "timeout";
    std::string eventSource;
    // New keyboard cases use the versioned semantic enum. Readers explicitly
    // retain false for historical absent markers (native U32, never ordinals).
    bool virtualKeyEnum = true;
    // Empty retains the legacy single kind/source descriptor. A configured
    // case can listen to several notifications with the common data schema.
    std::vector<DiagramEventBinding> eventBindings;
    std::string subdiagramLabelText;
    // Private Studio shelf selection. Absent retains all historical response
    // inputs; an explicit empty list exposes none. This is not a public event
    // dispatch contract. The first row keeps event_response.<semantic id>;
    // repeated authoring rows append #1, #2, ... without claiming arbitration.
    std::optional<std::vector<std::string>> eventResponseFields;
    // Presentation belongs to the stable case identity. Missing positions
    // inherit the historical structure-wide position without materialization.
    std::optional<double> eventDataNodeNormalizedY;
    bool eventDataNodeVisible = true;
    bool operator==(const DiagramEventCase &) const = default;
};

// Conditional Disable conditions are stored in disjunctive normal form:
// terms are OR-ed, comparisons inside one term are AND-ed. This preserves the
// public precedence rule (AND before OR) without keeping an opaque expression
// string as the semantic source of truth.
struct DiagramConditionalDisableComparison {
    std::string symbol = "TARGET_TYPE";
    std::string comparison = "==";
    std::string value = "Windows";
};

struct DiagramConditionalDisableTerm {
    std::vector<DiagramConditionalDisableComparison> comparisons;
};

struct DiagramConditionalDisableCase {
    std::string id = std::string(kConditionalDisablePrimaryRegionId);
    bool isDefault = false;
    std::vector<DiagramConditionalDisableTerm> anyOf{
        DiagramConditionalDisableTerm{
            std::vector<DiagramConditionalDisableComparison>{
                DiagramConditionalDisableComparison{} } } };
    // An explicit authoring draft after Remove Default. Never executable and
    // distinct from an invalid/malformed serialized comparison.
    bool conditionDraft = false;
    // Per-region documentation, independent of the compilation condition.
    std::string subdiagramLabelText;
};

// Durable scheduling inputs for the Timed Loop authoring contract. Values are
// expressed in ticks of the selected timing source; -1 keeps the platform or
// runtime choice automatic where applicable.
struct DiagramTimedLoopSchedule {
    std::string timingSource = "internal_1khz";
    std::int64_t period = 1000;
    std::int64_t offset = 0;
    std::int64_t deadline = 1000;
    std::int64_t timeout = -1;
    std::int64_t priority = 100;
    std::int64_t processor = -1;
    std::string missedPeriods = "discard";
    std::string phasePolicy = "maintain";
    bool operator==(const DiagramTimedLoopSchedule &) const = default;
};

// Versioned local Timed Loop authoring options. Disabled next values are
// retained as fallbacks, not sent to the next iteration. Legacy documents
// retain their literal deadline; new palette nodes opt into period linking.
struct DiagramTimedLoopOptions {
    std::string deadlineMode = "custom";
    std::string nextDeadlineMode = "custom";
    DiagramTimedLoopSchedule next;
    std::vector<std::string> nextEnabled;
    bool operator==(const DiagramTimedLoopOptions &) const = default;
};

struct DiagramEventRegistrationRow {
    // Durable terminal identity, independent of display order or source label.
    std::string id;
    std::string eventKind = "user_event";
    bool operator==(const DiagramEventRegistrationRow &) const = default;
};
enum class DiagramCommentTargetKind { Node, Wire, WireFragment };

struct DiagramCommentAttachment {
    DiagramCommentTargetKind kind = DiagramCommentTargetKind::Node;
    std::string targetId;
    Point nodeAnchor{0.5, 0.5};
    double wirePosition = 0.5;

    bool operator==(const DiagramCommentAttachment &other) const {
        return kind == other.kind && targetId == other.targetId &&
            nodeAnchor.x == other.nodeAnchor.x && nodeAnchor.y == other.nodeAnchor.y &&
            wirePosition == other.wirePosition;
    }
};

struct CommentTextFormat {
    std::string fontFamily;
    std::string textStyle = "plain";
    double fontPointSize = documentDefaultTextPointSize();
    std::optional<std::uint32_t> textRgb;
    bool useThemeTextColor = true;

    bool operator==(const CommentTextFormat &) const = default;
};

struct CommentTextRun {
    // Half-open ranges in UTF-16 code units, matching the native text editor.
    std::size_t start = 0;
    std::size_t length = 0;
    CommentTextFormat format;

    bool operator==(const CommentTextRun &) const = default;
};

struct StatisticsNodeProjection;
struct DiagramNode {
    std::string id;
    std::string kind = "primitive";
    std::string type;
    Rect layout{ 0.0, 0.0, 48.0, 48.0 };
    std::uint32_t indexArrayOutputs = 1U;
    // Missing on legacy Retain First Error nodes, whose height encoded 24-unit
    // rows. Explicit arity keeps compact presentation independent of migration.
    std::optional<std::uint32_t> retainFirstErrorInputs;
    // Derived editor layout cache, never serialized or used as type evidence.
    std::uint32_t indexArrayRank = 1U;
    std::uint32_t arrayEditGroups = 1U;
    // Derived from the connected array, never type evidence or persisted.
    std::uint32_t arrayEditRank = 1U;
    // Older Build Array nodes had two fixed inputs; palette placement uses one.
    std::uint32_t buildArrayInputs = 2U;
    bool buildArrayConcatenate = false;
    // Comparison palette: element-wise by default, scalar aggregate result
    // when explicitly selected. This is authoring configuration, not runtime.
    bool comparisonAggregates = false;
    // In Range and Coerce: [lower, upper) is the legacy/default presentation.
    bool inRangeIncludeUpperLimit = false;
    bool inRangeIncludeLowerLimit = true;
    // Private File authoring options. Live file references never belong to a
    // document: only the operation and these choices survive save/copy/Undo.
    bool fileReadLines = false;
    bool fileConvertEol = true;
    std::string fileDataType = "f64";
    bool fileXmlArray = false;
    BooleanArraySignExtensionMode booleanArraySignExtensionMode =
        BooleanArraySignExtensionMode::IfOutputIsSigned;
    // Durable count of size terminals: determines Reshape Array output rank.
    std::uint32_t reshapeArrayDimensions = 1U;
    // Count of repeated terminals/groups for the remaining Array palette nodes.
    // Zero preserves each older node's fixed-port default on loading.
    std::uint32_t arrayFunctionTerminals = 0U;
    std::uint32_t stringFunctionTerminals = 1U;
    std::vector<DiagramEventRegistrationRow> eventRegistrationRows;
    bool eventDynamicTerminalsVisible = false;
    std::uint32_t clusterFunctionTerminals = 0U;
    // Nominal choices survive field reordering and temporarily hidden rows.
    std::vector<std::string> clusterFieldNames;
    // Stable field identities survive renames and reject removed fields even
    // when another field later receives the same display name.
    std::vector<std::vector<std::string>> clusterFieldIdPaths;
    // Controls whether By Name rows show the complete nested field path.
    bool clusterShowFullNames = false;
    // Current literal-format inference, rebuilt from the graph; never persisted.
    std::optional<std::uint32_t> stringFunctionDerivedTerminals;
    bool matrixAppendByRows = false;
    std::string valueType;
    std::string widgetId;
    // Literal components share the diagram's editor storage, but are data,
    // not independently executable/wireable nodes or subdiagram members.
    std::string constantOwnerNodeId;
    std::string constantFieldId;
    std::uint32_t constantFieldOrder = 0;
    bool clusterConstantCollapsed = false;
    // Cells share a presentation/schema template, never mutable values.
    HeapOptional<WidgetInstance> arrayElementTemplate;
    bool arrayElementIcon = false;
    std::string clusterConstantAutoSizing = "none";
    double clusterConstantExpandedWidth = 180.0;
    double clusterConstantExpandedHeight = 110.0;
    std::string label;
    bool labelVisible = true;
    HeapOptional<StaticVisual> staticVisual;
    std::string text;
    std::string fontFamily;
    std::string textStyle = "plain";
    double fontPointSize = documentDefaultTextPointSize();
    std::optional<std::uint32_t> textRgb;
    std::optional<std::uint32_t> textBackgroundRgb;
    std::string textVerticalArrangement = "none";
    double labelOffsetX = 0.0;
    double labelOffsetY = 0.0;
    std::string labelFontFamily;
    std::string labelTextStyle = "plain";
    double labelFontPointSize = documentDefaultTextPointSize();
    std::optional<std::uint32_t> labelTextRgb;
    std::optional<std::uint32_t> labelBackgroundRgb;
    double labelWidth = 24.0;
    double labelHeight = 31.0;
    bool labelAutoSize = true;
    std::string labelVerticalArrangement = "none";
    std::optional<bool> numericRadixVisible;
    std::optional<std::string> numericDisplayFormat;
    std::optional<int> numericDisplayDigits;
    std::optional<std::string> numericPrecisionType;
    std::optional<bool> numericHideTrailingZeros;
    std::optional<bool> numericExponentMultipleOfThree;
    std::optional<int> numericMinimumFieldWidth;
    std::optional<std::string> numericPadding;
    std::optional<std::string> numericAlignment;
    std::optional<std::string> numericSignDisplay;
    std::optional<std::string> numericLocale;
    std::optional<std::string> numericDisplayPrefix;
    std::optional<std::string> numericDisplaySuffix;
    std::optional<bool> numericRadixShowPrefix;
    std::optional<bool> numericRadixUppercase;
    std::optional<int> numericRadixGroupSize;
    std::optional<std::string> numericRadixInterpretation;
    std::optional<std::string> numericCustomFormatString;
    std::string stringDisplayStyle = "normal";
    bool stringDisplayStyleVisible = false;
    bool stringLimitToSingleLine = false;
    bool stringWrapAtWordBreaks = true;
    bool stringUpdateWhileTyping = false;
    bool stringVerticalScrollbarVisible = false;
    bool stringHorizontalScrollbarVisible = false;
    bool stringSizeToText = false;
    // Unset identifies legacy geometry; false records an explicit user resize.
    // Presentation provenance only, independent of the constant's data/type.
    std::optional<bool> constantTextBoundsAutomatic;
    std::optional<bool> constantArrayCellBoundsAutomatic;
    std::string stringTextAlignment = "left";
    bool pathFolderVisible = false;
    bool pathBrowseButtonVisible = false;
    bool pathScrollbarVisible = false;
    std::string pathOverflowMode = "abbreviate";
    bool pathSizeToText = false;
    std::string pathBrowsePrompt;
    std::string pathBrowsePatternLabel;
    std::string pathBrowsePattern = "*.*";
    std::string pathBrowseSelectionMode = "files";
    std::string pathBrowseExistenceMode = "existing";
    std::string pathBrowseStartPath;
    std::vector<RingItem> enumItems;
    std::uint32_t enumSelectedValue = 0U;
    bool enumTextOverflowVisible = true;
    bool enumDigitalDisplayVisible = false;
    bool ringAllowUndefinedValues = false;
    std::uint32_t numericArrayRank = 1U;
    std::vector<std::uint32_t> numericArrayShape;
    std::vector<std::uint32_t> numericArrayFirstIndices;
    std::vector<std::uint32_t> numericArrayVisibleCounts;
    bool numericArrayOneDimensionalVertical = false;
    double numericArrayCellWidth = 48.0;
    double numericArrayElementGap = 0.0;
    double numericArrayCellHeight = 20.0;
    SharedBuffer<std::string> numericArrayValues;
    bool numericArrayElementLabelVisible = false;
    bool hidden = false;
    bool locked = false;
    std::string groupId;
    std::string documentationSummary;
    std::string documentationUrl;
    // Instance notes are separate from the published function/FROG help.
    // For literal data objects, an explicit description also supplies Context Help.
    std::optional<std::string> instanceDescription;
    std::optional<std::string> instanceTip;
    std::vector<std::string> invertedInputPorts;
    std::string feedbackNodeConfiguration = "inverse";
    // Empty values identify the outer diagram. Region-local nodes carry an
    // explicit owner and region identity; geometry is deliberately not used as
    // persisted evidence of structural membership.
    std::string diagramOwnerStructureId;
    std::string diagramOwnerRegionId;
    bool caseStructureTrueVisible = true;
    // Empty authoring collections retain the legacy Boolean False/True pair.
    // Once another supported type is connected, selector type, matchers and
    // the visible region are persisted explicitly in canonical .frog source.
    std::string caseStructureSelectorValueType = "bool";
    std::vector<DiagramCaseStructureRegion> caseStructureRegions;
    bool caseStructureManualOrder = false;
    std::string caseStructureVisibleRegionId;
    std::vector<RingItem> caseStructureSelectorEnumItems;
    bool caseStructureStringCaseInsensitive = false;
    int caseStructureRadix = 10;
    double caseStructureSelectorNormalizedY = 0.5;
    std::vector<DiagramCaseStructureTunnel> caseStructureTunnels;
    std::optional<std::uint32_t> caseStructureTrueBackgroundRgb;
    int caseStructureTrueBackgroundOpacityPercent = 100;
    std::optional<std::uint32_t> caseStructureTrueSubframeRgb;
    int caseStructureTrueSubframeOpacityPercent = 100;
    std::optional<std::uint32_t> caseStructureFalseBackgroundRgb;
    int caseStructureFalseBackgroundOpacityPercent = 100;
    std::optional<std::uint32_t> caseStructureFalseSubframeRgb;
    int caseStructureFalseSubframeOpacityPercent = 100;
    // Conditional owns one text per retained subdiagram. Visibility and height
    // remain common so switching True/False never changes container geometry.
    std::string caseStructureTrueSubdiagramLabelText;
    std::string caseStructureFalseSubdiagramLabelText;
    // The iteration terminal belongs to the For Loop body and can be placed
    // freely inside that encapsulated diagram. Normalized coordinates keep
    // its position stable when the structure is resized.
    double forLoopIterationNormalizedX = 0.02;
    double forLoopIterationNormalizedY = 0.98;
    bool forLoopIterationVisible = true;
    // Optional early-stop input; unlike iteration visibility this changes the
    // loop contract. Uses the shared While-style condition position/mode below.
    bool forLoopConditionalTerminal = false;
    // Authoring policy; does not change the structure's execution semantics.
    bool structureAutoGrow = true;
    // The subdiagram label is an authoring-only text surface fixed to the
    // inner top wall. It follows and resizes with the structure; unlike the
    // ordinary element label it has no independent position or drag state.
    bool structureSubdiagramLabelVisible = false;
    std::string forLoopSubdiagramLabelText;
    double structureSubdiagramLabelHeight =
        kStructureSubdiagramLabelDefaultHeight;
    // Shared by every retained region/frame, like the subdiagram label font.
    std::string structureSubdiagramLabelAlignment = "center";
    // Typography and surface colors use the shared Diagram label font fields.
    // The border remains specific because a Subdiagram Label is an integrated
    // structure region rather than an independently positioned terminal label.
    std::optional<std::uint32_t> structureSubdiagramLabelBorderRgb;
    std::optional<std::uint32_t> forLoopBackgroundRgb;
    int forLoopBackgroundOpacityPercent = 100;
    std::optional<std::uint32_t> forLoopSubframeRgb;
    int forLoopSubframeOpacityPercent = 100;
    // Both intrinsic While Loop terminals belong to the encapsulated body.
    // The optional For condition uses the same position and Stop/Continue mode.
    // Their normalized positions make them independently movable while still
    // following the structure during a move or resize.
    double whileLoopIterationNormalizedX = 0.08;
    double whileLoopIterationNormalizedY = 0.90;
    double whileLoopConditionNormalizedX = 0.92;
    double whileLoopConditionNormalizedY = 0.90;
    // Visibility is an authoring choice only: the index keeps its stable
    // semantic output identity so existing wires survive a hide/show cycle.
    bool whileLoopIterationVisible = true;
    bool whileLoopConditionVisible = true;
    // True displays the red "stop if True" terminal. False displays the
    // green "continue if True" terminal.
    bool whileLoopStopIfTrue = true;
    std::optional<std::uint32_t> whileLoopBackgroundRgb;
    int whileLoopBackgroundOpacityPercent = 100;
    std::optional<std::uint32_t> whileLoopSubframeRgb;
    int whileLoopSubframeOpacityPercent = 100;
    // Timed Loop retains the While body semantics while exposing its
    // scheduler as four expandable boundary data nodes.
    DiagramTimedLoopSchedule timedLoopSchedule;
    DiagramTimedLoopOptions timedLoopOptions;
    double timedLoopIterationNormalizedX = 0.08;
    double timedLoopIterationNormalizedY = 0.90;
    double timedLoopConditionNormalizedX = 0.92;
    double timedLoopConditionNormalizedY = 0.90;
    // A newly placed Timed Loop starts in the compact LabVIEW-like form. The
    // visible field vectors are deliberately independent: every shelf row can
    // expose any field from its own role catalogue, including duplicates.
    std::vector<std::string> timedLoopInitialFields{
        "source", "period", "offset", "processor" };
    std::vector<std::string> timedLoopFeedbackFields{ "error" };
    std::vector<std::string> timedLoopUpdateFields{ "mode" };
    std::vector<std::string> timedLoopResultFields{ "error" };
    std::uint32_t timedLoopInitialVisibleRowCount = 4U;
    std::uint32_t timedLoopFeedbackVisibleRowCount = 1U;
    std::uint32_t timedLoopUpdateVisibleRowCount = 1U;
    std::uint32_t timedLoopResultVisibleRowCount = 1U;
    double timedLoopInitialTimingNodeNormalizedY = 0.0;
    double timedLoopFeedbackTimingNodeNormalizedY = 0.0;
    double timedLoopUpdateTimingNodeNormalizedY = 0.0;
    double timedLoopResultTimingNodeNormalizedY = 0.0;
    std::optional<std::uint32_t> timedLoopBackgroundRgb;
    int timedLoopBackgroundOpacityPercent = 100;
    std::optional<std::uint32_t> timedLoopSubframeRgb;
    int timedLoopSubframeOpacityPercent = 100;
    // Each Event Case owns a durable region id and event descriptor. The Studio
    // data/response shelves store private presentation alongside that identity;
    // these selections and coordinates are not executable occurrence semantics.
    std::vector<DiagramEventCase> eventStructureCases{ DiagramEventCase{} };
    std::size_t eventStructureVisibleCaseIndex = 0U;
    // Historical fallback for cases without an authored regional position.
    double eventStructureDataNodeNormalizedY = 0.5;
    std::optional<std::uint32_t> eventStructureBackgroundRgb;
    int eventStructureBackgroundOpacityPercent = 100;
    std::optional<std::uint32_t> eventStructureSubframeRgb;
    int eventStructureSubframeOpacityPercent = 100;
    // Legacy two-region view flag; new edits use the stable ID and region states
    // below. Viewing another subdiagram must never change execution selection.
    bool disabledStructureEnabledVisible = true;
    // Empty preserves legacy Enabled/Disabled regions. Activation is independent
    // of the visible stable region ID; enabling never moves a region's contents.
    std::vector<DiagramDisabledRegion> disabledStructureRegions;
    std::string disabledStructureVisibleRegionId;
    std::optional<std::uint32_t> disabledStructureBackgroundRgb;
    int disabledStructureBackgroundOpacityPercent = 100;
    std::optional<std::uint32_t> disabledStructureSubframeRgb;
    int disabledStructureSubframeOpacityPercent = 100;
    std::optional<std::uint32_t> disabledStructureVeilRgb;
    int disabledStructureVeilOpacityPercent = 50;
    // Conditional Disable is resolved from compile-time symbols. The visible
    // index is authoring state only; the active case is always derived from a
    // CompilationContext and is therefore intentionally not persisted here.
    std::vector<DiagramConditionalDisableCase> conditionalDisableCases{
        DiagramConditionalDisableCase{
            std::string(kConditionalDisableDefaultRegionId), true, {} } };
    std::size_t conditionalDisableVisibleCaseIndex = 0U;
    std::optional<std::uint32_t> conditionalDisableBackgroundRgb;
    int conditionalDisableBackgroundOpacityPercent = 100;
    std::optional<std::uint32_t> conditionalDisableSubframeRgb;
    int conditionalDisableSubframeOpacityPercent = 100;
    std::optional<std::uint32_t> conditionalDisableVeilRgb;
    int conditionalDisableVeilOpacityPercent = 30;
    std::string callDocumentPath;
    std::vector<DiagramInPlacePair> inPlacePairs;
    std::string inPlaceSemanticProfile = "frog.studio.ipe@1";
    std::uint64_t inPlaceNextPairSerial = 0;
    std::vector<DiagramSequenceFrame> sequenceFrames{DiagramSequenceFrame{}};
    std::string sequenceSelectedFrameId = "frame_0";
    std::string sequenceSemanticProfile = "flat_sequence";
    std::string callDocumentTitle;
    std::string callIconSvgText;
    std::vector<DiagramCallPort> callPorts;
    // Domain identity survives Enum constant/widget conversions and save/load.
    std::string enumDomainId;
    // Optional authoring provenance: refers to the existing function catalogue,
    // never to an execution type. Terminals/presets can share one runtime type.
    std::string paletteCatalogId;
    bool numericColorBox = false;
    bool fontPointSizeUsesDefault = true;
    bool labelFontPointSizeUsesDefault = true;
    bool defaultTextGeometryInitialized = false;
    // Authoritative private authoring configuration; no live handles or UI caches.
    // Appended to preserve the existing positional aggregate initializers.
    std::optional<ApplicationControlNode> applicationControl;
    // Private shelf presentation, appended to retain all positional members.
    double eventStructureResponseNodeNormalizedY = 0.5;
    // Private explicit authoring state, independent of palettes and paint caches.
    std::optional<ConnectivityNodeConfiguration> connectivity;
    // Native signature is embedded per instance; no live library handle is persisted.
    std::optional<NativeCallConfiguration> nativeCall;
    HeapOptional<CollectionAuthoringState> collectionConstant;
    HeapOptional<CollectionBuildState> collectionBuild;
    // Private source configuration only. Runtime history belongs to its instance.
    std::uint32_t feedbackDelay = 1U;
    std::string feedbackInitialization = "compile_or_load";
    bool feedbackInitializationExplicit = false;
    std::string feedbackInitializerLoopId;
    bool feedbackHistory = false;
    bool feedbackZTransform = false;
    // Transient derived text metrics. Never serialized: each visual host
    // measures By Name field labels with its actual font before displaying.
    double clusterFunctionMeasuredWidth = 0.0;
    double clusterFunctionMeasuredRowPitch = 0.0;
    double connectivityMeasuredTextHeight = 0.0;
    HeapOptional<TypeDefinitionLink> typeDefinition;

    // Comment presentation only; never a dataflow edge or execution input.
    HeapOptional<DiagramCommentAttachment> commentAttachment;
    bool commentAutoSize = true;
    // Missing in legacy documents: resolve their known default text colors at
    // presentation time without rewriting the authored color or geometry.
    std::optional<bool> commentUseThemeTextColor;
    std::vector<CommentTextRun> commentTextRuns;

    // Semantic polymorphic selection, independent of the palette's one icon.
    // Parameter values remain ordinary typed sources and wires in the graph.
    std::string fittingVariant;
    std::string calculusVariant;
    std::string statisticsVariant;
    std::vector<std::string> statisticsOutputs;
    // Derived immutable connector projection; never serialized.
    std::shared_ptr<const StatisticsNodeProjection> statisticsProjection;
    std::string geometryVariant;
    std::string interpolationVariant;
    // Private presentation of the paired dynamic Event terminals. Missing keeps
    // the historical bottom offset; moving the pair never changes its schema.
    std::optional<double> eventDynamicTerminalsNormalizedY;

    // Studio label presentation only. Empty retains legacy geometry until its
    // horizontal anchor is inferred; this is never a dataflow/type property.
    std::string labelHorizontalAnchor;

    // Comment surface only. Missing preserves the borderless legacy surface;
    // textRgb/background_rgb retain their historical meanings.
    std::optional<std::uint32_t> commentBorderRgb;

    // Studio presentation only. Legacy documents retain visible ordinals;
    // this never selects, reorders or suppresses an executable frame.
    bool sequenceFrameNumbersVisible = true;

};

[[nodiscard]] inline std::string_view normalizeFeedbackNodeConfiguration(
    std::string_view configuration) noexcept {
    if (configuration == "inverse_activated" ||
        configuration == "direct" ||
        configuration == "direct_activated") {
        return configuration;
    }
    return "inverse";
}

[[nodiscard]] inline bool feedbackNodeConfigurationIsActivated(
    std::string_view configuration) noexcept {
    const auto normalized = normalizeFeedbackNodeConfiguration(configuration);
    return normalized == "inverse_activated" ||
        normalized == "direct_activated";
}

[[nodiscard]] inline bool feedbackNodeConfigurationIsDirect(
    std::string_view configuration) noexcept {
    const auto normalized = normalizeFeedbackNodeConfiguration(configuration);
    return normalized == "direct" || normalized == "direct_activated";
}

[[nodiscard]] inline double clampCaseStructureSelectorNormalizedY(
    double normalizedY) noexcept {
    // Corner clearance is a terminal-size constraint in the renderer. A
    // percentage margin would displace an anchored input during expansion.
    return normalizedY < 0.0 ? 0.0 : (normalizedY > 1.0 ? 1.0 : normalizedY);
}

[[nodiscard]] inline double clampCaseStructureTunnelNormalizedPosition(
    double normalizedPosition) noexcept {
    // Corner clearance is a physical tunnel-size constraint in the visual
    // geometry, not a percentage of the wall. A proportional 5% margin would
    // move an anchored tunnel again when a large structure is enlarged.
    return normalizedPosition < 0.0
        ? 0.0
        : (normalizedPosition > 1.0 ? 1.0 : normalizedPosition);
}

[[nodiscard]] inline double clampForLoopTerminalNormalizedPosition(
    double normalizedPosition) noexcept {
    return normalizedPosition < 0.0
        ? 0.0
        : (normalizedPosition > 1.0 ? 1.0 : normalizedPosition);
}

[[nodiscard]] inline double clampStructureSubdiagramLabelHeight(
    double height) noexcept {
    return height < kStructureSubdiagramLabelMinimumHeight
        ? kStructureSubdiagramLabelMinimumHeight
        : (height > kStructureSubdiagramLabelMaximumHeight
            ? kStructureSubdiagramLabelMaximumHeight
            : height);
}

[[nodiscard]] inline double clampWhileLoopTerminalNormalizedPosition(
    double normalizedPosition) noexcept {
    return normalizedPosition < 0.0
        ? 0.0
        : (normalizedPosition > 1.0 ? 1.0 : normalizedPosition);
}

[[nodiscard]] inline std::uint32_t clampTimedLoopVisibleRowCount(
    std::uint32_t count,
    std::uint32_t maximum) noexcept {
    return count < 1U ? 1U : (count > maximum ? maximum : count);
}

[[nodiscard]] inline double clampEventStructureDataNodeNormalizedY(
    double normalizedY) noexcept {
    return normalizedY < 0.0 ? 0.0 : (normalizedY > 1.0 ? 1.0 : normalizedY);
}

[[nodiscard]] inline std::string_view normalizeCaseStructureTunnelSide(
    std::string_view side) noexcept {
    if (side == "right" || side == "top" || side == "bottom") {
        return side;
    }
    return "left";
}

[[nodiscard]] std::string formatDiagramNumericValue(const DiagramNode &node);

struct DiagramEdge {
    std::string id;
    DiagramEdgeEndpoint from;
    DiagramEdgeEndpoint to;
    std::vector<Point> bends;
    std::string diagramOwnerStructureId;
    std::string diagramOwnerRegionId;
    // Optional authoring text painted directly over the wire. The normalized
    // route position keeps it attached when endpoints move or the wire reroutes.
    std::string label;
    double labelNormalizedPosition = 0.5;
    double labelOffsetX = 0.0;
    double labelOffsetY = 0.0;
};

struct DiagramWireFragment {
    std::string id;
    std::vector<Point> points;
    std::optional<DiagramEdgeEndpoint> source;
    std::optional<DiagramEdgeEndpoint> sink;
    std::string typeHint;
    std::string diagramOwnerStructureId;
    std::string diagramOwnerRegionId;
    bool typeHintAuthoritative = false;
    // Broken wires retain the same authored legend as their connected edge.
    std::string label;
    double labelNormalizedPosition = 0.5;
    double labelOffsetX = 0.0;
    double labelOffsetY = 0.0;
    // Private provenance only; never a live connection or an execution input.
    std::string detachedEdgeId;
};

struct DiagramWidgetCopySource {
    SceneObject object;
    DiagramNode node;
};

// Descriptive source information. The program-name policy is kept here because
// it is document identity, not a window-only override. None of the remaining
// documentation fields may change executable meaning.
struct FrogMetadataProperties {
    bool useDocumentName = true;
    std::string customProgramName;
    std::string description;
    std::string author;
    bool automaticVersioningEnabled = false;
    std::string programVersion = "0.0.0.0";
    std::vector<std::string> tags;
    std::string documentationUri;

    [[nodiscard]] bool operator==(const FrogMetadataProperties &) const noexcept = default;
};

// Host-facing launch and front-panel window policy. This is intentionally kept
// separate from both language semantics and IDE-only authoring preferences.
struct FrogHostProperties {
    int version = 1;
    std::string launchTrigger = "manual";
    std::string windowStyle = "standard";
    std::string initialPosition = "centered";
    int minimumWidth = 0;
    int minimumHeight = 0;
    bool allowClose = true;
    bool allowResize = true;
    bool allowMinimize = true;
    bool menuBarVisible = true;
    bool toolbarVisible = true;
    bool verticalScrollbarVisible = true;
    bool horizontalScrollbarVisible = true;
    bool scrollToOrigin = false;

    [[nodiscard]] bool operator==(const FrogHostProperties &) const noexcept = default;
};

// Versioned execution policy. Values are persisted as portable author intent.
// An execution backend may reject a policy it cannot honor, but must never
// silently reinterpret it.
struct FrogExecutionPolicy {
    int version = 2;
    bool debuggingAllowed = true;
    std::string priority = "normal";
    std::string reentrancy = "non_reentrant";
    std::string executionSystem = "caller";
    bool automaticErrorHandling = true;
    bool suspendWhenCalled = false;
    bool clearIndicatorsWhenCalled = false;

    [[nodiscard]] bool operator==(const FrogExecutionPolicy &) const noexcept = default;
};

enum class FrogArrayStorageMode : std::uint8_t { Readable, Automatic, CompactStandalone };

struct FrogSourceProperties {
    // Private Studio draft persistence only; never a runtime type or value.
    FrogArrayStorageMode arrayStorageMode = FrogArrayStorageMode::Readable;
    // Authoring typography inherited by newly created objects and literals.
    // Absent defaults identify legacy files and retain the host preference;
    // reading them must not invent an authored size or change global state.
    std::optional<double> defaultTextPointSize;
    std::optional<std::string> defaultTextFontFamily;
    FrogMetadataProperties metadata;
    FrogHostProperties host;
    FrogExecutionPolicy executionPolicy;
    // Source-authored custom symbols. Backend-owned built-ins are deliberately
    // absent: a saved document must not impersonate a different build target.
    std::map<std::string, std::string> compilationProjectSymbols;
    std::map<std::string, std::string> compilationTargetSymbols;
    // Optional private-draft native intent; absence does not rewrite legacy files.
    std::optional<NativeProgramOptions> nativeOptions;
    // File provenance is independent of the user's program_version. Recovery
    // is immutable and participates in existing history property snapshots.
    FrogFileVersionMetadata fileVersions;

    [[nodiscard]] bool operator==(const FrogSourceProperties &) const noexcept = default;
};

// Editor presentation only: independent of the authored canvas and runtime host.
// Bounds are the outer, restored window rectangle in screen pixels, even when
// the window was maximized (or minimized) at save time.
struct FrogEditorWindowPlacement {
    Rect bounds;
    bool maximized = false;
};

struct FrogEditorWindows {
    std::optional<FrogEditorWindowPlacement> frontPanel;
    std::optional<FrogEditorWindowPlacement> diagram;
    std::optional<FrogEditorWindowPlacement> frogSource;
};

[[nodiscard]] bool validFrogEditorWindowPlacement(
    const FrogEditorWindowPlacement &placement) noexcept;

// An identified private read adapter receipt, never semantic validation or FIR.
// Source ranges refer to the original bytes; no source file is rewritten by Read.
struct FrogPrivateTerminalHintMigration {
    std::string code;
    std::string nodeId;
    std::string widgetId;
    std::string replacementType;
    std::size_t sourceBegin = std::string_view::npos;
    std::size_t sourceEnd = std::string_view::npos;
};


template<class Properties>
struct FrogDocumentStateData {
    FrogEditorWindows editorWindows;
    std::vector<DiagramNode> diagramNodes;
    std::vector<DiagramEdge> diagramEdges;
    std::vector<DiagramWireFragment> diagramWireFragments;
    std::vector<FrogPrivateTerminalHintMigration> privateTerminalHintMigrations;
    int diagramZoomPercent = 100;
    Point diagramViewportOrigin;
    std::string title;
    Properties properties;
    std::string fileIconSvgText;
    // Absent only for legacy drafts without declarations. This authority is
    // independent of the canvas; it is not a canonical public source or FIR.
    std::optional<FrogDraftInterface> declaredInterface;
    std::string interfaceBindingMapProfileId;
    std::string interfaceMapLayoutPatternId;
    std::vector<std::string> interfaceMapVisualTransforms;
    int nextWidgetSerial = 1;
    int nextDiagramNodeSerial = 1;
    int nextDiagramEdgeSerial = 1;
    int nextDiagramWireFragmentSerial = 1;
    // A legacy import may expose an authoring projection of public source.
    // Retain its independent authority: this projection is never private FIR
    // or a substitute program for execution. Null denotes a private draft.
    std::shared_ptr<const FrogSourceDocument> publicSourceAuthority;
    // Historical private source without an authored Diagram node array.
    // Exact opaque bytes (including private legacy byte strings) survive Read.
    // This is not an executable projection, a public source, or migration consent.
    // Only an explicit, separately validated migration may replace this authority.
    std::shared_ptr<const std::string> privateSourceAwaitingDiagramMigration;

};

using FrogDocumentState = FrogDocumentStateData<FrogSourceProperties>;
// Transfer only source-owned state. A private destination's recovery sidecar
// is neither copied from the frontend nor destroyed by a semantic projection.
template<class Destination,class Source>
void moveFrogDocumentState(Destination &to,Source &&from) {
    to.editorWindows=std::move(from.editorWindows);
    to.diagramNodes=std::move(from.diagramNodes);
    to.diagramEdges=std::move(from.diagramEdges);
    to.diagramWireFragments=std::move(from.diagramWireFragments);
    to.privateTerminalHintMigrations=std::move(from.privateTerminalHintMigrations);
    to.diagramZoomPercent=std::move(from.diagramZoomPercent);
    to.diagramViewportOrigin=std::move(from.diagramViewportOrigin);
    to.title=std::move(from.title);
    to.fileIconSvgText=std::move(from.fileIconSvgText);
    to.declaredInterface=std::move(from.declaredInterface);
    to.interfaceBindingMapProfileId=std::move(from.interfaceBindingMapProfileId);
    to.interfaceMapLayoutPatternId=std::move(from.interfaceMapLayoutPatternId);
    to.interfaceMapVisualTransforms=std::move(from.interfaceMapVisualTransforms);
    to.nextWidgetSerial=std::move(from.nextWidgetSerial);
    to.nextDiagramNodeSerial=std::move(from.nextDiagramNodeSerial);
    to.nextDiagramEdgeSerial=std::move(from.nextDiagramEdgeSerial);
    to.nextDiagramWireFragmentSerial=std::move(from.nextDiagramWireFragmentSerial);
    to.publicSourceAuthority=std::move(from.publicSourceAuthority);
    to.privateSourceAwaitingDiagramMigration=std::move(from.privateSourceAwaitingDiagramMigration);
    static_cast<FrogSourceProperties&>(to.properties)=std::move(static_cast<FrogSourceProperties&>(from.properties));
}

template<class Destination,class Source>
void copyFrogDocumentState(Destination &to,const Source &from) {
    to.editorWindows=(from.editorWindows);
    to.diagramNodes=(from.diagramNodes);
    to.diagramEdges=(from.diagramEdges);
    to.diagramWireFragments=(from.diagramWireFragments);
    to.privateTerminalHintMigrations=(from.privateTerminalHintMigrations);
    to.diagramZoomPercent=(from.diagramZoomPercent);
    to.diagramViewportOrigin=(from.diagramViewportOrigin);
    to.title=(from.title);
    to.fileIconSvgText=(from.fileIconSvgText);
    to.declaredInterface=(from.declaredInterface);
    to.interfaceBindingMapProfileId=(from.interfaceBindingMapProfileId);
    to.interfaceMapLayoutPatternId=(from.interfaceMapLayoutPatternId);
    to.interfaceMapVisualTransforms=(from.interfaceMapVisualTransforms);
    to.nextWidgetSerial=(from.nextWidgetSerial);
    to.nextDiagramNodeSerial=(from.nextDiagramNodeSerial);
    to.nextDiagramEdgeSerial=(from.nextDiagramEdgeSerial);
    to.nextDiagramWireFragmentSerial=(from.nextDiagramWireFragmentSerial);
    to.publicSourceAuthority=(from.publicSourceAuthority);
    to.privateSourceAwaitingDiagramMigration=(from.privateSourceAwaitingDiagramMigration);
    static_cast<FrogSourceProperties&>(to.properties)=static_cast<const FrogSourceProperties&>(from.properties);
}

struct FrogDraftDataView {
    const std::vector<DiagramNode> &diagramNodes;
    const std::vector<DiagramEdge> &diagramEdges;
    FrogWidgetDataView canvas;
};
struct FrogDraftData : FrogDocumentState {
    FrogWidgetData widgets;
    [[nodiscard]] operator FrogDraftDataView() const noexcept {return {diagramNodes,diagramEdges,widgets};}
};

struct FrogDraftInterfaceMapSettings {
    bool mapPresent = false;
    std::string profileId;
    std::string patternId;
    std::vector<std::string> transformTokens;
};
[[nodiscard]] std::optional<FrogDraftInterfaceMapSettings> readFrogDraftInterfaceMapSettings(
    const FrogDraftInterface &declaration, std::string *error = nullptr);

// Optional structured attribution for failures already diagnosed by the
// document reader. The legacy error string remains unchanged. Unclassified
// means no typed attribution is available; it is not a success/failure flag.
enum class FrogDocumentReadErrorKind { Unclassified, InvalidPresentation };
struct FrogDocumentReadDiagnostic {
    FrogDocumentReadErrorKind kind = FrogDocumentReadErrorKind::Unclassified;
    std::string path;
    std::string message;
};

[[nodiscard]] std::optional<FrogDraftData> readFrogDraftData(
    std::string_view source, std::string *error = nullptr,
    FrogDocumentReadDiagnostic *diagnostic = nullptr);

[[nodiscard]] std::string diagramTerminalValueType(const WidgetInstance &widget);
[[nodiscard]] std::string serializeArrayElementWidget(const WidgetInstance &widget);
// Cell values refer to the real shared prototype. They never duplicate its
// presentation or its descendant prototypes in every stored Array cell.
[[nodiscard]] std::string serializeArrayElementValueWidget(
    const WidgetInstance &value,const WidgetInstance &prototype);
// Optional observation is true only after successful decoding of a compact
// value tree with no historical full-widget descendants. It does not change
// admission, migration, or values; failures and legacy payloads report false.
[[nodiscard]] std::optional<WidgetInstance> deserializeArrayElementWidget(
    std::string_view source,const WidgetInstance *prototype=nullptr,bool migrateSurvivingFields=false,
    bool *compactValueOnly=nullptr);
void applyArrayElementWidgetValue(WidgetInstance &target, const WidgetInstance &value);

using FrogDocumentCompressFn = bool (*)(std::span<const std::uint8_t>,std::vector<std::uint8_t>&);
[[nodiscard]] std::string serializeFrogDraft(
    FrogWidgetDataView canvas,
    std::string_view documentTitle,
    std::string_view interfaceMapLayoutPatternId = {},
    const std::vector<std::string> &interfaceMapVisualTransforms = {},
    std::string_view fileIconSvgText = {},
    const std::vector<DiagramNode> &diagramNodes = {},
    int diagramZoomPercent = 100,
    Point diagramViewportOrigin = {},
    const std::vector<DiagramEdge> &diagramEdges = {},
    const std::vector<DiagramWireFragment> &diagramWireFragments = {},
    std::string_view interfaceBindingMapProfileId = {},
    const FrogSourceProperties &properties = {},
    const FrogEditorWindows &editorWindows = {},
    bool preserveDiagramTerminalTypes = false,
    const FrogDraftInterface *declaredInterface = nullptr,
    FrogDocumentCompressFn compress = nullptr);
[[nodiscard]] std::string serializeFrogDraftData(const FrogDraftData &);
[[nodiscard]] std::string normalizeFrogProgramVersion(std::string_view);
[[nodiscard]] std::string resolvedFrogProgramName(std::string_view,const FrogMetadataProperties&);

} // namespace frog::native_engine
