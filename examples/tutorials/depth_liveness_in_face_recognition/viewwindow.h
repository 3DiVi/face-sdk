#ifndef VIEWWINDOW_H
#define VIEWWINDOW_H

#include <QWidget>

#include "worker.h"

#include <facerec/libfacerec.h>

namespace Ui {
class ViewWindow;
}

class ViewWindow : public QWidget
{
	Q_OBJECT

public:
	explicit ViewWindow(
		QWidget *parent,
		pbio::FacerecService::Ptr service,
		FaceSdkParameters face_sdk_parameters);

	~ViewWindow();

	void runProcessing();
	void stopProcessing();

private slots:
	void draw();

private:
	Ui::ViewWindow *ui;

	QScopedPointer<DepthSensorCapture> _camera;
	bool _running;

	std::shared_ptr<Worker> _worker;

	pbio::FacerecService::Ptr _service;
};

#endif // VIEWWINDOW_H
