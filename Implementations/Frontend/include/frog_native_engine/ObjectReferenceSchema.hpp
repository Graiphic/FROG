#pragma once
#include "frog_native_engine/ObjectReferenceRecord.hpp"
#include "frog_native_engine/ContainedWidgetPath.hpp"
#include "frog_native_engine/FrogWidgetData.hpp"
#include <map>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace frog::native_engine {
// Declarative object identities and their exact value schemas. These are not
// live handles or serializable runtime tokens and own no registry/session.
struct DocumentReferenceLocation {
    std::string rootId;
    ContainedWidgetPath path;
    bool presentationOnly=false;
};
struct ObjectReferenceSchema {
    std::vector<ObjectReferenceRecord> records;
    std::map<std::string,DocumentReferenceLocation,std::less<>> locations;
    std::map<std::pair<ObjectReferenceKind,std::string>,std::vector<std::string>> children;
    std::map<std::string,std::pair<std::string,std::string>,std::less<>> textParts;
    std::string error;
    [[nodiscard]] bool accepted()const noexcept{return error.empty();}
    [[nodiscard]] const ObjectReferenceRecord *find(ObjectReferenceKind,std::string_view)const noexcept;
};
[[nodiscard]] std::string objectReferenceTextPartId(std::string_view owner,std::string_view part);
// Reads the complete widget graph, including shared Array templates and Cluster
// fields, with the same stable identities and duplicate/cycle refusal as Studio.
[[nodiscard]] ObjectReferenceSchema buildFrogObjectReferenceSchema(FrogWidgetDataView);
}
