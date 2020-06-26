#ifndef QCAMERACAPTURE_H
#define QCAMERACAPTURE_H

#include <QObject>

#include <QCamera>
#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>
#include <QVideoFrame>
#include <QScopedPointer>

#include <memory>

class CameraSurface : public QAbstractVideoSurface
{
	Q_OBJECT

public:
	explicit CameraSurface(QObject* parent = 0);

	bool present(const QVideoFrame& frame);

	QList<QVideoFrame::PixelFormat> supportedPixelFormats(
		QAbstractVideoBuffer::HandleType type = QAbstractVideoBuffer::NoHandle) const;

	bool start(const QVideoSurfaceFormat& format);

signals:
	void frameUpdatedSignal(const QVideoFrame&);
};

class Worker;

class QCameraCapture : public QObject
{
	Q_OBJECT
public:
	typedef std::shared_ptr<QImage> FramePtr;

	explicit QCameraCapture(
		QWidget* parent,
		std::shared_ptr<Worker> worker,
		const int cam_id,
		const int res_width,
		const int res_height);
	virtual ~QCameraCapture();

	void start();
	void stop();

signals:
	void newFrameAvailable();

public slots:
	void frameUpdatedSlot(const QVideoFrame&);

private slots:
	void onStatusChanged();
	void cameraError();

private:
	static const int default_res_width;
	static const int default_res_height;

	std::shared_ptr<Worker> _worker;

	QWidget* _parent;
	int cam_id;
	int res_width;
	int res_height;

	QScopedPointer<QCamera> m_camera;
	QScopedPointer<CameraSurface> m_surface;
};

#endif // QCAMERACAPTURE_H
