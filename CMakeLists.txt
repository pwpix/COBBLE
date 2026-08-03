cmake_minimum_required(VERSION 3.16)

project(project1)

#set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(app main.cpp)

if(MSVC)
    target_compile_options(app PRIVATE "/Zc:preprocessor")
endif()

# 3. Add compiler options to the target BEFORE loading subdirectories
target_compile_features(app PRIVATE cxx_std_23)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(app PRIVATE -fexperimental-library)
endif()

# 4. Include the parser folder so main.cpp can find "#include "myLexer.h""
target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/parser)

# 5. NOW load the subdirectory safely (the 'app' target fully exists)
add_subdirectory(parser)

