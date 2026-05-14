cmake_minimum_required(VERSION 3.20)
project(frog_reference_runtime_cpp LANGUAGES CXX)

option(
    FROG_RUNTIME_CPP_ENABLE_LLVM_KERNEL_BRIDGE
    "Build the optional Example 05/06/07/08/09 LLVM-produced native kernel bridge tests and runtime executable."
    OFF
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(
    frog_reference_runtime_cpp_lib
    src/json.cpp
    src/contract.cpp
    src/kernel_bridge.cpp
    src/runtime.cpp
    src/execute.cpp
    src/ui.cpp
)

target_include_directories(frog_reference_runtime_cpp_lib PUBLIC include)
target_compile_definitions(frog_reference_runtime_cpp_lib PUBLIC FROG_RUNTIME_CPP_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

function(frog_copy_mingw_runtime_dlls target_name)
    if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        get_filename_component(FROG_RUNTIME_CPP_COMPILER_BIN "${CMAKE_CXX_COMPILER}" DIRECTORY)
        foreach(runtime_dll IN ITEMS libstdc++-6.dll libgcc_s_seh-1.dll libwinpthread-1.dll)
            if(EXISTS "${FROG_RUNTIME_CPP_COMPILER_BIN}/${runtime_dll}")
                add_custom_command(
                    TARGET "${target_name}"
                    POST_BUILD
                    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                            "${FROG_RUNTIME_CPP_COMPILER_BIN}/${runtime_dll}"
                            "$<TARGET_FILE_DIR:${target_name}>/${runtime_dll}"
                    VERBATIM
                )
            endif()
        endforeach()
    endif()
endfunction()

add_executable(frog_reference_runtime_cpp src/main.cpp)
target_link_libraries(frog_reference_runtime_cpp PRIVATE frog_reference_runtime_cpp_lib)
frog_copy_mingw_runtime_dlls(frog_reference_runtime_cpp)

if(WIN32)
    target_link_libraries(frog_reference_runtime_cpp_lib PRIVATE ws2_32)
endif()

enable_testing()
add_executable(frog_reference_runtime_cpp_tests tests/test_slice05.cpp)
target_link_libraries(frog_reference_runtime_cpp_tests PRIVATE frog_reference_runtime_cpp_lib)
frog_copy_mingw_runtime_dlls(frog_reference_runtime_cpp_tests)
add_test(NAME frog_reference_runtime_cpp_tests COMMAND frog_reference_runtime_cpp_tests)

add_executable(frog_reference_runtime_cpp_slice06_tests tests/test_slice06_boolean.cpp)
target_link_libraries(frog_reference_runtime_cpp_slice06_tests PRIVATE frog_reference_runtime_cpp_lib)
frog_copy_mingw_runtime_dlls(frog_reference_runtime_cpp_slice06_tests)
add_test(NAME frog_reference_runtime_cpp_slice06_tests COMMAND frog_reference_runtime_cpp_slice06_tests)

add_executable(frog_reference_runtime_cpp_slice07_tests tests/test_slice07_string.cpp)
target_link_libraries(frog_reference_runtime_cpp_slice07_tests PRIVATE frog_reference_runtime_cpp_lib)
frog_copy_mingw_runtime_dlls(frog_reference_runtime_cpp_slice07_tests)
add_test(NAME frog_reference_runtime_cpp_slice07_tests COMMAND frog_reference_runtime_cpp_slice07_tests)

add_executable(frog_reference_runtime_cpp_slice08_tests tests/test_slice08_enum.cpp)
target_link_libraries(frog_reference_runtime_cpp_slice08_tests PRIVATE frog_reference_runtime_cpp_lib)
frog_copy_mingw_runtime_dlls(frog_reference_runtime_cpp_slice08_tests)
add_test(NAME frog_reference_runtime_cpp_slice08_tests COMMAND frog_reference_runtime_cpp_slice08_tests)

add_executable(frog_reference_runtime_cpp_slice09_tests tests/test_slice09_path.cpp)
target_link_libraries(frog_reference_runtime_cpp_slice09_tests PRIVATE frog_reference_runtime_cpp_lib)
frog_copy_mingw_runtime_dlls(frog_reference_runtime_cpp_slice09_tests)
add_test(NAME frog_reference_runtime_cpp_slice09_tests COMMAND frog_reference_runtime_cpp_slice09_tests)

add_executable(frog_reference_runtime_cpp_reference_examples_tests tests/test_reference_examples.cpp)
target_link_libraries(frog_reference_runtime_cpp_reference_examples_tests PRIVATE frog_reference_runtime_cpp_lib)
frog_copy_mingw_runtime_dlls(frog_reference_runtime_cpp_reference_examples_tests)
add_test(NAME frog_reference_runtime_cpp_reference_examples_tests COMMAND frog_reference_runtime_cpp_reference_examples_tests)

if(WIN32)
    get_filename_component(FROG_RUNTIME_CPP_COMPILER_BIN "${CMAKE_CXX_COMPILER}" DIRECTORY)
    set_tests_properties(
        frog_reference_runtime_cpp_tests
        frog_reference_runtime_cpp_slice06_tests
        frog_reference_runtime_cpp_slice07_tests
        frog_reference_runtime_cpp_slice08_tests
        frog_reference_runtime_cpp_slice09_tests
        frog_reference_runtime_cpp_reference_examples_tests
        PROPERTIES ENVIRONMENT "PATH=${FROG_RUNTIME_CPP_COMPILER_BIN}\\;$ENV{PATH}"
    )
endif()

if(FROG_RUNTIME_CPP_ENABLE_LLVM_KERNEL_BRIDGE)
    find_program(CLANG_EXECUTABLE NAMES clang REQUIRED)
    set(FROG_EXAMPLE05_KERNEL_LL
        "${CMAKE_CURRENT_SOURCE_DIR}/../../LLVM/examples/05_bounded_ui_accumulator/kernel.ll"
    )
    set(FROG_EXAMPLE05_KERNEL_OBJECT
        "${CMAKE_CURRENT_BINARY_DIR}/example05_kernel${CMAKE_CXX_OUTPUT_EXTENSION}"
    )
    set(FROG_EXAMPLE06_KERNEL_LL
        "${CMAKE_CURRENT_SOURCE_DIR}/../../LLVM/examples/06_boolean_value_roundtrip/kernel.ll"
    )
    set(FROG_EXAMPLE06_KERNEL_OBJECT
        "${CMAKE_CURRENT_BINARY_DIR}/example06_kernel${CMAKE_CXX_OUTPUT_EXTENSION}"
    )
    set(FROG_EXAMPLE07_KERNEL_LL
        "${CMAKE_CURRENT_SOURCE_DIR}/../../LLVM/examples/07_string_value_roundtrip/kernel.ll"
    )
    set(FROG_EXAMPLE07_KERNEL_OBJECT
        "${CMAKE_CURRENT_BINARY_DIR}/example07_kernel${CMAKE_CXX_OUTPUT_EXTENSION}"
    )
    set(FROG_EXAMPLE08_KERNEL_LL
        "${CMAKE_CURRENT_SOURCE_DIR}/../../LLVM/examples/08_enum_value_roundtrip/kernel.ll"
    )
    set(FROG_EXAMPLE08_KERNEL_OBJECT
        "${CMAKE_CURRENT_BINARY_DIR}/example08_kernel${CMAKE_CXX_OUTPUT_EXTENSION}"
    )
    set(FROG_EXAMPLE09_KERNEL_LL
        "${CMAKE_CURRENT_SOURCE_DIR}/../../LLVM/examples/09_path_value_roundtrip/kernel.ll"
    )
    set(FROG_EXAMPLE09_KERNEL_OBJECT
        "${CMAKE_CURRENT_BINARY_DIR}/example09_kernel${CMAKE_CXX_OUTPUT_EXTENSION}"
    )

    add_custom_command(
        OUTPUT "${FROG_EXAMPLE05_KERNEL_OBJECT}"
        COMMAND "${CLANG_EXECUTABLE}" -c "${FROG_EXAMPLE05_KERNEL_LL}" -o "${FROG_EXAMPLE05_KERNEL_OBJECT}"
        DEPENDS "${FROG_EXAMPLE05_KERNEL_LL}"
        COMMENT "Compiling Example 05 LLVM native kernel artifact"
        VERBATIM
    )

    add_custom_target(frog_example05_llvm_kernel_object DEPENDS "${FROG_EXAMPLE05_KERNEL_OBJECT}")

    add_custom_command(
        OUTPUT "${FROG_EXAMPLE06_KERNEL_OBJECT}"
        COMMAND "${CLANG_EXECUTABLE}" -c "${FROG_EXAMPLE06_KERNEL_LL}" -o "${FROG_EXAMPLE06_KERNEL_OBJECT}"
        DEPENDS "${FROG_EXAMPLE06_KERNEL_LL}"
        COMMENT "Compiling Example 06 LLVM native bool kernel artifact"
        VERBATIM
    )

    add_custom_target(frog_example06_llvm_kernel_object DEPENDS "${FROG_EXAMPLE06_KERNEL_OBJECT}")

    add_custom_command(
        OUTPUT "${FROG_EXAMPLE07_KERNEL_OBJECT}"
        COMMAND "${CLANG_EXECUTABLE}" -c "${FROG_EXAMPLE07_KERNEL_LL}" -o "${FROG_EXAMPLE07_KERNEL_OBJECT}"
        DEPENDS "${FROG_EXAMPLE07_KERNEL_LL}"
        COMMENT "Compiling Example 07 LLVM native string kernel artifact"
        VERBATIM
    )

    add_custom_target(frog_example07_llvm_kernel_object DEPENDS "${FROG_EXAMPLE07_KERNEL_OBJECT}")

    add_custom_command(
        OUTPUT "${FROG_EXAMPLE08_KERNEL_OBJECT}"
        COMMAND "${CLANG_EXECUTABLE}" -c "${FROG_EXAMPLE08_KERNEL_LL}" -o "${FROG_EXAMPLE08_KERNEL_OBJECT}"
        DEPENDS "${FROG_EXAMPLE08_KERNEL_LL}"
        COMMENT "Compiling Example 08 LLVM native enum kernel artifact"
        VERBATIM
    )

    add_custom_target(frog_example08_llvm_kernel_object DEPENDS "${FROG_EXAMPLE08_KERNEL_OBJECT}")

    add_custom_command(
        OUTPUT "${FROG_EXAMPLE09_KERNEL_OBJECT}"
        COMMAND "${CLANG_EXECUTABLE}" -c "${FROG_EXAMPLE09_KERNEL_LL}" -o "${FROG_EXAMPLE09_KERNEL_OBJECT}"
        DEPENDS "${FROG_EXAMPLE09_KERNEL_LL}"
        COMMENT "Compiling Example 09 LLVM native path kernel artifact"
        VERBATIM
    )

    add_custom_target(frog_example09_llvm_kernel_object DEPENDS "${FROG_EXAMPLE09_KERNEL_OBJECT}")

    add_executable(
        frog_reference_runtime_cpp_llvm_kernel_tests
        tests/test_slice05_llvm_kernel.cpp
        "${FROG_EXAMPLE05_KERNEL_OBJECT}"
    )
    add_dependencies(frog_reference_runtime_cpp_llvm_kernel_tests frog_example05_llvm_kernel_object)
    target_link_libraries(frog_reference_runtime_cpp_llvm_kernel_tests PRIVATE frog_reference_runtime_cpp_lib)
    frog_copy_mingw_runtime_dlls(frog_reference_runtime_cpp_llvm_kernel_tests)
    add_test(
        NAME frog_reference_runtime_cpp_llvm_kernel_tests
        COMMAND frog_reference_runtime_cpp_llvm_kernel_tests
    )
    if(WIN32)
        set_tests_properties(
            frog_reference_runtime_cpp_llvm_kernel_tests
            PROPERTIES ENVIRONMENT "PATH=${FROG_RUNTIME_CPP_COMPILER_BIN}\\;$ENV{PATH}"
        )
    endif()

    add_executable(
        frog_reference_runtime_cpp_llvm_bool_kernel_tests
        tests/test_slice06_llvm_kernel.cpp
        "${FROG_EXAMPLE06_KERNEL_OBJECT}"
    )
    add_dependencies(frog_reference_runtime_cpp_llvm_bool_kernel_tests frog_example06_llvm_kernel_object)
    target_link_libraries(frog_reference_runtime_cpp_llvm_bool_kernel_tests PRIVATE frog_reference_runtime_cpp_lib)
    frog_copy_mingw_runtime_dlls(frog_reference_runtime_cpp_llvm_bool_kernel_tests)
    add_test(
        NAME frog_reference_runtime_cpp_llvm_bool_kernel_tests
        COMMAND frog_reference_runtime_cpp_llvm_bool_kernel_tests
    )
    if(WIN32)
        set_tests_properties(
            frog_reference_runtime_cpp_llvm_bool_kernel_tests
            PROPERTIES ENVIRONMENT "PATH=${FROG_RUNTIME_CPP_COMPILER_BIN}\\;$ENV{PATH}"
        )
    endif()

    add_executable(
        frog_reference_runtime_cpp_llvm_string_kernel_tests
        tests/test_slice07_llvm_kernel.cpp
        "${FROG_EXAMPLE07_KERNEL_OBJECT}"
    )
    add_dependencies(frog_reference_runtime_cpp_llvm_string_kernel_tests frog_example07_llvm_kernel_object)
    target_link_libraries(frog_reference_runtime_cpp_llvm_string_kernel_tests PRIVATE frog_reference_runtime_cpp_lib)
    frog_copy_mingw_runtime_dlls(frog_reference_runtime_cpp_llvm_string_kernel_tests)
    add_test(
        NAME frog_reference_runtime_cpp_llvm_string_kernel_tests
        COMMAND frog_reference_runtime_cpp_llvm_string_kernel_tests
    )
    if(WIN32)
        set_tests_properties(
            frog_reference_runtime_cpp_llvm_string_kernel_tests
            PROPERTIES ENVIRONMENT "PATH=${FROG_RUNTIME_CPP_COMPILER_BIN}\\;$ENV{PATH}"
        )
    endif()

    add_executable(
        frog_reference_runtime_cpp_llvm_enum_kernel_tests
        tests/test_slice08_llvm_kernel.cpp
        "${FROG_EXAMPLE08_KERNEL_OBJECT}"
    )
    add_dependencies(frog_reference_runtime_cpp_llvm_enum_kernel_tests frog_example08_llvm_kernel_object)
    target_link_libraries(frog_reference_runtime_cpp_llvm_enum_kernel_tests PRIVATE frog_reference_runtime_cpp_lib)
    frog_copy_mingw_runtime_dlls(frog_reference_runtime_cpp_llvm_enum_kernel_tests)
    add_test(
        NAME frog_reference_runtime_cpp_llvm_enum_kernel_tests
        COMMAND frog_reference_runtime_cpp_llvm_enum_kernel_tests
    )
    if(WIN32)
        set_tests_properties(
            frog_reference_runtime_cpp_llvm_enum_kernel_tests
            PROPERTIES ENVIRONMENT "PATH=${FROG_RUNTIME_CPP_COMPILER_BIN}\\;$ENV{PATH}"
        )
    endif()

    add_executable(
        frog_reference_runtime_cpp_llvm_path_kernel_tests
        tests/test_slice09_llvm_kernel.cpp
        "${FROG_EXAMPLE09_KERNEL_OBJECT}"
    )
    add_dependencies(frog_reference_runtime_cpp_llvm_path_kernel_tests frog_example09_llvm_kernel_object)
    target_link_libraries(frog_reference_runtime_cpp_llvm_path_kernel_tests PRIVATE frog_reference_runtime_cpp_lib)
    frog_copy_mingw_runtime_dlls(frog_reference_runtime_cpp_llvm_path_kernel_tests)
    add_test(
        NAME frog_reference_runtime_cpp_llvm_path_kernel_tests
        COMMAND frog_reference_runtime_cpp_llvm_path_kernel_tests
    )
    if(WIN32)
        set_tests_properties(
            frog_reference_runtime_cpp_llvm_path_kernel_tests
            PROPERTIES ENVIRONMENT "PATH=${FROG_RUNTIME_CPP_COMPILER_BIN}\\;$ENV{PATH}"
        )
    endif()

    add_executable(
        frog_reference_runtime_cpp_llvm_kernel
        src/main_llvm_kernel.cpp
        "${FROG_EXAMPLE05_KERNEL_OBJECT}"
        "${FROG_EXAMPLE06_KERNEL_OBJECT}"
        "${FROG_EXAMPLE07_KERNEL_OBJECT}"
        "${FROG_EXAMPLE08_KERNEL_OBJECT}"
        "${FROG_EXAMPLE09_KERNEL_OBJECT}"
    )
    add_dependencies(frog_reference_runtime_cpp_llvm_kernel frog_example05_llvm_kernel_object frog_example06_llvm_kernel_object frog_example07_llvm_kernel_object frog_example08_llvm_kernel_object frog_example09_llvm_kernel_object)
    target_link_libraries(frog_reference_runtime_cpp_llvm_kernel PRIVATE frog_reference_runtime_cpp_lib)
    frog_copy_mingw_runtime_dlls(frog_reference_runtime_cpp_llvm_kernel)
endif()
