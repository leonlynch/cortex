/**
 * @file mainwindow.cc
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "mainwindow.h"
#include "version.h"

#include <iostream>

MainWindow::MainWindow(QWidget* parent)
: QMainWindow(parent)
{
	setupUi(this);

	applicationVersionStr->setText(VERSION);

	demowidget = new DemoWidget(this);
	connect(demowidget, &DemoWidget::log, this, &MainWindow::appendLog);
	connect(demowidget, &DemoWidget::error, this, &MainWindow::appendError);
	connect(demowidget, &DemoWidget::versionString, openglVersionStr, &QLabel::setText);
	centralWidget()->layout()->addWidget(demowidget);
}

MainWindow::~MainWindow()
{
	if (demowidget)
		delete demowidget;
}

void MainWindow::appendLog(const QString& msg)
{
	std::cout << msg.toStdString() << std::endl;
}

void MainWindow::appendError(const QString& msg)
{
	std::cerr << msg.toStdString() << std::endl;
}
