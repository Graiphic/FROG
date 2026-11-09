#include "frog_native_engine/WidgetInstance.hpp"

#include <memory>

namespace frog::native_engine {

ArrayContainerState::ArrayContainerState(const ArrayContainerState &other)
    : rank(other.rank),
      shape(other.shape),
      firstIndices(other.firstIndices),
      visibleCounts(other.visibleCounts),
      orientation(other.orientation),
      allowResize(other.allowResize),
      cellWidth(other.cellWidth),
      cellHeight(other.cellHeight),
      elementGap(other.elementGap),
      indexDisplayVisible(other.indexDisplayVisible),
      verticalScrollbarVisible(other.verticalScrollbarVisible),
      horizontalScrollbarVisible(other.horizontalScrollbarVisible),
      scrollbarGap(other.scrollbarGap),
      indexDisplayWidth(other.indexDisplayWidth),
      indexBoxHeight(other.indexBoxHeight),
      indexBoxGap(other.indexBoxGap),
      indexStepWidth(other.indexStepWidth),
      indexStepGap(other.indexStepGap),
      indexValueGap(other.indexValueGap),
      indexContainerPadding(other.indexContainerPadding),
      indexContainerBorderWidth(other.indexContainerBorderWidth),
      indexContainerFillRgb(other.indexContainerFillRgb),
      indexContainerStrokeRgb(other.indexContainerStrokeRgb),
      indexValueFillRgb(other.indexValueFillRgb),
      indexValueStrokeRgb(other.indexValueStrokeRgb),
      indexStepButtonFillRgb(other.indexStepButtonFillRgb),
      indexStepButtonStrokeRgb(other.indexStepButtonStrokeRgb),
      indexStepArrowFillRgb(other.indexStepArrowFillRgb),
      indexStepArrowStrokeRgb(other.indexStepArrowStrokeRgb),
      frameFillRgb(other.frameFillRgb),
      frameStrokeRgb(other.frameStrokeRgb),
      arrayRegionFillRgb(other.arrayRegionFillRgb),
      arrayRegionStrokeRgb(other.arrayRegionStrokeRgb),
      widgetComposedElements(other.widgetComposedElements),
      elementClassId(other.elementClassId),
      elementValueType(other.elementValueType),
      elementRole(other.elementRole),
      elements(other.elements),
      elementIcon(other.elementIcon),
      elementTemplate(other.elementTemplate == nullptr
          ? nullptr
          : std::make_shared<WidgetInstance>(*other.elementTemplate)) {}

ArrayContainerState &ArrayContainerState::operator=(const ArrayContainerState &other) {
    if (this == &other) {
        return *this;
    }
    rank = other.rank;
    shape = other.shape;
    firstIndices = other.firstIndices;
    visibleCounts = other.visibleCounts;
    orientation = other.orientation;
    allowResize = other.allowResize;
    cellWidth = other.cellWidth;
    cellHeight = other.cellHeight;
    elementGap = other.elementGap;
    indexDisplayVisible = other.indexDisplayVisible;
    verticalScrollbarVisible = other.verticalScrollbarVisible;
    horizontalScrollbarVisible = other.horizontalScrollbarVisible;
    scrollbarGap = other.scrollbarGap;
    indexDisplayWidth = other.indexDisplayWidth;
    indexBoxHeight = other.indexBoxHeight;
    indexBoxGap = other.indexBoxGap;
    indexStepWidth = other.indexStepWidth;
    indexStepGap = other.indexStepGap;
    indexValueGap = other.indexValueGap;
    indexContainerPadding = other.indexContainerPadding;
    indexContainerBorderWidth = other.indexContainerBorderWidth;
    indexContainerFillRgb = other.indexContainerFillRgb;
    indexContainerStrokeRgb = other.indexContainerStrokeRgb;
    indexValueFillRgb = other.indexValueFillRgb;
    indexValueStrokeRgb = other.indexValueStrokeRgb;
    indexStepButtonFillRgb = other.indexStepButtonFillRgb;
    indexStepButtonStrokeRgb = other.indexStepButtonStrokeRgb;
    indexStepArrowFillRgb = other.indexStepArrowFillRgb;
    indexStepArrowStrokeRgb = other.indexStepArrowStrokeRgb;
    frameFillRgb = other.frameFillRgb;
    frameStrokeRgb = other.frameStrokeRgb;
    arrayRegionFillRgb = other.arrayRegionFillRgb;
    arrayRegionStrokeRgb = other.arrayRegionStrokeRgb;
    widgetComposedElements = other.widgetComposedElements;
    elementClassId = other.elementClassId;
    elementValueType = other.elementValueType;
    elementRole = other.elementRole;
    elements = other.elements;
    elementIcon = other.elementIcon;
    elementTemplate = other.elementTemplate == nullptr
        ? nullptr
        : std::make_shared<WidgetInstance>(*other.elementTemplate);
    return *this;
}

ClusterContainerState::ClusterContainerState(const ClusterContainerState &other)
    : padding(other.padding),
      autoSizing(other.autoSizing),
      frameFillRgb(other.frameFillRgb),
      frameStrokeRgb(other.frameStrokeRgb) {
    nextFieldSerial=other.nextFieldSerial;
    elements.reserve(other.elements.size());
    for (const auto &element : other.elements) {
        elements.push_back(ClusterElement{
            element.fieldId,
            element.technicalName,
            element.logicalOrder,
            element.widget == nullptr
                ? nullptr
                : std::make_shared<WidgetInstance>(*element.widget)
        });
    }
}

ClusterContainerState &ClusterContainerState::operator=(const ClusterContainerState &other) {
    if (this == &other) {
        return *this;
    }
    padding = other.padding;
    autoSizing = other.autoSizing;
    frameFillRgb = other.frameFillRgb;
    frameStrokeRgb = other.frameStrokeRgb;
    nextFieldSerial=other.nextFieldSerial;
    elements.clear();
    elements.reserve(other.elements.size());
    for (const auto &element : other.elements) {
        elements.push_back(ClusterElement{
            element.fieldId,
            element.technicalName,
            element.logicalOrder,
            element.widget == nullptr
                ? nullptr
                : std::make_shared<WidgetInstance>(*element.widget)
        });
    }
    return *this;
}

} // namespace frog::native_engine
