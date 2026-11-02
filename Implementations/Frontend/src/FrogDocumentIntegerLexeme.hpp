#pragma once
#include "frog_native_engine/FrogJson.hpp"
#include "frog_native_engine/NumericValueLexeme.hpp"
#include "frog_native_engine/FrogDocumentFormat.hpp"
#include <charconv>
#include <cmath>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace frog::native_engine::document_internal {
// Keep private callers source-compatible; the exact domain/algorithm has one
// portable Numeric owner. Only envelope/revision checks remain in this file.
inline std::optional<std::string> canonicalWidgetWideIntegerLexeme(
    std::string_view text,std::string_view valueType) {
    return numeric_value::canonicalWideIntegerLexeme(text,valueType);
}
inline std::optional<double> widgetWideIntegerCacheProjection(
    std::string_view text,std::string_view valueType) {
    return numeric_value::wideIntegerCacheProjection(text,valueType);
}

// Full-widget value envelopes are private codec transports, not public Source.
// Their root deliberately has no classId/class_ref: pre-revision-4 decoders
// reject it before constructing the numeric widget with its default zero.
inline const source::JsonValue *privateWidgetValueObject(const source::JsonValue &root) {
    const auto *format=root.member("format");
    if(!format)return &root;
    if(format->type!=source::JsonValue::Type::String || format->string!=kFrogWidgetValueDraftFormat)
        return root.member("classId") || root.member("class_ref") ? &root : nullptr;
    const auto *revision=root.member("draft_revision"), *widget=root.member("widget");
    if(!revision || revision->type!=source::JsonValue::Type::Number ||
        !widget || widget->type!=source::JsonValue::Type::Object ||
        root.member("classId") || root.member("class_ref") || root.member("spec_version") || root.member("array_cell_value"))return nullptr;
    const auto revisionLexeme=canonicalWidgetWideIntegerLexeme(revision->string,"u64");
    if(!revisionLexeme || (*revisionLexeme!=std::to_string(kFrogExactIntegerStateDraftRevision) &&
        *revisionLexeme!=std::to_string(kFrogComplexStateDraftRevision) &&
        *revisionLexeme!=std::to_string(kFrogApplicationControlDraftRevision) &&
        *revisionLexeme!=std::to_string(kFrogConnectivityDraftRevision) &&
        *revisionLexeme!=std::to_string(kFrogTypeDefinitionDraftRevision)))return nullptr;
    const auto *classId=widget->member("classId");
    if(!classId)classId=widget->member("class_ref");
    if(!classId || classId->type!=source::JsonValue::Type::String || classId->string.empty())return nullptr;
    return widget;
}
} // namespace frog::native_engine::document_internal
