/**
 * @file qt_test.cc
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include <QtGui/QGuiApplication>
#include <QtGui/QSurfaceFormat>
#include <QOpenGLContext>
#include <QtCore/QTimer>

#include "openglwindow.h"
#include "scene.h"

int main(int argc, char** argv)
{
	int r;

	QGuiApplication app(argc, argv);

	QSurfaceFormat format;
	format.setRenderableType(QSurfaceFormat::OpenGL);
// 	format.setMajorVersion(3);
// 	format.setMinorVersion(2);
// 	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);

	OpenGLWindow window;
	window.setFormat(format);
	window.resize(640, 480);
	window.init(&scene_resize, &scene_render);
	window.show();

	window.context->makeCurrent(&window);
	r = scene_init();
	if (r)
		return 1;
	r = scene_load_resources();
	if (r)
		return 1;

	QTimer timer;
	QObject::connect(&timer, SIGNAL(timeout()), &window, SLOT(render()));
    timer.start(16);

	r = app.exec();

	scene_unload_resources();

	return r;
}
