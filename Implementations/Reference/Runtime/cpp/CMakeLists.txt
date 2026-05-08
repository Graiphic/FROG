cmake_minimum_required(VERSION 3.20)
project(frog_reference_runtime_cpp LANGUAGES CXX)

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
