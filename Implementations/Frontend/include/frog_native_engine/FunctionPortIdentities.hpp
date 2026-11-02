#pragma once
#include "frog_native_engine/BindingType.hpp"
#include <algorithm>
#include <charconv>
#include <cmath>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
namespace frog::native_engine {
struct DiagramNode;

inline std::string buildArrayInputId(unsigned index) {
    if(index==0) return "array_a";
    if(index==1) return "array_b"; // Preserve historical wire identities.
    return "input_"+std::to_string(index+1);
}

inline std::optional<unsigned> buildArrayInputIndex(std::string_view id) noexcept {
    if(id=="array_a") return 0U;
    if(id=="array_b" || id=="input_2") return 1U;
    if(!id.starts_with("input_")) return {};
    id.remove_prefix(6);
    if(id.empty() || id.front()=='0') return {};
    unsigned n=0;
    const auto p=std::from_chars(id.data(),id.data()+id.size(),n);
    if(p.ec!=std::errc{} || p.ptr!=id.data()+id.size() || n<3 || n>32) return {};
    return n-1;
}

inline constexpr double kBuildArrayWidth = 47.33;

inline constexpr double kBuildArrayRowPitch = 11.14;

inline constexpr double kBuildArrayLeftRail = .665;

inline constexpr double kBuildArrayRightRail = 46.665;

inline double buildArrayHeight(unsigned count) noexcept {
    return kBuildArrayRowPitch * std::clamp(count,1U,32U);
}

inline double buildArrayInputY(unsigned index) noexcept { return kBuildArrayRowPitch * (index + .5); }

struct BuildArrayMode {
    bool hasInputs=false, mixedRanks=false, invalidRanks=false;
    bool concatenate=false, toggleEnabled=false;
    unsigned maximumRank=0, outputRank=0;
};

inline BuildArrayMode buildArrayMode(bool manual, std::span<const int> ranks) noexcept {
    BuildArrayMode state;
    unsigned minimum=16;
    for(const int rank:ranks) if(rank>=0) {
        state.hasInputs=true;
        minimum=(std::min)(minimum,static_cast<unsigned>(rank));
        state.maximumRank=(std::max)(state.maximumRank,static_cast<unsigned>(rank));
    }
    if(!state.hasInputs) return state;
    state.mixedRanks=minimum!=state.maximumRank;
    state.invalidRanks=state.maximumRank-minimum>1 || state.maximumRank>16;
    state.concatenate=state.maximumRank>0 && (manual || state.mixedRanks);
    state.toggleEnabled=state.maximumRank>0 && !state.mixedRanks && !state.invalidRanks;
    state.outputRank=state.concatenate ? state.maximumRank:state.maximumRank+1;
    return state;
}

struct BuildArrayVisualState {
    BuildArrayMode mode;
    std::vector<int> inputRanks; // -1: no connected, resolved type evidence.
};

inline constexpr unsigned kIndexArrayMaximumOutputs = 32;

inline constexpr double kIndexArrayFirstIndexY = 36.0;

inline constexpr double kIndexArrayIndexPitch = 16.0;

inline constexpr double kIndexArrayBottomPadding = 16.0;

struct IndexArrayPort { unsigned output = 0, dimension = 0; bool isOutput = false; };

inline std::string indexArrayInputId(unsigned group, unsigned dimension) {
    return group == 0 && dimension == 0 ? "index" :
        "index:" + std::to_string(group) + ":" + std::to_string(dimension);
}

inline std::string indexArrayOutputId(unsigned group) {
    return group == 0 ? "element" : "element:" + std::to_string(group);
}

inline std::optional<IndexArrayPort> parseIndexArrayPort(std::string_view id) noexcept {
    if (id == "index") return IndexArrayPort{};
    if (id == "element") return IndexArrayPort{0, 0, true};
    const auto number = [](std::string_view text, unsigned &value) {
        if (text.empty() || (text.size() > 1 && text.front() == '0')) return false;
        const auto parsed = std::from_chars(text.data(), text.data() + text.size(), value);
        return parsed.ec == std::errc{} && parsed.ptr == text.data() + text.size();
    };
    IndexArrayPort port;
    if (id.starts_with("element:")) {
        port.isOutput = true;
        if (!number(id.substr(8), port.output) || port.output == 0) return {};
    } else if (id.starts_with("index:")) {
        id.remove_prefix(6);
        const auto colon = id.find(':');
        if (colon == id.npos || !number(id.substr(0, colon), port.output) ||
            !number(id.substr(colon + 1), port.dimension) ||
            (port.output == 0 && port.dimension == 0)) return {};
    } else return {};
    if (port.output >= kIndexArrayMaximumOutputs || port.dimension >= 16) return {};
    return port;
}

inline const binding::TypeRef &indexArrayDataType(const binding::TypeRef &type) {
    if (type.kind == binding::TypeKind::Cluster &&
        std::count_if(type.children.begin(), type.children.end(), [](const auto &child) {
            return child.kind == binding::TypeKind::Array;
        }) == 1) {
        const auto field = std::find(type.fieldIds.begin(), type.fieldIds.end(), "Y");
        if (field != type.fieldIds.end()) return type.children[field - type.fieldIds.begin()];
    }
    return type;
}

inline double indexArrayHeight(unsigned rank, unsigned outputs) noexcept {
    const auto rows = std::clamp(rank, 1U, 16U) *
        std::clamp(outputs, 1U, kIndexArrayMaximumOutputs);
    return kIndexArrayFirstIndexY + kIndexArrayIndexPitch * (rows - 1U) +
        kIndexArrayBottomPadding;
}

inline double indexArrayGroupPitch(unsigned rank) noexcept {
    return kIndexArrayIndexPitch * std::clamp(rank, 1U, 16U);
}

inline double indexArrayIndexY(unsigned rank, unsigned group, unsigned dimension) noexcept {
    return kIndexArrayFirstIndexY + kIndexArrayIndexPitch * (rank * group + dimension);
}

inline double indexArrayOutputY(unsigned rank, unsigned group) noexcept {
    // NI: the output precedes the first index; repeat that relation per access.
    return indexArrayIndexY(rank, group, 0) - kIndexArrayIndexPitch * .5;
}

enum class ArrayEditKind { None, Replace, Insert, Delete, Subset };

inline ArrayEditKind arrayEditKind(std::string_view id) noexcept {
    if (id.starts_with("frog.studio.")) id.remove_prefix(12);
    if (id == "array.replace_subset") return ArrayEditKind::Replace;
    if (id == "array.insert") return ArrayEditKind::Insert;
    if (id == "array.delete") return ArrayEditKind::Delete;
    if (id == "array.subset") return ArrayEditKind::Subset;
    return ArrayEditKind::None;
}

enum class ArrayEditPortKind { Array, Index, Length, Value, Result, Deleted };

struct ArrayEditPort {
    ArrayEditPortKind kind;
    unsigned group = 0, dimension = 0;
    bool output() const noexcept { return kind == ArrayEditPortKind::Result || kind == ArrayEditPortKind::Deleted; }
};

// Geometry depends only on explicit bounded rank/group counts.
struct ReplaceSubsetVisualLayout {
    static constexpr double width = 48;
    static constexpr double scale = 43.5 / 46;
    static constexpr double left = 2.25 - .9 * scale;
    static constexpr double topHeight = 33.11 * scale;
    static constexpr double rowHeight = 11.14 * scale;
    unsigned rank, groups;
    explicit ReplaceSubsetVisualLayout(unsigned dimensions,unsigned groupCount) :
        rank(std::clamp(dimensions,1U,16U)), groups(std::clamp(groupCount,1U,32U)) {}
    explicit ReplaceSubsetVisualLayout(const DiagramNode &node);
    double rowTop(unsigned group,unsigned row) const noexcept { return topHeight+(group*(rank+1)+row)*rowHeight; }
    double height() const noexcept { return topHeight+groups*(rank+1)*rowHeight; }
    double portY(const ArrayEditPort &port) const noexcept {
        if(port.kind==ArrayEditPortKind::Index) return rowTop(port.group,port.dimension)+5.78*scale;
        if(port.kind==ArrayEditPortKind::Value) return rowTop(port.group,rank)+4.415*scale;
        return (17.61 + (port.kind==ArrayEditPortKind::Result ? 11.14 : 0))*scale;
    }
};

inline std::string arrayEditIndexId(unsigned group, unsigned dimension) {
    return group == 0 && dimension == 0 ? "index" : "index:" + std::to_string(group) + ":" + std::to_string(dimension);
}

inline std::string arrayEditValueId(unsigned group) { return group == 0 ? "subset" : "subset:" + std::to_string(group); }

inline std::string arrayEditLengthId(unsigned dimension) { return dimension == 0 ? "length" : "length:" + std::to_string(dimension); }

inline std::optional<ArrayEditPort> parseArrayEditPort(std::string_view id) noexcept {
    if (id == "array") return ArrayEditPort{ArrayEditPortKind::Array};
    if (id == "result") return ArrayEditPort{ArrayEditPortKind::Result};
    if (id == "deleted") return ArrayEditPort{ArrayEditPortKind::Deleted};
    if (id == "index") return ArrayEditPort{ArrayEditPortKind::Index};
    if (id == "length") return ArrayEditPort{ArrayEditPortKind::Length};
    if (id == "subset") return ArrayEditPort{ArrayEditPortKind::Value};
    const auto number = [](std::string_view s, unsigned &value) {
        if (s.empty() || (s.size() > 1 && s.front() == '0')) return false;
        const auto p = std::from_chars(s.data(), s.data() + s.size(), value);
        return p.ec == std::errc{} && p.ptr == s.data() + s.size();
    };
    ArrayEditPort p{ArrayEditPortKind::Index};
    if (id.starts_with("index:")) {
        id.remove_prefix(6); const auto sep = id.find(':');
        if (sep == id.npos || !number(id.substr(0,sep),p.group) || !number(id.substr(sep+1),p.dimension) ||
            (p.group == 0 && p.dimension == 0)) return {};
    } else if (id.starts_with("subset:")) {
        p.kind = ArrayEditPortKind::Value;
        if (!number(id.substr(7),p.group) || p.group == 0) return {};
    } else if (id.starts_with("length:")) {
        p.kind = ArrayEditPortKind::Length;
        if (!number(id.substr(7),p.dimension) || p.dimension == 0) return {};
    } else return {};
    if (p.group >= 32 || p.dimension >= 16) return {};
    return p;
}

inline bool arrayEditPortAllowed(ArrayEditKind kind, const ArrayEditPort &p, unsigned rank = 16, unsigned groups = 32) noexcept {
    if (kind == ArrayEditKind::None || p.group >= groups || p.dimension >= rank) return false;
    switch (p.kind) {
    case ArrayEditPortKind::Array: case ArrayEditPortKind::Result: return true;
    case ArrayEditPortKind::Deleted: return kind == ArrayEditKind::Delete;
    case ArrayEditPortKind::Index: return kind == ArrayEditKind::Replace || p.group == 0;
    case ArrayEditPortKind::Length: return kind == ArrayEditKind::Subset || (kind == ArrayEditKind::Delete && p.dimension == 0);
    case ArrayEditPortKind::Value: return kind == ArrayEditKind::Replace || kind == ArrayEditKind::Insert;
    }
    return false;
}

inline std::string_view arrayEditBasePort(ArrayEditPortKind kind) noexcept {
    switch (kind) {
    case ArrayEditPortKind::Array: return "array";
    case ArrayEditPortKind::Index: return "index";
    case ArrayEditPortKind::Length: return "length";
    case ArrayEditPortKind::Value: return "subset";
    case ArrayEditPortKind::Result: return "result";
    case ArrayEditPortKind::Deleted: return "deleted";
    }
    return {};
}

inline double arrayEditGroupPitch(ArrayEditKind kind, unsigned rank) noexcept {
    return kind == ArrayEditKind::Replace ? ReplaceSubsetVisualLayout::rowHeight*(std::clamp(rank,1U,16U)+1) :
        kind == ArrayEditKind::Insert ? 12.0 : 18.0;
}

struct ArrayEditPortDescriptor { std::string id; ArrayEditPort port; };

struct ArrayEditVisualPort { std::string id; ArrayEditPort port; double y; };

inline bool arrayEditIndexDisabled(ArrayEditKind kind, unsigned mask, const ArrayEditPort &p) noexcept {
    return kind == ArrayEditKind::Delete &&
        p.kind == ArrayEditPortKind::Index && mask && !(mask & (1U << p.dimension));
}

// Visual geometry only; explicit dimension count owns the output rank.
struct ReshapeArrayVisualLayout {
 static constexpr double width=48.0;
 static constexpr double scale=42.0/46.0;
 static constexpr double left=3.0-.9*scale;
 static constexpr double topHeight=33.11*scale;
 static constexpr double chunkHeight=11.14*scale;
 static constexpr double arrayY=17.28*scale;
 // NI places the result one existing visual row below the source.
 static constexpr double resultY=arrayY+chunkHeight;
 unsigned count;
 explicit ReshapeArrayVisualLayout(unsigned dimensions) noexcept : count(std::clamp(dimensions,1U,16U)) {}
 explicit ReshapeArrayVisualLayout(const DiagramNode &node) noexcept;
 double height() const noexcept { return topHeight+count*chunkHeight; }
 static double chunkTop(unsigned dimension) noexcept { return topHeight+dimension*chunkHeight; }
 static double dimensionY(unsigned dimension) noexcept { return chunkTop(dimension)+5.82*scale; }
};

inline std::string reshapeArrayDimensionId(unsigned dimension) {
    return dimension==0 ? "dimension_size" : "input_"+std::to_string(dimension+2);
}

inline std::optional<unsigned> reshapeArrayDimensionIndex(std::string_view id) noexcept {
    if(id=="dimension_size") return 0U;
    if(!id.starts_with("input_")) return {};
    id.remove_prefix(6);
    if(id.empty() || id.front()=='0') return {};
    unsigned n=0; const auto p=std::from_chars(id.data(),id.data()+id.size(),n);
    if(p.ec!=std::errc{} || p.ptr!=id.data()+id.size() || n<3 || n>17) return {};
    return n-2;
}

inline double reshapeArrayHeight(unsigned count) noexcept { return ReshapeArrayVisualLayout(count).height(); }

inline double reshapeArrayDimensionY(unsigned dimension) noexcept { return ReshapeArrayVisualLayout::dimensionY(dimension); }

enum class ArrayPaletteKind { None, Initialize, Interleave, Decimate, BuildMatrix,
    GetDiagonal, GetElements, GetSubset, SetDiagonal, SetElements, SetSubset };

inline std::string_view arrayPaletteId(std::string_view id) {
    if(id.starts_with("frog.studio.")) id.remove_prefix(12);
    return id;
}

inline ArrayPaletteKind arrayPaletteKind(std::string_view id) {
    id=arrayPaletteId(id);
    if(id=="array.initialize") return ArrayPaletteKind::Initialize;
    if(id=="array.interleave_1d") return ArrayPaletteKind::Interleave;
    if(id=="array.decimate_1d") return ArrayPaletteKind::Decimate;
    if(id=="array.matrix.build") return ArrayPaletteKind::BuildMatrix;
    if(id=="array.matrix.get_diagonal") return ArrayPaletteKind::GetDiagonal;
    if(id=="array.matrix.get_elements") return ArrayPaletteKind::GetElements;
    if(id=="array.matrix.get_subset") return ArrayPaletteKind::GetSubset;
    if(id=="array.matrix.set_diagonal") return ArrayPaletteKind::SetDiagonal;
    if(id=="array.matrix.set_elements") return ArrayPaletteKind::SetElements;
    if(id=="array.matrix.set_subset") return ArrayPaletteKind::SetSubset;
    return ArrayPaletteKind::None;
}

inline unsigned arrayPaletteMaximum(std::string_view type) {
    return arrayPaletteKind(type)==ArrayPaletteKind::Initialize ? 16U:32U;
}
inline unsigned arrayPaletteCount(std::string_view type,unsigned terminals) {
    const auto k=arrayPaletteKind(type);
    const unsigned legacy=k==ArrayPaletteKind::Interleave || k==ArrayPaletteKind::Decimate || k==ArrayPaletteKind::BuildMatrix ? 2U:1U;
    return std::clamp(terminals ? terminals:legacy,1U,arrayPaletteMaximum(type));
}

// Fixed Top and one dimension band; shared metadata, independent of the editor.
struct InitializeArrayVisualLayout {
    static constexpr double width = 48;
    static constexpr double scale = 42.0 / 46;
    static constexpr double left = 3 - .9 * scale;
    static constexpr double topHeight = 33.11 * scale;
    static constexpr double chunkHeight = 11.14 * scale;
    static constexpr double elementY = 17.18 * scale;
    static constexpr double resultY = elementY + chunkHeight;
    unsigned count;
    explicit InitializeArrayVisualLayout(unsigned dimensions) : count(std::clamp(dimensions,1U,16U)) {}
    explicit InitializeArrayVisualLayout(const DiagramNode &node);
    double height() const noexcept { return topHeight+count*chunkHeight; }
    static double chunkTop(unsigned dimension) noexcept { return topHeight+dimension*chunkHeight; }
    static double sizeY(unsigned dimension) noexcept { return chunkTop(dimension)+5.82*scale; }
};

// NI connector pixel centres on the 32-pixel Matrix body, uniformly mapped
// to the existing GS width. Repeated groups retain the GS extension pitch.
inline constexpr double kMatrixConnectorScale = 47.8 / 32.0;

struct BuildMatrixVisualLayout {
    static constexpr double width = 47.8;
    static constexpr double topHeight = 21.25;
    static constexpr double middleHeight = 11.14;
    static constexpr double bottomHeight = 14.28;
    static constexpr double firstInputY = 4 * kMatrixConnectorScale;
    static constexpr double middleInputY = 5.65;
    static constexpr double lastInputY = 5.41;
    static constexpr double resultY = 13.01; // compact singleton, not shown by NI
    static double outputY(unsigned count) noexcept { return count==1 ? resultY : 16*kMatrixConnectorScale; }
    static double height(unsigned count) noexcept {
        count = std::clamp(count, 1U, 32U);
        return count == 1 ? topHeight : topHeight + (count - 2) * middleHeight + bottomHeight;
    }
    static double inputY(unsigned count, unsigned index) noexcept {
        // NI does not show the compact singleton. Preserve its known layout.
        if (count == 1) return 13.01;
        return index == 0 ? firstInputY : 12 * kMatrixConnectorScale + (index-1)*middleHeight;
    }
};

struct GetMatrixDiagonalVisualLayout {
    static constexpr double width = 47.8;
    static constexpr double topHeight = 26.56;
    static constexpr double middleHeight = 19.91;
    static constexpr double bottomHeight = 25.47;
    static constexpr double matrixY = 8 * kMatrixConnectorScale;
    static double height(unsigned count) noexcept {
        return topHeight + (std::clamp(count, 1U, 32U) - 1) * middleHeight + bottomHeight;
    }
    static double groupTop(unsigned group) noexcept { return topHeight + group * middleHeight; }
    static double diagonalY(unsigned /*count*/, unsigned group) noexcept {
        return 20 * kMatrixConnectorScale + group * middleHeight;
    }
    static double columnY(unsigned /*count*/, unsigned group) noexcept {
        return 27 * kMatrixConnectorScale + group * middleHeight;
    }
    static double outputY(unsigned /*count*/, unsigned group) noexcept {
        return 21 * kMatrixConnectorScale + group * middleHeight;
    }
};

struct GetMatrixElementsVisualLayout : GetMatrixDiagonalVisualLayout {
    static double rowY(unsigned /*count*/, unsigned group) noexcept {
        return 20 * kMatrixConnectorScale + group * middleHeight;
    }
    static double columnY(unsigned /*count*/, unsigned group) noexcept {
        return 27 * kMatrixConnectorScale + group * middleHeight;
    }
    static double outputY(unsigned /*count*/, unsigned group) noexcept {
        return 21 * kMatrixConnectorScale + group * middleHeight;
    }
};

struct GetMatrixSubsetVisualLayout {
    static constexpr double width = 47.8;
    static constexpr double topHeight = 24.7;
    static constexpr double middleHeight = 44.76;
    static constexpr double bottomHeight = 43.82;
    static constexpr double matrixY = 9 * kMatrixConnectorScale;
    static double height(unsigned count) noexcept {
        return topHeight + (std::clamp(count, 1U, 32U) - 1) * middleHeight + bottomHeight;
    }
    static double groupTop(unsigned group) noexcept { return topHeight + group * middleHeight; }
    static double rowY(unsigned /*count*/, unsigned group) noexcept {
        return 21 * kMatrixConnectorScale + group * middleHeight;
    }
    static double columnY(unsigned /*count*/, unsigned group) noexcept {
        return 33 * kMatrixConnectorScale + group * middleHeight;
    }
    static double rowsY(unsigned /*count*/, unsigned group) noexcept {
        return 27 * kMatrixConnectorScale + group * middleHeight;
    }
    static double columnsY(unsigned /*count*/, unsigned group) noexcept {
        return 39 * kMatrixConnectorScale + group * middleHeight;
    }
    static double resultY(unsigned /*count*/, unsigned group) noexcept {
        return 16 * kMatrixConnectorScale + group * middleHeight;
    }
};

struct SetMatrixDiagonalVisualLayout {
    static constexpr double width = 47.8;
    static constexpr double topHeight = 28.73;
    static constexpr double middleHeight = 27.71;
    static constexpr double bottomHeight = 30.18;
    static constexpr double matrixY = 8 * kMatrixConnectorScale;
    static double height(unsigned count) noexcept {
        return topHeight + (std::clamp(count, 1U, 32U) - 1) * middleHeight + bottomHeight;
    }
    static double groupTop(unsigned group) noexcept { return topHeight + group * middleHeight; }
    static double rowY(unsigned /*count*/, unsigned group) noexcept {
        return 19 * kMatrixConnectorScale + group * middleHeight;
    }
    static double columnY(unsigned /*count*/, unsigned group) noexcept {
        return 25 * kMatrixConnectorScale + group * middleHeight;
    }
    static double valuesY(unsigned /*count*/, unsigned group) noexcept {
        return 31 * kMatrixConnectorScale + group * middleHeight;
    }
    static double resultY(unsigned /*count*/, unsigned group) noexcept {
        return 16 * kMatrixConnectorScale + group * middleHeight;
    }
};

struct SetMatrixElementsVisualLayout : SetMatrixDiagonalVisualLayout {
    static constexpr double topHeight = 27.33;
    static constexpr double matrixY = 8 * kMatrixConnectorScale;
    static double height(unsigned count) noexcept {
        return topHeight + (std::clamp(count, 1U, 32U) - 1) * middleHeight + bottomHeight;
    }
    static double groupTop(unsigned group) noexcept { return topHeight + group * middleHeight; }
    static double rowY(unsigned /*count*/, unsigned group) noexcept {
        return 19 * kMatrixConnectorScale + group * middleHeight;
    }
    static double columnY(unsigned /*count*/, unsigned group) noexcept {
        return 25 * kMatrixConnectorScale + group * middleHeight;
    }
    static double valuesY(unsigned /*count*/, unsigned group) noexcept {
        return 31 * kMatrixConnectorScale + group * middleHeight;
    }
    static double resultY(unsigned /*count*/, unsigned group) noexcept {
        return 16 * kMatrixConnectorScale + group * middleHeight;
    }
};

struct SetMatrixSubsetVisualLayout {
    static constexpr double width = 47.8;
    static constexpr double topHeight = 27.33;
    static constexpr double middleHeight = 52.88;
    static constexpr double bottomHeight = 50.87;
    static constexpr double matrixY = 6 * kMatrixConnectorScale;
    static double height(unsigned count) noexcept {
        return topHeight + (std::clamp(count, 1U, 32U) - 1) * middleHeight + bottomHeight;
    }
    static double groupTop(unsigned group) noexcept { return topHeight + group * middleHeight; }
    static double rowY(unsigned /*count*/, unsigned group) noexcept {
        return 16 * kMatrixConnectorScale + group * middleHeight;
    }
    static double columnY(unsigned /*count*/, unsigned group) noexcept {
        return 28 * kMatrixConnectorScale + group * middleHeight;
    }
    static double rowsY(unsigned /*count*/, unsigned group) noexcept {
        return 22 * kMatrixConnectorScale + group * middleHeight;
    }
    static double columnsY(unsigned /*count*/, unsigned group) noexcept {
        return 34 * kMatrixConnectorScale + group * middleHeight;
    }
    static double subsetY(unsigned /*count*/, unsigned group) noexcept {
        return 40 * kMatrixConnectorScale + group * middleHeight;
    }
    static double resultY(unsigned /*count*/, unsigned group) noexcept {
        return 16 * kMatrixConnectorScale + group * middleHeight;
    }
};

struct ArrayPalettePort { std::string id; std::string_view base; bool output; unsigned group; double y; };

inline unsigned arrayPaletteGroupRows(ArrayPaletteKind k) {
    switch(k) {
    case ArrayPaletteKind::GetDiagonal: case ArrayPaletteKind::GetElements: return 2;
    case ArrayPaletteKind::GetSubset: return 4;
    case ArrayPaletteKind::SetDiagonal: case ArrayPaletteKind::SetElements: return 3;
    case ArrayPaletteKind::SetSubset: return 5;
    default:return 1;
    }
}

inline bool arrayPaletteGrouped(ArrayPaletteKind k) { return k>=ArrayPaletteKind::GetDiagonal; }

inline bool arrayPaletteStrip(ArrayPaletteKind k) { return k==ArrayPaletteKind::Interleave || k==ArrayPaletteKind::Decimate; }

inline double arrayPaletteStripCell(ArrayPaletteKind k) { return k==ArrayPaletteKind::Interleave ? 6.26 : 5.24; }

inline double arrayPaletteStripRowY(ArrayPaletteKind k,unsigned row) {
    return (k==ArrayPaletteKind::Interleave ? 5.79 : 5.57)+2*arrayPaletteStripCell(k)*row;
}

inline std::string arrayPaletteGroupId(std::string_view base,unsigned group) {
    return std::string(base)+(group ? ":"+std::to_string(group):"");
}

inline std::optional<std::pair<std::string_view,unsigned>> arrayPaletteSplitPort(std::string_view id) {
    const auto colon=id.find(':');
    if(colon==std::string_view::npos) return std::pair{id,0U};
    const auto suffix=id.substr(colon+1); unsigned group=0;
    if(suffix.empty() || suffix.front()=='0') return {};
    const auto parsed=std::from_chars(suffix.data(),suffix.data()+suffix.size(),group);
    if(parsed.ec!=std::errc{} || parsed.ptr!=suffix.data()+suffix.size() || group>31) return {};
    return std::pair{id.substr(0,colon),group};
}

inline double arrayPaletteWidth(std::string_view type) {
    const auto kind=arrayPaletteKind(type);
    return kind==ArrayPaletteKind::Initialize ? InitializeArrayVisualLayout::width :
        kind==ArrayPaletteKind::BuildMatrix ? BuildMatrixVisualLayout::width :
        (kind==ArrayPaletteKind::GetDiagonal || kind==ArrayPaletteKind::GetElements) ? GetMatrixDiagonalVisualLayout::width :
        kind==ArrayPaletteKind::GetSubset ? GetMatrixSubsetVisualLayout::width :
        kind==ArrayPaletteKind::SetDiagonal ? SetMatrixDiagonalVisualLayout::width :
        kind==ArrayPaletteKind::SetElements ? SetMatrixElementsVisualLayout::width :
        kind==ArrayPaletteKind::SetSubset ? SetMatrixSubsetVisualLayout::width :
        arrayPaletteStrip(kind) ? 47.33 : 64.0;
}

inline double arrayPalettePitch(std::string_view type) {
    const auto k=arrayPaletteKind(type);
    return k==ArrayPaletteKind::Initialize ? InitializeArrayVisualLayout::chunkHeight :
        k==ArrayPaletteKind::BuildMatrix ? BuildMatrixVisualLayout::middleHeight :
        (k==ArrayPaletteKind::GetDiagonal || k==ArrayPaletteKind::GetElements) ? GetMatrixDiagonalVisualLayout::middleHeight :
        k==ArrayPaletteKind::GetSubset ? GetMatrixSubsetVisualLayout::middleHeight :
        k==ArrayPaletteKind::SetDiagonal ? SetMatrixDiagonalVisualLayout::middleHeight :
        k==ArrayPaletteKind::SetElements ? SetMatrixElementsVisualLayout::middleHeight :
        k==ArrayPaletteKind::SetSubset ? SetMatrixSubsetVisualLayout::middleHeight :
        arrayPaletteStrip(k) ? 2*arrayPaletteStripCell(k) : 18.0*arrayPaletteGroupRows(k);
}

inline double arrayPaletteHeight(std::string_view type,unsigned terminals) {
    const auto k=arrayPaletteKind(type);
    if(k==ArrayPaletteKind::BuildMatrix) return BuildMatrixVisualLayout::height(arrayPaletteCount(type,terminals));
    if(k==ArrayPaletteKind::GetDiagonal || k==ArrayPaletteKind::GetElements)
        return GetMatrixDiagonalVisualLayout::height(arrayPaletteCount(type,terminals));
    if(k==ArrayPaletteKind::GetSubset)
        return GetMatrixSubsetVisualLayout::height(arrayPaletteCount(type,terminals));
    if(k==ArrayPaletteKind::SetDiagonal)
        return SetMatrixDiagonalVisualLayout::height(arrayPaletteCount(type,terminals));
    if(k==ArrayPaletteKind::SetElements)
        return SetMatrixElementsVisualLayout::height(arrayPaletteCount(type,terminals));
    if(k==ArrayPaletteKind::SetSubset)
        return SetMatrixSubsetVisualLayout::height(arrayPaletteCount(type,terminals));
    if(arrayPaletteStrip(k)) return 11.14+arrayPalettePitch(type)*(arrayPaletteCount(type,terminals)-1);
    if(arrayPaletteGrouped(k)) return 30.0+arrayPalettePitch(type)*arrayPaletteCount(type,terminals);
    if(k==ArrayPaletteKind::Initialize) return InitializeArrayVisualLayout(arrayPaletteCount(type,terminals)).height();
    return (std::max)(48.0,12.0+18.0*arrayPaletteCount(type,terminals));
}

inline std::vector<ArrayPalettePort> arrayPalettePorts(std::string_view type,unsigned terminals) {
    const auto k=arrayPaletteKind(type); const auto count=arrayPaletteCount(type,terminals);
    std::vector<ArrayPalettePort> ports;
    const auto add=[&](std::string_view base,bool output,unsigned group,double y,std::string id={}) {
        ports.push_back({id.empty()?arrayPaletteGroupId(base,group):std::move(id),base,output,group,y});
    };
    if(k==ArrayPaletteKind::None) return ports;
    if(k==ArrayPaletteKind::Initialize) {
        add("element",false,0,InitializeArrayVisualLayout::elementY); add("result",true,0,InitializeArrayVisualLayout::resultY);
        for(unsigned i=0;i<count;++i) add("size",false,i,InitializeArrayVisualLayout::sizeY(i),i?"input_"+std::to_string(i+2):"size");
    } else if(k==ArrayPaletteKind::BuildMatrix) {
        for(unsigned i=0;i<count;++i)
            add("element1",false,i,BuildMatrixVisualLayout::inputY(count,i),
                i==0 ? "element1" : i==1 ? "element2" : "input_"+std::to_string(i+1));
        add("result",true,0,BuildMatrixVisualLayout::outputY(count));
    } else if(k==ArrayPaletteKind::Interleave) {
        for(unsigned i=0;i<count;++i)
            add("array_a",false,i,arrayPaletteStripRowY(k,i),
                i==0 ? "array_a" : i==1 ? "array_b" : "input_"+std::to_string(i+1));
        add("result",true,0,arrayPaletteHeight(type,terminals)/2);
    } else if(k==ArrayPaletteKind::Decimate) {
        add("array",false,0,arrayPaletteHeight(type,terminals)/2);
        for(unsigned i=0;i<count;++i) add("first",true,i,arrayPaletteStripRowY(k,i),
            i==0?"first":i==1?"second":"output_"+std::to_string(i+1));
    } else if(k==ArrayPaletteKind::GetDiagonal) {
        using Layout = GetMatrixDiagonalVisualLayout;
        add("matrix",false,0,Layout::matrixY);
        for(unsigned g=0;g<count;++g) {
            add("diagonal",false,g,Layout::diagonalY(count,g));
            add("column",false,g,Layout::columnY(count,g));
            add("diagonal_values",true,g,Layout::outputY(count,g));
        }
    } else if(k==ArrayPaletteKind::GetElements) {
        using Layout = GetMatrixElementsVisualLayout;
        add("matrix",false,0,Layout::matrixY);
        for(unsigned g=0;g<count;++g) {
            add("row_indices",false,g,Layout::rowY(count,g));
            add("column_indices",false,g,Layout::columnY(count,g));
            add("elements",true,g,Layout::outputY(count,g));
        }
    } else if(k==ArrayPaletteKind::GetSubset) {
        using Layout = GetMatrixSubsetVisualLayout;
        add("matrix",false,0,Layout::matrixY);
        for(unsigned g=0;g<count;++g) {
            add("row",false,g,Layout::rowY(count,g));
            add("column",false,g,Layout::columnY(count,g));
            add("rows",false,g,Layout::rowsY(count,g));
            add("columns",false,g,Layout::columnsY(count,g));
            add("result",true,g,Layout::resultY(count,g));
        }
    } else if(k==ArrayPaletteKind::SetDiagonal) {
        using Layout = SetMatrixDiagonalVisualLayout;
        add("matrix",false,0,Layout::matrixY);
        for(unsigned g=0;g<count;++g) {
            add("diagonal",false,g,Layout::rowY(count,g));
            add("column",false,g,Layout::columnY(count,g));
            add("diagonal_values",false,g,Layout::valuesY(count,g));
            add("result",true,g,Layout::resultY(count,g));
        }
    } else if(k==ArrayPaletteKind::SetElements) {
        using Layout = SetMatrixElementsVisualLayout;
        add("matrix",false,0,Layout::matrixY);
        for(unsigned g=0;g<count;++g) {
            add("row_indices",false,g,Layout::rowY(count,g));
            add("column_indices",false,g,Layout::columnY(count,g));
            add("elements",false,g,Layout::valuesY(count,g));
            add("result",true,g,Layout::resultY(count,g));
        }
    } else if(k==ArrayPaletteKind::SetSubset) {
        using Layout = SetMatrixSubsetVisualLayout;
        add("matrix",false,0,Layout::matrixY);
        for(unsigned g=0;g<count;++g) {
            add("row",false,g,Layout::rowY(count,g));
            add("column",false,g,Layout::columnY(count,g));
            add("rows",false,g,Layout::rowsY(count,g));
            add("columns",false,g,Layout::columnsY(count,g));
            add("subset",false,g,Layout::subsetY(count,g));
            add("result",true,g,Layout::resultY(count,g));
        }
    } else {
        add("matrix",false,0,15);
        for(unsigned g=0;g<count;++g) {
            const double y=33+arrayPalettePitch(type)*g;
            const bool diagonal=k==ArrayPaletteKind::GetDiagonal || k==ArrayPaletteKind::SetDiagonal;
            const bool elements=k==ArrayPaletteKind::GetElements || k==ArrayPaletteKind::SetElements;
            add(diagonal?"diagonal":elements?"row_indices":"row",false,g,y);
            add(elements?"column_indices":"column",false,g,y+18);
            if(k==ArrayPaletteKind::SetDiagonal) add("diagonal_values",false,g,y+36);
            if(k==ArrayPaletteKind::SetElements) add("elements",false,g,y+36);
            if(k==ArrayPaletteKind::SetSubset) {
                add("rows",false,g,y+36); add("columns",false,g,y+54); add("subset",false,g,y+72);
            }
            add(k==ArrayPaletteKind::GetDiagonal?"diagonal_values":k==ArrayPaletteKind::GetElements?"elements":"result",true,g,y);
        }
    }
    return ports;
}

inline std::optional<ArrayPalettePort> arrayPaletteDeclaredPort(std::string_view type,std::string_view id) {
    const auto k=arrayPaletteKind(type); if(k==ArrayPaletteKind::None) return {};
    const auto make=[&](std::string_view base,bool output,unsigned group=0,double y=15) {
        return std::optional<ArrayPalettePort>{{std::string(id),base,output,group,y}};
    };
    const auto ordinal=[&](std::string_view prefix,unsigned limit)->std::optional<unsigned> {
        if(!id.starts_with(prefix)) return {};
        const auto suffix=id.substr(prefix.size()); unsigned n=0;
        if(suffix.empty() || suffix.front()=='0') return {};
        const auto p=std::from_chars(suffix.data(),suffix.data()+suffix.size(),n);
        if(p.ec!=std::errc{} || p.ptr!=suffix.data()+suffix.size() || n<3 || n>limit) return {};
        return n;
    };
    if(k==ArrayPaletteKind::Initialize) {
        if(id=="element" || id=="result") return make(id,id=="result",0,
            id=="element"?InitializeArrayVisualLayout::elementY:InitializeArrayVisualLayout::resultY);
        if(id=="size") return make("size",false,0,InitializeArrayVisualLayout::sizeY(0));
        if(const auto n=ordinal("input_",17)) return make("size",false,*n-2,InitializeArrayVisualLayout::sizeY(*n-2));
        return {};
    }
    if(k==ArrayPaletteKind::BuildMatrix) {
        if(id=="result") return make("result",true,0,BuildMatrixVisualLayout::resultY);
        if(id=="element1") return make("element1",false,0,BuildMatrixVisualLayout::firstInputY);
        if(id=="element2") return make("element1",false,1,BuildMatrixVisualLayout::inputY(2,1));
        if(const auto n=ordinal("input_",32)) return make("element1",false,*n-1,
            BuildMatrixVisualLayout::topHeight+(*n-2)*BuildMatrixVisualLayout::middleHeight+
                BuildMatrixVisualLayout::lastInputY);
        return {};
    }
    if(k==ArrayPaletteKind::Interleave) {
        if(id=="result") return make("result",true);
        if(id=="array_a") return make("array_a",false);
        if(id=="array_b") return make("array_a",false,1,33);
        if(const auto n=ordinal("input_",32)) return make("array_a",false,*n-1,15+18*(*n-1));
        return {};
    }
    if(k==ArrayPaletteKind::Decimate) {
        if(id=="array") return make("array",false);
        if(id=="first") return make("first",true);
        if(id=="second") return make("first",true,1,33);
        if(const auto n=ordinal("output_",32)) return make("first",true,*n-1,15+18*(*n-1));
        return {};
    }
    if(k==ArrayPaletteKind::GetDiagonal) {
        using Layout = GetMatrixDiagonalVisualLayout;
        if(id=="matrix") return make("matrix",false,0,Layout::matrixY);
        const auto split=arrayPaletteSplitPort(id); if(!split) return {};
        const auto [base,g]=*split;
        if(base=="diagonal") return make(base,false,g,Layout::diagonalY(g+1,g));
        if(base=="column") return make(base,false,g,Layout::columnY(g+1,g));
        if(base=="diagonal_values") return make(base,true,g,Layout::outputY(g+1,g));
        return {};
    }
    if(k==ArrayPaletteKind::GetElements) {
        using Layout = GetMatrixElementsVisualLayout;
        if(id=="matrix") return make("matrix",false,0,Layout::matrixY);
        const auto split=arrayPaletteSplitPort(id); if(!split) return {};
        const auto [base,g]=*split;
        if(base=="row_indices") return make(base,false,g,Layout::rowY(g+1,g));
        if(base=="column_indices") return make(base,false,g,Layout::columnY(g+1,g));
        if(base=="elements") return make(base,true,g,Layout::outputY(g+1,g));
        return {};
    }
    if(k==ArrayPaletteKind::GetSubset) {
        using Layout = GetMatrixSubsetVisualLayout;
        if(id=="matrix") return make("matrix",false,0,Layout::matrixY);
        const auto split=arrayPaletteSplitPort(id); if(!split) return {};
        const auto [base,g]=*split;
        if(base=="row") return make(base,false,g,Layout::rowY(g+1,g));
        if(base=="column") return make(base,false,g,Layout::columnY(g+1,g));
        if(base=="rows") return make(base,false,g,Layout::rowsY(g+1,g));
        if(base=="columns") return make(base,false,g,Layout::columnsY(g+1,g));
        if(base=="result") return make(base,true,g,Layout::resultY(g+1,g));
        return {};
    }
    if(k==ArrayPaletteKind::SetDiagonal) {
        using Layout = SetMatrixDiagonalVisualLayout;
        if(id=="matrix") return make("matrix",false,0,Layout::matrixY);
        const auto split=arrayPaletteSplitPort(id); if(!split) return {};
        const auto [base,g]=*split;
        if(base=="diagonal") return make(base,false,g,Layout::rowY(g+1,g));
        if(base=="column") return make(base,false,g,Layout::columnY(g+1,g));
        if(base=="diagonal_values") return make(base,false,g,Layout::valuesY(g+1,g));
        if(base=="result") return make(base,true,g,Layout::resultY(g+1,g));
        return {};
    }
    if(k==ArrayPaletteKind::SetElements) {
        using Layout = SetMatrixElementsVisualLayout;
        if(id=="matrix") return make("matrix",false,0,Layout::matrixY);
        const auto split=arrayPaletteSplitPort(id); if(!split) return {};
        const auto [base,g]=*split;
        if(base=="row_indices") return make(base,false,g,Layout::rowY(g+1,g));
        if(base=="column_indices") return make(base,false,g,Layout::columnY(g+1,g));
        if(base=="elements") return make(base,false,g,Layout::valuesY(g+1,g));
        if(base=="result") return make(base,true,g,Layout::resultY(g+1,g));
        return {};
    }
    if(k==ArrayPaletteKind::SetSubset) {
        using Layout = SetMatrixSubsetVisualLayout;
        if(id=="matrix") return make("matrix",false,0,Layout::matrixY);
        const auto split=arrayPaletteSplitPort(id); if(!split) return {};
        const auto [base,g]=*split;
        if(base=="row") return make(base,false,g,Layout::rowY(g+1,g));
        if(base=="column") return make(base,false,g,Layout::columnY(g+1,g));
        if(base=="rows") return make(base,false,g,Layout::rowsY(g+1,g));
        if(base=="columns") return make(base,false,g,Layout::columnsY(g+1,g));
        if(base=="subset") return make(base,false,g,Layout::subsetY(g+1,g));
        if(base=="result") return make(base,true,g,Layout::resultY(g+1,g));
        return {};
    }
    if(id=="matrix") return make("matrix",false);
    const auto split=arrayPaletteSplitPort(id); if(!split) return {};
    const auto [base,g]=*split; const double y=33+18*arrayPaletteGroupRows(k)*g;
    const bool diagonal=k==ArrayPaletteKind::GetDiagonal || k==ArrayPaletteKind::SetDiagonal;
    const bool elements=k==ArrayPaletteKind::GetElements || k==ArrayPaletteKind::SetElements;
    if(base==(diagonal?"diagonal":elements?"row_indices":"row")) return make(base,false,g,y);
    if(base==(elements?"column_indices":"column")) return make(base,false,g,y+18);
    if(base==(k==ArrayPaletteKind::GetDiagonal?"diagonal_values":k==ArrayPaletteKind::GetElements?"elements":"result")) return make(base,true,g,y);
    if(k==ArrayPaletteKind::SetDiagonal && base=="diagonal_values") return make(base,false,g,y+36);
    if(k==ArrayPaletteKind::SetElements && base=="elements") return make(base,false,g,y+36);
    if(k==ArrayPaletteKind::SetSubset) {
        if(base=="rows") return make(base,false,g,y+36);
        if(base=="columns") return make(base,false,g,y+54);
        if(base=="subset") return make(base,false,g,y+72);
    }
    return {};
}

inline std::string_view stringPaletteId(std::string_view id) { if(id.starts_with("frog.studio.")) id.remove_prefix(12); return id; }

inline bool isPathPaletteFunction(std::string_view id) {
    id=stringPaletteId(id);
    return id.starts_with("path.") && !id.ends_with("constant") && !id.ends_with("terminal");
}

inline bool isStringFormat(std::string_view id) { return stringPaletteId(id)=="string.conversion.format_into_string"; }

inline bool isStringScan(std::string_view id) { return stringPaletteId(id)=="string.conversion.scan_from_string"; }

inline std::string_view stringPaletteRepeatedBase(std::string_view id,std::string_view port) {
    for(const auto base:{std::string_view("submatch"),std::string_view("value"),std::string_view("default_value"),std::string_view("result")}) {
        const bool allowed=(stringPaletteId(id)=="string.match_regular_expression" && base=="submatch") ||
            (isStringFormat(id) && base=="value") || (isStringScan(id) && (base=="default_value" || base=="result"));
        if(allowed && (port==base || (port.starts_with(base) && port.size()>base.size() && port[base.size()]=='_'))) return base;
    }
    return {};
}

inline std::optional<unsigned> stringPaletteIndex(std::string_view id,std::string_view port) {
    const auto base=stringPaletteRepeatedBase(id,port); if(base.empty()) return std::nullopt;
    if(port==base) return 0;
    port.remove_prefix(base.size()+1); unsigned n=0; const auto parsed=std::from_chars(port.data(),port.data()+port.size(),n);
    if(port.empty() || port.front()=='0' || parsed.ec!=std::errc{} || parsed.ptr!=port.data()+port.size() || n<2 || n>32) return std::nullopt;
    return n-1;
}

inline std::string_view clusterFunctionId(std::string_view id) {
    if (id.starts_with("frog.studio.")) id.remove_prefix(12);
    return id;
}

inline bool isClusterByName(std::string_view id) {
    id=clusterFunctionId(id);
    return id=="cluster.bundle_by_name" || id=="cluster.unbundle_by_name";
}

inline bool isClusterUnbundleByName(std::string_view id) {
    return clusterFunctionId(id)=="cluster.unbundle_by_name";
}

inline bool isClusterBundleByName(std::string_view id) {
    return clusterFunctionId(id)=="cluster.bundle_by_name";
}

inline bool isClusterBundleFunction(std::string_view id) {
    id=clusterFunctionId(id);
    return id=="cluster.bundle" || id=="cluster.bundle_by_name";
}

inline bool isClusterUnbundleFunction(std::string_view id) {
    id=clusterFunctionId(id);
    return id=="cluster.unbundle" || id=="cluster.unbundle_by_name";
}

inline bool isClusterRecordFunction(std::string_view id) {
    return isClusterBundleFunction(id) || isClusterUnbundleFunction(id);
}

inline bool isClusterBuildArray(std::string_view id) {
    return clusterFunctionId(id)=="cluster.build_array";
}

inline bool isClusterArrayStrip(std::string_view id) {
    return isClusterBuildArray(id) || clusterFunctionId(id)=="cluster.index_build_array";
}

inline constexpr double kClusterArrayStripWidth=47.33;

inline constexpr double kClusterArrayStripRowHeight=11.14;

inline bool isClusterFunction(std::string_view id) {
    id=clusterFunctionId(id);
    return isClusterRecordFunction(id) || id=="cluster.build_array" || id=="cluster.index_build_array";
}

inline std::optional<unsigned> clusterFunctionRow(std::string_view port) {
    if (!port.starts_with("element")) return {};
    port.remove_prefix(7);
    unsigned n=0;
    const auto parsed=std::from_chars(port.data(),port.data()+port.size(),n);
    if(port.empty() || port.front()=='0' || parsed.ec!=std::errc{} || parsed.ptr!=port.data()+port.size() || n<1 || n>32) return {};
    return n-1;
}

inline std::string clusterFunctionRowId(unsigned row) { return "element"+std::to_string(row+1); }

struct ClusterFunctionPort { std::string id; bool output; double y; bool top=false; };

inline std::string_view clusterFieldName(const binding::TypeRef &type,std::size_t index) {
    return index<type.fieldNames.size() && !type.fieldNames[index].empty() ? type.fieldNames[index] : type.fieldIds[index];
}

inline std::optional<std::size_t> clusterNamedField(const binding::TypeRef &type,std::string_view name) {
    if(type.kind!=binding::TypeKind::Cluster || name.empty()) return {};
    std::optional<std::size_t> found;
    for(std::size_t i=0;i<type.fieldIds.size();++i) if(clusterFieldName(type,i)==name) {
        if(found) return {};
        found=i;
    }
    return found;
}

inline const binding::TypeRef *clusterNamedFieldPath(const binding::TypeRef &type,std::string_view name) {
    if(type.kind!=binding::TypeKind::Cluster || name.empty()) return nullptr;
    const binding::TypeRef *found=nullptr;
    bool ambiguous=false;
    const auto visit=[&](const auto &self,const binding::TypeRef &parent,const std::string &prefix)->void {
        for(std::size_t i=0;i<parent.fieldIds.size() && i<parent.children.size();++i) {
            auto path=prefix;
            if(!path.empty()) path+='.';
            path+=clusterFieldName(parent,i);
            if(path==name) { if(found) ambiguous=true; else found=&parent.children[i]; }
            if(parent.children[i].kind==binding::TypeKind::Cluster) self(self,parent.children[i],path);
        }
    };
    visit(visit,type,{});
    return ambiguous?nullptr:found;
}

inline std::vector<std::string> clusterNamedFieldPaths(const binding::TypeRef &type) {
    std::vector<std::string> paths;
    if(type.kind!=binding::TypeKind::Cluster) return paths;
    const auto visit=[&](const auto &self,const binding::TypeRef &parent,const std::string &prefix)->void {
        for(std::size_t i=0;i<parent.fieldIds.size() && i<parent.children.size();++i) {
            auto path=prefix;
            if(!path.empty()) path+='.';
            path+=clusterFieldName(parent,i);
            paths.push_back(path);
            if(parent.children[i].kind==binding::TypeKind::Cluster) self(self,parent.children[i],path);
        }
    };
    visit(visit,type,{});
    return paths;
}

inline std::vector<std::string> clusterNamedFieldIdPath(const binding::TypeRef &type,std::string_view name) {
    if(!clusterNamedFieldPath(type,name)) return {};
    std::vector<std::string> found;
    const auto visit=[&](const auto &self,const binding::TypeRef &parent,const std::string &prefix,
        const std::vector<std::string> &ids)->void {
        for(std::size_t i=0;i<parent.fieldIds.size() && i<parent.children.size();++i) {
            auto path=prefix;
            if(!path.empty()) path+='.';
            path+=clusterFieldName(parent,i);
            auto next=ids;next.push_back(parent.fieldIds[i]);
            if(path==name) found=next;
            if(parent.children[i].kind==binding::TypeKind::Cluster)
                self(self,parent.children[i],path,next);
        }
    };
    visit(visit,type,{},{});
    return found;
}

inline const binding::TypeRef *clusterFieldByIdPath(const binding::TypeRef &type,
    const std::vector<std::string> &ids, std::string *currentName = nullptr) {
    if(type.kind!=binding::TypeKind::Cluster || ids.empty()) return nullptr;
    const auto *parent=&type;
    std::string name;
    for(const auto &id:ids) {
        if(parent->kind!=binding::TypeKind::Cluster) return nullptr;
        const auto found=std::find(parent->fieldIds.begin(),parent->fieldIds.end(),id);
        if(found==parent->fieldIds.end()) return nullptr;
        const auto index=static_cast<std::size_t>(found-parent->fieldIds.begin());
        if(index>=parent->children.size()) return nullptr;
        if(!name.empty()) name+='.';
        name+=clusterFieldName(*parent,index);
        parent=&parent->children[index];
    }
    if(currentName) *currentName=std::move(name);
    return parent;
}

struct ClusterFieldChoice {
    std::string name;
    std::vector<std::string> ids;
    std::vector<std::size_t> indices;
};

inline std::vector<ClusterFieldChoice> clusterFieldChoices(const binding::TypeRef &type) {
    struct Frame { const binding::TypeRef *parent; std::size_t next; ClusterFieldChoice prefix; };
    std::vector<ClusterFieldChoice> choices;
    if(type.kind!=binding::TypeKind::Cluster)return choices;
    std::vector<Frame> pending{{&type,0,{}}};
    while(!pending.empty()) {
        auto &frame=pending.back();
        if(frame.next>=frame.parent->fieldIds.size() || frame.next>=frame.parent->children.size()) {
            pending.pop_back();continue;
        }
        const auto index=frame.next++;
        auto choice=frame.prefix;
        if(!choice.name.empty())choice.name+='.';
        choice.name+=clusterFieldName(*frame.parent,index);
        choice.ids.push_back(frame.parent->fieldIds[index]);choice.indices.push_back(index);
        const auto *child=&frame.parent->children[index];
        choices.push_back(choice);
        if(child->kind==binding::TypeKind::Cluster)pending.push_back({child,0,std::move(choice)});
    }
    return choices;
}

inline std::string clusterFieldChoiceLabel(const ClusterFieldChoice &choice) {
    std::string index;
    for(const auto ordinal:choice.indices) {if(!index.empty())index+='.';index+=std::to_string(ordinal);}
    return "["+index+"] "+choice.name;
}
}
