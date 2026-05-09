/**
 * @file mainwindow.h
 *
 * Copyright 2013, 2026 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#ifndef DEMO_MAINWINDOW_H
#define DEMO_MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtCore/QString>

#include "ui_mainwindow.h"

// Forward declarations
class DemoWidget;

class MainWindow : public QMainWindow, private Ui::MainWindow
{
	Q_OBJECT

private:
	DemoWidget* demowidget;

public:
	MainWindow(QWidget* parent = nullptr);

private slots:
	// connect-by-name
	// TODO: add slots here

	void appendLog(const QString& msg);
	void appendError(const QString& msg);
};

#endif
