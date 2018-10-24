cmake_minimum_required(VERSION 3.11)
project(CopyBuildedFiles
        VERSION 0.0.1
        LANGUAGES CXX
        DESCRIPTION "Copyes all files from build machines")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
        
add_subdirectory(external)

#-------------------
# Add *.hpp and *.cpp files to project Visual Studio ${PROJECT_NAME}.vcxproj
#-------------------
set(PSD ${PROJECT_SOURCE_DIR})
set(HEADER_FILES  ${PSD}/include/CopyBuildedFiles/CopyBuildedFiles.hpp
                 # ${PSD}/include/CopyBuildedFiles/utils.hpp
                  ${PSD}/StdAfx.h
                  )
                  
set(SOURCE_FILES  ${PSD}/src/CopyBuildedFiles.cpp
                 # ${PSD}/src/utils.cpp
                  ${PSD}/StdAfx.cpp
                  ${PSD}/main.cpp 
                  )

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(CopyBuildedFiles PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(CopyBuildedFiles PRIVATE nlohmann_json::nlohmann_json spdlog expected)

#### Visual Studio stuff #####
# Set as startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
if (MSVC)
   set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/YuStdAfx.h")
   set_source_files_properties(StdAfx.cpp PROPERTIES COMPILE_FLAGS "/YcStdAfx.h")
endif(MSVC)