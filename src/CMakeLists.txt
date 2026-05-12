##############################################################################
# Copyright 2013, 2026 Leon Lynch
#
# This file is licensed under the terms of the MIT license.
# See LICENSE file.
##############################################################################

add_library(cortex
	teaset.cc
	internal/teaset_geometry.cc
	sceneloader.cc
	entity.cc
	material.cc
	mesh.cc
	gldebug.cc
)
target_include_directories(cortex
	INTERFACE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<INSTALL_INTERFACE:include/cortex>
)
target_link_libraries(cortex
	PRIVATE
		assimp::assimp
		GLEW::GLEW
		OpenGL::GL
)
