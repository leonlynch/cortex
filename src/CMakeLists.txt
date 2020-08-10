##############################################################################
# Copyright (c) 2013 Leon Lynch
#
# This file is licensed under the terms of the MIT license.
# See LICENSE file.
##############################################################################

cmake_minimum_required(VERSION 3.11)

set(LIBCORTEX_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "libcortex include path")
include_directories(${LIBCORTEX_INCLUDE_DIR})

add_library(cortex
	teaset.cc
	internal/teaset_geometry.cc
	sceneloader.cc
	entity.cc
	material.cc
	mesh.cc
)
target_link_libraries(cortex ${ASSIMP_LIBRARIES})
