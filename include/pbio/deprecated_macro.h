#ifndef __PBIO_API__PBIO__DEPRECATED_MACRO_H_
#define __PBIO_API__PBIO__DEPRECATED_MACRO_H_

#if defined(__GNUC__) || defined(__clang__)
#define PBIO_API_DEPRECATED_744e321c __attribute__((deprecated))
#elif defined(_MSC_VER)
#define PBIO_API_DEPRECATED_744e321c __declspec(deprecated)
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define PBIO_API_DEPRECATED_744e321c
#endif

#endif //__PBIO_API__PBIO__DEPRECATED_MACRO_H_ 