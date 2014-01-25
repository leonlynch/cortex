/**
 * @file simpleopenglwindow.h
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#ifndef __SIMPLE_OPENGL_WINDOW_H__
#define __SIMPLE_OPENGL_WINDOW_H__

#include "openglwindow.h"

class SimpleOpenGLWindow : public OpenGLWindow
{
	Q_OBJECT

private:
	bool valid;
	bool initialized;

public:
	explicit SimpleOpenGLWindow(QWindow* parent = 0);

protected:
	void resizeEvent(QResizeEvent*);

	virtual bool initGL() = 0;
	virtual void resizeGL(int width, int height) = 0;
	virtual void renderGL() = 0;

public slots:
	void doRender();
};

#endif
