/**
 * @file testwidget.cc
 *
 * Copyright 2013, 2026 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "testwidget.h"

#include "testscene.h"

#include <QtGui/QKeyEvent>

TestWidget::TestWidget(QWidget* parent)
: QOpenGLWidget(parent)
{
	setFocusPolicy(Qt::StrongFocus);
	connect(&timer, &QTimer::timeout, this, &TestWidget::doUpdate);

	setTextureFormat(GL_SRGB8_ALPHA8);
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
    scene_render(SCENE_DEMO_CUBE);
}

void TestWidget::doUpdate()
{
	scene_update();

	this->update();
}

void TestWidget::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_F) {
		if (isFullScreen()) {
			showNormal();
		} else {
			showFullScreen();
		}
	}
}
