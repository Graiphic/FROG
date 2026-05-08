cmake_minimum_required(VERSION 3.20)
project(frog_reference_runtime_cpp LANGUAGES CXX)

option(
    FROG_RUNTIME_CPP_ENABLE_LLVM_KERNEL_BRIDGE
    "Build the optional Example 05 LLVM-produced native kernel bridge test."
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

add_executable(frog_reference_runtime_cpp src/main.cpp)
target_link_libraries(frog_reference_runtime_cpp PRIVATE frog_reference_runtime_cpp_lib)

if(WIN32)
    target_link_libraries(frog_reference_runtime_cpp_lib PRIVATE ws2_32)
endif()

enable_testing()
add_executable(frog_reference_runtime_cpp_tests tests/test_slice05.cpp)
target_link_libraries(frog_reference_runtime_cpp_tests PRIVATE frog_reference_runtime_cpp_lib)
add_test(NAME frog_reference_runtime_cpp_tests COMMAND frog_reference_runtime_cpp_tests)

if(FROG_RUNTIME_CPP_ENABLE_LLVM_KERNEL_BRIDGE)
    find_program(CLANG_EXECUTABLE NAMES clang REQUIRED)
    set(FROG_EXAMPLE05_KERNEL_LL
        "${CMAKE_CURRENT_SOURCE_DIR}/../../LLVM/examples/05_bounded_ui_accumulator/kernel.ll"
    )
    set(FROG_EXAMPLE05_KERNEL_OBJECT
        "${CMAKE_CURRENT_BINARY_DIR}/example05_kernel${CMAKE_C_OUTPUT_EXTENSION}"
    )

    add_custom_command(
        OUTPUT "${FROG_EXAMPLE05_KERNEL_OBJECT}"
        COMMAND "${CLANG_EXECUTABLE}" -c "${FROG_EXAMPLE05_KERNEL_LL}" -o "${FROG_EXAMPLE05_KERNEL_OBJECT}"
        DEPENDS "${FROG_EXAMPLE05_KERNEL_LL}"
        COMMENT "Compiling Example 05 LLVM native kernel artifact"
        VERBATIM
    )

    add_custom_target(frog_example05_llvm_kernel_object DEPENDS "${FROG_EXAMPLE05_KERNEL_OBJECT}")

    add_executable(
        frog_reference_runtime_cpp_llvm_kernel_tests
        tests/test_slice05_llvm_kernel.cpp
        "${FROG_EXAMPLE05_KERNEL_OBJECT}"
    )
    add_dependencies(frog_reference_runtime_cpp_llvm_kernel_tests frog_example05_llvm_kernel_object)
    target_link_libraries(frog_reference_runtime_cpp_llvm_kernel_tests PRIVATE frog_reference_runtime_cpp_lib)
    add_test(
        NAME frog_reference_runtime_cpp_llvm_kernel_tests
        COMMAND frog_reference_runtime_cpp_llvm_kernel_tests
    )
endif()
