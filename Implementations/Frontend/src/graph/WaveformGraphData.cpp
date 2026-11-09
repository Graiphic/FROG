// SPDX-License-Identifier: Apache-2.0
// Extracted from the existing Studio data owner; no editor or runtime.
#include "frog_native_engine/graph/WaveformGraph.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <exception>
#include <type_traits>
#include <utility>
#include <unordered_set>
namespace frog::native_engine::graph {

bool validRange(Range range) noexcept {
    return std::isfinite(range.minimum) && std::isfinite(range.maximum) &&
        range.minimum < range.maximum && std::isfinite(range.maximum - range.minimum);
}

std::string validate(const Dataset &data) {
    if (data.series.size() > maximumSeries) return "WFG_SERIES_LIMIT: at most 64 series.";
    std::size_t count = 0;
    double xLow=std::numeric_limits<double>::infinity(), xHigh=-xLow, yLow=xLow, yHigh=-xLow;
    std::unordered_set<std::string> ids;
    for (const auto &series : data.series) {
        if (series.id.empty() || series.id.size() > 256 || !ids.insert(series.id).second)
            return "WFG_SERIES_ID: series IDs must be nonempty and unique (256 characters maximum).";
        if (series.name.size() > 1024) return "WFG_NAME_LIMIT: series name is too long.";
        if (series.y.size() > maximumSamples - count) return "WFG_SAMPLE_LIMIT: at most 1 000 000 samples per graph.";
        count += series.y.size();
        if (!std::isfinite(series.origin) || !std::isfinite(series.step) || series.step <= 0.0)
            return "WFG_X_DOMAIN: origin must be finite and step must be finite and positive.";
        if (!series.y.empty() && !std::isfinite(std::fma(static_cast<double>(series.y.size() - 1), series.step, series.origin)))
            return "WFG_X_OVERFLOW: the last abscissa is not representable.";
        if (!series.y.empty()) {
            xLow=(std::min)(xLow,series.origin);
            xHigh=(std::max)(xHigh,std::fma(static_cast<double>(series.y.size()-1),series.step,series.origin));
        }
        for (double sample:series.y) if (std::isfinite(sample)) {yLow=(std::min)(yLow,sample);yHigh=(std::max)(yHigh,sample);}
    }
    if ((std::isfinite(xLow) && !std::isfinite(xHigh-xLow)) || (std::isfinite(yLow) && !std::isfinite(yHigh-yLow)))
        return "WFG_RANGE_OVERFLOW: the complete dataset span exceeds this linear f64 profile.";
    return {};
}

std::string nextCursorId(const Design &design) {
    for(std::size_t i=1;;++i){auto id="cursor-"+std::to_string(i);
        if(std::none_of(design.cursors.begin(),design.cursors.end(),[&](const auto &c){return c.id==id;}))return id;}
}
std::optional<Point> cursorPosition(const State &state,const Cursor &c) {
    if(c.mode==CursorMode::Free)return std::isfinite(c.x)&&std::isfinite(c.y)?std::optional<Point>{{c.x,c.y}}:std::nullopt;
    const auto &series=state.displayedData().series;
    const auto it=std::find_if(series.begin(),series.end(),[&](const auto &s){return s.id==c.plotId;});
    if(it==series.end()||c.sourceIndex>=it->y.size()||!std::isfinite(it->y[c.sourceIndex]))return {};
    const double x=std::fma(static_cast<double>(c.sourceIndex),it->step,it->origin);
    if(!std::isfinite(x))return {};return Point{x,it->y[c.sourceIndex]};
}
bool positionCursor(const State &state,Cursor &c,Point p) {
    if(!std::isfinite(p.x)||!std::isfinite(p.y))return false;
    if(c.mode==CursorMode::Free){c.x=p.x;c.y=p.y;return true;}
    const auto &all=state.displayedData().series;
    const auto it=std::find_if(all.begin(),all.end(),[&](const auto &s){return s.id==c.plotId;});
    if(it==all.end()||it->y.empty())return false;
    // Uniform WFG abscissae: find the nearest finite sample to the requested X.
    const double at=std::clamp((p.x-it->origin)/it->step,0.,static_cast<double>(it->y.size()-1));
    const auto center=static_cast<std::size_t>(std::round(at));
    std::optional<std::size_t> best;
    for(std::size_t delta=0;delta<it->y.size();++delta){
        if(center>=delta&&std::isfinite(it->y[center-delta]))best=center-delta;
        if(center+delta<it->y.size()&&std::isfinite(it->y[center+delta])&&
            (!best||std::abs(static_cast<double>(center+delta)-at)<std::abs(static_cast<double>(*best)-at)))best=center+delta;
        if(best)break;
    }
    if(!best)return false;c.sourceIndex=*best;c.x=std::fma(static_cast<double>(*best),it->step,it->origin);c.y=it->y[*best];return true;
}
std::string validate(const Design &design) {
    if(design.cursors.size()>256)return "WFG_CURSOR_LIMIT: at most 256 cursors.";
    std::unordered_set<std::string> cursorIds;
    for(const auto &c:design.cursors)if(c.id.empty()||c.id.size()>256||!cursorIds.insert(c.id).second||c.name.size()>1024||
        c.plotId.size()>256||!std::isfinite(c.x)||!std::isfinite(c.y)||c.rgb>0xFFFFFF||
        (c.mode!=CursorMode::Free&&c.mode!=CursorMode::SinglePlot)||c.lineStyle<0||c.lineStyle>4||
        c.lineWidth<0||c.lineWidth>4||c.pointStyle<0||c.pointStyle>16||c.crossStyle<0||c.crossStyle>8)
        return "WFG_CURSOR: invalid identity, position or style.";
    if (design.annotations.size() > 256) return "WFG_ANNOTATION_LIMIT: too many annotations.";
    for (const auto &annotation : design.annotations)
        if (annotation.text.empty() || annotation.text.size() > 1024 || !std::isfinite(annotation.x) || !std::isfinite(annotation.y))
            return "WFG_ANNOTATION: finite coordinates and nonempty text are required.";
    for (const auto *title : {&design.xTitle, &design.yTitle}) {
        if (!std::isfinite(title->x) || !std::isfinite(title->y) || title->x < 0 || title->x > 1 || title->y < 0 || title->y > 1 ||
            !std::isfinite(title->width) || !std::isfinite(title->height) || title->width < 24 || title->width > 10000 ||
            title->height < 16 || title->height > 10000 || !std::isfinite(title->pointSize) || title->pointSize < 6 || title->pointSize > 144 ||
            title->rgb > 0xFFFFFF || title->family.empty() || title->family.size() > 256 || title->style.size() > 128 ||
            (title->alignment != "default" && title->alignment != "left" && title->alignment != "center" && title->alignment != "right"))
            return "WFG_TITLE_STYLE: invalid title bounds or font.";
    }
    if (!validRange(design.xRange) || !validRange(design.yRange))
        return "WFG_AXIS_RANGE: finite minimum must be less than maximum.";
    if (design.xLabel.size() > 1024 || design.yLabel.size() > 1024 || design.plots.size() > maximumSeries)
        return "WFG_CONFIG_LIMIT: axis label or plot count exceeds the profile limit.";
    std::unordered_set<std::string> ids;
    for (const auto &plot : design.plots) {
        if (plot.id.empty() || plot.id.size() > 256 || !ids.insert(plot.id).second || plot.name.size() > 1024)
            return "WFG_PLOT_ID: unique, nonempty plot IDs are required.";
        if (!std::isfinite(plot.width) || plot.width < 0.5 || plot.width > 8.0 || plot.rgb > 0xFFFFFF)
            return "WFG_PLOT_STYLE: width must be between 0.5 and 8; color must be RGB.";
    }
    return {};
}

bool setInitialData(State &state, const Dataset &data, std::string *error) {
    auto issue = validate(data);
    if (error) *error = issue;
    if (!issue.empty()) return false;
    try {
        State candidate;candidate.initialData=data;candidate.initialData.datatype.reset();candidate.initialData.exactSource.reset();
        candidate.datatype=legacyDataType(candidate.initialData);
        auto exact=initialValueSource(candidate);
        const auto decoded=decodeTypedValue(candidate.datatype,exact,error);if(!decoded)return false;
        candidate.initialData.datatype=candidate.datatype;candidate.initialData.exactSource=decoded->exactSource;
        auto preparedSource=decoded->exactSource;
        static_assert(std::is_nothrow_move_assignable_v<binding::TypeRef> && std::is_nothrow_move_assignable_v<Dataset>);
        state.initialData=std::move(candidate.initialData);state.datatype=std::move(candidate.datatype);
        state.typedInitialData=std::move(preparedSource);
        return true;
    } catch(const std::exception &failure) {if(error)*error=failure.what();return false;}
}


}
