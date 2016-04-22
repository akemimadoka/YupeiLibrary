#pragma once

#include "..\Config.hpp"

#if defined(HAS_COROUTINE)

#include "..\String.hpp"
#include <experimental\resumable>
#include <experimental\generator>

namespace Yupei
{
    template<string_type CharT>
    std::experimental::generator<basic_string_view<CharT>> split(basic_string_view<CharT> str,
        const char_type_t<CharT>* separators, std::size_t separatorCount)
    {
        auto start = cbegin(str);
        auto last = start;
        const auto separatorsLast = separators + separatorCount;
        while (last != cend(str))
        {
            if (std::find(separators, separatorsLast, *last) != separatorsLast)
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