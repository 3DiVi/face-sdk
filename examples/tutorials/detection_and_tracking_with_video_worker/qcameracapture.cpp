#include "worker.h"
#include "qcameracapture.h"

#include <QCameraInfo>

#include <stdexcept>

#include <QMessageBox>

CameraSurface::CameraSurface(QObject* parent) :
	QAbstractVideoSurface(parent)
{
}

QList<QVideoFrame::PixelFormat> CameraSurface::supportedPixelFormats(
	QAbstractVideoBuffer::HandleType handleType) const
{
	if (handleType == QAbstractVideoBuffer::NoHandle)
	{
		return QList<QVideoFrame::PixelFormat>()
				<< QVideoFrame::Format_RGB32
				<< QVideoFrame::Format_RGB24
				<< QVideoFrame::Format_BGR24
				<< QVideoFrame::Format_NV12
				<< QVideoFrame::Format_NV21;
	}

	return QList<QVideoFrame::PixelFormat>();
}

bool CameraSurface::start(const QVideoSurfaceFormat& format)
{
	if (!supportedPixelFormats(format.handleType()).contains(format.pixelFormat()))
	{
		qDebug() << format.handleType() << " " << format.pixelFormat() << " - format is not supported.";
		return false;
	}

	return QAbstractVideoSurface::start(format);
}

bool CameraSurface::present(const QVideoFrame& frame)
{
	if (!frame.isValid())
	{
		return false;
	}

	emit frameUpdatedSignal(frame);

	return true;
}

const int QCameraCapture::default_res_width = 640;
const int QCameraCapture::default_res_height = 480;


QCameraCapture::QCameraCapture(
	QWidget* parent,
	std::shared_ptr<Worker> worker,
	const int cam_id,
	const int res_width,
	const int res_height) : QObject(parent),
_worker(worker),
_parent(parent),
cam_id(cam_id),
res_width(res_width),
res_height(res_height)
{
	const QList<QCameraInfo> availableCameras = QCameraInfo::availableCameras();

	qDebug() << "Available cameras:";
	for (const QCameraInfo &cameraInfo : availableCameras)
	{
		qDebug() << cameraInfo.deviceName() << " " << cameraInfo.description();
	}

	if(availableCameras.empty())
	{
		throw std::runtime_error("List of available cameras is empty");
	}

	if (!(cam_id >= 0 && cam_id < availableCameras.size()))
	{
		throw std::runtime_error("Invalid camera index");
	}

	const QCameraInfo cameraInfo = availableCameras[cam_id];

	m_camera.reset(new QCamera(cameraInfo));

	connect(m_camera.data(), &QCamera::statusChanged, this, &QCameraCapture::onStatusChanged);
	connect(m_camera.data(), QOverload<QCamera::Error>::of(&QCamera::error), this, &QCameraCapture::cameraError);

	m_surface.reset(new CameraSurface());

	m_camera->setViewfinder(m_surface.data());

	connect(m_surface.data(), &CameraSurface::frameUpdatedSignal, this, &QCameraCapture::frameUpdatedSlot);
}

QCameraCapture::~QCameraCapture()
{
	if (m_camera)
	{
		m_camera->stop();
	}
}

void QCameraCapture::frameUpdatedSlot(
	const QVideoFrame& frame)
{
	QVideoFrame cloneFrame(frame);

	cloneFrame.map(QAbstractVideoBuffer::ReadOnly);

	if (cloneFrame.pixelFormat() == QVideoFrame::Format_RGB24 ||
		cloneFrame.pixelFormat() == QVideoFrame::Format_RGB32)
	{
		QImage image((const uchar*)cloneFrame.bits(),
			cloneFrame.width(),
			cloneFrame.height(),
			QVideoFrame::imageFormatFromPixelFormat(cloneFrame.pixelFormat()));

		if (image.format() == QImage::Format_RGB32)
		{
			image = image.convertToFormat(QImage::Format_RGB888);
		}

		try
		{
			FramePtr frame = FramePtr(new QImage(image));

			_worker->addFrame(frame);
		}
		catch(std::exception& ex)
		{
			stop();
			cloneFrame.unmap();

			QMessageBox::critical(
				_parent,
				tr("Face SDK error"),
				ex.what());

			return;
		}
	}

	cloneFrame.unmap();

	emit newFrameAvailable();
}

void QCameraCapture::start()
{
	m_camera->start();
}

void QCameraCapture::stop()
{
	m_camera->stop();
}

void QCameraCapture::onStatusChanged()
{
	if (m_camera->status() == QCamera::LoadedStatus)
	{
		bool found = false;
		const QList<QSize> supportedResolutions = m_camera->supportedViewfinderResolutions();
		for (const QSize &resolution : supportedResolutions)
		{
			if (resolution.width() == res_width &&
				resolution.height() == res_height)
			{
				found = true;
			}
		}

		if (!found)
		{
			qDebug() << "Resolution: " << res_width << "x" << res_width << " unsupported";
			qDebug() << "Set default resolution: " << default_res_width << "x" << default_res_height;

			res_width = default_res_width;
			res_height = default_res_height;
		}

		QCameraViewfinderSettings viewFinderSettings;
		viewFinderSettings.setResolution(
			res_width,
			res_height);
		m_camera->setViewfinderSettings(viewFinderSettings);
	}
}

void QCameraCapture::cameraError()
{
	qDebug() << "Camera error: " << m_camera->errorString();
}

