/**
 * @file testwindow.h
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#ifndef __TEST_WINDOW_H__
#define __TEST_WINDOW_H__

#include <qt/simpleopenglwindow.h>

class TestWindow : public SimpleOpenGLWindow
{
	Q_OBJECT

public:
	TestWindow(QWindow* parent = 0);
	virtual ~TestWindow();

protected:
	virtual bool initGL();
	virtual void resizeGL(int width, int height);
	virtual void renderGL();

public slots:
	void doUpdate();
};

#endif
