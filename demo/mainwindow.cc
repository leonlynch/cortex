/**
 * @file mainwindow.cc
 *
 * Copyright 2013, 2026 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "mainwindow.h"
#include "demo_config.h"
#include "demowidget.h"

#include <iostream>

MainWindow::MainWindow(QWidget* parent)
: QMainWindow(parent)
{
	setupUi(this);

	applicationVersionStr->setText(DEMO_VERSION);

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
	connect(animationIntervalSpinBox, &QSpinBox::valueChanged, demowidget, &DemoWidget::setAnimationInterval);
	connect(demowidget, &DemoWidget::log, this, &MainWindow::appendLog);
	connect(demowidget, &DemoWidget::error, this, &MainWindow::appendError);
	connect(demowidget, &DemoWidget::versionString, openglVersionStr, &QLabel::setText);
	centralWidget()->layout()->addWidget(demowidget);
}


void MainWindow::appendLog(const QString& msg)
{
	std::cout << msg.toStdString() << '\n';
}

void MainWindow::appendError(const QString& msg)
{
	std::cerr << msg.toStdString() << '\n';
}
