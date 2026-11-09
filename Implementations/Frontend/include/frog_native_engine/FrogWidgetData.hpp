#pragma once
// Persistent widget records and a borrowed semantic view. No canvas, selection,
// window, layout service or runtime session belongs to this interface.
#include "frog_native_engine/WidgetInstance.hpp"
#include <algorithm>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include <utility>
#include <unordered_map>
namespace frog::native_engine {
enum class StaticVisualKind {
    Svg,
    Dib
};

struct StaticVisual {
    StaticVisualKind kind = StaticVisualKind::Svg;
    std::string sourceName;
    std::string mediaRef;
    std::string mediaType;
    std::string svgText;
    // Raster payloads may be persisted as their original compressed data URI.
    // The decoded pixels are a cache; filling it does not change document content.
    mutable SharedBuffer<std::uint8_t> dibBytes;
    mutable SharedBuffer<std::uint8_t> rasterSourceCache;
    double naturalWidth = 1.0;
    double naturalHeight = 1.0;
    bool preserveAspectRatio = true;
    int rotationQuarterTurns = 0;
    bool flipHorizontal = false;
    bool flipVertical = false;
};

[[nodiscard]] inline bool staticVisualUsesEncodedRasterSource(const StaticVisual &visual) noexcept {
    const bool encoded = visual.mediaRef.starts_with("data:image/png;base64,") ||
        visual.mediaRef.starts_with("data:image/jpeg;base64,");
    return encoded && (visual.dibBytes.empty() ||
        visual.dibBytes.storageIdentity() == visual.rasterSourceCache.storageIdentity());
}

struct SceneObject {
    std::string id;
    Rect bounds;
    std::optional<WidgetInstance> widget{};
    std::optional<StaticVisual> staticVisual{};
    bool locked = false;
    bool hidden = false;
    std::string parentTabId;
    std::string parentTabPageId;
};

struct SceneObjectGroup {
    std::string id;
    std::vector<std::string> objectIds;
};


struct FrogWidgetPresentation {
    std::optional<std::uint32_t> frontBackground,frontGrid,diagramBackground,diagramGrid;
    int frontBackgroundOpacity=100,frontGridOpacity=100,diagramBackgroundOpacity=100,diagramGridOpacity=100;
    bool gridVisible=true,snapToGrid=true;
    int zoomPercent=100;
    Point origin{};
};
class FrogWidgetDataView {
    std::span<const SceneObject> objects_;
    std::span<const SceneObjectGroup> groups_;
    double gridPitch_=8.0;
    Size size_{};
    FrogWidgetPresentation presentation_;
    const void *lookupContext_ = nullptr;
    const SceneObject *(*lookup_)(const void*,std::string_view) = nullptr;
public:
    FrogWidgetDataView() = default;
    FrogWidgetDataView(std::span<const SceneObject> objects, std::span<const SceneObjectGroup> groups={}, double gridPitch=8.0, Size size={}, const void *context=nullptr, const SceneObject *(*lookup)(const void*,std::string_view)=nullptr,FrogWidgetPresentation presentation={}) : objects_(objects),groups_(groups),gridPitch_(gridPitch),size_(size),presentation_(std::move(presentation)),lookupContext_(context),lookup_(lookup) {}
    [[nodiscard]] const std::optional<std::uint32_t>& ideFrontPanelBackgroundRgb() const noexcept {return presentation_.frontBackground;}
    [[nodiscard]] const std::optional<std::uint32_t>& ideFrontPanelGridRgb() const noexcept {return presentation_.frontGrid;}
    [[nodiscard]] const std::optional<std::uint32_t>& ideDiagramBackgroundRgb() const noexcept {return presentation_.diagramBackground;}
    [[nodiscard]] const std::optional<std::uint32_t>& ideDiagramGridRgb() const noexcept {return presentation_.diagramGrid;}
    [[nodiscard]] int ideFrontPanelBackgroundOpacityPercent() const noexcept {return presentation_.frontBackgroundOpacity;}
    [[nodiscard]] int ideFrontPanelGridOpacityPercent() const noexcept {return presentation_.frontGridOpacity;}
    [[nodiscard]] int ideDiagramBackgroundOpacityPercent() const noexcept {return presentation_.diagramBackgroundOpacity;}
    [[nodiscard]] int ideDiagramGridOpacityPercent() const noexcept {return presentation_.diagramGridOpacity;}
    [[nodiscard]] bool ideFrontPanelGridVisible() const noexcept {return presentation_.gridVisible;}
    [[nodiscard]] bool ideFrontPanelSnapToGrid() const noexcept {return presentation_.snapToGrid;}
    [[nodiscard]] int ideFrontPanelZoomPercent() const noexcept {return presentation_.zoomPercent;}
    [[nodiscard]] Point ideFrontPanelViewportOrigin() const noexcept {return presentation_.origin;}
    [[nodiscard]] std::span<const SceneObjectGroup> objectGroups() const noexcept {return groups_;}
    [[nodiscard]] double gridPitch() const noexcept {return gridPitch_;}
    [[nodiscard]] Size frontPanelCanvasSize() const noexcept {return size_;}
    [[nodiscard]] std::span<const SceneObject> objects() const noexcept {return objects_;}
    [[nodiscard]] const SceneObject *objectById(std::string_view id) const noexcept {
        if(lookup_)return lookup_(lookupContext_,id);
        const auto found=std::find_if(objects_.begin(),objects_.end(),[&](const auto &o){return o.id==id;});
        return found==objects_.end()?nullptr:&*found;
    }
    [[nodiscard]] const WidgetInstance *widgetById(std::string_view id) const noexcept;
};
[[nodiscard]] bool validateFrogWidgetTabMembership(FrogWidgetDataView widgets, std::string *error = nullptr);
// Read-owned persistent records. This is not an editor scene: no hit testing,
// selection, drag/resize, layout refresh, paint cache or platform handle.
struct FrogWidgetIdHash {
    using is_transparent=void;
    std::size_t operator()(std::string_view value) const noexcept {return std::hash<std::string_view>{}(value);}
};
class FrogWidgetData {
    std::vector<SceneObject> objects_;
    std::vector<SceneObjectGroup> groups_;
    std::vector<bool> preserveBounds_;
    std::unordered_map<std::string,std::size_t,FrogWidgetIdHash,std::equal_to<>> objectIndex_;
public:
    double gridPitchValue=8.0;
    Size size{};
    std::optional<std::uint32_t> frontBackground,frontGrid,diagramBackground,diagramGrid;
    int frontBackgroundOpacity=100,frontGridOpacity=100,diagramBackgroundOpacity=100,diagramGridOpacity=100;
    bool gridVisible=true,snapToGrid=true;
    int zoomPercent=100;
    Point origin{};
    [[nodiscard]] operator FrogWidgetDataView() const noexcept {return FrogWidgetDataView(objects_,groups_,gridPitchValue,size,this,[](const void *owner,std::string_view id){return static_cast<const FrogWidgetData*>(owner)->objectById(id);},FrogWidgetPresentation{frontBackground,frontGrid,diagramBackground,diagramGrid,frontBackgroundOpacity,frontGridOpacity,diagramBackgroundOpacity,diagramGridOpacity,gridVisible,snapToGrid,zoomPercent,origin});}
    [[nodiscard]] const std::vector<SceneObject>& objects() const noexcept {return objects_;}
    [[nodiscard]] std::vector<SceneObject>& mutableObjects() noexcept {return objects_;}
    [[nodiscard]] const std::vector<SceneObjectGroup>& objectGroups() const noexcept {return groups_;}
    [[nodiscard]] bool preservesBounds(std::size_t i) const noexcept {return i<preserveBounds_.size() && preserveBounds_[i];}
    [[nodiscard]] const SceneObject *objectById(std::string_view id) const noexcept {const auto found=objectIndex_.find(id);return found==objectIndex_.end()?nullptr:&objects_[found->second];}
    [[nodiscard]] SceneObject *objectById(std::string_view id) noexcept {return const_cast<SceneObject*>(std::as_const(*this).objectById(id));}
    [[nodiscard]] const WidgetInstance *widgetById(std::string_view id) const noexcept {return static_cast<FrogWidgetDataView>(*this).widgetById(id);}
    [[nodiscard]] WidgetInstance *widgetById(std::string_view id) noexcept {return const_cast<WidgetInstance*>(std::as_const(*this).widgetById(id));}
    void addObject(SceneObject value,bool preserveBounds=false) {objectIndex_[value.id]=objects_.size();objects_.push_back(std::move(value));preserveBounds_.push_back(preserveBounds);}
    bool addObjectGroup(SceneObjectGroup value) {groups_.push_back(std::move(value));return true;}
    [[nodiscard]] bool validateTabMembership(std::string *error=nullptr) const {return validateFrogWidgetTabMembership(*this,error);}
    void setFrontPanelCanvasSize(Size value) noexcept {size=value;}
    void setGridPitch(double value) noexcept {gridPitchValue=value;}
    void setIdeFrontPanelBackground(std::optional<std::uint32_t> value,int opacity=100) noexcept {frontBackground=value;frontBackgroundOpacity=opacity;}
    void setIdeFrontPanelGrid(std::optional<std::uint32_t> value,int opacity=100) noexcept {frontGrid=value;frontGridOpacity=opacity;}
    void setIdeDiagramBackground(std::optional<std::uint32_t> value,int opacity=100) noexcept {diagramBackground=value;diagramBackgroundOpacity=opacity;}
    void setIdeDiagramGrid(std::optional<std::uint32_t> value,int opacity=100) noexcept {diagramGrid=value;diagramGridOpacity=opacity;}
    void setIdeFrontPanelGridVisible(bool value) noexcept {gridVisible=value;}
    void setIdeFrontPanelSnapToGrid(bool value) noexcept {snapToGrid=value;}
    void setIdeFrontPanelZoomPercent(int value) noexcept {zoomPercent=value;}
    void setIdeFrontPanelViewportOrigin(Point value) noexcept {origin=value;}
};

}
