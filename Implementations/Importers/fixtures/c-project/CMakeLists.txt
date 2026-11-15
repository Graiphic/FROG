# SPDX-License-Identifier: Apache-2.0
cmake_minimum_required(VERSION 3.23)
project(FrogCProjectFixture LANGUAGES C)
configure_file(include/generated.h.in generated/generated.h @ONLY)
add_library(calculation STATIC src/entry.c src/middle.c src/leaf.c)
set_target_properties(calculation PROPERTIES C_STANDARD 17 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_include_directories(calculation PRIVATE include "${CMAKE_CURRENT_BINARY_DIR}/generated")
target_compile_definitions(calculation PRIVATE MODE_VALUE=$<IF:$<CONFIG:Debug>,7,19>)
if(MSVC)
    target_compile_options(calculation PRIVATE /FIforced.h)
else()
    target_compile_options(calculation PRIVATE -include "${CMAKE_CURRENT_SOURCE_DIR}/include/forced.h")
endif()
add_executable(reference src/reference.c)
target_include_directories(reference PRIVATE include)
target_link_libraries(reference PRIVATE calculation)

# These entry points are imported only to verify refusals. They are not linked
# into the positive C reference.
add_library(import_refusals STATIC EXCLUDE_FROM_ALL src/refusals.c)
add_library(build_order_only STATIC EXCLUDE_FROM_ALL src/build_order_only.c)
add_dependencies(import_refusals build_order_only)
