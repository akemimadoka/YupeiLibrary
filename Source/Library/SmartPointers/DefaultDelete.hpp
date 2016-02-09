#pragma once

#include "..\TypeTraits.hpp"

namespace Yupei
{
	template<typename ObjectT>
	struct default_delete
	{
		constexpr default_delete() noexcept = default;

		template<typename UType, 
            typename = std::enable_if_t<std::is_convertible<UType*, ObjectT*>::value>>
		default_delete(const default_delete<UType>&) noexcept {}

		void operator()(ObjectT* ptr) const noexcept
		{
			delete ptr;
		}
	};

	template<typename ObjectT>
	struct default_delete<ObjectT[]>
	{
		constexpr default_delete() noexcept = default;

		template<typename UType,
			typename = std::enable_if_t <
            std::is_convertible<UType(*)[], ObjectT(*)[]>{} >>
		default_delete(const default_delete<UType>&) noexcept {}

		template<typename UType,
			typename = std::enable_if_t <
            std::is_convertible<ObjectT(*)[], UType(*)[]>::value>>
		void operator()(UType* ptr) const noexcept
		{
			delete[] ptr;
		}
	};
}
