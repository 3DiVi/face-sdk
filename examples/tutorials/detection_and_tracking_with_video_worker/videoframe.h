#ifndef VIDEOFRAME_H
#define VIDEOFRAME_H

#include "qcameracapture.h"

#include <pbio/IRawImage.h>

class VideoFrame : public pbio::IRawImage
{
public:
	VideoFrame(){}

	virtual ~VideoFrame(){}

	virtual const unsigned char* data() const throw();

	virtual int32_t width() const throw();

	virtual int32_t height() const throw();

	virtual int32_t format() const throw();

	QCameraCapture::FramePtr& frame();

	const QCameraCapture::FramePtr& frame() const;

private:
	QCameraCapture::FramePtr _frame;
};


inline
const unsigned char* VideoFrame::data() const throw()
{
	if(_frame->isNull() || _frame->size().isEmpty())
	{
		return NULL;
	}

	return _frame->bits();
}

inline
int32_t VideoFrame::width() const throw()
{
	return _frame->width();
}


inline
int32_t VideoFrame::height() const throw()
{
	return _frame->height();
}


inline
int32_t VideoFrame::format() const throw()
{
	if(_frame->format() == QImage::Format_Grayscale8)
	{
		return (int32_t) FORMAT_GRAY;
	}

	if(_frame->format() == QImage::Format_RGB888)
	{
		return (int32_t) FORMAT_RGB;
	}

	return -1;
}

inline
QCameraCapture::FramePtr& VideoFrame::frame()
{
	return _frame;
}

inline
const QCameraCapture::FramePtr& VideoFrame::frame() const
{
	return _frame;
}

#endif // VIDEOFRAME_H
