cmake_minimum_required(VERSION 3.16)

project(Parser3)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(app main.cpp)

#add_executable(app tests/test.cpp)

if(MSVC)
    target_compile_options(app PRIVATE "/Zc:preprocessor")
endif()

target_compile_features(app PRIVATE cxx_std_23)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(app PRIVATE -fexperimental-library)
endif()

target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/parser)

add_subdirectory(parser)

