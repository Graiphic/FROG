cmake_minimum_required(VERSION 3.24)
if(POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
endif()
# Use the canonical embedded debug format for compiler probes too. A shared
# PDB server must not retain the probe's redirected child-process pipes.
set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT Embedded)
project(FrogFrontend VERSION 0.2.0 LANGUAGES C CXX)
if(PROJECT_IS_TOP_LEVEL)
    include(cmake/VerifyMsvcIncludeDependencies.cmake)
endif()

option(FROG_FRONTEND_BUILD_TESTS "Build the bounded public Frontend checks" OFF)
set(FROG_FRONTEND_SOFTWARE_VERSION "0.2.0-local")
set_property(GLOBAL PROPERTY FROG_FRONTEND_SOFTWARE_VERSION "${FROG_FRONTEND_SOFTWARE_VERSION}")
set(FROG_FRONTEND_BASE_REVISION "32c8fee2f22a1231a21de7bc1965652d1e6b3f17")
include(production-sources.cmake)

# Generated declaration inputs are reviewed snapshots, not build-directory fallbacks.
# The parent IDE compiles exactly this owner. A standalone build reads no other repo.
include(cmake/FrontendInputs.cmake)
frog_frontend_input_identity("${CMAKE_CURRENT_SOURCE_DIR}" FROG_FRONTEND_CONTENT_SHA256 content_identity)
set_property(GLOBAL PROPERTY FROG_FRONTEND_CONTENT_SHA256 "${FROG_FRONTEND_CONTENT_SHA256}")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/frontend-content-identity.txt" "${FROG_FRONTEND_CONTENT_SHA256}\n${content_identity}")

# Existing Zstandard 1.5.7 document codec, including the same bounded writer
# already used by Studio. No network, dictionary builder, threads or runtime service.
add_library(frog-frontend-zstd STATIC
    third_party/zstd/lib/common/debug.c
    third_party/zstd/lib/common/entropy_common.c
    third_party/zstd/lib/common/error_private.c
    third_party/zstd/lib/common/fse_decompress.c
    third_party/zstd/lib/common/xxhash.c
    third_party/zstd/lib/common/zstd_common.c
    third_party/zstd/lib/decompress/huf_decompress.c
    third_party/zstd/lib/decompress/zstd_ddict.c
    third_party/zstd/lib/decompress/zstd_decompress.c
    third_party/zstd/lib/decompress/zstd_decompress_block.c
    third_party/zstd/lib/compress/fse_compress.c
    third_party/zstd/lib/compress/hist.c
    third_party/zstd/lib/compress/huf_compress.c
    third_party/zstd/lib/compress/zstd_compress.c
    third_party/zstd/lib/compress/zstd_compress_literals.c
    third_party/zstd/lib/compress/zstd_compress_sequences.c
    third_party/zstd/lib/compress/zstd_compress_superblock.c
    third_party/zstd/lib/compress/zstd_double_fast.c
    third_party/zstd/lib/compress/zstd_fast.c
    third_party/zstd/lib/compress/zstd_lazy.c
    third_party/zstd/lib/compress/zstd_ldm.c
    third_party/zstd/lib/compress/zstd_opt.c
    third_party/zstd/lib/compress/zstd_preSplit.c)
target_include_directories(frog-frontend-zstd PRIVATE third_party/zstd/lib)
target_compile_definitions(frog-frontend-zstd PRIVATE ZSTD_DISABLE_ASM=1 ZSTD_LEGACY_SUPPORT=0)
add_library(frog-frontend-production STATIC ${FROG_FRONTEND_PRODUCTION_SOURCES})
target_compile_features(frog-frontend-production PUBLIC cxx_std_20)
target_link_libraries(frog-frontend-production PRIVATE frog-frontend-zstd)
target_include_directories(frog-frontend-production PRIVATE third_party/zstd/lib)
target_include_directories(frog-frontend-production PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/snapshot/sdk/include>"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/snapshot/generated")
if(MSVC)
    target_compile_options(frog-frontend-production PRIVATE /utf-8 /W4 /FS /bigobj)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/FrontendOperation.cpp")
    add_library(frog-frontend STATIC src/FrontendApi.cpp src/FrontendOperation.cpp)
    target_compile_features(frog-frontend PUBLIC cxx_std_20)
    target_include_directories(frog-frontend PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/snapshot/api>"
        "$<INSTALL_INTERFACE:include>")
    target_link_libraries(frog-frontend PRIVATE frog-frontend-production)
    target_compile_definitions(frog-frontend PRIVATE
        FROG_FRONTEND_SOFTWARE_VERSION="${FROG_FRONTEND_SOFTWARE_VERSION}"
        FROG_FRONTEND_CONTENT_SHA256="${FROG_FRONTEND_CONTENT_SHA256}"
        FROG_FRONTEND_BASE_REVISION="${FROG_FRONTEND_BASE_REVISION}")
    add_executable(frog-frontend-cli src/FrontendCli.cpp)
    set_target_properties(frog-frontend-cli PROPERTIES OUTPUT_NAME frog-frontend)
    target_compile_features(frog-frontend-cli PRIVATE cxx_std_20)
    target_link_libraries(frog-frontend-cli PRIVATE frog-frontend frog-frontend-production)
    target_include_directories(frog-frontend-cli PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
    if(MSVC)
        target_compile_options(frog-frontend PRIVATE /utf-8 /W4 /FS /bigobj)
        target_compile_options(frog-frontend-cli PRIVATE /utf-8 /W4 /FS)
    endif()
    install(TARGETS frog-frontend frog-frontend-production frog-frontend-zstd frog-frontend-cli
        EXPORT FrogFrontendTargets ARCHIVE DESTINATION lib RUNTIME DESTINATION bin)
    install(FILES snapshot/api/frog/frontend.h DESTINATION include/frog)
    install(EXPORT FrogFrontendTargets NAMESPACE Frog:: DESTINATION lib/cmake/FrogFrontend)
    install(FILES cmake/FrogFrontendConfig.cmake DESTINATION lib/cmake/FrogFrontend)
    if(FROG_FRONTEND_BUILD_TESTS)
        enable_testing()
        add_executable(frog-frontend-draft-integration tests/frontend_draft_integration.cpp)
        target_link_libraries(frog-frontend-draft-integration PRIVATE frog-frontend frog-frontend-production)
        add_test(NAME frog-frontend-draft-integration COMMAND frog-frontend-draft-integration)
        set_tests_properties(frog-frontend-draft-integration PROPERTIES TIMEOUT 120 LABELS "frontend;public;draft;exact-types;focused")
        add_executable(frog-frontend-draft-dependency-smoke tests/draft_dependency_smoke.cpp)
        target_link_libraries(frog-frontend-draft-dependency-smoke PRIVATE frog-frontend-production)
        add_test(NAME frog-frontend-draft-dependency-smoke COMMAND frog-frontend-draft-dependency-smoke)
        set_tests_properties(frog-frontend-draft-dependency-smoke PROPERTIES TIMEOUT 90 LABELS "frontend;public;draft;dependency;focused")
        find_package(Threads REQUIRED)
        find_package(Python3 COMPONENTS Interpreter REQUIRED)
        add_executable(frog-frontend-c-consumer tests/frontend_c_consumer.c)
        set_target_properties(frog-frontend-c-consumer PROPERTIES LINKER_LANGUAGE CXX)
        target_link_libraries(frog-frontend-c-consumer PRIVATE frog-frontend)
        add_test(NAME frog-frontend-c-consumer COMMAND frog-frontend-c-consumer)
        add_test(NAME frog-frontend-cli-scalar COMMAND "${Python3_EXECUTABLE}" -B -X utf8
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/frontend_cli_checks.py"
            --cli $<TARGET_FILE:frog-frontend-cli> --source "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/pure-add.frog"
            --source-format frog.source --source-version 0.1 --fir-version 0.1
            --c-consumer $<TARGET_FILE:frog-frontend-c-consumer>)
        add_test(NAME frog-frontend-cli-type-definitions COMMAND "${Python3_EXECUTABLE}" -B -X utf8
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/frontend_cli_checks.py"
            --cli $<TARGET_FILE:frog-frontend-cli> --source "${CMAKE_CURRENT_SOURCE_DIR}/examples/type-definition/main.frog"
            --source-format frog.source --source-version 0.1 --fir-version 0.1
            --semantic-context "${CMAKE_CURRENT_SOURCE_DIR}/examples/type-definition/context.json"
            --dependencies "${CMAKE_CURRENT_SOURCE_DIR}/examples/type-definition/dependencies.json"
            --check-dependency-failures)
        set_tests_properties(frog-frontend-c-consumer frog-frontend-cli-scalar frog-frontend-cli-type-definitions
            PROPERTIES TIMEOUT 120 LABELS "frontend;public;contract;focused")
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/frontend_integration.cpp")
            add_executable(frog-frontend-integration tests/frontend_integration.cpp)
            target_link_libraries(frog-frontend-integration PRIVATE frog-frontend frog-frontend-production Threads::Threads)
            target_include_directories(frog-frontend-integration PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
            target_compile_definitions(frog-frontend-integration PRIVATE
                FROG_FRONTEND_FIXTURE_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures")
            add_test(NAME frog-frontend-integration COMMAND frog-frontend-integration)
        endif()
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/frontend_phase_benchmark.cpp")
            add_executable(frog-frontend-phase-benchmark tests/frontend_phase_benchmark.cpp)
            target_link_libraries(frog-frontend-phase-benchmark PRIVATE frog-frontend frog-frontend-production)
            target_include_directories(frog-frontend-phase-benchmark PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
        endif()
    endif()
endif()
