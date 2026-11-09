# A compiler cache can outlive the console code page used to create it.
# Dependency consumers then fail to recognize localized /showIncludes lines
# even though compilation succeeds. Probe the actual consumer on every
# configure, and require it to record the known header before generating rules.
function(frog_verify_msvc_include_dependencies)
    if(NOT MSVC OR NOT CMAKE_GENERATOR MATCHES "^(NMake|Ninja)")
        return()
    endif()

    set(probe_dir "${CMAKE_BINARY_DIR}/CMakeFiles/FrogIncludeDependencies")
    file(MAKE_DIRECTORY "${probe_dir}")
    file(WRITE "${probe_dir}/frog_dependency_probe.hpp"
        "inline int frog_dependency_probe_value() { return 1; }\n")
    file(WRITE "${probe_dir}/probe.cpp"
        "#include \"frog_dependency_probe.hpp\"\nint frog_dependency_probe() { return frog_dependency_probe_value(); }\n")
    separate_arguments(probe_flags NATIVE_COMMAND "${CMAKE_CXX_FLAGS}")
    separate_arguments(compiler_arg1 NATIVE_COMMAND "${CMAKE_CXX_COMPILER_ARG1}")
    set(compiler_command "${CMAKE_CXX_COMPILER}" ${compiler_arg1}
        ${probe_flags} /nologo /utf-8 /showIncludes /c)

    if(CMAKE_GENERATOR MATCHES "^Ninja")
        # Ninja matches compiler-output bytes, unlike NMake's decoding helper.
        # OUTPUT_VARIABLE/ENCODING can replace a localized non-UTF-8 byte and
        # silently produce a prefix which records zero dependencies.
        execute_process(
            COMMAND ${compiler_command} /Fodetect-ninja.obj probe.cpp
            WORKING_DIRECTORY "${probe_dir}"
            OUTPUT_FILE "${probe_dir}/detect-ninja.stdout"
            ERROR_FILE "${probe_dir}/detect-ninja.stderr"
            RESULT_VARIABLE probe_result TIMEOUT 30)
        file(READ "${probe_dir}/detect-ninja.stdout" probe_output)
        file(READ "${probe_dir}/detect-ninja.stderr" probe_error)
        string(APPEND probe_output "\n${probe_error}")
        if(NOT probe_result STREQUAL "0" OR NOT probe_output MATCHES
                "(^|\n)([^\r\n]*)([A-Za-z]:[/\\\\][^\r\n]*frog_dependency_probe\\.hpp)")
            message(FATAL_ERROR
                "Cannot detect the raw MSVC /showIncludes prefix for Ninja "
                "(${probe_result}).\n${probe_output}")
        endif()
        set(include_prefix "${CMAKE_MATCH_2}")

        # Exercise Ninja itself, not an approximation of its output parser.
        # Keep the native compiler flags intact and escape Ninja's $ syntax.
        set(ninja_compiler_command
            "\"${CMAKE_CXX_COMPILER}\" ${CMAKE_CXX_COMPILER_ARG1} ${CMAKE_CXX_FLAGS} /nologo /utf-8 /showIncludes /c")
        string(REPLACE "$" "$$" ninja_compiler_command "${ninja_compiler_command}")
        string(REPLACE "$" "$$" ninja_prefix "${include_prefix}")
        file(WRITE "${probe_dir}/probe.ninja"
            "rule frog_include_probe\n"
            "  command = ${ninja_compiler_command} /Foprobe-ninja.obj probe.cpp\n"
            "  description = FROG_NINJA_INCLUDE_PROBE_COMPILE\n"
            "  deps = msvc\n"
            "  msvc_deps_prefix = ${ninja_prefix}\n"
            "build probe-ninja.obj: frog_include_probe probe.cpp\n")
        file(REMOVE "${probe_dir}/probe-ninja.obj"
            "${probe_dir}/.ninja_deps" "${probe_dir}/.ninja_log")
        execute_process(
            COMMAND "${CMAKE_MAKE_PROGRAM}" -f probe.ninja probe-ninja.obj
            WORKING_DIRECTORY "${probe_dir}"
            OUTPUT_VARIABLE verify_output ERROR_VARIABLE verify_error
            RESULT_VARIABLE verify_result TIMEOUT 30)
        execute_process(
            COMMAND "${CMAKE_MAKE_PROGRAM}" -f probe.ninja -t deps probe-ninja.obj
            WORKING_DIRECTORY "${probe_dir}"
            OUTPUT_VARIABLE dependency_output ERROR_VARIABLE dependency_error
            RESULT_VARIABLE dependency_result TIMEOUT 30)
        set(dependency_verified FALSE)
        file(REAL_PATH "${probe_dir}/frog_dependency_probe.hpp" expected_header)
        string(REPLACE "\n" ";" dependencies "${dependency_output}")
        foreach(dependency IN LISTS dependencies)
            string(STRIP "${dependency}" dependency)
            file(TO_CMAKE_PATH "${dependency}" dependency)
            if(NOT IS_ABSOLUTE "${dependency}")
                set(dependency "${probe_dir}/${dependency}")
            endif()
            if(EXISTS "${dependency}")
                file(REAL_PATH "${dependency}" actual_header)
                if(actual_header STREQUAL expected_header)
                    set(dependency_verified TRUE)
                endif()
            endif()
        endforeach()
        if(NOT verify_result STREQUAL "0" OR NOT dependency_result STREQUAL "0" OR
                NOT dependency_verified)
            message(FATAL_ERROR
                "Ninja did not record the MSVC probe's included header "
                "(compile: ${verify_result}; dependency query: ${dependency_result}); "
                "refusing to generate unsafe build rules.\n"
                "${verify_output}\n${verify_error}\n${dependency_output}\n${dependency_error}")
        endif()
        execute_process(COMMAND "${CMAKE_COMMAND}" -E sleep 0.05)
        file(APPEND "${probe_dir}/frog_dependency_probe.hpp"
            "inline int frog_dependency_probe_changed() { return 2; }\n")
        execute_process(
            COMMAND "${CMAKE_MAKE_PROGRAM}" -f probe.ninja probe-ninja.obj
            WORKING_DIRECTORY "${probe_dir}"
            OUTPUT_VARIABLE rebuild_output ERROR_VARIABLE rebuild_error
            RESULT_VARIABLE rebuild_result TIMEOUT 30)
        # A workspace watcher can briefly hold the just-written probe object.
        # Retry only that exact compiler failure; all other failures stay fatal.
        foreach(retry_delay IN ITEMS 0.5 1 2)
            if(rebuild_result STREQUAL "0" OR NOT
                    "${rebuild_output}\n${rebuild_error}" MATCHES
                    "C1083[^\r\n]*probe-ninja\\.obj[^\r\n]*Permission denied")
                break()
            endif()
            message(STATUS "MSVC Ninja probe object temporarily locked; retrying in ${retry_delay}s")
            execute_process(COMMAND "${CMAKE_COMMAND}" -E sleep "${retry_delay}")
            execute_process(
                COMMAND "${CMAKE_MAKE_PROGRAM}" -f probe.ninja probe-ninja.obj
                WORKING_DIRECTORY "${probe_dir}"
                OUTPUT_VARIABLE rebuild_output ERROR_VARIABLE rebuild_error
                RESULT_VARIABLE rebuild_result TIMEOUT 30)
        endforeach()
        if(NOT rebuild_result STREQUAL "0" OR NOT rebuild_output MATCHES
                "FROG_NINJA_INCLUDE_PROBE_COMPILE")
            message(FATAL_ERROR
                "Changing an included header did not rebuild the Ninja MSVC probe "
                "(${rebuild_result}); "
                "refusing to generate unsafe build rules.\n${rebuild_output}\n${rebuild_error}")
        endif()
        # Keep the byte-exact prefix above for the Ninja consumer probe, but
        # CMake's Ninja generator expects its input String in UTF-8 and applies
        # WriteAltEncoding(ConsoleOutput) when emitting msvc_deps_prefix.
        # Passing raw localized bytes here can fail that stream silently and
        # leave rules.ninja stale while build.ninja is successfully updated.
        # The canonical wrapper starts configure/build without a console:
        # AUTO decoding and ConsoleOutput encoding then both use Windows ACP.
        # Decode the existing capture; do not compile the probe again or force
        # OEM (which need not be CMake's output code page).
        execute_process(
            COMMAND "${CMAKE_COMMAND}" -E cat
                "${probe_dir}/detect-ninja.stdout" "${probe_dir}/detect-ninja.stderr"
            OUTPUT_VARIABLE cmake_probe_output ERROR_VARIABLE decode_error
            RESULT_VARIABLE decode_result ENCODING AUTO TIMEOUT 10)
        if(NOT decode_result STREQUAL "0" OR NOT cmake_probe_output MATCHES
                "(^|\n)([^\r\n]*)([A-Za-z]:[/\\\\][^\r\n]*frog_dependency_probe\\.hpp)")
            message(FATAL_ERROR
                "Cannot decode the verified MSVC /showIncludes prefix for "
                "CMake's Ninja generator (${decode_result}).\n${decode_error}")
        endif()
        set(CMAKE_CXX_CL_SHOWINCLUDES_PREFIX "${CMAKE_MATCH_2}" PARENT_SCOPE)
        set(CMAKE_CL_SHOWINCLUDES_PREFIX "${CMAKE_MATCH_2}" PARENT_SCOPE)
        message(STATUS "MSVC Ninja include dependencies and header-change rebuild verified")
        return()
    endif()

    set(helper_command "${CMAKE_COMMAND}" -E cmake_cl_compile_depends
        "--working-dir=${probe_dir}")

    # An intentionally unmatched prefix lets the helper emit the decoded
    # include line. Its output is UTF-8, independently of cl's output encoding.
    execute_process(
        COMMAND ${helper_command}
            "--dep-file=${probe_dir}/detect.obj.d"
            --filter-prefix=FROG_UNMATCHED_INCLUDE_PREFIX -- ${compiler_command}
            /Fodetect.obj probe.cpp
        WORKING_DIRECTORY "${probe_dir}"
        OUTPUT_VARIABLE probe_output ERROR_VARIABLE probe_error
        RESULT_VARIABLE probe_result ENCODING UTF-8 TIMEOUT 30)
    if(NOT probe_result STREQUAL "0" OR NOT probe_output MATCHES
            "(^|\n)([^:\n][^:\n]+:[^:\n]*[^: \n][^: \n]:?[ \t]+)([A-Za-z]:\\\\|\\\\\\\\|\\./|\\.\\\\|/)")
        message(FATAL_ERROR
            "Cannot detect the MSVC /showIncludes prefix through the NMake "
            "dependency helper (${probe_result}).\n${probe_output}\n${probe_error}")
    endif()
    set(include_prefix "${CMAKE_MATCH_2}")

    # Do not trust detection alone: run the exact dependency producer again
    # and check that the recorded path resolves to our header, not stale data.
    # Distinct outputs avoid immediately reopening the detection object while
    # a file watcher or scanner still holds it in a synchronized workspace.
    file(REMOVE "${probe_dir}/probe.obj.d")
    execute_process(
        COMMAND ${helper_command} "--dep-file=${probe_dir}/probe.obj.d"
            "--filter-prefix=${include_prefix}"
            -- ${compiler_command} /Foprobe.obj probe.cpp
        WORKING_DIRECTORY "${probe_dir}"
        OUTPUT_VARIABLE verify_output ERROR_VARIABLE verify_error
        RESULT_VARIABLE verify_result ENCODING UTF-8 TIMEOUT 30)
    set(dependency_verified FALSE)
    if(verify_result STREQUAL "0" AND EXISTS "${probe_dir}/probe.obj.d")
        file(STRINGS "${probe_dir}/probe.obj.d" dependencies ENCODING UTF-8)
        file(REAL_PATH "${probe_dir}/frog_dependency_probe.hpp" expected_header)
        foreach(dependency IN LISTS dependencies)
            file(TO_CMAKE_PATH "${dependency}" dependency)
            if(NOT EXISTS "${dependency}")
                continue()
            endif()
            file(REAL_PATH "${dependency}" actual_header BASE_DIRECTORY "${probe_dir}")
            if(actual_header STREQUAL expected_header)
                set(dependency_verified TRUE)
            endif()
        endforeach()
    endif()
    if(NOT dependency_verified)
        message(FATAL_ERROR
            "The MSVC dependency probe did not record its included header "
            "(${verify_result}); refusing to generate unsafe NMake rules.\n"
            "${verify_output}\n${verify_error}")
    endif()

    set(CMAKE_CXX_CL_SHOWINCLUDES_PREFIX "${include_prefix}" PARENT_SCOPE)
    set(CMAKE_CL_SHOWINCLUDES_PREFIX "${include_prefix}" PARENT_SCOPE)
    message(STATUS "MSVC NMake include dependencies verified with the current compiler encoding")
endfunction()

frog_verify_msvc_include_dependencies()
