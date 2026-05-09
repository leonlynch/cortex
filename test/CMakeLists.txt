##############################################################################
# Copyright 2013, 2026 Leon Lynch
#
# This file is licensed under the terms of the MIT license.
# See LICENSE file.
##############################################################################

include_directories(${LIBCORTEX_INCLUDE_DIR})

set(TESTSCENE_SRCS testscene.cc ../src/teaset.cc ../src/internal/teaset_geometry.cc)

set(GLFW_TEST_SRCS glfw_test.c ${TESTSCENE_SRCS})
add_executable(glfw_test ${GLFW_TEST_SRCS})
target_link_libraries(glfw_test glfw GLEW::GLEW OpenGL::GL)

set(QT_TEST_SRCS qt_test.cc testwindow.cc ${TESTSCENE_SRCS})
qt_wrap_cpp(QT_TEST_MOC testwindow.h)
add_executable(qt_test ${QT_TEST_SRCS} ${QT_TEST_MOC})
target_link_libraries(qt_test Qt6::OpenGL GLEW::GLEW OpenGL::GL)

set(QTWIDGET_TEST_SRCS qtwidget_test.cc testwidget.cc ${TESTSCENE_SRCS})
qt_wrap_cpp(QTWIDGET_TEST_MOC testwidget.h)
add_executable(qtwidget_test ${QTWIDGET_TEST_SRCS} ${QTWIDGET_TEST_MOC})
target_link_libraries(qtwidget_test Qt6::OpenGLWidgets GLEW::GLEW OpenGL::GL)

add_executable(bezier_test bezier_test.cc)

add_executable(teaset_test teaset_test.cc)
target_link_libraries(teaset_test cortex)

add_executable(assimp_test assimp_test.cc)
target_link_libraries(assimp_test assimp::assimp)

add_executable(cortex_test cortex_test.cc)
target_link_libraries(cortex_test cortex)
