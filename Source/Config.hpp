#pragma once

#if defined(_RESUMABLE_FUNCTIONS_SUPPORTED)
#define HAS_COROUTINE 1
#endif

#if defined(_MSC_VER)
#define YPMSVC 1
#endif

#if defined(YPMSVC) && defined(_DEBUG)
#define YPDEBUG 1
#endif

#if defined(_WIN32)
#define YPWINDOWS
#elif defined(__linux) || defined(__unix)
#define YPUNIX
#else
#error "Non-supported platform."
#endif