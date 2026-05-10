##############################################################################
# Copyright 2013, 2026 Leon Lynch
#
# This file is licensed under the terms of the MIT license.
# See LICENSE file.
##############################################################################

cmake_minimum_required(VERSION 4.2)

project(cortex)

# Configure compiler
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	add_compile_options(-Wall -Werror)
	add_compile_options($<$<CONFIG:Debug>:-ggdb3>)
	add_compile_options($<$<CONFIG:Debug>:-fno-omit-frame-pointer>)
	add_compile_options($<$<CONFIG:RelWithDebInfo>:-ggdb3>)
endif()
if(CMAKE_C_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	add_compile_options(-Wall -Werror)
	add_compile_options($<$<CONFIG:Debug>:-g3>)
	add_compile_options($<$<CONFIG:Debug>:-fno-omit-frame-pointer>)
	add_compile_options($<$<CONFIG:RelWithDebInfo>:-g3>)
endif()

# Configure debugging sanitizers
option(ENABLE_SANITIZERS "Enable debugging sanitizers" OFF)
if(ENABLE_SANITIZERS)
	if(CMAKE_C_COMPILER_ID MATCHES "^(GNU|Clang)$" OR
		CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang)$"
	)
		add_compile_options(-fsanitize=address)
		add_compile_options(-fsanitize=undefined)
		add_link_options(-fsanitize=address)
		add_link_options(-fsanitize=undefined)
		add_compile_options(-fsanitize=leak)
		add_link_options(-fsanitize=leak)
	endif()
endif()

# Export compile commands for various IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Project dependencies
find_package(Git REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW 2.1 REQUIRED)
find_package(glm 1.0 REQUIRED)
find_package_handle_standard_args(glm CONFIG_MODE)
find_package(Qt6 6.10 REQUIRED COMPONENTS Widgets OpenGL OpenGLWidgets)
find_package_handle_standard_args(Qt6 CONFIG_MODE)
find_package(glfw3 3.4 REQUIRED)
find_package_handle_standard_args(glfw3 CONFIG_MODE)
find_package(assimp 6 REQUIRED)

execute_process(
	COMMAND ${GIT_EXECUTABLE} describe --all
	OUTPUT_VARIABLE GIT_DESCRIBE
	OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
	COMMAND date -Is
	OUTPUT_VARIABLE BUILD_DATE
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(demo)
