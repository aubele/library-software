#include "LibTest.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	LibTest w;
	w.show();
	return a.exec();
}
