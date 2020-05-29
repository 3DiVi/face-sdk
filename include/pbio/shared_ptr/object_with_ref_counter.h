#ifndef __FACE_SDK__PBIO__OBJECT_WITH_REF_COUNTER_H__2a81343862d047159b41d58bfe5e730b
#define __FACE_SDK__PBIO__OBJECT_WITH_REF_COUNTER_H__2a81343862d047159b41d58bfe5e730b

#include "atomic_exchange_add.h"

//! @cond IGNORED

namespace pbio
{

template<typename T>
class object_with_ref_counter
{
public:

	T object;

#ifdef __FACE_SDK_PBIO_LIGHT_SHARED_PTR_NEED_REFCOUNTER__d213e87df35b4d47af0999a1192bce7d

	int32_t refcounter_field;

	inline
	int32_t& refcounter()
	{
		return refcounter_field;
	}
#else

	inline
	int32_t& refcounter()
	{
		return object.refcounter4light_shared_ptr;
	}
#endif



	void increment_refcounter()
	{
		atomic_exchange_add(refcounter(), 1);
	}

	void decrement_refcounter()
	{
		if(atomic_exchange_add(refcounter(), -1) == 1)
		{
			delete this;
		}
	}


	object_with_ref_counter():
	object() { refcounter() = 0; }

	template<typename t1>
	object_with_ref_counter(t1 a1):
	object(a1) { refcounter() = 0; }


	template<typename t2, typename t1>
	object_with_ref_counter(t2 a2, t1 a1):
	object(a2, a1) { refcounter() = 0; }


	template<typename t3, typename t2, typename t1>
	object_with_ref_counter(t3 a3, t2 a2, t1 a1):
	object(a3, a2, a1) { refcounter() = 0; }

	template<typename t4, typename t3, typename t2, typename t1>
	object_with_ref_counter(t4 a4, t3 a3, t2 a2, t1 a1):
	object(a4, a3, a2, a1) { refcounter() = 0; }


	template<typename t5, typename t4, typename t3, typename t2, typename t1>
	object_with_ref_counter(t5 a5, t4 a4, t3 a3, t2 a2, t1 a1):
	object(a5, a4, a3, a2, a1) { refcounter() = 0; }

	template<typename t6, typename t5, typename t4, typename t3, typename t2, typename t1>
	object_with_ref_counter(t6 a6, t5 a5, t4 a4, t3 a3, t2 a2, t1 a1):
	object(a6, a5, a4, a3, a2, a1) { refcounter() = 0; }


	template<typename t7, typename t6, typename t5, typename t4, typename t3, typename t2, typename t1>
	object_with_ref_counter(t7 a7, t6 a6, t5 a5, t4 a4, t3 a3, t2 a2, t1 a1):
	object(a7, a6, a5, a4, a3, a2, a1) { refcounter() = 0; }

	template<typename t8, typename t7, typename t6, typename t5, typename t4, typename t3, typename t2, typename t1>
	object_with_ref_counter(t8 a8, t7 a7, t6 a6, t5 a5, t4 a4, t3 a3, t2 a2, t1 a1):
	object(a8, a7, a6, a5, a4, a3, a2, a1) { refcounter() = 0; }


	template<typename t9, typename t8, typename t7, typename t6, typename t5, typename t4, typename t3, typename t2, typename t1>
	object_with_ref_counter(t9 a9, t8 a8, t7 a7, t6 a6, t5 a5, t4 a4, t3 a3, t2 a2, t1 a1):
	object(a9, a8, a7, a6, a5, a4, a3, a2, a1) { refcounter() = 0; }


	template<typename t10, typename t9, typename t8, typename t7, typename t6, typename t5, typename t4, typename t3, typename t2, typename t1>
	object_with_ref_counter(t10 a10, t9 a9, t8 a8, t7 a7, t6 a6, t5 a5, t4 a4, t3 a3, t2 a2, t1 a1):
	object(a10, a9, a8, a7, a6, a5, a4, a3, a2, a1) { refcounter() = 0; }


	template<typename t11, typename t10, typename t9, typename t8, typename t7, typename t6, typename t5, typename t4, typename t3, typename t2, typename t1>
	object_with_ref_counter(t11 a11, t10 a10, t9 a9, t8 a8, t7 a7, t6 a6, t5 a5, t4 a4, t3 a3, t2 a2, t1 a1):
	object(a11, a10, a9, a8, a7, a6, a5, a4, a3, a2, a1) { refcounter() = 0; }

	template<typename t12, typename t11, typename t10, typename t9, typename t8, typename t7, typename t6, typename t5, typename t4, typename t3, typename t2, typename t1>
	object_with_ref_counter(t12 a12, t11 a11, t10 a10, t9 a9, t8 a8, t7 a7, t6 a6, t5 a5, t4 a4, t3 a3, t2 a2, t1 a1):
	object(a12, a11, a10, a9, a8, a7, a6, a5, a4, a3, a2, a1) { refcounter() = 0; }

};

}  // pbio namespace

//! @endcond

#endif  // __FACE_SDK__PBIO__OBJECT_WITH_REF_COUNTER_H__2a81343862d047159b41d58bfe5e730b