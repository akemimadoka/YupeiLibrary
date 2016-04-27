#pragma once

#include "Config.hpp"
#include <cassert>
#if defined(YPDEBUG)
#include <utility>
#endif

#define YPASSERT(expr, msg) assert(expr && msg)

#if defined(YPDEBUG)
#define DoWhenDebug(Func) Func()
#elif
#define DoWhenDebug
#endif