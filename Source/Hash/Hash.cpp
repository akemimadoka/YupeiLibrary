#include "Hash.hpp"

namespace Yupei
{
    void hash_combine_range(hash_code& h, const byte* start, const byte* last) noexcept
    {
        h.update(start, static_cast<std::size_t>(last - start));
    }
}