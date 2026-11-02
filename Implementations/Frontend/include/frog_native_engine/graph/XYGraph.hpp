#pragma once
#include "frog_native_engine/BindingType.hpp"
#include "frog_native_engine/graph/XYGraphReferenceContract.hpp"
#include "frog_native_engine/FrogJson.hpp"
#include "frog_native_engine/graph/WaveformGraph.hpp"
#include <variant>

namespace frog::native_engine {struct PictureDisplayState;}
namespace frog::native_engine::xy {
inline constexpr Size minimumSize{280,190}, defaultSize{640,400};
inline constexpr std::size_t maximumSeries=256, maximumSamples=1'000'000;
enum class Form {Invalid,D1,D2,D3,D4,D5,D6};
struct Classification {Form form=Form::Invalid; std::string diagnostic; [[nodiscard]] bool accepted()const noexcept{return form!=Form::Invalid;}};
[[nodiscard]] Classification classify(const binding::TypeRef &);
[[nodiscard]] std::string validateType(const binding::TypeRef &);
[[nodiscard]] binding::TypeRef defaultType();
[[nodiscard]] source::JsonValue emptyValue(const binding::TypeRef &);

// No allocation per scalar. Integers remain exact. Subtraction is performed in
// the integer domain BEFORE conversion to screen precision, including MSVC
// where long double has only binary64 precision. residual is for inverse-view
// coordinates; it is never written into a program's typed samples.
struct Coordinate {
    std::variant<double,std::int64_t,std::uint64_t> value=0.0;
    double residual=0;
    Coordinate()=default;
    Coordinate(double v):value(v){}
    Coordinate(std::int64_t v):value(v){}
    Coordinate(std::uint64_t v):value(v){}
    [[nodiscard]] double approximate()const noexcept;
    [[nodiscard]] bool finite()const noexcept;
    [[nodiscard]] bool positive()const noexcept;
    [[nodiscard]] std::string text()const;
    bool operator==(const Coordinate &)const=default;
};
[[nodiscard]] double difference(const Coordinate &,const Coordinate &)noexcept;
[[nodiscard]] Coordinate shifted(Coordinate,double)noexcept;
struct Sample {Coordinate x,y; std::size_t sourceIndex=0; bool hasX=true,hasY=true;};
struct Series {std::string id,name; std::vector<Sample> samples; std::size_t xCount=0,yCount=0;};
struct DataRangeCache;
struct Dataset {binding::TypeRef datatype; source::JsonValue exactSource; std::vector<Series> series; std::vector<std::string> diagnostics; mutable std::shared_ptr<const DataRangeCache> rangeCache;};
[[nodiscard]] std::shared_ptr<const Dataset> decodeTypedValue(const binding::TypeRef &,const source::JsonValue &,std::string *error=nullptr);

enum class Mapping {Linear,Logarithmic};
enum class Autoscale {Manual,Once,Continuous};
enum class Side {Bottom,Top,Left,Right};
enum class NumberFormat {Automatic,Decimal,Scientific,Engineering,Binary,Octal,Hexadecimal,Duration,DateTime};
enum class Symbol {None,Circle,Disc,HollowDisc,CircleDot,Square,FilledSquare,HollowFilledSquare,SquareDot,SmallSquare,SmallDot,Plus,SmallPlus,Cross,SmallCross,Diamond,SmallHollowSquare};
enum class LineStyle {Solid,LongDash,ShortDash,DashDot,DashDotDot};
enum class Interpolation {None,Staircase,Linear,Horizontal,HorizontalCentered,VerticalCentered};
enum class Presentation {Line,Points,LinePoints,Area,Comb,Bars};
enum class Fill {None,Zero,NegativeEdge,PositiveEdge,Plot};
enum class Orientation {Vertical,Horizontal};
enum class AnchorMode {Free,Plot,AnyPlot};
enum class PlaneKind {None,Nyquist,Nichols,S,Z};
enum class Tool {Cursor,ZoomRectangle,ZoomX,ZoomY,Fit,ZoomIn,ZoomOut,Pan};
struct AxisRange {Coordinate origin; graph::Range offsets{0,1}; double scale=1; bool operator==(const AxisRange &)const=default;};
struct Marker {Coordinate value; std::string label; bool operator==(const Marker &)const=default;};
struct Axis {
    std::string id,name,unit;
    bool horizontal=true,visible=true,titleVisible=true,inverted=false;
    Side side=Side::Bottom;
    Mapping mapping=Mapping::Linear;
    Autoscale autoscale=Autoscale::Continuous;
    AxisRange range;
    NumberFormat format=NumberFormat::Automatic;
    int precision=6;
    bool significantDigits=true,trailingZeros=false,looseFit=false;
    bool majorTicks=true,minorTicks=false,majorGrid=true,minorGrid=false;
    std::uint32_t rgb=0xDADEE3,gridRgb=0x444444;
    bool customRgb=false,customGridRgb=false;
    std::vector<Marker> markers;
    std::string timeOrigin="FROG epoch",timeUnit="s",timeZone="UTC";
    bool operator==(const Axis &)const=default;
};
struct Plot {
    std::string id,name,xAxis="x",yAxis="y";
    bool visible=true,antialias=true;
    std::uint32_t rgb=0x59B5F8,pointRgb=0x59B5F8,fillRgb=0x59B5F8;
    double width=1.5,symbolSize=5,opacity=1,fillOpacity=.25;
    LineStyle lineStyle=LineStyle::Solid;
    Symbol symbol=Symbol::None;
    Interpolation interpolation=Interpolation::Linear;
    Presentation presentation=Presentation::Line;
    Fill fill=Fill::None;
    std::string fillPlot;
    Orientation orientation=Orientation::Vertical;
    Coordinate reference;
    // Bar width and position are local plot pixels, independent of X spacing.
    double barWidth=8,barOffset=0;
    bool operator==(const Plot &)const=default;
};
struct Anchor {
    AnchorMode mode=AnchorMode::Free;
    std::string plotId,xAxis="x",yAxis="y";
    std::size_t sourceIndex=0;
    Coordinate x,y;
    bool operator==(const Anchor &)const=default;
};
struct Cursor {std::string id,name; Anchor anchor; bool visible=true,nameVisible=true,movable=true,edgeScroll=true; std::uint32_t rgb=0xFFE07A; LineStyle lineStyle=LineStyle::ShortDash; bool operator==(const Cursor &)const=default;};
struct Annotation {std::string id,text; Anchor anchor; Point textOffset{12,-12}; bool fixedText=false,visible=true,movable=true,arrow=true; std::uint32_t rgb=0xFFE07A; double pointSize=9; bool operator==(const Annotation &o)const{return id==o.id&&text==o.text&&anchor==o.anchor&&textOffset.x==o.textOffset.x&&textOffset.y==o.textOffset.y&&fixedText==o.fixedText&&visible==o.visible&&movable==o.movable&&arrow==o.arrow&&rgb==o.rgb&&pointSize==o.pointSize;}};
struct Plane {PlaneKind kind=PlaneKind::None; std::string xAxis="x",yAxis="y"; bool lines=true,labels=true,cartesian=true; bool operator==(const Plane &)const=default;};
enum class DecorationLayer {Background,BetweenGridAndPlots,Foreground};
inline constexpr std::size_t maximumDecorations=16,maximumDecorationPixels=1'000'000;
struct Decoration {
    std::string id;
    DecorationLayer layer=DecorationLayer::Background;
    bool visible=true,dataCoordinates=false;
    std::string xAxis="x",yAxis="y";
    // Local logical plot pixels, independently of the image's native size.
    Rect pixelRect{0,0,100,100};
    // Image top-left and bottom-right anchors in the explicit scale pair.
    Coordinate firstX,firstY,secondX=Coordinate(1.0),secondY=Coordinate(1.0);
    std::shared_ptr<const PictureDisplayState> image;
    std::string disabledReason;
    [[nodiscard]] bool operator==(const Decoration &)const;
};
struct Design {
    std::vector<Axis> axes;
    std::vector<Plot> plots;
    std::vector<Cursor> cursors;
    std::vector<Annotation> annotations;
    std::vector<Decoration> decorations;
    Plane plane;
    bool legendVisible=true,scaleLegendVisible=false,paletteVisible=true,cursorLegendVisible=false,xScrollbarVisible=false;
    std::size_t legendRows=4;
    bool legendAutoWidth=true,equalAspect=false;
    std::string aspectXAxis="x",aspectYAxis="y";
    [[nodiscard]] bool operator==(const Design &)const=default;
};
struct AxisView {std::string axisId; AxisRange range; bool continuous=true; bool explicitPolicy=false; bool operator==(const AxisView &)const=default;};
struct Navigation {std::vector<AxisView> axes; std::vector<std::vector<AxisView>> previous; Tool tool=Tool::Cursor; std::size_t legendOffset=0; std::optional<Size> aspectViewport;};
struct State {
    binding::TypeRef datatype=defaultType();
    bool datatypeExplicit=false;
    std::shared_ptr<const source::JsonValue> initialValue;
    Design design;
    std::shared_ptr<const Dataset> current;
    // Cache of authored initialValue; copies share immutable storage only.
    std::shared_ptr<const Dataset> initialSnapshot;
    Navigation navigation;
    std::uint64_t dataRevision=0;
    std::string diagnostic;
    std::optional<std::string> preservedSource;
    State();
};
[[nodiscard]] std::string validate(const Design &);
[[nodiscard]] std::string validateDecorations(const Design &);
[[nodiscard]] std::string validateState(const State &);
[[nodiscard]] bool replaceData(State &,const binding::TypeRef &,const source::JsonValue &,std::string *error=nullptr);
[[nodiscard]] bool setInitialData(State &,const binding::TypeRef &,const source::JsonValue &,std::string *error=nullptr);
void clearData(State &);
[[nodiscard]] std::shared_ptr<const Dataset> displayedData(const State &);
[[nodiscard]] const source::JsonValue &initialValueSource(const State &)noexcept;
[[nodiscard]] Plot plotForSeries(const Design &,const Series &,std::size_t ordinal);
[[nodiscard]] const Axis *findAxis(const Design &,std::string_view id)noexcept;
[[nodiscard]] Axis *findAxis(Design &,std::string_view id)noexcept;
[[nodiscard]] bool validRange(const Axis &,const AxisRange &)noexcept;
[[nodiscard]] AxisRange axisRange(const State &,std::string_view axisId);
[[nodiscard]] bool setAxisRange(State &,std::string_view,const AxisRange &,std::string *error=nullptr);
[[nodiscard]] bool fit(State &,std::string_view axisId={},bool continuous=false,std::string *error=nullptr);
[[nodiscard]] bool removeAxis(Design &,std::string_view,std::string *error=nullptr);
[[nodiscard]] std::string nextId(const Design &,std::string_view prefix);
[[nodiscard]] bool saveView(State &,std::string *error=nullptr);
[[nodiscard]] bool previousView(State &);

struct Transform {
    Rect plot; Axis xAxis,yAxis; AxisRange x,y;
    [[nodiscard]] std::optional<Point> toPlot(const Coordinate &,const Coordinate &)const noexcept;
    [[nodiscard]] std::optional<std::pair<Coordinate,Coordinate>> toData(Point)const noexcept;
    [[nodiscard]] bool representable(const Sample &)const noexcept;
};
[[nodiscard]] std::optional<Transform> transform(const State &,std::string_view xAxis,std::string_view yAxis,Rect);
struct DecorationGeometry {Rect bounds,clippedBounds,sourceFraction; bool flipX=false,flipY=false; std::string diagnostic; [[nodiscard]] bool available()const noexcept{return diagnostic.empty()&&clippedBounds.width>0&&clippedBounds.height>0;}};
[[nodiscard]] DecorationGeometry decorationGeometry(const State &,const Decoration &,Rect plot,double localScale=1);
void reassociateDecorations(Design &,std::string_view oldAxis,std::string_view replacement);
[[nodiscard]] graph::Layout layout(Size,bool legendVisible)noexcept;
struct Tick {Coordinate value; double fraction=0; std::string label; bool major=true;};
[[nodiscard]] std::vector<Tick> ticks(const Axis &,const AxisRange &,std::size_t desiredCount=6);
[[nodiscard]] std::string validateAxisTextFormat(const Axis &);
struct Vertex {Point point; std::size_t sample=0; bool original=true;};
using Run=std::vector<Vertex>;
struct Geometry {std::vector<Run> runs; std::vector<Vertex> symbols; std::vector<std::vector<Point>> fills; std::string diagnostic;};
[[nodiscard]] Geometry geometry(const Series &,const Plot &,const Transform &,const Series *fillSeries=nullptr,const Plot *fillPlot=nullptr,const Transform *fillTransform=nullptr);
[[nodiscard]] std::optional<std::pair<Point,Point>> clipSegment(Point,Point,Rect)noexcept;
[[nodiscard]] bool navigate(State &,Rect,double xFactor,double yFactor,Point anchor,std::optional<std::string_view> axis={},std::string *error=nullptr);
[[nodiscard]] bool pan(State &,Rect,Point pixelDelta,std::optional<std::string_view> axis={},std::string *error=nullptr);
[[nodiscard]] bool zoomRectangle(State &,Rect,Rect selection,bool x=true,bool y=true,std::string *error=nullptr);
[[nodiscard]] bool enforceAspect(State &,Rect,std::string *error=nullptr);
struct ScrollDomain {double minimum=0,maximum=1,start=0,extent=1; bool active=false;};
[[nodiscard]] ScrollDomain scrollDomain(const State &,std::string_view xAxis);
[[nodiscard]] bool scrollTo(State &,std::string_view xAxis,double fraction,std::string *error=nullptr);
struct Measurement {bool available=false; std::string diagnostic,plotId; std::size_t sourceIndex=0; Coordinate x,y; std::optional<Point> point;};
[[nodiscard]] Measurement measure(const State &,const Anchor &,Rect);
[[nodiscard]] std::optional<Anchor> nearestPoint(const State &,Point,Rect,double radius=12,const Anchor *preferred=nullptr,std::string_view plotId={});
[[nodiscard]] bool moveAttached(const State &,Anchor &,int direction,Rect);
[[nodiscard]] std::optional<std::pair<double,double>> cursorDelta(const State &,const Anchor &,const Anchor &,Rect,std::string *error=nullptr);
enum class ExportScope {Visible,All};
[[nodiscard]] std::string exportCsv(const State &,ExportScope=ExportScope::Visible,std::optional<std::string_view> plotId={},char separator=',');
struct PlaneGeometry {std::vector<std::vector<Point>> lines; std::vector<std::pair<Point,std::string>> labels; std::string diagnostic;};
[[nodiscard]] PlaneGeometry planeGeometry(const Plane &,const Transform &);
// Lossless JSON transport shared with the source/codec and runtime adapter.
[[nodiscard]] std::string exactJson(const source::JsonValue &);
}
