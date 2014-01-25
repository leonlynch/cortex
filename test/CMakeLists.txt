##############################################################################
# Copyright (c) 2013 Leon Lynch
#
# This file is licensed under the terms of the MIT license.
# See LICENSE file.
##############################################################################

cmake_minimum_required(VERSION 2.8.11)

set(GLFW_TEST_SRCS glfw_test.c scene.cc)
add_executable(glfw_test ${GLFW_TEST_SRCS})
target_link_libraries(glfw_test ${GLFW_LIBRARY} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})

set(GLUT_TEST_SRCS glut_test.c scene.cc)
add_executable(glut_test ${GLUT_TEST_SRCS})
target_link_libraries(glut_test ${GLUT_LIBRARY} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})

include_directories(${LIBCORTEX_INCLUDE_DIR})
set(QT_TEST_SRCS qt_test.cc testwindow.cc scene.cc)
qt5_wrap_cpp(QT_TEST_MOC testwindow.h)
add_executable(qt_test ${QT_TEST_SRCS} ${QT_TEST_MOC})
target_link_libraries(qt_test cortex_qt Qt5::Gui ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})
