cmake_minimum_required(VERSION 3.15)

project(Infecdead LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Files
set(HEADERS
	common.hpp
)
set(SOURCES
	main.cpp
)
list(TRANSFORM HEADERS PREPEND src/)
list(TRANSFORM SOURCES PREPEND src/)

# The executable
add_executable(Infecdead ${HEADERS} ${SOURCES})
target_include_directories(Infecdead PUBLIC ${CMAKE_SOURCE_DIR}/src)

# Copies resource files to the output directory
add_custom_command(TARGET Infecdead POST_BUILD COMMAND
				   ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res/ $<TARGET_FILE_DIR:Infecdead>/res/)

# Compiler specific configuration
if (MSVC)
	set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Infecdead)
	set_target_properties(Infecdead PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

	target_compile_options(Infecdead PUBLIC /J)
else()
	target_compile_options(Infecdead PUBLIC -funsigned-char)
endif()
