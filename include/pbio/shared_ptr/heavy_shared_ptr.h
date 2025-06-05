// Actually this is a smart pointer from OpenCV library assembled in a single file.


/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2013, NVIDIA Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the copyright holders or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#ifndef __PBIO__SMART_PTR__OPENCV_CORE_PTR_INL_HPP__dbae1c6458e54ffbb5f63f308a482027
#define __PBIO__SMART_PTR__OPENCV_CORE_PTR_INL_HPP__dbae1c6458e54ffbb5f63f308a482027

#include <algorithm>

#include "../Error.h"
#include "atomic_exchange_add.h"

//! @cond IGNORED


namespace pbio {
namespace cv_smart_ptr {
namespace cv {


namespace detail
{

// Metafunction to avoid taking a reference to void.
template<typename T>
struct RefOrVoid { typedef T& type; };

template<>
struct RefOrVoid<void>{ typedef void type; };

template<>
struct RefOrVoid<const void>{ typedef const void type; };

template<>
struct RefOrVoid<volatile void>{ typedef volatile void type; };

template<>
struct RefOrVoid<const volatile void>{ typedef const volatile void type; };

// This class would be private to Ptr, if it didn't have to be a non-template.
struct PtrOwner;

}

template<typename Y>
struct DefaultDeleter
{
    void operator () (Y* p) const;
};

template<typename T>
struct Ptr
{
    typedef T element_type;

    Ptr();

    template<typename Y>
    explicit
    Ptr(Y* p);

    template<typename Y, typename D>
    Ptr(Y* p, D d);

    Ptr(const Ptr& o);

    template<typename Y>
    Ptr(const Ptr<Y>& o);

    template<typename Y>
    Ptr(const Ptr<Y>& o, T* p);

    ~Ptr();

    Ptr& operator = (const Ptr& o);

    template<typename Y>
    Ptr& operator = (const Ptr<Y>& o);

    void release();

    template<typename Y>
    void reset(Y* p);

    template<typename Y, typename D>
    void reset(Y* p, D d);

    void swap(Ptr& o);

    T* get() const;

    typename detail::RefOrVoid<T>::type operator * () const;

    T* operator -> () const;

    operator T* () const;

    bool empty() const;

    template<typename Y>
    Ptr<Y> staticCast() const;

    template<typename Y>
    Ptr<Y> constCast() const;

    template<typename Y>
    Ptr<Y> dynamicCast() const;

private:
    detail::PtrOwner* owner;
    T* stored;

    template<typename Y>
    friend struct Ptr;
};

template<typename T>
void swap(Ptr<T>& ptr1, Ptr<T>& ptr2);

template<typename T>
bool operator == (const Ptr<T>& ptr1, const Ptr<T>& ptr2);
template<typename T>
bool operator != (const Ptr<T>& ptr1, const Ptr<T>& ptr2);






template<typename Y>
void DefaultDeleter<Y>::operator () (Y* p) const
{
    delete p;
}

namespace detail
{

struct PtrOwner
{
    PtrOwner() : refCount(1)
    {}

    void incRef()
    {
        atomic_exchange_add(refCount, 1);
    }

    void decRef()
    {
        if (atomic_exchange_add(refCount, -1) == 1) deleteSelf();
    }

protected:
    virtual ~PtrOwner()
    {}

    virtual void deleteSelf() = 0;

private:
    int refCount;

    // noncopyable
    PtrOwner(const PtrOwner&);
    PtrOwner& operator = (const PtrOwner&);
};

template<typename Y, typename D>
struct PtrOwnerImpl : PtrOwner
{
    PtrOwnerImpl(Y* p, D d) : owned(p), deleter(d)
    {}

    virtual void deleteSelf() override
    {
        deleter(owned);
        delete this;
    }

private:
    Y* owned;
    D deleter;
};


}

template<typename T>
Ptr<T>::Ptr() : owner(NULL), stored(NULL)
{}

template<typename T>
template<typename Y>
Ptr<T>::Ptr(Y* p)
  : owner(p
      ? new detail::PtrOwnerImpl<Y, DefaultDeleter<Y> >(p, DefaultDeleter<Y>())
      : NULL),
    stored(p)
{}

template<typename T>
template<typename Y, typename D>
Ptr<T>::Ptr(Y* p, D d)
  : owner(p
      ? new detail::PtrOwnerImpl<Y, D>(p, d)
      : NULL),
    stored(p)
{}

template<typename T>
Ptr<T>::Ptr(const Ptr& o) : owner(o.owner), stored(o.stored)
{
    if (owner) owner->incRef();
}

template<typename T>
template<typename Y>
Ptr<T>::Ptr(const Ptr<Y>& o) : owner(o.owner), stored(o.stored)
{
    if (owner) owner->incRef();
}

template<typename T>
template<typename Y>
Ptr<T>::Ptr(const Ptr<Y>& o, T* p) : owner(o.owner), stored(p)
{
    if (owner) owner->incRef();
}

template<typename T>
Ptr<T>::~Ptr()
{
    release();
}

template<typename T>
Ptr<T>& Ptr<T>::operator = (const Ptr<T>& o)
{
    Ptr(o).swap(*this);
    return *this;
}

template<typename T>
template<typename Y>
Ptr<T>& Ptr<T>::operator = (const Ptr<Y>& o)
{
    Ptr(o).swap(*this);
    return *this;
}

template<typename T>
void Ptr<T>::release()
{
    if (owner) owner->decRef();
    owner = NULL;
    stored = NULL;
}

template<typename T>
template<typename Y>
void Ptr<T>::reset(Y* p)
{
    Ptr(p).swap(*this);
}

template<typename T>
template<typename Y, typename D>
void Ptr<T>::reset(Y* p, D d)
{
    Ptr(p, d).swap(*this);
}

template<typename T>
void Ptr<T>::swap(Ptr<T>& o)
{
    std::swap(owner, o.owner);
    std::swap(stored, o.stored);
}

template<typename T>
T* Ptr<T>::get() const
{
    return stored;
}

template<typename T>
typename detail::RefOrVoid<T>::type Ptr<T>::operator * () const
{
    if(!stored)
    {
        throw pbio::Error(0x1afccd1c, "Error: Ptr::operator* called with NULL pointer stored, error code: 0x1afccd1c.");
    }
    return *stored;
}

template<typename T>
T* Ptr<T>::operator -> () const
{
    if(!stored)
    {
        throw pbio::Error(0x42a96bad, "Error: Ptr::operator-> called with NULL pointer stored, error code: 0x42a96bad.");
    }
    return stored;
}

template<typename T>
Ptr<T>::operator T* () const
{
    return stored;
}


template<typename T>
bool Ptr<T>::empty() const
{
    return !stored;
}

template<typename T>
template<typename Y>
Ptr<Y> Ptr<T>::staticCast() const
{
    return Ptr<Y>(*this, static_cast<Y*>(stored));
}

template<typename T>
template<typename Y>
Ptr<Y> Ptr<T>::constCast() const
{
    return Ptr<Y>(*this, const_cast<Y*>(stored));
}

template<typename T>
template<typename Y>
Ptr<Y> Ptr<T>::dynamicCast() const
{
    return Ptr<Y>(*this, dynamic_cast<Y*>(stored));
}

template<typename T>
void swap(Ptr<T>& ptr1, Ptr<T>& ptr2){
    ptr1.swap(ptr2);
}

template<typename T>
bool operator == (const Ptr<T>& ptr1, const Ptr<T>& ptr2)
{
    return ptr1.get() == ptr2.get();
}

template<typename T>
bool operator != (const Ptr<T>& ptr1, const Ptr<T>& ptr2)
{
    return ptr1.get() != ptr2.get();
}

} // namespace cv
} // namespace cv_smart_ptr
} // namespace pbio


//! @endcond

#endif // __PBIO__SMART_PTR__OPENCV_CORE_PTR_INL_HPP__dbae1c6458e54ffbb5f63f308a482027
