#pragma once

#include <utility>  //for std::forward

namespace Yupei
{
	template<typename ObjectT, typename... ParamsT>
    void construct(ObjectT* ptr, ParamsT&&... params) noexcept(std::is_nothrow_constructible<ObjectT, ParamsT&&...>::value)
	{
		::new (static_cast<void*>(ptr)) ObjectT(std::forward<ParamsT>(params)...);
	}

	template<typename ObjectT>
	void destroy(const ObjectT* ptr) noexcept
	{
		ptr->~ObjectT();
	}

	namespace Internal
	{
		template<typename ObjectT>
		void DestroyNImp(ObjectT* ptr, std::size_t count, std::true_type)
		{
			;//no-op
		}

		template<typename ObjectT>
		void DestroyNImp(ObjectT* ptr, std::size_t count, std::false_type)
		{
			for (std::size_t i{};i < count;++i)
				Yupei::destroy(ptr + i);
		}
	}

	template<typename ObjectT>
	void destroy_n(ObjectT* ptr, std::size_t count)
	{
		Internal::DestroyNImp(ptr, count, std::is_trivially_destructible<ObjectT>());
	}

	template<typename ObjectT,typename... ParamT>
	ObjectT* construct_n(ObjectT* ptr, std::size_t count,ParamT&&... param)
	{
		for (std::size_t i{};i < count;++i)
			Yupei::construct(ptr + i, std::forward<ParamT>(param)...);
        return ptr + count;
	}
}


