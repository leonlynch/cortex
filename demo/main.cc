/**
 * @file main.cc
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include <QtWidgets/QApplication>
#include <QtGui/QSurfaceFormat>

#include "mainwindow.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	app.setApplicationName("Cortex Demo");

	QSurfaceFormat format(QSurfaceFormat::DebugContext);
	format.setRenderableType(QSurfaceFormat::OpenGL);
	format.setVersion(3, 3);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setDepthBufferSize(32);
	format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
	QSurfaceFormat::setDefaultFormat(format);

	MainWindow mainwindow;
	mainwindow.showMaximized();

	return app.exec();
}
