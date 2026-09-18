cmake_minimum_required(VERSION 3.15)
project(lab2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(ifile STATIC
    IFile.cpp
)

target_include_directories(ifile PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(ifile PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)