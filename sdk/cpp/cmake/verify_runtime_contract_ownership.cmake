if(NOT DEFINED SCAN_ROOT OR SCAN_ROOT STREQUAL "")
    message(FATAL_ERROR "SCAN_ROOT must identify the repository source tree to inspect.")
endif()

if(NOT DEFINED CANONICAL_CONTRACT_HEADER OR
   CANONICAL_CONTRACT_HEADER STREQUAL "" OR
   NOT EXISTS "${CANONICAL_CONTRACT_HEADER}")
    message(FATAL_ERROR
        "CANONICAL_CONTRACT_HEADER must identify the public FROG Runtime contract.")
endif()

# Python is an explicit private validation-tool dependency, not a Runtime or
# language requirement. Never find an arbitrary interpreter or silently use an
# incomplete REAL_PATH result for Windows junctions.
if(NOT DEFINED FROG_OWNERSHIP_PYTHON_EXECUTABLE OR
   FROG_OWNERSHIP_PYTHON_EXECUTABLE STREQUAL "" OR
   NOT EXISTS "${FROG_OWNERSHIP_PYTHON_EXECUTABLE}")
    message(FATAL_ERROR
        "FROG_OWNERSHIP_PYTHON_EXECUTABLE must identify the explicitly selected Python 3.9+ validation interpreter.")
endif()
get_filename_component(normalized_scan_root "${SCAN_ROOT}" ABSOLUTE)
get_filename_component(normalized_contract_header "${CANONICAL_CONTRACT_HEADER}" ABSOLUTE)
file(TO_CMAKE_PATH "${normalized_scan_root}" normalized_scan_root)
file(TO_CMAKE_PATH "${normalized_contract_header}" normalized_contract_header)
# The helper may warm OS/Cloud IO with bounded original-file readers. Its
# inventory contains no content and CMake still READs and validates originals.
# Optional private diagnostics change neither inventory nor ownership checks.
# A whole-scan timeout can now distinguish OS discovery from JSON/READ/regex work.
set(ownership_diagnostics FALSE)
if(FROG_OWNERSHIP_DIAGNOSTICS OR "$ENV{FROG_OWNERSHIP_DIAGNOSTICS}" STREQUAL "1")
    set(ownership_diagnostics TRUE)
endif()
set(inventory_diagnostic_arguments)
set(inventory_echo_arguments)
if(ownership_diagnostics)
    list(APPEND inventory_diagnostic_arguments --diagnostics)
    list(APPEND inventory_echo_arguments ECHO_ERROR_VARIABLE)
    message(STATUS "OWNERSHIP_CHECK_DIAGNOSTIC phase=discovery_begin")
endif()

function(ownership_diagnostic_clock output)
    if(CMAKE_VERSION VERSION_LESS "3.23")
        string(TIMESTAMP ownership_clock "%s" UTC)
        math(EXPR ownership_clock "${ownership_clock} * 1000000")
    else()
        string(TIMESTAMP ownership_clock "%s%f" UTC)
    endif()
    set(${output} "${ownership_clock}" PARENT_SCOPE)
endfunction()

macro(ownership_diagnostic_accumulate phase started)
    ownership_diagnostic_clock(ownership_phase_end)
    math(EXPR ownership_${phase}_us "${ownership_${phase}_us} + ${ownership_phase_end} - ${${started}}")
endmacro()

macro(ownership_diagnostic_progress force)
    ownership_diagnostic_clock(ownership_progress_now)
    math(EXPR ownership_progress_delta "${ownership_progress_now} - ${ownership_last_progress}")
    if(${force} OR ownership_progress_delta GREATER_EQUAL 2000000)
        math(EXPR ownership_elapsed_us "${ownership_progress_now} - ${ownership_content_begin}")
        message(STATUS "OWNERSHIP_CHECK_DIAGNOSTIC phase=inspection records=${ownership_records} sources=${ownership_sources} reads=${ownership_reads} bytes=${ownership_bytes} regex=${ownership_regex} json_us=${ownership_json_us} read_us=${ownership_read_us} regex_us=${ownership_regex_us} elapsed_us=${ownership_elapsed_us}")
        set(ownership_last_progress "${ownership_progress_now}")
    endif()
endmacro()

execute_process(
    COMMAND "${FROG_OWNERSHIP_PYTHON_EXECUTABLE}" -B -X utf8
        "${CMAKE_CURRENT_LIST_DIR}/runtime_contract_source_inventory.py"
        --scan-root "${normalized_scan_root}"
        --canonical-header "${normalized_contract_header}"
        --prefetch
        ${inventory_diagnostic_arguments}
    RESULT_VARIABLE inventory_result
    OUTPUT_VARIABLE source_inventory
    ERROR_VARIABLE inventory_error
    ${inventory_echo_arguments})
if(NOT "${inventory_result}" STREQUAL "0")
    message(FATAL_ERROR
        "Runtime contract ownership discovery/prefetch failed; no source was silently omitted.\n${inventory_error}")
endif()
if(ownership_diagnostics)
    message(STATUS "OWNERSHIP_CHECK_DIAGNOSTIC phase=discovery_complete")
    foreach(phase json read regex)
        set(ownership_${phase}_us 0)
    endforeach()
    foreach(counter records sources reads bytes regex)
        set(ownership_${counter} 0)
    endforeach()
    ownership_diagnostic_clock(ownership_content_begin)
    set(ownership_last_progress "${ownership_content_begin}")
endif()
string(REPLACE "\r" "" source_inventory "${source_inventory}")
string(REPLACE "\n" ";" inventory_records "${source_inventory}")
set(canonical_inventory_seen FALSE)

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

# This is precisely the union of the twenty unchanged definition patterns.
# A negative union match can skip only work, never a READ or an assertion.
list(JOIN contract_types "|" contract_type_alternation)
set(contract_definition_union
    "(struct|class)[ \t\r\n]+(${contract_type_alternation})[ \t\r\n]*\\{")

set(violations)
foreach(inventory_record IN LISTS inventory_records)
    if(inventory_record STREQUAL "")
        continue()
    endif()
    if(ownership_diagnostics)
        math(EXPR ownership_records "${ownership_records} + 1")
        ownership_diagnostic_clock(ownership_json_begin)
    endif()
    string(JSON record_kind ERROR_VARIABLE record_error GET "${inventory_record}" kind)
    if(ownership_diagnostics)
        ownership_diagnostic_accumulate(json ownership_json_begin)
        ownership_diagnostic_progress(FALSE)
    endif()
    if(NOT record_error STREQUAL "NOTFOUND")
        message(FATAL_ERROR "Runtime contract ownership discovery returned malformed JSON: ${record_error}")
    endif()
    if(record_kind STREQUAL "canonical")
        if(canonical_inventory_seen)
            message(FATAL_ERROR "Runtime contract ownership discovery returned more than one canonical identity.")
        endif()
        if(ownership_diagnostics)
            ownership_diagnostic_clock(ownership_json_begin)
        endif()
        string(JSON canonical_physical_header GET "${inventory_record}" physical)
        if(ownership_diagnostics)
            ownership_diagnostic_accumulate(json ownership_json_begin)
        endif()
        set(canonical_inventory_seen TRUE)
        continue()
    elseif(record_kind STREQUAL "directory")
        continue()
    elseif(NOT record_kind STREQUAL "source" OR NOT canonical_inventory_seen)
        message(FATAL_ERROR "Runtime contract ownership discovery did not provide its canonical/source identities.")
    endif()
    if(ownership_diagnostics)
        math(EXPR ownership_sources "${ownership_sources} + 1")
        ownership_diagnostic_clock(ownership_json_begin)
    endif()
    string(JSON source_file GET "${inventory_record}" lexical)
    string(JSON resolved_source_file GET "${inventory_record}" physical)
    string(JSON source_read_file GET "${inventory_record}" read_path)
    if(ownership_diagnostics)
        ownership_diagnostic_accumulate(json ownership_json_begin)
    endif()
    file(TO_CMAKE_PATH "${source_file}" normalized_source_file)
    if(normalized_source_file MATCHES "/(build[^/]*)/" OR
       normalized_source_file MATCHES "/third_party/" OR
       normalized_source_file MATCHES "/\\.git/")
        continue()
    endif()

    if(resolved_source_file STREQUAL canonical_physical_header)
        continue()
    endif()

    get_filename_component(source_name "${source_file}" NAME)
    if(source_name STREQUAL "runtime_contract.hpp")
        list(APPEND violations
            "${source_file}: copied public contract header is forbidden")
        continue()
    endif()

    if(ownership_diagnostics)
        ownership_diagnostic_clock(ownership_read_begin)
    endif()
    file(READ "${source_read_file}" source_text)
    if(ownership_diagnostics)
        ownership_diagnostic_accumulate(read ownership_read_begin)
        math(EXPR ownership_reads "${ownership_reads} + 1")
        string(LENGTH "${source_text}" ownership_source_bytes)
        math(EXPR ownership_bytes "${ownership_bytes} + ${ownership_source_bytes}")
        ownership_diagnostic_clock(ownership_regex_begin)
    endif()
    string(REGEX MATCH "${contract_definition_union}" any_contract_definition "${source_text}")
    if(any_contract_definition)
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
    endif()
    if(ownership_diagnostics)
        ownership_diagnostic_accumulate(regex ownership_regex_begin)
        math(EXPR ownership_regex "${ownership_regex} + 1")
        if(any_contract_definition)
            list(LENGTH contract_types ownership_type_count)
            math(EXPR ownership_regex "${ownership_regex} + ${ownership_type_count}")
        endif()
        ownership_diagnostic_progress(FALSE)
    endif()
endforeach()

if(ownership_diagnostics)
    ownership_diagnostic_progress(TRUE)
endif()

if(NOT canonical_inventory_seen)
    message(FATAL_ERROR "Runtime contract ownership discovery returned no canonical identity.")
endif()

if(violations)
    list(JOIN violations "\n  - " formatted_violations)
    message(FATAL_ERROR
        "The public FROG Runtime contract must have one owner only:\n"
        "  - ${formatted_violations}\n"
        "Use ${normalized_contract_header} instead of copying or redefining it.")
endif()

message(STATUS
    "Runtime contract ownership verified for ${normalized_scan_root}")
