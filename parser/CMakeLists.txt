
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${SOURCE_DIR}/*.cpp")
message(STATUS "Found parser source files: ${SOURCES}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
target_sources(app PRIVATE ${SOURCES})
target_include_directories(app PRIVATE ${SOURCE_DIR})
