/**
 * @file mainwindow.cc
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "mainwindow.h"

#include <iostream>

MainWindow::MainWindow(QWidget* parent)
: QMainWindow(parent)
{
	setupUi(this);
}

MainWindow::~MainWindow()
{
}

void MainWindow::showError(const QString& msg)
{
	std::cerr << msg.toStdString() << std::endl;
}
