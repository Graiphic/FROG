#pragma once
// Source/authoring value contract only. No document, widget, catalogue,
// execution engine or LLVM dependency. Calculations use native SDK values.
#include "frog_native_engine/BindingType.hpp"
#include "frog_native_engine/FrogJson.hpp"
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace frog::native_engine {
inline constexpr std::string_view kCollectionAuthoringProfile="frog.studio.collection@1";
inline constexpr std::string_view kCollectionComparisonPolicy="frog.studio.collection.order@1";
inline constexpr std::size_t kMaximumCollectionEntries=4096;
inline constexpr std::size_t kMaximumCollectionBuildRows=32;
enum class CollectionKind {Map,Set};
struct CollectionDataStorage;
class CollectionData {
public:
    CollectionData() noexcept=default;
    [[nodiscard]] std::size_t size()const noexcept;
    [[nodiscard]] bool empty()const noexcept{return size()==0;}
    [[nodiscard]] std::size_t retainedBytes()const noexcept;
    [[nodiscard]] const void *storageIdentity()const noexcept{return storage_.get();}
    bool operator==(const CollectionData &)const;
private:
    std::shared_ptr<const CollectionDataStorage> storage_;
    explicit CollectionData(std::shared_ptr<const CollectionDataStorage> value):storage_(std::move(value)){}
    friend struct CollectionAuthoringCodec;
};
struct CollectionTypedValue {
    binding::TypeRef type;
    CollectionData data;
};
struct CollectionAuthoringState {
    CollectionKind kind=CollectionKind::Map;
    // Set uses valueType as T. Unknown models are editable only with no data.
    binding::TypeRef keyType,valueType;
    CollectionData data;
    std::string comparisonPolicy=std::string(kCollectionComparisonPolicy);
    // Existing private widget presentation codec, with intrinsic model values.
    // Opaque here to keep the value/schema contract independent of Widget.
    std::string keyPresentation,valuePresentation;
    std::size_t visibleIndex=0;
    // Map viewport presentation only. Missing in historical documents means
    // one row; revealing more rows never inserts, removes or retypes data.
    std::size_t visibleRows=1;
    bool operator==(const CollectionAuthoringState &)const;
};
struct CollectionBuildState {
    std::vector<std::string> rows{"row_0"};
    std::uint64_t nextRowSerial=1;
    // An empty Build retains its previously resolved complete result schema.
    binding::TypeRef typeWitness;
    bool operator==(const CollectionBuildState &)const;
};
[[nodiscard]] binding::TypeRef collectionType(const CollectionAuthoringState &);
[[nodiscard]] std::optional<CollectionTypedValue> collectionValue(const CollectionAuthoringState &);
[[nodiscard]] std::optional<CollectionAuthoringState> makeEmptyCollectionState(
    const binding::TypeRef &,std::string *error=nullptr);
[[nodiscard]] std::optional<CollectionAuthoringState> makeCollectionState(
    const CollectionTypedValue &,std::string *error=nullptr);
// Schema-preserving assignment. Unconfigured targets do not infer types here.
bool assignCollectionData(CollectionAuthoringState &,const CollectionTypedValue &,std::string *error=nullptr);
// Models do not create entries. A conflicting nonempty value is never erased.
bool configureCollectionTypeModels(CollectionAuthoringState &,const binding::TypeRef &key,
    const binding::TypeRef &value,std::string *error=nullptr);
[[nodiscard]] bool collectionComparableType(const binding::TypeRef &,std::string *error=nullptr);
[[nodiscard]] bool collectionTypesEqual(const binding::TypeRef &,const binding::TypeRef &);
// Exact typed JSON is a codec boundary, never the collection calculation API.
[[nodiscard]] std::optional<CollectionTypedValue> readCollectionData(
    const binding::TypeRef &,const source::JsonValue &,std::string *error=nullptr);
[[nodiscard]] source::JsonValue writeCollectionData(const CollectionTypedValue &);
// Immutable borrowed entry; valid while the source CollectionData is retained.
[[nodiscard]] const source::JsonValue *collectionEntrySourceValue(
    const CollectionAuthoringState &,bool key,std::size_t index) noexcept;
struct CollectionPreviewEntry {std::string key,value;};
// Bounded presentation projection; never copies or changes the full payload.
[[nodiscard]] std::vector<CollectionPreviewEntry> collectionPreviewEntries(
    const CollectionAuthoringState &,std::size_t maximumRows=4,std::size_t maximumCharacters=120);
[[nodiscard]] std::optional<CollectionAuthoringState> readCollectionAuthoringState(
    const source::JsonValue &,std::string *error=nullptr);
[[nodiscard]] source::JsonValue writeCollectionAuthoringState(const CollectionAuthoringState &);
[[nodiscard]] std::optional<CollectionBuildState> readCollectionBuildState(
    const source::JsonValue &,std::string *error=nullptr);
[[nodiscard]] source::JsonValue writeCollectionBuildState(const CollectionBuildState &);
} // namespace frog::native_engine
