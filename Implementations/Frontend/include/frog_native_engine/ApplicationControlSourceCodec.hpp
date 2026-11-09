#pragma once
// Pure, shared codec for the complete private ApplicationControl value model.
// No Diagram, Canvas, catalogue, SDK, evaluator or native loader dependency.
#include "ApplicationControlTypes.hpp"
#include "FrogJson.hpp"
#include <iosfwd>
namespace frog::native_engine::application_control_source {
enum class ReadPolicy { PrivateCompatible, ExactLocalProfile };
bool preflight(const source::JsonValue &,std::string *error=nullptr);
void appendConfiguration(std::ostringstream &,const ApplicationControlNode &,int indent);
source::JsonValue writeConfiguration(const ApplicationControlNode &);
std::optional<ApplicationControlNode> readConfiguration(const source::JsonValue &,
    std::string *error=nullptr,ReadPolicy policy=ReadPolicy::ExactLocalProfile);
source::JsonValue writeType(const binding::TypeRef &);
std::optional<binding::TypeRef> readType(const source::JsonValue &,
    std::string *error=nullptr,ReadPolicy policy=ReadPolicy::ExactLocalProfile);
}

namespace frog::native_engine {std::string applicationControlPropertyPortId(std::string_view);}
