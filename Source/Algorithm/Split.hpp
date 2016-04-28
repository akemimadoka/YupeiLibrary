#pragma once

#include "..\Config.hpp"

#if defined(HAS_COROUTINE)

#include "..\String.hpp"
#include "..\Span.hpp"
#include <experimental\resumable>
#include <experimental\generator>

namespace Yupei
{
    template<string_type CharT, std::ptrdiff_t Extent>
    std::experimental::generator<basic_string_view<CharT>> split(basic_string_view<CharT> str,
        cspan<char_type_t<CharT>, Extent> separators)
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