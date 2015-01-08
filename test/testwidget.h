/**
 * @file testwidget.h
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#ifndef __TEST_WIDGET_H__
#define __TEST_WIDGET_H__

#include <QtWidgets/QOpenGLWidget>
#include <QtCore/QTimer>

class TestWidget : public QOpenGLWidget
{
	Q_OBJECT

private:
	QTimer timer;

public:
	TestWidget(QWidget* parent = 0);
	virtual ~TestWidget();

protected:
	virtual void initializeGL();
	virtual void resizeGL(int width, int height);
	virtual void paintGL();

public slots:
	void doUpdate();

signals:
	void error(const QString& msg);
};

#endif
