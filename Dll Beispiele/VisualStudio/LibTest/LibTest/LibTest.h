#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_LibTest.h"

class LibTest : public QMainWindow
{
	Q_OBJECT

public:
	LibTest(QWidget *parent = Q_NULLPTR);

private:
	Ui::LibTestClass ui;
};
