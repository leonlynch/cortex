/**
 * @file openglwindow.cc
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "openglwindow.h"

#include <QtGui/QOpenGLContext>
#include <QtGui/QSurfaceFormat>

OpenGLWindow::OpenGLWindow(QWindow* parent)
: QWindow(parent),
  context(nullptr)
{
	QSurfaceFormat format;
	format.setRenderableType(QSurfaceFormat::OpenGL);
// 	format.setMajorVersion(3);
// 	format.setMinorVersion(2);
// 	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
	format.setDepthBufferSize(32);

	setSurfaceType(QSurface::OpenGLSurface);
	setFormat(format);

	context = new QOpenGLContext(this);
	context->setFormat(requestedFormat());
	context->create();
	context->makeCurrent(this);
}

OpenGLWindow::~OpenGLWindow()
{
}
