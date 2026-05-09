##############################################################################
# Copyright 2013, 2026 Leon Lynch
#
# This file is licensed under the terms of the MIT license.
# See LICENSE file.
##############################################################################

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
target_link_libraries(cortex assimp::assimp)
