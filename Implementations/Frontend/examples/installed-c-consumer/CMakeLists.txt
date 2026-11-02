cmake_minimum_required(VERSION 3.24)
project(FrogFrontendInstalledCConsumer LANGUAGES C CXX)
if(NOT FROG_FRONTEND_INSTALLED_PREFIX OR NOT IS_ABSOLUTE "${FROG_FRONTEND_INSTALLED_PREFIX}")
    message(FATAL_ERROR "An explicit absolute FROG_FRONTEND_INSTALLED_PREFIX is required")
endif()
find_package(FrogFrontend CONFIG REQUIRED PATHS
    "${FROG_FRONTEND_INSTALLED_PREFIX}/lib/cmake/FrogFrontend" NO_DEFAULT_PATH)
add_executable(frog-frontend-installed-c-consumer
    "${CMAKE_CURRENT_LIST_DIR}/../../tests/frontend_c_consumer.c")
set_target_properties(frog-frontend-installed-c-consumer PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(frog-frontend-installed-c-consumer PRIVATE Frog::frog-frontend)
