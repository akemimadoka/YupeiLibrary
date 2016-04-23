#pragma once

#include <climits>
#include <cfloat>
#include <type_traits>

namespace Yupei
{

#define DEFINE_LIMITS(type, maxValue, minValue) template<> struct limit<type> \
    {                                                                   \
        static constexpr type max_value = maxValue;                     \
        static constexpr type min_value = minValue;                     \
    };                                                                  

    template<typename T>
    struct limit
    {
        //Nothing!
    };

    DEFINE_LIMITS(char, CHAR_MAX, CHAR_MIN)
    DEFINE_LIMITS(unsigned char, UCHAR_MAX, {})
    DEFINE_LIMITS(signed char, SCHAR_MAX, SCHAR_MIN)
    DEFINE_LIMITS(short, SHRT_MAX, SHRT_MIN)
    DEFINE_LIMITS(unsigned short, USHRT_MAX, {})
    DEFINE_LIMITS(int, INT_MAX, INT_MIN)
    DEFINE_LIMITS(unsigned int, UINT_MAX, {})
    DEFINE_LIMITS(long, LONG_MAX, LONG_MIN)
    DEFINE_LIMITS(unsigned long, ULONG_MAX, {})
    //use if undeclared identifier LONG_LONG_MAX, etc.
    //DEFINE_LIMITS(long long, LONG_LONG_MAX, LONG_LONG_MIN)
    //DEFINE_LIMITS(unsigned long long, ULONG_LONG_MAX, {})
    DEFINE_LIMITS(float, FLT_MAX, FLT_MIN)
    DEFINE_LIMITS(double, DBL_MAX, DBL_MIN)
    DEFINE_LIMITS(long double, LDBL_MAX, LDBL_MIN)

	namespace Internal
	{
		template<typename T>
		struct LimitWrapper
		{
			using ArithmeticType = std::decay_t<T>;
			static_assert(std::is_arithmetic_v<ArithmeticType>, "T should be an arithmetic type");
			static constexpr ArithmeticType max_value = limit<ArithmeticType>::max_value;
			static constexpr ArithmeticType min_value = limit<ArithmeticType>::min_value;
		};
	}

	//workaround for MSVC.
    template<typename T>
	constexpr std::decay_t<T> limits_max_v = Internal::LimitWrapper<T>::max_value;

    template<typename T>
    constexpr std::decay_t<T> limits_min_v = Internal::LimitWrapper<T>::min_value;

#undef DEFINE_LIMITS
}