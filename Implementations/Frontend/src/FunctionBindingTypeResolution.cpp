#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include "frog_native_engine/FunctionPortIdentities.hpp"
#include "frog_native_engine/BindingType.hpp"
#include "PortableText.hpp"

#include <algorithm>
#include <charconv>
#include <limits>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace frog::native_engine {

namespace {

std::optional<std::size_t> variadicOrdinal(
    const FunctionBindingDefinition &definition,
    std::string_view portId) noexcept {
    if (definition.variadicInputPrefix.empty() ||
        !portId.starts_with(definition.variadicInputPrefix)) {
        return std::nullopt;
    }
    const std::string_view suffix = portId.substr(
        definition.variadicInputPrefix.size());
    std::size_t oneBased = 0U;
    const auto parsed = std::from_chars(
        suffix.data(),
        suffix.data() + suffix.size(),
        oneBased);
    if (suffix.empty() || (suffix.size() > 1U && suffix.front() == '0') ||
        parsed.ec != std::errc{} ||
        parsed.ptr != suffix.data() + suffix.size() ||
        oneBased <= definition.minimumInputCount) {
        return std::nullopt;
    }
    return oneBased - 1U;
}

} // namespace



std::optional<std::size_t> functionBindingInputPortOrdinal(
    const FunctionBindingDefinition &definition,
    std::string_view portId) noexcept {
    portId = canonicalFunctionBindingPortId(definition.catalogId, portId);
    if(arrayPaletteKind(definition.catalogId)!=ArrayPaletteKind::None) {
        std::size_t ordinal=0;
        for(const auto &p:arrayPalettePorts(definition.catalogId,32U)) if(!p.output) {
            if(p.id==portId) return ordinal;
            ++ordinal;
        }
        return {};
    }
    if (definition.catalogId == "array.build") return buildArrayInputIndex(portId);
    if (definition.catalogId == "array.reshape") {
        if (portId == "array") return 0U;
        if (const auto dimension = reshapeArrayDimensionIndex(portId)) return *dimension + 1U;
        return {};
    }
    std::size_t ordinal = 0U;
    for (const auto &port : definition.ports) {
        if (port.direction != FunctionPortDirection::Input) {
            continue;
        }
        if (port.portId == portId) {
            return ordinal;
        }
        ++ordinal;
    }
    return variadicOrdinal(definition, portId);
}

} // namespace frog::native_engine
