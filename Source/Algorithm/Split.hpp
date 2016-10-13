#pragma once

#include "../Config.hpp"

#if defined(HAS_COROUTINE)

#include <experimental/resumable>
#include <experimental/generator>
#include <string_view>
#include <span.h>

namespace Yupei
{
    template<typename CharT, std::ptrdiff_t Extent>
    std::experimental::generator<std::basic_string_view<CharT>> split(std::basic_string_view<CharT> str,
        gsl::span<CharT, Extent> separators)
    {
        auto start = cbegin(str);
        auto last = start;
        while (last != cend(str))
        {
            if (std::find(cbegin(separators), cend(separators), *last) != cend(separators))
            {
                co_yield { start, last };
                start = std::next(last);
            }
            ++last;
        }
        co_yield { start, cend(str) };
    }
}

#endif