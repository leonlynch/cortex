/**
 * @file main.cc
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include <QtWidgets/QApplication>

#include "mainwindow.h"

int main(int argc, char** argv)
{
	QApplication application(argc, argv);
	application.setApplicationName("Cortex Demo");

	MainWindow mainwindow;
	mainwindow.showMaximized();

	return application.exec();
}
