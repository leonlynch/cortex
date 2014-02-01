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

bool SimpleOpenGLWindow::event(QEvent* ev)
{
	switch (ev->type()) {
		case QEvent::Resize: {
			if (valid && !initialized && context && context->isValid()) {
				context->makeCurrent(this);
				initialized = initGL();
				if (!initialized) {
					error("initGL() failed");
					valid = false;
				}
			}

			QResizeEvent* event = static_cast<QResizeEvent*>(ev);
			resizeGL(event->size().width(), event->size().height());
			break;
		}
		default:
			break;
	}

	return QWindow::event(ev);
}

void SimpleOpenGLWindow::doRender()
{
	if (!valid || !initialized || !isExposed() || !context || !context->isValid())
		return;

	context->makeCurrent(this);
	renderGL();
	context->swapBuffers(this);
}
