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
#include <QtGui/QResizeEvent>
#include <QtGui/QOpenGLFunctions>

OpenGLWindow::OpenGLWindow(QWindow* parent)
: QWindow(parent)
{
	setSurfaceType(QSurface::OpenGLSurface);
}

OpenGLWindow::~OpenGLWindow()
{
}

void OpenGLWindow::init(void (*resize_func)(int width, int height), void (*render_func)())
{
	context = new QOpenGLContext(this);
	context->setFormat(requestedFormat());
	context->create();
	context->makeCurrent(this);

	this->resize_func = resize_func;
	this->render_func = render_func;
}

void OpenGLWindow::makeContextCurrent()
{
	context->makeCurrent(this);
}

void OpenGLWindow::resizeEvent(QResizeEvent* event)
{
	context->makeCurrent(this);
	resize_func(event->size().width(), event->size().height());

	QWindow::resizeEvent(event);
}


void OpenGLWindow::render()
{
	if (context && render_func && isExposed()) {
		context->makeCurrent(this);
		render_func();
		context->swapBuffers(this);
	}
}
