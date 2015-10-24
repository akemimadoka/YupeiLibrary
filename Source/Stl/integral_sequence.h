#pragma once

#include <cstddef> //for std::size_t

namespace Yupei
{
	// 20.5, Compile-time integer sequences
	/*template<class T, T...> struct integer_sequence;
	template<size_t... I>
	using index_sequence = integer_sequence<size_t, I...>;
	template<class T, T N>
	using make_integer_sequence = integer_sequence<T, see below >;
	template<size_t N>
	using make_index_sequence = make_integer_sequence<size_t, N>;
	template<class... T>
	using index_sequence_for = make_index_sequence<sizeof...(T)>;*/

	// 20.5, Compile-time integer sequences
	template<class T, T... I>
	struct integer_sequence
	{
		using value_type = T;
		static constexpr std::size_t size() noexcept
		{
			return sizeof...(I);
		}
	};

	namespace Internal
	{
		template<std::size_t NowCount,std::size_t Final,typename NowType>
		struct integer_seq_impl;

		template<std::size_t Final, typename T, T... I>
		struct integer_seq_impl<Final, Final, integer_sequence<T, I...>>
		{
			using type = integer_sequence<T, I...>;
		};

		template< std::size_t Now, std::size_t Final, typename T, T... I>
		struct integer_seq_impl<Now, Final, integer_sequence<T, I...>>
		{
			using type = typename integer_seq_impl<Now + 1, Final, integer_sequence<T, I..., Now>>::type;
		};
	}

	template<size_t... I>
	using index_sequence = integer_sequence<size_t, I...>;

	template<class T, T N>
	using make_integer_sequence = typename Internal::integer_seq_impl<0, N, integer_sequence<T>>::type;

	template<size_t N>
	using make_index_sequence = make_integer_sequence<size_t, N>;

	template<class... T>
	using index_sequence_for = make_index_sequence<sizeof...(T)>;
}