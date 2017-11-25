/**
 * @file qt_test.cc
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include <QtGui/QGuiApplication>

#include "testwindow.h"

int main(int argc, char** argv)
{
	QGuiApplication app(argc, argv);

	QSurfaceFormat format(QSurfaceFormat::DebugContext);
	format.setRenderableType(QSurfaceFormat::OpenGL);
	format.setProfile(QSurfaceFormat::CompatibilityProfile); // Core profile is only supported for QOpenGLWindow from Qt-5.9 due to QPainter limitations
	format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
	QSurfaceFormat::setDefaultFormat(format);

	TestWindow window;
	window.resize(640, 480);
	window.show();

	return app.exec();
}
