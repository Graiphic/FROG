#pragma once
#include "frog_native_engine/BindingType.hpp"
#include <string>

namespace frog::native_engine {
// Identity and value schema shared with native hosts. No document traversal,
// widget model, registry implementation or process-local handle is required.
enum class ObjectReferenceKind { Program, FrontPanel, Widget, Application,
    Menu, Cursor, GraphCursor, GraphScale, GraphPlot, TextPart, Pane };
struct ObjectReferenceRecord {
    ObjectReferenceKind kind=ObjectReferenceKind::Widget;
    std::string id,classId,parentId;
    binding::TypeRef valueType;
};
}
