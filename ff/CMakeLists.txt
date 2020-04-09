
cmake_minimum_required(VERSION 2.6)
project(ff)
add_compile_options(-std=c++11 -g)

MESSAGE(STATUS "operation system is ${CMAKE_SYSTEM}")

include_directories(
  include
  third_party/jsoncpp/include
)

add_definitions(
	-DLIBFF_EXPORTS
)

add_compile_options(-Wno-narrowing)

IF (MINGW)
	MESSAGE(STATUS "Now is MinGW")
	file(GLOB windows_files "src/windows/*.cpp")
ELSEIF (WIN32)
	MESSAGE(STATUS "Now is windows")
	file(GLOB_RECURSE windows_files "src/windows/*.cpp")
ELSEIF (APPLE)
	MESSAGE(STATUS "Now is Apple systens.")
ELSEIF (UNIX)
	MESSAGE(STATUS "Now is UNIX-like OS's.")
ENDIF ()

if(BUILDIN_JSONCPP)
	MESSAGE(STATUS "make with jsoncpp source.")
	file(GLOB jsoncpp_src "third_party/jsoncpp/src/lib_json/*.cpp" "third_party/jsoncpp/src/lib_json/*.inl")
endif()

add_library(ff
	${windows_files}
	${jsoncpp_src}
	src/Object.cpp
	src/app/Application.cpp
	src/app/ApplicationImpl.cpp
	src/buffer/Buffer.cpp
	src/buffer/BufferImpl.cpp
	src/cli/CliPacket.cpp
	src/cli/CliService.cpp
	src/datetime/DateTime.cpp
	src/datetime/Tick.cpp
	src/datetime/Timestamp.cpp
	src/db/Sqlite3DataBase.cpp
	src/dll/DllLoader.cpp
	src/encrypt/Base64.cpp
	src/encrypt/CRC.cpp
	src/encrypt/MD5.cpp
	src/encrypt/Rijndael.cpp
	src/endian/Endian.cpp
	src/errno/ErrNo.cpp
	src/exception/Exception.cpp
	src/file/File.cpp
	src/log/Log.cpp
	src/log/Logger.cpp
	src/log/LogInfo.cpp
	src/log/StdoutAppender.cpp
	src/network/EPoll.cpp
	src/network/IP.cpp
	src/network/Socket.cpp
	src/network/TcpConnection.cpp
	src/process/FileLock.cpp
	src/process/Process.cpp
	src/process/ProcessImplLinux.cpp
	src/random/Random.cpp
	src/random/UUID.cpp
	src/serial/Serial.cpp
	src/settings/Settings.cpp
	src/string/String.cpp
	src/string/StringWrapper.cpp
	src/thread/Runnable.cpp
	src/thread/Semaphore.cpp
	src/thread/Synchronizable.cpp
	src/thread/Thread.cpp
	src/thread/ThreadPool.cpp
	src/variant/Variant.cpp
)

file(GLOB test_files "test/*.cpp")

add_executable(ff_test
	${test_files}
)

target_link_libraries(ff_test
  ff
  gtest
  gmock
  pthread
)


