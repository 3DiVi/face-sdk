#ifndef DEPTHSENSORCAPTURE_H
#define DEPTHSENSORCAPTURE_H

#include "ImageAndDepthSource.h"

#include <QSharedPointer>
#include <QThread>
#include <QByteArray>
#include <memory>
#include <atomic>


class Worker;

class DepthSensorCapture : public QObject
{
	Q_OBJECT

public:
	typedef std::shared_ptr<QImage> RGBFramePtr;
	typedef std::shared_ptr<QByteArray> DepthFramePtr;

	explicit DepthSensorCapture(
		QWidget* parent,
		std::shared_ptr<Worker> worker);

	void start();
	void stop();

signals:
	void newFrameAvailable();

private:
	void frameUpdatedThread();

	QWidget* _parent;

	std::shared_ptr<Worker> _worker;

	QSharedPointer<QThread> thread;
	QSharedPointer<ImageAndDepthSource> depth_source;

	std::atomic<bool> shutdown = {false};
};

#endif // DEPTHSENSORCAPTURE_H
