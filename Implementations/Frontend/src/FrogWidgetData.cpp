#include "frog_native_engine/FrogWidgetData.hpp"
#include "frog_native_engine/TabControl.hpp"
#include <unordered_set>
namespace frog::native_engine {
bool validateFrogWidgetTabMembership(FrogWidgetDataView widgets,std::string *error) {
    const auto fail = [&](std::string text) { if (error) *error = std::move(text);return false; };
    for (const auto &object : widgets.objects()) {
        if (object.widget && isTabWidget(*object.widget) && !tabPagesValid(*object.widget)) return fail("Tab '" + object.id + "' has an invalid page model.");
        std::unordered_set<std::string> seen{object.id};
        const auto *child = &object;
        while (!child->parentTabId.empty() || !child->parentTabPageId.empty()) {
            const auto *parent = widgets.objectById(child->parentTabId);
            if (!parent || !parent->widget || !isTabWidget(*parent->widget) || !tabPageIndex(*parent->widget, child->parentTabPageId))
                return fail("Object '" + object.id + "' refers to a missing Tab page.");
            if (!seen.emplace(parent->id).second) return fail("Object '" + object.id + "' has a cyclic Tab parent.");
            child = parent;
        }
    }
    return true;
}
const WidgetInstance *FrogWidgetDataView::widgetById(std::string_view id) const noexcept {
    if(const auto *object=objectById(id);object&&object->widget)return &*object->widget;
    const auto find=[&](const auto &self,const WidgetInstance &widget)->const WidgetInstance* {
        if(widget.instanceId==id)return &widget;
        if(const auto *cluster=clusterContainerState(widget))
            for(const auto &field:cluster->elements)if(field.widget)
                if(const auto *found=self(self,*field.widget))return found;
        return nullptr;
    };
    for(const auto &object:objects_)if(object.widget)
        if(const auto *found=find(find,*object.widget))return found;
    return nullptr;
}

}
