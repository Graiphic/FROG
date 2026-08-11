if(NOT DEFINED SCAN_ROOT OR SCAN_ROOT STREQUAL "")
    message(FATAL_ERROR "SCAN_ROOT must identify the repository source tree to inspect.")
endif()

if(NOT DEFINED CANONICAL_CONTRACT_HEADER OR
   CANONICAL_CONTRACT_HEADER STREQUAL "" OR
   NOT EXISTS "${CANONICAL_CONTRACT_HEADER}")
    message(FATAL_ERROR
        "CANONICAL_CONTRACT_HEADER must identify the public FROG Runtime contract.")
endif()

file(REAL_PATH "${SCAN_ROOT}" normalized_scan_root)
file(REAL_PATH "${CANONICAL_CONTRACT_HEADER}" normalized_contract_header)

file(GLOB_RECURSE candidate_sources
    LIST_DIRECTORIES FALSE
    "${normalized_scan_root}/*.h"
    "${normalized_scan_root}/*.hpp"
    "${normalized_scan_root}/*.c"
    "${normalized_scan_root}/*.cc"
    "${normalized_scan_root}/*.cpp"
    "${normalized_scan_root}/*.cxx")

set(contract_types
    ArtifactId
    ArtifactRevision
    Diagnostic
    DocumentId
    EdgeId
    NodeId
    PortId
    ProgramArtifact
    ProtocolVersion
    RuntimeCapabilities
    RuntimeEdge
    RuntimeNode
    RuntimePort
    RuntimeValue
    SessionCommand
    SessionEvent
    SessionId
    SourceIdentity
    UiBinding
    WidgetId)

set(violations)
foreach(source_file IN LISTS candidate_sources)
    file(TO_CMAKE_PATH "${source_file}" normalized_source_file)
    if(normalized_source_file MATCHES "/(build[^/]*)/" OR
       normalized_source_file MATCHES "/third_party/" OR
       normalized_source_file MATCHES "/\\.git/")
        continue()
    endif()

    file(REAL_PATH "${source_file}" resolved_source_file)
    if(resolved_source_file STREQUAL normalized_contract_header)
        continue()
    endif()

    get_filename_component(source_name "${source_file}" NAME)
    if(source_name STREQUAL "runtime_contract.hpp")
        list(APPEND violations
            "${source_file}: copied public contract header is forbidden")
        continue()
    endif()

    file(READ "${source_file}" source_text)
    foreach(contract_type IN LISTS contract_types)
        string(REGEX MATCH
            "(struct|class)[ \t\r\n]+${contract_type}[ \t\r\n]*\\{"
            duplicate_definition
            "${source_text}")
        if(duplicate_definition)
            list(APPEND violations
                "${source_file}: private definition of ${contract_type}")
        endif()
    endforeach()
endforeach()

if(violations)
    list(JOIN violations "\n  - " formatted_violations)
    message(FATAL_ERROR
        "The public FROG Runtime contract must have one owner only:\n"
        "  - ${formatted_violations}\n"
        "Use ${normalized_contract_header} instead of copying or redefining it.")
endif()

message(STATUS
    "Runtime contract ownership verified for ${normalized_scan_root}")
