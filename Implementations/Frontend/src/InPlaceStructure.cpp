#include "frog_native_engine/InPlaceStructure.hpp"
#include "frog_native_engine/ClusterFunctionBinding.hpp"
#include <algorithm>
#include <charconv>
#include <cmath>
#include <limits>
#include <unordered_set>

namespace frog::native_engine {
namespace {
void seedSerial(std::uint64_t &next,std::string_view id,std::string_view prefix) {
    if(!id.starts_with(prefix))return;
    id.remove_prefix(prefix.size());if(id.empty())return;
    std::uint64_t serial=0;const auto parsed=std::from_chars(id.data(),id.data()+id.size(),serial);
    if(parsed.ec==std::errc{} && parsed.ptr==id.data()+id.size())
        next=(std::max)(next,serial==std::numeric_limits<std::uint64_t>::max()?serial:serial+1);
}
}
void seedInPlaceIdentityCounters(DiagramNode &n,std::span<const DiagramEdge> edges,
    std::span<const DiagramWireFragment> fragments) {
    for(auto &pair:n.inPlacePairs) {
        seedSerial(n.inPlaceNextPairSerial,pair.id,"pair_");
        for(const auto &access:pair.accesses)seedSerial(pair.nextAccessSerial,access.id,"element_");
    }
    const auto endpoint=[&](const DiagramEdgeEndpoint &e) {
        if(e.nodeId!=n.id || !e.portId.starts_with("ipe:"))return;
        auto part=std::string_view(e.portId).substr(4);
        const auto end=part.find(':');if(end==std::string_view::npos)return;
        const auto pairId=part.substr(0,end);seedSerial(n.inPlaceNextPairSerial,pairId,"pair_");
        const auto found=std::find_if(n.inPlacePairs.begin(),n.inPlacePairs.end(),[&](const auto &p){return p.id==pairId;});
        if(found==n.inPlacePairs.end())return;
        part.remove_prefix(end+1);const auto accessAt=part.find(':');if(accessAt==std::string_view::npos)return;
        part.remove_prefix(accessAt+1);const auto dimensionAt=part.find(':');
        seedSerial(found->nextAccessSerial,part.substr(0,dimensionAt),"element_");
    };
    for(const auto &edge:edges){endpoint(edge.from);endpoint(edge.to);}
    for(const auto &fragment:fragments){if(fragment.source)endpoint(*fragment.source);if(fragment.sink)endpoint(*fragment.sink);}
}

using namespace binding;
bool isInPlaceStructure(const DiagramNode &n) noexcept {
    return n.kind == "primitive" && n.type == kInPlaceStructureType;
}
bool inPlaceKindSupported(std::string_view k) noexcept {
    return k == "generic" || k == "array_index" || k == "array_split" || k == "cluster" ||
           k == "waveform" || k == "variant" || k == "variant_attribute" || k == "dvr" || k == "map";
}
TypeRef inPlaceMapActionType() {
    auto type=TypeRef::enumeration(std::string(kInPlaceMapActionDomain),TypeRef::primitive("u32"));
    type.choiceKind=ChoiceKind::Enum;
    type.choices={{"modify_if_present","Modify If Present",0},{"add_or_modify","Add or Modify",1},{"delete","Delete",2}};
    return type;
}
std::string inPlacePortId(std::string_view pair, std::string_view role, std::string_view access,
                          std::size_t dim) {
    auto id = "ipe:" + std::string(pair) + ":" + std::string(role);
    if (!access.empty())
        id += ":" + std::string(access);
    if (role == "index" || role == "index_out")
        id += ":" + std::to_string(dim);
    return id;
}
namespace {
TypeRef errorType() { return parseType("cluster<status:bool,code:i32,source:string>").type; }
TypeRef component(TypeRef source, const DiagramInPlaceAccess &a, bool waveform) {
    if (waveform) {
        if (a.fieldPath.size() > 1)
            return {};
        const auto field =
            a.fieldPath.empty() ? std::string_view("Y") : std::string_view(a.fieldPath.front());
        const auto found = std::find(source.fieldIds.begin(), source.fieldIds.end(), field);
        if (found != source.fieldIds.end() && static_cast<std::size_t>(found-source.fieldIds.begin())<source.children.size())
            return source.children[static_cast<std::size_t>(found - source.fieldIds.begin())];
        return {};
    }
    if (a.fieldPath.empty()) return {};
    for (const auto &id : a.fieldPath) {
        const auto field = std::find(source.fieldIds.begin(), source.fieldIds.end(), id);
        if (source.kind != TypeKind::Cluster || field == source.fieldIds.end())
            return {};
        const auto i = static_cast<std::size_t>(field - source.fieldIds.begin());
        if(i>=source.children.size())return {};
        auto next = source.children[i];
        source = std::move(next);
    }
    return source;
}
BindingResult bad(std::string message,std::string code="ipe.contract") {
    return {{ResolutionStatus::Incompatible, {}, std::move(code), std::move(message)}, false, {}};
}
bool waveformSchema(const TypeRef &t) {
    if(t.kind!=TypeKind::Cluster || !t.known())return false;
    const auto field=[&](std::string_view id)->const TypeRef* {
        const auto at=std::find(t.fieldIds.begin(),t.fieldIds.end(),id);
        return at==t.fieldIds.end()?nullptr:&t.children[static_cast<std::size_t>(at-t.fieldIds.begin())];
    };
    const auto *y=field("Y"),*t0=field("t0"),*dt=field("dt");
    return y && y->kind==TypeKind::Array && y->rank==1 && y->children.size()==1 &&
        t0 && *t0==TypeRef::primitive("timestamp") && dt && *dt==TypeRef::primitive("f64");
}
bool sourceAllowed(std::string_view kind, const TypeRef &t) {
    if (!t.known())
        return true;
    if (kind == "array_index" || kind == "array_split")
        return t.kind == TypeKind::Array;
    if (kind == "cluster")
        return t.kind == TypeKind::Cluster;
    if (kind == "waveform")
        return waveformSchema(t);
    if (kind == "dvr")
        return t.kind==TypeKind::Reference && t.name==kInPlaceDvrClass && t.children.size()==1;
    if (kind == "map") return t.kind==TypeKind::Map;
    if (kind == "variant" || kind == "variant_attribute")
        return t.name == "variant";
    return kind == "generic";
}
} // namespace
std::vector<InPlacePort> inPlacePorts(const DiagramNode &node, const InPlaceInputResolver &input) {
    std::vector<InPlacePort> ports;
    for (const auto &pair : node.inPlacePairs) {
        if (!inPlaceKindSupported(pair.kind))
            continue;
        auto source = input(inPlacePortId(pair.id, "source")).resolution.type;
        if (!source.known() && (pair.kind == "variant" || pair.kind == "variant_attribute"))
            source = TypeRef::primitive("variant");
        const auto add = [&](std::string_view role, std::string label, FunctionPortDirection direction,
                             bool inner, bool right, double row, TypeRef type,
                             bool required = false, std::string_view access = {},
                             std::size_t dim = 0) {
            ports.push_back({inPlacePortId(pair.id, role, access, dim), pair.id, std::move(label),
                             direction, inner, right, required, row, std::move(type)});
        };
        const auto in = FunctionPortDirection::Input, out = FunctionPortDirection::Output;
        add("source", "source", in, false, false, 0, source, true);
        add("result", "result", out, false, true, 0, source);
        double row = 1;
        const std::size_t rank = source.kind == TypeKind::Array
                                     ? std::clamp(source.rank, std::size_t(1), std::size_t(16))
                                     : 1;
        for (std::size_t i = 0; i < pair.accesses.size() && i < 32; ++i) {
            const auto &a = pair.accesses[i];
            TypeRef value = source;
            std::string label = "element " + std::to_string(i);
            if (pair.kind == "array_index") {
                value = source.kind == TypeKind::Array && !source.children.empty()
                            ? source.children[0]
                            : TypeRef{};
                for (std::size_t d = 0; d < rank; ++d) {
                    add("index", "index " + std::to_string(d), in, false, false, row,
                        TypeRef::primitive("i32"), false, a.id, d);
                    add("index_out", "index " + std::to_string(d), out, false, true, row,
                        TypeRef::primitive("i32"), false, a.id, d);
                    ++row;
                }
            } else if (pair.kind == "array_split" && i + 1 < pair.accesses.size()) {
                add("length", "split length", in, false, false, row, TypeRef::primitive("i32"),
                    false, a.id);
                add("length_out", "split length", out, false, true, row, TypeRef::primitive("i32"),
                    false, a.id);
                ++row;
            } else if (pair.kind == "cluster" || pair.kind == "waveform") {
                value = component(source, a, pair.kind == "waveform");
                if (!a.fieldPath.empty()) {
                    label.clear();
                    for (const auto &id : a.fieldPath) {
                        if (!label.empty())
                            label += " / ";
                        label += id;
                    }
                    if(pair.kind=="cluster") {
                        std::string currentName;
                        if(clusterFieldByIdPath(source,a.fieldPath,&currentName))label=std::move(currentName);
                    }
                } else if (pair.kind == "waveform")
                    label = "Y";
                else if (i < source.fieldIds.size())
                    label = source.fieldIds[i];
            } else if (pair.kind == "variant") {
                value = input(inPlacePortId(pair.id, "type", a.id)).resolution.type;
                add("type", "type", in, false, false, row, value, true, a.id);
                ++row;
            } else if (pair.kind == "variant_attribute") {
                value = TypeRef::primitive("variant");
                add("attribute_name", "attribute name", in, false, false, row,
                    TypeRef::primitive("string"), true, a.id);
                add("name_out", "attribute name", out, false, true, row,
                    TypeRef::primitive("string"), false, a.id);
                ++row;
            } else if (pair.kind == "dvr") {
                label="value";
                value=source.kind==TypeKind::Reference && source.name==kInPlaceDvrClass && source.children.size()==1
                    ?source.children.front():TypeRef{};
            } else if (pair.kind == "map") {
                label="value";
                const bool known=source.kind==TypeKind::Map && source.children.size()==2;
                const auto key=known?source.children[0]:TypeRef{};
                value=known?source.children[1]:TypeRef{};
                add("key","key",in,false,false,row,key,true,a.id);
                add("key_out","key",out,false,true,row,key,false,a.id);++row;
                add("default","default value",in,false,false,row,value,false,a.id);++row;
            }
            add("read", label, out, true, false, row, value, false, a.id);
            add("write", label, in, true, true, row, value,
                !(pair.kind=="dvr" && pair.allowParallelReadOnly), a.id);
            ++row;
            if (pair.kind == "variant_attribute") {
                add("found_inner", "found?", out, true, false, row, TypeRef::primitive("bool"),
                    false, a.id);
                add("found_outer", "found?", out, false, true, row, TypeRef::primitive("bool"),
                    false, a.id);
                ++row;
            } else if(pair.kind=="map") {
                add("action","action",in,true,true,row,inPlaceMapActionType(),false,a.id);++row;
                add("not_found_inner","key not found?",out,true,false,row,TypeRef::primitive("bool"),false,a.id);
                add("not_found_outer","key not found?",out,false,true,row,TypeRef::primitive("bool"),false,a.id);++row;
            }
        }
        if (pair.kind == "variant" || pair.kind == "dvr") {
            add("error_inner", "error out", out, true, false, row, errorType());
            add("error_outer", "error out", out, false, true, row, errorType());
        }
    }
    return ports;
}
std::optional<InPlacePort> inPlacePort(const DiagramNode &n, std::string_view id,
                                       const InPlaceInputResolver &input) {
    if (!id.starts_with("ipe:"))
        return {};
    const auto end = id.find(':', 4);
    if (end == std::string_view::npos)
        return {};
    const auto pairId = id.substr(4, end - 4);
    const auto pair = std::find_if(n.inPlacePairs.begin(), n.inPlacePairs.end(),
                                   [&](const auto &p) { return p.id == pairId; });
    if (pair == n.inPlacePairs.end())
        return {};
    // Resolve only the queried pair. Looking up unrelated pairs here would
    // introduce false cycles when one independent pair feeds another.
    DiagramNode single;
    single.inPlacePairs = {*pair};
    for (auto &p : inPlacePorts(single, input))
        if (p.id == id)
            return p;
    return {};
}
namespace {
std::optional<InPlacePort> structuralPort(const DiagramNode &n, std::string_view id) {
    // Rank-independent recognition includes index ports hidden after a rank change,
    // so existing wires retain their identities and become explicit diagnostics.
    return inPlacePort(n, id, [](std::string_view) {
        return BindingResult{
            {ResolutionStatus::Unresolved, TypeRef::array({}, 16), {}, {}}, false, {}};
    });
}
} // namespace
std::optional<bool> inPlacePortIsInner(const DiagramNode &n, std::string_view id) {
    if (const auto p = structuralPort(n, id))
        return p->inner;
    return {};
}
std::optional<FunctionPortDirection> inPlacePortDirection(const DiagramNode &n,
                                                          std::string_view id) {
    if (const auto p = structuralPort(n, id))
        return p->direction;
    return {};
}
BindingResult resolveInPlacePort(const DiagramNode &n, std::string_view id,
                                 const InPlaceInputResolver &input) {
    const auto port = inPlacePort(n, id, input);
    if (!port)
        return bad("The In Place terminal no longer exists for this configuration/rank.");
    const auto pair = std::find_if(n.inPlacePairs.begin(), n.inPlacePairs.end(),
                                   [&](const auto &p) { return p.id == port->pairId; });
    auto source = input(inPlacePortId(pair->id, "source"));
    if (source.resolution.status == ResolutionStatus::Incompatible)
        return source;
    if (!sourceAllowed(pair->kind, source.resolution.type))
        return pair->kind=="dvr"
            ?bad("Data Value Reference access requires ref<data_value,T>; control, Queue, program and file references are distinct.","ipe.reference_class")
            :bad("The source type does not match this In Place operation.","ipe.source_type");
    if (pair->kind == "array_split" && source.resolution.type.kind == TypeKind::Array &&
        pair->splitDimension >= source.resolution.type.rank)
        return bad("Split dimension is outside the source array rank.","ipe.split_dimension");
    if (pair->kind == "array_split" && pair->accesses.size() > 2 && pair->accesses.size() <= 32) {
        std::size_t lengths = 0;
        for (std::size_t i = 0; i + 1 < pair->accesses.size(); ++i) {
            const auto length =
                input(inPlacePortId(pair->id, "length", pair->accesses[i].id)).resolution;
            if (length.type.known() || length.status == ResolutionStatus::Incompatible)
                ++lengths;
        }
        if (lengths && lengths + 1 != pair->accesses.size())
            return bad("Wire all or none of the split lengths.");
    }
    if ((pair->kind == "cluster" || pair->kind == "waveform") &&
        source.resolution.type.known() && !port->type.known() &&
        (port->inner))
        return bad("The selected cluster field does not exist or has not been selected.","ipe.field_missing");
    return {{port->type.known() ? ResolutionStatus::Compatible : ResolutionStatus::Unresolved,
             port->type,
             {},
             {}},
            port->type.known() && source.complete && source.resolution.accepted(),
            {}};
}
TypeResolution checkInPlaceConnection(const DiagramNode &n, std::string_view id,
                                      const TypeRef &value, const InPlaceInputResolver &input) {
    const auto overrideInput = [&](std::string_view p) {
        return p == id ? BindingResult{{ResolutionStatus::Compatible, value, {}, {}}, true, {}}
                       : input(p);
    };
    const auto target = inPlacePort(n, id, overrideInput);
    if (!target || target->direction != FunctionPortDirection::Input)
        return bad("Invalid In Place input terminal.").resolution;
    const auto resolved = resolveInPlacePort(n, id, overrideInput);
    if (resolved.resolution.status == ResolutionStatus::Incompatible)
        return resolved.resolution;
    TypeResolution candidate{ResolutionStatus::Unresolved, value, {}, {}};
    if (target->type.known() && value.known()) {
        const bool numericParameter =
            !target->inner && (id.find(":index:") != std::string_view::npos ||
                               id.find(":length:") != std::string_view::npos);
        candidate =
            connect(value, target->type,
                    numericParameter ? ConnectionPolicy::NumericImplicit : ConnectionPolicy::Exact);
        if (!candidate.accepted())
            return candidate;
    }
    // A new source/type witness must also agree with already wired replacements.
    for (const auto &p : inPlacePorts(n, overrideInput))
        if (p.pairId == target->pairId && p.inner && p.direction == FunctionPortDirection::Input) {
            const auto replacement = overrideInput(p.id).resolution.type;
            if (replacement.known() && p.type.known()) {
                const auto match = connect(replacement, p.type, ConnectionPolicy::Exact);
                if (!match.accepted())
                    return match;
            }
        }
    return candidate;
}
std::vector<InPlaceDiagnostic> inPlaceStructureDiagnostics(const DiagramNode &n,
    std::span<const DiagramEdge> edges,const InPlaceInputResolver &input) {
    std::vector<InPlaceDiagnostic> errors;
    const auto error=[&](std::string code,std::string port,std::string message) {
        if(std::none_of(errors.begin(),errors.end(),[&](const auto &old) {
            return old.code==code && old.portId==port && old.message==message;
        }))errors.push_back({std::move(code),std::move(port),std::move(message)});
    };
    if(n.inPlaceSemanticProfile!=kInPlaceSemanticProfile)
        error("ipe.profile_unsupported",{},"This In Place semantic profile is not supported: "+n.inPlaceSemanticProfile);
    std::unordered_set<std::string> ids;
    for (const auto &pair : n.inPlacePairs) {
        if (pair.id.empty() || pair.id.find(':') != std::string::npos ||
            !ids.insert(pair.id).second)
            error("ipe.pair_identity",{},"Invalid or duplicate In Place pair ID.");
        if (!inPlaceKindSupported(pair.kind))
            error("ipe.kind_unsupported",{},"Unsupported In Place access kind: " + pair.kind);
        if (pair.accesses.empty() || pair.accesses.size() > 32 || !std::isfinite(pair.y))
            error("ipe.pair_configuration",{},"Invalid In Place pair geometry or access count.");
        if (((pair.kind == "generic" || pair.kind=="variant" || pair.kind=="dvr" || pair.kind=="map") && pair.accesses.size() != 1) ||
            (pair.kind == "array_split" && pair.accesses.size() < 2))
            error("ipe.access_count",{},"Invalid access count for this In Place operation.");
        if(pair.allowParallelReadOnly && pair.kind!="dvr")
            error("ipe.read_only_family",{},"Parallel read-only access belongs only to a Data Value Reference pair.");
        std::unordered_set<std::string> accessIds;
        for (const auto &a : pair.accesses)
            if (a.id.empty() || a.id.find(':') != std::string::npos ||
                !accessIds.insert(a.id).second)
                error("ipe.access_identity",{},"Invalid or duplicate access ID.");
        if(pair.kind=="cluster" || pair.kind=="waveform") {
            const auto path=[&](const auto &a) {
                return pair.kind=="waveform" && a.fieldPath.empty()
                    ?std::vector<std::string>{"Y"}:a.fieldPath;
            };
            for(std::size_t a=0;a<pair.accesses.size();++a)for(std::size_t b=a+1;b<pair.accesses.size();++b) {
                const auto left=path(pair.accesses[a]),right=path(pair.accesses[b]);
                const auto count=(std::min)(left.size(),right.size());
                if(count && std::equal(left.begin(),left.begin()+count,right.begin()))
                    error("ipe.field_overlap",inPlacePortId(pair.id,"read",pair.accesses[b].id),
                        "Selected In Place fields overlap; select each mutable region only once.");
            }
        }
    }
    const auto ports = inPlacePorts(n, input);
    for (const auto &p : ports) {
        const auto resolved = resolveInPlacePort(n, p.id, input);
        if (resolved.resolution.status == ResolutionStatus::Incompatible)
            error(resolved.resolution.code,p.id,resolved.resolution.message);
        if (p.direction != FunctionPortDirection::Input)
            continue;
        const auto count = std::count_if(edges.begin(), edges.end(), [&](const auto &e) {
            return e.to.nodeId == n.id && e.to.portId == p.id;
        });
        if (count > 1 || (p.required && count != 1))
            error(count>1?"ipe.multiple_producers":"ipe.required_input",p.id,
                "Missing or multiple producers: " + p.id);
        if (count &&
            !checkInPlaceConnection(n, p.id, input(p.id).resolution.type, input).accepted())
            error("ipe.input_type",p.id,"Incompatible input: " + p.id);
    }
    for (const auto &pair : n.inPlacePairs)
        if (pair.kind == "array_split" && pair.accesses.size() >= 2 && pair.accesses.size() <= 32) {
            std::size_t wired = 0, total = pair.accesses.size() - 1;
            for (std::size_t i = 0; i < total; ++i)
                if (std::any_of(edges.begin(), edges.end(), [&](const auto &e) {
                        return e.to.nodeId == n.id &&
                               e.to.portId == inPlacePortId(pair.id, "length", pair.accesses[i].id);
                    }))
                    ++wired;
            if (wired && wired != total)
                error("ipe.split_lengths",inPlacePortId(pair.id,"source"),"Wire all or none of the split lengths.");
        }
    for (const auto &e : edges)
        for (const auto &endpoint : {e.from, e.to})
            if (endpoint.nodeId == n.id && endpoint.portId.starts_with("ipe:") &&
                std::none_of(ports.begin(), ports.end(),
                             [&](const auto &p) { return p.id == endpoint.portId; }))
                error("ipe.obsolete_port",endpoint.portId,"Obsolete In Place terminal: " + endpoint.portId);
    return errors;
}
std::vector<std::string> validateInPlaceStructure(const DiagramNode &n,
    std::span<const DiagramEdge> edges,const InPlaceInputResolver &input) {
    std::vector<std::string> result;
    for(auto &diagnostic:inPlaceStructureDiagnostics(n,edges,input))result.push_back(std::move(diagnostic.message));
    return result;
}
}
