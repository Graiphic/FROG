// Generated from the attributed public Case working-tree snapshot. Do not edit.
#pragma once
#include <string_view>
namespace frog::native_engine::binding::generated {
inline constexpr unsigned schemaVersion = 1;
inline constexpr unsigned caseVersion = 1;
inline constexpr std::string_view caseId = "frog.structures.case";
inline constexpr std::string_view typeProfile = "frog.typed-binding@1";
inline constexpr std::string_view resolverId = "frog.structures.case.bindings@1";
inline constexpr std::string_view loweringId = "frog.structures.case.lowering@1";
inline constexpr std::string_view visualProfile = "studio.case";
inline constexpr std::string_view outerPort = "selector";
inline constexpr std::string_view innerPort = "selected_case";
inline constexpr std::string_view outerDirection = "input";
inline constexpr std::string_view innerDirection = "output";
inline constexpr std::string_view selectorVariable = "S";
inline constexpr std::string_view selectorExpression = "case_selector";
inline constexpr std::string_view selectorScope = "each_region";
inline constexpr std::string_view tunnelVariableScope = "per_boundary_id";
inline constexpr std::string_view tunnelTypeRelation = "exact";
inline constexpr std::string_view outputCoverage = "every_region";
inline constexpr std::string_view sourceCardinality = "single_per_region";
inline constexpr std::string_view persistedType = "validated_cache";
inline constexpr bool selectorReadOnly = true;
inline constexpr bool allowDefault = true;
inline constexpr bool everyRegion = outputCoverage == "every_region";
inline constexpr std::string_view sourceRepository = "https://github.com/Graiphic/FROG";
inline constexpr std::string_view sourceRelativePath = "Libraries/Structures/case.bindings.v1.json";
inline constexpr std::string_view sourceRevision = "55e4554b55d4738c8a8ef2e76338376f4eb84dd1";
inline constexpr std::string_view sourceSnapshotKind = "working-tree";
inline constexpr std::string_view sourceTracking = "untracked-at-snapshot";
inline constexpr std::string_view sourceSnapshotDate = "2026-09-07";
inline constexpr std::string_view sourceFingerprint = "472daefbe18f762aec93c45045c035cfc96d1587b0bc55f5e57eb13d4e828934";
inline constexpr std::string_view sourceLicense = "Apache-2.0";
inline constexpr std::string_view sourceLicensePath = "../../contracts/frog-source-v0.1/LICENSE";
inline constexpr std::string_view sourceNormalization = "utf8-crlf-to-lf-v1";
} // namespace frog::native_engine::binding::generated
