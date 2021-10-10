
cmake_minimum_required(VERSION 2.8.2)
project(libff)

# set(BUILD_SHARED_LIBS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/bin)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/build/ff/third_party/zlib-1.2.11
)

if(WIN32)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
endif()

add_subdirectory(ff/third_party/curl-7.77.0)
add_subdirectory(ff/third_party/googletest)
add_subdirectory(ff/third_party/zlib-1.2.11)
add_subdirectory(ff)