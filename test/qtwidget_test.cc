/**
 * @file qtwidget_test.cc
 *
 * Copyright 2013, 2026 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include <QtGui/QSurfaceFormat>
#include <QtGui/QColorSpace>
#include <QtWidgets/QApplication>

#include "testwidget.h"

int main(int argc, char** argv)
{
	QSurfaceFormat format(QSurfaceFormat::DebugContext);
	format.setRenderableType(QSurfaceFormat::OpenGL);
	format.setVersion(4, 6);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setColorSpace(QColorSpace(QColorSpace::SRgb));
	format.setDepthBufferSize(32);
	format.setSamples(4);
	format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
	QSurfaceFormat::setDefaultFormat(format);

	QApplication app(argc, argv);
	TestWidget widget;
	widget.show();

	return app.exec();
}
