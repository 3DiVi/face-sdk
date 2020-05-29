#ifndef __FACE_SDK__PBIO__ATOMIC_EXCHANGE_ADD_H__85f16090e1cc420091945dab3a112af4
#define __FACE_SDK__PBIO__ATOMIC_EXCHANGE_ADD_H__85f16090e1cc420091945dab3a112af4


//! @cond IGNORED

#if defined __INTEL_COMPILER && !(defined WIN32 || defined _WIN32)
   // atomic increment on the linux version of the Intel(tm) compiler

namespace pbio {
static inline int atomic_exchange_add(int &x, const int delta) { return (int)_InterlockedExchangeAdd(const_cast<void*>(reinterpret_cast<volatile void*>(&x)), delta); }
}  // pbio


#elif defined __GNUC__
#  if defined __clang__ && __clang_major__ >= 3 && !defined __ANDROID__ && !defined __EMSCRIPTEN__ && !defined(__CUDACC__)
#    ifdef __ATOMIC_ACQ_REL


namespace pbio {
static inline int atomic_exchange_add(int &x, const int delta) { return __c11_atomic_fetch_add((_Atomic(int)*)(&x), delta, __ATOMIC_ACQ_REL); }
}  // pbio


#    else


namespace pbio {
static inline int atomic_exchange_add(int &x, const int delta) { return __atomic_fetch_add((_Atomic(int)*)(&x), delta, 4); }
}  // pbio


#    endif
#  else
#    if defined __ATOMIC_ACQ_REL && !defined __clang__
       // version for gcc >= 4.7


namespace pbio {
static inline int atomic_exchange_add(int &x, const int delta) { return (int)__atomic_fetch_add((int*)(&x), (int)(delta), __ATOMIC_ACQ_REL); }
}  // pbio


#    else


namespace pbio {
static inline int atomic_exchange_add(int &x, const int delta) { return (int)__sync_fetch_and_add((int*)(&x), (int)(delta)); }
}  // pbio


#    endif
#  endif
#elif defined _MSC_VER && !defined RC_INVOKED


#include <intrin.h>

namespace pbio {
static inline int atomic_exchange_add(int &x, const int delta) { return (int)_InterlockedExchangeAdd((long volatile*)(&x), delta); }
}  // pbio


#else

#error no atomic_exchange_add implementation

#endif

//! @endcond

#endif  // __FACE_SDK__PBIO__ATOMIC_EXCHANGE_ADD_H__85f16090e1cc420091945dab3a112af4