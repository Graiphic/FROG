#pragma once

#include "frog_native_engine/Geometry.hpp"
#include "frog_native_engine/BindingType.hpp"
#include "frog_native_engine/FrogJson.hpp"
#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace frog::native_engine::graph {

inline constexpr char classId[] = "frog.widgets.waveform_graph";
inline constexpr char valueType[] = "frog.waveform_graph_value";
inline constexpr Size minimumSize{240.0, 160.0};
inline constexpr Size defaultSize{640.0, 400.0};
inline constexpr std::size_t maximumSeries = 64;
inline constexpr std::size_t maximumSamples = 1'000'000;

struct Series {
    std::string id;
    std::string name;
    std::vector<double> y;
    double origin = 0.0;
    double step = 1.0;
};
struct Dataset {
    std::vector<Series> series;
    // The renderer projects to double; the complete received value and schema
    // remain authoritative for transport, defaults and wide integer samples.
    std::optional<binding::TypeRef> datatype;
    std::shared_ptr<const source::JsonValue> exactSource;
};
struct Range {
    double minimum = 0.0;
    double maximum = 100.0;
    bool operator==(const Range &) const = default;
};
struct Plot {
    std::string id;
    std::string name;
    std::uint32_t rgb = 0x59B5F8;
    bool visible = true;
    bool lines = true;
    bool points = false;
    double width = 1.5;
    bool operator==(const Plot &) const = default;
};
// Authored title layout, independent of live samples and of the widget label.
// Positions are fractions of the graph body; sizes are unzoomed canvas units.
struct TitleStyle {
    bool positioned = false;
    double x = 0, y = 0, width = 120, height = 24;
    double pointSize = 9;
    std::string family = "Segoe UI";
    std::string style = "plain";
    std::uint32_t rgb = 0xDADEE3;
    std::string alignment = "default";
    bool pointSizeUsesDefault = true;
    bool operator==(const TitleStyle &) const = default;
};
enum class CursorMode {Free, SinglePlot};
// Symbol and dash indices use the existing XY graph rendering vocabulary.
struct Cursor {
    std::string id, name, plotId;
    double x=0, y=0;
    std::size_t sourceIndex=0;
    CursorMode mode=CursorMode::Free;
    bool visible=true, nameVisible=true, movable=true;
    std::uint32_t rgb=0xFFE07A;
    int lineStyle=0, lineWidth=1, pointStyle=1, crossStyle=8;
    bool operator==(const Cursor &) const = default;
};
struct Annotation {
    std::string text;
    double x = 0, y = 0;
    bool operator==(const Annotation &) const = default;
};
struct Design {
    std::string xLabel = "Time";
    std::string yLabel = "Amplitude";
    TitleStyle xTitle, yTitle;
    bool autoscaleX = true;
    bool autoscaleY = true;
    bool gridVisible = true;
    bool legendVisible = true;
    Range xRange;
    Range yRange{-10.0, 10.0};
    std::vector<Plot> plots;
    std::vector<Annotation> annotations; // legacy input only; never authored by the UI
    std::vector<Cursor> cursors;
    bool operator==(const Design &) const = default;
};

// Only design and initialData are source. Runtime snapshots and diagnostics are
// session state; ordinary Save must never promote them to authored defaults.
struct State {
    Design design;
    Dataset initialData;
    std::shared_ptr<const Dataset> current;
    std::uint64_t dataRevision = 0;
    std::string diagnostic;
    // Uninterpreted imported profile/configuration: preserve rather than silently
    // replacing a newer or invalid source with our defaults on Save.
    std::optional<std::string> preservedSource;
    binding::TypeRef datatype = binding::TypeRef::array(binding::TypeRef::primitive("f64"));
    bool datatypeExplicit = false;
    std::shared_ptr<const source::JsonValue> typedInitialData;
    [[nodiscard]] const Dataset &displayedData() const noexcept {
        return current ? *current : initialData;
    }
};

[[nodiscard]] std::string validate(const Dataset &data);
[[nodiscard]] std::string validateType(const binding::TypeRef &type);
[[nodiscard]] std::shared_ptr<const Dataset> decodeTypedValue(const binding::TypeRef &type,
    const source::JsonValue &value, std::string *error = nullptr);
[[nodiscard]] source::JsonValue emptyValue(const binding::TypeRef &type);
// Faithful bridge for the existing sample editor and WFG-1 initial_data.
[[nodiscard]] binding::TypeRef legacyDataType(const Dataset &data);
// A legacy source is reconstructed only if its declared shape/timing preserve
// all samples. Unrepresentable legacy data raises an explicit diagnostic.
[[nodiscard]] source::JsonValue initialValueSource(const State &state);
[[nodiscard]] bool setTypedInitialData(State &state, const binding::TypeRef &type,
    const source::JsonValue &value, std::string *error = nullptr);
[[nodiscard]] bool replaceTypedData(State &state, const binding::TypeRef &type,
    const source::JsonValue &value, std::string *error = nullptr);
[[nodiscard]] std::string validate(const Design &design);
[[nodiscard]] std::optional<Point> cursorPosition(const State &, const Cursor &);
[[nodiscard]] bool positionCursor(const State &, Cursor &, Point);
[[nodiscard]] std::string nextCursorId(const Design &);
[[nodiscard]] bool replaceData(State &state, const Dataset &data);
void clearData(State &state);
[[nodiscard]] bool setInitialData(State &state, const Dataset &data, std::string *error = nullptr);
[[nodiscard]] Dataset fromY(std::span<const double> y, double origin = 0.0, double step = 1.0);
[[nodiscard]] bool fromMatrix(std::span<const double> cells, std::size_t rows,
    std::size_t columns, Dataset &result, std::string *error = nullptr);
[[nodiscard]] Plot plotForSeries(const Design &design, const Series &series, std::size_t ordinal);
[[nodiscard]] Range dataRange(const State &state, bool xAxis);
[[nodiscard]] bool validRange(Range range) noexcept;

struct DataRanges {
    Range x;
    Range y;
    bool operator==(const DataRanges &) const = default;
};
// Both automatic axes share one pass over finite samples.
[[nodiscard]] DataRanges dataRanges(const State &state);

enum class AxisTextPart { XLabel, YLabel, XMinimum, XMaximum, YMinimum, YMaximum };
// The editor starts from the exact displayed bounds, not rounded tick labels or
// stored fallback ranges. Formatting is independent of the process locale.
[[nodiscard]] std::string axisTextEditValue(const Design &design,
    const DataRanges &displayedRanges, AxisTextPart part);
// Decimal dot or comma, optional sign/exponent and surrounding ASCII whitespace;
// no grouping or non-finite values. A rejected input leaves value unchanged.
[[nodiscard]] bool parseAxisNumber(std::string_view text, double &value);
// Only the selected bound and its axis autoscale policy change atomically.
// A numerical no-op preserves even an active autoscale policy and fallback range.
[[nodiscard]] bool applyAxisTextEdit(Design &design, const DataRanges &displayedRanges,
    AxisTextPart part, std::string_view text, std::string *error = nullptr);

// One entry, no retained samples. Only immutable runtime snapshots can be reused;
// authored initialData is publicly mutable and must always be inspected again.
class RangeCache {
public:
    [[nodiscard]] DataRanges ranges(const State &state);
    [[nodiscard]] std::uint64_t rebuildCount() const noexcept { return rebuildCount_; }
private:
    std::weak_ptr<const Dataset> snapshot_;
    std::uint64_t revision_ = 0;
    std::uint64_t rebuildCount_ = 0;
    Design design_;
    DataRanges ranges_;
};

struct Layout { Rect plot; Rect legend; };
[[nodiscard]] Layout layout(Size size, bool legendVisible) noexcept;
struct Transform {
    Rect plot;
    Range x;
    Range y;
    [[nodiscard]] std::optional<Point> toPlot(double xValue, double yValue) const noexcept;
    [[nodiscard]] std::optional<Point> toData(Point point) const noexcept;
};
struct Tick { double value; double fraction; std::string label; };
[[nodiscard]] std::vector<Tick> ticks(Range range, std::size_t desiredCount);
struct Vertex { Point point; std::size_t sample; };
using Run = std::vector<Vertex>;
// Pixel-column reduction retains first/min/max/last in ORIGINAL index order,
// with a separate run at every non-finite sample. The dataset is never changed.
[[nodiscard]] std::vector<Run> geometry(const Series &series, const Transform &transform);

// Authoring input only: one numeric Y series per nonempty line. Not a runtime
// coercion and not the persistence format. NaN/Inf represent discontinuities.
[[nodiscard]] bool parseSamples(std::string_view text, Dataset &result, std::string *error = nullptr);
[[nodiscard]] std::string sampleText(const Dataset &data);

} // namespace frog::native_engine::graph
