##############################################################################
# Copyright 2013, 2026 Leon Lynch
#
# This file is licensed under the terms of the MIT license.
# See LICENSE file.
##############################################################################

add_library(testscene OBJECT
	testscene.cc
	../src/teaset.cc
	../src/internal/teaset_geometry.cc
	../src/gldebug.cc
)
target_include_directories(testscene PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_executable(glfw_test glfw_test.c)
target_link_libraries(glfw_test testscene glfw GLEW::GLEW OpenGL::GL)

qt_wrap_cpp(QT_TEST_MOC testwindow.h)
add_executable(qt_test qt_test.cc testwindow.cc ${QT_TEST_MOC})
target_link_libraries(qt_test testscene Qt6::OpenGL GLEW::GLEW OpenGL::GL)

qt_wrap_cpp(QTWIDGET_TEST_MOC testwidget.h)
add_executable(qtwidget_test qtwidget_test.cc testwidget.cc ${QTWIDGET_TEST_MOC})
target_link_libraries(qtwidget_test testscene Qt6::OpenGLWidgets GLEW::GLEW OpenGL::GL)

add_executable(bezier_test bezier_test.cc)
target_include_directories(bezier_test PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_executable(teaset_test teaset_test.cc)
target_link_libraries(teaset_test cortex)

add_executable(assimp_test assimp_test.cc)
target_link_libraries(assimp_test assimp::assimp)

add_executable(cortex_test cortex_test.cc)
target_link_libraries(cortex_test cortex)
