#pragma once
#include "frog_native_engine/DiagramBindingResolver.hpp"

namespace frog::native_engine {
inline constexpr std::string_view kInPlaceStructureType =
    "frog.studio.structures.in_place_element_structure";
inline constexpr std::string_view kInPlaceBody = "body";
inline constexpr std::string_view kInPlaceSemanticProfile = "frog.studio.ipe@1";
inline constexpr std::string_view kInPlaceDvrClass = "data_value";
inline constexpr std::string_view kInPlaceMapActionDomain = "frog.studio.ipe.map_action@1";
// Seed absent legacy counters from both live and deliberately obsolete ports.
void seedInPlaceIdentityCounters(DiagramNode &, std::span<const DiagramEdge> = {},
                                 std::span<const DiagramWireFragment> = {});
binding::TypeRef inPlaceMapActionType();
bool isInPlaceStructure(const DiagramNode &) noexcept;
bool inPlaceKindSupported(std::string_view) noexcept;
std::string inPlacePortId(std::string_view pair, std::string_view role,
                          std::string_view access = {}, std::size_t dimension = 0);
struct InPlacePort {
    std::string id, pairId, label;
    FunctionPortDirection direction = FunctionPortDirection::Input;
    bool inner = false, right = false, required = false;
    double row = 0;
    binding::TypeRef type;
};
using InPlaceInputResolver = std::function<binding::BindingResult(std::string_view)>;
std::vector<InPlacePort> inPlacePorts(const DiagramNode &, const InPlaceInputResolver &);
std::optional<InPlacePort> inPlacePort(const DiagramNode &, std::string_view,
                                       const InPlaceInputResolver &);
std::optional<bool> inPlacePortIsInner(const DiagramNode &, std::string_view);
std::optional<FunctionPortDirection> inPlacePortDirection(const DiagramNode &, std::string_view);
binding::BindingResult resolveInPlacePort(const DiagramNode &, std::string_view,
                                          const InPlaceInputResolver &);
binding::TypeResolution checkInPlaceConnection(const DiagramNode &, std::string_view,
                                               const binding::TypeRef &,
                                               const InPlaceInputResolver &);
struct InPlaceDiagnostic { std::string code, portId, message; };
std::vector<InPlaceDiagnostic> inPlaceStructureDiagnostics(const DiagramNode &,
    std::span<const DiagramEdge>, const InPlaceInputResolver &);
std::vector<std::string> validateInPlaceStructure(const DiagramNode &, std::span<const DiagramEdge>,
                                                  const InPlaceInputResolver &);
} // namespace frog::native_engine
