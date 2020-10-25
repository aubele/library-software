#include "LibTest.h"
#include "MyLib.h"

LibTest::LibTest(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	MyLib lib;
	lib.test();
}
