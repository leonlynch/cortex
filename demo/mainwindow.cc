/**
 * @file mainwindow.cc
 *
 * Copyright 2013, 2026 Leon Lynch
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

	connect(animationCheckBox, &QCheckBox::checkStateChanged, animationIntervalSpinBox,
		[this](Qt::CheckState state) {
			animationIntervalSpinBox->setEnabled(state == Qt::Checked);
		}
	);

	demowidget = new DemoWidget(this);
	demowidget->setAnimation(animationCheckBox->isChecked());
	demowidget->setAnimationInterval(animationIntervalSpinBox->value());
	connect(animationCheckBox, &QCheckBox::checkStateChanged, demowidget,
		[this](Qt::CheckState state) {
			demowidget->setAnimation(state == Qt::Checked);
		}
	);
	connect(animationIntervalSpinBox, SIGNAL(valueChanged(int)), demowidget, SLOT(setAnimationInterval(int)));
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
