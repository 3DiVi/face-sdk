#ifndef __PBIO_API__PBIO__IMAGE_H_
#define __PBIO_API__PBIO__IMAGE_H_

//! @cond IGNORED

#include "pbio/IRawImage.h"

namespace pbio {
namespace image {

class Image : public pbio::IRawImage
{
public:

	typedef HeavySmartPtr<Image>::tPtr Ptr;

	Image(
		const int rows,
		const int cols,
		const Format format);

	virtual ~Image();

	virtual const unsigned char* data() const throw();
	virtual int32_t width() const throw();
	virtual int32_t height() const throw();
	virtual int32_t format() const throw();

	// not from IRawImage
	unsigned char* data() throw();

private:

	const int _rows;
	const int _cols;
	const Format _format;
	unsigned char* const _data;
};

}  // image namespace
}  // pbio namespace



////////////////////////
/////IMPLEMENTATION/////
////////////////////////


namespace pbio {
namespace image {


// static

inline
Image::Image(
	const int rows,
	const int cols,
	const Format format) :
_rows(rows),
_cols(cols),
_format(format),
_data(
	(rows <= 0 ||
		cols <= 0 ||
		(format != FORMAT_GRAY &&
			format != FORMAT_RGB &&
			format != FORMAT_BGR)) ?
				(unsigned char*) NULL :
				new unsigned char[rows * cols * (format == FORMAT_GRAY ? 1 : 3)])
{
	// nothing else
}

inline
Image::~Image()
{
	if(_data)
		delete [] _data;
}

inline
const unsigned char* Image::data() const throw()
{
	return _data;
}

inline
int32_t Image::width() const throw()
{
	return _cols;
}

inline
int32_t Image::height() const throw()
{
	return _rows;
}

inline
int32_t Image::format() const throw()
{
	return _format;
}

inline
unsigned char* Image::data() throw()
{
	return _data;
}

}  // image namespace
}  // pbio namespace


//! @endcond

#endif  // __PBIO_API__PBIO__IMAGE_H_
