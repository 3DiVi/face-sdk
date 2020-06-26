#include "viewwindow.h"
#include "ui_viewwindow.h"

#include "drawfunction.h"

#include "depthsensorcapture.h"


ViewWindow::ViewWindow(
	QWidget *parent,
	pbio::FacerecService::Ptr service,
	FaceSdkParameters face_sdk_parameters) :
QWidget(parent),
ui(new Ui::ViewWindow),
_service(service)
{
	ui->setupUi(this);

	_running = false;

	_worker = std::make_shared<Worker>(
		_service,
		face_sdk_parameters);

	_camera.reset(new DepthSensorCapture(
		this,
		_worker));

	connect(_camera.data(), &DepthSensorCapture::newFrameAvailable, this, &ViewWindow::draw);
}

ViewWindow::~ViewWindow()
{
	stopProcessing();

	delete ui;
}

void ViewWindow::draw()
{
	Worker::DrawingData data;
	_worker->getDataToDraw(data);

	// If data is the same, image is not redrawn
	if(!data.updated)
	{
		return;
	}

	// Drawing
	const QImage image = DrawFunction::Draw(data, _worker->getDatabase());

	ui->frame->setPixmap(QPixmap::fromImage(image));
}

void ViewWindow::stopProcessing()
{
	if (!_running)
		return;

	_camera->stop();
	_running = false;
}

void ViewWindow::runProcessing()
{
	if (_running)
		return;

	_camera->start();

	_running = true;
}

