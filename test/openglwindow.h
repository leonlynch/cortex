/**
 * @file openglwindow.h
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#ifndef __OPENGL_WINDOW_H__
#define __OPENGL_WINDOW_H__

#include <QtGui/QWindow>

QT_BEGIN_NAMESPACE
class QOpenGLContext;
QT_END_NAMESPACE

class OpenGLWindow : public QWindow
{
	Q_OBJECT

public:
	explicit OpenGLWindow(QWindow* parent = 0);
	virtual ~OpenGLWindow();

	void init(void (*resize_func)(int width, int height), void (*render_func)());
	void makeContextCurrent();

protected:
	virtual void resizeEvent(QResizeEvent*);

public slots:
	void render();

public:
	QOpenGLContext* context;

	void (*resize_func)(int width, int height);
	void (*render_func)();
};

#endif
