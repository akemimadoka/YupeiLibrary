#pragma once

#include <cstddef>

namespace Yupei
{
    template<typename InputItT, typename Func>
    void for_each_i(InputItT start, InputItT last, Func func)
    {
        std::size_t index {};
        for (; start != last; ++index, ++start)
            func(index, *start);
    }

    template<typename InputIt1T, typename InputIt2T, typename Func>
    void for_each_dual(InputIt1T start1, InputIt2T end1, InputIt2T start2, InputIt2T end2, Func func)
    {
        for (; start1 != end1 && start2 != end2; ++start1, (void)++start2)
            func(*start1, *start2);
    }
}