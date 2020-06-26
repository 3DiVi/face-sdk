#ifndef __OPENNI_SOURCE_H__0x03188cad__
#define __OPENNI_SOURCE_H__0x03188cad__

#include <OpenNI.h>

#include <pbio/IRawImage.h>
#include "ImageAndDepthSource.h"

#include <iostream>
#include <stdexcept>
#include <cmath>


class OpenniSource : public ImageAndDepthSource
{
public:

	virtual ~OpenniSource();

	virtual void get(ImageAndDepth& data);

	OpenniSource();

private:

	openni::Device device;

	openni::VideoStream streamdepth;
	openni::VideoStream streamcolor;

};

#define OPENNI_CALL_AND_CHECK_0xbe26e9b4(A) check_openni_status( A, #A )

static
inline
void check_openni_status(const openni::Status s, char const* const msg)
{
	if(s != openni::STATUS_OK)
		throw std::runtime_error(std::string(msg) + " failed: '" + openni::OpenNI::getExtendedError() + "'");
}


inline
OpenniSource::~OpenniSource()
{
	// abort();  // this is bad! but otherwise you need to reconnect device after every run

	streamdepth.stop();

	streamdepth.destroy();

	streamcolor.stop();

	streamcolor.destroy();

	device.close();

	openni::OpenNI::shutdown();
}

inline
OpenniSource::OpenniSource()
{
	OPENNI_CALL_AND_CHECK_0xbe26e9b4( openni::OpenNI::initialize() );

	OPENNI_CALL_AND_CHECK_0xbe26e9b4( device.open(openni::ANY_DEVICE) );

	assert(device.getSensorInfo(openni::SENSOR_DEPTH) != NULL);

	OPENNI_CALL_AND_CHECK_0xbe26e9b4( streamdepth.create(device, openni::SENSOR_DEPTH) );

	OPENNI_CALL_AND_CHECK_0xbe26e9b4( streamdepth.start() );

	assert(device.getSensorInfo(openni::SENSOR_COLOR) != NULL);

	OPENNI_CALL_AND_CHECK_0xbe26e9b4( streamcolor.create(device, openni::SENSOR_COLOR) );

	OPENNI_CALL_AND_CHECK_0xbe26e9b4( streamcolor.start() );

	if (openni::STATUS_OK != device.setDepthColorSyncEnabled(true))
	{
		std::cout << "\n\nATTENTION!!!\nCouldn't enable depth and color streams synchronization\n" << std::endl;
	}

	OPENNI_CALL_AND_CHECK_0xbe26e9b4( device.setImageRegistrationMode(openni::IMAGE_REGISTRATION_DEPTH_TO_COLOR) );
}

inline
void OpenniSource::get(ImageAndDepth& result_frame)
{
	openni::VideoStream* streams[] = {
		&streamcolor,
		&streamdepth};

	const int wait_timeout_milliseconds = 10000;

	int ready_stream_i = -1;
	OPENNI_CALL_AND_CHECK_0xbe26e9b4(
		openni::OpenNI::waitForAnyStream(
			streams,
			2,
			&ready_stream_i,
			wait_timeout_milliseconds));

	assert(ready_stream_i == 0 || ready_stream_i == 1);

	openni::VideoFrameRef new_frame;

	OPENNI_CALL_AND_CHECK_0xbe26e9b4( streams[ready_stream_i]->readFrame(&new_frame) );

	switch(new_frame.getSensorType())
	{
		case openni::SENSOR_COLOR:
		{
			assert(new_frame.getSensorType() == openni::SENSOR_COLOR);

			assert(new_frame.getVideoMode().getPixelFormat() == openni::PIXEL_FORMAT_RGB888);

			result_frame.image_timestamp_microsec = new_frame.getTimestamp();

			result_frame.color_image.assign(
				(const uint8_t*)new_frame.getData(),
				(const uint8_t*)new_frame.getData() + new_frame.getStrideInBytes() * new_frame.getHeight());

			result_frame.color_width = new_frame.getWidth();
			result_frame.color_height = new_frame.getHeight();
			result_frame.color_stride_in_bytes = new_frame.getStrideInBytes();
			result_frame.color_format = pbio::IRawImage::FORMAT_RGB;

			break;
		}

		case openni::SENSOR_DEPTH:
		{
			assert(new_frame.getSensorType() == openni::SENSOR_DEPTH);

			result_frame.depth_timestamp_microsec = new_frame.getTimestamp();

			result_frame.depth_image.assign(
				(const uint8_t*)new_frame.getData(),
				(const uint8_t*)new_frame.getData() + new_frame.getStrideInBytes() * new_frame.getHeight());

			result_frame.depth_opts.depth_data_stride_in_bytes = new_frame.getStrideInBytes();

			result_frame.depth_opts.depth_map_rows = new_frame.getHeight();
			result_frame.depth_opts.depth_map_cols = new_frame.getWidth();

			result_frame.depth_opts.depth_map_2_image_offset_x = 0;
			result_frame.depth_opts.depth_map_2_image_offset_y = 0;
			result_frame.depth_opts.depth_map_2_image_scale_x = 1;
			result_frame.depth_opts.depth_map_2_image_scale_y = 1;

			result_frame.depth_opts.horizontal_fov = streamdepth.getHorizontalFieldOfView();
			result_frame.depth_opts.vertical_fov   = streamdepth.getVerticalFieldOfView();

			result_frame.depth_opts.horizontal_fov *= 180 / M_PI;
			result_frame.depth_opts.vertical_fov   *= 180 / M_PI;

			switch( new_frame.getVideoMode().getPixelFormat() )
			{
				case openni::PIXEL_FORMAT_DEPTH_1_MM:
					result_frame.depth_opts.depth_unit_in_millimeters = 1;
					break;
				case openni::PIXEL_FORMAT_DEPTH_100_UM:
					result_frame.depth_opts.depth_unit_in_millimeters = 0.1;
					break;
				default:
					assert(false);
			}

			break;
		}

		default:
			assert(false);
	}

	new_frame.release();
}

#endif  // __OPENNI_SOURCE_H__0x03188cad__
