/**
 * @file testwindow.cc
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "testwindow.h"

#include "scene.h"

TestWindow::TestWindow(QWindow* parent)
: QOpenGLWindow(QOpenGLWindow::NoPartialUpdate, parent)
{
	connect(&timer, &QTimer::timeout, this, &TestWindow::doUpdate);
}

TestWindow::~TestWindow()
{
	timer.stop();

	makeCurrent();
	scene_unload_resources();
	doneCurrent();
}

void TestWindow::initializeGL()
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

void TestWindow::resizeGL(int width, int height)
{
	scene_resize(width, height);
}

void TestWindow::paintGL()
{
	scene_render();
}

void TestWindow::doUpdate()
{
	scene_update();

	this->update();
}
