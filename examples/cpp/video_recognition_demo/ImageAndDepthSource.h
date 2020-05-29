#ifndef __IMAGE_AND_DEPTH_SOURCE_H__0x6e5b9ea7__
#define __IMAGE_AND_DEPTH_SOURCE_H__0x6e5b9ea7__

#include <vector>
#include <cassert>
#include <cstdint>

#include <pbio/DepthMapRaw.h>


struct ImageAndDepth
{
	std::vector<uint8_t> color_image;
	int color_width;
	int color_height;
	int color_stride_in_bytes;
	int color_format;

	std::vector<uint8_t> depth_image;

	uint64_t image_timestamp_microsec;
	uint64_t depth_timestamp_microsec;

	pbio::DepthMapRaw depth_opts;  // all but depth_data must be set

	pbio::DepthMapRaw make_dmr() const
	{
		assert(!depth_image.empty());

		pbio::DepthMapRaw r = depth_opts;
		r.depth_data = (const uint16_t*)depth_image.data();

		return r;
	}
};

class ImageAndDepthSource
{
public:

	virtual ~ImageAndDepthSource(){}

	virtual void get(ImageAndDepth& data) = 0;

protected:
	ImageAndDepthSource(){}
};

#endif  // __IMAGE_AND_DEPTH_SOURCE_H__0x6e5b9ea7__
