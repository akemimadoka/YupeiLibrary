
#include <cassert>

#undef YPASSERT

#define YPASSERT(expr, msg) assert(expr && msg)
