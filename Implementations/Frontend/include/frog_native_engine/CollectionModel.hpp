#pragma once
#include "frog_native_engine/CollectionAuthoringState.hpp"
#include "frog_native_engine/WidgetInstance.hpp"
namespace frog::native_engine {
struct DiagramNode;
[[nodiscard]] std::span<const std::string> collectionBuildRows(const DiagramNode &) noexcept;
[[nodiscard]] std::string collectionRowPort(std::string_view,std::string_view);
bool isCollectionConstant(const DiagramNode &) noexcept;
const CollectionAuthoringState *collectionConstantState(const DiagramNode &) noexcept;
CollectionAuthoringState *collectionConstantState(DiagramNode &) noexcept;
const CollectionAuthoringState *collectionStateFromWidget(const WidgetInstance &) noexcept;
CollectionAuthoringState *collectionStateFromWidget(WidgetInstance &) noexcept;
}
