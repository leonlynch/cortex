/**
 * @file demowidget.h
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#ifndef __DEMO_WIDGET_H__
#define __DEMO_WIDGET_H__

#include <QtWidgets/QOpenGLWidget>
#include <QtCore/QTimer>
#include <QtCore/QString>

class DemoWidget : public QOpenGLWidget
{
	Q_OBJECT

private:
	QTimer timer;

public:
	explicit DemoWidget(QWidget* parent = 0);
	virtual ~DemoWidget();

protected:
	virtual void initializeGL();
	virtual void resizeGL(int width, int height);
	virtual void paintGL();

public slots:
	virtual void doUpdate();
	void setAnimation(bool enabled);
	void setAnimationInterval(int interval);

signals:
	void log(const QString& msg);
	void error(const QString& msg);
	void versionString(const QString& msg);
};

#endif
