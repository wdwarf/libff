
cmake_minimum_required(VERSION 2.6)
project(ff)
add_compile_options(-std=c++11 -g)

MESSAGE(STATUS "operation system is ${CMAKE_SYSTEM}")

include_directories(
  include
  third_party/jsoncpp/include
	third_party/zlib-1.2.11
	third_party/zlib-1.2.11/contrib
	third_party/zlib-1.2.11/contrib/minizip
)

add_definitions(
	-DLIBFF_EXPORTS
	-DBUILDING_LIBCURL
)

IF (MINGW)
	MESSAGE(STATUS "Now is MinGW")
	file(GLOB windows_files "src/windows/*.cpp")
ELSEIF (WIN32)
	MESSAGE(STATUS "Now is windows")
	file(GLOB_RECURSE windows_files "src/windows/*.cpp")

	include_directories(
		third_party/curl-7.77.0/include
	)
	
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")

ELSEIF (APPLE)
	MESSAGE(STATUS "Now is Apple OS's.")
ELSEIF (UNIX)
	MESSAGE(STATUS "Now is UNIX-like OS's.")

	add_compile_options(-Wno-narrowing)
#	add_compile_options(-mcpu=cortex-a7 -mthumb -O2 -ffunction-sections -fdata-sections)
ENDIF ()

if(BUILDIN_JSONCPP)
	MESSAGE(STATUS "make with jsoncpp source.")
	file(GLOB jsoncpp_src "third_party/jsoncpp/src/lib_json/*.cpp" "third_party/jsoncpp/src/lib_json/*.inl")
endif()

if(BUILDIN_ZLIB)
	MESSAGE(STATUS "make with zlib source.")
	set(ZLIB_SRCS
		third_party/zlib-1.2.11/adler32.c
		third_party/zlib-1.2.11/compress.c
		third_party/zlib-1.2.11/crc32.c
		third_party/zlib-1.2.11/deflate.c
		third_party/zlib-1.2.11/gzclose.c
		third_party/zlib-1.2.11/gzlib.c
		third_party/zlib-1.2.11/gzread.c
		third_party/zlib-1.2.11/gzwrite.c
		third_party/zlib-1.2.11/inflate.c
		third_party/zlib-1.2.11/infback.c
		third_party/zlib-1.2.11/inftrees.c
		third_party/zlib-1.2.11/inffast.c
		third_party/zlib-1.2.11/trees.c
		third_party/zlib-1.2.11/uncompr.c
		third_party/zlib-1.2.11/zutil.c
	)

	if (WIN32)
		set(MINIZIP_IOWIN32_SRCS
			third_party/zlib-1.2.11/contrib/minizip/iowin32.c
		)
	endif()
else()

	if(UNIX)
		set(EXTRA_LIBS
			z
		)
	endif()
endif()

set(MINIZIP_SRCS
third_party/zlib-1.2.11/contrib/minizip/zip.c
third_party/zlib-1.2.11/contrib/minizip/unzip.c
third_party/zlib-1.2.11/contrib/minizip/mztools.c
third_party/zlib-1.2.11/contrib/minizip/ioapi.c
)

add_library(ff
	${windows_files}
	${jsoncpp_src}
	${ZLIB_SRCS}
	${MINIZIP_SRCS}
	${MINIZIP_IOWIN32_SRCS}
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
	src/log/FileAppender.cpp
	src/message_bus/MessageBus.cpp
	src/network/EPoll.cpp
	src/network/IP.cpp
	src/network/Socket.cpp
	src/network/TcpConnection.cpp
	src/network/Curl.cpp
	src/process/FileLock.cpp
	src/process/Process.cpp
	src/process/ProcessImplLinux.cpp
	src/process/ProcessImplWindows.cpp
	src/random/Random.cpp
	src/random/UUID.cpp
	src/serial/Serial.cpp
	src/settings/Settings.cpp
	src/string/String.cpp
	src/string/StringWrapper.cpp
	src/string/StringEncoding.cpp
	src/system/FileSystem.cpp
	src/thread/Runnable.cpp
	src/thread/Semaphore.cpp
	src/thread/Synchronizable.cpp
	src/thread/Thread.cpp
	src/thread/ThreadPool.cpp
	src/variant/Variant.cpp
	src/rtp/RtpPacket.cpp
	src/compress/Zip.cpp
	src/compress/ZipEntry.cpp
	src/compress/Unzip.cpp
)

file(GLOB test_files "test/*.cpp")

add_executable(ff_test
	${test_files}
)

target_link_libraries(ff_test
  ff
	curl
	${EXTRA_LIBS}
  gtest
  gmock
  pthread
)


