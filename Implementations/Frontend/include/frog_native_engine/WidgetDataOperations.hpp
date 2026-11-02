#pragma once
#include "frog_native_engine/WidgetInstance.hpp"
#include "frog_native_engine/ContainedWidgetPath.hpp"
#include "frog_native_engine/WidgetDataCompatibility.hpp"
#include <optional>
#include <string>
namespace frog::native_engine::data_operations {
inline constexpr std::size_t maximumBytes=8U*1024U*1024U;
bool assign(WidgetInstance &,const WidgetInstance &);
std::string encode(const WidgetInstance &);
std::optional<WidgetInstance> decode(std::string_view);
std::optional<WidgetInstance> read(const WidgetInstance &,const ContainedWidgetPath &);
WidgetInstance defaultValue(const WidgetInstance &);
namespace widget_data_detail {
enum class PreparationOutcome {Rejected,Unchanged,Changed};
struct ArrayDefaultPreparation {WidgetInstance value;PreparationOutcome outcome=PreparationOutcome::Rejected;};
bool hasInterpretedXY(const WidgetInstance &);
bool compatibleWithFieldPolicy(const WidgetInstance &,const WidgetInstance &,bool);
bool compactPayloadHasNoSavedDefaults(std::string_view);
void sanitize(WidgetInstance &);
void intrinsic(WidgetInstance &);
bool materialize(WidgetInstance &,const ArrayElementValue *);
PreparationOutcome assignWithFieldPolicy(WidgetInstance &,const WidgetInstance &,bool);
ArrayDefaultPreparation prepareArrayDefaultValue(const WidgetInstance &);
bool emptyEditingArray(const WidgetInstance &);
}
}
