#include "Hash.hpp"

namespace Yupei
{
    hash_code hash_combine_range(hash_code h, const unsigned char* hashStart, const unsigned char* hashEnd) noexcept
    {
        h.update(hashStart, static_cast<std::size_t>(hashEnd - hashStart));
        return h;
    }
}