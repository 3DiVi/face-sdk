#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "facesdkparameters.h"

#include <facerec/libfacerec.h>

namespace Ui {
class MainWindow;
}

class ViewWindow;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	virtual ~MainWindow();

private:
	bool initFaceSdkService();

private:
	Ui::MainWindow *ui;

	QScopedPointer<ViewWindow> _view;

	FaceSdkParameters _face_sdk_parameters;

	pbio::FacerecService::Ptr _service;
};

#endif // MAINWINDOW_H
