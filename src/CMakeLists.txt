##############################################################################
# Copyright (c) 2013 Leon Lynch
#
# This file is licensed under the terms of the MIT license.
# See LICENSE file.
##############################################################################

cmake_minimum_required(VERSION 2.8.11)

set(LIBCORTEX_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "libcortex include path")
include_directories(${LIBCORTEX_INCLUDE_DIR})

set(CORTEX_QT_SRC
	qt/openglwindow.cc
	qt/simpleopenglwindow.cc
)
qt5_wrap_cpp(CORTEX_QT_MOC
	qt/openglwindow.h
	qt/simpleopenglwindow.h
)
add_library(cortex_qt
	${CORTEX_QT_SRC}
	${CORTEX_QT_MOC}
)
qt5_use_modules(cortex_qt Gui)
