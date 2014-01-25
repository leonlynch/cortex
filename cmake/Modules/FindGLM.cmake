##############################################################################
# Copyright (c) 2013 Leon Lynch
#
# This file is licensed under the terms of the MIT license.
# See LICENSE file.
##############################################################################

# This will define:
#
# GLM_FOUND
# GLM_INCLUDE_DIR
# GLM_VERSION

find_path(GLM_INCLUDE_DIR "glm/glm.hpp")
find_file(GLM_SETUP_HPP NAMES "glm/core/setup.hpp" "glm/detail/setup.hpp")

if(GLM_SETUP_HPP)
	file(STRINGS
		${GLM_SETUP_HPP}
		GLM_VERSION_DEFINES
		REGEX "#define[ \t]+GLM_VERSION_[A-Z]+[ \t]+[0-9]+$"
	)
	if(GLM_VERSION_DEFINES)
		foreach(item IN ITEMS MAJOR MINOR PATCH REVISION)
			string(REGEX REPLACE ".*#define[ \t]+GLM_VERSION_${item}[ \t]+([0-9]+).*" "\\1" GLM_VERSION_${item} ${GLM_VERSION_DEFINES})
		endforeach()
	endif()
	set(GLM_VERSION "${GLM_VERSION_MAJOR}.${GLM_VERSION_MINOR}.${GLM_VERSION_PATCH}.${GLM_VERSION_REVISION}")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLM
	REQUIRED_VARS GLM_INCLUDE_DIR GLM_SETUP_HPP
	VERSION_VAR GLM_VERSION
)

mark_as_advanced(GLM_FOUND GLM_INCLUDE_DIR GLM_SETUP_HPP GLM_VERSION_MAJOR GLM_VERSION_MINOR GLM_VERSION_PATCH GLM_VERSION_REVISION GLM_VERSION)
