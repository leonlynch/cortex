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
#include <QtGui/QResizeEvent>

OpenGLWindow::OpenGLWindow(
	void (*resize_func)(int width, int height),
	void (*render_func)(),
	void (*update_func)(),
	QWindow* parent
)
: QWindow(parent),
  resize_func(resize_func),
  render_func(render_func),
  update_func(update_func)
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

bool OpenGLWindow::makeContextCurrent()
{
	return context->makeCurrent(this);
}

void OpenGLWindow::resizeEvent(QResizeEvent* e)
{
	context->makeCurrent(this);
	resize_func(e->size().width(), e->size().height());
	QWindow::resizeEvent(e);
}


void OpenGLWindow::doRender()
{
	if (!isExposed() || !context || !render_func)
		return;

	context->makeCurrent(this);
	render_func();
	context->swapBuffers(this);
}

void OpenGLWindow::doUpdate()
{
	update_func();

	doRender();
}
