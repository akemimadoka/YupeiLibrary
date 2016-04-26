#include "Fnv32.hpp"

namespace Yupei
{
	void fnv32::update(const byte* data, std::size_t dataSize) noexcept
	{
		if (!inited_)
		{
			fnv32Reg_ = 2166136261u;
			inited_ = true;
		}

		const auto DoFnv32Byte = [&](byte byData)
		{
			auto reg = fnv32Reg_;
			reg ^= byData;
			reg *= 16777619u;
			fnv32Reg_ = reg;
		};

		const auto dataEnd = data + dataSize;

		if (dataSize > sizeof(std::uintptr_t) * 2)
		{
			while ((reinterpret_cast<std::uintptr_t>(data) & (sizeof(std::uintptr_t) - 1)) != 0)
			{
				DoFnv32Byte(*data);
				++data;
			}

			auto i = static_cast<std::size_t>(dataEnd - data) / sizeof(std::uintptr_t);
			while (i > 0)
			{
				auto word = *reinterpret_cast<const std::uintptr_t*>(data);
				i -= 1;
				data += sizeof(std::uintptr_t);
				for (unsigned i = 0; i < sizeof(std::uintptr_t); ++i)
				{
					DoFnv32Byte(word & 0xFF);
					word >>= 8;
				}
			}
		}

		while (data != dataEnd)
		{
			DoFnv32Byte(*data);
			++data;
		}
	}

	auto fnv32::finalize() noexcept -> result_type
	{
		if (inited_)
			inited_ = false;
		return fnv32Reg_;
	}
}
