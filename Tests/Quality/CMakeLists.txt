cmake_minimum_required(VERSION 3.14)

project(Chrishendo)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

if (WIN32)
	set(OS "windows")
	set(ARCHITECTURE "x64")
elseif(APPLE)
	EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
	set(OS "osx")
else()
	set(OS "linux")
	set(ARCHITECTURE "x64")
endif()

if (EXISTS "${VCPKG_DIR}")
	set(xxHash_DIR "${VCPKG_DIR}/installed/${ARCHITECTURE}-${OS}/share/xxhash")
endif()

find_package(xxHash CONFIG REQUIRED)

FetchContent_Declare(
  jsonifier
  GIT_REPOSITORY https://github.com/RealTimeChris/jsonifier
  GIT_TAG dev
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(jsonifier)

FetchContent_Declare(
  BenchmarkSuite
  GIT_REPOSITORY https://github.com/RealTimeChris/BenchmarkSuite
  GIT_TAG main
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(BenchmarkSuite)

add_executable("Hash-Performance" "./main.cpp")

target_link_libraries(
  "Hash-Performance" PUBLIC 
  Jsonifier::Jsonifier
  Chrishendo::Chrishendo 
  xxHash::xxhash
  BenchmarkSuite::BenchmarkSuite
)

target_compile_options(
	"Hash-Performance" PUBLIC
	"$<$<CXX_COMPILER_ID:CLANG>:-Wextra>"
	"$<$<CXX_COMPILER_ID:MSVC>:/bigobj>"
	"$<$<CXX_COMPILER_ID:CLANG>:-Wall>"
	"$<$<CXX_COMPILER_ID:GNU>:-Wextra>"
	"$<$<CXX_COMPILER_ID:MSVC>:/Wall>"
	"$<$<CXX_COMPILER_ID:MSVC>:/EHsc>"
	"$<$<CXX_COMPILER_ID:GNU>:-Wall>"
	"$<$<CXX_COMPILER_ID:MSVC>:/Zi>"
)

target_link_options(
	"Hash-Performance" PUBLIC
	"$<$<CXX_COMPILER_ID:GNU>:$<$<STREQUAL:${ASAN_ENABLED},TRUE>:-fsanitize=address>>"
	"$<$<CXX_COMPILER_ID:MSVC>:/DEBUG>"
	"$<$<CXX_COMPILER_ID:MSVC>:/OPT:REF>"
)

if (WIN32)
	install(
		FILES 
		"$<TARGET_PDB_FILE:Hash-Performance>"
		DESTINATION "${CMAKE_INSTALL_PREFIX}/Tests/${CMAKE_BUILD_TYPE}"
		OPTIONAL
	)
endif()

target_compile_definitions(
	"Hash-Performance" PUBLIC 
	"JSON_TEST_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/ConformanceTests\""
	"JSON_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/../Json\""
	"README_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.md\""
)

install(
	FILES 
	"$<TARGET_FILE:Hash-Performance>"
	DESTINATION "${CMAKE_INSTALL_PREFIX}/Tests/${CMAKE_BUILD_TYPE}"
	OPTIONAL
)

install(
	FILES 
	"$<TARGET_RUNTIME_DLLS:Hash-Performance>"
	DESTINATION "${CMAKE_INSTALL_PREFIX}/Tests/${CMAKE_BUILD_TYPE}"
	OPTIONAL
)