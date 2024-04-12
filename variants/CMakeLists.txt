cmake_minimum_required(VERSION 3.21)
project(Assignment_1)

set(CMAKE_CXX_STANDARD 17)

# Add sources
FILE(GLOB MySources Source/**)

# Maintain folder structure (useful for Visual Studio)
if (MSVC)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${MySources})
endif()

# Set warning level
if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Create executable
add_executable(Assignment_1 ${MySources})