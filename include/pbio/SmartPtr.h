/**
	\file SmartPtr.h
	\brief SmartPtr
*/

#ifndef __PBIO_API__PBIO__SMART_PTR_H_
#define __PBIO_API__PBIO__SMART_PTR_H_



#define __FACE_SDK_PBIO_LIGHT_SHARED_PTR_CHECK_NULL_USE__e8dbe74d9bf04bfa9d97e90c29e858f5
// #define __FACE_SDK_PBIO_LIGHT_SHARED_PTR_NEED_REFCOUNTER__d213e87df35b4d47af0999a1192bce7d

#if defined( PBIO_OPENCV_SMART_POINTER )

// for cv::Ptr
#include <opencv2/core/core.hpp>

#elif defined( PBIO_BOOST_SMART_POINTER )

// for boost::shared_ptr
#include <boost/shared_ptr.hpp>

#elif defined( PBIO_CXX11_SMART_POINTER )

// for std::shared_ptr
#include <memory>

#else

// default implementation of heavy pointer
#include "shared_ptr/heavy_shared_ptr.h"

#endif

#include "shared_ptr/light_shared_ptr.h"

namespace pbio{

// HeavySmartPtr is a proper shared ptr, that allow storing different types according to inheritance
// BUT: this cause it to use a lot more memory
// and this is very inconvenient for sets of many object (database of millions of pbio::Template, for example)
// so this must be used only when it is really needed
template<typename T>
class HeavySmartPtr{
public:


#if defined( PBIO_OPENCV_SMART_POINTER )

	typedef cv::Ptr<T> tPtr;

#elif defined( PBIO_BOOST_SMART_POINTER )

	typedef boost::shared_ptr<T> tPtr;

#elif defined( PBIO_CXX11_SMART_POINTER )

	typedef std::shared_ptr<T> tPtr;

#else

	typedef pbio::cv_smart_ptr::cv::Ptr<T> tPtr;

#endif


};

// for backward compatibility in case someone used this
// (which actually is not good - these classes are internal api, do not use them directly)
template<typename T>
class SmartPtr{
public:
	typedef typename HeavySmartPtr<T>::tPtr tPtr;
};


// SmartPtr is a light shared ptr
// is allow storing only type T
//   no inheritance, no specific deleters
// as simple as it can be in order to use less memory
template<typename T>
class LightSmartPtr{
public:

	typedef pbio::light_shared_ptr<T> tPtr;

};

}  // pbio namespace



#endif  // __PBIO_API__PBIO__SMART_PTR_H_
