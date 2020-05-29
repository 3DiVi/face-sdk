#ifndef __TEST_SDK__TASSERT_H__
#define __TEST_SDK__TASSERT_H__

#include <stdexcept>

#define TASSERT(expr) \
	if (expr) ; \
	else ( \
		throw std::runtime_error("Assertion '" #expr "' failed") )

#define TMASSERT(expr, msg) \
	if (expr) ; \
	else ( \
		throw std::runtime_error("Assertion '" #expr "' failed.\n" + std::string(msg)) )

#endif