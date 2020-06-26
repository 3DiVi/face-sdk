#ifndef __REALSENSE_SOURCE_H__0x9762092f__
#define __REALSENSE_SOURCE_H__0x9762092f__

#include <librealsense2/rs.hpp>
#include <librealsense2/rsutil.h>  // for rs2_fov

#include <pbio/IRawImage.h>
#include "ImageAndDepthSource.h"

#include <iostream>


class RealSenseSource : public ImageAndDepthSource
{
public:

	virtual ~RealSenseSource();

	virtual void get(ImageAndDepth& data);

	RealSenseSource();

private:
	std::shared_ptr<rs2::align> align_to_color;
	rs2::pipeline pipe;

	float _depth_scale = 0.f;

	int64_t _prev_color_frame_number = -1;
	int64_t _prev_depth_frame_number = -1;
};

RealSenseSource::RealSenseSource()
{
	align_to_color = std::make_shared<rs2::align>(RS2_STREAM_COLOR);

	rs2::config config;

	config.disable_all_streams();

	config.enable_stream(
		RS2_STREAM_COLOR,  // stream type
		-1,                // stream index, -1 mean any
		640,               // stream image width
		480,               // stream image height
		RS2_FORMAT_RGB8,   // stream data format
		0);                // framerate, 0 mean any

	config.enable_stream(
		RS2_STREAM_DEPTH,  // stream type
		-1,                // stream index, -1 mean any
		640,               // stream image width
		480,               // stream image height
		RS2_FORMAT_Z16,    // stream data format
		0);                // framerate, 0 mean any


	const rs2::pipeline_profile profile = pipe.start(config);

	const std::vector<rs2::sensor> sensors = profile.get_device().query_sensors();

	std::vector<rs2::sensor>::const_iterator depth_it = std::find_if(
		sensors.begin(),
		sensors.end(),
		[](const rs2::sensor &s) { return s.as<rs2::depth_sensor>(); });

	assert(depth_it != sensors.end());

	rs2::depth_sensor depth_sensor = depth_it->as<rs2::depth_sensor>();

	_depth_scale = depth_sensor.get_depth_scale() * 1000.f;
	assert(_depth_scale > 0);

	// set depth sensor parameters
	struct RS2Option
	{
		rs2_option option;
		std::string option_name;
		float value;
	};

	const std::vector<RS2Option> options = {
		{ RS2_OPTION_LASER_POWER,  "LASER_POWER", 16 },
		{ RS2_OPTION_ACCURACY,     "ACCURACY", 3 },
		{ RS2_OPTION_MOTION_RANGE, "MOTION_RANGE", 30 },
		{ RS2_OPTION_FILTER_OPTION, "FILTER_OPTION", 5 },
		{ RS2_OPTION_CONFIDENCE_THRESHOLD, "CONFIDENCE_THRESHOLD", 15 }
	};

	for(const RS2Option &opt: options)
	{
		const rs2_option option = opt.option;
		const float value = opt.value;

		if (!depth_sensor.supports(option))
		{
			std::clog << "option '" << opt.option_name << "' is not supported" << std::endl;
			continue;
		}

		assert(!depth_sensor.is_option_read_only(option));

		depth_sensor.set_option(option, value);

		assert(depth_sensor.get_option(option) == value);
	}
}

inline
RealSenseSource::~RealSenseSource()
{
	// nothing
}

inline
void RealSenseSource::get(ImageAndDepth& result_frame)
{
	const rs2::frameset frameset = pipe.wait_for_frames();

	if(frameset.first_or_default(RS2_STREAM_COLOR))
	{
		const rs2::video_frame color_frame = frameset.first_or_default(RS2_STREAM_COLOR);

		assert(color_frame);

		assert(color_frame.get_bits_per_pixel() == 24);
		assert(color_frame.get_stride_in_bytes() >= color_frame.get_width() * 3);

		if((int64_t)color_frame.get_frame_number() > _prev_color_frame_number)
		{
			_prev_color_frame_number = color_frame.get_frame_number();

			result_frame.color_image.assign(
				(const uint8_t*)color_frame.get_data(),
				(const uint8_t*)color_frame.get_data() + color_frame.get_stride_in_bytes() * color_frame.get_height());

			result_frame.color_width = color_frame.get_width();
			result_frame.color_height = color_frame.get_height();
			result_frame.color_stride_in_bytes = color_frame.get_stride_in_bytes();
			result_frame.color_format = pbio::IRawImage::FORMAT_RGB;

			result_frame.image_timestamp_microsec = color_frame.get_timestamp() * 1000;
		}
	}

	if(frameset.first_or_default(RS2_STREAM_DEPTH))
	{
		const rs2::frameset registered = align_to_color->process(frameset);
		const rs2::video_frame depth_frame = registered.first_or_default(RS2_STREAM_DEPTH);

		assert(depth_frame);

		assert(depth_frame.get_bits_per_pixel() == 16);
		assert(depth_frame.get_stride_in_bytes() >= depth_frame.get_width() * 2);

		if((int64_t)depth_frame.get_frame_number() > _prev_depth_frame_number)
		{
			_prev_depth_frame_number = depth_frame.get_frame_number();

			const rs2::video_stream_profile stream_profile =
				depth_frame.get_profile().as<rs2::video_stream_profile>();

			assert(stream_profile);

			const rs2_intrinsics stream_intrinsics = stream_profile.get_intrinsics();

			float fov_angles[] = {0, 0};
			rs2_fov(&stream_intrinsics, fov_angles);

			// angles are in degrees
			const float horizontal_fov = fov_angles[0];
			const float vertical_fov = fov_angles[1];

			assert(horizontal_fov > 0);
			assert(vertical_fov > 0);

			result_frame.depth_timestamp_microsec = depth_frame.get_timestamp() * 1000;

			result_frame.depth_image.assign(
				(const uint8_t*)depth_frame.get_data(),
				(const uint8_t*)depth_frame.get_data() + depth_frame.get_stride_in_bytes() * depth_frame.get_height());

			result_frame.depth_opts.depth_data_stride_in_bytes = depth_frame.get_stride_in_bytes();

			result_frame.depth_opts.depth_map_rows             = depth_frame.get_height();
			result_frame.depth_opts.depth_map_cols             = depth_frame.get_width();
			result_frame.depth_opts.depth_map_2_image_offset_x = 0;
			result_frame.depth_opts.depth_map_2_image_offset_y = 0;
			result_frame.depth_opts.depth_map_2_image_scale_x  = 1;
			result_frame.depth_opts.depth_map_2_image_scale_y  = 1;
			result_frame.depth_opts.horizontal_fov             = horizontal_fov;
			result_frame.depth_opts.vertical_fov               = vertical_fov;
			result_frame.depth_opts.depth_unit_in_millimeters  = _depth_scale;
		}
	}

	assert(!result_frame.color_image.empty() || !result_frame.depth_image.empty());
}

#endif // __REALSENSE_SOURCE_H__0x9762092f__
