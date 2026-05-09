##############################################################################
# Copyright 2013, 2026 Leon Lynch
#
# This file is licensed under the terms of the MIT license.
# See LICENSE file.
##############################################################################

cmake_minimum_required(VERSION 4.2)

project(cortex)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
	add_compile_options(-Wall -Werror)
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -ggdb")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb")
endif()

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
