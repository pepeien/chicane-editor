############## Include #######################
include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)

############## Setup Project #######################
cmake_minimum_required(VERSION 3.11.0)

set(CMAKE_CXX_STANDARD 17)

set(NAME "ChicaneEditor")

project(${NAME} VERSION 0.0.1)

##############  Set source files  #######################
set(BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Bin/${CMAKE_SYSTEM_NAME}")

add_subdirectory(Vendors/Engine)

##############  Set source files  #######################
file(
    GLOB_RECURSE
    SOURCES

    ${CMAKE_CURRENT_LIST_DIR}/Sources/*.cpp
)

############## Setup Executable #######################
add_executable("Editor" "")

target_sources(
    "Editor"

    PRIVATE
        ${SOURCES}
)

set_target_properties(
    "Editor"
    PROPERTIES

    VS_DEBUGGER_WORKING_DIRECTORY
    "${BUILD_DIR}"

    RUNTIME_OUTPUT_DIRECTORY
    "${BUILD_DIR}"

    RUNTIME_OUTPUT_DIRECTORY_DEBUG
    "${BUILD_DIR}"

    RUNTIME_OUTPUT_DIRECTORY_RELEASE
    "${BUILD_DIR}"
)

target_include_directories(
    "Editor"

    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/Includes>
        $<INSTALL_INTERFACE:Includes>
)

target_link_libraries(
    "Editor"

    PRIVATE
        Chicane::Runtime
)