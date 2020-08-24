/**
 * @file qtwidget_test.cc
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include <QtWidgets/QApplication>

#include "testwidget.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	QSurfaceFormat format(QSurfaceFormat::DebugContext);
	format.setRenderableType(QSurfaceFormat::OpenGL);
	format.setVersion(3, 3);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setDepthBufferSize(32);
	format.setSamples(4);
	format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
	QSurfaceFormat::setDefaultFormat(format);

	TestWidget widget;
	widget.show();

	return app.exec();
}
