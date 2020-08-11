##############################################################################
# Copyright (c) 2013 Leon Lynch
#
# This file is licensed under the terms of the MIT license.
# See LICENSE file.
##############################################################################

cmake_minimum_required(VERSION 3.11)

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

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(Git REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW 2 REQUIRED)
find_package(GLM 0.9.5 REQUIRED)
find_package(Qt5Core 5.4 REQUIRED)
find_package(Qt5Gui 5.4 REQUIRED)
find_package(Qt5Widgets 5.4 REQUIRED)
find_package(GLUT REQUIRED)
find_package(glfw3 3.2 REQUIRED)
find_package(assimp 5 REQUIRED)

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
