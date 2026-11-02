// SPDX-License-Identifier: Apache-2.0
// Extracted from the existing Studio data owner; no editor or runtime.
#include "frog_native_engine/CollectionModel.hpp"
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/CollectionBinding.hpp"
namespace frog::native_engine {

bool isCollectionConstant(const DiagramNode &node)noexcept {
    const auto id=collectionFunctionId(node.type);
    return node.kind=="primitive"&&(id=="collection.map.constant"||id=="collection.set.constant");
}

const CollectionAuthoringState *collectionConstantState(const DiagramNode &node)noexcept {
    return isCollectionConstant(node)&&node.collectionConstant?&*node.collectionConstant:nullptr;
}

CollectionAuthoringState *collectionConstantState(DiagramNode &node)noexcept {
    return isCollectionConstant(node)&&node.collectionConstant?&*node.collectionConstant:nullptr;
}

const CollectionAuthoringState *collectionStateFromWidget(const WidgetInstance &widget)noexcept {
    return widget.classId==kCollectionWidgetClass&&widget.collection?&*widget.collection:nullptr;
}

CollectionAuthoringState *collectionStateFromWidget(WidgetInstance &widget)noexcept {
    return widget.classId==kCollectionWidgetClass&&widget.collection?&*widget.collection:nullptr;
}


std::span<const std::string> collectionBuildRows(const DiagramNode &node)noexcept {
    const auto operation=collectionFunctionId(node.type);
    if(operation!="collection.map.build"&&operation!="collection.set.build")return {};
    static const std::vector<std::string> initial{"row_0"};
    return node.collectionBuild?std::span<const std::string>{node.collectionBuild->rows}:std::span<const std::string>{initial};
}
std::string collectionRowPort(std::string_view row,std::string_view role) {return "row:"+std::string(row)+":"+std::string(role);}

}
