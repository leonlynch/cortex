/**
 * @file simpleopenglwindow.cc
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "simpleopenglwindow.h"

#include <QtGui/QOpenGLContext>
#include <QtGui/QResizeEvent>

SimpleOpenGLWindow::SimpleOpenGLWindow(QWindow* parent)
: OpenGLWindow(parent),
  valid(false),
  initialized(false)
{
	if (context->isValid())
		valid = true;
}

void SimpleOpenGLWindow::resizeEvent(QResizeEvent* e)
{
	context->makeCurrent(this);
	resizeGL(e->size().width(), e->size().height());
	QWindow::resizeEvent(e);
}

void SimpleOpenGLWindow::doRender()
{
	if (!isExposed() || !context || !context->isValid() || !valid)
		return;

	context->makeCurrent(this);

	if (valid && !initialized) {
		initialized = initGL();
		if (!initialized) {
			error("initGL() failed");
			valid = false;
			return;
		}
	}

	renderGL();
	context->swapBuffers(this);
}
