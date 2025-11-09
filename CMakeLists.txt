cmake_minimum_required(VERSION 3.31)

# Setup project
project(
	SkyrimSoulsRE
	VERSION 2.4.0
	LANGUAGES CXX
)
set(FORMATTED_NAME "Skyrim Souls RE")
set(LICENSE "MIT License")
set(AUTHOR "Vermunds")

# Configure files
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/Version.h.in
	${CMAKE_CURRENT_BINARY_DIR}/include/Version.h
	@ONLY
)

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in
	${CMAKE_CURRENT_BINARY_DIR}/version.rc
	@ONLY
)

# Include sources
include(cmake/sourcelist.cmake)
source_group(
	TREE "${CMAKE_CURRENT_SOURCE_DIR}"
	FILES ${SOURCES}
)

# Create DLL
add_library(
	${PROJECT_NAME}
	SHARED
	${SOURCES}
	${CMAKE_CURRENT_BINARY_DIR}/include/Version.h
	${CMAKE_CURRENT_BINARY_DIR}/version.rc
)

# Include directories
target_include_directories(
	${PROJECT_NAME}
	PRIVATE
		${CMAKE_CURRENT_BINARY_DIR}/include
		${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_include_directories(
	${PROJECT_NAME}
	PUBLIC
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
		"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

# Precompile header
target_precompile_headers(
	${PROJECT_NAME}
	PRIVATE
		"include/PCH.h"
)

# Find and link dependencies
find_package(spdlog REQUIRED CONFIG)

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
		CommonLibSSE::CommonLibSSE
		spdlog::spdlog
)
