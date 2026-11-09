#pragma once
#include "frog_native_engine/CollectionModel.hpp"
#include "frog_native_engine/DiagramBindingResolver.hpp"
#include <functional>

namespace frog::native_engine {
struct CollectionPort {
    std::string id,label;
    FunctionPortDirection direction=FunctionPortDirection::Input;
    binding::TypeRef type;
    bool required=true;
    std::size_t row=0;
};
using CollectionInputResolver=std::function<binding::BindingResult(std::string_view)>;
[[nodiscard]] std::string_view collectionFunctionId(std::string_view)noexcept;
[[nodiscard]] bool isCollectionFunction(std::string_view)noexcept;
[[nodiscard]] std::vector<CollectionPort> collectionPorts(const DiagramNode &,const CollectionInputResolver &);
[[nodiscard]] binding::BindingResult resolveCollectionPort(const DiagramNode &,std::string_view,const CollectionInputResolver &);
[[nodiscard]] binding::TypeResolution checkCollectionConnection(const DiagramNode &,std::string_view,
    const binding::TypeRef &,const CollectionInputResolver &);
} // namespace frog::native_engine
