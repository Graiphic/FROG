#include "frog_native_engine/GeometryBinding.hpp"
#include <algorithm>
#include <array>
#include <set>
#include <vector>

namespace frog::native_engine {
namespace {
using binding::TypeKind;
using binding::TypeRef;
using binding::ResolutionStatus;
struct PortLaw { std::string_view id,type;bool required;std::string_view displayName; };
struct VariantLaw {
    GeometryVariantDescriptor descriptor;
    std::vector<PortLaw> inputs,outputs;
    std::vector<GeometryInputDefault> defaults;
    std::vector<std::string_view> inputIds,outputIds;
};
struct FunctionLaw {
    std::vector<VariantLaw> variants;
    std::vector<GeometryVariantDescriptor> variantDescriptors;
};

constexpr std::array<GeometryFunctionDescriptor,24> descriptions{{
    {"math.geometry.shift_2d","frog.studio.math.geometry.shift_2d.scalar.owned.v1","2D Shift"},
    {"math.geometry.rotation_2d","frog.studio.math.geometry.rotation_2d.scalar.owned.v1","2D Rotation"},
    {"math.geometry.shift_3d","frog.studio.math.geometry.shift_3d.scalar.owned.v1","3D Shift"},
    {"math.geometry.rotation_3d_euler","frog.studio.math.geometry.rotation_3d_euler.scalar.owned.v1","3D Rot. (Euler)"},
    {"math.geometry.rotation_3d_direction","frog.studio.math.geometry.rotation_3d_direction.scalar.owned.v1","3D Rot. (Direct)"},
    {"math.geometry.cross_product","frog.studio.math.geometry.cross_product.default.owned.v1","Cross Product"},
    {"math.geometry.euler_to_direction","frog.studio.math.geometry.euler_to_direction.default.owned.v1","Euler Angles To Direction Cosines"},
    {"math.geometry.direction_to_euler","frog.studio.math.geometry.direction_to_euler.default.owned.v1","Direction Cosines To Euler Angles"},
    {"math.geometry.coordinate_conversion_3d","frog.studio.math.geometry.coordinate_conversion_3d.scalar.owned.v1","3D Coordinate Conversion"},
    {"math.geometry.wrap_angle","frog.studio.math.geometry.wrap_angle.scalar.owned.v1","Wrap Angle"},
    {"math.geometry.complementary_angle","frog.studio.math.geometry.complementary_angle.scalar.owned.v1","Complementary Angle"},
    {"math.geometry.supplementary_angle","frog.studio.math.geometry.supplementary_angle.scalar.owned.v1","Supplementary Angle"},
    {"math.geometry.angle_rotation","frog.studio.math.geometry.angle_rotation.scalar.owned.v1","Angle Rotation"},
    {"math.geometry.bisect_angle","frog.studio.math.geometry.bisect_angle.scalar.owned.v1","Bisect Angle"},
    {"math.geometry.included_angle","frog.studio.math.geometry.included_angle.scalar.owned.v1","Check for Included Angle"},
    {"math.geometry.absolute_angle_difference","frog.studio.math.geometry.absolute_angle_difference.scalar.owned.v1","Absolute Angle Difference"},
    {"math.geometry.contour_line","frog.studio.math.geometry.contour_line.single.owned.v1","Contour Line"},
    {"math.geometry.polygon_area","frog.studio.math.geometry.polygon_area.default.owned.v1","Polygon Area"},
    {"math.geometry.point_in_polygon","frog.studio.math.geometry.point_in_polygon.scalar.owned.v1","Point in Polygon"},
    {"math.geometry.polygon_centroid","frog.studio.math.geometry.polygon_centroid.default.owned.v1","Polygon Centroid"},
    {"math.geometry.delaunay_triangulation","frog.studio.math.geometry.delaunay_triangulation.default.owned.v1","Delaunay Triangulation"},
    {"math.geometry.voronoi_diagram","frog.studio.math.geometry.voronoi_diagram.default.owned.v1","Voronoi Diagram"},
    {"math.geometry.convex_hull","frog.studio.math.geometry.convex_hull.default.owned.v1","Convex Hull"},
    {"math.geometry.convex_polygon_intersection","frog.studio.math.geometry.convex_polygon_intersection.default.owned.v1","Convex Polygon Intersection"},
}};
const std::array<FunctionLaw,24> &laws() {
    static const auto value=[] {
        std::array<FunctionLaw,24> result{{
            {{
                {{"scalar","frog.studio.math.geometry.shift_2d.scalar.owned.v1","Scalar"},{{"x","f64",true,"x"},{"y","f64",true,"y"},{"dx","f64",false,"dx"},{"dy","f64",false,"dy"}},{{"x_out","f64",true,"x out"},{"y_out","f64",true,"y out"},{"error","i32",true,"error"}},{{"dx","f64","0"},{"dy","f64","0"}},{},{}},
                {{"array","frog.studio.math.geometry.shift_2d.array.owned.v1","Array"},{{"x","array<f64>",true,"x"},{"y","array<f64>",true,"y"},{"dx","f64",false,"dx"},{"dy","f64",false,"dy"}},{{"x_out","array<f64>",true,"x out"},{"y_out","array<f64>",true,"y out"},{"error","i32",true,"error"}},{{"dx","f64","0"},{"dy","f64","0"}},{},{}},
            },{}},
            {{
                {{"scalar","frog.studio.math.geometry.rotation_2d.scalar.owned.v1","Scalar"},{{"x","f64",true,"x"},{"y","f64",true,"y"},{"theta","f64",false,"theta"}},{{"x_out","f64",true,"x out"},{"y_out","f64",true,"y out"},{"error","i32",true,"error"}},{{"theta","f64","0"}},{},{}},
                {{"array","frog.studio.math.geometry.rotation_2d.array.owned.v1","Array"},{{"x","array<f64>",true,"x"},{"y","array<f64>",true,"y"},{"theta","f64",false,"theta"}},{{"x_out","array<f64>",true,"x out"},{"y_out","array<f64>",true,"y out"},{"error","i32",true,"error"}},{{"theta","f64","0"}},{},{}},
            },{}},
            {{
                {{"scalar","frog.studio.math.geometry.shift_3d.scalar.owned.v1","Scalar"},{{"x","f64",true,"x"},{"y","f64",true,"y"},{"z","f64",true,"z"},{"dx","f64",false,"dx"},{"dy","f64",false,"dy"},{"dz","f64",false,"dz"}},{{"x_out","f64",true,"x out"},{"y_out","f64",true,"y out"},{"z_out","f64",true,"z out"},{"error","i32",true,"error"}},{{"dx","f64","0"},{"dy","f64","0"},{"dz","f64","0"}},{},{}},
                {{"array","frog.studio.math.geometry.shift_3d.array.owned.v1","Array"},{{"x","array<f64>",true,"x"},{"y","array<f64>",true,"y"},{"z","array<f64>",true,"z"},{"dx","f64",false,"dx"},{"dy","f64",false,"dy"},{"dz","f64",false,"dz"}},{{"x_out","array<f64>",true,"x out"},{"y_out","array<f64>",true,"y out"},{"z_out","array<f64>",true,"z out"},{"error","i32",true,"error"}},{{"dx","f64","0"},{"dy","f64","0"},{"dz","f64","0"}},{},{}},
            },{}},
            {{
                {{"scalar","frog.studio.math.geometry.rotation_3d_euler.scalar.owned.v1","Scalar"},{{"x","f64",true,"x"},{"y","f64",true,"y"},{"z","f64",true,"z"},{"euler_angles","cluster<phi:f64,theta:f64,psi:f64>",true,"euler angles"},{"rotation_order","u16",false,"rotation order"}},{{"x_out","f64",true,"x out"},{"y_out","f64",true,"y out"},{"z_out","f64",true,"z out"},{"error","i32",true,"error"}},{{"rotation_order","u16","10"}},{},{}},
                {{"array","frog.studio.math.geometry.rotation_3d_euler.array.owned.v1","Array"},{{"x","array<f64>",true,"x"},{"y","array<f64>",true,"y"},{"z","array<f64>",true,"z"},{"euler_angles","cluster<phi:f64,theta:f64,psi:f64>",true,"euler angles"},{"rotation_order","u16",false,"rotation order"}},{{"x_out","array<f64>",true,"x out"},{"y_out","array<f64>",true,"y out"},{"z_out","array<f64>",true,"z out"},{"error","i32",true,"error"}},{{"rotation_order","u16","10"}},{},{}},
            },{}},
            {{
                {{"scalar","frog.studio.math.geometry.rotation_3d_direction.scalar.owned.v1","Scalar"},{{"x","f64",true,"x"},{"y","f64",true,"y"},{"z","f64",true,"z"},{"direction_cosines","array<f64,rank=2>",true,"direction cosines"}},{{"x_out","f64",true,"x out"},{"y_out","f64",true,"y out"},{"z_out","f64",true,"z out"},{"error","i32",true,"error"}},{},{},{}},
                {{"array","frog.studio.math.geometry.rotation_3d_direction.array.owned.v1","Array"},{{"x","array<f64>",true,"x"},{"y","array<f64>",true,"y"},{"z","array<f64>",true,"z"},{"direction_cosines","array<f64,rank=2>",true,"direction cosines"}},{{"x_out","array<f64>",true,"x out"},{"y_out","array<f64>",true,"y out"},{"z_out","array<f64>",true,"z out"},{"error","i32",true,"error"}},{},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.geometry.cross_product.default.owned.v1","Default"},{{"a","array<f64>",true,"a"},{"b","array<f64>",true,"b"}},{{"cross","array<f64>",true,"cross"},{"error","i32",true,"error"}},{},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.geometry.euler_to_direction.default.owned.v1","Default"},{{"euler_angles","cluster<phi:f64,theta:f64,psi:f64>",true,"euler angles"},{"rotation_order","u16",false,"rotation order"}},{{"direction_cosines","array<f64,rank=2>",true,"direction cosines"},{"error","i32",true,"error"}},{{"rotation_order","u16","10"}},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.geometry.direction_to_euler.default.owned.v1","Default"},{{"direction_cosines","array<f64,rank=2>",true,"direction cosines"},{"rotation_order","u16",false,"rotation order"}},{{"euler_angles","cluster<phi:f64,theta:f64,psi:f64>",true,"euler angles"},{"error","i32",true,"error"}},{{"rotation_order","u16","10"}},{},{}},
            },{}},
            {{
                {{"scalar","frog.studio.math.geometry.coordinate_conversion_3d.scalar.owned.v1","Scalar"},{{"axis1","f64",true,"axis1"},{"axis2","f64",true,"axis2"},{"axis3","f64",true,"axis3"},{"conversion_type","u16",false,"conversion type"}},{{"axis1_out","f64",true,"axis1 out"},{"axis2_out","f64",true,"axis2 out"},{"axis3_out","f64",true,"axis3 out"},{"error","i32",true,"error"}},{{"conversion_type","u16","0"}},{},{}},
                {{"array","frog.studio.math.geometry.coordinate_conversion_3d.array.owned.v1","Array"},{{"axis1","array<f64>",true,"axis1"},{"axis2","array<f64>",true,"axis2"},{"axis3","array<f64>",true,"axis3"},{"conversion_type","u16",false,"conversion type"}},{{"axis1_out","array<f64>",true,"axis1 out"},{"axis2_out","array<f64>",true,"axis2 out"},{"axis3_out","array<f64>",true,"axis3 out"},{"error","i32",true,"error"}},{{"conversion_type","u16","0"}},{},{}},
            },{}},
            {{
                {{"scalar","frog.studio.math.geometry.wrap_angle.scalar.owned.v1","Scalar"},{{"angle","f64",true,"angle"},{"angle_units","i32",false,"angle units"},{"angle_range","u16",false,"angle range"}},{{"angle_out","f64",true,"angle out"},{"error","i32",true,"error"}},{{"angle_units","i32","0"},{"angle_range","u16","1"}},{},{}},
                {{"array","frog.studio.math.geometry.wrap_angle.array.owned.v1","Array"},{{"angle","array<f64>",true,"angle"},{"angle_units","i32",false,"angle units"},{"angle_range","u16",false,"angle range"}},{{"angle_out","array<f64>",true,"angle out"},{"error","i32",true,"error"}},{{"angle_units","i32","0"},{"angle_range","u16","1"}},{},{}},
            },{}},
            {{
                {{"scalar","frog.studio.math.geometry.complementary_angle.scalar.owned.v1","Scalar"},{{"angle","f64",true,"angle"},{"angle_units","i32",false,"angle units"},{"angle_range","u16",false,"angle range"}},{{"angle_out","f64",true,"angle out"},{"error","i32",true,"error"}},{{"angle_units","i32","0"},{"angle_range","u16","1"}},{},{}},
                {{"array","frog.studio.math.geometry.complementary_angle.array.owned.v1","Array"},{{"angle","array<f64>",true,"angle"},{"angle_units","i32",false,"angle units"},{"angle_range","u16",false,"angle range"}},{{"angle_out","array<f64>",true,"angle out"},{"error","i32",true,"error"}},{{"angle_units","i32","0"},{"angle_range","u16","1"}},{},{}},
            },{}},
            {{
                {{"scalar","frog.studio.math.geometry.supplementary_angle.scalar.owned.v1","Scalar"},{{"angle","f64",true,"angle"},{"angle_units","i32",false,"angle units"},{"angle_range","u16",false,"angle range"}},{{"angle_out","f64",true,"angle out"},{"error","i32",true,"error"}},{{"angle_units","i32","0"},{"angle_range","u16","1"}},{},{}},
                {{"array","frog.studio.math.geometry.supplementary_angle.array.owned.v1","Array"},{{"angle","array<f64>",true,"angle"},{"angle_units","i32",false,"angle units"},{"angle_range","u16",false,"angle range"}},{{"angle_out","array<f64>",true,"angle out"},{"error","i32",true,"error"}},{{"angle_units","i32","0"},{"angle_range","u16","1"}},{},{}},
            },{}},
            {{
                {{"scalar","frog.studio.math.geometry.angle_rotation.scalar.owned.v1","Scalar"},{{"angle","f64",true,"angle"},{"delta","f64",true,"delta"},{"angle_units","i32",false,"angle units"},{"angle_range","u16",false,"angle range"}},{{"angle_out","f64",true,"angle out"},{"error","i32",true,"error"}},{{"angle_units","i32","0"},{"angle_range","u16","1"}},{},{}},
                {{"array","frog.studio.math.geometry.angle_rotation.array.owned.v1","Array"},{{"angle","array<f64>",true,"angle"},{"delta","array<f64>",true,"delta"},{"angle_units","i32",false,"angle units"},{"angle_range","u16",false,"angle range"}},{{"angle_out","array<f64>",true,"angle out"},{"error","i32",true,"error"}},{{"angle_units","i32","0"},{"angle_range","u16","1"}},{},{}},
            },{}},
            {{
                {{"scalar","frog.studio.math.geometry.bisect_angle.scalar.owned.v1","Scalar"},{{"start","f64",true,"start"},{"end","f64",true,"end"},{"angle_units","i32",false,"angle units"},{"angle_range","u16",false,"angle range"},{"shortest_arc","bool",false,"shortest arc"}},{{"angle_out","f64",true,"angle out"},{"error","i32",true,"error"}},{{"angle_units","i32","0"},{"angle_range","u16","1"},{"shortest_arc","bool","false"}},{},{}},
                {{"array","frog.studio.math.geometry.bisect_angle.array.owned.v1","Array"},{{"start","array<f64>",true,"start"},{"end","array<f64>",true,"end"},{"angle_units","i32",false,"angle units"},{"angle_range","u16",false,"angle range"},{"shortest_arc","bool",false,"shortest arc"}},{{"angle_out","array<f64>",true,"angle out"},{"error","i32",true,"error"}},{{"angle_units","i32","0"},{"angle_range","u16","1"},{"shortest_arc","bool","false"}},{},{}},
            },{}},
            {{
                {{"scalar","frog.studio.math.geometry.included_angle.scalar.owned.v1","Scalar"},{{"angle","f64",true,"angle"},{"start","f64",true,"start"},{"end","f64",true,"end"},{"angle_units","i32",false,"angle units"},{"angle_range","u16",false,"angle range"},{"coerce_angle","bool",false,"coerce angle"}},{{"included","bool",true,"included"},{"angle_out","f64",true,"angle out"},{"error","i32",true,"error"}},{{"angle_units","i32","0"},{"angle_range","u16","1"},{"coerce_angle","bool","false"}},{},{}},
                {{"array","frog.studio.math.geometry.included_angle.array.owned.v1","Array"},{{"angle","array<f64>",true,"angle"},{"start","array<f64>",true,"start"},{"end","array<f64>",true,"end"},{"angle_units","i32",false,"angle units"},{"angle_range","u16",false,"angle range"},{"coerce_angle","bool",false,"coerce angle"}},{{"included","array<bool>",true,"included"},{"angle_out","array<f64>",true,"angle out"},{"error","i32",true,"error"}},{{"angle_units","i32","0"},{"angle_range","u16","1"},{"coerce_angle","bool","false"}},{},{}},
            },{}},
            {{
                {{"scalar","frog.studio.math.geometry.absolute_angle_difference.scalar.owned.v1","Scalar"},{{"a","f64",true,"a"},{"b","f64",true,"b"},{"angle_units","i32",false,"angle units"}},{{"angle_out","f64",true,"angle out"},{"error","i32",true,"error"}},{{"angle_units","i32","0"}},{},{}},
                {{"array","frog.studio.math.geometry.absolute_angle_difference.array.owned.v1","Array"},{{"a","array<f64>",true,"a"},{"b","array<f64>",true,"b"},{"angle_units","i32",false,"angle units"}},{{"angle_out","array<f64>",true,"angle out"},{"error","i32",true,"error"}},{{"angle_units","i32","0"}},{},{}},
            },{}},
            {{
                {{"single","frog.studio.math.geometry.contour_line.single.owned.v1","Single"},{{"z","array<f64,rank=2>",true,"z"},{"x_axis","array<f64>",false,"x axis"},{"y_axis","array<f64>",false,"y axis"},{"level","f64",true,"level"}},{{"points","array<f64,rank=2>",true,"points"},{"curve_offsets","array<i32>",true,"curve offsets"},{"curve_levels","array<f64>",true,"curve levels"},{"closed","array<bool>",true,"closed"},{"error","i32",true,"error"}},{{"x_axis","array<f64>","[]"},{"y_axis","array<f64>","[]"}},{},{}},
                {{"multiple","frog.studio.math.geometry.contour_line.multiple.owned.v1","Multiple"},{{"z","array<f64,rank=2>",true,"z"},{"x_axis","array<f64>",false,"x axis"},{"y_axis","array<f64>",false,"y axis"},{"levels","array<f64>",true,"levels"}},{{"points","array<f64,rank=2>",true,"points"},{"curve_offsets","array<i32>",true,"curve offsets"},{"curve_levels","array<f64>",true,"curve levels"},{"closed","array<bool>",true,"closed"},{"error","i32",true,"error"}},{{"x_axis","array<f64>","[]"},{"y_axis","array<f64>","[]"}},{},{}},
                {{"automatic","frog.studio.math.geometry.contour_line.automatic.owned.v1","Automatic"},{{"z","array<f64,rank=2>",true,"z"},{"x_axis","array<f64>",false,"x axis"},{"y_axis","array<f64>",false,"y axis"},{"count","i32",false,"count"}},{{"points","array<f64,rank=2>",true,"points"},{"curve_offsets","array<i32>",true,"curve offsets"},{"curve_levels","array<f64>",true,"curve levels"},{"closed","array<bool>",true,"closed"},{"error","i32",true,"error"}},{{"x_axis","array<f64>","[]"},{"y_axis","array<f64>","[]"},{"count","i32","10"}},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.geometry.polygon_area.default.owned.v1","Default"},{{"x","array<f64>",true,"x"},{"y","array<f64>",true,"y"}},{{"signed_area","f64",true,"signed area"},{"error","i32",true,"error"}},{},{},{}},
            },{}},
            {{
                {{"scalar","frog.studio.math.geometry.point_in_polygon.scalar.owned.v1","Scalar"},{{"x","array<f64>",true,"x"},{"y","array<f64>",true,"y"},{"x_query","f64",true,"x query"},{"y_query","f64",true,"y query"}},{{"inside","bool",true,"inside"},{"boundary","bool",true,"boundary"},{"error","i32",true,"error"}},{},{},{}},
                {{"array","frog.studio.math.geometry.point_in_polygon.array.owned.v1","Array"},{{"x","array<f64>",true,"x"},{"y","array<f64>",true,"y"},{"x_query","array<f64>",true,"x query"},{"y_query","array<f64>",true,"y query"}},{{"inside","array<bool>",true,"inside"},{"boundary","array<bool>",true,"boundary"},{"error","i32",true,"error"}},{},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.geometry.polygon_centroid.default.owned.v1","Default"},{{"x","array<f64>",true,"x"},{"y","array<f64>",true,"y"},{"centroid_method","u16",false,"centroid method"}},{{"centroid_x","f64",true,"centroid x"},{"centroid_y","f64",true,"centroid y"},{"error","i32",true,"error"}},{{"centroid_method","u16","0"}},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.geometry.delaunay_triangulation.default.owned.v1","Default"},{{"x","array<f64>",true,"x"},{"y","array<f64>",true,"y"}},{{"triangles","array<i32,rank=2>",true,"triangles"},{"error","i32",true,"error"}},{},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.geometry.voronoi_diagram.default.owned.v1","Default"},{{"x","array<f64>",true,"x"},{"y","array<f64>",true,"y"}},{{"vertices","array<f64,rank=2>",true,"vertices"},{"edges","array<i32,rank=2>",true,"edges"},{"directions","array<f64,rank=2>",true,"directions"},{"sites","array<i32,rank=2>",true,"sites"},{"edge_kind","array<i32>",true,"edge kind"},{"error","i32",true,"error"}},{},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.geometry.convex_hull.default.owned.v1","Default"},{{"x","array<f64>",true,"x"},{"y","array<f64>",true,"y"}},{{"hull","array<i32>",true,"hull"},{"error","i32",true,"error"}},{},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.geometry.convex_polygon_intersection.default.owned.v1","Default"},{{"x1","array<f64>",true,"x1"},{"y1","array<f64>",true,"y1"},{"x2","array<f64>",true,"x2"},{"y2","array<f64>",true,"y2"}},{{"intersection_x","array<f64>",true,"intersection x"},{"intersection_y","array<f64>",true,"intersection y"},{"error","i32",true,"error"}},{},{},{}},
            },{}},
        }};
        for(auto &function:result)for(auto &variant:function.variants) {
            for(const auto &p:variant.inputs)variant.inputIds.push_back(p.id);
            for(const auto &p:variant.outputs)variant.outputIds.push_back(p.id);
            function.variantDescriptors.push_back(variant.descriptor);
        }
        return result;
    }();
    return value;
}
std::string_view canonical(std::string_view id) {
    if(id.starts_with("frog.studio."))id.remove_prefix(12);
    if(id.ends_with(".owned.v1"))id.remove_suffix(9);
    return id;
}
struct Match { std::size_t function=descriptions.size(),variant=0;bool explicitVariant=false; };
Match match(std::string_view id) {
    id=canonical(id);
    for(std::size_t i=0;i<descriptions.size();++i) {
        if(descriptions[i].catalogId==id)return {i,0,false};
        for(std::size_t j=0;j<laws()[i].variants.size();++j)
            if(canonical(laws()[i].variants[j].descriptor.operationId)==id)return {i,j,true};
    }
    return {};
}
const VariantLaw *select(std::string_view id,std::string_view variant) {
    const auto found=match(id);if(found.function==descriptions.size())return nullptr;
    const auto &variants=laws()[found.function].variants;
    if(variant.empty())return &variants[found.variant];
    for(std::size_t i=0;i<variants.size();++i)
        if(variants[i].descriptor.variantId==variant)
            return found.explicitVariant&&i!=found.variant?nullptr:&variants[i];
    return nullptr;
}
GeometryTypeResult failure(std::string_view code,std::string_view message) {
    return {{ResolutionStatus::Incompatible,{},std::string(code),std::string(message)},false};
}
bool realNumeric(const TypeRef &type) {
    return type.kind==TypeKind::Primitive&&type.numeric()&&!type.name.starts_with("c")&&type.name!="f128";
}
bool accepts(const TypeRef &source,const TypeRef &target) {
    if(source==target)return true;
    if(target.kind==TypeKind::Primitive&&target.numeric()) {
        const auto &scalar=source.kind==TypeKind::Enum&&source.children.size()==1?source.children.front():source;
        return realNumeric(scalar);
    }
    if(target.kind==TypeKind::Array&&target.children.size()==1) {
        if(source.kind!=TypeKind::Array||source.rank!=target.rank||source.children.size()!=1)return false;
        return target.children.front().numeric()?realNumeric(source.children.front()):source.children.front()==target.children.front();
    }
    // Piecewise polynomials and endpoint-condition records keep their exact
    // field identities, order, ranks and representations.
    return false;
}
} // namespace

std::span<const GeometryFunctionDescriptor> geometryFunctions() noexcept { return descriptions; }
const GeometryFunctionDescriptor *findGeometryFunction(std::string_view id) noexcept {
    const auto found=match(id);return found.function==descriptions.size()?nullptr:&descriptions[found.function];
}
bool isGeometryFunction(std::string_view id) noexcept { return findGeometryFunction(id)!=nullptr; }
std::span<const GeometryVariantDescriptor> geometryVariants(std::string_view id) noexcept {
    const auto found=match(id);return found.function==descriptions.size()?std::span<const GeometryVariantDescriptor>{}:std::span<const GeometryVariantDescriptor>{laws()[found.function].variantDescriptors};
}
std::string_view geometryOperationId(std::string_view id,std::string_view variant) noexcept {
    const auto *law=select(id,variant);return law?law->descriptor.operationId:std::string_view{};
}
std::span<const std::string_view> geometryInputPortIds(std::string_view id,std::string_view variant) noexcept {
    const auto *law=select(id,variant);return law?std::span<const std::string_view>{law->inputIds}:std::span<const std::string_view>{};
}
std::span<const std::string_view> geometryOutputPortIds(std::string_view id,std::string_view variant) noexcept {
    const auto *law=select(id,variant);return law?std::span<const std::string_view>{law->outputIds}:std::span<const std::string_view>{};
}
std::span<const GeometryInputDefault> geometryInputDefaults(std::string_view id,std::string_view variant) noexcept {
    const auto *law=select(id,variant);return law?std::span<const GeometryInputDefault>{law->defaults}:std::span<const GeometryInputDefault>{};
}
bool geometryInputRequired(std::string_view id,std::string_view port,std::string_view variant) noexcept {
    const auto *law=select(id,variant);if(!law)return false;
    for(const auto &input:law->inputs)if(input.id==port)return input.required;
    return false;
}
std::string_view geometryPortDisplayName(std::string_view id,std::string_view port,std::string_view variant) noexcept {
    const auto *law=select(id,variant);if(!law)return {};
    for(const auto &input:law->inputs)if(input.id==port)return input.displayName;
    for(const auto &output:law->outputs)if(output.id==port)return output.displayName;
    return {};
}
GeometryTypeResult resolveGeometryPort(std::string_view id,std::string_view port,std::span<const GeometryTypeEvidence> inputs,std::string_view variant) {
    if(!isGeometryFunction(id))return failure("math.geometry.unknown_function","The Geometry function is not registered.");
    const auto *law=select(id,variant);
    if(!law)return failure("math.geometry.unknown_variant","The Geometry variant is unknown or contradicts the native operation identity.");
    const PortLaw *requested=nullptr;bool requestedInput=false;
    for(const auto &p:law->inputs)if(p.id==port){requested=&p;requestedInput=true;}
    for(const auto &p:law->outputs)if(p.id==port)requested=&p;
    if(!requested)return failure("math.geometry.missing_port","The selected Geometry variant has no such port.");
    std::set<std::string_view> seen;bool complete=true;
    const auto evidence=[&](std::string_view name)->const TypeRef* {
        for(const auto &e:inputs)if(e.portId==name)return &e.valueType;return nullptr;
    };
    for(const auto &e:inputs) {
        if(!seen.insert(e.portId).second)return failure("math.geometry.multiple_sources","A Geometry input has more than one source.");
        const auto found=std::find_if(law->inputs.begin(),law->inputs.end(),[&](const auto &p){return p.id==e.portId;});
        if(found==law->inputs.end())return failure("math.geometry.unexpected_input","The selected Geometry variant has no input with this identity.");
        if(!e.valueType.known()){complete=false;continue;}
        const auto expected=binding::parseType(found->type);
        if(!expected.valid())return failure("math.geometry.invalid_contract","The Geometry port contract is not a recognized public type.");
        if(!accepts(e.valueType,expected.type))return failure("math.geometry.input_type","The Geometry input requires its declared real numeric rank or exact structured schema; complex values and polynomial/boundary records are never silently converted.");
    }
    for(const auto &p:law->inputs)if(p.required){const auto *t=evidence(p.id);if(!t||!t->known())complete=false;}
    const auto parsed=binding::parseType(requested->type);
    if(!parsed.valid())return failure("math.geometry.invalid_contract","The Geometry port contract is not a recognized public type.");
    auto type=parsed.type;const auto *source=requestedInput?evidence(port):nullptr;
    if(source&&source->known()&&*source!=type)
        return {{ResolutionStatus::Coercion,std::move(type),"math.geometry.numeric_promotion","Geometry calculates in DBL; real numeric promotion is explicit and may round large integers."},complete};
    return {{ResolutionStatus::Compatible,std::move(type),{},{}},complete};
}
} // namespace frog::native_engine
