#if defined(WITH_OPENNI2)
#include "OpenniSource.h"
#elif defined(WITH_REALSENSE)
#include "RealSenseSource.h"
#endif

#include "worker.h"
#include "depthsensorcapture.h"

#include <assert.h>

#include <QMessageBox>


DepthSensorCapture::DepthSensorCapture(
	QWidget* parent,
	std::shared_ptr<Worker> worker) :
_parent(parent),
_worker(worker)
{
#if defined(WITH_OPENNI2)
	depth_source.reset(new OpenniSource());
#else
	depth_source.reset(new RealSenseSource());
#endif

	thread.reset(new QThread());
	this->moveToThread(thread.data());

	connect(thread.data(), &QThread::started, this, &DepthSensorCapture::frameUpdatedThread);
}

void DepthSensorCapture::start()
{
	if (!thread->isRunning())
	{
		thread->start();
	}
}

void DepthSensorCapture::stop()
{
	if (thread->isRunning())
	{
		shutdown = true;

		thread->quit();
		thread->wait();
	}
}

void DepthSensorCapture::frameUpdatedThread()
{
	while(!shutdown)
	{
		try
		{
			ImageAndDepth data;
			depth_source->get(data);

			_worker->addFrame(data);

			emit newFrameAvailable();
		}
		catch(std::exception& ex)
		{
			stop();

			QMessageBox::critical(
				_parent,
				tr("Face SDK error"),
				ex.what());

			break;
		}
	}
}
