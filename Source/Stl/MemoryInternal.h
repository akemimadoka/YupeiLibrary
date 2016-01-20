#pragma once

#include "utility_internal.h"
#include "..\CLib\raw_memory.h"

namespace Yupei
{
	template<typename ObjectT, typename... ParamT>
	void construct(ObjectT* ptr, ParamT&&... param)
	{
		::new (static_cast<void*>(ptr)) ObjectT(Yupei::forward<ParamT>(param)...);
	}

	template<typename ObjectT>
	void destroy(const ObjectT* ptr) noexcept
	{
		ptr->~ObjectT();
	}

	namespace Internal
	{
		template<typename ObjectT>
		void DestroyNImp(ObjectT* ptr, std::size_t count, true_type)
		{
			;//no-op
		}

		template<typename ObjectT>
		void DestroyNImp(ObjectT* ptr, std::size_t count, false_type)
		{
			for (std::size_t i{};i < count;++i)
				Yupei::destroy(ptr + i);
		}
	}

	template<typename ObjectT>
	void destroy_n(ObjectT* ptr, std::size_t count)
	{
		Internal::DestroyNImp(ptr, count, is_trivially_destructible<ObjectT>());
	}

	template<typename ObjectT,typename... ParamT>
	ObjectT* construct_n(ObjectT* ptr, std::size_t count,ParamT&&... param)
	{
		for (std::size_t i{};i < count;++i)
			Yupei::construct(ptr + i, Yupei::forward<ParamT>(param)...);
        return ptr + count;
	}
}


