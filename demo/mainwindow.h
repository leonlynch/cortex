/**
 * @file mainwindow.h
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include <QtWidgets/QMainWindow>
#include <QtCore/QString>

#include "ui_mainwindow.h"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = 0);
	virtual ~MainWindow();

private slots:
	// connect-by-name
	// TODO: add slots here

	void showError(const QString& msg);
};
