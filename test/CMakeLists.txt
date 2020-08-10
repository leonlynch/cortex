##############################################################################
# Copyright (c) 2013 Leon Lynch
#
# This file is licensed under the terms of the MIT license.
# See LICENSE file.
##############################################################################

cmake_minimum_required(VERSION 3.11)

include_directories(${LIBCORTEX_INCLUDE_DIR})

set(GLFW_TEST_SRCS glfw_test.c testscene.cc)
add_executable(glfw_test ${GLFW_TEST_SRCS})
target_link_libraries(glfw_test glfw GLEW::GLEW OpenGL::GL)

set(GLUT_TEST_SRCS glut_test.c testscene.cc)
add_executable(glut_test ${GLUT_TEST_SRCS})
target_link_libraries(glut_test GLUT::GLUT GLEW::GLEW OpenGL::GL)

include_directories(${LIBCORTEX_INCLUDE_DIR})
set(QT_TEST_SRCS qt_test.cc testwindow.cc testscene.cc)
qt5_wrap_cpp(QT_TEST_MOC testwindow.h)
add_executable(qt_test ${QT_TEST_SRCS} ${QT_TEST_MOC})
target_link_libraries(qt_test Qt5::Gui GLEW::GLEW OpenGL::GL)

set(QTWIDGET_TEST_SRCS qtwidget_test.cc testwidget.cc testscene.cc)
qt5_wrap_cpp(QTWIDGET_TEST_MOC testwidget.h)
add_executable(qtwidget_test ${QTWIDGET_TEST_SRCS} ${QTWIDGET_TEST_MOC})
target_link_libraries(qtwidget_test Qt5::Widgets GLEW::GLEW OpenGL::GL)

add_executable(bezier_test bezier_test.cc)

add_executable(teaset_test teaset_test.cc)
target_link_libraries(teaset_test cortex)

add_executable(assimp_test assimp_test.cc)
target_link_libraries(assimp_test ${ASSIMP_LIBRARIES})

add_executable(cortex_test cortex_test.cc)
target_link_libraries(cortex_test cortex)
