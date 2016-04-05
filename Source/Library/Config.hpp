#pragma once

#if __cpp_constexpr >= 201304
#define CXX14_CONSTEXPR constexpr
#else
#define CXX14_CONSTEXPR
#endif