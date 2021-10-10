
cmake_minimum_required(VERSION 2.8.2)
project(libff)

# set(BUILD_SHARED_LIBS OFF)

if("${CMAKE_BUILD_TYPE}" MATCHES "")
set(CMAKE_BUILD_TYPE Release)
endif()

MESSAGE(STATUS "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
	
if(WIN32)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/bin)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")

else()


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/lib/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/lib/${CMAKE_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/bin/${CMAKE_BUILD_TYPE})


endif()

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/build/ff/third_party/zlib-1.2.11
  ${CMAKE_CURRENT_SOURCE_DIR}/ff/third_party/zlib-1.2.11
)

add_subdirectory(ff/third_party/curl-7.77.0)
add_subdirectory(ff/third_party/googletest)
add_subdirectory(ff/third_party/zlib-1.2.11)
add_subdirectory(ff/third_party/jsoncpp)
add_subdirectory(ff)