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

protected:
	QOpenGLContext* context;

public:
	explicit OpenGLWindow(QWindow* parent = 0);
	virtual ~OpenGLWindow();

public slots:
	virtual void doRender() = 0;

signals:
	void error(const QString& msg);
};

#endif
