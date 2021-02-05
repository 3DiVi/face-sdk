#ifndef __FACE_SDK__PBIO__LIGHT_SHARED_PTR_H__56133c8ad934432e86a8b16f11bb18d7
#define __FACE_SDK__PBIO__LIGHT_SHARED_PTR_H__56133c8ad934432e86a8b16f11bb18d7

#include "object_with_ref_counter.h"

#ifdef __FACE_SDK_PBIO_LIGHT_SHARED_PTR_CHECK_NULL_USE__e8dbe74d9bf04bfa9d97e90c29e858f5
#include "../Error.h"
#endif

//! @cond IGNORED

namespace pbio
{

template<typename T>
class light_shared_ptr
{
public:


	light_shared_ptr() : ptr(NULL) {}


	light_shared_ptr(const light_shared_ptr &a) : ptr(a.ptr)
	{
		if(ptr)
		{
			ptr->increment_refcounter();
		}
	}


	~light_shared_ptr()
	{
		if(ptr)
		{
			ptr->decrement_refcounter();
		}
	}


	light_shared_ptr& operator=(const light_shared_ptr &a)
	{
		if(ptr == a.ptr)
		{
			return *this;
		}

		if(ptr)
		{
			ptr->decrement_refcounter();
		}

		ptr = a.ptr;

		if(ptr)
		{
			ptr->increment_refcounter();
		}

		return *this;
	}


	T* get () const
	{
		if(ptr)
		{
			return &ptr->object;
		}

		return NULL;
	}


	operator T* () const
	{
		return get();
	}

	bool empty () const
	{
		return !ptr;
	}

	void release ()
	{
		if(ptr)
		{
			ptr->decrement_refcounter();
			ptr = NULL;
		}
	}

	T* operator -> () const
	{
#ifdef __FACE_SDK_PBIO_LIGHT_SHARED_PTR_CHECK_NULL_USE__e8dbe74d9bf04bfa9d97e90c29e858f5
		if(!ptr)
		{
			throw pbio::Error(0xef621abe, "Error: Ptr::operator-> called with NULL pointer stored, error code: 0xef621abe.");
		}
#endif
		return &ptr->object;
	}


	T& operator * () const
	{
#ifdef __FACE_SDK_PBIO_LIGHT_SHARED_PTR_CHECK_NULL_USE__e8dbe74d9bf04bfa9d97e90c29e858f5
		if(!ptr)
		{
			throw pbio::Error(0xb3fe4d07, "Error: Ptr::operator* called with NULL pointer stored, error code: 0xb3fe4d07.");
		}
#endif
		return ptr->object;
	}


	static inline light_shared_ptr make()
	{ return light_shared_ptr(new object_with_ref_counter<T>()); }

	template<typename t1>
	static inline light_shared_ptr make(t1 a1)
	{ return light_shared_ptr(new object_with_ref_counter<T>(a1)); }

	template<typename t2, typename t1>
	static inline light_shared_ptr make(t2 a2, t1 a1)
	{ return light_shared_ptr(new object_with_ref_counter<T>(a2, a1)); }

	template<typename t3, typename t2, typename t1>
	static inline light_shared_ptr make(t3 a3, t2 a2, t1 a1)
	{ return light_shared_ptr(new object_with_ref_counter<T>(a3, a2, a1)); }

	template<typename t4, typename t3, typename t2, typename t1>
	static inline light_shared_ptr make(t4 a4, t3 a3, t2 a2, t1 a1)
	{ return light_shared_ptr(new object_with_ref_counter<T>(a4, a3, a2, a1)); }

	template<typename t5, typename t4, typename t3, typename t2, typename t1>
	static inline light_shared_ptr make(t5 a5, t4 a4, t3 a3, t2 a2, t1 a1)
	{ return light_shared_ptr(new object_with_ref_counter<T>(a5, a4, a3, a2, a1)); }

	template<typename t6, typename t5, typename t4, typename t3, typename t2, typename t1>
	static inline light_shared_ptr make(t6 a6, t5 a5, t4 a4, t3 a3, t2 a2, t1 a1)
	{ return light_shared_ptr(new object_with_ref_counter<T>(a6, a5, a4, a3, a2, a1)); }

	template<typename t7, typename t6, typename t5, typename t4, typename t3, typename t2, typename t1>
	static inline light_shared_ptr make(t7 a7, t6 a6, t5 a5, t4 a4, t3 a3, t2 a2, t1 a1)
	{ return light_shared_ptr(new object_with_ref_counter<T>(a7, a6, a5, a4, a3, a2, a1)); }

	template<typename t8, typename t7, typename t6, typename t5, typename t4, typename t3, typename t2, typename t1>
	static inline light_shared_ptr make(t8 a8, t7 a7, t6 a6, t5 a5, t4 a4, t3 a3, t2 a2, t1 a1)
	{ return light_shared_ptr(new object_with_ref_counter<T>(a8, a7, a6, a5, a4, a3, a2, a1)); }

	template<typename t9, typename t8, typename t7, typename t6, typename t5, typename t4, typename t3, typename t2, typename t1>
	static inline light_shared_ptr make(t9 a9, t8 a8, t7 a7, t6 a6, t5 a5, t4 a4, t3 a3, t2 a2, t1 a1)
	{ return light_shared_ptr(new object_with_ref_counter<T>(a9, a8, a7, a6, a5, a4, a3, a2, a1)); }

	template<typename t10, typename t9, typename t8, typename t7, typename t6, typename t5, typename t4, typename t3, typename t2, typename t1>
	static inline light_shared_ptr make(t10 a10, t9 a9, t8 a8, t7 a7, t6 a6, t5 a5, t4 a4, t3 a3, t2 a2, t1 a1)
	{ return light_shared_ptr(new object_with_ref_counter<T>(a10, a9, a8, a7, a6, a5, a4, a3, a2, a1)); }

	template<typename t11, typename t10, typename t9, typename t8, typename t7, typename t6, typename t5, typename t4, typename t3, typename t2, typename t1>
	static inline light_shared_ptr make(t11 a11, t10 a10, t9 a9, t8 a8, t7 a7, t6 a6, t5 a5, t4 a4, t3 a3, t2 a2, t1 a1)
	{ return light_shared_ptr(new object_with_ref_counter<T>(a11, a10, a9, a8, a7, a6, a5, a4, a3, a2, a1)); }

	template<typename t12, typename t11, typename t10, typename t9, typename t8, typename t7, typename t6, typename t5, typename t4, typename t3, typename t2, typename t1>
	static inline light_shared_ptr make(t12 a12, t11 a11, t10 a10, t9 a9, t8 a8, t7 a7, t6 a6, t5 a5, t4 a4, t3 a3, t2 a2, t1 a1)
	{ return light_shared_ptr(new object_with_ref_counter<T>(a12, a11, a10, a9, a8, a7, a6, a5, a4, a3, a2, a1)); }

private:

	light_shared_ptr(object_with_ref_counter<T>* ptr) :
		ptr(ptr)
	{
		if(ptr)
		{
			ptr->increment_refcounter();
		}
	}

	object_with_ref_counter<T> *ptr;

};


template<typename T>
inline
bool operator == (const light_shared_ptr<T> &a, const light_shared_ptr<T> &b)
{
	return a.get() == b.get();
}

template<typename T>
inline
bool operator != (const light_shared_ptr<T> &a, const light_shared_ptr<T> &b)
{
	return a.get() != b.get();
}

}  // pbio namespace

//! @endcond

#endif  // __FACE_SDK__PBIO__LIGHT_SHARED_PTR_H__56133c8ad934432e86a8b16f11bb18d7
