/**
 * @file demowidget.cc
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "demowidget.h"

DemoWidget::DemoWidget(QWidget* parent)
: QOpenGLWidget(parent)
{
}

DemoWidget::~DemoWidget()
{
}

void DemoWidget::initializeGL()
{
	// emit OpenGL version string
	const GLubyte* version_str;
	version_str = glGetString(GL_VERSION);
	if (!version_str) {
		emit error("GL version not available");
		emit versionString("Unknown");
	} else {
		emit log((const char*)version_str);
		emit versionString((const char*)version_str);
	}

	// TODO: initialize scene
}

void DemoWidget::resizeGL(int width, int height)
{
	// TODO: resize scene
}

void DemoWidget::paintGL()
{
	// TODO: render scene
}

void DemoWidget::doUpdate()
{
	// TODO: update scene

	this->update();
}
