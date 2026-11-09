#include "FrogDocumentWaveformGraph.hpp"
#include "FrogDocumentStructureSerializationHelpers.hpp"
#include "frog_native_engine/CanonicalObjectReferenceDescriptor.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>
#include <locale>
#include <sstream>
#include <stdexcept>

namespace frog::native_engine::document_internal {
namespace {
void dump(std::ostringstream &out, const JsonValue &v) {
    switch (v.type) {
    case JsonValue::Type::Null: out << "null"; break;
    case JsonValue::Type::Boolean: out << (v.boolean ? "true" : "false"); break;
    case JsonValue::Type::Number:
        // The parser retains numeric spelling, including integers wider than
        // f64's exact range. Unknown future payloads must keep those bits.
        if (!v.string.empty()) out << v.string; else out << v.number;
        break;
    case JsonValue::Type::String: appendJsonString(out, v.string); break;
    case JsonValue::Type::Array:
        out << '[';
        for (std::size_t i = 0; i < v.array.size(); ++i) { if (i) out << ','; dump(out, v.array[i]); }
        out << ']'; break;
    case JsonValue::Type::Object: {
        out << '{'; bool comma = false;
        for (const auto &[key, value] : v.object) {
            if (comma) out << ','; comma = true; appendJsonString(out, key); out << ':'; dump(out, value);
        }
        out << '}'; break;
    }}
}
const JsonValue *field(const JsonValue &v, std::string_view key, JsonValue::Type type) {
    auto *value = objectField(v, key);
    if (value && value->type != type) throw std::runtime_error("Invalid type for " + std::string(key));
    return value;
}
double number(const JsonValue &v, std::string_view key, double fallback) {
    auto *value = field(v, key, JsonValue::Type::Number); return value ? value->number : fallback;
}
std::string text(const JsonValue &v, std::string_view key, const std::string &fallback) {
    auto *value = field(v, key, JsonValue::Type::String); return value ? value->string : fallback;
}
bool flag(const JsonValue &v, std::string_view key, bool fallback) {
    auto *value = field(v, key, JsonValue::Type::Boolean); return value ? value->boolean : fallback;
}
void checkKeys(const JsonValue &v, std::initializer_list<std::string_view> known) {
    if (v.type != JsonValue::Type::Object) throw std::runtime_error("An object is required.");
    for (const auto &[key, value] : v.object)
        if (std::find(known.begin(), known.end(), key) == known.end())
            throw std::runtime_error("Unsupported field: " + key);
}
bool sameProjection(const graph::Dataset &a,const graph::Dataset &b) {
    const auto defaultEmpty=[](const auto &data){return data.series.size()==1 && data.series[0].y.empty() && data.series[0].origin==0 && data.series[0].step==1;};
    // The original empty WFG-1 profile has zero plots; an empty numeric Y
    // value has one empty plot. No timed or independently indexed plot is lost.
    if((a.series.empty() && defaultEmpty(b)) || (b.series.empty() && defaultEmpty(a)))return true;
    if(a.series.size()!=b.series.size())return false;
    for(std::size_t i=0;i<a.series.size();++i) {
        const auto &left=a.series[i],&right=b.series[i];
        if(left.origin!=right.origin || left.step!=right.step || left.y.size()!=right.y.size())return false;
        for(std::size_t j=0;j<left.y.size();++j)if(left.y[j]!=right.y[j] && !(std::isnan(left.y[j]) && std::isnan(right.y[j])))return false;
    }
    return true;
}
}
void deserializeWaveformGraph(const JsonValue &value, graph::State &state) {
    try {
        checkKeys(value, {"profile", "x_label", "y_label", "autoscale_x", "autoscale_y", "grid_visible", "legend_visible",
            "x_min", "x_max", "y_min", "y_max", "plots", "initial_data", "x_title", "y_title", "annotations",
            "datatype", "datatype_explicit", "typed_initial_data", "cursors"});
        if (text(value, "profile", "") != "WFG-1-linear") throw std::runtime_error("Unsupported Waveform Graph profile.");
        graph::State candidate;
        auto &d = candidate.design;
        d.xLabel = text(value, "x_label", d.xLabel); d.yLabel = text(value, "y_label", d.yLabel);
        const auto readTitle = [&](std::string_view key, graph::TitleStyle &title) {
            if (const auto *v = field(value, key, JsonValue::Type::Object)) {
                checkKeys(*v, {"positioned", "x", "y", "width", "height", "point_size", "use_default_size", "family", "style", "rgb", "alignment"});
                title.positioned = flag(*v, "positioned", false);
                title.x = number(*v, "x", 0); title.y = number(*v, "y", 0);
                title.width = number(*v, "width", 120); title.height = number(*v, "height", 24);
                title.pointSize = number(*v, "point_size", 9);
                title.pointSizeUsesDefault = flag(*v, "use_default_size", std::abs(title.pointSize - 9.0) < 0.001);
                title.family = text(*v, "family", title.family); title.style = text(*v, "style", title.style);
                title.alignment = text(*v, "alignment", title.alignment);
                const double rgb = number(*v, "rgb", title.rgb);
                if (!std::isfinite(rgb) || rgb < 0 || rgb > 0xFFFFFF || std::floor(rgb) != rgb) throw std::runtime_error("Invalid title color.");
                title.rgb = static_cast<std::uint32_t>(rgb);
            }
        };
        readTitle("x_title", d.xTitle); readTitle("y_title", d.yTitle);
        if (const auto *annotations = field(value, "annotations", JsonValue::Type::Array)) {
            if (annotations->array.size() > 256) throw std::runtime_error("Annotation quota exceeded.");
            for (const auto &annotation : annotations->array) {
                checkKeys(annotation, {"text", "x", "y"});
                d.annotations.push_back({text(annotation,"text",""), number(annotation,"x",0), number(annotation,"y",0)});
            }
        }
        if(const auto *list=field(value,"cursors",JsonValue::Type::Array)) {
            if(list->array.size()>256)throw std::runtime_error("Cursor quota exceeded.");
            for(const auto &v:list->array){
                checkKeys(v,{"id","name","plot","x","y","index","mode","visible","show_name","allow_dragging","rgb","line_style","line_width","point_style","style"});
                graph::Cursor c;c.id=text(v,"id","");c.name=text(v,"name","");c.plotId=text(v,"plot","");
                c.x=number(v,"x",0);c.y=number(v,"y",0);
                const auto integer=[&](const char *key,int maximum,int fallback){const double n=number(v,key,fallback);
                    if(!std::isfinite(n)||n<0||n>maximum||std::floor(n)!=n)throw std::runtime_error(std::string("Invalid cursor ")+key);return static_cast<int>(n);};
                c.sourceIndex=static_cast<std::size_t>(integer("index",2147483647,0));
                c.mode=static_cast<graph::CursorMode>(integer("mode",1,0));c.rgb=static_cast<std::uint32_t>(integer("rgb",0xFFFFFF,0xFFE07A));
                c.lineStyle=integer("line_style",4,0);c.lineWidth=integer("line_width",4,1);c.pointStyle=integer("point_style",16,1);c.crossStyle=integer("style",8,8);
                c.visible=flag(v,"visible",true);c.nameVisible=flag(v,"show_name",true);c.movable=flag(v,"allow_dragging",true);d.cursors.push_back(std::move(c));
            }
        }
        // Old WFG annotations were text + a small cross at a data coordinate.
        // Preserve each as an independently configurable free cursor, once.
        for(const auto &a:d.annotations){graph::Cursor c;c.id=graph::nextCursorId(d);c.name=a.text;c.x=a.x;c.y=a.y;
            c.pointStyle=11;c.crossStyle=0;c.rgb=0xDADEE3;d.cursors.push_back(std::move(c));}
        d.annotations.clear();
        d.autoscaleX = flag(value, "autoscale_x", true); d.autoscaleY = flag(value, "autoscale_y", true);
        d.gridVisible = flag(value, "grid_visible", true); d.legendVisible = flag(value, "legend_visible", true);
        d.xRange = {number(value, "x_min", 0), number(value, "x_max", 100)};
        d.yRange = {number(value, "y_min", -10), number(value, "y_max", 10)};
        if (auto *plots = field(value, "plots", JsonValue::Type::Array)) {
            if (plots->array.size() > graph::maximumSeries) throw std::runtime_error("Plot quota exceeded.");
            for (const auto &p : plots->array) {
                checkKeys(p, {"id", "name", "rgb", "visible", "lines", "points", "width"});
                graph::Plot plot;
                plot.id = text(p, "id", ""); plot.name = text(p, "name", "");
                const auto rgb = number(p, "rgb", plot.rgb);
                if (!std::isfinite(rgb) || rgb < 0 || rgb > 0xFFFFFF || std::floor(rgb) != rgb) throw std::runtime_error("Invalid plot color.");
                plot.rgb = static_cast<std::uint32_t>(rgb); plot.width = number(p, "width", plot.width);
                plot.visible = flag(p, "visible", true); plot.lines = flag(p, "lines", true); plot.points = flag(p, "points", false);
                d.plots.push_back(std::move(plot));
            }
        }
        std::size_t count = 0;
        if (auto *data = field(value, "initial_data", JsonValue::Type::Array)) {
            if (data->array.size() > graph::maximumSeries) throw std::runtime_error("Series quota exceeded.");
            for (const auto &s : data->array) {
                checkKeys(s, {"id", "name", "origin", "step", "y"});
                graph::Series series;
                series.id = text(s, "id", ""); series.name = text(s, "name", "");
                series.origin = number(s, "origin", 0); series.step = number(s, "step", 1);
                auto *samples = field(s, "y", JsonValue::Type::Array);
                if (!samples || samples->array.size() > graph::maximumSamples - count) throw std::runtime_error("Sample quota exceeded or missing Y array.");
                count += samples->array.size(); series.y.reserve(samples->array.size());
                for (const auto &y : samples->array) {
                    if (y.type == JsonValue::Type::Number) series.y.push_back(y.number);
                    else if (y.type == JsonValue::Type::String && y.string == "NaN") series.y.push_back(std::numeric_limits<double>::quiet_NaN());
                    else if (y.type == JsonValue::Type::String && y.string == "Inf") series.y.push_back(std::numeric_limits<double>::infinity());
                    else if (y.type == JsonValue::Type::String && y.string == "-Inf") series.y.push_back(-std::numeric_limits<double>::infinity());
                    else throw std::runtime_error("Y must be numeric or an explicit NaN/Inf/-Inf gap.");
                }
                candidate.initialData.series.push_back(std::move(series));
            }
        }
        if (const auto error = graph::validate(d); !error.empty()) throw std::runtime_error(error);
        if (const auto error = graph::validate(candidate.initialData); !error.empty()) throw std::runtime_error(error);
        const auto *declared=value.member("datatype");
        if(declared) {
            if(declared->type==JsonValue::Type::String) {
                const auto parsed=binding::parseType(declared->string);
                if(!parsed.valid())throw std::runtime_error(parsed.error);candidate.datatype=parsed.type;
            } else {
                const auto parsed=canonicalObjectReferenceReadType(*declared);
                if(!parsed)throw std::runtime_error("Invalid complete Waveform Graph datatype schema.");candidate.datatype=*parsed;
            }
            if(const auto error=graph::validateType(candidate.datatype);!error.empty())throw std::runtime_error(error);
        } else candidate.datatype=graph::legacyDataType(candidate.initialData);
        candidate.datatypeExplicit=flag(value,"datatype_explicit",declared!=nullptr);
        if(candidate.datatypeExplicit && !declared)throw std::runtime_error("An explicit Waveform Graph datatype is missing.");
        const auto *typed=value.member("typed_initial_data");
        if(typed && !declared)throw std::runtime_error("Typed Waveform Graph initial data requires its declared datatype.");
        auto exact=typed?*typed:graph::initialValueSource(candidate);
        std::string issue;const auto decoded=graph::decodeTypedValue(candidate.datatype,exact,&issue);
        if(!decoded)throw std::runtime_error(issue);
        if(value.member("initial_data")) {
            if(!sameProjection(candidate.initialData,*decoded))throw std::runtime_error("Typed and legacy Waveform Graph initial data disagree.");
        } else candidate.initialData=*decoded;
        // Retain legacy IDs/names/styles while recording the exact typed value.
        candidate.initialData.datatype=candidate.datatype;candidate.initialData.exactSource=decoded->exactSource;
        candidate.typedInitialData=decoded->exactSource;
        state = std::move(candidate);
    } catch (const std::exception &error) {
        std::ostringstream source; source.imbue(std::locale::classic()); source << std::setprecision(17); dump(source, value);
        state = {};
        state.preservedSource = source.str();
        state.diagnostic = "WFG_UNSUPPORTED_SOURCE: " + std::string(error.what());
    }
}
std::string serializeWaveformGraph(const graph::State &state) {
    if (state.preservedSource) return *state.preservedSource;
    std::ostringstream o; o.imbue(std::locale::classic()); o << std::boolalpha << std::setprecision(17);
    const auto &d = state.design;
    o << "{\"profile\":\"WFG-1-linear\",\"x_label\":"; appendJsonString(o, d.xLabel);
    o << ",\"y_label\":"; appendJsonString(o, d.yLabel);
    if(state.typedInitialData || state.datatypeExplicit || state.datatype!=binding::TypeRef::array(binding::TypeRef::primitive("f64"))) {
        o << ",\"datatype\":";dump(o,canonicalObjectReferenceWriteType(state.datatype));
        o << ",\"datatype_explicit\":" << state.datatypeExplicit << ",\"typed_initial_data\":";
        dump(o,graph::initialValueSource(state));
    }
    const auto writeTitle = [&](const char *key, const graph::TitleStyle &title) {
        if (title == graph::TitleStyle{}) return; // Preserve legacy defaults without extra source fields.
        o << ",\"" << key << "\":{\"positioned\":" << title.positioned << ",\"x\":" << title.x << ",\"y\":" << title.y
          << ",\"width\":" << title.width << ",\"height\":" << title.height << ",\"point_size\":" << title.pointSize
          << ",\"use_default_size\":" << (title.pointSizeUsesDefault ? "true" : "false") << ",\"family\":";
        appendJsonString(o, title.family); o << ",\"style\":"; appendJsonString(o, title.style);
        o << ",\"rgb\":" << title.rgb << ",\"alignment\":"; appendJsonString(o, title.alignment); o << '}';
    };
    writeTitle("x_title", d.xTitle); writeTitle("y_title", d.yTitle);
    if (!d.annotations.empty()) {
        o << ",\"annotations\":[";
        for (std::size_t i=0; i<d.annotations.size(); ++i) {
            if (i) o << ',';
            const auto &annotation=d.annotations[i]; o << "{\"text\":"; appendJsonString(o,annotation.text);
            o << ",\"x\":" << annotation.x << ",\"y\":" << annotation.y << '}';
        }
        o << ']';
    }
    if(!d.cursors.empty()) {
        o << ",\"cursors\":[";
        for(std::size_t i=0;i<d.cursors.size();++i){if(i)o<<',';const auto &c=d.cursors[i];
            o<<"{\"id\":";appendJsonString(o,c.id);o<<",\"name\":";appendJsonString(o,c.name);o<<",\"plot\":";appendJsonString(o,c.plotId);
            o<<",\"x\":"<<c.x<<",\"y\":"<<c.y<<",\"index\":"<<c.sourceIndex<<",\"mode\":"<<static_cast<int>(c.mode)
             <<",\"visible\":"<<c.visible<<",\"show_name\":"<<c.nameVisible<<",\"allow_dragging\":"<<c.movable
             <<",\"rgb\":"<<c.rgb<<",\"line_style\":"<<c.lineStyle<<",\"line_width\":"<<c.lineWidth
             <<",\"point_style\":"<<c.pointStyle<<",\"style\":"<<c.crossStyle<<'}';
        }o<<']';
    }
    o << ",\"autoscale_x\":" << d.autoscaleX << ",\"autoscale_y\":" << d.autoscaleY
      << ",\"grid_visible\":" << d.gridVisible << ",\"legend_visible\":" << d.legendVisible
      << ",\"x_min\":" << d.xRange.minimum << ",\"x_max\":" << d.xRange.maximum
      << ",\"y_min\":" << d.yRange.minimum << ",\"y_max\":" << d.yRange.maximum << ",\"plots\":[";
    for (std::size_t i = 0; i < d.plots.size(); ++i) {
        if (i) o << ','; const auto &p = d.plots[i];
        o << "{\"id\":"; appendJsonString(o, p.id); o << ",\"name\":"; appendJsonString(o, p.name);
        o << ",\"rgb\":" << p.rgb << ",\"visible\":" << p.visible << ",\"lines\":" << p.lines
          << ",\"points\":" << p.points << ",\"width\":" << p.width << '}';
    }
    o << "],\"initial_data\":[";
    for (std::size_t i = 0; i < state.initialData.series.size(); ++i) {
        if (i) o << ','; const auto &s = state.initialData.series[i];
        o << "{\"id\":"; appendJsonString(o, s.id); o << ",\"name\":"; appendJsonString(o, s.name);
        o << ",\"origin\":" << s.origin << ",\"step\":" << s.step << ",\"y\":[";
        for (std::size_t j = 0; j < s.y.size(); ++j) {
            if (j) o << ','; const double v = s.y[j];
            if (std::isnan(v)) o << "\"NaN\"";
            else if (std::isinf(v)) o << (v < 0 ? "\"-Inf\"" : "\"Inf\"");
            else o << v;
        }
        o << "]}";
    }
    return o.str() + "]}";
}
}
