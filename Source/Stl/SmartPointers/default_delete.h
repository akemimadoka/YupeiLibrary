#pragma once

#include "..\type_traits.h"

namespace Yupei
{
	template<typename Type>
	struct default_delete
	{
		constexpr default_delete() noexcept = default;

		template<typename UType,
			typename = enable_if_t <
			is_convertible<UType*, Type*>::value >>
			default_delete(const default_delete<UType>&) noexcept {}

		void operator()(Type* ptr) const noexcept
		{
			delete ptr;
		}
	};

	template<typename Type>
	struct default_delete<Type[]>
	{
		constexpr default_delete() noexcept = default;

		template<typename UType,
			typename = enable_if_t <
			is_convertible<UType(*)[], Type(*)[]>{} >>
			default_delete(const default_delete<UType>&) noexcept {}

		template<typename UType,
			typename = enable_if_t <
			is_convertible<Type(*)[], UType(*)[]>::value >>
			void operator()(UType* ptr)
		{
			delete[] ptr;
		}

	};
}
