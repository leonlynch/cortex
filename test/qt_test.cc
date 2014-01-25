/**
 * @file qt_test.cc
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include <QtGui/QGuiApplication>
#include <QtCore/QTimer>

#include "testwindow.h"

int main(int argc, char** argv)
{
	int r;

	QGuiApplication app(argc, argv);

	TestWindow window;
	window.resize(640, 480);
	window.show();

	QTimer timer;
	QObject::connect(&timer, SIGNAL(timeout()), &window, SLOT(doUpdate()));
	timer.start(20);

	r = app.exec();

	return r;
}
