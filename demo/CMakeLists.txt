##############################################################################
# Copyright 2013, 2026 Leon Lynch
#
# This file is licensed under the terms of the MIT license.
# See LICENSE file.
##############################################################################

set(DEMO_VERSION ${GIT_DESCRIBE})
configure_file(demo_config.h.in demo_config.h)

qt_wrap_ui(CORTEX_DEMO_UI mainwindow.ui)
qt_wrap_cpp(CORTEX_DEMO_MOC mainwindow.h demowidget.h)
add_executable(cortex_demo
	main.cc
	mainwindow.cc
	demowidget.cc
	${CORTEX_DEMO_UI}
	${CORTEX_DEMO_MOC}
	../test/testscene.cc # TODO: remove
)
target_include_directories(cortex_demo
	PRIVATE
		${CMAKE_CURRENT_BINARY_DIR} # For generated headers
)
target_link_libraries(cortex_demo
	PRIVATE
		cortex
		Qt6::Widgets
		Qt6::OpenGLWidgets
		GLEW::GLEW
		OpenGL::GL
)
