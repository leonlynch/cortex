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
: SimpleOpenGLWindow(parent)
{
}

TestWindow::~TestWindow()
{
	scene_unload_resources();
}

bool TestWindow::initGL()
{
	int r;

	r = scene_init();
	if (r) {
		error("Failed to initialize scene");
		return false;
	}
	r = scene_load_resources();
	if (r) {
		error("Failed to load scene resources");
		return false;
	}

	return true;
}

void TestWindow::resizeGL(int width, int height)
{
	scene_resize(width, height);
}

void TestWindow::renderGL()
{
	scene_render();
}

void TestWindow::doUpdate()
{
	scene_update();

	doRender();
}
