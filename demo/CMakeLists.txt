##############################################################################
# Copyright (c) 2013 Leon Lynch
#
# This file is licensed under the terms of the MIT license.
# See LICENSE file.
##############################################################################

cmake_minimum_required(VERSION 3.11)

set(VERSION ${GIT_DESCRIBE})
configure_file(version.h.in version.h)

include_directories(${LIBCORTEX_INCLUDE_DIR})
set(CMAKE_INCLUDE_CURRENT_DIR ON) # include generated files
qt5_wrap_ui(CORTEX_DEMO_UI mainwindow.ui)
qt5_wrap_cpp(CORTEX_DEMO_MOC mainwindow.h demowidget.h)
add_executable(cortex_demo
	main.cc
	mainwindow.cc
	demowidget.cc
	${CORTEX_DEMO_UI}
	${CORTEX_DEMO_MOC}
	../test/testscene.cc # TODO: remove
)
target_link_libraries(cortex_demo
	Qt5::Widgets
	GLEW::GLEW
	OpenGL::GL
)
