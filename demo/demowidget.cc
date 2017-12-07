/**
 * @file demowidget.cc
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "demowidget.h"

// TODO: remove test scene
#include "../test/testscene.h"

DemoWidget::DemoWidget(QWidget* parent)
: QOpenGLWidget(parent)
{
	connect(&timer, &QTimer::timeout, this, &DemoWidget::doUpdate);
}

DemoWidget::~DemoWidget()
{
}

void DemoWidget::initializeGL()
{
	int r;

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

	// TODO: remove test scene
	r = scene_init();
	if (r) {
		error("Failed to initialize scene");
		return;
	}
	r = scene_load_resources();
	if (r) {
		error("Failed to load scene resources");
		return;
	}
}

void DemoWidget::resizeGL(int width, int height)
{
	// TODO: resize scene

	// TODO: remove test scene
	scene_resize(width, height);
}

void DemoWidget::paintGL()
{
	// TODO: render scene

	// TODO: remove test scene
	scene_render();
}

void DemoWidget::doUpdate()
{
	// TODO: update scene

	// TODO: remove test scene
	scene_update();

	this->update();
}

void DemoWidget::setAnimation(bool enabled)
{
	emit log(QString::asprintf("%s(); enabled=%d", __FUNCTION__, enabled));
	if (enabled) {
		timer.start();
	} else {
		timer.stop();
	}
}

void DemoWidget::setAnimationInterval(int interval)
{
	emit log(QString::asprintf("%s(); interval=%d", __FUNCTION__, interval));
	timer.setInterval(interval);
}
