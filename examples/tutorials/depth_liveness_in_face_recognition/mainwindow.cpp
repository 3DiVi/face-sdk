#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "viewwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	if (!initFaceSdkService())
	{
		throw std::runtime_error("Face SDK initialization error");
	}

	try
	{
		_view.reset(new ViewWindow(
			this,
			_service,
			_face_sdk_parameters));

		ui->viewLayout->addWidget(_view.data());

		_view->runProcessing();
	}
	catch(std::exception &ex)
	{
		QMessageBox::critical(
			this,
			"Initialization Error",
			ex.what());

		throw;
	}
}

MainWindow::~MainWindow()
{
	if (_view)
	{
		_view->stopProcessing();
	}

	delete ui;
}

bool MainWindow::initFaceSdkService()
{
	// Integrate Face SDK
	QString error;
	try
	{
		#ifdef _WIN32
			std::string facerec_lib_path = _face_sdk_parameters.face_sdk_path + "/bin/facerec.dll";
		#else
			std::string facerec_lib_path = _face_sdk_parameters.face_sdk_path + "/lib/libfacerec.so";
		#endif
		_service = pbio::FacerecService::createService(
			facerec_lib_path,
			_face_sdk_parameters.face_sdk_path + "/conf/facerec");

		return true;
	}
	catch(const std::exception &e)
	{
		error = tr("Can't init Face SDK service: '") + e.what() + "'.";
	}
	catch(...)
	{
		error = tr("Can't init Face SDK service: ... exception.");
	}

	QMessageBox::critical(
		this,
		tr("Face SDK error"),
		error + "\n" +
		tr("Try to change face sdk parameters."));

	return false;
}
