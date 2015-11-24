#pragma once

#undef min
#undef max

#include "type_traits.h"
#include "compare_funtor.h"


namespace Yupei
{
	namespace Internal
	{
		constexpr std::size_t PtrNum = 6 + 16 / sizeof(void*);

		constexpr std::size_t SpaceSize = (PtrNum - 1) * sizeof(void*);

		template<typename T>
		struct IsFuncLarge : bool_constant < SpaceSize < sizeof(T) || !is_nothrow_move_constructible<T>::value>{};
	}
	

	template<typename T>
	class FunctionBase;

	template<typename RetT,typename... ParamsT>
	class FunctionBase
	{
	public:
		FunctionBase(const FunctionBase&) = delete;
		FunctionBase& operator = (const FunctionBase&) = delete;

		FunctionBase() = default;

		virtual ~FunctionBase() {}

		virtual FunctionBase* Clone() const = 0;
		virtual void Destroy() noexcept = 0;
		virtual void DestroyDeallocate() noexcept = 0;
		virtual RetT operator()(ParamsT&&...) = 0;
	};

	template<typename T> 
	class function;


	template<typename RetT, typename... ParamsT>
	class function
	{
	private:
		using BaseType = FunctionBase<RetT(ParamsT...)>;
		aligned_storage_t<3 * sizeof(void*)> buf_;

	};

}
