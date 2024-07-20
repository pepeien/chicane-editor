############## Include #######################
include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)

############## Setup Project #######################
cmake_minimum_required(VERSION 3.11.0)

set(CMAKE_CXX_STANDARD 17)

set(NAME "Factory")

project(${NAME} VERSION 0.0.1)

##############  Set values  #######################
set(BUILD_DIR "${CMAKE_CURRENT_LIST_DIR}/Bin/${CMAKE_SYSTEM_NAME}")

set(CONTENT_DIR  "${CMAKE_CURRENT_LIST_DIR}/Content")
set(INCLUDES_DIR "${CMAKE_CURRENT_LIST_DIR}/Includes")
set(SOURCES_DIR  "${CMAKE_CURRENT_LIST_DIR}/Sources")

set(CONTENT_TARGET_NAME "${NAME}_Content")
set(CONTENT_SOURCE_DIR  "${CONTENT_DIR}")

set(EDITOR_TARGET_NAME "${NAME}_Editor")
set(EDITOR_SOURCE_DIR  "${SOURCES_DIR}")

##############  Set source files  #######################
file(
    GLOB_RECURSE
    SOURCES

    ${EDITOR_SOURCE_DIR}/*.cpp
)

############## Setup Libs #######################
add_subdirectory(Vendors/Engine)

############## Setup Executable #######################
add_executable(${EDITOR_TARGET_NAME} "")

target_sources(
    ${EDITOR_TARGET_NAME}

    PRIVATE
        ${SOURCES}
)

set_target_properties(
    ${EDITOR_TARGET_NAME}
    PROPERTIES

    OUTPUT_NAME
    "Factory"

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
    ${EDITOR_TARGET_NAME}

    PRIVATE
        $<BUILD_INTERFACE:${INCLUDES_DIR}>
        $<INSTALL_INTERFACE:Includes>
)

target_link_libraries(
    ${EDITOR_TARGET_NAME}

    PRIVATE
        Chicane::Runtime
)

##############  Add subdirectories  #######################
add_subdirectory(Content)

##############  Add dependencies  #######################
add_dependencies(${EDITOR_TARGET_NAME} ${CONTENT_TARGET_NAME})