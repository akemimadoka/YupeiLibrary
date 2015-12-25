#pragma once

#include "type_traits.h"
#include "MemoryReasource\memory_resource.h"

namespace Yupei
{
	template<typename ElementT>
	class vector
	{
	public:

		using value_type = ElementT;
		using reference = value_type&;
		using const_reference = const value_type&;
		using size_type = std::size_t;

		constexpr vector() noexcept
			:storage_{}, size_{}, capacity_{}, allocator_{}
		{
		}

	private:
		void* storage_;
		size_type size_;
		size_type capacity_;
		memory_resource_ptr allocator_;
	};
}
