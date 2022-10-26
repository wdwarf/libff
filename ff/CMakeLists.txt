
cmake_minimum_required(VERSION 2.8.12)
project(ff LANGUAGES CXX)

set(PROJECT_VERSION "1")
set(PROJECT_VERSION_MAJOR "1")
set(PROJECT_VERSION_MINOR "0")
set(PROJECT_VERSION_PATCH "0")

MESSAGE(STATUS "operation system is ${CMAKE_SYSTEM}")

MESSAGE(STATUS "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")

option(BUILDIN_JSONCPP OFF)
option(BUILDIN_ZLIB OFF)
	
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

link_directories(
	${CMAKE_SOURCE_DIR}/build/lib/${CMAKE_BUILD_TYPE}
)

IF (MINGW)
	MESSAGE(STATUS "Now is MinGW")
	file(GLOB windows_files "src/windows/*.cpp")

ELSEIF (WIN32)
	MESSAGE(STATUS "Now is windows")

	file(GLOB_RECURSE windows_files "src/windows/*.cpp")

	include_directories(
		third_party/curl-7.77.0/include
		third_party/googletest/googletest/include
	)
	
	set(EXTRA_LIBS
		ws2_32
		winmm
		wldap32
	)

	if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")

	MESSAGE(STATUS "building ${CMAKE_BUILD_TYPE}")
	
	set(EXTRA_LIBS
	${EXTRA_LIBS}
		libcurl-d
		zlibd
		gtestd
  	gmockd
		jsoncppd
	)

	else()
	
	set(EXTRA_LIBS
	${EXTRA_LIBS}
	libvcruntime
		libcurl
		zlib
		gtest
  	gmock
		jsoncpp
	)

	endif()

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
		add_compile_options(-g)
	
		set(EXTRA_LIBS
			pthread
			z
			curl
			jsoncpp
			gtest
			gmock
		)
	endif()
endif()

set(MINIZIP_SRCS
third_party/zlib-1.2.11/contrib/minizip/zip.c
third_party/zlib-1.2.11/contrib/minizip/unzip.c
third_party/zlib-1.2.11/contrib/minizip/mztools.c
third_party/zlib-1.2.11/contrib/minizip/ioapi.c
)

set(lib_srcs
${windows_files}
	${jsoncpp_src}
	${ZLIB_SRCS}
	${MINIZIP_SRCS}
	${MINIZIP_IOWIN32_SRCS}
	src/Object.cpp
	src/app/Application.cpp
	src/app/ApplicationImpl.cpp
	src/app/CmdLineParser.cpp
	src/buffer/Buffer.cpp
	src/cli/CliPacket.cpp
	src/cli/CliService.cpp
	src/data_service/DataService.cpp
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
	src/network/NetworkDevices.cpp
	src/network/Socket.cpp
	src/network/TcpConnection.cpp
	src/network/Curl.cpp
	src/process/FileLock.cpp
	src/process/Process.cpp
	src/process/ProcessImplLinux.cpp
	src/process/ProcessImplWindows.cpp
	src/random/Random.cpp
	src/random/Snowflake.cpp
	src/random/UUID.cpp
	src/serial/Serial.cpp
	src/settings/Settings.cpp
	src/shm/SHM.cpp
	src/string/String.cpp
	src/string/StringWrapper.cpp
	src/string/StringEncoding.cpp
	src/system/FileSystem.cpp
	src/system/System.cpp
	src/timer/Timer.cpp
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

add_library(ff 
	${lib_srcs}
)

target_link_libraries(ff
	# STATIC
	${EXTRA_LIBS}
)

file(GLOB test_files "test/*.cpp")

add_executable(ff_test 
	# ${lib_srcs}
	${test_files}
)

target_link_libraries(ff_test 
	ff
	${EXTRA_LIBS}
)

if(MSVC)
target_compile_options(ff PUBLIC "/Zc:__cplusplus")
target_compile_options(ff_test PUBLIC "/Zc:__cplusplus")
endif()

add_subdirectory(test/shm_test)
