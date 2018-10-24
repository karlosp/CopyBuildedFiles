cmake_minimum_required(VERSION 3.11)
project(ConvertProjectsFromBuildAll
        VERSION 0.0.1
        LANGUAGES CXX
        DESCRIPTION "ConvertProjectsFromBuildAll")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
        

#-------------------
# Add *.hpp and *.cpp files to project Visual Studio ${PROJECT_NAME}.vcxproj
#-------------------
set(PSD ${PROJECT_SOURCE_DIR})
set(HEADER_FILES  ${PSD}/ConvertProjectsFromBuildAll.hpp
                  )
                  
set(SOURCE_FILES  ${PSD}/ConvertProjectsFromBuildAll.cpp
                  ${PSD}/main.cpp 
                  )

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})
#add_subdirectory(../spdlog ./spdlog2)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} spdlog)

#### Visual Studio stuff #####
# Set as startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
