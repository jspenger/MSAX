cmake_minimum_required(VERSION 2.8.11)

# Name of project
project(MSAX)

# Dependencies Boost - tested Boost version: 1.59.0
find_package(Boost COMPONENTS program_options REQUIRED)

# Add include directories
include_directories (${Boost_INCLUDE_DIR} include)

# Add definitions
add_definitions (${Boost_LIB_DIAGNOSTIC_DEFINITIONS})

# Set header files
set(HEADER_FILES)

# Set source files
set(SOURCE_FILES src/main.cpp src/msax.hpp)

# Set c++ flags
SET ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -W -Wall -Wstrict-aliasing -pedantic -Wno-long-long -Wno-variadic-macros" )

# Add executable
add_executable(MSAX ${SOURCE_FILES} ${HEADER_FILES})

# Link against SeqAn dependencies
target_link_libraries (MSAX ${Boost_LIBRARIES})
