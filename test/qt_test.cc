/**
 * @file qt_test.cc
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include <QtGui/QGuiApplication>
#include <QtCore/QTimer>

#include "openglwindow.h"
#include "scene.h"

int main(int argc, char** argv)
{
	int r;

	QGuiApplication app(argc, argv);

	OpenGLWindow window(&scene_resize, &scene_render, &scene_update);
	window.resize(640, 480);
	window.show();

	window.makeContextCurrent();
	r = scene_init();
	if (r)
		return 1;
	r = scene_load_resources();
	if (r)
		return 1;

	QTimer timer;
	QObject::connect(&timer, SIGNAL(timeout()), &window, SLOT(doUpdate()));
	timer.start(20);

	r = app.exec();

	scene_unload_resources();

	return r;
}
