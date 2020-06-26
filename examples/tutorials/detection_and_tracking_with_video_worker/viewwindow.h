#ifndef VIEWWINDOW_H
#define VIEWWINDOW_H

#include <QWidget>

#include "qcameracapture.h"
#include "worker.h"

#include "facesdkparameters.h"

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

	static const int camera_image_width;
	static const int camera_image_height;

	QScopedPointer<QCameraCapture> _qCamera;
	bool _running;

	std::shared_ptr<Worker> _worker;

	pbio::FacerecService::Ptr _service;
	FaceSdkParameters _face_sdk_parameters;
};

#endif // VIEWWINDOW_H
