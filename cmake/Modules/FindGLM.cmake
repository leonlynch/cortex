##############################################################################
# Copyright (c) 2013 Leon Lynch
#
# This file is licensed under the terms of the MIT license.
# See LICENSE file.
##############################################################################

find_path(GLM_INCLUDE_DIR "glm/glm.hpp")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLM
	REQUIRED_VARS GLM_INCLUDE_DIR
	FAIL_MESSAGE "Failed to find GLM header"
)

mark_as_advanced(GLM_INCLUDE_DIR)
