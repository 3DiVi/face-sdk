#include "pbio/Error.h"
#define tryCatch(...) \
    [&]() -> decltype(auto) { \
        try { \
            return __VA_ARGS__; \
        } catch (pbio::Error error) {\
            throw [[NSException alloc] initWithName:@(error.what()) reason:@(error.code()).stringValue userInfo:nil]; \
        }\
    }()
