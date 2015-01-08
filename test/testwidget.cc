/**
 * @file testwidget.cc
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "testwidget.h"

#include "scene.h"

TestWidget::TestWidget(QWidget* parent)
: QOpenGLWidget(parent)
{
	connect(&timer, SIGNAL(timeout()), SLOT(doUpdate()));
}

TestWidget::~TestWidget()
{
	timer.stop();

	makeCurrent();
	scene_unload_resources();
	doneCurrent();
}

void TestWidget::initializeGL()
{
	int r;

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

	timer.start(20);
}

void TestWidget::resizeGL(int width, int height)
{
	scene_resize(width, height);
}

void TestWidget::paintGL()
{
    scene_render();
}

void TestWidget::doUpdate()
{
	scene_update();

	this->update();
}
