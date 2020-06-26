#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	try
	{
		MainWindow w;
		w.show();

		return a.exec();
	}
	catch(std::exception& ex)
	{
		qDebug() << "Exception caught: " << ex.what();
	}
	catch(...)
	{
		qDebug() << "Unknown exception caught";
	}

	return 0;
}
