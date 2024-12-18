############## Include #######################
include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)

############## Setup Project #######################
cmake_minimum_required(VERSION 3.11.0)

set(CMAKE_CXX_STANDARD 17)

set(NAME "ChicaneEditor")

project(${NAME} VERSION 0.0.1)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

##############  Set values  #######################
set(BUILD_DIR "${CMAKE_CURRENT_LIST_DIR}/Bin/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}")

set(CONTENT_DIR "${CMAKE_CURRENT_LIST_DIR}/Content")

set(CONTENT_TARGET_NAME "${NAME}_Content")
set(CONTENT_SOURCE_DIR  "${CONTENT_DIR}")

set(EDITOR_TARGET_NAME "${NAME}_Editor")
set(EDITOR_SOURCE_DIR  "${CMAKE_CURRENT_LIST_DIR}/Source")

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
    "ChicaneEditor"

    VS_DEBUGGER_WORKING_DIRECTORY
    "${BUILD_DIR}"

    RUNTIME_OUTPUT_DIRECTORY_DEBUG
    "${BUILD_DIR}"

    RUNTIME_OUTPUT_DIRECTORY_RELEASE
    "${BUILD_DIR}"
)

target_include_directories(
    ${EDITOR_TARGET_NAME}

    PRIVATE
        ${EDITOR_SOURCE_DIR}
)

target_link_libraries(
    ${EDITOR_TARGET_NAME}

    PRIVATE
        Chicane::Runtime
)

if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (WIN32)
        target_link_options(${EDITOR_TARGET_NAME} PRIVATE -mwindows)
    endif()
endif()

##############  Set compiler  #######################
if (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
    if (NOT MINGW_PATH) 
      	message(FATAL_ERROR "MINGW_PATH not set in .env.cmake")
    endif()

    set(CMAKE_C_COMPILER   ${MINGW_PATH}/bin/gcc.exe)
    set(CMAKE_CXX_COMPILER ${MINGW_PATH}/bin/g++.exe)

    if (WIN32)
        target_include_directories(
            ${EDITOR_TARGET_NAME}

            PRIVATE
                ${MINGW_PATH}/include
        )

        target_link_directories(
            ${EDITOR_TARGET_NAME}

            PRIVATE
                ${MINGW_PATH}/lib
        )
    endif()

    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

##############  Add subdirectories  #######################
add_subdirectory(Content)

##############  Add dependencies  #######################
add_dependencies(${EDITOR_TARGET_NAME} ${CONTENT_TARGET_NAME})
