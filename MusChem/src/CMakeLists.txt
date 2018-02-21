# ========== CMake settings ==========
# Minimum version
cmake_minimum_required(VERSION 2.8)

# Compilation flags
set(CMAKE_C_FLAGS "-g -Wall -Wextra -pthread")
set(CMAKE_CXX_FLAGS "-g -Wall -Wextra -pthread")

# Include directories

# Linking flags
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/../lib/)

# Set output paths
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/../bin/)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/../lib/)

# Define and include some useful paths
set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR})
set(GENERIC_DIR ${PROJECT_SOURCE_DIR}/Generic)
set(MODULES_DIR ${PROJECT_SOURCE_DIR}/Modules)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/../include)
include_directories(${GENERIC_DIR})

# ========== Add libraries ==========
# Generic libraries
add_subdirectory(${GENERIC_DIR}/GLogger)
add_subdirectory(${GENERIC_DIR}/GParser)

# Module libraries
add_subdirectory(${MODULES_DIR}/Graphics)
add_subdirectory(${MODULES_DIR}/Input)

# Game libraries
add_subdirectory(${PROJECT_SOURCE_DIR}/Systems)

# ========== Add executables ==========
add_executable(MusChem
    main.cpp)

target_link_libraries(MusChem
    Graphics
    Input
    GLogger
    GParser
    Systems
    portaudio
    rt
    m
    asound
    jack)
