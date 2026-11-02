#include "frog_native_engine/WidgetTypedValueSource.hpp"
#include "frog_native_engine/graph/XYGraph.hpp"
#include "frog_native_engine/TimestampSourceProjection.hpp"
#include "frog_native_engine/ArrayCapacity.hpp"
#include "CanonicalJsonSupport.hpp"
#include "frog_native_engine/ApplicationControlSourceCodec.hpp"
#include "frog_native_engine/CollectionAuthoringState.hpp"
#include "frog_native_engine/ReferenceWidget.hpp"
#include "frog_native_engine/WidgetBindingType.hpp"
#include "frog_native_engine/WidgetDataOperations.hpp"
#include "frog_native_engine/NumericValueLexeme.hpp"
#include <array>
#include <charconv>
#include <cmath>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>

namespace frog::native_engine {
namespace {
using Json = source::JsonValue;
using Type = binding::TypeRef;
using Kind = binding::TypeKind;
using namespace canonical_json;
constexpr std::size_t maximumNodes = 4096, maximumBytes = 16'000'000;
void preflight(const WidgetInstance &widget,
               std::set<const WidgetInstance *> &active, unsigned depth,
               std::size_t &nodes) {
  if (depth > 32 || ++nodes > maximumNodes || !active.insert(&widget).second)
    throw std::runtime_error("Typed widget Source exceeds its depth/node bound "
                             "or contains a container cycle.");
  if (isArrayWidget(widget)) {
    const auto *array = arrayContainerState(widget);
    if (!array || !array->elementTemplate)
      throw std::runtime_error(
          "An Array Source value requires its real element prototype.");
    preflight(*array->elementTemplate, active, depth + 1, nodes);
  } else if (isClusterWidget(widget)) {
    const auto *cluster = clusterContainerState(widget);
    if (!cluster)
      throw std::runtime_error(
          "A Cluster Source value requires its real field inventory.");
    for (const auto &field : cluster->elements) {
      if (!field.widget)
        throw std::runtime_error("A Cluster field lost its widget.");
      preflight(*field.widget, active, depth + 1, nodes);
    }
  }
  active.erase(&widget);
}
Json numeric(std::string token) {
  Json value;
  value.type = Json::Type::Number;
  value.string = std::move(token);
  value.number = std::numeric_limits<double>::quiet_NaN();
  return value;
}
struct Projection {
  std::size_t nodes = 0, bytes = 0;
  // XY retains the authored JSON spelling. The execution Source boundary uses
  // the same explicit Array envelope as every other typed widget projection.
  Json xySource(const Type &type, const Json &source, unsigned depth) {
    if (depth > 32)
      throw std::runtime_error("Graph XY Source exceeds its nesting bound.");
    if (type.kind == Kind::Array) {
      const auto &items=source.type==Json::Type::Array ? source : *source.member("elements");
      auto shape=array(), elements=array();
      shape.array.push_back(numeric(std::to_string(items.array.size())));
      elements.array.reserve(items.array.size());
      for(const auto &item:items.array)
        elements.array.push_back(xySource(type.children.front(),item,depth+1));
      auto result=object();result.object={{"shape",std::move(shape)},{"elements",std::move(elements)}};
      return result;
    }
    if (type.kind == Kind::Cluster) {
      auto result=object();
      for(std::size_t i=0;i<type.children.size();++i)
        result.object.emplace(type.fieldIds[i],xySource(type.children[i],*source.member(type.fieldIds[i]),depth+1));
      return result;
    }
    return source;
  }
  void enter(unsigned depth, std::size_t charge = 0) {
    if (depth > 32 || ++nodes > maximumNodes)
      throw std::runtime_error(
          "Typed widget Source exceeds its 4096-value/32-depth bound.");
    addBytes(charge);
  }
  void addBytes(std::size_t count) {
    if (count > maximumBytes - bytes)
      throw std::runtime_error(
          "Typed widget Source exceeds its owned byte bound.");
    bytes += count;
  }
  void jsonBudget(const Json &value, unsigned depth) {
    enter(depth, value.string.size());
    for (const auto &[key, child] : value.object) {
      addBytes(key.size());
      jsonBudget(child, depth + 1);
    }
    for (const auto &child : value.array)
      jsonBudget(child, depth + 1);
  }
  std::string integerToken(const WidgetInstance &widget, const Type &type) {
    const bool unsignedType = type.name.starts_with('u');
    std::optional<std::string> token = widgetNumericIntegerValueLexeme(widget);
    if (!token && !widget.numericValue &&
        (isEnumWidget(widget) || isRingWidget(widget))) {
      const auto &items = ringItems(widget);
      const auto selected =
          widget.initialValue
              ? std::find_if(items.begin(), items.end(),
                             [&](const auto &item) {
                               return item.id == *widget.initialValue;
                             })
              : items.end();
      if (selected == items.end())
        throw std::runtime_error("The choice widget has no represented numeric "
                                 "value or valid selected choice ID.");
      token = std::to_string(selected->value);
    }
    if (!token) {
      if (isNumericWidget(widget) &&
          numericCanonicalValueTypeIsIntegral(widget.valueType))
        throw std::runtime_error(
            "The numeric widget lost its authoritative exact integer lexeme.");
      const double value = widgetNumericValue(widget);
      if (!std::isfinite(value) || std::trunc(value) != value ||
          std::abs(value) > 0x1p53)
        throw std::runtime_error(
            "A choice/integer widget has no exact represented integer value.");
      std::array<char, 128> buffer{};
      const auto r = std::to_chars(buffer.data(), buffer.data() + buffer.size(),
                                   value, std::chars_format::fixed, 0);
      if (r.ec != std::errc{})
        throw std::runtime_error("Integer widget value could not be encoded.");
      token = std::string(buffer.data(), r.ptr);
    }
    const auto canonical = numeric_value::canonicalWideIntegerLexeme(
        *token, unsignedType ? "u64" : "i64");
    if (!canonical)
      throw std::runtime_error("The widget integer does not fit its exact "
                               "signed/unsigned representation.");
    const unsigned bits = type.name=="i4" || type.name=="u4" ? 4
                          : type.name.ends_with("8")    ? 8
                          : type.name.ends_with("16") ? 16
                          : type.name.ends_with("32") ? 32
                          : type.name.ends_with("64") ? 64
                                                      : 0;
    if (!bits)
      throw std::runtime_error(
          "The widget integer width has no supported Source value projection.");
    if (unsignedType) {
      std::uint64_t n = 0;
      const auto r = std::from_chars(canonical->data(),
                                     canonical->data() + canonical->size(), n);
      if (r.ec != std::errc{} ||
          (bits < 64 && n > ((std::uint64_t{1} << bits) - 1)))
        throw std::runtime_error(
            "The widget integer exceeds its declared unsigned width.");
    } else {
      std::int64_t n = 0;
      const auto r = std::from_chars(canonical->data(),
                                     canonical->data() + canonical->size(), n);
      if (r.ec != std::errc{} ||
          (bits < 64 && (n < -(std::int64_t{1} << (bits - 1)) ||
                         n > ((std::int64_t{1} << (bits - 1)) - 1))))
        throw std::runtime_error(
            "The widget integer exceeds its declared signed width.");
    }
    return *canonical;
  }
  Json value(const WidgetInstance &widget, const Type &type, unsigned depth) {
    enter(depth);
    if (isXYGraphWidget(widget)) {
      if (widget.xyGraph && widget.xyGraph->preservedSource)
        throw std::runtime_error("Graph XY has an unsupported preserved Source configuration; no empty fallback value may execute.");
      if (!widget.xyGraph || widget.xyGraph->datatype != type)
        throw std::runtime_error("Graph XY lost its exact structural Source schema.");
      std::string why;
      const auto &source=xy::initialValueSource(*widget.xyGraph);
      if (!xy::decodeTypedValue(type, source, &why))
        throw std::runtime_error("Graph XY Source value: " + why);
      auto projected=xySource(type,source,depth);
      addBytes(xy::exactJson(projected).size());
      return projected;
    }
    if (type.kind == Kind::Map || type.kind == Kind::Set) {
      if (!widget.collection ||
          !collectionTypesEqual(collectionType(*widget.collection), type))
        throw std::runtime_error(
            "The Collection widget and complete Source schema disagree.");
      const auto typed = collectionValue(*widget.collection);
      if (!typed)
        throw std::runtime_error(
            "The Collection widget has no valid typed value.");
      if (widget.collection->data.retainedBytes() > maximumBytes)
        throw std::runtime_error(
            "The Collection widget exceeds its Source projection byte bound.");
      auto output = writeCollectionData(*typed);
      jsonBudget(output, depth);
      return output;
    }
    if (type.kind == Kind::Cluster) {
      const auto *cluster = clusterContainerState(widget);
      if (!cluster || cluster->elements.size() != type.children.size() ||
          type.fieldIds.size() != type.children.size())
        throw std::runtime_error(
            "The Cluster widget lost its complete logical field schema.");
      auto output = object();
      for (std::size_t i = 0; i < type.children.size(); ++i) {
        const auto field = std::find_if(
            cluster->elements.begin(), cluster->elements.end(),
            [&](const auto &f) { return f.fieldId == type.fieldIds[i]; });
        if (field == cluster->elements.end() || !field->widget)
          throw std::runtime_error(
              "The Cluster Source field identity no longer exists.");
        addBytes(type.fieldIds[i].size());
        output.object.emplace(
            type.fieldIds[i],
            value(*field->widget, type.children[i], depth + 1));
      }
      return output;
    }
    if (type.kind == Kind::Array) {
      const auto *state = arrayContainerState(widget);
      if (!state || !state->elementTemplate || type.children.size() != 1 ||
          state->rank != type.rank || state->shape.size() != type.rank ||
          type.rank < 1 || type.rank > 16)
        throw std::runtime_error(
            "Array Source shape, prototype and complete rank must agree.");
      const auto cardinality = checkedArrayElementCount(state->shape);
      if (!cardinality)
        throw std::runtime_error("Array Source exceeds 4294967295 elements.");
      const auto count = *cardinality;
      if (!arrayAllocationFitsBudget(count, sizeof(Json), 0, (std::numeric_limits<std::size_t>::max)()))
        throw std::runtime_error("Not enough memory to complete this operation.");
      std::map<std::vector<std::uint32_t>, const ArrayElementValue *> stored;
      for (const auto &cell : state->elements) {
        if (cell.indices.size() != state->rank)
          throw std::runtime_error("A stored Array cell has the wrong rank.");
        bool inside = true;
        for (std::size_t d = 0; d < state->rank; ++d)
          inside = inside && cell.indices[d] < state->shape[d];
        if (!inside) {
          if (cell.defined)
            throw std::runtime_error(
                "A defined Array cell lies outside its data shape.");
          continue;
        }
        if (!stored.emplace(cell.indices, &cell).second)
          throw std::runtime_error("Duplicate Array cell coordinates cannot be "
                                   "projected unambiguously.");
      }
      auto shape = array(), elements = array();
      for (const auto extent : state->shape)
        shape.array.push_back(numeric(std::to_string(extent)));
      elements.array.reserve(count);
      // A sparse one-cell read root reuses the authoritative materializer,
      // avoiding a copy/scan of every other cell for each requested position.
      WidgetInstance readRoot;
      readRoot.classId = widget.classId;
      readRoot.valueType = widget.valueType;
      readRoot.arrayContainer.emplace();
      auto &reader = *readRoot.arrayContainer;
      reader.rank = state->rank;
      reader.shape = state->shape;
      reader.elementTemplate = state->elementTemplate;
      std::size_t prototypeNodes = 0;
      std::set<const WidgetInstance *> active;
      preflight(*state->elementTemplate, active, depth + 1, prototypeNodes);
      for (std::size_t index = 0; index < count; ++index) {
        auto n = index;
        std::vector<std::uint32_t> coordinates(state->rank);
        for (std::size_t d = state->rank; d--;) {
          coordinates[d] = static_cast<std::uint32_t>(n % state->shape[d]);
          n /= state->shape[d];
        }
        reader.elements.clear();
        if (const auto found = stored.find(coordinates); found != stored.end())
          reader.elements.push_back(*found->second);
        const auto cell = data_operations::read(readRoot, {{{}, coordinates}});
        if (!cell)
          throw std::runtime_error("The actual Array cell could not be "
                                   "materialized with its prototype.");
        // Array cardinality is independent of the per-element recursive
        // schema/value bound; Map/Set and nested records retain their limits.
        Projection element;
        elements.array.push_back(element.value(*cell, type.children[0], depth + 1));
      }
      auto output = object();
      output.object = {{"shape", std::move(shape)},
                       {"elements", std::move(elements)}};
      return output;
    }
    if (type.kind == Kind::Reference) {
      if (!isReferenceWidget(widget) || !widget.referenceValue ||
          widget.referenceValue->kind != ApplicationControlKind::Reference ||
          !collectionTypesEqual(referenceWidgetType(widget), type))
        throw std::runtime_error("The reference widget has no matching "
                                 "symbolic Source configuration.");
      auto output = object();
      output.object.emplace("frog_studio_object_reference",
                            application_control_source::writeConfiguration(
                                *widget.referenceValue));
      jsonBudget(output, depth);
      return output;
    }
    const Type *physical = &type;
    if (type.kind == Kind::Enum) {
      if (type.children.size() != 1)
        throw std::runtime_error(
            "An Enum Source value requires its exact representation.");
      physical = &type.children[0];
    }
    if (physical->kind != Kind::Primitive)
      throw std::runtime_error(
          "This widget value has no declared portable Source projection.");
    if (physical->name == "bool")
      return boolean(widgetBooleanValue(widget));
    if (physical->name == "string" || physical->name == "path") {
      const auto text = physical->name == "path" ? widgetPathValue(widget)
                                                 : widgetStringValue(widget);
      addBytes(text.size());
      return string(std::string(text));
    }
    if(physical->name=="c32"||physical->name=="c64") {
      const auto lexical=widgetNumericComplexValueLexeme(widget);
      const auto parts=lexical?numeric_value::parseComplexNumber(*lexical,physical->name):std::nullopt;
      if(!parts)throw std::runtime_error("The complex widget lost one of its exact finite components.");
      const auto component=[&](double value) {
        std::array<char,128> buffer{};
        const auto result=physical->name=="c32"?
            std::to_chars(buffer.data(),buffer.data()+buffer.size(),static_cast<float>(value),std::chars_format::general,std::numeric_limits<float>::max_digits10):
            std::to_chars(buffer.data(),buffer.data()+buffer.size(),value,std::chars_format::general,std::numeric_limits<double>::max_digits10);
        if(result.ec!=std::errc{})throw std::runtime_error("Complex widget component could not be projected exactly.");
        return numeric(std::string(buffer.data(),result.ptr));
      };
      auto result=object();result.object={{"real",component(parts->real)},{"imaginary",component(parts->imaginary)}};
      jsonBudget(result,depth);return result;
    }
    if (physical->name=="timestamp") {
      std::optional<source::JsonValue> projected;
      if(widget.numericValue)projected=timestamp_source::json(*widget.numericValue);
      else if(widget.initialValue)projected=timestamp_source::lexical(*widget.initialValue);
      if(!projected)throw std::runtime_error("The Timestamp widget's actual binary64 seconds have no exact signed64.64 Source projection (missing, nonfinite, out of range or finer than2^-64).");
      jsonBudget(*projected,depth);return *projected;
    }
    if (physical->integer())
      return numeric(integerToken(widget, *physical));
    if (physical->name == "f32" || physical->name == "f64") {
      double number = 0.;
      if (widget.numericValue)
        number = *widget.numericValue;
      else if (widget.initialValue) {
        const auto r = std::from_chars(
            widget.initialValue->data(),
            widget.initialValue->data() + widget.initialValue->size(), number);
        if (r.ec != std::errc{} ||
            r.ptr != widget.initialValue->data() + widget.initialValue->size())
          throw std::runtime_error(
              "The real widget has no exact represented numeric token.");
      } else
        throw std::runtime_error(
            "The real widget has no represented numeric value.");
      if (!std::isfinite(number))
        throw std::runtime_error("A non-finite widget value has no finite "
                                 "Source literal projection.");
      std::array<char, 128> buffer{};
      std::to_chars_result result;
      if (physical->name == "f32") {
        const auto f = static_cast<float>(number);
        if (isRingWidget(widget) && static_cast<double>(f) != number)
          throw std::runtime_error("The Ring selected value is not exactly representable in its declared f32 type.");
        if (!std::isfinite(f) || (number != 0 && f == 0))
          throw std::runtime_error(
              "The widget value is outside its declared f32 range.");
        result = std::to_chars(buffer.data(), buffer.data() + buffer.size(), f,
                               std::chars_format::general,
                               std::numeric_limits<float>::max_digits10);
      } else
        result = std::to_chars(buffer.data(), buffer.data() + buffer.size(),
                               number, std::chars_format::general,
                               std::numeric_limits<double>::max_digits10);
      if (result.ec != std::errc{})
        throw std::runtime_error(
            "The widget real value could not be encoded exactly.");
      return numeric(std::string(buffer.data(), result.ptr));
    }
    throw std::runtime_error(
        "The widget type has no implemented Source value projection.");
  }
};
} // namespace
std::optional<source::JsonValue>
typedWidgetSourceValue(const WidgetInstance &widget, std::string *error) {
  try {
    std::set<const WidgetInstance *> active;
    std::size_t schemaNodes = 0;
    preflight(widget, active, 0, schemaNodes);
    const auto type = binding::widgetType(widget);
    if (!type.valid() || !type.type.known())
      throw std::runtime_error(type.error.empty()
                                   ? "The widget has no complete Source type."
                                   : type.error);
    Projection projection;
    return projection.value(widget, type.type, 0);
  } catch (const std::bad_alloc &) {
    if(error)*error="Not enough memory to complete this operation.";
    return {};
  } catch (const std::exception &e) {
    if (error)
      *error = e.what();
    return {};
  }
}
} // namespace frog::native_engine
