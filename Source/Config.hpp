#pragma once

#if __cpp_constexpr >= 201304
#define CXX14_CONSTEXPR constexpr
#else
#define CXX14_CONSTEXPR
#endif

//这里本打算测试 __cpp_coroutine，但是目前 MSVC 尚未定义。
//MSVC 目前对于 __has_include(xxx) && yyyy 有 bug，故只检测这一个文件。

//#if __has_include(<experimental\resumable>)
//#define HAS_COROUTINE
//#endif

//一个文件也有 bug...
#define HAS_COROUTINE