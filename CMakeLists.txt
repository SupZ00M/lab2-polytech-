cmake_minimum_required(VERSION 3.15)
project(lab2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_library(filecore STATIC
    src/My_string.cpp
    src/BaseFile.cpp
)

target_include_directories(filecore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/libs
)


target_link_libraries(filecore PUBLIC ${I_LIBRARY})


find_library(I_LIBRARY
    NAMES ifile
    PATHS ${CMAKE_CURRENT_SOURCE_DIR}/libs/build
    NO_DEFAULT_PATH
)

if(NOT I_LIBRARY)
    message(FATAL_ERROR
        "ifile не найдена. Сначала соберите её:\n"
        "  cd libs && mkdir -p build && cd build && cmake .. && make")
endif()

message(STATUS "ifile найдена: ${I_LIBRARY}")


add_executable(lab2 src/lab2.cpp)

target_include_directories(lab2 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/libs
)

target_link_libraries(lab2 PRIVATE
    filecore
    ${I_LIBRARY}
)


add_executable(test_runner tests/test.cpp)

target_include_directories(test_runner PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/libs
)

target_link_libraries(test_runner PRIVATE
    filecore
    ${I_LIBRARY}
)

enable_testing()
add_test(NAME class_tests COMMAND test_runner)