cmake_minimum_required(VERSION 3.16)
project(mathbits VERSION 0.1.0 LANGUAGES C
    DESCRIPTION "Header-only C11 dense linear algebra")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

option(MATHBITS_BUILD_TESTS "Build tests" ON)
option(MATHBITS_BUILD_BENCHMARKS "Build benchmarks" ON)
option(MATHBITS_BUILD_EXAMPLES "Build examples" ON)
option(MATHBITS_ENABLE_NATIVE "Host-native CPU flags for tests/benches" ON)
option(MATHBITS_USE_MEMENTO "Bind allocations to memento" OFF)
option(MATHBITS_USE_CBLAS "Redirect large gemm to CBLAS (stub)" OFF)
set(MATHBITS_SANITIZE "off" CACHE STRING
    "Sanitizer: off, address,undefined, or thread")
set(MATHBITS_MEMENTO_SOURCE "" CACHE PATH
    "Optional local memento tree (include/memento.h). Empty = FetchContent v3.0.0.")
set(MATHBITS_SUSPENDERS_SOURCE "" CACHE PATH
    "Optional suspenders.h include dir for ex_suspenders.")

add_library(mathbits INTERFACE)
add_library(mathbits::mathbits ALIAS mathbits)
target_include_directories(mathbits INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(mathbits INTERFACE c_std_11)
if(UNIX)
    target_link_libraries(mathbits INTERFACE m)
endif()

if(MATHBITS_USE_CBLAS)
    target_compile_definitions(mathbits INTERFACE MATHBITS_USE_CBLAS)
endif()

# Optional memento
set(MATHBITS_MEMENTO_INCLUDE "")
if(MATHBITS_USE_MEMENTO)
    include(FetchContent)
    if(MATHBITS_MEMENTO_SOURCE)
        if(NOT EXISTS "${MATHBITS_MEMENTO_SOURCE}/include/memento.h")
            message(FATAL_ERROR "MATHBITS_MEMENTO_SOURCE has no include/memento.h")
        endif()
        set(MATHBITS_MEMENTO_INCLUDE "${MATHBITS_MEMENTO_SOURCE}/include")
    else()
        FetchContent_Declare(memento
            GIT_REPOSITORY https://github.com/StraylightRunMOO/memento.git
            GIT_TAG        v3.0.0
            GIT_SHALLOW    TRUE)
        FetchContent_GetProperties(memento)
        if(NOT memento_POPULATED)
            FetchContent_Populate(memento)
        endif()
        set(MATHBITS_MEMENTO_INCLUDE "${memento_SOURCE_DIR}/include")
    endif()
    target_include_directories(mathbits SYSTEM INTERFACE "${MATHBITS_MEMENTO_INCLUDE}")
    find_package(Threads REQUIRED)
    target_link_libraries(mathbits INTERFACE Threads::Threads)
endif()

# Internal flags — tests/examples/benches only
add_library(mathbits_internal INTERFACE)
target_link_libraries(mathbits_internal INTERFACE mathbits)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(mathbits_internal INTERFACE
        -Wall -Wextra -Wpedantic
        -Werror
        -Wno-unused-function
        -Wno-unused-parameter
        -Wno-unused-variable
    )
    if(MATHBITS_ENABLE_NATIVE)
        include(CheckCCompilerFlag)
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64|ARM64")
            target_compile_options(mathbits_internal INTERFACE -mcpu=native)
        else()
            check_c_compiler_flag("-march=native" HAS_MARCH_NATIVE)
            if(HAS_MARCH_NATIVE)
                target_compile_options(mathbits_internal INTERFACE -march=native)
            endif()
        endif()
    endif()
elseif(MSVC)
    target_compile_options(mathbits_internal INTERFACE /W4)
endif()

if(NOT MATHBITS_SANITIZE STREQUAL "off")
    target_compile_options(mathbits_internal INTERFACE
        -fsanitize=${MATHBITS_SANITIZE} -fno-omit-frame-pointer -g)
    target_link_options(mathbits_internal INTERFACE
        -fsanitize=${MATHBITS_SANITIZE})
endif()

if(MATHBITS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
if(MATHBITS_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()
if(MATHBITS_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

include(GNUInstallDirs)
install(DIRECTORY include/mathbits DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS mathbits EXPORT mathbits-targets)
install(EXPORT mathbits-targets
    FILE mathbits-targets.cmake
    NAMESPACE mathbits::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mathbits)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/mathbits-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/mathbits-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mathbits)

message(STATUS "")
message(STATUS "mathbits ${PROJECT_VERSION}")
message(STATUS "  tests:       ${MATHBITS_BUILD_TESTS}")
message(STATUS "  benchmarks:  ${MATHBITS_BUILD_BENCHMARKS}")
message(STATUS "  examples:    ${MATHBITS_BUILD_EXAMPLES}")
message(STATUS "  native:      ${MATHBITS_ENABLE_NATIVE}")
message(STATUS "  sanitizer:   ${MATHBITS_SANITIZE}")
message(STATUS "  memento:     ${MATHBITS_USE_MEMENTO}")
message(STATUS "  cblas:       ${MATHBITS_USE_CBLAS}")
message(STATUS "  processor:   ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "")
