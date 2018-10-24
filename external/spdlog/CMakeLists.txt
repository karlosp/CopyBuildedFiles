cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#---------------------------------------------------------------------------------------
# spdlog target
#---------------------------------------------------------------------------------------
add_library(spdlog INTERFACE)
add_library(spdlog::spdlog ALIAS spdlog)

# Check if spdlog is being used directly or via add_subdirectory
set(SPDLOG_MASTER_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(SPDLOG_MASTER_PROJECT ON)
endif()

target_include_directories(
    spdlog
    INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

set(HEADER_BASE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# register project in CMake user registry
export(PACKAGE ${PROJECT_NAME})

file(GLOB_RECURSE spdlog_include_SRCS "${HEADER_BASE}/*.h")
add_custom_target(spdlog_headers_for_ide SOURCES ${spdlog_include_SRCS})