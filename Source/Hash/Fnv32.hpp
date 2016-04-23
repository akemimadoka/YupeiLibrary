#pragma once

#include <cstddef>
#include <cstdint>
#include "..\Extensions.hpp"

//Thanks to https://github.com/lhmouse/MCF/tree/master/MCF/Streams.

namespace Yupei
{
    class fnv32
    {
    public:
        using result_type = std::uint32_t;

        fnv32() noexcept = default;
        DISABLECOPY(fnv32)
        DEFAULTMOVE(fnv32)

        void update(const byte* data, std::size_t dataSize) noexcept;
        result_type finalize() noexcept;

    private:
        bool inited_ = {};
        result_type fnv32Reg_;
    };
}