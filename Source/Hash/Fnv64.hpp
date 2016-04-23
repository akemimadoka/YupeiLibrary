#pragma once

#include "..\Extensions.hpp"
#include <cstdint>
#include <cstddef>

namespace Yupei
{
    class fnv64
    {
    public:
        using result_type = std::uint64_t;

        fnv64() noexcept = default;
        DISABLECOPY(fnv64)
        DEFAULTMOVE(fnv64)

        void update(const byte* data, std::size_t dataSize) noexcept;
        result_type finalize() noexcept;

    private:
        bool inited_ = {};
        result_type fnv64Reg_;
    };
}


