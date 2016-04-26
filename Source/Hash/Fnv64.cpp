#include "fnv64.hpp"

namespace Yupei
{
	void fnv64::update(const byte* data, std::size_t dataSize) noexcept
	{
		if (!inited_)
		{
			fnv64Reg_ = 14695981039346656037u;;
			inited_ = true;
		}

		const auto Dofnv32Byte = [&](byte byData)
		{
			auto reg = fnv64Reg_;
			reg ^= byData;
			reg *= 1099511628211u;
			fnv64Reg_ = reg;
		};

		const auto dataEnd = data + dataSize;

		if (dataSize > sizeof(std::uintptr_t) * 2)
		{
			while ((reinterpret_cast<std::uintptr_t>(data) & (sizeof(std::uintptr_t) - 1)) != 0)
			{
				Dofnv32Byte(*data);
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
					Dofnv32Byte(word & 0xFF);
					word >>= 8;
				}
			}
		}

		while (data != dataEnd)
		{
			Dofnv32Byte(*data);
			++data;
		}
	}

	auto fnv64::finalize() noexcept -> result_type
	{
		if (inited_)
			inited_ = false;
		return fnv64Reg_;
	}
}
