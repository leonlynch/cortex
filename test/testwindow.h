/**
 * @file testwindow.h
 *
 * Copyright 2013, 2026 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#ifndef TEST_WINDOW_H
#define TEST_WINDOW_H

#include <QtOpenGL/QOpenGLWindow>
#include <QtCore/QTimer>

class TestWindow : public QOpenGLWindow
{
	Q_OBJECT

private:
	QTimer timer;

public:
	TestWindow(QWindow* parent = 0);
	virtual ~TestWindow();

protected:
	virtual void initializeGL() override;
	virtual void resizeGL(int width, int height) override;
	virtual void paintGL() override;

public slots:
	void doUpdate();

signals:
	void error(const QString& msg);
};

#endif
